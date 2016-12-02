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

#include "common/tools/brathl_error.h"
#include "brathl.h"

//#define BRAT_INTERNAL

#include "coda.h"

#include <string>

#include "Tools.h"
#include "common/tools/TraceLog.h"
#include "common/tools/Exception.h"
#include "Product.h"
#include "ProductNetCdf.h"
#include "Unit.h"
#include "Field.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;


namespace brathl
{

const std::string CField::m_BRAT_INDEX_DATA_NAME = "brat_index_data";
const std::string CField::m_BRAT_INDEX_DATA_DESC = "data index";

//-------------------------------------------------------------
//------------------- CField class --------------------
//-------------------------------------------------------------

//----------------------------------------

CField::CField()
{
  Init();
}

//----------------------------------------

CField::CField(const std::string& name, const std::string& description, const std::string& unit)
{
  Init();

  m_name = name;
  m_description = description;
  SetUnit(unit);
}
//----------------------------------------

CField::CField( const CField &f )
{
	Init();
	Set( f );
}

//----------------------------------------

CField::~CField()
{
  DeleteFieldIndexes();
  //std::cout << "CField::~CField()" << std::endl;
}

//----------------------------------------
CField& CField::operator =( const CField &f )
{
  Set(f);
  return *this;
}

//----------------------------------------
void CField::Init()
{
  m_fieldHasDefaultValue = true;

  m_dimsTransposed = false;

  m_virtualField = false;

  m_expandArray = false;

  m_fieldIndexes = new CObArray(false);

  m_typeClass = coda_record_class;
  m_specialType = coda_special_no_data;
  m_nativeType = coda_native_type_not_available;

  m_metaData = false;

  m_hidden = true;

  m_index = -1;

  m_isUnion = 0;
  m_toBeRemoved = false;

  SetDim(1);

  m_isFixedSize = true;

  m_currentPos = 0;

  m_goToAvailableUnionField = true;

  m_highResolution = false;

  setDefaultValue(m_validMin);
  setDefaultValue(m_validMax);
  setDefaultValue(m_numHighResolutionMeasure);

  m_convertDate = false;

  m_unitIsDate = false;

  m_offset = 0.0;

}

//----------------------------------------
void CField::SetDateRef(brathl_refDate refDate)
{
  CDate d(0.0, refDate);
  m_dateRef = d;
};

//----------------------------------------
void CField::SetDateRef(const CDate& value)
{
  m_dateRef = value;
}

//----------------------------------------
void CField::DeleteFieldIndexes()
{
  if (m_fieldIndexes != NULL)
  {
    delete m_fieldIndexes;
    m_fieldIndexes = NULL;
  }

}
//----------------------------------------
void CField::Set( const CField &f )
{
	m_currentPos = f.m_currentPos;
	m_cursor = f.m_cursor;
	m_description = f.m_description;

	SetDim( *f.m_dim );

	m_fullName = f.m_fullName;
	m_hidden = f.m_hidden;
	m_highResolution = f.m_highResolution;
	m_index = f.m_index;
	m_isFixedSize = f.m_isFixedSize;
	m_key = f.m_key;
	m_metaData = f.m_metaData;
	m_name = f.m_name;
	m_nativeType = f.m_nativeType;
	m_recordName = f.m_recordName;
	m_specialType = f.m_specialType;
	m_typeClass = f.m_typeClass;
	m_unit = f.m_unit;
	m_validMin = f.m_validMin;
	m_validMax = f.m_validMax;

	m_convertDate = f.m_convertDate;

	AddFieldIndexes( f.m_fieldIndexes );

	m_virtualField = f.m_virtualField;

	m_expandArray = f.m_expandArray;

	m_dateRef = f.m_dateRef;

	m_numHighResolutionMeasure = f.m_numHighResolutionMeasure;

	m_unitIsDate = f.m_unitIsDate;

	m_offset = f.m_offset;
}

//----------------------------------------
bool CField::HasEqualDims(CField* field)
{

  CUIntArray thisDims;
  this->GetDimAsVector(thisDims);

  CUIntArray fieldDims;
  field->GetDimAsVector(fieldDims);

  return (thisDims == fieldDims);

}
//----------------------------------------
void CField::AddFieldIndexes(CObArray* vect, bool removeAll /* = true */)
{

  if (removeAll)
  {
    m_fieldIndexes->RemoveAll();
  }

  CObArray::const_iterator it;

  for (it = vect->begin() ; it != vect->end() ; it++)
  {
    CFieldIndex* f = dynamic_cast<CFieldIndex*>(*it);
    if (f != NULL)
    {
      m_fieldIndexes->Insert(f);
    }
  }
}
//----------------------------------------
void CField::AddFieldIndexes(CFieldIndex* field)
{
  if (m_fieldIndexes == NULL)
  {
    return;
  }

  m_fieldIndexes->Insert(field);

}

//----------------------------------------
void CField::AdjustValidMinMax(double* data, int32_t size, double& min, double& max)
{
  for (int32_t i = 0 ; i < size ; i++)
  {
    AdjustValidMinMax(data[i], min, max);
  }

}
//----------------------------------------
void CField::AdjustValidMinMax(double* data, int32_t size)
{
  for (int32_t i = 0 ; i < size ; i++)
  {
    AdjustValidMinMax(data[i]);
  }

}
//----------------------------------------
void CField::AdjustValidMinMax(double value)
{
  AdjustValidMinMax(value, m_validMin, m_validMax);
}
//----------------------------------------
void CField::AdjustValidMinMax(double value, double& min, double& max)
{
  if (isDefaultValue(value))
  {
    return;
  }
  if (isDefaultValue(min))
  {
    min = value;
  }

  if (isDefaultValue(max))
  {
    max = value;
  }

  if (min > value)
  {
    min = value;
  }

  if (max < value)
  {
    max = value;
  }
}

//----------------------------------------
bool CField::TransposeValues(double* data, int32_t size)
{
  if (!m_dimsTransposed)
  {
    return false;
  }

  if (m_nbDims != 2)
  {
    return false;
  }

  if ( (m_dim[0] * m_dim[1]) != size)
  {
    return false;
  }

  double* tranposedValues = new double[size];


  // Warning dim are already transposed
  uint32_t i = 0;
  for (int32_t r = 0 ; r < m_dim[1] ; r++)
  {
    for (int32_t c = 0 ; c < m_dim[0] ; c++)
    {
      tranposedValues[(m_dim[1] * c) + r] = data[i];
      i++;

    }

  }

  memcpy(data, tranposedValues, sizeof(*data) * size);

  CDoubleArray d;
  d.Insert(data, size);
  d.Dump(*CTrace::GetInstance()->GetDumpContext());
  return true;
}
//----------------------------------------
void CField::ConvertDefaultValueInt8(double* data, int32_t size)
{
  for (int32_t i = 0 ; i < size ; i++)
  {
    if (isDefaultValue(static_cast<int8_t>(data[i])))
    {
      setDefaultValue(data[i]);
    }
  }
}
//----------------------------------------
void CField::ConvertDefaultValueUInt8(double* data, int32_t size)
{
  for (int32_t i = 0 ; i < size ; i++)
  {
    if (isDefaultValue(static_cast<uint8_t>(data[i])))
    {
      setDefaultValue(data[i]);
    }
  }
}
//----------------------------------------
void CField::ConvertDefaultValueInt16(double* data, int32_t size)
{
  for (int32_t i = 0 ; i < size ; i++)
  {
    if (isDefaultValue(static_cast<int16_t>(data[i])))
    {
      setDefaultValue(data[i]);
    }
  }
}
//----------------------------------------
void CField::ConvertDefaultValueUInt16(double* data, int32_t size)
{
  for (int32_t i = 0 ; i < size ; i++)
  {
    if (isDefaultValue(static_cast<uint16_t>(data[i])))
    {
      setDefaultValue(data[i]);
    }
  }
}
//----------------------------------------
void CField::ConvertDefaultValueInt32(double* data, int32_t size)
{
  for (int32_t i = 0 ; i < size ; i++)
  {
    if (isDefaultValue(static_cast<int32_t>(data[i])))
    {
      setDefaultValue(data[i]);
    }
  }
}
//----------------------------------------
void CField::ConvertDefaultValueUInt32(double* data, int32_t size)
{
  for (int32_t i = 0 ; i < size ; i++)
  {
    if (isDefaultValue(static_cast<uint32_t>(data[i])))
    {
      setDefaultValue(data[i]);
    }
  }
}
//----------------------------------------
void CField::ConvertDefaultValueInt64(double* data, int32_t size)
{
  for (int32_t i = 0 ; i < size ; i++)
  {
    if (isDefaultValue(static_cast<int64_t>(data[i])))
    {
      setDefaultValue(data[i]);
    }
  }
}
//----------------------------------------
void CField::ConvertDefaultValueUInt64(double* data, int32_t size)
{
  for (int32_t i = 0 ; i < size ; i++)
  {
    if (isDefaultValue(static_cast<uint64_t>(data[i])))
    {
      setDefaultValue(data[i]);
    }
  }
}
//----------------------------------------
void CField::ConvertDefaultValueFloat(double* data, int32_t size)
{
  for (int32_t i = 0 ; i < size ; i++)
  {
    if (isDefaultValue(static_cast<float>(data[i])))
    {
      setDefaultValue(data[i]);
    }
  }
}
//----------------------------------------
void CField::SetDefaultValue(double* data, int32_t size)
{
  for (int32_t i = 0 ; i < size ; i++)
  {
      setDefaultValue(data[i]);
  }
}
//----------------------------------------
void CField::Convert(double* data, int32_t size)
{
  try
  {

//    native_type_not_available = -1,        /**< native type value for compound entities that can not be read directly */
//    native_type_int8,                      /**< 8 bit signed integer (#brat_integer_class) */
//    native_type_uint8,                     /**< 8 bit unsigned integer (#brat_integer_class) */
//    native_type_int16,                     /**< 16 bit signed integer (#brat_integer_class) */
//    native_type_uint16,                    /**< 16 bit unsigned integer (#brat_integer_class) */
//    native_type_int32,                     /**< 32 bit signed integer (#brat_integer_class) */
//    native_type_uint32,                    /**< 32 bit unsigned integer (#brat_integer_class) */
//    native_type_int64,                     /**< 64 bit signed integer (#brat_integer_class) */
//    native_type_uint64,                    /**< 64 bit unsigned integer (#brat_integer_class) */
//    native_type_float,                     /**< 32 bit IEEE floating point type (#brat_real_class) */
//    native_type_double,                    /**< 64 bit IEEE floating point type (#brat_real_class) */
//
    if (m_fieldHasDefaultValue)
    {
      switch (m_nativeType)
      {
        case coda_native_type_int8 :
        {
          ConvertDefaultValueInt8(data, size);
          break;
        }
        case coda_native_type_uint8 :
        {
          ConvertDefaultValueUInt8(data, size);
          break;
        }
        case coda_native_type_int16 :
        {
          ConvertDefaultValueInt16(data, size);
          break;
        }
        case coda_native_type_uint16 :
        {
          ConvertDefaultValueUInt16(data, size);
          break;
        }
        case coda_native_type_int32 :
        {
          ConvertDefaultValueInt32(data, size);
          break;
        }
        case coda_native_type_uint32 :
        {
          ConvertDefaultValueUInt32(data, size);
          break;
        }
        case coda_native_type_int64 :
        {
          ConvertDefaultValueInt64(data, size);
          break;
        }
        case coda_native_type_uint64 :
        {
          ConvertDefaultValueUInt64(data, size);
          break;
        }
        case coda_native_type_float :
        {
          ConvertDefaultValueFloat(data, size);
          break;
        }
        case coda_native_type_double :
        {
          break;
        }
        default :
        {
          break;
        }
      }
    }

    //std::string temp = CTools::StringRemoveAllSpaces(m_unit);
    //temp = CTools::StringReplace(temp, '^', 'e');
    //CUnit unit = temp;
    CUnit unit( m_unit );
    unit.SetConversionToBaseUnit();
    unit.ConvertVector(data, size);
  }
  catch(...)
  {
    //No conversion possible --> do nothing
  }
}
//----------------------------------------
uint32_t CField::GetOffsetDim()
{
  return 0;
}
//----------------------------------------
int32_t CField::GetVirtualNbDims()
{
  if (m_nbDims <= 0)
  {
    return 0;
  }

  if (m_nbDims == 1)
  {
    if (m_dim[0] <= 1)
    {
      return 0;
    }
  }

  return m_nbDims;
}
//----------------------------------------
void CField::GetDimAsVector(CUIntArray& dim)
{
  for (int32_t i = 0 ; i < m_nbDims ; i++)
  {
    dim.Insert(m_dim[i]);
  }
}


//----------------------------------------
std::string CField::GetDimAsString()
{
  std::string dims;
  for (int32_t i = 0 ; i < m_nbDims ; i++)
  {
    dims += CTools::Format(30,  "[%d]=%ld ", i, m_dim[i]);
  }
  return dims;
}

//----------------------------------------
long CField::GetDimAt(int32_t index)
{
  if (index >= m_nbDims)
  {
    std::string msg = CTools::Format("ERROR - CField::GetDim(int32_t index) - invalid index %d - Field name %s - number of dimemnsions is %d",
                                index,
                                m_name.c_str(),
                                m_nbDims);
    CException e(msg, BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);

  }

  return m_dim[index];
}

//----------------------------------------


void CField::SetDim(int32_t nbDims, const long dim[])
{
  m_nbDims = nbDims;
  memset(&m_dim, 0, sizeof(m_dim));

  m_maxPos = 1;

  for (int32_t i = 0 ; i < m_nbDims ; i++)
  {
    m_dim[i] = dim[i];
    if (m_dim[i] <= 0)
    {
      m_isFixedSize = false;
    }
    m_maxPos *= m_dim[i];
  }
  m_maxPos--;

}

//----------------------------------------
void CField::SetDim(int32_t nbDims, const CUIntArray& dim)
{
  m_nbDims = nbDims;
  memset(&m_dim, 0, sizeof(m_dim));

  m_maxPos = 1;

  for (int32_t i = 0 ; i < m_nbDims ; i++)
  {
    m_dim[i] = dim[i];
    if (m_dim[i] <= 0)
    {
      m_isFixedSize = false;
    }
    m_maxPos *= m_dim[i];
  }
  m_maxPos--;

}

//----------------------------------------
void CField::SetDim(const CUIntArray& dim)
{
  SetDim(dim.size(), dim);

}
//----------------------------------------
void CField::SetDim(const CUIntArray* dim)
{
  if (dim != NULL)
  {
    SetDim(*dim);
  }

}


//----------------------------------------
void CField::SetDim(int32_t nbElts)
{
  //if (nbElts <= 0)
  //{
  //  std::string msg = CTools::Format("ERROR CFieldArray::SetDim(int32_t nbElts) - nbElts %d --> incorrect value - FieldName:%s",
  //                              nbElts, m_name.c_str());
  //  CException e(msg, BRATHL_INCONSISTENCY_ERROR);
  //  throw(e);
  //}
  m_nbDims = 1;
  memset(&m_dim, 0, sizeof(m_dim));
  m_dim[0] = nbElts;
  m_maxPos = m_dim[0] - 1;

}
//----------------------------------------
bool CField::TransposeDim()
{

  if (m_nbDims != 2)
  {
    return false;
  }

  long dim[MAX_NUM_DIMS];
  memset(&dim, 0, sizeof(dim));

  uint32_t maxPos = m_nbDims - 1;

  for (uint32_t i = 0 ; i <= maxPos ; i++)
  {
    dim[i] = m_dim[maxPos - i];
  }

  memcpy(&m_dim, dim, sizeof(dim));

  m_dimsTransposed = !m_dimsTransposed;

  return true;

}

//----------------------------------------
std::string CField::GetRecordName()
{
  if (m_recordName.empty() == false)
  {
    return m_recordName;
  }

  CStringArray strArray;

  strArray.ExtractStrings(m_key, CTreeField::m_keyDelimiter);

  if (strArray.size() <= 1)
  {
    return "";
  }
  m_recordName = strArray[1];  // RecordName
  return m_recordName;
}
//----------------------------------------
//virtual
std::string CField::GetFullName() const
{

  /*std::string fullName = m_key;
  std::string delim = CProduct::m_treeRootName + CTreeField::m_keyDelimiter;
  int32_t pos = m_key.find(delim, 0);

  if (pos >= 0)
  {
    uint32_t nLg = (m_key.length() - pos - delim.length());
    fullName = m_key.substr(pos + delim.length(), nLg);
  }
  return fullName;
  */

  if (m_fullName.empty() == false)
  {
    return m_fullName;
  }

  CStringArray strArray;

  strArray.ExtractStrings(m_key, CTreeField::m_keyDelimiter);

  for (uint32_t i = 2 ; i < strArray.size() ; i++)
  {
    m_fullName += strArray[i] + CTreeField::m_keyDelimiter;
  }

  if (m_fullName.empty() == false)
  {
    m_fullName.erase(m_fullName.length() - 1, CTreeField::m_keyDelimiter.size());
  }

  return m_fullName;
}

//----------------------------------------
//virtual
std::string CField::GetFullNameWithRecord()
{

  std::string fieldNameWithRecord;
  std::string fieldFullName = this->GetFullName();

  if (fieldFullName.empty())
  {
    return fieldNameWithRecord;
  }

  std::string recordName = this->GetRecordName();

  if (recordName.empty())
  {
    return fieldNameWithRecord;
  }

  fieldNameWithRecord.append(recordName);
  fieldNameWithRecord.append(CTreeField::m_keyDelimiter);
  fieldNameWithRecord.append(fieldFullName);

  return fieldNameWithRecord;
}
//----------------------------------------
void CField::PopRecordCusor(CObList* parentFieldList)
{
  // Pop cursor by reverse list from last (low level parent) to top (high level parent)
  CObList::reverse_iterator itParentField;
  for (itParentField = parentFieldList->rbegin() ; itParentField != parentFieldList->rend() ; itParentField++)
  {
    CFieldRecord* parentFieldRecord = dynamic_cast<CFieldRecord*>(*itParentField);
    if (parentFieldRecord != NULL)
    {
      parentFieldRecord->PopCursor();
    }
  }
}
/*
//----------------------------------------
void CField::ReadParent(CDoubleArray& vect, CFieldRecord* parentField)
{
  CException e("ERROR - CField::ReadParent(CDoubleArray& vect, CFieldRecord* parentField) - unexpected call - ReadParent(CDoubleArray& vect, CFieldRecord* parentField) have not to be called ", BRATHL_LOGIC_ERROR);
  CTrace::Tracer(e.what());
  Dump(*CTrace::GetDumpContext());
  throw (e);

}
*/
//----------------------------------------
void CField::ReadParent(CDoubleArray& vect, CFieldRecord* parentField)
{
  int32_t result = 0;

  if (parentField->GetTypeClass() != coda_array_class) //test the type of the parent type class
  {
    std::string msg = CTools::Format("ERROR - CFieldArray::ReadParent - parent Field typeClass (%s) must be coda_array_class "
                              "(field '%s')",
                              coda_type_get_class_name(parentField->GetTypeClass()),
                              parentField->GetKey().c_str());
    CProductException e(msg, BRATHL_UNIMPLEMENT_ERROR);
    throw (e);

  }

  if (parentField->GetNbDims() != 1 )
  {
    std::string msg = CTools::Format("ERROR - CFieldBasic::ReadParent - Number of array dim %d not implemented for this method "
                              "(field '%s')",
                              parentField->GetNbDims(),
                              parentField->GetKey().c_str());
    CProductException e(msg, BRATHL_UNIMPLEMENT_ERROR);
    throw (e);
  }

  uint32_t count = parentField->GetDim()[0];

  double* data = new double[m_maxPos+1];

  for (uint32_t iParent = 0 ; iParent < count ; iParent++)
  {

    result = coda_cursor_goto_array_element_by_index(parentField->GetCursor(), iParent);
    HandleBratError("coda_cursor_goto_array_element_by_index");
  //result = brat_cursor_goto_first_array_element(&m_cursor);
  //HandleBratError("brat_cursor_goto_first_array_element");

  //brat_cursor_get_num_elements(parentField->GetCursor(), &n_elements);
  //HandleBratError("brat_cursor_count_elements in CField::PushPos");

    result = coda_cursor_goto_record_field_by_name(parentField->GetCursor(), this->GetName().c_str());
    HandleBratError("coda_cursor_goto_record_field_by_name");

    this->SetCursor(*(parentField->GetCursor()));


    Read(data);

    vect.Insert(data, m_maxPos+1);

    parentField->PopCursor();
  }

  delete []data;

}
/*
//----------------------------------------
void CField::ReadParent(CDoubleArray& vect, CObList* parentFieldList)
{
  CException e("ERROR - CField::ReadParent(CDoubleArray& vect, CObList* parentFieldList) - unexpected call - ReadParent(CDoubleArray& vect, CObList* parentFieldList) have not to be called ", BRATHL_LOGIC_ERROR);
  CTrace::Tracer(e.what());
  Dump(*CTrace::GetDumpContext());
  throw (e);

}
*/
//----------------------------------------
void CField::ReadParent(CDoubleArray& vect, CObList* parentFieldList)
{
  int32_t result = 0;
  if (parentFieldList == NULL)
  {
    return;
  }

  if (parentFieldList->empty())
  {
    std::string msg = CTools::Format("ERROR - CFieldArray::ReadParent - parent Field list is empty()"
                              "(field '%s')",
                              this->GetKey().c_str());
    CProductException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }

  CFieldRecord* topParentField = dynamic_cast<CFieldRecord*>(*(parentFieldList->begin()));
  if (topParentField == NULL)
  {
    std::string msg = CTools::Format("ERROR - CFieldArray::ReadParent - top parent Field list NULL()"
                              "(field '%s')",
                              this->GetKey().c_str());
    CProductException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }


  if (topParentField->GetTypeClass() != coda_array_class) //test the type of the parent type class
  {
    std::string msg = CTools::Format("ERROR - CFieldArray::ReadParent - top parent Field typeClass (%s) must be coda_array_class "
                              "(field '%s')",
                              coda_type_get_class_name(topParentField->GetTypeClass()),
                              topParentField->GetKey().c_str());
    CProductException e(msg, BRATHL_UNIMPLEMENT_ERROR);
    throw (e);

  }

  if (topParentField->GetNbDims() != 1 )
  {
    std::string msg = CTools::Format("ERROR - CFieldArray::ReadParent - Number of array dim %d not implemented for this method "
                              "(field '%s')",
                              topParentField->GetNbDims(),
                              topParentField->GetKey().c_str());
    CProductException e(msg, BRATHL_UNIMPLEMENT_ERROR);
    throw (e);
  }


  CObList::iterator itParentField;
  for (itParentField = parentFieldList->begin() ; itParentField != parentFieldList->end() ; itParentField++)
  {
    if (itParentField == parentFieldList->begin()) //  ignore top level parent
    {
      continue;
    }
    CFieldRecord* parentFieldRecord = dynamic_cast<CFieldRecord*>(*itParentField);
    if (parentFieldRecord == NULL)
    {
      continue;
    }

    if (parentFieldRecord->GetTypeClass() == coda_array_class)
    {
      std::string msg = CTools::Format("ERROR - CFieldArray::ReadParent - itermediate parent Field typeClass %s not not implemented for this method "
                                "(field '%s')",
                                coda_type_get_class_name(parentFieldRecord->GetTypeClass()),
                                parentFieldRecord->GetKey().c_str());
      CProductException e(msg, BRATHL_UNIMPLEMENT_ERROR);
      throw (e);
    }

  }

  PopRecordCusor(parentFieldList);

  uint32_t count = topParentField->GetDim()[0];

  double* data = new double[m_maxPos+1];

  CFieldRecord* parentFieldRecord = NULL;
  CFieldRecord* parentFieldRecordPrevious = NULL;

  for (uint32_t iTopParent = 0 ; iTopParent < count ; iTopParent++)
  {
    for (itParentField = parentFieldList->begin() ; itParentField != parentFieldList->end() ; itParentField++)
    {
      parentFieldRecord = dynamic_cast<CFieldRecord*>(*itParentField);
      if (parentFieldRecord == NULL)
      {
        continue;
      }

      if (itParentField == parentFieldList->begin()) //  ignore top level parent
      {
        result = coda_cursor_goto_array_element_by_index(parentFieldRecord->GetCursor(), iTopParent);
        HandleBratError("coda_cursor_goto_array_element_by_index");
        parentFieldRecordPrevious = parentFieldRecord;
      }
      else
      {
        result = coda_cursor_goto_record_field_by_name(parentFieldRecordPrevious->GetCursor(), parentFieldRecord->GetName().c_str());
        HandleBratError("coda_cursor_goto_record_field_by_name");

        parentFieldRecord->SetCursor(*(parentFieldRecordPrevious->GetCursor()));

        parentFieldRecordPrevious = parentFieldRecord;
      }

    }

    if (parentFieldRecord == NULL)
    {
      std::string msg = CTools::Format("ERROR - CFieldArray::ReadParent - low parent Field list NULL()"
                                "(field '%s')",
                                this->GetKey().c_str());
      CProductException e(msg, BRATHL_INCONSISTENCY_ERROR);
      throw (e);
    }

    result = coda_cursor_goto_record_field_by_name(parentFieldRecord->GetCursor(), this->GetName().c_str());
    HandleBratError("coda_cursor_goto_record_field_by_name");

    this->SetCursor(*(parentFieldRecord->GetCursor()));

    Read(data);

    vect.Insert(data, m_maxPos+1);

    PopRecordCusor(parentFieldList);
  }

  delete []data;

}
//----------------------------------------
void CField::Read(double* data, bool skip /*= false*/)
{
    UNUSED(data);   UNUSED(skip);

  CException e("ERROR - CField::Read(double* data) - unexpected call - CField::Read(double* data) have not to be called ", BRATHL_LOGIC_ERROR);
  CTrace::Tracer("%s", e.what());
  Dump(*CTrace::GetDumpContext());
  throw (e);

}
//----------------------------------------
void CField::Read(CDoubleArray& vect, bool skip /*= false*/)
{
    UNUSED(vect);   UNUSED(skip);

  CException e("ERROR - CField::Read(CDoubleArray& vect) - unexpected call - CField::Read(CDoubleArray& vect) have not to be called ", BRATHL_LOGIC_ERROR);
  CTrace::Tracer("%s", e.what());
  Dump(*CTrace::GetDumpContext());
  throw (e);
}
//----------------------------------------
void CField::Read(std::string& value, bool skip /*= false*/)
{
    UNUSED(value);   UNUSED(skip);

  CException e("ERROR - CField::Read(std::string& value) - unexpected call - CField::Read(std::string& value) have not to be called ", BRATHL_LOGIC_ERROR);
  CTrace::Tracer("%s", e.what());
  Dump(*CTrace::GetDumpContext());
  throw (e);
}
//----------------------------------------
std::string CField::GetNativeTypeName()
{
  return coda_type_get_native_type_name(m_nativeType);
}
//----------------------------------------
std::string CField::GetSpecialTypeName()
{
  return coda_type_get_special_type_name(m_specialType);
}
//----------------------------------------
bool CField::IsFieldNetCdfCFAttr()
{
  return IsFieldNetCdfCFAttr(this);
}
//----------------------------------------
bool CField::IsFieldNetCdfCFAttr(CBratObject* ob)
{
  return (GetFieldNetCdfCFAttr(ob, false) != NULL);
}
//----------------------------------------
CFieldNetCdfCFAttr* CField::GetFieldNetCdfCFAttr(CBratObject* ob, bool withExcept /*=true*/)
{
  CFieldNetCdfCFAttr* fieldNetCdfCFAttr = dynamic_cast<CFieldNetCdfCFAttr*>(ob);
  if (withExcept)
  {
    if (fieldNetCdfCFAttr == NULL)
    {
      CException e("CField::GetFieldNetCdfCFAttr - dynamic_cast<CFieldNetCdfCFAttr*>(ob) returns NULL"
                   "object seems not to be an instance of CFieldNetCdfCFAttr",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }

  return fieldNetCdfCFAttr;

}
//----------------------------------------
CFieldNetCdfIndexData* CField::GetFieldNetCdfIndexData(CBratObject* ob, bool withExcept /*=true*/)
{
  CFieldNetCdfIndexData* fieldNetCdfIndexData = dynamic_cast<CFieldNetCdfIndexData*>(ob);
  if (withExcept)
  {
    if (fieldNetCdfIndexData == NULL)
    {
      CException e("CField::GetFieldNetCdfIndexData - dynamic_cast<CFieldNetCdfIndexData*>(ob) returns NULL"
                   "object seems not to be an instance of CFieldNetCdfIndexData",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }

  return fieldNetCdfIndexData;

}

//----------------------------------------

void CField::HandleBratError(const std::string& str /* = ""*/, int32_t errClass /* = BRATHL_LOGIC_ERROR*/)
{
  if (coda_errno == 0)
  {
    return;
  }

  std::string msg = CTools::Format("BRAT ERROR - %s - errno:#%d:%s", str.c_str(), coda_errno, coda_errno_to_string (coda_errno));
  CProductException e(msg, errClass);
  CTrace::Tracer("%s", e.what());
  Dump(*CTrace::GetDumpContext());
  throw (e);

}


//----------------------------------------
void  CField::DumpFieldDictionary(std::ostream& fOut)
{
  //fOut << typeid(*this).name() << "\t"
  fOut << m_name << "\t"
       << m_unit << "\t"
       << m_typeClass << " (" << coda_type_get_class_name(m_typeClass) << ")" << "\t"
       << m_specialType << " (" << coda_type_get_special_type_name(m_specialType) << ")" << "\t"
       << m_nativeType << " (" << coda_type_get_native_type_name(m_nativeType) << ")" << "\t"
       << m_metaData << "\t"
       << m_index << "\t"
       << "\"" << m_description << "\"" << "\t"
       << m_isFixedSize << "\t"
       << m_nbDims  << "\t";

       for (int32_t i = 0 ; i < m_nbDims ; i++)
       {
        fOut << "[" << i << "]=" << m_dim[i] << " ";
       }

}
//----------------------------------------

void CField::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CField Object at "<< this << std::endl;

  fOut << "m_name = " << m_name << std::endl;
  fOut << "m_description = " << m_description << std::endl;
  fOut << "m_unit = " << m_unit << std::endl;
  fOut << "m_typeClass = " << m_typeClass << " (" << coda_type_get_class_name(m_typeClass) << ")" << std::endl;
  fOut << "m_specialType = " << m_specialType << " (" << coda_type_get_special_type_name(m_specialType) << ")" << std::endl;
  fOut << "m_nativeType = " << m_nativeType << " (" << coda_type_get_native_type_name(m_nativeType) << ")" << std::endl;
  fOut << "m_metaData = " << m_metaData << std::endl;
  fOut << "m_index = " << m_index << std::endl;
  fOut << "m_isFixedSize = " << m_isFixedSize << std::endl;
  fOut << "m_offset = " << m_offset << std::endl;
  fOut << "m_nbDims = " << m_nbDims << std::endl;

  for (int32_t i = 0 ; i < m_nbDims ; i++)
  {
    fOut << "m_dim[" << i << "]=" << m_dim[i] << std::endl;
  }


  fOut << "==> END Dump a CField Object at "<< this << std::endl;

  fOut << std::endl;

}


//-------------------------------------------------------------
//------------------- CFieldRecord class --------------------
//-------------------------------------------------------------
CFieldRecord::CFieldRecord()
{
  m_nbFields = 0;
}
//----------------------------------------

CFieldRecord::CFieldRecord(size_t nbFields, const std::string& name, const std::string& description, const std::string& unit)
    : CFieldArray(name, description, unit)
{
  m_nbFields = nbFields;
}
//----------------------------------------

CFieldRecord::CFieldRecord(int32_t nbDims, const long dim[], size_t nbFields, const std::string& name, const std::string& description, const std::string& unit)
    : CFieldArray(nbDims, dim, name, description, unit)
{
  m_nbFields = nbFields;
}

//----------------------------------------

CFieldRecord::~CFieldRecord()
{

}
//----------------------------------------

CFieldRecord::CFieldRecord(CFieldRecord& f)
{
  Set(f);
}


//----------------------------------------
const CFieldRecord& CFieldRecord::operator =(CFieldRecord& f)
{
  Set(f);

  return *this;
}

//----------------------------------------
void CFieldRecord::Set(CFieldRecord& f)
{
  CFieldArray::Set(f);

  m_nbFields = f.m_nbFields;
}

//----------------------------------------
int32_t CFieldRecord::GetVirtualNbDims()
{
  return CField::GetVirtualNbDims();
}

//----------------------------------------

CFieldSet* CFieldRecord::CreateFieldSet(const CField::CListField& listFields)
{
  return CFieldArray::CreateFieldSet(listFields);
}


//----------------------------------------
void CFieldRecord::PopCursor()
{
  if (m_typeClass == coda_array_class)
  {
    coda_cursor_goto_parent(&m_cursor); // goto Level Array
    HandleBratError("coda_cursor_goto_parent in  CFieldRecord::PopCursor");
  }

  coda_cursor_goto_parent(&m_cursor); // goto Level Record
  HandleBratError("coda_cursor_goto_parent in  CFieldRecord::PopCursor");
}

//----------------------------------------
void CFieldRecord::PushPos()
{
  PushPos(m_nbDims-1);
}

//----------------------------------------
void CFieldRecord::PushPos(int32_t iDim)
{
  CFieldArray::PushPos(iDim);
}
/*
//----------------------------------------
void CFieldRecord::ReadParent(CDoubleArray& vect, CObList* parentFieldList)
{
  CFieldArray::ReadParent(vect, parentFieldList);
}
//----------------------------------------
void CFieldRecord::ReadParent(CDoubleArray& vect, CFieldRecord* parentField)
{
  CFieldArray::ReadParent(vect, parentField);
}
*/
//----------------------------------------
void CFieldRecord::Read(CDoubleArray& vect, bool skip /*= false*/)
{
  CFieldArray::Read(vect, skip);
}


//----------------------------------------
void CFieldRecord::Read(double* data, bool skip /*= false*/)
{
    UNUSED(data);   UNUSED(skip);

  coda_ProductFile *pf;
  const char *fileName;

  coda_cursor_get_product_file(&m_cursor, &pf);
  HandleBratError("coda_cursor_get_product in CFieldRecord::Read");

  coda_get_product_filename(pf, &fileName);
  HandleBratError("coda_get_product_filename in CFieldRecord::Read");

  std::string msg = CTools::Format("ERROR - CFieldRecord::Read : Fieldname:'%s' is a record (structure) - It is not directly readable -"
                              " Verify your field syntax with the file structure in the datadictionary - File:%s",
                              m_name.c_str(),
                              fileName);
  CUnImplementException e(msg, BRATHL_UNIMPLEMENT_ERROR);
  throw(e);
}

//----------------------------------------
void  CFieldRecord::DumpFieldDictionary(std::ostream& fOut)
{
  CFieldArray::DumpFieldDictionary(fOut);

  fOut << "\t"
       << m_nbFields;

}


//----------------------------------------

void CFieldRecord::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CFieldRecord Object at "<< this << std::endl;
  CFieldArray::Dump(fOut);
  fOut << "m_nbFields = " << m_nbFields << std::endl;

  fOut << "==> END Dump a CFieldRecord Object at "<< this << std::endl;

  fOut << std::endl;

}



//-------------------------------------------------------------
//------------------- CFieldArray class --------------------
//-------------------------------------------------------------


CFieldArray::CFieldArray()
{

}

//----------------------------------------

CFieldArray::CFieldArray(const std::string& name, const std::string& description, const std::string& unit)
    : CField(name, description, unit)
{

}

//----------------------------------------

CFieldArray::CFieldArray(int32_t nbDims, const long dim[], const std::string& name, const std::string& description, const std::string& unit)
    : CField(name, description, unit)
{

  SetDim(nbDims, dim);

}

//----------------------------------------

CFieldArray::~CFieldArray()
{

}
//----------------------------------------
CFieldArray::CFieldArray(CFieldArray& f)
{
  Set(f);
}


//----------------------------------------
const CFieldArray& CFieldArray::operator =(CFieldArray& f)
{
  Set(f);

  return *this;
}

//----------------------------------------
void CFieldArray::Set(CFieldArray& f)
{
  CField::Set(f);

}
//----------------------------------------
uint32_t CFieldArray::GetOffsetDim()
{
  // Ignore first dim for High resolution field (first dim is expanded)
  if (IsHighResolution())
  {
    if (m_dim[0] == static_cast<int32_t>(m_numHighResolutionMeasure))
    {
      return 1;
    }
  }

  return 0;

}

//----------------------------------------
int32_t CFieldArray::GetVirtualNbDims()
{
  // Ignore first dim for High resolution field (first dim is expanded)
  if (IsHighResolution())
  {
    if (m_dim[0] == static_cast<int32_t>(m_numHighResolutionMeasure))
    {
      return m_nbDims - 1;
    }
  }

  return m_nbDims;
}

//----------------------------------------
void CFieldArray::CreateFieldIndexes(CObArray& vect)
{

  if (!HasVirtualNbDims())
  {
    return;
  }

  if (!IsFixedSize())
  {
    return;
  }

  int32_t iStart = 0;

  if (IsHighResolution())
  {
    if (m_dim[0] == static_cast<int32_t>(m_numHighResolutionMeasure))
    {
      iStart = 1;
    }
  }

  for (int32_t i = 0 ; i < m_nbDims ; i++)
  {
    if (m_dim[i] <= 0)
    {
      continue;
    }

    std::string name = CTools::Format("%s_index_%ld", m_name.c_str(), m_dim[i]);
    std::string description = CTools::Format("dimension %d (value %ld) from '%s' field", i + 1, m_dim[i], m_name.c_str());
    CFieldIndex* field = new CFieldIndex(name, m_dim[i], description);

    AddFieldIndexes(field);

    vect.Insert(field);
  }



}
//----------------------------------------

CFieldSet* CFieldArray::CreateFieldSet(const CField::CListField& listFields)
{
  CFieldSetArrayDbl* fieldSet = new CFieldSetArrayDbl(m_key);
  fieldSet->SetField(this);
  fieldSet->m_nbDims = listFields.m_nbFieldSetDims;
  fieldSet->m_dim = listFields.m_fieldSetDim;
  return fieldSet;
}

//----------------------------------------
void CFieldArray::PopCursor()
{
  coda_cursor_goto_parent(&m_cursor); // goto Level Array
  HandleBratError("coda_cursor_goto_parent in CFieldArray::PopCursor");

  coda_cursor_goto_parent(&m_cursor); // goto Level Record
  HandleBratError("coda_cursor_goto_parent in CFieldArray::PopCursor");
}

//----------------------------------------
void CFieldArray::PushPos()
{
  PushPos(m_nbDims-1);
}
//----------------------------------------

void CFieldArray::PushPos(int32_t iDim)
{
  if ((iDim) < 0)
  {
    return;
  }

  PushPos(iDim-1);

  if (iDim == 0)
  {
    m_currentPos += 1;
  }
  else
  {
    m_currentPos += m_dim[iDim - 1];
  }

/*
  int32_t n_elements =0;
  brat_cursor_get_num_elements(&m_cursor, &n_elements);
  HandleBratError("brat_cursor_count_elements in CField::PushPos");


  brat_cursor_goto_array_element_by_index(&m_cursor, m_currentPos);
  HandleBratError("brat_cursor_goto_array_element_by_index in CField::PushPos");

  brat_cursor_get_num_elements(&m_cursor, &n_elements);
  HandleBratError("brat_cursor_get_num_elements in CField::PushPos");
*/
}
/*
//----------------------------------------
void CFieldArray::ReadParent(CDoubleArray& vect, CObList* parentFieldList)
{
  int32_t result = 0;
  if (parentFieldList == NULL)
  {
    return;
  }

  if (parentFieldList->empty())
  {
    std::string msg = CTools::Format("ERROR - CFieldArray::ReadParent - parent Field list is empty()"
                              "(field '%s')",
                              this->GetKey().c_str());
    CProductException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }

  CFieldRecord* topParentField = dynamic_cast<CFieldRecord*>(*(parentFieldList->begin()));
  if (topParentField == NULL)
  {
    std::string msg = CTools::Format("ERROR - CFieldArray::ReadParent - top parent Field list NULL()"
                              "(field '%s')",
                              this->GetKey().c_str());
    CProductException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }


  if (topParentField->GetTypeClass() != coda_array_class) //test the type of the parent type class
  {
    std::string msg = CTools::Format("ERROR - CFieldArray::ReadParent - top parent Field typeClass (%s) must be coda_array_class "
                              "(field '%s')",
                              coda_type_get_class_name(m_typeClass),
                              topParentField->GetKey().c_str());
    CProductException e(msg, BRATHL_UNIMPLEMENT_ERROR);
    throw (e);

  }

  if (topParentField->GetNbDims() != 1 )
  {
    std::string msg = CTools::Format("ERROR - CFieldArray::ReadParent - Number of array dim %d not implemented for this method "
                              "(field '%s')",
                              topParentField->GetNbDims(),
                              topParentField->GetKey().c_str());
    CProductException e(msg, BRATHL_UNIMPLEMENT_ERROR);
    throw (e);
  }


  CObList::iterator itParentField;
  for (itParentField = parentFieldList->begin() ; itParentField != parentFieldList->end() ; itParentField++)
  {
    if (itParentField == parentFieldList->begin()) //  ignore top level parent
    {
      continue;
    }
    CFieldRecord* parentFieldRecord = dynamic_cast<CFieldRecord*>(*itParentField);
    if (parentFieldRecord == NULL)
    {
      continue;
    }

    if (parentFieldRecord->GetTypeClass() == coda_array_class)
    {
      std::string msg = CTools::Format("ERROR - CFieldArray::ReadParent - itermediate parent Field typeClass %s not not implemented for this method "
                                "(field '%s')",
                                parentFieldRecord->GetNbDims(),
                                parentFieldRecord->GetKey().c_str());
      CProductException e(msg, BRATHL_UNIMPLEMENT_ERROR);
      throw (e);
    }

  }

  PopRecordCusor(parentFieldList);

  uint32_t count = topParentField->GetDim()[0];

  double* data = new double[m_maxPos+1];

  CFieldRecord* parentFieldRecord = NULL;
  CFieldRecord* parentFieldRecordPrevious = NULL;

  for (uint32_t iTopParent = 0 ; iTopParent < count ; iTopParent++)
  {
    for (itParentField = parentFieldList->begin() ; itParentField != parentFieldList->end() ; itParentField++)
    {
      parentFieldRecord = dynamic_cast<CFieldRecord*>(*itParentField);
      if (parentFieldRecord == NULL)
      {
        continue;
      }

      if (itParentField == parentFieldList->begin()) //  ignore top level parent
      {
        result = brat_cursor_goto_array_element_by_index(parentFieldRecord->GetCursor(), iTopParent);
        HandleBratError("brat_cursor_goto_array_element_by_index");
        parentFieldRecordPrevious = parentFieldRecord;
      }
      else
      {
        result = brat_cursor_goto_record_field_by_name(parentFieldRecordPrevious->GetCursor(), parentFieldRecord->GetName().c_str());
        HandleBratError("brat_cursor_goto_record_field_by_name");
        parentFieldRecordPrevious = parentFieldRecord;
      }

    }

    if (parentFieldRecord == NULL)
    {
      std::string msg = CTools::Format("ERROR - CFieldArray::ReadParent - low parent Field list NULL()"
                                "(field '%s')",
                                this->GetKey().c_str());
      CProductException e(msg, BRATHL_INCONSISTENCY_ERROR);
      throw (e);
    }

    result = brat_cursor_goto_record_field_by_name(parentFieldRecord->GetCursor(), this->GetName().c_str());
    HandleBratError("brat_cursor_goto_record_field_by_name");

    this->SetCursor(*(parentFieldRecord->GetCursor()));

    Read(data);

    vect.Insert(data, m_maxPos+1);

    PopRecordCusor(parentFieldList);
  }

  delete []data;

}
*/
//----------------------------------------
/*
void CFieldArray::ReadParent(CDoubleArray& vect, CFieldRecord* parentField)
{
  int32_t result = 0;

  if (parentField->GetTypeClass() != brat_array_class) //test the type of the parent type class
  {
    std::string msg = CTools::Format("ERROR - CFieldArray::ReadParent - parent Field typeClass (%s) must be brat_array_class "
                              "(field '%s')",
                              brat_type_get_class_name(m_typeClass),
                              parentField->GetKey().c_str());
    CProductException e(msg, BRATHL_UNIMPLEMENT_ERROR);
    throw (e);

  }

  if (parentField->GetNbDims() != 1 )
  {
    std::string msg = CTools::Format("ERROR - CFieldBasic::ReadParent - Number of array dim %d not implemented for this method "
                              "(field '%s')",
                              parentField->GetNbDims(),
                              parentField->GetKey().c_str());
    CProductException e(msg, BRATHL_UNIMPLEMENT_ERROR);
    throw (e);
  }

  uint32_t count = parentField->GetDim()[0];

  double* data = new double[m_maxPos+1];

  for (uint32_t iParent = 0 ; iParent < count ; iParent++)
  {

    result = brat_cursor_goto_array_element_by_index(parentField->GetCursor(), iParent);
    HandleBratError("brat_cursor_goto_array_element_by_index");
  //result = brat_cursor_goto_first_array_element(&m_cursor);
  //HandleBratError("brat_cursor_goto_first_array_element");

  //brat_cursor_get_num_elements(parentField->GetCursor(), &n_elements);
  //HandleBratError("brat_cursor_count_elements in CField::PushPos");

    result = brat_cursor_goto_record_field_by_name(parentField->GetCursor(), this->GetName().c_str());
    HandleBratError("brat_cursor_goto_record_field_by_name");

    this->SetCursor(*(parentField->GetCursor()));


    Read(data);

    vect.Insert(data, m_maxPos+1);

    parentField->PopCursor();
  }

  delete []data;

}
*/
//----------------------------------------
void CFieldArray::Read(CDoubleArray& vect, bool skip /*= false*/)
{
  double* data = new double[m_maxPos+1];
  Read(data, skip);
  vect.Insert(data, m_maxPos+1);
  delete []data;
}
//----------------------------------------
void CFieldArray::Read(double* data, bool skip /*= false*/)
{

  if (skip)
  {
    SetDefaultValue(data, m_maxPos+1);
    return;
  }

  switch (this->m_specialType)
  {
    //----------------------
    case coda_special_no_data:
    //----------------------
    {
      //ignore
      break;
    }
    //----------------------
    case coda_special_time:
    //----------------------
    {
      coda_cursor_read_double_array(&m_cursor, data, CProduct::m_arrayOrdering);
      HandleBratError("coda_cursor_read_double_array in CFieldArray::Read");

      // Convert data to SI
      Convert(data, m_maxPos+1);
      //-*-*-*-*-*
      return;
      //-*-*-*-*-*
    }

    //----------------------
    default:
    //----------------------
    {
      coda_ProductFile *pf;
      const char *fileName;

      coda_cursor_get_product_file(&m_cursor, &pf);
      HandleBratError("coda_cursor_get_product in CFieldArray::Read");

      coda_get_product_filename(pf, &fileName);
      HandleBratError("coda_get_product_filename in CFieldArray::Read");

      std::string msg = CTools::Format("ERROR - CFieldArray::Read : unexpected Brat special type %d (%s) -  FieldName:%s - File:%s",
                                  this->m_specialType,
                                  coda_type_get_special_type_name(this->m_specialType),
                                  m_name.c_str(),
                                  fileName);
      CUnImplementException e(msg, BRATHL_UNIMPLEMENT_ERROR);
      throw(e);
      break;
    }
  }

  switch (this->m_nativeType)
  {
    //----------------------
    case coda_native_type_int8:
    case coda_native_type_int16:
    case coda_native_type_int32:
    case coda_native_type_int64:
    case coda_native_type_uint8:
    case coda_native_type_uint16:
    case coda_native_type_uint32:
    case coda_native_type_uint64:
    case coda_native_type_float:
    case coda_native_type_double:
    //----------------------
    {
      coda_cursor_read_double_array(&m_cursor, data, CProduct::m_arrayOrdering);
      HandleBratError("coda_cursor_read_double_array in CFieldArray::Read");

      // Convert data to SI
      Convert(data, m_maxPos+1);

      break;
    }
    //----------------------
    default:
    //----------------------
    {
      coda_ProductFile *pf;
      const char *fileName;

      coda_cursor_get_product_file(&m_cursor, &pf);
      HandleBratError("coda_cursor_get_product in CFieldArray::Read");

      coda_get_product_filename(pf, &fileName);
      HandleBratError("coda_get_product_filename in CFieldArray::Read");

      std::string msg = CTools::Format("ERROR - CFieldArray::Read : unexpected Brat native type %d (%s) -  FieldName:%s - File:%s",
                                  this->m_nativeType,
                                  coda_type_get_native_type_name(this->m_nativeType),
                                  m_name.c_str(),
                                  fileName);
      CUnImplementException e(msg, BRATHL_UNIMPLEMENT_ERROR);
      throw(e);
      break;
    }
  }

  TransposeValues(data, m_maxPos+1);

  AdjustValidMinMax(data, m_maxPos+1);

  return;
}

//----------------------------------------
void  CFieldArray::DumpFieldDictionary(std::ostream& fOut)
{
  CField::DumpFieldDictionary(fOut);

}

//----------------------------------------

void CFieldArray::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CFieldArray Object at "<< this << std::endl;
  CField::Dump(fOut);
  fOut << "==> END Dump a CFieldArray Object at "<< this << std::endl;

  fOut << std::endl;

}


//-------------------------------------------------------------
//------------------- CFieldBasic class --------------------
//-------------------------------------------------------------
CFieldBasic::CFieldBasic()
{
  m_length = -1;
}

//----------------------------------------

CFieldBasic::CFieldBasic(long length, const std::string& name, const std::string& description, const std::string& unit)
    : CField(name, description, unit)
{
    m_length = length;
}


//----------------------------------------

CFieldBasic::~CFieldBasic()
{

}
//----------------------------------------

CFieldBasic::CFieldBasic(CFieldBasic& f)
{
  Set(f);
}


//----------------------------------------
const CFieldBasic& CFieldBasic::operator =(CFieldBasic& f)
{
  Set(f);

  return *this;
}

//----------------------------------------
void CFieldBasic::Set(CFieldBasic& f)
{
  CField::Set(f);

  m_length = f.m_length;
}

//----------------------------------------

CFieldSet* CFieldBasic::CreateFieldSet(const CField::CListField& listFields)
{

  if (this->GetNativeType() == coda_native_type_string)
  {
    CFieldSetString* fieldSet = new CFieldSetString(m_key);
    fieldSet->SetField(this);
    return fieldSet;
  }

  if (listFields.m_nbFieldSetDims > 0)
  {
    CFieldSetArrayDbl* fieldSet = new CFieldSetArrayDbl(m_key);
    fieldSet->SetField(this);
    fieldSet->m_nbDims = listFields.m_nbFieldSetDims;
    fieldSet->m_dim = listFields.m_fieldSetDim;
    return fieldSet;
  }
  else
  {
    CFieldSetDbl* fieldSet = new CFieldSetDbl(m_key);
    fieldSet->SetField(this);
    return fieldSet;
  }
}

/*
//----------------------------------------
void CFieldBasic::ReadParent(CDoubleArray& vect, CFieldRecord* parentField)
{
  int32_t result = 0;

  if (parentField->GetTypeClass() != brat_array_class) //test the type of the parent type class
  {
    std::string msg = CTools::Format("ERROR - CFieldBasic::ReadParent - parent Field typeClass (%s) must be brat_array_class "
                              "(field '%s')",
                              brat_type_get_class_name(m_typeClass),
                              parentField->GetKey().c_str());
    CProductException e(msg, BRATHL_UNIMPLEMENT_ERROR);
    throw (e);

  }

  if (parentField->GetNbDims() != 1 )
  {
    std::string msg = CTools::Format("ERROR - CFieldBasic::ReadParent - Number of array dim %d not implemented for this method "
                              "(field '%s')",
                              parentField->GetNbDims(),
                              parentField->GetKey().c_str());
    CProductException e(msg, BRATHL_UNIMPLEMENT_ERROR);
    throw (e);
  }

  uint32_t count = parentField->GetDim()[0];

  double* data = new double[m_maxPos+1];

  for (uint32_t iParent = 0 ; iParent < count ; iParent++)
  {

    result = brat_cursor_goto_array_element_by_index(parentField->GetCursor(), iParent);
    HandleBratError("brat_cursor_goto_array_element_by_index");
  //result = brat_cursor_goto_first_array_element(&m_cursor);
  //HandleBratError("brat_cursor_goto_first_array_element");

  //brat_cursor_get_num_elements(parentField->GetCursor(), &n_elements);
  //HandleBratError("brat_cursor_count_elements in CField::PushPos");

    result = brat_cursor_goto_record_field_by_name(parentField->GetCursor(), this->GetName().c_str());
    HandleBratError("brat_cursor_goto_record_field_by_name");

    this->SetCursor(*(parentField->GetCursor()));


    Read(data);

    vect.Insert(data, m_maxPos+1);

    parentField->PopCursor();
  }

  delete []data;

}
*/
//----------------------------------------
void CFieldBasic::Read(CDoubleArray& vect, bool skip /*= false*/)
{
  double* data = new double[m_maxPos+1];
  Read(data, skip);
  vect.Insert(data, m_maxPos+1);
  delete []data;
}
//----------------------------------------
void CFieldBasic::Read(double* data, bool skip /*= false*/)
{

  if (skip)
  {
    SetDefaultValue(data, m_maxPos+1);
    return;
  }

  switch (this->m_specialType)
  {
    //----------------------
    case coda_special_no_data:
    //----------------------
    {
      //ignore
      break;
    }
    //----------------------
    case coda_special_time:
    //----------------------
    {
      // read date (decimal seconds number)
      coda_cursor_read_double(&m_cursor, data);
      HandleBratError("coda_cursor_read_double in CFieldBasic::Read");

      // Convert data to SI
      Convert(data, m_maxPos+1);

      //-*-*-*-*-*
      return;
      //-*-*-*-*-*
    }

    //----------------------
    default:
    //----------------------
    {
      coda_ProductFile *pf;
      const char *fileName;

      coda_cursor_get_product_file(&m_cursor, &pf);
      HandleBratError("coda_cursor_get_product in CFieldBasic::Read");

      coda_get_product_filename(pf, &fileName);
      HandleBratError("coda_get_product_filename in CFieldBasic::Read");

      std::string msg = CTools::Format("ERROR - CFieldBasic::Read : unexpected Brat special type %d (%s) -  FieldName:%s - File:%s",
                                  this->m_specialType,
                                  coda_type_get_special_type_name(this->m_specialType),
                                  m_name.c_str(),
                                  fileName);
      CUnImplementException e(msg, BRATHL_UNIMPLEMENT_ERROR);
      throw(e);
      break;
    }
  }

  switch (this->m_nativeType)
  {
    //----------------------
    case coda_native_type_int8:
    case coda_native_type_int16:
    case coda_native_type_int32:
    case coda_native_type_int64:
    case coda_native_type_uint8:
    case coda_native_type_uint16:
    case coda_native_type_uint32:
    case coda_native_type_uint64:
    case coda_native_type_float:
    case coda_native_type_double:
    //----------------------
    {
      coda_cursor_read_double(&m_cursor, data);
      HandleBratError("coda_cursor_read_double in CFieldBasic::Read");

      // Convert data to SI
      Convert(data, m_maxPos+1);

      break;
    }
    //----------------------
    default:
    //----------------------
    {
      coda_ProductFile *pf;
      const char *fileName;

      coda_cursor_get_product_file(&m_cursor, &pf);
      HandleBratError("coda_cursor_get_product in CFieldBasic::Read");

      coda_get_product_filename(pf, &fileName);
      HandleBratError("coda_get_product_filename in CFieldBasic::Read");

      std::string msg = CTools::Format("ERROR - CFieldBasic::Read : unexpected Brat native type %d (%s) -  FieldName:%s - File:%s",
                                  this->m_nativeType,
                                  coda_type_get_native_type_name(this->m_nativeType),
                                  m_name.c_str(),
                                  fileName);
      CUnImplementException e(msg, BRATHL_UNIMPLEMENT_ERROR);
      throw(e);
      break;
    }
  }

  AdjustValidMinMax(data, m_maxPos+1);

  return;
}


//----------------------------------------
void CFieldBasic::Read(std::string& value, bool skip /*= false*/)
{
  if (skip)
  {
    value = "";
    return;
  }


  switch (this->m_specialType)
  {
    //----------------------
    case coda_special_no_data:
    //----------------------
    {
      //ignore
      break;
    }
    //----------------------
    default:
    //----------------------
    {
      coda_ProductFile *pf;
      const char *fileName;

      coda_cursor_get_product_file(&m_cursor, &pf);
      HandleBratError("coda_cursor_get_product in CFieldBasic::Read(std::string& value)");

      coda_get_product_filename(pf, &fileName);
      HandleBratError("coda_get_product_filename in CFieldBasic::Read(std::string& value)");

      std::string msg = CTools::Format("ERROR - CFieldBasic::Read (std::string& data): unexpected Brat special type %d (%s) -  FieldName:%s - File:%s",
                                  this->m_specialType,
                                  coda_type_get_special_type_name(this->m_specialType),
                                  m_name.c_str(),
                                  fileName);
      CUnImplementException e(msg, BRATHL_UNIMPLEMENT_ERROR);
      throw(e);
      break;
    }
  }

  switch (this->m_nativeType)
  {
    //----------------------
    case coda_native_type_string:
    //----------------------
    {
      long length = -1;
      char *data = NULL;

      coda_cursor_get_string_length(&m_cursor, &length);
      HandleBratError("coda_cursor_get_string_length in CFieldBasic::Read(std::string& value)");

      if (length <= 0)
      {
        value = "";
        break;
      }
      data = new char[length + 1];
      if (data == NULL)
      {
        coda_set_error(CODA_ERROR_OUT_OF_MEMORY,
                       "out of memory (could not allocate %lu bytes)",
                       (long)length + 1);
        HandleBratError("CFieldBasic::Read(std::string& value)");
      }

      coda_cursor_read_string(&m_cursor, data, length + 1);
      HandleBratError("coda_cursor_read_string in CFieldBasic::Read(std::string& value)");

      value = data;
      delete []data;
      break;
    }
    //----------------------
    default:
    //----------------------
    {
      coda_ProductFile *pf;
      const char *fileName;

      coda_cursor_get_product_file(&m_cursor, &pf);
      HandleBratError("coda_cursor_set_product in CFieldBasic::Read(std::string& value)");

      coda_get_product_filename(pf, &fileName);
      HandleBratError("coda_get_product_filename in CFieldBasic::Read(std::string& value)");

      std::string msg = CTools::Format("ERROR - CFieldBasic::Read(std::string& value) : unexpected Brat native type %d (%s) -  FieldName:%s - File:%s",
                                  this->m_nativeType,
                                  coda_type_get_native_type_name(this->m_nativeType),
                                  m_name.c_str(),
                                  fileName);
      CUnImplementException e(msg, BRATHL_UNIMPLEMENT_ERROR);
      throw(e);
      break;
    }
  }
  return;
}

//----------------------------------------
void CFieldBasic::PushPos()
{

}

//----------------------------------------
void CFieldBasic::PopCursor()
{

  coda_cursor_goto_parent(&m_cursor); // goto Level Record
  HandleBratError("coda_cursor_goto_parent in CFieldBasic::PopCursor");
}


//----------------------------------------
void  CFieldBasic::DumpFieldDictionary(std::ostream& fOut)
{
  CField::DumpFieldDictionary(fOut);

}

//----------------------------------------

void CFieldBasic::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CFieldBasic Object at "<< this << std::endl;
  CField::Dump(fOut);
  fOut << "==> END Dump a CFieldBasic Object at "<< this << std::endl;

  fOut << std::endl;

}



//-------------------------------------------------------------
//------------------- CFieldIndexData class --------------------
//-------------------------------------------------------------
CFieldIndexData::CFieldIndexData()
{
  Init();
}

//----------------------------------------

CFieldIndexData::CFieldIndexData(const std::string& name, const std::string& description, const std::string& unit /*= ""*/)
    : CFieldBasic(-1, name, description, unit)
{
  Init();
}


//----------------------------------------

CFieldIndexData::~CFieldIndexData()
{

}
//----------------------------------------

CFieldIndexData::CFieldIndexData(CFieldIndexData& f)
{
  Set(f);
}


//----------------------------------------
const CFieldIndexData& CFieldIndexData::operator =(CFieldIndexData& f)
{
  Set(f);

  return *this;
}

//----------------------------------------
void CFieldIndexData::Set(CFieldIndexData& f)
{
  CField::Set(f);
}

//----------------------------------------
void CFieldIndexData::Init()
{
}

//----------------------------------------

CFieldSet* CFieldIndexData::CreateFieldSet(const CField::CListField& listFields)
{
    UNUSED(listFields);

  CFieldSetDbl* fieldSet = new CFieldSetDbl(m_key);
  fieldSet->SetField(this);
  return fieldSet;
}

//----------------------------------------
void CFieldIndexData::Read(double& value)
{
  value = this->GetValue();
  m_currentPos++;
}

//----------------------------------------
double CFieldIndexData::Read()
{
  double value = this->GetValue();
  m_currentPos++;
  return value;
}


//----------------------------------------
void CFieldIndexData::Read(CDoubleArray& vect, bool skip /*= false*/)
{
    UNUSED(skip);

  vect.Insert(this->GetValue());
  m_currentPos++;
}
////----------------------------------------
//void CFieldIndexData::SetAtBeginning(bool value)
//{
//  m_atBeginning = value;
//
//  if (value)
//  {
//    m_currentPos = 0;
//  }
//};

//----------------------------------------
void CFieldIndexData::Read(double* data, bool skip /*= false*/)
{

  if (skip)
  {
    SetDefaultValue(data, m_maxPos+1);
    return;
  }

  data[m_maxPos] = this->GetValue();

}


//----------------------------------------
void CFieldIndexData::Read(std::string& value, bool skip /*= false*/)
{
  if (skip)
  {
    value = "";
    return;
  }

  value = CTools::DoubleToStr(this->GetValue(), 0);
}

//----------------------------------------
double CFieldIndexData::GetValue()
{
  return (static_cast<double>(m_currentPos) + m_offset);
}
//----------------------------------------
void CFieldIndexData::PushPos()
{

}

//----------------------------------------
void CFieldIndexData::PopCursor()
{
}


//----------------------------------------
void  CFieldIndexData::DumpFieldDictionary(std::ostream& fOut)
{
  CField::DumpFieldDictionary(fOut);

}

//----------------------------------------

void CFieldIndexData::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CFieldIndexData Object at "<< this << std::endl;
  CField::Dump(fOut);
  
