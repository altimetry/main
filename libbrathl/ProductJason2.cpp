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

#include "ProductJason2.h"
using namespace brathl;


//-------------------------------------------------------------
//------------------- CProductJason2 class --------------------
//-------------------------------------------------------------

void CProductJason2::Init()
{
	mLabel = "Jason-2 product";

	m_longitudeFieldName = "lon";
	m_latitudeFieldName = "lat";

	m_forceLatMinCriteriaValue = -67.0;
	m_forceLatMaxCriteriaValue = 67.0;

	InitDateRef();

	InitCriteriaInfo();

}


CProductJason2::CProductJason2()
{
  Init();


}

//----------------------------------------

CProductJason2::CProductJason2(const std::string& fileName)
      : CProductNetCdfCF(fileName)
{
  Init();
}


//----------------------------------------
CProductJason2::CProductJason2( const CStringList& fileNameList, bool check_only_first_files )
	: CProductNetCdfCF( fileNameList, check_only_first_files )
{
	Init();
}

//----------------------------------------
CProductJason2::~CProductJason2()
{
}


//----------------------------------------
void CProductJason2::InitDateRef()
{
  m_refDate = REF20000101;
}

//----------------------------------------
void CProductJason2::InitCriteriaInfo()
{

  CProduct::InitCriteriaInfo();

  //-------------------------
  // Lat/Lon criteria info
  //-------------------------
  CCriteriaLatLonInfo* criteriaLatLonInfo = new CCriteriaLatLonInfo();

  criteriaLatLonInfo->SetDataRecord(CProductNetCdf::m_virtualRecordName);
  
  /*
  criteriaLatLonInfo->SetStartLatField("first_meas_latitude");
  criteriaLatLonInfo->GetStartLatField().SetIsNetCdfGlobalAttribute(true);

  criteriaLatLonInfo->SetEndLatField("last_meas_latitude");
  criteriaLatLonInfo->GetEndLatField().SetIsNetCdfGlobalAttribute(true);
  
  criteriaLatLonInfo->SetStartLonField("first_meas_longitude");
  criteriaLatLonInfo->GetStartLonField().SetIsNetCdfGlobalAttribute(true);
  
  criteriaLatLonInfo->SetEndLonField("last_meas_longitude");
  criteriaLatLonInfo->GetEndLonField().SetIsNetCdfGlobalAttribute(true);
*/
  
  criteriaLatLonInfo->SetStartLatField(m_latitudeFieldName);
  criteriaLatLonInfo->GetStartLatField()->SetIsNetCdfVarValue(true);

  criteriaLatLonInfo->SetEndLatField(m_latitudeFieldName);
  criteriaLatLonInfo->GetEndLatField()->SetIsNetCdfVarValue(true);
  
  criteriaLatLonInfo->SetStartLonField(m_longitudeFieldName);
  criteriaLatLonInfo->GetStartLonField()->SetIsNetCdfVarValue(true);
  
  criteriaLatLonInfo->SetEndLonField(m_longitudeFieldName);
  criteriaLatLonInfo->GetEndLonField()->SetIsNetCdfVarValue(true);

  m_criteriaInfoMap.Insert(criteriaLatLonInfo->GetKey(), criteriaLatLonInfo); 

  //-------------------------
  // Datetime criteria info
  //-------------------------
  CCriteriaDatetimeInfo* criteriaDatetimeInfo = new CCriteriaDatetimeInfo();

  criteriaDatetimeInfo->SetDataRecord(CProductNetCdf::m_virtualRecordName);
  
  criteriaDatetimeInfo->SetStartDateField("first_meas_time");
  criteriaDatetimeInfo->GetStartDateField()->SetIsNetCdfGlobalAttribute(true);
  //criteriaDatetimeInfo->GetStartDateField().SetDataType(NC_CHAR);
  
  criteriaDatetimeInfo->SetEndDateField("last_meas_time");
  criteriaDatetimeInfo->GetEndDateField()->SetIsNetCdfGlobalAttribute(true);
  //criteriaDatetimeInfo->GetEndDateField().SetDataType(NC_CHAR);

  criteriaDatetimeInfo->SetRefDate(m_refDate);

  m_criteriaInfoMap.Insert(criteriaDatetimeInfo->GetKey(), criteriaDatetimeInfo); 

  //-------------------------
  // Pass criteria info
  //-------------------------

  CCriteriaPassIntInfo* criteriaPassInfo = new CCriteriaPassIntInfo();

  criteriaPassInfo->SetDataRecord(CProductNetCdf::m_virtualRecordName);
  
  criteriaPassInfo->SetStartPassField("pass_number");
  criteriaPassInfo->GetStartPassField()->SetIsNetCdfGlobalAttribute(true);
  criteriaPassInfo->SetEndPassField("pass_number");
  criteriaPassInfo->GetEndPassField()->SetIsNetCdfGlobalAttribute(true);

  m_criteriaInfoMap.Insert(criteriaPassInfo->GetKey(), criteriaPassInfo); 

  //-------------------------
  // Cycle criteria info
  //-------------------------
  CCriteriaCycleInfo* criteriaCycleInfo = new CCriteriaCycleInfo();

  criteriaCycleInfo->SetDataRecord(CProductNetCdf::m_virtualRecordName);
  
  criteriaCycleInfo->SetStartCycleField("cycle_number");
  criteriaCycleInfo->GetStartCycleField()->SetIsNetCdfGlobalAttribute(true);
  
  criteriaCycleInfo->SetEndCycleField("cycle_number");
  criteriaCycleInfo->GetEndCycleField()->SetIsNetCdfGlobalAttribute(true);

  m_criteriaInfoMap.Insert(criteriaCycleInfo->GetKey(), criteriaCycleInfo); 

}

//----------------------------------------
void CProductJason2::Dump(std::ostream& fOut /* = std::cerr */) 
{

  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CProductJason2 Object at "<< this << std::endl;

  //------------------
  CProductNetCdfCF::Dump(fOut);
  //------------------

  fOut << "==> END Dump a CProductJason2 Object at "<< this << std::endl;

  fOut << std::endl;

}


