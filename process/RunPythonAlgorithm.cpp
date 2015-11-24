/*
*  This file is part of BRAT.
*
*    BRAT is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    BRAT is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with BRAT.  If not, see <http://www.gnu.org/licenses/>.
*
*/

/*

  TODO: check T_CHAR  semantics in BRAT 3.1.0
  TODO: check T_VECTOR_DOUBLE  semantics in BRAT 3.1.0
  TODO: check differences, if any, int vs. long in python 3

*/

#include "RunPythonAlgorithm.hpp"

/* Example of embedding Python script in C application  */


//////////////////////////////////////////////////////////////
//						Error Reporting
//////////////////////////////////////////////////////////////

enum EMessageCode
{
    e_OK,
    e_unspecified_error,

    e_file_not_found,
    e_class_not_found,
    e_error_reading_file,
    e_error_add_module_dir_syspath,
    e_error_loading_module,
    e_error_create_python_arg,
    e_error_calling_method,
    e_error_evaluating_python,
    e_could_not_create_algorithm,
    e_wrong_number_input_param,
    e_invalid_parameter_type,

    EMessageCode_size
};

const char *error_messages[ EMessageCode_size ] =
{
    "",
    "Unspecified error.",

    "File not found.",
	"Class not found",
    "Error reading file.",
    "Error adding module directory to sys.Path.",
    "Error loading Python module",
    "Error creating Python arguments.",
    "Error calling a Python method.",
    "Error evaluating Python code.",
    "Could not create algorithm instance.",
    "Incorrect number of command line arguments. Check the number of input parameters of the algorithm (defined at 'GetNumInputParam').",
    "Invalid Brat parameter type. Check the parameter types of the algorithm (defined at 'GetInputParamFormat')."
};

static_assert( ARRAY_SIZE(error_messages) == EMessageCode_size,
               "Compile time error: number of message codes doesn't match the number of messages.");


//////////////////////////////////////////////////////////////
//						Brat type names
//////////////////////////////////////////////////////////////

inline const char* typeName(CBratAlgorithmParam::bratAlgoParamTypeVal t)
{
    static const size_t bratAlgoParamTypeVal_size = CBratAlgorithmParam::T_VECTOR_DOUBLE + 1;

    static const char *bratAlgoParamTypeVal_names[ bratAlgoParamTypeVal_size ] =
    {
        "UNDEFINED",
        "Py_INT",     //equivalent to T_INT
        "Py_LONG",    //equivalent to T_LONG
        "",           //equivalent to T_FLOAT (NOT USED)
        "Py_FLOAT",   //equivalent to T_DOUBLE
        "Py_BYTE",    //equivalent to T_CHAR
        "Py_STRING",  //equivalent to T_STRING
        "",           //equivalent to T_VECTOR_DOUBLE (NOT USED)
    };

    static_assert( ARRAY_SIZE(bratAlgoParamTypeVal_names) == bratAlgoParamTypeVal_size,
                   "Compile time error: number of types names doesn't match the number of types.");

    assert( t < bratAlgoParamTypeVal_size );

    return bratAlgoParamTypeVal_names[t];
}


//////////////////////////////////////////////////////////////
//						Python Engine
//////////////////////////////////////////////////////////////

//#define TEST_SET_PYTHON_ENVIRONMENT


/**
 * \class     PythonEngine
 * \brief     Definition of the object to hold the Python Interpreter and respective methods.
 * \ingroup
 */
class PythonEngine
{
    PyObject *m_global_dict = nullptr;

public:
    static PythonEngine& Instance()
    {
        static PythonEngine pe;
        return pe;
    }

protected:
    PythonEngine( wchar_t *wargv0 = nullptr )
    {

#if defined (TEST_SET_PYTHON_ENVIRONMENT)
		Py_SetPythonHome( L"J:\\Python\\301" );
		Py_SetPath( L"J:\\Python\\301" );
		std::wcout << Py_GetPythonHome() << std::endl;
		PyRun_SimpleString( "import sys" );
		PyRun_SimpleString( "print( sys.path )" );
#endif

		if ( wargv0 )
			Py_SetProgramName( wargv0 );  // optional but recommended

        Py_Initialize();

        // Get a reference to the main module and global dictionary
        //
        PyObject *main_module = PyImport_AddModule("__main__");
        if (main_module)
            m_global_dict = PyModule_GetDict( main_module );
    }

public:
    ~PythonEngine()
    {
        Py_Finalize();
    }

