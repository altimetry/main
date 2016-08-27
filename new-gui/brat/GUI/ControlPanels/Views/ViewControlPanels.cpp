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

#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/Workspaces/Display.h"
#include "DataModels/PlotData/BratLookupTable.h"

#include "new-gui/Common/GUI/TextWidget.h"
#include "GUI/ActionsTable.h"
#include "GUI/ControlPanels/ColorButton.h"

#include "ViewControlPanels.h"



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

	auto titled_edit = [&axis_font, &height, &width]( const char *title, QLineEdit *&l ) -> QLayout*	//, Qt::Orientation o = Qt::Horizontal
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
    // 1. Fields List
    //
    mFieldsList = new QListWidget;
	auto *fields_group = CreateGroupBox( ELayoutType::Horizontal, { mFieldsList }, "Fields", nullptr );
	fields_group->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum );
	mFieldsList->setMaximumHeight( MaxSmallListsHeight( 5 ) );


    // 2. Spectrogram Options

	mColorMapWidget = new CColorMapWidget( false, true, nullptr );
	mColorMapWidget->HideContourColorWidget();
	mColorMapWidget->HideContourWidthWidget();
	mColorMapWidget->SetContoursTitle( "2D Contours" );
	mSpectrogramOptions = CreateGroupBox( ELayoutType::Horizontal, { mColorMapWidget }, "", nullptr );

    //
    // 3. Line Options
    //
    mLineColorButton = new CColorButton;
    mLineOpacityValue = new QLineEdit;								mLineOpacityValue->setValidator( new QRegExpValidator( QRegExp( "[0-9]+" ) ) );
	mLineOpacityValue->setToolTip( "Press <enter> to assign value" );
    mStipplePattern = new QComboBox;
    mStipplePattern->setToolTip("Stipple pattern");
    mLineWidthValue = new QLineEdit;								mLineWidthValue->setValidator( new QRegExpValidator( QRegExp( "[0-9]+" ) ) );

    mLineOptions = CreateGroupBox( ELayoutType::Horizontal, 
	{
		new QLabel( "Color" ), mLineColorButton, nullptr, new QLabel( "Opacity" ), mLineOpacityValue, nullptr,
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
    mPointSizeValue = new QLineEdit;								mPointSizeValue->setValidator( new QRegExpValidator( QRegExp( "[0-9]+" ) ) );

    mPointOptions = CreateGroupBox( ELayoutType::Horizontal, 
	{ 
		new QLabel( "Color" ), mPointColorButton, nullptr, mFillPointCheck, new QLabel( "Fill point" ), nullptr,
		new QLabel( "Glyph"), mPointGlyph, nullptr, new QLabel( "Size" ), mPointSizeValue
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
	LayoutWidgets( Qt::Vertical, { mSpectrogramOptions }, mBox3D );

	//Histogram
	mBoxHistogram = new QWidget( this );
	LayoutWidgets( Qt::Vertical, { nullptr, mHistogramOptions, nullptr }, mBoxHistogram );


    // 6. Grouping all widgets
     AddTopLayout( ELayoutType::Horizontal, 
	 { 
		 fields_group,
		 //nullptr,
		 mBoxHistogram,
		 //nullptr,
		 mBox3D,
		 //nullptr,
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

//static 
const double CPlotControlsPanelAxisOptions::smMaxScale = 1000000.;


CPlotControlsPanelAxisOptions::CPlotControlsPanelAxisOptions( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( parent, f )
{
	QFont axis_font = font();
	axis_font.setPointSize( axis_font.pointSize() - 1 );
	setFont( axis_font );
	const int height = fontMetrics().lineSpacing() + 6;
	const int width = 12 * fontMetrics().width( '0' );

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

	auto wcenter = [this]( QWidget *w )
	{
		QWidget *parent = new QWidget;
		LayoutWidgets( Qt::Horizontal, { w }, parent )->setAlignment( Qt::AlignHCenter );
		return parent;
	};

	//

    mXAxisLabel = new QLineEdit;
    mYAxisLabel = new QLineEdit;
    mZAxisLabel = new QLineEdit;

    mXNbTicks = new QLineEdit;	mXNbTicks->setValidator( new QRegExpValidator( QRegExp( "[0-9]+" ) ) );
    mYNbTicks = new QLineEdit;	mYNbTicks->setValidator( new QRegExpValidator( QRegExp( "[0-9]+" ) ) );
    mZNbTicks = new QLineEdit;	mZNbTicks->setValidator( new QRegExpValidator( QRegExp( "[0-9]+" ) ) );

    mXNbDigits = new QLineEdit;	mXNbDigits->setValidator( new QRegExpValidator( QRegExp( "[0-9]+" ) ) );
    mYNbDigits = new QLineEdit;	mYNbDigits->setValidator( new QRegExpValidator( QRegExp( "[0-9]+" ) ) );
    mZNbDigits = new QLineEdit;	mZNbDigits->setValidator( new QRegExpValidator( QRegExp( "[0-9]+" ) ) );

	mX2DScaleSpin = new QDoubleSpinBox;
	mY2DScaleSpin = new QDoubleSpinBox;
	mZ2DScaleSpin = new QDoubleSpinBox;

    mX2DScaleSpin->setRange( 0.1, smMaxScale );
    mX2DScaleSpin->setSingleStep( 0.1 );
    mX2DScaleSpin->setValue( 1. );

    mY2DScaleSpin->setRange( 0.1, smMaxScale );
    mY2DScaleSpin->setSingleStep( 0.1 );
    mY2DScaleSpin->setValue( 1. );

    mZ2DScaleSpin->setRange( 0.1, smMaxScale );
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


    mX3DScaleSpin->setRange( 0.1, smMaxScale );
    mX3DScaleSpin->setSingleStep( 0.1 );
    mX3DScaleSpin->setValue( 1. );

    mY3DScaleSpin->setRange( 0.1, smMaxScale );
    mY3DScaleSpin->setSingleStep( 0.1 );
    mY3DScaleSpin->setValue( 1. );

    mZ3DScaleSpin->setRange( 0.1, smMaxScale );
    mZ3DScaleSpin->setSingleStep( 0.1 );
    mZ3DScaleSpin->setValue( 1. );

    mXLogScaleCheck = new QCheckBox;		mXLogScaleCheck->setVisible( false );
	mYLogScaleCheck = new QCheckBox;		mYLogScaleCheck->setVisible( false );
    mZLogScaleCheck = new QCheckBox;


    AddTopLayout( ELayoutType::Grid,  
	{ 
		new QLabel("Axis"), lshrink("label"),		lshrink("ticks"),		lshrink("digits"),		lshrink("2D scale"),		lshrink("2D scale range"),	lshrink("3D scale"),	lshrink("3D log"),	nullptr,
		lshrink("X"),		shrinkh( mXAxisLabel) ,	eshrink( mXNbTicks ),	eshrink( mXNbDigits ),	shrink( mX2DScaleSpin ),	eshrinkh( mXAxisMinMax ),	shrink( mX3DScaleSpin ), wcenter( mXLogScaleCheck ), nullptr,
		lshrink("Y"),		shrinkh( mYAxisLabel ),	eshrink( mYNbTicks ),	eshrink( mYNbDigits ),	shrink( mY2DScaleSpin ),	eshrinkh( mYAxisMinMax ),	shrink( mY3DScaleSpin ), wcenter( mYLogScaleCheck ), nullptr,
		lshrink("Z"),		shrinkh( mZAxisLabel ),	eshrink( mZNbTicks ),	eshrink( mZNbDigits ),	shrink( mZ2DScaleSpin ),	eshrinkh( mZAxisMinMax ),	shrink( mZ3DScaleSpin ), wcenter( mZLogScaleCheck ), nullptr,
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

	mXLogScaleCheck->setEnabled( false );
	mYLogScaleCheck->setEnabled( false );
	mZLogScaleCheck->setEnabled( false );

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

	mXLogScaleCheck->setEnabled( true );
	mYLogScaleCheck->setEnabled( true );
	mZLogScaleCheck->setEnabled( true );

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

	mInformation = new CTextWidget;
	SetReadOnlyEditor( mInformation, true );
	mInformation->SetSizeHint( mInformation->sizeHint().width(), 4 * fontMetrics().lineSpacing() );

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
//			Map Layers Tab					
////////////////////////////////////////

CMapControlsPanelDataLayers::CMapControlsPanelDataLayers( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 );
    : base_t( parent, f )
{
    // 1. Fields List
    //
    mFieldsList = new QListWidget;
	auto *fields_group = CreateGroupBox( ELayoutType::Horizontal, { mFieldsList }, "Fields", nullptr );
	fields_group->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum );
	mFieldsList->setMaximumHeight( MaxSmallListsHeight( 5 ) );

	mMagnitudeFactorEdit = new QLineEdit;			mMagnitudeFactorEdit->setValidator( new QRegExpValidator( QRegExp( "[.0-9]+" ) ) );
    mMagnitudeFactorEdit->setToolTip("Scaling Factor for Vectors");
    mMagnitudeFactorGroupBox = CreateGroupBox( ELayoutType::Vertical, { mMagnitudeFactorEdit }, "Vector Scale", nullptr );
	mMagnitudeFactorGroupBox->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );

    mColorMapWidget = new CColorMapWidget( false, true, this );
	mColorMapWidget->ShowPrecisionParameters( true );

	auto *color_l = CreateGroupBox( ELayoutType::Horizontal,
	{
		mColorMapWidget,
	},
	"", nullptr, 2, m, m, m, m );
	//color_l->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


    AddTopLayout( ELayoutType::Horizontal, 
	{
		fields_group,
		mMagnitudeFactorGroupBox,
		color_l,
	}, 
	s, m, m, m, m );
}



////////////////////////////////////////
//			Map View Tab
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
