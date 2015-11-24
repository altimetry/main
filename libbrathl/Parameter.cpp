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


#include <cerrno>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "new-gui/Common/tools/Exception.h"
#include "Expression.h"
#include <string>
#include "Tools.h"
#include "new-gui/Common/tools/TraceLog.h"
#include "brathl.h"
#include "new-gui/Common/tools/brathl_error.h"

#include "Parameter.h"
using namespace brathl;

namespace brathl
{


CParameter::CParameter()
{

}

//----------------------------------------

CParameter::CParameter(const char *keyword)
{
  m_keyword = keyword;

}


//----------------------------------------

CParameter::CParameter(const char *keyword, const char *value)
{
  m_keyword = keyword;

  AddValue(value);

}

//----------------------------------------

CParameter::CParameter(const std::string& keyword, const std::string& value)
{
  m_keyword = keyword;

  AddValue(value);


}

//----------------------------------------

CParameter::~CParameter()
{
  RemoveAllValue();
}

//----------------------------------------

void CParameter::AddValue(const char *value)
{
  std::string valueExpanded = CTools::ExpandShellVar(value);
  m_values.push_back(valueExpanded);
}
//----------------------------------------
void CParameter::AddValue(const std::string& value)
{
  AddValue(value.c_str());
}

//----------------------------------------

bool CParameter::RemoveValue(uint32_t i)
{
  //std::vector<std::string>::iterator iter;

  if (i >= m_values.size())
  {
    return false;
  }

  m_values.erase(m_values.begin() + i);

  return true;
}

//----------------------------------------

bool CParameter::RemoveAllValue()
{
  //m_values.erase(m_values.begin(), m_values.end());
  m_values.clear();
  return true;
}


//----------------------------------------
size_t CParameter::Count()
{
  return m_values.size();
}


//----------------------------------------
void CParameter::SetAliases(const CStringMap& aliases)
{
  for (CStringArray::iterator it = m_values.begin(); it != m_values.end(); it++)
  {
    // Don't raise an exception if an alias is not found because formulas can contain field aliases
    // and they must be kept as aliases 
    *it	= CTools::ExpandVariables(*it, &aliases, NULL, true, '%', NULL, false, NULL);
  }
}


//----------------------------------------
void CParameter::CommonCtrl(int32_t pos /* = 0*/)
{

  if ((pos < 0) || (pos >= int32_t(m_values.size())))
  {
    CParameterException e(CTools::Format("Parameter index out of range:#%d - acceptable index [0,%ld] for key %s",
					 pos,
					 (long)m_values.size(),
					 m_keyword.c_str()),
			  BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw e;
  }
}

//----------------------------------------
void CParameter::GetValue
		(int32_t	&value,
		 int32_t	pos /* = 0*/,
		 int32_t	DefValue /*= CTools::m_defaultValueINT32*/)
{

  char *stopString = NULL;

  CommonCtrl(pos);

  if (CTools::StrCaseCmp(m_values[pos].c_str(), "DV") == 0)
  {
    value	= DefValue;
    return;
  }

  errno	= 0;
  value = strtol(m_values[pos].c_str(), &stopString, 10);

  if ((*stopString != '\0') || (errno != 0) || (m_values[pos].size() == 0))
  {
    throw CParameterException(CTools::Format("Invalid integer value for key %s[%d]: %s",
					     m_keyword.c_str(),
					     pos,
					     m_values[pos].c_str()),
			      BRATHL_SYNTAX_ERROR);
  }
}

//----------------------------------------
void CParameter::GetValue
		(uint32_t	&value,
		 int32_t	pos /* = 0*/,
		 uint32_t	DefValue /*= CTools::m_defaultValueUINT32*/)
{

  char *stopString = NULL;

  CommonCtrl(pos);

  if (CTools::StrCaseCmp(m_values[pos].c_str(), "DV") == 0)
  {
    value	= DefValue;
    return;
  }

  errno	= 0;
  value = strtoul(m_values[pos].c_str(), &stopString, 10);

  if ((*stopString != '\0') || (errno != 0) || (m_values[pos].size() == 0))
  {
    throw CParameterException(CTools::Format("Invalid unsigned integer value for key %s[%d]: %s",
					     m_keyword.c_str(),
					     pos,
					     m_values[pos].c_str()),
			      BRATHL_SYNTAX_ERROR);
  }
}


//----------------------------------------
void CParameter::GetValue(double& value, int32_t pos /* = 0*/, double DefValue /*= CTools::m_defaultValueDOUBLE*/)
{

  CExpression expression;
  CExpressionValue exprValue;

  CommonCtrl(pos);

  if (CTools::StrCaseCmp(m_values[pos].c_str(), "DV") == 0)
  {
    value	= DefValue;
    return;
  }

  try
  {
    expression.SetExpression(m_values[pos]);
    if (expression.GetFieldNames()->size() != 0)
    {
      throw CParameterException("Fieldnames are not allowed for float parameter value",
	      	      	        BRATHL_SYNTAX_ERROR);
    }

    exprValue = expression.Execute();
    value = *(exprValue.GetValues());
  }
  catch (CException e)
  {
    throw CParameterException(CTools::Format("Can't get value - invalid expression for key %s[%d]: %s",
					     m_keyword.c_str(),
					     pos,
	      				     e.what()),
			      BRATHL_ERROR);

  }
}


//----------------------------------------
void CParameter::GetValue(bool& value, int32_t pos /* = 0*/, bool DefValue /*= false*/)
{
  CommonCtrl(pos);

  std::string What	= CTools::StringToUpper(m_values[pos]);
  if ((What == "YES")	||
      (What == "Y")	||
      (What == "TRUE")	||
      (What == "T")	||
      (What == "OUI")	||
      (What == "O")	||
      (What == "VRAI")	||
      (What == "V")	||
      (What == "1"))
  {
    value = true;
    return;
  }

  if ((What == "NO")	||
      (What == "N")	||
      (What == "FALSE")	||
      (What == "F")	||
      (What == "NON")	||
      (What == "FAUX")	||
      (What == "0"))
  {
    value = false;
    return;
  }

  if (What == "DV")
  {
    value = DefValue;
    return;
  }

  throw CParameterException(CTools::Format("Invalid boolean value for key %s[%d]: %s (Allowed=YES,Y,TRUE,T,1,NO,N,FALSE,F,0)",
					   m_keyword.c_str(),
					   pos,
					   m_values[pos].c_str()),
			    BRATHL_SYNTAX_ERROR);
}


//----------------------------------------
void CParameter::GetValue(CDate& value, int32_t pos /* = 0*/)
{
  CommonCtrl(pos);

  int32_t result = value.SetDate(m_values[pos].c_str());
  if (result != BRATHL_SUCCESS)
  {
    throw CParameterException(CTools::Format("Invalid date value for key %s[%d]: %s",
					     m_keyword.c_str(),
					     pos,
					     m_values[pos].c_str()),
			      BRATHL_SYNTAX_ERROR);
  }
}
//----------------------------------------
void CParameter::GetValue(CDate& value, const std::string& strUnit, int32_t pos /* = 0*/)
{
  CUnit unit;
  try
  {
    unit = strUnit;
  }
  catch (CException& e)
  {
    e.what();
  }

  GetValue(value, unit, pos);

}
//----------------------------------------
void CParameter::GetValue(CDate& value, CUnit* unit, int32_t pos /* = 0*/)
{
  if (unit == NULL)
  {
    GetValue(value, pos);
  }
  else
  {
    GetValue(value, *unit, pos);
  }
}
//----------------------------------------
void CParameter::GetValue(CDate& value, CUnit& unit, int32_t pos /* = 0*/)
{
  int32_t result = BRATHL_SUCCESS;

  CommonCtrl(pos);

  if (CDate::IsCharDate(m_values[pos].c_str()))
  {
    result = value.SetDate(m_values[pos].c_str());
  }
  else
  {
    double valueDouble;
    setDefaultValue(valueDouble);

    GetValue(valueDouble, pos);

    unit.ConvertToBaseUnit(valueDouble);

    result = value.SetDate(unit.ConvertToBaseUnit(valueDouble));

  }

  if (result != BRATHL_SUCCESS)
  {
    throw CParameterException(CTools::Format("Invalid date value for key %s[%d]: %s",
					     m_keyword.c_str(),
					     pos,
					     m_values[pos].c_str()),
			      BRATHL_SYNTAX_ERROR);
  }
}

//----------------------------------------
void CParameter::GetValue(std::string& value, int32_t pos /* = 0*/, const std::string &DefValue /*=""*/)
{
  CommonCtrl(pos);

  if (CTools::StrCaseCmp(m_values[pos].c_str(), "DV") == 0)
    value	= DefValue;
  else
    value	= m_values[pos];
}

//----------------------------------------
void CParameter::GetValue(char* value, int32_t bufferSize, int32_t pos /* = 0*/, const char *DefValue /*=""*/)
{
  memset(value, '\0', bufferSize);

  CommonCtrl(pos);

  if (CTools::StrCaseCmp(m_values[pos].c_str(), "DV") == 0)
  {
    strncpy(value, DefValue, bufferSize-1);
  }
  else
  {
    auto lenToCopy = m_values[pos].length();

    if (bufferSize <= lenToCopy)
    {
      CTrace::GetInstance()->Tracer(-1, "WARNING : Value of key %s[%d] is too long (%d) while only %d characters are expected. Value is troncated",
				    m_keyword.c_str(),
				    pos,
				    lenToCopy,
				    bufferSize);
    }

    strncpy(value, m_values[pos].c_str(), bufferSize-1);
  }
  value[bufferSize-1] = '\0';
}



//----------------------------------------
void CParameter::GetValue(CExpression& value, int32_t pos /* = 0*/)
{
  CommonCtrl(pos);

  try
  {
    value.SetExpression(m_values[pos]);
  }
  catch (CException e)
  {
    throw CParameterException(CTools::Format("Invalid expression value for key %s[%d]: %s",
					     m_keyword.c_str(),
					     pos,
	      				     e.what()),
			      BRATHL_ERROR);

  }
}



//----------------------------------------
void CParameter::GetValue(CUnit& value, int32_t pos /* = 0*/, const std::string &DefValue /*="count"*/)
{
  CommonCtrl(pos);

  try
  {
    if (CTools::StrCaseCmp(m_values[pos].c_str(), "DV") == 0)
      value	= DefValue;
    else
      value	= m_values[pos];
  }
  catch (CException e)
  {
    throw CParameterException(CTools::Format("Invalid unit value for key %s[%d]: %s",
					     m_keyword.c_str(),
					     pos,
	      				     e.what()),
			      BRATHL_ERROR);

  }
}

//----------------------------------------
void CParameter::GetValue
		(uint32_t		&value,
		 std::string			&ValueName,
		 const KWValueListEntry	*KeywordList,
		 int32_t		pos		/*= 0*/,
		 uint32_t		DefValue	/*= CTools::m_defaultValueUINT32*/)
{
  GetValue(ValueName, pos, "");
  ValueName	= CTools::StringToUpper(ValueName);
  if (ValueName == "")
  {
    value	= DefValue;
    return;
  }

  for (uint32_t Index = 0;
       KeywordList[Index].Name != NULL;
       Index++)
  {
    if (ValueName == KeywordList[Index].Name)
    {
      value	= KeywordList[Index].Value;
      return;
    }
  }
  throw CParameterException(CTools::Format("Invalid value for keyword %s: '%s'",
					   m_keyword.c_str(),
					   ValueName.c_str()),
			    BRATHL_SYNTAX_ERROR);
}

//----------------------------------------
void CParameter::GetValue
		(bitSet32		&value,
		 const KWValueListEntry	*KeywordList,
		 int32_t		pos		/*= 0*/,
		 const bitSet32 	&DefValue	/*= 0*/)
{
  std::string		Line;
  const std::string  	Separators	= " \t\n,:;";
  const std::string  	ValidChars	= "ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789";
  std::string::size_type	Begin;
  std::string::size_type	End;
  bool			NoKeyword	= true;
  GetValue(Line, pos, "");

  Line	= CTools::StringToUpper(Line);
  Begin	= 0;
  value.reset();
  while (Begin < Line.size())
  {
    End		= Begin;
    Begin	= Line.find_first_not_of(Separators, Begin);
    if (Begin == std::string::npos)
      break; // Only separators are left

    if (ValidChars.find(Line[Begin]) == std::string::npos)
    {
      throw CParameterException(CTools::Format("Invalid value for keyword %s: invalid char '%c' in keyword list '%s'",
					       m_keyword.c_str(),
					       Line[Begin],
					       Line.c_str()),
				BRATHL_SYNTAX_ERROR);
    }
    End	= Line.find_first_not_of(ValidChars, Begin);
    if (End == std::string::npos)
      End	= Line.size();
    // Extract one keyword
    std::string Keyword	= Line.substr(Begin, End-Begin);
    Begin		= End;
    NoKeyword		= false;
    // Find keyword
    uint32_t	KWIndex		= 0;
    while (KeywordList[KWIndex].Name != NULL)
    {
      if (Keyword == KeywordList[KWIndex].Name)
	break;
      KWIndex++;
    }
    if (KeywordList[KWIndex].Name == NULL)
    {
      throw CParameterException(CTools::Format("Invalid value for keyword %s: list value '%s' unknown",
					       m_keyword.c_str(),
					       Keyword.c_str()),
			        BRATHL_RANGE_ERROR);
    }
    value	|= bitSet32((int)KeywordList[KWIndex].Value);
  }
  if (NoKeyword)
    value = DefValue;
}
//----------------------------------------
void CParameter::GetValue
		(uint32_t		&value,
		 std::string			&ValueName,
		 CUIntMap&	        KeywordList,
		 int32_t		pos		/*= 0*/,
		 uint32_t		DefValue	/*= CTools::m_defaultValueUINT32*/)
{
  GetValue(ValueName, pos, "");
  ValueName	= CTools::StringToUpper(ValueName);
  if (ValueName.empty())
  {
    value	= DefValue;
    return;
  }

  value = KeywordList[ValueName];

  if (isDefaultValue(value))
  {
    throw CParameterException(CTools::Format("Invalid value for keyword %s: '%s'",
					     m_keyword.c_str(),
					     ValueName.c_str()),
			      BRATHL_SYNTAX_ERROR);

  }

}

//----------------------------------------
void CParameter::GetAllValues
		(CStringList&	listValues)
{
  listValues.clear();
  listValues.Insert(m_values);
}

//----------------------------------------
void CParameter::GetAllValues
		(CStringArray&	vectValue)
{
  vectValue.clear();
  vectValue.Insert(m_values);
}


//----------------------------------------
void CParameter::GetAllValues
		(CExpression	&value,
		 const std::string	&Combine /* = "&&" */)
{
  std::string Tmp;

  CommonCtrl(0);

  for (uint32_t Index=0; Index<m_values.size(); Index++)
  {
    if (Index != 0)
      Tmp	+= " " + Combine + " ";
    Tmp	+= "(" + m_values[Index] + ")";
  }
  try
  {
    value.SetExpression(Tmp);
  }
  catch (CException e)
  {
    throw CParameterException(CTools::Format("Invalid combined expression value for key %s: %s",
					     m_keyword.c_str(),
	      				     e.what()),
			      BRATHL_ERROR);
  }
}



//----------------------------------------
void CParameter::Dump(std::ostream& fOut /* = std::cerr */)
{
  int i = 0;
  std::vector<std::string>::iterator iteratorString;

  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CParameter Object at "<< this << std::endl;

  fOut << "m_keyword = " << m_keyword << std::endl;
  fOut << "m_values = " << std::endl;

  for (iteratorString = m_values.begin() ; iteratorString != m_values.end() ; iteratorString++)
  {
    fOut << "  Value " << i <<  "=" << *iteratorString << std::endl;
    i++;
  }

  fOut << "==> END Dump a CParameter Object at "<< this << std::endl;

  fOut << std::endl;

}

}
