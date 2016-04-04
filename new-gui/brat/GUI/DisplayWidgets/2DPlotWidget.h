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


#include "DataModels/PlotData/MapColor.h"

class QwtPlotCurve;
class QwtData;
class QwtPlotZoomer;
class QwtPlotMagnifier;
class QwtPlotPanner;
class CHistogram;


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
    assert__( s >= (int)eFirstPointGlyph && s < (int)EPointGlyph_size );

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




///////////////////////////////////////////////////////////////////////////

class SpectrogramJOFFData : public QwtRasterData
{
private:
    std::vector<double> x_vect;
    std::vector<double> y_vect;
    std::vector<bool> b_vect;
    std::vector<double> z_vect;
public:
    SpectrogramJOFFData() :
        QwtRasterData( QwtDoubleRect( 0, 0, 571, 128 ) )
    {
        //s=0;
    }

    void InjectData(std::vector<double> & _x,std::vector<double> & _y,std::vector<bool> & _b, std::vector<double> & _z)
    {
        x_vect =_x;
        y_vect =_y;
        b_vect =_b;
        z_vect =_z;
    }

    virtual QwtRasterData *copy() const
    {
        SpectrogramJOFFData* cp_sp = new SpectrogramJOFFData();
        std::vector<double> x_cp = x_vect;
        std::vector<double> y_cp = y_vect;
        std::vector<bool> b_cp = b_vect;
        std::vector<double> z_cp = z_vect;

        cp_sp->InjectData(x_cp, y_cp, b_cp, z_cp);
        return cp_sp;
        //return new SpectrogramJOFFData();
    }

    virtual QwtDoubleInterval range() const
    {
        return QwtDoubleInterval( 0.0, 100000.0 );
    }

    virtual double value( double x, double y ) const
    {        

        if ((x_vect.size()*(int)y + (int)x)>=z_vect.size())
        {
            return 0.0;
        }
        return z_vect[x_vect.size()*(int)y + (int)x];

//        return z_vect[s%z_vect.size()];
        //return 1.0 / (7*x*x + y*y);
    }
};

///////////////////////////////////////////////////////////////////////////





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

	QTimer mTimer;


	//construction / destruction

	void CommonConstruct();
public:
	explicit CGeneralizedCurve( const CQwtArrayPlotData *data );
	explicit CGeneralizedCurve( const CQwtArrayPlotData *data, const QwtText &title );
	explicit CGeneralizedCurve( const CQwtArrayPlotData *data, const QString &title );

	
	// access / assignment

	void SetRanges( double xMin, double xMax, double yMin, double yMax );

	void Animate( int updateinterval = 500 );


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

signals:
	void FrameChanged();

protected slots:

	void ChangeFrame();
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


	//static data

	static const std::string smFontName;
	static const int smAxisFontSize = 8;
	static const int smTitleFontSize = 10;


	//instance data

    std::vector< QwtPlotSpectrogram* > mSpectrograms;
	QwtPlotSpectrogram *mCurrentSpectrogram = nullptr;
	std::vector< CHistogram* > mHistograms;
	std::vector< CGeneralizedCurve* > mCurves;


	QSize mSizeHint = QSize( 72 * fontMetrics().width( 'x' ), 25 * fontMetrics().lineSpacing() );


	// construction / destruction

public:

    C2DPlotWidget( QWidget *parent = nullptr );

	virtual ~C2DPlotWidget();


	void Clear();


	///////////
	// general
	///////////

    void SetPlotTitle( const std::string &title );


	///////////
	// axis
	///////////

	void SetAxisTitles( const std::string &xtitle, const std::string &ytitle, const std::string &y2title = "" )
	{
		SetAxisTitle( xBottom, xtitle );
		SetAxisTitle( yLeft, ytitle );
		if ( !y2title.empty() )
			SetAxisTitle( yRight, y2title );
	}


	void SetAxisScales( double xMin, double xMax, double yMin, double yMax, double y2Min = defaultValue<double>(), double y2Max = defaultValue<double>() );


	void EnableAxisY2( bool enable = true ) 
	{ 
		enableAxis( QwtPlot::yRight, enable );
	}



	///////////
	// legends
	///////////

	QwtLegend* AddLegend( LegendPosition pos );


	///////////
	// raster
	///////////

	QwtPlotSpectrogram* AddRaster( const std::string &title, const C3DPlotInfo &maps, double min_contour, double max_contour, size_t ncontours, size_t index );

	//switch plot

	void SetCurrentRaster( int index );

    bool HasContour() const;
    void ShowContour( bool show );

    bool HasSolidColor() const;
    void ShowSolidColor( bool show );

    void ShowContour( int index, bool show );
    void ShowSolidColor( int index, bool show );


	////////////
	// histogram
	////////////


	CHistogram* AddHistogram( const std::string &title, QColor color, const CQwtArrayPlotData *data );


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

	QwtPlotZoomer* AddZoomer( QColor color );

	QwtPlotMagnifier* AddMagnifier();


	QwtPlotPanner* AddPanner();


protected:
	void SetAxisTitle( Axis axis, const std::string &title );

	void SetCurvesStyle( QwtPlotCurve::CurveStyle style );

	void Spectogram( QWidget *parent = nullptr );
	void SimplePlot();

    virtual void replot() override;

public:
    QSize sizeHint() const override;

public slots:
    void printPlot();

	void HandleFrameChanged();
};



#endif			// GUI_DISPLAY_WIDGETS_2D_PLOT_WIDGET_H
