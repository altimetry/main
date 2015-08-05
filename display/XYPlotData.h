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

#ifndef __XYPlotData_H__
#define __XYPlotData_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "XYPlotData.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/progdlg.h"

#include "vtkProperty2D.h"
//#include "vtkGlyphSource2D.h"
#include "vtkDataArrayPlotData.h"
#include "vtkDataArray.h"
#include "vtkDoubleArray.h"

#include "brathl.h"

#include "BratObject.h"
#include "Tools.h"
#include "List.h"
#include "Date.h"
#include "Exception.h"
#include "Unit.h"

#include "InternalFiles.h"
#include "InternalFilesZFXY.h"
#include "InternalFilesYFX.h"
using namespace brathl;

#include "VtkColor.h"
#include "vtkList.h"
#include "MapColor.h"
#include "Plot.h"


class CXYPlotData;

// WDR: class declarations
//-------------------------------------------------------------
//------------------- vtkDoubleArrayBrathl class --------------------
//-------------------------------------------------------------
class vtkDoubleArrayBrathl : public vtkDoubleArray
{
public:
  static vtkDoubleArrayBrathl* New();
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ComputeRange(int comp);
  // Description:
  // Return the range of the array values for the given component. 
  // Range is copied into the array provided.
  void GetRange2(double range[2], int comp)
    {
      this->ComputeRange(comp);
      memcpy(range, this->Range2, 2*sizeof(double));
    }
  double* GetRange2(int comp)
    {
      this->ComputeRange(comp);
      return this->Range2;
    }


  // Description:
  // Return the range of the array values for the 0th component. 
  // Range is copied into the array provided.
  double* GetRange2()
    {
      this->ComputeRange(0);
      return this->Range2;
    }
  void GetRange2(double range[2])
    {
      this->GetRange2(range,0);
    }

protected:
  vtkDoubleArrayBrathl(vtkIdType numComp=1);
  ~vtkDoubleArrayBrathl();
  // redefinition with suffix 2 because these attributes (without suffix 2) are private in vtkDataArray
  double Range2[2];

  // 5 components since you can compute the range of components
  // less than 0 to get a magnitude range. ComponentRange[4] is 
  // this magnitude range
  vtkTimeStamp ComponentRangeComputeTime2[5];
  double ComponentRange2[5][2];


};
//-------------------------------------------------------------
//------------------- CRow class --------------------
//-------------------------------------------------------------
/*
class CRow : public CBratObject
{
public:
  CRow(const vtkDoubleArray* vect);
  virtual ~CRow();

  vtkDoubleArray* GetValues() {return m_values;};
  void SetValues(const vtkDoubleArray* vect);

protected:

  vtkDoubleArray* m_values;

};
*/
/*
//-------------------------------------------------------------
//------------------- CRowDouble class --------------------
//-------------------------------------------------------------
class CRowDouble : public CRow
{
public:
  CRowDouble();
  virtual ~CRowDouble();

  CDoubleArray* GetValues() {return &m_values;};

protected:
  CDoubleArray m_values;
};
*/
//-------------------------------------------------------------
//------------------- CPlotArray class --------------------
//-------------------------------------------------------------

class CPlotArray : public CBratObject
{
public:
  CPlotArray();
  virtual ~CPlotArray();
  vtkDoubleArrayBrathl* GetFrameData(uint32_t r);
  vtkDoubleArrayBrathl* GetFrameData(vtkObArray::iterator it);

  void SetFrameData(uint32_t r, double* vect, int32_t size, bool bCopy = true);
  void SetFrameData(uint32_t r, vtkDoubleArrayBrathl* vect);

  void GetRange(double& min, double& max);

  void GetRange(double& min, double& max, uint32_t frame);

  int32_t GetNumberOfFrames() {return m_rows.size();};

  void Normalize(int32_t value = 1);

protected:

  bool m_didCalculateRange;
  double m_min;
  double m_max;

  vtkObArray m_rows;
};

//-------------------------------------------------------------
//------------------- CXYPlotProperty class --------------------
//-------------------------------------------------------------



class CXYPlotProperty : public CBratObject
{
public:

  CXYPlotProperty(CXYPlotData* parent = NULL);
  CXYPlotProperty(CXYPlotProperty& p);

  virtual ~CXYPlotProperty();

  const CXYPlotProperty& operator=(CXYPlotProperty& p);

  vtkProperty2D* GetVtkProperty2D() {return m_vtkProperty2D;};

  CXYPlotData* GetParent() {return m_parent;};
  void SetParent(CXYPlotData* parent) {m_parent = parent;};

