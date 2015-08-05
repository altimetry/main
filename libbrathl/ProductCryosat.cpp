
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

#include "Stl.h" 

#include "TraceLog.h" 
#include "Tools.h" 
#include "Exception.h" 
#include "ProductCryosat.h" 

using namespace brathl;

namespace brathl
{


const string CRYOSAT_SPH = "sph";
const string CRYOSAT_MPH = "mph";

CProductCryosat::CProductCryosat()
{
  Init();  
}


//----------------------------------------

CProductCryosat::CProductCryosat(const string& fileName)
      : CProduct(fileName)
{
  Init();    
}


//----------------------------------------
CProductCryosat::CProductCryosat(const CStringList& fileNameList)
      : CProduct(fileNameList)

{
  Init();  
}

//----------------------------------------

CProductCryosat::~CProductCryosat()
{

}
//----------------------------------------
bool CProductCryosat::FindParentToRead(CField* fromField, CObList* parentFieldList)
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
void CProductCryosat::InitDateRef()
{
  m_refDate = REF20000101;
}
//----------------------------------------
void CProductCryosat::Init()
{
  m_fieldsHaveDefaultValue = false;

  m_label = "Cryosat product";

  //m_forceLatMinCriteriaValue = -89.0;
  //m_forceLatMaxCriteriaValue = 89.0;

  m_numHighResolutionMeasure = 20;

  m_latitudeFieldName = "lat";
  m_longitudeFieldName = "lon";

  InitDateRef();
  
  InitCriteriaInfo();

}
//----------------------------------------
void CProductCryosat::InitCriteriaInfo()
{
  CProduct::InitCriteriaInfo();

  //-------------------------
  // Lat/Lon criteria info
  //-------------------------
  CCriteriaLatLonInfo* criteriaLatLonInfo = new CCriteriaLatLonInfo();

  criteriaLatLonInfo->SetDataRecord(CRYOSAT_SPH);
  criteriaLatLonInfo->SetStartLatField("start_lat");
  criteriaLatLonInfo->SetEndLatField("stop_lat");

  // WARNING -----------------------------------------
  // The longitude values are in decreasing order in the Cryosat product
  // So values are from 'stop_long' to 'start_long'
  //---------------------------------------------------
//  criteriaLatLonInfo->SetStartLonField("start_long");
//  criteriaLatLonInfo->SetEndLonField("stop_long");
  criteriaLatLonInfo->SetStartLonField("stop_long");
  criteriaLatLonInfo->SetEndLonField("start_long");

  m_criteriaInfoMap.Insert(criteriaLatLonInfo->GetKey(), criteriaLatLonInfo); 

  //-------------------------
  // Datetime criteria info
  //-------------------------
  CCriteriaDatetimeInfo* criteriaDatetimeInfo = new CCriteriaDatetimeInfo();

  criteriaDatetimeInfo->SetDataRecord(CRYOSAT_SPH);
  criteriaDatetimeInfo->SetStartDateField("start_record_tai_time");
  criteriaDatetimeInfo->SetEndDateField("stop_record_tai_time");

  criteriaDatetimeInfo->SetRefDate(m_refDate);

  m_criteriaInfoMap.Insert(criteriaDatetimeInfo->GetKey(), criteriaDatetimeInfo); 

  //-------------------------
  // Pass criteria info
  //-------------------------
  CCriteriaPassStringInfo* criteriaPassInfo = new CCriteriaPassStringInfo();

  criteriaPassInfo->SetDataRecord(CRYOSAT_SPH);
  criteriaPassInfo->SetStartPassField("ascending_flag");

  m_criteriaInfoMap.Insert(criteriaPassInfo->GetKey(), criteriaPassInfo); 

  //-------------------------
  // Cycle criteria info
  //-------------------------
  CCriteriaCycleInfo* criteriaCycleInfo = new CCriteriaCycleInfo();

  criteriaCycleInfo->SetDataRecord(CRYOSAT_MPH);
  criteriaCycleInfo->SetStartCycleField("cycle");

  m_criteriaInfoMap.Insert(criteriaCycleInfo->GetKey(), criteriaCycleInfo); 

}

//----------------------------------------
bool CProductCryosat::IsHighResolutionField(CField* field)
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
    string msg = CTools::Format("ERROR - CProductCryosat::IsHighResolutionField - top parent Field list NULL()"
                              "(field '%s')", 
                              field->GetKey().c_str());                                
    CProductException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }


  if (topParentField->GetTypeClass() != coda_array_class) //test the type of the parent type class
  {
    string msg = CTools::Format("ERROR - CProductCryosat::IsHighResolutionField - top parent Field typeClass (%s) must be coda_array_class "
                              "(field '%s')", 
                              coda_type_get_class_name(topParentField->GetTypeClass()),
                              topParentField->GetKey().c_str());                                
    CProductException e(msg, BRATHL_UNIMPLEMENT_ERROR);
    throw (e);

  }

  if (topParentField->GetNbDims() != 1 )
  {
    string msg = CTools::Format("ERROR - CProductCryosat::IsHighResolutionField - Number of array dim %d not implemented for this method "
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

  string name = CTools::StringToLower(topParentField->GetName());
  
  if ( (name.compare("time_orb_data") == 0) || (name.compare("meas_data") == 0) || (name.compare("wavef_data") == 0) )
  {
    return true;
  }


  return false;
}

//----------------------------------------
void CProductCryosat::ProcessHighResolutionWithoutFieldCalculation()
{
  if (m_dataSet.size() <= 0)
  {
    return;
  }
 
  // Get the current recordset (that which has been just read)
  CRecordSet* recordSetToProcess = m_dataSet.GetCurrentRecordSet();

  if (recordSetToProcess == NULL)
  {
    string msg = "ERROR in CProductCryosat::ProcessHighResolutionWithoutFieldCalculation - No current recordset";
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

  // Erase the current recordset in the main dataset 
  m_dataSet.Erase(recordSetToProcess);
  
  recordSetToProcess = NULL;
  
  // Insert the record of the new dataset in the main dataset
  m_dataSet.InsertDataset(&dataSetHighResolution);
  
}

//----------------------------------------
void CProductCryosat::Dump(ostream& fOut /* = cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CProductCryosat Object at "<< this << endl;

  //------------------
  CProduct::Dump(fOut);
  //------------------

  fOut << "==> END Dump a CProductCryosat Object at "<< this << endl;

  fOut << endl;

}


} // end namespace