  fOut << "GetValues returns " << this->GetValue() << std::endl;

  fOut << "==> END Dump a CFieldIndexData Object at "<< this << std::endl;

  fOut << std::endl;

}
//-------------------------------------------------------------
//------------------- CFieldNetCdf class --------------------
//-------------------------------------------------------------


CFieldNetCdf::CFieldNetCdf()
{
  Init();
}

//----------------------------------------
CFieldNetCdf::CFieldNetCdf(const std::string& name,
                           const std::string& description,
                           const std::string& unit,
                           int32_t netCdfId /*= NC_GLOBAL*/,
                           int32_t type /*= NC_NAT*/,
                           const CUIntArray* dimValues  /*= NULL*/,
                           const CStringArray* dimNames  /*= NULL*/,
                           const CIntArray* dimIds /*= NULL*/,
                           const CDoubleArray* values /*= NULL*/)
    : CField(name, description, unit)
{
  Init();

  SetUnit(unit);

  SetNetCdfId(netCdfId);
  SetNativeType(type);

  SetDim(dimValues);

  SetDimInfo(dimNames, dimIds, dimValues);

  SetValues(values);

}
//----------------------------------------

CFieldNetCdf::CFieldNetCdf(CFieldNetCdf& f)
{
  Init();
  Set(f);
}


