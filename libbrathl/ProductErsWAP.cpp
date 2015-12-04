
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
#include "ProductErsWAP.h" 

using namespace brathl;

namespace brathl
{


CProductErsWAP::CProductErsWAP()
{
  Init();
}


//----------------------------------------

CProductErsWAP::CProductErsWAP(const std::string& fileName)
      : CProductErs(fileName)
{
  Init();  
}


//----------------------------------------
CProductErsWAP::CProductErsWAP(const CStringList& fileNameList)
      : CProductErs(fileNameList)

{
  Init();
}

//----------------------------------------

CProductErsWAP::~CProductErsWAP()
{

}

//----------------------------------------
void CProductErsWAP::InitDateRef()
{
  m_refDate = REF19500101;
}
//----------------------------------------
void CProductErsWAP::Init()
{
  m_label = "Ers Radar Altimeter Waveform product";

  InitDateRef();

  m_timeStampDayFieldName = "Source_Packet_UTC.days";
  m_timeStampMillisecondFieldName = "Source_Packet_UTC.milliseconds";
  m_timeStampMicrosecondFieldName = "Source_Packet_UTC.microseconds";
  
  //Set delta time in seconds between two high resolution measures (basic delta time / number of high resolution measures)
  m_numHighResolutionMeasure = 20;
  m_deltaTimeHighResolution = 0.98/m_numHighResolutionMeasure; 

  // Reference point of high resolution measures corresponds to the tenth point (index : (m_numHighResolutionMeasure - 1) / 2)
  m_refPoint = (m_numHighResolutionMeasure - 1) / 2;

  InitCriteriaInfo();

}

//----------------------------------------
void CProductErsWAP::InitCriteriaInfo()
{
  CProduct::InitCriteriaInfo();

}
//----------------------------------------
bool CProductErsWAP::IsDirectHighResolutionField(CField* field)
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
bool CProductErsWAP::IsHighResolutionField(CField* field)
{
  if (field == NULL)
  {
    return false;
  }

  CObList parentFieldList(false);
  
  // find the topmost parent fixed record Array of this field
  // topmost parent is at the beginning of the list, lowermost parent is at the end of the list 
  // find parent is for specifiec product  because of their data structure 
  // It for reading CFieldRecord class data wih are an coda_array_class with fixed size and 
  // contains CFieldBasic class data or CFieldArray class data
  // or CFieldRecord class data that contains CFieldbasic class data or CFieldArray class data or CFiedlRecord class data ...etc, etc.
  bool readParent = FindParentToRead(field, &parentFieldList);

  if (readParent == false)
  {
    return IsDirectHighResolutionField(field);
  }


  if (parentFieldList.empty())
  {
    return false;
  }


  CFieldRecord* topParentField = dynamic_cast<CFieldRecord*>(*(parentFieldList.begin()));
  if (topParentField == NULL)
  {
    std::string msg = CTools::Format("ERROR - CProductErsWAP::IsHighResolutionField - top parent Field list NULL()"
                              "(field '%s')", 
                              field->GetKey().c_str());                                
    CProductException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }


  if (topParentField->GetTypeClass() != coda_array_class) //test the type of the parent type class
  {
    std::string msg = CTools::Format("ERROR - CProductErsWAP::IsHighResolutionField - top parent Field typeClass (%s) must be coda_array_class "
                              "(field '%s')", 
                              coda_type_get_class_name(topParentField->GetTypeClass()),
                              topParentField->GetKey().c_str());                                
    CProductException e(msg, BRATHL_UNIMPLEMENT_ERROR);
    throw (e);

  }

  if (topParentField->GetNbDims() != 1 )
  {
    std::string msg = CTools::Format("ERROR - CProductErsWAP::IsHighResolutionField - Number of array dim %d not implemented for this method "
                              "(field '%s')", 
                              topParentField->GetNbDims(),
                              topParentField->GetKey().c_str());                                
    CProductException e(msg, BRATHL_UNIMPLEMENT_ERROR);
    throw (e);
  }

  if ((topParentField->GetDim())[0] != static_cast<int32_t>(m_numHighResolutionMeasure))
  {
    return false;
  }

  return true;
}
//----------------------------------------
bool CProductErsWAP::FindParentToRead(CField* fromField, CObList* parentFieldList)
{  
  if (fromField == NULL)
  {
    return false;
  }

  CField* parentField = m_tree.FindParent(fromField);
  if (parentField == NULL)
  {
    return false;
  }

  CFieldRecord* parentFieldRecord = dynamic_cast<CFieldRecord*>(parentField);

  bool readParent =  (parentField->GetTypeClass() == coda_array_class) 
                  && (parentField->IsFixedSize())
                  && (parentFieldRecord != NULL);

  // Insert parent at the top of the list 
  // topmost parent is at the beginning of the list, lowermost parent is at the end of the list 
  parentFieldList->Insert(parentField, false);

  if (readParent)
  {
    return true;
  }
  return FindParentToRead(parentField, parentFieldList);
}
//----------------------------------------
void CProductErsWAP::AddInternalHighResolutionFieldCalculation()
{
  std::string internalFieldName;
  CField* fieldTest = NULL;

  fieldTest = FindFieldByName(m_timeStampDayFieldName, false, NULL, false);
  if (fieldTest != NULL)
  {
    internalFieldName = MakeInternalNameByAddingRoot(fieldTest->GetFullNameWithRecord());
    m_listInternalFieldName.InsertUnique( internalFieldName );
    m_fieldNameEquivalence.Insert(m_timeStampDayFieldName, internalFieldName, false);
  }

  fieldTest = FindFieldByName(m_timeStampMillisecondFieldName, false, NULL, false);
  if (fieldTest != NULL)
  {
    internalFieldName = MakeInternalNameByAddingRoot(fieldTest->GetFullNameWithRecord());
    m_listInternalFieldName.InsertUnique( internalFieldName );
    m_fieldNameEquivalence.Insert(m_timeStampMillisecondFieldName, internalFieldName, false);
  }

  
  fieldTest = FindFieldByName(m_timeStampMicrosecondFieldName, false, NULL, false);
  if (fieldTest != NULL)
  {
    internalFieldName = MakeInternalNameByAddingRoot(fieldTest->GetFullNameWithRecord());
    m_listInternalFieldName.InsertUnique( internalFieldName );
    m_fieldNameEquivalence.Insert(m_timeStampMicrosecondFieldName, internalFieldName, false);
  }


}
//----------------------------------------
void CProductErsWAP::ProcessHighResolutionWithoutFieldCalculation()
{
  if (m_dataSet.size() <= 0)
  {
    return;
  }
 
  // Get the current recordset (that which has been just read)
  CRecordSet* recordSetToProcess = m_dataSet.GetCurrentRecordSet();

  if (recordSetToProcess == NULL)
  {
    std::string msg = "ERROR in CProductErsWAP::ProcessHighResolutionWithoutFieldCalculation - No current recordset";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }

  CFieldSetDbl *fieldSetTimeStampDay = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampDayFieldName) );
  CFieldSetDbl *fieldSetTimeStampMillisecond = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampMillisecondFieldName) );
  CFieldSetDbl *fieldSetTimeStampMicrosecond = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampMicrosecondFieldName) );


  if (fieldSetTimeStampDay == NULL)
  {
    std::string msg = "ERROR in CProductErsWAP::ProcessHighResolutionWithoutFieldCalculation - timestamp day field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }
  if (fieldSetTimeStampMillisecond == NULL)
  {
    std::string msg = "ERROR in CProductErsWAP::ProcessHighResolutionWithoutFieldCalculation - timestamp millisecond field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }
  if (fieldSetTimeStampMicrosecond == NULL)
  {
    std::string msg = "ERROR in CProductErsWAP::ProcessHighResolutionWithoutFieldCalculation - timestamp microsecond field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }


  // if time difference between the the current timestamp and the previous timestamp is greater than 2 seconds 
  // or if current timestamp is less or equal than the previous timestamp
  // skip the current recordset (data will be lost)
  

  // WARNING -  fieldSetTimeStampDay,  fieldSetTimeStampMillisecond, fieldSetTimeStampMicrosecond are converted to SI
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
  ComputeHighResolutionFields(&dataSetHighResolution);      

  m_previousTimeStamp = timeStamp;

  // Erase the current recordset in the main dataset 
  m_dataSet.Erase(recordSetToProcess);
  
  recordSetToProcess = NULL;
  
  fieldSetTimeStampDay = NULL;
  fieldSetTimeStampMillisecond = NULL;
  fieldSetTimeStampMicrosecond = NULL;
   
  // Insert the record of the new dataset in the main dataset
  m_dataSet.InsertDataset(&dataSetHighResolution);
  
}

//----------------------------------------
void CProductErsWAP::ComputeHighResolutionFields(CDataSet* dataSet)
{
  // Save current recordset pointer
  CRecordSet* currentRecordSetOld = dataSet->GetCurrentRecordSet();

  //dataSet->SetCurrentRecordSet(dataSet->GetFirstRecordSet());

  CFieldSetDbl *fieldSetTimeStampDay = NULL;
  CFieldSetDbl *fieldSetTimeStampMillisecond = NULL;
  CFieldSetDbl *fieldSetTimeStampMicrosecond = NULL;

  size_t count = dataSet->size();

  for (size_t index = 0 ; index < count ; index++)
  {
    dataSet->SetCurrentRecordSet(index);

    fieldSetTimeStampDay = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampDayFieldName) );
    fieldSetTimeStampMillisecond = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampMillisecondFieldName) );
    fieldSetTimeStampMicrosecond = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampMicrosecondFieldName) );

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
      std::string msg = "ERROR in CProductErsWAP::ComputeHighResolutionFields - high resolution delta time has no been set";
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
void CProductErsWAP::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CProductErsWAP Object at "<< this << std::endl;

  //------------------
  CProduct::Dump(fOut);
  //------------------

  fOut << "==> END Dump a CProductErsWAP Object at "<< this << std::endl;

  fOut << std::endl;

}


} // end namespace
