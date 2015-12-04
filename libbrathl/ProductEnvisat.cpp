
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
#include "ProductEnvisat.h" 

using namespace brathl;

namespace brathl
{

const std::string ENVISAT_SPH = "sph";
const std::string ENVISAT_MPH = "mph";



CProductEnvisat::CProductEnvisat()
{
  Init();
}


//----------------------------------------

CProductEnvisat::CProductEnvisat(const std::string& fileName)
      : CProduct(fileName)
{
  Init();
}


//----------------------------------------
CProductEnvisat::CProductEnvisat(const CStringList& fileNameList)
      : CProduct(fileNameList)

{
  Init();
}

//----------------------------------------

CProductEnvisat::~CProductEnvisat()
{

}

//----------------------------------------
void CProductEnvisat::Init()
{
  m_label = "Envisat product";

  m_forceLatMinCriteriaValue = -83.0;
  m_forceLatMaxCriteriaValue = 83.0;

  InitDateRef();

  //std::string productype = CTools::StringToUpper(m_fileList.m_productType);
  std::string productype = m_fileList.m_productType;
  if ( (productype.compare("RA2_GDR_2P") == 0) || (productype.compare("RA2_MWS_2P") == 0) )
  {
    m_highResolutionLatDiffFieldName = "hz18_lat_diff";
    m_highResolutionLonDiffFieldName = "hz18_lon_diff";
  }
  else
  {
    m_highResolutionLatDiffFieldName = "";
    m_highResolutionLonDiffFieldName = "";
  }

  m_timeStampFieldName = "dsr_time";
  m_latitudeFieldName = "lat";
  m_longitudeFieldName = "lon";
  //Set delta time in seconds between two high resolution measures (basic delta time / number of high resolution measures)
  m_numHighResolutionMeasure = 20;
  m_deltaTimeHighResolution = 1.114/m_numHighResolutionMeasure; 

  // Reference point of high resolution measures corresponds to the tenth point (index : (m_numHighResolutionMeasure - 1) / 2)
  m_refPoint = (m_numHighResolutionMeasure - 1) / 2;
  
  InitCriteriaInfo();
  

}

//----------------------------------------
void CProductEnvisat::InitCriteriaInfo()
{
  CProduct::InitCriteriaInfo();

  //-------------------------
  // Lat/Lon criteria info
  //-------------------------
  CCriteriaLatLonInfo* criteriaLatLonInfo = new CCriteriaLatLonInfo();

  criteriaLatLonInfo->SetDataRecord(ENVISAT_SPH);
  criteriaLatLonInfo->SetStartLatField("ra2_first_lat");
  criteriaLatLonInfo->SetEndLatField("ra2_last_lat");
  // WARNING -----------------------------------------
  // The longitude values are in decreasing order in the Envisat product
  // So values are from 'stop_long' to 'start_long'
  //---------------------------------------------------
  //criteriaLatLonInfo->SetStartLonField("ra2_first_long");
  //criteriaLatLonInfo->SetEndLonField("ra2_last_long");
  criteriaLatLonInfo->SetStartLonField("ra2_last_long");
  criteriaLatLonInfo->SetEndLonField("ra2_first_long");

  m_criteriaInfoMap.Insert(criteriaLatLonInfo->GetKey(), criteriaLatLonInfo); 

  //-------------------------
  // Datetime criteria info
  //-------------------------
  CCriteriaDatetimeInfo* criteriaDatetimeInfo = new CCriteriaDatetimeInfo();

  criteriaDatetimeInfo->SetDataRecord(ENVISAT_SPH);
  criteriaDatetimeInfo->SetStartDateField("ra2_first_record_time");
  criteriaDatetimeInfo->SetEndDateField("ra2_last_record_time");

  criteriaDatetimeInfo->SetRefDate(m_refDate);

  m_criteriaInfoMap.Insert(criteriaDatetimeInfo->GetKey(), criteriaDatetimeInfo); 

  //-------------------------
  // Pass criteria info
  //-------------------------
  /* uncomment this block when pass_number will be defined in data dictionary
  CCriteriaPassIntInfo* criteriaPassInfo = new CCriteriaPassIntInfo();

  criteriaPassInfo->SetDataRecord(ENVISAT_SPH);
  criteriaPassInfo->SetPassField("pass_number");

  m_criteriaInfoMap.Insert(criteriaPassInfo->GetKey(), criteriaPassInfo); 
  
  // END uncomment this block when pass_number will be defined in data dictionary */

  //-------------------------
  // Cycle criteria info
  //-------------------------
  CCriteriaCycleInfo* criteriaCycleInfo = new CCriteriaCycleInfo();

  criteriaCycleInfo->SetDataRecord(ENVISAT_MPH);
  criteriaCycleInfo->SetStartCycleField("cycle");

  m_criteriaInfoMap.Insert(criteriaCycleInfo->GetKey(), criteriaCycleInfo); 

}


//----------------------------------------
void CProductEnvisat::AddInternalHighResolutionFieldCalculation()
{
  m_arrayLongitudeFieldName.RemoveAll();
  m_arrayLatitudeFieldName.RemoveAll();
  m_arrayTimeStampFieldName.RemoveAll();

  std::string internalFieldName;

  CField* fieldTest = NULL;

  if (HasHighResolutionFieldCalculation())
  {
    // Warning : lat/lon diff high resolution fields doesn't exist for some records inside the product (e.g. avg_waveforms_mds - 18 Hz Waveforms MDS)
////    internalFieldName = MakeInternalFieldName(m_highResolutionLatDiffFieldName);
    
////    fieldTest = FindFieldByInternalName(internalFieldName, false);
    
    fieldTest = FindFieldByName(m_highResolutionLatDiffFieldName, false, NULL, false);
    if (fieldTest == NULL)
    { 
      m_highResolutionLatDiffFieldName = "";
    }
    else
    {
      internalFieldName = MakeInternalNameByAddingRoot(fieldTest->GetFullNameWithRecord());
      m_listInternalFieldName.InsertUnique( internalFieldName );
      m_fieldNameEquivalence.Insert(m_highResolutionLatDiffFieldName, internalFieldName, false);
    }


//    internalFieldName = MakeInternalFieldName(m_highResolutionLonDiffFieldName);

//    fieldTest = FindFieldByInternalName(internalFieldName, false);

    fieldTest = FindFieldByName(m_highResolutionLonDiffFieldName, false, NULL, false);
    if (fieldTest == NULL)
    { 
      m_highResolutionLonDiffFieldName = "";
    }
    else
    {
      internalFieldName = MakeInternalNameByAddingRoot(fieldTest->GetFullNameWithRecord());
      m_listInternalFieldName.InsertUnique( internalFieldName );
      m_fieldNameEquivalence.Insert(m_highResolutionLonDiffFieldName, internalFieldName, false);
    }
  }

  // Warning : lat/lon fields doesn't exist for some records inside the product (e.g. avg_waveforms_mds - 18 Hz Waveforms MDS)

//  internalFieldName = MakeInternalFieldName(m_latitudeFieldName);
//  fieldTest = FindFieldByInternalName(internalFieldName, false);

  fieldTest = FindFieldByName(m_latitudeFieldName, false, NULL, false);
  if (fieldTest != NULL)
  {
    internalFieldName = MakeInternalNameByAddingRoot(fieldTest->GetFullNameWithRecord());
    m_listInternalFieldName.InsertUnique( internalFieldName );
    m_fieldNameEquivalence.Insert(m_latitudeFieldName, internalFieldName, false);
    m_arrayLatitudeFieldName.InsertUnique(m_latitudeFieldName);
  }
  else
  {
    std::string otherName;
    otherName.append("ra2_mds.");
    otherName.append(m_latitudeFieldName);

    fieldTest = FindFieldByName(otherName, false, NULL, false);

    if (fieldTest != NULL) {
      internalFieldName = MakeInternalNameByAddingRoot(fieldTest->GetFullNameWithRecord());
      m_listInternalFieldName.InsertUnique( internalFieldName );
      m_fieldNameEquivalence.Insert(m_latitudeFieldName, internalFieldName, false);
      m_arrayLatitudeFieldName.InsertUnique(m_latitudeFieldName);
    }

  }

  AddSameFieldName(m_latitudeFieldName, m_arrayLatitudeFieldName);

//  internalFieldName = MakeInternalFieldName(m_longitudeFieldName);
//  fieldTest = FindFieldByInternalName(internalFieldName, false);

  fieldTest = FindFieldByName(m_longitudeFieldName, false, NULL, false);
  if (fieldTest != NULL)
  {
    internalFieldName = MakeInternalNameByAddingRoot(fieldTest->GetFullNameWithRecord());
    m_listInternalFieldName.InsertUnique( internalFieldName );
    m_fieldNameEquivalence.Insert(m_longitudeFieldName, internalFieldName, false);
    m_arrayLongitudeFieldName.InsertUnique(m_longitudeFieldName);
  }
  else
  {
    std::string otherName;
    otherName.append("ra2_mds.");
    otherName.append(m_longitudeFieldName);

    fieldTest = FindFieldByName(otherName, false, NULL, false);

    if (fieldTest != NULL) {
      internalFieldName = MakeInternalNameByAddingRoot(fieldTest->GetFullNameWithRecord());
      m_listInternalFieldName.InsertUnique( internalFieldName );
      m_fieldNameEquivalence.Insert(m_longitudeFieldName, internalFieldName, false);
      m_arrayLongitudeFieldName.InsertUnique(m_longitudeFieldName);
    }
  }

  AddSameFieldName(m_longitudeFieldName, m_arrayLongitudeFieldName);



//  internalFieldName = MakeInternalFieldName(m_timeStampFieldName);
//  m_listInternalFieldName.InsertUnique( internalFieldName );
//  m_fieldNameEquivalence.Insert(m_timeStampFieldName, internalFieldName, false);

  fieldTest = FindFieldByName(m_timeStampFieldName, false, NULL, false);
  if (fieldTest != NULL)
  {
    internalFieldName = MakeInternalNameByAddingRoot(fieldTest->GetFullNameWithRecord());
    m_listInternalFieldName.InsertUnique( internalFieldName );
    m_fieldNameEquivalence.Insert(m_timeStampFieldName, internalFieldName, false);
    m_arrayTimeStampFieldName.InsertUnique(m_timeStampFieldName);
  }


  AddSameFieldName(m_timeStampFieldName, m_arrayTimeStampFieldName);

}

//----------------------------------------
bool CProductEnvisat::HasHighResolutionFieldCalculation()
{
  return   (m_highResolutionLatDiffFieldName.empty() == false)
        && (m_highResolutionLonDiffFieldName.empty() == false);
}

//----------------------------------------
bool CProductEnvisat::HasHighResolutionFieldCalculationValue(CDataSet* dataset)
{
  if (dataset->size() <= 0)
  {
    std::string msg = "ERROR in CProductEnvisat::HasHighResolutionFieldCalculationValue - dataset is empty";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }

  if (HasHighResolutionFieldCalculation() == false)
  {
    return false;
  }

  CFieldSetArrayDbl *fieldSetLatDiff = dataset->GetFieldSetAsArrayDbl( m_fieldNameEquivalence.Exists(m_highResolutionLatDiffFieldName) );
  CFieldSetArrayDbl *fieldSetLonDiff = dataset->GetFieldSetAsArrayDbl( m_fieldNameEquivalence.Exists(m_highResolutionLonDiffFieldName) );
  if ( (fieldSetLatDiff == NULL) || (fieldSetLonDiff == NULL) )
  {
    return false;
  }

  return    HasHighResolutionFieldCalculationValue(dataset, fieldSetLatDiff) 
         && HasHighResolutionFieldCalculationValue(dataset, fieldSetLonDiff) ;
}
//----------------------------------------
bool CProductEnvisat::HasHighResolutionFieldCalculationValue(CDataSet* dataset, CFieldSetArrayDbl* fieldSetArrayDbl)
{

  if (dataset->size() <= 0)
  {
    std::string msg = "ERROR in CProductEnvisat::HasHighResolutionFieldCalculationValue - dataset is empty";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }

  CheckConsistencyHighResolutionField(fieldSetArrayDbl);

  // if all the values are the same then consider there are no values
  double value = fieldSetArrayDbl->m_vector[0];
  bool hasValues = false;
  

  // loop starts at the second value and test if values are same than the first one
  for (uint32_t i = 1 ; i < m_numHighResolutionMeasure ; i++)
  {
    if (value != fieldSetArrayDbl->m_vector[i])
    {
      hasValues = true;
      break;
    }
  }
  
  return hasValues;
}

//----------------------------------------
bool CProductEnvisat::IsHighResolutionField(CField* field)
{
  if (field == NULL)
  {
    return false;
  }


  if (typeid(*field) == typeid(CFieldBasic))
  {
    return IsParentHighResolutionField(field);
  }


  if (field->GetNbDims() != 1)
  {
    return IsParentHighResolutionField(field);
  }

  if ((field->GetDim())[0] != static_cast<int32_t>(m_numHighResolutionMeasure))
  {
    return IsParentHighResolutionField(field);
  }


  std::string name = CTools::StringToLower(field->GetName());
  // Note that m_highResolutionLatDiffFieldName and m_highResolutionLonDiffFieldName are set only for RA2_GDR_2P and RA2_MWS_2P
  if ( (name.compare(m_highResolutionLatDiffFieldName) == 0) || (name.compare(m_highResolutionLonDiffFieldName) == 0) )
  {
    return false;
  }

  if ( (name.find("hz18") != std::string::npos) || (name.find("18hz") != std::string::npos) )
  {
    return true;
  }

  return false;

}
//----------------------------------------
bool CProductEnvisat::IsParentHighResolutionField(CField* field)
{
  if (field == NULL)
  {
    return false;
  }

  CObList parentFieldList(false);
  
  // find the topmost parent fixed record Array of this field
  // topmost parent is at the beginning of the list, lowermost parent is at the end of the list 
  // find parent is for specifiec product (as Cryosat by now) because of their data structure (see time_orbit_data, for example)
  // It for reading CFieldRecord class data wih are an coda_array_class with fixed size and 
  // contains CFieldBasic class data or CFieldArray class data
  // or CFieldRecord class data that contains CFieldbasic class data or CFieldArray class data or CFiedlRecord class data ...etc, etc.
  bool readParent = FindParentToRead(field, &parentFieldList);

  if (readParent == false)
  {
    return false;
  }


  if (parentFieldList.empty())
  {
    return false;
  }


  CFieldRecord* topParentField = dynamic_cast<CFieldRecord*>(*(parentFieldList.begin()));
  if (topParentField == NULL)
  {
    std::string msg = CTools::Format("ERROR - CProductEnvisat::IsParentHighResolutionField - top parent Field list NULL()"
                              "(field '%s')", 
                              field->GetKey().c_str());                                
    CProductException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }


  if (topParentField->GetTypeClass() != coda_array_class) //test the type of the parent type class
  {
    std::string msg = CTools::Format("ERROR - CProductEnvisat::IsParentHighResolutionField - top parent Field typeClass (%s) must be coda_array_class "
                              "(field '%s')", 
                              coda_type_get_class_name(topParentField->GetTypeClass()),
                              topParentField->GetKey().c_str());                                
    CProductException e(msg, BRATHL_UNIMPLEMENT_ERROR);
    throw (e);

  }

  if (topParentField->GetNbDims() != 1 )
  {
    std::string msg = CTools::Format("ERROR - CProductEnvisat::IsParentHighResolutionField - Number of array dim %d not implemented for this method "
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

  std::string name = CTools::StringToLower(topParentField->GetName());
  
  if (name.compare("data_blk_info") == 0) 
  {
    return true;
  }


  return false;
}

//----------------------------------------
bool CProductEnvisat::FindParentToRead(CField* fromField, CObList* parentFieldList)
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
void CProductEnvisat::InitDateRef()
{
  m_refDate = REF20000101;
}

//----------------------------------------
void CProductEnvisat::ProcessHighResolutionWithFieldCalculation()
{
  if (m_dataSet.size() <= 0)
  {
    return;
  }
 
  // Envisat off line product (RA2_GDR_2P, RA2_MWS_2P) have high resolution lat. diff. and  high resolution lon. diff.
  // define in the data dictionary, but in some data files, these values have not been set
  // so we consider that if all values in these fields are the same, they are not present
  // in the case, compute high resolution field without high resolution field calculation (by estimation).
  if (HasHighResolutionFieldCalculationValue(&m_dataSet) == false)
  {
    ProcessHighResolutionWithoutFieldCalculation();
    return;
  }

  // Note that m_highResolutionLatDiffFieldName and m_highResolutionLonDiffFieldName are set only for RA2_GDR_2P and RA2_MWS_2P 
  CFieldSetArrayDbl *fieldSetLatDiff = m_dataSet.GetFieldSetAsArrayDbl( m_fieldNameEquivalence.Exists(m_highResolutionLatDiffFieldName) );
  CFieldSetArrayDbl *fieldSetLonDiff = m_dataSet.GetFieldSetAsArrayDbl( m_fieldNameEquivalence.Exists(m_highResolutionLonDiffFieldName) );
  if ( (fieldSetLatDiff == NULL) || (fieldSetLonDiff == NULL) )
  {
    std::string msg = CTools::Format("ERROR in CProductEnvisat::ProcessHighResolutionWithFieldCalculation - at least one of the fields '%s' and '%s' has not been read or doesn't exist", 
                                m_highResolutionLatDiffFieldName.c_str(),
                                m_highResolutionLonDiffFieldName.c_str());
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }

  // Get the current recordset (that which has been just read)
  CRecordSet* recordSetToProcess = m_dataSet.GetCurrentRecordSet();

  if (recordSetToProcess == NULL)
  {
    std::string msg = "ERROR in CProductEnvisat::ProcessHighResolutionWithFieldCalculation - No current recordset";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
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


    // data is an array of double
    if (fieldSetArrayDbl != NULL)
    {
      // field is latitude difference or longitude difference fields, and is not hidden
      // transform array of double  in a set of records with a double value
      // compare pointer value !!!
      if ( (fieldSetArrayDbl == fieldSetLatDiff) || (fieldSetArrayDbl == fieldSetLonDiff))
      {
        PutFlatHighResolution(&dataSetHighResolution, fieldSetArrayDbl);
      }    
      // the field is hidden, it is just use for calculation, don't set it in output
      else if (fieldSetArrayDbl->GetField()->IsHidden() == false)
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
    }
    
    // data is a double
    if (fieldSetDbl != NULL)
    {
      // the field is hidden, it is just use for calculation, don't set it in output
      if (fieldSetDbl->GetField()->IsHidden() == false)
      {
        // field is a double
        // put it into the set of records (value is repeated for each record)
        Put(&dataSetHighResolution, fieldSetDbl);      
      }    
    }    
  }

  if (dataSetHighResolution.size() != m_numHighResolutionMeasure)
  {
    std::string msg = CTools::Format("ERROR - CProductEnvisat::ProcessHighResolutionWithFieldCalculation() - Numberd of record %ld is not equal number of high resolution measures %d",
                                (long)dataSetHighResolution.size(),
                                m_numHighResolutionMeasure);

    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }

  // Compute high resolution fields
  ComputeHighResolutionFields(&dataSetHighResolution);      


  // Erase the current recordset in the main dataset 
  m_dataSet.Erase(recordSetToProcess);
  
  recordSetToProcess = NULL;
  
  fieldSetLatDiff = NULL;
  fieldSetLonDiff = NULL;
  
  
  // Insert the record of the new dataset in the main dataset
  m_dataSet.InsertDataset(&dataSetHighResolution);
}
//----------------------------------------
void CProductEnvisat::ComputeHighResolutionFields(CDataSet* dataSet)
{
  // Save current recordset pointer
  CRecordSet* currentRecordSetOld = dataSet->GetCurrentRecordSet();

  // Go to the first recordset
  //dataSet->SetCurrentRecordSet(dataSet->GetFirstRecordSet());

  CFieldSetDbl *fieldSetLatDiff = NULL;
  CFieldSetDbl *fieldSetLonDiff = NULL;

  CFieldSetDbl *fieldSetLat = NULL;
  CFieldSetDbl *fieldSetLon = NULL;
  CFieldSetDbl *fieldSetTimeStamp = NULL;

  size_t count = dataSet->size();

  for (size_t index = 0 ; index < count ; index++)
  {
    dataSet->SetCurrentRecordSet(index);

    fieldSetLatDiff = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_highResolutionLatDiffFieldName) );
    fieldSetLonDiff = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_highResolutionLonDiffFieldName) );