//----------------------------------------

CFieldNetCdf::~CFieldNetCdf()
{
  DeleteDimIndexArray();
  DeleteValuesAsArray();
}
//----------------------------------------
const CFieldNetCdf& CFieldNetCdf::operator =(CFieldNetCdf& f)
{
  Set(f);

  return *this;
}

//----------------------------------------
CBratObject* CFieldNetCdf::Clone()
{
  return new CFieldNetCdf(*this);
}

//----------------------------------------
void CFieldNetCdf::AdjustValidMinMaxFromValues()
{
  CDoubleArray::const_iterator it;
  for (it = m_values.begin() ; it != m_values.end() ; it++)
  {
    AdjustValidMinMax(*it, m_validMin, m_validMax);
  }

}

//----------------------------------------
void CFieldNetCdf::Set(CFieldNetCdf& f)
{
  CField::Set(f);

  m_netCdfId = f.m_netCdfId;
  m_netCdfUnit = f.m_netCdfUnit;
  m_scaleFactor = f.m_scaleFactor;
  m_addOffset = f.m_addOffset;
  m_fillValue = f.m_fillValue;
  m_type = f.m_type;
  m_dimNames.Insert(f.m_dimNames);
  m_dimRanges.Insert(f.m_dimRanges);
  SetDim(f.m_nbDims, f.m_dim);
  m_dimValues.Insert(f.m_dimValues);
  m_dimIds.Insert(f.m_dimIds);
  SetValues(f.m_values);
  m_valuesAsArray = NULL;
  m_mapAttributes.Insert(f.m_mapAttributes, false);

  NewDimIndexArray(&f);


}

