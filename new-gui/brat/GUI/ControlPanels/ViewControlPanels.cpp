#include "new-gui/brat/stdafx.h"

#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/Workspaces/Display.h"

#include "ViewControlPanels.h"


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
{
    mCurrentColor = QColor(smInitColor);
    setFixedSize(24, 24);

    QIcon colorIcon = QIcon(":/images/OSGeo/paint.png");
    setIcon(colorIcon);

    mColorDisplay = new QWidget(this);
    mColorDisplay->setStyleSheet(smStyleSheet_colorButton + mCurrentColor.name());
    mColorDisplay->setFixedSize(18, 4);
    mColorDisplay->move(QPoint(3,17));
}

void CColorButton::SetColor()
{
    QColor color = QColorDialog::getColor(mCurrentColor, nullptr);

    if (color.isValid())
    {
        mCurrentColor = color;
        mColorDisplay->setStyleSheet(smStyleSheet_colorButton + mCurrentColor.name());
        //mColorDisplay->setPalette(QPalette(mCurrentColor));
    }
}



////////////////////////////////////////////////////////////////
//	 Axis Tab (used to build the tab page with axis options)
////////////////////////////////////////////////////////////////
CAxisTab::CAxisTab( QWidget *parent, Qt::WindowFlags f )
    : base_t( parent, f)
{
	const int vm = 0;	//vertical margins

    // 1. Axis label and Scale
    auto mAxisLabel = new QLineEdit();
    mAxisLabel->setText("Axis label text");
    mAxisLabel->setToolTip("Axis label");

    mLogScaleCheck = new QCheckBox();

    // 2. Nb. Ticks, Base and Digits
    auto mNbTicks  = new QLineEdit();
    auto mBase     = new QLineEdit();
    auto mNbDigits = new QLineEdit();

    mNbTicks ->setText("0");
    mBase    ->setText("0");
    mNbDigits->setText("0");

    // 3. Fallback Range
    auto mFromDataCheck = new QCheckBox();
    auto mFromUserCheck = new QCheckBox();

    auto mCurrent1 = new QLineEdit();
    auto mCurrent2 = new QLineEdit();
    auto mRange1   = new QLineEdit();
    auto mRange2   = new QLineEdit();

    QBoxLayout *Ranges1_layout = ::LayoutWidgets( Qt::Vertical, { mCurrent1, mRange1 }, nullptr, 2, 2, vm, 2, vm );
    QBoxLayout *Ranges2_layout = ::LayoutWidgets( Qt::Vertical, { mCurrent2, mRange2 }, nullptr, 2, 2, vm, 2, vm );
    QBoxLayout *Range_labels   = ::LayoutWidgets( Qt::Vertical, { new QLabel( "Current" ), new QLabel( "Range" ) }, nullptr, 2, 2, vm, 2, vm );


    QGroupBox *FallBackRange_group = CreateGroupBox(ELayoutType::Horizontal, {
                                                mFromDataCheck, new QLabel( "From Data" ), nullptr, mFromUserCheck, new QLabel( "From User" ),
                                                Ranges1_layout,
                                                nullptr,
                                                Range_labels,
                                                Ranges2_layout
												}, "Fallback Range");

    // 4. Creating Tab Group
    QGroupBox *TabGroup = CreateGroupBox( ELayoutType::Vertical, 
	{
		::LayoutWidgets( Qt::Horizontal, {
			mAxisLabel, nullptr, mLogScaleCheck, new QLabel( "Logarithmic Scale" )
		}, nullptr, 2, 2, vm, 2, vm ),

		::LayoutWidgets( Qt::Horizontal, {
			new QLabel( "Nb of Ticks:" ), mNbTicks, nullptr, new QLabel( "Base:" ), mBase, nullptr, new QLabel( "Nb of Digits:" ), mNbDigits
		}, nullptr, 2, 2, vm, 2, vm ),

		FallBackRange_group,

	}, "", nullptr, 4, 4, vm, 4, vm );

    AddWidget( this, TabGroup );

	Wire();
}


