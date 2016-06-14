#include "new-gui/brat/stdafx.h"

#include <QtGui>
#include <QColor>

#include <qwt3d_plot.h>
#include <qwt3d_function.h>


#include "libbrathl/Date.h"
using namespace brathl;

#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/+UtilsIO.h"
#include "DataModels/PlotData/PlotValues.h"

#include "GUI/ActionsTable.h"

#include "3DPlotWidget.h"


// TODO delete this macro; if active, no operation is expected to work besides the simple example display
//#define TEST_EXAMPLES


//////////////////////////////////////////
//
//		Auxiliary Qwt3D Classes
//
//			- color
//			- function
//			- scale
//			- plot
//
//////////////////////////////////////////

//////////////////////////////////////////
//	color
//////////////////////////////////////////

class CBratColor : public Qwt3D::StandardColor
{
	// types

	using base_t = Qwt3D::StandardColor;

	//static members

	static double interpolate( double val, double y0, double x0, double y1, double x1 )
	{
		return ( val - x0 )*( y1 - y0 ) / ( x1 - x0 ) + y0;
	}

	static double blue( double grayscale )
	{
		if ( grayscale < -0.33 ) return 1.0;
		else if ( grayscale < 0.33 ) return interpolate( grayscale, 1.0, -0.33, 0.0, 0.33 );
		else return 0.0;
	}

	static double green( double grayscale )
	{
		if ( grayscale < -1.0 ) return 0.0; // unexpected grayscale value
		if ( grayscale < -0.33 ) return interpolate( grayscale, 0.0, -1.0, 1.0, -0.33 );
		else if ( grayscale < 0.33 ) return 1.0;
		else if ( grayscale <= 1.0 ) return interpolate( grayscale, 1.0, 0.33, 0.0, 1.0 );
		else return 1.0; // unexpected grayscale value
	}

	static double red( double grayscale )
	{
		if ( grayscale < -0.33 ) return 0.0;
		else if ( grayscale < 0.33 ) return interpolate( grayscale, 0.0, -0.33, 1.0, 0.33 );
		else return 1.0;
	}

	//construction / destruction

public:
#if (QWT3D_MINOR_VERSION > 2)
	CBratColor( Qwt3D::Curve *data, unsigned size = 100 ) 
#else
	CBratColor( Qwt3D::Plot3D *data, unsigned size = 100 ) 
#endif
		: base_t( data, size )
	{
		reset(size);
	}

	virtual ~CBratColor()
	{}

	void reset( unsigned size )
	{
		colors_ = Qwt3D::ColorVector( size );
		Qwt3D::RGBA elem;

		double dsize = size;

		for ( unsigned int i=0; i != size; ++i )
		{
			elem.r = red( i / dsize );
			elem.g = green( i / dsize );
			elem.b = blue( i / dsize );
			elem.a = 1.0;
			colors_[ i ] = elem;
		}
	}
};



//////////////////////////////////////////
//			function
//////////////////////////////////////////

struct CBrat3DFunction : public Qwt3D::Function
{
	const C3DPlotParameters *mPlotValues = nullptr;

	double mxmin;
	double mxmax;
	double mymin;
	double mymax;
	double mzmin;
	double mzmax;

	bool mLogarithmic = false;

#if (QWT3D_MINOR_VERSION > 2)
	CBrat3DFunction( Qwt3D::Curve& pw ) 
#else
	CBrat3DFunction( SurfacePlot& pw ) 
#endif
		: Qwt3D::Function( pw )
	{}
	virtual ~CBrat3DFunction()
	{}


	inline double CorrectedValue( double value )
	{
		if ( !mLogarithmic )
			return value;

		return value == 0 ? 0 : log( value );
	}


	void AddSurface( const C3DPlotParameters &values, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax )
	{
		assert__( values.mValues.size() == values.mBits.size() );
		assert__( values.mValues.size() == values.mXaxis.size() * values.mYaxis.size() );

		mPlotValues = &values;
		mxmin = xmin;
		mxmax = xmax;
		mymin = ymin;
		mymax = ymax;

		setMesh( (int)mPlotValues->mXaxis.size(), (int)mPlotValues->mYaxis.size() );		//setMesh( xmax, ymax );
		setDomain( xmin, xmax, ymin, ymax );

		SetZRange( zmin, zmax );
	}


