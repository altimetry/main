#include "new-gui/brat/stdafx.h"

#include <qwt_plot_grid.h>				//histogram
#include <qwt_interval_data.h>			//histogram
#include <qwt_plot_zoomer.h>			
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_legend.h>

#include "new-gui/Common/QtUtils.h"
#include "PlotValues.h"

#include "Histogram.h"
#include "2DPlotWidget.h"


CBratScaleDraw::CBratScaleDraw()
{
    mantissa_digits = 2 % MAX_MANTISSA;
}


CBratScaleDraw::CBratScaleDraw(int _mantissa)
{
    mantissa_digits = _mantissa % MAX_MANTISSA;
}

QwtText CBratScaleDraw::label(double value) const
{
    //return QLocale::system().toString(value);
    return QString::number(value, 'f', mantissa_digits);
}

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

void CGeneralizedCurve::Ranges( double &xMin, double &xMax, double &yMin, double &yMax )
{
	xMin = mMinXValue;
	xMax = mMaxXValue;

	yMin = mMinYValue;
	yMax = mMaxYValue;
}

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
//						Main Class
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////



//static 
const std::string C2DPlotWidget::smFontName = "Arial";



C2DPlotWidget::C2DPlotWidget( QWidget *parent ) 
	: base_t( parent )
{
	setWindowIcon( QPixmap( ":/2.png" ) );
	setWindowTitle( "[*]" );
	setAttribute( Qt::WA_DeleteOnClose );

	setCanvasBackground( Qt::white );

///////////////////////////////////////////////////////////////////////////////////////////////////////
    mXScaler = new CBratScaleDraw;
    mYScaler = new CBratScaleDraw;
    //xAXiS

//    logScaler = new QwtLog10ScaleEngine();
//    linScaler = new QwtLinearScaleEngine();

    mIsLog = false;

    setAxisScaleDraw(QwtPlot::xBottom, mXScaler);
    setAxisScaleDraw(QwtPlot::yLeft, mYScaler);
///////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined (TEST_2D_EXAMPLES)

//	Spectogram( parent );

#endif

    setMinimumSize( min_plot_widget_width, min_plot_widget_height );
}


C2DPlotWidget::~C2DPlotWidget()
{
	DestroyPointersAndContainer( mCurves );
	DestroyPointersAndContainer( mSpectrograms );
}


void C2DPlotWidget::Clear()		// TODO analyze; not usable so far
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



