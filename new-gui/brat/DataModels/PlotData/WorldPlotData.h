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
#ifndef DATAMODELS_PLOTDATA_WORLDPLOT_DATA_H
#define DATAMODELS_PLOTDATA_WORLDPLOT_DATA_H


#include "libbrathl/brathl.h"

#include "PlotValues.h"
#include "WorldPlotCommonData.h"




class CWorldPlotData : public CWorldPlotCommonData
{
	using base_t = CWorldPlotCommonData;

public:
		//std::vector<bool> v4_valids;
		//std::vector<double> v4_vals;
		//std::vector<bool> v4_bits;
		//std::vector<double> v4_lons;
		//std::vector<double> v4_lats;

protected:

	CWorldPlotInfo mMaps;

	bool m_finished;
	uint32_t m_currentMap;
	uint32_t m_nrMaps;

	CObArray m_dataDates;
	CStringArray m_dataTitles;
	CObArray m_dataUnits;

	//vtkObArray m_geoMapFilterList;


	//vtkObArray m_labelPts;
	//vtkObArray m_labelIds;
	//vtkObArray m_labelData;

	bool m_contourLabelNeedUpdateOnWindow;
	bool m_contourLabelNeedUpdatePositionOnContour;

	double m_minhv;
	double m_maxhv;



	// v3 note: for base classes
	//
	CWorldPlotData( CWorldPlotProperties *plotProperty )
		: CWorldPlotCommonData( plotProperty )
	{}

public:
	CWorldPlotData( CPlotField* field )
		: CWorldPlotCommonData( field->m_worldProps )
	{
		Create( &field->m_internalFiles, field->m_name );
	}

	virtual ~CWorldPlotData()
	{}

protected:
	virtual void Create( CObArray *pinternal_files, const std::string& fieldName );

public:

	const CWorldPlotInfo& PlotInfo() const { return mMaps; }


	int32_t GetNrMaps() { return m_nrMaps; };

	//virtual void OnKeyframeChanged( uint32_t i ) override;
	virtual void Close() override;
	//virtual void Update() override;

    virtual void Update2D() override;

	void GetComputedRange( double& min, double& max ) { min = m_minhv; max = m_maxhv; }
	double GetComputedRangeMin() { return m_minhv; }
	//vtkObArray* GetGeoMapFilterList() { return &m_geoMapFilterList; }

	double GetComputedRangeMax() { return m_maxhv; }

	std::string GetDataDateString() { return GetDataDateString( m_currentMap ); }
	std::string GetDataDateString( uint32_t index );

	const CDate* GetDataDate() { return GetDataDate( m_currentMap ); }
	const CDate* GetDataDate( uint32_t index ) { return dynamic_cast<CDate*>( m_dataDates[ index ] ); }

    virtual std::string GetDataTitle() override { return GetDataTitle( m_currentMap ); }
    virtual std::string GetDataTitle( uint32_t index ) override { return m_dataTitles[ index ]; }

    virtual std::string GetDataName() override { return m_plotProperty.m_name; }

	std::string GetDataUnitString() { return GetDataUnitString( m_currentMap ); }
	std::string GetDataUnitString( uint32_t index );

	const CUnit* GetDataUnit() { return GetDataUnit( m_currentMap ); }
	const CUnit* GetDataUnit( uint32_t index ) { return dynamic_cast<CUnit*>( m_dataUnits[ index ] ); }

	//virtual void SetProjection( int32_t proj ) override;

    virtual void Set3D() override;
    virtual void Set2D() override;

	virtual void SetSolidColor3D();
	virtual void SetSolidColor2D();

	virtual void SetContour3D(){}
	virtual void SetContour2D(){}

	//void SetContourLabelProperties();
	//void ContourGenerateValues();
	//void SetContour3DProperties();
	//void SetContour2DProperties();

    virtual void CreateContourLabels() override {}
	//static void FindVisibleSpherePoints( void* arg );
    virtual void UpdateContourLabels() override {}

    virtual void CreateContourLabels2D() override {}
    virtual void UpdateContourLabels2D() override {}

	//static void FindVisiblePlanePoints( void* arg );

	//void SetContourLabels2DPosition();

	bool GetContourLabelNeedUpdateOnWindow() { return m_contourLabelNeedUpdateOnWindow; }
	void SetContourLabelNeedUpdateOnWindow( bool value ) { m_contourLabelNeedUpdateOnWindow = value; }

	bool GetContourLabelNeedUpdatePositionOnContour() { return m_contourLabelNeedUpdatePositionOnContour; }
	void SetContourLabelNeedUpdatePositionOnContour( bool value ) { m_contourLabelNeedUpdatePositionOnContour = value; }

//protected:
	static void GetMapLatLon( CInternalFilesZFXY* zfxy,
		int32_t& width, int32_t& height,
		CExpressionValue& varLat, CExpressionValue& varLon );
};




//-------------------------------------------------------------
//------------------- CWorldPlotVelocityData class --------------------
//-------------------------------------------------------------

class CWorldPlotVelocityData : public CWorldPlotData
{
	using base_t = CWorldPlotData;

protected:

   double m_minhvN;
   double m_maxhvN;

   double m_minhvE;
   double m_maxhvE;

   bool IsGlyph;
   double zoom;


   //vtkProj2DFilter *m_simple2DProjFilter;  // this could be of any implementation
   //vtkVelocityGlyphFilter *m_geoMapFilter;

   //// for glyphs
   //vtkMaskPoints*   m_pointMask;
   //vtkArrowSource* m_arrowSource;
   //vtkBratArrowSource* m_barrowSource;
   //vtkGlyph3D*  m_glyph;

   //only for vtk derived classes (globe, etc)
   //
   CWorldPlotVelocityData( CWorldPlotProperties* plotProperty = NULL )
	   :base_t( plotProperty )
   {}

   CWorldPlotVelocityData( CPlotField* field )
	   :base_t( field )
   {}

public:

  CWorldPlotVelocityData( CPlotField* northField, CPlotField* eastField );

  virtual ~CWorldPlotVelocityData();

protected:
	void Create(CObArray* northData, CObArray* eastData, const std::string& fieldNameNorth, const std::string& fieldNameEast);

public:
  //vtkVelocityGlyphFilter * GetGlyphFilter() { return m_geoMapFilter; }

  //void SetIsGlyph(bool val);

  void SetZoom(double z) { zoom =z; }

protected:
   //vtkProj2DFilter* GetVtkFilter();

   virtual void Set2D();
   virtual void Set3D();

   virtual void Set2DGlyphs();
   virtual void Set3DGlyphs();

};


#endif		//DATAMODELS_PLOTDATA_WORLDPLOT_DATA_H
