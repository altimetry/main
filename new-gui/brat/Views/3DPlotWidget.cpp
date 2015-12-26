#include "new-gui/brat/stdafx.h"

#include <QtGui>
#include <QColor>

#include <qwt3d_plot.h>
#include <qwt3d_function.h>

#include "new-gui/Common/QtUtils.h"
#include "3DPlotWidget.h"


//typedef Qwt3D::Function Function; // VC6/moc issue



//////////////////////////////////////////
//
//  Examples for user defined tic labels
//
//////////////////////////////////////////

class Letter : public Qwt3D::LinearScale
{
public:
	explicit Letter( bool uppercase = true ) : uc_( uppercase ) {}
	Scale* clone() const { return new Letter( *this ); }
	QString ticLabel( unsigned int idx ) const
	{
		if ( idx < majors_p.size() && idx < 26 )
			return ( uc_ ) ? QString( QChar( 'A' + idx ) ) : QString( QChar( 'a' + idx ) );
		return QString( "-" );
	}
private:
	bool uc_;
};

class Imaginary : public Qwt3D::LinearScale
{
public:
	Scale* clone() const { return new Imaginary; }
	QString ticLabel( unsigned int idx ) const
	{
		if ( idx < majors_p.size() )
		{
			double val = majors_p[ idx ];
			if ( val )
				return QString::number( val ) + "*i";
			return QString::number( val );
		}
		return QString( "" );
	}
};

class TimeItems : public Qwt3D::LinearScale
{
public:
	Scale* clone() const { return new TimeItems; }
	QString ticLabel( unsigned int idx ) const
	{
		if ( idx < majors_p.size() )
		{
			QTime t = QTime::currentTime();
			int h = t.hour();
			int m = t.minute();
			if ( m + idx > 59 )
			{
				if ( h == 23 )
					h=0;
				else
					h+=1;
				m = ( m + idx ) % 60;
			}
			else
				m += idx;

			return QTime( h, m ).toString( "hh:mm" ) + "h";
		}
		return QString( "" );
	}
};



//////////////////////////////////////////
//
//  Qwt3D functions
//
//////////////////////////////////////////

class Saddle : public Qwt3D::Function
{
public:
	Saddle(Qwt3D::SurfacePlot& pw)
	:Function(pw)
	{
		setDomain(-2.5,2.5,-2.5,2.5);
		setMaxZ(1.5);
		setMinZ(-1.5);
		setMesh(31,31);
	}

	double operator()(double x, double y)
	{
		return x*y;
	}
};

class Hat : public Qwt3D::Function
{
public:

	Hat(Qwt3D::SurfacePlot& pw)	: Qwt3D::Function(pw)
	{
		setDomain(-1.5,1.5,-1.5,1.5);
		setMesh(41,41);
	}

	double operator()(double x, double y)
	{
		return 1.0 / (x*x+y*y+0.3);
	}
};

class Rosenbrock : public Qwt3D::Function
{
public:

	Rosenbrock( SurfacePlot& pw ) : Qwt3D::Function( pw )
	{
	}

	double operator()( double x, double y )
	{
		return log( ( 1 - x )*( 1 - x ) + 100 * ( y - x*x )*( y - x*x ) ) / 8;
	}
};


class Hat2 : public Qwt3D::Function
{
public:

	Hat2( SurfacePlot& pw ) : Qwt3D::Function( pw )
	{
	}

	double operator()( double x, double y )
	{
		double ret = 1.0 / ( x*x + y*y + 0.5 );
		return ret;
	}
	QString name() const { return "$\\frac{1}{x^2+y^2+\\frac{1}{2}}$"; }
};




//////////////////////////////////////////
//
//  Custom Qwt3D::VertexEnrichment
//			&& Label3D
//
//////////////////////////////////////////

class  Label3D
{
public:
	void draw( Qwt3D::Triple const&, double w, double h );
};


void Label3D::draw( Qwt3D::Triple const& pos, double w, double h )
{
	double gap = 0.3;
	glColor3d( 1, 1, 1 );
	glBegin( GL_QUADS );
	glVertex3d( pos.x - w, pos.y, pos.z + gap );
	glVertex3d( pos.x + w, pos.y, pos.z + gap );
	glVertex3d( pos.x + w, pos.y, pos.z + gap + h );
	glVertex3d( pos.x - w, pos.y, pos.z + gap + h );
	glEnd();
	glColor3d( 0.4, 0, 0 );
	glBegin( GL_LINE_LOOP );
	glVertex3d( pos.x - w, pos.y, pos.z + gap );
	glVertex3d( pos.x + w, pos.y, pos.z + gap );
	glVertex3d( pos.x + w, pos.y, pos.z + gap + h );
	glVertex3d( pos.x - w, pos.y, pos.z + gap + h );
	glEnd();
	glBegin( GL_LINES );
	glVertex3d( pos.x, pos.y, pos.z );
	glVertex3d( pos.x, pos.y, pos.z + gap );
	glEnd();
}



