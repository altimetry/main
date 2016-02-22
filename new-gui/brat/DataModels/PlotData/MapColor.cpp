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

#include "libbrathl/Tools.h" 
using namespace brathl;

#include "MapColor.h" 

//-------------------------------------------------------------
//------------------- CPointGlyph class --------------------
//-------------------------------------------------------------
CPointGlyph::CPointGlyph(PointGlyph id, const std::string name)
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
bool CMapPointGlyph::ValidName(const std::string& name)
{ 
  std::string nameUpper = ToUpperCopy( name );
  CPointGlyph* value = dynamic_cast<CPointGlyph*>(Exists(nameUpper));
  return (value != NULL);
}
//----------------------------------------
std::string CMapPointGlyph::GlyphToName(PointGlyph id)
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
std::string CMapPointGlyph::GlyphToKey(PointGlyph id)
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
PointGlyph CMapPointGlyph::NameToGlyph(const std::string& name)
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
PointGlyph CMapPointGlyph::KeyToGlyph(const std::string& key)
{
  std::string keyUpper = ToUpperCopy( key );
  CPointGlyph* value = dynamic_cast<CPointGlyph*>(Exists(keyUpper));
  if (value != NULL)
  {
    return value->m_id;
  }

  return displayCIRCLE_GLYPH;

}
//----------------------------------------
std::string CMapPointGlyph::KeyToName(const std::string& key)
{
  std::string keyUpper = ToUpperCopy( key );
  CPointGlyph* value = dynamic_cast<CPointGlyph*>(Exists(keyUpper));
  if (value != NULL)
  {
    return value->m_name;
  }

  return "";

}

