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
//													STREAMS REDIRECTION
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#if defined(WIN32)
#include <io.h>

inline int pipe_( _Inout_updates_( 2 ) int * _PtHandles, _In_ unsigned int _PipeSize, _In_ int _TextMode )
{
	return _pipe( _PtHandles, _PipeSize, _TextMode );
}

inline bool dup2error( int result )
{
	return result != 0;
}

#else
#include <unistd.h>

inline int pipe_( int __pipedes[2], unsigned int _PipeSize = 0, int _TextMode = 0 )
{
    return pipe( __pipedes );
}

#define _dup dup
#define _dup2 dup2
#define _close close
#define _read read
#define _write write
#define _fileno fileno
#define O_BINARY 0          //whatever; not used in linux

inline bool dup2error( int result )
{
	return result == -1;
}

#endif


//TODO:
//	- This is problematic to use in production: you must know in advance how much is going to be written to
//	stdout or stderr, and reserve enough buffer, or it will hang on printf/writing (maybe waiting for a read
//	that empties the buffer, and that maybe should be done by other thread). 
//	In Linux the buffer size parameter is not used: does this mean that it supports any size...? I wonder.
//
//	- On the other hand, it also hangs if no data was written to the pipe, hence Stop writes an extra "\n"
//	before stopping and reading, which also means the buffer must accommodate these bytes too.
//
//Example:
//
//		StdFILERedirect stdoutRedirect( stdout );		//reset when variable goes out of scope
//		StdFILERedirect stderrRedirect( stderr );		//reset when variable goes out of scope
//
//		printf( "stdout redirection" );
//		fprintf( stderr, "stderr redirection" );
//
//	You can retrieve output using a buffer or a std::string:
//
//		std::string ss; 
//		int nOutRead = stdoutRedirect.Stop( ss );
//		if( nOutRead > 0 )
//		//	use string;
//		char szBuffer[ StdFILERedirect::m_out_buff_size ];
//		int nErrRead = stderrRedirect.Stop( szBuffer, StdFILERedirect::m_out_buff_size );
//		if( nErrRead > 0 )
//		//	 use buffer;
//
//(*) In some Linux example, close( m_fdStdOutPipe ) was done immediately after this
//(**) In some Linux example, read was called before dup
//
class StdFILERedirect
{
public:
	static const int READ_FD = 0;
	static const int WRITE_FD = 1;

protected:
	const int m_out_buff_size;
	int m_fdStdOutPipe[2];
	int m_fdStdFILE;
	FILE *m_stdio;
	bool m_started;

public:
    StdFILERedirect( FILE *stdio, int bufferSize = 32768, int textmode = O_BINARY ):	//one of O_BINARY, O_TEXT
		m_out_buff_size( bufferSize ),
		m_stdio( stdio ),
		m_started( false )
	{
		if ( pipe_( m_fdStdOutPipe, m_out_buff_size, textmode ) !=0 )
			throw std::string("ERROR: could not create redirection pipe.");

		m_fdStdFILE = _dup( _fileno( stdio ) );

		if ( Start() != 0 )
			throw std::string("ERROR: could not start redirector.");
	}
	~StdFILERedirect()
	{
		if ( m_started )
			Stop();
        _close( m_fdStdFILE );
        _close( m_fdStdOutPipe[WRITE_FD] );
        _close( m_fdStdOutPipe[READ_FD] );
	}

	int Start()
	{
		if ( !m_started ) 
		{
			fflush( m_stdio );
			if ( dup2error( _dup2( m_fdStdOutPipe[WRITE_FD], _fileno( m_stdio ) ) ) )	//(*)
                return -1;
			std::ios::sync_with_stdio();
			setvbuf( m_stdio, NULL, _IONBF, 0 ); // absolutely needed
			m_started = true;
		}
		return 0;
	}

