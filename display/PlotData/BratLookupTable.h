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
#ifndef __BratLookupTable_H__
#define __BratLookupTable_H__

#include <vtkLookupTable.h>

#include "wx/arrstr.h"
#include "wx/config.h"  // (to let wxWidgets choose a wxConfig class for your platform)
#include "wx/confbase.h"  // (base config class)
#include "wx/fileconf.h" // (wxFileConfig class)
//#include "wx/msw/regconf.h" // (wxRegConfig class)


#include "libbrathl/brathl.h"

#include "libbrathl/Tools.h"
#include "libbrathl/List.h"
#include "libbrathl/CallBack.h"
using namespace brathl;

#include "new-gui/brat/DataModels/PlotData/PlotColor.h"



class CBratLookupTable;

bool LoadFromFile( CBratLookupTable &lut, const std::string& fileName );
void SaveToFile( CBratLookupTable &lut, const std::string& fileName );


//-------------------------------------------------------------
//------------------- CCustomColor class --------------------
//-------------------------------------------------------------

class CCustomColor : public CBratObject
{

public:
  /// Ctor
  CCustomColor();
  CCustomColor(const CCustomColor& customColor);
  CCustomColor(const CPlotColor& vtkColor, int32_t xValue);

  /// Dtor
  virtual ~CCustomColor();

  CPlotColor* GetVtkColor() {return &m_vtkColor;}
  int32_t GetXValue() {return m_xValue;}
  std::string GetStringXValue() {return std::string( CTools::Format(20, "%d", m_xValue).c_str() );}

  void SetVtkColor(const CPlotColor& vtkColor) {m_vtkColor = vtkColor;}
  void SetXValue(int32_t xValue) {m_xValue = xValue;}

  ///Dump fonction
  //virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  void Init();

public:

  CPlotColor m_vtkColor;
  int32_t m_xValue;

};
//-------------------------------------------------------------
//------------------- CBratLookupTable class --------------------
//-------------------------------------------------------------

typedef CallBackv< CBratLookupTable, void > CallbackVoid;
typedef CallBack3< CBratLookupTable, double, double, double, double > CallbackFlank;


class CBratLookupTable : public CBratObject
{
    static const wxString ENTRY_NUMCOLORS()	{ return "ColorTable/NumberOfColors"; }
    static const wxString ENTRY_CURRENTFCT() { return "ColorTable/CurrentFunction"; }
    static const wxString ENTRY_CURVE()		{ return "ColorTable/Curve"; }
	//const std::string ENTRY_STD = "ColorTable/Std";
    static const wxString GROUP_COLOR()		{ return "ColorDef"; }

    static const wxString CURVE_LINEAR()		{ return "Linear"; }
    static const wxString CURVE_SQRT()		{ return "SQRT"; }
    static const wxString CURVE_COSINUS()	{ return "Cosinus"; }

public:
	friend bool LoadFromFile( CBratLookupTable &lut, const std::string& fileName );
	friend void SaveToFile( CBratLookupTable &lut, const std::string& fileName );

public:

  CBratLookupTable();
  CBratLookupTable(const CBratLookupTable& lut);

  virtual ~CBratLookupTable();

  vtkLookupTable* GetLookupTable() const {return m_vtkLookupTable;}

  std::string GetCurve() const {return m_curve;}
  std::string GetCurrentFunction() const {return m_currentFunction;}
  //void SetCurrentFunction(const std::string& value) {m_currentFunction = value;}
  std::string GetResetFunction() {return m_resetFunction;}
//  std::string GetStd() {return m_std;}

  const CObArray* GetGrad() const {return &m_grad;}
  const CObArray* GetCust() const {return &m_cust;}
  CObArray* GetCust() {return &m_cust;}
  int32_t GetMaxCustomXValue();

  const std::vector<std::string >* GetColorTableList() {return &m_colorTableList;}

  std::string MethodToLabeledMethod(const std::string& method);
  bool IsValidMethod(const std::string& methodName);

  std::string CurveToLabeledCurve(const std::string& curve);
  bool IsValidCurve(const std::string& curve);

  //const CObMap* GetNameToMethod() {return &m_nameToMethod;}
  void ExecMethod(const std::string& methodName);
  //femm void ExecMethod(const std::string& methodName);

  void ExecMethodDefaultColorTable();

  void ExecCurveMethod(const std::string& curve);

  bool IsCurrentCustom() {return m_currentFunction == m_customFunction;}
  bool IsCurrentGradient() {return m_currentFunction == m_gradientFunction;}
  bool IsCurrentReset() {return m_currentFunction == m_resetFunction;}

  std::string GetDefaultColorTable() {return m_defaultColorTable;}

  void BlackToWhite();
  void Black();
  void WhiteToBlack();
  void RedToGreen();
  void GreenToRed();
  void Cloud();
  void Rainbow();
  void Rainbow2();
  void Aerosol();
  void Aerosol2();
  void Ozone();
  void Blackbody();

  void SetCurveLinear();
  void SetCurveSQRT();
  void SetCurveCosinus();

  void SetFacets(int32_t f);
  void FacetsCorrection();

  void Reset();

  void Gradient(const CPlotColor& c1, const CPlotColor& c2);
  void Gradient();

  void Custom(const CObArray& cust);
  void Custom(const CCustomColor& c1, const CCustomColor& c2);
  void Custom();

  static void DupCustMap(const CObArray& custSrc, CObArray& custDest);

  void SaveToFile(const std::string& fileName);     // { return ::SaveToFile( *this, fileName );  }
  bool LoadFromFile( const std::string& fileName ); // { return ::LoadFromFile( *this, fileName );  }

  CBratLookupTable& operator=(const CBratLookupTable& lut);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:

  void Init();

  void DupCustMap(const CObArray& cust);
  void DupGradMap(const CObArray& grad);


  void SetCurveUpDown(CallbackFlank* up, CallbackFlank* down);

  double LineUp(double a, double b, double x);
  double LineDown(double a, double b, double x);
  double SqrtUp(double a, double b, double x);
  double SqrtDown(double a, double b, double x);
  double CosUp(double a, double b, double x);
  double CosDown(double a, double b, double x);

  void Update();

  void DrawGradient(const CPlotColor& c1, const CPlotColor& c2, int32_t i1, int32_t i2);

  //femmTODO - begin - review wxInterface
  void SaveGradToFile(wxFileConfig& file);
  void SaveCustToFile(wxFileConfig& file);

  bool LoadGradFromFile(wxFileConfig& file);	//moved to wxInterface
  bool LoadCustFromFile(wxFileConfig& file);	//moved to wxInterface

  void HandleLoadError(bool bOk, const std::string& entry);					 	//moved to wxInterface
  void HandleLoadColorError(bool bOk, const std::string& entry, double& color);	//moved to wxInterface
  //femmTODO - end

  int32_t InsertCustomColor(CCustomColor *color, std::string &warning );

protected:

  vtkLookupTable* m_vtkLookupTable;

  std::string m_currentFunction;
  //std::string m_std;

  std::string m_curve;
  CStringArray m_curveNames;

  CObMap m_nameToMethod;

  std::vector< std::string > m_colorTableList;
  std::string m_defaultColorTable;

  std::string m_resetFunction;

  std::string m_gradientFunction;
  std::string m_customFunction;

  CallbackFlank* UpFlank;
  CallbackFlank* DownFlank;

  CObArray m_cust;
  CObArray m_grad;

  std::string m_fileName;
};

#endif


