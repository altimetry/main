
/*
* 
*
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


#include <sys/stat.h>
#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "brathl_error.h"
#include "brathl.h"

#include "Stl.h"

#include "TraceLog.h"
#include "Tools.h"
#include "Exception.h"
#include "File.h"
using namespace brathl;

namespace brathl
{


const uint32_t CFile::m_maxBufferToRead = 4096;
//const uint32_t CFile::m_maxBufferToRead = 10;


CFile::CFile()
{
  m_hFile = NULL;
  m_fileName = "";
  m_mode = modeRead|typeBinary;
  m_length = 0;
  m_buffer = NULL;

}

//----------------------------------------

CFile::CFile(const string& name, uint32_t mode /* = modeRead|typeBinary */)
{
  m_hFile = NULL;
  m_fileName = name;
  m_mode = mode;
  m_length = 0;
  m_buffer = NULL;

  Open();

}

//----------------------------------------

CFile::~CFile()
{
  Destroy();

}

//----------------------------------------
void CFile::Destroy()
{
  Close();

  m_mode = modeRead|typeBinary;
  m_length = 0;
}


//----------------------------------------
bool CFile::Open()
{

  Close();

  char szMode[10];
  memset(szMode, '\0', 10);

  if ((m_mode & modeRead) == modeRead)
  {
    strcpy(szMode, "r");
  }
  else if ((m_mode & modeWrite) == modeWrite)
  {
    strcpy(szMode, "w");
  }
  else if ((m_mode & modeAppend) == modeAppend)
  {
    strcpy(szMode, "a");
  }
  else if ((m_mode & modeReadWrite) == modeReadWrite)
  {
    strcpy(szMode, "r+");
  }
  else if ((m_mode & modeRWCreate) == modeRWCreate)
  {
    strcpy(szMode, "w+");
  }
  else if ((m_mode & modeReadAppend) == modeReadAppend)
  {
    strcpy(szMode, "a+");
  }
  else
  {
    strcpy(szMode, "r");
  }


  if ((m_mode & typeText) == typeText)
  {
    strcat(szMode, "t");
  }
  else
  {
    strcat(szMode, "b");
  }

  if(m_fileName.empty())
  {
    CFileException e("Error opening file -  file name is empty", BRATHL_IO_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);
  }

  m_hFile = fopen(m_fileName.c_str(), szMode);

  if(m_hFile == NULL)
  {
    string msg = CTools::Format("Error while opening file - errno:#%d:%s", errno, strerror(errno));
    CFileException e(msg, m_fileName, BRATHL_IO_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);
  }

  // Make sure this is set before any I/O or fseek is performed
  SetBufferingMode(1);

  SeekToEnd();
  m_length = ftell(m_hFile);
  SeekToBegin();

  return true;
}

//----------------------------------------
bool CFile::Open(const string& name, uint32_t mode /* = modeRead|typeBinary */)
{

  Destroy();

  m_fileName = name;

  m_mode = mode;

  return Open();
}

//----------------------------------------


bool CFile::IsOpen()
{
  return (m_hFile != NULL);
}
//----------------------------------------

const string&  CFile::GetFileName()
{
  return m_fileName;
}

//----------------------------------------

uint32_t CFile::GetMode()
{
  return m_mode;
}

//----------------------------------------

long CFile::GetLength()
{
  return ((IsOpen() == true) ? m_length : 0);
}

//----------------------------------------
long CFile::GetPosition()
{
  return ((IsOpen() == true) ? ftell(m_hFile) : 0);
}
//----------------------------------------
void CFile::Flush()
{

  if(IsOpen() == false)
  {
    return;
  }

  if ((m_mode & modeRead) == modeRead)
  {
    return;
  }

  fflush(m_hFile);
}
//----------------------------------------
bool CFile::SetPosition(long positionOffset)
{
  bool bOk = false;

  if(IsOpen() == false)
  {
    return bOk;
  }

  if( fseek(m_hFile, positionOffset, SEEK_CUR) == 0 )
  {
    bOk = true;
  }

  return bOk;
}

