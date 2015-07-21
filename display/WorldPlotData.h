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
#ifndef __WorldPlotData_H__
#define __WorldPlotData_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "WorldPlotData.h"
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
#include "vtkRegularPolygonSource.h"

#include "vtkProj2DFilter.h"
#include "vtkGeoGridSource.h"
#include "vtkGSHHSReader.h"
#include "vtkList.h"

#include "vtkMaskPoints.h"
#include "vtkArrowSource.h"
#include "vtkBratArrowSource.h"
#include "vtkGlyph2D.h"
#include "vtkUnstructuredGrid.h"
#include "vtkDataSetSurfaceFilter.h"

#include "BratLookupTable.h"

#include "PlotField.h"

#include "vtkGeoMapFilter.h"
#include "vtkVelocityGlyphFilter.h"

class CGeoMap;
//-------------------------------------------------------------
//------------------- CLUTRenderer class --------------------
//-------------------------------------------------------------

class CLUTRenderer
{

public:

  CLUTRenderer();
  virtual ~CLUTRenderer();

  void SetLUT(CBratLookupTable* lut);

  bool ShouldDisplay() {return (m_LUT != NULL);};

  void SetNumberOfLabels(int32_t n);
  int32_t GetNumberOfLabels();

  void SetBackground(double r, double g, double b);

  vtkRenderer* GetVtkRenderer() {return m_vtkRend;};
  vtkScalarBarActor* GetScalarBarActor() {return m_scalarBarActor;};

  CBratLookupTable* GetLUT() {return m_LUT;};

  void ResetTextActor(CGeoMap* geoMap);
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
//------------------- CWorldPlotProperty class --------------------
//-------------------------------------------------------------

class CWorldPlotProperty : public CBratObject
{
public:

  CWorldPlotProperty();

  CWorldPlotProperty(const CWorldPlotProperty& p);

  virtual ~CWorldPlotProperty();

  const CWorldPlotProperty& operator=(const CWorldPlotProperty& p);

  wxString m_title;
  wxString m_name;

  string m_coastResolution;
  string m_projection;
  CBratLookupTable* m_LUT;
  //CDoubleArray m_colorRange;
  double m_opacity;
  double m_deltaRadius;
  bool m_showAnimationToolbar;
  bool m_showPropertyPanel;
  bool m_showColorBar;
  //int32_t m_lineWidth;
  //int32_t m_pointSize;
  //CUIntArray m_color;
  double m_heightFactor;
  double m_minHeightValue;
  double m_maxHeightValue;
  int32_t m_numColorLabels;
  //uint32_t m_stipplepattern;
  double m_valueConversion;
  double m_heightConversion;

  double m_centerLongitude;
  double m_centerLatitude;

  bool m_zoom;
  double m_zoomLon1;
  double m_zoomLon2;
  double m_zoomLat1;
  double m_zoomLat2;

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
  bool m_gridLabel;

  bool m_eastComponent;
  bool m_northComponent;

  bool m_withAnimation;

private:
  void DeleteLUT();

  void Copy(const CWorldPlotProperty& p);

};

//-------------------------------------------------------------
//------------------- CWorldPlotData class --------------------
//-------------------------------------------------------------


class CWorldPlotData : public CBratObject
{

public:

  CWorldPlotData(CWorldPlotProperty* plotProperty = NULL);
  virtual ~CWorldPlotData();

  virtual void SetProjection(int32_t proj);

  virtual void Update2D() {}

  bool Is3D() {return m_is3D;}
  bool Is2D() {return !m_is3D;}

  virtual void Set3D();
  virtual void Set2D();

  void SetCenterLatitude(double lat);
  void SetCenterLongitude(double lon);

  void SetInput(vtkPolyData* output);

  vtkTransform* GetTransform() {return m_transform;}
  vtkTransformPolyDataFilter* GetVtkTransform() {return m_vtkTransform;}

  virtual void OnKeyframeChanged(uint32_t /*i*/) { }
  virtual void Close() {}
  virtual void Update() {}

  virtual string GetDataTitle() {return "";}
  virtual string GetDataTitle(uint32_t index) {return "";}

  virtual string GetDataName() {return "";}


  vtkActor* GetVtkActor() {return m_vtkActor;}
  vtkActor2D* GetVtkActor2D() {return m_vtkActor2D;}

  vtkActor* GetVtkContourActor() {return m_vtkContourActor;}
  vtkActor2D* GetVtkContourActor2D() {return m_vtkContourActor2D;}

  vtkActor2D* GetVtkContourLabelActor() {return m_vtkContourLabelActor;}

  CLUTRenderer* GetColorBarRenderer() {return m_colorBarRenderer;}

  virtual vtkProj2DFilter* GetVtkFilter() { return m_vtkFilter; }

  CBratLookupTable* GetLUT() {return m_LUT;}

  void SetLUT(CBratLookupTable* lut);

  vtkLookupTable* GetLookupTable()
    {
      if (m_LUT == NULL)
      {
        return NULL;
      }
      return m_LUT->GetLookupTable();
    };

