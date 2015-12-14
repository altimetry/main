#ifndef BRAT_CROSS_PLATFORM_UTILS_H
#define BRAT_CROSS_PLATFORM_UTILS_H

#if defined (CROSS_PLATFORM_UTILS_H)
#error Wrong +Utils.h included 
#endif

#ifndef __cplusplus
#error Must use C++ for +utils.h
#endif


#include <stdarg.h>

#include <string.h>
#include <cassert>
#include <algorithm>
#include <stdexcept>
#include <cctype>

#include <vector>
#include <map>
#include <iterator>
#include <iomanip>

#include <time.h>


#if !defined (USE_STR_STREAM)
#include <sstream>
typedef std::stringstream stream_t;
#else
#include <strstream>
typedef std::strstream stream_t;
#endif


#if !defined(far)
#define far
#endif

//Avoid problems if windows headers are included before this none
//
#if defined (WIN32)	|| defined (_WIN32)
#if defined (min)
#undef min
#endif
#if defined (max)
#undef max
#endif
#endif



////////////////////////////////
//	PRE-PROCESSOR UTILITIES
////////////////////////////////

//
//DELAYED_STRINGIFICATION: helper macro to allow stringification of the result of a macro expansion
//DELAYED_T: helper macro to allow _T (concatenation of L) to the result of a macro expansion
//
#define STRINGIFY(TOKEN)				#TOKEN
#define DELAYED_STRINGIFICATION(TOKEN)	STRINGIFY(TOKEN)	//applying # directly would prevent TOKEN macro expansion
#define DELAYED_T(TOKEN)				_T(TOKEN)			//same reason: applying _T directly would prevent TOKEN macro expansion




//	- to be used like this, to see macro #definitions:
//
//		#pragma message ( OUTPUT_MACRO( THE_MACRO_I_WANT_TO_INSPECT ) )		//TODO: find equivalent pragma for Linux
//
//	- does NOT work with function macros
//
#define OUTPUT_MACRO_VALUE(a, b) "Information: macro " a## " defined as " #b

#define OUTPUT_MACRO(a) OUTPUT_MACRO_VALUE(#a, a)






//////////////////////////////////////////
//	assert__ : cross-platform assert
//////////////////////////////////////////


#if defined (__unix__)		//#if defined (__STDC_VERSION__) 

#include <csignal>

//#define DEBUG_BREAK asm("int $3")					//int $3 hangs on Debian (at least)
//#define assert__(TEST) if(!(TEST)) asm("int $3");

#if defined (_DEBUG) || defined(DEBUG)

#	define assert__(TEST) if(!(TEST)) ::raise(SIGTRAP); else ((void)0)
#else
#	define assert__(TEST)
#endif

#elif defined (_MSC_VER) || defined(__APPLE__)			// (__unix__)

#if defined (_DEBUG) || defined(DEBUG)
#	define assert__(TEST) assert( (TEST) )
#else
#	define assert__(TEST)
#endif

#endif								//(_MSC_VER)



//////////////////////////////////////////
//		Must come before STL...	 (?)
//////////////////////////////////////////

#if defined (__STDC_VERSION__) 
//macros taken from gcc site
//
#if __STDC_VERSION__ < 199901L
# if __GNUC__ >= 2
#  define __func__ __FUNCTION__
# else
#  define __func__ "<unknown>"
# endif
#endif

#elif defined (_MSC_VER)
#  define __func__ __FUNCTION__
#endif

//////////////////////////////////////////
//		Must come before STL...
//////////////////////////////////////////

/*
#if defined(_MSC_VER) && (_MSC_VER >= 1310)
//
//	- _HAS_ITERATOR_DEBUGGING must be defined before yvals;
//		does not work to undefine it after the templates (such as in <limits>) have seen it defined to 1 in _DEBUG mode
//
//	- (old note on _SECURE_SCL):	//All standard iterators are unchecked (same behavior as specified by the C++ standard).
//
#	undef _HAS_ITERATOR_DEBUGGING
#	define _HAS_ITERATOR_DEBUGGING 0	

#	undef _SECURE_SCL
#	define _SECURE_SCL 0			//if this sh... thing is defined, you cannot clear an empty vector (just created, for instance)

#	undef _ITERATOR_DEBUG_LEVEL
#	define _ITERATOR_DEBUG_LEVEL 0

#	include <yvals.h>
#endif
*/

//#include <vector>



//////////////////////////////////////////
//   		Compile-Time assert
//////////////////////////////////////////

