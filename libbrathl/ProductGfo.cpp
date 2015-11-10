
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

#include "brathl_error.h" 
#include "brathl.h" 

#include <string> 

#include "TraceLog.h" 
#include "Tools.h" 
#include "Exception.h" 
#include "ProductGfo.h" 

using namespace brathl;

namespace brathl
{

const std::string GFO_HEADER = "header";


CProductGfo::CProductGfo()
{  
  Init();
}


//----------------------------------------

CProductGfo::CProductGfo(const std::string& fileName)
      : CProduct(fileName)
{
  Init();  
}


//----------------------------------------
CProductGfo::CProductGfo(const CStringList& fileNameList)
      : CProduct(fileNameList)

{
  Init();
}

//----------------------------------------

CProductGfo::~CProductGfo()
{

}

//----------------------------------------
void CProductGfo::InitDateRef()
{
  m_refDate = REF19850101;
}
//----------------------------------------
void CProductGfo::Init()
{
  m_label = "Gfo product";

  InitDateRef();

  m_timeStampSecondFieldName = "Time_Past_Epoch";
  m_timeStampMicrosecondFieldName = "Time_Past_Epoch_Continued";
  m_latitudeFieldName = "Latitude";
  m_longitudeFieldName = "Longitude";

  m_numHighResolutionMeasure = 10;
  m_deltaTimeHighResolution = 0.979921/m_numHighResolutionMeasure; 

  // Reference point of high resolution measures corresponds to the tenth point (index : (m_numHighResolutionMeasure - 1) / 2)
  m_refPoint = (m_numHighResolutionMeasure - 1) / 2;

  InitCriteriaInfo();

}

//----------------------------------------
void CProductGfo::InitCriteriaInfo()
{
  CProduct::InitCriteriaInfo();

  //-------------------------
  // Datetime criteria info
  //-------------------------
  CCriteriaDatetimeInfo* criteriaDatetimeInfo = new CCriteriaDatetimeInfo();

  criteriaDatetimeInfo->SetDataRecord(GFO_HEADER);
  criteriaDatetimeInfo->SetStartDateField("PASS_BEGIN_TIME");
  criteriaDatetimeInfo->SetEndDateField("PASS_END_TIME");

  criteriaDatetimeInfo->SetRefDate(m_refDate);

  m_criteriaInfoMap.Insert(criteriaDatetimeInfo->GetKey(), criteriaDatetimeInfo); 

  //-------------------------
  // Pass criteria info
  //-------------------------
  CCriteriaPassIntInfo* criteriaPassInfo = new CCriteriaPassIntInfo();

  criteriaPassInfo->SetDataRecord(GFO_HEADER);
  criteriaPassInfo->SetStartPassField("PASS_NUMBER");

  m_criteriaInfoMap.Insert(criteriaPassInfo->GetKey(), criteriaPassInfo); 
  
  //-------------------------
  // Cycle criteria info
  //-------------------------
  CCriteriaCycleInfo* criteriaCycleInfo = new CCriteriaCycleInfo();

  criteriaCycleInfo->SetDataRecord(GFO_HEADER);
  criteriaCycleInfo->SetStartCycleField("CYCLE_NUMBER");

  m_criteriaInfoMap.Insert(criteriaCycleInfo->GetKey(), criteriaCycleInfo); 

}

//----------------------------------------
bool CProductGfo::IsHighResolutionField(CField* field)
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
void CProductGfo::AddInternalHighResolutionFieldCalculation()
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

//  internalFieldName = MakeInternalFieldName(m_timeStampSecondFieldName);
//  m_listInternalFieldName.InsertUnique( internalFieldName );
//  m_fieldNameEquivalence.Insert(m_timeStampSecondFieldName, internalFieldName, false);

