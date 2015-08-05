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
    #pragma implementation "MapColor.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/colordlg.h"

#include "vtkProj2DFilter.h" 

#include "Tools.h" 
using namespace brathl;

#include "MapColor.h" 

//-------------------------------------------------------------
//------------------- CPointGlyph class --------------------
//-------------------------------------------------------------
CPointGlyph::CPointGlyph(PointGlyph id, const wxString name)
{
  m_name = name;
  m_id = id;
}
//----------------------------------------
CPointGlyph::~CPointGlyph()
{

}


//-------------------------------------------------------------
//------------------- CMapPointGlyph class --------------------
//-------------------------------------------------------------
CMapPointGlyph::CMapPointGlyph()
{
  //Insert("NO", 		        new CPointGlyph(displayNO_GLYPH, "No glyph"));
  //Insert("VERTEX", 		new CPointGlyph(displayVERTEX_GLYPH, "Vertex"));
  Insert("DASH", 		new CPointGlyph(displayDASH_GLYPH, "Dash"));
  Insert("CROSS", 		new CPointGlyph(displayCROSS_GLYPH, "Cross"));
  Insert("THICKCROSS", 		new CPointGlyph(displayTHICKCROSS_GLYPH, "Thick cross"));
  Insert("TRIANGLE", 		new CPointGlyph(displayTRIANGLE_GLYPH, "Triangle"));
  Insert("SQUARE", 		new CPointGlyph(displaySQUARE_GLYPH, "Square"));
  Insert("CIRCLE", 		new CPointGlyph(displayCIRCLE_GLYPH, "Circle"));
  Insert("DIAMOND", 		new CPointGlyph(displayDIAMOND_GLYPH, "Diamond"));
  Insert("ARROW", 		new CPointGlyph(displayARROW_GLYPH, "Arrow"));
  Insert("THICKARROW", 		new CPointGlyph(displayTHICKARROW_GLYPH, "Thick arrow"));
  Insert("HOOKEDARROW", 	new CPointGlyph(displayHOOKEDARROW_GLYPH, "Hooked arrow"));
}


//----------------------------------------
CMapPointGlyph::~CMapPointGlyph()
{

}

//----------------------------------------
CMapPointGlyph& CMapPointGlyph::GetInstance()
{
 static CMapPointGlyph instance;

 return instance;
}
//----------------------------------------
bool CMapPointGlyph::ValidName(const string& name)
{   
  return ValidName(wxString(name.c_str()));
}
//----------------------------------------
bool CMapPointGlyph::ValidName(const wxString& name)
{ 
  wxString nameUpper = name.Upper();
  CPointGlyph* value = dynamic_cast<CPointGlyph*>(Exists((const char *)(nameUpper)));
  return (value != NULL);
}
//----------------------------------------
wxString CMapPointGlyph::GlyphToName(PointGlyph id)
{
  CMapPointGlyph::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CPointGlyph* value = dynamic_cast<CPointGlyph*>(it->second);
    if (value == NULL)
    {
      continue;
    }

    if (id == value->m_id)
    {
      return value->m_name;
    }

  }

  return "";

}
//----------------------------------------
wxString CMapPointGlyph::GlyphToKey(PointGlyph id)
{
  CMapPointGlyph::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CPointGlyph* value = dynamic_cast<CPointGlyph*>(it->second);
    if (value == NULL)
    {
      continue;
    }

    if (id == value->m_id)
    {
      return (it->first).c_str();
    }

  }

  return "";

}
//----------------------------------------
PointGlyph CMapPointGlyph::NameToGlyph(const string& name)
{
  return NameToGlyph(wxString(name.c_str()));

}
//----------------------------------------
PointGlyph CMapPointGlyph::NameToGlyph(const wxString& name)
{
  CMapPointGlyph::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CPointGlyph* value = dynamic_cast<CPointGlyph*>(it->second);
    if (value == NULL)
    {
      continue;
    }

    if (name == value->m_name)
    {
      return value->m_id;
    }

  }

  return displayCIRCLE_GLYPH;

}
//----------------------------------------
PointGlyph CMapPointGlyph::KeyToGlyph(const string& key)
{
  return KeyToGlyph(wxString(key.c_str()));

}
//----------------------------------------
PointGlyph CMapPointGlyph::KeyToGlyph(const wxString& key)
{
  wxString keyUpper = key.Upper();
  CPointGlyph* value = dynamic_cast<CPointGlyph*>(Exists((const char *)(keyUpper)));
  if (value != NULL)
  {
    return value->m_id;
  }

  return displayCIRCLE_GLYPH;

}

