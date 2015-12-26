/*
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

#include "new-gui/brat/stdafx.h"

#include "brathl.h"

#include "MapTypeDisp.h"


//-------------------------------------------------------------
//------------------- CMapTypeDisp class --------------------
//-------------------------------------------------------------

CMapTypeDisp::CMapTypeDisp()
{
  Insert("Y=F(X)", eTypeDispYFX);
  Insert("Z=F(Lon,Lat)", eTypeDispZFLatLon);
  Insert("Z=F(X,Y)", eTypeDispZFXY);
}

//----------------------------------------
CMapTypeDisp::~CMapTypeDisp()
{

}

//----------------------------------------
CMapTypeDisp& CMapTypeDisp::GetInstance()
{
 static CMapTypeDisp instance;

 return instance;
}

//----------------------------------------
bool CMapTypeDisp::ValidName(const std::string& name)
{   
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapTypeDisp::ValidName(const char* name)
{   
  uint32_t value = Exists(name);
  return (!isDefaultValue(value));
}

//----------------------------------------

std::string CMapTypeDisp::IdToName(uint32_t id)
{
  CMapTypeDisp::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (value == id)
    {
      return (it->first).c_str();
    }
  }

  return "";
}

//----------------------------------------
uint32_t CMapTypeDisp::NameToId( const std::string& name )
{
	return Exists( name );
}
//----------------------------------------
//void CMapTypeDisp::NamesToArrayString(wxArrayString& array)
//{
//  CMapTypeDisp::iterator it;
//
//  for (it = begin() ; it != end() ; it++)
//  {
//    uint32_t value = it->second;
//    if (!isDefaultValue(value))
//    {
//      array.Add( (it->first).c_str());
//    }
//  }
//
//}
//----------------------------------------
std::string CMapTypeDisp::Enum() const
{
	std::string result;

	for ( CMapTypeDisp::const_iterator it = begin(); it != end(); it++ )
	{
		result += ( it->first + " ==> " + n2s< std::string >( it->second ) + "  " );
	}

	return result;
}