  fieldTest = FindFieldByName(m_timeStampSecondFieldName, false, NULL, false);
  if (fieldTest != NULL)
  {
    internalFieldName = MakeInternalNameByAddingRoot(fieldTest->GetFullNameWithRecord());
    m_listInternalFieldName.InsertUnique( internalFieldName );
    m_fieldNameEquivalence.Insert(m_timeStampSecondFieldName, internalFieldName, false);
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

}
//----------------------------------------
void CProductGfo::ProcessHighResolutionWithoutFieldCalculation()
{
  if (m_dataSet.size() <= 0)
  {
    return;
  }
 
  // Get the current recordset (that which has been just read)
  CRecordSet* recordSetToProcess = m_dataSet.GetCurrentRecordSet();

  if (recordSetToProcess == NULL)
  {
    std::string msg = "ERROR in CProductGfo::ProcessHighResolutionWithoutFieldCalculation - No current recordset";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }

  CFieldSetDbl *fieldSetLat = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_latitudeFieldName) );
  CFieldSetDbl *fieldSetLon = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_longitudeFieldName) );
  CFieldSetDbl *fieldSetTimeStampSecond = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampSecondFieldName) );
  CFieldSetDbl *fieldSetTimeStampMicrosecond = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampMicrosecondFieldName) );

  if (fieldSetLat == NULL)
  {
    std::string msg = "ERROR in CProductGfo::ProcessHighResolutionWithoutFieldCalculation - latitude field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }

  if (fieldSetLon == NULL)
  {
    std::string msg = "ERROR in CProductGfo::ProcessHighResolutionWithoutFieldCalculation - longitude field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }

  if (fieldSetTimeStampSecond == NULL)
  {
    std::string msg = "ERROR in CProductGfo::ProcessHighResolutionWithoutFieldCalculation - timestamp second field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }
  if (fieldSetTimeStampMicrosecond == NULL)
  {
    std::string msg = "ERROR in CProductGfo::ProcessHighResolutionWithoutFieldCalculation - timestamp microsecond field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }


  // if time difference between the the current timestamp and the previous timestamp is greater than 2 seconds 
  // or if current timestamp is less or equal than the previous timestamp
  // skip the current recordset (data will be lost)
  

  // WARNING -  fieldSetTimeStampSecond, fieldSetTimeStampMicrosecond are converted to SI
  // after they have been read. So their value are stated in seconds
  double timeStamp = 0;
  timeStamp = CTools::Plus(timeStamp, fieldSetTimeStampSecond->m_value);
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

  if (CTools::IsDefaultValue(m_previousLatitude))
  {
    std::string msg = "ERROR in CProductGfo::ProcessHighResolutionWithoutFieldCalculation - previous latitude value read is inconsistent (is default value)";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }
  if (CTools::IsDefaultValue(m_previousLongitude))
  {
    std::string msg = "ERROR in CProductGfo::ProcessHighResolutionWithoutFieldCalculation - previous longitude value read is inconsistent (is default value)";
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
    std::string msg = CTools::Format("ERROR in CProductGfo::ProcessHighResolutionWithoutFieldCalculation - delta of longitude is set to an unexpected value: %f "
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
  fieldSetTimeStampSecond = NULL;
  fieldSetTimeStampMicrosecond = NULL;
   
  // Insert the record of the new dataset in the main dataset
  m_dataSet.InsertDataset(&dataSetHighResolution);
  
}

//----------------------------------------
void CProductGfo::ComputeHighResolutionFields(CDataSet* dataSet, double deltaLat, double deltaLon)
{
  // Save current recordset pointer
  CRecordSet* currentRecordSetOld = dataSet->GetCurrentRecordSet();

  //dataSet->SetCurrentRecordSet(dataSet->GetFirstRecordSet());

  CFieldSetDbl *fieldSetLat = NULL;
  CFieldSetDbl *fieldSetLon = NULL;
  CFieldSetDbl *fieldSetTimeStampSecond = NULL;
  CFieldSetDbl *fieldSetTimeStampMicrosecond = NULL;

  int32_t count = dataSet->size();

  for (int32_t index = 0 ; index < count ; index++)
  {
    dataSet->SetCurrentRecordSet(index);

    fieldSetLat = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_latitudeFieldName) );
    fieldSetLon = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_longitudeFieldName) );
    fieldSetTimeStampSecond = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampSecondFieldName) );
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


    // WARNING -  fieldSetTimeStampSecond, fieldSetTimeStampMicrosecond are converted to SI
    // after they have been read. So their value are stated in seconds
    
    // fieldSetTimeStampSecond = number of seconds from reference SI unit (i.e. 1950-01-01 00:00:00.0)
    
    // fieldSetTimeStampMicrosecond = number of seconds  within the seconds( < 1 second)
    
    double nbSeconds = 0;

    if (fieldSetTimeStampSecond != NULL)
    {
      nbSeconds = CTools::Plus(nbSeconds, fieldSetTimeStampSecond->m_value);
    }

    if (fieldSetTimeStampMicrosecond != NULL)
    {
      nbSeconds = CTools::Plus(nbSeconds, fieldSetTimeStampMicrosecond->m_value);
    }
            
    nbSeconds = CTools::Plus(nbSeconds, 
                             CTools::Multiply(m_deltaTimeHighResolution, 
                                              static_cast<double>(index - m_refPoint)));

    
    double seconds;
    double muSeconds;

    CDate timeStamp;
    timeStamp.SetDate(nbSeconds);

    //WARNING At this point,
    // fieldSetTimeStampSecond is stated  in number of seconds and fieldSetTimeStampMicrosecond in numbers of microseconds
    timeStamp.Convert2SM(seconds,
                         muSeconds);

    // WARNING -  fieldSetTimeStampSecond, fieldSetTimeStampMicrosecond are converted to SI
    // after they have been read. So their value are stated in seconds
    //
    //WARNING At this point, muSeconds is stated in numbers of microseconds
    //
    // ====> Convert again muSeconds to a number of seconds     

    if (fieldSetTimeStampSecond != NULL)
    {
      fieldSetTimeStampSecond->m_value = seconds;
    }

    if (fieldSetTimeStampMicrosecond != NULL)
    {
      fieldSetTimeStampMicrosecond->m_value = muSeconds / 1.0E+6;
    }
      

    /*      
    // WARNING -  convert fieldSetTimeStampSecond, fieldSetTimeStampMicrosecond in SI
    // after they have been read. So their value are stated in seconds
    CUnit unit = fieldSetTimeStampSecond->GetField()->GetUnit();
    fieldSetTimeStampSecond->m_value = unit.Convert(fieldSetTimeStampSecond->m_value);

    unit = fieldSetTimeStampMicrosecond->GetField()->GetUnit();
    fieldSetTimeStampMicrosecond->m_value = unit.Convert(fieldSetTimeStampMicrosecond->m_value);
    */
  }

  // Restore current recordset pointer
  dataSet->SetCurrentRecordSet(currentRecordSetOld);
}

//----------------------------------------
void CProductGfo::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CProductGfo Object at "<< this << std::endl;

  //------------------
  CProduct::Dump(fOut);
  //------------------

  fOut << "==> END Dump a CProductGfo Object at "<< this << std::endl;

  fOut << std::endl;

}


} // end namespace
