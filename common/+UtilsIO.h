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


inline bool IsFile( const char *name )
{
    if (FILE *file = fopen(name, "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

inline bool IsFile( const std::string &name )
{
	return IsFile( name.c_str() );
}




struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
#if defined (WIN32) || defined (_WIN32)
        return ch == '\\' || ch == '/';
#else
        return ch == '/';
#endif
    }
};

inline std::string
remove_extension( std::string const& filename )
{
    std::string::const_reverse_iterator pivot =
            std::find( filename.rbegin(), filename.rend(), '.' );
    return pivot == filename.rend()
        ? filename
        : std::string( filename.begin(), pivot.base() - 1 );
}

inline std::string
extension_from_path( std::string const& filename )
{
    std::string::const_reverse_iterator pivot =
            std::find( filename.rbegin(), filename.rend(), '.' );

    return pivot == filename.rend()
        ? ""
        : std::string( pivot.base() - 1, filename.end() );
}

inline std::string
filename_from_path( std::string const& path )
{
    return std::string(
        std::find_if( path.rbegin(), path.rend(),
                      MatchPathSeparator() ).base(),
        path.end() );
}

inline std::string
basename_from_path( std::string const& path )
{
    return remove_extension( filename_from_path( path ) );
}

inline std::string
dir_from_filepath( std::string const& path )
{
    std::string filename = filename_from_path( path );

    return path.substr( 0, path.find( filename ) );
}


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


inline bool IsPortableDataPath( const std::string &path )
{
    return StartsWith( path, portable_data_path_prefix() );
}


inline std::string normalizedPath( const std::string &path )
{
	std::string normalized = path;
	normalized = replace( normalized, "\\\\", "/" );
	normalized = replace( normalized, "\\", "/" );
	normalized = replace( normalized, "//", "/" );
	return normalized;
}


inline std::string Absolute2PortableDataPath( const std::string &path, const std::string &standard_path )
{
	const std::string data_path = normalizedPath( standard_path );
	const std::string new_path = normalizedPath( path );

    if ( StartsWith( new_path, data_path ) )
		return portable_data_path_prefix() + new_path.substr( data_path.length() ); 

	return new_path;
}


// (*) do NOT normalize. 2 reasons: 1) it is already normalized by setPath (if inside standard data 
//	path, and if not, is the user responsibility) and 2) can eliminate the 1st char of "prefix" ('/'),
//	which then will not be found.
//
inline std::string Portable2AbsoluteDataPath( const std::string &path, const std::string &standard_path )
{
	const std::string data_path = normalizedPath( standard_path );
	const std::string new_path = path;								//(*)

    if ( IsPortableDataPath( new_path ) )
		return data_path + new_path.substr( portable_data_path_prefix().length() ); 

    return new_path;
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													BASIC LOG FILE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class CLogFile : non_copyable
{
protected:
	std::fstream mFile;

public:
	CLogFile( bool append, const std::string &path ) :
		mFile( path, std::ios::binary | std::ios::out | ( append ? std::ios::app : std::ios::trunc ) )
	{}

	virtual ~CLogFile()
	{
		mFile.close();		// just in case
	}


	bool IsOk() const { return !!mFile; }

	
	///////////////////////////////////////////////////

	void Log( const std::string& text = "" )
	{
		assert__( !!mFile );

		mFile << text;
	}

	void LogLn( const std::string& text = "" )
	{
		Log( text + "\n" );
	}
};





#endif  //BRAT_CROSS_PLATFORM_UTILS_IO_H
