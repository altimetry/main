
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

#include <cstdio>
#include <cstring>
#include <string>

#include "Tools.h"
#include "common/tools/TraceLog.h"
#include "brathl.h"
#include "common/tools/brathl_error.h"
#include "common/tools/Exception.h"

#include "FileParams.h"

const std::string kwALIAS_NAME			= "ALIAS_NAME";
const std::string kwALIAS_VALUE			= "ALIAS_VALUE";
const std::string kwUNIT_ATTR_NAME = "UNIT_ATTR_NAME";
const std::string kwUNIT_ATTR_VALUE = "UNIT_ATTR_VALUE";
const std::string kwVERBOSE				= "VERBOSE";
const std::string kwLOGFILE				= "LOGFILE";

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;

namespace brathl
{

//----------------------------------------

CFileParams::CFileParams()
{
  Init();
}

//----------------------------------------

CFileParams::CFileParams(const std::string& name, uint32_t mode /* = modeRead|typeBinary */)
      : CFile(name, mode)
{
  Init();
  Load();
}

//----------------------------------------

CFileParams::~CFileParams()
{

}

//----------------------------------------

void CFileParams::Init()
{
  m_isLoaded = false;
}
//----------------------------------------

void CFileParams::Load()
{
	CTrace::Tracer( "Entered CFileParams::Load(name, mode)\n" );

	char *line = nullptr;

	try {
		if ( !IsOpen() )
		{
			Open();
		}

		m_mapParam.RemoveAll();

		const size_t size = GetLength() + 1;
		line = new char[ size ];

		while ( ReadLineData( line, size ) > 0 )
		{
			CTrace::Tracer( "ReadLineData Start:%s:End", line );
			Decode( line );
		}

		LoadAliases();

		LoadFieldSpecificUnits();


		Close();

		m_isLoaded = true;
	}
	catch ( ... )
	{
		std::cout << "Undefined exception caught in CFileParams::Load." << std::endl;
	}

	delete[] line;
}


//----------------------------------------
void CFileParams::Load(const std::string& name, uint32_t mode /* = modeRead|typeBinary */)
{
  CTrace::Tracer("Entered CFileParams::Load(name, mode)\n");
  Open(name, mode);
  Load();
}
//----------------------------------------
void CFileParams::LoadAliases()
{
  m_aliases.RemoveAll();

  uint32_t nbAliases = CheckCount(kwALIAS_NAME, 0, -1);

  const std::string ValidChars	= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789";

  if (nbAliases != 0)
  {
    CheckCount(kwALIAS_VALUE, nbAliases, -1);

    for (uint32_t Index=0; Index < nbAliases; Index++)
    {
      std::string		Name, Value;
      std::string::size_type	Where;
      
      m_mapParam[kwALIAS_NAME]->GetValue(Name, Index, "-NO-DEFAULT-VALUE-FOR-ALIAS-NAME-");

      m_mapParam[kwALIAS_VALUE]->GetValue(Value, Index);

      Where	= ValidChars.find(Name[0]);
      
      if ((Where == std::string::npos) || (Where >= 26*2))
      {
        throw CParameterException("Alias name must begin with a letter: '"+Name+"' is invalid",
				  BRATHL_SYNTAX_ERROR);
      }

      Where	= Name.find_first_not_of(ValidChars);

      if (Where != std::string::npos)
      {
        throw CParameterException("Alias name must contain only letter, digits and '_': '"+Name+"' is invalid",
				  BRATHL_SYNTAX_ERROR);
      }

      m_aliases[Name]	= Value;
    }
  }
  
  SubstituteAliases(m_aliases);

  // Remove alias keys
  m_mapParam.Erase(kwALIAS_NAME);
  m_mapParam.Erase(kwALIAS_VALUE);

}
//----------------------------------------
void CFileParams::SubstituteAliases(const CStringMap& aliases)
{

  CMapParameter::iterator it;
  for (it = m_mapParam.begin() ; it != m_mapParam.end() ; it++)
  {
    if ((it->first == kwALIAS_NAME) || (it->first == kwALIAS_VALUE))
    {
      continue;
    }

    it->second->SetAliases(aliases);
  }
}
//----------------------------------------
void CFileParams::LoadFieldSpecificUnits()
{
  m_fieldSpecificUnit.RemoveAll();

  uint32_t nbFieldSpecificUnit = CheckCount(kwUNIT_ATTR_NAME, 0, -1);

  if (nbFieldSpecificUnit == 0)
  {
    return;
  }
  std::string name;
  std::string value;

  CheckCount(kwUNIT_ATTR_VALUE, nbFieldSpecificUnit, -1);

  for (uint32_t index=0; index < nbFieldSpecificUnit; index++)
  {
    
    m_mapParam[kwUNIT_ATTR_NAME]->GetValue(name, index, "-NO-DEFAULT-VALUE-FOR-UNIT-ATTR-NAME-");

    m_mapParam[kwUNIT_ATTR_VALUE]->GetValue(value, index);

    m_fieldSpecificUnit.Insert(name, value);
  }
  
  // Remove unit attr keys
  m_mapParam.Erase(kwUNIT_ATTR_NAME);
  m_mapParam.Erase(kwUNIT_ATTR_VALUE);

}

//----------------------------------------
void CFileParams::Decode(char *line)
{
  CTrace::Tracer("Entered CFileParams::Decode\n");

  char *value = NULL;
  char *keyword = line;
  char *current = line;

  while (*current)
  {
    if ((*current == '=') && (value == NULL))
    {
      *current = '\0';	/* end of keyword*/
      value = current + 1;
    }
    current++;
  }

  if (value == NULL)
  {
    Dump(*CTrace::GetDumpContext());
    throw CParameterException("CFileParams::Decode - Error - Invalid line data (not a 'keyword=value' format) - filename:" +
			      GetFileName() +
			      " line:" +
			      std::string(line),
			      BRATHL_SYNTAX_ERROR);
  }

  CTools::RemoveAllSpaces(keyword);

  // Uppercase to avoid conflict
  CTools::ToUpper(keyword);

  CTools::Trim(value);

  if ((strlen(keyword) == 0) || (strlen(value) == 0))
  {
    Dump(*CTrace::GetDumpContext());
    throw CParameterException("CFileParams::Decode - Error - Invalid line data ('keyword' and/or 'value' empty) - filename:" +
			      GetFileName(),
			      BRATHL_SYNTAX_ERROR);
  }

  //pair <CMapParameter::iterator,bool> insertRet;
  CParameter* parameter = m_mapParam.Insert(keyword, value);

  if (parameter == NULL)
  {
    Dump(*CTrace::GetDumpContext());
    throw CParameterException("CFileParams::Decode - parameter object == NULL) - filename:" +
	      		      GetFileName(),
			      BRATHL_LOGIC_ERROR);
  }
}



//----------------------------------------
unsigned CFileParams::CheckCount( const std::string	&Key, 
	int32_t	ValidMin	/*= 1*/,
	int32_t	ValidMax	/*= 0 */ )
{
	int32_t	ActualCount	= 0;

	// DON'T USE this syntax if you are not sure the key exist, there's a bug in STL,  
	//   after calling  'record = m_recordSetMap[recordSetName]', if key not existed and the map is empty 
	//   then the key exists 
	//   in the map and points to a NULL object
	//CParameter	*p = m_mapParam[Key];
	CParameter	*p = m_mapParam.Exists( Key );
	if ( p != NULL )
		ActualCount	= p->Count();

	if ( ValidMin < 0 )
		ValidMin	= 0;
	if ( ValidMax > 0 )
	{
		if ( ValidMax < ValidMin )
			ValidMax	= ValidMin;
	}

	if ( ActualCount < ValidMin )
		throw  CParameterException( CTools::Format( "Invalid number of %s parameter: %d (min=%d)",
		Key.c_str(),
		ActualCount,
		ValidMin ),
		BRATHL_COUNT_ERROR );
	if ( ( ValidMax > 0 ) && ( ActualCount > ValidMax ) )
		throw  CParameterException( CTools::Format( "Invalid number of %s parameter: %d (max=%d)",
		Key.c_str(),
		ActualCount,
		ValidMax ),
		BRATHL_COUNT_ERROR );
	return ActualCount;
}

//----------------------------------------
void CFileParams::GetFieldNames(const std::string& key, CStringArray& fieldNames)
{

  if (!IsLoaded())
  {
    Load();
  }

  uint32_t nbExpr = CheckCount(key,  1, 32); // max 32 because of class CBitSet32

  for (uint32_t index = 0; index < nbExpr; index++)
  {
    CExpression expr;
    m_mapParam[key]->GetValue(expr, index);

    fieldNames.Insert(*expr.GetFieldNames());
  }
}
//----------------------------------------
std::string CFileParams::GetFirstFile(const std::string& key)
{

  if (!IsLoaded())
  {
    Load();
  }
  std::string fileName;
  
  CheckCount(key,  1, -1);
  
  m_mapParam[key]->GetValue(fileName, 0);
  return fileName;
}
//----------------------------------------
void CFileParams::GetFileList(const std::string& key, CStringArray& fileNames)
{

  if (!IsLoaded())
  {
    Load();
  }

  uint32_t nbFiles = CheckCount(key,  1, -1); // max 32 because of class CBitSet32
  
  fileNames.resize(nbFiles);

  for (uint32_t index = 0; index < nbFiles; index++)
  {
    m_mapParam[key]->GetValue(fileNames[index], index);
  }
}

//----------------------------------------
void CFileParams::SetVerboseLevel()
{
  int32_t level	= 0;
  if (CheckCount(kwVERBOSE, 0, 1) == 1)
  {
    m_mapParam[kwVERBOSE]->GetValue(level);
  }

  CTrace* pTrace = NULL;

  std::string logFile;

  if (CheckCount(kwLOGFILE, 0, 1) == 1)
  {
    m_mapParam[kwLOGFILE]->GetValue(logFile);
    pTrace = CTrace::GetInstance(logFile);
  }
  else
  {
    pTrace = CTrace::GetInstance();
  }


  pTrace->SetTraceLevel(level);

}



//----------------------------------------
void CFileParams::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (! CTrace::IsTrace())
  {
    return;
  }

  fOut << "==> Dump a CFileParams Object at "<< this << std::endl;

  //---------------------------------
  CFile::Dump(fOut);
  //---------------------------------

  fOut << "m_mapParam = " << std::endl;
  m_mapParam.Dump(fOut);

  fOut << "m_aliases = " << std::endl;
  m_aliases.Dump(fOut);

  fOut << "m_fieldSpecificUnit = " << std::endl;
  m_fieldSpecificUnit.Dump(fOut);

  fOut << "m_fieldSpecificUnit = " << m_isLoaded << std::endl;

  fOut << "==> END Dump a CFileParams Object at "<< this << std::endl;

  fOut << std::endl;

}

}
