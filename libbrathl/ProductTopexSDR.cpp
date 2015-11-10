
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
#include "ProductTopex.h" 
#include "ProductTopexSDR.h" 

using namespace brathl;


namespace brathl
{



//----------------------------------------
CProductTopexSDR::CProductTopexSDR()
{
  Init();
}

//----------------------------------------
CProductTopexSDR::CProductTopexSDR(const std::string& fileName)
      : CProductTopex(fileName)
{
  Init();
}


//----------------------------------------
CProductTopexSDR::CProductTopexSDR(const CStringList& fileNameList)
      : CProductTopex(fileNameList)

{
  Init();
}

//----------------------------------------

CProductTopexSDR::~CProductTopexSDR()
{
}



//----------------------------------------
void CProductTopexSDR::Init()
{
  m_label = "SDR Topex/Poseidon pass file";

  InitDateRef();

  m_timeStampDayFieldName = "Sci.Time_Past_Epoch_AltSci.day";
  m_timeStampMillisecondFieldName = "Sci.Time_Past_Epoch_AltSci.millisec";
  m_timeStampMicrosecondFieldName = "Sci.Time_Past_Epoch_AltSci.microsec";
  
  m_latitudeFieldName = "Sci.Latitude_AltSDR";
  m_longitudeFieldName = "Sci.Longitude_AltSDR";

  // Set number of high rate high resolution 
  m_highRateNumHighResolutionMeasure = 10;
 
  // Set number of low rate high resolution 
  m_lowRateNumHighResolutionMeasure = 5;

  // WARNING about m_deltaTimeHighResolution :
  // delta time depends on altimeter. However SDR data always come from Topex altimeter
  SetDeltaTimeHighResolution(CProductTopex::m_ALTIMETER_TOPEX);

  // WARNING The number of high resolution will be set to reflect the real number (dim.) of high resolution data used in the recordset
  m_numHighResolutionMeasure = 10;

  // WARNING  Reference point of high resolution measures will be computed when the real number (dim.) of high resolution data will be set
  CTools::SetDefaultValue(m_refPoint);
 
  // WARNING InitCriteriaInfo() is calles in the CProductTopex::Init() function
  
}
//----------------------------------------
std::string CProductTopexSDR::GetLabel()
{
  return m_label; 
}

//----------------------------------------
bool CProductTopexSDR::IsHighResolutionField(CField* field)
{
  if (field == NULL)
  {
    return false;
  }

  if (typeid(*field) == typeid(CFieldBasic))
  {
    return false;
  }

  if ((field->GetNbDims() != 1) && (field->GetNbDims() != 2))
  {
    return false;
  }

  if ((field->GetDim())[0] == static_cast<int32_t>(m_highRateNumHighResolutionMeasure))
  {
    field->SetNumHighResolutionMeasure(m_highRateNumHighResolutionMeasure);
    return true;
  }
  
  if ((field->GetDim())[0] == static_cast<int32_t>(m_lowRateNumHighResolutionMeasure))
  {
    field->SetNumHighResolutionMeasure(m_lowRateNumHighResolutionMeasure);
    return true;
  }

  return false;
}
//----------------------------------------
void CProductTopexSDR::SetHighResolution(CField* field)
{
  if (field == NULL)
  {
    return;
  }
  // WARNING : field->SetNumHighResolutionMeasure function is called in IsHighResolutionField function
  field->SetHighResolution(this->IsHighResolutionField(field));
}

//----------------------------------------
void CProductTopexSDR::ProcessHighResolutionWithoutFieldCalculation()
{
  if (m_dataSet.size() <= 0)
  {
    return;
  }
 
  // Get the current recordset (that which has been just read)
  CRecordSet* recordSetToProcess = m_dataSet.GetCurrentRecordSet();

  if (recordSetToProcess == NULL)
  {
    std::string msg = "ERROR in CProductTopexSDR::ProcessHighResolutionWithoutFieldCalculation - No current recordset";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }


  // Compute the real dimension of high resolution field in the recordset (high rate dim. or low rate dim.)
  
  m_numHighResolutionMeasure = 0;

  CRecordSet::iterator itRec;
  for (itRec = recordSetToProcess->begin() ; itRec != recordSetToProcess->end() ; itRec++)
  {
    CFieldSet* fieldSet = recordSetToProcess->GetFieldSet(itRec);
    //CFieldSetDbl* fieldSetDbl = dynamic_cast<CFieldSetDbl*>(fieldSet); 
    CFieldSetArrayDbl* fieldSetArrayDbl = dynamic_cast<CFieldSetArrayDbl*>(fieldSet); 

    // data is an array of double
    if (fieldSetArrayDbl != NULL)
    {
      // field is a high resolution field 
      // transform array of double  in a set of records with a double value
      if ( fieldSetArrayDbl->GetField()->IsHighResolution() )
      {
        m_numHighResolutionMeasure = static_cast<uint32_t>(CTools::Max(m_numHighResolutionMeasure, fieldSetArrayDbl->GetField()->GetDim()[0]));
      }
    }
  }

  // Reference point of high resolution measures corresponds to the tenth point (index : (m_numHighResolutionMeasure - 1) / 2)
  m_refPoint = (m_numHighResolutionMeasure - 1) / 2;




  CFieldSetDbl *fieldSetLat = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_latitudeFieldName) );
  CFieldSetDbl *fieldSetLon = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_longitudeFieldName) );
  CFieldSetDbl *fieldSetTimeStampDay = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampDayFieldName) );
  CFieldSetDbl *fieldSetTimeStampMillisecond = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampMillisecondFieldName) );
  CFieldSetDbl *fieldSetTimeStampMicrosecond = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampMicrosecondFieldName) );

  if (fieldSetLat == NULL)
  {
    std::string msg = "ERROR in CProductTopexSDR::ProcessHighResolutionWithoutFieldCalculation - latitude field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }

  if (fieldSetLon == NULL)
  {
    std::string msg = "ERROR in CProductTopexSDR::ProcessHighResolutionWithoutFieldCalculation - longitude field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }

  if (fieldSetTimeStampDay == NULL)
  {
    std::string msg = "ERROR in CProductTopexSDR::ProcessHighResolutionWithoutFieldCalculation - timestamp day field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }
  if (fieldSetTimeStampMillisecond == NULL)
  {
    std::string msg = "ERROR in CProductTopexSDR::ProcessHighResolutionWithoutFieldCalculation - timestamp millisecond field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }
  if (fieldSetTimeStampMicrosecond == NULL)
  {
    std::string msg = "ERROR in CProductTopexSDR::ProcessHighResolutionWithoutFieldCalculation - timestamp microsecond field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }


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

  if (CTools::IsDefaultValue(m_previousLatitude))
  {
    std::string msg = "ERROR in CProductTopexSDR::ProcessHighResolutionWithoutFieldCalculation - previous latitude value read is inconsistent (is default value)";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }
  if (CTools::IsDefaultValue(m_previousLongitude))
  {
    std::string msg = "ERROR in CProductTopexSDR::ProcessHighResolutionWithoutFieldCalculation - previous longitude value read is inconsistent (is default value)";
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
    std::string msg = CTools::Format("ERROR in CProductTopexSDR::ProcessHighResolutionWithoutFieldCalculation - delta of longitude is set to an unexpected value: %f "
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
void CProductTopexSDR::ComputeHighResolutionFields(CDataSet* dataSet, double deltaLat, double deltaLon)
{
  // Save current recordset pointer
  CRecordSet* currentRecordSetOld = dataSet->GetCurrentRecordSet();

  //dataSet->SetCurrentRecordSet(dataSet->GetFirstRecordSet());

  CFieldSetDbl *fieldSetLat = NULL;
  CFieldSetDbl *fieldSetLon = NULL;
  CFieldSetDbl *fieldSetTimeStampDay = NULL;
  CFieldSetDbl *fieldSetTimeStampMillisecond = NULL;
  CFieldSetDbl *fieldSetTimeStampMicrosecond = NULL;

  int32_t count = dataSet->size();

  for (int32_t index = 0 ; index < count ; index++)
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
      

      
    if (CTools::IsDefaultValue(m_deltaTimeHighResolution))
    {
      std::string msg = "ERROR in CProductTopexSDR::ComputeHighResolutionFields - high resolution delta time has no been set";
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
  }

  // Restore current recordset pointer
  dataSet->SetCurrentRecordSet(currentRecordSetOld);
}
//----------------------------------------
void CProductTopexSDR::CheckConsistencyHighResolutionField(CFieldSetArrayDbl* fieldSetArrayDbl)
{
  if ( (fieldSetArrayDbl->m_nbDims <= 0) || (fieldSetArrayDbl->m_nbDims > 2) )
  {
    std::string msg = CTools::Format("ERROR - CProductTopexSDR::CheckConsistencyHighResolutionField() - Number of array dim %d not implemented for this method "
                                "(field '%s')", 
                                fieldSetArrayDbl->m_nbDims,
                                fieldSetArrayDbl->GetField()->GetKey().c_str());                                
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_UNIMPLEMENT_ERROR);
    throw (e);
  }

  if (CTools::IsDefaultValue(m_numHighResolutionMeasure))
  {
    std::string msg = CTools::Format("ERROR - CProductTopexSDR::CheckConsistencyHighResolutionField() - Number of high resolution measures is not intialized (equals default value"
                                "(field '%s')", 
                                fieldSetArrayDbl->GetField()->GetKey().c_str());                                

    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }
  if (CTools::IsDefaultValue(m_lowRateNumHighResolutionMeasure))
  {
    std::string msg = CTools::Format("ERROR - CProductTopexSDR::CheckConsistencyHighResolutionField() - Number of low rate high resolution measures is not intialized (equals default value"
                                "(field '%s')", 
                                fieldSetArrayDbl->GetField()->GetKey().c_str());                                

    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }

  if ((fieldSetArrayDbl->m_dim[0] != m_highRateNumHighResolutionMeasure) && (fieldSetArrayDbl->m_dim[0] != m_lowRateNumHighResolutionMeasure))
  {
    std::string msg = CTools::Format("ERROR - CProduct::CheckConsistencyHighResolutionField() - Array dim[0]: %d is not equal to number of high resolution measures: %d or %d "
                                "(field '%s')", 
                                fieldSetArrayDbl->m_dim[0],
                                m_highRateNumHighResolutionMeasure,
                                m_lowRateNumHighResolutionMeasure,
                                fieldSetArrayDbl->GetField()->GetKey().c_str());                                

    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }
}
//----------------------------------------
void CProductTopexSDR::PutFlatHighResolution(CDataSet* dataSet, CFieldSetArrayDbl* fieldSetArrayDbl)
{
  if (dataSet == NULL)
  {
    return;
  }
  if (fieldSetArrayDbl == NULL)
  {
    return;
  }

  CheckConsistencyHighResolutionField(fieldSetArrayDbl);

  CFieldSet* fieldSet = NULL;
  
  uint32_t dimFieldSetArrayDbl = fieldSetArrayDbl->m_dim[0];
  double ratio = CTools::Divide(dimFieldSetArrayDbl, m_numHighResolutionMeasure);

  if (CTools::IsDefaultValue(ratio))
  {
    std::string msg = CTools::Format("ERROR - CProductTopexSDR::PutFlatHighResolution() - Ratio is equals to 'default value' (dimFieldSetArrayDbl=%d, m_numHighResolutionMeasure=%d)."
                                "(field '%s')", 
                                dimFieldSetArrayDbl,
                                m_numHighResolutionMeasure,
                                fieldSetArrayDbl->GetField()->GetKey().c_str());                                
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_UNIMPLEMENT_ERROR);
    throw (e);
  }



  for (uint32_t i = 0 ; i < m_numHighResolutionMeasure ; i++)
  {
    fieldSet = NULL;
    if (fieldSetArrayDbl->m_nbDims == 1)
    {
      uint32_t index = static_cast<uint32_t>(i * ratio);

      CFieldSetDbl* fieldSetDblTmp = new CFieldSetDbl(fieldSetArrayDbl->GetName());
      fieldSetDblTmp->m_value = fieldSetArrayDbl->m_vector[index];
      fieldSetDblTmp->SetField(fieldSetArrayDbl->GetField());
      fieldSet = fieldSetDblTmp;
    }
    else if (fieldSetArrayDbl->m_nbDims == 2)
    {
      uint32_t index = static_cast<uint32_t>(i * ratio);

      CFieldSetArrayDbl* fieldSetArrayDblTmp = new CFieldSetArrayDbl(fieldSetArrayDbl->GetName());
      fieldSetArrayDblTmp->m_nbDims = 1;
      fieldSetArrayDblTmp->m_dim.Insert(fieldSetArrayDbl->m_dim[1]);
      fieldSetArrayDblTmp->m_vector.Insert(fieldSetArrayDbl->m_vector,
                                           index * fieldSetArrayDblTmp->m_dim[0],
                                           ((index + 1) * fieldSetArrayDblTmp->m_dim[0]));
      fieldSetArrayDblTmp->SetField(fieldSetArrayDbl->GetField());
      fieldSet = fieldSetArrayDblTmp;

    } 
    else
    {
      std::string msg = CTools::Format("ERROR - CProductTopexSDR::PutFlatHighResolution() - Number of array dim %d not implemented for this method "
                                  "(field '%s')", 
                                  fieldSetArrayDbl->m_nbDims,
                                  fieldSetArrayDbl->GetField()->GetKey().c_str());                                
      CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_UNIMPLEMENT_ERROR);
      throw (e);
    }
    
    InsertRecord(*dataSet, (i + 1));

    dataSet->InsertFieldSet(fieldSet->GetField()->GetKey(), fieldSet);
  }

}

//----------------------------------------
void CProductTopexSDR::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CProductTopexSDR Object at "<< this << std::endl;

  //------------------
  CProductTopex::Dump(fOut);
  //------------------

  fOut << "==> END Dump a CProductTopexSDR Object at "<< this << std::endl;

  fOut << std::endl;
}
//----------------------------------------



} // end namespace
