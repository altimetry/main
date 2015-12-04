
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
#include <typeinfo> 
#include <string> 


#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include "new-gui/Common/tools/TraceLog.h" 
#include "Tools.h" 
#include "new-gui/Common/tools/Exception.h" 
#include "ProductTopex.h" 

using namespace brathl;


namespace brathl
{

/** Altimeter Indicator. 
    This element is computed for TOPEX and POSEIDON data. 
    It indicates which altimeter is on at the time of the measurement. 
    Value Definition: 0 = POSEIDON on, 1 = TOPEX on
    */
const int32_t CProductTopex::m_ALTIMETER_POSEIDON = 0;		
const int32_t CProductTopex::m_ALTIMETER_TOPEX = 1;		

const std::string CProductTopex::m_TOPEX_POSEIDON_HEADER = "header";

const std::string CProductTopex::m_PASS_FILE = "MGDR_pass_file";
const std::string CProductTopex::m_XNG_FILE = "MGDR_crossover_point_file";
const std::string CProductTopex::m_SDR_PASS_FILE = "SDR_pass_file";



//----------------------------------------
CProductTopex::CProductTopex()
{
  Init();
}

//----------------------------------------
CProductTopex::CProductTopex(const std::string& fileName)
      : CProduct(fileName)
{
  Init();
}


//----------------------------------------
CProductTopex::CProductTopex(const CStringList& fileNameList)
      : CProduct(fileNameList)

{
  Init();
}

//----------------------------------------

CProductTopex::~CProductTopex()
{
}

//----------------------------------------
void CProductTopex::InitDateRef()
{
  m_refDate = REF19580101;
}


//----------------------------------------
void CProductTopex::Init()
{
  m_label = "Topex/Poseidon product";

  InitDateRef();

  m_timeStampDayFieldName = "Tim_Moy_1";
  m_timeStampMillisecondFieldName = "Tim_Moy_2";
  m_timeStampMicrosecondFieldName = "Tim_Moy_3";
  
  m_latitudeFieldName = "Lat_Tra";
  m_longitudeFieldName = "Lon_Tra";

  m_altimeterIndicatorFieldName = "ALTON";

  //Set delta time in seconds between two high resolution measures (basic delta time / number of high resolution measures)
  m_numHighResolutionMeasure = 10;

  // WARNING m_deltaTimeHighResolution is not initialized here because its value is different for Topex and Poseidon

  // Reference point of high resolution measures corresponds to the tenth point (index : (m_numHighResolutionMeasure - 1) / 2)
  m_refPoint = (m_numHighResolutionMeasure - 1) / 2;

  InitCriteriaInfo();

}
//----------------------------------------
std::string CProductTopex::GetLabel()
{
  if (m_fileList.m_productType.compare(CProductTopex::m_PASS_FILE) == 0)
  {
    m_label = "Topex/Poseidon GDR-M product";
  }
  else if (m_fileList.m_productType.compare(CProductTopex::m_XNG_FILE) == 0)
  {
    m_label = "Topex/Poseidon crossover point product";
  }

  return m_label; 
}

//----------------------------------------
void CProductTopex::InitCriteriaInfo()
{
  CProduct::InitCriteriaInfo();

  //std::string productype = CTools::StringToUpper(m_fileList.m_productType);
  std::string productype = m_fileList.m_productType;
  if ((productype.compare(CProductTopex::m_PASS_FILE) == 0) || (productype.compare(CProductTopex::m_SDR_PASS_FILE) == 0))
  {
    //-------------------------
    // Datetime criteria info
    //-------------------------
    CCriteriaDatetimeInfo* criteriaDatetimeInfo = new CCriteriaDatetimeInfo();

    criteriaDatetimeInfo->SetDataRecord(CProductTopex::m_TOPEX_POSEIDON_HEADER);
    criteriaDatetimeInfo->SetStartDateField("Time_First_Pt");
    criteriaDatetimeInfo->SetEndDateField("Time_Last_Pt");

    criteriaDatetimeInfo->SetRefDate(m_refDate);

    m_criteriaInfoMap.Insert(criteriaDatetimeInfo->GetKey(), criteriaDatetimeInfo); 

    //-------------------------
    // Pass criteria info
    //-------------------------
    CCriteriaPassIntInfo* criteriaPassInfo = new CCriteriaPassIntInfo();

    criteriaPassInfo->SetDataRecord(CProductTopex::m_TOPEX_POSEIDON_HEADER);
    criteriaPassInfo->SetStartPassField("Pass_Number");

    m_criteriaInfoMap.Insert(criteriaPassInfo->GetKey(), criteriaPassInfo); 
  

    //-------------------------
    // Cycle criteria info
    //-------------------------
    CCriteriaCycleInfo* criteriaCycleInfo = new CCriteriaCycleInfo();

    criteriaCycleInfo->SetDataRecord(CProductTopex::m_TOPEX_POSEIDON_HEADER);
    criteriaCycleInfo->SetStartCycleField("Cycle_Number");

    m_criteriaInfoMap.Insert(criteriaCycleInfo->GetKey(), criteriaCycleInfo); 
  }
  else if (productype.compare(CProductTopex::m_XNG_FILE) == 0)
  {
    //-------------------------
    // Cycle criteria info
    //-------------------------
    CCriteriaCycleInfo* criteriaCycleInfo = new CCriteriaCycleInfo();

    criteriaCycleInfo->SetDataRecord(CProductTopex::m_TOPEX_POSEIDON_HEADER);
    criteriaCycleInfo->SetStartCycleField("Cycle_Number");

    m_criteriaInfoMap.Insert(criteriaCycleInfo->GetKey(), criteriaCycleInfo); 

  }

}

//----------------------------------------
void CProductTopex::SetDeltaTimeHighResolution(int32_t altimeterIndicator)
{
  setDefaultValue(m_deltaTimeHighResolution);

  if (altimeterIndicator == CProductTopex::m_ALTIMETER_POSEIDON)
  {
    m_deltaTimeHighResolution = 1.00104/m_numHighResolutionMeasure;       
  }
  else if (altimeterIndicator == CProductTopex::m_ALTIMETER_TOPEX)
  {
    m_deltaTimeHighResolution = 1.07858/m_numHighResolutionMeasure;       
  }
  else
  {
    std::string msg = CTools::Format("ERROR in CProductTopex::SetDeltaTimeHighResolution - Invalid altimeter indicator value: %d - valid valid are %d (POSEIDON) or %d (TOPEX)",
                                altimeterIndicator,
                                CProductTopex::m_ALTIMETER_POSEIDON,
                                CProductTopex::m_ALTIMETER_TOPEX);
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_INCONSISTENCY_ERROR);
    throw (e);      
  }
}

