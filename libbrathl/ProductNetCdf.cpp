
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

#include <string>

#include "TraceLog.h"
#include "Tools.h"
#include "Exception.h"
#include "ProductNetCdf.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;


namespace brathl
{



const std::string CProductNetCdf::m_virtualRecordName = "data";

/*
const int32_t CProductNetCdf::m_TIME_NAMES_SIZE = 6;
const std::string CProductNetCdf::m_TIME_NAMES[] = {"time", "TIME", "Time",
                                               "date", "Date", "DATE",
                                              };
const int32_t CProductNetCdf::m_LAT_NAMES_SIZE = 9;
const std::string CProductNetCdf::m_LAT_NAMES[] = {"latitude", "Latitude", "LATITUDE",
                                              "lat", "Lat", "LAT",
                                              "latitudes", "Latitudes", "LATITUDES",
                                             };
const int32_t CProductNetCdf::m_LON_NAMES_SIZE = 9;
const std::string CProductNetCdf::m_LON_NAMES[] = {"longitude", "Longitude", "LONGITUDE",
                                              "lon", "Lon", "LON",
                                              "latitudes", "Latitudes", "LATITUDES",
                                              };

const int32_t CProductNetCdf::m_CYCLE_NAMES_SIZE = 10;
const std::string CProductNetCdf::m_CYCLE_NAMES[] = {"cycle", "Cycle", "CYCLE",
                                                "cycles", "Cycles", "CYCLEs",
                                                "cycle_number", "Cycle_number", "Cycle_Number", "CYCLE_NUMBER",
                                               };
const int32_t CProductNetCdf::m_PASS_NAMES_SIZE = 13;
const std::string CProductNetCdf::m_PASS_NAMES[] = {"pass", "Pass", "PASS",
                                               "tracks", "Tracks", "TRACKS",
                                               "track", "Track", "TRACK",
                                                "pass_number", "Pass_number", "Pass_Number", "PASS_NUMBER",
                                              };
*/

CProductNetCdf::CProductNetCdf()
{
  Init();
}


//----------------------------------------

CProductNetCdf::CProductNetCdf(const std::string& fileName)
      : CProduct(fileName)
{
  Init();
}


//----------------------------------------
CProductNetCdf::CProductNetCdf(const CStringList& fileNameList)
      : CProduct(fileNameList)

{
  Init();
}

//----------------------------------------

CProductNetCdf::~CProductNetCdf()
{
  Close();
  DeleteFieldsToReadMap();
  DeleteExternalFile();
}
//----------------------------------------
void CProductNetCdf::Init()
{
  m_label = "Other Netcdf products";

  InitDateRef();

  m_externalFile = NULL;
  m_currentRecord = -1;
  //m_oneRecordsInMemory = false;
  m_forceReadDataOneByOne = false;
  //m_forceReadDataComplementDimsOneByOne = false;

  m_fieldsToRead = new CObMap(false); // false : CFieldNetCdf objects stored in this map have not to be delete (they are not a copy !!!)

  m_applyNetcdfProductInitialisation = true;

  InitCriteriaInfo();

}

//----------------------------------------
void CProductNetCdf::InitCriteriaInfo()
{
  CProduct::InitCriteriaInfo();

  if (m_fileList.empty())
  {
    return;
  }

  if (IsOpened() == false)
  {
    if (m_currFileName.empty())
    {
      Open(*(m_fileList.begin()));
    }
    else
    {
      Open();
    }
  }

  CFieldNetCdf* fieldLat = FindLatField();
  CFieldNetCdf* fieldLon = FindLonField();
  CFieldNetCdf* fieldTime = FindTimeField();
  CFieldNetCdf* fieldPass = FindPassField();
  CFieldNetCdf* fieldCycle = FindCycleField();

  if ( (fieldLat != NULL) && (fieldLon != NULL) )
  {
    m_longitudeFieldName = fieldLon->GetName();
    m_latitudeFieldName = fieldLat->GetName();

    //-------------------------
    // Lat/Lon criteria info
    //-------------------------
    CCriteriaLatLonInfo* criteriaLatLonInfo = new CCriteriaLatLonInfo();

    criteriaLatLonInfo->SetDataRecord(CProductNetCdf::m_virtualRecordName);

    criteriaLatLonInfo->SetStartLatField(fieldLat->GetName());
    criteriaLatLonInfo->GetStartLatField()->SetIsNetCdfVarValue(true);

    criteriaLatLonInfo->SetEndLatField(fieldLat->GetName());
    criteriaLatLonInfo->GetEndLatField()->SetIsNetCdfVarValue(true);

    criteriaLatLonInfo->SetStartLonField(fieldLon->GetName());
    criteriaLatLonInfo->GetStartLonField()->SetIsNetCdfVarValue(true);

    criteriaLatLonInfo->SetEndLonField(fieldLon->GetName());
    criteriaLatLonInfo->GetEndLonField()->SetIsNetCdfVarValue(true);

    m_criteriaInfoMap.Insert(criteriaLatLonInfo->GetKey(), criteriaLatLonInfo);


  }

  if (fieldTime != NULL)
  {
    //-------------------------
    // Datetime criteria info
    //-------------------------
    CCriteriaDatetimeInfo* criteriaDatetimeInfo = new CCriteriaDatetimeInfo();

    criteriaDatetimeInfo->SetDataRecord(CProductNetCdf::m_virtualRecordName);

    criteriaDatetimeInfo->SetStartDateField(fieldTime->GetName());
    criteriaDatetimeInfo->GetStartDateField()->SetIsNetCdfVarValue(true);

    criteriaDatetimeInfo->SetEndDateField(fieldTime->GetName());
    criteriaDatetimeInfo->GetEndDateField()->SetIsNetCdfVarValue(true);

    m_criteriaInfoMap.Insert(criteriaDatetimeInfo->GetKey(), criteriaDatetimeInfo);
  }

  if (fieldPass != NULL)
  {
    //-------------------------
    // Pass criteria info
    //-------------------------
    CCriteriaPassInfo* criteriaPassInfo = NULL;

    if (fieldPass->GetType() == NC_CHAR)
    {
      criteriaPassInfo = new CCriteriaPassStringInfo();
    }
    else
    {
      criteriaPassInfo = new CCriteriaPassIntInfo();
    }

    criteriaPassInfo->SetDataRecord(CProductNetCdf::m_virtualRecordName);

    criteriaPassInfo->SetStartPassField(fieldPass->GetName());
    criteriaPassInfo->GetStartPassField()->SetIsNetCdfVarValue(true);

    criteriaPassInfo->SetEndPassField(fieldPass->GetName());
    criteriaPassInfo->GetEndPassField()->SetIsNetCdfVarValue(true);

    m_criteriaInfoMap.Insert(criteriaPassInfo->GetKey(), criteriaPassInfo);
  }

  if (fieldCycle != NULL)
  {
    //-------------------------
    // Cycle criteria info
    //-------------------------
    CCriteriaCycleInfo* criteriaCycleInfo = new CCriteriaCycleInfo();

    criteriaCycleInfo->SetDataRecord(CProductNetCdf::m_virtualRecordName);

    criteriaCycleInfo->SetStartCycleField(fieldCycle->GetName());
    criteriaCycleInfo->GetStartCycleField()->SetIsNetCdfVarValue(true);

    m_criteriaInfoMap.Insert(criteriaCycleInfo->GetKey(), criteriaCycleInfo);
  }

}
//----------------------------------------
bool CProductNetCdf::IsLatField(CFieldNetCdf* field)
{
  bool bOk= false;

  if (IsOpened())
  {
    bOk = m_externalFile->IsLatField(field);
  }

  return bOk;
}//----------------------------------------
bool CProductNetCdf::IsLonField(CFieldNetCdf* field)
{
  bool bOk= false;

  if (IsOpened())
  {
    bOk = m_externalFile->IsLonField(field);
  }

  return bOk;
}
//----------------------------------------
CFieldNetCdf* CProductNetCdf::FindTimeField()
{
  CFieldNetCdf* field = NULL;

  if (IsOpened())
  {
    field = m_externalFile->FindTimeField();
  }

  return field;
}
//----------------------------------------
CFieldNetCdf* CProductNetCdf::FindLatField()
{
  CFieldNetCdf* field = NULL;

  if (IsOpened())
  {
    field = m_externalFile->FindLatField();
  }

  return field;
}
//----------------------------------------
CFieldNetCdf* CProductNetCdf::FindLonField()
{
  CFieldNetCdf* field = NULL;

  if (IsOpened())
  {
    field = m_externalFile->FindLonField();
  }

  return field;
}

//----------------------------------------
CFieldNetCdf* CProductNetCdf::FindPassField()
{
  CFieldNetCdf* field = NULL;

  if (IsOpened())
  {
    field = m_externalFile->FindPassField();
  }

  return field;
}

//----------------------------------------
CFieldNetCdf* CProductNetCdf::FindCycleField()
{
  CFieldNetCdf* field = NULL;
  if (IsOpened())
  {
    field = m_externalFile->FindCycleField();
  }

  return field;
}
//----------------------------------------
bool CProductNetCdf::GetLatLonMinMax(CLatLonRect& latlonRectMinMax)
{
  latlonRectMinMax.SetDefaultValue();

  if (!HasLatLonCriteriaInfo())
  {
    return false;
  }

  CCriteriaLatLonInfo * criteriaInfo = this->GetLatLonCriteriaInfo();
  if (criteriaInfo == NULL)
  {
    return false;
  }

  CProductList::iterator itFile;

  m_indexProcessedFile = 0;
  // Searches for each files
  for ( itFile = m_fileList.begin(); itFile != m_fileList.end(); itFile++ )
  {
    m_indexProcessedFile++;
    //CTrace::Tracer(1,"Process file %d of %d", (long)iFile, (long)m_fileList.size());

    this->Open(*itFile);

    double latMin = CTools::m_defaultValueDOUBLE;
    double lonMin = CTools::m_defaultValueDOUBLE;

    double latMax = CTools::m_defaultValueDOUBLE;
    double lonMax = CTools::m_defaultValueDOUBLE;

    // Gets start and end datetime
    CFieldInfo* fieldInfo = criteriaInfo->GetStartLatField();
    CFieldNetCdf* field = Read(*fieldInfo, latMin, true);

    // Optimization, to avoid to read same data again
    if (field != NULL)
    {
      latMax = field->GetValidMax();
    }
    else
    {
      fieldInfo = criteriaInfo->GetEndLatField();
      Read(*fieldInfo, latMax, false);
    }

    // Gets start and end  longitude
    fieldInfo = criteriaInfo->GetStartLonField();
    field = Read(*fieldInfo, lonMin, true);

    // Optimization, to avoid to read same data again
    if (field != NULL)
    {
      lonMax = field->GetValidMax();
    }
    else
    {
      fieldInfo = criteriaInfo->GetEndLonField();
      Read(*fieldInfo, lonMax, false);
    }

    CLatLonPoint left(latMin, lonMin);
    CLatLonPoint right(latMax, lonMax);

    CLatLonRect latLonRect(left, right);

    latlonRectMinMax.Extend(latLonRect);

    std::string str = latlonRectMinMax.AsString();

    this->Close();
  }

  m_indexProcessedFile = -1;

  return true;

}
//----------------------------------------
bool CProductNetCdf::GetDateMinMax(CDatePeriod& datePeriodMinMax)
{
  datePeriodMinMax.SetDefaultValue();

  if (!HasDatetimeCriteriaInfo())
  {
    return false;
  }

  CCriteriaDatetimeInfo * criteriaInfo = this->GetDatetimeCriteriaInfo();
  if (criteriaInfo == NULL)
  {
    return false;
  }

  CProductList::iterator itFile;

  m_indexProcessedFile = 0;
  // Searches for each files
  for ( itFile = m_fileList.begin(); itFile != m_fileList.end(); itFile++ )
  {
    m_indexProcessedFile++;
    //CTrace::Tracer(1,"Process file %d of %d", (long)m_indexProcessedFile, (long)m_fileList.size());

    this->Open(*itFile);

    CDate startDate;
    CDate endDate;

    // Gets start and end datetime
    CFieldInfo* fieldInfo = criteriaInfo->GetStartDateField();

    CFieldNetCdf* field = ReadDateCriteriaValue(*fieldInfo, startDate, true);

    // Optimization, to avoid to read same data again
    if (field != NULL)
    {
      /*
      CUnit* unit = field->GetNetCdfUnit();
      unit->SetConversionToBaseUnit();

      endDate = unit->Convert(field->GetValidMax());
      */

      endDate = field->GetValidMax();

    }
    else
    {
      fieldInfo = criteriaInfo->GetEndDateField();
      ReadDateCriteriaValue(*fieldInfo, endDate, false);
    }

    std::string str = startDate.AsString();
    str = endDate.AsString();

    datePeriodMinMax.Union(startDate, endDate);

    this->Close();
  }

  m_indexProcessedFile = -1;
  return true;

}
//----------------------------------------
void CProductNetCdf::ApplyCriteria(CStringList& filteredFileList, const std::string& logFileName /* = "" */)
{
  if (!logFileName.empty())
  {
    CreateLogFile(logFileName);
  }

  InitApplyCriteriaStats();

  // Initializes files list corresponding to criteria (all files)
  // then files that does not correspond to criteria will be removed from the list.
  filteredFileList.Insert(m_fileList);

  ////CRecordDataMap listRecord;

  // Sets a list of fields to be read (fields of all criteria)
  // To optimize reading data, fields are organized by data record
  ////BuildCriteriaFieldsToRead(listRecord);

  CProductList::iterator itFile;

  m_indexProcessedFile = 0;

  // Searches for each files if it corresponds to criteria
  for ( itFile = m_fileList.begin(); itFile != m_fileList.end(); itFile++ )
  {
    bool fileOk = true;

    m_indexProcessedFile++;
    CTrace::Tracer(1,"Process file %ld of %ld", (long)m_indexProcessedFile, (long)m_fileList.size());

    try
    {
      Log("---------------------------");
      Log("File ", false);
      Log(*itFile);
      Log("---------------------------");
      this->Open(*itFile);
    }
    catch (CException e)
    {
      Log("Error while opening file:", false);
      Log(e.what());
      continue;
    }
    catch (...)
    {
      Log("Unknown error while opening file");
      continue;
    }

    // Tests each criteria
    CObIntMap::iterator itMapCritInfo;
    for ( itMapCritInfo = m_criteriaInfoMap.begin(); itMapCritInfo != m_criteriaInfoMap.end(); itMapCritInfo++ )
    {
      //m_criteriaInfoMap.Dump(*CTrace::GetDumpContext());

      try
      {
        CCriteriaInfo* criteriaInfo = CCriteriaInfo::GetCriteriaInfo(itMapCritInfo->second);

        // Tests Lat/Lon criteria
        bool latLonCriteriaOk = ApplyCriteriaLatLon(criteriaInfo);

        if (!latLonCriteriaOk)
        {
          filteredFileList.Erase(*itFile);
          fileOk = false;
          break;
        }

        // Tests Datetime criteria
        bool datetimeCriteriaOk = ApplyCriteriaDatetime(criteriaInfo);

        if (!datetimeCriteriaOk)
        {
          filteredFileList.Erase(*itFile);
          fileOk = false;
          break;
        }

        // Tests pass criteria
        bool passCriteriaOk = ApplyCriteriaPass(criteriaInfo);

        if (!passCriteriaOk)
        {
          filteredFileList.Erase(*itFile);
          fileOk = false;
          break;
        }

        // Tests cycle criteria
        bool cycleCriteriaOk = ApplyCriteriaCycle(criteriaInfo);

        if (!cycleCriteriaOk)
        {
          filteredFileList.Erase(*itFile);
          fileOk = false;
          break;
        }

      }
      catch (CException e)
      {
        Log("Error while processing file:", false);
        Log(e.what());
        fileOk = false;
        break;
      }
      catch (...)
      {
        Log("Unknown error while processing file");
        fileOk = false;
        break;
      }
    }

    LogSelectionResult(*itFile, fileOk);

    this->Close();
  }

  m_indexProcessedFile = -1;

  EndApplyCriteriaStats(filteredFileList);

  DeleteLogFile();

}

//----------------------------------------
bool CProductNetCdf::ApplyCriteriaLatLon(CCriteriaInfo* criteriaInfo)
{

  if (criteriaInfo == NULL)
  {
    return true;
  }

  CCriteria* criteria = this->GetCriteria(criteriaInfo);
  if (criteria == NULL)
  {
    return true;
  }

  CCriteriaLatLon* criteriaLatLon = CCriteriaLatLon::GetCriteria(criteria, false);

  if (criteriaLatLon == NULL)
  {
    return true;
  }
  if (criteriaLatLon->IsDefaultValue())
  {
    return true;
  }

  double latMin = CTools::m_defaultValueDOUBLE;
  double lonMin = CTools::m_defaultValueDOUBLE;

  double latMax = CTools::m_defaultValueDOUBLE;
  double lonMax = CTools::m_defaultValueDOUBLE;

  CCriteriaLatLonInfo* criteriaLatLonInfo = CCriteriaLatLonInfo::GetCriteriaInfo(criteriaInfo);

  // Gets start and end latitudes
  CFieldInfo* fieldInfo = criteriaLatLonInfo->GetStartLatField();
  CFieldNetCdf* field = Read(*fieldInfo, latMin, true);

  // Optimization, to avoid to read same data again
  if (field != NULL)
  {
    latMax = field->GetValidMax();
  }
  else
  {
    fieldInfo = criteriaLatLonInfo->GetEndLatField();
    Read(*fieldInfo, latMax, false);
  }


  // Gets start and end  longitude
  fieldInfo = criteriaLatLonInfo->GetStartLonField();
  field = Read(*fieldInfo, lonMin, true);

  // Optimization, to avoid to read same data again
  if (field != NULL)
  {
    lonMax = field->GetValidMax();
  }
  else
  {
    fieldInfo = criteriaLatLonInfo->GetEndLonField();
    Read(*fieldInfo, lonMax, false);
  }

  CLatLonPoint left(latMin, lonMin);
  CLatLonPoint right(latMax, lonMax);

  CLatLonRect latLonRect(left, right);
  CLatLonRect intersect;

  bool bOk = criteriaLatLon->Intersect(latLonRect, intersect);

  Log("Criteria Lat/Lon box: ", false);
  Log(criteriaLatLon->GetAsText());
  Log("Lat/Lon box: ", false);
  Log(latLonRect.GetAsText(), false);
  Log(". Intersect: ", false);
  Log(bOk, (bOk ? false : true));
  if (bOk)
  {
    Log(". Intersection box ", false);
    Log(intersect.GetAsText());
  }

  return bOk;

}

//----------------------------------------
bool CProductNetCdf::ApplyCriteriaDatetime(CCriteriaInfo* criteriaInfo)
{

  if (criteriaInfo == NULL)
  {
    return true;
  }

  CCriteria* criteria = this->GetCriteria(criteriaInfo);
  if (criteria == NULL)
  {
    return true;
  }

  CCriteriaDatetime* criteriaDatetime = CCriteriaDatetime::GetCriteria(criteria, false);

  if (criteriaDatetime == NULL)
  {
    return true;
  }
  if (criteriaDatetime->IsDefaultValue())
  {
    return true;
  }

  CCriteriaDatetimeInfo* criteriaDatetimeInfo = CCriteriaDatetimeInfo::GetCriteriaInfo(criteriaInfo);
//  CObMap* fields = m_externalFile->GetFields();

//  CFieldNetCdf *field = NULL;

//  CUnit* unit = NULL;

  CDate startDate;
  CDate endDate;

  // Gets start and end datetime
  CFieldInfo* fieldInfo = criteriaDatetimeInfo->GetStartDateField();

  CFieldNetCdf* field = ReadDateCriteriaValue(*fieldInfo, startDate, true);

  // Optimization, to avoid to read same data again
  if (field != NULL)
  {
    /*
    CUnit* unit = field->GetNetCdfUnit();
    unit->SetConversionToBaseUnit();

    endDate = unit->Convert(field->GetValidMax());
    */
    endDate = field->GetValidMax();

  }
  else
  {
    fieldInfo = criteriaDatetimeInfo->GetEndDateField();
    ReadDateCriteriaValue(*fieldInfo, endDate, false);
  }

  std::string str = startDate.AsString();
  str = endDate.AsString();

  CDatePeriod datePeriod(startDate, endDate);
  CDatePeriod intersect;

  bool bOk = criteriaDatetime->Intersect(datePeriod, intersect);

  Log("Criteria Date period: ", false);
  Log(criteriaDatetime->GetAsText());
  Log("File Date period: ", false);
  Log(datePeriod.GetAsText(), false);
  Log(". Intersect: ", false);
  Log(bOk, (bOk ? false : true));
  if (bOk)
  {
    Log(". Intersection box ", false);
    Log(intersect.GetAsText());
  }

  return bOk;

}

//----------------------------------------
CFieldNetCdf* CProductNetCdf::ReadDateCriteriaValue(CFieldInfo& fieldInfo, CDate& date, bool wantMin /*= true*/)
{

  CObMap* fields = m_externalFile->GetFields();
  CFieldNetCdf *field = NULL;

  int32_t dataType = NC_NAT;


  if (fieldInfo.IsNetCdfGlobalAttribute())
  {
    dataType = m_externalFile->GetAttributeType(fieldInfo.GetName());
  }
  else if (fieldInfo.IsNetCdfVarAttribute())
  {
    dataType = m_externalFile->GetAttributeType(fieldInfo.GetName(), fieldInfo.GetAttributeVarName());
  }
  else if (fieldInfo.IsNetCdfVarValue())
  {
    //dataType = m_externalFile->GetVarType(fieldInfo.GetName());
    field = CExternalFiles::GetFieldNetCdf(fields->Exists(fieldInfo.GetName()));

  }


  if (dataType == NC_CHAR)
  {
    std::string strValue;

    Read(fieldInfo, strValue);

    int32_t result = date.SetDate(strValue.c_str());
    if (result != BRATHL_SUCCESS)
    {
      std::string msg = CTools::Format("ERROR - CProductNetCdf::ReadDateCriteriaValue(CFieldInfo& fieldInfo, CDate& date) - invalid date value '%s' - Field info : name '%s' - Attribute Name '%s'",
                                  strValue.c_str(),
                                  fieldInfo.GetName().c_str(),
                                  fieldInfo.GetAttributeVarName().c_str()
                                  );
      CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), result);
      throw (e);
    }
  }
  else
  {
    double value = CTools::m_defaultValueDOUBLE;

    Read(fieldInfo, value, wantMin);

    //field = CExternalFiles::GetFieldNetCdf(fields->Exists(fieldInfo.GetName()), false);
    if (field != NULL)
    {
      /*
      unit = field->GetNetCdfUnit();
      unit->SetConversionToBaseUnit();

      date = unit->Convert(value);
      */
      date = value;
    }
    else
    {
      date = value;
    }
  }

  return field;
}
//----------------------------------------
bool CProductNetCdf::ApplyCriteriaPass(CCriteriaInfo* criteriaInfo)
{
  if (criteriaInfo == NULL)
  {
    return true;
  }

  CCriteria* criteria = this->GetCriteria(criteriaInfo);
  if (criteria == NULL)
  {
    return true;
  }

  CCriteriaPassInt* criteriaPassInt = CCriteriaPassInt::GetCriteria(criteria, false);

  if (criteriaPassInt != NULL)
  {
    return ApplyCriteriaPassInt(criteriaInfo);
  }

  CCriteriaPassString* criteriaPassString = CCriteriaPassString::GetCriteria(criteria, false);

  if (criteriaPassString != NULL)
  {
    return ApplyCriteriaPassString(criteriaInfo);
  }

  return true;
}
//----------------------------------------
bool CProductNetCdf::ApplyCriteriaPassInt(CCriteriaInfo* criteriaInfo)
{
  if (criteriaInfo == NULL)
  {
    return true;
  }

  CCriteria* criteria = this->GetCriteria(criteriaInfo);
  if (criteria == NULL)
  {
    return true;
  }

  CCriteriaPassInt* criteriaPass = CCriteriaPassInt::GetCriteria(criteria, false);

  if (criteriaPass == NULL)
  {
    return true;
  }
  if (criteriaPass->IsDefaultValue())
  {
    return true;
  }

  CCriteriaPassIntInfo* criteriaPassInfo = CCriteriaPassIntInfo::GetCriteriaInfo(criteriaInfo);

  double startPassValue = CTools::m_defaultValueDOUBLE;
  double endPassValue = CTools::m_defaultValueDOUBLE;

  // Gets start and end pass
  CFieldInfo* fieldInfo = criteriaPassInfo->GetStartPassField();

  CFieldNetCdf* field = ReadDoubleCriteriaValue(*fieldInfo, startPassValue, true);

  // Optimization, to avoid to read same data again
  if (field != NULL)
  {
    endPassValue = field->GetValidMax();
  }
  else
  {
    fieldInfo = criteriaPassInfo->GetEndPassField();
    if (!fieldInfo->GetName().empty())
    {
      ReadDoubleCriteriaValue(*fieldInfo, endPassValue, false);
    }
    else
    {
      endPassValue = startPassValue;
    }
  }

  CIntArray intersect;

  bool bOk = criteriaPass->Intersect(startPassValue, endPassValue, intersect);

  Log("Criteria Pass: ", false);
  Log(criteriaPass->GetAsText());
  Log("File Pass from: ", false);
  Log(startPassValue, false);
  Log(" Pass end: ", false);
  Log(endPassValue, false);
  Log(". Intersect: ", false);
  Log(bOk, (bOk ? false : true));
  if (bOk)
  {
    Log(". Intersection box ", false);
    Log(intersect.ToString());
  }

  return bOk;

}
//----------------------------------------
bool CProductNetCdf::ApplyCriteriaPassString(CCriteriaInfo* criteriaInfo)
{
  if (criteriaInfo == NULL)
  {
    return true;
  }

  CCriteria* criteria = this->GetCriteria(criteriaInfo);
  if (criteria == NULL)
  {
    return true;
  }

  CCriteriaPassString* criteriaPass = CCriteriaPassString::GetCriteria(criteria, false);

  if (criteriaPass == NULL)
  {
    return true;
  }
  if (criteriaPass->IsDefaultValue())
  {
    return true;
  }

  CCriteriaPassStringInfo* criteriaPassInfo = CCriteriaPassStringInfo::GetCriteriaInfo(criteriaInfo);

  std::string startPassValue = "";
  std::string endPassValue = "";

  // Gets start and end pass
  CFieldInfo* fieldInfo = criteriaPassInfo->GetStartPassField();

  std::string msg = CTools::Format("ERROR - CProductNetCdf::ApplyCriteriaPassString(CCriteriaInfo* criteriaInfo): Reading variable as std::string value is NOT IMPLEMENTED - Field Name '%s'",
                             fieldInfo->GetName().c_str());
  CUnImplementException e(msg, BRATHL_UNIMPLEMENT_ERROR);
  throw(e);


}


