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
//		The Class
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





QwtPlotCurve* C2DPlotWidget::AddCurve( const std::string &title, QColor color, const QwtData *data )	//data = nullptr 
{
    QwtPlotCurve *c = new QwtPlotCurve( title.c_str() );
    c->setRenderHint(QwtPlotItem::RenderAntialiased);
    c->setPen( QPen( color ) );
    c->attach( this );
	if ( data )
	    c->setData( *data );
	return c;
}

void C2DPlotWidget::SetAxisTitle( Axis axis, const std::string &title )
{
    setAxisTitle( axis, title.c_str() );
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
