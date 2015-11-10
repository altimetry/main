
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

#include <cstdlib>
#include <cstdio>
#include <cstring> 

#include "brathl_error.h" 
#include "brathl.h" 
//#define BRAT_INTERNAL

#include "netcdf.h"

#include <string> 

#include "TraceLog.h" 
#include "Tools.h" 
#include "Exception.h" 


#include "CriteriaInfo.h" 
#include "Criteria.h" 


using namespace brathl;



namespace brathl
{
//-------------------------------------------------------------
//------------------- CFieldInfo class --------------------
//-------------------------------------------------------------



CFieldInfo::CFieldInfo() 
{

  Init();

}


//----------------------------------------
CFieldInfo::CFieldInfo(const std::string& name) 
{

  Init();
  SetName(name);

}

//----------------------------------------
CFieldInfo::CFieldInfo(CFieldInfo& f) 
{

  Init();
  Set(f);

}

//----------------------------------------

CFieldInfo::~CFieldInfo()
{

}

//----------------------------------------

void CFieldInfo::Init()
{
  m_isNetCdfVarAttribute = false;
  m_isNetCdfGlobalAttribute = false;
  m_isNetCdfVarValue = false;

//  m_dataType = NC_DOUBLE;
}
//----------------------------------------
const CFieldInfo& CFieldInfo::operator =(CFieldInfo& f)
{
  Set(f);  

  return *this;
   
}

//----------------------------------------
void CFieldInfo::Set(CFieldInfo& f) 
{

  Init();
  SetName(f.GetName());
  SetIsNetCdfVarAttribute(f.IsNetCdfVarAttribute());
  SetIsNetCdfGlobalAttribute(f.IsNetCdfGlobalAttribute());
  SetIsNetCdfVarValue(f.IsNetCdfVarValue());
  SetAttributeVarName(f.GetAttributeVarName());
  //SetDataType(f.GetDataType());

}


//----------------------------------------
void CFieldInfo::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CFieldInfo Object at "<< this << std::endl;

  fOut << "m_name "<< m_name << std::endl;
  fOut << "m_attributeVarName "<< m_attributeVarName << std::endl;
  fOut << "m_isNetCdfVarAttribute "<< m_isNetCdfVarAttribute << std::endl;
  fOut << "m_isNetCdfGlobalAttribute "<< m_isNetCdfGlobalAttribute << std::endl;
  fOut << "m_isNetCdfVarValue " << m_isNetCdfVarValue << std::endl;

  fOut << "==> END Dump a CFieldInfo Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CRecordDataMap class --------------------
//-------------------------------------------------------------

CObMap* CRecordDataMap::GetFields(const std::string& key, bool create /*= true*/)
{
    CObMap* fieldsInfo = NULL;
    CRecordData* recordData = CRecordData::GetRecordData(this->Exists(key), false);
    if (recordData != NULL)
    {
      fieldsInfo = recordData->GetFields();
    }
    else if (create)
    {
      CRecordData* recordData = new CRecordData(key);
      this->Insert(key, recordData);
      fieldsInfo = recordData->GetFields();
     }

    return fieldsInfo;
}
//----------------------------------------
void CRecordDataMap::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CRecordDataMap Object at "<< this << std::endl;

  this->Dump(fOut);
  fOut << "==> END Dump a CRecordDataMap Object at "<< this << std::endl;