class Bar : public Qwt3D::VertexEnrichment
{
public:
	Bar();
	Bar( double rad, double level );

	Qwt3D::Enrichment* clone() const { return new Bar( *this ); }

	void configure( double rad, double level );
	void drawBegin();
	void drawEnd();
	void draw( Qwt3D::Triple const& );

private:
	double level_, radius_;
	double diag_;
};



Bar::Bar()
{
	configure( 0, 1 );
}

Bar::Bar( double rad, double level )
{
	configure( rad, level );
}

void Bar::configure( double rad, double level )
{
	plot = 0;
	radius_ = rad;
	level_ = level;
}

void Bar::drawBegin()
{
	diag_ = ( plot->hull().maxVertex - plot->hull().minVertex ).length() * radius_;
	glLineWidth( 0 );
	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( 1, 1 );
}

void Bar::drawEnd()
{
}

void Bar::draw( Qwt3D::Triple const& pos )
{

	//	GLStateBewarer sb(GL_LINE_SMOOTH, true);
	//  sb.turnOn();

	double interval = plot->hull().maxVertex.z - plot->hull().minVertex.z;
	double numlevel = plot->hull().minVertex.z + level_ * interval;
	interval /=100;
	if ( pos.z > numlevel - interval && pos.z < numlevel + interval )
	{
		Label3D lb;
		lb.draw( pos, diag_, diag_ * 2 );
	}

	GLdouble minz = plot->hull().minVertex.z;

	Qwt3D::RGBA rgbat = ( *plot->dataColor() )( pos );
	Qwt3D::RGBA rgbab = ( *plot->dataColor() )( pos.x, pos.y, minz );

	glBegin( GL_QUADS );
	glColor4d( rgbab.r, rgbab.g, rgbab.b, rgbab.a );
	glVertex3d( pos.x - diag_, pos.y - diag_, minz );
	glVertex3d( pos.x + diag_, pos.y - diag_, minz );
	glVertex3d( pos.x + diag_, pos.y + diag_, minz );
	glVertex3d( pos.x - diag_, pos.y + diag_, minz );

	if ( pos.z > numlevel - interval && pos.z < numlevel + interval )
		glColor3d( 0.7, 0, 0 );
	else
		glColor4d( rgbat.r, rgbat.g, rgbat.b, rgbat.a );
	glVertex3d( pos.x - diag_, pos.y - diag_, pos.z );
	glVertex3d( pos.x + diag_, pos.y - diag_, pos.z );
	glVertex3d( pos.x + diag_, pos.y + diag_, pos.z );
	glVertex3d( pos.x - diag_, pos.y + diag_, pos.z );

	glColor4d( rgbab.r, rgbab.g, rgbat.b, rgbab.a );
	glVertex3d( pos.x - diag_, pos.y - diag_, minz );
	glVertex3d( pos.x + diag_, pos.y - diag_, minz );
	glColor4d( rgbat.r, rgbat.g, rgbat.b, rgbat.a );
	glVertex3d( pos.x + diag_, pos.y - diag_, pos.z );
	glVertex3d( pos.x - diag_, pos.y - diag_, pos.z );

	glColor4d( rgbab.r, rgbab.g, rgbat.b, rgbab.a );
	glVertex3d( pos.x - diag_, pos.y + diag_, minz );
	glVertex3d( pos.x + diag_, pos.y + diag_, minz );
	glColor4d( rgbat.r, rgbat.g, rgbat.b, rgbat.a );
	glVertex3d( pos.x + diag_, pos.y + diag_, pos.z );
	glVertex3d( pos.x - diag_, pos.y + diag_, pos.z );

	glColor4d( rgbab.r, rgbab.g, rgbat.b, rgbab.a );
	glVertex3d( pos.x - diag_, pos.y - diag_, minz );
	glVertex3d( pos.x - diag_, pos.y + diag_, minz );
	glColor4d( rgbat.r, rgbat.g, rgbat.b, rgbat.a );
	glVertex3d( pos.x - diag_, pos.y + diag_, pos.z );
	glVertex3d( pos.x - diag_, pos.y - diag_, pos.z );

	glColor4d( rgbab.r, rgbab.g, rgbat.b, rgbab.a );
	glVertex3d( pos.x + diag_, pos.y - diag_, minz );
	glVertex3d( pos.x + diag_, pos.y + diag_, minz );
	glColor4d( rgbat.r, rgbat.g, rgbat.b, rgbat.a );
	glVertex3d( pos.x + diag_, pos.y + diag_, pos.z );
	glVertex3d( pos.x + diag_, pos.y - diag_, pos.z );
	glEnd();

	glColor3d( 0, 0, 0 );
	glBegin( GL_LINES );
	glVertex3d( pos.x - diag_, pos.y - diag_, minz ); glVertex3d( pos.x + diag_, pos.y - diag_, minz );
	glVertex3d( pos.x - diag_, pos.y - diag_, pos.z ); glVertex3d( pos.x + diag_, pos.y - diag_, pos.z );
	glVertex3d( pos.x - diag_, pos.y + diag_, pos.z ); glVertex3d( pos.x + diag_, pos.y + diag_, pos.z );
	glVertex3d( pos.x - diag_, pos.y + diag_, minz ); glVertex3d( pos.x + diag_, pos.y + diag_, minz );

	glVertex3d( pos.x - diag_, pos.y - diag_, minz ); glVertex3d( pos.x - diag_, pos.y + diag_, minz );
	glVertex3d( pos.x + diag_, pos.y - diag_, minz ); glVertex3d( pos.x + diag_, pos.y + diag_, minz );
	glVertex3d( pos.x + diag_, pos.y - diag_, pos.z ); glVertex3d( pos.x + diag_, pos.y + diag_, pos.z );
	glVertex3d( pos.x - diag_, pos.y - diag_, pos.z ); glVertex3d( pos.x - diag_, pos.y + diag_, pos.z );

	glVertex3d( pos.x - diag_, pos.y - diag_, minz ); glVertex3d( pos.x - diag_, pos.y - diag_, pos.z );
	glVertex3d( pos.x + diag_, pos.y - diag_, minz ); glVertex3d( pos.x + diag_, pos.y - diag_, pos.z );
	glVertex3d( pos.x + diag_, pos.y + diag_, minz ); glVertex3d( pos.x + diag_, pos.y + diag_, pos.z );
	glVertex3d( pos.x - diag_, pos.y + diag_, minz ); glVertex3d( pos.x - diag_, pos.y + diag_, pos.z );
	glEnd();
}