	void SetZRange( double zmin, double zmax )
	{
		mzmin = zmin;
		mzmax = zmax;

		SetZRange();
	}


	void SetZRange()
	{
		setMinZ( CorrectedValue( mzmin ) );
		setMaxZ( CorrectedValue( mzmax ) );

		create();
	}


	void SetLogarithmic( bool log )
	{
		if ( mLogarithmic == log )
			return;

		mLogarithmic = log;

		SetZRange();
	}


    double operator()( double x, double y )
    {
        return CorrectedValue( mPlotValues->value( x, y ) );
    }
};




//////////////////////////////////////////
//				Scale
//////////////////////////////////////////



class CBrat3DScale : public Qwt3D::LinearScale
{
	//types

	using base_t = Qwt3D::LinearScale;

	//statics

public:
	static const int smDigits = 2;


protected:
	//instance data

	bool mIsDate = false;
    brathl_refDate mDateRef = REF19500101;
    bool mIsLog = false;
	int mDigits = smDigits;

	//construction / destruction

public:
	CBrat3DScale()
		: base_t()
	{}
	virtual ~CBrat3DScale()
	{}

    // getters / setters / testers

    //date scale

    bool IsDate() const { return mIsDate; }
    void SetDate( bool date, brathl_refDate date_ref = REF19500101 )
	{ 
		mIsDate = date; 
        if (mIsDate)
        {
            mIsLog = false;
            mDateRef = date_ref;
        }
	}

    //log scale

    bool IsLog() const { return mIsLog; }
    void SetLog( bool log ) 
	{ 
		mIsLog = log; 
		if (mIsLog) mIsDate = false;
	}


    //Number of digits on the scientific representation of the scale

    unsigned int GetNDigits() const { return mDigits; }
    void SetNDigits( int digits = -1 )
	{ 
		mDigits = digits <= 0 ? smDigits : digits; 
	}

protected:

	//qwt3d interface

	virtual Scale* clone() const override
	{
		CBrat3DScale * new_nb = new CBrat3DScale;
		new_nb->SetDate( mIsDate );
		new_nb->SetLog( mIsLog );
		new_nb->SetNDigits( mDigits );
		return new_nb;
	}

	virtual QString ticLabel( unsigned int idx ) const override
	{
		double scale_log = 0;
		if ( idx < majors_p.size() )
		{
			if ( mIsLog )
			{
				scale_log = pow( 10., majors_p[ idx ] );
				return QString::number( scale_log, 'g', mDigits );
			}
			else
			if ( mIsDate )
			{
				auto value = majors_p[ idx ];
                CDate d( value, mDateRef );
				if ( !d.IsDefaultValue() )
					return QString( d.AsString().c_str() );
			}

			return QString::number( majors_p[ idx ], 'g', mDigits );
		}

		return QString( "" );
	}
};





//////////////////////////////////////////
//			Qwt3D plot class
//////////////////////////////////////////

