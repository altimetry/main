#ifndef GUI_DISPLAY_WIDGETS_2D_PLOT_WIDGET_H
#define GUI_DISPLAY_WIDGETS_2D_PLOT_WIDGET_H

#include <QMainWindow>

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

#include <qprinter.h>					//spectogram
#include <qprintdialog.h>				//spectogram
#include <qwt_color_map.h>				//spectogram
#include <qwt_plot_spectrogram.h>		//spectogram
#include <qwt_scale_widget.h>			//spectogram
#include <qwt_scale_draw.h>				//spectogram
#include <qwt_plot_zoomer.h>			//spectogram
#include <qwt_plot_panner.h>			//spectogram
#include <qwt_plot_layout.h>			//spectogram
#include <qwt_scale_engine.h> 			//2D Scaling objects

#include "DataModels/PlotData/MapColor.h"

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

class CQwtArrayPlotData;

template< class PARAMS >
struct CGeneric3DPlotInfo;
struct C3DPlotParameters;
using C3DPlotInfo = CGeneric3DPlotInfo< C3DPlotParameters >;



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
//						Custom Curve
//////////////////////////////////////////////////////////////////

class CGeneralizedCurve : public QObject, public QwtPlotCurve
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

	using base_t = QwtPlotCurve;


	//instance data

	double mMinXValue = 0.;
	double mMaxXValue = 0.;

	double mMinYValue = 0.;
	double mMaxYValue = 0.;

	bool mRangeComputed = false;

	const CQwtArrayPlotData *mData = nullptr;

	bool mAnimForward = true;
	bool mLoop = true;

	std::string mXtitle;
	std::string mYtitle;
	std::string mZtitle;


	//construction / destruction

	void CommonConstruct();
public:
	explicit CGeneralizedCurve( const CQwtArrayPlotData *data );
	explicit CGeneralizedCurve( const CQwtArrayPlotData *data, const QwtText &title );
	explicit CGeneralizedCurve( const CQwtArrayPlotData *data, const QString &title );

	
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

public:

	// QWT interface


	// This overload is needed when using auto-scale
	//
	virtual QwtDoubleRect boundingRect() const;


	// This is a slow implementation: it might be worth to cache valid data ranges.
	//
	virtual void draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to ) const;


	// internal processing

private:
	static bool isNaN( double x )
	{
		return x != x;
	}

	void ComputeRange();
};





//////////////////////////////////////////////////////////////////
//		2D Plot Widget
//////////////////////////////////////////////////////////////////

class C2DPlotWidget : public QwtPlot
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


	//static members

	static const std::string smFontName;
	static const int smAxisFontSize = 8;
	static const int smTitleFontSize = 10;

	//static CHistogram* CreateHistogram near respective instance methods


	//instance data
	
    bool mIsLog = false;

	C2DMagnifier *mMagnifier = nullptr;
	QwtPlotPanner *mPanner = nullptr;
	C2DZoomer *mZoomer = nullptr; 

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


	QSize mSizeHint = QSize( 72 * fontMetrics().width( 'x' ), 25 * fontMetrics().lineSpacing() );


	/////////////////////////////
	// construction / destruction
	/////////////////////////////

public:

    C2DPlotWidget( QWidget *parent = nullptr );

	virtual ~C2DPlotWidget();


	void Clear();


	///////////
	// general
	///////////

    void SetPlotTitle( const std::string &title );


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

	void AxisTitles( std::string *xtitle, std::string *ytitle, std::string *y2title );
	void HistogramAxisTitles( int index, std::string &xtitle, std::string &ytitle, std::string &y2title );
	void SpectrogramAxisTitles( int index, std::string &xtitle, std::string &ytitle, std::string &y2title );

	void SetAxisTitles( int index, const std::string &xtitle, const std::string &ytitle, const std::string &y2title = "" );


	//...ticks
    int XAxisNbTicks() const
    {
        return axisMaxMajor(QwtPlot::xBottom);
    }
    void SetXAxisNbTicks(int new_value)
    {
        setAxisMaxMajor( QwtPlot::xBottom, new_value );
        replot();
    }


	int YAxisNbTicks() const
    {
        return axisMaxMajor(QwtPlot::yLeft);
    }
    void SetYAxisNbTicks( int new_value )
    {
        setAxisMaxMajor( QwtPlot::yLeft, new_value );
        replot();
    }


	//...digits
	int GetXAxisMantissa() const;
	void SetXAxisMantissa( int new_mantissa );

	int GetYAxisMantissa() const;
	void SetYAxisMantissa( int new_mantissa );


    //...scaling

    void SetLogScale(int axisId, bool _isLog);

	void SetPlotAxisScales( int index, double xMin, double xMax, double yMin, double yMax, double y2Min = defaultValue<double>(), double y2Max = defaultValue<double>() );

protected:
	void CurrentHistogramRanges( double &xMin, double &xMax, double &yMin, double &yMax );
	void CurrentSpectrogramRanges( double &xMin, double &xMax, double &yMin, double &yMax, double &y2Min, double &y2Max );

	void AxisScales( double &xMin, double &xMax, double &yMin, double &yMax, double &y2Min, double &y2Max );
	void SetAxisScales( double xMin, double xMax, double yMin, double yMax, double y2Min = defaultValue<double>(), double y2Max = defaultValue<double>() );

public:

	void CurrentRanges( double &xMin, double &xMax, double &yMin, double &yMax, double &y2Min, double &y2Max );
	void RescaleX( double x );
	void RescaleY( double y );


	///////////
	// legends
	///////////

	QwtLegend* AddLegend( LegendPosition pos );


	///////////
	// raster
	///////////

	QwtPlotSpectrogram* PushRaster( const std::string &title, const C3DPlotInfo &maps, double min_contour, double max_contour, size_t ncontours, const QwtColorMap &color_map );

	//switch plot
	void SetCurrentRaster( int index );

    bool HasContour() const;
    void ShowContour( bool show );

    bool HasSolidColor() const;
    void ShowSolidColor( bool show );

    void ShowContour( int index, bool show );
    void ShowSolidColor( int index, bool show );

	void SetRasterColorMap( const QwtColorMap &color_map );


	////////////
	// histogram
	////////////

	int NumberOfBins( int index ) const;

protected:
	template< typename DATA >
	static CHistogram* CreateHistogram( const std::string &title, QColor color, const DATA *data, double &max_freq, int bins );

public:
	CHistogram* AddHistogram( const std::string &title, QColor color, const CQwtArrayPlotData *data, double &max_freq, int bins );

	CHistogram* PushHistogram( const std::string &title, QColor color, const C3DPlotInfo *data, double &max_freq, int bins );

	void SetCurrentHistogram( int index );


	//////////
	// curves
	//////////

	QwtPlotCurve* AddCurve( const std::string &title, QColor color, const CQwtArrayPlotData *data = nullptr );
	QwtPlotCurve* AddCurve( const std::string &title, QColor color, const CQwtArrayPlotData &data )
	{
		return AddCurve( title, color, &data );
	}
	QwtPlotCurve* AddCurve( const QwtData &data, const std::string &title, QColor color );	//for experimental samples


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

	EPointGlyph CurvePointGlyph( int curve ) const;
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

	C2DMagnifier* AddMagnifier();

	QwtPlotPanner* AddPanner();

	void Home();


protected:
	void SetAxisTitle( Axis axis, const std::string &title );

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
};



#endif			// GUI_DISPLAY_WIDGETS_2D_PLOT_WIDGET_H