//////////////////////////////////////////
//
//  Custom Qwt3D::SurfacePlot
//
//////////////////////////////////////////

Plot::Plot(QWidget* pw, int updateinterval) : Qwt3D::SurfacePlot(pw)
{
	if ( updateinterval )
	{
		setRotation( 30, 0, 15 );
		setShift( 0.1, 0, 0 );
		setZoom( 0.8 );
		coordinates()->setNumberFont( "Courier", 8 );

		for ( unsigned i=0; i != coordinates()->axes.size(); ++i )
		{
			coordinates()->axes[ i ].setMajors( 7 );
			coordinates()->axes[ i ].setMinors( 4 );
		}

        static const QString x = "x";
        static const QString y = "y";
        static const QString z = "z";

        coordinates()->axes[ Qwt3D::X1 ].setLabelString( x );
        coordinates()->axes[ Qwt3D::Y1 ].setLabelString( y );
        coordinates()->axes[ Qwt3D::Z1 ].setLabelString( z );
        coordinates()->axes[ Qwt3D::X2 ].setLabelString( x );
        coordinates()->axes[ Qwt3D::Y2 ].setLabelString( y );
        coordinates()->axes[ Qwt3D::Z2 ].setLabelString( z );
        coordinates()->axes[ Qwt3D::X3 ].setLabelString( x );
        coordinates()->axes[ Qwt3D::Y3 ].setLabelString( y );
        coordinates()->axes[ Qwt3D::Z3 ].setLabelString( z );
        coordinates()->axes[ Qwt3D::X4 ].setLabelString( x );
        coordinates()->axes[ Qwt3D::Y4 ].setLabelString( y );
        coordinates()->axes[ Qwt3D::Z4 ].setLabelString( z );


		QTimer* timer = new QTimer( this );
		connect( timer, SIGNAL( timeout() ), this, SLOT( rotate() ) );

		timer->start( updateinterval );
	}
}