//----------------------------------------
void CFieldNetCdf::Init()
{
  //m_externalFile = NULL;
  m_netCdfId = NC_GLOBAL;
  m_type = NC_NAT;
  m_dimsIndexArray = NULL;
  m_dimsCountArray = NULL;

  m_scaleFactor = 1.0;
  m_addOffset = 0.0;
  m_fillValue = NC_FILL_DOUBLE;

  m_atBeginning = true;

  // Init with zero dimension
  SetDim(0, NULL);

  m_valuesAsArray = NULL;

}

//----------------------------------------
int32_t CFieldNetCdf::GetVirtualNbDims()
{
  if (m_nbDims <= 1)
  {
    return 0;
  }

  return m_nbDims - 1;
}

//----------------------------------------
std::string CFieldNetCdf::GetMostExplicitName()
{
  std::string str = m_mapAttributes.Exists(STANDARD_NAME_ATTR);

  if (str.empty())
  {
    str = m_mapAttributes.Exists(LONG_NAME_ATTR);

    if (str.empty())
    {
      str = m_name;
    }

  }

  return str;
}
//----------------------------------------
void CFieldNetCdf::SetUnit(const std::string& unit)
{

  CUnit unitTmp;
  try
  {
    unitTmp = unit;
  }
  catch (CException& e)
  {
    //std::cerr << "Warning: " << m_name << " - unknown unit '" << unit << "'" << std::endl;
    e.TypeOf();  // To avoid warning compilation
    unitTmp = "";
  }

  SetUnit(unitTmp);

}
//----------------------------------------
void CFieldNetCdf::SetUnit(const CUnit& unit)
{
  try
  {
    m_netCdfUnit = unit;
  }
  catch (CException& e)
  {
    //std::cerr << "Warning: " << m_name << " - unknown unit '" << unit << "'" << std::endl;
    e.TypeOf();  // To avoid warning compilation
    m_netCdfUnit = "";
  }

  CField::SetUnit(m_netCdfUnit.AsString(false));

}

//----------------------------------------
std::string CFieldNetCdf::GetTypeName()
{
  return CNetCDFFiles::GetTypeAsString(m_type);
}
//----------------------------------------
std::string CFieldNetCdf::GetSpecialTypeName()
{
  return CNetCDFFiles::GetTypeAsString(GetSpecialType());
}
//----------------------------------------
int32_t CFieldNetCdf::GetSpecialType()
{
  return NC_NAT;
}

