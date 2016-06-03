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

#ifndef DATA_MODELS_PLOT_DATA_XYPLOTDATA_H
#define DATA_MODELS_PLOT_DATA_XYPLOTDATA_H

#include <cmath>


#if defined(BRAT_V3)
	#include <vtkProperty2D.h>

	class VTK_CXYPlotData;

#endif

//#include <vtkDataArray.h>
//#include <vtkDoubleArray.h>
//
//#include "vtkPlotData.h"		//VISAN

#include "new-gui/Common/tools/Exception.h"
#include "new-gui/Common/+Utils.h"

#include "libbrathl/brathl.h"
#include "libbrathl/BratObject.h"
#include "libbrathl/Tools.h"
#include "libbrathl/List.h"
#include "libbrathl/Date.h"
#include "libbrathl/Unit.h"

#include "libbrathl/InternalFiles.h"
#include "libbrathl/InternalFilesZFXY.h"
#include "libbrathl/InternalFilesYFX.h"
using namespace brathl;

#include "PlotValues.h"
#include "DataModels/PlotData/MapColor.h"
#include "DataModels/PlotData/XYPlot.h"


class CXYPlotData;
class CPlotColor;


//// WDR: class declarations
////-------------------------------------------------------------
////------------------- vtkDoubleArrayBrathl class
////-------------------------------------------------------------
//class vtkDoubleArrayBrathl : public vtkDoubleArray
//{
//	// redefinition with suffix 2 because these attributes (without suffix 2) are private in vtkDataArray
//	double Range2[ 2 ];
//
//	// 5 components since you can compute the range of components
//	// less than 0 to get a magnitude range. ComponentRange[4] is 
//	// this magnitude range
//
//	vtkTimeStamp ComponentRangeComputeTime2[ 5 ];
//	double ComponentRange2[ 5 ][ 2 ];
//
//protected:
//
//	vtkDoubleArrayBrathl( vtkIdType numComp=1 ) : vtkDoubleArray( numComp )
//	{}
//	~vtkDoubleArrayBrathl()
//	{}
//
//public:
//	static vtkDoubleArrayBrathl* New();
//	void PrintSelf( std::ostream& os, vtkIndent indent );
//
//	// Description:
//	// Return the range of the array values for the 0th component. 
//	// Range is copied into the array provided.
//	double* GetRange2()
//	{
//		this->ComputeRange( 0 );
//		return this->Range2;
//	}
//private:
//	virtual void ComputeRange( int comp );
//};
//


//-------------------------------------------------------------
//------------------- vtkDataArrayPlotData class
//-------------------------------------------------------------
//class vtkDataArrayPlotData : public vtkPlotData
//{
//public:
//	vtkTypeMacro( vtkDataArrayPlotData, vtkPlotData );
//	static vtkDataArrayPlotData *New() { return new vtkDataArrayPlotData; }
//
//protected:
//	void SetXDataArray( vtkDoubleArrayBrathl *dataArray );
//	void SetYDataArray( vtkDoubleArrayBrathl *dataArray );
//	void SetZDataArray( vtkDoubleArrayBrathl *dataArray );
//
//public:
//	void SetDataArrays( vtkDoubleArrayBrathl *XdataArray, vtkDoubleArrayBrathl *YdataArray )
//	{
//		SetXDataArray( XdataArray );
//		SetYDataArray( YdataArray );
//	}
//
//	vtkDataArray *GetXDataArray() { return this->XDataArray; }
//	vtkDataArray *GetYDataArray() { return this->YDataArray; }
//	vtkDataArray *GetZDataArray() { return this->ZDataArray; }
//
//	virtual double GetXValue( int i );
//	virtual double GetYValue( int i );
//	virtual double GetZValue( int i );
//	virtual int GetNumberOfItems();
//
//	// Description:
//	// Override GetMTime because we depend on vtkDataArray objects
//	unsigned long GetMTime();
//
//protected:
//	vtkDataArrayPlotData();
//	virtual ~vtkDataArrayPlotData();
//
//	// Description:
//	// Override Execute so we can update NumberOfItems;
//	virtual void Execute();
//
//	vtkDoubleArrayBrathl *XDataArray;
//	vtkDoubleArrayBrathl *YDataArray;
//	vtkDoubleArrayBrathl *ZDataArray;
//
//	int NumberOfItems;
//
//private:
//	vtkDataArrayPlotData( const vtkDataArrayPlotData& );  // Not implemented.
//	void operator=( const vtkDataArrayPlotData& );  // Not implemented.
//};
//



