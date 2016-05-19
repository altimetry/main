#include "new-gui/brat/stdafx.h"

#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/Workspaces/Display.h"
#include "DataModels/PlotData/BratLookupTable.h"

#include "GUI/ActionsTable.h"
#include "GUI/DisplayWidgets/TextWidget.h"


#include "ViewControlPanels.h"



/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//							Specialized Widgets
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

QColor wavelengthToColor( qreal lambda )
{
	// Based on: http://www.efg2.com/Lab/ScienceAndEngineering/Spectra.htm
	// The foregoing is based on: http://www.midnightkite.com/color.html
	struct Color {
		qreal c[ 3 ];
		QColor toColor( qreal factor ) const {
			qreal const gamma = 0.8;
			int ci[ 3 ];
			for ( int i = 0; i < 3; ++i ) {
				ci[ i ] = c[ i ] == 0.0 ? 0.0 : qRound( 255 * pow( c[ i ] * factor, gamma ) );
			}
			return QColor( ci[ 0 ], ci[ 1 ], ci[ 2 ] );
		}
	} color;
	qreal factor = 0.0;

	static qreal thresholds[] ={ 380, 440, 490, 510, 580, 645, 780 };
	for ( unsigned int i = 0; i < sizeof( thresholds ) / sizeof( thresholds[ 0 ] ); ++ i ) {
		qreal t1 = thresholds[ i ], t2 = thresholds[ i + 1 ];
		if ( lambda < t1 || lambda >= t2 ) continue;
		if ( i % 2 ) std::swap( t1, t2 );
		color.c[ i % 3 ] = ( i < 5 ) ? ( lambda - t2 ) / ( t1 - t2 ) : 0.0;;
		color.c[ 2 - i / 2 ] = 1.0;
		factor = 1.0;
		break;
	}

	// Let the intensity fall off near the vision limits
	if ( lambda >= 380 && lambda < 420 ) {
		factor = 0.3 + 0.7*( lambda - 380 ) / ( 420 - 380 );
	}
	else if ( lambda >= 700 && lambda < 780 ) {
		factor = 0.3 + 0.7*( 780 - lambda ) / ( 780 - 700 );
	}
	return color.toColor( factor );
}

QPixmap rainbow( int w, int h )
{
	QPixmap pm( w, h );
	QPainter p( &pm );
	qreal f1 = 1.0 / 400;
	qreal f2 = 1.0 / 780;
	for ( int x = 0; x < w; ++ x ) {
		// Iterate across frequencies, not wavelengths
		qreal lambda = 1.0 / ( f1 - ( x / qreal( w )*( f1 - f2 ) ) );
		p.setPen( wavelengthToColor( lambda ) );
		p.drawLine( x, 0, x, h );
	}
	return pm;
}



CColorMapLabel::CColorMapLabel( QWidget *parent )	//parent = nullptr 
	: base_t( parent )
{
	setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
}


//virtual 
void CColorMapLabel::resizeEvent( QResizeEvent *event )
{
	base_t::resizeEvent( event );

	SetLUT( mLut );
}

void CColorMapLabel::SetLUT( const CBratLookupTable *lut )
{
	mLut = lut;
	int w = width();
	int h = height();
	QPixmap pm( w, h );
	if ( mLut )
	{
		QPainter p( &pm );
		auto *const lut = mLut->GetLookupTable();
		const size_t size = lut->GetNumberOfTableValues();
		for ( int x = 0; x < w; ++ x ) 
		{
			size_t index = x * size / w;
			p.setPen( lut->GetTableValue( index ) );
			p.drawLine( x, 0, x, h );
		}
	}
	setPixmap( pm );
	//else
	//	setPixmap( rainbow( w, h ) );
}