void Plot::rotate()
{
	int prec = 3;
		
	setRotation(
			(int(prec*xRotation() + 2) % (360*prec))/double(prec),
			(int(prec*yRotation() + 2) % (360*prec))/double(prec),
			(int(prec*zRotation() + 2) % (360*prec))/double(prec)
			);
}

//////////////////////////////////////////
//
//			Examples
//
//////////////////////////////////////////


enum Plot3dExample
{
	e_Autoswitching_axes, e_Plot3dExample_first = e_Autoswitching_axes,
	e_Simple_SurfacePlot,
	e_Vertex_Enrichment,

	e_Plot3dExample_size
};

Plot3dExample& operator ++( Plot3dExample &ex )
{
	int exi = static_cast<int>( ex ) + 1;

	if ( exi >= e_Plot3dExample_size )
		ex = e_Plot3dExample_first;
	else
		ex = static_cast<Plot3dExample>( exi );

	return ex;// static_cast<Plot3dExample>( static_cast<int>(ex)+ 1 );
}

void C3DPlotWidget::setCentralWidget( QWidget *w )
{
	QHBoxLayout *hl = new QHBoxLayout( this );
	hl->addWidget( w );
	setLayout( hl );
}

void C3DPlotWidget::Autoswitching_axes()
{
	QSplitter* spl = new QSplitter(Qt::Horizontal, this);
	setCentralWidget( spl );

	Plot* plot1 = new Plot( spl, 30 );
	plot1->setFloorStyle( Qwt3D::FLOORISO );
	plot1->setCoordinateStyle( Qwt3D::BOX );
	Saddle saddle( *plot1 );
	saddle.create();
	plot1->setTitle( "Autoswitching axes" );
	plot1->setBackgroundColor( Qwt3D::RGBA( 1, 1, 157. / 255 ) );
	plot1->makeCurrent();
	plot1->updateData();
	plot1->updateGL();

	mPlots.push_back( plot1 );

	Plot* plot2 = new Plot( spl, 80 );
	plot2->setZoom( 0.8 );
	Hat hat( *plot2 );
	hat.create();
	plot2->setPlotStyle( Qwt3D::HIDDENLINE );
	plot2->setFloorStyle( Qwt3D::FLOORDATA );
	plot2->setCoordinateStyle( Qwt3D::FRAME );
	plot2->setBackgroundColor( Qwt3D::RGBA( 1, 1, 157. / 255 ) );
	plot2->makeCurrent();
	plot2->updateData();
	plot2->updateGL();

	mPlots.push_back( plot2 );
}
void C3DPlotWidget::Simple_SurfacePlot()
{
	Plot* plot = new Plot( this );
	setCentralWidget( plot );

    plot->setTitle("A Simple SurfacePlot Demonstration");
    
    Rosenbrock rosenbrock(*plot);

    rosenbrock.setMesh(41,31);
    rosenbrock.setDomain(-1.73,1.5,-1.5,1.5);
    rosenbrock.setMinZ(-10);

    rosenbrock.create();

    plot->setRotation(30,0,15);
    plot->setScale(1,1,1);
    plot->setShift(0.15,0,0);
    plot->setZoom(0.9);

    for (unsigned i=0; i != plot->coordinates()->axes.size(); ++i)
    {
      plot->coordinates()->axes[i].setMajors(7);
      plot->coordinates()->axes[i].setMinors(4);
    }

    plot->coordinates()->axes[Qwt3D::X1].setLabelString("x-axis");
    plot->coordinates()->axes[Qwt3D::Y1].setLabelString("y-axis");
    //coordinates()->axes[Z1].setLabelString(QChar(0x38f)); // Omega - see http://www.unicode.org/charts/

    plot->setCoordinateStyle(Qwt3D::BOX);

    plot->updateData();
    plot->updateGL();
}