void CBrat3DPlot::CreateContextMenu()
{
	setContextMenuPolicy( Qt::ActionsContextMenu );

    mAnimateAction = new QAction( "Animate", this );
	mAnimateAction->setCheckable( true );
	mAnimateAction->setChecked( false );

    mStandardColorAction = new QAction( "Standard Color", this );
	mStandardColorAction->setCheckable( true );
	mStandardColorAction->setChecked( true );

    mHomeAction = new QAction( "Reset", this );

	mStyleGroup = new QActionGroup( this );
	mStyleGroup->setExclusive( true );
	QList<QAction*> style_actions_list;

	auto create_style_item = [ this, &style_actions_list ]( const std::string &name, const std::string tip, Qwt3D::PLOTSTYLE style )
	{
		auto a = new QAction( name.c_str(), mStyleGroup );
		a->setCheckable( true );
#if (QWT3D_MINOR_VERSION > 2)
		a->setChecked( curve()->plotStyle() == style );
#else
		a->setChecked( plotStyle() == style );
#endif
		a->setToolTip( tip.c_str() );
		a->setData( style );
		style_actions_list << a;
		connect( a, SIGNAL( triggered() ), this, SLOT( PlotStyle() ) );
	};
	create_style_item( "Qwt3D::NOPLOT", "No visible data", Qwt3D::NOPLOT );
	create_style_item( "Qwt3D::WIREFRAME", "Wire-frame style", Qwt3D::WIREFRAME );
	create_style_item( "Qwt3D::HIDDENLINE", "Hidden Line style", Qwt3D::HIDDENLINE );
	create_style_item( "Qwt3D::FILLED", "Color filled polygons w/o edges", Qwt3D::FILLED );
	create_style_item( "Qwt3D::FILLEDMESH", "Color filled polygons w/ separately colored edges", Qwt3D::FILLEDMESH );

	mCoordinateStyleGroup = new QActionGroup( this );
	mStyleGroup->setExclusive( true );

	QList<QAction*> coord_style_actions_list;
	auto create_coordinate_style_item = [ this, &coord_style_actions_list ]( const std::string &name, const std::string tip, Qwt3D::COORDSTYLE style )
	{
		auto a = new QAction( name.c_str(), mCoordinateStyleGroup );
		a->setCheckable( true );
		a->setChecked( coordinates()->style() == style );
		a->setToolTip( tip.c_str() );
		a->setData( style );
		coord_style_actions_list << a;
		connect( a, SIGNAL( triggered() ), this, SLOT( CoordinateStyle() ) );
	};
	create_coordinate_style_item( "Qwt3D::NOCOORD", "Coordinate system is not visible", Qwt3D::NOCOORD );
	create_coordinate_style_item( "Qwt3D::BOX", "Boxed", Qwt3D::BOX );
	create_coordinate_style_item( "Qwt3D::FRAME", "Frame - 3 visible axes", Qwt3D::FRAME );

    addAction( mAnimateAction );
	addAction( mStandardColorAction );
    addAction( CActionInfo::CreateAction( this, eAction_Separator) );
    addAction( mHomeAction );
    addAction( CActionInfo::CreateAction( this, eAction_Separator) );
	addActions( style_actions_list );
    addAction( CActionInfo::CreateAction( this, eAction_Separator) );
	addActions( coord_style_actions_list );

    connect( mAnimateAction,		SIGNAL(toggled(bool)),	this, SLOT( Animate() ) );
    connect( mStandardColorAction,	SIGNAL(toggled(bool)),	this, SLOT( StandardColor(bool) ) );
    connect( mHomeAction,			SIGNAL(triggered()),	this, SLOT( Reset() ) );
}



//static 
const std::vector< Qwt3D::AXIS > CBrat3DPlot::smXaxis = { Qwt3D::AXIS::X1, Qwt3D::AXIS::X2, Qwt3D::AXIS::X3, Qwt3D::AXIS::X4 };
//static 
const std::vector< Qwt3D::AXIS > CBrat3DPlot::smYaxis = { Qwt3D::AXIS::Y1, Qwt3D::AXIS::Y2, Qwt3D::AXIS::Y3, Qwt3D::AXIS::Y4 };
//static 
const std::vector< Qwt3D::AXIS > CBrat3DPlot::smZaxis = { Qwt3D::AXIS::Z1, Qwt3D::AXIS::Z2, Qwt3D::AXIS::Z3, Qwt3D::AXIS::Z4 };





CBrat3DPlot::CBrat3DPlot( QWidget *pw ) 
	: base_t( pw )
	, mTimer( this )
	, mXDigits( CBrat3DScale::smDigits )
	, mYDigits( CBrat3DScale::smDigits )
	, mZDigits( CBrat3DScale::smDigits )
{
	connect( &mTimer, SIGNAL( timeout() ), this, SLOT( rotate() ) );

	CreateContextMenu();
	Reset();

	setSmoothMesh( true );
	enableLighting( false );

	coordinates()->setNumberFont( t2q( C3DPlotWidget::smFontName ), C3DPlotWidget::smAxisFontSize );
	coordinates()->setLabelFont( t2q( C3DPlotWidget::smFontName ), C3DPlotWidget::smAxisFontSize );

	const size_t size = coordinates()->axes.size();
	for ( unsigned i = 0; i != size; ++i )
	{
		coordinates()->axes[ i ].setMajors( 7 );
		coordinates()->axes[ i ].setMinors( 4 );
	}
}