CColorMapWidget::CColorMapWidget( QWidget *parent )		//parent = nullptr 
	: base_t( parent )
{
	mShowSolidColor = new QCheckBox( "Solid Color");
	mShowContour = new QCheckBox( "Contour");
	auto *contour_l = LayoutWidgets( Qt::Vertical, { mShowSolidColor, mShowContour }, nullptr, 2,2,2,2,2 );
	mColorTables = new QComboBox;
	mColorMapLabel = new CColorMapLabel;
	auto *color_map_l = LayoutWidgets( Qt::Vertical, { mColorTables, mColorMapLabel }, nullptr, 2,2,2,2,2 );

	LayoutWidgets( Qt::Horizontal, { contour_l, color_map_l }, this, 2,2,2,2,2 );

	connect( mShowContour, SIGNAL( toggled( bool ) ), this, SIGNAL( ShowContourToggled( bool ) ), Qt::QueuedConnection );
	connect( mShowSolidColor, SIGNAL( toggled( bool ) ), this, SIGNAL( ShowSolidColorToggled( bool ) ), Qt::QueuedConnection );

	connect( mColorTables, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleColorTablesIndexChanged( int ) ) );
}


void CColorMapWidget::SetShowContour( bool checked )
{
	mShowContour->setChecked( checked );		
}

void CColorMapWidget::SetShowSolidColor( bool checked )
{
	mShowSolidColor->setChecked( checked );	
}



void CColorMapWidget::SetLUT( CBratLookupTable *lut )
{
	mLut = lut;
	mColorTables->blockSignals( true );		//we don't want client code to think that the user changed the color map
	mColorTables->clear();
	for ( auto ii = mLut->GetColorTableList()->cbegin(); ii != mLut->GetColorTableList()->cend(); ++ii )
		mColorTables->addItem( ii->c_str() );	
	mColorTables->blockSignals( false );

	int index = mColorTables->findText( mLut->GetCurrentFunction().c_str() );
	if ( index == mColorTables->currentIndex() )
		HandleColorTablesIndexChanged( index );
	else
		mColorTables->setCurrentIndex( index );
}


void CColorMapWidget::HandleColorTablesIndexChanged( int index )
{
	auto name = q2a( itemText( index ) );
	if ( mLut )
		mLut->ExecMethod( name );

	mColorMapLabel->SetLUT( mLut );
	emit CurrentIndexChanged( index );
}





////////////////////////////////////////////////////////////////
//	 Color Button (Builds a button for selecting a color)
////////////////////////////////////////////////////////////////


//Definition of Static Variables
const QString CColorButton::smInitColor = "white";
const QString CColorButton::smStyleSheet_colorButton = "background-color: ";

//TODO: delete this:
//                                                        border-style: outset; \
//                                                        border-width: 1px; \
//                                                        border-color: black; \
//        qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 1 #dadbde, stop: 1 ";


//explicit
CColorButton::CColorButton( QWidget *parent )
    : base_t( parent)
    , mCurrentColor( smInitColor )
{
    //setFixedSize(24, 24);

    QIcon colorIcon = QIcon(":/images/OSGeo/paint.png");
    setIcon( colorIcon );
	setIconSize( QSize( tool_icon_size, tool_icon_size ) );

    mColorDisplay = new QWidget(this);
    mColorDisplay->setStyleSheet( smStyleSheet_colorButton + mCurrentColor.name() );
    mColorDisplay->setFixedSize( 18, 4 );
    mColorDisplay->move( QPoint( 3, 17 ) );

	connect( this, SIGNAL( clicked() ), this, SLOT( InputColor() ) );
}