  bool GetPoints() {return m_points;};
  void SetPoints();
  void SetPoints(bool value);

  double GetPointSize() {return m_pointSize;};
  void SetPointSize();
  void SetPointSize(double value);

  bool GetLines() {return m_lines;};
  void SetLines();
  void SetLines(bool value);

  double GetLineWidth() {return m_vtkProperty2D->GetLineWidth();};
  void SetLineWidth(double value);

  StipplePattern GetLineStipple() {return static_cast<StipplePattern>(m_vtkProperty2D->GetLineStipplePattern());};
  void SetLineStipple(StipplePattern value) { m_vtkProperty2D->SetLineStipplePattern(value);};

  wxString GetName() {return m_name;};
  void SetName(const wxString& value);

  double GetOpacity() {return m_vtkProperty2D->GetOpacity();};
  void SetOpacity(double value);

  bool GetXLog() {return m_xLog;};
  void SetXLog(bool value);

  bool GetYLog() {return m_yLog;};
  void SetYLog(bool value);

  void GetXRange(double& min, double& max) {min = m_xMin; max = m_xMax;};
  void SetXRange(double min, double max) {m_xMin = min; m_xMax = max;};

  void GetYRange(double& min, double& max) {min = m_yMin; max = m_yMax;};
  void SetYRange(double min, double max) {m_yMin = min; m_yMax = max;};

  double GetXMin() {return m_xMin;};
  double GetXMax() {return m_xMax;};
  void SetXMin(double value) {m_xMin = value;};
  void SetXMax(double value) {m_xMax = value;};

  double GetYMin() {return m_yMin;};
  double GetYMax() {return m_yMax;};
  void SetYMin(double value) {m_yMin = value;};
  void SetYMax(double value) {m_yMax = value;};

  double GetXBase() {return m_xBase;};
  double GetYBase() {return m_yBase;};
  void SetXBase(double value) {m_xBase = value;};
  void SetYBase(double value) {m_yBase = value;};

  wxString GetTitle() {return m_title;};
  void SetTitle(const wxString& value) {m_title = value;};

  wxString GetXAxis() {return m_xAxis;};
  void SetXAxis(const wxString& value) {m_xAxis = value;};
  
  wxString GetXLabel() {return m_xLabel;};
  void SetXLabel(const wxString& value) {m_xLabel = value;};
  
  wxString GetYLabel() {return m_yLabel;};
  void SetYLabel(const wxString& value) {m_yLabel = value;};

  bool GetShowPropertyPanel() {return m_showPropertyPanel;};
  void SetShowPropertyPanel(bool value) {m_showPropertyPanel = value;};

  bool GetShowAnimationToolbar() {return m_showAnimationToolbar;};
  void SetShowAnimationToolbar(bool value) {m_showAnimationToolbar = value;};

  bool GetFocus() {return m_focus;};
  void SetFocus(bool value);

  bool GetLoop() {return m_loop;};
  void SetLoop(bool value) {m_loop = value;};

  uint32_t GetFps() {return m_fps;};
  void SetFps(uint32_t value) { m_fps = value;};

  uint32_t GetXNumTicks() {return m_xNumTicks;};
  uint32_t GetYNumTicks() {return m_yNumTicks;};
  void SetXNumTicks(uint32_t value) {m_xNumTicks = value;};
  void SetYNumTicks(uint32_t value) {m_yNumTicks = value;};

  int32_t GetNormalizeX() {return m_normalizeX;};
  int32_t GetNormalizeY() {return m_normalizeY;};
  void SetNormalizeX();
  void SetNormalizeY();
  void SetNormalizeX(int32_t value);
  void SetNormalizeY(int32_t value);

  //double* GetColor() {return m_vtkProperty2D->GetColor();};
  CVtkColor GetColor();
  void GetColor(CVtkColor& vtkColor);
  void GetColor(double* value) {m_vtkProperty2D->GetColor(value);};
  void SetColor(double* value) {m_vtkProperty2D->SetColor(value);};
  void SetColor(const CVtkColor& vtkColor);
  void SetColor(double r, double g, double b) {m_vtkProperty2D->SetColor(r, g, b);};
  void SetColor(const wxColour& color);
  void SetColor(wxColourData& colorData, int32_t indexCustomColor = -1);
  

  PointGlyph GetPointGlyph() { return m_glyphType;};
  void SetPointGlyph(PointGlyph value);
  void SetPointGlyph();

  bool GetFilledPoint() { return m_filledPoint;};
  void SetFilledPoint(bool value);
  void SetFilledPoint();