void C2DPlotWidget::HandleFrameChanged()
{
	replot();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void C2DPlotWidget::SetLogScale(int axisId, bool _isLog)
{
    // setAxisScaleEngine deletes the previously set pointer for a
    // QwtScaleEngine object

    mIsLog=_isLog;
    if (mIsLog)
    {
        setAxisScaleEngine(axisId, new QwtLog10ScaleEngine());
    }
    else
    {
        setAxisScaleEngine(axisId, new QwtLinearScaleEngine());
        //setAxisAutoScale(axisId);
    }

    replot();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////
//									Title
/////////////////////////////////////////////////////////////////////////////////////////



void C2DPlotWidget::SetPlotTitle( const std::string &title )
{
	QwtText text( t2q( title ) );
	QFont font( t2q( smFontName ), smTitleFontSize );
	font.setBold( true );
	text.setFont( font );
	setTitle( text );
}


/////////////////////////////////////////////////////////////////////////////////////////
//									Axis
/////////////////////////////////////////////////////////////////////////////////////////



void C2DPlotWidget::SetAxisTitle( Axis axis, const std::string &title )
{
	QwtText text( t2q( title ) );
	QFont font( t2q( smFontName ), smAxisFontSize );
	font.setBold( true );
	text.setFont( font );
    setAxisTitle( axis, text );
}


void C2DPlotWidget::SetAxisScales( double xMin, double xMax, double yMin, double yMax, double y2Min, double y2Max )   //y2Min = defaultValue<double>(), double y2Max = defaultValue<double>()
{
	for ( auto *c : mCurves )
		c->SetRanges( xMin, xMax, yMin, yMax );

	for ( auto *h : mHistograms )
		h->SetRanges( xMin, xMax, yMin, yMax );

	setAxisScale( xBottom, xMin, xMax );
	setAxisScale( yLeft, yMin, yMax  );
	if ( !isDefaultValue( y2Min ) && !isDefaultValue( y2Max ) )
		setAxisScale( yRight, y2Min, y2Max  );
}




/////////////////////////////////////////////////////////////////////////////////////////
//									Spectrogram
/////////////////////////////////////////////////////////////////////////////////////////

QwtPlotSpectrogram* C2DPlotWidget::AddRaster( const std::string &title, const C3DPlotInfo &maps, double min_contour, double max_contour, size_t ncontours, size_t index )
{
    assert__( mCurves.size() == 0 );            Q_UNUSED( index );

    mSpectrograms.push_back( new QwtPlotSpectrogram );
	mCurrentSpectrogram = mSpectrograms.back();

	const C3DPlotInfo::value_type &map = maps[ 0 ];	//the index parameter is the index of recently created spectrogram, not the maps index
    mCurrentSpectrogram->setData( maps );
	mCurrentSpectrogram->attach( this );
    //QVector<double> vmatrix;
    //for ( double d : map.mValues )
    //    vmatrix.append(d);
    //const_cast<C3DPlotInfo&>( maps ).setValueMatrix( vmatrix, 1 );
	//const_cast<C3DPlotInfo&>( maps ).setBoundingRect( QwtDoubleRect( 0, 0, 10*(map.mMaxX - map.mMinX), 10*(map.mMaxY - map.mMinY )) );

	EnableAxisY2();
	SetAxisScales( map.mMinX, map.mMaxX, map.mMinY, map.mMaxY, mCurrentSpectrogram->data().range().minValue(), mCurrentSpectrogram->data().range().maxValue() );

    // Color map

	QwtLinearColorMap color_map( Qt::darkCyan, Qt::red );
	color_map.addColorStop( 0.1, Qt::cyan );
	color_map.addColorStop( 0.4, Qt::green );
	color_map.addColorStop( 0.90, Qt::yellow );
	mCurrentSpectrogram->setColorMap( color_map );
	//color_map.setMode( color_map.ScaledColors );
	//color_map.setMode( color_map.FixedColors );

	//...color bar on the right axis
	QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
    rightAxis->setTitle( title.c_str() );
	rightAxis->setColorBarEnabled( true );
	rightAxis->setColorMap( mCurrentSpectrogram->data().range(), mCurrentSpectrogram->colorMap() );

	// Contour levels

	if ( ncontours > 0 && !isDefaultValue( ncontours ) )
	{
		if ( !isDefaultValue( min_contour ) && !isDefaultValue( max_contour ) )
		{
			QwtValueList contour_levels;
			double step = ( max_contour - min_contour ) / ncontours;						assert__( step > 0 );
			if ( step > 0 )
				for ( double level = map.mMinHeightValue; level < max_contour; level += step )
				{
					contour_levels += level;
				}
			mCurrentSpectrogram->setContourLevels( contour_levels );
		}
	}

	plotLayout()->setAlignCanvasToScales( true );
	replot();
	
    // Avoid jumping when labels with more/less digits appear/disappear when scrolling vertically

	const QFontMetrics fm( axisWidget( QwtPlot::yLeft )->font() );
    QwtScaleDraw *sd = axisScaleDraw( QwtPlot::yLeft );
	sd->setMinimumExtent( fm.width( "100.00" ) );

	return mCurrentSpectrogram;
}


void C2DPlotWidget::SetCurrentRaster( int index )
{
	assert__( index < mSpectrograms.size() );

	mCurrentSpectrogram->detach();
	mCurrentSpectrogram = mSpectrograms[ index ];
	mCurrentSpectrogram->attach( this );
	replot();
}



bool C2DPlotWidget::HasContour() const
{
	assert__( mCurrentSpectrogram );

	return mCurrentSpectrogram->testDisplayMode( QwtPlotSpectrogram::ContourMode );
}

void C2DPlotWidget::ShowContour( bool show )
{
	assert__( mCurrentSpectrogram );

	mCurrentSpectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode, show );
	replot();
}


bool C2DPlotWidget::HasSolidColor() const
{
	assert__( mCurrentSpectrogram );

	return mCurrentSpectrogram->testDisplayMode( QwtPlotSpectrogram::ImageMode );
}

void C2DPlotWidget::ShowSolidColor( bool show )
{
	assert__( mCurrentSpectrogram );

	mCurrentSpectrogram->setDisplayMode( QwtPlotSpectrogram::ImageMode, show );
	mCurrentSpectrogram->setDefaultContourPen( show ? QPen() : QPen( Qt::NoPen ) );
	replot();
}


void C2DPlotWidget::ShowContour( int index, bool show )
{
	assert__( index < mSpectrograms.size() );

	mSpectrograms[index]->setDisplayMode( QwtPlotSpectrogram::ContourMode, show );
}
void C2DPlotWidget::ShowSolidColor( int index, bool show )
{
	assert__( index < mSpectrograms.size() );

	mSpectrograms[index]->setDisplayMode( QwtPlotSpectrogram::ImageMode, show );
	mSpectrograms[index]->setDefaultContourPen( show ? QPen() : QPen( Qt::NoPen ) );
}






/////////////////////////////////////////////////////////////////////////////////////////
//									Histograms
/////////////////////////////////////////////////////////////////////////////////////////

//QwtPlotGrid *grid = new QwtPlotGrid;
//grid->enableXMin(true);
//grid->enableYMin(true);
//grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
//grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
//grid->attach(&plot);
//
//HistogramItem *histogram = new HistogramItem();
//histogram->setColor(Qt::darkCyan);
//
//const int numValues = 20;
//
//QwtArray<QwtDoubleInterval> intervals(numValues);
//QwtArray<double> values(numValues);
//
//double pos = 0.0;
//for ( int i = 0; i < (int)intervals.size(); i++ )
//{
//    const int width = 5 + rand() % 15;
//    const int value = rand() % 100;
//
//    intervals[i] = QwtDoubleInterval(pos, pos + double(width));
//    values[i] = value; 
//
//    pos += width;
//}
//
//histogram->setData(QwtIntervalData(intervals, values));
//histogram->attach(&plot);
//
//plot.setAxisScale(QwtPlot::yLeft, 0.0, 100.0);
//plot.setAxisScale(QwtPlot::xBottom, 0.0, pos);
//plot.replot();

CHistogram* C2DPlotWidget::AddHistogram( const std::string &title, QColor color, const CQwtArrayPlotData *data )
{
	assert__( mSpectrograms.size() == 0 && mCurves.size() == 0 );

	QwtPlotGrid *grid = new QwtPlotGrid;
	grid->enableXMin( true );
	grid->enableYMin( true );
	grid->setMajPen( QPen( Qt::black, 0, Qt::DotLine ) );
	grid->setMinPen( QPen( Qt::gray, 0, Qt::DotLine ) );
	grid->attach( this );

	mHistograms.push_back( new CHistogram );
	CHistogram *histogram = mHistograms.back();

	histogram->setColor( color );
	histogram->setTitle( t2q( title ) );

	size_t data_size = data->size();			//is sizeX
	double xmin, xmax;
	data->GetXRange( xmin, xmax );				//double ymin, ymax;	//data->GetYRange( ymin, ymax );

	const int num_values = 24;					//arbitrary, must be an input parameter
	const int step = (int)data_size / num_values;

	QwtArray< QwtDoubleInterval > intervals( num_values );
	QwtArray< double > values( num_values );

	double pos = xmin;
	for ( int i = 0; i < intervals.size(); i++ )
	{
		size_t index = i * step;

		values[ i ] = data->y( index );
		if ( isDefaultValue( values[ i ] ) || std::isnan( values[ i ] ) )
			values[ i ] = 0;

		double xvalue = data->x( index + step );
		intervals[ i ] = QwtDoubleInterval( pos, xvalue );

		pos = xvalue;
	}

	histogram->setData( QwtIntervalData( intervals, values ) );
	histogram->attach( this );

	return histogram;
}



/////////////////////////////////////////////////////////////////////////////////////////
//									Curves
/////////////////////////////////////////////////////////////////////////////////////////

QwtPlotCurve* C2DPlotWidget::AddCurve( const std::string &title, QColor color, const CQwtArrayPlotData *data )	//data = nullptr 
{
	assert__( mSpectrograms.size() == 0 && mHistograms.size() == 0 );

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
	assert__( mSpectrograms.size() == 0 && mHistograms.size() == 0 );

    QwtPlotCurve *c = new QwtPlotCurve( title.c_str() );
    c->setRenderHint(QwtPlotItem::RenderAntialiased);
    c->setPen( QPen( color ) );
    c->attach( this );
    c->setData( data );
	return c;
}


void C2DPlotWidget::SetCurvesStyle( QwtPlotCurve::CurveStyle style )
{
	for ( auto *c : mCurves )
		c->setStyle( style );

	replot();
}


//////////////
//	Curve Line
//////////////


void C2DPlotWidget::EnableCurveLine( int curve, bool enable )
{
	assert__( curve < mCurves.size() );

	CGeneralizedCurve * pcurve = mCurves[ curve ];
    QwtSymbol c_symbol = pcurve->symbol();
    pcurve->setStyle( enable ? QwtPlotCurve::Lines : QwtPlotCurve::NoCurve );
	replot();
}



QColor C2DPlotWidget::CurveLineColor( int curve ) const
{
	assert__( curve < mCurves.size() );

	return mCurves[ curve ]->pen().color();
}

void C2DPlotWidget::SetCurveLineColor( int curve, QColor color )
{
	assert__( curve < mCurves.size() );

	CGeneralizedCurve *pcurve = mCurves[ curve ];			assert__( pcurve );

	QPen pen = pcurve->pen();
	QColor old_color = pen.color();

	old_color.setGreen( color.green() );
	old_color.setRed( color.red() );
	old_color.setBlue( color.blue() );

	pen.setColor( old_color );
	pcurve->setPen( pen );
	replot();
}



int C2DPlotWidget::CurveLineOpacity( int curve ) const
{
	assert__( curve < mCurves.size() );

	return mCurves[ curve ]->pen().color().alpha();
}

void C2DPlotWidget::SetCurveLineOpacity( int curve, int alpha )
{
	assert__( curve < mCurves.size() );

	CGeneralizedCurve *pcurve = this->mCurves[ curve ];		assert__( pcurve );

	QPen pen = pcurve->pen();
	QColor old_color = pen.color();

	old_color.setAlpha( alpha );
	pen.setColor( old_color );
	pcurve->setPen( pen );
	replot();
}



EStipplePattern C2DPlotWidget::CurveLinePattern( int curve ) const
{
	assert__( curve < mCurves.size() );

	return pattern_cast< EStipplePattern >( mCurves[ curve ]->pen().style() );
}

void C2DPlotWidget::SetCurveLinePattern( int curve, EStipplePattern p )
{
	assert__( curve < mCurves.size() && p >= eFirstStipplePattern && p < EStipplePattern_size );

	CGeneralizedCurve *pcurve = mCurves[ curve ];	   		assert__( pcurve );

	QPen old_pen = pcurve->pen();
	old_pen.setStyle( pattern_cast<Qt::PenStyle >( p ) );
	pcurve->setPen( old_pen );
	replot();
}




int C2DPlotWidget::CurveLineWidth( int curve ) const
{
	assert__( curve < mCurves.size() );

	return mCurves[ curve ]->pen().width();
}

void C2DPlotWidget::SetCurveLineWidth( int curve, int pixels )
{
	assert__( curve < mCurves.size() );

	CGeneralizedCurve *pcurve = mCurves[ curve ];	   		assert__( pcurve );

	QPen pen = pcurve->pen();
	pen.setWidth( pixels );
	pcurve->setPen( pen );
	replot();
}



////////////////
//	Curve Points
////////////////


void C2DPlotWidget::EnableCurvePoints( int curve, bool enable, EPointGlyph default_symbol )
{
	assert__( curve < mCurves.size() );

	CGeneralizedCurve *pcurve = mCurves[ curve ];	   		assert__( pcurve );

	QwtSymbol c_symbol = pcurve->symbol();

	c_symbol.setStyle( enable ? symbol_cast< QwtSymbol::Style >( default_symbol ) : QwtSymbol::NoSymbol );
	pcurve->setSymbol( c_symbol );

	replot();
}



QColor C2DPlotWidget::CurvePointColor( int curve ) const
{
	assert__( curve < mCurves.size() );

	return mCurves[ curve ]->symbol().pen().color();
}

void C2DPlotWidget::SetCurvePointColor( int curve, QColor color )
{
	assert__( curve < mCurves.size() );

	CGeneralizedCurve *pcurve = mCurves[ curve ];	   		assert__( pcurve );

	QwtSymbol c_symbol = pcurve->symbol();
	QPen pen = c_symbol.pen();
	pen.setColor( color );
	c_symbol.setPen( pen );
	pcurve->setSymbol( c_symbol );
	replot();
}



bool C2DPlotWidget::IsCurvePointFilled( int curve ) const
{
	assert__( curve < mCurves.size() );

	return mCurves[ curve ]->symbol().brush() != Qt::NoBrush;
}

void C2DPlotWidget::SetCurvePointFilled( int curve, bool fill )
{
	assert__( curve < mCurves.size() );

	CGeneralizedCurve *pcurve = mCurves[ curve ];	   		assert__( pcurve );

	QwtSymbol c_symbol = pcurve->symbol();
	QBrush curr_brush = c_symbol.brush();

	if ( !fill )
	{
		curr_brush = Qt::NoBrush;
	}
	else
	{
		curr_brush.setColor( pcurve->symbol().pen().color() );
		curr_brush.setStyle( Qt::SolidPattern );
	}
	c_symbol.setBrush( curr_brush );
	pcurve->setSymbol( c_symbol );
	replot();
}



EPointGlyph C2DPlotWidget::CurvePointGlyph( int curve ) const
{
	assert__( curve < mCurves.size() );

	return symbol_cast< EPointGlyph >( mCurves[ curve ]->symbol().style() );
}

void C2DPlotWidget::SetCurvePointGlyph( int curve, EPointGlyph symbol )
{
	assert__( curve < mCurves.size() );

	CGeneralizedCurve *pcurve = mCurves[ curve ];	   		assert__( pcurve );

    QwtSymbol c_symbol = pcurve->symbol();
    c_symbol.setStyle( symbol_cast< QwtSymbol::Style >( symbol) );
    pcurve->setSymbol( c_symbol );
	replot();
}



int C2DPlotWidget::CurvePointSize( int curve ) const
{
	assert__( curve < mCurves.size() );

	return mCurves[ curve ]->symbol().size().height();
}

void C2DPlotWidget::SetCurvePointSize( int curve, int pixels )
{
	assert__( curve < mCurves.size() );

	CGeneralizedCurve *pcurve = mCurves[ curve ];	   		assert__( pcurve );

	QSize new_size( pixels, pixels );
	QwtSymbol c_symbol = pcurve->symbol();
	c_symbol.setSize( new_size );
	pcurve->setSymbol( c_symbol );
	replot();
}






/////////////////////////////////////////////////////////////////////////////////////////
//								Markers
/////////////////////////////////////////////////////////////////////////////////////////


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


/////////////////////////////////////////////////////////////////////////////////////////
//						Magnifiers and Zoomers and Panners
/////////////////////////////////////////////////////////////////////////////////////////

////////////////////
//	Custom Zoomers	
////////////////////

class C2DZoomer : public QwtPlotZoomer
{
	using base_t = QwtPlotZoomer;

public:
	C2DZoomer( QwtPlotCanvas *canvas ) :
		QwtPlotZoomer( canvas )
	{
		setTrackerMode( AlwaysOn );
		setMousePattern( QwtEventPattern::MouseSelect3, Qt::RightButton );
		setMousePattern( QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier );
		const QColor c(Qt::darkBlue);
		setRubberBandPen(c);
		setTrackerPen(c);
	}

	virtual QwtText trackerText( const QwtDoublePoint &pos ) const
	{
		QColor bg( Qt::white );
		bg.setAlpha( 200 );
		QwtText text = QwtPlotZoomer::trackerText( pos );
		text.setBackgroundBrush( QBrush( bg ) );
		return text;
	}

	void Home()
	{
		zoom( 0 );
	}
};


void C2DPlotWidget::Home()
{
	if ( mZoomer )			//null for curves
	{
		mZoomer->Home();
	}

	double xMin, xMax, yMin, yMax;
	if ( mCurves.size() > 0 )
	{
		CGeneralizedCurve *pcurve = mCurves[ 0 ];
		pcurve->Ranges( xMin, xMax, yMin, yMax );
	}
	else
	if ( mHistograms.size() > 0 )
	{						//mHistograms
		auto *phisto = mHistograms[ 0 ];
		phisto->Ranges( xMin, xMax, yMin, yMax );
	}
	else
		return;

	SetAxisScales( xMin, xMax, yMin, yMax );
	replot();
}


QwtPlotMagnifier* C2DPlotWidget::AddMagnifier()
{
	assert__( !mMagnifier );

	mMagnifier = new QwtPlotMagnifier( canvas() );
	return mMagnifier;
}


QwtPlotZoomer* C2DPlotWidget::AddZoomer()
{
	// LeftButton for the zooming
	// RightButton: zoom out by 1
	// Ctrl+RighButton: zoom out to full size

	assert__( !mZoomer );

	mZoomer = new C2DZoomer( canvas() );
	return mZoomer;
}


QwtPlotPanner* C2DPlotWidget::AddPanner()
{
	assert__( !mPanner );

	// MidButton for the panning
	mPanner = new QwtPlotPanner( canvas() );
	mPanner->setAxisEnabled( QwtPlot::yRight, false );
	mPanner->setMouseButton( Qt::MidButton );

	return mPanner;
}



/////////////////////////////////////////////////////////////////////////////////////////
//									Legends
/////////////////////////////////////////////////////////////////////////////////////////


QwtLegend* C2DPlotWidget::AddLegend( LegendPosition pos )
{
	QwtLegend *legend = new QwtLegend;
    insertLegend( legend, pos );
	legend->setDisplayPolicy( QwtLegend::AutoIdentifier, 0 );
	return legend;
}



/////////////////////////////////////////////////////////////////////////////////////////
//									Print
/////////////////////////////////////////////////////////////////////////////////////////


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
	return mSizeHint;
}


//virtual 
void C2DPlotWidget::replot()
{
	try
	{
		base_t::replot();
	}
	catch ( ... )
	{
		clear();
	}
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_2DPlotWidget.cpp"
