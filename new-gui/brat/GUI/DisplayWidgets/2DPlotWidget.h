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
#ifndef GUI_DISPLAY_WIDGETS_2D_PLOT_WIDGET_H
#define GUI_DISPLAY_WIDGETS_2D_PLOT_WIDGET_H

#include <QMainWindow>

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

#include <qwt_color_map.h>				//spectogram
#include <qwt_plot_spectrogram.h>		//spectogram
#include <qwt_scale_widget.h>			//spectogram
#include <qwt_scale_draw.h>				//spectogram
#include <qwt_plot_zoomer.h>			//spectogram
#include <qwt_plot_panner.h>			//spectogram
#include <qwt_plot_layout.h>			//spectogram
#include <qwt_scale_engine.h> 			//2D Scaling objects

#include "DataModels/PlotData/MapColor.h"

#include "PlotsGraphicParameters.h"


class QwtPlotCurve;
class QwtData;
class QwtPlotZoomer;
class C2DMagnifier;
class QwtPlotPanner;
class CHistogram;
class C2DZoomer;
class C2DMagnifier;
class CBratSpectrogram;
class CBratScaleDraw;

class CYFXValues;

template< class PARAMS >
class CGenericZFXYValues;
struct CZFXYPlotParameters;
using CZFXYValues = CGenericZFXYValues< CZFXYPlotParameters >;




static_assert( QwtSymbol::StyleCnt == (int)EPointGlyph_size, "Mapping from qwt symbols to brat glyphs must be updated" );
static_assert( QwtSymbol::Ellipse == (int)eFirstPointGlyph, "Mapping from qwt symbols to brat glyphs must be updated" );


template< typename SYMBOL_TO, typename SYMBOL_FROM >
SYMBOL_TO symbol_cast( SYMBOL_FROM s );


template<>
inline EPointGlyph symbol_cast< EPointGlyph >( QwtSymbol::Style s )
{
    //TODO assert__( s >= (int)eFirstPointGlyph && s < (int)EPointGlyph_size );

	return static_cast< EPointGlyph >( s );
}

template<>
inline QwtSymbol::Style symbol_cast< QwtSymbol::Style >( EPointGlyph g )
{
	assert__( g >= eFirstPointGlyph && g < EPointGlyph_size );

	return static_cast< QwtSymbol::Style >( g );
}



static_assert( Qt::PenStyle::DashDotDotLine == (int)EStipplePattern_size, "Mapping from Qt::PenStyle to brat stipple patterns must be updated" );
static_assert( Qt::PenStyle::SolidLine == eFirstStipplePattern +1, "Mapping from Qt::PenStyle to brat stipple patterns must be updated" );


template< typename PATTERN_TO, typename PATTERN_FROM >
PATTERN_TO pattern_cast( PATTERN_FROM s );


template<>
inline EStipplePattern pattern_cast< EStipplePattern >( Qt::PenStyle s )
{
    assert__( (int)s >= ( eFirstPointGlyph + 1 ) && (int)s < ( EPointGlyph_size + 1 ) );

	return static_cast< EStipplePattern >( s - 1 );
}

template<>
inline Qt::PenStyle pattern_cast< Qt::PenStyle >( EStipplePattern p )
{
	assert__( p >= eFirstStipplePattern && p < EStipplePattern_size );

	return static_cast< Qt::PenStyle >( p + 1 );
}




//////////////////////////////////////////////////////////////////
//						Custom Curves
//////////////////////////////////////////////////////////////////



template< class DATA >
class CPartialCurve : public QwtPlotCurve
{
	// types

	using base_t = QwtPlotCurve;

protected:

	using data_t = DATA;


	//instance data

	double mMinXValue = 0.;
	double mMaxXValue = 0.;

	double mMinYValue = 0.;
	double mMaxYValue = 0.;

	bool mRangeComputed = false;

	const data_t *mData = nullptr;

	//construction / destruction

	void CommonConstruct()
	{
		if ( mData )
			setData( *mData );
	}
public:
	explicit CPartialCurve( const data_t *data )
		: base_t()
		, mData( data )
	{
		CommonConstruct();
	}
	explicit CPartialCurve( const data_t *data, const QwtText &title )
        : base_t( title )
		, mData( data )
	{
		CommonConstruct();
	}
	explicit CPartialCurve( const data_t *data, const QString &title )
        : base_t( title )
		, mData( data )
	{
		CommonConstruct();
	}

	
	// access / assignment

public:

	// QWT interface


	// This overload is needed when using auto-scale
	//
	virtual QwtDoubleRect boundingRect() const override;


	// This is a slow implementation: it might be worth to cache valid data ranges.
	//
	virtual void draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to ) const override;


	// internal processing

private:
	static bool isNaN( double x )
	{
		return x != x;
	}

	void ComputeRange();
};





class CGeneralizedCurve : public QObject, public CPartialCurve< CYFXValues >
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// types

	using base_t = CPartialCurve< CYFXValues >;


	//instance data

	bool mAnimForward = true;
	bool mLoop = true;

	std::string mXtitle;
	std::string mYtitle;
	std::string mZtitle;


	//construction / destruction

public:
	explicit CGeneralizedCurve( const CYFXValues *data );
	explicit CGeneralizedCurve( const CYFXValues *data, const QwtText &title );
	explicit CGeneralizedCurve( const CYFXValues *data, const QString &title );

	
	// access / assignment

	void Ranges( double &xMin, double &xMax, double &yMin, double &yMax );
	void SetRanges( double xMin, double xMax, double yMin, double yMax );

	void AxisTitles( std::string &xtitle, std::string &ytitle, std::string &ztitle );
	void SetAxisTitles( const std::string &xtitle, const std::string &ytitle, const std::string &ztitle );



	bool MovingForward() const { return mAnimForward;}

	void SetMovingForward( bool forward );

	void SetLoop( bool loop );

	bool ResetFrame();

	bool SetFrame( size_t frame );

	bool ChangeFrame();

	size_t CurrentFrame();

protected:
	bool FrameUpdated();
};





//////////////////////////////////////////////////////////////////
//		2D Plot Widget
//////////////////////////////////////////////////////////////////

class C2DPlotWidget : public QwtPlot, public CPlotsGraphicParameters
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// types

	using base_t = QwtPlot;
	using graphic_parameters_base_t = CPlotsGraphicParameters;


	//static members

public:
	static const std::string smHistogramXtitle;
	static const std::string smHistogramYtitle;

private:
	//static CHistogram* CreateHistogram near respective instance methods

	static void Save2All( C2DPlotWidget *p, const QString &path );
	// ok
	static bool Save2svg( C2DPlotWidget *p, const QString &path );
	// unsuccessful
	static bool Save2unsupported( C2DPlotWidget *p, const QString &path, const QString &format );
	static bool Save2gif( C2DPlotWidget *p, const QString &path );
	static bool Save2ps( C2DPlotWidget *p, const QString &path );


	//instance data
	
    bool mIsLog = false;

	C2DMagnifier *mMagnifier = nullptr;
	QwtPlotPanner *mPanner = nullptr;
	C2DZoomer *mZoomer = nullptr; 

	QwtLegend *mLegend = nullptr; 

    std::vector< CBratSpectrogram* > mSpectrograms;
	CBratSpectrogram *mCurrentSpectrogram = nullptr;
	std::vector< CHistogram* > mHistograms;
	CHistogram *mCurrentHistogram = nullptr;
	std::vector< CGeneralizedCurve* > mCurves;

    CBratScaleDraw *mXScaler = nullptr;
    CBratScaleDraw *mYScaler = nullptr;
	double mXScaleFactor = 1.;
	double mYScaleFactor = 1.;
	double mY2ScaleFactor = 1.;


	QTimer mTimer;



	/////////////////////////////
	// construction / destruction
	/////////////////////////////

public:

    C2DPlotWidget( const CPlotsGraphicParameters &plots_graphic_parameters, QWidget *parent = nullptr );

	virtual ~C2DPlotWidget();


	void Clear();


	///////////
	// general
	///////////

    void SetPlotTitle( const std::string &title );


	////////////////////////////
	// persistence (image only)
	//		- see also unsupported format tests in static section
	////////////////////////////

	bool Save2Image( const QString &path, const QString &format, const QString &extension );


	////////////
	// animation
	////////////

	void Animate( int updateinterval );

	void StopAnimation();

	void SetMovingForward( bool forward );

	void SetFrame( unsigned int frame );

	void SetSpeed( int updateinterval );

	void SetLoop( bool loop );


	///////////
	// axis
	///////////

	void EnableAxisY2( bool enable = true ) 
	{ 
		enableAxis( QwtPlot::yRight, enable );
	}


	//...titles

	void CurrentAxisTitles( std::string *xtitle, std::string *ytitle, std::string *y2title ) const;