//----------------------------------------
CFieldNetCdf* CProductNetCdf::ReadDoubleCriteriaValue(CFieldInfo& fieldInfo, double& value, bool wantMin /*= true*/)
{

  CObMap* fields = m_externalFile->GetFields();
  CFieldNetCdf *field = NULL;

  int32_t dataType = NC_NAT;


  if (fieldInfo.IsNetCdfGlobalAttribute())
  {
    dataType = m_externalFile->GetAttributeType(fieldInfo.GetName());
  }
  else if (fieldInfo.IsNetCdfVarAttribute())
  {
    dataType = m_externalFile->GetAttributeType(fieldInfo.GetName(), fieldInfo.GetAttributeVarName());
  }
  else if (fieldInfo.IsNetCdfVarValue())
  {
    //dataType = m_externalFile->GetVarType(fieldInfo.GetName());
    field = CExternalFiles::GetFieldNetCdf(fields->Exists(fieldInfo.GetName()));

  }


  if (dataType == NC_CHAR)
  {
    std::string strValue;
    Read(fieldInfo, strValue);
    value = CTools::StrToDouble(strValue);

    if (CTools::IsDefaultValue(value))
    {
      std::string msg = CTools::Format("ERROR - CProductNetCdf::ReadDoubleCriteriaValue(CFieldInfo& fieldInfo, double& date) - invalid 'double' value '%s' - Field info : name '%s' - Attribute Name '%s'",
                                  strValue.c_str(),
                                  fieldInfo.GetName().c_str(),
                                  fieldInfo.GetAttributeVarName().c_str()
                                  );
      CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), errno);
      throw (e);
    }
  }
  else
  {
    value = CTools::m_defaultValueDOUBLE;

    Read(fieldInfo, value, wantMin);

  }

  return field;
}

