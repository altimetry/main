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


//////////////////////////////////////////////////////////////////
//						Custom Curve
//////////////////////////////////////////////////////////////////

void CGeneralizedCurve::CommonConstruct()
{
	if ( mData )
		setData( *mData );
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

void CGeneralizedCurve::AxisTitles( std::string &xtitle, std::string &ytitle, std::string &ztitle )
{
	xtitle = mXtitle;
	ytitle = mYtitle;
	ztitle = mZtitle;
}
void CGeneralizedCurve::SetAxisTitles( const std::string &xtitle, const std::string &ytitle, const std::string &ztitle )
{
	mXtitle = xtitle;
	mYtitle = ytitle;
	mZtitle = ztitle;
}


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


size_t CGeneralizedCurve::CurrentFrame()
{
	return mData->CurrentFrame();
}

bool CGeneralizedCurve::FrameUpdated()
{
	bool stop = false;
	if ( !mLoop )
	{
		stop = ( mAnimForward && CurrentFrame() == 0 ) || ( !mAnimForward && CurrentFrame() == mData->GetNumberOfFrames() - 1 );
	}

	if ( stop )
	{
		size_t start_frame = mAnimForward ? 0 : mData->GetNumberOfFrames() - 1;
		mData->SetFrame( start_frame );
	}

	mRangeComputed = false;		//TODO really needed? 
	setData( *mData );

	return stop;
}


bool CGeneralizedCurve::SetFrame( size_t frame )
{
	assert__( mData );

	mData->SetFrame( frame );
	return FrameUpdated();
}


bool CGeneralizedCurve::ChangeFrame()
{
	assert__( mData );

	mData->SetNextFrame( mAnimForward );
	return FrameUpdated();
}


bool CGeneralizedCurve::ResetFrame()
{
	size_t start_frame = mAnimForward ? 0 : mData->GetNumberOfFrames() - 1;
	return SetFrame( start_frame );
}


void CGeneralizedCurve::SetMovingForward( bool forward )
{
	mAnimForward = forward;
}


void CGeneralizedCurve::SetLoop( bool loop )
{
	mLoop = loop;
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
//						Custom Spectrogram
//////////////////////////////////////////////////////////////////

class CBratSpectrogram : public QwtPlotSpectrogram
{
	using base_t = QwtPlotSpectrogram;

	double mMinXValue = 0.;
	double mMaxXValue = 0.;

	double mMinYValue = 0.;
	double mMaxYValue = 0.;

	std::string mXtitle;
	std::string mYtitle;
	std::string mZtitle;

public:

	explicit CBratSpectrogram( const QString &title = QString::null )
		: base_t( title )
	{}
	virtual ~CBratSpectrogram()
	{}

	// access

	void AxisTitles( std::string &xtitle, std::string &ytitle, std::string &ztitle )
	{
		xtitle = mXtitle;
		ytitle = mYtitle;
		ztitle = mZtitle;
	}
	void SetAxisTitles( const std::string &xtitle, const std::string &ytitle, const std::string &ztitle )
	{
		mXtitle = xtitle;
		mYtitle = ytitle;
		mZtitle = ztitle;
	}


	void Ranges( double &xMin, double &xMax, double &yMin, double &yMax )
	{
		xMin = mMinXValue;
		xMax = mMaxXValue;

		yMin = mMinYValue;
		yMax = mMaxYValue;
	}

	void SetRanges( double xMin, double xMax, double yMin, double yMax )
	{
		mMinXValue = xMin;
		mMaxXValue = xMax;

		mMinYValue = yMin;
		mMaxYValue = yMax;
	}
};



//////////////////////////////////////////////////////////////////
//						Custom Scale Draw
//////////////////////////////////////////////////////////////////

//choice of MAX_MANTISSA as 15 because
//the compiler generated code is lighter (and value, 0xF)

static const int MAX_MANTISSA = 15;

class CBratScaleDraw : public QwtScaleDraw
{
private:
    int mantissa_digits;
public:
    CBratScaleDraw();
    CBratScaleDraw(int _mantissa);

    //our overloads

    QwtText label(double) const;

    int GetMantissa() const
    {
        return mantissa_digits;
    }

    void SetMantissa(int _mantissa)
    {
        mantissa_digits = _mantissa;
    }
};



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
    return QString::number(value, 'g', mantissa_digits);
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
	, mIsLog( false )
{
	setWindowIcon( QPixmap( ":/2.png" ) );
	setWindowTitle( "[*]" );
	setAttribute( Qt::WA_DeleteOnClose );

	setCanvasBackground( Qt::white );

	QwtLegend *legend = AddLegend( RightLegend );		Q_UNUSED( legend );

	connect( &mTimer, SIGNAL( timeout() ), this, SLOT( ChangeFrame() ) );


	QwtScaleWidget *x = axisWidget( xBottom );
	QwtScaleWidget *y = axisWidget( yLeft );
	QwtScaleWidget *y2 = axisWidget( yRight );

	connect( x, SIGNAL( scaleDivChanged() ), this, SLOT( HandleScaleDivChanged() ) );
	connect( y, SIGNAL( scaleDivChanged() ), this, SLOT( HandleScaleDivChanged() ) );
	connect( y2, SIGNAL( scaleDivChanged() ), this, SLOT( HandleScaleDivChanged() ) );


    CBratScaleDraw* pDrawer = new CBratScaleDraw();
    pDrawer->SetMantissa(2);
    setAxisScaleDraw(QwtPlot::xBottom, pDrawer);
    pDrawer = new CBratScaleDraw();
    pDrawer->SetMantissa(2);
    setAxisScaleDraw(QwtPlot::yLeft, pDrawer);

    setMinimumSize( min_plot_widget_width, min_plot_widget_height );
}



void C2DPlotWidget::RescaleX( double x )
{
	double xMin, xMax, yMin, yMax, y2Min, y2Max;
	CurrentRanges( xMin, xMax, yMin, yMax, y2Min, y2Max );

	const double range = xMax - xMin;
	const double center = xMin + range / 2;
	const double width = range / 2 * ( 1 / x );

	setAxisScale( xBottom, center - width, center + width );
	replot();
}
void C2DPlotWidget::RescaleY( double y )
{
	double xMin, xMax, yMin, yMax, y2Min, y2Max;
	CurrentRanges( xMin, xMax, yMin, yMax, y2Min, y2Max );

	const double range = yMax - yMin;
	const double center = yMin + range / 2;
	const double width = range / 2 * ( 1 / y );

	setAxisScale( yLeft, center - width, center + width );
	replot();
}

void C2DPlotWidget::HandleScaleDivChanged()
{
	QwtScaleWidget *axis = qobject_cast<QwtScaleWidget*>( sender() );
	if ( !axis )
		return;

	double xMin, xMax, yMin, yMax, y2Min, y2Max;
	CurrentRanges( xMin, xMax, yMin, yMax, y2Min, y2Max );

	int iaxis = 0;
	QwtScaleDiv *div = nullptr;
	double *pscale_factor = nullptr;
	double R = 0.;
	if ( axis == axisWidget( yLeft ) )
	{
		iaxis = 1;
		div = axisScaleDiv( yLeft );
		R = yMax - yMin;
		pscale_factor = &mYScaleFactor;
	}
	else
	if ( axis == axisWidget( yRight ) )
	{
		iaxis = 2;
		div = axisScaleDiv( yRight );
		R = y2Max - y2Min;
		pscale_factor = &mY2ScaleFactor;
	}
	else
	{
		assert__( axis == axisWidget( xBottom ) );
		div = axisScaleDiv( xBottom );

		R = xMax - xMin;
		pscale_factor = &mXScaleFactor;
	}

	std::string msg = n2s<std::string>( div->lowerBound() ) + " <=> " + n2s<std::string>( div->upperBound() );

	*pscale_factor = R / ( div->upperBound() - div->lowerBound() );

	emit ScaleDivChanged( iaxis, *pscale_factor, msg.c_str() );
}



C2DPlotWidget::~C2DPlotWidget()
{
	DestroyPointersAndContainer( mCurves );
	DestroyPointersAndContainer( mSpectrograms );
	DestroyPointersAndContainer( mHistograms );
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
//									Animation
/////////////////////////////////////////////////////////////////////////////////////////


void C2DPlotWidget::Animate( int updateinterval )
{
	if ( updateinterval == 0 )
	{
		mTimer.stop();
	}
	else
	{
		//first "manual" impulse: if current frame is at the end, it will be reset
		//
		for ( auto *c : mCurves )
			c->ChangeFrame();

		mTimer.start( updateinterval );
	}
}


void C2DPlotWidget::SetSpeed( int updateinterval )
{
	mTimer.setInterval( updateinterval );
}


//slot
void C2DPlotWidget::ChangeFrame()
{
	bool stop = false;
	for ( auto *c : mCurves )
		stop |= c->ChangeFrame();

	replot();

	if ( stop )
	{
		Animate( 0 );
		emit AnimationStopped();
	}

	emit FrameChanged( mCurves[0]->CurrentFrame() );
}


void C2DPlotWidget::StopAnimation()
{
	Animate( 0 );
	for ( auto *c : mCurves )
		c->ResetFrame();

	replot();

	size_t current_frame = 0;
	if ( mCurves.size() > 0 )
		current_frame = mCurves[ 0 ]->CurrentFrame();

	emit FrameChanged( current_frame );
}


void C2DPlotWidget::SetFrame( unsigned int frame )
{
	for ( auto *c : mCurves )
		c->SetFrame( frame );

	replot();

	//emit FrameChanged( mCurves[0]->CurrentFrame() );	if assigned from outside, do not emit
}


void C2DPlotWidget::SetMovingForward( bool forward )
{
	for ( auto *c : mCurves )
		c->SetMovingForward( forward );
}


void C2DPlotWidget::SetLoop( bool loop )
{
	for ( auto *c : mCurves )
		c->SetLoop( loop );
}




/////////////////////////////////////////////////////////////////////////////////////////
//									Axis
/////////////////////////////////////////////////////////////////////////////////////////



void C2DPlotWidget::AxisTitles( std::string *xtitle, std::string *ytitle, std::string *y2title )
{
	if ( xtitle )
		*xtitle = q2a( axisTitle( xBottom ).text() );
	if ( ytitle )
		*ytitle = q2a( axisTitle( yLeft ).text() );
	if ( y2title )
		*y2title = q2a( axisTitle( yRight ).text() );
}


void C2DPlotWidget::SetAxisTitles( int index, const std::string &xtitle, const std::string &ytitle, const std::string &y2title )		//y2title = "" 
{
	if ( mCurves.size() > 0 )
		mCurves[index]->SetAxisTitles( xtitle, ytitle, y2title );

	if ( mHistograms.size() > 0 )
		mHistograms[index]->SetAxisTitles( xtitle, ytitle, y2title );

	if ( mSpectrograms.size() > 0 )
		mSpectrograms[index]->SetAxisTitles( xtitle, ytitle, y2title );

	SetAxisTitle( xBottom, xtitle );
	SetAxisTitle( yLeft, ytitle );
	if ( !y2title.empty() )
		SetAxisTitle( yRight, y2title );
}


void C2DPlotWidget::SetAxisTitle( Axis axis, const std::string &title )
{
	QwtText text( t2q( title ) );
	QFont font( t2q( smFontName ), smAxisFontSize );
	font.setBold( true );
	text.setFont( font );
    setAxisTitle( axis, text );
}


void C2DPlotWidget::HistogramAxisTitles( int index, std::string &xtitle, std::string &ytitle, std::string &y2title )
{
	assert__( index < (int)mHistograms.size() );

	mHistograms[ index ]->AxisTitles( xtitle, ytitle, y2title );
}


void C2DPlotWidget::SpectrogramAxisTitles( int index, std::string &xtitle, std::string &ytitle, std::string &y2title )
{
	assert__( index < (int)mSpectrograms.size() );

	mSpectrograms[ index ]->AxisTitles( xtitle, ytitle, y2title );
}



//...digits

int C2DPlotWidget::GetXAxisMantissa() const
{
	const CBratScaleDraw* curr_drawer = dynamic_cast<const CBratScaleDraw*>( axisScaleDraw( QwtPlot::xBottom ) );
	if ( !curr_drawer )
	{
		//default value
		return 2;
	}
	return curr_drawer->GetMantissa();
}

int C2DPlotWidget::GetYAxisMantissa() const
{
	const CBratScaleDraw* curr_drawer = dynamic_cast<const CBratScaleDraw*>( axisScaleDraw( QwtPlot::yLeft ) );
	if ( !curr_drawer )
	{
		//default value
		return 2;
	}
	return curr_drawer->GetMantissa();
}

void C2DPlotWidget::SetXAxisMantissa( int new_mantissa )
{
	CBratScaleDraw* scale_drawer = new CBratScaleDraw();
	scale_drawer->SetMantissa( new_mantissa );
	setAxisScaleDraw( QwtPlot::xBottom, scale_drawer );
	replot();
}

void C2DPlotWidget::SetYAxisMantissa( int new_mantissa )
{
	CBratScaleDraw* scale_drawer = new CBratScaleDraw();
	scale_drawer->SetMantissa( new_mantissa );
	setAxisScaleDraw( QwtPlot::yLeft, scale_drawer );
	replot();
}




//...scale

void C2DPlotWidget::CurrentHistogramRanges( double &xMin, double &xMax, double &yMin, double &yMax )
{
	assert__( mCurrentHistogram );

	mCurrentHistogram->Ranges( xMin, xMax, yMin, yMax );
}

void C2DPlotWidget::CurrentSpectrogramRanges( double &xMin, double &xMax, double &yMin, double &yMax, double &y2Min, double &y2Max )
{
	assert__( mCurrentSpectrogram );

	y2Min = mCurrentSpectrogram->data().range().minValue();
	y2Max = mCurrentSpectrogram->data().range().maxValue();

	mCurrentSpectrogram->Ranges( xMin, xMax, yMin, yMax );
}

void C2DPlotWidget::CurrentRanges( double &xMin, double &xMax, double &yMin, double &yMax, double &y2Min, double &y2Max  )
{
	if ( mSpectrograms.size() > 0 )
	{	
		assert__( mCurrentSpectrogram );

		CurrentSpectrogramRanges( xMin, xMax, yMin, yMax, y2Min, y2Max );
	}
	else
	{
		AxisScales( xMin, xMax, yMin, yMax, y2Min, y2Max );

		if ( mCurves.size() > 0 )
		{
			CGeneralizedCurve *pcurve = mCurves[ 0 ];
			pcurve->Ranges( xMin, xMax, yMin, yMax );
		}
		else
		if ( mHistograms.size() > 0 )
		{
			assert__( mCurrentHistogram );

			mCurrentHistogram->Ranges( xMin, xMax, yMin, yMax );
		}
	}
}


void C2DPlotWidget::SetPlotAxisScales( int index, double xMin, double xMax, double yMin, double yMax, double y2Min, double y2Max )   //y2Min = defaultValue<double>(), double y2Max = defaultValue<double>()
{
	for ( auto *curve : mCurves )
		curve->SetRanges( xMin, xMax, yMin, yMax );

	if ( mHistograms.size() > 0 )
		mHistograms[index]->SetRanges( xMin, xMax, yMin, yMax );

	if ( mSpectrograms.size() > 0 )
		mSpectrograms[index]->SetRanges( xMin, xMax, yMin, yMax );

	SetAxisScales( xMin, xMax, yMin, yMax, y2Min, y2Max );
}


void C2DPlotWidget::AxisScales( double &xMin, double &xMax, double &yMin, double &yMax, double &y2Min, double &y2Max )
{
	QwtScaleDiv *xdiv = axisScaleDiv( xBottom );
	QwtScaleDiv *ydiv = axisScaleDiv( yLeft );
	QwtScaleDiv *y2div = axisScaleDiv( yRight );
	
	xMin = xdiv->lowerBound();
	xMax = xdiv->upperBound();

	yMin = ydiv->lowerBound();
	yMax = ydiv->upperBound();

	y2Min = y2div->lowerBound();
	y2Max = y2div->upperBound();
}
void C2DPlotWidget::SetAxisScales( double xMin, double xMax, double yMin, double yMax, double y2Min, double y2Max )   //y2Min = defaultValue<double>(), double y2Max = defaultValue<double>()
{
	setAxisScale( xBottom, xMin, xMax );
	setAxisScale( yLeft, yMin, yMax  );
	if ( !isDefaultValue( y2Min ) && !isDefaultValue( y2Max ) )
		setAxisScale( yRight, y2Min, y2Max  );
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
//									Spectrogram
/////////////////////////////////////////////////////////////////////////////////////////


QwtPlotSpectrogram* C2DPlotWidget::PushRaster( const std::string &title, const C3DPlotInfo &maps, double min_contour, double max_contour, size_t ncontours, const QwtColorMap &color_map )
{
    assert__( mCurves.size() == 0 );

    mSpectrograms.push_back( new CBratSpectrogram );
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
	SetPlotAxisScales( (int)mSpectrograms.size() - 1, map.mMinX, map.mMaxX, map.mMinY, map.mMaxY, mCurrentSpectrogram->data().range().minValue(), mCurrentSpectrogram->data().range().maxValue() );

    // Color map

	////////////////QwtLinearColorMap color_map( Qt::darkCyan, Qt::red );
	////////////////color_map.addColorStop( 0.1, Qt::cyan );
	////////////////color_map.addColorStop( 0.4, Qt::green );
	////////////////color_map.addColorStop( 0.90, Qt::yellow );
	mCurrentSpectrogram->setColorMap( color_map );

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
	assert__( size_t( index ) < mSpectrograms.size() );

	mCurrentSpectrogram->detach();
	mCurrentSpectrogram = mSpectrograms[ index ];
	mCurrentSpectrogram->attach( this );

	double xMin, xMax, yMin, yMax, y2Min, y2Max;
	CurrentSpectrogramRanges( xMin, xMax, yMin, yMax, y2Min, y2Max );
	EnableAxisY2();
	SetAxisScales( xMin, xMax, yMin, yMax, y2Min, y2Max );

	std::string xtitle, ytitle, ztitle;
	SpectrogramAxisTitles( index, xtitle, ytitle, ztitle );
	SetAxisTitles( index, xtitle, ytitle, ztitle );

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
	assert__( size_t( index ) < mSpectrograms.size() );

	mSpectrograms[index]->setDisplayMode( QwtPlotSpectrogram::ContourMode, show );
}
void C2DPlotWidget::ShowSolidColor( int index, bool show )
{
	assert__( size_t( index ) < mSpectrograms.size() );

	mSpectrograms[index]->setDisplayMode( QwtPlotSpectrogram::ImageMode, show );
	mSpectrograms[index]->setDefaultContourPen( show ? QPen() : QPen( Qt::NoPen ) );
}


void C2DPlotWidget::SetRasterColorMap( const QwtColorMap &color_map )
{
	assert__( mCurrentSpectrogram );

	mCurrentSpectrogram->setColorMap( color_map );
	QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
	rightAxis->setColorMap( mCurrentSpectrogram->data().range(), mCurrentSpectrogram->colorMap() );

	replot();
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

int C2DPlotWidget::NumberOfBins( int index ) const 
{
	assert__( index < (int)mHistograms.size() );

	return (int)mHistograms[ index ]->data().size();
}


template< typename DATA >
CHistogram* C2DPlotWidget::CreateHistogram( const std::string &title, QColor color, const DATA *data, double &max_freq, int bins )
{
	CHistogram *h = new CHistogram;

	h->setColor( color );
	h->setTitle( t2q( title ) );

	//size_t data_size = data->GetDataSize( 0 );
	double m, M;
	data->GetDataRange( m, M, 0 );

	QwtArray< QwtDoubleInterval > intervals( bins );
	QwtArray< double > values( bins );

    const double width = ( M - m ) / double( bins );
	double b0 = m;
	max_freq = std::numeric_limits<double>::lowest();
	for ( int i = 0; i < (int)intervals.size(); i++ )
	{
		double b1 = b0 + width;

	    intervals[i] = QwtDoubleInterval( b0, b1 );

		values[ i ] = data->GetDataCountIf( 0, [&b0, &b1]( const double &v )
		{
			return !isDefaultValue( v ) && v >= b0 && v < b1;
		}
		);

		max_freq = std::max( max_freq, values[ i ] );
	    b0 = b1;
	}

	//SetAxisScales( m, b0, 0, max_value );

	h->setData( QwtIntervalData( intervals, values ) );

	return h;
}



CHistogram* C2DPlotWidget::AddHistogram( const std::string &title, QColor color, const CQwtArrayPlotData *data, double &max_freq, int bins )
{
	assert__( mSpectrograms.size() == 0 && mCurves.size() == 0 );

	if ( mHistograms.size() == 0 )
	{
		QwtPlotGrid *grid = new QwtPlotGrid;
		grid->enableXMin( true );
		grid->enableYMin( true );
		grid->setMajPen( QPen( Qt::black, 0, Qt::DotLine ) );
		grid->setMinPen( QPen( Qt::gray, 0, Qt::DotLine ) );
		grid->attach( this );
	}

	color.setAlpha( 127 );
	CHistogram *h = CreateHistogram( title, color, data, max_freq, bins );
	mHistograms.push_back( h );
	h->attach( this );
	mCurrentHistogram = h;
	return h;
}

CHistogram* C2DPlotWidget::PushHistogram( const std::string &title, QColor color, const C3DPlotInfo *data, double &max_freq, int bins )
{
	assert__( mSpectrograms.size() == 0 && mCurves.size() == 0 );

	if ( mHistograms.size() == 0 )
	{
		QwtPlotGrid *grid = new QwtPlotGrid;
		grid->enableXMin( true );
		grid->enableYMin( true );
		grid->setMajPen( QPen( Qt::black, 0, Qt::DotLine ) );
		grid->setMinPen( QPen( Qt::gray, 0, Qt::DotLine ) );
		grid->attach( this );
	}

	mCurrentHistogram = CreateHistogram( title, color, data, max_freq, bins );
	mHistograms.push_back( mCurrentHistogram );

	return mCurrentHistogram;
}


void C2DPlotWidget::SetCurrentHistogram( int index )
{
	assert__( size_t( index ) < mHistograms.size() );

	//if ( mHistograms[ index ] == mCurrentHistogram )
	//	return;

	mCurrentHistogram->detach();
	mCurrentHistogram = mHistograms[ index ];
	mCurrentHistogram->attach( this );

	double xMin, xMax, yMin, yMax;
	CurrentHistogramRanges( xMin, xMax, yMin, yMax );
	SetAxisScales( xMin, xMax, yMin, yMax );

	std::string xtitle, ytitle, ztitle;
	HistogramAxisTitles( index, xtitle, ytitle, ztitle );
	SetAxisTitles( index, xtitle, ytitle, ztitle );

	replot();
}




/////////////////////////////////////////////////////////////////////////////////////////
//									Curves
/////////////////////////////////////////////////////////////////////////////////////////

QwtPlotCurve* C2DPlotWidget::AddCurve( const std::string &title, QColor color, const CQwtArrayPlotData *data )	//data = nullptr 
{
	assert__( mSpectrograms.size() == 0 && mHistograms.size() == 0 );

    CGeneralizedCurve *c = new CGeneralizedCurve( data, title.c_str() );
	mCurves.push_back( c );
    c->setRenderHint( QwtPlotItem::RenderAntialiased );
    c->setPen( QPen( color ) );
    c->attach( this );

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
	assert__( size_t( curve ) < mCurves.size() );

	CGeneralizedCurve * pcurve = mCurves[ curve ];
    QwtSymbol c_symbol = pcurve->symbol();
    pcurve->setStyle( enable ? QwtPlotCurve::Lines : QwtPlotCurve::NoCurve );
	replot();
}



QColor C2DPlotWidget::CurveLineColor( int curve ) const
{
	assert__( size_t( curve ) < mCurves.size() );

	return mCurves[ curve ]->pen().color();
}

void C2DPlotWidget::SetCurveLineColor( int curve, QColor color )
{
	assert__( size_t( curve ) < mCurves.size() );

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
	assert__( size_t( curve ) < mCurves.size() );

	return mCurves[ curve ]->pen().color().alpha();
}

void C2DPlotWidget::SetCurveLineOpacity( int curve, int alpha )
{
	assert__( size_t( curve ) < mCurves.size() );

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
	assert__( size_t( curve ) < mCurves.size() );

	return pattern_cast< EStipplePattern >( mCurves[ curve ]->pen().style() );
}

void C2DPlotWidget::SetCurveLinePattern( int curve, EStipplePattern p )
{
	assert__( size_t( curve ) < mCurves.size() && p >= eFirstStipplePattern && p < EStipplePattern_size );

	CGeneralizedCurve *pcurve = mCurves[ curve ];	   		assert__( pcurve );

	QPen old_pen = pcurve->pen();
	old_pen.setStyle( pattern_cast<Qt::PenStyle >( p ) );
	pcurve->setPen( old_pen );
	replot();
}




int C2DPlotWidget::CurveLineWidth( int curve ) const
{
	assert__( size_t( curve ) < mCurves.size() );

	return mCurves[ curve ]->pen().width();
}

void C2DPlotWidget::SetCurveLineWidth( int curve, int pixels )
{
	assert__( size_t( curve ) < mCurves.size() );

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
	assert__( size_t( curve ) < mCurves.size() );

	CGeneralizedCurve *pcurve = mCurves[ curve ];	   		assert__( pcurve );

	QwtSymbol c_symbol = pcurve->symbol();

	c_symbol.setStyle( enable ? symbol_cast< QwtSymbol::Style >( default_symbol ) : QwtSymbol::NoSymbol );
	pcurve->setSymbol( c_symbol );

	replot();
}



QColor C2DPlotWidget::CurvePointColor( int curve ) const
{
	assert__( size_t( curve ) < mCurves.size() );

	return mCurves[ curve ]->symbol().pen().color();
}

void C2DPlotWidget::SetCurvePointColor( int curve, QColor color )
{
	assert__( size_t( curve ) < mCurves.size() );

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
	assert__( size_t( curve ) < mCurves.size() );

	return mCurves[ curve ]->symbol().brush() != Qt::NoBrush;
}

void C2DPlotWidget::SetCurvePointFilled( int curve, bool fill )
{
	assert__( size_t( curve ) < mCurves.size() );

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
	assert__( size_t( curve ) < mCurves.size() );

	return symbol_cast< EPointGlyph >( mCurves[ curve ]->symbol().style() );
}

void C2DPlotWidget::SetCurvePointGlyph( int curve, EPointGlyph symbol )
{
	assert__( size_t( curve ) < mCurves.size() );

	CGeneralizedCurve *pcurve = mCurves[ curve ];	   		assert__( pcurve );

    QwtSymbol c_symbol = pcurve->symbol();
    c_symbol.setStyle( symbol_cast< QwtSymbol::Style >( symbol) );
    pcurve->setSymbol( c_symbol );
	replot();
}



int C2DPlotWidget::CurvePointSize( int curve ) const
{
	assert__( size_t( curve ) < mCurves.size() );

	return mCurves[ curve ]->symbol().size().height();
}

void C2DPlotWidget::SetCurvePointSize( int curve, int pixels )
{
	assert__( size_t( curve ) < mCurves.size() );

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

class C2DMagnifier : public QwtPlotMagnifier
{
	using base_t = QwtPlotMagnifier;

	double mFactor = 1.;

public:
	explicit C2DMagnifier( QwtPlotCanvas *canvas )
		: base_t( canvas )
	{}

	virtual ~C2DMagnifier()
	{}

	void Home()		//works only if always notified through rescale of factor changes, which is not the case, unless linked to scale change signal
	{
		rescale( 1 / mFactor );
	}

protected:
	virtual void rescale( double factor )
	{
		if ( factor == 0.0 )
			return;

		mFactor *= factor;
		base_t::rescale( 1 / factor );
	}
};


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
	if ( mZoomer )			//NULL for curves and histograms, USED for rasters
	{
		mZoomer->Home();
		return;
	}

	//if ( mMagnifier )
	//{
	//	mMagnifier->Home();
	//	return;
	//}

	double xMin, xMax, yMin, yMax;
	if ( mCurves.size() > 0 )
	{
		CGeneralizedCurve *pcurve = mCurves[ 0 ];
		pcurve->Ranges( xMin, xMax, yMin, yMax );
	}
	else
	if ( mHistograms.size() > 0 )
	{	
		assert__( mCurrentHistogram );

		mCurrentHistogram->Ranges( xMin, xMax, yMin, yMax );
	}
	else
		return;

	SetAxisScales( xMin, xMax, yMin, yMax );
	replot();
}


C2DMagnifier* C2DPlotWidget::AddMagnifier()
{
	assert__( !mMagnifier );

	mMagnifier = new C2DMagnifier( canvas() );
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