/*
template<bool> struct CTASSERT;
template<> struct CTASSERT<true>{ CTASSERT<true>(){} };
//#define ctassert(x) CTASSERT< (x) != 0 >::CTASSERT< (x) != 0 >();
//#define ctassert(x) CTASSERT_FUNCTION( CTASSERT< (x) != 0 >() );
#define ctassert(x) typedef CTASSERT< (x) != 0 > STATIC_ASSERTION_FAILED;
//#define ctassert_local(x) { CTASSERT< (x) != 0 >(); }
*/
template<bool> struct STATIC_ASSERTION_FAILED_T;
typedef STATIC_ASSERTION_FAILED_T< true > In_fact_a_static_assertion_failed_here_SO_DISREGARD_THE_FOLLOWING_COMPILER_ERROR_MESSAGE;
#define ctassert(x) typedef STATIC_ASSERTION_FAILED_T< (x) > In_fact_a_static_assertion_failed_here_SO_DISREGARD_THE_FOLLOWING_COMPILER_ERROR_MESSAGE;



//////////////////////////////////////////
//
//////////////////////////////////////////


#define nullInt (-1)  //sempre equivalente ao maior unsigned int, seja este de 16 ou 32 bits

template< typename I >
inline bool
IsNull(I n)
{
    return n == (I)nullInt;
}



////////////////
//	UNICODE
////////////////

/*
INFO: UNICODE and _UNICODE Needed to Compile for Unicode
ID: Q99359

--------------------------------------------------------------------------------
The information in this article applies to:

Microsoft Win32 Software Development Kit (SDK), used with:
the operating system Microsoft Windows NT, versions 3.1, 3.5, 3.51, 4.0
the operating system Microsoft Windows 2000

--------------------------------------------------------------------------------


SUMMARY
To compile code for Unicode, you need to #define UNICODE for the Win32 header files and #define _UNICODE for the C Run-time header
 files. These #defines must appear before the following:
   #include <windows.h>
and any included C Run-time headers. The leading underscore indicates deviance from the ANSI C standard. Because the Windows header
files are not part of this standard, it is allowable to use UNICODE without the leading underscore.
*/

#if defined (UNICODE) || defined (_UNICODE)		//Old comment: Dado Q99359 acima, como uso _UNICODE, que UNICODE seja tb definido.
#	undef UNICODE								//New comment (17/09/09): anyway, ensure that it is enough to use UNICODE (not also _UNICODE) as flag for unicode environment tests
#	undef _UNICODE
#	define UNICODE
#	define _UNICODE
#endif


#include <string>
//#include <sstream>		//above
#include <iostream>