//----------------------------------------
int32_t CFieldNetCdf::GetPosFromDimIndexArray()
{
  if (!HasDim())
  {
    return -1;
  }

  uint32_t pos = 0;
  if (m_nbDims == 1)
  {
    return m_dimsIndexArray[0];
  }
  for (int32_t i = 0 ; i < m_nbDims - 1 ; i++)
  {
    pos += m_dimsIndexArray[i] * m_dim[i + 1];
  }

  pos += m_dimsIndexArray[m_nbDims- 1];
  return pos;

}
//----------------------------------------
int32_t CFieldNetCdf::GetCounFromDimCountArray()
{
  uint32_t count = 1;
  for (int32_t i = 0 ; i < m_nbDims ; i++)
  {
    count *= m_dimsCountArray[i];
  }

  return count;
}
//----------------------------------------
void CFieldNetCdf::DeleteDimIndexArray()
{
  if (m_dimsIndexArray != NULL)
  {
    delete m_dimsIndexArray;
  }
  if (m_dimsCountArray != NULL)
  {
    delete m_dimsCountArray;
  }
}
//----------------------------------------
uint32_t* CFieldNetCdf::NewDimIndexArray(CFieldNetCdf* fromField /* = NULL */)
{
  DeleteDimIndexArray();

  if (HasDim())
  {
    m_dimsIndexArray = new uint32_t[m_nbDims];
    m_dimsCountArray = new uint32_t[m_nbDims];

    for (int32_t i = 0 ; i < m_nbDims ; i++)
    {
      m_dimsIndexArray[i] = AT_BEGINNING;
      m_dimsCountArray[i] = 1;
    }

    if (fromField == NULL)
    {
      return m_dimsIndexArray;
    }

    m_atBeginning = fromField->m_atBeginning;

    if ((fromField->GetDimsIndexArray() != NULL) && (fromField->GetDimsCountArray() != NULL))
    {
      for (int32_t i = 0 ; i < m_nbDims ; i++)
      {
        m_dimsIndexArray[i] = fromField->GetDimsIndexArray()[i];
        m_dimsCountArray[i] = fromField->GetDimsCountArray()[i];
      }
    }
  }

  return m_dimsIndexArray;

}

//----------------------------------------
void CFieldNetCdf::SetDimIds(const CIntMap& dimIds)
{
  m_dimIds.Insert(dimIds);
}
//----------------------------------------
void CFieldNetCdf::SetDimIds(const CIntMap* dimIds)
{
  if (dimIds != NULL)
  {
    SetDimIds(*dimIds);
  }
}


//----------------------------------------
void CFieldNetCdf::SetAttributes(const CStringMap& mapAttributes)
{
  m_mapAttributes.Insert(mapAttributes);
}
//----------------------------------------
void CFieldNetCdf::SetAttributes(const CStringMap* mapAttributes)
{
  if (mapAttributes != NULL)
  {
    SetAttributes(*mapAttributes);
  }

}

//----------------------------------------
std::string CFieldNetCdf::GetAttribute(const std::string attrName)
{
  return m_mapAttributes.Exists(attrName);
}

//----------------------------------------
void CFieldNetCdf::SetDimValues(const CUIntMap* dimValues)
{
  if (dimValues != NULL)
  {
    SetDimValues(*dimValues);
  }
}
//----------------------------------------
void CFieldNetCdf::SetDimValues(const CUIntMap& dimValues)
{
  m_dimValues.Insert(dimValues);
}

//----------------------------------------
void CFieldNetCdf::SetDimNames(const CStringArray* dimNames)
{
  if (dimNames != NULL)
  {
    SetDimNames(*dimNames);
  }
}
//----------------------------------------
void CFieldNetCdf::SetDimNames(const CStringArray& dimNames)
{
  m_dimNames.RemoveAll();

  m_dimNames.Insert(dimNames);

  m_dimRanges.RemoveAll();

  for (uint32_t i =  0  ; i < dimNames.size() ; i++)
  {
    m_dimRanges.Insert(dimNames[i], i);
  }

}

//----------------------------------------
uint32_t CFieldNetCdf::GetDimRange(const std::string& dimName)
{
  return m_dimRanges.Exists(dimName);
}

//----------------------------------------
void CFieldNetCdf::SetDimInfo(const CStringArray* dimNames, const CIntArray* dimIds, const CUIntArray* dimValues)
{
  if ((dimNames == NULL) && (dimIds == NULL) && (dimValues == NULL))
  {
    return;
  }

  if ((dimNames != NULL) && (dimIds != NULL) && (dimValues != NULL))
  {
    SetDimInfo(*dimNames, *dimIds, *dimValues);
    return;
  }

  CException e(CTools::Format("ERROR - CFieldNetCdf::SetDimInfo() - Field '%s' : At least one of the array parameter is NULL: ",
                              m_name.c_str()),
              BRATHL_LOGIC_ERROR);
  CTrace::Tracer("%s", e.what());
  Dump(*CTrace::GetDumpContext());
  throw (e);

  return; // To avoid compilation warning
}
//----------------------------------------
void CFieldNetCdf::SetDimInfo(const CStringArray& dimNames, const CIntArray& dimIds, const CUIntArray& dimValues)
{
  size_t nbNames = dimNames.size();
  size_t nbIds = dimIds.size();
  size_t nbValues = dimValues.size();

  if ((nbNames != nbIds) ||  (nbNames != nbValues))
  {
    CException e(CTools::Format("ERROR - CFieldNetCdf::SetDimInfo() - Field '%s' : Arrays have not the same size: "
                                "dimNames size=%d - dimIds size=%d - dimValues size=%d",
                                m_name.c_str(),
                                nbNames,
                                nbIds,
                                nbValues),
                BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);
  }

  SetDimNames(dimNames);

  m_dimIds.Insert(dimNames, dimIds);
  m_dimValues.Insert(dimNames, dimValues);
}


//----------------------------------------
void CFieldNetCdf::GetDimValuesAsArray(CUIntArray& values, bool bRemoveAll /*= true*/)
{
  if (bRemoveAll)
  {
    values.RemoveAll();
  }

  CStringArray::const_iterator it;

  uint32_t value;

  for (it = m_dimNames.begin() ; it != m_dimNames.end() ; it++)
  {
    value = m_dimValues.Exists(*it);

    if (isDefaultValue(value))
    {
      continue;
    }

    values.Insert(value);
  }

}
//----------------------------------------
void CFieldNetCdf::GetDimIdsAsArray(CIntArray& values, bool bRemoveAll /*= true*/)
{
  if (bRemoveAll)
  {
    values.RemoveAll();
  }

  CStringArray::const_iterator it;

  int32_t value;

  for (it = m_dimNames.begin() ; it != m_dimNames.end() ; it++)
  {
    value = m_dimIds.Exists(*it);

    if (isDefaultValue(value))
    {
      continue;
    }

    values.Insert(value);
  }

}
//----------------------------------------
CDoubleArray& CFieldNetCdf::GetValuesWithUnitConversion(const std::string& wantedUnit)
{
  CUnit* unit = GetNetCdfUnit();

  if (unit->HasNoUnit())
  {
    return m_values;
  }

  if ((wantedUnit != "") && (wantedUnit != CUnit::m_UNIT_SI))
  {
    unit->SetConversionTo(wantedUnit);
  }
  else
  {
    unit->SetConversionToBaseUnit();
  }

  m_valuesWithUnitConversion.RemoveAll();

  CDoubleArray::iterator it;

  for (it =  m_values.begin() ; it != m_values.end() ; it++)
  {
    m_valuesWithUnitConversion.Insert(unit->Convert( (*it) ));
  }

  return m_valuesWithUnitConversion;
}
//----------------------------------------
void CFieldNetCdf::SetValues(const CDoubleArray* values)
{
  if (values != NULL)
  {
    SetValues(*values);
  }
}

//----------------------------------------
void CFieldNetCdf::SetValues(const std::string& values)
{
  m_values.RemoveAll();
  m_values.Insert(values);
}

//----------------------------------------
void CFieldNetCdf::SetValues(const CDoubleArray& values)
{
  m_values.RemoveAll();
  m_values.Insert(values);
}
//----------------------------------------
void CFieldNetCdf::SetValues(double values)
{
  // Don't remove m_values here, values can be added by several call
  // m_values.RemoveAll();
  m_values.Insert(values);
}

//----------------------------------------
void CFieldNetCdf::SetValues(double* values, size_t length)
{
  // Don't remove m_values here, values can be added by several call
  // m_values.RemoveAll();
  m_values.Insert(values, length);
}

//----------------------------------------
void CFieldNetCdf::SetValues(const CInt16Array* values)
{
  if (values != NULL)
  {
    SetValues(*values);
  }
}
//----------------------------------------
void CFieldNetCdf::SetValues(const CInt16Array& values)
{

  m_values.RemoveAll();
  m_values.Insert(values);
}

//----------------------------------------
void CFieldNetCdf::SetValues(const CInt8Array* values)
{
  if (values != NULL)
  {
    SetValues(*values);
  }
}
//----------------------------------------
void CFieldNetCdf::SetValues(const CInt8Array& values)
{

  m_values.RemoveAll();
  m_values.Insert(values);
}

//----------------------------------------
void CFieldNetCdf::SetValues(const CUInt8Array* values)
{
  if (values != NULL)
  {
    SetValues(*values);
  }
}
//----------------------------------------
void CFieldNetCdf::SetValues(const CUInt8Array& values)
{

  m_values.RemoveAll();
  m_values.Insert(values);
}
//----------------------------------------
void CFieldNetCdf::SetValues(const CIntArray* values)
{
  if (values != NULL)
  {
    SetValues(*values);
  }
}
//----------------------------------------
void CFieldNetCdf::SetValues(const CIntArray& values)
{

  m_values.RemoveAll();
  m_values.Insert(values);
}

//----------------------------------------
void CFieldNetCdf::SetValues(const CFloatArray* values)
{
  if (values != NULL)
  {
    SetValues(*values);
  }
}

//----------------------------------------
void CFieldNetCdf::SetValues(const CFloatArray& values)
{

  m_values.RemoveAll();
  m_values.Insert(values);
}


//----------------------------------------
void CFieldNetCdf::PushPos()
{
}
//----------------------------------------
void CFieldNetCdf::PopCursor()
{

}
//----------------------------------------
bool CFieldNetCdf::IsAtBeginning()
{
  if (HasDim())
  {
    if (m_dimsIndexArray == NULL)
    {
      if (NewDimIndexArray() == NULL)
      {
        CException e(CTools::Format("ERROR - CFieldNetCdf::IsAtBeginning() - Field '%s' has no dimension",
                                    m_name.c_str()),
                    BRATHL_LOGIC_ERROR);
        CTrace::Tracer("%s", e.what());
        Dump(*CTrace::GetDumpContext());
        throw (e);
      }
    }
  }

  return (m_atBeginning);
}

//----------------------------------------
void CFieldNetCdf::InitDimIndexes (uint32_t value)
{
  if (value == AT_BEGINNING)
  {
    m_atBeginning = true;
  }
  else
  {
    m_atBeginning = false;
  }

  if (!HasDim())
  {
    return;
  }

  if (m_dimsIndexArray == NULL)
  {
    if (NewDimIndexArray() == NULL)
    {
      return;
    }
  }

  for (int32_t i = 0 ; i < m_nbDims ; i++)
  {
    m_dimsIndexArray[i] = value;
  }
}


//----------------------------------------
void CFieldNetCdf::InitDimsIndexToMax()
{
  for (int32_t i = 0 ; i < m_nbDims ; i++)
  {
    //std::string dimName = m_dimNames.at(i);
    //m_dimsIndexArray[i] = m_dimValues.Exists(dimName) - 1;
    InitDimsIndexToMax(i);
  }
}
//----------------------------------------
void CFieldNetCdf::InitDimsIndexToMax(uint32_t index)
{
  std::string dimName = m_dimNames.at(index);
  m_dimsIndexArray[index] = m_dimValues.Exists(dimName) - 1;
}

//----------------------------------------
bool CFieldNetCdf::NextIndex()
{

  bool result = true;

  if (IsAtBeginning())
  {
    InitDimIndexes(0);
    return result;
  }

  if (!HasDim())
  {
    return result;
  }


  for (int32_t i = m_nbDims - 1 ; i >= 0 ; i--)
  {
    std::string dimName = m_dimNames.at(i);

    uint32_t currentIndex = m_dimsIndexArray[i];
    uint32_t count = m_dimsCountArray[i];

    uint32_t maxIndex = m_dimValues.Exists(dimName);

    currentIndex += count;

    m_dimsIndexArray[i] = currentIndex;

    if (currentIndex >= maxIndex)
    {
      if (i == 0) // end
      {
        InitDimsIndexToMax();
        result = false;
      }
      else
      {
        currentIndex = 0;
        m_dimsIndexArray[i] = currentIndex;
      }
    }
    else
    {
      break;
    }

  }

  return result;
}


//----------------------------------------
bool CFieldNetCdf::PrevIndex()
{

  bool result = true;

  if (IsAtBeginning())
  {
    InitDimIndexes(AT_BEGINNING);
    return result;
  }

  if (!HasDim())
  {
    return result;
  }


  for (int32_t i = m_nbDims - 1 ; i >= 0 ; i--)
  {
    std::string dimName = m_dimNames.at(i);

    uint32_t currentIndex = m_dimsIndexArray[i];
    uint32_t count = m_dimsCountArray[i];

    //uint32_t maxIndex = m_dimValues.Exists(dimName);

    if ((static_cast<int32_t>(currentIndex) - static_cast<int32_t>(count)) < 0)
    {
      currentIndex = AT_BEGINNING;
    }
    else
    {
      currentIndex -= count;
    }

    m_dimsIndexArray[i] = currentIndex;

    if (currentIndex == AT_BEGINNING)
    {
      if (i == 0) // end
      {
        InitDimIndexes(AT_BEGINNING);
        result = false;
      }
      else
      {
        InitDimsIndexToMax(i);
      }
    }
    else
    {
      break;
    }

  }

  return result;
}
//----------------------------------------
void CFieldNetCdf::SetIndex(const std::string& dimName, uint32_t index, uint32_t count)
{
  if (index == AT_BEGINNING)
  {
    m_atBeginning = true;
  }
  else
  {
    m_atBeginning = false;
  }


  if (!HasDim())
  {
    return;
  }

  uint32_t range = m_dimRanges.Exists(dimName);

  if (isDefaultValue(range))
  {
    CException e(CTools::Format("ERROR - CFieldNetCdf::SetIndex(const std::string& dimNames, uint32_t index, uint32_t count) - "
                                "dimension name not found : '%s'"
                                " - Field name '%s'",
                                dimName.c_str(),
                                m_name.c_str()),
                BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);
  }

  if (int32_t(range) >= m_nbDims)
  {
    CException e(CTools::Format("ERROR - CFieldNetCdf::SetIndex(const std::string& dimNames, uint32_t index, uint32_t count) - "
                                "range %d >= number of dimensions %d"
                                " - Field name '%s' - Dimension name '%s' ",
                                range,
                                m_nbDims,
                                m_name.c_str(),
                                dimName.c_str()),
                BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);
  }

  if (int32_t(index) >= m_dim[range])
  {
    CException e(CTools::Format("ERROR - CFieldNetCdf::SetIndex(const std::string& dimNames, uint32_t index, uint32_t count) - "
                                "dimension index  %d >= max dimension index %d"
                                " - Field name '%s' - Dimension name '%s' - Dimension range %ld",
                                range,
                                m_nbDims,
                                m_name.c_str(),
                                dimName.c_str(),
                                m_dim[range]),
                BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);
  }

  if (int32_t(count) > m_dim[range])
  {
    CException e(CTools::Format("ERROR - CFieldNetCdf::SetIndex(const std::string& dimNames, uint32_t index, uint32_t count) - "
                                "dimension counter  %d > max dimension index %d"
                                " - Field name '%s' - Dimension name '%s' - Dimension range %ld",
                                range,
                                m_nbDims,
                                m_name.c_str(),
                                dimName.c_str(),
                                m_dim[range]),
                BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);
  }

  m_dimsIndexArray[range] = index;

  m_dimsCountArray[range] = count;
}
//----------------------------------------
CFieldSet* CFieldNetCdf::CreateFieldSet(const CField::CListField& listFields)
{
    UNUSED(listFields);

  CException e("ERROR - CFieldNetCdf::CreateFieldSet(const CField::CListField& listFields)  - unexpected call - CFieldNetCdf::CreateFieldSet(const CField::CListField& listFields)  have not to be called ", BRATHL_LOGIC_ERROR);
  throw (e);

/*
  CFieldSetArrayDbl* fieldSet = new CFieldSetArrayDbl(m_key);
  fieldSet->SetField(this);
  fieldSet->m_nbDims = listFields.m_nbFieldSetDims;
  fieldSet->m_dim = listFields.m_fieldSetDim;
  return fieldSet;
*/
}
//--------------------------------------
/*
CFieldSet* CFieldNetCdf::CreateFieldSet(const CUIntArray& dims)
{
  CFieldSetArrayDbl* fieldSet = new CFieldSetArrayDbl(m_key);
  fieldSet->SetField(this);

  int32_t nbDims = dims.size();

  if (nbDims <= 0)
  {
    CException e(CTools::Format("ERROR - CFieldNetCdf::CreateFieldSet(const CUIntArray& dims) - number of dimensions (%d) is  negative",
                                nbDims),
                BRATHL_LOGIC_ERROR);
    CTrace::Tracer(e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);
  }

  fieldSet->m_nbDims = nbDims;
  //CUIntArray dims;
  //GetDimAsVector(dims);

  fieldSet->m_dim.Insert(dims);
  return fieldSet;
}
*/
//----------------------------------------
CFieldSet* CFieldNetCdf::CreateFieldSet()
{
  //CFieldSetDbl* fieldSet = new CFieldSetDbl(m_key);
  CFieldSet* fieldSet = NULL;
  bool isArray = true;

  if (m_dimIds.size() <= 0)
  {
    isArray = false;
  }
  else if (m_dimIds.size() == 1)
  {
    if (m_dimsCountArray[0] <= 1)
    {
      isArray = false;
    }
  }

  if (isArray)
  {
    CFieldSetArrayDbl* fieldSetArrayDbl = new CFieldSetArrayDbl(m_key);

    fieldSetArrayDbl->m_nbDims = m_dimIds.size();
    fieldSetArrayDbl->m_dim.Insert(m_dimsCountArray, fieldSetArrayDbl->m_nbDims);

    fieldSet = fieldSetArrayDbl;
  }
  else
  {
    fieldSet = new CFieldSetDbl(m_key);
  }

  fieldSet->SetField(this);
  return fieldSet;

}