	int Stop( char *buffer = NULL, int size = 0 )
	{
		static const std::string end = "\n";

		if ( m_started ) 
		{
			fprintf( m_stdio, end.c_str() );	//ensure GetBuffer does not hang because of empty buffer
			if ( dup2error( _dup2( m_fdStdFILE, _fileno( m_stdio ) ) ) )
                return -1;
			std::ios::sync_with_stdio();
			m_started = false;
		}
		if ( buffer ) 
		{
            int nOutRead = _read( m_fdStdOutPipe[READ_FD], buffer, size );
			buffer[nOutRead] = '\0';
			return nOutRead - (int)( end.length() );
		}
		return 0;
	}

	int Stop( std::string &s )
	{
		int result = Stop();
		if ( m_started )
			return result;
        char *buffer = new char[ m_out_buff_size ];
		int nOutRead = 0;
		//do {
            nOutRead = _read( m_fdStdOutPipe[READ_FD], buffer, m_out_buff_size );	//hangs if no data in m_fdStdOutPipe buffer
			if ( nOutRead > 0 ) {
				buffer[nOutRead] = '\0';
				s += buffer;
			}
		//} while( nOutRead > 0 );
		delete[] buffer;

		return (int)s.length();
	}
};




// Usage Example:
//
//        StdInFILERedirect stdinRedirect( content.c_str(), content.length() );	//reset when variable goes out of scope
//
//  where content is the input to provide via stdin to following code, unitl the redirector Stop or destructor are called.
//
//(*)  Taken from output cases, not sure if really needed
//(**) Do not risk hanging because of empty buffer; taken from output cases, not sure if really needed
//
class StdInFILERedirect
{
public:
    static const int READ_FD = 0;
    static const int WRITE_FD = 1;

protected:
    int m_fdStdInPipe[2];
    int m_fdStdFILE;
    bool m_started;

public:
    StdInFILERedirect( const char *buffer, int size, int textmode = O_BINARY ) :	//one of O_BINARY, O_TEXT
        m_started( false )
    {
        static const char end[] ={ '\n', 0 };
        static const DEFINE_ARRAY_SIZE( end );

        if ( pipe_( m_fdStdInPipe, size + end_size, textmode ) != 0 )				//1. create redirection pipe
            throw std::string( "ERROR: could not create redirection pipe." );

        m_fdStdFILE = _dup( _fileno( stdin ) );										//2. save standard IO FILE handle in class member

        if ( Start( buffer, size, end, (int)end_size ) != 0 )
            throw std::string( "ERROR: could not start redirector." );
    }
    ~StdInFILERedirect()
    {
        if ( m_started )
            Stop();
        _close( m_fdStdFILE );
        _close( m_fdStdInPipe[WRITE_FD] );
        _close( m_fdStdInPipe[READ_FD] );
    }

    int Start( const char *buffer, int size, const char *end, int end_size )
    {
        if ( !m_started )
        {
            fflush( stdin );												//(*)
            if ( dup2error( _dup2( m_fdStdInPipe[READ_FD], _fileno( stdin ) ) ) )	//3. redirect stdin read pointer to our pipe, for client use until EOF or stop called
                return -1;
            std::ios::sync_with_stdio();									//(*)
            setvbuf( stdin, NULL, _IONBF, 0 ); // absolutely needed
            m_started = true;
        }
        if ( buffer )
        {
            int nInRead = _write( m_fdStdInPipe[WRITE_FD], buffer, size );			//4. write input for clients to our pipe, using the write pointer
            nInRead += _write( m_fdStdInPipe[WRITE_FD], end, end_size );	//(**)
            return nInRead - size - end_size;
        }
        return 0;
    }

    int Stop()
    {
        if ( m_started )
        {
            fflush( stdin );												//(*)
            if ( dup2error( _dup2( m_fdStdFILE, _fileno( stdin ) ) ) )				//5. restore standard IO FILE handle previously saved in class member
                return -1;
            std::ios::sync_with_stdio();									//(*)
            m_started = false;
        }
        return 0;
    }
};






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