void C3DPlotWidget::setupUi()
{
	QWidget *centralWidget;
	QGridLayout *gridLayout;
	QVBoxLayout *vboxLayout;
	QHBoxLayout *hboxLayout;
	QVBoxLayout *vboxLayout1;
	QLabel *label;
	QHBoxLayout *hboxLayout1;
	QLabel *label_2;

	if ( objectName().isEmpty() )
		setObjectName( QString::fromUtf8( "" ) );
	resize( 800, 600 );
	centralWidget = new QWidget(  );
	centralWidget->setObjectName( QString::fromUtf8( "centralWidget" ) );
	gridLayout = new QGridLayout( centralWidget );
#ifndef Q_OS_MAC
	gridLayout->setSpacing( 6 );
#endif
	gridLayout->setContentsMargins( 8, 8, 8, 8 );
	gridLayout->setObjectName( QString::fromUtf8( "gridLayout" ) );
	vboxLayout = new QVBoxLayout();
#ifndef Q_OS_MAC
	vboxLayout->setSpacing( 6 );
#endif
#ifndef Q_OS_MAC
	vboxLayout->setContentsMargins( 0, 0, 0, 0 );
#endif
	vboxLayout->setObjectName( QString::fromUtf8( "vboxLayout" ) );
	hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
	hboxLayout->setSpacing( 6 );
#endif
#ifndef Q_OS_MAC
	hboxLayout->setContentsMargins( 0, 0, 0, 0 );
#endif
	hboxLayout->setObjectName( QString::fromUtf8( "hboxLayout" ) );
	frame = new QFrame( centralWidget );
	frame->setObjectName( QString::fromUtf8( "frame" ) );
	frame->setFrameShape( QFrame::StyledPanel );
	frame->setFrameShadow( QFrame::Sunken );

	hboxLayout->addWidget( frame );

	vboxLayout1 = new QVBoxLayout();
#ifndef Q_OS_MAC
	vboxLayout1->setSpacing( 6 );
#endif
	vboxLayout1->setContentsMargins( 0, 0, 0, 0 );
	vboxLayout1->setObjectName( QString::fromUtf8( "vboxLayout1" ) );
	label = new QLabel( centralWidget );
	label->setObjectName( QString::fromUtf8( "label" ) );
	QSizePolicy sizePolicy( static_cast<QSizePolicy::Policy>( 5 ), static_cast<QSizePolicy::Policy>( 0 ) );
	sizePolicy.setHorizontalStretch( 0 );
	sizePolicy.setVerticalStretch( 0 );
	sizePolicy.setHeightForWidth( label->sizePolicy().hasHeightForWidth() );
	label->setSizePolicy( sizePolicy );

	vboxLayout1->addWidget( label );

	levelSlider = new QSlider( centralWidget );
	levelSlider->setObjectName( QString::fromUtf8( "levelSlider" ) );
	QSizePolicy sizePolicy1( static_cast<QSizePolicy::Policy>( 0 ), static_cast<QSizePolicy::Policy>( 7 ) );
	sizePolicy1.setHorizontalStretch( 0 );
	sizePolicy1.setVerticalStretch( 0 );
	sizePolicy1.setHeightForWidth( levelSlider->sizePolicy().hasHeightForWidth() );
	levelSlider->setSizePolicy( sizePolicy1 );
	levelSlider->setMaximum( 100 );
	levelSlider->setValue( 10 );
	levelSlider->setOrientation( Qt::Vertical );
	levelSlider->setInvertedAppearance( true );
	levelSlider->setTickPosition( QSlider::TicksBelow );
	levelSlider->setTickInterval( 10 );

	vboxLayout1->addWidget( levelSlider );


	hboxLayout->addLayout( vboxLayout1 );


	vboxLayout->addLayout( hboxLayout );

	hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
	hboxLayout1->setSpacing( 6 );
#endif
	hboxLayout1->setContentsMargins( 0, 0, 0, 0 );
	hboxLayout1->setObjectName( QString::fromUtf8( "hboxLayout1" ) );
	label_2 = new QLabel( centralWidget );
	label_2->setObjectName( QString::fromUtf8( "label_2" ) );
	label_2->setEnabled( true );
	QSizePolicy sizePolicy2( static_cast<QSizePolicy::Policy>( 0 ), static_cast<QSizePolicy::Policy>( 0 ) );
	sizePolicy2.setHorizontalStretch( 0 );
	sizePolicy2.setVerticalStretch( 0 );
	sizePolicy2.setHeightForWidth( label_2->sizePolicy().hasHeightForWidth() );
	label_2->setSizePolicy( sizePolicy2 );

	hboxLayout1->addWidget( label_2 );

	widthSlider = new QSlider( centralWidget );
	widthSlider->setObjectName( QString::fromUtf8( "widthSlider" ) );
	widthSlider->setEnabled( true );
	widthSlider->setMinimum( 2 );
	widthSlider->setMaximum( 200 );
	widthSlider->setValue( 20 );
	widthSlider->setOrientation( Qt::Horizontal );
	widthSlider->setTickPosition( QSlider::TicksBelow );

	hboxLayout1->addWidget( widthSlider );


	vboxLayout->addLayout( hboxLayout1 );


	gridLayout->addLayout( vboxLayout, 0, 0, 1, 1 );

	setCentralWidget( centralWidget );

    setWindowTitle( QApplication::translate( "", "", 0
#if QT_VERSION < 0x050000
                                             , QApplication::UnicodeUTF8 ) );
#else
                                             ) );