void CAxisTab::Wire()
{
	connect( mLogScaleCheck, SIGNAL( toggled( bool ) ), this, SIGNAL( LogarithmicScale( bool ) ), Qt::QueuedConnection );
}




/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//							View Common Tabs
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////
//			General Tab					
////////////////////////////////////////


void CViewControlsPanelGeneral::Wire()
{
	if ( mCmdLineProcessor )
	{
		mOpenAplot->addItem( t2q( mCmdLineProcessor->ParamFile() ) );
	}
	else
	for ( auto *display : mDisplays )
	{
		mOpenAplot->addItem( t2q( display->GetName() ) );
	}
	mOpenAplot->setCurrentIndex( 0 );

	connect( mRunPlot, SIGNAL( clicked() ), this, SLOT( RunButtonClicked() ), Qt::QueuedConnection );
	connect( mOpenAplot, SIGNAL( currentIndexChanged(int) ), this, SIGNAL( CurrentDisplayIndexChanged(int) ), Qt::QueuedConnection );
	connect( mNewPlot, SIGNAL( clicked() ), this, SIGNAL( NewButtonClicked() ), Qt::QueuedConnection );
}

void CViewControlsPanelGeneral::CreateWidgets()
{
    // I. Plot selection buttons and info
    //
    //    I.1 Selection buttons
    auto mPlotName   = new QLineEdit(this);
    mPlotName->setText("Plot name");

    mNewPlot		 = new QPushButton( "New" );
    auto mSavePlot   = new QPushButton( "Save" );
    auto mDeletePlot = new QPushButton( "Delete" );
    mRunPlot		 = new QPushButton( "Run" );

    mOpenAplot  = new QComboBox;

    mOpenAplot ->setToolTip( "Open a plot" );

    //    Adding previous widgets to this...
    AddTopGroupBox( ELayoutType::Vertical, {
                                 LayoutWidgets( Qt::Horizontal, { mPlotName, mNewPlot, mSavePlot, mDeletePlot, mRunPlot, mOpenAplot } ),
                                            }, "", 4, 4, 4, 4, 4 );
	Wire();
}


CViewControlsPanelGeneral::CViewControlsPanelGeneral( const CDisplayFilesProcessor *cmd_line_processor, QWidget *parent, Qt::WindowFlags f )		//parent = nullptr, f = 0
    : base_t( parent, f )
	, mCmdLineProcessor( cmd_line_processor )
{
	CreateWidgets();
}


CViewControlsPanelGeneral::CViewControlsPanelGeneral( const std::vector< const CDisplay* > &displays, QWidget *parent, Qt::WindowFlags f )		//parent = nullptr, f = 0
    : base_t( parent, f )
	, mCmdLineProcessor( nullptr )
	, mDisplays( displays )
{
	CreateWidgets();
}



void CViewControlsPanelGeneral::RunButtonClicked()
{
	int index = mOpenAplot->currentIndex();
	if ( index >= 0 )
		emit RunButtonClicked( index );
}




////////////////////////////////////////
//			Plots Tab					
////////////////////////////////////////


CViewControlsPanelPlots::CViewControlsPanelPlots( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 );
    : base_t( parent, f )
{
    //    II.2 Table with map plot information
    mPlotInfoTable = new QTableWidget;
    mPlotInfoTable->setSortingEnabled( true );                             // Enabling sorting
    mPlotInfoTable->setAlternatingRowColors( true );                       // Alternating row colors
    mPlotInfoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);    // Disable editable table items
    //mPlotInfoTable->setSelectionBehavior( QAbstractItemView::SelectRows ); // Whole row selection
    //mPlotInfoTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    mPlotInfoTable->setMinimumWidth( ComputeTableWidth( mPlotInfoTable ) );  // Setting minimum width

    //    Adding previous widgets to this...
    AddTopGroupBox( ELayoutType::Vertical, {
                                 mPlotInfoTable,
                                            }, "", 4, 4, 4, 4, 4 );
}



/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//								Plot View Tabs
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////
//		Curve Edit/Data Tab					
////////////////////////////////////////