////////////////////////////////////////////////////////////////
//	 Axis Tab (used to build the tab page with axis options)
////////////////////////////////////////////////////////////////
CAxisTab::CAxisTab( QWidget *parent, Qt::WindowFlags f )
    : base_t( parent, f)
{
	QFont axis_font = font();
	axis_font.setPointSize( axis_font.pointSize() - 1 );
	setFont( axis_font );
	const int height = fontMetrics().lineSpacing() + 6;
	const int width = 10 * fontMetrics().width( '0' );

	auto titled_edit = [&axis_font, &height, &width]( const char *title, QLineEdit *&l/*, Qt::Orientation o = Qt::Horizontal */) -> QLayout*
	{
		l = new QLineEdit;
		l->setMaximumWidth( width );
		l->setMaximumHeight( height );
		return LayoutWidgets( Qt::Horizontal, { nullptr, new QLabel( title ), l }, nullptr, 2, 0, 0, 0, 0 );
	};

	const int lm = 6;
	const int rm = 6;

    mAxisLabel = new QLineEdit;
	mAxisLabel->setMaximumWidth( width );
	mAxisLabel->setMaximumHeight( height );
    mLogScaleCheck = new QCheckBox( "Logarithmic" );

	auto *left_l = LayoutWidgets( Qt::Vertical, 
	{ 
		LayoutWidgets( Qt::Horizontal, { new QLabel( "Label" ), mAxisLabel }, nullptr, 0,0,0,0,0 ), mLogScaleCheck
	}, 
	nullptr, 2, lm, 0, rm, 0 );

	auto *ticks_l = LayoutWidgets( Qt::Horizontal,
	{
		titled_edit( "Max Ticks", mNbTicks ), titled_edit( "Digits", mNbDigits )
	}, 
	nullptr, 6, lm, 0, rm, 0 );
	
	auto *range_l = LayoutWidgets( Qt::Horizontal,
	{
		nullptr, titled_edit( "Min", mAxisMin ), titled_edit( "Max", mAxisMax )
	},
	nullptr, 6, lm, 0, rm, 0 );


	LayoutWidgets( Qt::Horizontal,
	{
		left_l, nullptr, LayoutWidgets( Qt::Vertical, { ticks_l, range_l }, nullptr, 4, lm, 0, rm, 0 )
	}, 
	this, 2, 2, 2, 2, 2 );

	setToolTip( 
		"Y=F(X) plots:\n"
		"Use mouse middle button to pan and mouse left or wheel buttons to zoom.\n"
		"\nZ=F(X,Y) plots:\n"
		"In 2D, use mouse left and right buttons to respectively zoom in and out.\n"
		"In 3D, use Alt (x,y) or Alt+Shift (z) with the mouse left button to \n"
		"zoom a particular axis." );
}



/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//								Plot View Tabs
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////
//		Curve Options Tab					
////////////////////////////////////////

