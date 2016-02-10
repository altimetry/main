#ifndef GRAPHICS_2D_PLOT_WIDGET_H
#define GRAPHICS_2D_PLOT_WIDGET_H

#include <QMainWindow>

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot_marker.h>


class QwtPlotCurve;
class QwtData;
class QwtPlotZoomer;
class QwtPlotPanner;



//////////////////////////////////////////////////////////////////
//		2D Plot Types
//////////////////////////////////////////////////////////////////

enum E2DPlotType
{
	eSpectogram, ePlot2dType_first = eSpectogram, ePlotDefault = eSpectogram,

	e2DPlotType_size
};

inline E2DPlotType& operator ++( E2DPlotType &ex )
{
	int exi = static_cast<int>(ex)+ 1;

	if ( exi >= e2DPlotType_size )
		ex = ePlot2dType_first;
	else
		ex = static_cast<E2DPlotType>( exi );

	return ex;// static_cast<E2DPlotType>( static_cast<int>(ex)+ 1 );
}




//////////////////////////////////////////////////////////////////
//		2D Plot Widget
//////////////////////////////////////////////////////////////////

class C2DPlotWidget : public QwtPlot
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	using base_t = QwtPlot;


    QwtPlotSpectrogram *mSpectrogram = nullptr;

	QSize m_SizeHint = QSize( 72 * fontMetrics().width( 'x' ), 25 * fontMetrics().lineSpacing() );

public:
    C2DPlotWidget( QWidget *parent = nullptr );

	virtual ~C2DPlotWidget();

	//

	void SetPlotTitle( const std::string &title )
	{
		setTitle( title.c_str() );
	}

	void SetAxisTitles( const std::string &xtitle, const std::string &ytitle )
	{
		SetAxisTitle( xBottom, xtitle );
		SetAxisTitle( yLeft, ytitle );
	}

	//

	QwtLegend* AddLegend( LegendPosition pos );


	QwtPlotCurve* AddCurve( const std::string &title, QColor color, const QwtData *data = nullptr );
	QwtPlotCurve* AddCurve( const std::string &title, QColor color, const QwtData &data )
	{
		return AddCurve( title, color, &data );
	}


	QwtPlotMarker* AddMarker( const std::string &label, Qt::Alignment label_alignment, Qt::Orientation label_orientation, 
		QwtPlotMarker::LineStyle st, double value );

	QwtPlotMarker* AddMarker( const std::string &label, QwtPlotMarker::LineStyle st, double value );


	QwtPlotZoomer* AddZoomer( QColor color );


	QwtPlotPanner* AddPanner();

protected:
	void SetAxisTitle( Axis axis, const std::string &title );

	void Spectogram( QWidget *parent = nullptr );
	void SimplePlot();

public:
    QSize sizeHint() const override;

public slots:
    void showContour(bool on);
    void showSpectrogram(bool on);
    void printPlot();
};



#endif			// GRAPHICS_2D_PLOT_WIDGET_H