//-------------------------------------------------------------
//------------------- CPlotArray class --------------------
//-------------------------------------------------------------

//class CPlotArray : public CBratObject
//{
//protected:
//	bool m_didCalculateRange;
//	double m_min;
//	double m_max;
//
//	vtkObArray m_rows;
//
//public:
//	CPlotArray();
//	virtual ~CPlotArray();
//	vtkDoubleArrayBrathl* GetFrameData( size_t r );
//	vtkDoubleArrayBrathl* GetFrameData( vtkObArray::iterator it );
//
//	void SetFrameData( uint32_t r, double* vect, int32_t size );
//	void SetFrameData( uint32_t r, vtkDoubleArrayBrathl* vect );
//
//	void GetRange( double& min, double& max );
//
//	void GetRange( double& min, double& max, uint32_t frame );
//
//	size_t GetNumberOfFrames() { return m_rows.size(); }
//
//	void Normalize( int32_t value = 1 );
//};
//
//-------------------------------------------------------------
//------------------- CXYPlotProperties class --------------------
//-------------------------------------------------------------



class CXYPlotProperties : public CBratObject
{
#if defined(BRAT_V3)
	vtkProperty2D* m_vtkProperty2D;
#endif

	CXYPlotData* m_parent;

	CPlotColor mColor;
	CPlotColor mPointColor = CPlotColor(0.,0.,0.);
	int mOpacity = 0xFF;
	double mLineWidth = 2.;

#if !defined(BRAT_V3)

	EStipplePattern mStipplePattern = EStipplePattern::eSolidLine;
#endif

	bool m_lines;
	bool m_points;

	double m_pointSize;

	std::string m_name;
	std::string m_title;
	std::string m_xAxis;
	std::string m_xLabel;
	std::string m_yLabel;

	double m_xMax;
	double m_yMax;
	double m_xMin;
	double m_yMin;

    double m_xMinCurr;
    double m_yMinCurr;
    double m_xMaxCurr;
    double m_yMaxCurr;

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

    uint32_t m_xNbDigits = 3;	//v4 (...)
    uint32_t m_yNbDigits = 3;	//v4 (...)


	EPointGlyph m_glyphType;
	bool m_filledPoint;

	double m_lineWidthFactor;
	double m_opacityFactor;

	bool m_hide;

protected:
	bool HasParent();
	void Copy( const CXYPlotProperties& p );

public:

	CXYPlotProperties( CXYPlotData* parent = NULL );
	CXYPlotProperties( CXYPlotProperties& p );

	virtual ~CXYPlotProperties();

	const CXYPlotProperties& operator=( const CXYPlotProperties& p );

#if defined(BRAT_V3)

	VTK_CXYPlotData* parent();

	vtkProperty2D* GetVtkProperty2D() { return m_vtkProperty2D; }

	double GetLineWidth() const { return m_vtkProperty2D->GetLineWidth(); }
	double GetOpacity() const { return m_vtkProperty2D->GetOpacity(); }

	EStipplePattern GetLineStipple() const { return static_cast<EStipplePattern>( m_vtkProperty2D->GetLineStipplePattern() ); }
	void SetLineStipple( EStipplePattern value ) { m_vtkProperty2D->SetLineStipplePattern( value ); }

	void GetColor( double* value ) const { m_vtkProperty2D->GetColor( value ); }
	void SetColor( double* value ) { m_vtkProperty2D->SetColor( value ); }

	void SetColor( double r, double g, double b ) { m_vtkProperty2D->SetColor( r, g, b ); }

#else
	int GetOpacity() const { return mOpacity; }

	int GetLineWidth() const { return std::round( mLineWidth ); }

	EStipplePattern GetStipplePattern() { return mStipplePattern; }
	void SetStipplePattern( EStipplePattern e ) { mStipplePattern = e; }

#endif
	void SetOpacity( int op ) { mOpacity = op; }
	void SetLineWidth( double value );


	CXYPlotData* GetParent() const { return m_parent; }
	void SetParent( CXYPlotData* parent ) { m_parent = parent; }

	bool GetPoints() const { return m_points; }
	void SetPoints();
	void SetPoints( bool value );

	double GetPointSize() const { return m_pointSize; }
	void SetPointSize();
	void SetPointSize( double value );

	bool GetLines() const { return m_lines; }
	void SetLines();
	void SetLines( bool value );


