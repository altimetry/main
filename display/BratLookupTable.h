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
#ifndef __BratLookupTable_H__
#define __BratLookupTable_H__

#include "wx/arrstr.h"
#include "wx/config.h"  // (to let wxWidgets choose a wxConfig class for your platform)
#include "wx/confbase.h"  // (base config class)
#include "wx/fileconf.h" // (wxFileConfig class)
//#include "wx/msw/regconf.h" // (wxRegConfig class)

#include "brathl.h"

#include "vtkLookupTable.h"

#include "BratObject.h"
#include "Tools.h"
#include "List.h"
#include "CallBack.h"
using namespace brathl;

#include "VtkColor.h"


//-------------------------------------------------------------
//------------------- CCustomColor class --------------------
//-------------------------------------------------------------

class CCustomColor : public CBratObject
{

public:
  /// Ctor
  CCustomColor();
  CCustomColor(const CCustomColor& customColor);
  CCustomColor(const CVtkColor& vtkColor, int32_t xValue);

  /// Dtor
  virtual ~CCustomColor();

  CVtkColor* GetVtkColor() {return &m_vtkColor;};
  int32_t GetXValue() {return m_xValue;};
  wxString GetStringXValue() {return wxString( CTools::Format(20, "%d", m_xValue).c_str() );};

  void SetVtkColor(const CVtkColor& vtkColor) {m_vtkColor = vtkColor;};
  void SetXValue(int32_t xValue) {m_xValue = xValue;};

  ///Dump fonction
  //virtual void Dump(ostream& fOut = cerr);

protected:
  void Init();

public:

  CVtkColor m_vtkColor;
  int32_t m_xValue;

};
//-------------------------------------------------------------
//------------------- CBratLookupTable class --------------------
//-------------------------------------------------------------

class CBratLookupTable;
typedef CallBackv< CBratLookupTable, void > CallbackVoid;
typedef CallBack3< CBratLookupTable, double, double, double, double > CallbackFlank;


class CBratLookupTable : public CBratObject
{
public:

  CBratLookupTable();
  CBratLookupTable(CBratLookupTable& lut);

  virtual ~CBratLookupTable();

  vtkLookupTable* GetLookupTable() {return m_vtkLookupTable;};

  string GetCurve() {return m_curve;};
  string GetCurrentFunction() {return m_currentFunction;};
  //void SetCurrentFunction(const string& value) {m_currentFunction = value;};
  string GetResetFunction() {return m_resetFunction;};
//  string GetStd() {return m_std;};

  CObArray* GetGrad() {return &m_grad;};

  CObArray* GetCust() {return &m_cust;};
  int32_t GetMaxCustomXValue();

  const wxArrayString* GetColorTableList() {return &m_colorTableList;};

  string MethodToLabeledMethod(const string& method);
  bool IsValidMethod(const string& methodName);

  string CurveToLabeledCurve(const string& curve);
  bool IsValidCurve(const string& curve);

  //const CObMap* GetNameToMethod() {return &m_nameToMethod;};
  void ExecMethod(const wxString& methodName);
  void ExecMethod(const string& methodName);

  void ExecMethodDefaultColorTable();

  void ExecCurveMethod(const string& curve);

  bool IsCurrentCustom() {return m_currentFunction == m_customFunction;};
  bool IsCurrentGradient() {return m_currentFunction == m_gradientFunction;};
  bool IsCurrentReset() {return m_currentFunction == m_resetFunction;};

  wxString GetDefaultColorTable() {return m_defaultColorTable;};

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

  void Gradient(const CVtkColor& c1, const CVtkColor& c2);
  void Gradient();

  void Custom(const CObArray& cust);
  void Custom(const CCustomColor& c1, const CCustomColor& c2);
  void Custom();

  static void DupCustMap(const CObArray& custSrc, CObArray& custDest);

  void SaveToFile(const wxString& fileName);

  bool LoadFromFile(const wxString& fileName);

  CBratLookupTable& operator=(CBratLookupTable& lut);

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr);

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

  void DrawGradient(const CVtkColor& c1, const CVtkColor& c2, int32_t i1, int32_t i2);

  void SaveGradToFile(wxFileConfig& file);
  void SaveCustToFile(wxFileConfig& file);

  bool LoadGradFromFile(wxFileConfig& file);
  bool LoadCustFromFile(wxFileConfig& file);

  void HandleLoadError(bool bOk, const wxString& entry);
  void HandleLoadColorError(bool bOk, const wxString& entry, double& color);

  int32_t InsertCustomColor(CCustomColor *color);