//this is a boost header sentinel to not force all users to include boost; if one wants to use this, then include, before this file:
//
//#include <boost/archive/iterators/mb_from_wchar.hpp>    //for UNICODE/MBCS literals
//#include <boost/archive/iterators/wchar_from_mb.hpp>    //for UNICODE/MBCS literals
//
#if defined( BOOST_ARCHIVE_ITERATORS_MB_FROM_WCHAR_HPP )

	typedef boost::archive::iterators::wchar_from_mb< const char* > atranslator;
	typedef boost::archive::iterators::mb_from_wchar< const wchar_t* > wtranslator;


	inline std::wstring a2w( const char *a, size_t length = (size_t)-1 )
	{
		return std::wstring( atranslator( a ), atranslator( a + ( length == (size_t)-1 ? strlen( a ) : length ) ) );
	}

	inline std::string w2a( const wchar_t *w, size_t length = (size_t)-1 )
	{
		return std::string( wtranslator( w ), wtranslator( w + ( length == (size_t)-1 ? wcslen( w ) : length ) ) );
	}

	inline std::wstring a2w( const std::string& a )
	{
		return a2w( a.c_str(), a.length() );
	//    std::wstring temp(s.length(),L' ');
	//    std::copy(s.begin(), s.end(), temp.begin());
	//    return temp;
	}
	inline std::string w2a( const std::wstring& w )
	{
		return w2a( w.c_str(), w.length() );
	//    std::string temp(s.length(),' ');
	//    for ( size_t i = 0; i < s.length(); ++i )
	//        temp[i] = (char)s[i];
	//    //std::copy(s.begin(), s.end(), temp.begin());
	//    return temp;
	}

	// t 2 X ////////////////////////////////////////////////////////////////////////////////////////////////////////


	inline std::string t2a( const std::string &s )
	{
		return s;
	}
	inline std::string t2a( const std::wstring &s )
	{
		return w2a( s );
	}

	inline std::wstring t2w( const std::string &s )
	{
		return a2w( s );
	}
	inline std::wstring t2w( const std::wstring &s )
	{
		return s;
	}


	inline const std::string& t2ac( const std::string &s )
	{
		return s;
	}
	inline std::string t2ac( const std::wstring &s )
	{
		return w2a( s );
	}

	inline std::wstring t2wc( const std::string &s )
	{
		return a2w( s );
	}
	inline const std::wstring& t2wc( const std::wstring &s )
	{
		return s;
	}


	// X 2 t ////////////////////////////////////////////////////////////////////////////////////////////////////////

	template< class STRING >
	inline STRING w2t( const std::wstring &s );

	template< class STRING >
	inline STRING a2t( const std::string &s );

	template<>
	inline std::wstring w2t( const std::wstring &s )
	{
		return s;
	}

	template<>
	inline std::string w2t( const std::wstring &s )
	{
		return w2a( s );
	}

	template<>
	inline std::wstring a2t( const std::string &s )
	{
		return a2w( s );
	}

	template<>
	inline std::string a2t( const std::string &s )
	{
		return s;
	}


	template< class STRING >
	inline STRING w2tc( const std::wstring &s );

	template< class STRING >
	inline STRING a2tc( const std::string &s );

	template<>
	inline const std::wstring& w2tc( const std::wstring &s )
	{
		return s;
	}

	template<>
	inline std::string w2tc( const std::wstring &s )
	{
		return w2a( s );
	}

	template<>
	inline std::wstring a2tc( const std::string &s )
	{
		return a2w( s );
	}

	template<>
	inline const std::string& a2tc( const std::string &s )
	{
		return s;
	}


	//////////////////////////////////// "polymorphic" string literals ////////////////////////////////////


	template< typename CHAR >
	struct poly_string_literal;

	template<>
	struct poly_string_literal< char >
	{
		static inline std::string TS_( const char *a ){ return std::string( a ); }

		static inline std::string TS_( const char &a ){ return std::string( a, 1 ); }
	};

	template<>
	struct poly_string_literal< wchar_t >
	{
		static inline std::wstring TS_( const char *a )
		{ 
			return a2w( a ); 
		}

		static inline std::wstring TS_( const char &a )
		{ 
			return a2w( &a, 1 ); 
		}
	};



	namespace std {					//otherwise, if defined at global scope and std:: qualified, the  the compiler won't match it

		inline wostream& operator << ( wostream &o, const char *s )
		{
			return o << a2w( s );
		}

		inline wostream& operator << ( wostream &o, const std::string s )
		{
			return o << a2w( s );
		}
	}

#endif




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




#if defined (UNICODE)													 //TODO for all this file: try to avoid std_string_t uses (dependency of macro UNICODE) and REPLACE BY TEMPLATES
	typedef std::wstring std_string_t;
	typedef std::wstringstream std_stringstream_t;
#else
	typedef std::string std_string_t;
	typedef std::stringstream std_stringstream_t;
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


////////////////////////////////// char_traits //////////////////////////////////
//

template< typename CHAR >
struct char_traits;

template<>
struct char_traits< char >
{
    typedef std::string									string_type;
    typedef string_traits< string_type >::ostream_type	ostream_type;

    //typedef std::basic_ostream< char, std::char_traits< char > > ostream_t;
};

template<>
struct char_traits< wchar_t >
{
    typedef std::wstring								string_type;
    typedef string_traits< string_type >::ostream_type	ostream_type;

    //typedef std::basic_ostream< wchar_t, std::char_traits< wchar_t > > ostream_t;
};





//////////////////////////////////////////////////////////////////
//	To be able to return references to an empty string singleton
//////////////////////////////////////////////////////////////////


template< class STRING >
inline const STRING& empty_string()
{
	static const STRING empty;
	return empty; 
}

//////////////////////////////////////////////////////////////////
//					STANDARD IN/OUTPUT
//////////////////////////////////////////////////////////////////



template< class STRING >
inline typename string_traits< STRING >::ostream_type& std_tcout()
{
	return string_traits< STRING >::tcout();
}

template< class STRING >
inline typename string_traits< STRING >::istream_type& std_tcin()
{
	return string_traits< STRING >::tcin();
}



#if defined (UNICODE)

	inline std::wostream& tcout()
	{ 
        return string_traits< std::wstring >::tcout();
	}

#else
	inline std::ostream& tcout()
	{ 
        return string_traits< std::string >::tcout();
    }
#endif



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