//----------------------------------------
wxString CMapPointGlyph::KeyToName(const string& key)
{
  return KeyToName(wxString(key.c_str()));

}
//----------------------------------------
wxString CMapPointGlyph::KeyToName(const wxString& key)
{
  wxString keyUpper = key.Upper();
  CPointGlyph* value = dynamic_cast<CPointGlyph*>(Exists((const char *)(keyUpper)));
  if (value != NULL)
  {
    return value->m_name;
  }

  return "";

}

//----------------------------------------
void CMapPointGlyph::NamesToArrayString(wxArrayString& array)
{

  CMapPointGlyph::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CPointGlyph* value = dynamic_cast<CPointGlyph*>(it->second);
    if (value != NULL)
    {
      array.Add(value->m_name.c_str());
    }
  }


}
//----------------------------------------
void CMapPointGlyph::NamesToMapString(CStringMap& strMap)
{
  CMapPointGlyph::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CPointGlyph* value = dynamic_cast<CPointGlyph*>(it->second);
    if (value != NULL)
    {
      strMap.Insert(it->first, (const char *)(value->m_name));
    }
  }

}
//----------------------------------------
void CMapPointGlyph::GlyphToMap(CUIntMap& uintMap)
{
  CMapPointGlyph::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CPointGlyph* value = dynamic_cast<CPointGlyph*>(it->second);
    if (value != NULL)
    {
      uintMap.Insert(it->first, value->m_id);
    }
  }

}

//----------------------------------------
PointGlyph CMapPointGlyph::GetPointGlyph(CFileParams& params,
		                                     int32_t index /*= 0*/,
		                                     const string& keyword /*= "DISPLAY_PointGlyph"*/,
		                                     PointGlyph defaultValue  /*= displayFULL*/)
{
  CUIntMap glyphs;
  GlyphToMap(glyphs);

  string	valueName;
  uint32_t	valueGlyph;
  CParameter* p = params.m_mapParam.Exists(keyword);
  if (p == NULL)
  {
    return defaultValue;
  }

  p->GetValue(valueGlyph,
              valueName,
	      glyphs,
	      index,
	      defaultValue);
  
  return static_cast<PointGlyph>(valueGlyph);
}



//-------------------------------------------------------------
//------------------- CStipplePattern class --------------------
//-------------------------------------------------------------
CStipplePattern::CStipplePattern(StipplePattern id, const wxString name)
{
  m_name = name;
  m_id = id;
}
//----------------------------------------
CStipplePattern::~CStipplePattern()
{

}


//-------------------------------------------------------------
//------------------- CMapStipplePattern class --------------------
//-------------------------------------------------------------
CMapStipplePattern::CMapStipplePattern()
{
  Insert("DOT", 		new CStipplePattern(displayDOT, "Dot"));
  Insert("DASHTINY", 		new CStipplePattern(displayDASH_TINY, "Dash tiny"));
  Insert("DASH", 		new CStipplePattern(displayDASH, "Dash"));
  Insert("DASHDOT", 		new CStipplePattern(displayDASH_DOT, "Dash Dot"));
  Insert("FULL", 		new CStipplePattern(displayFULL, "Full"));
}


//----------------------------------------
CMapStipplePattern::~CMapStipplePattern()
{

}

