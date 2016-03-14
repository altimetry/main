#ifndef GUI_DISPLAY_WIDGETS_2D_PLOT_WIDGET_H
#define GUI_DISPLAY_WIDGETS_2D_PLOT_WIDGET_H

#include <QMainWindow>

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>


class QwtPlotCurve;
class QwtData;
class QwtPlotZoomer;
class QwtPlotMagnifier;
class QwtPlotPanner;

class CQwtArrayPlotData;


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


	//instance data

    QwtPlotSpectrogram *mSpectrogram = nullptr;

	std::vector< CGeneralizedCurve* > mCurves;


	QSize m_SizeHint = QSize( 72 * fontMetrics().width( 'x' ), 25 * fontMetrics().lineSpacing() );


	// construction / destruction

public:
    C2DPlotWidget( QWidget *parent = nullptr );

	virtual ~C2DPlotWidget();


	void Clear();


	// plot

	void SetPlotTitle( const std::string &title )
	{
		setTitle( title.c_str() );
	}


	// axis

	void SetAxisTitles( const std::string &xtitle, const std::string &ytitle )
	{
		SetAxisTitle( xBottom, xtitle );
		SetAxisTitle( yLeft, ytitle );
	}


	void SetAxisScales( double xMin, double xMax, double yMin, double yMax );


	// legends

	QwtLegend* AddLegend( LegendPosition pos );


	// curves

	QwtPlotCurve* AddCurve( const std::string &title, QColor color, const CQwtArrayPlotData *data = nullptr );
	QwtPlotCurve* AddCurve( const std::string &title, QColor color, const CQwtArrayPlotData &data )
	{
		return AddCurve( title, color, &data );
	}
	QwtPlotCurve* AddCurve( const QwtData &data, const std::string &title, QColor color );	//for experimental samples


	void SetPointCurves()
	{
		SetCurvesStyle( QwtPlotCurve::Dots );
	}
	void SetLineCurves()
	{
		SetCurvesStyle( QwtPlotCurve::Lines );
	}


        void SetTargetCurveStyleLine(int index)
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];

            pcurve->setStyle(QwtPlotCurve::Lines);
        }

        void SetTargetCurveStyleDots(int index)
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];

            pcurve->setStyle(QwtPlotCurve::Dots);
        }

        bool IsTargetCurveStyleLine(int index) const
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];
            return pcurve->style() == QwtPlotCurve::Lines;
        }

        bool IsTargetCurveStyleDots(int index) const
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];
            return pcurve->style() == QwtPlotCurve::Dots;
        }



        QColor GetTargetCurvePenColor(int index) const
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];


          //  return pcurve->pen().brush().color();
            return pcurve->pen().color();
        }

        void SetTargetCurvePenColor(int index,QColor new_color) const
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];
            QPen new_pen (new_color);

            pcurve->setPen(new_pen);
        }


        int GetTargetCurvePenAlpha(int index) const
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];


          //  return pcurve->pen().brush().color();
            return pcurve->pen().color().alpha();
        }

        void SetTargetCurvePenAlpha(int index,int new_alpha) const
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];
            QPen new_pen = pcurve->pen();
            QColor old_color = new_pen.color();
            old_color.setAlpha(new_alpha);
            new_pen.setColor(old_color);


            pcurve->setPen(new_pen);
        }

        int GetTargetCurvePenWidth(int index) const
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];

            //  return pcurve->pen().brush().color();
            return pcurve->pen().width();
        }


        void SetTargetCurvePenWidth(int index, int pixels) const
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];

            QPen old_pen = pcurve->pen();

            //  return pcurve->pen().brush().color();
            old_pen.setWidth(pixels);

            pcurve->setPen(old_pen);
        }



        int GetTargetCurveLinePattern(int index) const
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];

            QPen curr_pen = pcurve->pen();

            return curr_pen.style();

          //  QwtSymbol c_symbol = pcurve->symbol();

          //  return c_symbol.style();
        }

        void SetTargetCurveLinePattern(int index, int pat)
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];
            QPen old_pen = pcurve->pen();

            switch (pat)
            {
            case 0:
                old_pen.setStyle(Qt::NoPen);
                break;
            case 1:
                old_pen.setStyle(Qt::SolidLine);
                break;
            case 2:
                old_pen.setStyle(Qt::DashLine);
                break;
            case 3:
                old_pen.setStyle(Qt::DotLine);
                break;
            case 4:
                old_pen.setStyle(Qt::DashDotLine);
                break;
            case 5:
                old_pen.setStyle(Qt::DashDotDotLine);
                break;
            default:
               // old_pen.setStyle(Qt::SolidLine);
                break;
            }
            pcurve->setPen(old_pen);
        }


        int GetTargetCurvePointSymbol(int index) const
        {

            CGeneralizedCurve * pcurve = this->mCurves[index];

            QwtSymbol c_symbol = pcurve->symbol();


            return (c_symbol.style()+1);
        }

        void SetTargetCurvePointSymbol(int index, int val_symbol)
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];

            QwtSymbol c_symbol = pcurve->symbol();

            c_symbol.setStyle((QwtSymbol::Style)(val_symbol-1));

            pcurve->setSymbol(c_symbol);
        }

        void SetTargetCurveGlyphColor(int index, QColor new_color)
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];
            QwtSymbol c_symbol = pcurve->symbol();
            QPen old_pen = c_symbol.pen();
            old_pen.setColor(new_color);
            c_symbol.setPen(old_pen);
            pcurve->setSymbol(c_symbol);
            pcurve->setPen(Qt::NoPen);
        }

        QColor GetTargetCurveGlyphColor(int index ) const
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];
            QwtSymbol c_symbol = pcurve->symbol();
            QPen curr_pen = c_symbol.pen();
            return curr_pen.color();
        }

        void SetInteriorBrush(bool flag, int index)
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];
            QwtSymbol c_symbol = pcurve->symbol();
            QBrush curr_brush = c_symbol.brush();

            if (!flag)
            {
                curr_brush = Qt::NoBrush;
            }
            else
            {
                curr_brush.setColor(pcurve->symbol().pen().color());
            }
            c_symbol.setBrush(curr_brush);
            pcurve->setSymbol(c_symbol);
        }

        int GetTargetCurvePointSize(int index) const
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];

            const QSize glyph_sz = pcurve->symbol().size();
            //  return pcurve->pen().brush().color();
            return glyph_sz.height();
        }


        void SetTargetCurvePointSize(int index, int pixels)
        {
            CGeneralizedCurve * pcurve = this->mCurves[index];
            QSize new_size(pixels, pixels);
            QwtSymbol c_symbol = pcurve->symbol();

            c_symbol.setSize(new_size);

            pcurve->setSymbol(c_symbol);
        }

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

public:
    QSize sizeHint() const override;

public slots:
    void showContour(bool on);
    void showSpectrogram(bool on);
    void printPlot();

	void HandleFrameChanged();
};



#endif			// GUI_DISPLAY_WIDGETS_2D_PLOT_WIDGET_H