  double GetLineWidthFactor() { return m_lineWidthFactor;};
  void SetLineWidthFactor(double value) { m_lineWidthFactor = value;};
  double GetOpacityFactor() { return m_opacityFactor;};
  void SetOpacityFactor(double value) { m_opacityFactor = value;};

  bool GetHide() {return m_hide;};
  void SetHide(bool value);
  void SetHide();

  //wxString GetInputFile() {return m_inputFile;};
  //void SetInputFile(const wxString& value) {m_inputFile = value;};

  void Update();


protected:
  bool HasParent();
  void Copy(CXYPlotProperty& p);
  void Init();

protected:
  vtkProperty2D* m_vtkProperty2D;
  
  CXYPlotData* m_parent;

  //int32_t m_lineStipple;
  //double m_lineWidth;
  //double m_opacity;
  
  bool m_lines;
  bool m_points;
  
  double m_pointSize;

  wxString m_name;
  wxString m_title;
  wxString m_xAxis;
  wxString m_xLabel;
  wxString m_yLabel;

  double m_xMax;
  double m_yMax;
  double m_xMin;
  double m_yMin;

  double m_xBase;
  double m_yBase;

  bool m_xLog;
  bool m_yLog;


  bool m_showAnimationToolbar;
  bool m_showPropertyPanel;
  bool m_focus;
  bool m_loop;
  int32_t m_normalizeX;
  int32_t m_normalizeY;

  uint32_t m_fps;

  uint32_t m_xNumTicks;
  uint32_t m_yNumTicks;


  PointGlyph m_glyphType;
  bool m_filledPoint;

  double m_lineWidthFactor;
  double m_opacityFactor;

  bool m_hide;

  //wxString m_inputFile;

/*
    def __init__(self, parent=None):
        self.parent = parent
        self.__vars={
        "lines":True,
        "points":False,
        "pointsize":1.0,
        "name":"",
        "title":"",
        "xmax":None,
        "ymax":None,
        "xmin":None,
        "ymin":None,
        "opacity":0.6,
        "xlog":False,
        "ylog":False,
        "xbase":None,
        "ybase":None,
        "xlabel":"",
        "ylabel":"",
        "showanimationtoolbar":None,
        "showpropertypanel":False,
        "focus":False,
        "loop":False,
        "fps":30,
        "normalize_x":False,
        "normalize_y":False,
	"xnumticks":None,
	"ynumticks":None
	}
        self.VTKPROPERTY = vtk.vtkProperty2D()
        self.line_stipple=0xFFFF
        self.linewidth=0.8
        self.opacity=0.4
*/


};


//-------------------------------------------------------------
//------------------- CXYPlotData class --------------------
//-------------------------------------------------------------

class CXYPlotData : public CBratObject
{

public:

  CXYPlotData(const wxString& name = "", CXYPlotProperty* plotProperty = NULL);
  virtual ~CXYPlotData();

  void GetXRange(double& min, double& max, uint32_t frame);
  void GetYRange(double& min, double& max, uint32_t frame);

  void GetXRange(double& min, double& max);
  void GetYRange(double& min, double& max);
  
  const CUnit& GetXUnit(){return m_unitX;};
  const CUnit& GetYUnit(){return m_unitY;};

  void SetXUnit(const CUnit& unit) {m_unitX = unit;};
  void SetYUnit(const CUnit& unit) {m_unitY = unit;};
  
  int32_t GetNumberOfFrames();

  void NormalizeX(int32_t value);
  void NormalizeY(int32_t value);
  void Normalize(int32_t value);

  virtual void SetData(double* xArray, const CUIntArray& xDims,  
                       double* yArray, const CUIntArray& yDims, 
                       const CUIntArray& xCommonDimIndex,
                       const CUIntArray& yCommonDimIndex,
                       bool bCopy = true) = 0;

  /*
  virtual void SetData(double* xArray, const vector<uint32_t>& xDims,  
                       double* yArray, const vector<uint32_t>& yDims, bool bCopy = true) = 0;
  */
  wxString GetName() {return m_plotProperty.GetName();};
  void SetName(const wxString& name ) {m_plotProperty.SetName(name);};

  void OnFrameChange(int32_t f);

  vtkDataArrayPlotData* GetVtkDataArrayPlotData() { return m_vtkDataArrayPlotData;};
  
  CXYPlotProperty* GetPlotProperty() {return &m_plotProperty;};

