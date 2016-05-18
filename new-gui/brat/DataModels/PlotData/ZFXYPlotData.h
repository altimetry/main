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
#ifndef DATA_MODELS_PLOT_DATA_ZFXYPLOTDATA_H
#define DATA_MODELS_PLOT_DATA_ZFXYPLOTDATA_H

#include "new-gui/Common/tools/Exception.h"

#include "libbrathl/brathl.h"

#include "libbrathl/BratObject.h"
#include "libbrathl/Tools.h"
#include "libbrathl/List.h"
#include "libbrathl/Date.h"
#include "libbrathl/Unit.h"

#include "libbrathl/InternalFiles.h"
#include "libbrathl/InternalFilesZFXY.h"

using namespace brathl;

#include "PlotValues.h"
#include "PlotColor.h"
#include "ZFXYPlot.h"

#include "new-gui/brat/DataModels/PlotData/BratLookupTable.h"



//-------------------------------------------------------------
//------------------- CZFXYPlotProperties class --------------------
//-------------------------------------------------------------

struct CZFXYPlotProperties : public CBratObject
{
	std::string m_title;
	std::string m_name;
	std::string m_xLabel;
	std::string m_yLabel;

	CBratLookupTable* mLUT = nullptr;

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

	unsigned int m_xNumTicks;
	unsigned int m_yNumTicks;
    unsigned int m_zNumTicks;


	bool m_withContour;
	bool m_withContourLabel;
	double m_minContourValue;
	double m_maxContourValue;
	int32_t m_numContour;
	int32_t m_numContourLabel;

	int32_t m_contourLabelSize;
	double m_contourLineWidth;

	CPlotColor m_contourLineColor;
	CPlotColor m_contourLabelColor;

	std::string m_contourLabelFormat;

	bool m_solidColor;

	bool m_withAnimation;


	//construction / destruction

	CZFXYPlotProperties();

	CZFXYPlotProperties( const CZFXYPlotProperties& p );

	virtual ~CZFXYPlotProperties();

	const CZFXYPlotProperties& operator=( const CZFXYPlotProperties& p );


private:

	void DeleteLUT();

	void Copy( const CZFXYPlotProperties& p );
};




//-------------------------------------------------------------
//------------------- CZFXYPlotData class --------------------
//-------------------------------------------------------------

class CZFXYPlotData : public CBratObject
{
protected:
	bool m_aborted;

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

	CUnit m_unitX;
	CUnit m_unitY;

	C3DPlotInfo mMaps;

	CBratLookupTable *mLUT = nullptr;

public:
	CZFXYPlotProperties m_plotProperties;

	//construction / destruction

	void Create( CObArray* data, const std::string& fieldName, CZFXYPlot* plot );
public:
	CZFXYPlotData( CZFXYPlot* plot, CPlotField* field );

	virtual ~CZFXYPlotData();

	// access

	const C3DPlotInfo& Maps() const { return mMaps; }

	const CUnit& GetXUnit() const { return m_unitX; }
	const CUnit& GetYUnit() const { return m_unitY; }

	uint32_t GetCurrentMap() const { return m_currentMap; }

	virtual const std::string& GetDataTitle() const { return GetDataTitle( m_currentMap ); }
	virtual const std::string& GetDataTitle( uint32_t index ) const { return m_dataTitles[ index ]; }

	virtual const std::string& GetDataName() const { return m_plotProperties.m_name; }

	bool GetContour() const { return m_plotProperties.m_withContour; }
	void SetContour( bool value ) { m_plotProperties.m_withContour = value; }

	bool GetSolidColor() const { return m_plotProperties.m_solidColor; }
	void SetSolidColor( bool value ) { m_plotProperties.m_solidColor = value; }

	void GetComputedRange( double& min, double& max ) { min = m_minhv; max = m_maxhv; }
	double GetComputedRangeMin() { return m_minhv; }
	double GetComputedRangeMax() { return m_maxhv; }

	void GetXRange( double& min, double& max, size_t frame ) const;
	void GetXRange( double& min, double& max ) const { min = m_xMin; max = m_xMax; }
	double GetXRangeMin() const { return m_xMin; }
	double GetXRangeMax() const { return m_xMax; }

	void GetYRange( double& min, double& max, size_t frame ) const;
	void GetYRange( double& min, double& max ) const { min = m_yMin; max = m_yMax; }
	double GetYRangeMin() const { return m_yMin; }
	double GetYRangeMax() const { return m_yMax; }


	int32_t GetNrMaps() const { return m_nrMaps; }

	CZFXYPlotProperties* GetPlotProperties() { return &m_plotProperties; }


	std::string GetDataDateString() const { return GetDataDateString( m_currentMap ); }
	std::string GetDataDateString( size_t index ) const;

	std::string GetDataUnitString() const { return GetDataUnitString( m_currentMap ); }
	std::string GetDataUnitString( size_t index ) const;


	CBratLookupTable* GetLUT() { return mLUT; }

	virtual void SetLUT( CBratLookupTable* lut );

	QLookupTable* GetLookupTable()
	{
		return mLUT ? mLUT->GetLookupTable() : nullptr;
	};

	void DeleteLUT();
};





#endif		//DATA_MODELS_PLOT_DATA_ZFXYPLOTDATA_H

