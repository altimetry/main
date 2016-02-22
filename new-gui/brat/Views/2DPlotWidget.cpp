#include "new-gui/brat/stdafx.h"


//#include <qprinter.h>					//spectogram
//#include <qprintdialog.h>				//spectogram
//#include <qwt_color_map.h>				//spectogram
//#include <qwt_plot_spectrogram.h>		//spectogram
//#include <qwt_scale_widget.h>			//spectogram
//#include <qwt_scale_draw.h>				//spectogram
#include <qwt_plot_zoomer.h>			
#include <qwt_plot_panner.h>
//#include <qwt_plot_layout.h>			//spectogram

#include <qwt_plot_magnifier.h>


//#include <qwt_data.h>           //simple
#include <qwt_legend.h>


#include "new-gui/Common/QtUtils.h"
#include "new-gui/brat/PlotValues.h"

#include "2DPlotWidget.h"


// TODO delete this macro; if active, no operation is expected to work besides the simple example display
// #define TEST_2D_EXAMPLES


//////////////////////////////////////////////////////////////////
//						Custom Zoomer
//////////////////////////////////////////////////////////////////

class CZoomer : public QwtPlotZoomer
{
public:
	CZoomer( QwtPlotCanvas *canvas ) :
		QwtPlotZoomer( canvas )
	{
		setTrackerMode( AlwaysOn );
	}

	virtual QwtText trackerText( const QwtDoublePoint &pos ) const
	{
		QColor bg( Qt::white );
		bg.setAlpha( 200 );
		QwtText text = QwtPlotZoomer::trackerText( pos );
		text.setBackgroundBrush( QBrush( bg ) );
		return text;
	}
};


//////////////////////////////////////////////////////////////////
//						Custom Curve
//////////////////////////////////////////////////////////////////

void CGeneralizedCurve::CommonConstruct()
{
	if ( mData )
		setData( *mData );

	connect( &mTimer, SIGNAL( timeout() ), this, SLOT( ChangeFrame() ) );
}


CGeneralizedCurve::CGeneralizedCurve( const CQwtArrayPlotData *data )
	: QwtPlotCurve()
	, mData( data )
{
	CommonConstruct();
}
CGeneralizedCurve::CGeneralizedCurve( const CQwtArrayPlotData *data, const QwtText &title )
	: QwtPlotCurve( title )
	, mData( data )
{
	CommonConstruct();
}
CGeneralizedCurve::CGeneralizedCurve( const CQwtArrayPlotData *data, const QString &title )
	: QwtPlotCurve( title )
	, mData( data )
{
	CommonConstruct();
}


// access

void CGeneralizedCurve::SetRanges( double xMin, double xMax, double yMin, double yMax )
{
	mMinXValue = xMin;
	mMaxXValue = xMax;

	mMinYValue = yMin;
	mMaxYValue = yMax;

	mRangeComputed = true;
}


void CGeneralizedCurve::Animate( int updateinterval )		//updateinterval = 500 
{
	if ( updateinterval == 0 )
	{
		mTimer.stop();
	}
	else
	{
		mTimer.start( updateinterval );
	}
}

void CGeneralizedCurve::ChangeFrame()
{
	assert__( mData );

	mData->SetNextFrame();
	mRangeComputed = false;		//TODO really needed? 
	setData( *mData );
	emit FrameChanged();
}



// QWT interface


// This overload is needed when using auto-scale
//
//virtual 
QwtDoubleRect CGeneralizedCurve::boundingRect() const
{
	if ( dataSize() <= 0 )
		return QwtDoubleRect( 1.0, 1.0, -2.0, -2.0 ); // Empty data.

	int first = 0;
	while ( first < dataSize() && ( isNaN( x( first ) ) || isNaN( y( first ) ) ) )
		++first;

	if ( first == dataSize() )
		return QwtDoubleRect( 1.0, 1.0, -2.0, -2.0 ); // Empty data.

	const_cast<CGeneralizedCurve*>( this )->ComputeRange();

	return QwtDoubleRect( mMinXValue, mMinYValue, mMaxXValue - mMinXValue, mMaxYValue - mMinYValue );
}


// This is a slow implementation: it might be worth to cache valid data ranges.
//
//virtual 
void CGeneralizedCurve::draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to ) const
{
	if ( to < 0 )
		to = dataSize() - 1;

	int first, last = from;
	while ( last <= to )
	{
		first = last;
		while ( first <= to && ( isNaN( x( first ) ) || isNaN( y( first ) ) ) )
			++first;
		last = first;
		while ( last <= to && !isNaN( x( last ) ) && !isNaN( y( last ) ) )
			++last;
		if ( first <= to )
			QwtPlotCurve::draw( painter, xMap, yMap, first, last - 1 );
	}
}


// internal processing

void CGeneralizedCurve::ComputeRange()
{
	if ( mRangeComputed )
		return;

	int first = 0;
	double minX, maxX, minY, maxY;
	minX = maxX = x( first );
	minY = maxY = y( first );
	for ( int i = first + 1; i < dataSize(); ++i )
	{
		const double xv = x( i );
		if ( xv < minX )
			minX = xv;
		if ( xv > maxX )
			maxX = xv;
		const double yv = y( i );
		if ( yv < minY )
			minY = yv;
		if ( yv > maxY )
			maxY = yv;
	}

	mMinXValue = minX;
	mMaxXValue = maxX;

	mMinYValue = minY;
	mMaxYValue = maxY;

	mRangeComputed = true;
}



//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//						The Class
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