  CPlotArray* GetRawData(int32_t i);
/*
  static void GetAxisX(CInternalFilesYFX* yfx, 
                       ExpressionValueDimensions& dimVal,
                       CExpressionValue& varX,
                       string& varXName);
*/
  ///////void Create(CObArray* data, CPlot* plot, int32_t iField);
  void Create(CInternalFiles* yfx, CPlot* plot, int32_t iField);

//  const string& GetVarComplementName() { return m_varComplementName; };

  CExpressionValue& GetVarComplement() { return m_varComplement; };
  
  CFieldNetCdf& GetFieldComplement() { return m_fieldComplement; };
  
  //const vector<CExpressionValue>& GetOtherVars() { return m_otherVars; };
  const CObArray& GetOtherVars() { return m_otherVars; };
  const CObArray& GetOtherFields() { return m_otherFields; };

protected:
  void Init();
  
  void SetRawData(CPlotArray* x, CPlotArray* y);

  void Update();

public:


protected:
  
  //wxString m_name;
  int32_t m_currentFrame;
  int32_t m_length;

  CXYPlotProperty m_plotProperty;

  vtkDataArrayPlotData* m_vtkDataArrayPlotData;

  CObArray m_rawData;

  CUnit m_unitX;
  CUnit m_unitY;

//  string m_varComplementName;
  CExpressionValue m_varComplement;
//  ExpressionValueDimensions m_dimValComplement;

  CFieldNetCdf m_fieldComplement;


  //vector<CExpressionValue> m_otherVars;
  // A "CExpressionValue*" array
  CObArray m_otherVars;
  
  CObArray m_otherFields;
  

  bool m_aborted;


};
//-------------------------------------------------------------
//------------------- CXYPlotDataSingle class --------------------
//-------------------------------------------------------------
/*
class CXYPlotDataSingle : public CXYPlotData
{ 

public:

  CXYPlotDataSingle(const wxString& name = "");
  virtual ~CXYPlotDataSingle();

  virtual void SetData(double* xArray, const CUIntArray& xDims,  double* yArray, const CUIntArray& yDims, bool bCopy = true);
  virtual void SetData(double* xArray, const vector<uint32_t>& xDims,  double* yArray, const vector<uint32_t>& yDims, bool bCopy = true);

protected:
  virtual void SetData(double* xArray, int32_t xSize,  double* yArray, int32_t ySize, bool bCopy = true);
  virtual void SetData(vtkDoubleArrayBrathl* xArray, vtkDoubleArrayBrathl* yArray);


};
*/
//-------------------------------------------------------------
//------------------- CXYPlotDataMulti class --------------------
//-------------------------------------------------------------

class CXYPlotDataMulti : public CXYPlotData
{

public:

  CXYPlotDataMulti(const wxString& name = "", CXYPlotProperty* plotProperty = NULL);
  virtual ~CXYPlotDataMulti();

                       
  virtual void SetData(double* xArray, const CUIntArray& xDims,  double* yArray, const CUIntArray& yDims, 
                       const CUIntArray& xCommonDimIndex,
                       const CUIntArray& yCommonDimIndex,
                       bool bCopy = true);
  //virtual void SetData(double* xArray, const vector<uint32_t>& xDims,  double* yArray, const vector<uint32_t>& yDims, bool bCopy = true);
  //virtual void SetData(vtkDoubleArray* xArray, vtkDoubleArray* yArray);


};
//-------------------------------------------------------------
//------------------- CXYPlotDataCollection class --------------------
//-------------------------------------------------------------

class CXYPlotDataCollection : public CObArray
{

public:

  CXYPlotDataCollection();
  virtual ~CXYPlotDataCollection();

  void GetXRange(double& min, double& max, uint32_t frame);
  void GetYRange(double& min, double& max, uint32_t frame);

  void GetXRange(double& min, double& max);
  void GetYRange(double& min, double& max);

  CXYPlotData* Get(CObArray::iterator it);
  CXYPlotData* Get(int32_t index);
  CXYPlotProperty* GetPlotProperty(int32_t index);

  void OnFrameChange(int32_t f);

  bool ShowPropertyMenu();

  void GetNames(wxArrayString& names);

  bool IsZoomToDataRange() { return m_zoomToDataRange; };
  void SetZoomToDataRange(bool value) { m_zoomToDataRange = value; };

  int32_t GetCurrentFrame() { return m_currentFrame; };
  void SetCurrentFrame(int32_t value) { m_currentFrame = value; };


protected:
  void Init();

protected:

  bool m_zoomToDataRange;
  int32_t m_currentFrame;

};


#endif