//----------------------------------------
std::string CFieldNetCdf::GetRecordName()
{
  if (m_recordName.empty() == false)
  {
    return m_recordName;
  }

  m_recordName = CProductNetCdf::m_virtualRecordName;

  return m_recordName;
}
//----------------------------------------
//virtual
std::string CFieldNetCdf::GetFullName() const
{

  if (m_fullName.empty() == false)
  {
    return m_fullName;
  }

  CStringArray strArray;

  strArray.ExtractStrings(m_key, CTreeField::m_keyDelimiter);

  for (uint32_t i = 1 ; i < strArray.size() ; i++)
  {
    m_fullName += strArray[i] + CTreeField::m_keyDelimiter;
  }

  if (m_fullName.empty() == false)
  {
    m_fullName.erase(m_fullName.length() - 1, 1);
  }

  return m_fullName;
}
//----------------------------------------
//virtual
std::string CFieldNetCdf::GetFullNameWithRecord()
{
  return GetFullName();
}
//----------------------------------------

NetCDFVarKind CFieldNetCdf::SearchDimKind()
{
  if (m_netCdfUnit.IsDate())
  {
    return T;
  }

  std::string str = GetAttribute(STANDARD_NAME_ATTR);

  NetCDFVarKind dimKind = CNetCDFFiles::GetVarKindFromStringValue(str);

  if (dimKind != Unknown)
  {
    return dimKind;
  }

  str = GetAttribute(LONG_NAME_ATTR);
  dimKind = CNetCDFFiles::GetVarKindFromStringValue(str);

  if (dimKind != Unknown)
  {
    return dimKind;
  }

  dimKind = CNetCDFFiles::GetVarKindFromStringValue(m_name);

  return dimKind;
}
//----------------------------------------
void CFieldNetCdf::Read(CDoubleArray& vect, bool skip /*= false*/)
{
    UNUSED(vect);   UNUSED(skip);

  CException e("ERROR - CFieldNetCdf::Read(CDoubleArray& vect) - unexpected call - CFieldNetCdf::Read(CDoubleArray& vect) have not to be called ", BRATHL_LOGIC_ERROR);
  CTrace::Tracer("%s", e.what());
  Dump(*CTrace::GetDumpContext());
  throw (e);

}
//----------------------------------------
void CFieldNetCdf::Read(CExpressionValue& value, bool skip /*= false*/)
{
    UNUSED(value);   UNUSED(skip);

  CException e("ERROR - CFieldNetCdf::Read(CExpressionValue& vect) - unexpected call - CFieldNetCdf::Read(CExpressionValue& vect) have not to be called ", BRATHL_LOGIC_ERROR);
  CTrace::Tracer("%s", e.what());
  Dump(*CTrace::GetDumpContext());
  throw (e);
}

//----------------------------------------
void CFieldNetCdf::DumpFieldDictionary(std::ostream& fOut)
{
  CField::DumpFieldDictionary(fOut);

}

//----------------------------------------
void CFieldNetCdf::DeleteValuesAsArray()
{
  if (m_valuesAsArray != NULL)
  {
    delete []m_valuesAsArray;
    m_valuesAsArray = NULL;
  }
}

//----------------------------------------
void CFieldNetCdf::EmptyValues()
{
  m_values.RemoveAll();
}
//----------------------------------------
void CFieldNetCdf::SetValuesAsArray()
{
  SetValuesAsArray(m_values);
}

//----------------------------------------
void CFieldNetCdf::SetValuesAsArray(const CDoubleArray& values)
{
    UNUSED(values);

  DeleteValuesAsArray();

  m_valuesAsArray = new double[m_values.size()];

  std::copy(m_values.begin(), m_values.end(), m_valuesAsArray);
}

//----------------------------------------
void CFieldNetCdf::SetValuesAsArray(const CDoubleArray* values)
{
  SetValuesAsArray(*values);
}

//----------------------------------------

void CFieldNetCdf::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CFieldNetCdf Object at "<< this << std::endl;
  CField::Dump(fOut);

  fOut << "m_type: " << m_type << std::endl;
  fOut << "m_netCdfUnit: "  << std::endl;
  m_netCdfUnit.Dump(fOut);
  fOut << "m_scaleFactor: " << m_scaleFactor << std::endl;
  fOut << "m_addOffset: " << m_addOffset << std::endl;
  fOut << "m_fillValue: " << m_fillValue << std::endl;
  fOut << "m_netCdfId: " << m_netCdfId << std::endl;
  fOut << "m_type: " << m_type << std::endl;
  fOut << "m_dimNames: "  << std::endl;
  m_dimNames.Dump(fOut);
  fOut << "m_dimValues: "  << std::endl;
  m_dimValues.Dump(fOut);
  fOut << "m_dimIds: "  << std::endl;
  m_dimIds.Dump(fOut);
  fOut << "m_dimRanges: "  << std::endl;
  m_dimRanges.Dump(fOut);
  fOut << "m_valuesWithUnitConversion: "  << std::endl;
  m_valuesWithUnitConversion.Dump(fOut);
  fOut << "m_values: "  << std::endl;
  m_values.Dump(fOut);
  fOut << "m_mapAttributes: "  << std::endl;
  m_mapAttributes.Dump(fOut);
  if (m_dimsIndexArray == NULL)
  {
    fOut << "m_dimsIndexArray: NULL"  << std::endl;
  }
  else
  {
    fOut << "m_dimsIndexArray: "  << std::endl;
    for (int32_t i = 0 ; i < m_nbDims ; i++)
    {
      fOut << "m_dimsIndexArray[" << i << "]=" <<  m_dimsIndexArray[i] << std::endl;
    }
  }
  if (m_dimsCountArray == NULL)
  {
    fOut << "m_dimsCountArray: NULL"  << std::endl;
  }
  else
  {
    fOut << "m_dimsCountArray: "  << std::endl;
    for (int32_t i = 0 ; i < m_nbDims ; i++)
    {
      fOut << "m_dimsCountArray[" << i << "]=" <<  m_dimsCountArray[i] << std::endl;
    }
  }
  fOut << "m_atBeginning: " << m_atBeginning << std::endl;

  fOut << "==> END Dump a CFieldNetCdf Object at "<< this << std::endl;

  fOut << std::endl;

}



//-------------------------------------------------------------
//------------------- CFieldNetCdfCF class --------------------
//-------------------------------------------------------------


CFieldNetCdfCF::CFieldNetCdfCF()
{
  Init();
}

//----------------------------------------
CFieldNetCdfCF::CFieldNetCdfCF(const std::string& name,
                           const std::string& description,
                           const std::string& unit,
                           int32_t netCdfId /*= NC_GLOBAL*/,
                           int32_t type /*= NC_NAT*/,
                           const CUIntArray* dimValues  /*= NULL*/,
                           const CStringArray* dimNames  /*= NULL*/,
                           const CIntArray* dimIds /*= NULL*/,
                           const CDoubleArray* values /*= NULL*/)
    : CFieldNetCdf(name, description, unit, netCdfId, type, dimValues, dimNames, dimIds, values)
{
  Init();
}

//----------------------------------------

void CFieldNetCdfCF::Init()
{
}
//----------------------------------------

CFieldNetCdfCF::~CFieldNetCdfCF()
{

}

//----------------------------------------
CFieldNetCdfCF::CFieldNetCdfCF(CFieldNetCdfCF& f)
{
  Init();
  Set(f);
}


//----------------------------------------
const CFieldNetCdfCF& CFieldNetCdfCF::operator =(CFieldNetCdfCF& f)
{
  Set(f);

  return *this;
}

//----------------------------------------
CBratObject* CFieldNetCdfCF::Clone()
{
  return new CFieldNetCdfCF(*this);
}
//----------------------------------------
void CFieldNetCdfCF::Set(CFieldNetCdfCF& f)
{
  CFieldNetCdf::Set(f);

}

//----------------------------------------
std::string CFieldNetCdfCF::GetDimAsString()
{
  std::string dims;

  if ((m_dimValues.size() <= 0) || (int32_t(m_dimValues.size()) != m_nbDims))
  {
    dims = GetDimAsStringWithIndexes();
  }
  else
  {
    dims = GetDimAsStringWithNames();
  }
  return dims;
}
//----------------------------------------
std::string CFieldNetCdfCF::GetDimAsStringWithIndexes()
{
  return CFieldNetCdf::GetDimAsString();
}
//----------------------------------------
std::string CFieldNetCdfCF::GetDimAsStringWithNames()
{
  std::string dims;
  for (int32_t i = 0 ; i < m_nbDims ; i++)
  {
    dims += CTools::Format(128,  "[%s]=%ld ", m_dimNames.at(i).c_str(), m_dim[i]);
  }
  return dims;
}


//----------------------------------------
void CFieldNetCdfCF::DumpFieldDictionary(std::ostream& fOut)
{
  CFieldNetCdf::DumpFieldDictionary(fOut);

}

//----------------------------------------

void CFieldNetCdfCF::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CFieldNetCdfCF Object at "<< this << std::endl;
  CFieldNetCdf::Dump(fOut);
  fOut << "==> END Dump a CFieldNetCdfCF Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CFieldNetCdfIndexData class --------------------
//-------------------------------------------------------------


CFieldNetCdfIndexData::CFieldNetCdfIndexData()
{
  Init();
}

//----------------------------------------
CFieldNetCdfIndexData::CFieldNetCdfIndexData(const std::string& name,
                           const std::string& description,
                           const std::string& unit,
                           int32_t netCdfId /*= NC_GLOBAL*/,
                           int32_t type /*= NC_NAT*/,
                           const CUIntArray* dimValues  /*= NULL*/,
                           const CStringArray* dimNames  /*= NULL*/,
                           const CIntArray* dimIds /*= NULL*/,
                           const CDoubleArray* values /*= NULL*/)
    : CFieldNetCdfCF(name, description, unit, netCdfId, type, dimValues, dimNames, dimIds, values)
{
  Init();
}

//----------------------------------------

void CFieldNetCdfIndexData::Init()
{
  m_offset = 0.0;
}
//----------------------------------------

CFieldNetCdfIndexData::~CFieldNetCdfIndexData()
{

}

//----------------------------------------
CFieldNetCdfIndexData::CFieldNetCdfIndexData(CFieldNetCdfIndexData& f)
{
  Init();
  Set(f);
}


//----------------------------------------
CFieldNetCdfIndexData::CFieldNetCdfIndexData(CNetCDFVarDef& netCdfVarDef)
{
  Init();
  Set(netCdfVarDef);
}

//----------------------------------------
const CFieldNetCdfIndexData& CFieldNetCdfIndexData::operator =(CFieldNetCdfIndexData& f)
{
  Set(f);

  return *this;
}

//----------------------------------------
const CFieldNetCdfIndexData& CFieldNetCdfIndexData::operator =(CNetCDFVarDef& netCdfVarDef)
{
  Set(netCdfVarDef);

  return *this;
}
//----------------------------------------
CBratObject* CFieldNetCdfIndexData::Clone()
{
  return new CFieldNetCdfIndexData(*this);
}
//----------------------------------------
void CFieldNetCdfIndexData::Set(CFieldNetCdfIndexData& f)
{
  CFieldNetCdfCF::Set(f);
  
  SetOffset(f.m_offset);

}
//----------------------------------------
void CFieldNetCdfIndexData::Set(CNetCDFVarDef& netCdfVarDef)
{
  CStringArray dimNames;
  netCdfVarDef.GetNetCdfDimNames(dimNames);

  CIntArray dimIds;
  netCdfVarDef.GetNetCdfDimIds(dimIds);

  CFieldNetCdfIndexData field(netCdfVarDef.GetName(),
                       netCdfVarDef.GetLongName(),
                       "count",
                       NC_GLOBAL,
                       NC_DOUBLE,
                       netCdfVarDef.GetDims(),
                       &dimNames,
                       &dimIds);

  Set(field);

}

//----------------------------------------
bool CFieldNetCdfIndexData::operator== ( const CFieldNetCdfIndexData& f ) const 
{
  return (this->m_dimIds == f.m_dimIds);
}

//----------------------------------------
void CFieldNetCdfIndexData::SetOffset(double value, bool force /*= false*/)
{
  SetValuesWithOffset(value, force);
}

//----------------------------------------
void CFieldNetCdfIndexData::AddOffset(double value, bool force /*= false*/)
{
  double offset = CField::GetOffset() + value;
  SetValuesWithOffset(offset, force);
}
//----------------------------------------
void CFieldNetCdfIndexData::SetValuesWithOffset(double newOffset, bool force /*= false*/)
{
  size_t valuesSize = m_values.size();
  if (!force && (static_cast<int>(valuesSize) == this->GetNbElts()) && (m_offset == newOffset))
  {
    return;
  }

  CField::SetOffset(newOffset);

  m_values.resize(this->GetNbElts());

  for (uint32_t i = 0 ; i < m_values.size() ; i++)
  {
    m_values[i] = static_cast<double>(i) + m_offset; 
  }

}

//----------------------------------------
void CFieldNetCdfIndexData::DumpFieldDictionary(std::ostream& fOut)
{
  CFieldNetCdfCF::DumpFieldDictionary(fOut);

}

//----------------------------------------

void CFieldNetCdfIndexData::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CFieldNetCdfIndexData Object at "<< this << std::endl;
  CFieldNetCdfCF::Dump(fOut);
  fOut << "==> END Dump a CFieldNetCdfIndexData Object at "<< this << std::endl;

  fOut << std::endl;

}


//-------------------------------------------------------------
//------------------- CFieldNetCdfCFAttr class --------------------
//-------------------------------------------------------------


CFieldNetCdfCFAttr::CFieldNetCdfCFAttr()
{
  Init();
}

//----------------------------------------
CFieldNetCdfCFAttr::CFieldNetCdfCFAttr(CNetCDFVarDef* netCDFVarDef,
                                       CNetCDFAttr* netCDFAttr)
{
  Init();

  SetNetCDFAttr(netCDFAttr);

  std::string attrFullName;

  attrFullName.append(netCDFVarDef->GetName());
  attrFullName.append("_");
  attrFullName.append(netCDFAttr->GetName());

  SetName(attrFullName);

  SetDescription(CTools::Format("'%s' attribute of the '%s' field", netCDFAttr->GetName().c_str(), netCDFVarDef->GetName().c_str()));

  SetNetCdfId(netCDFVarDef->GetVarId());

  SetInfoFromAttr(netCDFAttr, netCDFVarDef);
  SetValuesFromAttr(netCDFAttr);

  SetRelatedVarName(netCDFVarDef->GetName());
}

//----------------------------------------
CFieldNetCdfCFAttr::CFieldNetCdfCFAttr(CNetCDFAttr* netCDFAttr)
{
  Init();

  SetNetCDFAttr(netCDFAttr);

  SetName(netCDFAttr->GetName());

  SetDescription(CTools::Format("'%s' global attribute", netCDFAttr->GetName().c_str()));

  SetNetCdfId(NC_GLOBAL);

  SetInfoFromAttr(netCDFAttr);
  SetValuesFromAttr(netCDFAttr);


}
//----------------------------------------

CFieldNetCdfCFAttr::CFieldNetCdfCFAttr(CFieldNetCdfCFAttr& f)
{
  Init();
  Set(f);
}


//----------------------------------------

CFieldNetCdfCFAttr::~CFieldNetCdfCFAttr()
{
  DeleteNetCDFAttr();
}

//----------------------------------------
void CFieldNetCdfCFAttr::Init()
{
  m_netCDFAttr = NULL;
}


//----------------------------------------
const CFieldNetCdfCFAttr& CFieldNetCdfCFAttr::operator =(CFieldNetCdfCFAttr& f)
{
  Set(f);

  return *this;
}

//----------------------------------------
CBratObject* CFieldNetCdfCFAttr::Clone()
{
  return new CFieldNetCdfCFAttr(*this);
}


//----------------------------------------
void CFieldNetCdfCFAttr::Set(CFieldNetCdfCFAttr& f)
{
  CFieldNetCdfCF::Set(f);
  m_netCDFAttr = f.GetNetCDFAttr()->CloneThis();
  m_relatedVarName= f.m_relatedVarName;
}