    if ( (fieldSetLatDiff == NULL) || (fieldSetLonDiff == NULL) )
    {
      std::string msg = CTools::Format("ERROR in CProductEnvisat::ComputeHighResolutionFields - no data found for at least one of the fields '%s' and '%s'", 
                                  m_highResolutionLatDiffFieldName.c_str(),
                                  m_highResolutionLonDiffFieldName.c_str());
      CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_RANGE_ERROR);
      throw (e);
    }


    //fieldSetLat = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_latitudeFieldName) );
    //fieldSetLon = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_longitudeFieldName) );
    //fieldSetTimeStamp = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampFieldName) );

    CStringArray::const_iterator it;

    // Compute latitude
    for (it = m_arrayLatitudeFieldName.begin() ; it != m_arrayLatitudeFieldName.end() ; it++)
    {
      fieldSetLat = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(*it) );
      if (fieldSetLat != NULL)
      {
        fieldSetLat->m_value = CTools::Plus(fieldSetLat->m_value, fieldSetLatDiff->m_value);
      }
    }

    // Compute longitude
    for (it = m_arrayLongitudeFieldName.begin() ; it != m_arrayLongitudeFieldName.end() ; it++)
    {
      fieldSetLon = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(*it) );
      if (fieldSetLon != NULL)
      {
        fieldSetLon->m_value = CTools::Plus(fieldSetLon->m_value, fieldSetLonDiff->m_value);
      }
    }


    // Compute timestamp
    for (it = m_arrayTimeStampFieldName.begin() ; it != m_arrayTimeStampFieldName.end() ; it++)
    {
      fieldSetTimeStamp = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(*it) );
      if (fieldSetTimeStamp != NULL)
      {
        fieldSetTimeStamp->m_value = CTools::Plus(fieldSetTimeStamp->m_value, 
                     CTools::Multiply(m_deltaTimeHighResolution, static_cast<double>(index - m_refPoint)));
      }

    }

    if (fieldSetLatDiff->GetField()->IsHidden())
    {
      dataSet->EraseFieldSet(fieldSetLatDiff->GetField()->GetKey());
    }

    if (fieldSetLonDiff->GetField()->IsHidden())
    {
      dataSet->EraseFieldSet(fieldSetLonDiff->GetField()->GetKey());
    }

  }

  // Restore current recordset pointer
  dataSet->SetCurrentRecordSet(currentRecordSetOld);
}