protected:

  vtkLookupTable* m_vtkLookupTable;

  string m_currentFunction;
  //string m_std;

  string m_curve;
  CStringArray m_curveNames;

  CObMap m_nameToMethod;

  wxArrayString m_colorTableList;
  wxString m_defaultColorTable;

  string m_resetFunction;

  string m_gradientFunction;
  string m_customFunction;

  CallbackFlank* UpFlank;
  CallbackFlank* DownFlank;

  CObArray m_cust;
  CObArray m_grad;

  wxString m_fileName;
/*
    def Gradient(self, c1, c2):
        self.Map=[]
        self.Grad=[c1, c2]
        self.Std="Reset"
        self.CurrentFunction="self.Gradient(" + str(c1) + ", " + str(c2) + ")"
        self.__DrawGradient(c1, c2, 0, self.VTKOBJECT.GetNumberOfTableValues())


    def Custom(self, map):
        self.Map=map
        self.Grad=[]
        self.Std="Reset"
        self.CurrentFunction="self.Custom(" + str(self.Map) + ")"
        l = len(map)
        u = self.VTKOBJECT.GetNumberOfTableValues()/float((map[l-1][0]))
        for i in range(l-1):
            self.__DrawGradient(map[i][1], map[i+1][1], map[i][0]*u, map[i+1][0]*u)



    def __DrawGradient(self, c1, c2, i1, i2):
        for i in range(int(i1), int(i2)):
            r =  min(c1[0] + c1[0] * self.__downflank(float(i1), float(i2), i) + c2[0] * self.__upflank(float(i1), float(i2), i), 1.)
            g =  min(c1[1] + c1[1] * self.__downflank(float(i1), float(i2), i) + c2[1] * self.__upflank(float(i1), float(i2), i), 1.)
            b =  min(c1[2] + c1[2] * self.__downflank(float(i1), float(i2), i) + c2[2] * self.__upflank(float(i1), float(i2), i), 1.)
            a =  min(c1[3] + c1[3] * self.__downflank(float(i1), float(i2), i) + c2[3] * self.__upflank(float(i1), float(i2), i), 1.)

            self.VTKOBJECT.SetTableValue(i, r, g, b, a)


    def FromFile(self, filename):
        clutfile = file(filename, 'r')
        cline = [l.strip() for l in clutfile]
        clutfile.close()

        self.VTKOBJECT.SetNumberOfColors(int(cline[1]))
        exec("self.SetCurve" + cline[3] + "()")


        exec("self.Map = " + cline[4])
        exec("self.Grad = " + cline[5])
        self.Std =  cline[6]
	exec(cline[2])


    def SaveToFile(self, filename):
        f = file(filename, "w")
	f.write('VISAN CLUT ' + str(visan.__version__) + '\n')
        f.write(str(self.VTKOBJECT.GetNumberOfTableValues()) + '\n')
        f.write(self.CurrentFunction + "\n")
        f.write(self.Curve + "\n")
        f.write(str(self.Map) + "\n")
        f.write(str(self.Grad) + "\n")
        f.write(self.Std + "\n")
        f.close()


    def SetFacets(self, f):
        old = self.VTKOBJECT.GetNumberOfTableValues()
        self.VTKOBJECT.SetNumberOfTableValues(f)
        for i in self.Map:
            i[0] = int(i[0] * (float(f)/float(old)))

        if self.CurrentFunction.find("Custom")>=0:
            self.CurrentFunction = "self.Custom(" + str(self.Map) + ")"
        self.Update()


    def Clone(self):
	v = VisanLookupTable()
	v.Map = self.Map
	v.Grd = self.Grad
	v.Std = self.Std
	v.CurrentFunction = self.CurrentFunction
	exec("v.SetCurve" + self.Curve + "()")
	v.VTKOBJECT.SetNumberOfTableValues(self.VTKOBJECT.GetNumberOfTableValues())
	v.VTKOBJECT.SetTableRange(self.VTKOBJECT.GetTableRange())
	v.Update()

	return v
*/
};

#endif