//----------------------------------------
void CFieldNetCdfCFAttr::DeleteNetCDFAttr()
{
  if (m_netCDFAttr != NULL)
  {
    delete m_netCDFAttr;
    m_netCDFAttr = NULL;
  }

}
//----------------------------------------
void CFieldNetCdfCFAttr::SetNetCDFAttr(CNetCDFAttr* value)
{
  DeleteNetCDFAttr();

  m_netCDFAttr = value->CloneThis();
}
//----------------------------------------
void CFieldNetCdfCFAttr::SetInfoFromAttr(CNetCDFVarDef* netCDFVarDef /*= NULL*/)
{
  SetInfoFromAttr(m_netCDFAttr, netCDFVarDef);
}
//----------------------------------------
void CFieldNetCdfCFAttr::SetInfoFromAttr(CNetCDFAttr* netCDFAttr, CNetCDFVarDef* netCDFVarDef /*= NULL*/)
{
  if (netCDFAttr == NULL)
  {
    return;
  }

  // If attribute value is a std::string date representation then
  // - set unit
  // - change data type
  if (netCDFAttr->IsValueDate())
  {
    SetUnit(CNetCDFAttrString::m_DATE_UNIT);
    SetNativeType(NC_DOUBLE);
  }
  else
  {
    if (netCDFVarDef != NULL)
    {
      //SetUnit(netCDFVarDef->GetUnit()->AsString(false));
    }

    SetNativeType(netCDFAttr->GetType());
  }

}
//----------------------------------------
void CFieldNetCdfCFAttr::SetValuesFromAttr()
{
  SetValuesFromAttr(m_netCDFAttr);
}

//----------------------------------------
void CFieldNetCdfCFAttr::SetValuesFromAttr(CNetCDFAttr* netCDFAttr)
{
  if (netCDFAttr == NULL)
  {
    return;
  }

  // If attribute value is a std::string date representation then
  // - set value from double data
  if (netCDFAttr->IsValueDate())
  {
    CNetCDFAttrString* netCDFAttrString = CNetCDFAttr::GetNetCDFAttrString(netCDFAttr, true);

    SetValues(netCDFAttrString->GetValueDateAsDouble());
    return;
  }


  // Warning: non-numeric attributes are excluded

  CNetCDFAttrDouble* netCDFAttrDouble = CNetCDFAttr::GetNetCDFAttrDouble(netCDFAttr);
  if (netCDFAttrDouble != NULL)
  {
    SetValues(netCDFAttrDouble->GetValue());
    return;
  }

  CNetCDFAttrFloat* netCDFAttrFloat = CNetCDFAttr::GetNetCDFAttrFloat(netCDFAttr);
  if (netCDFAttrFloat != NULL)
  {
    SetValues(netCDFAttrFloat->GetValue());
    return;
  }

  CNetCDFAttrShort* netCDFAttrShort = CNetCDFAttr::GetNetCDFAttrShort(netCDFAttr);
  if (netCDFAttrShort != NULL)
  {
    SetValues(netCDFAttrShort->GetValue());
    return;
  }

  CNetCDFAttrString* netCDFAttrString = CNetCDFAttr::GetNetCDFAttrString(netCDFAttr);
  if (netCDFAttrString != NULL)
  {
    SetValues(netCDFAttrString->GetValue());
    return;
  }

  CNetCDFAttrInt* netCDFAttrInt = CNetCDFAttr::GetNetCDFAttrInt(netCDFAttr);
  if (netCDFAttrInt != NULL)
  {
    SetValues(netCDFAttrInt->GetValue());
    return;
  }

  CNetCDFAttrByte* netCDFAttrByte = CNetCDFAttr::GetNetCDFAttrByte(netCDFAttr);
  if (netCDFAttrByte != NULL)
  {
    SetValues(netCDFAttrByte->GetValue());
    return;
  }

  CNetCDFAttrUByte* netCDFAttrUByte = CNetCDFAttr::GetNetCDFAttrUByte(netCDFAttr);
  if (netCDFAttrUByte != NULL)
  {
    SetValues(netCDFAttrUByte->GetValue());
    return;
  }



}


//----------------------------------------
bool CFieldNetCdfCFAttr::IsFieldNetCdfCFAttrGlobal()
{
  return (GetRelatedVarName().empty());

}
//----------------------------------------
bool CFieldNetCdfCFAttr::IsFieldNetCdfCFAttrVariable()
{
  return !IsFieldNetCdfCFAttrGlobal();
}
//----------------------------------------
bool CFieldNetCdfCFAttr::IsFieldNetCdfCFAttrGlobal(CBratObject* ob)
{
  CFieldNetCdfCFAttr* fieldNetCdfCFAttr = CField::GetFieldNetCdfCFAttr(ob, false);

  if (fieldNetCdfCFAttr == NULL)
  {
    return false;
  }

  return fieldNetCdfCFAttr->IsFieldNetCdfCFAttrGlobal();

}
//----------------------------------------
bool CFieldNetCdfCFAttr::IsFieldNetCdfCFAttrVariable(CBratObject* ob)
{
  CFieldNetCdfCFAttr* fieldNetCdfCFAttr = CField::GetFieldNetCdfCFAttr(ob, false);

  if (fieldNetCdfCFAttr == NULL)
  {
    return false;
  }

  return fieldNetCdfCFAttr->IsFieldNetCdfCFAttrVariable();

}

//----------------------------------------
std::string CFieldNetCdfCFAttr::GetMostExplicitName()
{
  return m_name;
}

//----------------------------------------
void CFieldNetCdfCFAttr::SetAttributes(const CStringMap& mapAttributes)
{
    UNUSED(mapAttributes);
}
//----------------------------------------
void CFieldNetCdfCFAttr::SetAttributes(const CStringMap* mapAttributes)
{
    UNUSED(mapAttributes);
}

//----------------------------------------
void CFieldNetCdfCFAttr::DumpFieldDictionary(std::ostream& fOut)
{
  CFieldNetCdfCF::DumpFieldDictionary(fOut);

  m_netCDFAttr->Dump(fOut);

}

//----------------------------------------

void CFieldNetCdfCFAttr::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CFieldNetCdfCFAttr Object at "<< this << std::endl;
  CFieldNetCdfCF::Dump(fOut);
  m_netCDFAttr->Dump(fOut);
  fOut << "==> END Dump a CFieldNetCdfCFAttr Object at "<< this << std::endl;

  fOut << std::endl;

}




//-------------------------------------------------------------
//------------------- CFieldIndex class --------------------
//-------------------------------------------------------------


CFieldIndex::CFieldIndex()
{
  Init();
}

//----------------------------------------

CFieldIndex::CFieldIndex(const std::string& name, int32_t nbElts, const std::string& description, const std::string& unit)
    : CField(name, description, unit)
{
  Init();
  SetDim(nbElts);
}


//----------------------------------------

CFieldIndex::~CFieldIndex()
{

}
//----------------------------------------
CFieldIndex::CFieldIndex(CFieldIndex& f)
{
  Set(f);
}


//----------------------------------------
const CFieldIndex& CFieldIndex::operator =(CFieldIndex& f)
{
  Set(f);

  return *this;
}

//----------------------------------------
void CFieldIndex::Set(CFieldIndex& f)
{
  CField::Set(f);

}

//----------------------------------------
void CFieldIndex::Init()
{
  m_virtualField = true;

  m_atBeginning = true;
}
//----------------------------------------
void CFieldIndex::PushPos()
{
}
//----------------------------------------
void CFieldIndex::PopCursor()
{

}

//----------------------------------------
void CFieldIndex::SetDim(int32_t nbElts)
{
  CField::SetDim(nbElts);

  m_values.resize(nbElts);

  for (int32_t i = 0 ; i < nbElts ; i++)
  {
    m_values[i] = i + 1;
  }

}

//----------------------------------------

CFieldSet* CFieldIndex::CreateFieldSet()
{
  CFieldSetDbl* fieldSet = new CFieldSetDbl(m_key);
  fieldSet->SetField(this);
  return fieldSet;
}
//----------------------------------------
CFieldSet* CFieldIndex::CreateFieldSet(const CField::CListField& listFields)
{
    UNUSED(listFields);

  return CreateFieldSet();
}

//----------------------------------------
void CFieldIndex::SetAtBeginning(bool value)
{
  m_atBeginning = value;

  if (value)
  {
    m_currentPos = 0;
  }
};

//----------------------------------------
void CFieldIndex::Read(CDoubleArray& vect, bool skip /*= false*/ /* NOT USED */)
{
    UNUSED(skip);

  if (m_expandArray)
  {
    ReadOneValue(vect);
  }
  else
  {
    ReadAllValues(vect);
  }
}
//----------------------------------------
void CFieldIndex::ReadOneValue(CDoubleArray& vect)
{
  if ( End() )
  {
    m_currentPos = 0;
    return;
  }

  vect.Insert(m_values.at(m_currentPos));
  m_currentPos++;
}

//----------------------------------------
void CFieldIndex::ReadAllValues(CDoubleArray& vect)
{
  if ( End() )
  {
    m_currentPos = 0;
    return;
  }

  int32_t nbElts = GetNbElts();

  for (int32_t i = 0 ; i < nbElts ; i++)
  {
    vect.Insert(i);
  }
}

//----------------------------------------
void  CFieldIndex::DumpFieldDictionary(std::ostream& fOut)
{
  CField::DumpFieldDictionary(fOut);

}

//----------------------------------------

void CFieldIndex::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CFieldIndex Object at "<< this << std::endl;
  CField::Dump(fOut);
  fOut << "==> END Dump a CFieldIndex Object at "<< this << std::endl;

  fOut << std::endl;

}


//-------------------------------------------------------------
//------------------- CListField class --------------------
//-------------------------------------------------------------


CField* CField::CListField::Back(bool withExcept)
{

  if (this->empty())
  {
    if (withExcept)
    {
      CException e("ERROR in CField::CListField::Back - List is empty", BRATHL_LOGIC_ERROR);
      throw(e);
    }
    else
    {
      return NULL;
    }
  }

  CField* p = static_cast<CField *>(CObList::back());
  if ((p == NULL) && (withExcept))
  {
    CException e("ERROR in CField::CListField::Back() returns NULL pointer - List seems to be empty", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  return p;
}

//----------------------------------------

CField* CField::CListField::Front(bool withExcept)
{
  if (this->empty())
  {
    if (withExcept)
    {
      CException e("ERROR in CField::CListField::Front() - List is empty", BRATHL_LOGIC_ERROR);
      throw(e);
    }
    else
    {
      return NULL;
    }
  }

  CField* p = static_cast<CField *>(CObList::front());
  if ((p == NULL) && (withExcept))
  {
    CException e("ERROR in CField::CListField::Front() returns NULL pointer - List seems to be empty", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  return p;
}

//----------------------------------------

void  CField::CListField::InsertField(CField* field, bool hasDataset /*= true*/, bool bEnd /*= true*/)
{
  //ignore first field which is the the Brat dataset if hasDataset = true
  // so if the list is empty, field is the Brat dataset
  bool ignoreField = false;
  if ((hasDataset) && (this->empty()))
  {
    ignoreField = true;
  }

  if (ignoreField == false)
  {
    if ( (field->GetTypeClass() == coda_array_class) ||
         (typeid(*field) == typeid(CFieldNetCdf)) )

    {
      CUIntArray dim;
      field->GetDimAsVector(dim);
      m_fieldSetDim.Insert(dim);
      m_nbFieldSetDims = m_fieldSetDim.size();
    }

  }

  CObList::Insert(field, bEnd);

}

//----------------------------------------

void  CField::CListField::RemoveAll()
{

  m_fieldSetDim.clear();
  m_nbFieldSetDims = m_fieldSetDim.size();

  CObList::RemoveAll();

}


//-------------------------------------------------------------
//------------------- CFieldSet class --------------------
//-------------------------------------------------------------

CFieldSet::CFieldSet(const std::string& name /* = ""*/)
{
  Init();
  m_name = name;
}
//----------------------------------------
CFieldSet::CFieldSet(CFieldSet& f)
{
  Init();

  Copy(f);
}
//----------------------------------------

CFieldSet::~CFieldSet()
{


}
//----------------------------------------
void CFieldSet::Init()
{
  m_field = NULL;
}
//----------------------------------------
void CFieldSet::Copy(CFieldSet& f)
{
  m_name = f.m_name;
  m_field = f.m_field;

}
//----------------------------------------
CFieldSet& CFieldSet::operator=(CFieldSet& f)
{
  Copy(f);
  return *this;
}

//----------------------------------------
void CFieldSet::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CFieldSet Object at "<< this << std::endl;
  fOut << "m_name " << m_name << std::endl;
  fOut << "==> END Dump a CFieldSet Object at "<< this << std::endl;

  fOut << std::endl;

}
//-------------------------------------------------------------
//------------------- CFieldSetString class --------------------
//-------------------------------------------------------------

CFieldSetString::CFieldSetString(const std::string& name /* = "" */)
      : CFieldSet(name)
{
  Init();
}
//----------------------------------------
CFieldSetString::CFieldSetString(CFieldSetString& f)
      : CFieldSet(f)
{
  Init();

  Copy(f);
}
//----------------------------------------
CFieldSetString::~CFieldSetString()
{
}
//----------------------------------------
void CFieldSetString::Init()
{
}
//----------------------------------------
void CFieldSetString::Copy(CFieldSetString& f)
{
  CFieldSet::operator =(f);
  m_value = f.m_value;
}
//----------------------------------------
CFieldSetString& CFieldSetString::operator=(CFieldSetString& f)
{
  Copy(f);
  return *this;
}

//----------------------------------------
void CFieldSetString::Insert(const CDoubleArray& vect, bool bRemove)
{
    UNUSED(bRemove);

  if (vect.empty())
  {
    m_value = "";
  }
  else
  {
    m_value = CTools::DoubleToStr(vect.front());
  }
}
//----------------------------------------
void CFieldSetString::Insert(double value, bool bRemove)
{
    UNUSED(bRemove);

  m_value = CTools::DoubleToStr(value);
}

//----------------------------------------
void CFieldSetString::Insert(const std::string& value, bool bRemove)
{
    UNUSED(bRemove);

  m_value = value;
}

//----------------------------------------

void CFieldSetString::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CFieldSetString Object at "<< this << std::endl;
  CFieldSet::Dump(fOut);
  fOut << "m_value " << m_value << std::endl;
  fOut << "==> END Dump a CFieldSetString Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CFieldSetDbl class --------------------
//-------------------------------------------------------------

CFieldSetDbl::CFieldSetDbl(const std::string& name /* = "" */)
      : CFieldSet(name)
{
  Init();
}
//----------------------------------------
CFieldSetDbl::CFieldSetDbl(CFieldSetDbl& f)
      : CFieldSet(f)
{
  Init();

  Copy(f);
}
//----------------------------------------
CFieldSetDbl::~CFieldSetDbl()
{
}
//----------------------------------------
void CFieldSetDbl::Init()
{
}
//----------------------------------------
void CFieldSetDbl::Copy(CFieldSetDbl& f)
{
  CFieldSet::operator =(f);
  m_value = f.m_value;
}
//----------------------------------------
CFieldSetDbl& CFieldSetDbl::operator=(CFieldSetDbl& f)
{
  Copy(f);
  return *this;
}

/*
//----------------------------------------
CFieldSetDbl& CFieldSetDbl::operator=(uint32_t value)
{
  m_value = static_cast<double>(value);
  return *this;
}

//----------------------------------------
CFieldSetDbl& CFieldSetDbl::operator=(int32_t value)
{
  m_value = static_cast<double>(value);
  return *this;
}

*/

//----------------------------------------
void CFieldSetDbl::Insert(const CDoubleArray& vect, bool bRemove)
{
    UNUSED(bRemove);

  m_value = vect.front();
}
//----------------------------------------

void CFieldSetDbl::Insert(double value, bool bRemove)
{
    UNUSED(bRemove);

  m_value = value;
}
//----------------------------------------
void CFieldSetDbl::Insert(const std::string& value, bool bRemove)
{
    UNUSED(bRemove);

  m_value = CTools::StrToDouble(value);
}

//----------------------------------------

void CFieldSetDbl::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CFieldSetDbl Object at "<< this << std::endl;
  CFieldSet::Dump(fOut);
  fOut << "m_value " << m_value << std::endl;
  fOut << "==> END Dump a CFieldSetDbl Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CFieldSetArrayDbl class --------------------
//-------------------------------------------------------------

CFieldSetArrayDbl::CFieldSetArrayDbl(const std::string& name /* = "" */)
      : CFieldSet(name)
{
  Init();
}
//----------------------------------------
CFieldSetArrayDbl::CFieldSetArrayDbl(CFieldSetArrayDbl& f)
      : CFieldSet(f)
{
  Init();

  Copy(f);
}
//----------------------------------------
CFieldSetArrayDbl::~CFieldSetArrayDbl()
{
}
//----------------------------------------
void CFieldSetArrayDbl::Init()
{
  m_nbDims = 0;
}
//----------------------------------------
void CFieldSetArrayDbl::Copy(CFieldSetArrayDbl& f)
{
  CFieldSet::operator =(f);

  m_nbDims = f.m_nbDims;
  m_dim = f.m_dim;
  m_vector = f.m_vector;

}
//----------------------------------------
CFieldSetArrayDbl& CFieldSetArrayDbl::operator=(CFieldSetArrayDbl& f)
{
  Copy(f);
  return *this;
}

//----------------------------------------
void CFieldSetArrayDbl::Insert(const CDoubleArray& vect, bool bRemove)
{
  if (bRemove)
  {
    m_vector.RemoveAll();
  }
  m_vector.Insert(vect);
}
//----------------------------------------

void CFieldSetArrayDbl::Insert(double value, bool bRemove)
{
  if (bRemove)
  {
    m_vector.RemoveAll();
  }

  m_vector.Insert(value);
}

//----------------------------------------
void CFieldSetArrayDbl::Insert(const std::string& value, bool bRemove)
{
  if (bRemove)
  {
    m_vector.RemoveAll();
  }

 m_vector.Insert(CTools::StrToDouble(value));
}

//----------------------------------------

void CFieldSetArrayDbl::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CFieldSetArrayDbl Object at "<< this << std::endl;
  CFieldSet::Dump(fOut);
  fOut << "m_nbDims " << m_nbDims << std::endl;
  fOut << "m_dim:" << std::endl;
  m_dim.Dump(fOut);
  fOut << "data:" << std::endl;
  m_vector.Dump(fOut);
  fOut << "==> END Dump a CFieldSetArrayDbl Object at "<< this << std::endl;

  fOut << std::endl;

}



//-------------------------------------------------------------
//------------------- CRecordSet class --------------------
//-------------------------------------------------------------

CRecordSet::CRecordSet(const std::string& name /* = "" */, bool bDelete /*= true*/)
          : CObMap(bDelete)
{
  m_name = name;
}

//----------------------------------------

CRecordSet::~CRecordSet()
{


}

//----------------------------------------

CFieldSet* CRecordSet::ExistsFieldSet(const std::string& key)
{
  return dynamic_cast<CFieldSet*>(CObMap::Exists(key));

}
//----------------------------------------

CField* CRecordSet::GetField(CRecordSet::iterator it)
{
  CField* field = NULL;

  CFieldSet* fieldSet = GetFieldSet(it);
  if (fieldSet == NULL)
  {
    return field;
  }

  field = fieldSet->GetField();
  return field;

}
//----------------------------------------
bool CRecordSet::IsFieldHasToBeExpanded(CRecordSet::iterator it, const CStringList& listFieldExpandArray)
{
  return IsFieldHasToBeExpanded(GetFieldSet(it), listFieldExpandArray);
}

//----------------------------------------
bool CRecordSet::IsFieldHasToBeExpanded(CFieldSet* fieldSet, const CStringList& listFieldExpandArray)
{
  if (listFieldExpandArray.size() <= 0)
  {
    return false;
  }

  if (fieldSet == NULL)
  {
    return false;
  }

  CField* field = fieldSet->GetField();

  if (field == NULL)
  {
    return false;
  }

  return listFieldExpandArray.ExistsNoCase(field->GetName());

}

//----------------------------------------
CFieldSet* CRecordSet::GetFieldSet(const std::string& dataSetName, const std::string& fieldName)
{
  //Search field with dataset name, some products (HDF4 and NetCDf have no dataset name)
  //If not found, search field wihtout dataset name
  std::string str = CProduct::m_treeRootName + "." + dataSetName + "." + fieldName;
  CFieldSet* fieldSet = this->ExistsFieldSet(str);
  if (fieldSet == NULL)
  {
    str = CProduct::m_treeRootName + "." + fieldName;
    fieldSet = this->ExistsFieldSet(str);
  }

  return fieldSet;

}
//----------------------------------------
CFieldSet* CRecordSet::GetFieldSet(CRecordSet::iterator it)
{
  return dynamic_cast<CFieldSet*>(it->second);
}
//----------------------------------------
void CRecordSet::ExecuteExpression(CExpression &expr, const std::string &recordName, CExpressionValue& exprValue, CProduct* product /* = NULL */)
{
  CStringList::iterator itFieldName;
  CStringList exprNames(*expr.GetFieldNames());

  CExpressionValue* exprValueWork = NULL;

  if (! exprNames.empty())
  {
    // Set the values for each fields in the expression
    for (itFieldName = exprNames.begin(); itFieldName != exprNames.end() ; itFieldName++)
    {
      CFieldSet* fieldSet = GetFieldSet(recordName, *itFieldName);
      if (fieldSet == NULL)
      {
        this->Dump(*CTrace::GetDumpContext());
        throw CException(CTools::Format("ERROR in CRecordSet::ExecuteExpression -  GetFieldSet returns NULL pointer - "
                	       "recordSet seems to contain objects other than those of the class CFieldSet. Field names is: '%s'", itFieldName->c_str()),
			        BRATHL_LOGIC_ERROR);
      }

      if (typeid(*fieldSet) == typeid(CFieldSetDbl))
      {
	      CFieldSetDbl* fieldSetDbl = dynamic_cast<CFieldSetDbl*>(fieldSet);
	      exprValueWork	= new CExpressionValue(fieldSetDbl->m_value);

      }
      else if (typeid(*fieldSet) == typeid(CFieldSetArrayDbl))
      {
	      CFieldSetArrayDbl* fieldSetArrayDbl = dynamic_cast<CFieldSetArrayDbl*>(fieldSet);
	      //exprValue	= CExpressionValue(FloatType, fieldSetArrayDbl->m_dim, &fieldSetArrayDbl->m_vector[0]);
	      exprValueWork	= new CExpressionValue(FloatType, fieldSetArrayDbl->m_dim, fieldSetArrayDbl->m_vector);
      }
      else if (typeid(*fieldSet) == typeid(CFieldSetString))
      {
	      CFieldSetString* fieldSetString = dynamic_cast<CFieldSetString*>(fieldSet);
	      //exprValue	= CExpressionValue(FloatType, fieldSetArrayDbl->m_dim, &fieldSetArrayDbl->m_vector[0]);
	      exprValueWork	= new CExpressionValue(fieldSetString->m_value);
      }
      else
      {
	      throw CException("ERROR in  ExecuteExpression -  CFieldSet : unknown object type", BRATHL_LOGIC_ERROR);
      }

      // Insert a "CExpressionValue*" into the expression fields values
      // (delete CExpressionValue object is done within CExpression).
      expr.SetValue(*itFieldName, exprValueWork);
    }
  }

  // Execute the expression and get the result
  exprValue = expr.Execute(product);

}


//----------------------------------------
void CRecordSet::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CRecordSet Object at "<< this << " with " <<  size() << " elements" << std::endl;
  fOut << "m_name " << m_name << std::endl;
  CObMap::Dump(fOut);
  fOut << "==> END Dump a CRecordSet Object at "<< this << " with " <<  size() << " elements" << std::endl;
  fOut << std::endl;

}
//-------------------------------------------------------------
//------------------- CRecord class --------------------
//-------------------------------------------------------------

CRecord::CRecord(CRecordSet* recordSet /* = NULL */)
{
  m_recordSet = recordSet;
}

//----------------------------------------

CRecord::~CRecord()
{
  if (m_recordSet != NULL)
  {
    delete m_recordSet;
    m_recordSet = NULL;
  }


}


//----------------------------------------
const std::string& CRecord::GetName()
{
  if (m_recordSet == NULL)
  {
    CException e("ERROR in  CRecord::GetName() :  m_recordSet is NULL pointer - ", BRATHL_LOGIC_ERROR);
    throw(e);
  }

  return m_recordSet->m_name;

}

//----------------------------------------
void CRecord::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CRecord Object at "<< this << std::endl;
  m_recordSet->Dump(fOut);
  fOut << "==> END Dump a CRecord Object at "<< this << std::endl;
  fOut << std::endl;

}