//----------------------------------------
CMapStipplePattern& CMapStipplePattern::GetInstance()
{
 static CMapStipplePattern instance;

 return instance;
}
//----------------------------------------
bool CMapStipplePattern::ValidName(const string& name)
{   
  return ValidName(wxString(name.c_str()));
}
//----------------------------------------
bool CMapStipplePattern::ValidName(const wxString& name)
{ 
  wxString nameUpper = name.Upper();
  CStipplePattern* value = dynamic_cast<CStipplePattern*>(Exists((const char *)(nameUpper)));
  return (value != NULL);
}
//----------------------------------------
wxString CMapStipplePattern::StippleToKey(StipplePattern id)
{
  CMapStipplePattern::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CStipplePattern* value = dynamic_cast<CStipplePattern*>(it->second);
    if (value == NULL)
    {
      continue;
    }

    if (id == value->m_id)
    {
      return (it->first).c_str();
    }

  }

  return "";

}
//----------------------------------------
wxString CMapStipplePattern::StippleToName(StipplePattern id)
{
  CMapStipplePattern::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CStipplePattern* value = dynamic_cast<CStipplePattern*>(it->second);
    if (value == NULL)
    {
      continue;
    }

    if (id == value->m_id)
    {
      return value->m_name;
    }

  }

  return "";

}
//----------------------------------------
StipplePattern CMapStipplePattern::NameToStipple(const string& name)
{
  return NameToStipple(wxString(name.c_str()));

}
//----------------------------------------
StipplePattern CMapStipplePattern::NameToStipple(const wxString& name)
{
  CMapStipplePattern::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CStipplePattern* value = dynamic_cast<CStipplePattern*>(it->second);
    if (value == NULL)
    {
      continue;
    }

    if (name == value->m_name)
    {
      return value->m_id;
    }

  }

  return displayFULL;

}
//----------------------------------------
StipplePattern CMapStipplePattern::KeyToStipple(const string& key)
{
  return KeyToStipple(wxString(key.c_str()));

}
//----------------------------------------
StipplePattern CMapStipplePattern::KeyToStipple(const wxString& key)
{
  wxString keyUpper = key.Upper();
  CStipplePattern* value = dynamic_cast<CStipplePattern*>(Exists((const char *)(keyUpper)));
  if (value != NULL)
  {
    return value->m_id;
  }

  return displayFULL;

}

//----------------------------------------
wxString CMapStipplePattern::KeyToName(const string& key)
{
  return KeyToName(wxString(key.c_str()));

}
//----------------------------------------
wxString CMapStipplePattern::KeyToName(const wxString& key)
{
  wxString keyUpper = key.Upper();
  CStipplePattern* value = dynamic_cast<CStipplePattern*>(Exists((const char *)(keyUpper)));
  if (value != NULL)
  {
    return value->m_name;
  }

  return "";

}

//----------------------------------------
void CMapStipplePattern::NamesToArrayString(wxArrayString& array)
{

  CMapStipplePattern::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CStipplePattern* value = dynamic_cast<CStipplePattern*>(it->second);
    if (value != NULL)
    {
      array.Add(value->m_name.c_str());
    }
  }


}
//----------------------------------------
void CMapStipplePattern::NamesToMapString(CStringMap& strMap)
{
  CMapStipplePattern::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CStipplePattern* value = dynamic_cast<CStipplePattern*>(it->second);
    if (value != NULL)
    {
      strMap.Insert(it->first, (const char *)(value->m_name));
    }
  }

}
//----------------------------------------
void CMapStipplePattern::StippleToMap(CUIntMap& uintMap)
{
  CMapStipplePattern::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CStipplePattern* value = dynamic_cast<CStipplePattern*>(it->second);
    if (value != NULL)
    {
      uintMap.Insert(it->first, value->m_id);
    }
  }

}

//----------------------------------------
StipplePattern CMapStipplePattern::GetStipplePattern(CFileParams& params,
		                                     int32_t index /*= 0*/,
		                                     const string& keyword /*= "DISPLAY_STIPPLEPATTERN"*/,
		                                     StipplePattern defaultValue  /*= displayFULL*/)
{
  CUIntMap stipples;
  StippleToMap(stipples);

  string	valueName;
  uint32_t	valueStipple;
  CParameter* p = params.m_mapParam.Exists(keyword);
  if (p == NULL)
  {
    return defaultValue;
  }

  p->GetValue(valueStipple,
              valueName,
	      stipples,
	      index,
	      defaultValue);
  
  return static_cast<StipplePattern>(valueStipple);
}