//----------------------------------------
bool CFile::SeekToBegin()
{
  bool bOk = false;

  if(IsOpen() == false)
  {
    return bOk;
  }

  if( fseek(m_hFile, 0, SEEK_SET) == 0 )
  {
    bOk = true;
  }

  return bOk;
}

//----------------------------------------
bool CFile::SeekToEnd()
{
  bool bOk = false;

  if(IsOpen() == false)
  {
    return bOk;
  }

  if( fseek(m_hFile, 0, SEEK_END) == 0 )
  {
    bOk = true;
  }

  return bOk;
}


//----------------------------------------
int32_t CFile::ReadLineData(char *lineRead, uint32_t size )
{
  char *current = NULL;
  int32_t result = -1;

  do
  {
    result = ReadLine(lineRead, size);

    if (result < 0)
    {
      break;    // end-of-file reached
    }

    // replace tab and newline characters by spaces
    for (current = lineRead; *current ; current++)
    {
      if (isspace(*current) || (*current == '\n'))
      {
	*current = ' ';
      }
    }
    // suppress leading and trailing blank characters
    CTools::Trim(lineRead);

    if ((CTools::IsEmpty(lineRead) || *lineRead == '#'))
    {
      result = 0;
    }

  } while (result == 0);

  return result;
}


//----------------------------------------
int32_t CFile::ReadLine( char *lineRead, uint32_t size )
{
	//char line[CFile::m_maxBufferToRead];
	char * line = NULL;
	char * buff = new char[ CFile::m_maxBufferToRead ];
	char *current = NULL;
	uint32_t bytesRead = 0;
	char * last = NULL;
	uint32_t lenToCopy = 0;

	// if start of file (first read) then delete and reallocate m_buffer member

	do
	{

		if ( m_buffer != NULL )
		{
			memset( buff, '\0', CFile::m_maxBufferToRead );
			strcpy( buff, m_buffer );
			delete[]m_buffer;
			m_buffer = NULL;

			bytesRead = strlen( buff );

			//printf("bytesRead from m_buffer %d buff %s\n", bytesRead, buff);
		}
		else
		{
			memset( buff, '\0', CFile::m_maxBufferToRead );
			bytesRead = ReadToBuffer( buff, CFile::m_maxBufferToRead - 1 );
			//printf("bytesRead %d buff %s\n", bytesRead, buff);

		}

		if ( bytesRead == 0 )
		{
			//---------------------------------
			break;  // exit do - End of file
			//---------------------------------
		}

		last = ( buff + strlen( buff ) - 1 );

		current = strpbrk( ( buff ), "\r\n" );

		if ( current == NULL )
		{
			// Get buffer and read next
			char * linePrev = line;
			if ( linePrev != NULL )
			{
				line = new char[ strlen( linePrev ) + strlen( buff ) + 1 ];
				strcpy( line, linePrev );
				strcat( line, buff );

				delete[]linePrev;
				linePrev = NULL;
			}
			else
			{
				line = new char[ strlen( buff ) + 1 ];
				strcpy( line, buff );
			}

		}
		else  //(current != NULL)
		{
			if ( *current == '\r' )
			{
				*current = '\0';
				// if current is the last character :
				// - read next character
				// - if next is not '\n', skip it
				if ( current == last )
				{
					char c1[ 1 ];
					int32_t nRead = ReadToBuffer( c1, 1 );

					//printf(" 2nd - bytesRead \n");

					if ( nRead > 0 )
					{
						if ( *c1 != '\n' )
						{
							SetPosition( -( nRead ) ); // rewind the nRead bytes read
						}
					}
				}
				else if ( *( current + 1 ) == '\n' ) // (current != last), if the next character of current is '\n', skip it
				{
					current++;
					*current = '\0';
				}
			}
			else //(*current != '\r') ==> (*current == '\n')
			{
				*current = '\0';
			}

			//Get buffer  from start to character before current.
			lenToCopy = uint32_t( current - buff + 1 );

			char * linePrev = line;

			if ( linePrev != NULL )
			{
				line = new char[ strlen( linePrev ) + lenToCopy ];
				strcpy( line, linePrev );
				strcat( line, buff );

				delete[]linePrev;
				linePrev = NULL;
			}
			else
			{
				line = new char[ lenToCopy ];
				strcpy( line, buff );
			}

			if ( current != NULL )
			{
				if ( current != last )
				{
					current++;
				}
				else
				{
					current = NULL;
				}

			}

			if ( current != NULL )
			{
				if ( m_buffer == NULL )
				{
					m_buffer = new char[ CFile::m_maxBufferToRead ];
				}
				lenToCopy = ( strlen( current ) > m_maxBufferToRead ? m_maxBufferToRead - 1 : strlen( current ) );
				memset( m_buffer, '\0', CFile::m_maxBufferToRead );
				strncpy( m_buffer, current, lenToCopy );

			}


			//---------------------------------
			break; //a complete line is read
			//---------------------------------

		} //else (current != NULL)

	} while ( bytesRead > 0 );


	memset( lineRead, '\0', size );

	if ( line == NULL )
	{
		delete[]buff;
		buff = NULL;
		return -1;
	}

	//printf("Line read Start:%s:End\n", line);

	lenToCopy = strlen( line );


	if ( size <= lenToCopy )
	{
		cout << "CFile::ReadLine - line size " << lenToCopy << " - buffer size too small - line truncated";
	}

	size = strlen( line ) + 1;
	strncpy( lineRead, line, lenToCopy );

	delete[]buff;
	buff = NULL;
	delete[]line;
	line = NULL;
	//  int32_t result = (int32_t(bytesRead) == 0 ?  -1 : int32_t(size));
	//return result;

	return size;

}