//!!!!!!!!!!!!!	cin redirector NOT TESTED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
template< class STRING >
class std_cin_redirector
{
	typedef typename string_traits< STRING >::streambuf_type	streambuf_type; 
	typedef typename string_traits< STRING >::istream_type		istream_type; 

	streambuf_type *m_cinbuf;

public:
	static inline istream_type& tcin()
	{
		return string_traits< STRING >::tcin();
	}

	std_cin_redirector( istream_type &in )
	{
		m_cinbuf = tcin().rdbuf();
		tcin().rdbuf( in.rdbuf() );
	}

	~std_cin_redirector()
	{
		tcin().rdbuf( m_cinbuf );	//reset to standard input
	}
};




//////////////////////////////////////////////////////////////////
//					DECLARE_ARRAY_SIZE
//////////////////////////////////////////////////////////////////


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))

#define DECLARE_ARRAY_SIZE_DEPRECATED(a) size_t a##_size = ARRAY_SIZE(a)

#define DECLARE_ARRAY_SIZE_(a) size_t a##_size

#define DEFINE_ARRAY_SIZE(a) DECLARE_ARRAY_SIZE_(a) = ARRAY_SIZE(a)


//////////////////////////////////////////////////////////////////
//				Create vectors from arrays
//////////////////////////////////////////////////////////////////


template< class T >
std::vector< T > Array2VectorT ( std::vector< T > &v, const T *array, size_t size )
{
	v.clear();
	for ( size_t i = 0; i < size; ++i )
		v.push_back( array[i] );
	return v;
}

template< class T >
std::vector< T > Array2VectorT ( const T *array, size_t size )
{
	std::vector< T > v;
	return Array2VectorT ( v, array, size );
}


template< class STRING >
std::vector< STRING > & vArray2Vector ( std::vector< STRING > &v, const typename STRING::value_type *head, va_list args )
{
	v.clear();
	while ( head ) {
		v.push_back( head );
        head = va_arg( args, const typename STRING::value_type* );
	}
	return v;
}

template< class STRING >
std::vector< STRING > Array2Vector (std::vector< STRING > &v, const typename STRING::value_type *head, ...)
{
	va_list args;
	va_start ( args, head );
	vArray2Vector (v, head, args );
	va_end ( args );
	return v;
}

template< class STRING >
std::vector< STRING > Array2Vector (const typename STRING::value_type *head, ...)
{
	std::vector< STRING > v;

	va_list args;
	va_start( args, head );
	vArray2Vector (v, head, args );
	va_end( args );
	return v;
}



//////////////////////////////////////////////////////////////////
//			Create vectors of strings from strings
//////////////////////////////////////////////////////////////////

template< class STRING >
void String2Vector( std::vector< STRING > &v, const STRING &s, const STRING &nl = STRING( "\n" ) )
{
	const size_t nl_len = nl.length();
	size_t pos;
	STRING source(s);
	do {
		pos = source.find( nl );
		if ( pos != STRING::npos )
		{
			v.push_back( source.substr( 0, pos ) );
			source = source.substr( pos + nl_len );
		}
		else
			v.push_back( source );
	} while ( pos != STRING::npos );
}


template< class STRING >
std::vector< STRING > String2Vector( const STRING &s, const STRING &nl = STRING( "\n" ) )		//STRING::value_type => STRING (2014/04/25)
{
	std::vector< STRING > v;
	String2Vector( v, s, nl );
	return v;
}


//////////////////////////////////////////////////////////////////
//			Create strings from vectors of strings
//////////////////////////////////////////////////////////////////

template< class STRING >
STRING Vector2String( const std::vector< STRING >&v, const STRING separator = STRING( "," ) )	//STRING::value_type => STRING (2015/12/12)
{
	STRING result;
	if ( !v.empty() ){
		result= v[0];
        for ( typename std::vector< STRING >::const_iterator ii = v.begin() + 1; ii != v.end(); ++ii )
			result += (separator + *ii);
	}
	return result;
}
template< class STRING >
inline STRING Vector2String( const std::vector< STRING >&v, const typename STRING::value_type *separator = ",")	//STRING::value_type => STRING (2015/12/12)
{
	return Vector2String( v, STRING( separator ) );
}



//////////////////////////////////////////////////////////////////
//				+= operator for std::vector < T >
//////////////////////////////////////////////////////////////////
//
//  Concatenates two vectors. The first argument is the result
//  of the operation.
//
//  @param vDest
//  		Vector where the copies of the vSource items are
//  		appended.
//  @param vSource
//  		Vector whose items are be copied to vDest.
//
//  @return
//  		vDest with the copies of the vSource items appended at
//  		the end and in the same order.
//
template< typename T >
inline std::vector< T >& operator += (std::vector< T > &vDest, const std::vector< T > &vSource )
{
	vDest.reserve(vDest.size() + vSource.size());
	std::copy ( vSource.begin(), vSource.end(), std::back_inserter( vDest ));
	return vDest;
}