#endif
    label->setText( QApplication::translate( "", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:MS Shell Dlg; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Level</p></body></html>", 0
#if QT_VERSION < 0x050000
                                             , QApplication::UnicodeUTF8 ) );
#else
                                             ) );
#endif

    label_2->setText( QApplication::translate( "", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:MS Shell Dlg; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Width</p></body></html>", 0
#if QT_VERSION < 0x050000
                                               , QApplication::UnicodeUTF8 ) );
#else
                                             ) );
#endif

	QMetaObject::connectSlotsByName( this );
} // setupUi


void C3DPlotWidget::Vertex_Enrichment()
{
#if QT_VERSION < 0x040000
	setCaption("enrichment");
	QGridLayout *grid = new QGridLayout( frame, 0, 0 );
#else
	setupUi();
	QGridLayout *grid = new QGridLayout( frame );
#endif

	Plot *plot = new Plot( frame );
	mPlots.push_back( plot );
	grid->addWidget( plot, 0, 0 );

	plot->setTitle( "Bar Style (Vertex Enrichment)" );
	plot->setTitleFont( "Arial", 12 );

	plot->setZoom( 0.8 );
	plot->setRotation( 30, 0, 15 );

	plot->setCoordinateStyle( Qwt3D::BOX );
	bar = (Bar*)plot->setPlotStyle( Bar( 0.007, 0.5 ) );

	hat = new Hat2( *plot );

	hat->setMesh( 23, 21 );
	hat->setDomain( -1.8, 1.7, -1.6, 1.7 );

	hat->create();

	for ( unsigned i=0; i != plot->coordinates()->axes.size(); ++i )
	{
		plot->coordinates()->axes[ i ].setMajors( 5 );
		plot->coordinates()->axes[ i ].setMinors( 4 );
	}


	//plot->setMeshLineWidth(1);
	plot->coordinates()->setGridLinesColor( Qwt3D::RGBA( 0, 0, 0.5 ) );
	plot->coordinates()->setLineWidth( 1 );
	plot->coordinates()->setNumberFont( "Courier", 8 );
	plot->coordinates()->adjustNumbers( 5 );

	setColor();
	plot->updateData();
	plot->updateGL();


	levelSlider->setValue( 50 );
	level_ = 0.5;
	width_ = 0.007;
	connect( levelSlider, SIGNAL( valueChanged( int ) ), this, SLOT( setLevel( int ) ) );
	connect( widthSlider, SIGNAL( valueChanged( int ) ), this, SLOT( setWidth( int ) ) );

	//connect( barBtn, SIGNAL(clicked()), this, SLOT(barSlot()) );
	//connect( sliceBtn, SIGNAL(clicked()), this, SLOT(sliceSlot()) );

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDisable( GL_LINE_SMOOTH );
}

C3DPlotWidget::~C3DPlotWidget()
{
	delete hat;
}


void C3DPlotWidget::setColor()
{
//	Qwt3D::ColorVector cv;
    std::vector<Qwt3D::RGBA> cv;

	Qwt3D::RGBA rgb;
	int i = 252;
	int step = 4;

	while ( i >= 0 )
	{
		rgb.r = i / 255.; rgb.g=( i - 60 > 0 ) ? ( i - 60 ) / 255. : 0; rgb.b=0;
		//  rgb.a = 0.2;
		cv.push_back( rgb );
		if ( !--step )
		{
			i-=4;
			step=4;
		}
	}
	Qwt3D::StandardColor* col = new Qwt3D::StandardColor( mPlots[0] );
    col->setColorVector( static_cast<Qwt3D::ColorVector const&>( cv ) );

	mPlots[0]->setDataColor( col );
}


void C3DPlotWidget::setLevel( int i )
{
	level_ = 1 - i / 100.;
	bar->configure( width_, level_ );
	mPlots[0]->updateData();
	mPlots[0]->updateGL();
}

