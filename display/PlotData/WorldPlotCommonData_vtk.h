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
#ifndef DATAMODELS_PLOTDATA_WORLDPLOT_COMMON_DATA_VTK_H
#define DATAMODELS_PLOTDATA_WORLDPLOT_COMMON_DATA_VTK_H



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

#include "display/PlotData/vtkProj2DFilter.h"
#include "display/PlotData/vtkGeoGridSource.h"
#include "display/PlotData/vtkGSHHSReader.h"
#include "display/PlotData/vtkList.h"

#include "vtkMaskPoints.h"
#include "vtkArrowSource.h"
#include "display/PlotData/vtkBratArrowSource.h"
#include "vtkGlyph2D.h"
#include "vtkUnstructuredGrid.h"
#include "vtkDataSetSurfaceFilter.h"


#include "display/PlotData/vtkGeoMapFilter.h"
#include "display/PlotData/vtkVelocityGlyphFilter.h"

#include "new-gui/brat/DataModels/PlotData/WorldPlotData.h"


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

  void ResetTextActor(CWorldPlotData* geoMap);
  void ResetTextActor(const std::string& text);

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
//------------------- CWorldPlotProperties class --------------------
//-------------------------------------------------------------
//
//class CWorldPlotProperties : public CBratObject
//{
//public:
//
//	CWorldPlotProperties();
//
//	CWorldPlotProperties( const CWorldPlotProperties& p );
//
//	virtual ~CWorldPlotProperties();
//
//	const CWorldPlotProperties& operator=( const CWorldPlotProperties& p );
//
//	std::string m_title;
//	std::string m_name;
//
//	std::string m_coastResolution;
//	std::string m_projection;
//	CBratLookupTable* m_LUT;
//	//CDoubleArray m_colorRange;
//	double m_opacity;
//	double m_deltaRadius;
//	bool m_showAnimationToolbar;
//	bool m_showPropertyPanel;
//	bool m_showColorBar;
//	//int32_t m_lineWidth;
//	//int32_t m_pointSize;
//	//CUIntArray m_color;
//	double m_heightFactor;
//	double m_minHeightValue;
//	double m_maxHeightValue;
//	int32_t m_numColorLabels;
//	//uint32_t m_stipplepattern;
//	double m_valueConversion;
//	double m_heightConversion;
//
//	double m_centerLongitude;
//	double m_centerLatitude;
//
//	bool m_zoom;
//	double m_zoomLon1;
//	double m_zoomLon2;
//	double m_zoomLat1;
//	double m_zoomLat2;
//
//	bool m_withContour;
//	bool m_withContourLabel;
//	double m_minContourValue;
//	double m_maxContourValue;
//	int32_t m_numContour;
//	int32_t m_numContourLabel;
//
//	int32_t m_contourLabelSize;
//	double m_contourLineWidth;
//
//	CPlotColor m_contourLineColor;
//	CPlotColor m_contourLabelColor;
//
//	std::string m_contourLabelFormat;
//
//	bool m_solidColor;
//	bool m_gridLabel;
//
//	bool m_eastComponent;
//	bool m_northComponent;
//
//	bool m_withAnimation;
//
//private:
//	void DeleteLUT();
//
//	void Copy( const CWorldPlotProperties& p );
//};
//


//-------------------------------------------------------------
//------------------- VTK_CWorldPlotCommonData class --------------------
//-------------------------------------------------------------


class VTK_CWorldPlotCommonData : public CWorldPlotVelocityData
{
	using base_t = CWorldPlotVelocityData;

  // instance data

public:


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


  vtkRenderer* m_vtkRend;

  CLUTRenderer* m_colorBarRenderer;


  //construction /destruction

  void Init();
  VTK_CWorldPlotCommonData( CWorldPlotProperties* plotProperty = NULL )
	  : base_t( plotProperty )
  {
	  Init();
  }

  VTK_CWorldPlotCommonData(CPlotField* field)
	: base_t(field)
  {
	  Init();
  }

  VTK_CWorldPlotCommonData( CPlotField* northField, CPlotField* eastField )
		: base_t( northField, eastField )
  {
	  Init();
  }

public:

  virtual ~VTK_CWorldPlotCommonData();


  // remaining member functions

  virtual void SetProjection(int32_t proj) override;



  virtual void Set3D() override;
  virtual void Set2D() override;

  void SetCenterLatitude(double lat);
  void SetCenterLongitude(double lon);

  void SetInput(vtkPolyData* output);

  vtkTransform* GetTransform() {return m_transform;}
  vtkTransformPolyDataFilter* GetVtkTransform() {return m_vtkTransform;}



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


  void SetRenderer(vtkRenderer* vtkRend) {m_vtkRend = vtkRend;}




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
};


//-------------------------------------------------------------
//------------------- CGeoGrid class --------------------
//-------------------------------------------------------------

class CGeoGrid : public VTK_CWorldPlotCommonData
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

class CCoastLine : public VTK_CWorldPlotCommonData
{
public:

  CCoastLine(const std::string& fileName, int32_t maxLevel = 1);
  virtual ~CCoastLine();

protected:

  virtual void Update();
  void SetGSHHSReader(const std::string& fileName, int32_t maxLevel = 1);

protected:

  vtkGSHHSReader* m_GSHHSReader;

};


//-------------------------------------------------------------
//------------------- CGlobe class --------------------
//-------------------------------------------------------------
class CGlobe : public VTK_CWorldPlotCommonData
{
public:

  CGlobe();
  virtual ~CGlobe();

  virtual void SetProjection(int32_t proj);

protected:


  vtkSphereSource* m_source;

};


#endif			//DATAMODELS_PLOTDATA_WORLDPLOT_COMMON_DATA_VTK_H
