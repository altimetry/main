
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
#include "ProductNetCdfCF.h"

using namespace brathl;


namespace brathl
{


CProductNetCdfCF::CProductNetCdfCF()
{
  Init();
}


//----------------------------------------

CProductNetCdfCF::CProductNetCdfCF(const std::string& fileName)
      : CProductNetCdf(fileName)
{
  Init();
}


//----------------------------------------
CProductNetCdfCF::CProductNetCdfCF(const CStringList& fileNameList)
      : CProductNetCdf(fileNameList)

{
  Init();
}

//----------------------------------------

CProductNetCdfCF::~CProductNetCdfCF()
{

}
//----------------------------------------
void CProductNetCdfCF::Init()
{
  InitDateRef();

  m_nbRecords = -1;
  m_atBeginning = true;

  InitCriteriaInfo();
}

//----------------------------------------
void CProductNetCdfCF::InitDimIndexes (uint32_t value)
{
  if (value == AT_BEGINNING)
  {
    m_atBeginning = true;
  }
  else
  {
    m_atBeginning = false;
  }

  CUIntMap::iterator it;

  for (it = m_dimIndexes.begin(); it != m_dimIndexes.end(); it++)
  {
    it->second = value;
  }
}
//----------------------------------------
void CProductNetCdfCF::RewindInit ()
{
  CProductNetCdf::RewindInit();
}
//----------------------------------------
void CProductNetCdfCF::RewindEnd ()
{

  CProductNetCdf::RewindEnd();

}

//----------------------------------------
void CProductNetCdfCF::RewindProcess ()
{
  m_externalFile->MustBeOpened();

  CStringArray		dimNamesArray;

  m_dimIds.RemoveAll();
  m_dimValues.RemoveAll();
  m_dimIndexes.RemoveAll();
  m_dimsCount.RemoveAll();

  m_atBeginning = true;

  if (m_fieldsToRead->size() <= 0)
  {
    m_nbRecords = 0;
    return;
  }

  m_nbRecords = 1;
  bool allFiedsScalar = true;

  CObMap::iterator it;
  for (it = m_fieldsToRead->begin() ; it != m_fieldsToRead->end() ; it++)
  {
    CFieldNetCdf *field = CExternalFiles::GetFieldNetCdf(it->second);

    field->InitDimIndexes(AT_BEGINNING);

    if (field->HasDim())
    {
      allFiedsScalar = false;
      m_dimIndexes.Insert(field->GetDimNames(), AT_BEGINNING, false, false);
      m_dimIds.Insert(field->GetDimIds(), false, false);
      m_dimValues.Insert(field->GetDimValues(), false, false);
    }

  }
/*
  CUIntMap::iterator itUintMap;
  for (itUintMap = m_dimValues.begin(); itUintMap != m_dimValues.end(); itUintMap++)
  {
    m_nbRecords *= itUintMap->second;
  }
  */
  CNetCDFFiles* file = m_externalFile->GetFile();

  bool readAxisOneByOne = true;

  if ( (m_axisDims.size() <= 0))
  {
    CStringArray fields;
    m_fieldsToRead->GetKeys(fields);
    GetNetCdfDimensions(fields, m_axisDims);
    readAxisOneByOne = false;
  }

  CStringArray::const_iterator itStringArray;
  for (itStringArray = m_axisDims.begin(); itStringArray != m_axisDims.end(); itStringArray++)
  {
    CNetCDFDimension* netCDFDim = file->GetNetCDFDim(*itStringArray);
    if (netCDFDim == NULL)
    {
      throw CException(CTools::Format("ERROR: CProductNetCdfCF::Rewind() - dimension '%s' not found in file '%s'\n",
                                    itStringArray->c_str(),
                                    file->GetName().c_str()),
			               BRATHL_LOGIC_ERROR);
    }

    uint32_t countValue = 1;

    m_nbRecords *= netCDFDim->GetLength();
    if ( ( ! readAxisOneByOne ) && ( ! m_forceReadDataOneByOne ))
    {
      countValue = netCDFDim->GetLength();
      m_nbRecords = 1;
    }

    if (m_dimsToReadOneByOne.Exists(*itStringArray))
    {
      countValue = 1;
    }

    m_dimsCount.Insert(*itStringArray, countValue);
  }

  for (itStringArray = m_complementDims.begin(); itStringArray != m_complementDims.end(); itStringArray++)
  {
    CNetCDFDimension* netCDFDim = file->GetNetCDFDim(*itStringArray);
    if (netCDFDim == NULL)
    {
      throw CException(CTools::Format("ERROR: CProductNetCdfCF::Rewind() - dimension '%s' not found in file '%s'\n",
                                    itStringArray->c_str(),
                                    file->GetName().c_str()),
			               BRATHL_LOGIC_ERROR);
    }

    uint32_t maxIndex = m_dimValues.Exists(*itStringArray);

    //if ((m_forceReadDataOneByOne) || (m_forceReadDataComplementDimsOneByOne))
    if (m_forceReadDataOneByOne)
    {
      //m_nbRecords *= netCDFDim->GetLength();
      maxIndex = 1;
    }

    if (m_dimsToReadOneByOne.Exists(*itStringArray))
    {
      //m_nbRecords *= netCDFDim->GetLength();
      maxIndex = 1;
    }

    m_dimsCount.Insert(*itStringArray, maxIndex);
  }


  uint32_t nbIndexes = m_dimIndexes.size();
  uint32_t nbIds = m_dimIds.size();
  uint32_t nbDimValues = m_dimValues.size();
  uint32_t nbDimsCount = m_dimsCount.size();

  if ((nbIndexes != nbIds) || (nbIndexes != nbDimValues) || (nbIndexes != nbDimsCount))
  {
      CException e(CTools::Format("ERROR - CProductNetCdfCF::Rewind() - : Arrays have not the same size: "
                                  "m_dimIndexes size=%d - m_dimIds size=%d - m_dimValues size=%d - m_dimsCount size=%d",
                                  nbIndexes,
                                  nbIds,
                                  nbDimValues,
                                  nbDimsCount),
                  BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      Dump(*CTrace::GetDumpContext());
      throw (e);

  }
  /*
  if (nbIndexes <= 0)
  {
      CException e(CTools::Format("ERROR - CProductNetCdfCF::Rewind() - : Array sizes are zero : "
                                  "m_dimIndexes size=%d - m_dimIds size=%d - m_dimValues size=%d",
                                  nbIndexes,
                                  nbIds,
                                  nbDimValues),
                  BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      Dump(*CTrace::GetDumpContext());
      throw (e);

  }
  */

  //////////////////SetFieldIndex();

  //----------------------------
  CProductNetCdf::RewindProcess();
  //----------------------------

}
//----------------------------------------
void CProductNetCdfCF::Rewind ()
{
  RewindInit();

  RewindProcess();

  RewindEnd();



}

//----------------------------------------
bool CProductNetCdfCF::IsAtBeginning()
{
  /*
  if (m_dimIndexes.size() <= 0)
  {
    throw CException("ERROR - CProductNetCdf::IsAtBeginning() - dimension index array has not been sized",
		       BRATHL_LOGIC_ERROR);
  }

  CUIntMap::iterator it = m_dimIndexes.begin();
  return (it->second == AT_BEGINNING);
  */
  return (m_atBeginning);

}
//----------------------------------------
void CProductNetCdfCF::SetFieldIndex(CFieldNetCdf *field)
{

  if (!field->HasDim())
  {
    field->SetAtBeginning(false);
    return;
  }

  const CStringArray& dimFieldNames = field->GetDimNames();
  CStringArray::const_iterator it;

  for (it = dimFieldNames.begin(); it != dimFieldNames.end(); it++)
  {
    // index can have AT_BEGINNING value
    uint32_t index = m_dimIndexes.Exists(*it);
    if (CTools::IsDefaultValue(index))
    {
      CException e(CTools::Format("ERROR - CProductNetCdfCF::SetFieldIndex(CFieldNetCdf *field) - "
                                  "dimension name not found : '%s'"
                                  " - Field name '%s'",
                                  (*it).c_str(),
                                  field->GetName().c_str()),
                  BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      Dump(*CTrace::GetDumpContext());
      throw (e);
    }

    uint32_t count = m_dimsCount.Exists(*it);

    if (CTools::IsDefaultValue(count))
    {
      CException e(CTools::Format("ERROR - CProductNetCdfCF::SetFieldIndex(CFieldNetCdf *field) - "
                                  "dimension counter not found : '%s'"
                                  " - Field name '%s'",
                                  (*it).c_str(),
                                  field->GetName().c_str()),
                  BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      Dump(*CTrace::GetDumpContext());
      throw (e);
    }
    field->SetIndex(*it, index, count);

  }


}
/*
//----------------------------------------
bool CProductNetCdfCF::NextIndex()
{
  bool result = true;

  if (IsAtBeginning())
  {
    InitDimIndexes(0);
    return result;
  }

  if (m_dimIndexes.size() <= 0)
  {
    return false;
  }

  CUIntMap::iterator it;
  it = m_dimIndexes.begin();

  //int32_t i = m_dimIndexes.size() - 1;

  while (true)
  {
    if (it == m_dimIndexes.end())
    {
      break;
    }

    std::string dimName = it->first;

    uint32_t count = m_dimsCount.Exists(dimName);

    if (CTools::IsDefaultValue(count))
    {
      CException e(CTools::Format("ERROR - CProductNetCdfCF::NextIndex - "
                                  "dimension counter not found : '%s'",
                                  dimName.c_str()),
                  BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      Dump(*CTrace::GetDumpContext());
      throw (e);
    }

    //-----------------
    //it->second += (count - 1);
    it->second += count;
    //-----------------

    uint32_t currentIndex = it->second;
    uint32_t maxIndex = m_dimValues.Exists(dimName);


    if (currentIndex >= maxIndex)
    {
      if (CheckEOF())
      {
        InitDimsIndexToMax();
        result = false;
        break;
      }
      else
      {
        it->second = 0;
        it++;
      }
    }
    else
    {
      break;
    }

  }

  return result;
}
*/
//----------------------------------------
bool CProductNetCdfCF::CheckEOF()
{
  CUIntMap::iterator it;

  bool bEOF = true;

  for (it = m_dimIndexes.begin(); it != m_dimIndexes.end(); it++)
  {
    uint32_t maxIndex = m_dimValues.Exists(it->first) - 1;

    bEOF &= (it->second >= maxIndex);

  }

  return bEOF;
}
//----------------------------------------
void CProductNetCdfCF::InitDimsIndexToMax()
{
  CUIntMap::iterator it;

  for (it = m_dimIndexes.begin(); it != m_dimIndexes.end(); it++)
  {
    it->second = m_dimValues.Exists(it->first) - 1;
  }
}

//----------------------------------------
// True while there is something to read
bool CProductNetCdfCF::NextRecord ()
{
  return NextFieldIndex();

}
//----------------------------------------
// True while there is something to read
bool CProductNetCdfCF::PrevRecord ()
{
  return PrevFieldIndex();

}

//----------------------------------------
bool CProductNetCdfCF::NextFieldIndex()
{

  bool hasNext = false;

  CObMap::iterator it;
  for (it = m_fieldsToRead->begin() ; it != m_fieldsToRead->end() ; it++)
  {
  // Push dimension index for each field to read
    CFieldNetCdf *field = CExternalFiles::GetFieldNetCdf(it->second);

    hasNext |= field->NextIndex();

    AdjustIndexesFromField(field);
  }

  if (hasNext)
  {
    AdjustIndexesToMin();

    SetFieldIndex();
  }

  return hasNext;
}
//----------------------------------------
bool CProductNetCdfCF::PrevFieldIndex()
{

  bool hasPrev = false;

  CObMap::iterator it;
  for (it = m_fieldsToRead->begin() ; it != m_fieldsToRead->end() ; it++)
  {
  // Push dimension index for each field to read
    CFieldNetCdf *field = CExternalFiles::GetFieldNetCdf(it->second);

    hasPrev |= field->PrevIndex();

    AdjustIndexesFromField(field, false);
  }

  if (hasPrev)
  {
    AdjustIndexesToMin(false);

    SetFieldIndex();
  }

  return hasPrev;
}
//----------------------------------------

void CProductNetCdfCF::AdjustIndexesToMin(bool next /* = true */)
{
  CObMap::iterator it;
  for (it = m_fieldsToRead->begin() ; it != m_fieldsToRead->end() ; it++)
  {
    CFieldNetCdf *field = CExternalFiles::GetFieldNetCdf(it->second);
    AdjustIndexesToMin(field, next);
  }

}

//----------------------------------------
void CProductNetCdfCF::SetFieldIndex()
{
  CObMap::iterator it;
  for (it = m_fieldsToRead->begin() ; it != m_fieldsToRead->end() ; it++)
  {
  // Push dimension index for each field to read
    CFieldNetCdf *field = CExternalFiles::GetFieldNetCdf(it->second);
    SetFieldIndex(field);
  }

}
//----------------------------------------

void CProductNetCdfCF::AdjustIndexesFromField(CFieldNetCdf *field, bool next /* = true */)
{

  if (!field->HasDim())
  {
    return;
  }

  for (int32_t i = 0 ; i < field->GetNbDims() ; i++)
  {
    std::string dimName = field->GetDimNames().at(i);
    uint32_t currentFieldIndex = field->GetDimsIndexArray()[i];

    CUIntMap::iterator it = m_dimIndexes.find(dimName);
    if (it == m_dimIndexes.end())
    {
      CException e(CTools::Format("ERROR - CProductNetCdfCF::AdjustIndexesFromField(CFieldNetCdf *field) - "
                                  "dimension name not found : '%s'"
                                  " - Field name '%s'",
                                  dimName.c_str(),
                                  field->GetName().c_str()),
                  BRATHL_LOGIC_ERROR);
    }


    if (next)
    {
      if (it->second < currentFieldIndex)
      {
        it->second = currentFieldIndex;

      }
    }
    else
    {
      if (it->second > currentFieldIndex)
      {
        it->second = currentFieldIndex;

      }
    }
  }

}
//----------------------------------------

void CProductNetCdfCF::AdjustIndexesToMin(CFieldNetCdf *field, bool next /* = true */)
{

  if (!field->HasDim())
  {
    return;
  }

  for (int32_t i = 0 ; i < field->GetNbDims() ; i++)
  {
    std::string dimName = field->GetDimNames().at(i);
    uint32_t currentFieldIndex = field->GetDimsIndexArray()[i];

    CUIntMap::iterator it = m_dimIndexes.find(dimName);
    if (it == m_dimIndexes.end())
    {
      CException e(CTools::Format("ERROR - CProductNetCdfCF::AdjustIndexesToMin(CFieldNetCdf *field) - "
                                  "dimension name not found : '%s'"
                                  " - Field name '%s'",
                                  dimName.c_str(),
                                  field->GetName().c_str()),
                  BRATHL_LOGIC_ERROR);
    }


    if (next)
    {
      if (it->second > currentFieldIndex)
      {
        it->second = currentFieldIndex;

      }
    }
    else
    {
      if (it->second < currentFieldIndex)
      {
        it->second = currentFieldIndex;

      }
    }
  }

}
//----------------------------------------
int32_t CProductNetCdfCF::GetNumberOfRecords()
{

  return m_nbRecords;

}

//----------------------------------------
int32_t CProductNetCdfCF::GetNumberOfRecords(const std::string& dataSetName /*NOT USED*/)
{
  return GetNumberOfRecords();
}

//----------------------------------------
bool CProductNetCdfCF::IsProductNetCdfCF(CBratObject* ob)
{
  return (CProductNetCdfCF::GetProductNetCdfCF(ob, false) != NULL);
}
//----------------------------------------
CProductNetCdfCF* CProductNetCdfCF::GetProductNetCdfCF(CBratObject* ob, bool withExcept /*= true*/)
{
  CProductNetCdfCF* productNetCdfCF = dynamic_cast<CProductNetCdfCF*>(ob);
  if (withExcept)
  {
    if (productNetCdfCF == NULL)
    {
      CException e("CProductNetCdfCF::GetProductNetCdfCF - dynamic_cast<productNetCdfCF*>(ob) returns NULL"
                   "object seems not to be an instance of CProductNetCdfCF",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }

  return productNetCdfCF;

}

//----------------------------------------
CProduct* CProductNetCdfCF::Clone()
{

  CProduct* product = CProductNetCdf::Clone();

  if (product == NULL)
  {
    return product;
  }

  return product;
}
//----------------------------------------
void CProductNetCdfCF::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CProductNetCdfCF Object at "<< this << std::endl;

  //------------------
  CProductNetCdf::Dump(fOut);
  //------------------

  fOut << "m_atBeginning = " << m_atBeginning << std::endl;

  fOut << "m_dimIds = " << std::endl;
  m_dimIds.Dump(fOut);

  fOut << "m_dimIndexes = " << std::endl;
  m_dimIndexes.Dump(fOut);

  fOut << "m_dimsCount = " << std::endl;
  m_dimsCount.Dump(fOut);

  fOut << "m_dimValues = " << std::endl;
  m_dimValues.Dump(fOut);

  fOut << "==> END Dump a CProductNetCdfCF Object at "<< this << std::endl;

  fOut << std::endl;

}


} // end namespace