//----------------------------------------
void CProductTopex::AddInternalHighResolutionFieldCalculation()
{
  std::string internalFieldName;
  CField* fieldTest = NULL;

//  internalFieldName = MakeInternalFieldName(m_latitudeFieldName);
//  m_listInternalFieldName.InsertUnique( internalFieldName );
//  m_fieldNameEquivalence.Insert(m_latitudeFieldName, internalFieldName, false);

  fieldTest = FindFieldByName(m_latitudeFieldName, false, NULL, false);
  if (fieldTest != NULL)
  {
    internalFieldName = MakeInternalNameByAddingRoot(fieldTest->GetFullNameWithRecord());
    m_listInternalFieldName.InsertUnique( internalFieldName );
    m_fieldNameEquivalence.Insert(m_latitudeFieldName, internalFieldName, false);
  }

//  internalFieldName = MakeInternalFieldName(m_longitudeFieldName);
//  m_listInternalFieldName.InsertUnique( internalFieldName );
//  m_fieldNameEquivalence.Insert(m_longitudeFieldName, internalFieldName, false);

  fieldTest = FindFieldByName(m_longitudeFieldName, false, NULL, false);
  if (fieldTest != NULL)
  {
    internalFieldName = MakeInternalNameByAddingRoot(fieldTest->GetFullNameWithRecord());
    m_listInternalFieldName.InsertUnique( internalFieldName );
    m_fieldNameEquivalence.Insert(m_longitudeFieldName, internalFieldName, false);
  }

//  internalFieldName = MakeInternalFieldName(m_timeStampDayFieldName);
//  m_listInternalFieldName.InsertUnique( internalFieldName );
//  m_fieldNameEquivalence.Insert(m_timeStampDayFieldName, internalFieldName, false);

  fieldTest = FindFieldByName(m_timeStampDayFieldName, false, NULL, false);
  if (fieldTest != NULL)
  {
    internalFieldName = MakeInternalNameByAddingRoot(fieldTest->GetFullNameWithRecord());
    m_listInternalFieldName.InsertUnique( internalFieldName );
    m_fieldNameEquivalence.Insert(m_timeStampDayFieldName, internalFieldName, false);
  }

//  internalFieldName = MakeInternalFieldName(m_timeStampMillisecondFieldName);
//  m_listInternalFieldName.InsertUnique( internalFieldName );
//  m_fieldNameEquivalence.Insert(m_timeStampMillisecondFieldName, internalFieldName, false);

  fieldTest = FindFieldByName(m_timeStampMillisecondFieldName, false, NULL, false);
  if (fieldTest != NULL)
  {
    internalFieldName = MakeInternalNameByAddingRoot(fieldTest->GetFullNameWithRecord());
    m_listInternalFieldName.InsertUnique( internalFieldName );
    m_fieldNameEquivalence.Insert(m_timeStampMillisecondFieldName, internalFieldName, false);
  }

//  internalFieldName = MakeInternalFieldName(m_timeStampMicrosecondFieldName);
//  m_listInternalFieldName.InsertUnique( internalFieldName );
//  m_fieldNameEquivalence.Insert(m_timeStampMicrosecondFieldName, internalFieldName, false);

  fieldTest = FindFieldByName(m_timeStampMicrosecondFieldName, false, NULL, false);
  if (fieldTest != NULL)
  {
    internalFieldName = MakeInternalNameByAddingRoot(fieldTest->GetFullNameWithRecord());
    m_listInternalFieldName.InsertUnique( internalFieldName );
    m_fieldNameEquivalence.Insert(m_timeStampMicrosecondFieldName, internalFieldName, false);
  }


//  internalFieldName = MakeInternalFieldName(m_altimeterIndicatorFieldName);
//  m_listInternalFieldName.InsertUnique( internalFieldName );
//  m_fieldNameEquivalence.Insert(m_altimeterIndicatorFieldName, internalFieldName, false);

  fieldTest = FindFieldByName(m_altimeterIndicatorFieldName, false, NULL, false);
  if (fieldTest != NULL)
  {
    internalFieldName = MakeInternalNameByAddingRoot(fieldTest->GetFullNameWithRecord());
    m_listInternalFieldName.InsertUnique( internalFieldName );
    m_fieldNameEquivalence.Insert(m_altimeterIndicatorFieldName, internalFieldName, false);
  }

}