//----------------------------------------
bool CProductNetCdf::ApplyCriteriaCycle(CCriteriaInfo* criteriaInfo)
{
  if (criteriaInfo == NULL)
  {
    return true;
  }

  CCriteria* criteria = this->GetCriteria(criteriaInfo);
  if (criteria == NULL)
  {
    return true;
  }

  CCriteriaCycle* criteriaCycle = CCriteriaCycle::GetCriteria(criteria, false);

  if (criteriaCycle == NULL)
  {
    return true;
  }
  if (criteriaCycle->IsDefaultValue())
  {
    return true;
  }

  CCriteriaCycleInfo* criteriaCycleInfo = CCriteriaCycleInfo::GetCriteriaInfo(criteriaInfo);

  double startCycleValue = CTools::m_defaultValueDOUBLE;
  double endCycleValue = CTools::m_defaultValueDOUBLE;

  // Gets start and end pass
  CFieldInfo* fieldInfo = criteriaCycleInfo->GetStartCycleField();

  CFieldNetCdf* field = ReadDoubleCriteriaValue(*fieldInfo, startCycleValue, true);

  // Optimization, to avoid to read same data again
  if (field != NULL)
  {
    endCycleValue = field->GetValidMax();
  }
  else
  {
    fieldInfo = criteriaCycleInfo->GetEndCycleField();
    if (!fieldInfo->GetName().empty())
    {
      ReadDoubleCriteriaValue(*fieldInfo, endCycleValue, false);
    }
    else
    {
      endCycleValue = startCycleValue;
    }
  }

  CIntArray intersect;

  bool bOk = criteriaCycle->Intersect(startCycleValue, endCycleValue, intersect);

  Log("Criteria Cycle: ", false);
  Log(criteriaCycle->GetAsText());
  Log("File Cycle from: ", false);
  Log(startCycleValue, false);
  Log(" Cycle end: ", false);
  Log(endCycleValue, false);
  Log(". Intersect: ", false);
  Log(bOk, (bOk ? false : true));
  if (bOk)
  {
    Log(". Intersection box ", false);
    Log(intersect.ToString());
  }

  return bOk;

}

