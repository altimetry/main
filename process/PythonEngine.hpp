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
#ifndef PYTHON_ENGINE_HPP
#define PYTHON_ENGINE_HPP


#ifndef __cplusplus
#error Must use C++ for PythonEngine
#endif


/*

  TODO: check T_CHAR  semantics in BRAT 3.1.0
  TODO: check T_VECTOR_DOUBLE  semantics in BRAT 3.1.0
  TODO: check differences, if any, int vs. long in python 3

*/


//////////////////////////////////////////////////////////////
//				System (C standard Library)
//////////////////////////////////////////////////////////////


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <algorithm>
#include <fstream>


//////////////////////////////////////////////////////////////
//						Brathl Header
//////////////////////////////////////////////////////////////

#if defined (_MSC_VER)
#pragma warning ( disable : 4267 )
#endif

#include "libbrathl/BratAlgorithmBase.h"

#if defined (_MSC_VER)
#pragma warning ( default : 4267 )
#endif


//////////////////////////////////////////////////////////////
//						Python Headers
//////////////////////////////////////////////////////////////

// Mess caused by including Python.h

// I. avoid redefinition warnings (lots of them): save definitions

#if !defined(WIN32)
#define SAVE_XOPEN_SOURCE   _XOPEN_SOURCE
#define SAVE_POSIX_C_SOURCE _POSIX_C_SOURCE
#undef _XOPEN_SOURCE
#undef _POSIX_C_SOURCE
#endif

#if defined (HAVE_STAT)				// HAVE_STAT is defined by brathl.h
#define SAVE_HAVE_STAT HAVE_STAT
#undef HAVE_STAT				
#endif


// II. include Python headers


#include <patchlevel.h>

#if PY_VERSION_HEX < 0x03040300
#if defined(_MSC_VER) && (_MSC_VER >= 1800)
#define HAVE_ROUND
#endif
#endif

//#define Py_NO_ENABLE_SHARED	0

#if defined (_DEBUG)            //avoid automatic inclusion of pythonX_Y.lib (see .....\Python\default\include\pyconfig.h)
#undef _DEBUG
#include <Python.h>				//must come before Qt headers: has a struct with a member named "slots"...
#define _DEBUG
#else
#include <Python.h>				//must come before Qt headers: has a struct with a member named "slots"...
#endif


// III. avoid redefinition warnings: reset definitions

#if !defined(WIN32)
#undef _XOPEN_SOURCE
#undef _POSIX_C_SOURCE
#define _XOPEN_SOURCE   SAVE_XOPEN_SOURCE
#define _POSIX_C_SOURCE SAVE_POSIX_C_SOURCE
#endif

#if !defined (HAVE_STAT)
#define HAVE_STAT SAVE_HAVE_STAT
#endif



#include "new-gui/Common/+UtilsIO.h"



/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//							Python Engine
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////



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


inline const char* py_error_message( EMessageCode code )
{
	static const char *py_error_messages[ EMessageCode_size ] =
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

	static_assert( ARRAY_SIZE( py_error_messages ) == EMessageCode_size,
		"Compile time error: number of message codes doesn't match the number of messages." );


	assert__( code < EMessageCode_size );

	return py_error_messages[ code ];
}


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

    assert( (int)t < bratAlgoParamTypeVal_size );

    return bratAlgoParamTypeVal_names[t];
}





//////////////////////////////////////////////////////////////
//						Hack
//////////////////////////////////////////////////////////////

//...caused by poor python API

template< typename CHAR >
inline CHAR* ccast( const CHAR *s )
{
    return const_cast<CHAR*>( s );
}



//////////////////////////////////////////////////////////////
//						Python Engine
//////////////////////////////////////////////////////////////


/**
 * \class     PythonEngine
 * \brief     Definition of the object to hold the Python Interpreter and respective methods.
 * \ingroup
 */
class PythonEngine
{
    static PythonEngine*& InstancePointer()
    {
        static PythonEngine *pe = nullptr;
        return pe;
    }

public:
    static PythonEngine* CreateInstance( wchar_t *pypath )
    {
        static PythonEngine *&pe = InstancePointer();
		if (!pe)
			pe = new PythonEngine( pypath );

        return pe;
    }

    static PythonEngine* Instance()
    {
		return InstancePointer();
    }

protected:
    PyObject *m_global_dict = nullptr;