protected:
	void SetCurrentAxisTitle( Axis axis, const std::string &title );
	void SetCurrentAxisTitles( const std::string &xtitle, const std::string &ytitle, const std::string &y2title );

public:
	//void AllCurvesAxisTitles( std::string &xtitle, std::string &ytitle, std::string &y2title ) const { CurrentAxisTitles( &xtitle, &ytitle, nullptr ); }
	void SetAllCurvesAxisTitles( const std::string &xtitle, const std::string &ytitle );

	void SpectrogramAxisTitles( int index, std::string &xtitle, std::string &ytitle, std::string &y2title ) const;
	void SetSpectrogramAxisTitles( int index, const std::string &xtitle, const std::string &ytitle, const std::string &y2title );


	//...ticks

    int XAxisTicks() const
    {
        return axisMaxMajor( QwtPlot::xBottom );
    }
    void SetXAxisTicks(int new_value)
    {
        setAxisMaxMajor( QwtPlot::xBottom, new_value );
        replot();
    }

	int YAxisTicks() const
    {
        return axisMaxMajor( QwtPlot::yLeft );
    }
    void SetYAxisTicks( int new_value )
    {
        setAxisMaxMajor( QwtPlot::yLeft, new_value );
        replot();
    }

	int Y2AxisTicks() const
    {
        return axisMaxMajor( QwtPlot::yRight );
    }
    void SetY2AxisTicks( int new_value )
    {
        setAxisMaxMajor( QwtPlot::yRight, new_value );
        replot();
    }



    //...digits / date

    int XAxisDigits() const;
    int YAxisDigits() const;
    int Y2AxisDigits() const;

    bool XAxisIsDateTime() const;
    bool YAxisIsDateTime() const;
    bool Y2AxisIsDateTime() const;

protected:
	void SetDigits( Axis axisId, bool isdate, int digits, brathl_refDate date_ref );

public:
	void SetXAxisDigits( bool isdate, int digits, brathl_refDate date_ref = REF19500101 );
    void SetYAxisDigits( bool isdate, int digits, brathl_refDate date_ref = REF19500101 );
    void SetY2AxisDigits( bool isdate, int digits, brathl_refDate date_ref = REF19500101 );


    //...scaling

protected:
    void SetLogScale(int axisId, bool _isLog);

	void CurrentHistogramRanges( double &xMin, double &xMax, double &yMin, double &yMax );
	void CurrentSpectrogramRanges( double &xMin, double &xMax, double &yMin, double &yMax, double &y2Min, double &y2Max );
	void AxisRanges( double &xMin, double &xMax, double &yMin, double &yMax, double &y2Min, double &y2Max );

	void AxisScales( double &xMin, double &xMax, double &yMin, double &yMax, double &y2Min, double &y2Max );
	void SetAxisScales( double xMin, double xMax, double yMin, double yMax, double y2Min = defaultValue<double>(), double y2Max = defaultValue<double>(), bool plot = true );

public:
	void SetPlotAxisRanges( int index, double xMin, double xMax, double yMin, double yMax, double y2Min = defaultValue<double>(), double y2Max = defaultValue<double>(), bool plot = true );
	void RescaleX( double x, bool plot = true );
	void RescaleY( double y, bool plot = true );


	///////////
	// legends
	///////////

	void AddLegend( LegendPosition pos, bool checkable = false );



	///////////
	// raster
	///////////

	QwtPlotSpectrogram* PushRaster( const std::string &title, const CZFXYValues &maps, double min_contour, double max_contour, size_t ncontours, const QwtColorMap &color_map );

	//switch plot
	void SetCurrentRaster( int index );

    bool HasContour() const;
    void ShowContour( bool show );

    bool HasSolidColor() const;
    void ShowSolidColor( bool show );

	int NumberOfContours() const;
	void SetNumberOfContours( double min_contour, double max_contour, size_t ncontours );
	
	void ShowContour( int index, bool show );
    void ShowSolidColor( int index, bool show );    

	void SetRasterColorMap( const QwtColorMap &color_map );


	////////////
	// histogram
	////////////

	int NumberOfBins( int index ) const;

