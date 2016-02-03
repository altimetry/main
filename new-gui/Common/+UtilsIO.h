#ifndef BRAT_CROSS_PLATFORM_UTILS_IO_H
#define BRAT_CROSS_PLATFORM_UTILS_IO_H

#if defined (CROSS_PLATFORM_UTILS_IO_H)
#error Wrong +UtilsIO.h included 
#endif

#ifndef __cplusplus
#error Must use C++ for +UtilsIO.h
#endif

#include <fcntl.h>
#include <fstream>

#include "+Utils.h"

#if defined(WIN32)
#include <codecvt>
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													Generic
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline std::string setExecExtension( const std::string &execPath )
{
    static const std::string &ext =

#ifdef WIN32
        ".exe"
#else
        ""
#endif
        ;

    return execPath + ext;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													READ / WRITE to std strings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//inline bool ReadFromUnicodeFile( std::wstring &s, const std::wstring &path )
//{
//	std::wifstream f( path, std::ios::binary );
//	f.imbue( std::locale( f.getloc(), new std::codecvt_utf16< wchar_t, 0x10ffff, std::little_endian>) );
//	if ( !f )
//		return false;
//	std::copy( std::istreambuf_iterator< wchar_t >( f ), std::istreambuf_iterator< wchar_t >(), std::back_inserter( s ) );
//	return true;
//}
inline void PrepareRead2String( string_traits<std::string>::ifstream_type  &inf )
{
	UNUSED( inf );
}
inline void PrepareRead2String( string_traits<std::wstring>::ifstream_type  &inf )
{
#if defined(WIN32)
	inf.imbue( std::locale( inf.getloc(), new std::codecvt_utf16< wchar_t, 0x10ffff, std::little_endian>) );
#else
	UNUSED( inf );
#endif
}

template< typename STRING >
inline bool Read2String( STRING &s, const STRING &path )
{
    typename string_traits<STRING>::ifstream_type inf( path, std::ios::binary );
	PrepareRead2String( inf );
	if ( !inf )
		return false;
	std::copy( std::istreambuf_iterator< typename STRING::value_type >( inf ), std::istreambuf_iterator< typename STRING::value_type >(), std::back_inserter( s ) );
	return true;
}

template< typename STRING >
inline bool Write2File( const STRING &s, const STRING &path )
{
    typename string_traits<STRING>::ofstream_type outf( path, std::ios::binary | std::ios::out );
	if ( !outf )
		return false;

	return !!outf.write( s.c_str(), s.size() );
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													PORTABLE PATHS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



inline const std::string portable_data_path_prefix()
{
	static const std::string s = ".../";
	return s;
}


inline bool isPortableDataPath( const std::string &path )
{
	return startsWith( path, portable_data_path_prefix() );
}


inline std::string normalizedPath( const std::string &path )
{
	std::string normalized = path;
	replace( normalized, "//", "/" );
	replace( normalized, "\\\\", "/" );
	replace( normalized, "\\", "/" );
	return normalized;
}


inline std::string Absolute2PortableDataPath( const std::string &path, const std::string &standard_path )
{
	const std::string data_path = normalizedPath( standard_path );
	const std::string new_path = normalizedPath( path );

    if ( startsWith( new_path, data_path ) )
		return portable_data_path_prefix() + new_path.substr( data_path.length() ); 

	return new_path;
}


// (*) do NOT normalize. 2 reasons: 1) it is already normalized by setPath (if inside standard data 
//	path, and if not, is the user responsibility) and 2) can eliminate the 1st char of "prefix" ('/'),
//	which then will not be found.
//
inline std::string PortableData2AbsolutePath( const std::string &path, const std::string &standard_path )
{
	const std::string data_path = normalizedPath( standard_path );
	const std::string new_path = path;								//(*)

    if ( isPortableDataPath( new_path ) )
		return data_path + new_path.substr( portable_data_path_prefix().length() ); 

    return new_path;
}



#endif  //BRAT_CROSS_PLATFORM_UTILS_IO_H
