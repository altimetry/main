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

#include <qwt_symbol.h>


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


enum EPointGlyph 
{ 
	//eNoSymbol = QwtSymbol::NoSymbol, eFirstPointGlyph = eNoSymbol,
	eEllipse = QwtSymbol::Ellipse, eFirstPointGlyph = eEllipse,
	eRect = QwtSymbol::Rect, 
	eDiamond = QwtSymbol::Diamond, 
	eTriangle = QwtSymbol::Triangle, 
	eDTriangle = QwtSymbol::DTriangle, 
	eUTriangle = QwtSymbol::UTriangle, 
	eLTriangle = QwtSymbol::LTriangle, 
	eRTriangle = QwtSymbol::RTriangle, 
	eCross = QwtSymbol::Cross, 
	eXCross = QwtSymbol::XCross, 
	eHLine = QwtSymbol::HLine, 
	eVLine = QwtSymbol::VLine, 
	eStar1 = QwtSymbol::Star1, 
	eStar2 = QwtSymbol::Star2, 
	eHexagon = QwtSymbol::Hexagon, 

	EPointGlyph_size
};


//v3 compatibility

const EPointGlyph displayNO_GLYPH = EPointGlyph_size;
const EPointGlyph displayVERTEX_GLYPH = eDTriangle;
const EPointGlyph displayDASH_GLYPH = eHLine;
const EPointGlyph displayCROSS_GLYPH = eCross;
const EPointGlyph displayTHICKCROSS_GLYPH = eXCross;
const EPointGlyph displayTRIANGLE_GLYPH = eTriangle;
const EPointGlyph displaySQUARE_GLYPH = eRect;
const EPointGlyph displayCIRCLE_GLYPH = eEllipse;
const EPointGlyph displayDIAMOND_GLYPH = eDiamond;
const EPointGlyph displayARROW_GLYPH = eUTriangle;
const EPointGlyph displayTHICKARROW_GLYPH = eStar1;
const EPointGlyph displayHOOKEDARROW_GLYPH = eStar2;



//-------------------------------------------------------------
//------------------- CPointGlyph class --------------------
//-------------------------------------------------------------

//v3 compatibility
//
class CPointGlyph : public CBratObject
{
public:
	std::string m_name;
	EPointGlyph m_id;

	CPointGlyph( EPointGlyph id, const std::string name )
	{
		m_name = name;
		m_id = id;
	}

	virtual ~CPointGlyph()
	{}
};

//-------------------------------------------------------------
//------------------- CMapPointGlyph class --------------------
//-------------------------------------------------------------

class CMapPointGlyph : public CObMap
{
public:

	//v4 must use only these functions; all remaining 
	//class code id for v3 compatibility

	static const std::string& Name( EPointGlyph e );
	static const std::vector<std::string>& Names();

public:
	//v3 compatibility

	CMapPointGlyph();

	virtual ~CMapPointGlyph();

	static CMapPointGlyph& GetInstance();

	bool ValidName( const char* name );
	bool ValidName( const std::string& name );

	std::string GlyphToName( EPointGlyph id );
	std::string GlyphToKey( EPointGlyph id );


	EPointGlyph NameToGlyph( const std::string& name );
	EPointGlyph KeyToGlyph( const std::string& name );
	std::string KeyToName( const std::string& name );

	void NamesToArrayString( std::vector<std::string>& v );
	void NamesToMapString( CStringMap& strMap );

	void GlyphToMap( CUIntMap& uintMap );

	EPointGlyph GetPointGlyph( CFileParams& params,
		int32_t index = 0,
		const std::string& keyword = "DISPLAY_PointGlyph",
		EPointGlyph defaultValue = displayCIRCLE_GLYPH );
};





//-------------------------------------------------------------
//------------------- CStipplePattern class --------------------
//-------------------------------------------------------------


#if defined(BRAT_V3)

enum EStipplePattern
{
	displayDOT = 0x8888U,
	displayDASH_TINY = 0xF0F0U,
	displayDASH = 0xFF00U,
	displayDASH_DOT = 0xFF3CU,
    displayFULL = 0xFFFFU,

    EStipplePattern_size
};

#else

enum EStipplePattern
{
	//eNoPen = Qt::PenStyle::NoPen, eFirstStipplePattern = eNoPen,
	eSolidLine = Qt::PenStyle::SolidLine - 1, eFirstStipplePattern = eSolidLine,
	eDashLine = Qt::PenStyle::DashLine - 1,
	eDotLine = Qt::PenStyle::DotLine - 1,
	eDashDotLine = Qt::PenStyle::DashDotLine -1, 
	eDashDotDotLine = Qt::PenStyle::DashDotDotLine -1,

	EStipplePattern_size
};


const EStipplePattern displayDOT = eDotLine;
const EStipplePattern displayDASH_TINY = eDashDotDotLine;
const EStipplePattern displayDASH = eDashLine;
const EStipplePattern displayDASH_DOT = eDashDotLine;
const EStipplePattern displayFULL = eSolidLine;

#endif


class CStipplePattern : public CBratObject
{
public:

	CStipplePattern( EStipplePattern id, const std::string name );

	virtual ~CStipplePattern();

	std::string m_name;
	EStipplePattern m_id;
};


//-------------------------------------------------------------
//------------------- CMapStipplePattern class --------------------
//-------------------------------------------------------------

class CMapStipplePattern : public CObMap
{
public:
	//v4 must use only these functions; all remaining 
	//class code id for v3 compatibility

	static const std::string& Name( EStipplePattern e );
	static const std::vector<std::string>& Names();

public:
	//v3 compatibility

	CMapStipplePattern();

	virtual ~CMapStipplePattern();

	static CMapStipplePattern& GetInstance();

	bool ValidName( const char* name );
	bool ValidName( const std::string& name );

	std::string StippleToName( EStipplePattern id );
	std::string StippleToKey( EStipplePattern id );

	EStipplePattern NameToStipple( const std::string& name );
	EStipplePattern KeyToStipple( const std::string& name );
	std::string KeyToName( const std::string& name );

	void NamesToArrayString( std::vector<std::string>& );
	void NamesToMapString( CStringMap& strMap );

	void StippleToMap( CUIntMap& uintMap );

	EStipplePattern GetStipplePattern( CFileParams& params,
		int32_t index = 0,
		const std::string& keyword = "DISPLAY_STIPPLEPATTERN",
		EStipplePattern defaultValue = displayFULL );
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
			i = i % m_primaryColors.size();		//v3 i = 0;
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