//----------------------------------------
void CProductEnvisat::ProcessHighResolutionWithoutFieldCalculation()
{
  if (m_dataSet.size() <= 0)
  {
    return;
  }
 
  // Get the current recordset (that which has been just read)
  CRecordSet* recordSetToProcess = m_dataSet.GetCurrentRecordSet();

  if (recordSetToProcess == NULL)
  {
    std::string msg = "ERROR in CProductEnvisat::ProcessHighResolutionWithoutFieldCalculation - No current recordset";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }


  CFieldSetArrayDbl *fieldSetLatDiff = m_dataSet.GetFieldSetAsArrayDbl( m_fieldNameEquivalence.Exists(m_highResolutionLatDiffFieldName) );
  CFieldSetArrayDbl *fieldSetLonDiff = m_dataSet.GetFieldSetAsArrayDbl( m_fieldNameEquivalence.Exists(m_highResolutionLonDiffFieldName) );

  //CFieldSetDbl *fieldSetLat = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_latitudeFieldName) );
  //CFieldSetDbl *fieldSetLon = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_longitudeFieldName) );
  //CFieldSetDbl *fieldSetTimeStamp = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampFieldName) );
  CFieldSetDbl *fieldSetLat = NULL;
  CFieldSetDbl *fieldSetLon = NULL;
  CFieldSetDbl *fieldSetTimeStamp = NULL;

  CStringArray::const_iterator itArray;

  CStringArray fieldUsed;

  for (itArray = m_arrayLatitudeFieldName.begin() ; itArray != m_arrayLatitudeFieldName.end() ; itArray++)
  {
    CFieldSetDbl *fieldSetTemp = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(*itArray) );
    if (fieldSetTemp != NULL)
    {
      fieldUsed.InsertUnique(*itArray);
      if (fieldSetLat == NULL)
      {
        fieldSetLat = fieldSetTemp;
      }
    }
  }

  static bool warnedLat = false;

  if ((fieldUsed.size() > 1) && (fieldSetLat != NULL) && !warnedLat)
  {
    warnedLat = true;
    std::string msg = CTools::Format("WARNING - you use more than one field as latitude: %s.\n"
                                "Only field %s will be use to compute latitude delta for high resolution fields.\n",
                                fieldUsed.ToString().c_str(),
                                fieldSetLat->GetName().c_str());
    CTrace::Tracer(1, msg); 
  }

  fieldUsed.RemoveAll();

  for (itArray = m_arrayLongitudeFieldName.begin() ; itArray != m_arrayLongitudeFieldName.end() ; itArray++)
  {
    CFieldSetDbl *fieldSetTemp = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(*itArray) );
    if (fieldSetTemp != NULL)
    {
      fieldUsed.InsertUnique(*itArray);
      if (fieldSetLon == NULL)
      {
        fieldSetLon = fieldSetTemp;
      }
    }
  }

  static bool warnedLon = false;

  if ((fieldUsed.size() > 1) && (fieldSetLon != NULL) && !warnedLon)
  {
    warnedLon = true;
    std::string msg = CTools::Format("WARNING - you use more than one field as longitude: %s.\n"
                                "Only field %s will be use to compute longitude delta for high resolution fields.\n",
                                fieldUsed.ToString().c_str(),
                                fieldSetLon->GetName().c_str());
    CTrace::Tracer(1, msg); 
  }

  fieldUsed.RemoveAll();

  for (itArray = m_arrayTimeStampFieldName.begin() ; itArray != m_arrayTimeStampFieldName.end() ; itArray++)
  {
    CFieldSetDbl *fieldSetTemp = m_dataSet.GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(*itArray) );
    if (fieldSetTemp != NULL)
    {
      fieldUsed.InsertUnique(*itArray);
      if (fieldSetTimeStamp == NULL)
      {
        fieldSetTimeStamp = fieldSetTemp;
      }
    }
  }
  static bool warnedTimeStamp = false;

  if ((fieldUsed.size() > 1) && (fieldSetTimeStamp != NULL) && !warnedTimeStamp)
  {
    warnedTimeStamp = true;
    std::string msg = CTools::Format("WARNING - you use more than one field as date/time: %s.\n"
                                "Only field %s will be use to compute date/time delta for high resolution fields.\n",
                                fieldUsed.ToString().c_str(),
                                fieldSetLon->GetName().c_str());
    CTrace::Tracer(1, msg); 
  }
  // Warning Lat/lon doesn't exist for some record (e.g. avg_waveforms_mds - 18 Hz Waveforms MDS)
