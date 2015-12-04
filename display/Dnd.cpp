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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "Dnd.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Dnd.h"

//----------------------------------------------------------------------------
// CDndField
//----------------------------------------------------------------------------
/*
CDndField::CDndField(CDndField& dndField)
            : m_fields(false)
{
   AddField(dndField.m_fields);
}
*/
//----------------------------------------
CDndField::CDndField(CDndField& dndField)
{
   m_field = dndField.m_field;
}
//----------------------------------------
/*
void CDndField::AddField(CField* field)
{
  if (field != NULL)
  {
    m_fields.Insert(field);
  }
}
*/

//----------------------------------------
/*
void CDndField::AddField(CObArray& fields)
{
  m_fields.RemoveAll();

  CObArray::const_iterator it;
  for (it = fields.begin() ; it != fields.end() ; it++)
  {
    AddField(dynamic_cast<CField*>(*it));
  }
}
*/
//----------------------------------------
/*
void CDndField::GetField(CObArray& fields)
{
  fields.RemoveAll();

  CObArray::const_iterator it;
  for (it = m_fields.begin() ; it != m_fields.end() ; it++)
  {
    CField* field = dynamic_cast<CField*>(*it);
    if (field != NULL)
    {
      fields.Insert(field);
    }
  }
}
*/
//----------------------------------------------------------------------------
// CDndFieldDataObject
//----------------------------------------------------------------------------

CDndFieldDataObject::CDndFieldDataObject(CDndField* dndField)
{
  if (dndField != NULL)  
  {
    m_dndField = new CDndField(*dndField);
  }
  else
  {
    m_dndField = NULL;
  }
  
  wxDataFormat fieldformat;
  fieldformat.SetId(fieldFormatId);
  SetFormat(fieldformat);
}
//----------------------------------------

size_t CDndFieldDataObject::GetDataSize() const
{
  size_t ret = 0;
  //size_t count = 0;

  if (m_dndField == NULL)  
  {
    return 0;
  }

  /*
  count = m_dndField->GetCount();
  for (size_t i = 0; i < count; i++)
  {
    ret += sizeof(void*);
  }
*/
  ret = sizeof(long);

  return ret;
}
//----------------------------------------

bool CDndFieldDataObject::GetDataHere(void* buf) const
{
  if (m_dndField == NULL)  
  {
    return false;
  }

  /*
  CObArray* fields = static_cast<CObArray*>(buf);

  if (fields == NULL)  
  {
    return false;
  }
  if (fields->size() <= 0)  
  {
    return false;
  }

  fields->RemoveAll();
  
  m_dndField->GetField(*fields);
*/
  
  if (m_dndField->m_field == NULL)  
  {
    return false;
  }

  long ptr = (long)m_dndField->m_field;
  memset(buf, 0, sizeof(long));
  memcpy(buf, &ptr, sizeof(long));

  return true;
}
//----------------------------------------

bool CDndFieldDataObject::SetData(size_t len, const void* buf)
{
    UNUSED(len);
  /*
  CObArray* fields = static_cast<CObArray*>(const_cast<void*>(buf));

  if (fields == NULL)  
  {
    return false;
  }
  if (fields->size() <= 0)  
  {
    return false;
  }

*/
  if (m_dndField == NULL) 
  {
    m_dndField = new CDndField();
  }

  long ptr = 0;
  memcpy(&ptr, buf, sizeof(long));

  CField* field = (CField*)(ptr);
//  m_dndField->AddField(*fields);
  m_dndField->m_field = field;

  return true;
}


// WDR: class implementations