	std::string GetName() const { return m_name; }
	void SetName( const std::string& value );

	void SetOpacity( double value );

	bool GetXLog() const { return m_xLog; }
	void SetXLog( bool value );

	bool GetYLog() const { return m_yLog; }
	void SetYLog( bool value );

	void GetXRange( double& min, double& max ) { min = m_xMin; max = m_xMax; }
	void SetXRange( double min, double max ) { m_xMin = min; m_xMax = max; }

	void GetYRange( double& min, double& max ) { min = m_yMin; max = m_yMax; }
	void SetYRange( double min, double max ) { m_yMin = min; m_yMax = max; }

	double GetXMin() const { return m_xMin; }
	double GetXMax() const { return m_xMax; }
	void SetXMin( double value ) { m_xMin = value; }
	void SetXMax( double value ) { m_xMax = value; }

	double GetYMin() const { return m_yMin; }
	double GetYMax() const { return m_yMax; }
	void SetYMin( double value ) { m_yMin = value; }
	void SetYMax( double value ) { m_yMax = value; }

    double GetCurrXMin() const { return m_xMinCurr; }
    double GetCurrXMax() const { return m_xMaxCurr; }
    void SetCurrXMin( double value ) { m_xMinCurr = value; }
    void SetCurrXMax( double value ) { m_xMaxCurr = value; }

    double GetCurrYMin() const { return m_yMinCurr; }
    double GetCurrYMax() const { return m_yMaxCurr; }
    void SetCurrYMin( double value ) { m_yMinCurr = value; }
    void SetCurrYMax( double value ) { m_yMaxCurr = value; }

	double GetXBase() const { return m_xBase; }
	double GetYBase() const { return m_yBase; }
	void SetXBase( double value ) { m_xBase = value; }
	void SetYBase( double value ) { m_yBase = value; }

	std::string GetTitle() const { return m_title; }
	void SetTitle( const std::string& value ) { m_title = value; }

	std::string GetXAxis() const { return m_xAxis; }
	void SetXAxis( const std::string& value ) { m_xAxis = value; }

	std::string GetXLabel() const { return m_xLabel; }
	void SetXLabel( const std::string& value ) { m_xLabel = value; }

	std::string GetYLabel() const { return m_yLabel; }
	void SetYLabel( const std::string& value ) { m_yLabel = value; }

	bool GetShowPropertyPanel() const { return m_showPropertyPanel; }
	void SetShowPropertyPanel( bool value ) { m_showPropertyPanel = value; }

	bool GetShowAnimationToolbar() const { return m_showAnimationToolbar; }
	void SetShowAnimationToolbar( bool value ) { m_showAnimationToolbar = value; }

	bool GetFocus() const { return m_focus; }
	void SetFocus( bool value );

	bool GetLoop() const { return m_loop; }
	void SetLoop( bool value ) { m_loop = value; }

	uint32_t GetFps() const { return m_fps; }
	void SetFps( uint32_t value ) { m_fps = value; }

	unsigned int GetXNumTicks() const { return m_xNumTicks; }
	unsigned int GetYNumTicks() const { return m_yNumTicks; }
	void SetXNumTicks( unsigned int value ) { m_xNumTicks = value; }
	void SetYNumTicks( unsigned int value ) { m_yNumTicks = value; }

    uint32_t GetXNbDigits() const { return m_xNbDigits; }
    uint32_t GetYNbDigits() const { return m_yNbDigits; }
    void SetXNbDigits( uint32_t value ) { m_xNbDigits = value; }
    void SetYNbDigits( uint32_t value ) { m_yNbDigits = value; }

	int32_t GetNormalizeX() const { return m_normalizeX; }
	int32_t GetNormalizeY() const { return m_normalizeY; }
	void SetNormalizeX();
	void SetNormalizeY();
	void SetNormalizeX( int32_t value );
	void SetNormalizeY( int32_t value );

	CPlotColor GetColor() const;
	void GetColor( CPlotColor& color ) const ;
	void SetColor( const CPlotColor& color );

	const CPlotColor& GetPointColor() const { return mPointColor; }
	void SetPointColor( const CPlotColor &color ) { mPointColor = color; }

	EPointGlyph GetPointGlyph() const { return m_glyphType; }
	void SetPointGlyph( EPointGlyph value );
	void SetPointGlyph();

	bool GetFilledPoint() const { return m_filledPoint; }
	void SetFilledPoint( bool value );
	void SetFilledPoint();

