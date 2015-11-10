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
    #pragma implementation "MapImageType.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Tools.h" 
#include "Exception.h" 
using namespace brathl;

#include "PlotData/vtkProj2DFilter.h" 
#include "MapImageType.h" 

//CMapImageType* CMapImageType::m_instance = NULL;
//CSmartCleanerMapImageType	CMapImageType::m_SmartCleaner;

//-------------------------------------------------------------
//------------------- CImageType class --------------------
//-------------------------------------------------------------
CImageType::CImageType(const wxString& name, vtkImageWriter* vtkImageWriter)
{
  m_name = name;
  m_vtkImageWriter = vtkImageWriter;
}
//----------------------------------------
CImageType::~CImageType()
{
  if (m_vtkImageWriter != NULL)
  {
    m_vtkImageWriter->Delete();
    m_vtkImageWriter = NULL;
  }

}

//-------------------------------------------------------------
//------------------- CMapImageType class --------------------
//-------------------------------------------------------------


CMapImageType::CMapImageType()
{
  Insert("tif", 		new CImageType("TIFF", vtkTIFFWriter::New()));
  Insert("bmp", 		new CImageType("Windows Bitmap", vtkBMPWriter::New()));
  Insert("jpg", 		new CImageType("JPEG", vtkJPEGWriter::New()));
  Insert("png", 		new CImageType("PNG", vtkPNGWriter::New()));
  Insert("pnm", 		new CImageType("PNM", vtkPNMWriter::New()));

  //m_SmartCleaner.SetObject( this );
}


//----------------------------------------
CMapImageType::~CMapImageType()
{

  // Step 1 - Set m_instance to NULL when the object is deleted
//  CMapImageType::m_instance = NULL;
  // Step 2 - Set NULL pointer to smart cleaner
  //m_SmartCleaner.SetObject( NULL );
}


/*
//----------------------------------------
CMapImageType& CMapImageType::GetInstance()
{
   static CMapImageType instance;

   return instance;
}
 */
//----------------------------------------
bool CMapImageType::ValidName(const std::string& name)
{   
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapImageType::ValidName(const char* name)
{   
  CImageType* value = dynamic_cast<CImageType*>(Exists(name));
  return (value != NULL);
}

//----------------------------------------
void CMapImageType::NamesToArrayString(wxArrayString& array)
{
  CMapImageType::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CImageType* value = dynamic_cast<CImageType*>(it->second);
    if (value != NULL)
    {
      array.Add(value->m_name.c_str());
    }
  }

}

//----------------------------------------
void CMapImageType::NamesToMapString(CStringMap& strMap)
{
  CMapImageType::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CImageType* value = dynamic_cast<CImageType*>(it->second);
    if (value != NULL)
    {
      strMap.Insert(it->first, (const char *)(value->m_name));
    }
  }

}