//----------------------------------------
void CProductNetCdf::DeleteFieldsToReadMap()
{
  if (m_fieldsToRead != NULL)
  {
    delete m_fieldsToRead;
    m_fieldsToRead = NULL;
  }
}
//----------------------------------------
void CProductNetCdf::DeleteExternalFile()
{
  if (m_externalFile != NULL)
  {
    m_externalFile->Close();
    delete m_externalFile;
    m_externalFile = NULL;
  }

}

//----------------------------------------
void CProductNetCdf::InitDateRef()
{
  m_refDate = REF19500101;
}

//----------------------------------------
std::string CProductNetCdf::MakeInternalFieldName(const std::string& dataSetName, const std::string& field)
{
  return MakeInternalFieldName(field);
}
//----------------------------------------
std::string CProductNetCdf::MakeInternalFieldName(const std::string& field)
{
  //For NetCdf there is no dataset name (no record Brathl parameter)
  return CProduct::m_treeRootName + "." + field;
}

//----------------------------------------

bool CProductNetCdf::Close()
{
  if (m_externalFile != NULL)
  {
    m_externalFile->Close();
  }

  m_currentRecord = -1;

  m_axisDims.RemoveAll();
  m_complementDims.RemoveAll();

  return true;
}

//----------------------------------------
void CProductNetCdf::InitInternalFieldName(CStringList& listField, bool convertDate /*= false*/)
{
  InitInternalFieldName(m_dataSetNameToRead, listField, convertDate);
}