//////////////////////////////////////////////////////////////////
//		Destroy Containers of Pointers (deleting the pointers)
//////////////////////////////////////////////////////////////////


template< typename T >
inline void	destroyPointersAndContainer( std::vector< T* > &c )
{
	while ( !c.empty() ) delete c.back(), c.pop_back();
}
template< typename K, typename T >
inline void	destroyPointersAndContainer( std::map< K, T* > &c )			//check better this function
{
	while ( !c.empty() ) delete c.begin()->second, c.erase( c.begin()->first );
}
template< typename CAST, typename T >
inline void	destroyPointerCastsAndContainer( std::vector< T* > &c )
{
	while ( !c.empty() ) delete dynamic_cast<CAST*>( c.back() ), c.pop_back();
}






//////////////////////////////////////////////////////////////////
//						Compare Strings
//////////////////////////////////////////////////////////////////


template< class STRING, class PRED >
bool compare_( STRING const& a, STRING const& b, const PRED &pred )
{
	if ( a.length() != b.length() )
		return false;

	return std::equal(
		a.begin(), a.end(),
		b.begin(), pred );
}


template< class STRING >
bool str_icmp( STRING const& a, STRING const& b )
{
	static const auto pred = [](typename STRING::value_type a, typename STRING::value_type b)
	{
		return std::tolower(a) == std::tolower(b);
	};

	return compare_( a, b, pred );
}

template< class STRING >
bool str_cmp( STRING const& a, STRING const& b )
{
	static const auto pred = [](typename STRING::value_type a, typename STRING::value_type b)
	{
		return a == b;
	};

	return compare_( a, b, pred );
}


//convenience for pointers

template< class CHAR >
bool str_icmp( const CHAR *a, const CHAR *b )
{
	typedef typename char_traits< CHAR >::string_type string_type;

	return str_icmp( string_type( a ), string_type( b ) );
}






//////////////////////////////////////////////////////////////////
//				Change std string case
//////////////////////////////////////////////////////////////////


template< typename CHAR >
struct upper_char
{
    int operator()( int c )
    {
        return std::toupper((CHAR)c);
    }
};

template< class STRING >
inline STRING& toUpper( STRING &s )
{
    std::transform( s.begin(), s.end(), s.begin(), upper_char< typename STRING::value_type > () );
    return s;
}

template< typename CHAR >
struct lower_char
{
    int operator()( int c )
    {
        return std::tolower((CHAR)c);
    }
};

template< class STRING >
inline STRING& toLower( STRING &s )
{
    std::transform( s.begin(), s.end(), s.begin(), lower_char< typename STRING::value_type >() );
    return s;
}


template< class STRING >
inline STRING ToUpperCopy( const STRING &s )
{
    STRING s2(s);
    return toUpper(s2);
}

template< class STRING >
inline STRING ToLowerCopy( const STRING &s )
{
    STRING s2(s);
    return toLower(s2);
}


//////////////////////////////////////////////////////////////////
//				Convert numbers to strings
//////////////////////////////////////////////////////////////////

// with hexadecimal option

template< class STRING, typename T >
STRING hn2s( T n, bool hex )
{
    //return ((typename string_traits< STRING >::str_stream_type&)(typename string_traits< STRING >::str_stream_type() << n)).str();	//<< yields ostream type (not stringstream)
    typename string_traits< STRING >::str_stream_type st;
	if ( hex )
		st << std::hex;
	st << n;
    if ( !st )
        throw std::invalid_argument( "Invalid number." );
    return st.str();                                        //<< yields ostream type (not stringstream)
}

// with precision

template< class STRING, typename T >
STRING n2s( T n, std::streamsize precision )
{
    typename string_traits< STRING >::str_stream_type st;
	st << std::setprecision(precision) << n;
    if ( !st )
        throw std::invalid_argument( "Invalid number." );
    return st.str();                                        //<< yields ostream type (not stringstream)
}


// default

template< class STRING, typename T >
STRING n2s( T n )
{
    return hn2s< STRING >( n, false );
}

// replaces itoa

template< typename T >
std_string_t itoa( T n )
{
	return ((std_stringstream_t&)(std_stringstream_t() << n)).str();	//<< yields ostream type (not stringstream)
}