//virtual 
CBrat3DPlot::~CBrat3DPlot()
{
	DestroyPointersAndContainer( mFunctions );
}


void CBrat3DPlot::SetAxisTitle( Qwt3D::AXIS axis, const std::string &title )
{
	std::string clean_title = replace( title, "\n", " - " );
	clean_title = replace( clean_title, "\t", "" );

	coordinates()->axes[ axis ].setLabelString( clean_title.c_str() );
}


unsigned int CBrat3DPlot::XAxisNbTicks() const
{
	return const_cast<CBrat3DPlot*>( this )->coordinates()->axes[ Qwt3D::AXIS::X1 ].majors();
}
void CBrat3DPlot::SetXAxisTicks( unsigned int nticks )
{
    for ( auto axis : smXaxis )
    {
        coordinates()->axes[ axis ].setMajors( nticks );
    }
    updateGL();
}


unsigned int CBrat3DPlot::YAxisNbTicks() const
{
	return const_cast<CBrat3DPlot*>( this )->coordinates()->axes[ Qwt3D::AXIS::Y1 ].majors();
}
void CBrat3DPlot::SetYAxisTicks( unsigned int nticks )
{
    for ( auto axis : smYaxis )
    {
        coordinates()->axes[ axis ].setMajors( nticks );
    }
    updateGL();
}


unsigned int CBrat3DPlot::ZAxisNbTicks() const
{
	return const_cast<CBrat3DPlot*>( this )->coordinates()->axes[ Qwt3D::AXIS::Z1 ].majors();
}
void CBrat3DPlot::SetZAxisTicks( unsigned int nticks )
{
    for ( auto axis : smZaxis )
    {
        coordinates()->axes[ axis ].setMajors( nticks );
    }
    updateGL();
}






void CBrat3DPlot::AddSurface( const C3DPlotParameters &values, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax )
{
	mFunctions.push_back( new CBrat3DFunction( *this ) );
	mFunctions.back()->AddSurface( values, xmin, xmax, ymin, ymax, zmin, zmax );
}

void CBrat3DPlot::SetLogarithmicScale( bool onlyz, bool log )
{
	for ( auto *f : mFunctions )
	{
		assert__( f );	// TODO: do we support x,y logarithmic axis ?

		f->SetLogarithmic( log );
	}

	auto scale = log ? Qwt3D::LOG10SCALE : Qwt3D::LINEARSCALE;

	for ( auto axis : smZaxis )
		coordinates()->axes[ axis ].setScale( scale );

	if ( !onlyz )
	{
		for ( auto axis : smXaxis )
			coordinates()->axes[ axis ].setScale( scale );
		for ( auto axis : smYaxis )
			coordinates()->axes[ axis ].setScale( scale );
	}

	//updateData();
	updateGL();
}



void CBrat3DPlot::Animate( int updateinterval )		//updateinterval = 200 
{
	if ( !mAnimateAction->isChecked() || updateinterval == 0 )
	{
		mTimer.stop();
	}
	else
	{
		mTimer.start( updateinterval );
	}
}

void CBrat3DPlot::rotate()
{
	int prec = 3;

	setRotation(
			(int(prec*xRotation() + 2) % (360*prec))/double(prec),
			(int(prec*yRotation() + 2) % (360*prec))/double(prec),
			(int(prec*zRotation() + 2) % (360*prec))/double(prec)
			);
}


void CBrat3DPlot::Scale( double &xVal, double &yVal, double &zVal )
{
	xVal = xScale();
	yVal = yScale();
	zVal = zScale();
}

template< typename T >
inline int MTicLength( T &t )
{
	double m, M;
	t.ticLength( M, m );
	return M;
}
template< typename T >
inline int mTicLength( T &t )
{
	double m, M;
	t.ticLength( M, m );
	return m;
}


