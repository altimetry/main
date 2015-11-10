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
#if !defined(_File_h_)
#define _File_h_

#include <sys/stat.h>

#include "brathl_error.h"
#include "brathl.h"

#include <string>


namespace brathl
{



/** \addtogroup file File services
  @{ */

/**
  File management class.

 This class provides unbuffered, binary and ascii disk input/output services.

 While managing the file, if an error occurred, a CFileException is raised.

 \version 1.0
*/


class CFile
{

public:

  /// Empty CFile ctor
  CFile();

  /** Creates new CFile object and opens the file.
    If an error occurred, a CFileException is raised.
    \param name [in] : full name of the file;
    \param mode [in] : access mode - default value : modeRead|typeBinary (see #openFlags);
  */
  CFile(const std::string& name, uint32_t mode = modeRead|typeBinary);


  /// Destructor
  virtual ~CFile();

  /** Opens a file.
    If file object is open, it is closed.
    If an error occurred, a CFileException is raised.
    \param name [in] : full name of the file;
    \param mode [in] : access mode - default value : modeRead|typeBinary (see #openFlags);
    \return true on success, otherwise false */
  bool Open(const std::string& name, uint32_t mode = modeRead|typeBinary);


  /** Opens the current file object.
      If an error occurred, a CFileException is raised.
    \return true on success, otherwise false */
  bool Open();

  /** Tests if file is opened or not
    \return true if opened, false otherwise */
  bool IsOpen();

  /** Closes file object.
    IsOpen() and Open() are the only functions available just after this operation.
    \return true on success, otherwise false */
  bool Close();

  /** Gets the name of the file */
  const std::string& GetFileName();

  /** Gets the name of the file */
  uint32_t GetMode();

  /// Returns the current length of the file.
  long GetLength();

  /// Returns the current position of the file pointer.
  long GetPosition();

  /** Function moves file pointer by PositionOffset bytes
    relative to current position.
    \param positionOffset [in] : offset to move
    \return true on success, otherwise false */
  bool SetPosition(long positionOffset);

  void Flush();

  /** Function moves moves file pointer to the beginning of file.
    \return true on success, otherwise false */
  bool SeekToBegin();

  /** Function moves moves file pointer to the end of file.
    \return true on success, otherwise false */
  bool SeekToEnd();

  /** Function reads 'NumBytesToRead' bytes from the current file position
    and places file contents into buffer pointed by destinationBuffer
    If an error occurred, a CFileException is raised.

    \param destinationBuffer [out] : destination buffer
    \param numBytesToRead [in] : number of bytes to reads
    \return the number of bytes actually reads, zero if end of file reached */
  int32_t ReadToBuffer(char *destinationBuffer, uint32_t numBytesToRead = CFile::m_maxBufferToRead);

  /** Function reads lines from the current file
    and places contents into buffer pointed by lineRead
    If an error occurred, a CFileException is raised.
    \param lineRead [out] : line read
    \param size [in] : max number of bytes of the line
    \return the number of bytes in the lineRead parameter. -1 if end of file reached*/
private:
  int32_t ReadLine(char *lineRead, uint32_t size);

public:
  /** Same as #ReadLine, but reads only line of data and skip comments
    and places contents into buffer pointed by lineRead.
    Comments start with character '#' anywhere in the line.
    Empty line or space line are also skipped
    If an error occurred, a CFileException is raised.
    \param lineRead [out] : line data read
    \param size [in] : max number of bytes of the line
    \return the number of bytes in the lineRead parameter. -1 if end of file reached*/
  int32_t ReadLineData(char *lineRead, uint32_t size);


  /** Writes a single character to a file
    If an error occurred, a CFileException is raised.
    \param character [in] : character to write
    \return true on success, otherwise false */
  bool WriteChar(const int character);

  /** Writes a std::string to a file
    If an error occurred, a CFileException is raised.
    \param str [in] : std::string to write
    \return true on success, otherwise false */
  bool WriteString(const char* str);