	double GetLineWidthFactor() const { return m_lineWidthFactor; }
	void SetLineWidthFactor( double value ) { m_lineWidthFactor = value; }
	double GetOpacityFactor() const { return m_opacityFactor; }
	void SetOpacityFactor( double value ) { m_opacityFactor = value; }

	bool GetHide() const { return m_hide; }
	void SetHide( bool value );
	void SetHide();

	void Update();
};


//-------------------------------------------------------------
//------------------- CXYPlotData class --------------------
//-------------------------------------------------------------

class CXYPlotData : public CBratObject
{

protected:
	CXYPlotProperties m_plotProperty;

	int32_t m_currentFrame;
	int32_t m_length;


	CQwtArrayPlotData mQwtPlotData;

	CUnit m_unitX;
	CUnit m_unitY;

	CExpressionValue m_varComplement;

	CFieldNetCdf m_fieldComplement;


	CObArray m_otherVars;

	CObArray m_otherFields;


	bool m_aborted;

public:
	CXYPlotData( CPlot* plot, int32_t iField );

	virtual ~CXYPlotData();

protected:
	void Create(CInternalFiles* yfx, CPlot* plot, int32_t iField);

public:


	virtual void GetXRange( double& min, double& max, uint32_t frame );
	virtual void GetYRange( double& min, double& max, uint32_t frame );

	virtual void GetXRange( double& min, double& max );
	virtual void GetYRange( double& min, double& max );

	virtual void NormalizeX( int32_t value );
	virtual void NormalizeY( int32_t value );
	virtual void Normalize( int32_t value );

	virtual int32_t GetNumberOfFrames();


	const CUnit& GetXUnit(){ return m_unitX; }
	const CUnit& GetYUnit(){ return m_unitY; }

	void SetXUnit( const CUnit& unit ) { m_unitX = unit; }
	void SetYUnit( const CUnit& unit ) { m_unitY = unit; }

	virtual void SetData( double* xArray, const CUIntArray& xDims,
		double* yArray, const CUIntArray& yDims,
		const CUIntArray& xCommonDimIndex,
		const CUIntArray& yCommonDimIndex );

	std::string GetName() { return m_plotProperty.GetName(); }
	void SetName( const std::string& name ) { m_plotProperty.SetName( name ); }

	const CQwtArrayPlotData*  GetQwtArrayPlotData() const { return &mQwtPlotData; }

	CXYPlotProperties* GetPlotProperties() { return &m_plotProperty; }

    CExpressionValue& GetVarComplement() { return m_varComplement; }

    CFieldNetCdf& GetFieldComplement() { return m_fieldComplement; }

    const CObArray& GetOtherVars() { return m_otherVars; }
    const CObArray& GetOtherFields() { return m_otherFields; }

#if defined(BRAT_V3)

	void OnFrameChange( int32_t f );
	virtual void Update();

#endif
};


//-------------------------------------------------------------
//------------------- CXYPlotDataCollection class --------------------
//-------------------------------------------------------------

class CXYPlotDataCollection : public CObArray
{
	using base_t = CObArray;

	bool m_zoomToDataRange;
	int32_t m_currentFrame;

public:

	CXYPlotDataCollection()
		: m_zoomToDataRange( false )
		, m_currentFrame( 0 )
	{}
	virtual ~CXYPlotDataCollection()
	{}

	void AddData( CBratObject* ob );

	void GetXRange( double& min, double& max, uint32_t frame );
	void GetYRange( double& min, double& max, uint32_t frame );

	void GetXRange( double& min, double& max );
	void GetYRange( double& min, double& max );

#if defined(BRAT_V3)
	void OnFrameChange( int32_t f );

	VTK_CXYPlotData* Get( CObArray::iterator it );
	VTK_CXYPlotData* Get( int32_t index );
#else
	CXYPlotData* Get( CObArray::iterator it );
	CXYPlotData* Get( int32_t index );
#endif
	CXYPlotProperties* GetPlotProperties( int32_t index );

	bool ShowPropertyMenu();

	void GetNames( std::vector<std::string>& names );

	bool IsZoomToDataRange() { return m_zoomToDataRange; };
	void SetZoomToDataRange( bool value ) { m_zoomToDataRange = value; };

	int32_t GetCurrentFrame() { return m_currentFrame; };
	void SetCurrentFrame( int32_t value ) { m_currentFrame = value; };
};


#endif			//DATA_MODELS_PLOT_DATA_XYPLOTDATA_H
