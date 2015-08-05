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
#ifndef __MapColor_H__
#define __MapColor_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "MapColor.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//#include "vtkGlyphSource2D.h"

#include "brathl.h"

#include "List.h"
#include "Tools.h" 
#include "Parameter.h" 
#include "FileParams.h" 
using namespace brathl;

#include "VtkColor.h"

#include "wx/arrstr.h"

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
  CPointGlyph(PointGlyph id, const wxString name);
  /// CPointGlyph dtor
  virtual ~CPointGlyph();
  
  wxString m_name;
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
  bool ValidName(const string& name);
  bool ValidName(const wxString& name);

  wxString GlyphToName(PointGlyph id);
  wxString GlyphToKey(PointGlyph id);


  PointGlyph NameToGlyph(const string& name);
  PointGlyph NameToGlyph(const wxString& name);
  PointGlyph KeyToGlyph(const string& name);
  PointGlyph KeyToGlyph(const wxString& name);
  wxString KeyToName(const string& name);
  wxString KeyToName(const wxString& name);


  void NamesToArrayString(wxArrayString& array);
  void NamesToMapString(CStringMap& strMap);
  
  void GlyphToMap(CUIntMap& uintMap);

  PointGlyph GetPointGlyph(CFileParams& params,
		                   int32_t index = 0,
		                   const string& keyword = "DISPLAY_POINTGLYPH",
		                   PointGlyph defaultValue = displayCIRCLE_GLYPH);


protected:


public:


private :


};


//-------------------------------------------------------------
//------------------- CStipplePattern class --------------------
//-------------------------------------------------------------

class CStipplePattern: public CBratObject
{
public:
  /// CStipplePattern ctor
  CStipplePattern(StipplePattern id, const wxString name);
  /// CStipplePattern dtor
  virtual ~CStipplePattern();
  
  wxString m_name;
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
  bool ValidName(const string& name);
  bool ValidName(const wxString& name);

  wxString StippleToName(StipplePattern id);
  wxString StippleToKey(StipplePattern id);

  StipplePattern NameToStipple(const string& name);
  StipplePattern NameToStipple(const wxString& name);
  StipplePattern KeyToStipple(const string& name);
  StipplePattern KeyToStipple(const wxString& name);
  wxString KeyToName(const string& name);
  wxString KeyToName(const wxString& name);


  void NamesToArrayString(wxArrayString& array);
  void NamesToMapString(CStringMap& strMap);
  
  void StippleToMap(CUIntMap& uintMap);

  StipplePattern GetStipplePattern(CFileParams& params,
		                   int32_t index = 0,
		                   const string& keyword = "DISPLAY_STIPPLEPATTERN",
		                   StipplePattern defaultValue = displayFULL);


protected:


public:


private :


};

//-------------------------------------------------------------
//------------------- CMapColor class --------------------
//-------------------------------------------------------------

class CMapColor: public CBratObject
{
public:
  CMapColor();

  virtual ~CMapColor();

  static CMapColor& GetInstance();

  bool ValidName(const char* name);
  bool ValidName(const string& name);
  bool ValidName(const wxString& name);

  wxString ColorToName(const wxColour& id);
  wxString ColorToName(const CVtkColor& id);

  CVtkColor NameToVtkColor(const char* name);
  CVtkColor NameToVtkColor(const string& name);
  CVtkColor NameToVtkColor(const wxString& name);

  wxColour NameToColour(const char* name);
  wxColour NameToColour(const string& name);
  wxColour NameToColour(const wxString& name);

  wxColourData ChooseColor(const CVtkColor& color, wxWindow* parent = NULL);
  wxColourData ChooseColor(const wxColour& color, wxWindow* parent = NULL);


  void NamesToArrayString(wxArrayString& array);

  CVtkColor NextPrimaryColors();
  CVtkColor PrimaryColors(uint32_t i);

  void ResetPrimaryColors() {m_nextPrimaryColor = 0;};

protected:

  wxColourDatabase m_colourDatabase;
  
  wxColourData m_clrData;


public:


private :
  CStringArray m_primaryColors;

  uint32_t m_nextPrimaryColor;



};

#endif