    PyObject* getObject( const std::string &name ) const
    {
        assert( m_global_dict );

        return PyDict_GetItemString( m_global_dict, name.c_str() );
    }

    bool evaluate( const std::string &expression ) const
    {
        assert( m_global_dict );

        // Returns 0 on success or -1 if an std::exception was raised.
        //
        return !PyRun_SimpleString( expression.c_str() );
    }

    //static utilities

    static PyObject* convert( PyObject *py_result, std::string &result )
    {
        PyObject *ascii_string = PyUnicode_AsASCIIString( py_result );
        result = PyBytes_AsString( ascii_string );
        Py_XDECREF( ascii_string );
        return py_result;
    }
    static PyObject* convert( PyObject *py_result, uint32_t &result )
    {
        result = PyLong_AsLong( py_result );
        return py_result;
    }
    static PyObject* convert( PyObject *py_result, double &result )
    {
        result = PyFloat_AsDouble( py_result );
        return py_result;
    }

};


//////////////////////////////////////////////////////////////
//						PyAlgo class
//////////////////////////////////////////////////////////////

/**
 * \class     PyAlgo
 * \brief     Definition of the object to hold each Python Algorithm and respective variables/methods.
 * \ingroup
 */
class PyAlgo : public CBratAlgorithmBase
{
    typedef CBratAlgorithmBase base_t;

    const std::string m_file_path;    /**< The path of the algorithm python script/module. */
    const std::string m_className;    /**< Name of the algorithm class.                    */

    PyObject *m_module = nullptr;     /**< Python object to hold the module.               */
    PyObject *m_class = nullptr;      /**< Python object to hold the algorithm class.      */
    PyObject *m_instance = nullptr;   /**< Python object to hold an instance of the class. */

    static const PythonEngine &sm_pe; /**< PythonEngine object (Python interpreter).       */

public:

    /**
     * \brief          User defined constructor for PyAlgo.
     * \param[in]    file_path         The path of the algorithm python script/module.
     * \param[in]    className         Name of the algorithm class.
     */
    PyAlgo( const std::string file_path, const std::string &className ) :
        m_file_path( file_path ), m_className( className )
    {
        if ( !IsFile(m_file_path.c_str()) )
            throw e_file_not_found;

        std::string modulePath = dir_from_filepath( file_path );
        std::string moduleName = basename_from_path( file_path );

		// Add the directory (with module) to the python sys.path

        PyObject *sysPath = PySys_GetObject("path");                        //TODO Py_DECREF sysPath???
        PyObject *path    = PyUnicode_FromString( modulePath.c_str() );     //TODO Py_DECREF path???
		if ( PyList_Insert(sysPath, 0, path))
            throw e_error_add_module_dir_syspath;

		// Build the name object and load the module object

        PyObject *pName   = PyUnicode_FromString( moduleName.c_str() );
		m_module = PyImport_Import(pName);
        Py_DECREF(pName);
		if (PyErr_Occurred()){
			PyErr_Print();
			throw e_error_loading_module;
		}

		std::cout << "=> Loaded a python Module object at " << m_module << std::endl;

	    // Build the name of a callable class

		m_class = PyObject_GetAttrString( m_module, m_className.c_str() );


		// Create an instance of the class

		if (m_class && PyCallable_Check(m_class))
		{
			m_instance = PyObject_CallObject(m_class, NULL);

            std::cout << "=> Loaded a python Class object at " << m_class << std::endl;
		}
		else {
			if (PyErr_Occurred())
				PyErr_Print();
			throw e_class_not_found;
		}
    }