  fOut << std::endl;

}
//-------------------------------------------------------------
//------------------- CRecordData class --------------------
//-------------------------------------------------------------
CRecordData* CRecordData::GetRecordData(CBratObject* ob, bool withExcept /*= true*/)
{
  CRecordData* recordData = dynamic_cast<CRecordData*>(ob);
  if (withExcept)
  {
    if (recordData == NULL)
    {
      CException e("CRecordData::GetFields - dynamic_cast<CRecordData*>(ob) returns NULL"
                   "object seems not to be an instance of CRecordData",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }

  return recordData;

}

//----------------------------------------
void CRecordData::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CRecordData Object at "<< this << std::endl;

  fOut << "m_name" << m_name << std::endl;
  fOut << "m_fields at " << &m_fields << std::endl;
  m_fields.Dump(fOut);
  fOut << "==> END Dump a CRecordData Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CCriteriaInfo class --------------------
//-------------------------------------------------------------


CCriteriaInfo::CCriteriaInfo() 
{

  Init();

}


//----------------------------------------

CCriteriaInfo::~CCriteriaInfo()
{

}

//----------------------------------------

void CCriteriaInfo::Init()
{
  m_key = CCriteria::UNKNOWN;
}

//----------------------------------------
void CCriteriaInfo::GetFields(CRecordDataMap& listRecord)
{
  CObMap* fieldsInfo = listRecord.GetFields(m_dataRecord, true);
  
  GetFieldsInfo(fieldsInfo);

}

//----------------------------------------
void CCriteriaInfo::GetFieldNames(CStringList& fieldNames)
{
  CObMap fieldsInfo;
  
  GetFieldsInfo(&fieldsInfo);

  fieldsInfo.GetKeys(fieldNames);

}

//----------------------------------------
void CCriteriaInfo::GetFieldNames(CStringArray& fieldNames)
{
  CObMap fieldsInfo;
  
  GetFieldsInfo(&fieldsInfo);

  fieldsInfo.GetKeys(fieldNames);

}

//----------------------------------------
CCriteriaInfo* CCriteriaInfo::GetCriteriaInfo(CBratObject* ob, bool withExcept /*= true*/)
{
  CCriteriaInfo* criteria = dynamic_cast<CCriteriaInfo*>(ob);
  if (withExcept)
  {
    if (criteria == NULL)
    {
      CException e("CCriteriaInfo::GetCriteriaInfo - dynamic_cast<CCriteriaInfo*>(ob) returns NULL"
                   "object seems not to be an instance of CCriteriaInfo",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return criteria;

}
//----------------------------------------
//CStringList* CCriteriaInfo::GetRecordDataFields(const std::string& key, CRecordDataMap& listRecord, bool create /*= true*/)
/*
{
  CStringList* fieldNames = NULL;
  CRecordData* recordData = CRecordData::GetRecordData(listRecord.Exists(key), false);
  if (recordData != NULL)
  {
    fieldNames = recordData->GetFields();
  }
  else if (create)
  {
    CRecordData* recordData = new CRecordData(key);
    listRecord.Insert(key, recordData);
    fieldNames = recordData->GetFields();
  }

  return fieldNames;

}
*/
//----------------------------------------
void CCriteriaInfo::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }
  fOut << "==> Dump a CCriteriaInfo Object at "<< this << std::endl;

  fOut << "m_key "<< m_key << std::endl;
  fOut << "m_dataRecord "<< m_dataRecord << std::endl;

  fOut << "==> END Dump a CCriteriaInfo Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CCriteriaDatetimeInfo class --------------------
//-------------------------------------------------------------


CCriteriaDatetimeInfo::CCriteriaDatetimeInfo() 
{

  Init();

}


//----------------------------------------

CCriteriaDatetimeInfo::~CCriteriaDatetimeInfo()
{

}

//----------------------------------------

void CCriteriaDatetimeInfo::Init()
{
  m_key = CCriteria::DATETIME;
  
  m_refDate = REF20000101;
}

//----------------------------------------
void CCriteriaDatetimeInfo::GetFieldsInfo(CObMap* fieldsInfo)
{
  if (fieldsInfo == NULL)
  {
    return;
  }

  if (!m_startDateField.GetName().empty())
  {
    fieldsInfo->Insert(m_startDateField.GetName(), new CFieldInfo(m_startDateField));
  }
  if (!m_endDateField.GetName().empty())
  {
    fieldsInfo->Insert(m_endDateField.GetName(), new CFieldInfo(m_endDateField));
  }
}
//----------------------------------------
CCriteriaDatetimeInfo* CCriteriaDatetimeInfo::GetCriteriaInfo(CBratObject* ob, bool withExcept /*= true*/)
{
  CCriteriaDatetimeInfo* criteria = dynamic_cast<CCriteriaDatetimeInfo*>(ob);
  if (withExcept)
  {
    if (criteria == NULL)
    {
      CException e("CCriteriaDatetimeInfo::GetCriteriaInfo - dynamic_cast<CCriteriaDatetimeInfo*>(ob) returns NULL"
                   "object seems not to be an instance of CCriteriaDatetimeInfo",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return criteria;

}
//----------------------------------------
void CCriteriaDatetimeInfo::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CCriteriaInfo::Dump(fOut);

  fOut << "==> Dump a CCriteriaDatetimeInfo Object at "<< this << std::endl;

  fOut << "m_startDateField at " << &m_startDateField << std::endl;
  m_startDateField.Dump(fOut);
  fOut << "m_endDateField at " << &m_endDateField << std::endl;
  m_endDateField.Dump(fOut);

  fOut << "==> END Dump a CCriteriaDatetimeInfo Object at "<< this << std::endl;

  fOut << std::endl;

}


//-------------------------------------------------------------
//------------------- CCriteriaLatLonInfo class --------------------
//-------------------------------------------------------------


CCriteriaLatLonInfo::CCriteriaLatLonInfo() 
{

  Init();

}


//----------------------------------------

CCriteriaLatLonInfo::~CCriteriaLatLonInfo()
{

}

//----------------------------------------

void CCriteriaLatLonInfo::Init()
{
  m_key = CCriteria::LATLON;
}
//----------------------------------------
void CCriteriaLatLonInfo::GetFieldsInfo(CObMap* fieldsInfo)
{
  if (fieldsInfo == NULL)
  {
    return;
  }

  if (!m_startLatField.GetName().empty())
  {
    fieldsInfo->Insert(m_startLatField.GetName(), new CFieldInfo(m_startLatField));
  }
  if (!m_endLatField.GetName().empty())
  {
    fieldsInfo->Insert(m_endLatField.GetName(), new CFieldInfo(m_endLatField));
  }
  if (!m_startLonField.GetName().empty())
  {
    fieldsInfo->Insert(m_startLonField.GetName(), new CFieldInfo(m_startLonField));
  }
  if (!m_endLonField.GetName().empty())
  {
    fieldsInfo->Insert(m_endLonField.GetName(), new CFieldInfo(m_endLonField));
  }
}
//----------------------------------------
CCriteriaLatLonInfo* CCriteriaLatLonInfo::GetCriteriaInfo(CBratObject* ob, bool withExcept /*= true*/)
{
  CCriteriaLatLonInfo* criteria = dynamic_cast<CCriteriaLatLonInfo*>(ob);
  if (withExcept)
  {
    if (criteria == NULL)
    {
      CException e("CCriteriaLatLonInfo::GetCriteriaInfo - dynamic_cast<CCriteriaLatLonInfo*>(ob) returns NULL"
                   "object seems not to be an instance of CCriteriaLatLonInfo",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return criteria;

}

//----------------------------------------
void CCriteriaLatLonInfo::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  CCriteriaInfo::Dump(fOut);

  fOut << "==> Dump a CCriteriaLatLonInfo Object at "<< this << std::endl;

  fOut << "m_startLatField at " << &m_startLatField << std::endl;
  m_startLatField.Dump(fOut);
  fOut << "m_endLatField at " << &m_endLatField << std::endl;
  m_endLatField.Dump(fOut);
  fOut << "m_startLonField at " << &m_startLonField << std::endl;
  m_startLonField.Dump(fOut);
  fOut << "m_endLonField at " << &m_endLonField << std::endl;
  m_endLonField.Dump(fOut);

  fOut << "==> END Dump a CCriteriaLatLonInfo Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CCriteriaPassInfo class --------------------
//-------------------------------------------------------------


CCriteriaPassInfo::CCriteriaPassInfo() 
{

  Init();

}


//----------------------------------------

CCriteriaPassInfo::~CCriteriaPassInfo()
{

}

//----------------------------------------

void CCriteriaPassInfo::Init()
{
  m_key = CCriteria::PASS;
}
//----------------------------------------
void CCriteriaPassInfo::GetFieldsInfo(CObMap* fieldsInfo)
{
  if (fieldsInfo == NULL)
  {
    return;
  }

  if (!m_startPassField.GetName().empty())
  {
    fieldsInfo->Insert(m_startPassField.GetName(), new CFieldInfo(m_startPassField));
  }

  if (!m_endPassField.GetName().empty())
  {
    fieldsInfo->Insert(m_endPassField.GetName(), new CFieldInfo(m_endPassField));
  }
}
//----------------------------------------
CCriteriaPassInfo* CCriteriaPassInfo::GetCriteriaInfo(CBratObject* ob, bool withExcept /*= true*/)
{
  CCriteriaPassInfo* criteria = dynamic_cast<CCriteriaPassInfo*>(ob);
  if (withExcept)
  {
    if (criteria == NULL)
    {
      CException e("CCriteriaPassInfo::GetCriteriaInfo - dynamic_cast<CCriteriaPassInfo*>(ob) returns NULL"
                   "object seems not to be an instance of CCriteriaPassInfo",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return criteria;

}

//----------------------------------------
void CCriteriaPassInfo::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CCriteriaInfo::Dump(fOut);

  fOut << "==> Dump a CCriteriaPassInfo Object at "<< this << std::endl;

  fOut << "m_startPassField at " << &m_startPassField << std::endl;
  m_startPassField.Dump(fOut);
  fOut << "m_endPassField at " << &m_endPassField << std::endl;
  m_endPassField.Dump(fOut);
  fOut << "==> END Dump a CCriteriaPassInfo Object at "<< this << std::endl;

  fOut << std::endl;

}


//-------------------------------------------------------------
//------------------- CCriteriaPassIntInfo class --------------------
//-------------------------------------------------------------

//----------------------------------------

void CCriteriaPassIntInfo::Init()
{

}
//----------------------------------------
CCriteriaPassIntInfo* CCriteriaPassIntInfo::GetCriteriaInfo(CBratObject* ob, bool withExcept /*= true*/)
{
  CCriteriaPassIntInfo* criteria = dynamic_cast<CCriteriaPassIntInfo*>(ob);
  if (withExcept)
  {
    if (criteria == NULL)
    {
      CException e("CCriteriaPassIntInfo::GetCriteriaInfo - dynamic_cast<CCriteriaPassIntInfo*>(ob) returns NULL"
                   "object seems not to be an instance of CCriteriaPassIntInfo",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return criteria;

}
//----------------------------------------
void CCriteriaPassIntInfo::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CCriteriaPassInfo::Dump(fOut);

  fOut << "==> Dump a CCriteriaPassIntInfo Object at "<< this << std::endl;
  fOut << "==> END Dump a CCriteriaPassIntInfo Object at "<< this << std::endl;

  fOut << std::endl;

}
//-------------------------------------------------------------
//------------------- CCriteriaPassStringInfo class --------------------
//-------------------------------------------------------------

//----------------------------------------

void CCriteriaPassStringInfo::Init()
{

}
//----------------------------------------
CCriteriaPassStringInfo* CCriteriaPassStringInfo::GetCriteriaInfo(CBratObject* ob, bool withExcept /*= true*/)
{
  CCriteriaPassStringInfo* criteria = dynamic_cast<CCriteriaPassStringInfo*>(ob);
  if (withExcept)
  {
    if (criteria == NULL)
    {
      CException e("CCriteriaPassStringInfo::GetCriteriaInfo - dynamic_cast<CCriteriaPassStringInfo*>(ob) returns NULL"
                   "object seems not to be an instance of CCriteriaPassStringInfo",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return criteria;

}
//----------------------------------------
void CCriteriaPassStringInfo::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CCriteriaPassInfo::Dump(fOut);

  fOut << "==> Dump a CCriteriaPassStringInfo Object at "<< this << std::endl;
  fOut << "==> END Dump a CCriteriaPassStringInfo Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CCriteriaCycleInfo class --------------------
//-------------------------------------------------------------


CCriteriaCycleInfo::CCriteriaCycleInfo() 
{

  Init();

}


//----------------------------------------

CCriteriaCycleInfo::~CCriteriaCycleInfo()
{

}

//----------------------------------------

void CCriteriaCycleInfo::Init()
{
  m_key = CCriteria::CYCLE;
}
//----------------------------------------
void CCriteriaCycleInfo::GetFieldsInfo(CObMap* fieldsInfo)
{
  if (fieldsInfo == NULL)
  {
    return;
  }
  if (!m_startCycleField.GetName().empty())
  {
    fieldsInfo->Insert(m_startCycleField.GetName(), new CFieldInfo(m_startCycleField));
  }
  if (!m_endCycleField.GetName().empty())
  {
    fieldsInfo->Insert(m_endCycleField.GetName(), new CFieldInfo(m_endCycleField));
  }
}
//----------------------------------------
CCriteriaCycleInfo* CCriteriaCycleInfo::GetCriteriaInfo(CBratObject* ob, bool withExcept /*= true*/)
{
  CCriteriaCycleInfo* criteria = dynamic_cast<CCriteriaCycleInfo*>(ob);
  if (withExcept)
  {
    if (criteria == NULL)
    {
      CException e("CCriteriaCycleInfo::GetCriteriaInfo - dynamic_cast<CCriteriaCycleInfo*>(ob) returns NULL"
                   "object seems not to be an instance of CCriteriaCycleInfo",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return criteria;

}

//----------------------------------------
void CCriteriaCycleInfo::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CCriteriaInfo::Dump(fOut);

  fOut << "==> Dump a CCriteriaCycleInfo Object at "<< this << std::endl;

  fOut << "m_startCycleField at " << &m_startCycleField << std::endl;
  
  m_startCycleField.Dump(fOut);

  fOut << "==> END Dump a CCriteriaCycleInfo Object at "<< this << std::endl;

  fOut << std::endl;

}






} // end namespace
