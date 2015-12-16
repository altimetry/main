#ifndef GEO_MAP_H
#define GEO_MAP_H


#include "libbrathl/brathl.h"

//#include "BratObject.h"
//#include "Tools.h"
//#include "List.h"
//#include "Date.h"
//#include "Exception.h"
//#include "Unit.h"
//
//#include "InternalFiles.h"
//#include "InternalFilesZFXY.h"
//
//using namespace brathl;
//
//#include "PlotData/WorldPlotProperty.h"
//#include "PlotData/BratLookupTable.h"
//#include "PlotData/PlotField.h"
//
//
//#include "vtkActor.h"
//#include "vtkActor2D.h"
//#include "vtkPolyDataMapper.h"
//#include "vtkPolyDataMapper2D.h"
//#include "vtkTransformPolyDataFilter.h"
//#include "vtkTransform.h"
//#include "vtkPolyData.h"
//#include "vtkSphereSource.h"
//#include "vtkContourFilter.h"
//#include "vtkDoubleArray.h"
//#include "vtkPoints.h"
//#include "vtkPolyDataConnectivityFilter.h"
//#include "vtkSelectVisiblePoints.h"
//#include "vtkProgrammableFilter.h"
//#include "vtkLabeledDataMapper.h"
//#include "vtkRenderer.h"
//#include "vtkScalarBarActor.h"
//#include "vtkRegularPolygonSource.h"
//
//#include "vtkProj2DFilter.h"
//#include "vtkGeoGridSource.h"
//#include "vtkGSHHSReader.h"
//#include "vtkList.h"
//
//#include "vtkMaskPoints.h"
//#include "vtkArrowSource.h"
//#include "vtkBratArrowSource.h"
//#include "vtkGlyph2D.h"
//#include "vtkUnstructuredGrid.h"
//#include "vtkDataSetSurfaceFilter.h"
//
//
//#include "vtkGeoMapFilter.h"
//#include "vtkVelocityGlyphFilter.h"

#include "WorldPlotData.h"



class CGeoMap : public CWorldPlotData
{
public:
		std::vector<bool> valids;
		std::vector<double> vals;
		std::vector<bool> bits;
		std::vector<double> lons;
		std::vector<double> lats;

	CGeoMap( CPlotField* field );
	CGeoMap( CObArray* data, const std::string& fieldName, CWorldPlotProperty* plotProperty = NULL );
	virtual ~CGeoMap();

	virtual void Create( CObArray* data, const std::string& fieldName );

	int32_t GetNrMaps() { return m_nrMaps; };

	virtual void OnKeyframeChanged( uint32_t i );
	virtual void Close();
	virtual void Update();

	virtual void Update2D();

	void GetComputedRange( double& min, double& max ) { min = m_minhv; max = m_maxhv; }
	double GetComputedRangeMin() { return m_minhv; }
	vtkObArray* GetGeoMapFilterList() { return &m_geoMapFilterList; }

	double GetComputedRangeMax() { return m_maxhv; }

	std::string GetDataDateString() { return GetDataDateString( m_currentMap ); }
	std::string GetDataDateString( uint32_t index );

	const CDate* GetDataDate() { return GetDataDate( m_currentMap ); }
	const CDate* GetDataDate( uint32_t index ) { return dynamic_cast<CDate*>( m_dataDates[ index ] ); }

	virtual std::string GetDataTitle() { return GetDataTitle( m_currentMap ); }
	virtual std::string GetDataTitle( uint32_t index ) { return m_dataTitles[ index ]; }

	virtual std::string GetDataName() { return m_plotProperty.m_name; }

	std::string GetDataUnitString() { return GetDataUnitString( m_currentMap ); }
	std::string GetDataUnitString( uint32_t index );

	const CUnit* GetDataUnit() { return GetDataUnit( m_currentMap ); }
	const CUnit* GetDataUnit( uint32_t index ) { return dynamic_cast<CUnit*>( m_dataUnits[ index ] ); }

	virtual void SetProjection( int32_t proj );

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
	static void FindVisibleSpherePoints( void* arg );
	virtual void UpdateContourLabels();

	virtual void CreateContourLabels2D();
	virtual void UpdateContourLabels2D();

	static void FindVisiblePlanePoints( void* arg );

	void SetContourLabels2DPosition();

	bool GetContourLabelNeedUpdateOnWindow() { return m_contourLabelNeedUpdateOnWindow; }
	void SetContourLabelNeedUpdateOnWindow( bool value ) { m_contourLabelNeedUpdateOnWindow = value; }

	bool GetContourLabelNeedUpdatePositionOnContour() { return m_contourLabelNeedUpdatePositionOnContour; }
	void SetContourLabelNeedUpdatePositionOnContour( bool value ) { m_contourLabelNeedUpdatePositionOnContour = value; }

protected:
	void GetMapLatLon( CInternalFilesZFXY* zfxy,
		int32_t& width, int32_t& height,
		CExpressionValue& varLat, CExpressionValue& varLon );

	// for base classes
	CGeoMap( CWorldPlotProperty* plotProperty );

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

  CGeoVelocityMap(CPlotField* northField, CPlotField* eastField);
  CGeoVelocityMap(CObArray* northData, CObArray* eastData, const std::string& fieldNameNorth, const std::string& fieldNameEast, CWorldPlotProperty* plotProperty = NULL);
  virtual ~CGeoVelocityMap();

  void Create(CObArray* northData, CObArray* eastData, const std::string& fieldNameNorth, const std::string& fieldNameEast);

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


#endif		//GEO_MAP_H