    /**
     * \brief        Default destructor for PyAlgo.
     */
    virtual ~PyAlgo()
    {
        Py_DECREF(m_module);
        Py_DECREF(m_class);
        Py_DECREF(m_instance);
    }


protected:
    /**
     * \brief           Method to create a list of Python arguments.
     * \return          pArgs      Python List with python objects/arguments.
     */
    static PyObject* createPyArguments( CVectorBratAlgorithmParam &args )
    {
        // Creating python list with items of different python types (arguments)
        PyObject *pArgs   = PyList_New(0);

        for (int i = 0; i < args.size(); ++i )
        {
            PyObject *pValue;
            CBratAlgorithmParam::bratAlgoParamTypeVal ParamTypeVal = args[i].GetTypeVal();

            switch (ParamTypeVal)
            {
              //case CBratAlgorithmParam::T_UNDEF: break;
              case CBratAlgorithmParam::T_INT:    pValue = PyLong_FromLong( args.at(i).GetValueAsInt() );  break;
              case CBratAlgorithmParam::T_LONG:   pValue = PyLong_FromLong( static_cast<long>( args.at(i).GetValueAsLong() ) ); break;
              case CBratAlgorithmParam::T_FLOAT:  pValue = PyFloat_FromDouble( args.at(i).GetValueAsFloat() ); break;
              case CBratAlgorithmParam::T_DOUBLE: pValue = PyFloat_FromDouble( args.at(i).GetValueAsDouble() ); break;
              case CBratAlgorithmParam::T_CHAR:   pValue = PyLong_FromLong( args.at(i).GetValueAsChar() ); break;
              case CBratAlgorithmParam::T_STRING: pValue = PyUnicode_FromString( args.at(i).GetValueAsString().c_str() ); break;
              //case CBratAlgorithmParam::T_VECTOR_DOUBLE:  break;
              default:
               std::cout << "Error while creating PyArguments: Brat algorithm type " << ParamTypeVal << " is not valid!" << std::endl;
            }

            if (PyErr_Occurred()) {
                PyErr_Print();
                throw e_error_create_python_arg;
            }

            PyList_Append(pArgs, pValue);
        }
        return pArgs;
    }

    /**
     * \brief           Method to process the result of a method call.
     * \return          result     Result after conversion to proper data type.
     */
    template< typename T >
    static T& processCall( PyObject *py_result, T &result )
    {
        if (PyErr_Occurred()) {
            PyErr_Print();
            throw e_error_calling_method;
        }
        Py_XDECREF( PythonEngine::convert( py_result, result ) );
        return result;
    }


public:
    //////////////////////////////////////////////////////////////
    // CBratAlgorithmBase implementation
    //////////////////////////////////////////////////////////////

    virtual std::string GetName() const override
    {
        std::string result;
        return processCall( PyObject_CallMethod( m_instance, "GetName", nullptr ), result );
    }

    virtual std::string GetDescription() override
    {
        std::string result;
        return processCall( PyObject_CallMethod( m_instance, "GetDescription", nullptr ), result );
    }

    virtual uint32_t GetNumInputParam() override
    {
        uint32_t result;
        return processCall( PyObject_CallMethod( m_instance, "GetNumInputParam", nullptr ), result );
    }

    virtual std::string GetInputParamDesc(uint32_t indexParam) override
    {
        std::string result;
        return processCall( PyObject_CallMethod( m_instance, "GetInputParamDesc", "(i)", indexParam ), result );
    }

    virtual CBratAlgorithmParam::bratAlgoParamTypeVal GetInputParamFormat(uint32_t indexParam) override
    {
        uint32_t result;
        return (CBratAlgorithmParam::bratAlgoParamTypeVal)
                processCall( PyObject_CallMethod( m_instance, "GetInputParamFormat", "(i)", indexParam ), result );
    }

    virtual std::string GetInputParamUnit(uint32_t indexParam) override
    {
        std::string result;
        return processCall( PyObject_CallMethod( m_instance, "GetInputParamUnit", "(i)", indexParam ), result );
    }

    virtual std::string GetOutputUnit() override
    {
        std::string result;
        return processCall( PyObject_CallMethod( m_instance, "GetOutputUnit", nullptr ), result );
    }

    virtual double Run(CVectorBratAlgorithmParam& args) override
    {
        double result;
        return processCall( PyObject_CallMethod( m_instance, "Run","(O)", createPyArguments( args ) ), result );
    }
};


//////////////////////////////////////////////////////////////
//  Starting Python interpreter
//////////////////////////////////////////////////////////////
const PythonEngine& PyAlgo::sm_pe = PythonEngine::Instance();




//////////////////////////////////////////////////////////////
//							Main
//////////////////////////////////////////////////////////////