//----------------------------------------
void CProductNetCdf::InitInternalFieldName(const std::string& dataSetName, CStringList& listField, bool convertDate /*= false*/)
{
  CProduct::InitInternalFieldName(dataSetName, listField, convertDate);

  CStringMap::iterator it;
  CObMap* fields = m_externalFile->GetFields();

  m_fieldsToRead->RemoveAll();

  // Push dimension index for each field to read
  for (it = m_fieldNameEquivalence.begin() ; it != m_fieldNameEquivalence.end() ; it++)
  {
    CFieldNetCdf *field = CExternalFiles::GetFieldNetCdf(fields->Exists(it->first));
    field->SetConvertDate(convertDate);
    m_fieldsToRead->Insert(it->first, field);
  }

}
/*
//----------------------------------------
CField* CProductNetCdf::GetFieldRead(const std::string& fieldName)
{
  return dynamic_cast<CField*> (m_fieldsToRead->Exists(fieldName));
}
*/

//----------------------------------------
bool CProductNetCdf::Open(const std::string& fileName, const std::string& dataSetName, CStringList& listFieldToRead)
{
  return CProduct::Open(fileName, dataSetName, listFieldToRead);
}
//----------------------------------------
bool CProductNetCdf::Open(const std::string& fileName, const std::string& dataSetName)
{
  return CProduct::Open(fileName, dataSetName);
}
//----------------------------------------
bool CProductNetCdf::Open(const std::string& fileName)
{
  return CProduct::Open(fileName);
}

//----------------------------------------
bool CProductNetCdf::Open()
{
  // Close and delete
  Close();
  DeleteExternalFile();

  if (m_currFileName.empty())
  {
    CFileException e("CProductNetCdf::Open - Error opening file -  file name is empty", BRATHL_IO_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);
  }

  m_externalFile = dynamic_cast<CExternalFilesNetCDF*>(BuildExistingExternalFileKind(m_currFileName.c_str()));
  if (m_externalFile == NULL)
  {
    CFileException e(CTools::Format("CProductNetCdf::Open - Unknown product/unknown netcdf type - File %s", m_currFileName.c_str()), BRATHL_IO_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);
  }

  m_externalFile->Open();

  m_externalFile->SetOffset(m_offset);

  //Loads data dictionary.
  LoadFieldsInfo();

  FillDescription();


  return true;

}

//----------------------------------------
int32_t CProductNetCdf::GetNumberOfRecords()
{
  if (IsOpened() == false)
  {
    return -1;
  }

  return m_externalFile->NumberOfRecords();
}

//----------------------------------------
int32_t CProductNetCdf::GetNumberOfRecords(const std::string& dataSetName /*NOT USED*/)
{
  return GetNumberOfRecords();
}



//----------------------------------------
// True while there is something to read
bool CProductNetCdf::NextRecord ()
{
  return m_externalFile->NextRecord();
}
//----------------------------------------
// True while there is something to read
bool CProductNetCdf::PrevRecord ()
{
  return m_externalFile->PrevRecord();
}
//----------------------------------------
void CProductNetCdf::RewindInit ()
{
  m_externalFile->Rewind();

  m_nbRecords = -1;
  m_recordCount = 0;
  m_traceProcessRecordRatio = 1;

  m_dataSet.RemoveAll();

}
//----------------------------------------
void CProductNetCdf::RewindEnd ()
{

  m_countForTrace = static_cast<uint32_t>(CTools::Round(GetNumberOfRecords() / 4));
  if (m_countForTrace == 0)
  {
    m_countForTrace = 1;
  }

}
//----------------------------------------
void CProductNetCdf::RewindProcess ()
{

  InitDataset();

}
//----------------------------------------
void CProductNetCdf::Rewind ()
{
  RewindInit();

  RewindProcess();

  RewindEnd();

}
//----------------------------------------
void CProductNetCdf::InitDataset()
{
  m_dataSet.RemoveAll();
  m_recordCount = 0;

  CreateFieldSets();

}

//----------------------------------------
void CProductNetCdf::SetOffset(double value) 
{
  CProduct::SetOffset(value);

  if (m_externalFile != NULL)
  {
    m_externalFile->SetOffset(value);
  }
}
//----------------------------------------
void CProductNetCdf::AddOffset(double value, CField* field /*= NULL*/)
{
  CProduct::AddOffset(value);

  if (m_externalFile != NULL)
  {
    m_externalFile->AddOffset(value);
  }

}

//----------------------------------------
void CProductNetCdf::ReadBratRecord(int32_t iRecord)
{

  CheckFileOpened();

  // If the file to open is the first in the list or if the file is not in the list
  // reset offset
  int32_t indexFile = m_fileList.FindIndex(m_currFileName);
  if (indexFile <= 0)
  {
    SetOffset(0.0);
  }

  CStringList::iterator itField;

 // m_oneRecordsInMemory = true;

  if ( (iRecord < 0) || (iRecord >= GetNumberOfRecords()) )
  {
    std::string msg = CTools::Format("ERROR in CProductNetCdf::ReadBratRecord - record index %d out of range (min = 0 and max = %d)",
                                iRecord,
                                GetNumberOfRecords() - 1);
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_RANGE_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);
  }

  if (m_currentRecord > iRecord)
  {
    m_currentRecord = -1;
    Rewind();
  }