void C3DPlotWidget::setWidth( int i )
{
	width_ = i / 20000.;
	bar->configure( width_, level_ );
	mPlots[0]->updateData();
	mPlots[0]->updateGL();
}

void C3DPlotWidget::barSlot()
{
	Bar b( width_, level_ );
	bar = (Bar*)mPlots[0]->setPlotStyle( b );
	mPlots[0]->updateData();
	mPlots[0]->updateGL();
}


//////////////////////////////////////////
//
//				The Editor
//
//////////////////////////////////////////


C3DPlotWidget::C3DPlotWidget( QWidget *parent ) : base_t( parent ), m_ToolEditor( false )
{
	static Plot3dExample ex = e_Plot3dExample_size;

	setWindowIcon( QPixmap( ":/3.png" ) );
	setWindowTitle( "[*]" );
	setAttribute( Qt::WA_DeleteOnClose );

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
			throw "idiot";
	}

	//mAction = new QAction(this);
	//mAction->setCheckable(true);
	//connect(mAction, SIGNAL(triggered()), this, SLOT(show()));
	//connect(mAction, SIGNAL(triggered()), this, SLOT(setFocus()));

	isUntitled = true;

	//connect( document(), SIGNAL(modificationChanged(bool)), this, SLOT(setWindowModified(bool)));
	//if ( qobject_cast< QMainWindow* >( parent ) )
	//{
	//    //assert( isMainChild() );
	//    connect( this, SIGNAL( setCurrentFile( const C3DPlotWidget* )), parent, SLOT(setCurrentFile( const C3DPlotWidget* )) );
	//}

	setVisible( true );
}


void C3DPlotWidget::ToolEditor( bool tool )
{
    m_ToolEditor = tool;
    setWindowModified( false );
    disconnect( this, SLOT(setWindowModified(bool)) );
}

//////////////////////////////////////////////////////////////////////////
//                          ACCESS ACTIONS
//////////////////////////////////////////////////////////////////////////


bool C3DPlotWidget::isEmpty() const
{
	return false;	// ::isEmpty( this );
}

bool C3DPlotWidget::isMDIChild() const
{
    SimpleMsgBox( "bool C3DPlotWidget::isMDIChild() const \n Not implemented yet..." );
    return false;
}

QString C3DPlotWidget::getSelectedText()
{
	return "";		// ::getSelectedText( this );
}


//////////////////////////////////////////////////////////////////////////
//                          UPDATE ACTIONS
//////////////////////////////////////////////////////////////////////////


//this is very nice but does not free us from manually updating the ations in the Edit update menu function
//
//void C3DPlotWidget::connectSaveAvailableAction( QAction *pa )      {    connect( document(), SIGNAL(modificationChanged(bool)), pa, SLOT(setEnabled(bool)));}
//
//void C3DPlotWidget::connectUndoAvailableAction( QAction *pa )      {    connect( document(), SIGNAL( undoAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
//void C3DPlotWidget::connectRedoAvailableAction( QAction *pa )      {    connect( document(), SIGNAL( redoAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void C3DPlotWidget::connectCutAvailableAction( QAction *pa )       {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void C3DPlotWidget::connectCopyAvailableAction( QAction *pa )      {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void C3DPlotWidget::connectDeleteSelAvailableAction( QAction *pa ) {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}

void  C3DPlotWidget::focusInEvent ( QFocusEvent * event )
{
    base_t::focusInEvent ( event );
    //if ( m_ToolEditor )
        emit toolWindowActivated( this );
}

//////////////////////////////////////////////////////////////////////////
//                              FILE ACTIONS
//////////////////////////////////////////////////////////////////////////


enum EFileType {
    e_txt,
    e_html,
    e_odt,
    e_unknown
};

inline EFileType TypeFromExtension( const QString &fileName )
{
    if ( fileName.endsWith(".odt", Qt::CaseInsensitive) )       //Qt does not read, write only option
        return e_odt;
    if (    fileName.endsWith(".htm", Qt::CaseInsensitive)
         || fileName.endsWith(".html", Qt::CaseInsensitive)
         || fileName.endsWith(".rtf", Qt::CaseInsensitive)      //mightBeRichText returns false for this (...) and Qt really reads it as text
         )
        return e_html;
    if ( fileName.endsWith(".txt", Qt::CaseInsensitive) )       //default on write
        return e_txt;
    return e_unknown;
}