C2DPlotWidget::C2DPlotWidget( QWidget *parent ) 
	: base_t( parent )
{
	setWindowIcon( QPixmap( ":/2.png" ) );
	setWindowTitle( "[*]" );
	setAttribute( Qt::WA_DeleteOnClose );

	setCanvasBackground( Qt::white );

    setMinimumSize( min_plot_widget_width, min_plot_widget_height );

#if defined (TEST_2D_EXAMPLES)

	E2DPlotType type = eSpectogram;

	switch ( type )
	{
		case eSpectogram:

            Spectogram( parent );
			break;

		default:
			throw "2D error";
	}

    setMinimumSize( min_plot_widget_width, min_plot_widget_height );
#endif

}


C2DPlotWidget::~C2DPlotWidget()
{}


void C2DPlotWidget::Clear()
{
	//clear();

 //   detachItems(QwtPlotItem::Rtti_PlotCurve);
 //   detachItems(QwtPlotItem::Rtti_PlotMarker);

    detachItems();
    // deleteAxesData();		private

	while ( mCurves.begin() != mCurves.end() )
		mCurves.erase( mCurves.begin() );

	//DestroyPointersAndContainer( mCurves );

	replot();
}


void C2DPlotWidget::SetAxisTitle( Axis axis, const std::string &title )
{
    setAxisTitle( axis, title.c_str() );
}


void C2DPlotWidget::SetAxisScales( double xMin, double xMax, double yMin, double yMax )
{
	for ( auto *c : mCurves )
		c->SetRanges( xMin, xMax, yMin, yMax );

	setAxisScale( xBottom, xMin, xMax  );
	setAxisScale( yLeft, yMin, yMax  );
}



QwtPlotCurve* C2DPlotWidget::AddCurve( const std::string &title, QColor color, const CQwtArrayPlotData *data )	//data = nullptr 
{
    CGeneralizedCurve *c = new CGeneralizedCurve( data, title.c_str() );
	mCurves.push_back( c );
	connect( c, SIGNAL( FrameChanged() ), this, SLOT( HandleFrameChanged() ) );
    c->setRenderHint( QwtPlotItem::RenderAntialiased );
    c->setPen( QPen( color ) );
    c->attach( this );

	// TODO this needs a lot more work, including GUI to control animation and animation settings, and above all check if 
	//	axis and everything else besides the curve needs change; but for now, good for testing values 
	//
	if ( data->GetNumberOfFrames() > 1 )
		c->Animate();
	//
	return c;
}
QwtPlotCurve* C2DPlotWidget::AddCurve( const QwtData &data, const std::string &title, QColor color )	//for experimental samples
{
    QwtPlotCurve *c = new QwtPlotCurve( title.c_str() );
    c->setRenderHint(QwtPlotItem::RenderAntialiased);
    c->setPen( QPen( color ) );
    c->attach( this );
    c->setData( data );
	return c;
}


void C2DPlotWidget::HandleFrameChanged()
{
	replot();
}



void C2DPlotWidget::SetCurvesStyle( QwtPlotCurve::CurveStyle style )
{
	for ( auto *c : mCurves )
		c->setStyle( style );

	replot();
}



QwtPlotMarker* C2DPlotWidget::AddMarker( const std::string &label, Qt::Alignment label_alignment, Qt::Orientation label_orientation, 
	QwtPlotMarker::LineStyle st, double value )
{
    QwtPlotMarker *m = new QwtPlotMarker();
	m->setLabel( QString::fromLatin1( label.c_str() ) );
    m->setLabelAlignment( label_alignment );
    m->setLabelOrientation( label_orientation );
    m->setLineStyle( st );
	if ( st == QwtPlotMarker::VLine )
	{
		m->setXValue( value );
	}
	else {
		m->setYValue( value );
	}
    m->attach(this);

	return m;
}
QwtPlotMarker* C2DPlotWidget::AddMarker( const std::string &label, QwtPlotMarker::LineStyle st, double value )
{
	if ( st == QwtPlotMarker::HLine )
		return AddMarker( label, Qt::AlignRight | Qt::AlignTop, Qt::Horizontal, st, value );
	else
		return AddMarker( label, Qt::AlignLeft | Qt::AlignBottom, Qt::Vertical, st, value );

	return nullptr;
}

QwtPlotMagnifier* C2DPlotWidget::AddMagnifier()
{
	return new QwtPlotMagnifier( canvas() );
}
QwtPlotZoomer* C2DPlotWidget::AddZoomer( QColor color )
{
	// LeftButton for the zooming
	// RightButton: zoom out by 1
	// Ctrl+RighButton: zoom out to full size

	QwtPlotZoomer* zoomer = new CZoomer( canvas() );
	zoomer->setMousePattern( QwtEventPattern::MouseSelect2,	Qt::RightButton, Qt::ControlModifier );
	zoomer->setMousePattern( QwtEventPattern::MouseSelect3,	Qt::RightButton );

	const QColor c( color );
	zoomer->setRubberBandPen( c );
	zoomer->setTrackerPen( c );

	return zoomer;
}

QwtPlotPanner* C2DPlotWidget::AddPanner()
{
	// MidButton for the panning
	QwtPlotPanner *panner = new QwtPlotPanner( canvas() );
	panner->setAxisEnabled( QwtPlot::yRight, false );
	panner->setMouseButton( Qt::MidButton );

	return panner;
}

QwtLegend* C2DPlotWidget::AddLegend( LegendPosition pos )
{
	QwtLegend *legend = new QwtLegend;
    insertLegend( legend, pos );
	return legend;
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


QSize C2DPlotWidget::sizeHint() const
{
	return m_SizeHint;
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_2DPlotWidget.cpp"
