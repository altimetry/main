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
#if !defined(_FileParams_h_)
#define _FileParams_h_

#include <string>

#include "common/tools/brathl_error.h"
#include "brathl.h"
       
#include "MapParameter.h" 
#include "File.h" 
#include "FileParams.h" 
using namespace brathl;
 

namespace brathl
{

  
/** \addtogroup parameters Parameters
  @{ */

/** 
  Parameters file management class.

  This class provides ascii parameters file services

  It makes it possible to acquire the whole of information which they contain 

  Parameters are described as 'keyword'='value'
  
  keyword is character strings identifying a type of data.
  value is character strings associated with the key. 

  keyword and value have to be on the same line;
  
  It don't make distinction between upper-case and lower-case letters.

  While managing the file, if an error occurred, a CFileException is raised.
  While managing parameter (keyword, value), if an error occurred, a CParameterException is raised.


 \version 1.0
*/

class CFileParams : public CFile
{

public:
    
  /// Empty CFileParams ctor
  CFileParams();
  
  /** Creates new CFileParams object and opens the parameters file.
    On error, a CFileException or CParameterException exception is raised.
    \param name [in] : full name of the file;
    \param mode [in] : access mode - default value : modeRead|typeBinary (see #openFlags);
  */
  CFileParams(const std::string& name, uint32_t mode = modeRead|typeBinary);

  
  /// Destructor
  virtual ~CFileParams();
  
  /** Reads file parameters and load parameters 
    On error, a CFileException or CParameterException exception is raised.
  */
  void Load();
  
  void LoadAliases();

  void SubstituteAliases(const CStringMap& aliases);

  void LoadFieldSpecificUnits();

  CStringMap* GetFieldSpecificUnits() { return &m_fieldSpecificUnit; };


   //@{
   /** Reads file parameters and load parameters 
    On error, a CFileException or CParameterException exception is raised.
    \param name [in] : full name of the file;
    \param mode [in] : access mode - default value : modeRead|typeBinary (see #openFlags);
    */
  void Load(const std::string& name, uint32_t mode = modeRead|typeBinary);
  //@}
    
  /** Throw an exception if the number of values is
      not valid.
    \param ValidMin [in] : Minimal number of values
    \param ValidMax [in] : Maximal number of values. If <=0, it is considered
	as infinite. If < ValidMin, it is considered as equal to ValidMin.
    \return actual number of occurences of the parameter
  */
  unsigned CheckCount
		(const std::string	&Key,
		 int32_t	ValidMin	= 1,
		 int32_t	ValidMax	= 1);
    
  void GetFieldNames(const std::string& key, CStringArray& fieldNames);
  void GetFileList(const std::string& key, CStringArray& fileNames);
  std::string GetFirstFile(const std::string& key);

  bool IsLoaded() { return m_isLoaded; };



  /** Set the verbosity level from the standard keyword VERBOSE
  */
  void SetVerboseLevel();


  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

private:

  /** Extract keyword and value from a line  
    On error, CParameterException exception is raised.
    \param line [in] : data to extract (musn't be a valid data line - not a comment, empty line);
    \return true on success, otherwise false */
  void Decode(char *line);
    
  void Init();


public:
    
  /** A map containing all the parameters */
   CMapParameter m_mapParam; 
        

private:

  /** A map containing the aliases values */
   CStringMap m_aliases;

   /** A map containing the specific necdf attributes units */
   CStringMap m_fieldSpecificUnit;

   bool m_isLoaded;


};

/** @} */

}

#endif // !defined(_FileParams_h_)