//  if (m_currentRecord < 0)
//  {
//    Rewind();
//  }

  while (m_currentRecord < iRecord)
  {
    if (NextRecord() == false)
    {
      return;
    }
    m_currentRecord++;
  }

  InitDataset();

  if (!m_disableTrace)
  {
    if (CTools::Mod(m_currentRecord+1, m_countForTrace * m_traceProcessRecordRatio) == 0)
    {
      CTrace::Tracer(1,"Process record %d of %d", m_currentRecord+1, GetNumberOfRecords());
      m_traceProcessRecordRatio++;
    }
  }

  CObMap::iterator it;
  for (it = m_fieldsToRead->begin() ; it != m_fieldsToRead->end() ; it++)
  {
    CFieldNetCdf *field = CExternalFiles::GetFieldNetCdf(it->second);
    field->SetCurrentPos(m_currentRecord);


    CFieldSetArrayDbl* fieldSetArrayDbl = dynamic_cast<CFieldSetArrayDbl*>(m_dataSet.GetFieldSet(field->GetKey()));
/*
    if (fieldSetArrayDbl == NULL)
    {
      std::string msg = CTools::Format("ERROR in CProductNetCdf::ReadBratRecord - fieldSetArrayDbl not found for field '%s' at the record number %d of %d"
                                  "(fieldSetArrayDbl seems to be NULL or not a CFieldSetArrayDbl object",
                                  field->GetName().c_str(),
                                  iRecord,
                                  GetNumberOfRecords() - 1);
      CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
      throw (e);
    }
*/
    CFieldSetDbl* fieldSetDbl = dynamic_cast<CFieldSetDbl*>(m_dataSet.GetFieldSet(field->GetKey()));

/*
    if (fieldSetDbl == NULL)
    {
      std::string msg = CTools::Format("ERROR in CProductNetCdf::ReadBratRecord - fieldSetDbl not found for field '%s' at the record number %d of %d"
                                  "(fieldSetDbl seems to be NULL or not a CFieldSetDbl object",
                                  field->GetName().c_str(),
                                  iRecord,
                                  GetNumberOfRecords() - 1);
      CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
      throw (e);
    }
*/
    // Erase previous read data values
    //fieldSetArrayDbl->GetDataVector().RemoveAll();

    // -------------- Read data --------------------
    if (fieldSetArrayDbl != NULL)
    {
      Read(field, fieldSetArrayDbl->GetDataVector());
    }
    else if (fieldSetDbl != NULL)
    {
      Read(field, fieldSetDbl->GetDataRef());
    }
    //Read(field, fieldSetDbl->GetDataRef());

    // Insert a new Record or return record if exists
//    if (m_oneRecordsInMemory)
//    {
//      InsertRecord(0);
//    }
//    else
//    {
//      InsertRecord(field->GetCurrentPos());
//    }

  }

  m_recordCount += m_dataSet.size();

  if (iRecord >= (this->GetNumberOfRecords() - 1))
  {
    AddOffset(this->GetNumberOfRecords());
  }


}

//----------------------------------------
void CProductNetCdf::GetNetCdfDimensions(const std::vector<CExpression>& expressions, CStringArray& commonDimNames)
{
  GetNetCdfDimensions(expressions, commonDimNames, m_dataSetNameToRead);
}
//----------------------------------------
void CProductNetCdf::GetNetCdfDimensions(const CExpression& expr, CStringArray& commonDimNames)
{
  GetNetCdfDimensions(expr, commonDimNames, m_dataSetNameToRead);
}
//----------------------------------------
void CProductNetCdf::GetNetCdfDimensions(const CStringArray& fields, CStringArray& commonDimNames)
{
  GetNetCdfDimensions(fields, commonDimNames, m_dataSetNameToRead);
}
//----------------------------------------
void CProductNetCdf::GetNetCdfDimensions(const std::vector<CExpression>& expressions, CStringArray& commonDimNames, const std::string& recordName)
{
  std::vector<CExpression>::const_iterator it;

  for (it = expressions.begin() ; it != expressions.end() ; it++)
  {
    GetNetCdfDimensions(*it, commonDimNames);
  }

}
//----------------------------------------
void CProductNetCdf::GetNetCdfDimensions(const CExpression& expr, CStringArray& commonDimNames, const std::string& recordName)
{
   GetNetCdfDimensions(*(expr.GetFieldNames()), commonDimNames);
}
//----------------------------------------
void CProductNetCdf::GetNetCdfDimensionsWithoutAlgo(const std::vector<CExpression>& expressions, CStringArray& commonDimNames, const std::string& recordName)
{
  std::vector<CExpression>::const_iterator it;

  for (it = expressions.begin() ; it != expressions.end() ; it++)
  {
    GetNetCdfDimensionsWithoutAlgo(*it, commonDimNames, recordName);
  }

}//----------------------------------------
void CProductNetCdf::GetNetCdfDimensionsWithoutAlgo(const CExpression& expr, CStringArray& commonDimNames, const std::string& recordName)
{
  if (!expr.HasAlgorithms())
  {
    GetNetCdfDimensions(*(expr.GetFieldNames()), commonDimNames);
    return;
  }

  std::string exprString = expr.AsString();
  exprString = CTools::StringRemoveAllSpaces(exprString);

  const CVectorBratAlgorithm* algoArray = expr.GetAlgorithms();
  CVectorBratAlgorithm::const_iterator it;

  for (it = algoArray->begin() ; it != algoArray->end() ; it++)
  {
    CBratAlgorithmBase* algo = *it;
    std::string algoExpression = algo->GetAlgoExpression();
    algoExpression = CTools::StringRemoveAllSpaces(algoExpression);
    exprString = CTools::StringReplace(exprString, algoExpression, "1");
  }

  CExpression exprTmp;
  try
  {
    exprTmp.SetExpression(exprString);
  }
  catch (CException& e)
  {
    // Do nothing
    e.what(); // to avoid compilation warning

  }

  GetNetCdfDimensions(*(exprTmp.GetFieldNames()), commonDimNames);

}
//----------------------------------------
void CProductNetCdf::GetNetCdfDimensions(const CStringArray& fields, CStringArray& commonDimNames, const std::string& recordName)
{

  CStringArray::const_iterator it;

  for ( it = fields.begin( ); it != fields.end( ); it++ )
  {
    CFieldNetCdf* field = dynamic_cast<CFieldNetCdf*>(FindFieldByName(*it, recordName, false));
    if (field == NULL)
    {
      continue;
    }

    commonDimNames.InsertUnique( field->GetDimNames() );

  }



}
//----------------------------------------
void CProductNetCdf::LoadFieldsInfo()
{

  //CStringArray::iterator it;
  //CStringArray fieldsName;
  //m_externalFile->GetFieldNames(fieldsName);

  CObMap::iterator it;
  CObMap* fields = m_externalFile->GetFields();

  m_tree.DeleteTree();

  std::string desc = GetProductClass() + "/" + GetProductType();

  CFieldRecord *virtualFieldRoot = new CFieldRecord(fields->size(), m_treeRootName, desc);
  m_tree.SetRoot(m_treeRootName, virtualFieldRoot, true);


  for (it = fields->begin() ; it != fields->end() ; it++)
  {
    // Warning : clone a field that has not yet key set.
    CFieldNetCdf *fieldOriginal = CExternalFiles::GetFieldNetCdf(it->second);

    CFieldNetCdf *fieldCloned = fieldOriginal->CloneThis();
    //CFieldNetCdf *field = CExternalFiles::GetFieldNetCdf(it->second);

    // Add child add set the key to the coned field
    m_tree.AddChild(m_tree.GetRoot(), it->first, fieldCloned, true);

    //If field has a specific unit, loads and sets it.
    //This have to be set after m_tree.AddChild
    SetFieldSpecificUnit(fieldCloned);

    // Set key to the original field
    fieldOriginal->SetKey(fieldCloned->GetKey());
    SetFieldSpecificUnit(fieldOriginal);

  }

}