// hexadecimal

template< class STRING, typename T >
STRING hn2s( T n )
{
    return hn2s< STRING >( n, true );
}


//////////////////////////////////////////////////////////////////
//				Convert strings to numbers
//////////////////////////////////////////////////////////////////


//replaces atoi

/**
 * \brief       Function to convert a std::string to a number (of a selected data type).
 * \param[in]   s     String to be converted.
 * \return      v     Converted number
 */
template< typename T, class STRING >
inline T s2n( const STRING& s )
{
    typename string_traits< STRING >::str_stream_type st( s );
    T v;
    st >> v;
    if ( !st )
        throw std::invalid_argument( std::string( s.begin(), s.end() ) + ": Invalid number format." );
    return v;
}


template< class T >
inline T atoi( const std_string_t& s )
{
    T v;
    std_stringstream_t( s ) >> v;
    return v;
}


// for pointers

template< typename T, typename CHAR >
inline T s2n( CHAR *s )
{
    typedef typename char_traits< CHAR >::string_type string_type;

	return s2n< T >( string_type( s ) );
}




//////////////////////////////////////////////
//	  										//
//   	Template functions to t_Bufs:
//		Tstrlen, Tstrcmp,
//		Tinsert, Tdelete, Tinvert
//              	         	  			//
//////////////////////////////////////////////


//
// returns char before Sep or last char (e)
//
template< typename I, typename L >
inline I
EndItem(L far* Bf, I b, I e, L far* Sep)
{
    I i = Tscan(b, Bf, e, (const L*)Sep, (I)1);   //cast only to force the creation of source Tscan from the template
    return IsNull(i) ? e : i-1;
}

template< typename L >			//cannot have typename I because it doesn't have I parameters
inline std::size_t
Tstrlen(const L far* Bf)
{
    std::size_t count;
    for (count = 0; *Bf++; count++);
    return count;
}
//
//	- it is expected that, like strlen, strcmp supports strings untill size_t, in spite of returning int (this obs. only makes sense in win16)
//	- it returns int because the original returns int and because in the return value we only care about the relation with 0 (>, < ou ==)
//
template< typename L >
inline int
Tstrcmp(const L far* Bf1, const L far* Bf2)
{
    return strcmp((char far*)Bf1, (char far*)Bf2);    //return _tcscmp((TCHAR*)Bf1, (TCHAR*)Bf2);
}
template< typename I, typename L >
inline L far*
Tinvert(L far* Bf, I BfLen)
{
    L tmp;
    for (I i = 0, j = BfLen - 1; i < j; i++, j--){
        tmp	  = Bf[i];
        Bf[i] = Bf[j];
        Bf[j] = tmp;
    }
    return Bf;
}
//
//	- void *memmove( void *dest, const void *src, size_t count );
//	move to 1st position of the del string the 1st position after the cut the
//	final size - number before the cut (= 1st position of the cut)
//
template < typename I, typename L >
void
Tdelete(L far* const Bf, I &BfLen, I Where, I Count)
{
    assert(BfLen >= 0 && Where >= 0 && Count >= 0 && (BfLen - Where >= Count));

    BfLen -= Count;
    memmove(&Bf[Where], &Bf[Where + Count], (size_t)(BfLen - Where)*sizeof(L));
}
//
//	- unlike Tdelete, it can return false: lack of space is not considered error
//	- Where can be == BfLen for insertion at the end
//
template< typename I, typename L >
bool
Tinsert(const L far* Source, I SizeS, L far* const Bf, I far &BfLen, I TotalLen, I Where)
{
    assert(SizeS >= 0  && BfLen >= 0 && TotalLen >= 0 && Where >= 0 && Where <= BfLen);

    if (SizeS > TotalLen - BfLen)
        return false;

    memmove((void *)&Bf[Where+SizeS], &Bf[Where], (size_t)(BfLen-Where)*sizeof(L));
    memmove((void *)&Bf[Where], Source, (size_t)SizeS*sizeof(L));

    BfLen += SizeS;
    return true;
}
//
//	- accepts Till - From + 1 == 0 (i.e. insertion len == 0)
//	- in Pascal: procedure Move(var Source, Dest; Count: Word);
//	- see Tinsert for other comments
//
template< typename I, typename L >
bool
TselfInsert(I From, I Till, L far* const Bf, I far &BfLen, I TotalLen, I Where)
{
    assert(From >= 0  && Till >= 0  && BfLen >= 0 && TotalLen >= 0 && Where >= 0);
    assert(Where <= BfLen && From < BfLen && Till < BfLen);

    I SizeS = Till - From + 1;
    if (Where > Till)
        return Tinsert(&Bf[From], SizeS, Bf, BfLen, TotalLen, Where);

    if (SizeS > TotalLen - BfLen)
        return false;

    memmove((void *)&Bf[Where+SizeS], &Bf[Where], (size_t)(BfLen-Where)*sizeof(L));		//move de where (inc) em diante, size bytes
    I Size1;
    if (Where > From)
        Size1 = Where - From;		//1st Source block to be moved: previous move cut Source in where
    else {
        From += SizeS;
        Size1 = SizeS; 				//1st Source block to be moved is Size if previous didn't break Source
    }
    memmove((void *)&Bf[Where], &Bf[From], (size_t)Size1*sizeof(L));						//move 1st block
    memmove((void *)&Bf[Where+Size1], &Bf[Where+SizeS], (size_t)(SizeS-Size1)*sizeof(L));	//move 2nd block, from the position where it was placed in the 1st move

    BfLen += SizeS;
    return true;
}



