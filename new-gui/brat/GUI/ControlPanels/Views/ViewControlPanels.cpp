#include "new-gui/brat/stdafx.h"

#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/Workspaces/Display.h"

#include "GUI/DisplayWidgets/TextWidget.h"

#include "ViewControlPanels.h"



/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//							Specialized Widgets
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


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
	auto short_line_edit = [this]()
	{
		auto *line = new QLineEdit(this);
		line->setMaximumWidth( 60 );
		return line;
	};

	const int vm = 0;	//vertical margins

    // 1. Axis label and Scale
	//
    mAxisLabel = new QLineEdit;
	mAxisLabel->setToolTip("Axis label");

    mLogScaleCheck = new QCheckBox( "Logarithmic Scale" );

    // 2. Nb. Ticks, Base and Digits
	//
    mNbTicks = short_line_edit();
    mBase = short_line_edit();
    mNbDigits = short_line_edit();

    // 3. Range
	//
    mReset = new QPushButton("Reset");
    mReset->setAutoDefault(false);
    mAxisMax = short_line_edit();
    mAxisMin = short_line_edit();


    QGroupBox *range_group = CreateGroupBox( ELayoutType::Horizontal, 
	{
		mReset, nullptr, new QLabel("Min"), mAxisMin, nullptr, new QLabel("Max"), mAxisMax,
	}, 
	"Range");


    // 4. Creating Tab Group
    QGroupBox *tab_group = CreateGroupBox( ELayoutType::Vertical, 
	{
		::LayoutWidgets( Qt::Horizontal, 
		{
			new QLabel("Axis Label"), mAxisLabel, nullptr, mLogScaleCheck
		}, 
		nullptr, 2, 2, vm, 2, vm ),

		::LayoutWidgets( Qt::Horizontal, 
		{
			new QLabel( "Ticks" ), mNbTicks, nullptr, new QLabel( "Base" ), mBase, nullptr, new QLabel( "Digits" ), mNbDigits
		}, 
		nullptr, 2, 2, vm, 2, vm ),

		range_group,

	}, "", nullptr, 4, 4, vm, 4, vm );
    //tab_group->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );


	// 5. help text
	QPushButton *b = new QPushButton( "Help" );
	b->setCheckable( true );
	auto help = new CTextWidget;	//use spaces and/or try to have lines without very dissimilar lengths
	help->SetHelpProperties( 
		"Y=F(X) plots:\n"
		"Use mouse middle button to pan and mouse left or wheel buttons to zoom.\n"
		"\nZ=F(X,Y) plots:\n"
		"In 2D, use mouse left and right buttons to respectively zoom in and out.\n"
		"In 3D, use Alt (x,y) or Alt+Shift (z) with the mouse left button to \n"
		"zoom a particular axis."
		, 3, 6, Qt::AlignCenter );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Maximum );
    help_group->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum );
	connect( b, SIGNAL( toggled( bool ) ), help_group, SLOT( setVisible(bool) ) );
	help_group->setVisible( false );

	LayoutWidgets( Qt::Horizontal, { tab_group, b, help_group }, this, 2,2,2,2,2 );
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
	fields_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


    // 2. Spectrogram Options

	mShowSolidColor = new QCheckBox( "Solid Color");
	mShowContour = new QCheckBox( "Contour");
	mSpectrogramOptions = CreateGroupBox( ELayoutType::Vertical, { mShowSolidColor, mShowContour }, "", nullptr );


    //
    // 3. Line Options
    //
    mLineColorButton = new CColorButton();
    mLineOpacityValue = new QLineEdit(this);
	mLineOpacityValue->setToolTip( "Press <enter> to assign value" );
    mStipplePattern = new QComboBox;
    mStipplePattern->setToolTip("Stipple pattern");
    mLineWidthValue = new QLineEdit(this);
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
    mPointColorButton = new CColorButton();
    mFillPointCheck   = new QCheckBox(this);
    mPointGlyph = new QComboBox;
    mPointGlyph->setToolTip("Point Glyph");
    mPointSizeValue = new QLineEdit(this);
	mPointSizeValue->setToolTip( "Press <enter> to assign value" );

    mPointOptions = CreateGroupBox( ELayoutType::Horizontal, 
	{ 
		new QLabel( "Plot Color" ), mPointColorButton, nullptr, mFillPointCheck, new QLabel( "Fill point" ), nullptr,
		new QLabel( "Point Glyph"), mPointGlyph, nullptr, new QLabel( "Size" ), mPointSizeValue
	}, 
	"Point", nullptr, s, m, m, m, m );
    mPointOptions->setCheckable( true );
    mPointOptions->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

	//2D
	auto *box2d = CreateGroupBox( ELayoutType::Vertical, { mLineOptions, mPointOptions }, "2D", nullptr );

	//3D
	auto *box3d = CreateGroupBox( ELayoutType::Vertical, { nullptr, mSpectrogramOptions, nullptr }, "3D", nullptr );


	// 5. help text
	auto help = new CTextWidget;
	help->SetHelpProperties( "Press enter to \n assign values \n entered with \n the keyboard.", 0, 6, Qt::AlignCenter );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum );
    //help_group->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum );

    // 6. Grouping all widgets
     AddTopGroupBox( ELayoutType::Horizontal, 
	 { 
		 fields_group,
		 nullptr,
		 box3d,
		 box2d,
		 help_group
	}, 
	"", s, m, m, m, m );
}