//----------------------------------------
/*
CFieldNetCdf* CProductNetCdf::CreateField(const std::string& fieldName)
{
  CExternalFileFieldDescription* fieldDescription = m_externalFile->GetFieldDescription(fieldName);

  return new CFieldNetCdf(fieldName,
                          fieldDescription->GetDescription(),
                          fieldDescription->GetUnit());
}
*/
//----------------------------------------
void CProductNetCdf::CreateFieldSets()
{
//  CStringArray::iterator it;
  //CStringArray fieldsName;
  //m_externalFile->GetFieldNames(fieldsName);


  m_dataSet.SetName("Dataset");

  // Insert a new Record at position 0,  or return record if exists
  InsertRecord(0);

  CObMap::iterator it;
  for (it = m_fieldsToRead->begin() ; it != m_fieldsToRead->end() ; it++)
  {
    CFieldNetCdf *field = CExternalFiles::GetFieldNetCdf(it->second);

    CFieldSet *fieldSet = m_dataSet.GetFieldSet(field->GetKey());

    if (fieldSet == NULL)
    {
      fieldSet = field->CreateFieldSet();
      m_dataSet.InsertFieldSet(field->GetKey(), fieldSet); //insert new CFieldSet in CRecordSet
    }
  }
}

//----------------------------------------
void CProductNetCdf::ReadBratFieldRecord(const std::string& key)
{

  CException e("ERROR - CProductNetCdf::ReadBratFieldRecord(const std::string& key) - unexpected call - CProductNetCdf::ReadBratFieldRecord(const std::string& key) have not to be called ", BRATHL_LOGIC_ERROR);
  throw (e);
/*
  m_listFields.RemoveAll();

  FillListFields(key);

  m_tree.SetWalkDownRootPivot();

  CField* field = NULL;
  bool bFoundField = false;

  do
  {
    bFoundField = false;
    bool bOk = m_tree.SubTreeWalkDown();
    if (bOk == false) // No more child
    {
      break;
    }

    field = m_tree.GetCurrentData();

    //if (field->GetName().compare(m_fieldsToProcess.front()) == 0)
    if (field->GetKey().compare(m_fieldsToProcess.front()) == 0)
    {
      bFoundField = true;
      m_listFields.InsertField(field, false);
      m_fieldsToProcess.pop_front();
    }
  }
  while (m_fieldsToProcess.empty() == false);

  if (bFoundField == false)
  {
    std::string msg = CTools::Format("ERROR - CProduct::ReadBratField() - Unknown dataset name or field name '%s'"
                                "(internal name searched:'%s'",
                                m_fieldsToProcess.front().c_str(),
                                key.c_str());
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer("%s", e.what());
    //Dump(*CTrace::GetDumpContext());
    throw (e);
  }


  CField::CListField::iterator it = m_listFields.begin();

  // If m_listFields contains only one item, there is no record, all fields are at the first level.
  if (m_listFields.size() == 1)
  {
    m_dataSet.SetName("Dataset");
  }
  else
  {
    field = static_cast<CField*>(*(it));
    m_dataSet.SetName(field->GetName());
  }


  ReadBratFieldRecord(it);

  return;
*/
}

//----------------------------------------
void CProductNetCdf::ReadBratFieldRecord(CField::CListField::iterator it)
{

  CException e("ERROR - CProductNetCdf::ReadBratFieldRecord(CField::CListField::iterator it) - unexpected call - CProductNetCdf::ReadBratFieldRecord(CField::CListField::iterator it) have not to be called ", BRATHL_LOGIC_ERROR);
  throw (e);
/*
  CFieldNetCdf *field = dynamic_cast<CFieldNetCdf*>(*(it));
  if (field == NULL)
  {
    CException e("ERROR in CProductNetCdf::ReadBratField :  dynamic_cast<CFieldNetCdf*>(*(it)) returns NULL pointer - "
                 "Field seems to be NULL or not a CFieldNetCdf object", BRATHL_LOGIC_ERROR);
    throw(e);
  }

//  field->SetExternalFile(m_externalFile);

  field->SetCurrentPos(m_currentRecord);

  CDoubleArray vect;
  Read(field, vect);

  // Insert a new Record or return record if exists
  if (m_oneRecordsInMemory)
  {
    InsertRecord(0);
  }
  else
  {
    InsertRecord(field->GetCurrentPos());
  }

  CFieldSet *fieldSet = m_dataSet.GetFieldSet(field->GetKey());

  if (fieldSet == NULL)
  {
    fieldSet = field->CreateFieldSet(m_listFields);

    m_dataSet.InsertFieldSet(field->GetKey(), fieldSet); //insert new CFieldSet in CRecordSet
  }

  fieldSet->Insert(vect, m_oneRecordsInMemory);

*/

}



//----------------------------------------
void CProductNetCdf::FillDescription()
{
  MustBeOpened();
  m_externalFile->GetGlobalAttributes(m_description);
}

//----------------------------------------
CFieldNetCdf* CProductNetCdf::Read(CFieldInfo& fieldInfo, double& value, bool wantMin /*= true*/)
{
  CFieldNetCdf *field = NULL;

  MustBeOpened();

  if (fieldInfo.IsNetCdfGlobalAttribute())
  {
    m_externalFile->GetGlobalAttribute(fieldInfo.GetName(), value);
  }
  else if (fieldInfo.IsNetCdfVarAttribute())
  {
    m_externalFile->GetAttribute(fieldInfo.GetName(), fieldInfo.GetAttributeVarName(), value);

  }
  else if (fieldInfo.IsNetCdfVarValue())
  {
    CObMap* fields = m_externalFile->GetFields();

    field = CExternalFiles::GetFieldNetCdf(fields->Exists(fieldInfo.GetName()));

    //CDoubleArray vect;

    ReadAll(field);

    if (wantMin)
    {
      value = field->GetValidMin();
    }
    else
    {
      value = field->GetValidMax();
    }

//    std::string msg = CTools::Format("ERROR - CProductNetCdf::Read(CFieldInfo& fieldInfo, double& value) : Reading variable value is NOT IMPLEMENTED - Field Name '%s'",
//                                fieldInfo.GetName().c_str());
//    CUnImplementException e(msg, BRATHL_UNIMPLEMENT_ERROR);
//    throw(e);
  }
  else
  {
      CException e(CTools::Format("ERROR - CProductNetCdf::Read(CFieldInfo& fieldInfo, double& value) - doesn't know what to read  - Field Name '%s'",
                                   fieldInfo.GetName().c_str()),
                   BRATHL_LOGIC_ERROR);
      throw(e);

  }

  return field;
}