/*
  if (fieldSetLat == NULL)
  {
    std::string msg = "ERROR in CProductEnvisat::ProcessHighResolutionWithoutFieldCalculation - latitude field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }

  if (fieldSetLon == NULL)
  {
    std::string msg = "ERROR in CProductEnvisat::ProcessHighResolutionWithoutFieldCalculation - longitude field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }
*/
  if (fieldSetTimeStamp == NULL)
  {
    std::string msg = "ERROR in CProductEnvisat::ProcessHighResolutionWithoutFieldCalculation - timestamp field has not been read";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_LOGIC_ERROR);
    throw (e);
  }

  // if time difference between the the current timestamp and the previous timestamp is greater than 2 seconds 
  // or if current timestamp is less or equal than the previous timestamp
  // skip the current recordset (data will be lost)
  if  ( (fieldSetTimeStamp->m_value - m_previousTimeStamp > 2.0) ||
        (fieldSetTimeStamp->m_value <= m_previousTimeStamp) )
  {
    std::string msg = CTools::Format("INFO - record skipped due to inconsistency between two measures\n"
                                "\t previous record --> timestamp %f seconds\n"
                                "\t current record --> timestamp %f seconds\n"
                                "\t timestamp difference is %f seconds\n",
                                m_previousTimeStamp,
                                fieldSetTimeStamp->m_value,
                                fieldSetTimeStamp->m_value - m_previousTimeStamp);
    CTrace::Tracer(3, msg); 

    if (fieldSetLat != NULL)
    {
      m_previousLatitude = fieldSetLat->m_value;
    }

    if (fieldSetLon != NULL)
    {
      m_previousLongitude = fieldSetLon->m_value;
    }

    m_previousTimeStamp = fieldSetTimeStamp->m_value;
    
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

  // Warning Lat/lon doesn't exist for some record (e.g. avg_waveforms_mds - 18 Hz Waveforms MDS)
  // So m_previousLatitude and m_previousLongitude can be equal to default value
  /*
  if (isDefaultValue(m_previousLatitude))
  {
    std::string msg = "ERROR in CProductEnvisat::ProcessHighResolutionWithoutFieldCalculation - previous latitude value read is inconsistent (is default value)";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }
  if (isDefaultValue(m_previousLongitude))
  {
    std::string msg = "ERROR in CProductEnvisat::ProcessHighResolutionWithoutFieldCalculation - previous longitude value read is inconsistent (is default value)";
    CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }
*/
  double deltaLat;
  setDefaultValue(deltaLat);
  double deltaLon;
  setDefaultValue(deltaLon);

  // Compute delta latitude an deltat longitude.
  if ( ! isDefaultValue(m_previousLatitude) ) 
  {
    deltaLat = (fieldSetLat->m_value - m_previousLatitude) / m_numHighResolutionMeasure;
  }

  if ( ! isDefaultValue(m_previousLongitude) ) 
  {
    deltaLon = (fieldSetLon->m_value - m_previousLongitude) / m_numHighResolutionMeasure;

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
      std::string msg = CTools::Format("ERROR in CProductEnvisat::ProcessHighResolutionWithoutFieldCalculation - delta of longitude is set to an unexpected value: %f "
                                  "\n\tcurrent datetime: %f current latitude: %f current longitude: %f"
                                  "\n\tprevious datetime: %f previous latitude: %f previous longitude: %f",
                                  deltaLon,
                                  fieldSetTimeStamp->m_value, 
                                  (fieldSetLat != NULL ? fieldSetLat->m_value : CTools::m_defaultValueDOUBLE), 
                                  (fieldSetLon != NULL ? fieldSetLon->m_value : CTools::m_defaultValueDOUBLE), 
                                  m_previousTimeStamp,
                                  m_previousLatitude,
                                  m_previousLongitude);
      CProductException e(msg, m_currFileName, GetProductClass(), GetProductType(), BRATHL_INCONSISTENCY_ERROR);
      throw (e);
    }
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
      // field is latitude difference or longitude difference fields, and is not hidden
      // transform array of double  in a set of records with a double value
      // compare pointer value !!!
      if ( (fieldSetArrayDbl == fieldSetLatDiff) || (fieldSetArrayDbl == fieldSetLonDiff))
      {
        PutFlatHighResolution(&dataSetHighResolution, fieldSetArrayDbl);
      }    
      // field is a high resolution field 
      // transform array of double  in a set of records with a double value
      else if ( fieldSetArrayDbl->GetField()->IsHighResolution() )
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
  
  if (fieldSetLat != NULL)
  {
    m_previousLatitude = fieldSetLat->m_value;
  }

  if (fieldSetLon != NULL)
  {
    m_previousLongitude = fieldSetLon->m_value;
  }

  m_previousTimeStamp = fieldSetTimeStamp->m_value;

  // Erase the current recordset in the main dataset 
  m_dataSet.Erase(recordSetToProcess);
  
  recordSetToProcess = NULL;
  
  fieldSetLat = NULL;
  fieldSetLon = NULL;
  fieldSetTimeStamp = NULL;
   
  // Insert the record of the new dataset in the main dataset
  m_dataSet.InsertDataset(&dataSetHighResolution);
  
}