void CBrat3DPlot::SetScale( double xVal, double yVal, double zVal )
{
	setScale( xVal, yVal, zVal );

	for ( auto axis : smXaxis )
		coordinates()->axes[ axis ].setTicLength( 2, 1 );
	for ( auto axis : smYaxis )
		coordinates()->axes[ axis ].setTicLength( 2, 1 );
	for ( auto axis : smZaxis )
		coordinates()->axes[ axis ].setTicLength( 2, 1 );
}


void CBrat3DPlot::Reset()
{
	setRotation( 30, 0, 15 );
	setScale( 1, 1, 1 );
	setShift( 0.1, 0, 0 );
	setZoom( 0.8 );
}


void CBrat3DPlot::StandardColor( bool standard )
{
	if ( standard )
		setDataColor( new Qwt3D::StandardColor( this ) );
	else
#if (QWT3D_MINOR_VERSION > 2)
		setDataColor( new CBratColor( curve() ) );
#else
		setDataColor( new CBratColor( this ) );
#endif
	updateData();
}


void CBrat3DPlot::SetPlotStyle( Qwt3D::PLOTSTYLE style )		//style = Qwt3D::FILLEDMESH
{
	setPlotStyle( style );
	updateData();
	updateGL();
}

void CBrat3DPlot::PlotStyle()
{
	auto a = qobject_cast<QAction*>( sender() );		assert__( a );
	auto index = mStyleGroup->actions().indexOf( a );	assert__( index >= 0 && a->data().toInt() == index );

    Q_UNUSED( index );

	SetPlotStyle( static_cast<Qwt3D::PLOTSTYLE>( a->data().toInt() ) );
}


void CBrat3DPlot::SetCoordinateStyle( Qwt3D::COORDSTYLE style )		//style = Qwt3D::FRAME
{
	setCoordinateStyle( style );
}

void CBrat3DPlot::CoordinateStyle()
{
	auto a = qobject_cast<QAction*>( sender() );					assert__( a );
	auto index = mCoordinateStyleGroup->actions().indexOf( a );		assert__( index >= 0 && a->data().toInt() == index );

    Q_UNUSED( index );

    SetCoordinateStyle( static_cast<Qwt3D::COORDSTYLE>( a->data().toInt() ) );
}




void CBrat3DPlot::SetColorMap( Qwt3D::Color *pcolor_map )
{
	setDataColor( pcolor_map );
	updateData();
	updateGL();

	//legend()->setMajors( majors );
	//legend()->setMinors( minors );
	double start, stop;
	coordinates()->axes[ Qwt3D::AXIS::Z1 ].limits(start,stop);
#if (QWT3D_MINOR_VERSION > 2)
	curve()->legend()->setLimits(start, stop);
#else
	legend()->setLimits(start, stop);
#endif

	showColorLegend( true );
}


// digits / date

void CBrat3DPlot::SetDigits(int &this_digits, const std::vector< Qwt3D::AXIS > &axis_ids, bool isdate, int digits, brathl_refDate date_ref )
{
	assert__( this_digits != -1 );		//can only be assigned once

	this_digits = isdate ? -1 : digits;
	for ( auto axis : axis_ids )
    {
        CBrat3DScale *s = new CBrat3DScale;        //s->SetLog(is_log);
        s->SetDate( isdate, date_ref );
		s->SetNDigits( this_digits );
        coordinates()->axes[ axis ].setScale( s );
    }

    updateGL();
}

void CBrat3DPlot::SetXDigits( bool isdate, int digits, brathl_refDate date_ref )      //date_ref = REF19500101
{
    SetDigits( mXDigits, smXaxis, isdate, digits, date_ref );
}

void CBrat3DPlot::SetYDigits( bool isdate, int digits, brathl_refDate date_ref )      //date_ref = REF19500101
{
    SetDigits( mYDigits, smYaxis, isdate, digits, date_ref );
}

void CBrat3DPlot::SetZDigits( bool isdate, int digits, brathl_refDate date_ref )      //date_ref = REF19500101
{
    //for ( auto *f : mFunctions )
    //{
    //    assert__( f );	// TODO: do we support x,y logarithmic axis ?

    //    f->SetLogarithmic( is_log );
    //}
    SetDigits( mZDigits, smZaxis, isdate, digits, date_ref );
}




//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//						Main Class
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////



//static 
const std::string C3DPlotWidget::smFontName = "Arial";



