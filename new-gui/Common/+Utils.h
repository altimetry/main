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
#include <string>
#include <iostream>

#include <time.h>


#if !defined (USE_STR_STREAM)
#include <sstream>
typedef std::stringstream stream_t;
#else
#include <strstream>
typedef std::strstream stream_t;
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


#undef UNUSED
#if defined (Q_UNUSED)
    #define UNUSED Q_UNUSED
#else
    #if defined(Q_CC_INTEL) && !defined(Q_OS_WIN) || defined(Q_CC_RVCT)
    template <typename T>
    inline void DummyUnused(T &x) { (void)x; }
    #  define UNUSED(x) DummyUnused(x);
    #else
    #  define UNUSED(x) (void)x;
    #endif
#endif



//////////////////////////////////////////
//	assert__ : cross-platform assert
//////////////////////////////////////////


#if defined (__unix__)		//#if defined (__STDC_VERSION__) 

#include <csignal>

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


#if defined (UNICODE) || defined (_UNICODE)
#	undef UNICODE
#	undef _UNICODE
#	define UNICODE
#	define _UNICODE
#endif









////////////////////////////////// string_traits //////////////////////////////////
//

template< typename STRING >
struct string_traits;

template<>
struct string_traits< std::string >
{
	using str_stream_type	= std::stringstream;
	using ostr_stream_type	= std::ostringstream;
	using ostream_type		= std::basic_ostream< char >;
	using istream_type		= std::basic_istream< char >;
	using streambuf_type	= std::streambuf;
	using ifstream_type		= std::ifstream;
	using ofstream_type		= std::ofstream;

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
	using str_stream_type	= std::wstringstream;
	using ostr_stream_type	= std::wostringstream;
	using ostream_type		= std::basic_ostream< wchar_t >;
	using istream_type		= std::basic_istream< wchar_t >;
	using streambuf_type	= std::wstreambuf;
	using ifstream_type		= std::wifstream;
	using ofstream_type		= std::wofstream;

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
//					DECLARE_ARRAY_SIZE
//////////////////////////////////////////////////////////////////


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))

#define DECLARE_ARRAY_SIZE_DEPRECATED(a) size_t a##_size = ARRAY_SIZE(a)

#define DECLARE_ARRAY_SIZE_(a) size_t a##_size

#define DEFINE_ARRAY_SIZE(a) DECLARE_ARRAY_SIZE_(a) = ARRAY_SIZE(a)




//////////////////////////////////////////////////////////////////
//					Find In Container
//////////////////////////////////////////////////////////////////


template< typename CONTAINER >
inline bool In( const typename CONTAINER::value_type &value, const CONTAINER &c )
{
	return std::find( c.begin(), c.end(), value ) != c.end();
}




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
inline void	DestroyPointersAndContainer( std::vector< T* > &c )
{
	while ( !c.empty() ) delete c.back(), c.pop_back();
}
template< typename K, typename T >
inline void	DestroyPointersAndContainer( std::map< K, T* > &c )			//check better this function
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

template< typename T >
std::string n2s( T n )
{
    return hn2s< std::string >( n, false );
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


// for pointers

template< typename T, typename CHAR >
inline T s2n( CHAR *s )
{
    typedef typename char_traits< CHAR >::string_type string_type;

	return s2n< T >( string_type( s ) );
}



//////////////////////////////////////////////
//
//   			Find&Replace
//
//////////////////////////////////////////////


template< typename STRING >
inline bool StartsWith( const STRING &s, const STRING &substr )
{
   size_t pos = s.find( substr, 0 );
   return pos != STRING::npos && ( s.substr( 0, substr.length() ) == substr );
}


template< typename STRING >
inline bool EndsWith( const STRING &s, const STRING &substr )
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
							//   			Non-copyable
							//              	         	  		//
							//////////////////////////////////////////

class non_copyable
{
protected:
	non_copyable() = default;
	~non_copyable() = default;

	non_copyable( non_copyable const & ) = delete;
	void operator=( non_copyable const &x ) = delete;
};



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