//----------------------------------------
void CMapPointGlyph::NamesToArrayString( std::vector<std::string>& v )
{
	for ( CMapPointGlyph::iterator it = begin(); it != end(); it++ )
	{
		CPointGlyph* value = dynamic_cast<CPointGlyph*>( it->second );
		if ( value != NULL )
		{
			v.push_back( value->m_name );
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
      strMap.Insert(it->first, value->m_name);
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
		                                     const std::string& keyword /*= "DISPLAY_PointGlyph"*/,
		                                     PointGlyph defaultValue  /*= displayFULL*/)
{
  CUIntMap glyphs;
  GlyphToMap(glyphs);

  std::string	valueName;
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
CStipplePattern::CStipplePattern(StipplePattern id, const std::string name)
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
bool CMapStipplePattern::ValidName(const std::string& name)
{ 
  std::string nameUpper = ToUpperCopy( name );
  CStipplePattern* value = dynamic_cast<CStipplePattern*>(Exists(nameUpper));
  return (value != NULL);
}
//----------------------------------------
std::string CMapStipplePattern::StippleToKey(StipplePattern id)
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
std::string CMapStipplePattern::StippleToName(StipplePattern id)
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
StipplePattern CMapStipplePattern::NameToStipple(const std::string& name)
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
StipplePattern CMapStipplePattern::KeyToStipple(const std::string& key)
{
  std::string keyUpper = ToUpperCopy( key );
  CStipplePattern* value = dynamic_cast<CStipplePattern*>(Exists(keyUpper));
  if (value != NULL)
  {
    return value->m_id;
  }

  return displayFULL;

}

//----------------------------------------
std::string CMapStipplePattern::KeyToName(const std::string& key)
{
  std::string keyUpper = ToUpperCopy( key );
  CStipplePattern* value = dynamic_cast<CStipplePattern*>(Exists(keyUpper));
  if (value != NULL)
  {
    return value->m_name;
  }

  return "";

}

//----------------------------------------
void CMapStipplePattern::NamesToArrayString( std::vector<std::string>& v )
{
	for ( CMapStipplePattern::iterator it = begin(); it != end(); it++ )
	{
		CStipplePattern* value = dynamic_cast<CStipplePattern*>( it->second );
		if ( value != NULL )
		{
			v.push_back( value->m_name );
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
      strMap.Insert(it->first, value->m_name);
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
		                                     const std::string& keyword /*= "DISPLAY_STIPPLEPATTERN"*/,
		                                     StipplePattern defaultValue  /*= displayFULL*/)
{
  CUIntMap stipples;
  StippleToMap(stipples);

  std::string	valueName;
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

void CMapColor::NamesToArrayString( std::vector<std::string>& v )
{
	v =
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
}



static const struct ColourDesc
{
	const char *name;
	unsigned char r, g, b;
}
ColourTable[] =
{
	{ "AQUAMARINE", 112, 219, 147 },
	{ "BLACK", 0, 0, 0 },
	{ "BLUE", 0, 0, 255 },
	{ "BLUE VIOLET", 159, 95, 159 },
	{ "BROWN", 165, 42, 42 },
	{ "CADET BLUE", 95, 159, 159 },
	{ "CORAL", 255, 127, 0 },
	{ "CORNFLOWER BLUE", 66, 66, 111 },
	{ "CYAN", 0, 255, 255 },
	{ "DARK GREY", 47, 47, 47 },   // ?

	{ "DARK GREEN", 47, 79, 47 },
	{ "DARK OLIVE GREEN", 79, 79, 47 },
	{ "DARK ORCHID", 153, 50, 204 },
	{ "DARK SLATE BLUE", 107, 35, 142 },
	{ "DARK SLATE GREY", 47, 79, 79 },
	{ "DARK TURQUOISE", 112, 147, 219 },
	{ "DIM GREY", 84, 84, 84 },
	{ "FIREBRICK", 142, 35, 35 },
	{ "FOREST GREEN", 35, 142, 35 },
	{ "GOLD", 204, 127, 50 },
	{ "GOLDENROD", 219, 219, 112 },
	{ "GREY", 128, 128, 128 },
	{ "GREEN", 0, 255, 0 },
	{ "GREEN YELLOW", 147, 219, 112 },
	{ "INDIAN RED", 79, 47, 47 },
	{ "KHAKI", 159, 159, 95 },
	{ "LIGHT BLUE", 191, 216, 216 },
	{ "LIGHT GREY", 192, 192, 192 },
	{ "LIGHT STEEL BLUE", 143, 143, 188 },
	{ "LIME GREEN", 50, 204, 50 },
	{ "LIGHT MAGENTA", 255, 119, 255 },
	{ "MAGENTA", 255, 0, 255 },
	{ "MAROON", 142, 35, 107 },
	{ "MEDIUM AQUAMARINE", 50, 204, 153 },
	{ "MEDIUM GREY", 100, 100, 100 },
	{ "MEDIUM BLUE", 50, 50, 204 },
	{ "MEDIUM FOREST GREEN", 107, 142, 35 },
	{ "MEDIUM GOLDENROD", 234, 234, 173 },
	{ "MEDIUM ORCHID", 147, 112, 219 },
	{ "MEDIUM SEA GREEN", 66, 111, 66 },
	{ "MEDIUM SLATE BLUE", 127, 0, 255 },
	{ "MEDIUM SPRING GREEN", 127, 255, 0 },
	{ "MEDIUM TURQUOISE", 112, 219, 219 },
	{ "MEDIUM VIOLET RED", 219, 112, 147 },
	{ "MIDNIGHT BLUE", 47, 47, 79 },
	{ "NAVY", 35, 35, 142 },
	{ "ORANGE", 204, 50, 50 },
	{ "ORANGE RED", 255, 0, 127 },
	{ "ORCHID", 219, 112, 219 },
	{ "PALE GREEN", 143, 188, 143 },
	{ "PINK", 255, 192, 203 },
	{ "PLUM", 234, 173, 234 },
	{ "PURPLE", 176, 0, 255 },
	{ "RED", 255, 0, 0 },
	{ "SALMON", 111, 66, 66 },
	{ "SEA GREEN", 35, 142, 107 },
	{ "SIENNA", 142, 107, 35 },
	{ "SKY BLUE", 50, 153, 204 },
	{ "SLATE BLUE", 0, 127, 255 },
	{ "SPRING GREEN", 0, 255, 127 },
	{ "STEEL BLUE", 35, 107, 142 },
	{ "TAN", 219, 147, 112 },
	{ "THISTLE", 216, 191, 216 },
	{ "TURQUOISE", 173, 234, 234 },
	{ "VIOLET", 79, 47, 79 },
	{ "VIOLET RED", 204, 50, 153 },
	{ "WHEAT", 216, 216, 191 },
	{ "WHITE", 255, 255, 255 },
	{ "YELLOW", 255, 255, 0 },
	{ "YELLOW GREEN", 153, 204, 50 }
};

static const ColourDesc* FindColor( const std::string &name )
{
    // make the comparison case insensitive and also match both grey and gray

    std::string colName = ToUpperCopy( name );
    std::string colNameAlt = colName;
	replace( colNameAlt, "GRAY", "GREY" );

	for ( auto &desc : ColourTable )
		if ( desc.name == colNameAlt )
			return &desc;

    return nullptr;
}

CPlotColor CMapColor::NameToPlotColor( const std::string& name )
{
	const ColourDesc *desc =  FindColor( name );
	if ( !desc )
		return CPlotColor();

	return CPlotColor( desc->r, desc->g, desc->b );
}
