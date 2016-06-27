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
#ifndef DATAMODELS_PLOTDATA_WORLDPLOT_COMMON_DATA_H
#define DATAMODELS_PLOTDATA_WORLDPLOT_COMMON_DATA_H

#include "libbrathl/brathl.h"

#include "libbrathl/BratObject.h"
#include "libbrathl/Tools.h"
#include "libbrathl/List.h"
#include "libbrathl/Date.h"
#include "new-gui/Common/tools/Exception.h"
#include "libbrathl/Unit.h"

#include "libbrathl/InternalFiles.h"
#include "libbrathl/InternalFilesZFXY.h"

using namespace brathl;

#include "PlotColor.h"
#include "PlotField.h"
#include "FieldData.h"


class CBratLookupTable;
class QLookupTable;

//-------------------------------------------------------------
//------------------- CWorldPlotProperties class --------------------
//-------------------------------------------------------------

class CWorldPlotProperties : public CFieldData
{
	using base_t = CFieldData;

public:
	std::string m_coastResolution;
	std::string m_projection;

	CBratLookupTable *mLUT = nullptr;

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
	double mContourLineWidth = 0.1;

	CPlotColor m_contourLineColor;
	CPlotColor m_contourLabelColor;

	std::string m_contourLabelFormat;

	bool m_solidColor;
	bool m_gridLabel;

	bool m_eastComponent;
	bool m_northComponent;

	bool m_withAnimation;

public:

	CWorldPlotProperties();

	CWorldPlotProperties( const CWorldPlotProperties &o )
	{
		*this = o;
	}

	virtual ~CWorldPlotProperties()
	{
		DeleteLUT();
	}


	const CWorldPlotProperties& operator = ( const CWorldPlotProperties &o );

private:
	void DeleteLUT();
};



//-------------------------------------------------------------
//------------------- CWorldPlotCommonData class --------------------
//-------------------------------------------------------------

class CWorldPlotCommonData : public CPlotData
{
	using base_t = CPlotData;

	// instance data

public:

	CWorldPlotProperties m_plotProperty;

protected:

	//vtkActor* m_vtkActor;
	//vtkActor* m_vtkContourActor;

	//vtkActor2D* m_vtkActor2D;
	//vtkActor2D* m_vtkContourActor2D;

	//vtkPolyDataMapper* m_vtkMapper;
	//vtkPolyDataMapper* m_vtkContourMapper;

	//vtkPolyDataMapper2D* m_vtkMapper2D;
	//vtkPolyDataMapper2D* m_vtkContourMapper2D;

	//vtkProj2DFilter* m_vtkFilter;
	//vtkTransformPolyDataFilter* m_vtkTransform;
	//vtkTransform* m_transform;

	//vtkContourFilter* m_vtkContourFilter;

	//vtkDoubleArray* m_vtkLabelContourData;
	//vtkPoints* m_vtkLabelContourPoints;
	//vtkPolyData* m_vtkLabelContourPositions;

	//vtkLabeledDataMapper* m_vtkContourLabelMapper;
	//vtkActor2D* m_vtkContourLabelActor;

	//vtkPolyDataConnectivityFilter* m_vtkConnectivity;

	//vtkSelectVisiblePoints* m_vtkVisiblePointsFilter;
	//vtkProgrammableFilter* m_vtkVisibleSpherePointsFilter;
	//vtkPoints* m_vtkVisibleSpherePoints;
	//vtkDoubleArray* m_vtkVisibleSpherePointsData;

	CBratLookupTable* mLUT = nullptr;

	bool m_aborted;
	bool m_is3D;

	//vtkRenderer* m_vtkRend;

	//CLUTRenderer* m_colorBarRenderer;

	int32_t m_projection;

	//construction /destruction

	CWorldPlotCommonData( CWorldPlotProperties *plotProperty, std::initializer_list< const std::string > names );

public:

	virtual ~CWorldPlotCommonData()
	{}


	// remaining member functions

	virtual void SetProjection( int32_t proj );

	virtual void Update2D() {}

	bool Is3D() { return m_is3D; }
	bool Is2D() { return !m_is3D; }

	virtual void Set3D();
	virtual void Set2D();

	//void SetCenterLatitude(double lat);
	//void SetCenterLongitude(double lon);

	//void SetInput(vtkPolyData* output);

	//vtkTransform* GetTransform() {return m_transform;}
	//vtkTransformPolyDataFilter* GetVtkTransform() {return m_vtkTransform;}

	virtual void OnKeyframeChanged( uint32_t /*i*/ ) { }
	virtual void Close() {}
	virtual void Update() {}

	virtual std::string GetDataTitle() { return ""; }
	virtual std::string GetDataTitle( uint32_t index ) { UNUSED( index ); return ""; }

	virtual std::string GetDataName() { return ""; }


	//vtkActor* GetVtkActor() {return m_vtkActor;}
	//vtkActor2D* GetVtkActor2D() {return m_vtkActor2D;}

