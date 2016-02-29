#include "new-gui/brat/stdafx.h"

#include <QtGui>
#include <QColor>

#include <qwt3d_plot.h>
#include <qwt3d_function.h>

#include "new-gui/Common/QtUtils.h"

#include "PlotValues.h"

#include "GUI/ActionsTable.h"

#include "3DPlotWidget.h"


// TODO delete this macro; if active, no operation is expected to work besides the simple example display
// #define TEST_EXAMPLES


//////////////////////////////////////////
//
//		Auxiliary Qwt3D Classes
//
//			- color
//			- function
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
	CBratColor( Qwt3D::Plot3D *data, unsigned size = 100 ) 
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
//function
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

	CBrat3DFunction( SurfacePlot& pw ) : Qwt3D::Function( pw )
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
//        auto xindex = std::find( mPlotValues->mXaxis->begin(), mPlotValues->mXaxis->end(), x );
//        if ( xindex == mPlotValues->mXaxis->end() )
//            return std::numeric_limits<double>::quiet_NaN();
//        auto yindex = std::find( mPlotValues->mYaxis->begin(), mPlotValues->mYaxis->end(), x );
//        if ( yindex == mPlotValues->mYaxis->end() )
//            return std::numeric_limits<double>::quiet_NaN();

		auto index = x * y;
		if ( index >= mPlotValues->mBits.size() || !mPlotValues->mBits.at( index ) )
			return std::numeric_limits<double>::quiet_NaN();

		return CorrectedValue( mPlotValues->mValues.at( index ) );
	}
};



//////////////////////////////////////////
//qwt3D plot class
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
		a->setChecked( plotStyle() == style );
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



CBrat3DPlot::CBrat3DPlot( QWidget *pw ) 
	: base_t( pw )
	, mTimer( this )
{
	connect( &mTimer, SIGNAL( timeout() ), this, SLOT( rotate() ) );

	CreateContextMenu();
	Reset();

	setSmoothMesh( true );
	enableLighting( false );

	coordinates()->setNumberFont( "Courier", 8 );

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
	delete mFunction;
}


void CBrat3DPlot::SetAxisTitle( Qwt3D::AXIS axis, const std::string &title )
{
	coordinates()->axes[ axis ].setLabelString( title.c_str() );
}


void CBrat3DPlot::AddSurface( const C3DPlotParameters &values, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax )
{
	mFunction = new CBrat3DFunction( *this );
	mFunction->AddSurface( values, xmin, xmax, ymin, ymax, zmin, zmax );
}


void CBrat3DPlot::SetLogarithmicScale( Qwt3D::AXIS axis, bool log )
{
	assert__( mFunction );

	if ( axis != Qwt3D::AXIS::Z1 )		// TODO: do we support x,y logarithmic axis ?
		return;

	mFunction->SetLogarithmic( log );

	auto scale = log ? Qwt3D::LOG10SCALE : Qwt3D::LINEARSCALE;
	coordinates()->axes[ axis ].setScale( scale );
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
		setDataColor( new CBratColor( this ) );

	updateData();
}


void CBrat3DPlot::SetPlotStyle( Qwt3D::PLOTSTYLE style )		//style = Qwt3D::FILLEDMESH
{
	setPlotStyle( style );
	updateData();
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





//////////////////////////////////////////
//
//				The View
//
//////////////////////////////////////////


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


void C3DPlotWidget::AddSurface( const C3DPlotParameters &values, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax )
{
	CBrat3DPlot *plot = nullptr;
	if ( mPlots.size() > 0 )
		DestroyPointersAndContainer(  mPlots );

	plot = new CBrat3DPlot( this );
	mPlots.push_back( plot );
	SetCentralWidget( mPlots[ 0 ] );

	plot->AddSurface( values, xmin, xmax, ymin, ymax, zmin, zmax );
}


void C3DPlotWidget::SetLogarithmicScale( Qwt3D::AXIS axis, bool log )
{
	CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mPlots[0] );

	assert__( p );

	p->SetLogarithmicScale( axis, log );
}


void C3DPlotWidget::SetAxisTitles( const std::string &xtitle, const std::string &ytitle, const std::string &ztitle )
{
	CBrat3DPlot *p = dynamic_cast< CBrat3DPlot* >( mPlots[0] );

	assert__( p );

	p->SetAxisTitles( xtitle, ytitle, ztitle );
}



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////


void C3DPlotWidget::SetCentralWidget( QWidget *w )
{
	QHBoxLayout *hl = dynamic_cast< QHBoxLayout* >( layout() );
	if ( hl )
		delete hl;
	hl = new QHBoxLayout( this );
	hl->addWidget( w );
	setLayout( hl );
}



QSize C3DPlotWidget::sizeHint() const
{
	return m_SizeHint;
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_3DPlotWidget.cpp"