  /** Writes data from memory to a file
    If an error occurred, a CFileException is raised.
    \param sourceBuffer [in] : data to write
    \param sourceBufferLength [in] : data lentgh to write
    \return the number of bytes actually written. */
  uint32_t WriteFromBuffer(const char *sourceBuffer, uint32_t sourceBufferLength);

  /** Creates a copy of current file with the newFileName.
    If file with specified filename exists, it's contents are erased.
    \param newFileName [in] : copy to file name
    \return true on success, otherwise false */
  bool Duplicate(const std::string& newFileName);


  /** Renames file object
    If file with specified filename exists, it's contents are erased.
    The current file is closed, renamed and opened as new name
    \param newName [in] : new file name
    \return true on success, otherwise false */
  bool Rename(const std::string& newName);

  /** Renames a file
    If file with specified filename exists, it's contents are erased.
    \param oldName [in] : file to rename
    \param newName [in] : new file name
    \return true on success, otherwise false */
  static bool Rename(const std::string& oldName, const std::string& newName);

  /** Closes file object and deletes (removes) the file.
    IsOpen() and Open() are the only functions available just after this operation.
    \return true on success, otherwise false */
  bool Delete();

  /** Deletes (removes) a file.
    \param filename [in] : file to delete/remove
    IsOpen() and Open() are the only functions available just after this operation.
    \return true on success, otherwise false */
  static bool Delete(const std::string& filename);

  /** Change buffering mode.
     Function must be used before any read/write operation occurs!
    \param mode [in] : true if buffered I/O (default), false if unbuffered I/O
    \return true on success, otherwise false */
  bool SetBufferingMode(bool mode = true);

  /** Gets information about the file.
    \param fileStatus [in] : structure to store results
    \return true on success, otherwise false */
  bool GetStatus(struct stat& fileStatus);

  /** Gets information about a file.
    \param filename [in] : file toget the status
    \param fileStatus [in] : structure to store results
    \return true on success, otherwise false */
  static bool GetStatus(const std::string& filename, struct stat& fileStatus);


  bool Write(const int character);
  bool Write(const std::string& str);
  bool Write(const char *str);

  /// Gets the las error message encountered
  //const char* const GetLastError() {return m_lastError;};

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);


private:

      /// Close the file and delete pointers
  void Destroy();


public:

  /** File access mode enumeration:
    Flags can be combined by using the bitwise-OR (|) operator
  */
  enum openFlags{
		modeRead       = 0x0001,       ///< Opens for reading. If the file does not exist or cannot be found, open fails
		modeWrite      = 0x0002,       ///< Opens an empty file for writing. If the given file exists, its contents are destroyed
		modeAppend     = 0x0004,       ///< Opens for writing at the end of the file (appending) without removing the EOF marker before writing new data to the file; creates the file first if it doesn't exist.
		modeReadWrite  = 0x0008,       ///< Opens for both reading and writing. (The file must exist.)
		modeRWCreate   = 0x0010,       ///< Opens an empty file for both reading and writing. If the given file exists, its contents are destroyed.
		modeReadAppend = 0x0020,       ///< Opens for reading and appending; the appending operation includes the removal of the EOF marker before new data is written to the file and the EOF marker is restored after writing is complete; creates the file first if it doesn't exist.
		typeText       = 0x4000,        ///< Open in text (translated) mode
		typeBinary     = static_cast<int32_t>(0x8000) ///< Open in binary (untranslated) mode
	      };

protected:

  /// last error message
 char m_lastError[BRATHL_MAX_ERRMSG_LEN+1];


private:
  /// File handle
  FILE *m_hFile;
  /// Full file name
  std::string m_fileName;
  /// Length of file, in bytes
  uint32_t m_length;
  /// File access mode - See #openFlags
  uint32_t m_mode;


  /// Maximum size of buffer for data reading
  static const uint32_t m_maxBufferToRead;

  /// buffer for data reading
  char * m_buffer;



};

/** @} */

}

#endif // !defined(_File_h_)
