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
    #pragma implementation "Dataset.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "brathl.h"

#include "new-gui/Common/tools/Trace.h"
#include "Tools.h"
#include "new-gui/Common/tools/Exception.h"
using namespace brathl;

#include "BratGui.h"
#include "Dataset.h"


//----------------------------------------

CDataset::CDataset(wxString name)
{
  Init();
  m_name = name;
}
//----------------------------------------
CDataset::CDataset(CDataset& d)
{
  Init();
  Copy(d);

}
//----------------------------------------

CDataset::~CDataset()
{
  m_fieldSpecificUnit.RemoveAll();

  //DeleteProduct();
}
//----------------------------------------
CDataset& CDataset::operator=(CDataset& d)
{
  Init();
  Copy(d);
  return *this;
}

//----------------------------------------
void CDataset::Copy(CDataset& d)
{
  m_name = d.m_name;
  //m_product = NULL;
  m_files.Insert(d.m_files);
}
//----------------------------------------
void CDataset::Init()
{
  //m_product = NULL;
}
//----------------------------------------
std::string CDataset::GetFieldSpecificUnit(const std::string& key)
{
  return m_fieldSpecificUnit.Exists(key);
}
//----------------------------------------
void CDataset::SetFieldSpecificUnit(const std::string& key, const std::string& value)
{
  m_fieldSpecificUnit.Erase(key);
  m_fieldSpecificUnit.Insert(key, value, false);

}

//----------------------------------------
void CDataset::GetFiles( wxArrayString& array )
{

  CBratGuiApp::CStringListToWxArray(m_files, array);
  /*
  CProductList::iterator it;

  for (it = m_files.begin() ; it != m_files.end() ; it++)
  {
    array.Add((*it).c_str());
  }
 */
}
//----------------------------------------
void CDataset::GetFiles( wxListBox& array )
{

  CProductList::iterator it;

  for (it = m_files.begin() ; it != m_files.end() ; it++)
  {
    array.Append((*it).c_str());
  }
}
//----------------------------------------
bool CDataset::CtrlFiles()
{

  CProductList::iterator it;

  for (it = m_files.begin() ; it != m_files.end() ; it++)
  {
    if (wxFileExists( (*it).c_str() ) == false)
    {
      wxMessageBox(wxString::Format("Dataset '%s':\n contains file '%s' that doesn't exist\n",
                                     m_name.c_str(),
                                     (*it).c_str()),
                   "Warning",
                    wxOK | wxICON_EXCLAMATION);
      return false;
    }
  }
  return true;
}
/*
//----------------------------------------
void CDataset::GetRecordNames( wxArrayString& array, CProduct* product )
{
  CStringArray arrayTmp;

  GetRecordNames( arrayTmp, product);

  CBratGuiApp::CStringArrayToWxArray(arrayTmp, array);

}
//----------------------------------------
void CDataset::GetRecordNames( CStringArray& array, CProduct* product )
{
  if (product == NULL)
  {
    return;
  }

  product->GetRecords(array);

}

//----------------------------------------
void CDataset::GetRecordNames(wxComboBox& combo, CProduct* product)
{
  CStringArray array;

  GetRecordNames( array, product );

  CStringArray::iterator it;

  for (it = array.begin() ; it != array.end() ; it++)
  {
    combo.Append( (*it).c_str() );
  }
}
*/
/*
//----------------------------------------
void CDataset::DeleteProduct()
{
  if (m_product != NULL)
  {
    delete m_product;
    m_product = NULL;
  }

}
*/
//----------------------------------------
CProduct* CDataset::SetProduct()
{
  if (m_files.size() <= 0)
  {
    return NULL;
  }
  return SetProduct((*(m_files.begin())).c_str());
}
//----------------------------------------
CProduct* CDataset::SetProduct( const wxString& fileName )
{
  ///////DeleteProduct();
  CProduct* product = NULL;

  try
  {
    product = CProduct::Construct((const char *)fileName.c_str());
    product->SetFieldSpecificUnits(m_fieldSpecificUnit);
    product->Open((const char *)fileName.c_str());
  }
  catch (CException& e)
  {
    if (product != NULL)
    {
      delete product;
      product = NULL;
    }

    throw(e);
  }


 return product;

}
/*
//----------------------------------------
bool CDataset::OpenProduct()
{
  if (m_product == NULL)
  {
    return false;
  }

  return m_product->Open();

}
*/
/*
//----------------------------------------
bool CDataset::CloseProduct()
{
  if (m_product == NULL)
  {
    return true;
  }

  return m_product->Close();

}
*/

//----------------------------------------
bool CDataset::SaveConfig(wxFileConfig* config)
{
  bool bOk = true;
  bOk &=SaveConfig(config, ENTRY_FILE);
  bOk &=SaveConfigSpecificUnit(config, ENTRY_UNIT);
  return bOk;
}
//----------------------------------------
bool CDataset::SaveConfig(wxFileConfig* config, const wxString& entry)
{
  bool bOk = true;
  if (config == NULL)
  {
    return true;
  }

  config->SetPath("/" + m_name);

  CProductList::iterator it;

  int32_t index = 0;

  for (it = m_files.begin() ; it != m_files.end() ; it++)
  {
    index++;
    bOk &= config->Write(wxString::Format("%s%d", entry.c_str(), index),
                         (*it).c_str());
  }

  return bOk;


}
//----------------------------------------
bool CDataset::SaveConfigSpecificUnit(wxFileConfig* config, const wxString& entry)
{
  bool bOk = true;
  if (config == NULL)
  {
    return true;
  }

  config->SetPath("/" + m_name);

  CStringMap::iterator itMap;

  for (itMap = m_fieldSpecificUnit.begin() ; itMap != m_fieldSpecificUnit.end() ; itMap++)
  {
    bOk &= config->Write(wxString::Format("%s_%s", entry.c_str(), (itMap->first).c_str()),
                         (itMap->second).c_str());

  }

  return bOk;


}
//----------------------------------------
bool CDataset::LoadConfig(wxFileConfig* config)
{
  bool bOk = true;
  if (config == NULL)
  {
    return true;
  }

  config->SetPath("/" + m_name);

  //long maxEntries =
  config->GetNumberOfEntries();
  wxString entry;
  wxString valueString;
  long i = 0;

  CStringArray findStrings;

  do
  {
    bOk = config->GetNextEntry(entry, i);
    if (bOk)
    {
      // Finds ENTRY_FILE entries (dataset files entries)
      findStrings.RemoveAll();
      CTools::Find((const char *)entry.c_str(), (const char *)ENTRY_FILE_REGEX.c_str(), findStrings);

      if (findStrings.size() > 0)
      {
        valueString = config->Read(entry);
        m_files.Insert((const char *)valueString.c_str());
        continue;
      }

      // Finds specific unit entrie
      findStrings.RemoveAll();
      CTools::Find((const char *)entry.c_str(), (const char *)ENTRY_UNIT_REGEX.c_str(), findStrings);

      if (findStrings.size() > 0)
      {
        valueString = config->Read(entry);
        m_fieldSpecificUnit.Insert(findStrings.at(0), (const char *)valueString.c_str());
        //m_fieldSpecificUnit.Dump(*CTrace::GetDumpContext());
        continue;
      }

    }
  }
  while (bOk);

  return true;

}
//----------------------------------------
void CDataset::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CDataset Object at "<< this << std::endl;

  m_files.Dump(fOut);
  fOut << "==> END Dump a CDataset Object at "<< this << std::endl;

  fOut << std::endl;

}