C3DPlotWidget::C3DPlotWidget( QWidget *parent ) 
	: base_t( parent )
	, m_SizeHint( DefaultSizeHint( this ) )
{
	setWindowIcon( QPixmap( ":/3.png" ) );
	setWindowTitle( "[*]" );
	setAttribute( Qt::WA_DeleteOnClose );

	//same as QwtPlot (2D)

    setFrameStyle( QFrame::Panel | QFrame::Sunken );
    setLineWidth(2);
    setMidLineWidth(0);

	QPalette pal( palette() ); 
	pal.setColor( QPalette::Background, Qt::white );
	setAutoFillBackground( true );
	setPalette( pal );

	setMinimumSize( min_plot_widget_width, min_plot_widget_height );

#if defined (TEST_EXAMPLES)

	static Plot3dExample ex = e_Plot3dExample_size;

	switch ( ++ex )
	{
		case e_Autoswitching_axes:
				Autoswitching_axes();
			break;

		case e_Simple_SurfacePlot:
				Simple_SurfacePlot();
			break;

		case e_Vertex_Enrichment:
				Vertex_Enrichment();
			break;

		default:
			throw "C3DPlotWidget development error";
	}
#endif
}


C3DPlotWidget::~C3DPlotWidget()
{
	//delete hat;		//enrichment example
}


void C3DPlotWidget::PushPlot( const C3DPlotParameters &values, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, Qwt3D::Color *pcolor_map )
{
#if defined (TEST_EXAMPLES)
	return;
#endif

	mCurrentPlot = new CBrat3DPlot( this );
	mSurfacePlots.push_back( mCurrentPlot );
	AddWidget( mCurrentPlot );
	
	connect( mCurrentPlot, SIGNAL( scaleChanged( double, double, double ) ), this, SLOT( HandleScaleChanged( double, double, double ) ) );

	mCurrentPlot->AddSurface( values, xmin, xmax, ymin, ymax, zmin, zmax );

	mCurrentPlot->setDataColor( pcolor_map );

	double mmax = std::max( xmax, std::max( ymax, zmax ) );
	double xscale = mmax / xmax;
	double yscale = mmax / ymax;
	double zscale = mmax / zmax;
	const double r = 2. / 3.;

	SetScale( xscale * r, yscale * r, zscale * r );
}


void C3DPlotWidget::SetColorMap( Qwt3D::Color *pcolor_map )
{
	assert__( mSurfacePlots.size() );

	CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );

	p->SetColorMap( pcolor_map );
}




void C3DPlotWidget::SetCurrentPlot( int index )
{
	assert__( size_t( index ) <mSurfacePlots.size() );

	mCurrentPlot = dynamic_cast<CBrat3DPlot*>( mSurfacePlots[ index ] );

	mStackedLayout->setCurrentIndex( index );								assert__( mCurrentPlot == mStackedLayout->currentWidget() );
}


//title

void C3DPlotWidget::SetPlotTitle( const std::string &title )
{
	assert__( mSurfacePlots.size() );

	CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );


	p->setTitleFont( t2q( smFontName ), smTitleFontSize, QFont::Bold );

	p->setTitle( title.c_str() );
}


// persistence (image only)

//static 
bool C3DPlotWidget::Save2ps( CBrat3DPlot *p, const QString &path )	//Supported
{
	QString qpath = path;
	QString f = "PS";
	SetFileExtension( qpath, f );
	return p->saveVector( qpath, f, Qwt3D::VectorWriter::NATIVE, Qwt3D::VectorWriter::NOSORT );
}
//static 
bool C3DPlotWidget::Save2unsupported( CBrat3DPlot *p, const QString &path, const QString &format )
{
	QPixmap pix = QPixmap::grabWidget( p );
	if ( pix.isNull() )
	{
		return false;
	}

	QString qpath = path;
	QString f = format;
	SetFileExtension( qpath, f );
	if ( p->saveVector( qpath, f, Qwt3D::VectorWriter::NATIVE, Qwt3D::VectorWriter::NOSORT ) || 
		p->save( qpath, f ) || 
		pix.save( qpath, f.toStdString().c_str() ) )
		return true;

	f = format.toLower();
	SetFileExtension( qpath, f );
	return  
		p->saveVector( qpath, f, Qwt3D::VectorWriter::NATIVE, Qwt3D::VectorWriter::NOSORT ) || 
		p->save( qpath, f ) || 
		pix.save( qpath, f.toStdString().c_str() );
}
//static 
bool C3DPlotWidget::Save2svg( CBrat3DPlot *p, const QString &path )	//NOT supported
{
	return Save2unsupported( p, path, "SVG" );
}
//static 
bool C3DPlotWidget::Save2gif( CBrat3DPlot *p, const QString &path )
{
	return Save2unsupported( p, path, "GIF" );
}