  vtkPolyDataMapper* GetVtkMapper() {return m_vtkMapper;}
  vtkPolyDataMapper2D* GetVtkMapper2D() {return m_vtkMapper2D;}

  vtkPolyDataMapper* GetVtkContourMapper() {return m_vtkContourMapper;}
  vtkPolyDataMapper2D* GetVtkContourMapper2D() {return m_vtkContourMapper2D;}

  vtkProgrammableFilter* GetVtkVisibleSpherePointsFilter() {return m_vtkVisibleSpherePointsFilter;}
  vtkPoints* GetVtkVisibleSpherePoints() {return m_vtkVisibleSpherePoints;}
  vtkDoubleArray* GetVtkVisibleSpherePointsData() {return m_vtkVisibleSpherePointsData;}

  vtkRenderer* GetVtkRenderer() {return m_vtkRend;}

  bool HasActor();
  bool HasActor2D();

 //bool IsNumberOfMapsEquals();

  bool GetContour() {return m_plotProperty.m_withContour;}
  void SetContour(bool value) {m_plotProperty.m_withContour = value;}

  bool GetSolidColor() {return m_plotProperty.m_solidColor;}
  void SetSolidColor(bool value) {m_plotProperty.m_solidColor = value;}

  void SetRenderer(vtkRenderer* vtkRend) {m_vtkRend = vtkRend;}

  virtual void CreateContourLabels() {}
  virtual void UpdateContourLabels() {}

  virtual void CreateContourLabels2D() {}
  virtual void UpdateContourLabels2D() {}


  //CWorldPlotProperty* GetWordlPlotProperty() {return &m_plotProperty;};

protected:

  void NoTransform();

  void NewVtkContourLabelObject();

  void DeleteLUT();

  void DeleteAll3D();
  void DeleteAll2D();

  void Delete3D();
  void Delete2D();

  void DeleteContour3D();
  void DeleteContour2D();

  void DeleteActor();
  void DeleteActor2D();

  void DeleteContourActor();
  void DeleteContourActor2D();

  void DeleteMapper();
  void DeleteMapper2D();

  void DeleteContourMapper();
  void DeleteContourMapper2D();

  void DeleteContourFilter();

  void DeleteContourLabelActor();
  void DeleteContourLabelMapper();

  void DeleteVtkContourLabelObject();

public:

  CWorldPlotProperty m_plotProperty;

protected:

  vtkActor* m_vtkActor;
  vtkActor* m_vtkContourActor;

  vtkActor2D* m_vtkActor2D;
  vtkActor2D* m_vtkContourActor2D;

  vtkPolyDataMapper* m_vtkMapper;
  vtkPolyDataMapper* m_vtkContourMapper;

  vtkPolyDataMapper2D* m_vtkMapper2D;
  vtkPolyDataMapper2D* m_vtkContourMapper2D;

  vtkProj2DFilter* m_vtkFilter;
  vtkTransformPolyDataFilter* m_vtkTransform;
  vtkTransform* m_transform;

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
  bool m_is3D;

  vtkRenderer* m_vtkRend;

  CLUTRenderer* m_colorBarRenderer;

  int32_t m_projection;

};


//-------------------------------------------------------------
//------------------- CGeoGrid class --------------------
//-------------------------------------------------------------

class CGeoGrid : public CWorldPlotData
{
public:

  CGeoGrid();
  virtual ~CGeoGrid();

  virtual void SetProjection(int32_t proj);

  vtkGeoGridSource* GetVtkGeoGridSource() {return m_source;}

  bool FindDataCoords2D(vtkRenderer* ren, double xValue, double yValue, int& xCoord, int& yCoord);
  bool FindDataCoords2D(vtkRenderer* ren, double value[2], int& xCoord, int& yCoord);

protected:

  virtual void Update();

protected:

  vtkGeoGridSource* m_source;


};

//-------------------------------------------------------------
//------------------- CCoastLine class --------------------
//-------------------------------------------------------------

class CCoastLine : public CWorldPlotData
{
public:

  CCoastLine(const string& fileName, int32_t maxLevel = 1);
  virtual ~CCoastLine();

protected:

  virtual void Update();
  void SetGSHHSReader(const string& fileName, int32_t maxLevel = 1);

protected:

  vtkGSHHSReader* m_GSHHSReader;

};


//-------------------------------------------------------------
//------------------- CGlobe class --------------------
//-------------------------------------------------------------
class CGlobe : public CWorldPlotData
{
public:

  CGlobe();
  virtual ~CGlobe();

  virtual void SetProjection(int32_t proj);

protected:


  vtkSphereSource* m_source;

};


//-------------------------------------------------------------
//------------------- CGeoMap class --------------------
//-------------------------------------------------------------

class CGeoMap : public CWorldPlotData
{
public:

  CGeoMap(wxWindow* parent, CPlotField* field);
  CGeoMap(wxWindow* parent, CObArray* data, const string& fieldName, CWorldPlotProperty* plotProperty = NULL);
  virtual ~CGeoMap();

  virtual void Create(wxWindow* parent, CObArray* data, const string& fieldName);

