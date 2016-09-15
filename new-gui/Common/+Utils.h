/*
* This file is part of BRAT 
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
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
#include <limits>

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




////////////////////////////////// c++11 compatibility ////////////////////////////
//


#if !defined(WIN32) && !defined(_WIN32)

    #if __cplusplus <= 199711L

        #if !defined(PRE_CPP11)
        #define PRE_CPP11
        #endif

    #endif
#endif




#if defined(PRE_CPP11)

#if !defined(override)
#define override
#endif

#if !defined(nullptr)
#define nullptr NULL
#endif

#endif


////////////////////////////////// string_traits //////////////////////////////////
//

template< typename STRING >
struct string_traits;

template<>
struct string_traits< std::string >
{
#if defined(PRE_CPP11)

    typedef std::stringstream           str_stream_type;
    typedef std::ostringstream          ostr_stream_type;
    typedef std::basic_ostream< char >  ostream_type;
    typedef std::basic_istream< char >  istream_type;
    typedef std::streambuf              streambuf_type;
    typedef std::ifstream               ifstream_type;
    typedef std::ofstream               ofstream_type;
#else
    using str_stream_type	= std::stringstream;
	using ostr_stream_type	= std::ostringstream;
	using ostream_type		= std::basic_ostream< char >;
	using istream_type		= std::basic_istream< char >;
	using streambuf_type	= std::streambuf;
	using ifstream_type		= std::ifstream;
	using ofstream_type		= std::ofstream;
#endif

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
#if defined(PRE_CPP11)

    typedef std::wstringstream              str_stream_type;
    typedef std::wostringstream             ostr_stream_type;
    typedef std::basic_ostream< wchar_t >   ostream_type;
    typedef std::basic_istream< wchar_t >   istream_type;
    typedef std::wstreambuf                 streambuf_type;
    typedef std::wifstream                  ifstream_type;
    typedef std::wofstream                  ofstream_type;
#else
    using str_stream_type	= std::wstringstream;
	using ostr_stream_type	= std::wostringstream;
	using ostream_type		= std::basic_ostream< wchar_t >;
	using istream_type		= std::basic_istream< wchar_t >;
	using streambuf_type	= std::wstreambuf;
	using ifstream_type		= std::wifstream;
	using ofstream_type		= std::wofstream;
#endif

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





#if defined(PRE_CPP11)

static inline bool str_icmp_pred( char a, char b )
{
    return std::tolower(a) == std::tolower(b);
}

static inline bool str_cmp_pred( char a, char b )
{
    return a == b;
}

#endif



template< class STRING >
bool str_icmp( STRING const& a, STRING const& b )
{
#if defined(PRE_CPP11)

    typedef bool (*predicate_t)(typename STRING::value_type, typename STRING::value_type);
    predicate_t pred = str_icmp_pred;
#else
    static const auto pred = [](typename STRING::value_type a, typename STRING::value_type b)
	{
		return std::tolower(a) == std::tolower(b);
	};
#endif

	return compare_( a, b, pred );
}


template< class STRING >
bool str_cmp( STRING const& a, STRING const& b )
{
#if defined(PRE_CPP11)

    typedef bool (*predicate_t)(typename STRING::value_type, typename STRING::value_type);
    predicate_t pred = str_cmp_pred;
#else
    static const auto pred = [](typename STRING::value_type a, typename STRING::value_type b)
	{
		return a == b;
	};
#endif

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




#if !defined(PRE_CPP11)

//////////////////////////////////////////////////////////////////
//				Find minimum/maximum in numeric arrays
//////////////////////////////////////////////////////////////////


template< typename T >
inline T array_min( std::vector< T > const &values )
{
    T m = std::numeric_limits< T >::max();

    std::for_each( values.begin(), values.end(), [&m]( const T &v ) { if ( v < m ) m = v; } );

    return m;
}


template< typename T >
inline T array_max( std::vector< T > const &values )
{
    T M = std::numeric_limits< T >::lowest();

    std::for_each( values.begin(), values.end(), [&M]( const T &v ) { if ( v > M ) M = v; } );

    return M;
}


#endif



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



#if defined(PRE_CPP11)
static inline bool i_find_predicate( char ch1, char ch2 )
{
    return std::toupper( ch1 ) == std::toupper( ch2 );
}
#endif


template< typename STRING >
inline 
typename STRING::size_type i_find( const STRING &source, const STRING &find, size_t pos = 0 )
{
#if defined(PRE_CPP11)

    auto it = std::search( source.begin() + pos, source.end(), find.begin(), find.end(), i_find_predicate );
#else
    auto it = std::search(
        source.begin() + pos, source.end(), find.begin(), find.end(),
        []( typename STRING::value_type ch1, typename STRING::value_type ch2 )
        {
            return std::toupper( ch1 ) == std::toupper( ch2 );
        }
    );
#endif


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