//----------------------------------------
int32_t CFile::ReadToBuffer(char *destinationBuffer, uint32_t numBytesToRead /* = CFile::m_maxBufferToRead */)
{
  if(IsOpen() == false)
  {
    return 0;
  }

  long diff = GetLength() - GetPosition();

  if ( static_cast<unsigned long>(diff) < numBytesToRead)
  {
    numBytesToRead = diff;
  }

  if (numBytesToRead == 0)
  {
    return 0;
  }


  long bytesRead = fread(destinationBuffer, sizeof(char), numBytesToRead, m_hFile);

  if ( ferror(m_hFile) )
  {
    string msg = CTools::Format("Error while reading file - errno:#%d:%s", errno, strerror(errno));
    CFileException e(msg, m_fileName, BRATHL_IO_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);
  }


  return bytesRead;
}

//----------------------------------------
bool CFile::WriteChar(const int character)
{
  if(IsOpen() == false)
  {
    return false;
  }

  if ((m_mode & modeRead) == modeRead)
  {
    return false;
  }


  if ( fputc(character, m_hFile) == -1)
  {
    string msg = CTools::Format("Error while writing file - errno:#%d:%s", errno, strerror(errno));
    CFileException e(msg, m_fileName, BRATHL_IO_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);
  }


  Flush();

  m_length++;

  return true;
}

//----------------------------------------
bool CFile::Write(const int character)
{
  return WriteChar(character);
}
//----------------------------------------
bool CFile::Write(const string& str)
{
  return WriteString(str.c_str());
}
//----------------------------------------
bool CFile::Write(const char *str)
{
  return WriteString(str);
}
//----------------------------------------
bool CFile::WriteString(const char *str)
{
  if(IsOpen() == false)
  {
    return false;
  }

  if ((m_mode & modeRead) == modeRead)
  {
    return false;
  }

  if (fputs(str, m_hFile) == -1 )
  {
    string msg = CTools::Format("Error while writing file - errno:#%d:%s", errno, strerror(errno));
    CFileException e(msg, m_fileName, BRATHL_IO_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);
  }

  Flush();

  m_length += strlen(str);

  return true;
}

//----------------------------------------
uint32_t CFile::WriteFromBuffer(const char *sourceBuffer, uint32_t sourceBufferLength)
{
  if(IsOpen() == false)
  {
    return 0;
  }

  if ((m_mode & modeRead) == modeRead)
  {
    return 0;
  }

  uint32_t written = fwrite(sourceBuffer, sizeof(char), sourceBufferLength, m_hFile);

  if ( ferror(m_hFile) )
  {
    string msg = CTools::Format("Error while writing file - errno:#%d:%s", errno, strerror(errno));
    CFileException e(msg, m_fileName, BRATHL_IO_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);
  }


  Flush();

  m_length += written;

  return written;

}