//----------------------------------------
void CProductNetCdf::Read(CFieldInfo& fieldInfo, std::string& value)
{
  MustBeOpened();

  if (fieldInfo.IsNetCdfGlobalAttribute())
  {
    m_externalFile->GetGlobalAttribute(fieldInfo.GetName(), value);
  }
  else if (fieldInfo.IsNetCdfVarAttribute())
  {
    m_externalFile->GetAttribute(fieldInfo.GetName(), fieldInfo.GetAttributeVarName(), value);

  }
  else if (fieldInfo.IsNetCdfVarValue())
  {
    std::string msg = CTools::Format("ERROR - CProductNetCdf::Read(CFieldInfo& fieldInfo, std::string& value) : Reading variable as std::string value is NOT IMPLEMENTED - Field Name '%s'",
                                fieldInfo.GetName().c_str());
    CUnImplementException e(msg, BRATHL_UNIMPLEMENT_ERROR);
    throw(e);
  }
  else
  {
      CException e(CTools::Format("ERROR - CProductNetCdf::Read(CFieldInfo& fieldInfo, std::string& value) - doesn't know what to read  - Field Name '%s'",
                                   fieldInfo.GetName().c_str()),
                   BRATHL_LOGIC_ERROR);
      throw(e);

  }
}
//----------------------------------------
void CProductNetCdf::Read(CFieldNetCdf* field, double& value)
{
  CExpressionValue data;
  Read(field, data);

  int32_t dataSize = data.GetNbValues();
  if (dataSize <= 0)
  {
    CTools::SetDefaultValue(value);
  }
  else
  {
    value = data.GetValues()[0];
  }

}
//----------------------------------------
void CProductNetCdf::Read(CFieldNetCdf* field, CDoubleArray& vect)
{
  MustBeOpened();

  CExpressionValue data;
  Read(field, data);

  int32_t dataSize = data.GetNbValues();
  for (int32_t i = 0 ; i < dataSize ; i++)
  {
    vect.Insert(data.GetValues()[i]);
  }

}
//----------------------------------------
void CProductNetCdf::Read(CFieldNetCdf* field, CExpressionValue& value)
{
  MustBeOpened();

  m_externalFile->GetValues(field, value, CUnit::m_UNIT_SI);

  int32_t dataSize = value.GetNbValues();
  for (int32_t i = 0 ; i < dataSize ; i++)
  {
    field->AdjustValidMinMax(value.GetValues()[i]);
  }


}
//----------------------------------------
void CProductNetCdf::ReadAll(CFieldNetCdf* field)
{
  MustBeOpened();
  m_externalFile->GetAllValues(field, CUnit::m_UNIT_SI);

  field->AdjustValidMinMaxFromValues();

}

//----------------------------------------
void CProductNetCdf::ReadAll(CFieldNetCdf* field, CExpressionValue& value)
{
  MustBeOpened();
  m_externalFile->GetAllValues(field, value, CUnit::m_UNIT_SI);

  int32_t dataSize = value.GetNbValues();
  for (int32_t i = 0 ; i < dataSize ; i++)
  {
    field->AdjustValidMinMax(value.GetValues()[i]);
  }


}
//----------------------------------------
bool CProductNetCdf::IsOpened()
{
  if (m_externalFile == NULL)
  {
    return false;
  }

  return m_externalFile->IsOpened();
}

//----------------------------------------
bool CProductNetCdf::IsOpened(const std::string& fileName)
{
  bool bOpen = IsOpened();

  if (!bOpen)
  {
    return bOpen;
  }

  if (!CTools::CompareNoCase(m_externalFile->GetName(), fileName))
  {
    bOpen = false;
  }

  return bOpen;
}
//----------------------------------------
void CProductNetCdf::CheckFileOpened()
{
  if (m_currFileName.empty())
  {
    CFileException e("CProductNetCdf::CheckFileOpened - No current file is setting for the product", BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);

  }

  if (IsOpened() == false)
  {
    CFileException e("CProductNetCdf::CheckFileOpened - File is not opened", m_currFileName, BRATHL_IO_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);

  }
}

//----------------------------------------
void CProductNetCdf::GetRecords(CStringArray& array)
{
  array.Insert(CProductNetCdf::m_virtualRecordName);
}



//----------------------------------------
bool CProductNetCdf::IsProductNetCdf(CBratObject* ob)
{
  return (CProductNetCdf::GetProductNetCdf(ob, false) != NULL);
}
//----------------------------------------
CProductNetCdf* CProductNetCdf::GetProductNetCdf(CBratObject* ob, bool withExcept /*= true*/)
{
  CProductNetCdf* productNetCdf = dynamic_cast<CProductNetCdf*>(ob);
  if (withExcept)
  {
    if (productNetCdf == NULL)
    {
      CException e("CProductNetCdf::GetProductNetCdf - dynamic_cast<CProductNetCdf*>(ob) returns NULL"
                   "object seems not to be an instance of CProductNetCdf",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }

  return productNetCdf;

}



//----------------------------------------
void CProductNetCdf::MustBeOpened()
{
  if (m_externalFile == NULL)
  {
    throw CProductException("Try to do an operation on an unallocated file",
			 m_currFileName, GetProductClass(), GetProductType(),
			 BRATHL_LOGIC_ERROR);
  }

  if (!m_externalFile->IsOpened())
  {
    throw CFileException("Try to do an operation on a closed file",
			 m_externalFile->GetName(),
			 BRATHL_LOGIC_ERROR);
  }
}
//----------------------------------------
CProduct* CProductNetCdf::Clone()
{

  CProduct* product = CProduct::Clone();

  if (product == NULL)
  {
    return product;
  }

  if (!product->IsNetCdf())
  {
    throw CException("Error while cloning Netcdf product: product is not CProductNetCdf instance", BRATHL_LOGIC_ERROR);
  }

  CProductNetCdf* productNetCdf = dynamic_cast<CProductNetCdf*>(product);

  productNetCdf->NetCdfProductInitialization(this);

  return product;
}

//----------------------------------------
void CProductNetCdf::NetCdfProductInitialization(CProduct* from)
{
  if (!m_applyNetcdfProductInitialisation)
  {
    return;
  }

  if (from == NULL)
  {
    throw CException("Error while process Netcdf product initialisation: orignal product is NULL", BRATHL_LOGIC_ERROR);
  }

  if (!from->IsNetCdf())
  {
    throw CException("Error while process Netcdf product initialisation: orignal product is not CProductNetCdf instance", BRATHL_LOGIC_ERROR);
  }

  CProductNetCdf* productNetCdf = dynamic_cast<CProductNetCdf*>(from);

  this->SetAxisDims(*(productNetCdf->GetAxisDims()));
  this->SetComplementDims(*(productNetCdf->GetComplementDims()));
  this->SetDimsToReadOneByOne(*(productNetCdf->GetDimsToReadOneByOne()));

}
//----------------------------------------
void CProductNetCdf::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CProductNetCdf Object at "<< this << std::endl;

  //------------------
  CProduct::Dump(fOut);
  //------------------

  fOut << "m_axisDims " << std::endl;
  m_axisDims.Dump(fOut);

  fOut << "m_complementDims = " << std::endl;
  m_complementDims.Dump(fOut);

  fOut << "m_dimsToReadOneByOne = " << std::endl;
  m_dimsToReadOneByOne.Dump(fOut);


  fOut << "m_currentRecord = " << m_currentRecord << std::endl;
  fOut << "m_externalFile = " << m_externalFile << std::endl;

  fOut << "m_forceReadDataOneByOne = " << m_forceReadDataOneByOne << std::endl;
//  fOut << "m_forceReadDataComplementDimsOneByOne = " << m_forceReadDataComplementDimsOneByOne << std::endl;

  fOut << "m_virtualRecordName = " << m_virtualRecordName << std::endl;

  fOut << "==> END Dump a CProductNetCdf Object at "<< this << std::endl;

  fOut << std::endl;

}



} // end namespace