//////////////////////////////////////////////
//
//   			Find&Replace
//
//////////////////////////////////////////////


template< typename STRING >
inline bool startsWith( const STRING &s, const STRING &substr )
{
   size_t pos = s.find( substr, 0 );
   return pos != STRING::npos && ( s.substr( 0, substr.length() ) == substr );
}


template< typename STRING >
inline bool endsWith( const STRING &s, const STRING &substr )
{
	std::size_t found = s.find( substr );
	return ( found != STRING::npos ) && found == ( s.length() - substr.length() );	//condition found != STRING::npos is critical (if its value is -1, it can issue a false positive)
}

template< typename STRING >
inline 
typename STRING::size_type i_find( const STRING &source, const STRING &find, size_t pos = 0 )
{
	auto it = std::search(
		source.begin() + pos, source.end(), find.begin(), find.end(),
		[]( typename STRING::value_type ch1, typename STRING::value_type ch2 )
		{ 
			return std::toupper( ch1 ) == std::toupper( ch2 ); 
		}
	);

        return ( it == source.end() ?  STRING::npos  :  it - source.begin() );
}

//template< typename STRING >
//inline STRING& FindAndReplace( STRING& source, const typename STRING::value_type *find, const typename STRING::value_type *replace, size_t pos = 0 )
//{
//																	assert__( find && replace );
//	size_t findLen = Tstrlen( find );
//	size_t replaceLen = Tstrlen( replace );
//
//	while ( ( pos = source.find( find, pos) ) != std::string::npos ) {
//		source.replace( pos, findLen, replace );
//		pos += replaceLen;
//	}
//	return source;
//}


template< typename STRING, typename TOKEN_STRING >
STRING replace( const STRING &str, const TOKEN_STRING &strToFind, const TOKEN_STRING &replaceBy, bool compareNoCase = false )
{
    std::string strRet = str;

    auto pos = compareNoCase ? i_find( strRet, strToFind ) : strRet.find( strToFind );
    while (pos != std::string::npos)
    {
        strRet.replace( pos, strToFind.length(), replaceBy );

        if ( compareNoCase ){  pos = i_find(strRet, strToFind, pos + replaceBy.length() ); }
        else                {  pos = strRet.find( strToFind, pos + replaceBy.length() );  }
    }
    return strRet;
}

template< typename STRING >
STRING replace( const STRING &str, const typename STRING::value_type *strToFind, const typename STRING::value_type *replaceBy, bool compareNoCase = false )
{
	return replace( str, STRING( strToFind ), STRING( replaceBy ), compareNoCase );
}







#include <cwctype>


template< typename STRING > 
struct isspace_t;

template<> 
struct isspace_t< std::string >
{
	typedef int parameter_type;

	static inline int is( parameter_type ch )
	{
		return std::isspace( ch );
	}
};
template<> 
struct isspace_t< std::wstring >
{
	typedef wint_t parameter_type;

	static inline int is( parameter_type ch )
	{
		return std::iswspace( ch );
	}
};