/**
 * \brief       Function to create a std::vector with the algorithm parameters.
 * \param[in]      algo        Algorithm object (type: PyAlgo).
 * \param[in,out]  args        Vector with algorithm parameters (type: CVectorBratAlgorithmParam).
 * \param[in]      argc        Number of command line arguments (type: int).
 * \param[in]      argv        Array with command line arguments (type: char*).
 * \return         ----        ---
 */
bool createAlgorithmParamVector( PyAlgo &algo, CVectorBratAlgorithmParam &args, int argc, char *argv[] )
{

    static const uint32_t first_arg_index = 3, argcount = argc; //argcount: avoid sign mismatch in loop condition

    // Checking the number of input parameters provided at command line
    if (argc - first_arg_index != algo.GetNumInputParam())
    {
        throw e_wrong_number_input_param;
    }

    // Filling the std::vector with the algorithm parameters
    for (uint32_t i = first_arg_index; i < argcount; ++i )
    {
        CBratAlgorithmParam value;
        CBratAlgorithmParam::bratAlgoParamTypeVal ParamTypeVal = algo.GetInputParamFormat(i - first_arg_index);

        switch (ParamTypeVal)
        {
          //case CBratAlgorithmParam::T_UNDEF: break;
          case CBratAlgorithmParam::T_INT:    value.SetValue( s2n<int32_t>(argv[i]) ); break;
          case CBratAlgorithmParam::T_LONG:   value.SetValue( s2n<int64_t>(argv[i]) ); break;
          case CBratAlgorithmParam::T_FLOAT:  value.SetValue( s2n<float>(argv[i])   ); break;
          case CBratAlgorithmParam::T_DOUBLE: value.SetValue( s2n<double>(argv[i])  ); break;
          case CBratAlgorithmParam::T_CHAR:   value.SetValue( static_cast<unsigned char>(s2n<unsigned>(argv[i])) ); break;
          case CBratAlgorithmParam::T_STRING: value.SetValue( std::string(argv[i])  ); break;
          //case CBratAlgorithmParam::T_VECTOR_DOUBLE:  break;
          default:
            throw e_invalid_parameter_type;
        }
        args.Insert(value);
    }

    return true;
}


int main(int argc, char *argv[])
{
    static auto usage =[]()
    {
        std::cout << "Usage: RunPythonAlgorithm <python-algorithm-file> <algorithm-class-name> [list-of-algorithm-arguments]" << std::endl;
    };

    if ( argc < 3 ) {
        std::cout << "Error calling RunPythonAlgorithm: wrong number of parameters." << std::endl;
        usage();
		return -1;
	}

    EMessageCode error_code = e_OK;
    try {

        // I. create algorithm object

        PyAlgo algo( argv[1], argv[2] );

        // II. output algorithm properties

        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Algorithm Name:        " << algo.GetName()        << std::endl;
        std::cout << "------------------------------------------------" << std::endl;

        std::cout << "Algorithm Description: " << algo.GetDescription() << std::endl;
        std::cout << "------------------------------------------------" << std::endl;

        int size = algo.GetNumInputParam();
        std::cout << "Nb. Input Parameters:  " << size                  << std::endl;

        for ( int i = 0; i < size; ++i )
        {
            std::cout << "  " << i+1 << ":" << std::endl;;
            std::cout << "    Description: " <<           algo.GetInputParamDesc(i)     << std::endl;
            std::cout << "    Format:      " << typeName( algo.GetInputParamFormat(i) ) << std::endl;
            std::cout << "    Unit:        " <<           algo.GetInputParamUnit(i)     << std::endl;
        }
        std::cout << "------------------------------------------------" << std::endl;

        std::cout << "Algorithm Output Unit: " << algo.GetOutputUnit()  << std::endl;
        std::cout << "------------------------------------------------" << std::endl;

        // III. use algorithm to compute something

        CVectorBratAlgorithmParam args;
        createAlgorithmParamVector( algo, args, argc, argv );

        std::cout << "Running Algorithm... "                        << std::endl;
        double result = algo.Run( args );
        std::cout << "The result is: " << result << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    catch( EMessageCode msg )
    {
        error_code = msg;
    }
    catch( ... )
    {
        error_code = e_unspecified_error;
    }

    if ( error_code != e_OK )
    {
        std::cout << error_messages[error_code] << std::endl;
        return -1;
    }

    return 0;
}