//----------------------------------------

bool CFile::Duplicate(const string& newFileName)
{
  if (IsOpen() == false)
  {
    return false;
  }

  if (m_fileName.compare(newFileName) == 0)
  {
    return false;
  }

  Flush();

  long curPos = GetPosition();

  SeekToBegin();

  long  dupLength = GetLength();

  CFile dupFile(newFileName, modeWrite);

  if( dupFile.IsOpen() == false)
  {
    return false;
  }

  char *buf = new char[dupLength];

  if (buf == NULL)
  {
    return false;
  }

  uint32_t written = 0;

  if (ReadToBuffer(buf, dupLength) != 0)
  {
    written = dupFile.WriteFromBuffer(buf, dupLength);
    dupFile.Destroy();
  }

  delete []buf;
  buf = NULL;

  SeekToBegin();
  SetPosition(curPos);

  return (written != 0);
}
//----------------------------------------

bool CFile::Rename(const string& newFileName)
{
  if (IsOpen() == false)
  {
    return false;
  }

  if (m_fileName.compare(newFileName) == 0)
  {
    return false;
  }

  Flush();

  Close();

  ::rename(m_fileName.c_str(), newFileName.c_str());

  if (Open(newFileName, m_mode) == false)
  {
    return false;
  }

  return true;
}
//----------------------------------------

 bool CFile::Rename(const string& oldName, const string& newName)
 {
   return (::rename(oldName.c_str(), newName.c_str()) == 0);
 }


//----------------------------------------

bool CFile::Delete()
{
  if (IsOpen() == false)
  {
    return false;
  }


  Close();


  if (::remove(m_fileName.c_str()) != 0)
  {
    // file not removed - re-open it
    Open();
    return false;
  }

  Destroy();
  return true;
}


bool CFile::Delete(const string& fileName)
{
  return (::remove(fileName.c_str()) == 0);
}


//----------------------------------------
bool CFile::Close()
{
  if(IsOpen() == false)
  {
    return true;
  }

  Flush();

  if (m_buffer != NULL)
  {
    delete[] m_buffer;
    m_buffer = NULL;
  }

  fclose(m_hFile);
  m_hFile = NULL;

  return true;
}

//----------------------------------------
bool CFile::SetBufferingMode(bool mode /*  = true */)
{
  int32_t result = false;

  if(IsOpen() == false)
  {
    return false;
  }

  if(mode == false)
  {
    result = setvbuf(m_hFile, NULL, _IONBF, 0);
  }
  else
  {
    result = setvbuf(m_hFile, NULL, _IOFBF, CFile::m_maxBufferToRead);
  }

  return (result == 0);
}

//----------------------------------------

bool CFile::GetStatus(struct stat& fileStatus)
{
  bool bOk = (fstat(fileno(m_hFile), &fileStatus) != 0);

  return bOk;

}

//----------------------------------------
bool CFile::GetStatus(const string& fileName, struct stat& fileStatus)
{
  CFile file;
  bool bOk = file.Open(fileName, modeRead);

  if (bOk == false)
  {
    bOk = (fstat(fileno(file.m_hFile), &fileStatus) != 0);
  }

  return bOk;

}

//----------------------------------------
void CFile::Dump(ostream& fOut /* = cerr */)
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   fOut << "==> Dump a CFile Object at "<< this << endl;

   fOut << "m_hFile = " << m_hFile << endl;
   fOut << "m_fileName = " << m_fileName  << endl;
   fOut << "m_length = " << m_length << endl;
   fOut << "m_mode = " << m_mode << endl;
   fOut << "m_maxBufferToRead = " << m_maxBufferToRead << endl;
   fOut << "m_buffer = " << ((m_buffer != NULL) ? m_buffer : "NULL") << endl;

   fOut << "==> END Dump a CFile Object at "<< this << endl;

   fOut << endl;


}

}
