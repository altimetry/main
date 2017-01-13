/*
* This file is part of BRAT 
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "new-gui/brat/stdafx.h"

#include <QtSvg/QSvgGenerator>

#include <qwt_plot_grid.h>				//histogram
#include <qwt_interval_data.h>			//histogram
#include <qwt_plot_zoomer.h>			
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>

#include "new-gui/Common/QtUtils.h"
#include "DataModels/PlotData/PlotValues.h"

#include "Histogram.h"
#include "2DPlotWidget.h"


using CFitCurve = CPartialCurve< CQwtFitData >;


//virtual 
template< class DATA >
QwtDoubleRect CPartialCurve< DATA >::boundingRect() const
{
	if ( dataSize() <= 0 )
		return QwtDoubleRect( 1.0, 1.0, -2.0, -2.0 ); // Empty data.

	int first = 0;
	while ( first < dataSize() && ( isNaN( x( first ) ) || isNaN( y( first ) ) ) )
		++first;

	if ( first == dataSize() )
		return QwtDoubleRect( 1.0, 1.0, -2.0, -2.0 ); // Empty data.

	const_cast<CPartialCurve*>( this )->ComputeRange();

	return QwtDoubleRect( mMinXValue, mMinYValue, mMaxXValue - mMinXValue, mMaxYValue - mMinYValue );
}


// This is a slow implementation: it might be worth to cache valid data ranges.
//
//virtual 
template< class DATA >
void CPartialCurve< DATA >::draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to ) const
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
			base_t::draw( painter, xMap, yMap, first, last - 1 );
	}
}


template< class DATA >
void CPartialCurve< DATA >::ComputeRange()
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
//						Custom Curve
//////////////////////////////////////////////////////////////////

CGeneralizedCurve::CGeneralizedCurve( const CYFXValues *data )
	: base_t( data )
{}
CGeneralizedCurve::CGeneralizedCurve( const CYFXValues *data, const QwtText &title )
	: base_t( data, title )
{}
CGeneralizedCurve::CGeneralizedCurve( const CYFXValues *data, const QString &title )
	: base_t( data, title )
{}


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




//// QWT interface
//
//
//// This overload is needed when using auto-scale
////
////virtual 
//QwtDoubleRect CGeneralizedCurve::boundingRect() const
//{
//	if ( dataSize() <= 0 )
//		return QwtDoubleRect( 1.0, 1.0, -2.0, -2.0 ); // Empty data.
//
//	int first = 0;
//	while ( first < dataSize() && ( isNaN( x( first ) ) || isNaN( y( first ) ) ) )
//		++first;
//
//	if ( first == dataSize() )
//		return QwtDoubleRect( 1.0, 1.0, -2.0, -2.0 ); // Empty data.
//
//	const_cast<CGeneralizedCurve*>( this )->ComputeRange();
//
//	return QwtDoubleRect( mMinXValue, mMinYValue, mMaxXValue - mMinXValue, mMaxYValue - mMinYValue );
//}
//
//
//// This is a slow implementation: it might be worth to cache valid data ranges.
////
////virtual 
//void CGeneralizedCurve::draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to ) const
//{
//	if ( to < 0 )
//		to = dataSize() - 1;
//
//	int first, last = from;
//	while ( last <= to )
//	{
//		first = last;
//		while ( first <= to && ( isNaN( x( first ) ) || isNaN( y( first ) ) ) )
//			++first;
//		last = first;
//		while ( last <= to && !isNaN( x( last ) ) && !isNaN( y( last ) ) )
//			++last;
//		if ( first <= to )
//			QwtPlotCurve::draw( painter, xMap, yMap, first, last - 1 );
//	}
//}
//
//
//// internal processing
//
//void CGeneralizedCurve::ComputeRange()
//{
//	if ( mRangeComputed )
//		return;
//
//	int first = 0;
//	double minX, maxX, minY, maxY;
//	minX = maxX = x( first );
//	minY = maxY = y( first );
//	for ( int i = first + 1; i < dataSize(); ++i )
//	{
//		const double xv = x( i );
//		if ( xv < minX )
//			minX = xv;
//		if ( xv > maxX )
//			maxX = xv;
//		const double yv = y( i );
//		if ( yv < minY )
//			minY = yv;
//		if ( yv > maxY )
//			maxY = yv;
//	}
//
//	mMinXValue = minX;
//	mMaxXValue = maxX;
//
//	mMinYValue = minY;
//	mMaxYValue = maxY;
//
//	mRangeComputed = true;
//}






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

	double mXScaleFactor = 1.;
	double mYScaleFactor = 1.;
	double mZScaleFactor = 1.;

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

	void ScaleFactors( double &x, double &y, double &y2 ) const
	{
		x = mXScaleFactor;
		y = mYScaleFactor;
		y2 = mZScaleFactor;
	}

	void SetScaleFactors( double x, double y, double y2 )
	{
		mXScaleFactor = x;
		mYScaleFactor = y;
		mZScaleFactor = y2;
	}
};



//////////////////////////////////////////////////////////////////
//						Custom Scale
//////////////////////////////////////////////////////////////////

//choice of MAX_MANTISSA as 15 because
//the compiler generated code is lighter (and value, 0xF)

static const int MAX_MANTISSA = 15;

class CBratScaleDraw : public QwtScaleDraw
{
    brathl_refDate mDateRef = REF19500101;
    bool mIsDate = false;
    int mMantissaDigits;

public:
	CBratScaleDraw()
	{
		mMantissaDigits = 2 % MAX_MANTISSA;
	}
	virtual ~CBratScaleDraw()
	{}

    int GetMantissaDigits() const
    {
        return mMantissaDigits;
    }

    void SetMantissaDigits( int mantissa )
    {
        mMantissaDigits = mantissa;
		invalidateCache();
    }


	bool IsDate() const
	{
		return mIsDate;
	}

    void SetAsDate( bool isdate, brathl_refDate date_ref = REF19500101 )
	{
		mIsDate = isdate;
        mDateRef = date_ref;
		invalidateCache();
	}


	// QwtScaleDraw interface

	virtual QwtText label( double value ) const override
	{
		if ( mIsDate )
		{
            CDate d( value, mDateRef );
			if ( !d.IsDefaultValue() )
                return QString( d.AsString().c_str() );
		}

		return QString::number( value, 'g', mMantissaDigits );
	}
};






//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//						Main Class
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////





C2DPlotWidget::C2DPlotWidget( const CPlotsGraphicParameters &plots_graphic_parameters, QWidget *parent ) 
	: base_t( parent )
	, graphic_parameters_base_t( plots_graphic_parameters )
	, mIsLog( false )
{
	setWindowIcon( QPixmap( ":/2.png" ) );
	setWindowTitle( "[*]" );
	setAttribute( Qt::WA_DeleteOnClose );

	//same as 3D

	setFrameStyle( QFrame::Panel | QFrame::Sunken );
	setLineWidth(2);
	setMidLineWidth(0);

	QPalette pal = palette();
	pal.setColor( QPalette::Background, ViewsDefaultBackgroundColor() );
	setAutoFillBackground( true );
	setPalette( pal );

	setCanvasBackground( Qt::white );

	//

	AddLegend( RightLegend, true );

	connect( &mTimer, SIGNAL( timeout() ), this, SLOT( ChangeFrame() ) );


	QwtScaleWidget *x = axisWidget( xBottom );
	QwtScaleWidget *y = axisWidget( yLeft );
	QwtScaleWidget *y2 = axisWidget( yRight );

	connect( x, SIGNAL( scaleDivChanged() ), this, SLOT( HandleScaleDivChanged() ) );
	connect( y, SIGNAL( scaleDivChanged() ), this, SLOT( HandleScaleDivChanged() ) );
	connect( y2, SIGNAL( scaleDivChanged() ), this, SLOT( HandleScaleDivChanged() ) );


    CBratScaleDraw* 
	pDrawer = new CBratScaleDraw();
    pDrawer->SetMantissaDigits(2);
    setAxisScaleDraw( xBottom, pDrawer);
    pDrawer = new CBratScaleDraw();
    pDrawer->SetMantissaDigits(2);
    setAxisScaleDraw( yLeft, pDrawer);
    pDrawer = new CBratScaleDraw();
    pDrawer->SetMantissaDigits(2);
    setAxisScaleDraw( yRight, pDrawer);
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
	QFont font( t2q( mFontName ), mTitleFontSize );
	if ( mTitleBold )
		font.setBold( true );
	text.setFont( font );
	setTitle( text );
	//setWindowTitle( text );
}


/////////////////////////////////////////////////////////////////////////////////////////
//								Persistence (image only)
/////////////////////////////////////////////////////////////////////////////////////////

//static 
bool C2DPlotWidget::Save2unsupported( C2DPlotWidget *p, const QString &path, const QString &format )
{
	QPixmap pix = QPixmap::grabWidget( p );
	if ( pix.isNull() )
	{
		return false;
	}

	QString qpath = path;
	SetFileExtension( qpath, format );
	return pix.save( qpath, q2a( format ).c_str() );
}
//static 
bool C2DPlotWidget::Save2ps( C2DPlotWidget *p, const QString &path )
{
#if QT_VERSION >= 0x050000

    Q_UNUSED( p );      Q_UNUSED( path );

	return false;
#else
	QPrinter printer;
	//QPrinter printer(QPrinter::HighResolution);

	QString format = "ps";
	QString qpath = path;
	SetFileExtension( qpath, format );

//#if QT_VERSION < 0x040000
	//printer.setOutputToFile(true);
	printer.setOutputFileName( qpath );
	printer.setOutputFormat( QPrinter::PostScriptFormat );
	printer.setColorMode( QPrinter::Color );
//#else
//	printer.setOutputFileName("/tmp/bode.pdf");
//#endif

	QString docName = p->title().text();
	if ( !docName.isEmpty() )
	{
		docName.replace( QRegExp( QString::fromLatin1( "\n" ) ), tr( " -- " ) );
		printer.setDocName( docName );
	}

	printer.setCreator( "Brat v4.0.0" );
	printer.setOrientation( QPrinter::Landscape );

//#if QT_VERSION >= 0x040000
//	QPrintDialog dialog( &printer );
//	if ( dialog.exec() )
//	{
//#else
	//if (printer.setup())
	{
//#endif
		QwtPlotPrintFilter filter;
		if ( printer.colorMode() == QPrinter::GrayScale )
		{
			int options = QwtPlotPrintFilter::PrintAll;
			options &= ~QwtPlotPrintFilter::PrintBackground;
			options |= QwtPlotPrintFilter::PrintFrameWithScales;
			filter.setOptions( options );
		}
		p->print( printer, filter );
	}

	return true;

	//return Save2unsupported( p, path, "ps" );
#endif
}
//static 
bool C2DPlotWidget::Save2gif( C2DPlotWidget *p, const QString &path )
{
	return Save2unsupported( p, path, "gif" );
}
//static 
bool C2DPlotWidget::Save2svg( C2DPlotWidget *p, const QString &path )
{
	QString qpath = path;
	SetFileExtension( qpath, QString( "svg" ) );

	QPixmap pix = QPixmap::grabWidget( p );
	if ( pix.isNull() )
	{
		return false;
	}

	QSvgGenerator generator;
	generator.setFileName( qpath );
	generator.setSize( QSize( p->width(), p->height() ) );
	QwtPlotPrintFilter filter;
	filter.setOptions( 
		QwtPlotPrintFilter::PrintAll | 
		QwtPlotPrintFilter::PrintBackground | 
		QwtPlotPrintFilter::PrintFrameWithScales | 
		QwtPlotPrintFilter::PrintMargin 
		);
	QPainter painter( &generator );
	painter.drawPixmap( 0, 0, -1, -1, pix );	//pixmap.save( qpath, "SVG" );		//fails

	return true;
}

//static 
void C2DPlotWidget::Save2All( C2DPlotWidget *p, const QString &path )
{
	Save2ps( p, path );		//fails
	Save2gif( p, path );	//fails
	Save2svg( p, path ); 	//ok

	QPixmap pix = QPixmap::grabWidget( p );
	if ( pix.isNull() )
	{
		qDebug( "Failed to capture the plot for saving" );
	}

	QString qpath = path;

	SetFileExtension( qpath, QString( "jpg" ) );
	pix.save( qpath, "JPEG" );	//ok

	SetFileExtension( qpath, QString( "png" ) );
	pix.save( qpath, "PNG" );		//ok

	SetFileExtension( qpath, QString( "pnm" ) );
	pix.save( qpath, "PPM" );		//ok;	fails with "PNM" as format

	SetFileExtension( qpath, QString( "bmp" ) );
	pix.save( qpath, "BMP" );		//ok

	SetFileExtension( qpath, QString( "tif" ) );
	pix.save( qpath, "TIF" );		//ok
}
// Supported (and required) formats:
//
//enum EImageExportType
//{
//	eTif,
//	eBmp,
//	eJpg,
//	ePng,
//	ePnm,
//};
//
bool C2DPlotWidget::Save2Image( const QString &path, const QString &format, const QString &extension )
{
//#if defined (_DEBUG) || defined(DEBUG)
//	Save2All( this, path );
//#endif

	QString f = format.toLower();
	if ( f == "ps" )
		return Save2ps( this, path );

	QPixmap pix = QPixmap::grabWidget( this );
	if ( pix.isNull() )
	{
		return false;
	}

	QString qpath = path;
	SetFileExtension( qpath, extension );
	return pix.save( qpath, q2a( format ).c_str() );
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


//...titles

void C2DPlotWidget::CurrentAxisTitles( std::string *xtitle, std::string *ytitle, std::string *y2title ) const
{
	if ( xtitle )
		*xtitle = q2a( axisTitle( xBottom ).text() );
	if ( ytitle )
		*ytitle = q2a( axisTitle( yLeft ).text() );
	if ( y2title )
		*y2title = q2a( axisTitle( yRight ).text() );
}
void C2DPlotWidget::SetCurrentAxisTitles( const std::string &xtitle, const std::string &ytitle, const std::string &y2title )
{
	SetCurrentAxisTitle( xBottom, xtitle );
	SetCurrentAxisTitle( yLeft, ytitle );
	if ( !y2title.empty() )
		SetCurrentAxisTitle( yRight, y2title );
}
void C2DPlotWidget::SetCurrentAxisTitle( Axis axis, const std::string &title )
{
	QwtText text( t2q( title ) );
	QFont font( t2q( mFontName ), mAxisFontSize );
	if ( mAxisBold )
		font.setBold( true );
	text.setFont( font );
    setAxisTitle( axis, text );
}


//void C2DPlotWidget::SetAllAxisTitles( int index, const std::string &xtitle, const std::string &ytitle, const std::string &y2title )		//y2title = "" 
//{
//	if ( mCurves.size() > 0 )
//		mCurves[index]->SetAxisTitles( xtitle, ytitle, y2title );
//
//	if ( mHistograms.size() > 0 )
//		mHistograms[index]->SetAxisTitles( xtitle, ytitle, y2title );
//
//	if ( mSpectrograms.size() > 0 )
//		mSpectrograms[index]->SetAxisTitles( xtitle, ytitle, y2title );
//
//	SetCurrentAxisTitles( xtitle, ytitle, y2title );
//}


void C2DPlotWidget::SetAllCurvesAxisTitles( const std::string &xtitle, const std::string &ytitle )
{
	for ( auto *curve : mCurves )
		curve->SetAxisTitles( xtitle, ytitle, "" );

	SetCurrentAxisTitles( xtitle, ytitle, "" );
}


void C2DPlotWidget::SpectrogramAxisTitles( int index, std::string &xtitle, std::string &ytitle, std::string &y2title ) const
{
	assert__( index < (int)mSpectrograms.size() );

	mSpectrograms[ index ]->AxisTitles( xtitle, ytitle, y2title );
}
void C2DPlotWidget::SetSpectrogramAxisTitles( int index, const std::string &xtitle, const std::string &ytitle, const std::string &y2title )
{
	assert__( index < (int)mSpectrograms.size() );

	mSpectrograms[index]->SetAxisTitles( xtitle, ytitle, y2title );

	SetCurrentAxisTitles( xtitle, ytitle, y2title );
}




//...digits / date

int C2DPlotWidget::XAxisDigits() const
{
	const CBratScaleDraw *xdrawer = dynamic_cast<const CBratScaleDraw*>( axisScaleDraw( xBottom ) );	assert__( xdrawer );
	return xdrawer->GetMantissaDigits();
}

int C2DPlotWidget::YAxisDigits() const
{
	const CBratScaleDraw *ydrawer = dynamic_cast<const CBratScaleDraw*>( axisScaleDraw( yLeft ) );		assert__( ydrawer );
	return ydrawer->GetMantissaDigits();
}

int C2DPlotWidget::Y2AxisDigits() const
{
	const CBratScaleDraw *ydrawer = dynamic_cast<const CBratScaleDraw*>( axisScaleDraw( yRight ) );		assert__( ydrawer );
	return ydrawer->GetMantissaDigits();
}


bool C2DPlotWidget::XAxisIsDateTime() const
{
    const CBratScaleDraw *xdrawer = dynamic_cast<const CBratScaleDraw*>( axisScaleDraw( xBottom ) );	assert__( xdrawer );
    return xdrawer->IsDate();
}
bool C2DPlotWidget::YAxisIsDateTime() const
{
    const CBratScaleDraw *ydrawer = dynamic_cast<const CBratScaleDraw*>( axisScaleDraw( yLeft ) );		assert__( ydrawer );
    return ydrawer->IsDate();
}
bool C2DPlotWidget::Y2AxisIsDateTime() const
{
    const CBratScaleDraw *y2drawer = dynamic_cast<const CBratScaleDraw*>( axisScaleDraw( yRight ) );	assert__( y2drawer );
    return y2drawer->IsDate();
}

void C2DPlotWidget::SetDigits( Axis axisId, bool isdate, int digits, brathl_refDate date_ref )
{
	CBratScaleDraw *drawer = dynamic_cast<CBratScaleDraw*>( axisScaleDraw( axisId ) );			assert__( drawer );
    if (digits <= 0)
        digits = drawer->GetMantissaDigits();
    drawer->SetMantissaDigits( digits );
    drawer->SetAsDate( isdate, date_ref );
	axisWidget( axisId )->update();
}

void C2DPlotWidget::SetXAxisDigits( bool isdate, int digits, brathl_refDate date_ref )		//date_ref = REF19500101
{
	static const auto axisId = xBottom;

	SetDigits( axisId, isdate, digits, date_ref );
}

void C2DPlotWidget::SetYAxisDigits( bool isdate, int digits, brathl_refDate date_ref )		//date_ref = REF19500101
{
	static const auto axisId = yLeft;

	SetDigits( axisId, isdate, digits, date_ref );
}

void C2DPlotWidget::SetY2AxisDigits( bool isdate, int digits, brathl_refDate date_ref )		//date_ref = REF19500101 
{
	static const auto axisId = yRight;

	SetDigits( axisId, isdate, digits, date_ref );
}



//////////
//...scale
//////////

// widget axis only (only manipulate widget, not data)

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
void C2DPlotWidget::SetAxisScales( double xMin, double xMax, double yMin, double yMax, double y2Min, double y2Max, bool plot )   //y2Min = defaultValue<double>(), double y2Max = defaultValue<double>(), bool plot = true
{
	setAxisScale( xBottom, xMin, xMax );
	setAxisScale( yLeft, yMin, yMax  );
	if ( !isDefaultValue( y2Min ) && !isDefaultValue( y2Max ) )
		setAxisScale( yRight, y2Min, y2Max  );

	updateAxes();		//assigns/updates axis scaleDiv

	if (plot) replot();
}


// scale data ranges

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
void C2DPlotWidget::AxisRanges( double &xMin, double &xMax, double &yMin, double &yMax, double &y2Min, double &y2Max  )
{
	if ( mSpectrograms.size() > 0 )
	{	
		assert__( mCurrentSpectrogram );

		CurrentSpectrogramRanges( xMin, xMax, yMin, yMax, y2Min, y2Max );
	}
	else
	{
		AxisScales( xMin, xMax, yMin, yMax, y2Min, y2Max );		//for y2

		if ( mCurves.size() > 0 )
		{
			CGeneralizedCurve *pcurve = mCurves[ 0 ];
			pcurve->Ranges( xMin, xMax, yMin, yMax );
		}
		else
		if ( mHistograms.size() > 0 )
		{
			assert__( mCurrentHistogram );

			CurrentHistogramRanges( xMin, xMax, yMin, yMax );
		}
	}
}


void C2DPlotWidget::SetPlotAxisRanges( int index, double xMin, double xMax, double yMin, double yMax, double y2Min, double y2Max, bool plot )	//y2Min = defaultValue<double>(), double y2Max = defaultValue<double>(), bool plot )	//plot = true 
{
	for ( auto *curve : mCurves )
		curve->SetRanges( xMin, xMax, yMin, yMax );

	if ( mHistograms.size() > 0 )
		mHistograms[index]->SetRanges( xMin, xMax, yMin, yMax );

	if ( mSpectrograms.size() > 0 )
		mSpectrograms[index]->SetRanges( xMin, xMax, yMin, yMax );

	SetAxisScales( xMin, xMax, yMin, yMax, y2Min, y2Max, plot );
}

void C2DPlotWidget::RescaleX( double x, bool plot )	//plot = true 
{
	double xMin, xMax, yMin, yMax, y2Min, y2Max;
	AxisRanges( xMin, xMax, yMin, yMax, y2Min, y2Max );

	const QwtScaleDiv *scaleDiv = axisScaleDiv( QwtPlot::xBottom );

	const double range = xMax - xMin;
	const double center = scaleDiv->lowerBound() + scaleDiv->range() / 2;	//this will 0-recenter axis: xMin + range / 2;
	const double width = range / 2 * ( 1 / x );

	setAxisScale( xBottom, center - width, center + width );
	if ( plot ) replot();
}
void C2DPlotWidget::RescaleY( double y, bool plot )
{
	double xMin, xMax, yMin, yMax, y2Min, y2Max;
	AxisRanges( xMin, xMax, yMin, yMax, y2Min, y2Max );

	const QwtScaleDiv *scaleDiv = axisScaleDiv( QwtPlot::yLeft );

	const double range = yMax - yMin;
	const double center = scaleDiv->lowerBound() + scaleDiv->range() / 2;	//this will 0-recenter axis: yMin + range / 2;
	const double width = range / 2 * ( 1 / y );

	setAxisScale( yLeft, center - width, center + width );
	if ( plot ) replot();
}


// scale auto notification handler

void C2DPlotWidget::HandleScaleDivChanged()
{
	QwtScaleWidget *axis = qobject_cast<QwtScaleWidget*>( sender() );
	if ( !axis )
		return;

	double xMin, xMax, yMin, yMax, y2Min, y2Max;
	AxisRanges( xMin, xMax, yMin, yMax, y2Min, y2Max );

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

	if ( mCurrentHistogram )
		mCurrentHistogram->SetScaleFactors( mXScaleFactor, mYScaleFactor, mY2ScaleFactor );
	if ( mCurrentSpectrogram )
		mCurrentSpectrogram->SetScaleFactors( mXScaleFactor, mYScaleFactor, mY2ScaleFactor );

	emit ScaleDivChanged( iaxis, *pscale_factor, msg.c_str() );
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


QwtPlotSpectrogram* C2DPlotWidget::PushRaster( const std::string &title, const CZFXYValues &maps, double min_contour, double max_contour, size_t ncontours, const QwtColorMap &color_map )
{
    assert__( mCurves.size() == 0 );

    mSpectrograms.push_back( new CBratSpectrogram );
	if ( mCurrentSpectrogram )
		mCurrentSpectrogram->detach();
	mCurrentSpectrogram = mSpectrograms.back();

	const CZFXYValues::value_type &map = maps[ 0 ];
    mCurrentSpectrogram->setData( maps );
	mCurrentSpectrogram->attach( this );
    //QVector<double> vmatrix;
    //for ( double d : map.mValues )
    //    vmatrix.append(d);
    //const_cast<CZFXYValues&>( maps ).setValueMatrix( vmatrix, 1 );
	//const_cast<CZFXYValues&>( maps ).setBoundingRect( QwtDoubleRect( 0, 0, 10*(map.mMaxX - map.mMinX), 10*(map.mMaxY - map.mMinY )) );

    // Color map

	////////////////QwtLinearColorMap color_map( Qt::darkCyan, Qt::red );
	////////////////color_map.addColorStop( 0.1, Qt::cyan );
	////////////////color_map.addColorStop( 0.4, Qt::green );
	////////////////color_map.addColorStop( 0.90, Qt::yellow );
	mCurrentSpectrogram->setColorMap( color_map );

    // Axis, scales (do this after color map or prevent re-plotting)

	EnableAxisY2();
	SetPlotAxisRanges( (int)mSpectrograms.size() - 1, map.mMinX, map.mMaxX, map.mMinY, map.mMaxY, 
		mCurrentSpectrogram->data().range().minValue(), mCurrentSpectrogram->data().range().maxValue(), false );

	//...color bar on the right axis
	QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
    rightAxis->setTitle( title.c_str() );
	rightAxis->setColorBarEnabled( true );
	rightAxis->setColorMap( mCurrentSpectrogram->data().range(), mCurrentSpectrogram->colorMap() );

	// Contour levels

	SetNumberOfContours( min_contour, max_contour, ncontours );
	
    // Avoid jumping when labels with more/less digits appear/disappear when scrolling vertically

	const QFontMetrics fm( axisWidget( QwtPlot::yLeft )->font() );
    QwtScaleDraw *sd = axisScaleDraw( QwtPlot::yLeft );
	sd->setMinimumExtent( fm.width( "100.00" ) );

	replot();

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

	double x, y, z;
	mCurrentSpectrogram->ScaleFactors( x, y, z );

	SetAxisScales( xMin, xMax, yMin, yMax, y2Min, y2Max, false );	//changes both this and plot scale factors (to 1.)
	RescaleX( x, false );
	RescaleY( y, false  );

	std::string xtitle, ytitle, ztitle;
	SpectrogramAxisTitles( index, xtitle, ytitle, ztitle );
	SetCurrentAxisTitles( xtitle, ytitle, ztitle );

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


int C2DPlotWidget::NumberOfContours() const
{
	assert__( mCurrentSpectrogram );

	return mCurrentSpectrogram->contourLevels().size();
}
void C2DPlotWidget::SetNumberOfContours( double min_contour, double max_contour, size_t ncontours )
{
	assert__( mCurrentSpectrogram );

	if ( !isDefaultValue( ncontours ) )
	{
		if ( !isDefaultValue( min_contour ) && !isDefaultValue( max_contour ) )
		{
			QwtValueList contour_levels;
			double step = ( max_contour - min_contour ) / ncontours;						assert__( step > 0 );
			if ( step > 0 )
				for ( double level = min_contour; level < max_contour; level += step )
				{
					contour_levels += level;
				}
			mCurrentSpectrogram->setContourLevels( contour_levels );
			plotLayout()->setAlignCanvasToScales( true );
			replot();
		}
	}
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


//static 
const std::string C2DPlotWidget::smHistogramXtitle = "Values";
//static 
const std::string C2DPlotWidget::smHistogramYtitle = "Frequency";


template< typename DATA >
CHistogram* C2DPlotWidget::CreateHistogram( const std::string &title, QColor color, const DATA &data, double &max_freq, int bins, 
	bool isdate, brathl_refDate date_ref )		//brathl_refDate date_ref = REF19500101 
{
	if ( mHistograms.size() == 0 )
	{
		QwtPlotGrid *grid = new QwtPlotGrid;
		grid->enableXMin( true );
		grid->enableYMin( true );
		grid->setMajPen( QPen( Qt::black, 0, Qt::DotLine ) );
		grid->setMinPen( QPen( Qt::gray, 0, Qt::DotLine ) );
		grid->attach( this );
	}

	CHistogram *h = new CHistogram;

	h->setColor( color );
	h->setTitle( t2q( title ) );

	//size_t data_size = data->GetDataSize( 0 );
	double m, M;
	data.GetDataRange( m, M, 0 );

	QwtArray< QwtDoubleInterval > intervals( bins );
	QwtArray< double > values( bins );

    const double width = ( M - m ) / double( bins );
	double b0 = m;
	max_freq = std::numeric_limits<double>::lowest();
	for ( int i = 0; i < (int)intervals.size(); i++ )
	{
		double b1 = b0 + width;

	    intervals[i] = QwtDoubleInterval( b0, b1 );

		values[ i ] = data.GetDataCountIf( 0, [&b0, &b1]( const double &v )
		{
			return !isDefaultValue( v ) && v >= b0 && v < b1;
		}
		);

		max_freq = std::max( max_freq, values[ i ] );
	    b0 = b1;
	}

	//SetAxisScales( m, b0, 0, max_value );

	h->setData( QwtIntervalData( intervals, values ) );

	h->SetAxisTitles( smHistogramXtitle, smHistogramYtitle, "" );		//histograms do not allow custom titles.
	SetCurrentAxisTitles( smHistogramXtitle, smHistogramYtitle, "" );

	SetXAxisDigits( isdate, XAxisDigits(), date_ref );
	SetYAxisDigits( false, YAxisDigits() );

	return h;
}



CHistogram* C2DPlotWidget::PushHistogram( const std::string &title, QColor color, const CYFXValues &data, double &max_freq, int bins, 
	bool isdate, brathl_refDate date_ref )		//brathl_refDate date_ref = REF19500101 
{
	assert__( mSpectrograms.size() == 0 && mCurves.size() == 0 );

	color.setAlpha( 127 );
	mCurrentHistogram = CreateHistogram( title, color, data, max_freq, bins, isdate, date_ref );
	mHistograms.push_back( mCurrentHistogram );
	mCurrentHistogram->attach( this );

	return mCurrentHistogram;
}

CHistogram* C2DPlotWidget::SetSingleHistogram( const std::string &title, QColor color, const CZFXYValues &data, double &max_freq, int bins, 
	bool isdate, brathl_refDate date_ref )		//brathl_refDate date_ref = REF19500101 
{
	assert__( mSpectrograms.size() == 0 && mCurves.size() == 0 );

	mCurrentHistogram = CreateHistogram( title, color, data, max_freq, bins, isdate, date_ref );
	mHistograms.push_back( mCurrentHistogram );
	return mCurrentHistogram;
}


void C2DPlotWidget::SetCurrentHistogram( int index )
{
	assert__( size_t( index ) < mHistograms.size() );

	mCurrentHistogram->detach();
	mCurrentHistogram = mHistograms[ index ];
	mCurrentHistogram->attach( this );

	double xMin, xMax, yMin, yMax;
	CurrentHistogramRanges( xMin, xMax, yMin, yMax );
	double x, y, z;
	mCurrentHistogram->ScaleFactors( x, y, z );

	SetAxisScales( xMin, xMax, yMin, yMax );	//changes both this and plot scale factors (to 1.)
	RescaleX( x );
	RescaleY( y );

	SetCurrentAxisTitles( smHistogramXtitle, smHistogramYtitle, "" );

	replot();
}




/////////////////////////////////////////////////////////////////////////////////////////
//									Curves
/////////////////////////////////////////////////////////////////////////////////////////

void C2DPlotWidget::ShowFitCurve( QwtPlotItem *item, bool on )
{
	if ( dynamic_cast< CFitCurve* >( item ) )
		item->setVisible( on );
	else
	if (!on)
	{
		QWidget *w = legend()->find( item );
		if ( w && w->inherits( "QwtLegendItem" ) )
			( (QwtLegendItem *)w )->setChecked( true );
	}

	replot();
}




QwtPlotCurve* C2DPlotWidget::PushCurve( const std::string &title, QColor color, const CYFXValues *data )	//data = nullptr 
{
	assert__( mSpectrograms.size() == 0 && mHistograms.size() == 0 );

    CGeneralizedCurve *c = new CGeneralizedCurve( data, title.c_str() );
	mCurves.push_back( c );
    c->setRenderHint( QwtPlotItem::RenderAntialiased );
    c->setPen( QPen( color ) );
    c->attach( this );
	c->setVisible( true );
	QwtLegendItem* curve_legend = (QwtLegendItem*)mLegend->find( c );	//label->setItemMode(QwtLegend::CheckableItem);
	curve_legend->setChecked(true);


	//Linear Regression

	CQwtFitData *cfit_data = new CQwtFitData( *data );
	if ( !cfit_data->viable() )
	{
		delete cfit_data;
	}
	else
	{
		CFitCurve *cfit = new CFitCurve( cfit_data, ( "linear fit - " + n2s<std::string>( mCurves.size() ) ).c_str() );
		cfit->setData( *cfit_data );
		cfit->setPen( QPen( QColor( Qt::darkCyan ) ) );
		cfit->attach( this );
		cfit->setVisible( false );
		QwtLegendItem *fit_legend = (QwtLegendItem*)mLegend->find( cfit );		//label->setItemMode(QwtLegend::CheckableItem);
		fit_legend->setToolTip( ( title + " - linear regression\n" + "y = " + n2s<std::string>( cfit_data->yintersect() ) + " + " + n2s<std::string>( cfit_data->slope() ) + " x" ).c_str() );
	}

	return c;
}
//QwtPlotCurve* C2DPlotWidget::PushCurve( const QwtData &data, const std::string &title, QColor color )	//for experimental samples
//{
//	assert__( mSpectrograms.size() == 0 && mHistograms.size() == 0 );
//
//    QwtPlotCurve *c = new QwtPlotCurve( title.c_str() );
//    c->setRenderHint(QwtPlotItem::RenderAntialiased);
//    c->setPen( QPen( color ) );
//    c->attach( this );
//    c->setData( data );
//	return c;
//}


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

	//see CurvePointGlyph before changing

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



EPointGlyph C2DPlotWidget::CurvePointGlyph( int curve, EPointGlyph default_symbol ) const
{
	assert__( size_t( curve ) < mCurves.size() );

	//see EnableCurvePoints: when points not enabled symbol changes to QwtSymbol::NoSymbol

	if ( mCurves[ curve ]->symbol().style() == QwtSymbol::NoSymbol )
		return default_symbol;

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
	explicit C2DMagnifier( QwtPlotCanvas *canvas, bool enable_x, bool enable_y, bool enable_y2 )
		: base_t( canvas )
	{
		setAxisEnabled( QwtPlot::xBottom, enable_x );
		setAxisEnabled( QwtPlot::yLeft, enable_y );
		setAxisEnabled( QwtPlot::yRight, enable_y2 );
	}

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


C2DMagnifier* C2DPlotWidget::AddMagnifier( bool enable_x, bool enable_y, bool enable_y2 )	//enable_x = true, bool enable_y = true, bool enable_y2 = true );
{
	assert__( !mMagnifier );

	mMagnifier = new C2DMagnifier( canvas(), enable_x, enable_y, enable_y2 );
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


void C2DPlotWidget::AddLegend( LegendPosition pos, bool checkable )		//checkable = false 
{
	mLegend  = new QwtLegend;
    insertLegend( mLegend , pos );
	mLegend ->setDisplayPolicy( QwtLegend::AutoIdentifier, 0 );
	if ( checkable )
	{
		mLegend ->setItemMode( QwtLegend::CheckableItem );
		connect( this, SIGNAL( legendChecked( QwtPlotItem *, bool ) ), SLOT( ShowFitCurve( QwtPlotItem *, bool ) ) );
	}
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