//----------------------------------------
void CProductEnvisat::ComputeHighResolutionFields(CDataSet* dataSet, double deltaLat, double deltaLon)
{
//  if ( isDefaultValue(deltaLat) || isDefaultValue(deltaLon) ) 
//  {
//    return;
//  }

  // Save current recordset pointer
  CRecordSet* currentRecordSetOld = dataSet->GetCurrentRecordSet();

  //dataSet->SetCurrentRecordSet(dataSet->GetFirstRecordSet());

  CFieldSetDbl *fieldSetLat = NULL;
  CFieldSetDbl *fieldSetLon = NULL;
  CFieldSetDbl *fieldSetTimeStamp = NULL;

  size_t count = dataSet->size();

  for (size_t index = 0 ; index < count ; index++)
  {
    dataSet->SetCurrentRecordSet(index);

    //fieldSetLat = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_latitudeFieldName) );
    //fieldSetLon = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_longitudeFieldName) );
    //fieldSetTimeStamp = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(m_timeStampFieldName) );

    CStringArray::const_iterator it;

    // Compute latitude
    for (it = m_arrayLatitudeFieldName.begin() ; it != m_arrayLatitudeFieldName.end() ; it++)
    {
      fieldSetLat = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(*it) );
      if (fieldSetLat != NULL)
      {
        fieldSetLat->m_value = CTools::Plus(fieldSetLat->m_value, 
                                            CTools::Multiply(deltaLat, 
                                                             static_cast<double>(index - m_refPoint)));
      }
    }

    // Compute longitude
    for (it = m_arrayLongitudeFieldName.begin() ; it != m_arrayLongitudeFieldName.end() ; it++)
    {
      fieldSetLon = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(*it) );
      if (fieldSetLon != NULL)
      {
        fieldSetLon->m_value = CTools::Plus(fieldSetLon->m_value, 
                                            CTools::Multiply(deltaLon, 
                                                             static_cast<double>(index - m_refPoint)));
      }
    }

    // Compute timestamp
    for (it = m_arrayTimeStampFieldName.begin() ; it != m_arrayTimeStampFieldName.end() ; it++)
    {
      fieldSetTimeStamp = dataSet->GetFieldSetAsDbl( m_fieldNameEquivalence.Exists(*it) );
      if (fieldSetTimeStamp != NULL)
      {
        fieldSetTimeStamp->m_value = CTools::Plus(fieldSetTimeStamp->m_value, 
                                                  CTools::Multiply(m_deltaTimeHighResolution, 
                                                                   static_cast<double>(index - m_refPoint)));
      }

    }
  }

  // Restore current recordset pointer
  dataSet->SetCurrentRecordSet(currentRecordSetOld);
}

//----------------------------------------
void CProductEnvisat::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CProductEnvisat Object at "<< this << std::endl;

  //------------------
  CProduct::Dump(fOut);
  //------------------

  fOut << "==> END Dump a CProductEnvisat Object at "<< this << std::endl;

  fOut << std::endl;

}


} // end namespace
