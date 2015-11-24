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
    #pragma implementation "MapProjection.h"
#endif

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "../libbrathl/Tools.h"
#include "new-gui/Common/tools/Exception.h"
using namespace brathl;

#include "vtkProj2DFilter.h"	//VISAN
#include "MapProjection.h"

/*
// Defines for vtkProjections (values must be > 0) ===> projection constants are declared in vtkProj2DFilter.h
uint32_t CMapProjection::VTK_PROJ2D_LAMBERT_CYLINDRICAL      = 1;
uint32_t CMapProjection::VTK_PROJ2D_PLATE_CAREE              = 2;
uint32_t CMapProjection::VTK_PROJ2D_MOLLWEIDE                = 3;
uint32_t CMapProjection::VTK_PROJ2D_ROBINSON                 = 4;
uint32_t CMapProjection::VTK_PROJ2D_LAMBERT_AZIMUTHAL        = 5;
uint32_t CMapProjection::VTK_PROJ2D_AZIMUTHAL_EQUIDISTANT    = 6;
uint32_t CMapProjection::VTK_PROJ2D_3D                       = 7;
*/

CMapProjection* CMapProjection::m_instance = NULL;
CSmartCleanerMapProj	CMapProjection::m_SmartCleaner;


CMapProjection::CMapProjection()
{
  //projection constants are declared in vtkProj2DFilter.h
  Insert("Plate Caree", VTK_PROJ2D_PLATE_CAREE);
  Insert("Robinson", VTK_PROJ2D_ROBINSON);
  Insert("Mollweide", VTK_PROJ2D_MOLLWEIDE);
  Insert("Lambert Cylindrical", VTK_PROJ2D_LAMBERT_CYLINDRICAL);
  Insert("Lambert Azimuthal", VTK_PROJ2D_LAMBERT_AZIMUTHAL);
  Insert("Azimuthal Equidistant", VTK_PROJ2D_AZIMUTHAL_EQUIDISTANT);
  Insert("Mercator", VTK_PROJ2D_MERCATOR);
  Insert("Orthographic", VTK_PROJ2D_ORTHO);
  Insert("Near-Sided Perspective", VTK_PROJ2D_NEAR_SIGHTED);
  Insert("Stereographic", VTK_PROJ2D_STEREOGRAPHIC);
  //Insert("Transverse Mercator", VTK_PROJ2D_TMERCATOR);  // this projection is weird..
  Insert("3D", VTK_PROJ2D_3D);

  m_SmartCleaner.SetObject( this );


}


//----------------------------------------
CMapProjection::~CMapProjection()
{

  // Step 1 - Set m_instance to NULL when the object is deleted
  CMapProjection::m_instance = NULL;
  // Step 2 - Set NULL pointer to smart cleaner
  m_SmartCleaner.SetObject( NULL );
}


//----------------------------------------
CMapProjection* CMapProjection::GetInstance()
 {
   if ( CMapProjection::m_instance == NULL)
   {
     CMapProjection::m_instance = new CMapProjection;
   }
   return CMapProjection::m_instance;
 }

//----------------------------------------

bool CMapProjection::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}


//----------------------------------------
bool CMapProjection::ValidName(const char* name)
{
  uint32_t value = Exists(name);
  return (!isDefaultValue(value));
}

//----------------------------------------
std::string CMapProjection::NameToLabeledName(const std::string& name)
{
  CMapProjection::iterator it;
  std::string nameTmp = CTools::StringToLower(name);

  for (it = begin() ; it != end() ; it++)
  {
    std::string lowerName = CTools::StringToLower(it->first);
    if (nameTmp == lowerName)
    {
      return it->first;
    }
  }

  return "";
}

//----------------------------------------

std::string CMapProjection::IdToName(uint32_t id)
{
  CMapProjection::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (value == id)
    {
      return it->first;
    }
  }

  return "";
}

//----------------------------------------
uint32_t CMapProjection::NameToId(const char* name)
{
  return Exists(name);
}

//----------------------------------------
uint32_t CMapProjection::NameToId(const std::string& name)
{
  return Exists(name);
}

//----------------------------------------
/*femmTODO - as inline function in wcGuiInterface
void CMapProjection::NamesToArrayString(wxArrayString& array)
{
  CMapProjection::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (!isDefaultValue(value))
    {
      array.Add( (it->first).c_str());
    }
  }

}
//----------------------------------------
void CMapProjection::NamesToComboBox(wxComboBox& combo)
{
  CMapProjection::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (!isDefaultValue(value))
    {
      combo.Append((it->first).c_str());
    }
  }

}
femmTODO */

//----------------------------------------

uint32_t CMapProjection::Exists(const std::string& key)
{
  uint32_t value = CUIntMap::Exists(key);

  /*
  if (isDefaultValue(value))
  {
    CException e(CTools::Format("ERROR in CMapProjection::Exists - Invalid projection name '%s' - id %d",
                                key.c_str(), value),
                 BRATHL_LOGIC_ERROR);
    throw(e);
  }
*/
  return value;
}
//----------------------------------------

uint32_t CMapProjection::operator[](const std::string& key)
{
   return Exists(key);
}