//-------------------------------------------------------------
//------------------- CMapColor class --------------------
//-------------------------------------------------------------

/* --- Listr of color's name 

  AQUAMARINE,
  BLACK, 
  BLUE,
  BLUE VIOLET,
  BROWN,
  CADET BLUE,
  CORAL,
  CORNFLOWER BLUE,
  CYAN, 
  DARK GREY,
  DARK GREEN, 
  DARK OLIVE GREEN,
  DARK ORCHID,
  DARK SLATE BLUE,
  DARK SLATE GREY, 
  DARK TURQUOISE,
  DIM GREY, 
  FIREBRICK,
  FOREST GREEN, 
  GOLD,
  GOLDENROD,
  GREY,
  GREEN,
  GREEN YELLOW, 
  INDIAN RED,
  KHAKI,
  LIGHT BLUE,
  LIGHT GREY,
  LIGHT STEEL BLUE,
  LIME GREEN,
  MAGENTA,
  MAROON,
  MEDIUM AQUAMARINE,
  MEDIUM BLUE,
  MEDIUM FOREST GREEN,
  MEDIUM GOLDENROD,
  MEDIUM ORCHID,
  MEDIUM SEA GREEN,
  MEDIUM SLATE BLUE,
  MEDIUM SPRING GREEN,
  MEDIUM TURQUOISE,
  MEDIUM VIOLET RED,
  MIDNIGHT BLUE,
  NAVY,
  ORANGE,
  ORANGE RED, 
  ORCHID,
  PALE GREEN,
  PINK,
  PLUM,
  PURPLE,
  RED,
  SALMON,
  SEA GREEN,
  SIENNA,
  SKY BLUE,
  SLATE BLUE, 
  SPRING GREEN,
  STEEL BLUE, 
  TAN,
  THISTLE,
  TURQUOISE, 
  VIOLET,
  VIOLET RED, 
  WHEAT,
  WHITE, 
  YELLOW,
  YELLOW GREEN.
*/


CMapColor::CMapColor()
{
  m_primaryColors.Insert("RED");
  m_primaryColors.Insert("GREEN");
  m_primaryColors.Insert("BLUE");
  m_primaryColors.Insert("MAGENTA");
  m_primaryColors.Insert("ORANGE");
  m_primaryColors.Insert("BROWN");
  m_primaryColors.Insert("DARK OLIVE GREEN");
  m_primaryColors.Insert("CYAN");
  m_primaryColors.Insert("VIOLET");
  m_primaryColors.Insert("NAVY");
  m_primaryColors.Insert("LIGHT BLUE");

  m_nextPrimaryColor = 0;
}


//----------------------------------------
CMapColor::~CMapColor()
{

}

//----------------------------------------
CMapColor& CMapColor::GetInstance()
{
 static CMapColor instance;

 return instance;
}
//----------------------------------------
CVtkColor CMapColor::NextPrimaryColors()
{ 
  CVtkColor value = PrimaryColors(m_nextPrimaryColor);
  m_nextPrimaryColor++;

  return value;
}
//----------------------------------------
CVtkColor CMapColor::PrimaryColors(uint32_t i)
{ 
  if (i >= m_primaryColors.size())
  {
    i = 0;
  }

  return NameToVtkColor(m_primaryColors[i]);
}
//----------------------------------------
bool CMapColor::ValidName(const string& name)
{   
  return ValidName(wxString(name.c_str()));
}
//----------------------------------------
bool CMapColor::ValidName(const wxString& name)
{ 
  wxString nameUpper = name.Upper();
  wxColour colour = m_colourDatabase.Find(nameUpper);
  if (colour.Ok())
  {
    return true;
  }

  return false;
}
//----------------------------------------
wxString CMapColor::ColorToName(const wxColour& id)
{
  return m_colourDatabase.FindName(id);

}
//----------------------------------------
wxString CMapColor::ColorToName(const CVtkColor& id)
{
  return ColorToName(id.GetWXColor());
}