CPlotControlsPanelEdit::CPlotControlsPanelEdit( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( parent, f )
{
    // II. Plot selection buttons and info
    //
    //    II.1 Selection buttons

    mSelectplot = new QComboBox;
    mvarX       = new QComboBox;
    mvarY       = new QComboBox;
    mvarY2      = new QComboBox;
    mvarZ       = new QComboBox;

    mSelectplot->setToolTip( "Select a plot type");
    mvarX      ->setToolTip( "X" );
    mvarY      ->setToolTip( "Y");
    mvarY2     ->setToolTip( "Y2");
    mvarZ      ->setToolTip( "Z" );

    //    II.3 Link to Plot
    auto mPlotTitle   = new QLineEdit(this);
    mPlotTitle->setText("Plot title");

    mLinkToPlot  = new QComboBox;
    mLinkToPlot->setToolTip("Link to Plot");


    //    Adding previous widgets to this...
    AddTopGroupBox( ELayoutType::Vertical, {
                                 LayoutWidgets( Qt::Horizontal, { mSelectplot, new QLabel( "Add data:" ), mvarX, mvarY, mvarY2, mvarZ } ),
                                 LayoutWidgets( Qt::Horizontal, { mPlotTitle, mLinkToPlot } )
                                            }, "", 4, 4, 4, 4, 4 );
}



////////////////////////////////////////
//		Curve Options Tab					
////////////////////////////////////////

CPlotControlsPanelCurveOptions::CPlotControlsPanelCurveOptions( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( parent, f )
{
    // III. Line Options
    //
    mLineColorButton = new CColorButton();

    auto mOpacityValue   = new QLineEdit(this);
    mOpacityValue->setText("0.1");

    mStipplePattern = new QComboBox;
    mStipplePattern->setToolTip("Stipple pattern");

    auto mWidthValue = new QLineEdit(this);
    mWidthValue->setText("0.5");

    mLineOptions = AddTopGroupBox( ELayoutType::Horizontal, { new QLabel( "Plot Color" ),
                                                                       mLineColorButton,
                                                                       nullptr,
                                                                       new QLabel( "Opacity" ),
                                                                       mOpacityValue,
                                                                       nullptr,
                                                                       new QLabel( "Stipple pattern"),
                                                                       mStipplePattern,
                                                                       nullptr,
                                                                       new QLabel( "Width" ),
                                                                       mWidthValue
                                                                      }, "Line", 4, 4, 4, 4, 4 );
	mLineOptions->setCheckable( true );
    mLineOptions->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);


    // IV. Point Options
    //
    mPointColorButton = new CColorButton();

    auto mFillPointCheck   = new QCheckBox(this);

    mPointGlyph = new QComboBox;
    mPointGlyph->setToolTip("Point Glyph");

    auto mSizeValue = new QLineEdit(this);
    mSizeValue->setText("0.5");

    mPointOptions = AddTopGroupBox( ELayoutType::Horizontal, { new QLabel( "Plot Color" ),
                                                                         mPointColorButton,
                                                                         nullptr,
                                                                         mFillPointCheck,
                                                                         new QLabel( "Fill point" ),
                                                                         nullptr,
                                                                         new QLabel( "Point Glyph"),
                                                                         mPointGlyph,
                                                                         nullptr,
                                                                         new QLabel( "Size" ),
                                                                         mSizeValue
                                                                      }, "Point", 4, 4, 4, 4, 4 );

	mPointOptions->setCheckable( true );
    mPointOptions->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

	Wire();
}


void CPlotControlsPanelCurveOptions::Wire()
{
    connect( mLineColorButton, SIGNAL( clicked() ), mLineColorButton, SLOT( SetColor() ) );
    connect( mPointColorButton, SIGNAL( clicked() ), mPointColorButton, SLOT( SetColor() ) );

	connect( mPointOptions, SIGNAL( toggled( bool ) ), this, SLOT( HandlePointOptionsChecked( bool ) ) );
	connect( mLineOptions, SIGNAL( toggled( bool ) ), this, SLOT( HandleLineOptionsChecked( bool ) ) );

	mLineOptions->setChecked( true );
	mPointOptions->setChecked( false );
}


void CPlotControlsPanelCurveOptions::HandleLineOptionsChecked( bool checked )
{
	mPointOptions->setChecked( !checked );
	emit LineOptionsChecked( checked );
}

void CPlotControlsPanelCurveOptions::HandlePointOptionsChecked( bool checked )
{
	mLineOptions->setChecked( !checked );
	emit PointOptionsChecked( checked );
}





////////////////////////////////////////
//		Axis Options Tab					
////////////////////////////////////////

CPlotControlsPanelAxisOptions::CPlotControlsPanelAxisOptions( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( parent, f )
{
    // V. Axes Options
    //
    mAxisOptionsTabs = new QTabWidget();
	mAxisOptionsTabs->setTabPosition( QTabWidget::West );

	mX_axis = new CAxisTab();
	mY_axis = new CAxisTab();
	mZ_axis = new CAxisTab();

    mAxisOptionsTabs->addTab( mX_axis, "X" );
    mAxisOptionsTabs->addTab( mY_axis, "Y" );
    mAxisOptionsTabs->addTab( mZ_axis, "Y2" );

    mAxisOptionsTabs->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    AddTopGroupBox( ELayoutType::Horizontal, { mAxisOptionsTabs });

	Wire();
}


void CPlotControlsPanelAxisOptions::Wire()
{
	connect( mX_axis, SIGNAL( LogarithmicScale( bool ) ), this, SIGNAL( LogarithmicScaleX( bool ) ), Qt::QueuedConnection );
	connect( mY_axis, SIGNAL( LogarithmicScale( bool ) ), this, SIGNAL( LogarithmicScaleY( bool ) ), Qt::QueuedConnection );
	connect( mZ_axis, SIGNAL( LogarithmicScale( bool ) ), this, SIGNAL( LogarithmicScaleZ( bool ) ), Qt::QueuedConnection );
}



/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//								Map View Tabs
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////
//			Map Edit Tab					
////////////////////////////////////////

CMapControlsPanelEdit::CMapControlsPanelEdit( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 );
    : base_t( parent, f )
{
    mAddData = new QComboBox;
    mAddData->setToolTip( "Add Data");

	AddTopWidget( mAddData );

    // III. Data Layers
    //
    mDataLayer = new QComboBox;
    mDataLayer->setToolTip("Data Layer");
    auto mNbLabels = new QLineEdit(this);
    mNbLabels->setText("0");

    auto mShowSolidColorCheck = new QCheckBox(this);
    auto mSolidColorEdit      = new QPushButton("Edit");
    auto mShowContourCheck    = new QCheckBox(this);
    auto mContourEdit         = new QPushButton("Edit");

    QFrame *lineHorizontal = WidgetLine( nullptr, Qt::Horizontal );

    auto mMinRange = new QLineEdit(this);
    mMinRange->setText("0");
    auto mMaxRange = new QLineEdit(this);
    mMaxRange->setText("0");
    auto mReset = new QPushButton("Reset");

    QGroupBox *DataLayersOptions_group = AddTopGroupBox( ELayoutType::Vertical, {
                                                   LayoutWidgets( Qt::Horizontal, { mDataLayer, nullptr, new QLabel( "Number of Labels (Color Bar)" ), mNbLabels } ),
                                                   LayoutWidgets( Qt::Horizontal, { mShowSolidColorCheck, new QLabel( "Show Solid Color" ), mSolidColorEdit, nullptr, mShowContourCheck, new QLabel( "Show Contour" ), mContourEdit } ),
                                                   lineHorizontal,
                                                   LayoutWidgets( Qt::Horizontal, { new QLabel( "Range:" ), nullptr, new QLabel( "Min." ), mMinRange, nullptr, new QLabel( "Max." ), mMaxRange, nullptr, mReset } ),
                                                                                  }, "Data Layers", 4, 4, 4, 4, 4 );

    DataLayersOptions_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
}



////////////////////////////////////////
//			Map Options Tab
////////////////////////////////////////

CMapControlsPanelOptions::CMapControlsPanelOptions( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
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

    AddTopGroupBox( ELayoutType::Horizontal, { ViewOptionsTabs }, "View");
}





///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ViewControlPanels.cpp"