  int32_t GetNrMaps() {return m_nrMaps;};

  virtual void OnKeyframeChanged(uint32_t i);
  virtual void Close();
  virtual void Update();

  virtual void Update2D();

  void GetComputedRange(double& min, double& max) {min = m_minhv; max = m_maxhv;}
  double GetComputedRangeMin() {return m_minhv;}
  vtkObArray* GetGeoMapFilterList() {return &m_geoMapFilterList;}

  double GetComputedRangeMax() {return m_maxhv;}

  string GetDataDateString() {return GetDataDateString(m_currentMap);}
  string GetDataDateString(uint32_t index);

  const CDate* GetDataDate() {return GetDataDate(m_currentMap);}
  const CDate* GetDataDate(uint32_t index) {return dynamic_cast<CDate*>(m_dataDates[index]);}

  virtual string GetDataTitle() {return GetDataTitle(m_currentMap);}
  virtual string GetDataTitle(uint32_t index) {return m_dataTitles[index];}

  virtual string GetDataName() {return (const char *)(m_plotProperty.m_name);}

  string GetDataUnitString() {return GetDataUnitString(m_currentMap);}
  string GetDataUnitString(uint32_t index);

  const CUnit* GetDataUnit() {return GetDataUnit(m_currentMap);}
  const CUnit* GetDataUnit(uint32_t index) {return dynamic_cast<CUnit*>(m_dataUnits[index]);}

  virtual void SetProjection(int32_t proj);

  virtual void Set3D();
  virtual void Set2D();

  virtual void SetSolidColor3D();
  virtual void SetSolidColor2D();

  virtual void SetContour3D();
  virtual void SetContour2D();

  void SetContourLabelProperties();
  void ContourGenerateValues();
  void SetContour3DProperties();
  void SetContour2DProperties();

  virtual void CreateContourLabels();
  static void FindVisibleSpherePoints(void* arg);
  virtual void UpdateContourLabels();

  virtual void CreateContourLabels2D();
  virtual void UpdateContourLabels2D();

  static void FindVisiblePlanePoints(void* arg);

  void SetContourLabels2DPosition();

  bool GetContourLabelNeedUpdateOnWindow() {return m_contourLabelNeedUpdateOnWindow;}
  void SetContourLabelNeedUpdateOnWindow(bool value ) {m_contourLabelNeedUpdateOnWindow = value;}

  bool GetContourLabelNeedUpdatePositionOnContour() {return m_contourLabelNeedUpdatePositionOnContour;}
  void SetContourLabelNeedUpdatePositionOnContour(bool value) {m_contourLabelNeedUpdatePositionOnContour = value;}

protected:
  void GetMapLatLon(CInternalFilesZFXY* zfxy,
                    int32_t& width, int32_t& height,
                    CExpressionValue& varLat, CExpressionValue& varLon);

  // for base classes
  CGeoMap(wxWindow* parent, CWorldPlotProperty* plotProperty);

protected:

  bool m_finished;
  uint32_t m_currentMap;
  uint32_t m_nrMaps;

  CObArray m_dataDates;
  CStringArray m_dataTitles;
  CObArray m_dataUnits;

  vtkObArray m_geoMapFilterList;


  vtkObArray m_labelPts;
  vtkObArray m_labelIds;
  vtkObArray m_labelData;

  bool m_contourLabelNeedUpdateOnWindow;
  bool m_contourLabelNeedUpdatePositionOnContour;

  double m_minhv;
  double m_maxhv;

};



//-------------------------------------------------------------
//------------------- CGeoVelocityMap class --------------------
//-------------------------------------------------------------

class CGeoVelocityMap : public CGeoMap
{
public:

  CGeoVelocityMap(wxWindow* parent, CPlotField* northField, CPlotField* eastField);
  CGeoVelocityMap(wxWindow* parent, CObArray* northData, CObArray* eastData, const string& fieldNameNorth, const string& fieldNameEast, CWorldPlotProperty* plotProperty = NULL);
  virtual ~CGeoVelocityMap();

  void Create(wxWindow* parent, CObArray* northData, CObArray* eastData, const string& fieldNameNorth, const string& fieldNameEast);

  vtkVelocityGlyphFilter * GetGlyphFilter() { return m_geoMapFilter; }

  void SetIsGlyph(bool val);

  void SetZoom(double z) { zoom =z; }

 protected:

   double m_minhvN;
   double m_maxhvN;

   double m_minhvE;
   double m_maxhvE;

   bool IsGlyph;
   double zoom;


   vtkProj2DFilter *m_simple2DProjFilter;  // this could be of any implementation
   vtkVelocityGlyphFilter *m_geoMapFilter;

   // for glyphs
   vtkMaskPoints*   m_pointMask;
   vtkArrowSource* m_arrowSource;
   vtkBratArrowSource* m_barrowSource;
   vtkGlyph3D*  m_glyph;

protected:
   vtkProj2DFilter* GetVtkFilter();

   void Set2D();
   void Set3D();

   void Set2DGlyphs();
   void Set3DGlyphs();

};


#endif