CPlotControlsPanelCurveOptions::CPlotControlsPanelCurveOptions( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( parent, f )
{
    // 1. Plot List
    //
    mFieldsList = new QListWidget;
	auto *fields_group = CreateGroupBox( ELayoutType::Horizontal, { mFieldsList }, "Fields", nullptr );
	fields_group->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum );
	mFieldsList->setMaximumHeight( MaxSmallListsHeight( 5 ) );


    // 2. Spectrogram Options

	mColorMapWidget = new CColorMapWidget;
	mSpectrogramOptions = CreateGroupBox( ELayoutType::Horizontal, { mColorMapWidget }, "3D", nullptr );

    //
    // 3. Line Options
    //
    mLineColorButton = new CColorButton;
    mLineOpacityValue = new QLineEdit;
	mLineOpacityValue->setToolTip( "Press <enter> to assign value" );
    mStipplePattern = new QComboBox;
    mStipplePattern->setToolTip("Stipple pattern");
    mLineWidthValue = new QLineEdit;
	mLineWidthValue->setToolTip( "Press <enter> to assign value" );

    mLineOptions = CreateGroupBox( ELayoutType::Horizontal, 
	{
		new QLabel( "Plot Color" ), mLineColorButton, nullptr, new QLabel( "Opacity" ), mLineOpacityValue, nullptr,
		new QLabel( "Stipple pattern"), mStipplePattern, nullptr, new QLabel( "Width" ), mLineWidthValue
    }, 
	"Line", nullptr, s, m, m, m, m );
    mLineOptions->setCheckable( true );
    mLineOptions->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);


    // 4. Point Options
    //
    mPointColorButton = new CColorButton;
    mFillPointCheck = new QCheckBox;
    mPointGlyph = new QComboBox;
    mPointGlyph->setToolTip("Point Glyph");
    mPointSizeValue = new QLineEdit;
	mPointSizeValue->setToolTip( "Press <enter> to assign value" );

    mPointOptions = CreateGroupBox( ELayoutType::Horizontal, 
	{ 
		new QLabel( "Plot Color" ), mPointColorButton, nullptr, mFillPointCheck, new QLabel( "Fill point" ), nullptr,
		new QLabel( "Point Glyph"), mPointGlyph, nullptr, new QLabel( "Size" ), mPointSizeValue
	}, 
	"Point", nullptr, s, m, m, m, m );
    mPointOptions->setCheckable( true );
    mPointOptions->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);


    // 5. Histogram
    //
	mNumberOfBins = new QLineEdit;
    mHistogramOptions = CreateGroupBox( ELayoutType::Vertical, 
	{
		new QLabel( "Bins (recreates plot)" ), mNumberOfBins,
	}, 
	"Histogram", nullptr, s, m, m, m, m );


	//2D
	mBox2D = new QWidget( this );
	LayoutWidgets( Qt::Vertical, { mLineOptions, mPointOptions }, mBox2D );

	//3D
	mBox3D = new QWidget( this );
	LayoutWidgets( Qt::Vertical, { nullptr, mSpectrogramOptions, nullptr }, mBox3D );

	//Histogram
	mBoxHistogram = new QWidget( this );
	LayoutWidgets( Qt::Vertical, { nullptr, mHistogramOptions, nullptr }, mBoxHistogram );


    // 6. Grouping all widgets
     AddTopLayout( ELayoutType::Horizontal, 
	 { 
		 fields_group,
		 nullptr,
		 mBoxHistogram,
		 nullptr,
		 mBox3D,
		 nullptr,
		 mBox2D,
	}, 
	s, m, m, m, m );
}



void CPlotControlsPanelCurveOptions::SwitchTo2D()
{
	mBox2D->setVisible( true );
	mBox3D->setVisible( false );
	mBoxHistogram->setVisible( false );
	//setToolTip( "Press enter to \n assign values \n entered with \n the keyboard." );
}

void CPlotControlsPanelCurveOptions::SwitchTo3D()
{
	mBoxHistogram->setVisible( false );
	mBox2D->setVisible( false );
	mBox3D->setVisible( true );
	setToolTip( "" );
	//setToolTip( 
	//	"Y=F(X) plots:\n"
	//	"Use mouse middle button to pan and mouse left or wheel buttons to zoom.\n"
	//	"\nZ=F(X,Y) plots:\n"
	//	"In 2D, use mouse left and right buttons to respectively zoom in and out.\n"
	//	"In 3D, use Alt (x,y) or Alt+Shift (z) with the mouse left button to \n"
	//	"zoom a particular axis."
	//	, 3, 6, Qt::AlignCenter );
}

void CPlotControlsPanelCurveOptions::SwitchToHistogram( bool hide3d )
{
	mBox2D->setVisible( false );
	mBox3D->setVisible( !hide3d );
	mBoxHistogram->setVisible( true );
}






////////////////////////////////////////
//		Axis Options Tab					
////////////////////////////////////////