//----------------------------------------
bool CProductTopex::IsHighResolutionField(CField* field)
{
  if (field == NULL)
  {
    return false;
  }

  if (typeid(*field) == typeid(CFieldBasic))
  {
    return false;
  }

  if (field->GetNbDims() != 1)
  {
    return false;
  }

  if ((field->GetDim())[0] == static_cast<int32_t>(m_numHighResolutionMeasure))
  {
    return true;
  }

  return false;
}
//----------------------------------------
void CProductTopex::ProcessHighResolutionWithoutFieldCalculation()
{
  if (m_dataSet.size() <= 0)
  {
    return;
  }
 
  // Get the current recordset (that which has been just read)
  CRecordSet* recordSetToProcess = m_dataSet.GetCurrentRecordSet();

  if (recordSetToProcess == NULL)
  {
    std::string msg = "ERROR in CProductTopex::ProcessHighResolutionWithoutFieldCalculation - No current recordset";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }

  CFieldSetDbl *fieldSetLat = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_latitudeFieldName) );
  CFieldSetDbl *fieldSetLon = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_longitudeFieldName) );
  CFieldSetDbl *fieldSetTimeStampDay = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampDayFieldName) );
  CFieldSetDbl *fieldSetTimeStampMillisecond = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampMillisecondFieldName) );
  CFieldSetDbl *fieldSetTimeStampMicrosecond = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampMicrosecondFieldName) );
  CFieldSetDbl *fieldSetAltimeterIndicator = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_altimeterIndicatorFieldName) );

  if (fieldSetLat == NULL)
  {
    std::string msg = "ERROR in CProductTopex::ProcessHighResolutionWithoutFieldCalculation - latitude field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }

  if (fieldSetLon == NULL)
  {
    std::string msg = "ERROR in CProductTopex::ProcessHighResolutionWithoutFieldCalculation - longitude field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }

  if (fieldSetTimeStampDay == NULL)
  {
    std::string msg = "ERROR in CProductTopex::ProcessHighResolutionWithoutFieldCalculation - timestamp day field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }
  if (fieldSetTimeStampMillisecond == NULL)
  {
    std::string msg = "ERROR in CProductTopex::ProcessHighResolutionWithoutFieldCalculation - timestamp millisecond field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }
  if (fieldSetTimeStampMicrosecond == NULL)
  {
    std::string msg = "ERROR in CProductTopex::ProcessHighResolutionWithoutFieldCalculation - timestamp microsecond field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }

  if (fieldSetAltimeterIndicator == NULL)
  {
    std::string msg = "ERROR in CProductTopex::ProcessHighResolutionWithoutFieldCalculation - altimeter indicator field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }


  // delta time depends on altimeter;
  SetDeltaTimeHighResolution(static_cast<int32_t>(fieldSetAltimeterIndicator->m_value));

  // if time difference between the the current timestamp and the previous timestamp is greater than 2 seconds 
  // or if current timestamp is less or equal than the previous timestamp
  // skip the current recordset (data will be lost)
  

  // WARNING -  fieldSetTimeStampDay, fieldSetTimeStampMillisecond, fieldSetTimeStampMicrosecond are converted to SI
  // after they have been read. So their value are stated in seconds
  double timeStamp = 0;
  timeStamp = CTools::Plus(timeStamp, fieldSetTimeStampDay->m_value);
  timeStamp = CTools::Plus(timeStamp, fieldSetTimeStampMillisecond->m_value);
  timeStamp = CTools::Plus(timeStamp, fieldSetTimeStampMicrosecond->m_value);

  if  ( (timeStamp - m_previousTimeStamp > 2.0) ||
        (timeStamp <= m_previousTimeStamp) )
  {
    std::string msg = CTools::Format("INFO - record skipped due to inconsistency between two measures\n"
                                "\t previous record --> timestamp %f seconds\n"
                                "\t current record --> timestamp %f seconds\n"
                                "\t timestamp difference is %f seconds\n",
                                m_previousTimeStamp,
                                timeStamp,
                                timeStamp - m_previousTimeStamp);
    CTrace::Tracer(3, msg); 

    m_previousLatitude = fieldSetLat->m_value;
    m_previousLongitude = fieldSetLon->m_value;
    m_previousTimeStamp = timeStamp;
    
    // Erase the current recordset in the main dataset (data are lost)
    m_dataSet.Erase(recordSetToProcess);
    recordSetToProcess = NULL;  
    m_nSkippedRecord++;
    return;
  }

  // Create a new dataset which contains 'normal' fields of the current recordset of the main dataset and
  // high resolution fields
  // This dataset will contains as  many records as number of high resolution measures (m_numHighResolutionMeasure)
  CDataSet dataSetHighResolution("HighResolution", false);

  if (isDefaultValue(m_previousLatitude))
  {
    std::string msg = "ERROR in CProductTopex::ProcessHighResolutionWithoutFieldCalculation - previous latitude value read is inconsistent (is default value)";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }
  if (isDefaultValue(m_previousLongitude))
  {
    std::string msg = "ERROR in CProductTopex::ProcessHighResolutionWithoutFieldCalculation - previous longitude value read is inconsistent (is default value)";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }

  // Compute delta latitude an deltat longitude.
  double deltaLat = (fieldSetLat->m_value - m_previousLatitude) / m_numHighResolutionMeasure;
  double deltaLon = (fieldSetLon->m_value - m_previousLongitude) / m_numHighResolutionMeasure;

  if ( (m_numHighResolutionMeasure * deltaLon) < -1.0)
  {
    deltaLon += 360 / m_numHighResolutionMeasure;
  }
  else if ( (m_numHighResolutionMeasure * deltaLon) > 1.0)
  {
    deltaLon -= 360 / m_numHighResolutionMeasure;
  }

  if ( (m_numHighResolutionMeasure * CTools::Abs(deltaLon)) > 1.0)
  {
    std::string msg = CTools::Format("ERROR in CProductTopex::ProcessHighResolutionWithoutFieldCalculation - delta of longitude is set to an unexpected value: %f "
                                "\n\tcurrent datetime: %f current latitude: %f current longitude: %f"
                                "\n\tprevious datetime: %f previous latitude: %f previous longitude: %f",
                                deltaLon,
                                timeStamp, 
                                fieldSetLat->m_value, 
                                fieldSetLon->m_value,
                                m_previousTimeStamp,
                                m_previousLatitude,
                                m_previousLongitude);
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }

  CRecordSet::iterator it;
  for (it = recordSetToProcess->begin() ; it != recordSetToProcess->end() ; it++)
  {
    CFieldSet* fieldSet = recordSetToProcess->GetFieldSet(it);
    CFieldSetDbl* fieldSetDbl = dynamic_cast<CFieldSetDbl*>(fieldSet); 
    CFieldSetArrayDbl* fieldSetArrayDbl = dynamic_cast<CFieldSetArrayDbl*>(fieldSet); 

    // the field is hidden, it is just use for calculation, don't set it in output
    if (fieldSet->GetField()->IsHidden())
    {
      continue;
    }    

    // data is an array of double
    if (fieldSetArrayDbl != NULL)
    {
      // field is a high resolution field 
      // transform array of double  in a set of records with a double value
      if ( fieldSetArrayDbl->GetField()->IsHighResolution() )
      {
        PutFlatHighResolution(&dataSetHighResolution, fieldSetArrayDbl);      
      }
      else
      {
        // field is an array
        // put it into the set of records (array is repeated for each record)
        Put(&dataSetHighResolution, fieldSetArrayDbl, m_numHighResolutionMeasure);      
      }
    }
    
    // data is a double
    if (fieldSetDbl != NULL)
    {
      // field is a double
      // put it into the set of records (value is repeated for each record)
      Put(&dataSetHighResolution, fieldSetDbl);      
    }    
  }


  // Compute high resolution fields
  ComputeHighResolutionFields(&dataSetHighResolution, deltaLat, deltaLon);      

  m_previousLatitude = fieldSetLat->m_value;
  m_previousLongitude = fieldSetLon->m_value;
  m_previousTimeStamp = timeStamp;

  // Erase the current recordset in the main dataset 
  m_dataSet.Erase(recordSetToProcess);
  
  recordSetToProcess = NULL;
  
  fieldSetLat = NULL;
  fieldSetLon = NULL;
  fieldSetTimeStampDay = NULL;
  fieldSetTimeStampMillisecond = NULL;
  fieldSetTimeStampMicrosecond = NULL;
   
  // Insert the record of the new dataset in the main dataset
  m_dataSet.InsertDataset(&dataSetHighResolution);
  
}

//----------------------------------------
void CProductTopex::ComputeHighResolutionFields(CDataSet* dataSet, double deltaLat, double deltaLon)
{
  // Save current recordset pointer
  CRecordSet* currentRecordSetOld = dataSet->GetCurrentRecordSet();

  //dataSet->SetCurrentRecordSet(dataSet->GetFirstRecordSet());

  CFieldSetDbl *fieldSetLat = NULL;
  CFieldSetDbl *fieldSetLon = NULL;
  CFieldSetDbl *fieldSetTimeStampDay = NULL;
  CFieldSetDbl *fieldSetTimeStampMillisecond = NULL;
  CFieldSetDbl *fieldSetTimeStampMicrosecond = NULL;

  size_t count = dataSet->size();

  for (size_t index = 0 ; index < count ; index++)
  {
    dataSet->SetCurrentRecordSet(index);

    fieldSetLat = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_latitudeFieldName) );
    fieldSetLon = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_longitudeFieldName) );
    fieldSetTimeStampDay = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampDayFieldName) );
    fieldSetTimeStampMillisecond = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampMillisecondFieldName) );
    fieldSetTimeStampMicrosecond = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampMicrosecondFieldName) );

    // Compute latitude
    if (fieldSetLat != NULL)
    {
      fieldSetLat->m_value = CTools::Plus(fieldSetLat->m_value, 
                                          CTools::Multiply(deltaLat, 
                                                           static_cast<double>(index - m_refPoint)));
    }

    // Compute longitude
    if (fieldSetLon != NULL)
    {
      //fieldSetLon->m_value = CTools::NormalizeLongitude(-180,  
      //                                                  CTools::Plus(fieldSetLon->m_value, 
      //                                                               CTools::Multiply(deltaLon, 
      //                                                                                static_cast<double>(index - m_refPoint))));
      fieldSetLon->m_value = CTools::Plus(fieldSetLon->m_value, 
                                          CTools::Multiply(deltaLon,
                                                           static_cast<double>(index - m_refPoint)));
    }

    // Compute timestamp

    // WARNING -  fieldSetTimeStampDay, fieldSetTimeStampMillisecond, fieldSetTimeStampMicrosecond are converted to SI
    // after they have been read. So their value are stated in seconds
    
    // fieldSetTimeStampDay = number of seconds from reference SI unit (i.e. 1950-01-01 00:00:00.0)
    
    // fieldSetTimeStampMillisecond = number of seconds within the day
    
    // fieldSetTimeStampMicrosecond = number of seconds within the seconds ( < 1 second)


    double nbSeconds = 0;

    if (fieldSetTimeStampDay != NULL)
    {
      nbSeconds = CTools::Plus(nbSeconds, fieldSetTimeStampDay->m_value);
    }

    if (fieldSetTimeStampMillisecond != NULL)
    {
      nbSeconds = CTools::Plus(nbSeconds, fieldSetTimeStampMillisecond->m_value);
    }

    if (fieldSetTimeStampMicrosecond != NULL)
    {
      nbSeconds = CTools::Plus(nbSeconds, fieldSetTimeStampMicrosecond->m_value);
    }
      

      
    if (isDefaultValue(m_deltaTimeHighResolution))
    {
      std::string msg = "ERROR in CProductTopex::ComputeHighResolutionFields - high resolution delta time has no been set";
      CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_INCONSISTENCY_ERROR);
      throw (e);

    }

    nbSeconds = CTools::Plus(nbSeconds, 
                             CTools::Multiply(m_deltaTimeHighResolution, 
                                              static_cast<double>(index - m_refPoint)));

    
    CDate timeStamp;
    timeStamp.SetDate(nbSeconds);

    double days;
    double milliseconds;
    double muSeconds;


    //WARNING At this point, fieldSetTimeStampDay is stated in number of day,
    // fieldSetTimeStampMillisecond in number of milliseconds and fieldSetTimeStampMicrosecond in numbers of microseconds
    timeStamp.Convert2DMM(days,
                          milliseconds,
                          muSeconds);

    //WARNING At this point, days is stated in number of day,
    // milliseconds in number of milliseconds and muSeconds in numbers of microseconds
    //
    // ====> Convert again days to number of seconds from reference SI unit (i.e. 1950-01-01 00:00:00.0)
    // ====> Convert again milliseconds to a number of seconds     
    // ====> Convert again muSeconds to a number of seconds     

    if (fieldSetTimeStampDay != NULL)
    {
      CDate date1950(days, 0.0, 0.0);
      fieldSetTimeStampDay->m_value = date1950.Value();
    }

    if (fieldSetTimeStampMillisecond != NULL)
    {
      fieldSetTimeStampMillisecond->m_value = milliseconds / 1.0E+3;
    }

    if (fieldSetTimeStampMicrosecond != NULL)
    {
      fieldSetTimeStampMicrosecond->m_value = muSeconds / 1.0E+6;
    }
    /*  
    // WARNING -  convert fieldSetTimeStampDay, fieldSetTimeStampMillisecond, fieldSetTimeStampMicrosecond in SI
    // after they have been read. So their value are stated in seconds
    CUnit unit = fieldSetTimeStampDay->GetField()->GetUnit();
    fieldSetTimeStampDay->m_value = unit.Convert(fieldSetTimeStampDay->m_value);

    unit = fieldSetTimeStampMillisecond->GetField()->GetUnit();
    fieldSetTimeStampMillisecond->m_value = unit.Convert(fieldSetTimeStampMillisecond->m_value);

    unit = fieldSetTimeStampMicrosecond->GetField()->GetUnit();
    fieldSetTimeStampMicrosecond->m_value = unit.Convert(fieldSetTimeStampMicrosecond->m_value);
    */
  }

  // Restore current recordset pointer
  dataSet->SetCurrentRecordSet(currentRecordSetOld);
}


//----------------------------------------
void CProductTopex::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CProductTopex Object at "<< this << std::endl;

  //------------------
  CProduct::Dump(fOut);
  //------------------

  fOut << "==> END Dump a CProductTopex Object at "<< this << std::endl;

  fOut << std::endl;
}
//----------------------------------------



} // end namespace
