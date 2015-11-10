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
#include "Exception.h"
#include "Expression.h"

#include "ExternalFiles.h"

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;

namespace brathl
{

const int32_t CExternalFiles::m_EXCLUDE_ATTR_AS_FIELD_SIZE = 4;
const std::string CExternalFiles::m_EXCLUDE_ATTR_AS_FIELD[] = {FILL_VALUE_ATTR, 
                                                          SCALE_FACTOR_ATTR,
                                                          ADD_OFFSET_ATTR, 
                                                          UNITS_ATTR};

/*
const int32_t CExternalFiles::m_TIME_NAMES_SIZE = 6;
const std::string CExternalFiles::m_TIME_NAMES[] = {"time", "TIME", "Time",
                                               "date", "Date", "DATE",
                                              };
const int32_t CExternalFiles::m_LAT_NAMES_SIZE = 9;
const std::string CExternalFiles::m_LAT_NAMES[] = {"latitude", "Latitude", "LATITUDE",
                                              "lat", "Lat", "LAT",
                                              "latitudes", "Latitudes", "LATITUDES",
                                             };
const int32_t CExternalFiles::m_LON_NAMES_SIZE = 9;
const std::string CExternalFiles::m_LON_NAMES[] = {"longitude", "Longitude", "LONGITUDE",
                                              "lon", "Lon", "LON",
                                              "latitudes", "Latitudes", "LATITUDES",
                                              };

const int32_t CExternalFiles::m_CYCLE_NAMES_SIZE = 10;
const std::string CExternalFiles::m_CYCLE_NAMES[] = {"cycle", "Cycle", "CYCLE",
                                                "cycles", "Cycles", "CYCLEs",
                                                "cycle_number", "Cycle_number", "Cycle_Number", "CYCLE_NUMBER",
                                               };
const int32_t CExternalFiles::m_PASS_NAMES_SIZE = 13;
const std::string CExternalFiles::m_PASS_NAMES[] = {"pass", "Pass", "PASS",
                                               "tracks", "Tracks", "TRACKS",
                                               "track", "Track", "TRACK",
                                                "pass_number", "Pass_number", "Pass_Number", "PASS_NUMBER",
                                              };

*/


//-------------------------------------------------------------
//------------------- CExternalFiles class -----
//-------------------------------------------------------------



CExternalFiles::CExternalFiles()
{
  Init();
}

//----------------------------------------
CExternalFiles::~CExternalFiles()
{
  Close();
}

//----------------------------------------
void CExternalFiles::Init()
{
  SetProductClass(UNKNOWN_PRODUCT_CLASS);
  m_offset = 0.0;
}
//----------------------------------------
void CExternalFiles::MustBeOpened()
{
  if (! IsOpened())
  {
    throw CFileException("Try to do an operation on a closed file",
			 GetName(),
			 BRATHL_LOGIC_ERROR);
  }
}


//----------------------------------------
//void CExternalFiles::SetVector
//		(CDoubleArray& Vector,
//		 double			*Data,
//		 uint32_t		Count,
//		 bool			ExactSize	/*= false*/)
/*{
  CDoubleArray::iterator	Index;
  if (ExactSize || (Vector.size() < Count))
    Vector.resize(Count);
  for (Index=Vector.begin(); Count>0; Count--, Index++)
    *Index	= *(Data++);
}
*/
/*
//----------------------------------------
void CExternalFiles::InsertVector
		(CDoubleArray &array,
		 double	*data,
		 uint32_t count)
{
  for (uint32_t i = 0 ; i < count ; i++)
  {
    array.Insert( *(data++) );
  }
}
*/
//----------------------------------------
CField* CExternalFiles::GetField(CBratObject* ob, bool withExcept /*= true*/)
{
  CField* var = dynamic_cast<CField*>(ob);
  if (withExcept)
  {
    if (var == NULL)
    {
      CException e("CExternalFiles::GetField - dynamic_cast<CField*>(ob) returns NULL"
                   "object seems not to be an instance of CField",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return var;

}

//----------------------------------------
CFieldNetCdf* CExternalFiles::GetFieldNetCdf(CBratObject* ob, bool withExcept /*= true*/)
{
  CFieldNetCdf* var = dynamic_cast<CFieldNetCdf*>(ob);
  if (withExcept)
  {
    if (var == NULL)
    {
      CException e("CExternalFiles::GetFieldNetCdf - dynamic_cast<CFieldNetCdf*>(ob) returns NULL"
                   "object seems not to be an instance of CFieldNetCdf",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return var;

}

//----------------------------------------
CFieldNetCdfIndexData* CExternalFiles::GetFieldNetCdfIndexData(CBratObject* ob, bool withExcept /*= true*/)
{
  CFieldNetCdfIndexData* var = dynamic_cast<CFieldNetCdfIndexData*>(ob);
  if (withExcept)
  {
    if (var == NULL)
    {
      CException e("CExternalFiles::GetFieldNetCdfIndexData - dynamic_cast<CFieldNetCdfIndexData*>(ob) returns NULL"
                   "object seems not to be an instance of CFieldNetCdfIndexData",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return var;

}

//----------------------------------------
bool CExternalFiles::IsExcludedAttrAsField(const std::string& name)
{
  bool candidate = false;

  for (int i = 0 ; i < CExternalFiles::m_EXCLUDE_ATTR_AS_FIELD_SIZE ; i++)
  {
    candidate = (name.compare(CExternalFiles::m_EXCLUDE_ATTR_AS_FIELD[i]) == 0);
    if (candidate) 
    {
      break;
    }

  }

  return candidate;
}

//----------------------------------------
void CExternalFiles::AddOffset(double value)
{
  m_offset += value;
}
//----------------------------------------
//CExternalFileFieldDescription* CExternalFiles::GetRegisteredVarDescription(CBratObject* ob, bool withExcept /*= true*/)
/*
{
  CRegisteredVar* var = CExternalFiles::GetRegisteredVar(ob, withExcept);
  if (var == NULL)
  {
    return NULL;
  }

  return &(var->GetExternalFileFieldDescription());

}
*/
}