////////////////////////////////////////
//		Axis Options Tab					
////////////////////////////////////////

CPlotControlsPanelAxisOptions::CPlotControlsPanelAxisOptions( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( parent, f )
{
    // V. Axes Options
    //
    mAxisOptionsTabs = new QTabWidget;
	mAxisOptionsTabs->setTabPosition( QTabWidget::West );

	mX_axis = new CAxisTab;
	mY_axis = new CAxisTab;
	mZ_axis = new CAxisTab;

    mAxisOptionsTabs->addTab( mX_axis, "X" );
    mAxisOptionsTabs->addTab( mY_axis, "Y" );
    mAxisOptionsTabs->addTab( mZ_axis, "Z/Y2" );

    AddTopGroupBox( ELayoutType::Horizontal, { mAxisOptionsTabs } );
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
    auto mNbLabels = new QLineEdit(this);

    mShowSolidColorCheck = new QCheckBox( "Show Solid Color", this );
    auto mSolidColorEdit = new QPushButton("Edit");
    auto mShowContourCheck = new QCheckBox( "Show Contour", this );
    auto mContourEdit = new QPushButton("Edit");

    QFrame *lineHorizontal = WidgetLine( nullptr, Qt::Horizontal );

    auto mMinRange = new QLineEdit(this);
    mMinRange->setText("0");
    auto mMaxRange = new QLineEdit(this);
    mMaxRange->setText("0");
    auto mReset = new QPushButton("Reset");

    QLayout *group = LayoutWidgets( Qt::Vertical, 
		{
			LayoutWidgets( Qt::Horizontal, { new QLabel( "Number of Labels (Color Bar)" ), mNbLabels } ),
			LayoutWidgets( Qt::Horizontal, { mShowSolidColorCheck, mSolidColorEdit } ),
			LayoutWidgets( Qt::Horizontal, { mShowContourCheck, mContourEdit } ),
		}, 
		nullptr, 2, m, m, m, m );

    AddTopLayout( ELayoutType::Vertical, 
	{
		LayoutWidgets( Qt::Horizontal, { mFieldsList, group }, nullptr, s, m, m, m, m ),
        lineHorizontal,
        LayoutWidgets( Qt::Horizontal, 
		{ 
			new QLabel( "Range:" ), nullptr, new QLabel( "Min." ), mMinRange, nullptr, new QLabel( "Max." ), mMaxRange, nullptr, mReset 
		} )
	}, 
	s, m, m, m, m );


	//TODO delete remaining lines after implementation
	mShowContourCheck->setEnabled( false );
	mSolidColorEdit->setEnabled( false );
	mContourEdit->setEnabled( false );
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