CPlotControlsPanelAxisOptions::CPlotControlsPanelAxisOptions( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( parent, f )
{
	QFont axis_font = font();
	axis_font.setPointSize( axis_font.pointSize() - 1 );
	setFont( axis_font );
	const int height = fontMetrics().lineSpacing() + 6;
	const int width = 10 * fontMetrics().width( '0' );

	auto shrinkh = [&height]( QWidget *w )
	{
		w->setMaximumHeight( height );
		return w;
	};

	auto shrink = [&width, &shrinkh]( QWidget *w )
	{
		shrinkh( w );
		w->setMaximumWidth( width );
		return w;
	};

	auto lshrink = [&shrinkh]( const std::string &text )
	{
		QLabel *l = new QLabel( text.c_str() );
		l->setAlignment( Qt::AlignCenter );
		shrinkh( l );
		return l;
	};

	auto eshrinkh = [&shrinkh]( QLineEdit *e )
	{
		e->setAlignment( Qt::AlignCenter );
		shrinkh( e );
		return e;
	};

	auto eshrink = [&width, &eshrinkh]( QLineEdit *e )
	{
		eshrinkh( e );
		e->setMaximumWidth( width );
		return e;
	};

	//mX_axis = new CAxisTab;
	//mY_axis = new CAxisTab;
	//mZ_axis = new CAxisTab;

	//mXScaleSpin = new QDoubleSpinBox;
	//mYScaleSpin = new QDoubleSpinBox;
	//mZScaleSpin = new QDoubleSpinBox;

    mXAxisLabel = new QLineEdit;
    mYAxisLabel = new QLineEdit;
    mZAxisLabel = new QLineEdit;

    mXNbTicks = new QLineEdit;
    mYNbTicks = new QLineEdit;
    mZNbTicks = new QLineEdit;

    mXNbDigits = new QLineEdit;
    mYNbDigits = new QLineEdit;
    mZNbDigits = new QLineEdit;

	mX2DScaleSpin = new QDoubleSpinBox;
	mY2DScaleSpin = new QDoubleSpinBox;
	mZ2DScaleSpin = new QDoubleSpinBox;

    mX2DScaleSpin->setRange( 0.1, 10000. );
    mX2DScaleSpin->setSingleStep( 0.1 );
    mX2DScaleSpin->setValue( 1. );

    mY2DScaleSpin->setRange( 0.1, 10000. );
    mY2DScaleSpin->setSingleStep( 0.1 );
    mY2DScaleSpin->setValue( 1. );

    mZ2DScaleSpin->setRange( 0.1, 10000. );
    mZ2DScaleSpin->setSingleStep( 0.1 );
    mZ2DScaleSpin->setValue( 1. );


    mXAxisMinMax = new QLineEdit;
    mYAxisMinMax = new QLineEdit;
    mZAxisMinMax = new QLineEdit;

    SetReadOnlyEditor( mXAxisMinMax, true );
    SetReadOnlyEditor( mYAxisMinMax, true );
    SetReadOnlyEditor( mZAxisMinMax, true );
	mXAxisMinMax->setFont( axis_font );			//SetReadOnlyEditor indirectly assigns font
	mYAxisMinMax->setFont( axis_font );			//idem
	mZAxisMinMax->setFont( axis_font );			//idem

	mX3DScaleSpin = new QDoubleSpinBox;
	mY3DScaleSpin = new QDoubleSpinBox;
	mZ3DScaleSpin = new QDoubleSpinBox;


    mX3DScaleSpin->setRange( 0.1, 10000. );
    mX3DScaleSpin->setSingleStep( 0.1 );
    mX3DScaleSpin->setValue( 1. );

    mY3DScaleSpin->setRange( 0.1, 10000. );
    mY3DScaleSpin->setSingleStep( 0.1 );
    mY3DScaleSpin->setValue( 1. );

    mZ3DScaleSpin->setRange( 0.1, 10000. );
    mZ3DScaleSpin->setSingleStep( 0.1 );
    mZ3DScaleSpin->setValue( 1. );

    AddTopLayout( ELayoutType::Grid,  
	{ 
		new QLabel("Axis"), lshrink("label"),		lshrink("ticks"),		lshrink("digits"),		lshrink("2D scale"),		lshrink("2D range"),		lshrink("3D scale"),	nullptr,
		lshrink("X"),		shrinkh( mXAxisLabel) ,	eshrink( mXNbTicks ),	eshrink( mXNbDigits ),	shrink( mX2DScaleSpin ),	eshrinkh( mXAxisMinMax ),	shrink( mX3DScaleSpin ), nullptr,
		lshrink("Y"),		shrinkh( mYAxisLabel ),	eshrink( mYNbTicks ),	eshrink( mYNbDigits ),	shrink( mY2DScaleSpin ),	eshrinkh( mYAxisMinMax ),	shrink( mY3DScaleSpin ), nullptr,
		lshrink("Z"),		shrinkh( mZAxisLabel ),	eshrink( mZNbTicks ),	eshrink( mZNbDigits ),	shrink( mZ2DScaleSpin ),	eshrinkh( mZAxisMinMax ),	shrink( mZ3DScaleSpin ), nullptr,
	}, 2, 2, 0, 2, 0 
	);


#if defined USE_AXIS_TABS
    mAxisOptionsTabs = new QTabWidget;
	mAxisOptionsTabs->setTabPosition( QTabWidget::South );

    mAxisOptionsTabs->addTab( mX_axis, "X" );
    mAxisOptionsTabs->addTab( mY_axis, "Y" );
    mAxisOptionsTabs->addTab( mZ_axis, "Z" );

    AddTopLayout( ELayoutType::Horizontal, { mAxisOptionsTabs } );
#else
#endif

 //   AddTopLayout( ELayoutType::Horizontal, 
	//{ 
	//	mX_axis, mY_axis, mZ_axis, LayoutWidgets( Qt::Vertical, { mXScaleSpin, mYScaleSpin, mZScaleSpin }, nullptr )
	//} 
	//);
}


void CPlotControlsPanelAxisOptions::SelectTab( int index )
{
	assert__( index < 3 );

#if defined USE_AXIS_TABS
	mAxisOptionsTabs->setCurrentIndex( index );
#else
	Q_UNUSED( index );
#endif
}


void CPlotControlsPanelAxisOptions::SwitchTo2D()
{
	//mZ_axis->setEnabled( false );
	mZAxisLabel->setEnabled( false );
	mZNbTicks->setEnabled( false );
	mZNbDigits->setEnabled( false );
	mZ2DScaleSpin->setEnabled( false );
	//mZAxisMinMax->setEnabled( false );

	mX3DScaleSpin->setEnabled( false );
	mY3DScaleSpin->setEnabled( false );
	mZ3DScaleSpin->setEnabled( false );

#if defined USE_AXIS_TABS
	if ( mAxisOptionsTabs->currentIndex() == 2 )
		SelectTab( 0 );
#endif
}

void CPlotControlsPanelAxisOptions::SwitchTo3D()
{
	//mZ_axis->setEnabled( true );
	mZAxisLabel->setEnabled( true );
	mZNbTicks->setEnabled( true );
	mZNbDigits->setEnabled( true );
	//mZ2DScaleSpin->setEnabled( true );
	//mZAxisMinMax->setEnabled( true );

	mX3DScaleSpin->setEnabled( true );
	mY3DScaleSpin->setEnabled( true );
	mZ3DScaleSpin->setEnabled( true );

	SelectTab( 2 );					//TODO delete when properly implemented
}








////////////////////////////////////////
//		Animation Options Tab					
////////////////////////////////////////

CPlotControlsPanelAnimateOptions::CPlotControlsPanelAnimateOptions( QWidget *parent, Qt::WindowFlags f )		//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( parent, f )
{
	mAnimateButton = CreateToolButton( "", ":/images/themes/default/media/media-play-16.png", CActionInfo::FormatTip("Animate") ); 
	mAnimateButton->setCheckable( true );
	mAnimateButton->setChecked( false );
	mStopButton = CreateToolButton( "", ":/images/themes/default/media/media-stop-16.png", CActionInfo::FormatTip("Stop") ); 
	mMoveForwardButton = CreateToolButton( "", ":/images/themes/default/media/media-seek-forward-16.png", CActionInfo::FormatTip("Forward animation") ); 
	mMoveForwardButton->setCheckable( true );
	mMoveForwardButton->setAutoExclusive( true );
	mMoveBackwardButton = CreateToolButton( "", ":/images/themes/default/media/media-seek-backward-16.png", CActionInfo::FormatTip("Backward animation") ); 
	mMoveBackwardButton ->setCheckable( true );
	mMoveBackwardButton ->setAutoExclusive( true );
	mLoopButton = CreateToolButton( "", ":/images/themes/default/media/media-loop.png", CActionInfo::FormatTip("Loop") );
	mLoopButton->setCheckable( true );
	mLoopButton->setChecked( false );
	mSpeedEdit = new QLineEdit; 

	mFrameIndexSpin = new QSpinBox;
    mFrameIndexSpin->setRange( 0, 100 );
    mFrameIndexSpin->setSingleStep( 1 );
    mFrameIndexSpin->setValue( 0 );

	QButtonGroup *direction_group = new QButtonGroup;
	direction_group->addButton( mMoveForwardButton );
	direction_group->addButton( mMoveBackwardButton );
    auto *options_row = CreateButtonRow( false, Qt::Horizontal, { nullptr, new QLabel( "Speed" ), mSpeedEdit, mMoveForwardButton, mMoveBackwardButton, mLoopButton }, 2 );
    auto *animate_row = CreateButtonRow( false, Qt::Horizontal, { mAnimateButton, mStopButton, new QLabel( "Frame" ), mFrameIndexSpin, nullptr }, 2 );

	auto *autoanimationgroup = CreateGroupBox( ELayoutType::Vertical,
	{
		animate_row,
		options_row,
		//LayoutWidgets( Qt::Horizontal, { new QLabel( "Speed" ), mSpeedEdit, nullptr, options_row, }, nullptr )
	}, "" );

	mInformation = new QTextEdit;
	SetReadOnlyEditor( mInformation, true );

	AddTopLayout( ELayoutType::Horizontal, { autoanimationgroup, CreateGroupBox( ELayoutType::Horizontal, { mInformation }, "" ) } );


	HandleAnimateButtonToggled( false );

	connect( mAnimateButton, SIGNAL(toggled(bool)), this, SLOT(HandleAnimateButtonToggled(bool)) );
}


void CPlotControlsPanelAnimateOptions::SetRange( unsigned int max )
{
    mFrameIndexSpin->setRange( 0, max );
}


//slot
void CPlotControlsPanelAnimateOptions::HandleFrameChanged( size_t iframe )
{					 
    mFrameIndexSpin->setValue( (int)iframe );
}


//slot
void CPlotControlsPanelAnimateOptions::HandleAnimateButtonToggled( bool toggled )
{
	std::string pix_path, tip;
	if ( toggled )
	{
		pix_path = ":/images/themes/default/media/media-pause-16.png";
		tip = "Pause";
	}
	else
	{
		pix_path = ":/images/themes/default/media/media-play-16.png";
		tip = "Animate";
	}

	mAnimateButton->setToolTip( tip.c_str() );

	QPixmap pix( pix_path.c_str() );
	QIcon icon(pix);	
	mAnimateButton->setIcon(icon);
	mAnimateButton->setIconSize(QSize(tool_icon_size, tool_icon_size));
	mSpeedEdit->setEnabled( !toggled );
	mFrameIndexSpin->setEnabled( !toggled );
}


void CPlotControlsPanelAnimateOptions::SwitchTo2D()
{
	setVisible( true );
}


void CPlotControlsPanelAnimateOptions::SwitchTo3D()
{
	setVisible( false );
}










/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//								Map View Tabs
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////
//			Map Edit Tab					
////////////////////////////////////////

CMapControlsPanelDataLayers::CMapControlsPanelDataLayers( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 );
    : base_t( parent, f )
{
    // III. Data Layers
    //
    mFieldsList = new QListWidget;
    mFieldsList->setToolTip("Data Layer");

    mColorMapWidget = new CColorMapWidget( this );
	mColorMapWidget->setMaximumSize( 200, 60 );
    auto mNbLabels = new QLineEdit;

    mMinRange = new QLineEdit;
    mMaxRange = new QLineEdit;
    mReset = new QPushButton( "Reset" );

	auto *color_l = CreateGroupBox( ELayoutType::Horizontal,
	{
		mColorMapWidget,
		LayoutWidgets( Qt::Vertical, { new QLabel( "# Labels (Color Bar)" ), mNbLabels }, nullptr ),
	},
	"", nullptr, 2, m, m, m, m );

	auto *range_box = CreateGroupBox( ELayoutType::Vertical, 
	{ 
		LayoutWidgets( Qt::Horizontal, { new QLabel( "Min." ), mMinRange }, nullptr, s, m, m, m, m ),
        LayoutWidgets( Qt::Horizontal, { new QLabel( "Max." ), mMaxRange }, nullptr, s, m, m, m, m ),
		mReset 
	}, 
	"Range", nullptr );

    AddTopLayout( ELayoutType::Horizontal, 
	{
		mFieldsList, 
        nullptr,
		color_l,
        nullptr,
		range_box
	}, 
	s, m, m, m, m );


	//TODO delete remaining lines after implementation
	mColorMapWidget->mShowContour->setEnabled( false );
	mNbLabels->setEnabled( false );

    mMinRange->setEnabled( false );
    mMaxRange->setEnabled( false );
    mReset->setEnabled( false );
}



////////////////////////////////////////
//			Map Options Tab
////////////////////////////////////////

void CMapControlsPanelView::CreateWidgets()
{

}

CMapControlsPanelView::CMapControlsPanelView( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( parent, f )
{
    // IV. View Options
    //
    ViewOptionsTabs = new QTabWidget();

    //    IV.1 State tab
    auto stateSpinBox = new QSpinBox;
    stateSpinBox->setValue(0);
    //stateSpinBox->setRange(0,100);

    auto mSaveState  = new QPushButton("Save");
    auto mClearState = new QPushButton("Clear");
    auto mFullState  = new QPushButton("Full");

    QGroupBox *StateTab = CreateGroupBox( ELayoutType::Horizontal, { stateSpinBox, nullptr, mSaveState, mClearState, mFullState } );

    ViewOptionsTabs->addTab(StateTab, "State");

    //    IV.2 Zoom tab
    auto mLon1 = new QLineEdit(this);
    auto mLon2 = new QLineEdit(this);
    auto mLat1 = new QLineEdit(this);
    auto mLat2 = new QLineEdit(this);

    mLon1->setText("-180");
    mLon2->setText("180");
    mLat1->setText("-90");
    mLat2->setText("90");

    auto mZoomGo  = new QPushButton("Go");
    QFrame *ZoomlineHoriz = WidgetLine( nullptr, Qt::Horizontal );

    QGroupBox *ZoomTab = CreateGroupBox( ELayoutType::Vertical, {
                                             LayoutWidgets( Qt::Horizontal, { new QLabel( "Lon1:" ), mLon1, nullptr, new QLabel( "Lat1:" ), mLat1 }),
                                             LayoutWidgets( Qt::Horizontal, { ZoomlineHoriz, mZoomGo }),
                                             LayoutWidgets( Qt::Horizontal, { new QLabel( "Lon2:" ), mLon2, nullptr, new QLabel( "Lat2:" ), mLat2 }),
                                                                  } );

    ViewOptionsTabs->addTab(ZoomTab, "Zoom");

    //    IV.3 Z(height) tab
    auto ZRadius = new QSpinBox;
    auto ZFactor = new QSpinBox;

    ZRadius->setValue(0);
    ZFactor->setValue(0);

    QGroupBox *ZTab = CreateGroupBox( ELayoutType::Horizontal, { new QLabel( "Radius" ), ZRadius, nullptr, new QLabel( "Factor" ), ZFactor } );

    ViewOptionsTabs->addTab(ZTab, "Z (height)");


    ViewOptionsTabs->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    AddTopWidget( ViewOptionsTabs );
}





///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ViewControlPanels.cpp"
