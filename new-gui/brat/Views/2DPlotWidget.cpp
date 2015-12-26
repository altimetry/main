#include "new-gui/brat/stdafx.h"


#include <qprinter.h>					//spectogram
#if QT_VERSION >= 0x040000
#include <qprintdialog.h>				//spectogram
#endif
#include <qwt_color_map.h>				//spectogram
#include <qwt_plot_spectrogram.h>		//spectogram
#include <qwt_scale_widget.h>			//spectogram
#include <qwt_scale_draw.h>				//spectogram
#include <qwt_plot_zoomer.h>			//spectogram
#include <qwt_plot_panner.h>			//spectogram
#include <qwt_plot_layout.h>			//spectogram



#include "2DPlotWidget.h"

//////////////////////////////////////////////////////////////////
//		Plot Data
//////////////////////////////////////////////////////////////////


class MyZoomer : public QwtPlotZoomer
{
public:
	MyZoomer( QwtPlotCanvas *canvas ) :
		QwtPlotZoomer( canvas )
	{
		setTrackerMode( AlwaysOn );
	}

	virtual QwtText trackerText( const QwtDoublePoint &pos ) const
	{
		QColor bg( Qt::white );
#if QT_VERSION >= 0x040300
		bg.setAlpha( 200 );
#endif

		QwtText text = QwtPlotZoomer::trackerText( pos );
		text.setBackgroundBrush( QBrush( bg ) );
		return text;
	}
};

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


//////////////////////////////////////////////////////////////////
//		The Class
//////////////////////////////////////////////////////////////////

C2DPlotWidget::C2DPlotWidget( E2DPlotType type, QWidget *parent ) : base_t( parent )
{
	switch ( type )
	{
		case eSpectogram:
			Spectogram( parent );
			break;

		default:
			throw "2D error";
	}
}

C2DPlotWidget::~C2DPlotWidget()
{}


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
	rightAxis->setColorMap( mSpectrogram->data().range(),
		mSpectrogram->colorMap() );

	setAxisScale( QwtPlot::yRight,
		mSpectrogram->data().range().minValue(),
		mSpectrogram->data().range().maxValue() );
	enableAxis( QwtPlot::yRight );

	plotLayout()->setAlignCanvasToScales( true );
	replot();

	// LeftButton for the zooming
	// MidButton for the panning
	// RightButton: zoom out by 1
	// Ctrl+RighButton: zoom out to full size

	QwtPlotZoomer* zoomer = new MyZoomer( canvas() );
#if QT_VERSION < 0x040000
	zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
		Qt::RightButton, Qt::ControlButton);
#else
	zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
		Qt::RightButton, Qt::ControlModifier );
#endif
	zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
		Qt::RightButton );

	QwtPlotPanner *panner = new QwtPlotPanner( canvas() );
	panner->setAxisEnabled( QwtPlot::yRight, false );
	panner->setMouseButton( Qt::MidButton );

	// Avoid jumping when labels with more/less digits
	// appear/disappear when scrolling vertically

	const QFontMetrics fm( axisWidget( QwtPlot::yLeft )->font() );
	QwtScaleDraw *sd = axisScaleDraw( QwtPlot::yLeft );
	sd->setMinimumExtent( fm.width( "100.00" ) );

	const QColor c( Qt::darkBlue );
	zoomer->setRubberBandPen( c );
	zoomer->setTrackerPen( c );


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

void C2DPlotWidget::printPlot()
{
	QPrinter printer;
	printer.setOrientation( QPrinter::Landscape );
#if QT_VERSION < 0x040000
	printer.setColorMode(QPrinter::Color);
#if 0
	printer.setOutputFileName("/tmp/spectrogram.ps");
#endif
	if (printer.setup())
#else
#if 0
	printer.setOutputFileName("/tmp/spectrogram.pdf");
#endif
	QPrintDialog dialog( &printer );
	if ( dialog.exec() )
#endif
	{
		print( printer );
	}
}




//C2DPlotWidget::C2DPlotWidget( QWidget *parent ) : base_t( parent )
//{
//
//	setWindowIcon( QPixmap( ":/2.png" ) );
//	setWindowTitle( "[*]" );
//	setAttribute( Qt::WA_DeleteOnClose );
//
//	setVisible( true );
//}


QSize C2DPlotWidget::sizeHint() const
{
	return m_SizeHint;
	//return QSize(72 * fontMetrics().width('x'),
	//             25 * fontMetrics().lineSpacing());
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_2DPlotWidget.cpp"
