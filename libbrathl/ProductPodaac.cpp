
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
#include "ProductPodaac.h" 

using namespace brathl;

namespace brathl
{

const string PODAAC_HEADER = "header";

const string CProductPodaac::m_J1SSHA_PASS_FILE = "J1SSHA_PASS_FILE";
const string CProductPodaac::m_J1SSHA_ATG_FILE = "J1SSHA_ATG_FILE";
const string CProductPodaac::m_TPSSHA_PASS_FILE = "TPSSHA_PASS_FILE";
const string CProductPodaac::m_TPSSHA_ATG_FILE = "TPSSHA_ATG_FILE";

CProductPodaac::CProductPodaac()
{
  Init();
}


//----------------------------------------

CProductPodaac::CProductPodaac(const string& fileName)
      : CProduct(fileName)
{  
  Init();
}


//----------------------------------------
CProductPodaac::CProductPodaac(const CStringList& fileNameList)
      : CProduct(fileNameList)

{
  Init();
}

//----------------------------------------

CProductPodaac::~CProductPodaac()
{
}

//----------------------------------------
void CProductPodaac::InitDateRef()
{
  m_refDate = REF19580101;
}
//----------------------------------------
void CProductPodaac::Init()
{
  m_label = "POODAAC products";

  InitDateRef();

  m_latitudeFieldName = "latitude";
  m_longitudeFieldName = "longitude";

  InitCriteriaInfo();
}
//----------------------------------------
string CProductPodaac::GetLabel()
{
  if ((m_fileList.m_productType.compare(CProductPodaac::m_J1SSHA_PASS_FILE) == 0) ||
      (m_fileList.m_productType.compare(CProductPodaac::m_TPSSHA_PASS_FILE) == 0))
  {
    m_label = "PODAAC Jason-1 or Topex/Poseidon SSHA pass product";
  }
  else if ((m_fileList.m_productType.compare(CProductPodaac::m_J1SSHA_ATG_FILE) == 0) ||
           (m_fileList.m_productType.compare(CProductPodaac::m_TPSSHA_ATG_FILE) == 0))
  {
    m_label = "PODAAC Jason-1 or Topex/Poseidon Along Track Gridded SSHA product";
  }

  return m_label; 
}

//----------------------------------------
void CProductPodaac::InitCriteriaInfo()
{
  CProduct::InitCriteriaInfo();

  //string productype = CTools::StringToUpper(m_fileList.m_productType);
  string productype = m_fileList.m_productType;
  if ((productype.compare(CProductPodaac::m_J1SSHA_PASS_FILE) == 0) || ((productype.compare(CProductPodaac::m_TPSSHA_PASS_FILE) == 0)))
  {
    //-------------------------
    // Datetime criteria info
    //-------------------------
    CCriteriaDatetimeInfo* criteriaDatetimeInfo = new CCriteriaDatetimeInfo();

    criteriaDatetimeInfo->SetDataRecord(PODAAC_HEADER);
    criteriaDatetimeInfo->SetStartDateField("Frst");
    criteriaDatetimeInfo->SetEndDateField("Last");

    criteriaDatetimeInfo->SetRefDate(m_refDate);

    m_criteriaInfoMap.Insert(criteriaDatetimeInfo->GetKey(), criteriaDatetimeInfo); 

    //-------------------------
    // Pass criteria info
    //-------------------------
    CCriteriaPassIntInfo* criteriaPassInfo = new CCriteriaPassIntInfo();

    criteriaPassInfo->SetDataRecord(PODAAC_HEADER);
    criteriaPassInfo->SetStartPassField("Pass_Number");

    m_criteriaInfoMap.Insert(criteriaPassInfo->GetKey(), criteriaPassInfo); 
  

    //-------------------------
    // Cycle criteria info
    //-------------------------
    CCriteriaCycleInfo* criteriaCycleInfo = new CCriteriaCycleInfo();

    criteriaCycleInfo->SetDataRecord(PODAAC_HEADER);
    criteriaCycleInfo->SetStartCycleField("Cycle_Number");

    m_criteriaInfoMap.Insert(criteriaCycleInfo->GetKey(), criteriaCycleInfo); 

  } 
  else if ((productype.compare(CProductPodaac::m_J1SSHA_ATG_FILE) == 0) || ((productype.compare(CProductPodaac::m_TPSSHA_ATG_FILE) == 0)))
  {
    //-------------------------
    // Cycle criteria info
    //-------------------------
    CCriteriaCycleInfo* criteriaCycleInfo = new CCriteriaCycleInfo();

    criteriaCycleInfo->SetDataRecord(PODAAC_HEADER);
    criteriaCycleInfo->SetStartCycleField("Cycle_Number");

    m_criteriaInfoMap.Insert(criteriaCycleInfo->GetKey(), criteriaCycleInfo); 

  }
}


//----------------------------------------
void CProductPodaac::Dump(ostream& fOut /* = cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CProductPodaac Object at "<< this << endl;

  //------------------
  CProduct::Dump(fOut);
  //------------------

  fOut << "==> END Dump a CProductPodaac Object at "<< this << endl;

  fOut << endl;

}


} // end namespace
