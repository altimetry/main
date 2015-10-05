
#include "RunPythonAlgorithm-Tools.hpp"


//////////////////////////////////////////////////////////////
//						File Utilities
//////////////////////////////////////////////////////////////

bool IsFile( const char *name )
{
    if (FILE *file = fopen(name, "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

bool readFile( const std::string &path, std::string &content )
{
	std::fstream f( path, std::ios::in );
	content = std::string((std::istreambuf_iterator<char>(f)),
					 std::istreambuf_iterator<char>());
	return !!f;
}




//////////////////////////////////////////////////////////////////
//					for utf8 <-> utf16
//////////////////////////////////////////////////////////////////


#include <boost/locale.hpp>           //for utf8 <-> utf16 conversions


//this is a boost header sentinel to not force all users to include boost; if one wants to use this, then include, before this file:
//
//#include <boost/locale.hpp>           //for utf8 <-> utf16 conversions
//
#if defined(BOOST_LOCALE_HPP_INCLUDED)

    inline std::wstring utf8to16( const std::string utf8_string )
    {
        std::wstring utf16_string = boost::locale::conv::utf_to_utf< wchar_t >( utf8_string );
        return utf16_string;
    }
    inline std::string utf16to8( const std::wstring utf16_string )
    {
        std::string utf8_string = boost::locale::conv::utf_to_utf< char >( utf16_string );
        return utf8_string;
    }

#endif


////////////////////////////////// string_traits //////////////////////////////////
//

template< typename STRING >
struct string_traits;

template<>
struct string_traits< std::string >
{
    typedef std::stringstream				str_stream_type;
    typedef std::ostringstream				ostr_stream_type;
    typedef std::basic_ostream< char >		ostream_type;
    typedef std::basic_istream< char >		istream_type;
	typedef std::streambuf					streambuf_type;

    static inline ostream_type& tcout()
    {
        return std::cout;
    }
    static inline ostream_type& tcerr()
    {
        return std::cerr;
    }
    static inline istream_type& tcin()
    {
        return std::cin;
    }
};

template<>
struct string_traits< std::wstring >
{
    typedef std::wstringstream              str_stream_type;
    typedef std::wostringstream				ostr_stream_type;
    typedef std::basic_ostream< wchar_t >   ostream_type;
    typedef std::basic_istream< wchar_t >   istream_type;
	typedef std::wstreambuf					streambuf_type;

    static inline ostream_type& tcout()
    {
        return std::wcout;
    }
    static inline ostream_type& tcerr()
    {
        return std::wcerr;
    }
    static inline istream_type& tcin()
    {
        return std::wcin;
    }
};


///////////////////////////
//		REDIRECTORS
///////////////////////////
//
//	to redirect std::cout/std::cin at instance scope: declare a variable of the type, passing
//	the stream to redirect out/input as argument; the destructor will restore streamimg to the
//	standard objects.
//

template< class STRING >
class std_cout_redirector
{
	typedef typename string_traits< STRING >::streambuf_type	streambuf_type; 
	typedef typename string_traits< STRING >::ostream_type		ostream_type; 

	streambuf_type *m_coutbuf;

public:
	static inline ostream_type& tcout()
	{
		return string_traits< STRING >::tcout();
	}

	std_cout_redirector( ostream_type &out )
	{
		m_coutbuf = tcout().rdbuf();
		tcout().rdbuf( out.rdbuf() );
	}

	~std_cout_redirector()
	{
		tcout().rdbuf( m_coutbuf ); //reset to standard output
	}
};

template< class STRING >
class std_cerr_redirector
{
	typedef typename string_traits< STRING >::streambuf_type	streambuf_type; 
	typedef typename string_traits< STRING >::ostream_type		ostream_type; 

	streambuf_type *m_cerrbuf;

public:
	static inline ostream_type& tcerr()
	{
		return string_traits< STRING >::tcerr();
	}

	std_cerr_redirector( ostream_type &err )
	{
		m_cerrbuf = tcerr().rdbuf();
		tcerr().rdbuf( err.rdbuf() );
	}

	~std_cerr_redirector()
	{
		tcerr().rdbuf( m_cerrbuf ); //reset to standard output
	}
};


void process_expression( int num, const char *func_name )
{
    //FILE*        py_file_handle;

    // Initialize a global variable for display of expression results
    //
    PyRun_SimpleString( "x = 0" );
    PyRun_SimpleString(
                "def add(a,b):"
                "	return a+b" );

    // Open and execute the Python file
    //
    //py_file_handle = fopen( exp, "r" );
    //PyRun_SimpleFile(py_file_handle, exp);

    // Get a reference to the main module and global dictionary
    //
    PyObject *main_module = PyImport_AddModule("__main__");
    PyObject *global_dict = PyModule_GetDict(main_module);

    // Extract a reference to the function "func_name" from the global dictionary
    //
    PyObject *expression = PyDict_GetItemString(global_dict, func_name);

    while(num--) {
        // Make a call to the function referenced by "expression"
        //
        PyObject_CallObject( expression, NULL );
    }
    PyRun_SimpleString( "print(x)" );
}


// call_class.c - A sample of python embedding
// (calling python classes from C code)
//
int main_prop(int argc, char *argv[])
{
    PyObject *pName, *pModule, *pDict, *pClass, *pInstance, *pValue;
    int i, arg[2];
    if (argc < 4)
    {
        printf("Usage: exe_name python_fileclass_name function_name\n");
        return 1;
    }
    // Initialize the Python Interpreter
    Py_Initialize();
    // Build the name object
    pName = PyUnicode_FromString(argv[1]);
    // Load the module object
    pModule = PyImport_Import(pName);
    // pDict is a borrowed reference
    pDict = PyModule_GetDict(pModule);
    // Build the name of a callable class
    pClass = PyDict_GetItemString(pDict, argv[2]);
    // Create an instance of the class
    if (PyCallable_Check(pClass))
    {
        pInstance = PyObject_CallObject(pClass, NULL);
    }
    // Build the parameter list
    if( argc > 4 )
    {
        for (i = 0; i < argc - 4; i++)
        {
            arg[i] = atoi(argv[i + 4]);
        }
        // Call a method of the class with two parameters
        pValue = PyObject_CallMethod(pInstance,
                                     argv[3], "(ii)", arg[0], arg[1]);
    } else
    {
        // Call a method of the class with no parameters
        pValue = PyObject_CallMethod(pInstance, argv[3], NULL);
    }
    if (pValue != NULL)
    {
        printf("Return of call : %d\n", PyLong_AsLong(pValue));
        Py_DECREF(pValue);
    }
    else
    {
        PyErr_Print();
    }
    // Clean up
    Py_DECREF(pModule);
    Py_DECREF(pName);
    // Finish the Python Interpreter
    Py_Finalize();

	return 0;
}