template< typename STRING >
inline STRING &ltrim( STRING &s ) 
{
    s.erase( s.begin(), std::find_if( s.begin(), s.end(), std::not1( std::ptr_fun< typename isspace_t<STRING>::parameter_type, int >( isspace_t<STRING>::is ) ) ) );
    return s;
}
template< typename STRING >
inline STRING &rtrim( STRING &s ) 
{
    s.erase( std::find_if( s.rbegin(), s.rend(), std::not1( std::ptr_fun< typename isspace_t<STRING>::parameter_type, int >( isspace_t<STRING>::is ) ) ).base(), s.end() );
    return s;
}
template< typename STRING >
inline STRING ltrim( const STRING &s )
{
    STRING stmp( s );
    return ltrim( stmp );
}
template< typename STRING >
inline STRING rtrim( const STRING &s )
{
    STRING stmp( s );
    return rtrim( stmp );
}
template< typename STRING >
inline STRING &trim( STRING &s ) 
{
	return ltrim(rtrim(s));
}





							//////////////////////////////////////////
							//	  									//
							//   			Date / Time
							//              	         	  		//
							//////////////////////////////////////////



inline const wchar_t* defaultDateTimeFormat()
{
	static const wchar_t *format = L"%Y-%m-%d %X";
	return format;
}
inline std::wstring currentDateTime( const std::wstring &format = defaultDateTimeFormat() ) 
{
    time_t now = time( 0 );
    wchar_t buf[ 80 ];

#if defined (_MSC_VER)				//_CRT_SECURE_NO_WARNINGS cannot disable this, don't know why
	tm tstruct;
	localtime_s( &tstruct, &now );
#else
    tm tstruct = *localtime( &now );
#endif

	wcsftime( buf, sizeof( buf ) / sizeof(wchar_t), format.c_str(), &tstruct );
    return buf;
}



//////////////////////////////////////////
//   		Chronometers
//////////////////////////////////////////



typedef struct {
	int Hours;
	int Mins;
	int Secs;
	int mSecs;
} TIME_FIELD;


inline void FormatTimeFields( double nano100SecsTime, TIME_FIELD *pTimeFld )
{
    nano100SecsTime /= 1.0e4;

    pTimeFld->Hours = (int)((nano100SecsTime /1000) /3600);
	nano100SecsTime -= (pTimeFld->Hours * 3600.0 * 1000.0);
    pTimeFld->Mins = (int)((nano100SecsTime /1000) /60);
	nano100SecsTime -= (pTimeFld->Mins * 60.0 * 1000.0);
	pTimeFld->Secs = (int)(nano100SecsTime /1000);
	nano100SecsTime -= (pTimeFld->Secs * 1000.0);
    pTimeFld->mSecs = (int)nano100SecsTime;
}







							//////////////////////////////////////////
							//	  									//
							//   			Rand
							//              	         	  		//
							//////////////////////////////////////////


//
//	stdlib.h: #define RAND_MAX 0x7fff
//
inline 
int SeedRand()
{
	int i = (unsigned)time( NULL );
	srand(i);
	return i;
}

#if defined (__unix__)
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

//
//	- returns values in the range [bottom, top], whose limits are, respectively, 0 and RAND_MAX
//	- arguments must have values in accordance with first assert, here (*)
//
inline 
short Rand(short top = (short)RAND_MAX, short bottom = 0)
{
#ifdef _MT
#if defined (_MSC_VER)
	__declspec( thread )
#else
	__thread
#endif
	 static int dummy = 0;
	if (!dummy)
		dummy = SeedRand();
#else
	static int dummy = SeedRand();
#endif

	assert(top >= 0 && bottom >= 0 && top >= bottom);	//(*)
	short i = rand() % ((unsigned short)top + 1);		//0 <= i <= top
	if (i < bottom)
		i = std::max((short)(top - i), bottom);			//i is always <= top
	assert(i >= bottom && i <= top);
	return i;
}

#if defined (__unix__)
#pragma GCC diagnostic warning "-Wunused-variable"
#endif




////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//					REFORMULATED BRAT UTILITIES
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

#if defined (WIN32) || defined (_WIN32)
#pragma warning ( disable : 4996 )		//_CRT_SECURE_NO_WARNINGS: "This function or variable may be unsafe"
#endif


inline std::string stdFormat( size_t size, const char *format, va_list args )
{
	std::string	result;
	char *str = nullptr;
	try
	{
		str = new char[ size ];
		vsnprintf( str, size, format, args );	//_vsnprintf(str, size, format, args );		//vsprintf( str, format, args );
		result	= str;
	}
	catch ( ... )
	{
		delete[] str;
		throw;
	}
	delete[]str;
	return result;
}


inline std::string stdFormat( const char *format, va_list args )
{
	return stdFormat( 4096, format, args );
}


#if defined (WIN32) || defined (_WIN32)
#pragma warning ( default : 4996 )
#endif




#endif  //BRAT_CROSS_PLATFORM_UTILS_H
