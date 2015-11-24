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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA	 02110-1301, USA.
*/

#include <cmath>
#include <cctype>
#include <cstring>
#include <cassert>
#include <string>

#include "brathl.h"

#include "Tools.h"
#include "new-gui/Common/tools/Exception.h"
#include "Expression.h"

#include "Unit.h"

using namespace brathl;

namespace brathl
{

const std::string CUnit::m_DATE_REF_UNIT = "seconds since 1950-01-01 00:00:00.000000 UTC";

const std::string CUnit::m_UNIT_SI = "SI";

bool CUnit::m_initialized = false;

std::string CUnit::m_ErrInit	= "";

CDate CUnit::m_dateRefUdunits;

//----------------------------------------

CUnit::CUnit(const std::string	&text /* = "" */)
{
  // initialize on instanciation of the first CUnit object.

  if (!m_initialized)
  {
    InitializeUnitSystem(); // will throw on error
  }

  Set(text);
}

/*
//----------------------------------------
CUnit::CUnit(const char* text)
{
  // initialize on instanciation of the first CUnit object.

  if (!m_initialized)
    InitializeUnitSystem(); // will throw on error

  Set(text);
}
*/
//----------------------------------------

CUnit::CUnit
		(const CUnit	&Copy)
{
  m_Text	= Copy.m_Text;
  m_Compiled	= Copy.m_Compiled;
  m_Slope	= Copy.m_Slope;
  m_Offset	= Copy.m_Offset;
}


//----------------------------------------

CUnit::~CUnit
		()
{
//   if (--m_UnitObjectCount < 1)
//   {
//     utTerm();
//   }
}

//----------------------------------------
void CUnit::InitializeUnitSystem()
{
  CUnit::m_dateRefUdunits = "2001-01-01 00:00:00.0";

  std::string FileName = CTools::FindDataFile(BRATHL_UNITFILE);

  m_initialized = true; // Only get one shot at this - failed or otherwise

  if (FileName.empty())
    {
    std::string errorMsg = CTools::Format("Units system file '%s' not found in '%s' directory."
                                     "Check directory '%s' or set correct environment variable %s",
                                      BRATHL_UNITFILE,
                                      CTools::GetDataDir().c_str(),
                                      CTools::GetDataDir().c_str(),
                                      BRATHL_ENVVAR);

      throw CFileException(errorMsg, BRATHL_UNIT_ERROR);
    }

  // didnt throw (yet), so initilaize the units system - this might throw
  CUnit::CheckUdunits(utInit(FileName.c_str()), FileName);
  
  // if the initializationdid not throw, then create the static instance
  // of the cleaner to ensure the unit system is closed properly.

  static CUnitSmartCleaner object;
}

//----------------------------------------
void CUnit::Set(const CUnit& u)
{
  m_Text	= u.m_Text;
  m_Compiled	= u.m_Compiled;
  m_Slope	= u.m_Slope;
  m_Offset	= u.m_Offset;
}
//----------------------------------------
void CUnit::Set(const std::string& text)
{
  //try
  //{
    if (text == "")
    {
      m_Text	= "count";
    }
    else

	if ( text == "seconds in the day" )
		m_Text = "seconds_in_the_day";
	else
	if (text == "FFT power unit")
		m_Text = "FFT_power_unit";
	else
	if (text == "/")
		m_Text = "meter";
    else
    {
      m_Text	= text;
    }
    Compile();
    //}

    /*
  catch (CException &e)
  {
    if (m_smartCleaner.m_Initialized)
    {
      throw; // Main program has been called
    }
    m_smartCleaner.m_ErrInit.append(e.what());
    m_smartCleaner.m_ErrInit.append("\n");
  }
    */

}
/*
//----------------------------------------
void CUnit::Set(const char* text)
{
  if (text == NULL)
  {
    return;
  }

  std::string str = text;
  Set(str);
}
*/
//----------------------------------------

CUnit &CUnit::operator=(const std::string& text)
{
  Set(text);
  return *this;
}

//----------------------------------------
/*
CUnit &CUnit::operator=(const char* text)
{

  Set(text);

  return *this;
}

*/
//----------------------------------------

CUnit &CUnit::operator=
		(const CUnit	&Source)
{
  m_Text	= Source.m_Text;
  m_Slope	= Source.m_Slope;
  m_Offset	= Source.m_Offset;
  m_Compiled	= Source.m_Compiled;
  return *this;
}

//----------------------------------------
bool CUnit::operator==(CUnit& u)
{
  return str_icmp(this->AsString().c_str(), u.AsString().c_str());
}
//----------------------------------------
bool CUnit::operator==(const std::string& text)
{
  CUnit u = text;
  return (*this == u);
}

//----------------------------------------
bool CUnit::HasNoUnit()
{
  return (*this == "") || (*this == "count");
}
//----------------------------------------
bool CUnit::HasUnit()
{
  return !HasNoUnit();
}
//----------------------------------------
CUnit CUnit::ToUnit(const std::string& unitStr)
{

  CUnit unit;
  try
  {
    unit = unitStr;
  }
  catch (CException &e)
  {
    e.what(); // To avoid compiler warning
  }

  return unit;
}

//----------------------------------------

CUnit CUnit::BaseUnit
		()
{
  CUnit	Result;
  Result.m_Compiled		= m_Compiled;
  FindBaseUnit(Result.m_Compiled);
  Result.m_Text			= Result.AsString();
  return Result;
}

//----------------------------------------

void CUnit::SetConversionTo
		(const CUnit	&Destination)
{
  CheckUdunits(utConvert(&m_Compiled,
			 &Destination.m_Compiled,
			 &m_Slope,
			 &m_Offset),
	       m_Text,
	       Destination.m_Text);
}

//----------------------------------------

void CUnit::SetConversionTo
		(const std::string	&Destination)
{
  SetConversionTo(CUnit(Destination));
}

//----------------------------------------

void CUnit::SetConversionFrom
		(const CUnit	&Source)
{
  CheckUdunits(utConvert(&Source.m_Compiled,
			 &m_Compiled,
			 &m_Slope,
			 &m_Offset),
	       Source.m_Text,
	       m_Text);
}

//----------------------------------------

void CUnit::SetConversionFrom
		(const std::string	&Source)
{
  SetConversionFrom(CUnit(Source));
}
//----------------------------------------
void CUnit::SetConversionFromBaseUnit()
{
  // if unit has a date reference (eg xxxx since YYYY-MM-DD hh:MM:SS.MS)
  // ==> Set conversion to 'seconds since 1950-01-01 00:00:00.0 UTC"
  if (HasDateRef())
  {
    SetConversionFrom(CUnit::m_DATE_REF_UNIT);  
  }
  else
  {
    SetConversionFrom(BaseUnit());
  }
}
//----------------------------------------
void CUnit::SetConversionToBaseUnit()
{
  // if unit has a date reference (eg xxxx since YYYY-MM-DD hh:MM:SS.MS)
  // ==> Set conversion to 'seconds since 1950-01-01 00:00:00.0 UTC"
  if (HasDateRef())
  {
    SetConversionTo(CUnit::m_DATE_REF_UNIT);  
  }
  else
  {
    SetConversionTo(BaseUnit());
  }
}

//----------------------------------------
double CUnit::ConvertToUnit(CUnit* unitIn, const double	fromValue, double defaultValue /* = CTools::m_defaultValueDOUBLE */)
{
  if (unitIn == NULL)
  {
    return defaultValue;
  }
  
  return unitIn->ConvertToUnit(fromValue, defaultValue);
}
//----------------------------------------
double CUnit::ConvertToUnit(const double	fromValue, double defaultValue /* = CTools::m_defaultValueDOUBLE */)
{
  double out = defaultValue;

  if (isDefaultValue(fromValue))
  {
    if (!isDefaultValue(defaultValue))
    {
      out = ConvertToUnit(defaultValue);
    }
    return out;
    
  }
  
  CUnit	unit	= this->BaseUnit();
  unit.SetConversionTo(*this);
  out	= unit.Convert(fromValue);

  return out;
}
//----------------------------------------
double CUnit::ConvertToBaseUnit(CUnit* unitIn, const double	fromValue, double defaultValue /* = CTools::m_defaultValueDOUBLE */)
{
  if (unitIn == NULL)
  {
    return defaultValue;
  }
  
  return unitIn->ConvertToBaseUnit(fromValue, defaultValue);

}

//----------------------------------------
double CUnit::ConvertToBaseUnit(const double	fromValue, double defaultValue /* = CTools::m_defaultValueDOUBLE */)
{
  double out = defaultValue;

  if (isDefaultValue(fromValue))
  {
    if (!isDefaultValue(defaultValue))
    {
      out = ConvertToBaseUnit(defaultValue);
    }
    return out;
  }

  CUnit	unit	= *this;
  unit.SetConversionToBaseUnit();
  out	= unit.Convert(fromValue);
  
  return out;
}
//----------------------------------------
double CUnit::Convert(const double fromValue)
{
  return CTools::Plus(CTools::Multiply(fromValue, m_Slope), m_Offset);
}


//----------------------------------------

void CUnit::ConvertVector(std::vector<double>& vect)
{
  for (uint32_t index = 0; index < vect.size(); index++)
  {
    vect[index]	= Convert(vect[index]);
  }
}
//----------------------------------------

void CUnit::ConvertVector(double* vect, int32_t size)
{
  for (int32_t index = 0; index < size; index++)
  {
    vect[index]	= Convert(vect[index]);
  }
}


//----------------------------------------

bool CUnit::IsDate
		() const
{
  return utIsTime(&m_Compiled) != 0;
}


//----------------------------------------

bool CUnit::IsCompatible(const std::string& otherUnit, std::string* errorMsg /* = NULL */) const
{
  bool bOk = false;

  try 
  {
    CUnit unit = otherUnit;
    bOk = IsCompatible(unit);
  }
  catch (CException& e)
  {
    if (errorMsg != NULL)
    {
      errorMsg->append(CTools::Format("Error while checking unit (CUnit::IsCompatible) : %s\n",
                                  e.GetMessage().c_str()));
    }
    else
    {
      throw e;
    }
  }
  return bOk;
}


//----------------------------------------
bool CUnit::IsCompatible(const CUnit*	otherUnit) const
{
  return IsCompatible(*otherUnit);
}

//----------------------------------------
bool CUnit::IsCompatible(const CUnit& otherUnit) const
{
  double	N1;
  double	N2;

  int32_t	ConvertStatus;
  
  ConvertStatus	= utConvert(&m_Compiled, &otherUnit.m_Compiled, &N1, &N2);

  if (ConvertStatus == 0)
  {
    return true;
  }

  if (ConvertStatus == UT_ECONVERT)
  {
    return false;
  }

  CheckUdunits(ConvertStatus, m_Text, otherUnit.m_Text);

  return false; // never reached but avoids compiler complaint
}


//----------------------------------------

std::string CUnit::AsString
		(bool asBaseUnit /*= true*/, bool withDateOrigin /*= false*/, CDate* dateRef /*= NULL*/) const
{
  std::string result;

  if (asBaseUnit)
  {
    char	*Buffer;
    CheckUdunits(utPrint(&m_Compiled, &Buffer), m_Text);
  
    result = std::string(Buffer);
  }
  else
  {
    result = m_Text;
  }

  if ( (withDateOrigin) && IsDate() )
  {
    result.append(GetDateRefAsString(dateRef));
  }

  return result;
}
//----------------------------------------
std::string CUnit::GetDateUnitWithoutDateOrigin() const
{
  std::string str = GetText();

  if (!IsDate())
  {
    return str;
  }

  if (!HasDateRef())
  {
    return str;
  }

  CStringArray ar;

  ar.ExtractStrings(GetText().c_str(), " since ");

  size_t len = ar.size();


  if (len <= 1)
  {
    return str;
  }

  str = ar[0];

  return str;

}


//----------------------------------------
bool CUnit::HasDateRef(CDate* dateRef /*= NULL*/, CStringArray* array /*= NULL*/) const
{
  if (!IsDate())
  {
    return false;
  }

  CStringArray ar;

  ar.ExtractStrings(GetText().c_str(), ' ');

  size_t len = ar.size();


  if (len <= 2)
  {
    return false;
  }

  int32_t index = ar.FindIndex("since");

  if (index < 0)
  {
    return false;
  }

  CDate dateTmp;

  int32_t result = BRATHL_ERROR;
  std::string strDate;

  for (uint32_t i = index + 1 ; i < ar.size() ; i++)
  {
    strDate.append(ar.at(i));
    strDate.append(" ");

  }

  strDate = CTools::StringTrim(strDate);
  
  result = dateTmp.SetDate(strDate.c_str());

  if (result == BRATHL_SUCCESS)
  {
    if (dateRef != NULL)
    {
      *dateRef = dateTmp;
    }
  }
  
  if (array != NULL)
  {
    array->Insert(ar);
  }

  return (result == BRATHL_SUCCESS);
}

//----------------------------------------
std::string CUnit::GetDateRefAsString(CDate* dateRef /*= NULL*/) const
{

  std::string result;

  if (!IsDate())
  {
    return result;
  }

  CDate dateTmp;
  if (HasDateRef(&dateTmp))
  {
    return result;
  }

  if (dateRef == NULL)
  {
    CDate d;
    result = GetDateRefAsString(&d);
  }
  else
  {
    result.append(" since ");
    result.append(dateRef->AsString("", true));
    result.append(" UTC");
  }

  return result;

}
//----------------------------------------
std::string CUnit::GetDateRefAs1950() const
{

  std::string result;

  if (!IsDate())
  {
    return result;
  }

  CDate dateTmp;
  CStringArray array;

  if (!HasDateRef(NULL, &array))
  {
    return result;
  }

  result.append(array[0]);
  result.append(" since ");
  result.append(dateTmp.AsString("", true));
  result.append(" UTC");

  return result;

}

//----------------------------------------

void CUnit::Dump
		(std::ostream& fOut /*  = std::cerr */)
{
  fOut << std::endl;
  fOut << "Unit dump of         : " << m_Text << std::endl;
  fOut << "Base unit            : " << AsString() << std::endl;
  fOut << "Conversion slope set : " << m_Slope << std::endl;
  fOut << "Conversion offset set: " << m_Offset << std::endl;
}





//----------------------------------------

void CUnit::Compile
		()
{
  utUnit	Base;
  m_Slope	= 1.0;
  m_Offset	= 0.0;
  if (CTools::StringToUpper(m_Text) == "DATE")
  {
    m_Text	= "days";
    m_Text.append(GetDateRefAsString());
  }
  
  m_Text = CTools::RemoveCharSurroundingNumber(m_Text);
  
  CheckUdunits(utScan(m_Text.c_str(), &m_Compiled), m_Text);
  Base		= m_Compiled;
  // = SetConversionTo(BaseUnit(self))
  FindBaseUnit(Base);
  CheckUdunits(utConvert(&m_Compiled,
			 &Base,
			 &m_Slope,
			 &m_Offset),
		m_Text,
		"Base unit");
}







//----------------------------------------


void CUnit::CheckUdunits
		(uint32_t	Code,
		 const std::string	&Msg1	/*= ""*/,
		 const std::string	&Msg2	/*= ""*/)
{
  static CUnitSmartCleaner object; // can throw - but otherwise initializes the units system

  std::string Message;

  if ((Code == 0) && CUnit::m_ErrInit.empty())
    return;

  switch ((int32_t)Code)
  {
    case 0:
	break;
    case UT_ENOFILE:
	Message	= "No unit file ("+Msg1+")";
	break;
    case UT_ESYNTAX:
	Message	= "Syntax error in unit '"+Msg1+"'";
	break;
    case UT_EUNKNOWN:
	Message	= "Unknown unit '"+Msg1+"'";
	break;
    case UT_EIO:
	Message	= "I/O error for unit file "+Msg1;
	break;
    case UT_EINVALID:
	Message	= "Invalid unit description for '"+Msg1+"' or '"+Msg2+"'";
	break;
    case UT_ENOINIT:
	Message	= "PROGRAM ERROR: udunit package non initialized";
	break;
    case UT_ECONVERT:
	Message	= "Units are not compatible for conversion ('"+Msg1+"' vs '"+Msg2+"')";
	break;
    case UT_EALLOC:
	Message	= "Memory allocation problem for unit management";
	break;
    case UT_ENOROOM:
	Message	= "Not enough room";
	break;
    case UT_ENOTTIME:
	Message	= "Not a time unit: '"+Msg1+"'";
	break;
    default:
	Message	= CTools::Format("Unknown error code %d from udunits", Code);
	break;
  }
  if (! CUnit::m_initialized)
  {
 
    if (Code != 0)
    {
      CUnit::m_ErrInit.append(Message);
      CUnit::m_ErrInit.append("\n");
    }

    Message = CUnit::m_ErrInit;
  }
  throw CException(Message, BRATHL_UNIT_ERROR);
}


//----------------------------------------


void CUnit::FindBaseUnit
		(utUnit	&Base)
{
  Base.factor		= 1.0;
  Base.origin		= 0.0;
}



//----------------------------------------
void CUnit::ClearInitError
		()
{
  // this is never called
  assert(false);

// Verify logic of above code
// Signal 6 if udunits is initialized and there is an initialization error message
// which is IMPOSSIBLE.
}


//----------------------------------------

CUnitSmartCleaner::CUnitSmartCleaner()
{
  // Only one (static) instance of this object is ever created,
  // and only of the initialization succeeds. The point of this
  // class is to call utTerm at program exit.
}

//----------------------------------------
CUnitSmartCleaner::~CUnitSmartCleaner()
{
    utTerm();
}



}