//static 
void C3DPlotWidget::Save2All( CBrat3DPlot *p, const QString &path )
{
	Save2ps( p, path );		//supported
	Save2svg( p, path );	//NOT supported
	Save2gif( p, path );	//NOT supported

	QString qpath = path;
	QString 
	f = "tif";
	SetFileExtension( qpath, f );	//OK
	p->savePixmap( qpath, f );

	f = "bmp";
	SetFileExtension( qpath, f );	//OK
	p->savePixmap( qpath, f );

	f = "jpg";
	SetFileExtension( qpath, f );	//OK
	p->savePixmap( qpath, f );

	f = "png";
	SetFileExtension( qpath, f );	//OK
	p->savePixmap( qpath, f );

	f = "ppm";						//OK
	SetFileExtension( qpath, QString( "pnm" ) );
	p->save( qpath, f );
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
bool C3DPlotWidget::Save2Image( const QString &path, const QString &format, const QString &extension )
{
	assert__( mSurfacePlots.size() );

	CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );

//#if defined (_DEBUG) || defined(DEBUG)
//	Save2All( p, path );
//#endif

	QString qpath = path;
	QString f = format.toLower();
	SetFileExtension( qpath, extension );
	return p->savePixmap( qpath, f );
}




//style

void C3DPlotWidget::SetStyle( bool show_contour, bool show_mesh )
{
	assert__( mCurrentPlot );

	Qwt3D::PLOTSTYLE style = Qwt3D::WIREFRAME;

	if ( show_contour )
	{
		if ( show_mesh )
			style = Qwt3D::FILLEDMESH;
		else
			style = Qwt3D::HIDDENLINE;
	}
	else
	{
		if ( show_mesh )
			style = Qwt3D::FILLED;
		else
			style = Qwt3D::NOPLOT;
	}

	mCurrentPlot->SetPlotStyle( style );
}

bool C3DPlotWidget::HasMesh() const
{
	assert__( mCurrentPlot );

	auto style = mCurrentPlot->plotStyle();

	return style == Qwt3D::FILLEDMESH || style == Qwt3D::HIDDENLINE;
}

void C3DPlotWidget::ShowMesh( bool show )
{
	SetStyle( show, HasSolidColor() );
}


bool C3DPlotWidget::HasSolidColor() const
{
	assert__( mCurrentPlot );

	auto style = mCurrentPlot->plotStyle();

	return style == Qwt3D::FILLEDMESH || style == Qwt3D::FILLED;
}

void C3DPlotWidget::ShowSolidColor( bool show )
{
	SetStyle( HasMesh(), show );
}


void C3DPlotWidget::ShowMesh( int index, bool show )
{
	assert__( size_t( index ) <mSurfacePlots.size() );

	auto save = mCurrentPlot;
	mCurrentPlot = dynamic_cast<CBrat3DPlot*>( mSurfacePlots[ index ] );
	ShowMesh( show );
	mCurrentPlot = save;
}

void C3DPlotWidget::ShowSolidColor( int index, bool show )
{
	assert__( size_t( index ) <mSurfacePlots.size() );

	auto save = mCurrentPlot;
	mCurrentPlot = dynamic_cast<CBrat3DPlot*>( mSurfacePlots[ index ] );
	ShowSolidColor( show );
	mCurrentPlot = save;
}




//axis

void C3DPlotWidget::HandleScaleChanged( double xVal, double yVal, double zVal )
{
	emit ScaleChanged( xVal, yVal, zVal );
}


