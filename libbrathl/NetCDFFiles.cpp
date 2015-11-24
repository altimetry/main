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

#include <algorithm>
#include <string>

#include "brathl.h"

#include "Tools.h"
#include "Date.h"
#include "List.h"
#include "new-gui/Common/tools/Exception.h"
#include "Expression.h"
#include "Mission.h"
#include "ProductNetCdf.h"

#include "NetCDFFiles.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"


using namespace brathl;

namespace brathl
{

extern "C" {
// Avoids warnings about old cast
static const double NcFillByte = NC_FILL_BYTE;
static const double NcFillChar = NC_FILL_CHAR;
static const double NcFillShort = NC_FILL_SHORT;
static const double NcFillInt = NC_FILL_INT;
static const double NcFillFloat = NC_FILL_FLOAT;
static const double NcFillDouble = NC_FILL_DOUBLE;
//new
static const double NcFillUByte = NC_FILL_UBYTE;
static const double NcFillUShort = NC_FILL_USHORT;
static const double NcFillUInt = NC_FILL_UINT;
static const double NcFillInt64 = NC_FILL_INT64;
static const double NcFillUInt64 = NC_FILL_UINT64;
static const double NcFillString = (double)(ptrdiff_t)NC_FILL_STRING;
}

const std::string NC_NAT_NAME = "Not A Type";
const std::string NC_BYTE_NAME = "signed 1 byte integer";
const std::string NC_CHAR_NAME = "ASCII character";
const std::string NC_SHORT_NAME = "signed 2 byte integer";
const std::string NC_INT_NAME = "signed 4 byte integer";
const std::string NC_FLOAT_NAME = "single precision floating point number";
const std::string NC_DOUBLE_NAME = "double precision floating point number";
//new
const std::string NC_UBYTE_NAME = "unsigned 1 byte integer";
const std::string NC_USHORT_NAME = "unsigned 2 byte integer";
const std::string NC_UINT_NAME = "unsigned 4 byte integer";
const std::string NC_INT64_NAME = "signed 8 byte integer";
const std::string NC_UINT64_NAME = "unsigned 8 byte integer";
const std::string NC_STRING_NAME = "array of strings";



const int32_t CNetCDFFiles::m_TIME_NAMES_SIZE = 6;
const std::string CNetCDFFiles::m_TIME_NAMES[] = {"time", "TIME", "Time",
                                               "date", "Date", "DATE",
                                              };
const int32_t CNetCDFFiles::m_LAT_NAMES_SIZE = 12;
const std::string CNetCDFFiles::m_LAT_NAMES[] = {"latitude", "Latitude", "LATITUDE",
                                              "lat", "Lat", "LAT",
                                              "latitudes", "Latitudes", "LATITUDES",
                                              "nbLatitudes", "NbLatitudes", "NBLATITUDES",
                                             };
const int32_t CNetCDFFiles::m_LON_NAMES_SIZE = 12;
const std::string CNetCDFFiles::m_LON_NAMES[] = {"longitude", "Longitude", "LONGITUDE",
                                              "lon", "Lon", "LON",
                                              "longitudes", "Longitudes", "LONGITUDES",
                                              "nbLongitudes", "NbLongitudes", "NBLONGITUDES",
                                              };

const int32_t CNetCDFFiles::m_CYCLE_NAMES_SIZE = 10;
const std::string CNetCDFFiles::m_CYCLE_NAMES[] = {"cycle", "Cycle", "CYCLE",
                                                "cycles", "Cycles", "CYCLEs",
                                                "cycle_number", "Cycle_number", "Cycle_Number", "CYCLE_NUMBER",
                                               };
const int32_t CNetCDFFiles::m_PASS_NAMES_SIZE = 13;
const std::string CNetCDFFiles::m_PASS_NAMES[] = {"pass", "Pass", "PASS",
                                               "tracks", "Tracks", "TRACKS",
                                               "track", "Track", "TRACK",
                                                "pass_number", "Pass_number", "Pass_Number", "PASS_NUMBER",
                                              };

const int32_t CNetCDFFiles::m_LAT_UNITS_SIZE = 4;
const std::string CNetCDFFiles::m_LAT_UNITS[] = {"degrees_north", "degree_north",
                                            "degrees_south", "degree_south",
                                             };

const int32_t CNetCDFFiles::m_LON_UNITS_SIZE = 4;
const std::string CNetCDFFiles::m_LON_UNITS[] = {"degrees_east", "degree_east",
                                            "degrees_west", "degree_west",
                                             };



//-------------------------------------------------------------
//------------------- CNetCDFAttr class --------------------
//-------------------------------------------------------------
CNetCDFAttr::CNetCDFAttr()
{
  Init();
}
//----------------------------------------
CNetCDFAttr::CNetCDFAttr(CNetCDFAttr& a)
{
  Init();
  Set(a);
}

//----------------------------------------
CNetCDFAttr::CNetCDFAttr(const std::string& name, bool globalAttr)
{
  Init();
  SetName(name);
  SetGlobalAttr(globalAttr);
}
//----------------------------------------
CNetCDFAttr::~CNetCDFAttr()
{
}

//----------------------------------------
void CNetCDFAttr::Init()
{
  m_type = NC_NAT;
  m_globalAttr = false;
  m_length = 1;
}
//-------------------------------------------------------------
void CNetCDFAttr::Set(CNetCDFAttr& a) 
{
  
  m_name = a.m_name;
  m_globalAttr = a.m_globalAttr;
  m_type = a.m_type;
  m_length = a.m_length;

  
}
//----------------------------------------
const CNetCDFAttr& CNetCDFAttr::operator =(CNetCDFAttr& a)
{
  Set(a);    
  return *this;    
}

//----------------------------------------
CNetCDFAttr* CNetCDFAttr::GetNetCDFAttr(CBratObject* ob, bool withExcept /*= false*/)
{
  CNetCDFAttr* netCDFAttr = dynamic_cast<CNetCDFAttr*>(ob);
  if (withExcept)
  {
    if (netCDFAttr == NULL)
    {
      CException e("CNetCDFAttr::GetNetCDFAttr - dynamic_cast<CNetCDFAttr*>(ob) returns NULL"
                   "object seems not to be an instance of CNetCDFAttr",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }

  return netCDFAttr;

}
//----------------------------------------
CNetCDFAttrDouble* CNetCDFAttr::GetNetCDFAttrDouble(CBratObject* ob, bool withExcept /*= false*/)
{
  CNetCDFAttrDouble* netCDFAttr = dynamic_cast<CNetCDFAttrDouble*>(ob);
  if (withExcept)
  {
    if (netCDFAttr == NULL)
    {
      CException e("CNetCDFAttrDouble::GetNetCDFAttrDouble - dynamic_cast<CNetCDFAttrDouble*>(ob) returns NULL"
                   "object seems not to be an instance of CNetCDFAttrDouble",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }

  return netCDFAttr;

}

//----------------------------------------
CNetCDFAttrFloat* CNetCDFAttr::GetNetCDFAttrFloat(CBratObject* ob, bool withExcept /*= false*/)
{
  CNetCDFAttrFloat* netCDFAttr = dynamic_cast<CNetCDFAttrFloat*>(ob);
  if (withExcept)
  {
    if (netCDFAttr == NULL)
    {
      CException e("CNetCDFAttrFloat::GetNetCDFAttrFloat - dynamic_cast<CNetCDFAttrFloat*>(ob) returns NULL"
                   "object seems not to be an instance of CNetCDFAttrFloat",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }

  return netCDFAttr;

}
//----------------------------------------
CNetCDFAttrInt* CNetCDFAttr::GetNetCDFAttrInt(CBratObject* ob, bool withExcept /*= false*/)
{
  CNetCDFAttrInt* netCDFAttr = dynamic_cast<CNetCDFAttrInt*>(ob);
  if (withExcept)
  {
    if (netCDFAttr == NULL)
    {
      CException e("CNetCDFAttrInt::GetNetCDFAttrInt - dynamic_cast<CNetCDFAttrInt*>(ob) returns NULL"
                   "object seems not to be an instance of CNetCDFAttrInt",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }

  return netCDFAttr;

}
//----------------------------------------
CNetCDFAttrUInt* CNetCDFAttr::GetNetCDFAttrUInt(CBratObject* ob, bool withExcept /*= false*/)
{
  CNetCDFAttrUInt* netCDFAttr = dynamic_cast<CNetCDFAttrUInt*>(ob);
  if (withExcept)
  {
    if (netCDFAttr == NULL)
    {
      CException e("CNetCDFAttrUInt::GetNetCDFAttrUInt - dynamic_cast<CNetCDFAttrUInt*>(ob) returns NULL"
                   "object seems not to be an instance of CNetCDFAttrUInt",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }

  return netCDFAttr;

}
//----------------------------------------
CNetCDFAttrShort* CNetCDFAttr::GetNetCDFAttrShort(CBratObject* ob, bool withExcept /*= false*/)
{
  CNetCDFAttrShort* netCDFAttr = dynamic_cast<CNetCDFAttrShort*>(ob);
  if (withExcept)
  {
    if (netCDFAttr == NULL)
    {
      CException e("CNetCDFAttrShort::GetNetCDFAttrShort - dynamic_cast<CNetCDFAttrShort*>(ob) returns NULL"
                   "object seems not to be an instance of CNetCDFAttrShort",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }

  return netCDFAttr;

}
//----------------------------------------
CNetCDFAttrUShort* CNetCDFAttr::GetNetCDFAttrUShort(CBratObject* ob, bool withExcept /*= false*/)
{
  CNetCDFAttrUShort* netCDFAttr = dynamic_cast<CNetCDFAttrUShort*>(ob);
  if (withExcept)
  {
    if (netCDFAttr == NULL)
    {
      CException e("CNetCDFAttrUShort::GetNetCDFAttrUShort - dynamic_cast<CNetCDFAttrUShort*>(ob) returns NULL"
                   "object seems not to be an instance of CNetCDFAttrUShort",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }

  return netCDFAttr;

}
//----------------------------------------
CNetCDFAttrString* CNetCDFAttr::GetNetCDFAttrString(CBratObject* ob, bool withExcept /*= false*/)
{
  CNetCDFAttrString* netCDFAttr = dynamic_cast<CNetCDFAttrString*>(ob);
  if (withExcept)
  {
    if (netCDFAttr == NULL)
    {
      CException e("CNetCDFAttrString::GetNetCDFAttrString - dynamic_cast<CNetCDFAttrString*>(ob) returns NULL"
                   "object seems not to be an instance of CNetCDFAttrString",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }

  return netCDFAttr;

}
//----------------------------------------
CNetCDFAttrUByte* CNetCDFAttr::GetNetCDFAttrUByte(CBratObject* ob, bool withExcept /*= false*/)
{
  CNetCDFAttrUByte* netCDFAttr = dynamic_cast<CNetCDFAttrUByte*>(ob);
  if (withExcept)
  {
    if (netCDFAttr == NULL)
    {
      CException e("CNetCDFAttrUByte::GetNetCDFAttrUByte - dynamic_cast<CNetCDFAttrUByte*>(ob) returns NULL"
                   "object seems not to be an instance of CNetCDFAttrUByte",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }

  return netCDFAttr;

}

//----------------------------------------
CNetCDFAttrByte* CNetCDFAttr::GetNetCDFAttrByte(CBratObject* ob, bool withExcept /*= false*/)
{
  CNetCDFAttrByte* netCDFAttr = dynamic_cast<CNetCDFAttrByte*>(ob);
  if (withExcept)
  {
    if (netCDFAttr == NULL)
    {
      CException e("CNetCDFAttrByte::GetNetCDFAttrByte - dynamic_cast<CNetCDFAttrByte*>(ob) returns NULL"
                   "object seems not to be an instance of CNetCDFAttrByte",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }

  return netCDFAttr;

}

//----------------------------------------
bool CNetCDFAttr::IsTypeNumeric() 
{

  bool bOk = false;

  switch (m_type)
  {
    case NC_BYTE:	
    case NC_SHORT:
    case NC_INT:
    case NC_FLOAT:
    case NC_DOUBLE:
      bOk = true;
      break;
    default:
      bOk = false;
      break;
  }
  return bOk;
}

//----------------------------------------
bool CNetCDFAttr::IsTypeDateString() 
{


  if (m_type != NC_CHAR)
  {
    return false;
  }

  CNetCDFAttrString* netCDFAttrString = CNetCDFAttr::GetNetCDFAttrString(this);
  if (netCDFAttrString == NULL)
  {
    return false;
  }

  return netCDFAttrString->IsValueDate();
}
//----------------------------------------
bool CNetCDFAttr::IsTypeNumericOrDateString() 
{
  return IsTypeNumeric() || IsTypeDateString();
}

//----------------------------------------
CNetCDFAttr* CNetCDFAttr::NewFillValueAttr(nc_type type) 
{
  CNetCDFAttr* attr = NULL;

  switch (type)
  {
    case NC_BYTE:	
      attr = new CNetCDFAttrByte(FILL_VALUE_ATTR, CTools::m_defaultValueINT8);
      break;
    case NC_CHAR:
      attr = new CNetCDFAttrString(FILL_VALUE_ATTR, "");
      break;
    case NC_SHORT:
      attr = new CNetCDFAttrShort(FILL_VALUE_ATTR, CTools::m_defaultValueINT16);
      break;
    case NC_INT:
      attr = new CNetCDFAttrInt(FILL_VALUE_ATTR, CTools::m_defaultValueINT32);
      break;
    case NC_FLOAT:
      attr = new CNetCDFAttrFloat(FILL_VALUE_ATTR, CTools::m_defaultValueFLOAT);
      break;
    case NC_DOUBLE:
      attr = new CNetCDFAttrDouble(FILL_VALUE_ATTR, CTools::m_defaultValueDOUBLE);
      break;

	  //new

    case NC_UBYTE:	
      attr = new CNetCDFAttrUByte(FILL_VALUE_ATTR, CTools::m_defaultValueUINT8);
      break;
    case NC_USHORT:
      attr = new CNetCDFAttrUShort(FILL_VALUE_ATTR, CTools::m_defaultValueUINT16);
      break;
    case NC_UINT:
      attr = new CNetCDFAttrUInt(FILL_VALUE_ATTR, CTools::m_defaultValueUINT32);
      break;
    case NC_INT64:
      attr = new CNetCDFAttrInt64(FILL_VALUE_ATTR, CTools::m_defaultValueINT64);
      break;
    case NC_UINT64:
      attr = new CNetCDFAttrUInt64(FILL_VALUE_ATTR, CTools::m_defaultValueUINT64);
      break;
    case NC_STRING:
      attr = new CNetCDFAttrString(FILL_VALUE_ATTR, "");
      break;

    default:
      break;
  }
  return attr;
}
//----------------------------------------
CNetCDFAttr* CNetCDFAttr::NewAttr(nc_type type, const std::string& name, size_t length /* = 1 */) 
{
  CNetCDFAttr* attr = NULL;
  
  switch (type)
  {
    case NC_BYTE:	
      attr = new CNetCDFAttrByte(name);
      break;
    case NC_CHAR:
      attr = new CNetCDFAttrString(name);
      break;
    case NC_SHORT:
      attr = new CNetCDFAttrShort(name);
      break;
    case NC_INT:
      attr = new CNetCDFAttrInt(name);
      break;
    case NC_FLOAT:
      attr = new CNetCDFAttrFloat(name);
      break;
    case NC_DOUBLE:
      attr = new CNetCDFAttrDouble(name);
      break;

	  //new

    case NC_UBYTE:	
      attr = new CNetCDFAttrUByte(name);
      break;
    case NC_USHORT:
      attr = new CNetCDFAttrUShort(name);
      break;
    case NC_UINT:
      attr = new CNetCDFAttrUInt(name);
      break;
    case NC_INT64:
      attr = new CNetCDFAttrInt64(name);
      break;
    case NC_UINT64:
      attr = new CNetCDFAttrUInt64(name);
      break;
    case NC_STRING:
      attr = new CNetCDFAttrString(name);
      break;

    default:
      attr = new CNetCDFAttrString(name);
      break;
  }

  attr->SetLength(length);

  return attr;

}
//----------------------------------------

void CNetCDFAttr::DeleteAttribute(int32_t fileId, int32_t varId)
{
  int32_t status	= nc_del_att(fileId, varId, m_name.c_str());

  if ((status != NC_NOERR) && (status != NC_ENOTATT))
  {
    CNetCDFFiles::HandleNetcdfError(status, CTools::Format("Error deleting attribute '%s' of var #%d",
				                                                    m_name.c_str(), varId));
  }

}
//----------------------------------------
void CNetCDFAttr::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CNetCDFAttr Object at "<< this << std::endl;
  
  fOut << "m_globalAttr "<< m_globalAttr << std::endl;
  fOut << "m_length "<< m_length << std::endl;
  fOut << "m_name "<< m_name << std::endl;
  fOut << "m_type "<< m_type << std::endl;
  
  fOut << "==> END Dump a CNetCDFAttr Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CNetCDFAttrString class --------------------
//-------------------------------------------------------------

const std::string CNetCDFAttrString::m_DATE_UNIT = "seconds since 1950-01-01 00:00:00.000000 UTC";

CNetCDFAttrString::CNetCDFAttrString()
{
  Init();
}
//----------------------------------------
CNetCDFAttrString::CNetCDFAttrString(const std::string& name)
  : CNetCDFAttr(name)
{
  Init();
}
//----------------------------------------
CNetCDFAttrString::CNetCDFAttrString(const std::string& name, const std::string& value, bool globalAttr /* = false */)
  : CNetCDFAttr(name, globalAttr)
{
  Init();
  SetValue(value);
}
//----------------------------------------
CNetCDFAttrString::CNetCDFAttrString(const std::string& name, CUnit& unit, bool globalAttr /* = false */)
  : CNetCDFAttr(name, globalAttr)
{
  Init();
  SetValue(unit.GetText());
}
//----------------------------------------
CNetCDFAttrString::CNetCDFAttrString(CNetCDFAttrString& a)
{
  Init();
  Set(a);
}
//----------------------------------------
CNetCDFAttrString::~CNetCDFAttrString()
{
}

//----------------------------------------
void CNetCDFAttrString::Init()
{
  m_type = NC_CHAR;
  m_date.SetDefaultValue();
}

//-------------------------------------------------------------
double CNetCDFAttrString::GetValueDateAsDouble() 
{
  if (m_date.IsDefaultValue())
  {
    return CTools::m_defaultValueDOUBLE;
  }

  return m_date.Value();
}

//-------------------------------------------------------------
void CNetCDFAttrString::Set(CNetCDFAttrString& a) 
{
  CNetCDFAttr::Set(a);
  m_value = a.m_value;
  m_date= a.m_date;
  
}
//----------------------------------------
const CNetCDFAttrString& CNetCDFAttrString::operator =(CNetCDFAttrString& a)
{
  Set(a);    
  return *this;    
}
//----------------------------------------
CBratObject* CNetCDFAttrString::Clone()
{
  return new CNetCDFAttrString(*this);
}
//----------------------------------------
void CNetCDFAttrString::WriteAttribute(int32_t fileId, int32_t varId)
{

  if (m_value.empty())
  {
    DeleteAttribute(fileId, varId); 
    return;
  }

  CNetCDFFiles::HandleNetcdfError( nc_put_att_text(fileId,
				                                           varId,
				                                           m_name.c_str(),
				                                           m_value.length(),
				                                           m_value.c_str()),
                                  CTools::Format("Error deleting attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId));



}
//----------------------------------------
void CNetCDFAttrString::ReadAttribute(int32_t fileId, int32_t varId)
{
  if (m_length < 1)
  {
    return;
  }

  char* attStr = new char[m_length + 1];
  memset(attStr, '\0', m_length + 1); 


  CNetCDFFiles::HandleNetcdfError( nc_get_att_text(fileId, varId, m_name.c_str(), attStr),
                                   CTools::Format("Error readind attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId) );
  attStr[m_length] = '\0';
  CTools::Trim(attStr);
  
  SetValue(attStr);
  
  delete []attStr;
  attStr = NULL;
}
//----------------------------------------
void CNetCDFAttrString::SetValue(const std::string& value) 
{
  m_value = value;
  
  int32_t result = m_date.SetDate(value.c_str());
  
  if(result != BRATHL_SUCCESS) 
  {
    m_date.SetDefaultValue();
  }
  
}
//----------------------------------------
void CNetCDFAttrString::SetValue(const char* value)
{
  std::string str;
  str.assign(value);

  SetValue(str);
}
//----------------------------------------
void CNetCDFAttrString::SetValue(const char* value, size_t length)
{
  std::string str;
  str.assign(value, length);

  SetValue(str);
}

//----------------------------------------
void CNetCDFAttrString::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CNetCDFAttrString Object at "<< this << std::endl;
  CNetCDFAttr::Dump(fOut);
  fOut << "m_value "<< m_value << std::endl;
  m_date.Dump(fOut);
  
  fOut << "==> END Dump a CNetCDFAttrString Object at "<< this << std::endl;

  fOut << std::endl;

}

/*
//-------------------------------------------------------------
//------------------- CNetCDFAttrChar class --------------------
//-------------------------------------------------------------
CNetCDFAttrChar::CNetCDFAttrChar()
{
  Init();
}
//----------------------------------------
CNetCDFAttrChar::CNetCDFAttrChar(const std::string& name)
  : CNetCDFAttr(name)
{
  Init();
}
//----------------------------------------
CNetCDFAttrChar::CNetCDFAttrChar(const std::string& name, const signed char& value, bool globalAttr)
  : CNetCDFAttr(name, globalAttr)
{
  Init();
  SetValue(value);
}
//----------------------------------------
CNetCDFAttrChar::CNetCDFAttrChar(CNetCDFAttrChar& a)
{
  Init();
  Set(a);
}
//----------------------------------------
CNetCDFAttrChar::~CNetCDFAttrChar()
{
}

//----------------------------------------
void CNetCDFAttrChar::Init()
{
  m_type = NC_CHAR;
}
//-------------------------------------------------------------
void CNetCDFAttrChar::Set(CNetCDFAttrChar& a) 
{
  CNetCDFAttr::Set(a);
  m_value = a.m_value;
  
}
//----------------------------------------
const CNetCDFAttrChar& CNetCDFAttrChar::operator =(CNetCDFAttrChar& a)
{
  Set(a);    
  return *this;    
}
//----------------------------------------
CBratObject* CNetCDFAttrChar::Clone()
{
  return new CNetCDFAttrChar(*this);
}
//----------------------------------------
void CNetCDFAttrChar::WriteAttribute(int32_t fileId, int32_t varId)
{

  if (m_value = ' ')
  {
    DeleteAttribute(fileId, varId); 
    return;
  }

  CNetCDFFiles::HandleNetcdfError( nc_put_att_text(fileId,
				                                           varId,
				                                           m_name.c_str(),
				                                           1,
				                                           &m_value),
                                  CTools::Format("Error deleting attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId));



}
//----------------------------------------
void CNetCDFAttrChar::ReadAttribute(int32_t fileId, int32_t varId)
{
  if (m_length != 1)
  {
    return;
  }

  char* attStr = new char[m_length + 1];
  memset(attStr, '\0', m_length + 1); 

  m_value = '\0';

  CNetCDFFiles::HandleNetcdfError( nc_get_att_text(fileId, varId, m_name.c_str(), attStr),
                                   CTools::Format("Error readind attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId) );
  attStr[m_length] = '\0';
  CTools::Trim(attStr);
  
  m_value = attStr[0];
  
  delete []attStr;
  attStr = NULL;
}
*/
//-------------------------------------------------------------
//------------------- CNetCDFAttrUByte class --------------------
//-------------------------------------------------------------
CNetCDFAttrUByte::CNetCDFAttrUByte()
{
  Init();
}
//----------------------------------------
CNetCDFAttrUByte::CNetCDFAttrUByte(const std::string& name)
  : CNetCDFAttr(name)
{
  Init();
}
//----------------------------------------
CNetCDFAttrUByte::CNetCDFAttrUByte(const std::string& name, uint8_t value, bool globalAttr /* = false */)
  : CNetCDFAttr(name, globalAttr)
{
  Init();
  SetValue(value);
}
//----------------------------------------
CNetCDFAttrUByte::CNetCDFAttrUByte(CNetCDFAttrUByte& a)
{
  Init();
  Set(a);
}
//----------------------------------------
CNetCDFAttrUByte::~CNetCDFAttrUByte()
{
}

//----------------------------------------
void CNetCDFAttrUByte::Init()
{
  m_type = NC_CHAR;
}
//-------------------------------------------------------------
void CNetCDFAttrUByte::Set(CNetCDFAttrUByte& a) 
{
  CNetCDFAttr::Set(a);
  m_value = a.m_value;
  
}
//----------------------------------------
const CNetCDFAttrUByte& CNetCDFAttrUByte::operator =(CNetCDFAttrUByte& a)
{
  Set(a);    
  return *this;    
}
//----------------------------------------
CBratObject* CNetCDFAttrUByte::Clone()
{
  return new CNetCDFAttrUByte(*this);
}
//----------------------------------------
void CNetCDFAttrUByte::WriteAttribute(int32_t fileId, int32_t varId)
{
  uint8_t* values = m_value.ToArray();

  CNetCDFFiles::HandleNetcdfError( nc_put_att_uchar(fileId,
				                                           varId,
				                                           m_name.c_str(),
                                                   m_type,
				                                           m_value.size(),
				                                           values),
                                  CTools::Format("Error deleting attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId));


  delete []values;

}

//----------------------------------------
void CNetCDFAttrUByte::ReadAttribute(int32_t fileId, int32_t varId)
{
  if (m_length <= 0)
  {
    return;
  }

  uint8_t* values = new uint8_t[m_length];;

  CNetCDFFiles::HandleNetcdfError( nc_get_att_uchar(fileId, varId, m_name.c_str(), values),
                                   CTools::Format("Error readind attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId) ); 
  SetValue(values, m_length);
  
  delete []values;
  values = NULL;
}
//----------------------------------------
void CNetCDFAttrUByte::SetValue(const CUInt8Array& value)
{
  m_value.RemoveAll();
  m_value = value;
}
//----------------------------------------
void CNetCDFAttrUByte::SetValue(uint8_t* values, size_t length)
{
  m_value.RemoveAll();
  m_value.Insert(values, length);
}
//----------------------------------------
void CNetCDFAttrUByte::SetValue(uint8_t value)
{
  m_value.RemoveAll();
  m_value.Insert(value);
}

//----------------------------------------
void CNetCDFAttrUByte::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CNetCDFAttrUByte Object at "<< this << std::endl;
  CNetCDFAttr::Dump(fOut);
  fOut << "m_value at "<< &m_value << std::endl;
  m_value.Dump(fOut);
  
  fOut << "==> END Dump a CNetCDFAttrUByte Object at "<< this << std::endl;

  fOut << std::endl;

}



//-------------------------------------------------------------
//------------------- CNetCDFAttrDouble class --------------------
//-------------------------------------------------------------
CNetCDFAttrDouble::CNetCDFAttrDouble()
{
  Init();
}
//----------------------------------------
CNetCDFAttrDouble::CNetCDFAttrDouble(const std::string& name)
  : CNetCDFAttr(name)
{
  Init();
}
//----------------------------------------
CNetCDFAttrDouble::CNetCDFAttrDouble(const std::string& name, double value, bool globalAttr /* = false */)
  : CNetCDFAttr(name, globalAttr)
{
  Init();
  SetValue(value);
}
//----------------------------------------
CNetCDFAttrDouble::CNetCDFAttrDouble(CNetCDFAttrDouble& a)
{
  Init();
  Set(a);
}
//----------------------------------------
CNetCDFAttrDouble::~CNetCDFAttrDouble()
{
}

//----------------------------------------
void CNetCDFAttrDouble::Init()
{
  m_type = NC_DOUBLE;
}
//-------------------------------------------------------------
void CNetCDFAttrDouble::Set(CNetCDFAttrDouble& a) 
{
  CNetCDFAttr::Set(a);
  m_value = a.m_value;
  
}
//----------------------------------------
const CNetCDFAttrDouble& CNetCDFAttrDouble::operator =(CNetCDFAttrDouble& a)
{
  Set(a);    
  return *this;    
}
//----------------------------------------
CBratObject* CNetCDFAttrDouble::Clone()
{
  return new CNetCDFAttrDouble(*this);
}
//----------------------------------------
void CNetCDFAttrDouble::WriteAttribute(int32_t fileId, int32_t varId)
{
  double* values = m_value.ToArray();

  CNetCDFFiles::HandleNetcdfError( nc_put_att_double(fileId,
				                                           varId,
				                                           m_name.c_str(),
                                                   m_type,
				                                           m_value.size(),
				                                           values),
                                  CTools::Format("Error deleting attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId));


  delete []values;

}

//----------------------------------------
void CNetCDFAttrDouble::ReadAttribute(int32_t fileId, int32_t varId)
{
  if (m_length <= 0)
  {
    return;
  }

  double* values = new double[m_length];;

  CNetCDFFiles::HandleNetcdfError( nc_get_att_double(fileId, varId, m_name.c_str(), values),
                                   CTools::Format("Error readind attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId) ); 
  SetValue(values, m_length);
  
  delete []values;
  values = NULL;
}

//----------------------------------------
void CNetCDFAttrDouble::SetValue(const CDoubleArray& value)
{
  m_value.RemoveAll();
  m_value = value;
}
//----------------------------------------
void CNetCDFAttrDouble::SetValue(double* values, size_t length)
{
  m_value.RemoveAll();
  m_value.Insert(values, length);
}
//----------------------------------------
void CNetCDFAttrDouble::SetValue(double value)
{
  m_value.RemoveAll();
  m_value.Insert(value);
}

//----------------------------------------
void CNetCDFAttrDouble::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CNetCDFAttrDouble Object at "<< this << std::endl;
  CNetCDFAttr::Dump(fOut);
  fOut << "m_value at "<< &m_value << std::endl;
  m_value.Dump(fOut);
  
  fOut << "==> END Dump a CNetCDFAttrDouble Object at "<< this << std::endl;

  fOut << std::endl;

}


//-------------------------------------------------------------
//------------------- CNetCDFAttrByte class --------------------
//-------------------------------------------------------------
CNetCDFAttrByte::CNetCDFAttrByte()
{
  Init();
}
//----------------------------------------
CNetCDFAttrByte::CNetCDFAttrByte(const std::string& name)
  : CNetCDFAttr(name)
{
  Init();
}
//----------------------------------------
CNetCDFAttrByte::CNetCDFAttrByte(const std::string& name, int8_t value, bool globalAttr /* = false */)
  : CNetCDFAttr(name, globalAttr)
{
  Init();
  SetValue(value);
}
//----------------------------------------
CNetCDFAttrByte::CNetCDFAttrByte(CNetCDFAttrByte& a)
{
  Init();
  Set(a);
}
//----------------------------------------
CNetCDFAttrByte::~CNetCDFAttrByte()
{
}

//----------------------------------------
void CNetCDFAttrByte::Init()
{
  m_type = NC_BYTE;
}
//-------------------------------------------------------------
void CNetCDFAttrByte::Set(CNetCDFAttrByte& a) 
{
  CNetCDFAttr::Set(a);
  m_value = a.m_value;
  
}
//----------------------------------------
const CNetCDFAttrByte& CNetCDFAttrByte::operator =(CNetCDFAttrByte& a)
{
  Set(a);    
  return *this;    
}
//----------------------------------------
CBratObject* CNetCDFAttrByte::Clone()
{
  return new CNetCDFAttrByte(*this);
}
//----------------------------------------
void CNetCDFAttrByte::WriteAttribute(int32_t fileId, int32_t varId)
{

  int8_t* values = m_value.ToArray();

  CNetCDFFiles::HandleNetcdfError( nc_put_att_schar(fileId,
				                                           varId,
				                                           m_name.c_str(),
                                                   m_type,
				                                           m_value.size(),
				                                           values),
                                  CTools::Format("Error deleting attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId));


  delete []values;

}
//----------------------------------------
void CNetCDFAttrByte::ReadAttribute(int32_t fileId, int32_t varId)
{
  if (m_length <= 0)
  {
    return;
  }

  int8_t* values = new int8_t[m_length];;

  CNetCDFFiles::HandleNetcdfError( nc_get_att_schar(fileId, varId, m_name.c_str(), values),
                                   CTools::Format("Error readind attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId) ); 
  SetValue(values, m_length);

  delete []values;
  values = NULL;
}

//----------------------------------------
void CNetCDFAttrByte::SetValue(const CInt8Array& value)
{
  m_value.RemoveAll();
  m_value = value;
}
//----------------------------------------
void CNetCDFAttrByte::SetValue(int8_t* values, size_t length)
{
  m_value.RemoveAll();
  m_value.Insert(values, length);
}
//----------------------------------------
void CNetCDFAttrByte::SetValue(int8_t value)
{
  m_value.RemoveAll();
  m_value.Insert(value);
}
//----------------------------------------
void CNetCDFAttrByte::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CNetCDFAttrByte Object at "<< this << std::endl;
  CNetCDFAttr::Dump(fOut);
  fOut << "m_value at "<< &m_value << std::endl;
  m_value.Dump(fOut);
  
  fOut << "==> END Dump a CNetCDFAttrByte Object at "<< this << std::endl;

  fOut << std::endl;

}



//-------------------------------------------------------------
//------------------- CNetCDFAttrShort class --------------------
//-------------------------------------------------------------
CNetCDFAttrShort::CNetCDFAttrShort()
{
  Init();
}
//----------------------------------------
CNetCDFAttrShort::CNetCDFAttrShort(const std::string& name)
  : CNetCDFAttr(name)
{
  Init();
}
//----------------------------------------
CNetCDFAttrShort::CNetCDFAttrShort(const std::string& name, int16_t value, bool globalAttr /* = false */)
  : CNetCDFAttr(name, globalAttr)
{
  Init();
  SetValue(value);
}
//----------------------------------------
CNetCDFAttrShort::CNetCDFAttrShort(CNetCDFAttrShort& a)
{
  Init();
  Set(a);
}
//----------------------------------------
CNetCDFAttrShort::~CNetCDFAttrShort()
{
}

//----------------------------------------
void CNetCDFAttrShort::Init()
{
  m_type = NC_SHORT;
}

//-------------------------------------------------------------
void CNetCDFAttrShort::Set(CNetCDFAttrShort& a) 
{
  CNetCDFAttr::Set(a);
  m_value = a.m_value;
  
}
//----------------------------------------
const CNetCDFAttrShort& CNetCDFAttrShort::operator =(CNetCDFAttrShort& a)
{
  Set(a);    
  return *this;    
}
//----------------------------------------
CBratObject* CNetCDFAttrShort::Clone()
{
  return new CNetCDFAttrShort(*this);
}
//----------------------------------------
void CNetCDFAttrShort::WriteAttribute(int32_t fileId, int32_t varId)
{

  int16_t* values = m_value.ToArray();

  CNetCDFFiles::HandleNetcdfError( nc_put_att_short(fileId,
				                                           varId,
				                                           m_name.c_str(),
                                                   m_type,
				                                           m_value.size(),
				                                           values),
                                  CTools::Format("Error deleting attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId));


  delete []values;

}
//----------------------------------------
void CNetCDFAttrShort::ReadAttribute(int32_t fileId, int32_t varId)
{
  if (m_length <= 0)
  {
    return;
  }

  int16_t* values = new int16_t[m_length];;

  CNetCDFFiles::HandleNetcdfError( nc_get_att_short(fileId, varId, m_name.c_str(), values),
                                   CTools::Format("Error readind attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId) ); 
  SetValue(values, m_length);
  
  delete []values;
  values = NULL;
}
//----------------------------------------
void CNetCDFAttrShort::SetValue(const CInt16Array& value)
{
  m_value.RemoveAll();
  m_value = value;
}
//----------------------------------------
void CNetCDFAttrShort::SetValue(int16_t* values, size_t length)
{
  m_value.RemoveAll();
  m_value.Insert(values, length);
}
//----------------------------------------
void CNetCDFAttrShort::SetValue(int16_t value)
{
  m_value.RemoveAll();
  m_value.Insert(value);
}

//----------------------------------------
void CNetCDFAttrShort::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CNetCDFAttrShort Object at "<< this << std::endl;
  CNetCDFAttr::Dump(fOut);
  fOut << "m_value at "<< &m_value << std::endl;
  m_value.Dump(fOut);
  
  fOut << "==> END Dump a CNetCDFAttrShort Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CNetCDFAttrUShort class --------------------
//-------------------------------------------------------------
CNetCDFAttrUShort::CNetCDFAttrUShort()
{
  Init();
}
//----------------------------------------
CNetCDFAttrUShort::CNetCDFAttrUShort(const std::string& name)
  : CNetCDFAttr(name)
{
  Init();
}
//----------------------------------------
CNetCDFAttrUShort::CNetCDFAttrUShort(const std::string& name, uint16_t value, bool globalAttr /* = false */)
  : CNetCDFAttr(name, globalAttr)
{
  Init();
  SetValue(value);
}
//----------------------------------------
CNetCDFAttrUShort::CNetCDFAttrUShort(CNetCDFAttrUShort& a)
{
  Init();
  Set(a);
}
//----------------------------------------
CNetCDFAttrUShort::~CNetCDFAttrUShort()
{
}

//----------------------------------------
void CNetCDFAttrUShort::Init()
{
  m_type = NC_USHORT;
}

//-------------------------------------------------------------
void CNetCDFAttrUShort::Set(CNetCDFAttrUShort& a) 
{
  CNetCDFAttr::Set(a);
  m_value = a.m_value;
  
}
//----------------------------------------
const CNetCDFAttrUShort& CNetCDFAttrUShort::operator =(CNetCDFAttrUShort& a)
{
  Set(a);    
  return *this;    
}
//----------------------------------------
CBratObject* CNetCDFAttrUShort::Clone()
{
  return new CNetCDFAttrUShort(*this);
}
//----------------------------------------
void CNetCDFAttrUShort::WriteAttribute(int32_t fileId, int32_t varId)
{

  uint16_t* values = m_value.ToArray();

  CNetCDFFiles::HandleNetcdfError( nc_put_att_ushort(fileId,
				                                           varId,
				                                           m_name.c_str(),
                                                   m_type,
				                                           m_value.size(),
				                                           values),
                                  CTools::Format("Error deleting attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId));


  delete []values;

}
//----------------------------------------
void CNetCDFAttrUShort::ReadAttribute(int32_t fileId, int32_t varId)
{
  if (m_length <= 0)
  {
    return;
  }

  uint16_t* values = new uint16_t[m_length];

  CNetCDFFiles::HandleNetcdfError( nc_get_att_ushort(fileId, varId, m_name.c_str(), values),
                                   CTools::Format("Error readind attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId) ); 
  SetValue(values, m_length);
  
  delete []values;
  values = NULL;
}
//----------------------------------------
void CNetCDFAttrUShort::SetValue(const CUInt16Array& value)
{
  m_value.RemoveAll();
  m_value = value;
}
//----------------------------------------
void CNetCDFAttrUShort::SetValue(uint16_t* values, size_t length)
{
  m_value.RemoveAll();
  m_value.Insert(values, length);
}
//----------------------------------------
void CNetCDFAttrUShort::SetValue(uint16_t value)
{
  m_value.RemoveAll();
  m_value.Insert(value);
}

//----------------------------------------
void CNetCDFAttrUShort::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CNetCDFAttrUShort Object at "<< this << std::endl;
  CNetCDFAttr::Dump(fOut);
  fOut << "m_value at "<< &m_value << std::endl;
  m_value.Dump(fOut);
  
  fOut << "==> END Dump a CNetCDFAttrUShort Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CNetCDFAttrInt class --------------------
//-------------------------------------------------------------
CNetCDFAttrInt::CNetCDFAttrInt()
{
  Init();
}
//----------------------------------------
CNetCDFAttrInt::CNetCDFAttrInt(const std::string& name)
  : CNetCDFAttr(name)
{
  Init();
}
//----------------------------------------
CNetCDFAttrInt::CNetCDFAttrInt(const std::string& name, int32_t value, bool globalAttr /* = false */)
  : CNetCDFAttr(name, globalAttr)
{
  Init();
  SetValue(value);
}
//----------------------------------------
CNetCDFAttrInt::CNetCDFAttrInt(CNetCDFAttrInt& a)
{
  Init();
  Set(a);
}
//----------------------------------------
CNetCDFAttrInt::~CNetCDFAttrInt()
{
}

//----------------------------------------
void CNetCDFAttrInt::Init()
{
  m_type = NC_INT;
}
//-------------------------------------------------------------
void CNetCDFAttrInt::Set(CNetCDFAttrInt& a) 
{
  CNetCDFAttr::Set(a);
  m_value = a.m_value;
  
}
//----------------------------------------
const CNetCDFAttrInt& CNetCDFAttrInt::operator =(CNetCDFAttrInt& a)
{
  Set(a);    
  return *this;    
}
//----------------------------------------
CBratObject* CNetCDFAttrInt::Clone()
{
  return new CNetCDFAttrInt(*this);
}

//----------------------------------------
void CNetCDFAttrInt::WriteAttribute(int32_t fileId, int32_t varId)
{

  int32_t* values = m_value.ToArray();

  CNetCDFFiles::HandleNetcdfError( nc_put_att_int(fileId,
				                                           varId,
				                                           m_name.c_str(),
                                                   m_type,
				                                           m_value.size(),
				                                           values),
                                  CTools::Format("Error deleting attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId));


  delete []values;

}
//----------------------------------------
void CNetCDFAttrInt::ReadAttribute(int32_t fileId, int32_t varId)
{
  if (m_length <= 0)
  {
    return;
  }

  int32_t* values = new int32_t[m_length];;

  CNetCDFFiles::HandleNetcdfError( nc_get_att_int(fileId, varId, m_name.c_str(), values),
                                   CTools::Format("Error readind attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId) ); 
  SetValue(values, m_length);
  
  delete []values;
  values = NULL;
}
//----------------------------------------
void CNetCDFAttrInt::SetValue(const CIntArray& value)
{
  m_value.RemoveAll();
  m_value = value;
}
//----------------------------------------
void CNetCDFAttrInt::SetValue(int32_t* values, size_t length)
{
  m_value.RemoveAll();
  m_value.Insert(values, length);
}
//----------------------------------------
void CNetCDFAttrInt::SetValue(int32_t value)
{
  m_value.RemoveAll();
  m_value.Insert(value);
}

//----------------------------------------
void CNetCDFAttrInt::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CNetCDFAttrInt Object at "<< this << std::endl;
  CNetCDFAttr::Dump(fOut);
  fOut << "m_value at "<< &m_value << std::endl;
  m_value.Dump(fOut);
  
  fOut << "==> END Dump a CNetCDFAttrInt Object at "<< this << std::endl;

  fOut << std::endl;

}
//-------------------------------------------------------------
//------------------- CNetCDFAttrInt64 class --------------------
//-------------------------------------------------------------
CNetCDFAttrInt64::CNetCDFAttrInt64()
{
  Init();
}
//----------------------------------------
CNetCDFAttrInt64::CNetCDFAttrInt64(const std::string& name)
  : CNetCDFAttr(name)
{
  Init();
}
//----------------------------------------
CNetCDFAttrInt64::CNetCDFAttrInt64(const std::string& name, int64_t value, bool globalAttr /* = false */)
  : CNetCDFAttr(name, globalAttr)
{
  Init();
  SetValue(value);
}
//----------------------------------------
CNetCDFAttrInt64::CNetCDFAttrInt64(CNetCDFAttrInt64& a)
{
  Init();
  Set(a);
}
//----------------------------------------
CNetCDFAttrInt64::~CNetCDFAttrInt64()
{
}

//----------------------------------------
void CNetCDFAttrInt64::Init()
{
  m_type = NC_INT64;
}
//-------------------------------------------------------------
void CNetCDFAttrInt64::Set(CNetCDFAttrInt64& a) 
{
  CNetCDFAttr::Set(a);
  m_value = a.m_value;
  
}
//----------------------------------------
const CNetCDFAttrInt64& CNetCDFAttrInt64::operator =(CNetCDFAttrInt64& a)
{
  Set(a);    
  return *this;    
}
//----------------------------------------
CBratObject* CNetCDFAttrInt64::Clone()
{
  return new CNetCDFAttrInt64(*this);
}

//----------------------------------------
void CNetCDFAttrInt64::WriteAttribute(int32_t fileId, int32_t varId)
{

  int64_t* values = m_value.ToArray();

  CNetCDFFiles::HandleNetcdfError( nc_put_att_longlong(fileId,
				                                           varId,
				                                           m_name.c_str(),
                                                   m_type,
				                                           m_value.size(),
                                                           (long long*)values),             //cast or linux; max and windows ok
                                  CTools::Format("Error deleting attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId));


  delete []values;

}
//----------------------------------------
void CNetCDFAttrInt64::ReadAttribute(int32_t fileId, int32_t varId)
{
  if (m_length <= 0)
  {
    return;
  }

  int64_t* values = new int64_t[m_length];;

  CNetCDFFiles::HandleNetcdfError( nc_get_att_longlong(fileId,
                                                       varId,
                                                       m_name.c_str(),
                                                       (long long*)values),             //cast or linux; max and windows ok
                                   CTools::Format("Error readind attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId) ); 
  SetValue(values, m_length);
  
  delete []values;
  values = NULL;
}
//----------------------------------------
void CNetCDFAttrInt64::SetValue(const CInt64Array& value)
{
  m_value.RemoveAll();
  m_value = value;
}
//----------------------------------------
void CNetCDFAttrInt64::SetValue(int64_t* values, size_t length)
{
  m_value.RemoveAll();
  m_value.Insert(values, length);
}
//----------------------------------------
void CNetCDFAttrInt64::SetValue(int64_t value)
{
  m_value.RemoveAll();
  m_value.Insert(value);
}

//----------------------------------------
void CNetCDFAttrInt64::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CNetCDFAttrInt64 Object at "<< this << std::endl;
  CNetCDFAttr::Dump(fOut);
  fOut << "m_value at "<< &m_value << std::endl;
  m_value.Dump(fOut);
  
  fOut << "==> END Dump a CNetCDFAttrInt64 Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CNetCDFAttrUInt class --------------------
//-------------------------------------------------------------
CNetCDFAttrUInt::CNetCDFAttrUInt()
{
  Init();
}
//----------------------------------------
CNetCDFAttrUInt::CNetCDFAttrUInt(const std::string& name)
  : CNetCDFAttr(name)
{
  Init();
}
//----------------------------------------
CNetCDFAttrUInt::CNetCDFAttrUInt(const std::string& name, uint32_t value, bool globalAttr /* = false */)
  : CNetCDFAttr(name, globalAttr)
{
  Init();
  SetValue(value);
}
//----------------------------------------
CNetCDFAttrUInt::CNetCDFAttrUInt(CNetCDFAttrUInt& a)
{
  Init();
  Set(a);
}
//----------------------------------------
CNetCDFAttrUInt::~CNetCDFAttrUInt()
{
}

//----------------------------------------
void CNetCDFAttrUInt::Init()
{
  m_type = NC_UINT;
}
//-------------------------------------------------------------
void CNetCDFAttrUInt::Set(CNetCDFAttrUInt& a) 
{
  CNetCDFAttr::Set(a);
  m_value = a.m_value;
  
}
//----------------------------------------
const CNetCDFAttrUInt& CNetCDFAttrUInt::operator =(CNetCDFAttrUInt& a)
{
  Set(a);    
  return *this;    
}
//----------------------------------------
CBratObject* CNetCDFAttrUInt::Clone()
{
  return new CNetCDFAttrUInt(*this);
}

//----------------------------------------
void CNetCDFAttrUInt::WriteAttribute(int32_t fileId, int32_t varId)
{

  uint32_t* values = m_value.ToArray();

  CNetCDFFiles::HandleNetcdfError( nc_put_att_uint(fileId,
				                                           varId,
				                                           m_name.c_str(),
                                                   m_type,
				                                           m_value.size(),
				                                           values),
                                  CTools::Format("Error deleting attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId));


  delete []values;

}
//----------------------------------------
void CNetCDFAttrUInt::ReadAttribute(int32_t fileId, int32_t varId)
{
  if (m_length <= 0)
  {
    return;
  }

  uint32_t* values = new uint32_t[m_length];

  CNetCDFFiles::HandleNetcdfError( nc_get_att_uint(fileId, varId, m_name.c_str(), values),
                                   CTools::Format("Error readind attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId) ); 
  SetValue(values, m_length);
  
  delete []values;
  values = NULL;
}
//----------------------------------------
void CNetCDFAttrUInt::SetValue(const CUIntArray& value)
{
  m_value.RemoveAll();
  m_value = value;
}
//----------------------------------------
void CNetCDFAttrUInt::SetValue(uint32_t* values, size_t length)
{
  m_value.RemoveAll();
  m_value.Insert(values, length);
}
//----------------------------------------
void CNetCDFAttrUInt::SetValue(uint32_t value)
{
  m_value.RemoveAll();
  m_value.Insert(value);
}

//----------------------------------------
void CNetCDFAttrUInt::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CNetCDFAttrUInt Object at "<< this << std::endl;
  CNetCDFAttr::Dump(fOut);
  fOut << "m_value at "<< &m_value << std::endl;
  m_value.Dump(fOut);
  
  fOut << "==> END Dump a CNetCDFAttrUInt Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CNetCDFAttrUInt64 class --------------------
//-------------------------------------------------------------
CNetCDFAttrUInt64::CNetCDFAttrUInt64()
{
  Init();
}
//----------------------------------------
CNetCDFAttrUInt64::CNetCDFAttrUInt64(const std::string& name)
  : CNetCDFAttr(name)
{
  Init();
}
//----------------------------------------
CNetCDFAttrUInt64::CNetCDFAttrUInt64(const std::string& name, uint64_t value, bool globalAttr /* = false */)
  : CNetCDFAttr(name, globalAttr)
{
  Init();
  SetValue(value);
}
//----------------------------------------
CNetCDFAttrUInt64::CNetCDFAttrUInt64(CNetCDFAttrUInt64& a)
{
  Init();
  Set(a);
}
//----------------------------------------
CNetCDFAttrUInt64::~CNetCDFAttrUInt64()
{
}

//----------------------------------------
void CNetCDFAttrUInt64::Init()
{
  m_type = NC_UINT64;
}
//-------------------------------------------------------------
void CNetCDFAttrUInt64::Set(CNetCDFAttrUInt64& a) 
{
  CNetCDFAttr::Set(a);
  m_value = a.m_value;
  
}
//----------------------------------------
const CNetCDFAttrUInt64& CNetCDFAttrUInt64::operator =(CNetCDFAttrUInt64& a)
{
  Set(a);    
  return *this;    
}
//----------------------------------------
CBratObject* CNetCDFAttrUInt64::Clone()
{
  return new CNetCDFAttrUInt64(*this);
}

//----------------------------------------
void CNetCDFAttrUInt64::WriteAttribute(int32_t fileId, int32_t varId)
{

  uint64_t* values = m_value.ToArray();

  CNetCDFFiles::HandleNetcdfError( nc_put_att_ulonglong(fileId,
				                                           varId,
				                                           m_name.c_str(),
                                                   m_type,
				                                           m_value.size(),
                                                        (unsigned long long*)values),             //cast or linux; max and windows ok
                                  CTools::Format("Error deleting attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId));


  delete []values;

}
//----------------------------------------
void CNetCDFAttrUInt64::ReadAttribute(int32_t fileId, int32_t varId)
{
  if (m_length <= 0)
  {
    return;
  }

  uint64_t* values = new uint64_t[m_length];

  CNetCDFFiles::HandleNetcdfError( nc_get_att_ulonglong(fileId,
                                                        varId,
                                                        m_name.c_str(),
                                                        (unsigned long long*)values),             //cast or linux; max and windows ok
                                   CTools::Format("Error readind attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId) ); 
  SetValue(values, m_length);
  
  delete []values;
  values = NULL;
}
//----------------------------------------
void CNetCDFAttrUInt64::SetValue(const CUInt64Array& value)
{
  m_value.RemoveAll();
  m_value = value;
}
//----------------------------------------
void CNetCDFAttrUInt64::SetValue(uint64_t* values, size_t length)
{
  m_value.RemoveAll();
  m_value.Insert(values, length);
}
//----------------------------------------
void CNetCDFAttrUInt64::SetValue(uint64_t value)
{
  m_value.RemoveAll();
  m_value.Insert(value);
}

//----------------------------------------
void CNetCDFAttrUInt64::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CNetCDFAttrUInt64 Object at "<< this << std::endl;
  CNetCDFAttr::Dump(fOut);
  fOut << "m_value at "<< &m_value << std::endl;
  m_value.Dump(fOut);
  
  fOut << "==> END Dump a CNetCDFAttrUInt64 Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CNetCDFAttrFloat class --------------------
//-------------------------------------------------------------
CNetCDFAttrFloat::CNetCDFAttrFloat()
{
  Init();
}
//----------------------------------------
CNetCDFAttrFloat::CNetCDFAttrFloat(const std::string& name)
  : CNetCDFAttr(name)
{
  Init();
}
//----------------------------------------
CNetCDFAttrFloat::CNetCDFAttrFloat(const std::string& name, float value, bool globalAttr /* = false */)
  : CNetCDFAttr(name, globalAttr)
{
  Init();
  SetValue(value);
}
//----------------------------------------
CNetCDFAttrFloat::CNetCDFAttrFloat(CNetCDFAttrFloat& a)
{
  Init();
  Set(a);
}
//----------------------------------------
CNetCDFAttrFloat::~CNetCDFAttrFloat()
{
}

//----------------------------------------
void CNetCDFAttrFloat::Init()
{
  m_type = NC_FLOAT;
}
//-------------------------------------------------------------
void CNetCDFAttrFloat::Set(CNetCDFAttrFloat& a) 
{
  CNetCDFAttr::Set(a);
  m_value = a.m_value;
  
}
//----------------------------------------
const CNetCDFAttrFloat& CNetCDFAttrFloat::operator =(CNetCDFAttrFloat& a)
{
  Set(a);    
  return *this;    
}
//----------------------------------------
CBratObject* CNetCDFAttrFloat::Clone()
{
  return new CNetCDFAttrFloat(*this);
}
//----------------------------------------
void CNetCDFAttrFloat::WriteAttribute(int32_t fileId, int32_t varId)
{

  float* values = m_value.ToArray();

  CNetCDFFiles::HandleNetcdfError( nc_put_att_float(fileId,
				                                           varId,
				                                           m_name.c_str(),
                                                   m_type,
				                                           m_value.size(),
				                                           values),
                                  CTools::Format("Error deleting attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId));


  delete []values;

}
//----------------------------------------
void CNetCDFAttrFloat::ReadAttribute(int32_t fileId, int32_t varId)
{
  if (m_length <= 0)
  {
    return;
  }

  float* values = new float[m_length];;

  CNetCDFFiles::HandleNetcdfError( nc_get_att_float(fileId, varId, m_name.c_str(), values),
                                   CTools::Format("Error readind attribute '%s' of var #%d",
				                                          m_name.c_str(),
				                                          varId) ); 
  SetValue(values, m_length);
  
  delete []values;
  values = NULL;
}
//----------------------------------------
void CNetCDFAttrFloat::SetValue(const CFloatArray& value)
{
  m_value.RemoveAll();
  m_value = value;
}
//----------------------------------------
void CNetCDFAttrFloat::SetValue(float* values, size_t length)
{
  m_value.RemoveAll();
  m_value.Insert(values, length);
}
//----------------------------------------
void CNetCDFAttrFloat::SetValue(float value)
{
  m_value.RemoveAll();
  m_value.Insert(value);
}


//----------------------------------------
void CNetCDFAttrFloat::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CNetCDFAttrFloat Object at "<< this << std::endl;
  CNetCDFAttr::Dump(fOut);
  fOut << "m_value at "<< &m_value << std::endl;
  m_value.Dump(fOut);
  
  fOut << "==> END Dump a CNetCDFAttrFloat Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CNetCDFDimension class --------------------
//----------------------------------------
CNetCDFDimension::CNetCDFDimension()
{
  Init();
}
//----------------------------------------
CNetCDFDimension::CNetCDFDimension(const std::string& name, int32_t length, bool isUnlimited /* = false */)
{
  Init();
  SetName(name);
  SetLength(length);
  SetIsUnlimited(isUnlimited);
}
//----------------------------------------
CNetCDFDimension::CNetCDFDimension(const std::string& name, CNetCDFDimension& from)
{
  Init();
  Set(from);
  SetName(name);
}
//----------------------------------------
CNetCDFDimension::CNetCDFDimension(CNetCDFDimension& d)
{
  Init();
  Set(d);
}
//----------------------------------------
CNetCDFDimension::~CNetCDFDimension()
{
  m_coordVars.RemoveAll();
}

//----------------------------------------
void CNetCDFDimension::Init()
{
  m_dimId = -1;
  m_length = -1;
  m_isUnlimited = false;
}
//----------------------------------------
CBratObject* CNetCDFDimension::Clone()
{
  return new CNetCDFDimension(*this);
}

//-------------------------------------------------------------
void CNetCDFDimension::Set(CNetCDFDimension& d) 
{
  SetName(d.m_name);
  SetLength(d.m_length);
  SetIsUnlimited(d.m_isUnlimited);
  SetDimId(d.m_dimId);

  m_coordVars = d.m_coordVars;

}
//----------------------------------------
const CNetCDFDimension& CNetCDFDimension::operator =(CNetCDFDimension& d)
{
  Set(d);    
  return *this;    
}


//----------------------------------------
void CNetCDFDimension::AddCoordinateVariable(CNetCDFVarDef* v)
{
  AddCoordinateVariable(*v);
}
//----------------------------------------
void CNetCDFDimension::AddCoordinateVariable(CNetCDFVarDef& v)
{

  if (!IsCoordinateVariable(v))
  {
    m_coordVars.Insert(v.GetName());
  }

}
//----------------------------------------
void CNetCDFDimension::AddCoordinateVariable(const std::string& name)
{

  if (!IsCoordinateVariable(name))
  {
    m_coordVars.Insert(name);
  }

}

//----------------------------------------
bool CNetCDFDimension::IsCoordinateVariable(const std::string& name)
{

  bool bOk = false;
  CStringArray::iterator it;

  for (it = m_coordVars.begin() ; it !=  m_coordVars.end() ; it++)
  {
    if (str_icmp(*it, name))
    {
      bOk = true;
      break;
    }
  }

  return bOk;
}

//----------------------------------------
bool CNetCDFDimension::IsCoordinateVariable(CNetCDFVarDef& cv)
{
  return IsCoordinateVariable(cv.GetName());

}
//----------------------------------------
bool CNetCDFDimension::IsCoordinateVariable(CNetCDFVarDef* cv)
{
  return IsCoordinateVariable(*cv);

}
//----------------------------------------
void CNetCDFDimension::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CNetCDFDimension Object at "<< this << std::endl;
  fOut << "m_coordVars at "<< &m_coordVars << std::endl;
  m_coordVars.Dump(fOut);
  fOut << "m_dimId "<< m_dimId << std::endl;
  fOut << "m_isUnlimited "<< m_isUnlimited << std::endl;
  fOut << "m_length "<< m_length << std::endl;
  fOut << "m_name "<< m_name << std::endl;

  fOut << "==> END Dump a CNetCDFDimension Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CNetCDFVarDef class --------------------
//-------------------------------------------------------------
//----------------------------------------
CNetCDFVarDef::CNetCDFVarDef(nc_type type /* = NC_DOUBLE */)
{
  Init();
  SetType(type);
}
//----------------------------------------
CNetCDFVarDef::CNetCDFVarDef(const std::string& name, nc_type type /* = NC_DOUBLE */)
{
  Init();
  SetName(name);
  SetType(type);
}
//----------------------------------------
CNetCDFVarDef::CNetCDFVarDef(const std::string& name, const CUnit& unit, nc_type type /* = NC_DOUBLE */)
{
  Init();
  SetName(name);
  SetUnit(unit);
  SetType(type);
}
//----------------------------------------
CNetCDFVarDef::CNetCDFVarDef(const std::string& name, const std::string& unit, nc_type type /* = NC_DOUBLE */)
{
  Init();
  SetName(name);
  SetUnit(unit);
  SetType(type);
}
//----------------------------------------

//CNetCDFVarDef::CNetCDFVarDef(const std::string& name, const CNetCDFDimension& dim, nc_type type /* = NC_DOUBLE */)
/*{
  Init();
  SetName(name);
  SetType(type);

  AddNetCDFDim(dim);
}
*/
//----------------------------------------
CNetCDFVarDef::CNetCDFVarDef(CNetCDFVarDef& v)
{
  Init();
  Set(v);
}
//----------------------------------------
CNetCDFVarDef::~CNetCDFVarDef()
{
  m_mapAttributes.RemoveAll();
}
//----------------------------------------
void CNetCDFVarDef::Init()
{
  m_varId = -1;
  m_type = NC_DOUBLE;
  setDefaultValue(m_validMin);
  setDefaultValue(m_validMax);
  
  m_scaleFactor = 1.0;
  m_addOffset = 0.0;
  setDefaultValue(m_fillValue);

}
//----------------------------------------
CNetCDFVarDef* CNetCDFVarDef::GetNetCDFVarDef(CBratObject* ob, bool withExcept /*= false*/)
{
  CNetCDFVarDef* netCDFVarDef = dynamic_cast<CNetCDFVarDef*>(ob);
  if (withExcept)
  {
    if (netCDFVarDef == NULL)
    {
      CException e("CNetCDFFiles::GetNetCDFVarDef - dynamic_cast<CNetCDFVarDef*>(ob) returns NULL"
                   "object seems not to be an instance of CNetCDFVarDef",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return netCDFVarDef;

}
//----------------------------------------
bool CNetCDFVarDef::NetcdfVarCompareByName(CBratObject* o1, CBratObject* o2)
{
  CNetCDFVarDef* v1 = CNetCDFVarDef::GetNetCDFVarDef(o1);
  CNetCDFVarDef* v2 = CNetCDFVarDef::GetNetCDFVarDef(o2);
  if ((v1 == NULL) || (v2 == NULL))
  {
    return false;
  }

  return (v1->GetName().compare(v2->GetName()) < 0); 
}

//-------------------------------------------------------------
bool CNetCDFVarDef::IsCoordinateAxis(CNetCDFVarDef* var) 
{
  if (var == NULL)
  {
    return false;
  }
  
  return var->IsCoordinateAxis();
}
//-------------------------------------------------------------
bool CNetCDFVarDef::IsCoordinateAxis() 
{
    return (dynamic_cast<CNetCDFCoordinateAxis*>(this) != NULL);
}
//-------------------------------------------------------------
void CNetCDFVarDef::SetType(nc_type value) 
{
  m_type = value;
 
  AddFillValueAttr();
}
//-------------------------------------------------------------
void CNetCDFVarDef::AddFillValueAttr() 
{
  CNetCDFAttr* attr = CNetCDFAttr::NewFillValueAttr(m_type);
  
  AddAttribute(attr);

};

//-------------------------------------------------------------
void CNetCDFVarDef::Set(CNetCDFVarDef& v) 
{
  
  m_name = v.m_name;
  m_type = v.m_type;
  m_unit = v.m_unit;

  m_varId = v.m_varId;

  m_validMin = v.m_validMin;
  m_validMax = v.m_validMax;

  m_mapAttributes.RemoveAll();
  m_mapAttributes = v.m_mapAttributes;

  m_dims.RemoveAll();
  m_dims.Insert(v.GetDims());

  m_netCDFdims.RemoveAll();
  m_netCDFdims = v.m_netCDFdims;
}
//----------------------------------------
const CNetCDFVarDef& CNetCDFVarDef::operator =(CNetCDFVarDef& v)
{
  Set(v);    
  return *this;    
}
//----------------------------------------
CBratObject* CNetCDFVarDef::Clone()
{
  return new CNetCDFVarDef(*this);
}

//----------------------------------------
std::string CNetCDFVarDef::GetLongName()
{
  std::string longName;
  CNetCDFAttrString* netCDFAttr = CNetCDFAttr::GetNetCDFAttrString(GetAttribute(LONG_NAME_ATTR));
  if (netCDFAttr != NULL)
  {
    return netCDFAttr->GetValue();
  }

  return "";
}
//----------------------------------------
void CNetCDFVarDef::AddAttributes(const CStringMap& mapAttributes)
{
  CStringMap::const_iterator it;
  for (it = mapAttributes.begin() ; it != mapAttributes.end() ; it++)
  {
    AddAttribute(new CNetCDFAttrString(it->first, it->second));
  }
}

//----------------------------------------
void CNetCDFVarDef::SetNetCDFDims(const CStringArray* dimNames, const CUIntArray* dimValues, const CIntArray* dimIds /*= NULL*/)
{
  if (dimNames == NULL)
  {
    return;
  }
  if (dimValues == NULL)
  {
    return;
  }

  size_t size = dimNames->size();
  if (dimValues->size() != size)
  {
    return;
  }

  if (dimIds != NULL)
  {
    if (dimIds->size() != size)
    {
      return;
    }
  }

  for (uint32_t i = 0 ; i < size ; i++)
  {
    CNetCDFDimension netCDFDim(dimNames->at(i), dimValues->at(i));
    
    if (dimIds != NULL)
    {
      netCDFDim.SetDimId(dimIds->at(i));
    }

    AddNetCDFDim(netCDFDim);
  }
}

/*
//----------------------------------------
void CNetCDFVarDef::SetNetCDFDims(const CUIntMap& dimsMap)
{
  CUIntMap::const_iterator it;
  for (it = dimsMap.begin() ; it != dimsMap.end() ; it++)
  {
    AddNetCDFDim(new CNetCDFDimension(it->first, it->second));
  }
}
*/
//----------------------------------------
void CNetCDFVarDef::AddAttribute(CNetCDFAttr* attr)
{
  if (attr == NULL)
  {
    return;
  }
  m_mapAttributes.Erase(attr->GetName());
  m_mapAttributes.Insert(attr->GetName(), attr);
}

//----------------------------------------
CNetCDFAttr* CNetCDFVarDef::GetAttribute(const std::string& name)
{
  return CNetCDFAttr::GetNetCDFAttr(m_mapAttributes.Exists(name));
}

//----------------------------------------
bool CNetCDFVarDef::HasCompatibleUnit(CNetCDFVarDef* netCDFVarDef)
{
  return m_unit.IsCompatible(netCDFVarDef->GetUnit());
}

//----------------------------------------
void CNetCDFVarDef::SetDims(const CUIntArray& dims)
{
  m_dims.clear();
  m_dims.Insert(dims);
}
//----------------------------------------
void CNetCDFVarDef::AddDim(uint32_t dim)
{
  m_dims.Insert(dim);
}
//----------------------------------------
void CNetCDFVarDef::SetDim(uint32_t dim, uint32_t index)
{
  if (m_dims.size() <= index)
  {
    m_dims.resize(index + 1);
  }
  m_dims[index] = dim;
}
//----------------------------------------
void CNetCDFVarDef::InsertDim(uint32_t dim, uint32_t index)
{

  if (m_dims.size() <= index)
  {
    m_dims.resize(index + 1);
    m_dims[index] = dim;
  }
  else
  {
    CUIntArray::iterator itInsertAt = m_dims.begin() + index;
    m_dims.InsertAt(itInsertAt, dim);
  }
 

}
//----------------------------------------
int32_t CNetCDFVarDef::GetNbDims()
{

  if (m_dims.size() != m_netCDFdims.size())
  {
    throw CException(CTools::Format("ERROR in  CNetCDFFiles::GetNbDims - Variable %s "
                                    "- Inconsistency dimension arrays (shape array size = %ld - definition array size = %ld)", 
                                    this->GetName().c_str(), 
                                    (long)m_dims.size(),
                                    (long)m_netCDFdims.size()),
                     BRATHL_LOGIC_ERROR);

  }

  return m_dims.size();
}
//----------------------------------------
long CNetCDFVarDef::GetNbData()
{
  long nbData = 1;

  if (m_dims.size() <= 0 )
  {
    return nbData;
  }


  nbData = m_dims.GetProductValues();

  return nbData;
}

//----------------------------------------
void CNetCDFVarDef::AddNetCDFDims(CObArray& netCDFdims)
{  
  CObArray::const_iterator it;
  for (it = netCDFdims.begin() ; it != netCDFdims.end() ; it++)
  {
    CNetCDFDimension* netCDFdim = dynamic_cast<CNetCDFDimension*>(*it);
    if (netCDFdim == NULL)
    {
      continue;
    }

    AddNetCDFDim(*netCDFdim);

  }
}
/*
//----------------------------------------
CNetCDFDimension* CNetCDFVarDef::AddNetCDFDim(CNetCDFDimension* dim)
{  
  if (dim == NULL)
  {
    return NULL;
  }

  return SetNetCDFDim(*dim, m_netCDFdims.size());
}
*/
//----------------------------------------
CNetCDFDimension* CNetCDFVarDef::AddNetCDFDim(CNetCDFDimension& dim)
{  
  return SetNetCDFDim(dim, m_netCDFdims.size());
}
//----------------------------------------
CNetCDFDimension* CNetCDFVarDef::SetNetCDFDim(CNetCDFDimension& dim, uint32_t index)
{
  CNetCDFDimension* newDim = NULL;

  if (m_netCDFdims.size() <= index)
  {
    m_netCDFdims.resize(index + 1);
  }
  else
  {
    newDim = dynamic_cast<CNetCDFDimension*>(m_netCDFdims.at(index));
  }

  if (newDim != NULL)
  {
    delete newDim;
    newDim = NULL;
  }

  newDim = new CNetCDFDimension(dim);
  m_netCDFdims[index] = newDim;

  SetDim(newDim->GetLength(), index);

  return newDim;

}

//----------------------------------------
CNetCDFDimension* CNetCDFVarDef::ReplaceNetCDFDim(CNetCDFDimension& dim)
{

  CNetCDFDimension* newDim = GetNetCDFDim(dim.GetName());

  if (newDim == NULL)
  {
    return newDim;
  }

  uint32_t range = GetNetCDFDimRange(dim.GetName());

  if (isDefaultValue(range))
  {
    return NULL;
  }

  *newDim = dim;

  SetDim(newDim->GetLength(), range);

  return newDim;

}
//----------------------------------------
CNetCDFDimension* CNetCDFVarDef::InsertNetCDFDim(CNetCDFDimension& dim, uint32_t index)
{

  CNetCDFDimension* newDim = new CNetCDFDimension(dim);
  

  if (index >= m_netCDFdims.size())
  {
    m_netCDFdims.resize(index + 1);
    m_netCDFdims[index] = newDim;
  }
  else
  {
    CObArray::iterator itInsertAt = m_netCDFdims.begin() + index;
    m_netCDFdims.InsertAt(itInsertAt, newDim);
  }

  InsertDim(newDim->GetLength(), index);
  
  return newDim;

}
//----------------------------------------
CNetCDFDimension* CNetCDFVarDef::GetNetCDFDim(const std::string& name)
{
  CNetCDFDimension* dimFound = NULL;

  CObArray::const_iterator it;
  for (it = m_netCDFdims.begin() ; it != m_netCDFdims.end() ; it++)
  {
    CNetCDFDimension* dim = dynamic_cast<CNetCDFDimension*>(*it);
    if (dim == NULL)
    {
      continue;
    }
    if (*dim == name)
    {
      dimFound = dim;
      break;
    }
  }

  return dimFound;
}
//----------------------------------------
CNetCDFDimension* CNetCDFVarDef::GetNetCDFDim(uint32_t index)
{
  CNetCDFDimension* dim = NULL;

  if (m_netCDFdims.size() > index)
  {
    dim = dynamic_cast<CNetCDFDimension*>(m_netCDFdims.at(index));
  }

  return dim;
}

//----------------------------------------
uint32_t CNetCDFVarDef::GetNetCDFDimRange(const std::string& name)
{
  CNetCDFDimension* dim = NULL;
  uint32_t range;
  
  setDefaultValue(range);

  for (uint32_t i = 0 ; i < m_netCDFdims.size() ; i++)
  {
    
    dim = GetNetCDFDim(i);

    if (dim == NULL)
    {
      continue;
    }
    if (*dim == name)
    {
      range = i;
      break;
    }
  }

  return range;
}
//----------------------------------------
void CNetCDFVarDef::GetNetCdfDimNames(CStringArray& dimNames) const
{
  
  CObArray::const_iterator it;
  
  for ( it = m_netCDFdims.begin(); it != m_netCDFdims.end(); it++ )
  {
    CNetCDFDimension* netCDFDim = dynamic_cast<CNetCDFDimension*>(*it);
    if (netCDFDim == NULL)
    {
      continue;
    }

    dimNames.InsertUnique( netCDFDim->GetName() );

  }

}
//----------------------------------------
void CNetCDFVarDef::GetNetCdfDimIds(CIntArray& dimId) const
{
  
  CObArray::const_iterator it;
  
  for ( it = m_netCDFdims.begin(); it != m_netCDFdims.end(); it++ )
  {
    CNetCDFDimension* netCDFDim = dynamic_cast<CNetCDFDimension*>(*it);
    if (netCDFDim == NULL)
    {
      continue;
    }

    dimId.Insert( netCDFDim->GetDimId() );

  }

}

//----------------------------------------
bool CNetCDFVarDef::HasCommonDims(const std::string& dimName)
{
  CStringArray intersect;
  
  GetCommomDims(dimName, intersect);

  return (intersect.size() > 0);

}

//----------------------------------------
bool CNetCDFVarDef::HasCommonDims(const CStringArray& dimNames)
{
  CStringArray intersect;
  
  GetCommomDims(dimNames, intersect);

  return (intersect.size() > 0);
}

//----------------------------------------
void CNetCDFVarDef::GetCommomDims(const std::string& dimName, CStringArray& intersect)
{
  CStringArray dimNames;
  dimNames.Insert(dimName);

  GetCommomDims(dimNames, intersect);

}
//----------------------------------------
void CNetCDFVarDef::GetCommomDims(const CStringArray& dimNames, CStringArray& intersect)
{
  CStringArray varDimNames;
  GetNetCdfDimNames(varDimNames);

  varDimNames.Intersect(dimNames, intersect);
}

//----------------------------------------
void CNetCDFVarDef::GetComplementDims(const std::string& dimName, CStringArray& complement)
{
  CStringArray dimNames;
  dimNames.Insert(dimName);

  GetComplementDims(dimNames, complement);

}
//----------------------------------------
void CNetCDFVarDef::GetComplementDims(const CStringArray& dimNames, CStringArray& complement)
{
  CStringArray varDimNames;
  GetNetCdfDimNames(varDimNames);

  dimNames.Complement(varDimNames, complement);
}
//----------------------------------------
bool CNetCDFVarDef::HaveEqualDimSizes(CNetCDFVarDef* netCDFVarDef, std::string* msg /*= NULL*/)
{
  bool bOk = true;
  CObArray* netCDFDims = netCDFVarDef->GetNetCDFDims();

  CObArray* thisDims = this->GetNetCDFDims();

  if (thisDims->size() != netCDFDims->size())
  {
    if (msg != NULL)
    {
      msg->append(CTools::Format("\tFields '%s' and '%s' have not the same number of dimensions (%ld vs %ld)\n",
                                this->GetName().c_str(), 
                                netCDFVarDef->GetName().c_str(), 
                                (long)thisDims->size(),
                                (long)netCDFDims->size()
                                ));
    }
    bOk = false;
  }

  return bOk;
}

//----------------------------------------
bool CNetCDFVarDef::HaveEqualDims(CNetCDFVarDef* netCDFVarDef, CNetCDFFiles* netCDFFiles1 /*= NULL*/, CNetCDFFiles* netCDFFiles2 /*= NULL*/, std::string* msg /*= NULL*/)
{
  if (netCDFVarDef == NULL)
  {
    return false;
  }

  if (! HaveEqualDimSizes(netCDFVarDef, msg))
  {
    return false;
  }

  CObArray* netCDFDims = netCDFVarDef->GetNetCDFDims();

  CObArray* thisDims = this->GetNetCDFDims();

  bool sameDims = true;

  size_t nbDims = thisDims->size();

  for (size_t i = 0 ; i < nbDims ; i++)
  {
    CNetCDFDimension* thisDim = dynamic_cast<CNetCDFDimension*>(thisDims->at(i));
    CNetCDFDimension* netCDFDim = dynamic_cast<CNetCDFDimension*>(netCDFDims->at(i));
    
    if (netCDFDim == NULL)
    {
      if (msg != NULL)
      {
        msg->append(CTools::Format("\tField '%s' has no dimension at index '%d'\n",
                                  netCDFDim->GetName().c_str(), 
                                  i+1
                                  ));
      }
      sameDims = false;
      continue;
    }

    if (thisDim == NULL)
    {
      if (msg != NULL)
      {
        msg->append(CTools::Format("\tField '%s' has no dimension at index '%d'\n",
                                  this->GetName().c_str(), 
                                  i+1
                                  ));
      }
      sameDims = false;
      continue;
    }

    if ( ! thisDim->IsEqual(*netCDFDim) )
    {
      if (msg != NULL)
      {
        msg->append(CTools::Format("\tFields '%s' and '%s' have not the same dimension's name or length at index %d. (dimension '%s' vs '%s', length %d vs %d)\n",
                                  this->GetName().c_str(), 
                                  netCDFVarDef->GetName().c_str(), 
                                  i+1,
                                  thisDim->GetName().c_str(),
                                  netCDFDim->GetName().c_str(),
                                  thisDim->GetLength(),
                                  netCDFDim->GetLength()
                                  ));
      }
      sameDims = false;
      continue;
    }

    if ( (netCDFFiles1 == NULL) && (netCDFFiles2 == NULL) )
    {
      continue;
    }

    if ( (netCDFFiles1 != NULL) && (netCDFFiles2 == NULL) )
    {
      netCDFFiles2 = netCDFFiles1;
    }

    if ( (netCDFFiles1 == NULL) && (netCDFFiles2 != NULL) )
    {
      netCDFFiles1 = netCDFFiles2;
    }

    CDoubleArray thisDimValues;
    CDoubleArray netCDFDimValues;

    netCDFFiles1->ReadEntireVar(thisDim, thisDimValues, CUnit::m_UNIT_SI);
    netCDFFiles2->ReadEntireVar(netCDFDim, netCDFDimValues, CUnit::m_UNIT_SI);

    if (thisDimValues != netCDFDimValues)
    {
      if (msg != NULL)
      {
        msg->append(CTools::Format("\tFields '%s' and '%s' have not the same dimension's values at index %d. (dimension '%s' vs '%s', length %d vs %d)\n",
                                  this->GetName().c_str(), 
                                  netCDFVarDef->GetName().c_str(), 
                                  i+1,
                                  thisDim->GetName().c_str(),
                                  netCDFDim->GetName().c_str(),
                                  thisDim->GetLength(),
                                  netCDFDim->GetLength()
                                  ));
      }
      sameDims = false;
      continue;
    }

  }

  return sameDims;

}
//----------------------------------------
bool CNetCDFVarDef::HaveEqualDimNames(CNetCDFVarDef* netCDFVarDef, std::string* msg /*= NULL*/)
{
  if (netCDFVarDef == NULL)
  {
    return false;
  }

  if (! HaveEqualDimSizes(netCDFVarDef, msg))
  {
    return false;
  }

  CObArray* netCDFDims = netCDFVarDef->GetNetCDFDims();

  CObArray* thisDims = this->GetNetCDFDims();

  bool sameDims = true;

  size_t nbDims = thisDims->size();

  for (uint32_t i = 0 ; i < nbDims ; i++)
  {
    CNetCDFDimension* thisDim = dynamic_cast<CNetCDFDimension*>(thisDims->at(i));
    CNetCDFDimension* netCDFDim = dynamic_cast<CNetCDFDimension*>(netCDFDims->at(i));
    
    if (netCDFDim == NULL)
    {
      if (msg != NULL)
      {
        msg->append(CTools::Format("\tField '%s' has no dimension at index '%d'\n",
                                  netCDFDim->GetName().c_str(), 
                                  i+1
                                  ));
      }
      sameDims = false;
      continue;
    }

    if (thisDim == NULL)
    {
      if (msg != NULL)
      {
        msg->append(CTools::Format("\tField '%s' has no dimension at index '%d'\n",
                                  this->GetName().c_str(), 
                                  i+1
                                  ));
      }
      sameDims = false;
      continue;
    }


    if ( *thisDim != *netCDFDim)
    {
      if (msg != NULL)
      {
        msg->append(CTools::Format("\tFields '%s' and '%s' have not the same dimension's name at index %d. (dimension '%s' vs '%s')\n",
                                  this->GetName().c_str(), 
                                  netCDFVarDef->GetName().c_str(), 
                                  i+1,
                                  thisDim->GetName().c_str(),
                                  netCDFDim->GetName().c_str()
                                  ));
      }
      sameDims = false;
      continue;
    }
  }

  return sameDims;

}

//----------------------------------------
bool CNetCDFVarDef::HaveCompatibleDimUnits(CNetCDFVarDef* netCDFVarDef, CNetCDFFiles* netCDFFiles1, CNetCDFFiles* netCDFFiles2, std::string* msg /*= NULL*/)
{

  if ( (netCDFFiles1 == NULL) && (netCDFFiles2 == NULL) )
  {
    return false;
  }

  if ( (netCDFFiles1 != NULL) && (netCDFFiles2 == NULL) )
  {
    netCDFFiles2 = netCDFFiles1;
  }

  if ( (netCDFFiles1 == NULL) && (netCDFFiles2 != NULL) )
  {
    netCDFFiles1 = netCDFFiles2;
  }

  CObArray thisCoordAxes;
  CObArray netCDFCoordAxes;

  netCDFFiles1->GetNetCDFCoordAxes(this->GetName(), &thisCoordAxes);
  netCDFFiles2->GetNetCDFCoordAxes(netCDFVarDef->GetName(), &netCDFCoordAxes);

  if (thisCoordAxes.size() != netCDFCoordAxes.size())
  {
    return false;
  }


  bool bOk = true;

  size_t nbDims = thisCoordAxes.size();

  for (uint32_t i = 0 ; i < nbDims ; i++)
  {
    CNetCDFCoordinateAxis* thisAxis = dynamic_cast<CNetCDFCoordinateAxis*>(thisCoordAxes.at(i));
    CNetCDFCoordinateAxis* netCDFAxis = dynamic_cast<CNetCDFCoordinateAxis*>(netCDFCoordAxes.at(i));

    if (netCDFAxis == NULL)
    {
      if (msg != NULL)
      {
        msg->append(CTools::Format("\tField '%s' has no dimension at index '%d'\n",
                                  netCDFVarDef->GetName().c_str(), 
                                  i+1
                                  ));
      }
      bOk = false;
      break;
    }

    if (thisAxis == NULL)
    {
      if (msg != NULL)
      {
        msg->append(CTools::Format("\tField '%s' has no dimension at index '%d'\n",
                                  this->GetName().c_str(), 
                                  i+1
                                  ));
      }
      bOk = false;
      break;
    }


    if ( ! thisAxis->HasCompatibleUnit(netCDFAxis))
    {
      if (msg != NULL)
      {
        msg->append(CTools::Format("\tFields '%s' and '%s' have incompatible dimension's units at index %d. (dimension '%s' vs '%s', unit %s vs %s)\n",
                                  this->GetName().c_str(), 
                                  netCDFVarDef->GetName().c_str(), 
                                  i+1,
                                  thisAxis->GetName().c_str(),
                                  netCDFAxis->GetName().c_str(),
                                  thisAxis->GetUnit()->GetText().c_str(),
                                  netCDFAxis->GetUnit()->GetText().c_str()
                                  ));
      }
      bOk = false;
      break;
    }
  }

  return bOk;

}
//----------------------------------------
void CNetCDFVarDef::WriteAttributes(int32_t fileId)
{

  CObMap::const_iterator it;
  for (it = m_mapAttributes.begin() ; it != m_mapAttributes.end(); it++)
  {
    CNetCDFAttr* attr = CNetCDFAttr::GetNetCDFAttr(it->second);
    if (attr == NULL)
    {
      continue;
    }
    
    attr->WriteAttribute(fileId, m_varId);

  }

}
//----------------------------------------
void CNetCDFVarDef::WriteData(int32_t fileId, CMatrix* matrix)
{
  CMatrixDouble* matrixDouble = dynamic_cast<CMatrixDouble*>(matrix);
  CMatrixDoublePtr* matrixDoublePtr = dynamic_cast<CMatrixDoublePtr*>(matrix);

  if (matrixDouble != NULL)
  {
    WriteData(fileId, matrixDouble);
  }
  else if (matrixDoublePtr != NULL)
  {
    WriteData(fileId, matrixDoublePtr);
  }

}
//----------------------------------------
void CNetCDFVarDef::WriteData(int32_t fileId, CMatrixDoublePtr* matrix)
{

  //this->Dump(*CTrace::GetInstance()->GetDumpContext());
  
  //matrix->GetMatrixDataDimIndexes()->Dump(*CTrace::GetInstance()->GetDumpContext());
  
  //std::cout << matrix->GetXName() << matrix->GetYName() << std::endl;

  long nbData = GetNbData();
  size_t nbDataToWrite = matrix->GetNumberOfValues();

  if (nbData != nbDataToWrite)
  {
    throw CException(CTools::Format("Error in CNetCDFVarDef::WriteData - Defined dimension of variable '%s' : %ld does not correspond to number of values to write : %ld",
				                                m_name.c_str(),
					                              (long)nbData,
					                              (long)nbDataToWrite),
			                    BRATHL_LOGIC_ERROR);  
  }

 
  if (m_varId < 0)
  {
    throw CException(CTools::Format("Error in CNetCDFVarDef::WriteData - Variable Id has not been set. Variable ('%s') have to be added before writing data.",
				                                m_name.c_str()),
			                    BRATHL_LOGIC_ERROR);  
  }

  bool conversion = !(isDefaultValue(m_fillValue) && (m_scaleFactor == 1.0) && (m_addOffset == 0.0));


  size_t nRows = matrix->GetNumberOfRows();
  size_t nCols = matrix->GetNumberOfCols();

  if (conversion)
  {
    matrix->ScaleDownData(m_scaleFactor, m_addOffset, m_fillValue);
  }

  uint32_t indexXDim = this->GetNetCDFDimRange(matrix->GetXName());
  uint32_t indexYDim = this->GetNetCDFDimRange(matrix->GetYName());


  if (isDefaultValue(indexXDim))
  {
    throw CException(CTools::Format("Error in CNetCDFVarDef::WriteData - Dimension range of field %s not found. "
                                    "Variable is %s. Matrix name is %s.",
                                      matrix->GetXName().c_str(),
				                             m_name.c_str(),
                                     matrix->GetName().c_str()),
			                    BRATHL_LOGIC_ERROR);  

  }
  if (isDefaultValue(indexYDim))
  {
    throw CException(CTools::Format("Error in CNetCDFVarDef::WriteData - Dimension range of field %s not found. "
                                    "Variable is %s. Matrix name is %s.",
                                      matrix->GetYName().c_str(),
				                             m_name.c_str(),
                                     matrix->GetName().c_str()),
			                    BRATHL_LOGIC_ERROR);  

  }

  CStringArray* matrixDataDimIndexes = matrix->GetMatrixDataDimIndexes();

  uint32_t nbDims = this->GetNbDims();

  size_t *ncstart = new size_t[nbDims];
  
  memset(ncstart, 0, sizeof(*ncstart) * nbDims);

  size_t *nccount = new size_t[nbDims];
  memset(nccount, 0, sizeof(*nccount) * nbDims);
  
  nccount[indexXDim] = 1;
  nccount[indexYDim] = 1;

  uint32_t indexDimOther = 0;

  CStringArray::const_iterator it;
  for (it = matrixDataDimIndexes->begin() ; it != matrixDataDimIndexes->end() ; it++)
  {
    indexDimOther = this->GetNetCDFDimRange(*it);

    if (isDefaultValue(indexDimOther))
    {
      throw CException(CTools::Format("Error in CNetCDFVarDef::WriteData - Dimension range of field %s not found. "
                                      "Variable is %s. Matrix name is %s.",
                                        it->c_str(),
				                               m_name.c_str(),
                                       matrix->GetName().c_str()),
			                      BRATHL_LOGIC_ERROR);  

    }
    
    if ( (indexDimOther == indexXDim) || (indexDimOther == indexYDim) )
    {
      throw CException(CTools::Format("Error in CNetCDFVarDef::WriteData - Inconsistency dimension range of field %s. "
                                      "Variable is %s. Matrix name is %s."
                                      "X index is %d, Y index is %d, and %s range %d is common to either X or Y range.",
                                       it->c_str(),
				                               m_name.c_str(),
                                       matrix->GetName().c_str(),
                                       indexXDim,
                                       indexYDim,
                                       it->c_str(),
                                       indexDimOther),
			                      BRATHL_LOGIC_ERROR);  

    }
    
    nccount[indexDimOther] = this->GetNetCDFDim(indexDimOther)->GetLength();

    
  }



  for ( uint32_t row = 0 ; row < nRows ; row++ )
  {
    for ( uint32_t col = 0 ; col < nCols ; col++ )
    {
      memset(ncstart, 0, sizeof(*ncstart) * nbDims);

      ncstart[indexXDim] = row;
      ncstart[indexYDim] = col;

      DoublePtr matrixData = matrix->At(row, col);
      if ( matrixData == NULL)
      {
        continue;
      }

      CNetCDFFiles::HandleNetcdfError(nc_put_vara_double(fileId, m_varId, ncstart, nccount, matrixData ),
		                   CTools::Format("Error writing variable %s at row/col %ld/%ld",
				                              m_name.c_str(),
                                      (long)ncstart[0],
                                      (long)ncstart[1]));
    }
    

  }

  delete []ncstart;
  ncstart = NULL;
  delete []nccount;
  nccount = NULL;
}
//----------------------------------------
void CNetCDFVarDef::WriteData(int32_t fileId, CMatrixDouble* matrix)
{
  long nbData = GetNbData();
  size_t nbDataToWrite = matrix->GetNumberOfValues();

  if (nbData != nbDataToWrite)
  {
    throw CException(CTools::Format("Error in CNetCDFVarDef::WriteData - Defined dimension of variable '%s' : %ld does not correspond to number of values to write : %ld",
				                                m_name.c_str(),
					                              nbData,
					                              nbDataToWrite),
			                    BRATHL_LOGIC_ERROR);  
  }

  if (m_varId < 0)
  {
    throw CException(CTools::Format("Error in CNetCDFVarDef::WriteData - Variable Id has not been set. Variable ('%s') have to be added before writing data.",
				                                m_name.c_str()),
			                    BRATHL_LOGIC_ERROR);  
  }

  bool conversion = !(isDefaultValue(m_fillValue) && (m_scaleFactor == 1.0) && (m_addOffset == 0.0));

  size_t nRows = matrix->GetNumberOfRows();
  size_t nCols = matrix->GetNumberOfCols();

  if (conversion)
  {
    matrix->ScaleDownData(m_scaleFactor, m_addOffset, m_fillValue);
  }


  uint32_t indexXDim = this->GetNetCDFDimRange(matrix->GetXName());
  uint32_t indexYDim = this->GetNetCDFDimRange(matrix->GetYName());

  if (isDefaultValue(indexXDim))
  {
    throw CException(CTools::Format("Error in CNetCDFVarDef::WriteData - Dimension range of field %s not found. "
                                    "Variable is %s. Matrix name is %s.",
                                      matrix->GetXName().c_str(),
				                             m_name.c_str(),
                                     matrix->GetName().c_str()),
			                    BRATHL_LOGIC_ERROR);  

  }
  if (isDefaultValue(indexYDim))
  {
    throw CException(CTools::Format("Error in CNetCDFVarDef::WriteData - Dimension range of field %s not found. "
                                    "Variable is %s. Matrix name is %s.",
                                      matrix->GetYName().c_str(),
				                             m_name.c_str(),
                                     matrix->GetName().c_str()),
			                    BRATHL_LOGIC_ERROR);  

  }

  uint32_t nbDims = this->GetNbDims();

  size_t *ncstart = new size_t[nbDims];
  
  memset(ncstart, 0, sizeof(*ncstart) * nbDims);

  size_t *nccount = new size_t[nbDims];
  memset(nccount, 0, sizeof(*nccount) * nbDims);
  
  nccount[indexXDim] = 1;
  nccount[indexYDim] = nCols;



  for ( uint32_t row = 0 ; row < nRows ; row++ )
  {
    memset(ncstart, 0, sizeof(*ncstart) * nbDims);
    
    ncstart[indexXDim] = row;

    CNetCDFFiles::HandleNetcdfError(nc_put_vara_double(fileId, m_varId, ncstart, nccount, &((*matrix)[row].front()) ),
		                 CTools::Format("Error writing variable %s at row %ld",
				                            m_name.c_str(),
                                    (long)ncstart[0]));
    

  }

  delete []ncstart;
  ncstart = NULL;
  delete []nccount;
  nccount = NULL;

}

//----------------------------------------
void CNetCDFVarDef::WriteData(int32_t fileId, CExpressionValue* values)
{

  long nbData = GetNbData();
  size_t nbDataToWrite = values->GetNbValues();

  if (nbData != nbDataToWrite)
  {
    throw CException(CTools::Format("Error in CNetCDFVarDef::WriteData - Defined dimension of variable '%s' : %ld does not correspond to number of values to write : %ld",
				                                m_name.c_str(),
					                              (long)nbData,
					                              (long)nbDataToWrite),
			                    BRATHL_LOGIC_ERROR);  
  }

  if (m_varId < 0)
  {
    throw CException(CTools::Format("Error in CNetCDFVarDef::WriteData - Variable Id has not been set. Variable ('%s') have to be added before writing data.",
				                                m_name.c_str()),
			                    BRATHL_LOGIC_ERROR);  
  }

  bool conversion = !(isDefaultValue(m_fillValue) && (m_scaleFactor == 1.0) && (m_addOffset == 0.0));
  
  double *source	= values->GetValues();
  double *dest	= values->GetValues();

  if (conversion)
  {
    CUIntArray* dims = GetDims();
    CExpressionValue	varValues(FloatType, *dims, NULL);
    dest	= varValues.GetValues();

    size_t nbValues	= varValues.GetNbValues();

    for (uint32_t index = 0 ; index < nbValues ; index++)
    {
      if (isDefaultValue(source[index]))
      {
	      dest[index]	= m_fillValue;
      }
      else
      {
	      dest[index]	= (source[index] - m_addOffset) / m_scaleFactor;
      }
    }

  }

  CNetCDFFiles::HandleNetcdfError(nc_put_var_double(fileId, m_varId, dest),
		               CTools::Format("Error writing variable %s",
				                          m_name.c_str()));


}

/*
//----------------------------------------
void CNetCDFVarDef::SetFillValue(char value)
{ 
  m_fillValue = static_cast<double>(value); 
  AddAttribute(new CNetCDFAttrChar(FILL_VALUE_ATTR, m_fillValue));
}
*/
//----------------------------------------
void CNetCDFVarDef::SetFillValue(int8_t value)
{ 
  m_fillValue = static_cast<double>(value); 
  AddAttribute(new CNetCDFAttrByte(FILL_VALUE_ATTR, value));
}
//----------------------------------------
void CNetCDFVarDef::SetFillValue(uint8_t value)
{
  m_fillValue = static_cast<double>(value); 
  AddAttribute(new CNetCDFAttrUByte(FILL_VALUE_ATTR, value));
}
//----------------------------------------
void CNetCDFVarDef::SetFillValue(int16_t value)
{ 
  m_fillValue = static_cast<double>(value); 
  AddAttribute(new CNetCDFAttrShort(FILL_VALUE_ATTR, value));
}
//----------------------------------------
void CNetCDFVarDef::SetFillValue(uint16_t value)
{ 
  m_fillValue = static_cast<double>(value); 
  AddAttribute(new CNetCDFAttrUShort(FILL_VALUE_ATTR, value));
}
//----------------------------------------
void CNetCDFVarDef::SetFillValue(int32_t value)
{ 
  m_fillValue = static_cast<double>(value); 
  AddAttribute(new CNetCDFAttrInt(FILL_VALUE_ATTR, value));
}
//----------------------------------------
void CNetCDFVarDef::SetFillValue(uint32_t value)
{ 
  m_fillValue = static_cast<double>(value); 
  AddAttribute(new CNetCDFAttrUInt(FILL_VALUE_ATTR, value));
}
//----------------------------------------
void CNetCDFVarDef::SetFillValue(float value)
{ 
  m_fillValue = static_cast<double>(value); 
  AddAttribute(new CNetCDFAttrFloat(FILL_VALUE_ATTR, value));
}
//----------------------------------------
void CNetCDFVarDef::SetFillValue(double value)
{ 
  m_fillValue = value; 
  AddAttribute(new CNetCDFAttrDouble(FILL_VALUE_ATTR, value));
}
//----------------------------------------
void CNetCDFVarDef::SetUnit(const CUnit& value)
{ 
  m_unit = value; 
  AddAttribute(new CNetCDFAttrString(UNITS_ATTR, m_unit));
}
//----------------------------------------
void CNetCDFVarDef::SetUnit(const std::string& value)
{ 
  m_unit = value; 
  AddAttribute(new CNetCDFAttrString(UNITS_ATTR, m_unit));
}

//----------------------------------------
void CNetCDFVarDef::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CNetCDFVarDef Object at "<< this << std::endl;
  fOut << "m_addOffset "<< m_addOffset << std::endl;
  fOut << "m_dims at "<< &m_dims << std::endl;
  m_dims.Dump(fOut);
  fOut << "m_fillValue "<< m_fillValue << std::endl;
  fOut << "m_mapAttributes at "<< &m_mapAttributes << std::endl;
  m_mapAttributes.Dump(fOut);
  fOut << "m_name "<< m_name << std::endl;
  fOut << "m_netCDFdims at "<< &m_netCDFdims << std::endl;
  m_netCDFdims.Dump(fOut);
  fOut << "m_scaleFactor "<< m_scaleFactor << std::endl;
  fOut << "m_type "<< m_type << std::endl;
  fOut << "m_unit at "<< &m_unit << std::endl;
  m_unit.Dump(fOut);
  fOut << "m_validMin "<< m_validMin << std::endl;
  fOut << "m_validMax "<< m_validMax << std::endl;
  fOut << "m_varId "<< m_varId << std::endl;

  fOut << "==> END Dump a CNetCDFVarDef Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CNetCDFCoordinateAxis class --------------------
//-------------------------------------------------------------
//----------------------------------------
CNetCDFCoordinateAxis::CNetCDFCoordinateAxis(NetCDFVarKind dimKind /* = Unknown */, nc_type type /* = NC_DOUBLE */)
                : CNetCDFVarDef(type)
{
  Init();
  SetDimKind(dimKind);
}
//----------------------------------------
CNetCDFCoordinateAxis::CNetCDFCoordinateAxis(const std::string& name, NetCDFVarKind dimKind /* = Unknown */, nc_type type /* = NC_DOUBLE */)
        : CNetCDFVarDef(name, type)
{
  Init();
  SetDimKind(dimKind);
}
//----------------------------------------
CNetCDFCoordinateAxis::CNetCDFCoordinateAxis(const std::string& name, const CUnit& unit, NetCDFVarKind dimKind /* = Unknown */, nc_type type /* = NC_DOUBLE */)
        : CNetCDFVarDef(name, unit, type)
{
  Init();
  SetDimKind(dimKind);
}
//----------------------------------------
CNetCDFCoordinateAxis::CNetCDFCoordinateAxis(const std::string& name, const std::string& unit, NetCDFVarKind dimKind /* = Unknown */, nc_type type /* = NC_DOUBLE */)
        : CNetCDFVarDef(name, unit, type)
{
  Init();
  SetDimKind(dimKind);
}
//----------------------------------------
//CNetCDFCoordinateAxis::CNetCDFCoordinateAxis(const std::string& name, CNetCDFDimension* dim, NetCDFVarKind dimKind /* = Unknown */, nc_type type /* = NC_DOUBLE */)
/*        : CNetCDFVarDef(name, dim, type)
{
  Init();
  SetDimKind(dimKind);
}
*/
//----------------------------------------
CNetCDFCoordinateAxis::CNetCDFCoordinateAxis(CNetCDFVarDef& v)
{
  Init();
  Set(v);
}
//----------------------------------------
CNetCDFCoordinateAxis::CNetCDFCoordinateAxis(CNetCDFCoordinateAxis& v)
{
  Init();
  Set(v);
}
//----------------------------------------
CNetCDFCoordinateAxis::~CNetCDFCoordinateAxis()
{
}
//----------------------------------------
void CNetCDFCoordinateAxis::Init()
{
  m_dimKind = Unknown;
}

//-------------------------------------------------------------
void CNetCDFCoordinateAxis::Set(CNetCDFCoordinateAxis& v) 
{
  CNetCDFVarDef::Set(v);

  m_dimKind = v.m_dimKind;
  m_dimType = v.m_dimType;
  m_axis = v.m_axis;  
}
//-------------------------------------------------------------
void CNetCDFCoordinateAxis::Set(CNetCDFVarDef& v) 
{
  CNetCDFVarDef::Set(v);

  m_dimKind = Unknown;
  m_dimType.empty();
  m_axis.empty();  
}
//----------------------------------------
const CNetCDFCoordinateAxis& CNetCDFCoordinateAxis::operator =(CNetCDFCoordinateAxis& v)
{
  Set(v);    
  return *this;    
}
//----------------------------------------
const CNetCDFCoordinateAxis& CNetCDFCoordinateAxis::operator =(CNetCDFVarDef& v)
{
  Set(v);    
  return *this;    
}
//----------------------------------------
CBratObject* CNetCDFCoordinateAxis::Clone()
{
  return new CNetCDFCoordinateAxis(*this);
}

//----------------------------------------
void CNetCDFCoordinateAxis::SetDimKind(NetCDFVarKind dimKind)
{
  m_dimKind = dimKind;

  switch (m_dimKind)
  {
    //--------------
    case X:
    //--------------
		  m_dimType	= "";
		  m_axis	= "X";
		  break;
    //--------------
    case Y:
    //--------------
		  m_dimType	= "";
		  m_axis	= "Y";
		  break;
    //--------------
    case Z:
    //--------------
		  m_dimType	= "vertical";
		  m_axis	= "Z";
		  break;
    //--------------
    case T:
    //--------------
		  m_dimType	= "time";
		  m_axis	= "T";
		  break;
    //--------------
    case Latitude:
    //--------------
		  m_dimType	= "latitude";
		  m_axis	= "Y";
		  break;
    //--------------
    case Longitude:
    //--------------
		  m_dimType	= "longitude";
		  m_axis	= "X";
		  break;
    //--------------
    default:
    //--------------
		  break;
  }
}

//----------------------------------------
void CNetCDFCoordinateAxis::WriteAttributes(int32_t fileId)
{

  if (!m_dimType.empty())
  {
    AddAttribute(new CNetCDFAttrString(STANDARD_NAME_ATTR, m_dimType));
  }

  if (!m_axis.empty())
  {
    AddAttribute(new CNetCDFAttrString(AXIS_ATTR, m_axis));
  }


  //-------------------------------
  CNetCDFVarDef::WriteAttributes(fileId);
  //-------------------------------

  WriteMinMaxGobalAttr(fileId);

}
//----------------------------------------
void CNetCDFCoordinateAxis::WriteMinMaxGobalAttr(int32_t fileId)
{
  WriteMinMaxGobalAttr(fileId, true);
  WriteMinMaxGobalAttr(fileId, false);
}
//----------------------------------------
void CNetCDFCoordinateAxis::WriteMinMaxGobalAttr(int32_t fileId, bool wantMin)
{
  CNetCDFAttr *attr = NULL;
  std::string name = m_name;
  double value;

  if (wantMin)
  {
    name.append("_min");
    value = m_validMin;
  }
  else
  {
    name.append("_max");
    value = m_validMax;
  }

  
  if (!isDefaultValue(value))
  {
    //if (m_dimKind == T)
    if (m_unit.IsDate())
    {
      m_unit.SetConversionToBaseUnit();

      CDate date(m_unit.Convert(value));          
      
      attr = new CNetCDFAttrString(name, date.AsString("", true), true);
      
    }
    else
    {
      attr = new CNetCDFAttrDouble(name, m_unit.ConvertToBaseUnit(value), true);
    }

  }

  if (attr != NULL)
  {
    attr->WriteAttribute(fileId);
    delete attr;
    attr = NULL;
  }
  
}


//----------------------------------------
/*
void CNetCDFCoordinateAxis::GetDimType(NetCDFVarKind dimKind, std::string& dimType, std::string& axis)
{
  CNetCDFCoordinateAxis var(dimKind);

  dimType = var.GetDimType();
  axis = var.GetAxis();
}
*/
//----------------------------------------
CNetCDFCoordinateAxis* CNetCDFCoordinateAxis::GetNetCDFCoordAxis(CBratObject* ob, bool withExcept /*= false*/)
{
  CNetCDFCoordinateAxis* netCDFVarDef = dynamic_cast<CNetCDFCoordinateAxis*>(ob);
  if (withExcept)
  {
    if (netCDFVarDef == NULL)
    {
      CException e("CNetCDFFiles::GetNetCDFCoordAxis - dynamic_cast<CNetCDFCoordinateAxis*>(ob) returns NULL"
                   "object seems not to be an instance of CNetCDFCoordinateAxis",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return netCDFVarDef;

}



//----------------------------------------
void CNetCDFCoordinateAxis::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CNetCDFCoordinateAxis Object at "<< this << std::endl;
  CNetCDFVarDef::Dump(fOut);
  fOut << "m_axis "<< m_axis << std::endl;
  fOut << "m_dimKind "<< m_dimKind << std::endl;
  fOut << "m_dimType "<< m_dimType << std::endl;

  fOut << "==> END Dump a CNetCDFCoordinateAxis Object at "<< this << std::endl;

  fOut << std::endl;

}
//-------------------------------------------------------------
//------------------- CNetCDFFiles class --------------------
//-------------------------------------------------------------

//----------------------------------------
CNetCDFFiles::CNetCDFFiles ()
{
  Init();
}

//----------------------------------------
CNetCDFFiles::CNetCDFFiles (const std::string& name, brathl_FileMode	mode /*= ReadOnly*/)
{
  Init();

  m_fileName = name;
  m_fileMode = mode;
  Open();
}

//----------------------------------------
CNetCDFFiles::~CNetCDFFiles()
{
  Close();

}
//----------------------------------------
void CNetCDFFiles::Init()
{
	m_fileMode = ReadOnly;
  m_file = -1;
  m_nDims = -1;
  m_nVars = -1;
  m_nGlobalAttrs = -1;
  m_unlimitedDimId = -1;

}

//----------------------------------------
CNetCDFDimension* CNetCDFFiles::GetNetCDFDim(const std::string& name)
{
  return dynamic_cast<CNetCDFDimension*>(m_mapNetCDFDims.Exists(name));
}
//----------------------------------------
void CNetCDFFiles::GetNetCDFDims(const std::string& varName, CObArray* dims)
{
  
  if (dims == NULL)
  {
    return;
  }
  
  dims->SetDelete(false);

  CObMap::const_iterator it;
  for (it = m_mapNetCDFDims.begin() ; it != m_mapNetCDFDims.end() ; it++)
  {
    CNetCDFDimension* netCDFDim = dynamic_cast<CNetCDFDimension*>(it->second);
    if (netCDFDim == NULL)
    {
      continue;
    }

    if (netCDFDim->IsCoordinateVariable(varName))
    {
      dims->Insert(netCDFDim);
    }
  }

}

//----------------------------------------
void CNetCDFFiles::GetNetCDFCoordAxes(const std::string& varName, CObArray* coordAxes)
{
  if (coordAxes == NULL)
  {
    return;
  }
  
  coordAxes->SetDelete(false);

  CObMap mapCoordAxes;
  
  GetNetCDFCoordAxes(varName, &mapCoordAxes);

  mapCoordAxes.ToArray(*coordAxes);

}
//----------------------------------------
void CNetCDFFiles::GetNetCDFCoordAxes(const std::string& varName, CObMap* coordAxes)
{
  
  if (coordAxes == NULL)
  {
    return;
  }
  
  coordAxes->SetDelete(false);

  CNetCDFVarDef* netCDFVarDef = GetNetCDFVarDef(varName);

  if (netCDFVarDef == NULL)
  {
    return;
  }

  CObArray* dims = netCDFVarDef->GetNetCDFDims();

  if (dims == NULL)
  {
    return;
  }

  CObArray::const_iterator it;
  for (it = dims->begin() ; it != dims->end() ; it++)
  {
    CNetCDFDimension* netCDFDim = dynamic_cast<CNetCDFDimension*>(*it);
    if (netCDFDim == NULL)
    {
      continue;
    }
    
    CNetCDFVarDef* coordAxis = GetNetCDFVarDef(netCDFDim->GetName());
    if (coordAxis == NULL)
    {
      continue;
    }

    if (CNetCDFVarDef::IsCoordinateAxis(coordAxis))
    {
      coordAxes->Insert(netCDFDim->GetName(), coordAxis);
    }
  }

}

//----------------------------------------
void CNetCDFFiles::GetNetCDFCoordAxes(const std::string& varName, CStringArray* coordAxes, 
                                      bool bRemoveAll /* = true */, bool bUnique /* = false */)
{
  if (coordAxes == NULL)
  {
    return;
  }
  
  CObMap mapCoordAxes;
  
  GetNetCDFCoordAxes(varName, &mapCoordAxes);

  mapCoordAxes.GetKeys(*coordAxes, bRemoveAll, bUnique);



}


//----------------------------------------
void CNetCDFFiles::GetNetCDFCoordAxes(const CStringArray& varNames, CStringArray* coordAxes, 
                                      bool bRemoveAll /* = true */, bool bUnique /* = false */)
{

  CStringArray::const_iterator it;
  
  for (it = varNames.begin() ; it != varNames.end() ; it++)
  {
    GetNetCDFCoordAxes(*it, coordAxes, bRemoveAll, bUnique);
  }

}


//----------------------------------------
void CNetCDFFiles::GetNetCDFCoordAxes(CObArray* coordAxes)
{
  
  if (coordAxes == NULL)
  {
    return;
  }
  
  coordAxes->SetDelete(false);

  CObMap mapCoordAxes;
  
  GetNetCDFCoordAxes(&mapCoordAxes);

  mapCoordAxes.ToArray(*coordAxes);

}
//----------------------------------------
void CNetCDFFiles::GetNetCDFCoordAxes(CObMap* coordAxes)
{
  
  if (coordAxes == NULL)
  {
    return;
  }
  
  coordAxes->SetDelete(false);

  CObMap::const_iterator it;
  for (it = m_mapNetCDFVarDefs.begin() ; it != m_mapNetCDFVarDefs.end(); it++)
  {
    CNetCDFCoordinateAxis* var = CNetCDFCoordinateAxis::GetNetCDFCoordAxis((it->second));
    if (var == NULL)
    {
      continue;
    }

    coordAxes->Insert(it->first, var);
  }

}
//----------------------------------------
void CNetCDFFiles::GetNetCDFCoordAxes(CStringArray* coordAxesNames, bool bRemoveAll /* = true */, bool bUnique /* = false */)
{
  
  if (coordAxesNames == NULL)
  {
    return;
  }
  
  CObMap mapCoordAxes;
  
  GetNetCDFCoordAxes(&mapCoordAxes);

  mapCoordAxes.GetKeys(*coordAxesNames, bRemoveAll, bUnique);

}
//----------------------------------------
void CNetCDFFiles::GetNetCDFCoordAxesTime(CObArray* coordAxes)
{
  
  if (coordAxes == NULL)
  {
    return;
  }
  
  coordAxes->SetDelete(false);

  CObMap mapCoordAxes;
  
  GetNetCDFCoordAxesTime(&mapCoordAxes);

  mapCoordAxes.ToArray(*coordAxes);

}
//----------------------------------------
void CNetCDFFiles::GetNetCDFCoordAxesTime(CObMap* coordAxes)
{
  
  if (coordAxes == NULL)
  {
    return;
  }
  
  coordAxes->SetDelete(false);

  CObMap::const_iterator it;
  for (it = m_mapNetCDFVarDefs.begin() ; it != m_mapNetCDFVarDefs.end(); it++)
  {
    CNetCDFCoordinateAxis* var = CNetCDFCoordinateAxis::GetNetCDFCoordAxis((it->second));
    if (var == NULL)
    {
      continue;
    }

    if (var->IsTimeAxis())
    {
      coordAxes->Insert(it->first, var);
    }
  }

}
//----------------------------------------
void CNetCDFFiles::GetNetCDFVarDefsTime(CObArray* netCDFVarDefs)
{
  
  if (netCDFVarDefs == NULL)
  {
    return;
  }
  
  netCDFVarDefs->SetDelete(false);

  CObMap mapNetCDFVarDefs;
  
  GetNetCDFCoordAxesTime(&mapNetCDFVarDefs);

  mapNetCDFVarDefs.ToArray(*netCDFVarDefs);

}

//----------------------------------------
void CNetCDFFiles::GetNetCDFVarDefsTime(CObMap* netCDFVarDefs)
{
  
  if (netCDFVarDefs == NULL)
  {
    return;
  }
  
  netCDFVarDefs->SetDelete(false);

  CObMap::const_iterator it;
  for (it = m_mapNetCDFVarDefs.begin() ; it != m_mapNetCDFVarDefs.end(); it++)
  {
    CNetCDFVarDef* var = CNetCDFVarDef::GetNetCDFVarDef((it->second));
    if (var == NULL)
    {
      continue;
    }

    if (var->IsDateTime())
    {
      netCDFVarDefs->Insert(it->first, var);
    }
  }

}

//----------------------------------------
void CNetCDFFiles::GetNetCDFVarDefsTimeByDim(CObArray* netCDFVarDefs, bool sortedArray /*= false*/)
{
  
  if (netCDFVarDefs == NULL)
  {
    return;
  }
  
  netCDFVarDefs->SetDelete(false);

  CObMap mapNetCDFVarDefs;
  
  GetNetCDFVarDefsTimeByDim(&mapNetCDFVarDefs);

  mapNetCDFVarDefs.ToArray(*netCDFVarDefs);

  if (sortedArray)
  {
    sort(netCDFVarDefs->begin(), netCDFVarDefs->end(), CNetCDFVarDef::NetcdfVarCompareByName);
  }


}
//----------------------------------------
void CNetCDFFiles::GetNetCDFVarDefsTimeByDim(CObMap* netCDFVarDefs)
{
  
  if (netCDFVarDefs == NULL)
  {
    return;
  }
  
  netCDFVarDefs->SetDelete(false);

  CObMap::const_iterator it;
  for (it = m_mapNetCDFVarDefs.begin() ; it != m_mapNetCDFVarDefs.end(); it++)
  {
    CNetCDFVarDef* var = CNetCDFVarDef::GetNetCDFVarDef((it->second));
    if (var == NULL)
    {
      continue;
    }

    if (var->IsDateTime())
    {
      CStringArray dimNames;
      var->GetNetCdfDimNames(dimNames);
      netCDFVarDefs->Insert(dimNames.ToString(), var, false);
    }
  }

}

//----------------------------------------
void CNetCDFFiles::ReplaceNetCDFDim(CNetCDFDimension& dim)
{
  CNetCDFDimension* newDim = GetNetCDFDim(dim.GetName());

  if (newDim != NULL)
  {
    *newDim = dim;
  }

  CObMap::iterator it;
  for (it = m_mapNetCDFVarDefs.begin() ; it != m_mapNetCDFVarDefs.end() ; it++)
  {
    CNetCDFVarDef* var = CNetCDFVarDef::GetNetCDFVarDef((it->second));
    if (var == NULL)
    {
      continue;
    }

    var->ReplaceNetCDFDim(dim);
  }
}


//----------------------------------------
std::string CNetCDFFiles::IdentifyExistingFile
		(const std::string		&name,
		 bool			checkConvention	/*= false*/,
		 bool			noError		/*= false*/)
{
  std::string		fileTypeStr	= "---unknown file type---";

  if (name == "")
  {
    throw CFileException("Empty file name cannot be opened", BRATHL_LOGIC_ERROR);
  }

  try 
  {
    CNetCDFFiles netCdfFiles(name);
    fileTypeStr = netCdfFiles.IdentifyExistingFile(checkConvention, noError);
  }
  catch (CException &e) 
  {
    if (noError)
    {
      return fileTypeStr;
    }
    else
    {
      throw (e);
    }

  }
  return fileTypeStr;

}
//----------------------------------------
std::string CNetCDFFiles::IdentifyExistingFile
		(bool			checkConvention	/*= false*/, 
                 bool			noError		/*= false*/)
{
  std::string		fileTypeStr	= "---unknown file type---";
  std::string		attrNotFound	= "---Attribute  not found---";
  
  int status =  NC_NOERR;

  try
  {
    // ------------------------------------------------------------------
    // Along Track Product or Aviso Grid Netcdf or Brat Output NetCdf
    // ------------------------------------------------------------------
    std::string attValue;

    // search file_type global attribute value and return it if found
    int foundFileTypeAttr = GetAtt(NC_GLOBAL, FILE_TYPE_ATTR, attValue, false, attrNotFound);

    if ( foundFileTypeAttr != NC_NOERR)
    {
      if ((foundFileTypeAttr != NC_ENOTATT) && checkConvention)
      {
        status = GetAtt(NC_GLOBAL, CONVENTIONS_ATTR, attValue, false, attrNotFound);
      }
      if (status != NC_NOERR)
      {
        throw CFileException(CTools::Format("Accessing global attribute '%s': %s", CONVENTIONS_ATTR, nc_strerror(status)),
			     m_fileName,
			     BRATHL_LOGIC_ERROR);
      }
    }
    else
    {
      fileTypeStr = attValue;
      return fileTypeStr;
    }

    // ---------------------------------------------------------
    // Jason-2 Netcdf
    // ---------------------------------------------------------
    // search product_type and data_set global attribute values (Netcdf Jason-2) 
    std::string missionNameAttrValue;
    std::string productTypeAttrValue;
    std::string dataSetAttrValue;
    std::string titleAttrValue;

    int foundMissionNameAttr = GetAtt(NC_GLOBAL, MISSION_NAME_ATTR, missionNameAttrValue, false, attrNotFound);
    //int foundProductTypeAttr = GetAtt(NC_GLOBAL, PRODUCT_TYPE_ATTR, productTypeAttrValue, false, attrNotFound);
    int foundDataSetAttr = GetAtt(NC_GLOBAL, DATA_SET_ATTR, dataSetAttrValue, false, attrNotFound);
    int foundTitleAttr = GetAtt(NC_GLOBAL, TITLE_ATTR, titleAttrValue, false, attrNotFound);

    int32_t pos = -1;

    if (foundDataSetAttr != NC_NOERR)
    {
      if (foundTitleAttr == NC_NOERR)
      {
        pos = CTools::FindNoCase(titleAttrValue, SSHA_TITLE);
        if (pos >= 0)
        {
          dataSetAttrValue = SSHA;
          foundDataSetAttr = NC_NOERR;
        } 

        // ----------------------------------------
        // WARNING : first test "GDR" then "SGDR" 
        // ----------------------------------------
        pos = CTools::FindNoCase(titleAttrValue, GDR_TITLE);
        if (pos >= 0)
        {
          dataSetAttrValue = GDR;
          foundDataSetAttr = NC_NOERR;
        }

        pos = CTools::FindNoCase(titleAttrValue, SGDR_TITLE);
        if (pos >= 0)
        {
          dataSetAttrValue = SGDR;
          foundDataSetAttr = NC_NOERR;
        }
      }
    }
    //if ((foundMissionNameAttr == NC_NOERR) && (foundProductTypeAttr == NC_NOERR) && (foundDataSetAttr == NC_NOERR)) 
    if ((foundMissionNameAttr == NC_NOERR) && (foundDataSetAttr == NC_NOERR))
        
    {
      CMission mission(JASON2, false);
      CStringList aliases;
      mission.LoadAliasName(aliases);
      if (aliases.ExistsNoCase(missionNameAttrValue))
      {
//        fileTypeStr = CTools::StringToUpper(CTools::Format("%s/%s/%s",
//                                            mission.GetName(), 
//                                            dataSetAttrValue.c_str(),
//                                            productTypeAttrValue.c_str()
//                                            )
        fileTypeStr = CTools::StringToUpper(CTools::Format("%s/%s",
                                            mission.GetName(), 
                                            dataSetAttrValue.c_str()
                                            )
                                      );
        return fileTypeStr;
      }
    }

    // ---------------------------------------------------------
    // Generic Netcdf
    // ---------------------------------------------------------
    fileTypeStr = GENERIC_NETCDF_TYPE;

  }
  catch (CException &e)
  {
    if (!noError)
      throw (e);
  }
  catch (...)
  {
    if (!noError)
      throw;
  }

  return fileTypeStr;

/*
    char	*AttName;
    char	AttValue[1024];

    
    AttName	= FILE_TYPE_ATTR;
    Status	= nc_inq_att(File, NC_GLOBAL, AttName, &Type, &Length);
    if (Status != NC_NOERR)
    {
      if ((Status != NC_ENOTATT) && CheckConvention)
      {
	AttName	= CONVENTIONS_ATTR;
        Status	= nc_inq_att(File, NC_GLOBAL, AttName, &Type, &Length);
      }
      if (Status != NC_NOERR)
        throw CFileException(CTools::Format("Accessing global attribute '%s': %s", AttName, nc_strerror(Status)),
			     Name,
			     BRATHL_LOGIC_ERROR);
    }
    if (Length >= sizeof(AttValue))
    {
      throw CFileException(CTools::Format("Global attribute '%s' is too long: type = %d", AttName, Length),
			   Name,
			   BRATHL_IO_ERROR);
    }
    if (Type != NC_CHAR)
    {
      throw CFileException(CTools::Format("Global attribute 'FileType' is not a char: type = %d", Type),
			   Name,
			   BRATHL_LOGIC_ERROR);
    }
    Status	= nc_get_att_text(File, NC_GLOBAL, FILE_TYPE_ATTR, AttValue);
    // Put the final '\0' and remove all trailing blanks
    AttValue[Length]	= '\0';
    while ((Length > 0) && (AttValue[Length-1] == ' '))
      AttValue[--Length] = '\0';
    fileTypeStr	= AttValue;
  }
  catch (...)
  {
    nc_close(File);
    File	= -1;
    if (! NoError)
      throw;
  }
  if (File != -1)
    nc_close(File);
  return fileTypeStr;
  */
}

//----------------------------------------

void CNetCDFFiles::SetName(const std::string&	name)
{
  m_fileName	= name;
}
//----------------------------------------

void CNetCDFFiles::SetMode(brathl_FileMode	mode)
{
  m_fileMode	= mode;
}
//----------------------------------------
void CNetCDFFiles::Open(brathl_FileMode mode)
{
  m_fileMode	= mode;
  Open();
}

//----------------------------------------
void CNetCDFFiles::Open()
{
  Close();

  int mode = NC_NOWRITE;
  //int fileId;
  
  if (m_fileName == "")
  {
    throw CFileException("Empty file name cannot be opened", BRATHL_LOGIC_ERROR);
  }

  switch (m_fileMode)
  {
    //----------
    case Write:
    //----------
      mode = NC_WRITE;
      /*FALLTHRU*/
    //----------
    case ReadOnly:
    //----------
      CheckNetcdfError(nc_open(m_fileName.c_str(), mode /*| NC_64BIT_OFFSET*/, &m_file),
		       CTools::Format("Error opening file for %s access",
				      (m_fileMode == Write ? "write" : "read")));
      m_changingStructure = false;
      m_creating		= false;
      LoadStructure();
      break;
    //----------
    case New:
    //----------
      mode = NC_NOCLOBBER;
      /*FALLTHRU*/
    //----------
    case Replace:
    //----------
      // use netcdf-3 interface to permit specifying tuning parameters
      CheckNetcdfError(nc_create(m_fileName.c_str(), mode | NC_NETCDF4 /*| NC_64BIT_OFFSET*/, &m_file),
		       CTools::Format("Error creating%s file",
				      (m_fileMode == New ? "" : "/overwriting")));
      m_changingStructure = true;
      m_creating		= true;
      break;
    //----------
    default:
    //----------
      throw CFileException(CTools::Format("Open mode %d unknown", mode),
			   m_fileName,
			   BRATHL_LOGIC_ERROR);
  }

  //m_file	= fileId;
}

//----------------------------------------
void CNetCDFFiles::Close
		()
{
  if (IsOpened())
  {
    nc_close(m_file);
    m_file	= -1;
    if (m_fileMode != ReadOnly)
      m_fileMode	= Write;	// Avoid recreating the file on reopen
  }

  m_mapNetCDFDims.RemoveAll();
  m_mapNetCDFVarDefs.RemoveAll();
  m_mapNetCDFGlobalAttrs.RemoveAll();
}

//----------------------------------------

bool CNetCDFFiles::IsOpened
		() const
{
  return m_file >= 0;
}
//----------------------------------------

bool CNetCDFFiles::IsCreating
		()
{
  return m_creating;
}
//----------------------------------------
bool CNetCDFFiles::LoadStructure()
{

  MustBeOpened();
  
  bool bOk = true;

  try
  {
    GetInfoStructure(m_nDims, m_nVars, m_nGlobalAttrs, m_unlimitedDimId);

    if (m_nVars <= 0)
    {
      return false;
    }

    bOk &= LoadDimensions();

    bOk &= LoadVariables();
  }
  catch(CException& e)
  {
    Close();
    throw e;
  }

  return bOk;

}
//----------------------------------------
bool CNetCDFFiles::LoadDimensions()
{
  m_mapNetCDFDims.RemoveAll();

  char name[NC_MAX_NAME+1];
  size_t length;

  for (int32_t i = 0 ; i < m_nDims ; i++)
  {
    memset(name, '\0', NC_MAX_NAME+1); 

    CheckNetcdfError(nc_inq_dim(m_file, i, name, &length));
    // Ensure that \0 is at the end and remove all trailing blanks
    name[NC_MAX_NAME] = '\0';
    CTools::Trim(name);
    
    CNetCDFDimension* netCDFDim = new CNetCDFDimension(name, static_cast<uint32_t>(length), (i == m_unlimitedDimId));  
   
    m_mapNetCDFDims.Insert(name, netCDFDim);
  
  }

  return true;

}
//----------------------------------------
bool CNetCDFFiles::LoadVariables()
{
  m_mapNetCDFVarDefs.RemoveAll();

  char name[NC_MAX_NAME+1];
  int32_t nDims;
  int32_t nAttrs;
  nc_type type = NC_NAT;
  int32_t	dimIds[NC_MAX_VAR_DIMS];


  for (int32_t i = 0 ; i < m_nVars ; i++)
  {
    memset(name, '\0', NC_MAX_NAME+1); 

    CheckNetcdfError(nc_inq_var(m_file, i, name, &type, &nDims, dimIds, &nAttrs));
    // Ensure that \0 is at the end and remove all trailing blanks
    name[NC_MAX_NAME] = '\0';
    CTools::Trim(name);

    CNetCDFVarDef* netCDFVarDef = NULL;
    
    CNetCDFDimension* netCDFDim = dynamic_cast<CNetCDFDimension*>(m_mapNetCDFDims.Exists(name));
    
    if (netCDFDim != NULL)
    {
      NetCDFVarKind axisType = GetVarKind(i);
      netCDFVarDef = new CNetCDFCoordinateAxis(name, axisType, type);
    }
    else
    {
      netCDFVarDef = new CNetCDFVarDef(name, type);
    }
       
    netCDFVarDef->SetVarId(i);

    m_mapNetCDFVarDefs.Insert(name, netCDFVarDef);
  
    for (int32_t idim = 0 ; idim < nDims ; idim++)
    {
      std::string dimName = GetDimName(dimIds[idim]);
      
      CNetCDFDimension* netCDFDim = dynamic_cast<CNetCDFDimension*>(m_mapNetCDFDims.Exists(dimName));
      
      if (netCDFDim == NULL)
      {
        throw CException(CTools::Format("ERROR - CNetCDFFiles::LoadVariables - Unable to find dimension '%s' for variable '%s'.",
                                        dimName.c_str(), 
                                        netCDFVarDef->GetName().c_str()),
                         BRATHL_LOGIC_ERROR);
      }

      netCDFVarDef->AddNetCDFDim(*netCDFDim);
      netCDFDim->AddCoordinateVariable(netCDFVarDef->GetName());
    }

    bool con = true;
    do {
  try {
    LoadAttributes(netCDFVarDef);

    CNetCDFAttrString* netCDFAttrString = CNetCDFAttr::GetNetCDFAttrString(netCDFVarDef->GetAttribute(UNITS_ATTR));
    if (netCDFAttrString != NULL)
    {
      netCDFVarDef->SetUnit(netCDFAttrString->GetValue());
    }

    CNetCDFAttrDouble* netCDFAttrDouble = CNetCDFAttr::GetNetCDFAttrDouble(netCDFVarDef->GetAttribute(SCALE_FACTOR_ATTR));
    if (netCDFAttrDouble != NULL)
    {
      netCDFVarDef->SetScaleFactor(netCDFAttrDouble->GetValue()->at(0));
    }

    netCDFAttrDouble = CNetCDFAttr::GetNetCDFAttrDouble(netCDFVarDef->GetAttribute(ADD_OFFSET_ATTR));
    if (netCDFAttrDouble != NULL)
    {
      netCDFVarDef->SetAddOffset(netCDFAttrDouble->GetValue()->at(0));
    }

    }
    catch( ... )
    {
        std::cout << i << " <======== " << std::endl;
        con = false;
    }
  } while(!con);
  }


  // Loads global attributes
  LoadAttributes();

  return true;

}
//----------------------------------------
void CNetCDFFiles::LoadAttributes(CNetCDFVarDef* netCDFVarDef /*= NULL*/)
{

  int32_t varId = NC_GLOBAL;

  if (netCDFVarDef != NULL)
  {
    varId = netCDFVarDef->GetVarId();  
  }

  int32_t nAtts;

  char attName[NC_MAX_NAME+1];

  nc_type type = NC_NAT;
  size_t length = -1;
  
  memset(attName, '\0', NC_MAX_NAME+1); 

  CheckNetcdfError(nc_inq_varnatts (m_file, varId, &nAtts));

  // The attributes for each variable are numbered from 0 (the first attribute) to natts-1, 
  // where natts is the number of attributes for the variable, as returned from a call to nc_inq_varnatts. 
  for (int i = 0 ; i < nAtts ; i++) 
  {
    memset(attName, '\0', NC_MAX_NAME+1); 
    
    CheckNetcdfError(nc_inq_attname(m_file, varId, i, attName));
    // Ensure that \0 is at the end and remove all trailing blanks
    attName[NC_MAX_NAME] = '\0';
    CTools::Trim(attName);

    GetAttributeInfo(varId, attName, type, length);

    CNetCDFAttr* netCDFAttr = CNetCDFAttr::NewAttr(type, attName, length);
    netCDFAttr->ReadAttribute(m_file, varId);

    if (netCDFVarDef != NULL)
    {
      netCDFVarDef->AddAttribute(netCDFAttr);
    }
    else
    {    
      AddNetCDFGlobalAttributes(netCDFAttr);
    }

  }


}
//----------------------------------------

void CNetCDFFiles::WriteFileTitle
		(const std::string	&Title)
{
  MustBeOpened();
  PutAtt(NC_GLOBAL, FILE_TITLE_ATTR, Title);
}

//----------------------------------------
void CNetCDFFiles::MustBeOpened
		()
{
  if (! IsOpened())
  {
    throw CFileException("Try to do an operation on a closed file",
			 m_fileName,
			 BRATHL_LOGIC_ERROR);
  }
}

//----------------------------------------

bool CNetCDFFiles::IsAxisVar
		(const std::string	&Name)
{
  int varId	= GetVarId(Name);
  int Tmp;

  CheckNetcdfError(nc_inq_varndims(m_file, varId, &Tmp));
  if (Tmp != 1)
    return false;

  CheckNetcdfError(nc_inq_vardimid(m_file, varId, &Tmp));
  return GetDimName(Tmp) == Name;
}

//----------------------------------------

bool CNetCDFFiles::IsAxisVar
		(int		varId)
{
  int Tmp;
  MustBeOpened();
  CheckNetcdfError(nc_inq_varndims(m_file, varId, &Tmp));
  if (Tmp != 1)
    return false;

  CheckNetcdfError(nc_inq_vardimid(m_file, varId, &Tmp));
  return GetDimName(Tmp) == GetVarName(varId);
}

//----------------------------------------

void CNetCDFFiles::GetInfoStructure(int32_t& nbDims, int32_t& nbVars, int32_t& nbGlobalAttr, int32_t& unlimitedDimId)
{
  MustBeOpened();
  CheckNetcdfError(nc_inq(m_file, &nbDims, &nbVars, &nbGlobalAttr, &unlimitedDimId));
}
//----------------------------------------

int32_t CNetCDFFiles::GetNbDimensions
		()
{
  int NbDims;
  int NbVars;
  int NbGAtt;
  int UnlimitedDimId;

  MustBeOpened();
  CheckNetcdfError(nc_inq(m_file, &NbDims, &NbVars, &NbGAtt, &UnlimitedDimId));
  return NbDims;
}

//----------------------------------------

int32_t CNetCDFFiles::GetNbVariables
		()
{
  int NbDims;
  int NbVars;
  int NbGAtt;
  int UnlimitedDimId;

  MustBeOpened();
  CheckNetcdfError(nc_inq(m_file, &NbDims, &NbVars, &NbGAtt, &UnlimitedDimId));
  return NbVars;
}
//----------------------------------------

void CNetCDFFiles::GetDimensions
		(std::vector<std::string>			&Names)
{
  int NbDims;
  char Name[NC_MAX_NAME+1];
  int Index;

  MustBeOpened();
  Names.clear();
  NbDims	= GetNbDimensions();
  for (Index=0; Index<NbDims; ++Index)
  {
    CheckNetcdfError(nc_inq_dimname(m_file, Index, Name));
    Names.push_back(Name);
  }
}

//----------------------------------------

bool CNetCDFFiles::DimExists
		(const std::string& name)
{
  int32_t dimId;

  MustBeOpened();

  int32_t check		= nc_inq_dimid(m_file, name.c_str(), &dimId);

  return (check == NC_NOERR);
}
//----------------------------------------

uint32_t CNetCDFFiles::GetDimension
		(const std::string		&name)
{
  MustBeOpened();
  int dimId = GetDimId(name);

  return GetDimension(dimId);
}

//----------------------------------------
size_t CNetCDFFiles::GetDimension(int dimId)
{
  size_t	DimLen;
  MustBeOpened();
  CheckNetcdfError(nc_inq_dimlen(m_file, dimId, &DimLen));
  return DimLen;
}
//----------------------------------------
void CNetCDFFiles::GetVariables
		(std::vector<std::string>		&VarNames)
{
  int NbVars;
  char Name[NC_MAX_NAME+1];
  int Index;

  MustBeOpened();
  VarNames.clear();
  NbVars	= GetNbVariables();
  for (Index=0; Index<NbVars; ++Index)
  {
    CheckNetcdfError(nc_inq_varname(m_file, Index, Name));
    VarNames.push_back(Name);
  }
}
//----------------------------------------

void CNetCDFFiles::GetAxisVars
		(std::vector<std::string>		&VarNames)
{
  int NbVars;
  int Index;

  MustBeOpened();
  VarNames.clear();
  NbVars	= GetNbVariables();
  for (Index=0; Index<NbVars; ++Index)
  {
    if (IsAxisVar(Index))
      VarNames.push_back(GetVarName(Index));
  }
}
//----------------------------------------

void CNetCDFFiles::GetAxisVars
		(std::vector<int>		&VarIds)
{
  int NbVars;
  int Index;

  MustBeOpened();
  VarIds.clear();
  NbVars	= GetNbVariables();
  for (Index=0; Index<NbVars; ++Index)
  {
    if (IsAxisVar(Index))
      VarIds.push_back(Index);
  }
}
//----------------------------------------

void CNetCDFFiles::GetDataVars
		(std::vector<std::string> &VarNames)
{
  int NbVars;
  int Index;

  MustBeOpened();
  VarNames.clear();
  NbVars	= GetNbVariables();
  for (Index=0; Index<NbVars; ++Index)
  {
    if (! IsAxisVar(Index))
      VarNames.push_back(GetVarName(Index));
  }
}
//----------------------------------------

void CNetCDFFiles::GetDataVars
		(std::vector<int> &VarIds)
{
  int NbVars;
  int Index;

  MustBeOpened();
  VarIds.clear();
  NbVars	= GetNbVariables();
  for (Index=0; Index<NbVars; ++Index)
  {
    if (! IsAxisVar(Index))
      VarIds.push_back(Index);
  }
}
//----------------------------------------

bool CNetCDFFiles::VarExists
		(const std::string		&Name)
{
  int varId;
  int Check;

  MustBeOpened();
  Check		= nc_inq_varid(m_file, Name.c_str(), &varId);
  return Check == NC_NOERR;
}
//----------------------------------------

void CNetCDFFiles::WriteData(CNetCDFVarDef* varDef, CExpressionValue* values)
{  
  SetDataMode();
  varDef->WriteData(m_file, values);
}

//----------------------------------------
void CNetCDFFiles::WriteData(CNetCDFVarDef* varDef, CMatrix* matrix)
{
  SetDataMode();
  varDef->WriteData(m_file, matrix);
}

//----------------------------------------
void CNetCDFFiles::WriteVariables()
{
  SetDefineMode();

  CObMap::const_iterator it;
  for (it = m_mapNetCDFVarDefs.begin() ; it != m_mapNetCDFVarDefs.end(); it++)
  {
    CNetCDFVarDef* var = CNetCDFVarDef::GetNetCDFVarDef((it->second));
    if (var == NULL)
    {
      continue;
    }

    WriteVariable(var);


  }

}
//----------------------------------------
void CNetCDFFiles::WriteVariable(CNetCDFVarDef* var)
{

  if (var == NULL)
  {
    return;
  }

  SetDefineMode();

  CObArray* varDims = var->GetNetCDFDims();

  CIntArray dimIds;

  CObArray::const_iterator it;

  for (it = varDims->begin() ; it != varDims->end(); it++)
  {
    CNetCDFDimension* netCDFDimVar = dynamic_cast<CNetCDFDimension*>(*it);
    if (netCDFDimVar == NULL)
    {
      continue;
    }
    CNetCDFDimension* netCDFDimFile = dynamic_cast<CNetCDFDimension*>(m_mapNetCDFDims.Exists(netCDFDimVar->GetName()));
    if (netCDFDimFile == NULL)
    {
      throw CException(CTools::Format("ERROR in  CNetCDFFiles::WriteVariable - Variable %s - Dimension %s must be added first (dimension definition is NULL)", 
                                      var->GetName().c_str(), netCDFDimVar->GetName().c_str()), 
                        BRATHL_LOGIC_ERROR);

    }
    int32_t dimId = netCDFDimFile->GetDimId();

    if (dimId < 0)
    {
      throw CException(CTools::Format("ERROR in  CNetCDFFiles::WriteVariable - Variable %s - Dimension %s must be added first (dimension id is negative)", 
                                      var->GetName().c_str(), netCDFDimVar->GetName().c_str()), 
                        BRATHL_LOGIC_ERROR);

    }
  
    dimIds.Insert(dimId);
  }


  int32_t* dimIdsArray = dimIds.ToArray();

  int varId = -1;

  CheckNetcdfError(nc_def_var(m_file,
			                        var->GetName().c_str(),
			                        var->GetType(),
			                        var->GetNbDims(),
			                        dimIdsArray,
			                        &varId),
		               CTools::Format("Error defining variable '%s' with %d dimensions",
				              var->GetName().c_str(),
				              var->GetNbDims()));

  delete []dimIdsArray;
  dimIdsArray = NULL;

  var->SetVarId(varId);

  WriteAttributes(var);
}

//----------------------------------------
void CNetCDFFiles::WriteAttributes(CNetCDFVarDef* var)
{ 
  if (var == NULL)
  {
    return;
  }

  SetDefineMode();

  var->WriteAttributes(m_file);
}
//----------------------------------------
void CNetCDFFiles::WriteDimensions()
{
  SetDefineMode();

  CObMap::const_iterator it;
  for (it = m_mapNetCDFDims.begin() ; it != m_mapNetCDFDims.end(); it++)
  {
    CNetCDFDimension* netCDFDim = dynamic_cast<CNetCDFDimension*>(it->second);
    if (netCDFDim == NULL)
    {
      continue;
    }

    WriteDimension(netCDFDim);
  }
}
//----------------------------------------
void CNetCDFFiles::WriteDimension(CNetCDFDimension* netCDFDim)
{
  if (netCDFDim == NULL)
  {
    return;
  }

  SetDefineMode();

  long length = NC_UNLIMITED;

  if (!netCDFDim->IsUnlimited())
  {
    length = netCDFDim->GetLength();
  }
  if (length <= 0)
  {
    length = NC_UNLIMITED;
  }

  std::string name = netCDFDim->GetName();
  
  int32_t dimId = GetDimId(name, false);

  // if dim already exists ==> return
  if (dimId >= 0)
  {
    return;
  }

  CheckNetcdfError(nc_def_dim(m_file,
				                      name.c_str(),
				                      length,
				                      &dimId),
		               CTools::Format("Error defining dimension '%s' to %ld",
				                          name.c_str(),
				                          length));

  netCDFDim->SetDimId(dimId);
}



//----------------------------------------
int CNetCDFFiles::SetDimension
                    (NetCDFVarKind dimKind,
		                 const std::string	&dimName,
		                 int32_t value,
		                 const CExpressionValue	&dimValue,
		                 const std::string	&units,
		                 const std::string	&longName,
                     const std::string &comment /* = ""*/,
                     double	validMin /* = CTools::m_defaultValueDOUBLE */,
		                 double	validMax /* = CTools::m_defaultValueDOUBLE */)
{
  std::string	actualName	= dimName;
  int		varId;
  
  CNetCDFCoordinateAxis var(dimKind);

  std::string	dimType = var.GetDimType();
  std::string	axis = var.GetAxis();


  MustBeOpened();


  if (actualName == "")
  {
    actualName	= dimType;
  }

  if (actualName == "")
  {
    actualName	= axis;
  }

  if (actualName == "")
  {
    int32_t ndims = GetNbDimensions();
    actualName = CTools::Format("dim%d", ndims + 1);
  }


  int dimId;
  size_t wantedLength	= static_cast<size_t>(value);
  size_t dimLength;
  
  int Status	= nc_inq_dimid(m_file, actualName.c_str(), &dimId);
  
  if (Status != NC_NOERR)
  {
    if (Status == NC_EBADDIM)
    {
      SetDefineMode();
      CheckNetcdfError(nc_def_dim(m_file,
				                          actualName.c_str(),
				                          (wantedLength == 0 ? NC_UNLIMITED : wantedLength),
				                          &dimId),
		                   CTools::Format("Error defining dimension '%s' to %ld",
				                              actualName.c_str(),
				                              (long)wantedLength));
      
      CUnit unit = CUnit::ToUnit(units);
      std::string strUnit = unit.AsString(false, true);

      varId	= AddVar(actualName, NC_DOUBLE, strUnit, longName, comment, dimId, -1, -1, -1, validMin, validMax);

      PutAtt(varId, STANDARD_NAME_ATTR, dimType);

      PutAtt(varId, AXIS_ATTR, axis);


      if (!isDefaultValue(validMin))
      {
        if (dimKind == T)
        {
          unit.SetConversionToBaseUnit();

          CDate date(unit.Convert(validMin));          
          
          PutAtt(NC_GLOBAL, actualName + "_min", date.AsString("", true));
        }
        else
        {
          PutAtt(NC_GLOBAL, actualName + "_min", unit.ConvertToBaseUnit(validMin));
        }

      }
      if (!isDefaultValue(validMax))
      {
        if (dimKind == T)
        {
          unit.SetConversionToBaseUnit();

          CDate date(unit.Convert(validMax));          

          PutAtt(NC_GLOBAL, actualName + "_max", date.AsString("", true));
        }
        else
        {
          PutAtt(NC_GLOBAL, actualName + "_max", unit.ConvertToBaseUnit(validMax));
        }
      }

    }
    else
    {
      CheckNetcdfError(Status,
		       CTools::Format("Error identifying dimension '%s'",
				      actualName.c_str()));
    }
  }

  CheckNetcdfError(nc_inq_dimlen(m_file, dimId, &dimLength));

  if (dimLength != wantedLength)
  {
    throw CFileException(CTools::Format("Dimension '%s' has not the expected value (%ld): %ld",
                                        actualName.c_str(),
                                        (long)wantedLength,
                                        (long)dimLength));
  }

  WriteVar(actualName, dimValue);

  return dimId;
}
//----------------------------------------

void CNetCDFFiles::WriteVar
		(const std::string		&Name,
		 const CExpressionValue	&value)
{
  int		varId		= GetVarId(Name);
  double	DefValue	= GetNetcdfVarDefValue(varId);
  bool		DimOk;
  int32_t	NbData;

  ExpressionValueDimensions VarDimensions	= value.GetDimensions();
  ExpressionValueDimensions Dimensions;

  MustBeOpened();

  GetVarDims(varId, Dimensions);

  double ScaleFactor = GetNetcdfVarScaleFactor(varId);
  double AddOffset = GetNetcdfVarAddOffset(varId);

  DimOk		= Dimensions.size() == VarDimensions.size();
  NbData	= 1;
  for (uint32_t Index=0; Index < Dimensions.size(); ++Index)
  {
    DimOk	= DimOk && (Dimensions[Index] == VarDimensions[Index]);
    NbData	*= Dimensions[Index];
  }
  if (! DimOk)
  {
    throw CFileException(CTools::Format("Dimensions of variable to write does not correspond do the ones in the file: %s (%d data vs %d wanted data)",
				        Name.c_str(),
					NbData,
					value.GetNbValues()),
			 m_fileName,
			 BRATHL_IO_ERROR);
  }

  SetDataMode();

  if ((DefValue == CTools::m_defaultValueDOUBLE) &&
      (ScaleFactor == 1.0) && (AddOffset == 0.0))
  {// No conversion needed
    CheckNetcdfError(nc_put_var_double(m_file, varId, value.GetValues()),
		     CTools::Format("Error writing variable %s",
				    Name.c_str()));

  }
  else
  {
    CExpressionValue	VarValue(FloatType, Dimensions, NULL);
    double *Source	= value.GetValues();
    double *Dest	= VarValue.GetValues();
    size_t NbValues	= VarValue.GetNbValues();

    for (uint32_t Index=0; Index<NbValues; ++Index)
    {
      if (Source[Index] == CTools::m_defaultValueDOUBLE)
          Dest[Index]	= DefValue;
      else
	        Dest[Index]	= (Source[Index]-AddOffset)/ScaleFactor;
    }
    CheckNetcdfError(nc_put_var_double(m_file, varId, Dest),
		     CTools::Format("Error writing variable %s",
				    Name.c_str()));
  }
}

//----------------------------------------

void CNetCDFFiles::ReadVar(const std::string& name, CDoubleArray& array, const std::string& wantedUnit)
{

  CExpressionValue varValue;

  ReadVar(name, varValue, wantedUnit);

  array.Insert(varValue.GetValues(), varValue.GetNbValues());
}


//----------------------------------------

void CNetCDFFiles::ReadVar(const std::string& name, CExpressionValue	&exprValue, const std::string& WantedUnit)
{
  ExpressionValueDimensions	Dimensions;

  int varId		= GetVarId(name);
  GetVarDims(varId, Dimensions);

  exprValue.SetNewValue(FloatType, Dimensions, NULL);

  size_t nbValues	= exprValue.GetNbValues();
  double* values	= exprValue.GetValues();

  CheckNetcdfError(nc_get_var_double(m_file, varId, values),
		               CTools::Format("Error reading variable %s",
				                          name.c_str()));

  ApplyUnitAndFactorsToDataRead(values,
				nbValues,
				varId,
				WantedUnit);

  //value		= VarValue;
}

//----------------------------------------
void CNetCDFFiles::ReadVar(const std::string& name, uint32_t	nbDimensions, uint32_t *start, uint32_t	*count, double *value,
                      		 const std::string& wantedUnit)
{
  int				varId;
  uint32_t			nbValues;
  ExpressionValueDimensions	dimensions;

  varId		= GetVarId(name);
  GetVarDims(varId, dimensions);

  if (dimensions.size()	!= nbDimensions)
  {
    throw CFileException(CTools::Format("%d dimensions asked for %s while it is defined with %ld",
					nbDimensions, name.c_str(), (long)dimensions.size()),
			     m_fileName,
			     BRATHL_LOGIC_ERROR);
  }

  nbValues	= 1;
  if (nbDimensions == 0)
  {
    CheckNetcdfError(nc_get_var_double(m_file, varId, value),
		   CTools::Format("Error reading variable %s",
				  name.c_str()));
  }
  else
  {
    for (uint32_t index = 0; index < nbDimensions; index++)
    {
      nbValues		*= count[index];
    }
// For optimization
//    CheckNetcdfError(nc_get_vara_double(m_file, varId, Start, Count, value),
//		   CTools::Format("Error reading variable %s",
//				  Name.c_str()));
    size_t *ncstart = new size_t[nbDimensions];
    size_t *nccount = new size_t[nbDimensions];
    for (uint32_t i = 0; i < nbDimensions; ++i)
    {
        ncstart[i] = start[i];
        nccount[i] = count[i];
    }
    int32_t status = nc_get_vara_double(m_file, varId, ncstart, nccount, value);
    
    delete []ncstart;
    ncstart = NULL;
    delete []nccount;
    nccount = NULL;

    if (status != NC_NOERR)
    {
      throw CFileException(CTools::Format("Error reading variable %s.\nNetcdf error: %s",
					  name.c_str(),
					  nc_strerror(status)),
			   m_fileName,
			   BRATHL_IO_ERROR);
    }

  }
  ApplyUnitAndFactorsToDataRead(value,
				nbValues,
				varId,
				wantedUnit);
}

//----------------------------------------
void CNetCDFFiles::ReadVar(CFieldNetCdf* field, double& value, const std::string& wantedUnit)
{
  if (field == NULL)
  {
    return;
  }


  uint32_t nbDimensions = field->GetNbDims();

  uint32_t* start = field->GetDimsIndexArray();
  
  int32_t varId = field->GetNetCdfId();
  
  CUnit* unit = field->GetNetCdfUnit();
  
  double defValue = field->GetFillValue();
  
  if (nbDimensions == 0)
  {
    CheckNetcdfError(nc_get_var_double(m_file, varId, &value),
		   CTools::Format("Error reading variable %s",
				  field->GetName().c_str()));
  }
  else
  {
// For optimization
//    CheckNetcdfError(nc_get_vara_double(m_file, varId, Start, Count, value),
//		   CTools::Format("Error reading variable %s",
//				  Name.c_str()));
    size_t *ncstart = new size_t[nbDimensions];
    size_t *nccount = new size_t[nbDimensions];
    for (uint32_t i = 0; i < nbDimensions; ++i)
    {
        ncstart[i] = start[i];
        nccount[i] = 1;
    }
    int32_t status = nc_get_vara_double(m_file, varId, ncstart, nccount, &value);

    delete []ncstart;
    ncstart = NULL;
    delete []nccount;
    nccount = NULL;

    if (status != NC_NOERR)
    {
      throw CFileException(CTools::Format("Error reading variable %s.\nNetcdf error: %s",
					  field->GetName().c_str(),
					  nc_strerror(status)),
			   m_fileName,
			   BRATHL_IO_ERROR);
    }

  }


  if ((wantedUnit != "") && (wantedUnit != CUnit::m_UNIT_SI))
  {
    unit->SetConversionTo(wantedUnit);
  }
  else
  {  
    unit->SetConversionToBaseUnit();
  }

  if (value == defValue)
  {
    value	= CTools::m_defaultValueDOUBLE;
  }
  else
  {
    value = unit->Convert( value * field->GetScaleFactor() + field->GetAddOffset() );
  }

}

//----------------------------------------
void CNetCDFFiles::ReadVar(CFieldNetCdf* field, const std::string& wantedUnit)
{
  ReadVar(field, field->GetValues(), wantedUnit);
}
//----------------------------------------
void CNetCDFFiles::ReadVar(CFieldNetCdf* field, CDoubleArray& values, const std::string& wantedUnit)
{
  values.RemoveAll();
  CExpressionValue exprValue;

  ReadVar(field, exprValue, wantedUnit);

  values.Insert(exprValue.GetValues(), exprValue.GetNbValues());

}
//----------------------------------------
void CNetCDFFiles::ReadVar(CFieldNetCdf* field, CExpressionValue& exprValue, const std::string& wantedUnit)
{
  if (field == NULL)
  {
    return;
  }

  uint32_t nbDimensions = field->GetNbDims();

  uint32_t* start = field->GetDimsIndexArray();

  uint32_t* count = field->GetDimsCountArray();
  
  int32_t varId = field->GetNetCdfId();
  
  CUnit* unit = field->GetNetCdfUnit();
  
  double defValue = field->GetFillValue();

  uint32_t index = 0;

  exprValue.SetNewValue(FloatType, count, nbDimensions, NULL);

  size_t nbValues = exprValue.GetNbValues();
  double* values = exprValue.GetValues();

  if (nbDimensions == 0)
  {
    CheckNetcdfError(nc_get_var_double(m_file, varId, &values[0]),
		   CTools::Format("Error reading variable %s",
				  field->GetName().c_str()));
  }
  else
  {

// For optimization
//    CheckNetcdfError(nc_get_vara_double(m_file, varId, Start, Count, value),
//		   CTools::Format("Error reading variable %s",
//				  Name.c_str()));
    size_t *ncstart = new size_t[nbDimensions];
    size_t *nccount = new size_t[nbDimensions];
    for (uint32_t i = 0; i < nbDimensions; ++i)
    {
        ncstart[i] = start[i];
        nccount[i] = count[i];
    }
    int32_t status = nc_get_vara_double(m_file, varId, ncstart, nccount, values);

    delete []ncstart;
    ncstart = NULL;
    delete []nccount;
    nccount = NULL;

    if (status != NC_NOERR)
    {
      throw CFileException(CTools::Format("Error reading variable %s.\nNetcdf error: %s",
					  field->GetName().c_str(),
					  nc_strerror(status)),
			   m_fileName,
			   BRATHL_IO_ERROR);
    }

  }


  if ((wantedUnit != "") && (wantedUnit != CUnit::m_UNIT_SI))
  {
    unit->SetConversionTo(wantedUnit);
  }
  else
  {  
    unit->SetConversionToBaseUnit();
  }

  for (index = 0; index < nbValues; index++)
  {
    if (values[index] == defValue)
    {
      values[index]	= CTools::m_defaultValueDOUBLE;
    }
    else
    {
      values[index]	= unit->Convert( values[index] * field->GetScaleFactor() + field->GetAddOffset() );
    }
  }

}

//----------------------------------------
void CNetCDFFiles::ReadEntireVar(CNetCDFVarDef* netCDFVarDef, CDoubleArray& array, const std::string& wantedUnit)
{

  if (netCDFVarDef == NULL)
  {
    return;
  }


  uint32_t nbDimensions = netCDFVarDef->GetNbDims();

  int32_t varId = netCDFVarDef->GetVarId();
  
  CUnit* unit = netCDFVarDef->GetUnit();
  
  double defValue = netCDFVarDef->GetFillValue();

  uint32_t index = 0;

  array.RemoveAll();
  
  uint32_t nbValues = netCDFVarDef->GetNbData();

  array.resize(nbValues);

  if (nbDimensions == 0)
  {
    CheckNetcdfError(nc_get_var_double(m_file, varId, &array[0]),
		   CTools::Format("Error reading variable %s",
				  netCDFVarDef->GetName().c_str()));
  }
  else
  {

    int32_t status = nc_get_var_double(m_file, varId, &array.front());

    if (status != NC_NOERR)
    {
      throw CFileException(CTools::Format("Error reading variable %s.\nNetcdf error: %s",
					  netCDFVarDef->GetName().c_str(),
					  nc_strerror(status)),
			   m_fileName,
			   BRATHL_IO_ERROR);
    }

  }


  if ((wantedUnit != "") && (wantedUnit != CUnit::m_UNIT_SI))
  {
    unit->SetConversionTo(wantedUnit);
  }
  else
  {  
    unit->SetConversionToBaseUnit();
  }

  for (index = 0; index < nbValues; index++)
  {
    if (array[index] == defValue)
    {
      array[index]	= CTools::m_defaultValueDOUBLE;
    }
    else
    {
      array[index]	= unit->Convert( array[index] * netCDFVarDef->GetScaleFactor() + netCDFVarDef->GetAddOffset() );
    }
  }

}
//----------------------------------------
void CNetCDFFiles::ReadEntireVar(CNetCDFDimension* netCDFDimension, CDoubleArray& array, const std::string& wantedUnit)
{

  if (netCDFDimension == NULL)
  {
    return;
  }

  CNetCDFVarDef* netCDFVarDef = this->GetNetCDFVarDef(netCDFDimension->GetName());

  ReadEntireVar(netCDFVarDef, array, wantedUnit);

}
//----------------------------------------


bool CNetCDFFiles::IsVarNameValid
		(const std::string	&Name)
{
  if (Name.size() == 0)
    return false;

  for (size_t Index=0; Index<Name.size(); ++Index)
  {
    char c	= Name[Index];
    if (! ((isascii(c) && isalnum(c)) || isdigit(c) || (c == '_')))
      return false;
  }
  return true;
}
//----------------------------------------


void CNetCDFFiles::SetDefineMode
		()
{
  if (! m_changingStructure)
  {
    CheckNetcdfError(nc_redef(m_file),
		     "Error changing definition mode (nc_redef)");
    m_changingStructure	= true;
  }
}

//----------------------------------------
void CNetCDFFiles::SetDataMode
		()
{
  if (m_changingStructure)
  {
    CheckNetcdfError(nc_enddef(m_file),
		     "Error changing definition mode (nc_enddef)");
    m_changingStructure	= false;
  }
}

//----------------------------------------

int CNetCDFFiles::GetVarId(const std::string	&name, bool	global /*= false*/, bool withExcept /* = true */)
{
  int varId;
  MustBeOpened();

  if (name.empty())
  {
    if (!global)
    {
      throw CFileException("Empty var name not allowed", m_fileName, BRATHL_IO_ERROR);
    }
    varId	= NC_GLOBAL;
  }
  else
  {
    if (withExcept)
    {
      CheckNetcdfError(nc_inq_varid(m_file, name.c_str(), &varId));
    }
    else
    {
      nc_inq_varid(m_file, name.c_str(), &varId);
    }
  }

  return varId;
}


//----------------------------------------
int CNetCDFFiles::GetDimId(const std::string& name, bool withExcept /* = true */)
{
  int dimId = -1;

  MustBeOpened();

  if (withExcept)
  {
    CheckNetcdfError(nc_inq_dimid(m_file, name.c_str(), &dimId));
  }
  else
  {
    nc_inq_dimid(m_file, name.c_str(), &dimId);
  }

  return dimId;
}

//----------------------------------------

std::string CNetCDFFiles::GetDimName
		(int		dimId)
{
  char dimName[NC_MAX_NAME+1];
  MustBeOpened();
  CheckNetcdfError(nc_inq_dimname(m_file, dimId, dimName));
  return dimName;
}
//----------------------------------------

std::string CNetCDFFiles::GetVarName
		(int		varId)
{
  char VarName[NC_MAX_NAME+1];
  MustBeOpened();
  CheckNetcdfError(nc_inq_varname(m_file, varId, VarName));
  return VarName;
}

//----------------------------------------

int CNetCDFFiles::GetVarNbDims
		(const std::string			&Name)
{
  return GetVarNbDims(GetVarId(Name));
}
//----------------------------------------

void CNetCDFFiles::GetVarDims
		(const std::string			&Name,
		 ExpressionValueDimensions	&Dimensions)
{
  GetVarDims(GetVarId(Name), Dimensions);
}
//----------------------------------------

void CNetCDFFiles::GetVarDims
		(const std::string			&Name,
		 std::vector<std::string>			&Dimensions)
{
  GetVarDims(GetVarId(Name), Dimensions);
}
//----------------------------------------

int CNetCDFFiles::GetVarNbDims
		(int				varId)
{
  int		NbDims;
  MustBeOpened();
  CheckNetcdfError(nc_inq_varndims(m_file,
				   varId,
				   &NbDims));
  return NbDims;
}
//----------------------------------------

void CNetCDFFiles::GetVarDims
		(int				varId,
		 ExpressionValueDimensions	&Dimensions)
{
  int		DimIds[NC_MAX_VAR_DIMS];
  int		NbDims;
  size_t	dimValue;
  MustBeOpened();
  CheckNetcdfError(nc_inq_varndims(m_file,
				   varId,
				   &NbDims));
  CheckNetcdfError(nc_inq_vardimid(m_file,
				   varId,
				   DimIds));
  Dimensions.clear();
  for (int Index=0; Index<NbDims; Index++)
  {
    CheckNetcdfError(nc_inq_dimlen(m_file,
				   DimIds[Index],
				   &dimValue));
    Dimensions.push_back(static_cast<uint32_t>(dimValue));
  }
}
//----------------------------------------

void CNetCDFFiles::GetVarDims
		(int				varId,
		 std::vector<std::string>			&Dimensions)
{
  int		DimIds[NC_MAX_VAR_DIMS];
  char		dimName[NC_MAX_NAME+1];
  int		NbDims;

  MustBeOpened();
  CheckNetcdfError(nc_inq_varndims(m_file,
				   varId,
				   &NbDims));
  CheckNetcdfError(nc_inq_vardimid(m_file,
				   varId,
				   DimIds));
  Dimensions.clear();
  for (int Index=0; Index<NbDims; Index++)
  {
    CheckNetcdfError(nc_inq_dimname(m_file,
				    DimIds[Index],
				    dimName));
    Dimensions.push_back(dimName);
  }
}
//----------------------------------------

void CNetCDFFiles::GetVarDimIds
		(const std::string	&name,
		 CIntArray& dimIds)
{
  GetVarDimIds(GetVarId(name), dimIds);
}
//----------------------------------------
void CNetCDFFiles::GetVarDimIds
		(int		varId,
		 CIntArray& dimIds)
{
  int		dimIdsTmp[NC_MAX_VAR_DIMS];
  int		nbDims;

  MustBeOpened();
  CheckNetcdfError(nc_inq_varndims(m_file,
				   varId,
				   &nbDims));
  CheckNetcdfError(nc_inq_vardimid(m_file,
				   varId,
				   dimIdsTmp));
  for (int i = 0 ; i < nbDims ; i++)
  {
    dimIds.Insert(dimIdsTmp[i]);
  }

}

//----------------------------------------
double CNetCDFFiles::GetNetcdfVarScaleFactor(int32_t  varId)
{
  double scaleFactor = 1.0;
  GetAtt(varId, SCALE_FACTOR_ATTR, scaleFactor, false, scaleFactor);
  return scaleFactor;
}
//----------------------------------------
double CNetCDFFiles::GetNetcdfVarAddOffset(int32_t  varId)
{
  double addOffset = 0.0;
  GetAtt(varId, ADD_OFFSET_ATTR, addOffset, false, addOffset);
  return addOffset;
}
//----------------------------------------
double CNetCDFFiles::GetNetcdfVarDefValue(int	varId)
{
  nc_type varType = GetVarType(varId);
  
  return GetNetcdfVarDefValue(varId, varType);
}

//----------------------------------------
double CNetCDFFiles::GetNetcdfVarDefValue(int varId, nc_type varType)
{
  double result;
  double defValue;
    
  switch (varType)
  {
    case NC_BYTE:	defValue	= NcFillByte;	break;
    case NC_CHAR:	defValue	= NcFillChar;	break;
    case NC_SHORT:	defValue	= NcFillShort;	break;
    case NC_INT:	defValue	= NcFillInt;	break;
    case NC_FLOAT:	defValue	= NcFillFloat;	break;
    case NC_DOUBLE:	defValue	= NcFillDouble;	break;
      //new
    case NC_UBYTE:	defValue	= NcFillUByte;	break;
    case NC_USHORT:	defValue	= NcFillUShort;	break;
    case NC_UINT:	defValue	= NcFillUInt;	break;
    case NC_INT64:	defValue	= NcFillInt64;	break;
    case NC_UINT64:	defValue	= NcFillUInt64;	break;
    case NC_STRING:
      defValue	= NcFillString;
      break;
    default:
	throw CFileException(CTools::Format("Var type #%d unknown for variable #%d",
					    varType,
					    varId),
			     m_fileName,
			     BRATHL_LOGIC_ERROR);
  }
  GetAtt(varId, FILL_VALUE_ATTR, result, false, defValue);
  if (CTools::IsNan(result))
  {
    result = defValue;
  }
  return result;
}
//----------------------------------------
double CNetCDFFiles::GetDefValue(nc_type VarType)
{
  switch (VarType)
  {
    case NC_BYTE:	return static_cast<double>(CTools::m_defaultValueINT8);
    case NC_CHAR:	return static_cast<double>(CTools::m_defaultValueCHAR);
    case NC_SHORT:	return static_cast<double>(CTools::m_defaultValueINT16);
    case NC_INT:	return static_cast<double>(CTools::m_defaultValueINT32);
    case NC_FLOAT:	return static_cast<double>(CTools::m_defaultValueFLOAT);
    case NC_DOUBLE:	return CTools::m_defaultValueDOUBLE;
      //new
    case NC_UBYTE:	return static_cast<double>(CTools::m_defaultValueINT8);
    case NC_USHORT:	return static_cast<double>(CTools::m_defaultValueUINT16);
    case NC_UINT:	return static_cast<double>(CTools::m_defaultValueUINT32);
    case NC_INT64:	return static_cast<double>(CTools::m_defaultValueINT64);
    case NC_UINT64:	return static_cast<double>(CTools::m_defaultValueUINT64);
    case NC_STRING:
      return static_cast<double>((ptrdiff_t)CTools::m_defaultValueString);
    default:
	throw CFileException(CTools::Format("Data type #%d unknown", VarType),
			     m_fileName,
			     BRATHL_LOGIC_ERROR);
  }
  return 0.0; // Never reached but avoids compiler complaint.
}
//----------------------------------------
nc_type CNetCDFFiles::GetVarType(const std::string &name)
{
  return GetVarType(GetVarId(name));
}

//----------------------------------------
nc_type CNetCDFFiles::GetVarType(int varId)
{
  nc_type varType = NC_NAT;
  CheckNetcdfError(nc_inq_vartype(m_file,
				  varId,
				  &varType));
  return varType;
}

//----------------------------------------
std::string CNetCDFFiles::GetVarTypeName(const std::string &name)
{
  return GetVarTypeName(GetVarId(name));
}
//----------------------------------------
std::string CNetCDFFiles::GetTypeAsString(int varType)
{
  std::string	result;
  switch (varType)
  {
    case NC_NAT:	result	= NC_NAT_NAME;	  break;
    case NC_BYTE:	result	= NC_BYTE_NAME;	  break;
    case NC_CHAR:	result	= NC_CHAR_NAME;	  break;
    case NC_SHORT:	result	= NC_SHORT_NAME;  break;
    case NC_INT:	result	= NC_INT_NAME;	  break;
    case NC_FLOAT:	result	= NC_FLOAT_NAME;  break;
    case NC_DOUBLE:	result	= NC_DOUBLE_NAME; break;
		//new
	case NC_UBYTE:  result	= NC_UBYTE_NAME; break;
	case NC_USHORT: result	= NC_USHORT_NAME; break;
	case NC_UINT:	result	= NC_UINT_NAME; break;
	case NC_INT64:  result	= NC_INT64_NAME; break;
	case NC_UINT64: result	= NC_UINT64_NAME; break;
	case NC_STRING: result	= NC_STRING_NAME; break;

    default:
	throw CException(CTools::Format("var type #%d unknown",
					    varType),
			     BRATHL_LOGIC_ERROR);
  }
  return result;
}
//----------------------------------------
std::string CNetCDFFiles::GetVarTypeName(int varId)
{
  nc_type varType = NC_NAT;
  
  try 
  {
  
    varType = GetVarType(varId);

  }
  catch(CException& e)
  {
    
    throw CFileException(CTools::Format("var type #%d unknown for variable #%d (native error %s)",
					    varType,
					    varId,
                                            e.what()),
			     m_fileName,
			     BRATHL_LOGIC_ERROR);
  }

  return CNetCDFFiles::GetTypeAsString(varType);
}

//----------------------------------------
nc_type CNetCDFFiles::GetAttributeType(const std::string& attName)
{
  return GetAttributeType(NC_GLOBAL, attName);
}
//----------------------------------------
nc_type CNetCDFFiles::GetAttributeType(const std::string& varName, const std::string& attName)
{
  return GetAttributeType(GetVarId(varName), attName);
}
//----------------------------------------
CNetCDFAttr* CNetCDFFiles::GetNetCDFGlobalAttribute(const std::string& name, bool withExcept /*= false*/)
{
  return CNetCDFAttr::GetNetCDFAttr(m_mapNetCDFGlobalAttrs.Exists(name), withExcept);
}
/*
//----------------------------------------
CNetCDFAttr* CNetCDFFiles::AddNetCDFGlobalAttributes(CNetCDFAttr& attr)
{
  CNetCDFAttr* newAttr = GetNetCDFGlobalAttribute(attr.GetName());

  if (newAttr != NULL)
  {
      return newAttr;
  }
  
  newAttr = attr.CloneThis();

  m_mapNetCDFGlobalAttrs.Insert(newAttr->GetName(), newAttr, false);

  return newAttr;

}
*/
//----------------------------------------
CNetCDFAttr* CNetCDFFiles::AddNetCDFGlobalAttributes(CNetCDFAttr* attr)
{
  m_mapNetCDFGlobalAttrs.Erase(attr->GetName());
  CNetCDFAttr* insertedAttr = CNetCDFAttr::GetNetCDFAttr(m_mapNetCDFGlobalAttrs.Insert(attr->GetName(), attr, false), false);;

  return insertedAttr;

}

//----------------------------------------
nc_type CNetCDFFiles::GetAttributeType(int varId, const std::string& attName)
{
  nc_type attType = NC_NAT;
  CheckNetcdfError(nc_inq_atttype(m_file,
				  varId,
                                  attName.c_str(),
				  &attType));
  return attType;
}
//----------------------------------------
CNetCDFVarDef* CNetCDFFiles::GetNetCDFVarDef(const std::string& name, bool withExcept /*= false*/)
{
  return CNetCDFVarDef::GetNetCDFVarDef(m_mapNetCDFVarDefs.Exists(name), withExcept);
}

//----------------------------------------
CNetCDFVarDef* CNetCDFFiles::AddNetCDFVarDef(CNetCDFVarDef& var, bool forceReplace /*= false */)
{  
  if (forceReplace)
  {
    m_mapNetCDFVarDefs.Erase(var.GetName());
  }


  CNetCDFVarDef* newVar = GetNetCDFVarDef(var.GetName());

  if (newVar != NULL)
  {
      return newVar;
  }
  
  newVar = var.CloneThis();

  m_mapNetCDFVarDefs.Insert(newVar->GetName(), newVar, false);

  return newVar;
}


//----------------------------------------
CNetCDFDimension* CNetCDFFiles::AddNetCDFDim(CNetCDFDimension& dim, bool forceReplace /*= false */)
{
  
  if (forceReplace)
  {
    m_mapNetCDFDims.Erase(dim.GetName());
  }

  CNetCDFDimension* newDim = GetNetCDFDim(dim.GetName());

  if (newDim != NULL)
  {
    return newDim;
  }
  
  newDim = dim.CloneThis();

  m_mapNetCDFDims.Insert(newDim->GetName(), newDim, false);

  return newDim;
}

//----------------------------------------
void CNetCDFFiles::SetNetCDFDims(const CStringArray* dimNames, const CUIntArray* dimValues, const CIntArray* dimIds /*= NULL*/)
{
  if (dimNames == NULL)
  {
    return;
  }
  if (dimValues == NULL)
  {
    return;
  }

  size_t size = dimNames->size();
  if (dimValues->size() != size)
  {
    return;
  }

  if (dimIds != NULL)
  {
    if (dimIds->size() != size)
    {
      return;
    }
  }

  for (uint32_t i = 0 ; i < size ; i++)
  {
    CNetCDFDimension netCDFDim(dimNames->at(i), dimValues->at(i));
    
    if (dimIds != NULL)
    {
      netCDFDim.SetDimId(dimIds->at(i));
    }

    AddNetCDFDim(netCDFDim);
  }
}
//----------------------------------------
bool CNetCDFFiles::HaveEqualDimNames(const std::string& v1, const CStringArray& v2, std::string* msg /*= NULL*/)
{ 
  CStringArray v1Array;
  v1Array.Insert(v1);
  return HaveEqualDimNames(v1Array, v2);
}
//----------------------------------------
bool CNetCDFFiles::HaveEqualDimNames(const CStringArray& v1, const CStringArray& v2, std::string* msg /*= NULL*/)
{ 
  uint32_t iv1 = 0;
  uint32_t iv2 = 0;

  bool bOk = true;

  for (iv1 = 0 ; iv1 < v1.size() ; iv1++)
  {
    for (iv2 = 0 ; iv2 < v2.size() ; iv2++)
    {
      bOk &= HaveEqualDimNames(v1.at(iv1), v2.at(iv2), msg);

      if (!bOk)
      {
        break;
      }
    }

    if (!bOk)
    {
      break;
    }
  }

  return bOk;
}
//----------------------------------------
bool CNetCDFFiles::HaveEqualDimNames(const std::string& v1, const std::string& v2, std::string* msg /*= NULL*/)
{ 
  CNetCDFVarDef* v1Def = this->GetNetCDFVarDef(v1);
  CNetCDFVarDef* v2Def = this->GetNetCDFVarDef(v2);

  if ((v1Def == NULL) || (v2Def == NULL))
  {
    return false;
  }
  return HaveEqualDimNames(v1Def, v2Def, msg);
}
//----------------------------------------
bool CNetCDFFiles::HaveEqualDimNames(CNetCDFVarDef* v1, CNetCDFVarDef* v2, std::string* msg /*= NULL*/)
{
  return v1->HaveEqualDimNames(v2, msg);
}

//----------------------------------------
uint32_t CNetCDFFiles::GetMaxFieldNumberOfDims(const CStringArray* fieldNames /* = NULL */)
{
  if (m_mapNetCDFDims.size() <= 0)
  {
    return 0;
  }

  uint32_t max = 0;
  uint32_t value = 0;

  CStringArray fieldNamesTmp;

  if (fieldNames != NULL)
  {
    fieldNamesTmp.Insert(*fieldNames);
  }
  else
  {
    GetVariables(fieldNamesTmp);
  }

  try
  {
    CStringArray::const_iterator it;
    for (it = fieldNamesTmp.begin() ; it != fieldNamesTmp.end() ; it++)
    {
      value = GetVarNbDims(*it);
  
      if (max < value)
      {
        max = value;
      }
    }
  }
  catch (CException& e)
  {
    e.what();
  }

  return max;
}
//----------------------------------------
void CNetCDFFiles::AddCoordinateVariable(const CStringArray* dimNames, const std::string& varName)
{
  if (dimNames == NULL)
  {
    return;
  }

  AddCoordinateVariable(*dimNames, varName);
}
//----------------------------------------
void CNetCDFFiles::AddCoordinateVariable(const CStringArray& dimNames, const std::string& varName)
{

  CStringArray::const_iterator it;
  for (it = dimNames.begin() ; it != dimNames.end(); it++)
  {
    CNetCDFDimension* netCDFDim = GetNetCDFDim(*it);
    if (netCDFDim == NULL)
    {
      continue;
    }
    netCDFDim->AddCoordinateVariable(varName);

  }


}

//----------------------------------------
int CNetCDFFiles::AddVar
		(const std::string	&VarName,
		       nc_type	VarType,
		 const std::string	&units,
		 const std::string	&longName,
		 const std::string	&comment /* = "" */,
		       int	Dim1Id	/*= -1*/,
		       int	Dim2Id	/*= -1*/,
		       int	Dim3Id	/*= -1*/,
		       int	Dim4Id	/*= -1*/,
      	               double	validMin /* = CTools::m_defaultValueDOUBLE */,
		       double	validMax /* = CTools::m_defaultValueDOUBLE */)
{
  int	DimIds[NC_MAX_VAR_DIMS];
  int   NbDims;
  int   varId;

  NbDims	= 0;
  if (Dim1Id >= 0) DimIds[NbDims++]	= Dim1Id;
  if (Dim2Id >= 0) DimIds[NbDims++]	= Dim2Id;
  if (Dim3Id >= 0) DimIds[NbDims++]	= Dim3Id;
  if (Dim4Id >= 0) DimIds[NbDims++]	= Dim4Id;
  SetDefineMode();
  CheckNetcdfError(nc_def_var(m_file,
			      VarName.c_str(),
			      VarType,
			      NbDims,
			      DimIds,
			      &varId),
		   CTools::Format("Error defining variable '%s' with %d dimensions",
				  VarName.c_str(),
				  NbDims));
  PutAtt(varId, FILL_VALUE_ATTR, GetDefValue(VarType), VarType);
  PutAtt(varId, LONG_NAME_ATTR, longName);
  PutAtt(varId, UNITS_ATTR, units);

  if (comment.empty() == false)
  {
    PutAtt(varId, COMMENT_ATTR, comment);
  }
  if (!isDefaultValue(validMin))
  {
    PutAtt(varId, VALID_MIN_ATTR, validMin);
  }
  if (!isDefaultValue(validMax))
  {
    PutAtt(varId, VALID_MAX_ATTR, validMax);
  }

  return varId;
}

//----------------------------------------

void CNetCDFFiles::DelAtt
		(const std::string	&VarName,
		 const std::string	&AttName)
{
  DelAtt(GetVarId(VarName, true), AttName);
}
//----------------------------------------

void CNetCDFFiles::DelAtt
		(int		varId,
		 const std::string	&AttName)
{
  MustBeOpened();
  SetDefineMode();
  int Status	= nc_del_att(m_file, varId, AttName.c_str());
  if ((Status != NC_NOERR) && (Status != NC_ENOTATT))
    CheckNetcdfError(Status,
		     CTools::Format("Error deleting attribute '%s' of var #%d",
				    AttName.c_str(),
				    varId));
}
//----------------------------------------

void CNetCDFFiles::PutAtt
		(const std::string	&VarName,
		 const std::string	&AttName,
		 const std::string	&AttValue)
{
  PutAtt(GetVarId(VarName, true), AttName, AttValue);
}
//----------------------------------------

void CNetCDFFiles::PutAtt
		(int		varId,
		 const std::string	&AttName,
		 const std::string	&AttValue)
{
  MustBeOpened();
  SetDefineMode();
  if (AttValue.length() == 0)
    DelAtt(varId, AttName);
  else
    CheckNetcdfError(nc_put_att_text(m_file,
				     varId,
				     AttName.c_str(),
				     AttValue.length(),
				     AttValue.c_str()));
}
//----------------------------------------

void CNetCDFFiles::PutAtt
		(const std::string	&VarName,
		 const std::string	&AttName,
		       double	AttValue,
		       nc_type	AttType		/*= NC_DOUBLE*/)
{
  PutAtt(GetVarId(VarName, true), AttName, AttValue, AttType);
}
//----------------------------------------

void CNetCDFFiles::PutAtt
		(int		varId,
		 const std::string	&AttName,
		       double	AttValue,
		       nc_type	AttType		/*= NC_DOUBLE*/)
{
  MustBeOpened();
  SetDefineMode();
  CheckNetcdfError(nc_put_att_double(m_file,
				     varId,
				     AttName.c_str(),
				     AttType,
				     1,
				     &AttValue));
}
//----------------------------------------

int CNetCDFFiles::GetAtt
		(const std::string	&VarName,
		 const std::string	&AttName,
		       std::string	&AttValue,
		       bool	MustExist	/*= true*/,
		 const std::string	Default		/*= ""*/)
{
  return GetAtt(GetVarId(VarName, true), AttName, AttValue, MustExist, Default);
}
//----------------------------------------

int CNetCDFFiles::GetAtt
		(int		varId,
		 const std::string	&AttName,
		       std::string	&AttValue,
		       bool	MustExist	/*= true*/,
		 const std::string	Default		/*= ""*/)
{
  MustBeOpened();

  int		Status;
  nc_type	Type;
  size_t	Length;

  Status	= nc_inq_att(m_file, varId, AttName.c_str(), &Type, &Length);
  if (Status != NC_NOERR)
  {
    if (MustExist || (Status != NC_ENOTATT))
      CheckNetcdfError(Status, CTools::Format("Error getting attribute '%s' of var #%d",
					      AttName.c_str(),
					      varId));
    else
    {
      AttValue.assign(Default);
      return Status;
    }
  }
  if (Type != NC_CHAR)
  {
    throw CFileException(CTools::Format("Attribute '%s' of var #%d is not a std::string: type = %d",
					AttName.c_str(),
					varId,
					Type),
			 m_fileName,
			 BRATHL_IO_ERROR);
  }

  char* AttStr = new char[Length+1];
  CheckNetcdfError(nc_get_att_text(m_file, varId, AttName.c_str(), AttStr));
  // Ensure that \0 is at the end and remove all trailing blanks
  // (std::string attributes can be fortran strings)
  AttStr[Length]	= '\0';
  while ((Length > 0) && (AttStr[Length-1] == ' '))
    AttStr[--Length] = '\0';
  AttValue.assign(AttStr);
  delete []AttStr;
  
  return NC_NOERR;

}
//----------------------------------------

int CNetCDFFiles::GetAtt
		(const std::string	&VarName,
		 const std::string	&AttName,
		       double	&AttValue,
		       bool	MustExist	/*= true*/,
		       double   Default		/*= CTools::m_defaultValueDOUBLE*/)
{
  return GetAtt(GetVarId(VarName, true), AttName, AttValue, MustExist, Default);
}
//----------------------------------------

int CNetCDFFiles::GetAtt
		(int		varId,
		 const std::string	&AttName,
		       double	&AttValue,
		       bool	MustExist	/*= true*/,
		       double   Default		/*= CTools::m_defaultValueDOUBLE*/)
{
  MustBeOpened();

  int		Status;
  nc_type	Type;
  size_t	Length;

  AttValue	= Default;
  Status	= nc_inq_att(m_file, varId, AttName.c_str(), &Type, &Length);
  if ((Status != NC_NOERR) || (Length <= 0))
  {
    if (MustExist || (Status != NC_ENOTATT))
    {
      CheckNetcdfError(Status, CTools::Format("Error getting attribute '%s' of var #%d",
					      AttName.c_str(),
					      varId));
    }
    else
    {
      return Status;
    }
  }
  if (Type == NC_CHAR)
  {
    throw CFileException(CTools::Format("Attribute '%s' of var #%d is a std::string, not a number",
					AttName.c_str(),
					varId),
			 m_fileName,
			 BRATHL_IO_ERROR);
  }
  if (Length != 1)
  {
    CheckNetcdfError(Status, CTools::Format("Attribute  '%s' of var #%d has %ld values not 1",
					      AttName.c_str(),
					      varId,
					      (long)Length));
  }

  CheckNetcdfError(nc_get_att_double(m_file, varId, AttName.c_str(), &AttValue));

  return NC_NOERR;  
}

//----------------------------------------
int CNetCDFFiles::GetAttributeInfo(const std::string	&varName, const std::string&  attName, nc_type&	type, size_t&	length, bool mustExist /*= true*/)
{
  return GetAttributeInfo(GetVarId(varName, true), attName, type, length, mustExist);
}

//----------------------------------------
int CNetCDFFiles::GetAttributeInfo(int varId, const std::string&  attName, nc_type& type, size_t&	length, bool mustExist /*= true*/)
{
  int status = nc_inq_att(m_file, varId, attName.c_str(), &type, &length);
  if (status != NC_NOERR)
  {
    if (mustExist || (status != NC_ENOTATT))
      CheckNetcdfError(status, CTools::Format("Error getting attribute '%s' of var #%d",
					      attName.c_str(),
					      varId));
  }
  return status;

}

//----------------------------------------

int CNetCDFFiles::GetGlobalAttributeAsDouble
		(const std::string	&attName,
		       double	&attValue,
		       bool	mustExist	/*= true*/,
		       double   defaultValue	/*= CTools::m_defaultValueDOUBLE*/)
{
  return GetAttributeAsDouble(NC_GLOBAL, attName, attValue, mustExist, defaultValue);
}

//----------------------------------------

int CNetCDFFiles::GetAttributeAsDouble
		(const std::string	&varName,
		 const std::string	&attName,
		       double	&attValue,
		       bool	mustExist	/*= true*/,
		       double   defaultValue	/*= CTools::m_defaultValueDOUBLE*/)
{
  return GetAttributeAsDouble(GetVarId(varName, true), attName, attValue, mustExist, defaultValue);
}
//----------------------------------------

int CNetCDFFiles::GetAttributeAsDouble
		(int		varId,
		 const std::string	&attName,
		       double	&attValue,
		       bool	mustExist	/*= true*/,
		       double   defaultValue	/*= CTools::m_defaultValueDOUBLE*/)
{
  MustBeOpened();

  int		status;
  nc_type	type;
  size_t	length;

  attValue	= defaultValue;
  status	= nc_inq_att(m_file, varId, attName.c_str(), &type, &length);
  if ((status != NC_NOERR) || (length <= 0))
  {
    if (mustExist || (status != NC_ENOTATT))
    {
      CheckNetcdfError(status, CTools::Format("Error getting attribute '%s' of var #%d",
					      attName.c_str(),
					      varId));
    }
    else
    {
      return status;
    }
  }

  if (type == NC_CHAR)
  {
    attValue = defaultValue;
    return NC2_ERR;
  }

  if (length != 1)
  {
    CheckNetcdfError(status, CTools::Format("Attribute  '%s' of var #%d has %ld values not 1",
					      attName.c_str(),
					      varId,
					      (long)length));
  }

  CheckNetcdfError(nc_get_att_double(m_file, varId, attName.c_str(), &attValue));

  return NC_NOERR;  
}
//----------------------------------------

int CNetCDFFiles::GetGlobalAttributeAsString
		(const std::string	&attName,
		       std::string	&attValue,
		       bool	mustExist	/*= true*/,
		       std::string   defaultValue	/*= ""*/)
{
  return GetAttributeAsString(NC_GLOBAL, attName, attValue, mustExist, defaultValue);
}

//----------------------------------------
int CNetCDFFiles::GetAttributeAsString
		(const std::string	&varName,
		 const std::string&	attName,
		       std::string&	attValue,
		       bool	mustExist	/*= true*/,
		 const std::string&	defaultValue		/*= ""*/)
{
  return GetAttributeAsString(GetVarId(varName, true), attName, attValue, mustExist, defaultValue);
}

//----------------------------------------
int CNetCDFFiles::GetAttributeAsString
		(int		varId,
		 const std::string&	attName,
		       std::string&	attValue,
		       bool	mustExist	/*= true*/,
		 const std::string&	defaultValue		/*= ""*/)
{
  MustBeOpened();

  nc_type	type;
  size_t	length;
  size_t i = 0;
  std::string tmp;

  int status = GetAttributeInfo(varId, attName, type, length, mustExist);

  if (status == NC_ENOTATT)
  {
    attValue.assign(defaultValue);
    return status;
  }
    
  switch (type) 
  {
    case NC_BYTE :
    {
      unsigned char *values = new unsigned char[length];;
      CheckNetcdfError(nc_get_att_uchar(m_file, varId, attName.c_str(), values));
      for (i = 0 ; i < length ; i++) 
      {
        attValue += CTools::Format("%u", values[i]);
        if (i != length - 1)
        {
          attValue += ",";
        }
      }
      delete []values;
      break;
    }
    case NC_SHORT :
    {
      short *values = new short[length];
      CheckNetcdfError(nc_get_att_short(m_file, varId, attName.c_str(), values));
      for (i = 0 ; i < length ; i++) 
      {
        attValue += CTools::Format("%d", values[i]);
        if (i != length - 1)
        {
          attValue += ",";
        }
      }
      delete []values;
      break;
    }
    case NC_INT :
    {
      int *values = new int[length];
      CheckNetcdfError(nc_get_att_int(m_file, varId, attName.c_str(), values));
      for (i = 0 ; i < length ; i++) 
      {
        attValue += CTools::Format("%d", values[i]);
        if (i != length - 1)
        {
          attValue += ",";
        }
      }
      delete []values;
      break;
    }
    case NC_FLOAT :
    {
      float *values = new float[length];
      CheckNetcdfError(nc_get_att_float(m_file, varId, attName.c_str(), values));
      for (i = 0 ; i < length ; i++) 
      {
        attValue += CTools::Format("%f", values[i]);
        if (i != length - 1)
        {
          attValue += ",";
        }
      }
      delete []values;
      break;
    }
    case NC_DOUBLE :
    {
      double *values = new double[length];
      CheckNetcdfError(nc_get_att_double(m_file, varId, attName.c_str(), values));
      for (i = 0 ; i < length ; i++) 
      {
        attValue += CTools::Format("%f", values[i]);
        if (i != length - 1)
        {
          attValue += ",";
        }
      }
      delete []values;
      break;
    }
    case NC_CHAR :
    { 
      char* attStr = new char[length+1];
      memset(attStr, '\0', length+1); 
      
      CheckNetcdfError(nc_get_att_text(m_file, varId, attName.c_str(), attStr));
      attStr[length] = '\0';
      CTools::Trim(attStr);
      
      attValue.assign(attStr);
      
      delete []attStr;

      break;
    }

    default : 
    {
      attValue.assign(defaultValue);
      break;
    }
  }

  return NC_NOERR;

}


//----------------------------------------

void CNetCDFFiles::GetAttributes
		(const std::string&  varName,
		 CStringMap&	mapAttributes)
{
  GetAttributes(GetVarId(varName, true), mapAttributes);
}

//----------------------------------------
void CNetCDFFiles::GetAttributes
		(const std::string&  varName,
		 CDoubleMap&	mapAttributes)
{
  GetAttributes(GetVarId(varName, true), mapAttributes);
}
//----------------------------------------
void CNetCDFFiles::GetAttributes
		(int		varId,
		 CStringMap&	mapAttributes)
{
  int nAtts;
  int status;
  char attName[NC_MAX_NAME+1];
 

  CheckNetcdfError(nc_inq_varnatts (m_file, varId, &nAtts));

  // The attributes for each variable are numbered from 0 (the first attribute) to natts-1, 
  // where natts is the number of attributes for the variable, as returned from a call to nc_inq_varnatts. 
  for (int i = 0 ; i < nAtts ; i++) 
  {
    memset(attName, '\0', NC_MAX_NAME+1); 
    
    CheckNetcdfError(nc_inq_attname(m_file, varId, i, attName));
    // Ensure that \0 is at the end and remove all trailing blanks
    attName[NC_MAX_NAME] = '\0';
    CTools::Trim(attName);
    
    std::string attValue;
    status = GetAttributeAsString(varId, attName, attValue, true, "value not found");
    if (status == NC_NOERR)
    {
      mapAttributes.Insert(attName, attValue);
    }

  }


}
//----------------------------------------
void CNetCDFFiles::GetAttributes
		(int		varId,
		 CDoubleMap&	mapAttributes)
{
  int nAtts;
  int status;
  char attName[NC_MAX_NAME+1];
 

  CheckNetcdfError(nc_inq_varnatts (m_file, varId, &nAtts));

  // The attributes for each variable are numbered from 0 (the first attribute) to natts-1, 
  // where natts is the number of attributes for the variable, as returned from a call to nc_inq_varnatts. 
  for (int i = 0 ; i < nAtts ; i++) 
  {
    memset(attName, '\0', NC_MAX_NAME+1); 
    
    CheckNetcdfError(nc_inq_attname(m_file, varId, i, attName));
    // Ensure that \0 is at the end and remove all trailing blanks
    attName[NC_MAX_NAME] = '\0';
    CTools::Trim(attName);
    
    double attValue;
    
    status = GetAttributeAsDouble(varId, attName, attValue);
    if (status == NC_NOERR)
    {
      mapAttributes.Insert(attName, attValue);
    }

  }


}

//----------------------------------------
CUnit CNetCDFFiles::GetUnit
		(const std::string			&Name)
{
  return GetUnit(GetVarId(Name));
}

//----------------------------------------

CUnit CNetCDFFiles::GetUnit
		(int				varId)
{
  std::string	UnitStr;
  GetAtt(varId, UNITS_ATTR, UnitStr, false, "count");
  return CUnit(UnitStr);
}

//----------------------------------------

std::string CNetCDFFiles::GetTitle
		(const std::string			&Name)
{
  int Id;
  std::string	Result;
  Id	= GetVarId(Name, true);
  if (Id == NC_GLOBAL)
    GetAtt(Id, FILE_TITLE_ATTR, Result, false);
  else
    GetAtt(Id, LONG_NAME_ATTR, Result, false);
  return Result;
}

//----------------------------------------

void CNetCDFFiles::CheckNetcdfError
		(int		Error,
		 const std::string	&Message)
{
  if (Error != NC_NOERR)
  {
    throw CFileException(CTools::Format("%s.\nNetcdf error: %s",
					Message.c_str(),
					nc_strerror(Error)),
			 m_fileName,
			 BRATHL_IO_ERROR);
  }
}

//----------------------------------------

void CNetCDFFiles::HandleNetcdfError
		(int32_t status,
		 const std::string& message)
{
  if (status != NC_NOERR)
  {
    throw CException(CTools::Format("%s.\nNetcdf error: %s",
					                          message.c_str(),
					                          nc_strerror(status)),
			 BRATHL_IO_ERROR);
  }
}

//----------------------------------------

NetCDFVarKind CNetCDFFiles::StringToVarKind
		(const std::string	&Name)
{
  std::string UpcaseName	= CTools::StringToUpper(Name);
  if (UpcaseName == "X")		return X;
  if (UpcaseName == "Y")		return Y;
  if (UpcaseName == "Z")		return Z;
  if (UpcaseName == "T")		return T;
  if (UpcaseName == "TIME")		return T;
  if (UpcaseName == "DATE")		return T;
  if (UpcaseName == "DATETIME")		return T;
  if (UpcaseName == "LATITUDE")		return Latitude;
  if (UpcaseName == "LONGITUDE")	return Longitude;
  if (UpcaseName == "DATA")		return Data;
  return Unknown;
}

//----------------------------------------

std::string CNetCDFFiles::VarKindToString
		(NetCDFVarKind	VarKind)
{
  switch (VarKind)
  {
    case Unknown:	return "";
    case X:		return "X";
    case Y:		return "Y";
    case Z:		return "Z";
    case T:		return "T";
    case Longitude:	return "Longitude";
    case Latitude:	return "Latitude";
    case Data:		return "Data";
    default:
	throw CException(CTools::Format("PROGRAM ERROR: CNetCDFFiles::VarKindToString: unknown kind %d",
					VarKind),
			 BRATHL_LOGIC_ERROR);
	return ""; // Never reached but avoids compiler complaint;
  }
}
//----------------------------------------

NetCDFVarKind CNetCDFFiles::GetVarKind
		(const std::string		&Name)
{
  if (VarExists(Name))
  {
    return GetVarKind(GetVarId(Name));
  }
  return Unknown;
}
//----------------------------------------
NetCDFVarKind CNetCDFFiles::GetVarKind(int varId)
{
  std::string	attVal;

  if (! IsAxisVar(varId))
  {
    return Data;
  }
  
  GetAtt(varId, STANDARD_NAME_ATTR, attVal, false);

  NetCDFVarKind	axisType	= GetVarKindFromStringValue(attVal);

  if (axisType != Unknown)
  {
    return axisType;
  }

  GetAtt(varId, AXIS_ATTR, attVal, false);

  axisType = StringToVarKind(attVal);

  if (axisType != Unknown)
  {
    return axisType;
  }
  
  std::string name = GetVarName(varId); 
  axisType	= GetVarKindFromStringValue(name);

  return axisType;

}
//----------------------------------------
NetCDFVarKind CNetCDFFiles::GetVarKindFromStringValue(const std::string& value)
{
  NetCDFVarKind	axisType = Unknown;

  if (value == "latitude")
  {
    axisType = Latitude;
  }
  else if (value == "longitude")
  {
    axisType = Longitude;
  }
  else if (value == "time")
  {
    axisType = T;
  }
  else if (value == "vertical")
  {
    axisType = Z;
  }
  else if (IsCandidateForLatitude(value))
  {
    axisType = Latitude;
  } 
  else if (IsCandidateForLongitude(value))
  {
    axisType = Longitude;
  } 
  else if (IsCandidateForTime(value))
  {
    axisType = T;
  } 

  return axisType;
}
//----------------------------------------
bool CNetCDFFiles::IsCandidateForLatitude(const std::string& name)
{
  bool candidate = false;

  for (int i = 0 ; i < CNetCDFFiles::m_LAT_NAMES_SIZE - 1 ; i++)
  {
    candidate = (name.compare(CNetCDFFiles::m_LAT_NAMES[i]) == 0);
    if (candidate) 
    {
      break;
    }

  }
  
  return candidate;

}
//----------------------------------------
bool CNetCDFFiles::IsCandidateForLongitude(const std::string& name)
{
  bool candidate = false;

  for (int i = 0 ; i < CNetCDFFiles::m_LON_NAMES_SIZE - 1 ; i++)
  {
    candidate = (name.compare(CNetCDFFiles::m_LON_NAMES[i]) == 0);
    if (candidate) 
    {
      break;
    }

  }

  return candidate;
}
//----------------------------------------
bool CNetCDFFiles::IsCandidateForTime(const std::string& name)
{
  bool candidate = false;

  for (int i = 0 ; i < CNetCDFFiles::m_TIME_NAMES_SIZE - 1 ; i++)
  {
    candidate = (name.compare(CNetCDFFiles::m_TIME_NAMES[i]) == 0);
    if (candidate) 
    {
      break;
    }

  }

  return candidate;
}
//----------------------------------------
bool CNetCDFFiles::HasVar (NetCDFVarKind	VarKind)
{
  for (int Index=0; Index<GetNbVariables(); ++Index)
  {
    if (GetVarKind(Index) == VarKind)
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------
CNetCDFCoordinateAxis* CNetCDFFiles::WhichCoordAxis(NetCDFVarKind varKind, CObMap* coordAxes)
{
  CNetCDFCoordinateAxis* coordAxis = NULL;

  CObMap::const_iterator it;

  for (it = coordAxes->begin() ; it != coordAxes->end(); it++)
  {
    coordAxis = dynamic_cast<CNetCDFCoordinateAxis*>(it->second);
    if (coordAxis == NULL)
    {
      continue;
    }

    if (coordAxis->GetDimKind() == varKind)
    {
      break;
    }

  }
  return coordAxis;
}
//----------------------------------------
CNetCDFCoordinateAxis* CNetCDFFiles::WhichCoordAxis(NetCDFVarKind varKind, const std::string& varName)
{
  CObMap coordAxes;
  
  GetNetCDFCoordAxes(varName, &coordAxes);

  return CNetCDFFiles::WhichCoordAxis(varKind, &coordAxes);

}
//----------------------------------------
CNetCDFCoordinateAxis* CNetCDFFiles::WhichCoordAxis(NetCDFVarKind varKind)
{
  return CNetCDFFiles::WhichCoordAxis(varKind, &m_mapNetCDFVarDefs);

}
//----------------------------------------

int CNetCDFFiles::WhichAxisVar (NetCDFVarKind	VarKind)
{
  for (int Index=0; Index<GetNbVariables(); ++Index)
  {
    if (IsAxisVar(Index))
    {
      if (GetVarKind(Index) == VarKind)
      {
	      return Index;
      }
    }
  }
  return -1;
}
//----------------------------------------
void CNetCDFFiles::ApplyUnitAndFactorsToDataRead
		(double		*Data,
		 uint32_t	NbData,
		 int		varId,
		 const std::string	&WantedUnit)
{
  
  double ScaleFactor = GetNetcdfVarScaleFactor(varId);
  double AddOffset = GetNetcdfVarAddOffset(varId);

  std::string UnitStr;
  GetAtt(varId, UNITS_ATTR, UnitStr, false, "");

  CUnit	Unit	= UnitStr;

  if ((WantedUnit != "") && (WantedUnit != CUnit::m_UNIT_SI))
  {
    Unit.SetConversionTo(WantedUnit);
  }
  else
  {  
    Unit.SetConversionToBaseUnit();
  }

  double DefValue	= GetNetcdfVarDefValue(varId);
  
  for (uint32_t Index=0; Index<NbData; ++Index)
  {
    if (Data[Index] == DefValue)
    {
      Data[Index]	= CTools::m_defaultValueDOUBLE;
    }
    else
    {
      Data[Index]	= Unit.Convert(Data[Index]*ScaleFactor + AddOffset);
    }
  }
}
//----------------------------------------
bool CNetCDFFiles::IsGeographic()
{
  return (WhichAxisVar(Latitude) >= 0) && (WhichAxisVar(Longitude) >= 0);
}



/*
//----------------------------------------
void CNetCDFFiles::ApplyUnitAndFactorsToDataRead
		(CDoubleArray& array,
		 int		varId,
		 const std::string& wantedUnit)
{
  CUnit	unit;
  double scaleFactor = GetNetcdfVarScaleFactor(varId);
  double addOffset = GetNetcdfVarAddOffset(varId);
  if (wantedUnit != "")
  {
    std::string unitStr;
    GetAtt(varId, UNITS_ATTR, unitStr, false, "");

    if (unitStr != "")
    {
      unit = unitStr;
      if (wantedUnit != "SI")
      {
        unit.SetConversionTo(wantedUnit);
      }
    }
  }
  double defValue = GetNetcdfVarDefValue(varId);
  
  CDoubleArray::iterator it;

  for (it = array.begin() ; it != array.end() ; it++)
  {
    if (*it == defValue)
    {
      *it = CTools::m_defaultValueDOUBLE;
    }
    else
    {
      *it = unit.Convert((*it)*scaleFactor + addOffset);
    }
  }
}
  */

//----------------------------------------
void CNetCDFFiles::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CNetCDFFiles Object at "<< this << std::endl;
  fOut << "m_file "<< m_file << std::endl;
  fOut << "m_fileName "<< m_fileName << std::endl;
  fOut << "m_fileMode "<< m_fileMode << std::endl;
  
  fOut << "m_mapNetCDFDims at "<< &m_mapNetCDFDims << std::endl;
  m_mapNetCDFDims.Dump(fOut);

  fOut << "m_mapNetCDFVarDefs at "<< &m_mapNetCDFVarDefs << std::endl;
  m_mapNetCDFVarDefs.Dump(fOut);
  
  fOut << "m_nDims "<< m_nDims << std::endl;
  fOut << "m_nGlobalAttrs "<< m_nGlobalAttrs << std::endl;
  fOut << "m_nVars "<< m_nVars << std::endl;
  fOut << "m_unlimitedDimId "<< m_unlimitedDimId << std::endl;

  fOut << "==> END Dump a CNetCDFFiles Object at "<< this << std::endl;

  fOut << std::endl;

}


} // namespace