//----------------------------------------
CVtkColor CMapColor::NameToVtkColor(const char* name)
{
  return NameToVtkColor(wxString(name));

}
//----------------------------------------
CVtkColor CMapColor::NameToVtkColor(const string& name)
{
  return NameToVtkColor(wxString(name.c_str()));

}
//----------------------------------------
CVtkColor CMapColor::NameToVtkColor(const wxString& name)
{
  wxColour colour = NameToColour(name);
  if (colour.Ok() == false)
  {
    return CVtkColor();
  }

  return CVtkColor(colour.Red(), colour.Green(), colour.Blue());
}
//----------------------------------------
wxColour CMapColor::NameToColour(const char* name)
{
  return  NameToColour(wxString(name));

}
//----------------------------------------
wxColour CMapColor::NameToColour(const string& name)
{
  return  NameToColour(wxString(name.c_str()));

}
//----------------------------------------
wxColour CMapColor::NameToColour(const wxString& name)
{
  wxString nameUpper = name.Upper();
  return  m_colourDatabase.Find(nameUpper);

}

//----------------------------------------
wxColourData CMapColor::ChooseColor(const CVtkColor& color, wxWindow* parent)
{
  return ChooseColor(color.GetWXColor(), parent);
}
//----------------------------------------
wxColourData CMapColor::ChooseColor(const wxColour& color, wxWindow* parent)
{
  m_clrData.SetColour(color);

  wxColourDialog dialog(parent, &m_clrData);
  dialog.SetTitle(_T("Choose color"));
  if (dialog.ShowModal() == wxID_OK)
  {
      m_clrData = dialog.GetColourData();
  }

  return m_clrData;
}
//----------------------------------------
void CMapColor::NamesToArrayString(wxArrayString& array)
{

  static wxString wxColourNames[] = 
  {
  "AQUAMARINE",
  "BLACK", 
  "BLUE",
  "BLUE VIOLET",
  "BROWN",
  "CADET BLUE",
  "CORAL",
  "CORNFLOWER BLUE",
  "CYAN",
  "DARK GREY",
  "DARK GREEN", 
  "DARK OLIVE GREEN",
  "DARK ORCHID",
  "DARK SLATE BLUE",
  "DARK SLATE GREY",
  "DARK TURQUOISE",
  "DIM GREY",
  "FIREBRICK",
  "FOREST GREEN", 
  "GOLD",
  "GOLDENROD",
  "GREY",
  "GREEN",
  "GREEN YELLOW", 
  "INDIAN RED",
  "KHAKI",
  "LIGHT BLUE",
  "LIGHT GREY",
  "LIGHT STEEL BLUE",
  "LIME GREEN",
  "MAGENTA",
  "MAROON",
  "MEDIUM AQUAMARINE",
  "MEDIUM BLUE",
  "MEDIUM FOREST GREEN",
  "MEDIUM GOLDENROD",
  "MEDIUM ORCHID",
  "MEDIUM SEA GREEN",
  "MEDIUM SLATE BLUE",
  "MEDIUM SPRING GREEN",
  "MEDIUM TURQUOISE",
  "MEDIUM VIOLET RED",
  "MIDNIGHT BLUE",
  "NAVY",
  "ORANGE",
  "ORANGE RED", 
  "ORCHID",
  "PALE GREEN",
  "PINK",
  "PLUM",
  "PURPLE",
  "RED",
  "SALMON",
  "SEA GREEN",
  "SIENNA",
  "SKY BLUE",
  "SLATE BLUE", 
  "SPRING GREEN",
  "STEEL BLUE", 
  "TAN",
  "THISTLE",
  "TURQUOISE", 
  "VIOLET",
  "VIOLET RED", 
  "WHEAT",
  "WHITE", 
  "YELLOW",
  "YELLOW GREEN"
  };

  size_t n;

  for ( n = 0; n < WXSIZEOF(wxColourNames); n++ )
  {
    array.Add(wxColourNames[n]);
  }

}