    PythonEngine( wchar_t *pypath )
    {
		assert__( pypath );

        Py_SetPythonHome( pypath );

        Py_InitializeEx( 0 );

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
    //types

    typedef CBratAlgorithmBase base_t;


    //static members

    //instance members

    const std::string mFilePath;    /**< The path of the algorithm python script/module. */
    const std::string mClassName;    /**< Name of the algorithm class.                    */

    PyObject *m_module = nullptr;     /**< Python object to hold the module.               */
    PyObject *m_class = nullptr;      /**< Python object to hold the algorithm class.      */
    PyObject *m_instance = nullptr;   /**< Python object to hold an instance of the class. */

    //static const PythonEngine *sm_pe; /**< PythonEngine object (Python interpreter).       */

    //construction / destruction
public:

    /**
     * \brief          User defined constructor for PyAlgo.
     * \param[in]    file_path         The path of the algorithm python script/module.
     * \param[in]    class_name         Name of the algorithm class.
     */
    PyAlgo( const std::string file_path, const std::string &class_name )
        : base_t()
        , mFilePath( file_path )
        , mClassName( class_name )
    {
        if ( !IsFile(mFilePath.c_str()) )
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

		std::cout << "- Loaded python Module object " + moduleName + " at " << m_module << std::endl;

	    // Build the name of a callable class

		m_class = PyObject_GetAttrString( m_module, mClassName.c_str() );


		// Create an instance of the class

		if (m_class && PyCallable_Check(m_class))
		{
			m_instance = PyObject_CallObject(m_class, NULL);

            std::cout << "-- Loaded a python Class object " + mClassName + " at " << m_class << std::endl;
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


    //remaining

protected:
    /**
     * \brief           Method to create a list of Python arguments.
     * \return          pArgs      Python List with python objects/arguments.
     */
    static PyObject* createPyArguments( CVectorBratAlgorithmParam &args )
    {
        // Creating python list with items of different python types (arguments)
        PyObject *pArgs   = PyList_New(0);

        for (size_t i = 0; i < args.size(); ++i )
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

	void CreateAlgorithmParamVector( CVectorBratAlgorithmParam &brat_args, char **args, size_t argcount )
	{
		// Filling the std::vector with the algorithm parameters
		for ( size_t i = 0; i < argcount; ++i )
		{
			CBratAlgorithmParam value;
			CBratAlgorithmParam::bratAlgoParamTypeVal ParamTypeVal = GetInputParamFormat( (uint32_t)i );

			switch ( ParamTypeVal )
			{
				//case CBratAlgorithmParam::T_UNDEF: break;
				case CBratAlgorithmParam::T_INT:    value.SetValue( s2n<int32_t>( args[ i ] ) ); break;
				case CBratAlgorithmParam::T_LONG:   value.SetValue( s2n<int64_t>( args[ i ] ) ); break;
				case CBratAlgorithmParam::T_FLOAT:  value.SetValue( s2n<float>( args[ i ] ) ); break;
				case CBratAlgorithmParam::T_DOUBLE: value.SetValue( s2n<double>( args[ i ] ) ); break;
				case CBratAlgorithmParam::T_CHAR:   value.SetValue( static_cast<unsigned char>( s2n<unsigned>( args[ i ] ) ) ); break;
				case CBratAlgorithmParam::T_STRING: value.SetValue( std::string( args[ i ] ) ); break;
					//case CBratAlgorithmParam::T_VECTOR_DOUBLE:  break;
				default:
					throw e_invalid_parameter_type;
			}
			brat_args.Insert( value );
		}
	}


    //////////////////////////////////////////////////////////////
    // CBratAlgorithmBase implementation
    //////////////////////////////////////////////////////////////

    virtual std::string GetName() const override
    {
        std::string result;
        return processCall( PyObject_CallMethod( m_instance, ccast( "GetName" ), nullptr ), result );
    }

    virtual std::string GetDescription() const override
    {
        std::string result;
        return processCall( PyObject_CallMethod( m_instance, ccast( "GetDescription" ), nullptr ), result );
    }

    virtual uint32_t GetNumInputParam() const override
    {
        uint32_t result;
        return processCall( PyObject_CallMethod( m_instance, ccast( "GetNumInputParam" ), nullptr ), result );
    }

	virtual std::string GetParamName(uint32_t indexParam) const override 
    {
        std::string result;
        return processCall( PyObject_CallMethod( m_instance, ccast( "GetInputParamName" ), ccast( "(i)" ), indexParam ), result );
    }

    virtual std::string GetInputParamDesc(uint32_t indexParam) const override
    {
        std::string result;
        return processCall( PyObject_CallMethod( m_instance, ccast( "GetInputParamDesc" ), ccast( "(i)" ), indexParam ), result );
    }

    virtual CBratAlgorithmParam::bratAlgoParamTypeVal GetInputParamFormat(uint32_t indexParam) const override
    {
        uint32_t result;
        return (CBratAlgorithmParam::bratAlgoParamTypeVal)
                processCall( PyObject_CallMethod( m_instance, ccast( "GetInputParamFormat" ), ccast( "(i)" ), indexParam ), result );
    }

    virtual std::string GetInputParamUnit(uint32_t indexParam) const override
    {
        std::string result;
        return processCall( PyObject_CallMethod( m_instance, ccast( "GetInputParamUnit" ), ccast( "(i)" ), indexParam ), result );
    }

    virtual std::string GetOutputUnit() const override
    {
        std::string result;
        return processCall( PyObject_CallMethod( m_instance, ccast( "GetOutputUnit" ), nullptr ), result );
    }

    virtual double Run(CVectorBratAlgorithmParam& args) override
    {
        double result;
        return processCall( PyObject_CallMethod( m_instance, ccast( "Run" ),ccast( "(O)"), createPyArguments( args ) ), result );
    }
};


#endif //PYTHON_ENGINE_HPP