void C3DPlotWidget::documentWasModified()
{
    setWindowModified(true);
}

QString C3DPlotWidget::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


/////////////////////////////////////////////////////////////////////////
// FILE ACTIONS - OPEN/ INPUT
/////////////////////////////////////////////////////////////////////////



void C3DPlotWidget::setCurrentFile( const QString &fileName )
{
    curFile = fileName;

    isUntitled = false;
//    windowMenuAction()->setText(strippedName(curFile));
////    document()->setModified(false);
    setWindowTitle(strippedName(curFile) + "[*]");
    setWindowModified(false);
    emit setCurrentFile( this );
}


void C3DPlotWidget::newFile()
{
    static int documentNumber = 1;

    curFile = tr("document%1.txt").arg(documentNumber);
    setWindowTitle(curFile + "[*]");
//    windowMenuAction()->setText(curFile);
    isUntitled = true;
    ++documentNumber;
}


bool C3DPlotWidget::readFile( const QString &fileName )
{
    if (!QFile::exists( fileName ))
        return false;


    return true;
}


C3DPlotWidget *C3DPlotWidget::openFile(const QString &fileName, QWidget *parent)
{
    C3DPlotWidget *editor = new C3DPlotWidget( parent );
    if (editor->readFile( fileName )) {
        editor->setCurrentFile( fileName );
        return editor;
    } else {
        delete editor;
        return 0;
    }
}

bool C3DPlotWidget::reOpen()
{
    if ( !IsUntitled() && readFile( curFile ) )
        setCurrentFile( curFile );
    else
        return false;
    return true;
}

C3DPlotWidget *C3DPlotWidget::open(QWidget *parent)
{
    QString fileName = QFileDialog::getOpenFileName( parent, tr("Open"), "m_bs.GetDataPath()",
                                                     tr("Expressions (*.mm *.txt);;HTML-Files (*.htm *.html);;All Files (*)") );
    if (fileName.isEmpty())
        return 0;

    return openFile(fileName, parent);
}


/////////////////////////////////////////////////////////////////////////
// FILE ACTIONS - CLOSE / OUTPUT ACTIONS
/////////////////////////////////////////////////////////////////////////

bool C3DPlotWidget::writeFile( const QString &fileName )
{
    Q_UNUSED( fileName );
    //writing, the old way
    //
//    QFile file(fileName);
//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//        QMessageBox::warning(this, tr("MDI C3DPlotWidget"),
//                             tr("Cannot write file %1:\n%2.")
//                             .arg(file.fileName())
//                             .arg(file.errorString()));
//        return false;
//    }

    bool success = false;
    return success;
}


bool C3DPlotWidget::saveFile(const QString &fileName)
{
    if (writeFile(fileName))
    {
        setCurrentFile(fileName);
        return true;
    }
    return false;
}


bool C3DPlotWidget::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As..."), curFile,
                                                    tr("Expressions (*.mm *.txt);;HTML-Files (*.htm *.html);;All Files (*);;ODF files (*.odt)"));
    if ( fileName.isEmpty() )
        return false;

    if ( TypeFromExtension( fileName ) == e_unknown )
        fileName += ".txt"; // default
        //fileName += ".odt"; // default

    return saveFile( fileName );
}


bool C3DPlotWidget::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

void C3DPlotWidget::closeEvent(QCloseEvent *event)
{
    if (okToContinue()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool C3DPlotWidget::okToContinue()
{
    if (curFile.startsWith(QLatin1String(":/")))
        return true;

    //if ( document()->isModified() ) {
    //    int r = QMessageBox::warning(this, tr("MDI C3DPlotWidget"),
    //                    tr("File %1 has been modified.\n"
    //                       "Do you want to save your changes?")
    //                    .arg(strippedName(curFile)),
    //                    QMessageBox::Yes | QMessageBox::No
    //                    | QMessageBox::Cancel);
    //    if (r == QMessageBox::Yes) {
    //        return save();
    //    } else if (r == QMessageBox::Cancel) {
    //        return false;
    //    }
    //}
    return true;
}



//////////////////////////////////////////////////////////////////////////
//                              ??? ACTIONS
//////////////////////////////////////////////////////////////////////////


QSize C3DPlotWidget::sizeHint() const
{
	return m_SizeHint;
    //return QSize(72 * fontMetrics().width('x'),
    //             25 * fontMetrics().lineSpacing());
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_3DPlotWidget.cpp"
