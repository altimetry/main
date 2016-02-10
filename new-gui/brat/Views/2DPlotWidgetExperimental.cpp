#include "new-gui/brat/stdafx.h"


#include <qprinter.h>					//spectogram
#include <qprintdialog.h>				//spectogram
#include <qwt_color_map.h>				//spectogram
#include <qwt_plot_spectrogram.h>		//spectogram
#include <qwt_scale_widget.h>			//spectogram
#include <qwt_scale_draw.h>				//spectogram
#include <qwt_plot_zoomer.h>			//spectogram
#include <qwt_plot_panner.h>			//spectogram
#include <qwt_plot_layout.h>			//spectogram


#include <qwt_data.h>           //simple
#include <qwt_legend.h>         //simple
#include <qwt_plot_curve.h>     //simple
#include <qwt_plot_marker.h>    //simple


#include "new-gui/Common/QtUtils.h"

#include "2DPlotWidget.h"



//////////////////////////////////////////////////////////////////
//				SimplePlot sample Data
//////////////////////////////////////////////////////////////////


class SimpleData: public QwtData
{
    // The x values depend on its index and the y values
    // can be calculated from the corresponding x value.
    // So we don't need to store the values.
    // Such an implementation is slower because every point
    // has to be recalculated for every replot, but it demonstrates how
    // QwtData can be used.

public:
    SimpleData(double(*y)(double), size_t size):
        d_size(size),
        d_y(y)
    {
    }

    virtual QwtData *copy() const
    {
        return new SimpleData(d_y, d_size);
    }

    virtual size_t size() const
    {
        return d_size;
    }

    virtual double x(size_t i) const
    {
        return 0.1 * i;
    }

    virtual double y(size_t i) const
    {
        return d_y(x(i));
    }
private:
    size_t d_size;
    double(*d_y)(double);
};




//////////////////////////////////////////////////////////////////
//				Spectrogram sample Data
//////////////////////////////////////////////////////////////////

static_assert( QT_VERSION >= 0x040300, "" );

class SpectrogramData : public QwtRasterData
{
public:
	SpectrogramData() :
		QwtRasterData( QwtDoubleRect( -1.5, -1.5, 3.0, 3.0 ) )
	{
	}

	virtual QwtRasterData *copy() const
	{
		return new SpectrogramData();
	}

	virtual QwtDoubleInterval range() const
	{
		return QwtDoubleInterval( 0.0, 10.0 );
	}

	virtual double value( double x, double y ) const
	{
		const double c = 0.842;

		const double v1 = x * x + ( y - c ) * ( y + c );
		const double v2 = x * ( y + c ) + x * ( y + c );

		return 1.0 / ( v1 * v1 + v2 * v2 );
	}
};


void C2DPlotWidget::SimplePlot()
{
    setTitle("A Simple QwtPlot Demonstration");
	QwtLegend *legend = AddLegend( RightLegend );

    // Set axis titles
    SetAxisTitles( "x -->", "y -->" );

    // Insert new curves
    const int nPoints = 100;
	QwtPlotCurve *cSin = AddCurve( "y = sin(x)", Qt::red, SimpleData( ::sin, nPoints ) );
	QwtPlotCurve *cCos = AddCurve( "y = cos(x)", Qt::blue, SimpleData( ::cos, nPoints ) );

    // Markers
    //  ...a horizontal line at y = 0...
    //  ...a vertical line at x = 2 * pi
	//
    QwtPlotMarker *mY = AddMarker( "y = 0", QwtPlotMarker::HLine, 0.0 );
    QwtPlotMarker *mX = AddMarker( "x = 2 pi", QwtPlotMarker::VLine, 2.0 * M_PI );
    mX->setLinePen( QPen( Qt::black, 0, Qt::DashDotLine ) );

	QwtPlotZoomer *zoomer = AddZoomer( Qt::darkBlue );
	QwtPlotPanner *panner = AddPanner();

	Q_UNUSED( cSin );		Q_UNUSED( cCos );		Q_UNUSED( mY );		Q_UNUSED( zoomer );		Q_UNUSED( panner );		Q_UNUSED( legend );
}

void C2DPlotWidget::Spectogram( QWidget *parent )
{
	mSpectrogram = new QwtPlotSpectrogram();

	QwtLinearColorMap colorMap( Qt::darkCyan, Qt::red );
	colorMap.addColorStop( 0.1, Qt::cyan );
	colorMap.addColorStop( 0.6, Qt::green );
	colorMap.addColorStop( 0.95, Qt::yellow );

	mSpectrogram->setColorMap( colorMap );

	mSpectrogram->setData( SpectrogramData() );
	mSpectrogram->attach( this );

	QwtValueList contourLevels;
	for ( double level = 0.5; level < 10.0; level += 1.0 )
		contourLevels += level;
	mSpectrogram->setContourLevels( contourLevels );

	// A color bar on the right axis
	QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
	rightAxis->setTitle( "Intensity" );
	rightAxis->setColorBarEnabled( true );
	rightAxis->setColorMap( mSpectrogram->data().range(), mSpectrogram->colorMap() );

	setAxisScale( QwtPlot::yRight,
		mSpectrogram->data().range().minValue(),
		mSpectrogram->data().range().maxValue() );
	enableAxis( QwtPlot::yRight );

	plotLayout()->setAlignCanvasToScales( true );
	replot();

	AddZoomer( Qt::darkBlue );
	AddPanner();

	// Avoid jumping when labels with more/less digits
	// appear/disappear when scrolling vertically

	const QFontMetrics fm( axisWidget( QwtPlot::yLeft )->font() );
	QwtScaleDraw *sd = axisScaleDraw( QwtPlot::yLeft );
	sd->setMinimumExtent( fm.width( "100.00" ) );

	// parent

	QMainWindow *m = dynamic_cast<QMainWindow *>( parent );

	QToolBar *toolBar = new QToolBar( m );

	QToolButton *btnSpectrogram = new QToolButton( toolBar );
	QToolButton *btnContour = new QToolButton( toolBar );
	QToolButton *btnPrint = new QToolButton( toolBar );

	btnSpectrogram->setText( "Spectrogram" );
	//btnSpectrogram->setIcon(QIcon());
	btnSpectrogram->setCheckable( true );
	btnSpectrogram->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	toolBar->addWidget( btnSpectrogram );

	btnContour->setText( "Contour" );
	//btnContour->setIcon(QIcon());
	btnContour->setCheckable( true );
	btnContour->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	toolBar->addWidget( btnContour );

	btnPrint->setText( "Print" );
	btnPrint->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	toolBar->addWidget( btnPrint );

	m->addToolBar( toolBar );

	connect( btnSpectrogram, SIGNAL( toggled( bool ) ), this, SLOT( showSpectrogram( bool ) ) );
	connect( btnContour, SIGNAL( toggled( bool ) ), this, SLOT( showContour( bool ) ) );
	connect( btnPrint, SIGNAL( clicked() ), this, SLOT( printPlot() ) );


	btnSpectrogram->setChecked( true );
	btnContour->setChecked( false );
}


void C2DPlotWidget::showContour( bool on )
{
	mSpectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode, on );
	replot();
}

void C2DPlotWidget::showSpectrogram( bool on )
{
	mSpectrogram->setDisplayMode( QwtPlotSpectrogram::ImageMode, on );
	mSpectrogram->setDefaultContourPen( on ? QPen() : QPen( Qt::NoPen ) );
	replot();
}