void C3DPlotWidget::SetLogarithmicScaleZ( bool log )
{
	assert__( mSurfacePlots.size() );

	CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );

	p->SetLogarithmicScale( true, log );
}

void C3DPlotWidget::SetLogarithmicScale( bool log )
{
	assert__( mSurfacePlots.size() );

	CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );

	p->SetLogarithmicScale( false, log );
}


void C3DPlotWidget::SetAxisTitles( const std::string &xtitle, const std::string &ytitle, const std::string &ztitle )
{
	assert__( mSurfacePlots.size() );

#if defined (TEST_EXAMPLES)
	return;
#endif

	CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );

	p->SetAxisTitles( xtitle, ytitle, ztitle );
}



// ticks

unsigned int C3DPlotWidget::XAxisNbTicks() const
{
    assert__( mSurfacePlots.size() );
    const CBrat3DPlot *p = dynamic_cast< const CBrat3DPlot* >( mCurrentPlot );		assert__( p );
    return p->XAxisNbTicks();
}
void C3DPlotWidget::SetXAxisNbTicks( unsigned int nbticks )
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
    p->SetXAxisTicks( nbticks );
}


unsigned int C3DPlotWidget::YAxisNbTicks() const
{
    assert__( mSurfacePlots.size() );
    const CBrat3DPlot *p = dynamic_cast< const CBrat3DPlot* >( mCurrentPlot );		assert__( p );
    return p->YAxisNbTicks();
}
void C3DPlotWidget::SetYAxisNbTicks( unsigned int nbticks )
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
    p->SetYAxisTicks( nbticks );
}

unsigned int C3DPlotWidget::ZAxisNbTicks() const
{
    assert__( mSurfacePlots.size() );
    const CBrat3DPlot *p = dynamic_cast< const CBrat3DPlot* >( mCurrentPlot );		assert__( p );
    return p->ZAxisNbTicks();
}
void C3DPlotWidget::SetZAxisNbTicks( unsigned int nbticks )
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
    p->SetZAxisTicks( nbticks );
}




// digits / date

int C3DPlotWidget::XDigits() const
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	return p->XDigits();
}
int C3DPlotWidget::YDigits() const
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	return p->YDigits();
}
int C3DPlotWidget::ZDigits() const
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	return p->ZDigits();
}

bool C3DPlotWidget::XisDateTime() const
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	return p->XisDateTinme();
}
bool C3DPlotWidget::YisDateTime() const
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	return p->YisDateTinme();
}
bool C3DPlotWidget::ZisDateTime() const
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
	return p->ZisDateTinme();
}


void C3DPlotWidget::SetXDigits( bool isdate, unsigned int digits, brathl_refDate date_ref )       //date_ref = REF19500101
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
    p->SetXDigits( isdate, digits, date_ref );
}

void C3DPlotWidget::SetYDigits( bool isdate, unsigned int digits, brathl_refDate date_ref )       //date_ref = REF19500101
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
    p->SetYDigits( isdate, digits, date_ref );
}

void C3DPlotWidget::SetZDigits( bool isdate, unsigned int digits, brathl_refDate date_ref )       //date_ref = REF19500101
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
    p->SetZDigits( isdate, digits, date_ref );
}




// scale

void C3DPlotWidget::Scale( double &xVal, double &yVal, double &zVal )
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
    p->Scale( xVal, yVal, zVal );
}
void C3DPlotWidget::SetScale( double xVal, double yVal, double zVal )
{
    assert__( mSurfacePlots.size() );
    CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );
    p->SetScale( xVal, yVal, zVal );
}



//interaction

void C3DPlotWidget::Home()
{
	assert__( mSurfacePlots.size() );

	CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mCurrentPlot );		assert__( p );

	p->Reset();
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////


void C3DPlotWidget::AddWidget( QWidget *w )
{
	mStackedLayout = dynamic_cast< QStackedLayout* >( layout() );
	if ( !mStackedLayout )
	{
		mStackedLayout = new QStackedLayout( this );
		setLayout( mStackedLayout );
	}
	mStackedLayout->addWidget( w );
	mStackedLayout->setCurrentWidget( w );
}



QSize C3DPlotWidget::sizeHint() const
{
	return m_SizeHint;
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_3DPlotWidget.cpp"
