/*
* Copyright (C) 2005 Collecte Localisation Satellites (CLS), France (31)
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
#ifndef __ZFXYPlotData_H__
#define __ZFXYPlotData_H__


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "ZFXYPlotData.h"
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/progdlg.h"

#include "brathl.h"

#include "BratObject.h"
#include "Tools.h"
#include "List.h"
#include "Date.h"
#include "Exception.h"
#include "Unit.h"

#include "InternalFiles.h"
#include "InternalFilesZFXY.h"

using namespace brathl;

class CZFXYPlotData;


#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkPolyData.h"
#include "vtkSphereSource.h"
#include "vtkContourFilter.h"
#include "vtkDoubleArray.h"
#include "vtkPoints.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkSelectVisiblePoints.h"
#include "vtkProgrammableFilter.h"
#include "vtkLabeledDataMapper.h"
#include "vtkRenderer.h"
#include "vtkScalarBarActor.h"

#include "vtkProj2DFilter.h"
#include "vtkGeoGridSource.h"
#include "vtkGSHHSReader.h"
#include "vtkList.h"

#include "vtkSmoothPolyDataFilter.h"

#include "BratLookupTable.h"

#include "vtkZFXYPlotActor.h"

#include "ZFXYPlot.h"
#include "PlotField.h"

//-------------------------------------------------------------
//------------------- CLUTRenderer class --------------------
//-------------------------------------------------------------

class CLUTZFXYRenderer
{

public:
  
  CLUTZFXYRenderer();
  virtual ~CLUTZFXYRenderer();

  void SetLUT(CBratLookupTable* lut);

  bool ShouldDisplay() {return (m_LUT != NULL);};

  void SetNumberOfLabels(int32_t n);
  int32_t GetNumberOfLabels();

  void SetBackground(double r, double g, double b);
  
  vtkRenderer* GetVtkRenderer() {return m_vtkRend;};
  vtkScalarBarActor* GetScalarBarActor() {return m_scalarBarActor;};

  CBratLookupTable* GetLUT() {return m_LUT;};

  void ResetTextActor(CZFXYPlotData* geoMap);
  void ResetTextActor(const string& text);

  vtkLookupTable* GetLookupTable() 
    { 
      if (m_LUT == NULL)
      {
        return NULL;
      }
      return m_LUT->GetLookupTable();
    };

protected:

  void DeleteLUT();


protected:

  vtkRenderer* m_vtkRend;

  CBratLookupTable* m_LUT;

  vtkScalarBarActor* m_scalarBarActor;


};

//-------------------------------------------------------------
//------------------- CZFXYPlotProperty class --------------------
//-------------------------------------------------------------

class CZFXYPlotProperty : public CBratObject
{
public:

  CZFXYPlotProperty();

  CZFXYPlotProperty(const CZFXYPlotProperty& p);

  virtual ~CZFXYPlotProperty();

  const CZFXYPlotProperty& operator=(const CZFXYPlotProperty& p);

  wxString m_title;
  wxString m_name;
  wxString m_xLabel;
  wxString m_yLabel;

  CBratLookupTable* m_LUT;
  double m_opacity;

  bool m_showAnimationToolbar;
  bool m_showPropertyPanel;
  bool m_showColorBar;
  double m_minHeightValue;
  double m_maxHeightValue;
  int32_t m_numColorLabels;

  double m_xMax;
  double m_yMax;
  double m_xMin;
  double m_yMin;

  double m_xBase;
  double m_yBase;

  bool m_xLog;
  bool m_yLog;

  uint32_t m_xNumTicks;
  uint32_t m_yNumTicks;


  bool m_withContour;
  bool m_withContourLabel;
  double m_minContourValue;
  double m_maxContourValue;
  int32_t m_numContour;
  int32_t m_numContourLabel;
  
  int32_t m_contourLabelSize;
  double m_contourLineWidth;
  
  CVtkColor m_contourLineColor;
  CVtkColor m_contourLabelColor;
  
  wxString m_contourLabelFormat;

  bool m_solidColor;

  bool m_withAnimation;

private:
  void DeleteLUT();

  void Copy(const CZFXYPlotProperty& p);

};




//-------------------------------------------------------------
//------------------- CZFXYPlotData class --------------------
//-------------------------------------------------------------


class CZFXYPlotData : public CBratObject
{

public:

  CZFXYPlotData(wxWindow* parent, CZFXYPlot* plot, CPlotField* field);

  virtual ~CZFXYPlotData();


  void Create(wxWindow* parent, CObArray* data, const string& fieldName, CZFXYPlot* plot);

  
  void SetInput(vtkPolyData* output);
  

  virtual void OnKeyframeChanged(uint32_t i);
  virtual void Close();
  
  const CUnit& GetXUnit(){return m_unitX;};
  const CUnit& GetYUnit(){return m_unitY;};

  uint32_t GetCurrentMap() {return m_currentMap;};

  vtkZFXYPlotFilter* GetCurrentPlotData();
  vtkZFXYPlotFilter* GetPlotData(int32_t i);

  virtual string GetDataTitle() {return GetDataTitle(m_currentMap);};
  virtual string GetDataTitle(uint32_t index) {return m_dataTitles[index];};

  virtual string GetDataName() {return (const char *)(m_plotProperty.m_name);};


  vtkActor2D* GetVtkActor2D() {return m_vtkActor2D;};

  vtkActor2D* GetVtkContourActor2D() {return m_vtkContourActor2D;};

  vtkActor2D* GetVtkContourLabelActor() {return m_vtkContourLabelActor;};

  CLUTZFXYRenderer* GetColorBarRenderer() {return m_colorBarRenderer;};

  CBratLookupTable* GetLUT() {return m_LUT;};

  void SetLUT(CBratLookupTable* lut);

  vtkLookupTable* GetLookupTable() 
    { 
      if (m_LUT == NULL)
      {
        return NULL;
      }
      return m_LUT->GetLookupTable();
    };

  vtkPolyDataMapper2D* GetVtkMapper2D() {return m_vtkMapper2D;};

  vtkPolyDataMapper2D* GetVtkContourMapper2D() {return m_vtkContourMapper2D;};

  vtkProgrammableFilter* GetVtkVisibleSpherePointsFilter() {return m_vtkVisibleSpherePointsFilter;};
  vtkPoints* GetVtkVisibleSpherePoints() {return m_vtkVisibleSpherePoints;};
  vtkDoubleArray* GetVtkVisibleSpherePointsData() {return m_vtkVisibleSpherePointsData;};

  vtkRenderer* GetVtkRenderer() {return m_vtkRend;};

  bool HasActor2D();

  //bool IsNumberOfMapsEquals();

  virtual void SetContour2D();

  void SetContourLabelProperties();
  void ContourGenerateValues();
  void SetContour2DProperties();


  bool GetContour() {return m_plotProperty.m_withContour;};
  void SetContour(bool value) {m_plotProperty.m_withContour = value;};
  
  bool GetSolidColor() {return m_plotProperty.m_solidColor;};
  void SetSolidColor(bool value) {m_plotProperty.m_solidColor = value;};

  void SetRenderer(vtkRenderer* vtkRend) {m_vtkRend = vtkRend;};

  static void FindVisiblePlanePoints(void* arg);

  //void SetContourLabels2DPosition();

  virtual void CreateContourLabels2D();
  virtual void UpdateContourLabels2D();

  void GetComputedRange(double& min, double& max) {min = m_minhv; max = m_maxhv;};
  double GetComputedRangeMin() {return m_minhv;};
  double GetComputedRangeMax() {return m_maxhv;};

  void GetXRange(double& min, double& max, uint32_t frame);
  void GetXRange(double& min, double& max) {min = m_xMin; max = m_xMax;};
  double GetXRangeMin() {return m_xMin;};
  double GetXRangeMax() {return m_xMax;};

  void GetYRange(double& min, double& max, uint32_t frame);
  void GetYRange(double& min, double& max) {min = m_yMin; max = m_yMax;};
  double GetYRangeMin() {return m_yMin;};
  double GetYRangeMax() {return m_yMax;};

  vtkObArray* GetZFXYPlotFilterList() {return &m_zfxyPlotFilterList;};
  
  int32_t GetNrMaps() {return m_nrMaps;};

  CZFXYPlotProperty* GetPlotProperty() {return &m_plotProperty;};

  bool GetContourLabelNeedUpdateOnWindow() {return m_contourLabelNeedUpdateOnWindow;};
  void SetContourLabelNeedUpdateOnWindow(bool value ) {m_contourLabelNeedUpdateOnWindow = value;};

  bool GetContourLabelNeedUpdatePositionOnContour() {return m_contourLabelNeedUpdatePositionOnContour;};
  void SetContourLabelNeedUpdatePositionOnContour(bool value) {m_contourLabelNeedUpdatePositionOnContour = value;};

  string GetDataDateString() {return GetDataDateString(m_currentMap);};
  string GetDataDateString(uint32_t index);

  string GetDataUnitString() {return GetDataUnitString(m_currentMap);};
  string GetDataUnitString(uint32_t index);


protected:

  void DeleteMapper2D();
  void DeleteContourMapper2D();

  void DeleteActor2D();
  void DeleteContourActor2D();

  void DeleteAll2D();
  void Delete2D();
  void DeleteContour2D();


  void DeleteContourFilter();

  void DeleteContourLabelActor();
  void DeleteContourLabelMapper();
  void DeleteVtkContourLabelObject();

  void NewVtkContourLabelObject();


  void DeleteLUT();

public:

  CZFXYPlotProperty m_plotProperty;


protected:

protected:

  
  vtkActor2D* m_vtkActor2D;
  vtkActor2D* m_vtkContourActor2D;
  

  vtkPolyDataMapper2D* m_vtkMapper2D;
  vtkPolyDataMapper2D* m_vtkContourMapper2D;

  vtkContourFilter* m_vtkContourFilter;

  vtkDoubleArray* m_vtkLabelContourData;
  vtkPoints* m_vtkLabelContourPoints;
  vtkPolyData* m_vtkLabelContourPositions;

  vtkLabeledDataMapper* m_vtkContourLabelMapper;
  vtkActor2D* m_vtkContourLabelActor;

  vtkPolyDataConnectivityFilter* m_vtkConnectivity;

  vtkSelectVisiblePoints* m_vtkVisiblePointsFilter;
  vtkProgrammableFilter* m_vtkVisibleSpherePointsFilter;
  vtkPoints* m_vtkVisibleSpherePoints;
  vtkDoubleArray* m_vtkVisibleSpherePointsData;

  CBratLookupTable* m_LUT;
  
  bool m_aborted;

  vtkRenderer* m_vtkRend;

  CLUTZFXYRenderer* m_colorBarRenderer;
    
  bool m_finished;
  uint32_t m_currentMap;
  uint32_t m_nrMaps;

  double m_minhv;
  double m_maxhv;

  double m_xMin;
  double m_xMax;

  double m_yMin;
  double m_yMax;

  CObArray m_dataDates;
  CStringArray m_dataTitles;
  CObArray m_dataUnits;

  vtkObArray m_zfxyPlotFilterList;


  vtkObArray m_labelPts;
  vtkObArray m_labelIds;
  vtkObArray m_labelData;

  bool m_contourLabelNeedUpdateOnWindow;
  bool m_contourLabelNeedUpdatePositionOnContour;

  CUnit m_unitX;
  CUnit m_unitY;


};





#endif
