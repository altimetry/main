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


#if defined (__unix__) && defined(__DEPRECATED)
#define __DEPRECATED_DEFINED __DEPRECATED
#undef __DEPRECATED
#endif

#include <vtkLookupTable.h>

#if defined (__unix__) && defined(__DEPRECATED_DEFINED)
#define __DEPRECATED __DEPRECATED_DEFINED
#endif



#include "libbrathl/brathl.h"

#include "libbrathl/Tools.h"
#include "libbrathl/List.h"
#include "libbrathl/CallBack.h"
using namespace brathl;

#include "vtkBratColor.h"



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
  CCustomColor(const CVtkColor& vtkColor, int32_t xValue);

  /// Dtor
  virtual ~CCustomColor();

  CVtkColor* GetVtkColor() {return &m_vtkColor;};
  int32_t GetXValue() {return m_xValue;};
  std::string GetStringXValue() {return std::string( CTools::Format(20, "%d", m_xValue).c_str() );};

  void SetVtkColor(const CVtkColor& vtkColor) {m_vtkColor = vtkColor;};
  void SetXValue(int32_t xValue) {m_xValue = xValue;};

  ///Dump fonction
  //virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  void Init();

public:

  CVtkColor m_vtkColor;
  int32_t m_xValue;

};
//-------------------------------------------------------------
//------------------- CBratLookupTable class --------------------
//-------------------------------------------------------------

typedef CallBackv< CBratLookupTable, void > CallbackVoid;
typedef CallBack3< CBratLookupTable, double, double, double, double > CallbackFlank;


class CBratLookupTable : public CBratObject
{
	static const std::string ENTRY_NUMCOLORS()	{ return "ColorTable/NumberOfColors"; }
	static const std::string ENTRY_CURRENTFCT() { return "ColorTable/CurrentFunction"; }
	static const std::string ENTRY_CURVE()		{ return "ColorTable/Curve"; }
	//const std::string ENTRY_STD = "ColorTable/Std";
	static const std::string GROUP_COLOR()		{ return "ColorDef"; }

	static const std::string CURVE_LINEAR()		{ return "Linear"; }
	static const std::string CURVE_SQRT()		{ return "SQRT"; }
	static const std::string CURVE_COSINUS()	{ return "Cosinus"; }

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
  //void SetCurrentFunction(const std::string& value) {m_currentFunction = value;};
  std::string GetResetFunction() {return m_resetFunction;};
//  std::string GetStd() {return m_std;};

  const CObArray* GetGrad() const {return &m_grad;}
  const CObArray* GetCust() const {return &m_cust;}
  CObArray* GetCust() {return &m_cust;}
  int32_t GetMaxCustomXValue();

  const std::vector<std::string >* GetColorTableList() {return &m_colorTableList;}

  std::string MethodToLabeledMethod(const std::string& method);
  bool IsValidMethod(const std::string& methodName);

  std::string CurveToLabeledCurve(const std::string& curve);
  bool IsValidCurve(const std::string& curve);

  //const CObMap* GetNameToMethod() {return &m_nameToMethod;};
  void ExecMethod(const std::string& methodName);
  //femm void ExecMethod(const std::string& methodName);

  void ExecMethodDefaultColorTable();

  void ExecCurveMethod(const std::string& curve);

  bool IsCurrentCustom() {return m_currentFunction == m_customFunction;};
  bool IsCurrentGradient() {return m_currentFunction == m_gradientFunction;};
  bool IsCurrentReset() {return m_currentFunction == m_resetFunction;};

  std::string GetDefaultColorTable() {return m_defaultColorTable;};

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

  void SaveToFile(const std::string& fileName) { return ::SaveToFile( *this, fileName );  }
  bool LoadFromFile( const std::string& fileName ) { return ::LoadFromFile( *this, fileName );  }

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

  void DrawGradient(const CVtkColor& c1, const CVtkColor& c2, int32_t i1, int32_t i2);

  //femmTODO - begin - review wxInterface
  //void SaveGradToFile(wxFileConfig& file);
  //void SaveCustToFile(wxFileConfig& file);

  //bool LoadGradFromFile(wxFileConfig& file);	moved to wxInterface
  //bool LoadCustFromFile(wxFileConfig& file);	moved to wxInterface

  //void HandleLoadError(bool bOk, const std::string& entry);					 	moved to wxInterface
  //void HandleLoadColorError(bool bOk, const std::string& entry, double& color);	moved to wxInterface
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