protected:
	template< typename DATA >
	CHistogram* CreateHistogram( const std::string &title, QColor color, const DATA &data, double &max_freq, int bins, bool isdate, brathl_refDate date_ref = REF19500101 );

public:
	CHistogram* PushHistogram( const std::string &title, QColor color, const CYFXValues &data, double &max_freq, int bins, bool isdate, brathl_refDate date_ref = REF19500101 );
	CHistogram* SetSingleHistogram( const std::string &title, QColor color, const CZFXYValues &data, double &max_freq, int bins, bool isdate, brathl_refDate date_ref = REF19500101 );

	void SetCurrentHistogram( int index );


	//////////
	// curves
	//////////

	QwtPlotCurve* PushCurve( const std::string &title, QColor color, const CYFXValues *data = nullptr );
	QwtPlotCurve* PushCurve( const std::string &title, QColor color, const CYFXValues &data )
	{
		return PushCurve( title, color, &data );
	}
	//QwtPlotCurve* PushCurve( const QwtData &data, const std::string &title, QColor color );	//for experimental samples


	// curve line

	void EnableCurveLine( int curve, bool enable );

	QColor CurveLineColor( int curve ) const;
	void SetCurveLineColor( int curve, QColor color );
	void SetCurveLineColor( int curve, CPlotColor color )
	{
		SetCurveLineColor( curve, color_cast<QColor>( color ) );
	}

	int CurveLineOpacity( int curve ) const;
	void SetCurveLineOpacity( int curve, int alpha );

	EStipplePattern CurveLinePattern( int curve ) const;
	void SetCurveLinePattern( int curve, EStipplePattern p );

	int CurveLineWidth( int curve ) const;
	void SetCurveLineWidth( int curve, int pixels );


	// curve points

	void EnableCurvePoints( int curve, bool enable, EPointGlyph default_symbol );

	QColor CurvePointColor( int curve ) const;
	void SetCurvePointColor( int curve, QColor color );
	void SetCurvePointColor( int curve, CPlotColor color )
	{
		SetCurvePointColor( curve, color_cast<QColor>( color ) );
	}

	bool IsCurvePointFilled( int curve ) const;
	void SetCurvePointFilled( int curve, bool fill );

	EPointGlyph CurvePointGlyph( int curve, EPointGlyph default_symbol ) const;
	void SetCurvePointGlyph( int curve, EPointGlyph symbol );

	int CurvePointSize( int curve ) const;
	void SetCurvePointSize( int curve, int pixels );


	///////////////////////////////////////////////
	///////////////////////////////////////////////

	// markers

	QwtPlotMarker* AddMarker( const std::string &label, Qt::Alignment label_alignment, Qt::Orientation label_orientation, 
		QwtPlotMarker::LineStyle st, double value );

	QwtPlotMarker* AddMarker( const std::string &label, QwtPlotMarker::LineStyle st, double value );


	// interaction

	QwtPlotZoomer* AddZoomer();

	C2DMagnifier* AddMagnifier( bool enable_x = true, bool enable_y = true, bool enable_y2 = true );

	QwtPlotPanner* AddPanner();

	void Home();


protected:
	void SetCurvesStyle( QwtPlotCurve::CurveStyle style );

	void Spectogram( QWidget *parent = nullptr );
	void SimplePlot();

    virtual void replot() override;

public:
    QSize sizeHint() const override;

signals:
	void FrameChanged( size_t iframe );
	void AnimationStopped();
	void ScaleDivChanged( int iaxis, double factor, QString range );

public slots:
    void printPlot();

protected slots:
	void HandleScaleDivChanged();
	void ChangeFrame();

	void ShowFitCurve( QwtPlotItem *item, bool on );
};



#endif			// GUI_DISPLAY_WIDGETS_2D_PLOT_WIDGET_H
