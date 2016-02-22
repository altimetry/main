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
#ifndef DATA_MODELS_PLOT_DATA_MAPCOLOR_H
#define DATA_MODELS_PLOT_DATA_MAPCOLOR_H


#if defined ChooseColor
#undef ChooseColor
#endif

#include "libbrathl/brathl.h"

#include "libbrathl/List.h"
#include "libbrathl/Tools.h" 
#include "libbrathl/Parameter.h" 
#include "libbrathl/FileParams.h" 
using namespace brathl;

#if defined(BRAT_V3)
#include "wx/arrstr.h"
#include "wx/colourdata.h"
#include "wx/colordlg.h"
#include "wx/window.h"
#endif
#include "PlotColor.h"

/*
enum PointGlyph{
        displayNO_GLYPH = VTK_NO_GLYPH,
        displayVERTEX_GLYPH = VTK_VERTEX_GLYPH,
        displayDASH_GLYPH = VTK_DASH_GLYPH,
        displayCROSS_GLYPH = VTK_CROSS_GLYPH,
        displayTHICKCROSS_GLYPH = VTK_THICKCROSS_GLYPH,
        displayTRIANGLE_GLYPH = VTK_TRIANGLE_GLYPH,
        displaySQUARE_GLYPH = VTK_SQUARE_GLYPH,
        displayCIRCLE_GLYPH = VTK_CIRCLE_GLYPH,
        displayDIAMOND_GLYPH = VTK_DIAMOND_GLYPH,
        displayARROW_GLYPH = VTK_ARROW_GLYPH,
        displayTHICKARROW_GLYPH = VTK_THICKARROW_GLYPH,
        displayHOOKEDARROW_GLYPH = VTK_HOOKEDARROW_GLYPH
};
*/
enum PointGlyph{
        displayNO_GLYPH = 0,
        displayVERTEX_GLYPH = 1,
        displayDASH_GLYPH = 2,
        displayCROSS_GLYPH = 3,
        displayTHICKCROSS_GLYPH = 4,
        displayTRIANGLE_GLYPH = 5,
        displaySQUARE_GLYPH = 6,
        displayCIRCLE_GLYPH = 7,
        displayDIAMOND_GLYPH = 8,
        displayARROW_GLYPH = 9,
        displayTHICKARROW_GLYPH = 10,
        displayHOOKEDARROW_GLYPH = 11
};

enum StipplePattern{
	displayDOT = 0x8888U,
	displayDASH_TINY = 0xF0F0U,
	displayDASH = 0xFF00U,
	displayDASH_DOT = 0xFF3CU,
        displayFULL = 0xFFFFU
};


//-------------------------------------------------------------
//------------------- CPointGlyph class --------------------
//-------------------------------------------------------------

class CPointGlyph: public CBratObject
{
public:
  /// CPointGlyph ctor
  CPointGlyph(PointGlyph id, const std::string name);
  /// CPointGlyph dtor
  virtual ~CPointGlyph();
  
  std::string m_name;
  PointGlyph m_id;
};

//-------------------------------------------------------------
//------------------- CMapPointGlyph class --------------------
//-------------------------------------------------------------

class CMapPointGlyph: public CObMap
{
public:
  CMapPointGlyph();

  virtual ~CMapPointGlyph();

  static CMapPointGlyph& GetInstance();

  bool ValidName(const char* name);
  bool ValidName(const std::string& name);

  std::string GlyphToName(PointGlyph id);
  std::string GlyphToKey(PointGlyph id);


  PointGlyph NameToGlyph(const std::string& name);
  PointGlyph KeyToGlyph(const std::string& name);
  std::string KeyToName(const std::string& name);

  void NamesToArrayString(std::vector<std::string>& v);
  void NamesToMapString(CStringMap& strMap);
  
  void GlyphToMap(CUIntMap& uintMap);

  PointGlyph GetPointGlyph(CFileParams& params,
		                   int32_t index = 0,
		                   const std::string& keyword = "DISPLAY_POINTGLYPH",
		                   PointGlyph defaultValue = displayCIRCLE_GLYPH);
};