	//vtkActor* GetVtkContourActor() {return m_vtkContourActor;}
	//vtkActor2D* GetVtkContourActor2D() {return m_vtkContourActor2D;}

	//vtkActor2D* GetVtkContourLabelActor() {return m_vtkContourLabelActor;}

	//CLUTRenderer* GetColorBarRenderer() {return m_colorBarRenderer;}

	//virtual vtkProj2DFilter* GetVtkFilter() { return m_vtkFilter; }

	//CBratLookupTable* GetLUT() {return m_LUT;}

	//void SetLUT(CBratLookupTable* lut);

	//vtkLookupTable* GetLookupTable()
	//  {
	//    if (m_LUT == nullptr)
	//    {
	//      return nullptr;
	//    }
	//    return m_LUT->GetLookupTable();
	//  };

	//vtkPolyDataMapper* GetVtkMapper() {return m_vtkMapper;}
	//vtkPolyDataMapper2D* GetVtkMapper2D() {return m_vtkMapper2D;}

	//vtkPolyDataMapper* GetVtkContourMapper() {return m_vtkContourMapper;}
	//vtkPolyDataMapper2D* GetVtkContourMapper2D() {return m_vtkContourMapper2D;}

	//vtkProgrammableFilter* GetVtkVisibleSpherePointsFilter() {return m_vtkVisibleSpherePointsFilter;}
	//vtkPoints* GetVtkVisibleSpherePoints() {return m_vtkVisibleSpherePoints;}
	//vtkDoubleArray* GetVtkVisibleSpherePointsData() {return m_vtkVisibleSpherePointsData;}

	//vtkRenderer* GetVtkRenderer() {return m_vtkRend;}

	//bool HasActor();
	//bool HasActor2D();

	bool GetContour() { return m_plotProperty.m_withContour; }
	void SetContour( bool value ) { m_plotProperty.m_withContour = value; }

	bool GetSolidColor() { return m_plotProperty.m_solidColor; }
	void SetSolidColor( bool value ) { m_plotProperty.m_solidColor = value; }

	//void SetRenderer(vtkRenderer* vtkRend) {m_vtkRend = vtkRend;}

	virtual void CreateContourLabels() {}
	virtual void UpdateContourLabels() {}

	virtual void CreateContourLabels2D() {}
	virtual void UpdateContourLabels2D() {}



	CBratLookupTable* GetLUT() { return mLUT; }

	QLookupTable* GetLookupTable();

protected:

	virtual void SetLUT( CBratLookupTable* lut );

	void DeleteLUT();

	//void NoTransform();

	//void NewVtkContourLabelObject();

	//void DeleteLUT();

	//void DeleteAll3D();
	//void DeleteAll2D();

	//void Delete3D();
	//void Delete2D();

	//void DeleteContour3D();
	//void DeleteContour2D();

	//void DeleteActor();
	//void DeleteActor2D();

	//void DeleteContourActor();
	//void DeleteContourActor2D();

	//void DeleteMapper();
	//void DeleteMapper2D();

	//void DeleteContourMapper();
	//void DeleteContourMapper2D();

	//void DeleteContourFilter();

	//void DeleteContourLabelActor();
	//void DeleteContourLabelMapper();

	//void DeleteVtkContourLabelObject();
};


//-------------------------------------------------------------
//------------------- CGeoGrid class --------------------
//-------------------------------------------------------------

//class CGeoGrid : public CWorldPlotCommonData
//{
//public:
//
//  CGeoGrid();
//  virtual ~CGeoGrid();
//
//  virtual void SetProjection(int32_t proj);
//
//  vtkGeoGridSource* GetVtkGeoGridSource() {return m_source;}
//
//  bool FindDataCoords2D(vtkRenderer* ren, double xValue, double yValue, int& xCoord, int& yCoord);
//  bool FindDataCoords2D(vtkRenderer* ren, double value[2], int& xCoord, int& yCoord);
//
//protected:
//
//  virtual void Update();
//
//protected:
//
//  vtkGeoGridSource* m_source;
//
//
//};

//-------------------------------------------------------------
//------------------- CCoastLine class --------------------
//-------------------------------------------------------------

//class CCoastLine : public CWorldPlotCommonData
//{
//public:
//
//  CCoastLine(const std::string& fileName, int32_t maxLevel = 1);
//  virtual ~CCoastLine();
//
//protected:
//
//  virtual void Update();
//  void SetGSHHSReader(const std::string& fileName, int32_t maxLevel = 1);
//
//protected:
//
//  vtkGSHHSReader* m_GSHHSReader;
//
//};
//

//-------------------------------------------------------------
//------------------- CGlobe class --------------------
//-------------------------------------------------------------
//class CGlobe : public CWorldPlotCommonData
//{
//public:
//
//  CGlobe();
//  virtual ~CGlobe();
//
//  virtual void SetProjection(int32_t proj);
//
//protected:
//
//
//  vtkSphereSource* m_source;
//
//};
//

#endif			//DATAMODELS_PLOTDATA_WORLDPLOT_COMMON_DATA_H