//-------------------------------------------------------------
//------------------- CDataSet class --------------------
//-------------------------------------------------------------

CDataSet::CDataSet(const std::string& name /* = "" */, bool bDelete /*= true*/)
      : CObArray(bDelete),
        m_recordSetMap(false)
{
  m_name = name;
  m_currentRecordSet = NULL;

}

//----------------------------------------

CDataSet::~CDataSet()
{

}

//----------------------------------------
void CDataSet::RemoveAll()
{
  CObArray::RemoveAll();
  m_currentRecordSet = NULL;
  m_recordSetMap.RemoveAll();

}
//----------------------------------------
bool CDataSet::EraseCurrentRecordSet()
{
  return Erase(m_currentRecordSet);
}
//----------------------------------------
bool CDataSet::Erase(CRecordSet* recordSet)
{
  if (recordSet == NULL)
  {
    return false;
  }

  if (m_currentRecordSet == recordSet)
  {
    m_currentRecordSet = NULL;
  }

  CRecord* record = GetRecord(recordSet);

  m_recordSetMap.Erase(recordSet->m_name);

  return CObArray::Erase(record);

}
//----------------------------------------
CRecordSet* CDataSet::FindRecord(const std::string& recordSetName)
{

  // DON'T USE this syntax, there's a bug in STL,
  //   after calling  'record = m_recordSetMap[recordSetName]', if key not existed and the map is empty
  //   then the key exists
  //   in the map and points to a NULL object
  //CRecord* record = dynamic_cast<CRecord*>(m_recordSetMap[recordSetName]);
  CRecord* record = GetRecord(recordSetName);
  return record->GetRecordSet();

}
//----------------------------------------
void CDataSet::InsertDataset(CDataSet* dataSet, bool setAsCurrent /* = true */)
{
  CObMap* recordSetMap = dataSet->GetRecordSetMap();

  CObMap::iterator itObMap;
  for (itObMap = recordSetMap->begin() ; itObMap != recordSetMap->end() ; itObMap++)
  {
    m_recordSetMap.Insert(itObMap->first, itObMap->second);
  }

  CObArray::iterator itObArray;
  for (itObArray = dataSet->begin() ; itObArray != dataSet->end() ; itObArray++)
  {
    CRecord* record = dynamic_cast<CRecord*>(*itObArray);
    if (record == NULL)
    {
      continue;
    }

    Insert(record);
  }

  if (setAsCurrent)
  {
    SetCurrentRecordSet(dataSet->GetCurrentRecordSet());
  }
}
//----------------------------------------
CRecordSet* CDataSet::InsertRecord(const std::string& recordSetName, bool setAsCurrent /* = true */)
{

  CRecordSet* recordSet = NULL;
  // DON'T USE this syntax, there's a bug in STL,
  //   after calling  'record = m_recordSetMap[recordSetName]', if key not existed and the map is empty
  //   then the key exists
  //   in the map and points to a NULL object
  //CRecord* record = dynamic_cast<CRecord*>(m_recordSetMap[recordSetName]);
  CRecord* record = GetRecord(recordSetName);

  if (record == NULL)
  {
    recordSet = new CRecordSet(recordSetName);
    record = new CRecord(recordSet); // Create a new CRecord

    this->Insert(record); //insert new CRecordSet

    m_recordSetMap.Insert(recordSetName, record); // add recordSet to the map
  }


  if (setAsCurrent)
  {
    //set the recordSet (found or inserted) as current recordSet
    SetCurrentRecordSet(record->GetRecordSet());
  }

  //return the recordSet (found or inserted)
  return record->GetRecordSet();

}
//----------------------------------------
CFieldSet* CDataSet::GetFieldSet(const std::string& fieldSetKey)
{
  CRecordSet* recordSet = this->GetCurrentRecordSet();
  if (recordSet == NULL)
  {
    return NULL;
  }

  return recordSet->ExistsFieldSet(fieldSetKey);

}
//----------------------------------------
CFieldSetArrayDbl* CDataSet::GetFieldSetAsArrayDbl(const std::string& fieldSetKey)
{
  return dynamic_cast<CFieldSetArrayDbl*>(GetFieldSet(fieldSetKey));
}
//----------------------------------------
CFieldSetDbl* CDataSet::GetFieldSetAsDbl(const std::string& fieldSetKey)
{
  return dynamic_cast<CFieldSetDbl*>(GetFieldSet(fieldSetKey));
}
//----------------------------------------
CFieldSetString* CDataSet::GetFieldSetAsString(const std::string& fieldSetKey)
{
  return dynamic_cast<CFieldSetString*>(GetFieldSet(fieldSetKey));
}

//----------------------------------------
double CDataSet::GetFieldSetAsDblValue(const std::string& fieldSetKey)
{

  double value = CTools::m_defaultValueDOUBLE;
  CFieldSetDbl* fieldSet = GetFieldSetAsDbl(fieldSetKey);

  if (fieldSet != NULL)
  {
    value =  fieldSet->GetData();
  }

  return value;
}
//----------------------------------------
std::string CDataSet::GetFieldSetAsStringValue(const std::string& fieldSetKey)
{

  std::string value = "";
  CFieldSetString* fieldSet = GetFieldSetAsString(fieldSetKey);

  if (fieldSet != NULL)
  {
    value =  fieldSet->GetData();
  }

  return value;
}
//----------------------------------------
void CDataSet::InsertFieldSet(const std::string& fieldSetKey, CFieldSet* fieldSet)
{
  CRecordSet* recordSet = this->GetCurrentRecordSet();

  if (recordSet == NULL)
  {
    std::string msg = CTools::Format("ERROR - CDataSet::InsertFieldSet - current recordset is NULL - Fieldset name %s",
                                fieldSetKey.c_str());
    CException e(msg, BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);
  }

  recordSet->Insert(fieldSetKey, fieldSet);

}
//----------------------------------------
void CDataSet::EraseFieldSet(const std::string& fieldSetKey)
{
  CRecordSet* recordSet = this->GetCurrentRecordSet();
  if (recordSet == NULL)
  {
    std::string msg = CTools::Format("ERROR - CDataSet::EraseFieldSet - current recordset is NULL - Fieldset name %s",
                                fieldSetKey.c_str());
    CException e(msg, BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);
  }

  recordSet->Erase(fieldSetKey);
}
//----------------------------------------
void CDataSet::SetCurrentRecordSet(int32_t index)
{
  if (this->size() <= 0)
  {
    m_currentRecordSet = NULL;
    return;
  }

  SetCurrentRecordSet(GetRecordSet(index));
}
//----------------------------------------
void CDataSet::SetCurrentRecordSet(CDataSet::iterator itDataSet)
{
  if (this->size() <= 0)
  {
    m_currentRecordSet = NULL;
    return;
  }

  SetCurrentRecordSet(GetRecordSet(itDataSet));
}
//----------------------------------------
void CDataSet::SetCurrentRecordSet(CRecordSet* recordSet)
{
  if (recordSet == NULL)
  {
    CException e("ERROR - SetCurrentRecordSet(CRecordSet* recordSet) - recordSet object is NULL", BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);

  }

  m_currentRecordSet = recordSet;
}
//----------------------------------------
void CDataSet::SetCurrentRecordSet(const std::string& recordSetName)
{
  CRecord* record = GetRecord(recordSetName);

  if (record == NULL)
  {
    std::string msg = CTools::Format("ERROR - SetCurrentRecordSet(CRecordSet* recordSet) - recordSet name '%s' not found name",
                                recordSetName.c_str());
    CException e(msg, BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    Dump(*CTrace::GetDumpContext());
    throw (e);

  }

  SetCurrentRecordSet(record->GetRecordSet());

}
//----------------------------------------
CRecordSet* CDataSet::Back(bool withExcept)
{

  if (this->empty())
  {
    if (withExcept)
    {
      CException e("ERROR in CDataSet::Back - Array is empty", BRATHL_LOGIC_ERROR);
      throw(e);
    }
    else
    {
      return NULL;
    }
  }

  CRecord* record = dynamic_cast<CRecord *>(CObArray::back());
  if ((record == NULL) && (withExcept))
  {
    CException e("ERROR in  CDataSet::Back returns NULL pointer - "
                 "Array seems to contain objects other than those of the class CRecord", BRATHL_LOGIC_ERROR);
    throw(e);
  }

  if (record == NULL)
  {
    return NULL;
  }

  CRecordSet* recordSet = record->GetRecordSet();
  if ((recordSet == NULL) && (withExcept))
  {
    CException e("ERROR in  CDataSet::Back returns NULL pointer - "
                 "Array seems to contain objects other than those of the class CRecordSet", BRATHL_LOGIC_ERROR);
    throw(e);
  }
  return recordSet;
}


//----------------------------------------
CRecordSet* CDataSet::GetFirstRecordSet()
{
  if (this->size() <= 0)
  {
    return NULL;
  }

  return GetRecordSet(0);
}
//----------------------------------------
CRecordSet* CDataSet::GetRecordSet(CDataSet::iterator itDataSet)
{
  if (this->size() <= 0)
  {
    return NULL;
  }

  if (itDataSet == this->end())
  {
    return NULL;
  }

  CRecord* record = dynamic_cast<CRecord*>(*itDataSet);
  if (record == NULL)
  {
    return NULL;
  }

  return record->GetRecordSet();
}
//----------------------------------------
CRecordSet* CDataSet::GetRecordSet(int32_t index)
{
  if (this->size() <= 0)
  {
    return NULL;
  }

  if (index < 0)
  {
    return NULL;
  }
  return GetRecordSet(begin() + index);
}
//----------------------------------------
CRecord* CDataSet::GetRecord(CRecordSet* recordSet)
{
  return GetRecord(recordSet->m_name);
}
//----------------------------------------
CRecord* CDataSet::GetRecord(const std::string& recordSetName)
{
  return dynamic_cast<CRecord*>(m_recordSetMap.Exists(recordSetName));
}

//----------------------------------------
void CDataSet::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CObMap::iterator itObMap;

  fOut << "==> Dump a CDataSet Object at "<< this << " with " <<  size() << " elements" << std::endl;
  fOut << "m_name " << m_name << std::endl;

  fOut << "m_recordSetMap " << std::endl;
  for (itObMap = m_recordSetMap.begin() ; itObMap != m_recordSetMap.end() ; itObMap++)
  {
    fOut << "\tm_recordSetMap Key is = '" << (*itObMap).first;
    fOut << "' and object at " << itObMap->second << std::endl;
  }

  CObArray::Dump(fOut);
  fOut << "==> END Dump a CDataSet Object at "<< this << " with " <<  size() << " elements" << std::endl;

  fOut << std::endl;

}

} //end namespace