//-------------------------------------------------------------
//------------------- CStipplePattern class --------------------
//-------------------------------------------------------------

class CStipplePattern: public CBratObject
{
public:
  /// CStipplePattern ctor
  CStipplePattern(StipplePattern id, const std::string name);
  /// CStipplePattern dtor
  virtual ~CStipplePattern();
  
  std::string m_name;
  StipplePattern m_id;
};


//-------------------------------------------------------------
//------------------- CMapStipplePattern class --------------------
//-------------------------------------------------------------

class CMapStipplePattern: public CObMap
{
public:
  CMapStipplePattern();

  virtual ~CMapStipplePattern();

  static CMapStipplePattern& GetInstance();

  bool ValidName(const char* name);
  bool ValidName(const std::string& name);

  std::string StippleToName(StipplePattern id);
  std::string StippleToKey(StipplePattern id);

  StipplePattern NameToStipple(const std::string& name);
  StipplePattern KeyToStipple(const std::string& name);
  std::string KeyToName(const std::string& name);

  void NamesToArrayString(std::vector<std::string>&);
  void NamesToMapString(CStringMap& strMap);
  
  void StippleToMap(CUIntMap& uintMap);

  StipplePattern GetStipplePattern(CFileParams& params,
		                   int32_t index = 0,
		                   const std::string& keyword = "DISPLAY_STIPPLEPATTERN",
		                   StipplePattern defaultValue = displayFULL);
};

//-------------------------------------------------------------
//------------------- CMapColor class --------------------
//-------------------------------------------------------------


class CMapColor : public CBratObject
{
	CStringArray m_primaryColors;
	uint32_t m_nextPrimaryColor;

	CMapColor()
	{
		m_primaryColors.Insert( "RED" );
		m_primaryColors.Insert( "GREEN" );
		m_primaryColors.Insert( "BLUE" );
		m_primaryColors.Insert( "MAGENTA" );
		m_primaryColors.Insert( "ORANGE" );
		m_primaryColors.Insert( "BROWN" );
		m_primaryColors.Insert( "DARK OLIVE GREEN" );
		m_primaryColors.Insert( "CYAN" );
		m_primaryColors.Insert( "VIOLET" );
		m_primaryColors.Insert( "NAVY" );
		m_primaryColors.Insert( "LIGHT BLUE" );

		m_nextPrimaryColor = 0;
	}


public:
	virtual ~CMapColor()
	{}

	static CMapColor& GetInstance()
	{
		static CMapColor instance;
		return instance;
	}

    CPlotColor NextPrimaryColors()
	{ 
		CPlotColor value = PrimaryColors( m_nextPrimaryColor );
		m_nextPrimaryColor++;
		return value;
	}

    CPlotColor PrimaryColors( uint32_t i )
	{ 
		if (i >= m_primaryColors.size())
		{
			i = 0;
		}

		return NameToPlotColor(m_primaryColors[i]);
	}

    void ResetPrimaryColors() { m_nextPrimaryColor = 0; }

	CPlotColor NameToPlotColor( const std::string& name );

	void NamesToArrayString( std::vector<std::string>& v );


#if defined(BRAT_V3)

    bool ValidName( const std::string& name )
    {
      std::string nameUpper = ToUpperCopy( name );
      wxColour colour = m_colourDatabase.Find(nameUpper);
      if (colour.Ok())
      {
        return true;
      }

      return false;
    }

	wxString ColorToName( const wxColour& id )
	{
		return m_colourDatabase.FindName(id);
	}
	//wxString ColorToName( const CPlotColor& id )
	//{
	//	return ColorToName(color_cast(id));
	//}

    wxColour NameToColour( const std::string& name )
    {
        std::string nameUpper = ToUpperCopy( name );
        return  m_colourDatabase.Find(nameUpper);
    }

    //wxColourData ChooseColor( const CPlotColor& color, wxWindow* parent = NULL );
    wxColourData ChooseColor( const wxColour& color, wxWindow* parent = NULL )
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


protected:

	wxColourDatabase m_colourDatabase;

	wxColourData m_clrData;

#endif

};




#endif		//DATA_MODELS_PLOT_DATA_MAPCOLOR_H
