#include "new-gui/brat/stdafx.h"

#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/Workspaces/Display.h"

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
//								Plot View Tabs
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////
//		Curve Options Tab					
////////////////////////////////////////

CPlotControlsPanelCurveOptions::CPlotControlsPanelCurveOptions( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( parent, f )
{
    // I. Plot List
    //
    mPlotList = new QListWidget();

    // II. Line Options
    //
    mLineColorButton = new CColorButton();

    mLineOpacityValue   = new QLineEdit(this);
    mLineOpacityValue->setText("0.1");

    mStipplePattern = new QComboBox;
    mStipplePattern->setToolTip("Stipple pattern");

    mLineWidthValue = new QLineEdit(this);
    mLineWidthValue->setText("0.5");

    mLineOptions = CreateGroupBox( ELayoutType::Horizontal, { new QLabel( "Plot Color" ),
                                                                       mLineColorButton,
                                                                       nullptr,
                                                                       new QLabel( "Opacity" ),
                                                                       mLineOpacityValue,
                                                                       nullptr,
                                                                       new QLabel( "Stipple pattern"),
                                                                       mStipplePattern,
                                                                       nullptr,
                                                                       new QLabel( "Width" ),
                                                                       mLineWidthValue
                                                                      }, "Line", nullptr, 4, 4, 4, 4, 4 );
    mLineOptions->setCheckable( true );
    mLineOptions->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);


    // III. Point Options
    //
    mPointColorButton = new CColorButton();

    mFillPointCheck   = new QCheckBox(this);

    mPointGlyph = new QComboBox;
    mPointGlyph->setToolTip("Point Glyph");

    mPointSizeValue = new QLineEdit(this);
    mPointSizeValue->setText("0.5");

    mPointOptions = CreateGroupBox( ELayoutType::Horizontal, { new QLabel( "Plot Color" ),
                                                                         mPointColorButton,
                                                                         nullptr,
                                                                         mFillPointCheck,
                                                                         new QLabel( "Fill point" ),
                                                                         nullptr,
                                                                         new QLabel( "Point Glyph"),
                                                                         mPointGlyph,
                                                                         nullptr,
                                                                         new QLabel( "Size" ),
                                                                         mPointSizeValue
                                                                      }, "Point", nullptr, 4, 4, 4, 4, 4 );

    mPointOptions->setCheckable( true );
    mPointOptions->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    // IV. Grouping all widgets
     AddTopGroupBox( ELayoutType::Horizontal, { mPlotList,
                                                LayoutWidgets( Qt::Vertical, {mLineOptions, mPointOptions } )
                                                }, "", 4, 4, 4, 4, 4);

     /*
                old_pen.setStyle(Qt::NoPen);
                break;
            case 1:
                old_pen.setStyle(Qt::SolidLine);
                break;
            case 2:
                old_pen.setStyle(Qt::DashLine);
            case 3:
                old_pen.setStyle(Qt::DotLine);
                break;
            case 4:
                old_pen.setStyle(Qt::DashDotLine);
                break;
            case 5:
                old_pen.setStyle(Qt::DashDotDotLine);
*/

     /*

        NoSymbol = -1,

        Ellipse,
        Rect,
        Diamond,
        Triangle,
        DTriangle,
        UTriangle,
        LTriangle,
        RTriangle,
        Cross,
        XCross,
        HLine,
        VLine,
        Star1,
        Star2,
        Hexagon,
*/
     this->mStipplePattern->addItem("No Line");
     this->mStipplePattern->addItem("Solid Line");
     this->mStipplePattern->addItem("Dash Line");
     this->mStipplePattern->addItem("Dot Line");
     this->mStipplePattern->addItem("Dash-Dot Line");
     this->mStipplePattern->addItem("Dash-Dot-Dot Line");

     this->mPointGlyph->addItem("No Symbol");
     this->mPointGlyph->addItem("Ellipse");
     this->mPointGlyph->addItem("Rect");
     this->mPointGlyph->addItem("Diamond");
     this->mPointGlyph->addItem("Triangle");
     this->mPointGlyph->addItem("DTriangle");
     this->mPointGlyph->addItem("UTriangle");
     this->mPointGlyph->addItem("LTriangle");
     this->mPointGlyph->addItem("RTriangle");
     this->mPointGlyph->addItem("Cross");
     this->mPointGlyph->addItem("XCross");
     this->mPointGlyph->addItem("HLine");
     this->mPointGlyph->addItem("VLine");
     this->mPointGlyph->addItem("Star1");
     this->mPointGlyph->addItem("Star2");
     this->mPointGlyph->addItem("Hexagon");



     Wire();
}

void CPlotControlsPanelCurveOptions::AddNewPlot2DName(QString plot_name)
{
    assert(false);
    assert(mPlotList);
    mPlotList->addItem(plot_name);
    //to garantee that at least a single item is selected
    //execute signal OnCurrentPlotChanged
    mLineOptions->setEnabled( false );
    mPointOptions->setEnabled( false );

}

void CPlotControlsPanelCurveOptions::Wire()
{
    connect( mLineColorButton, SIGNAL( clicked() ), mLineColorButton, SLOT( SetColor() ) );
    connect( mPointColorButton, SIGNAL( clicked() ), mPointColorButton, SLOT( SetColor() ) );

    connect( mPointOptions, SIGNAL( toggled( bool ) ), this, SLOT( HandlePointOptionsChecked( bool ) ) );
    connect( mPointOptions, SIGNAL( toggled( bool )), this, SLOT( HandleCurvePointColorSelected()));

    connect( mLineOptions, SIGNAL( toggled( bool ) ), this, SLOT( HandleLineOptionsChecked( bool ) ) );
    connect( mLineOptions, SIGNAL( toggled( bool ) ), this, SLOT( HandleCurveLineColorSelected() ) );

    connect(mLineColorButton, SIGNAL(clicked( bool )), this, SLOT(HandleCurveLineColorSelected()));
    connect(mPointColorButton, SIGNAL(clicked( bool )), this, SLOT(HandleCurvePointColorSelected()));

    connect(mPlotList, SIGNAL(currentRowChanged(int)), this, SLOT(HandleCurrCurveChanged(int)));

    connect(mLineOpacityValue, SIGNAL (returnPressed()), this, SLOT (HandleCurveLineOpacityEntered()));
    connect(mLineWidthValue, SIGNAL (returnPressed()), this, SLOT (HandleCurveLineWidthEntered()));

    connect( mStipplePattern, SIGNAL (currentIndexChanged(int)), this, SLOT (HandleStipplePatternChanged(int)));

    connect( mPointGlyph, SIGNAL (currentIndexChanged(int)), this, SLOT (HandleGlyphPatternChanged(int)));

    connect(mPointSizeValue, SIGNAL (returnPressed()), this, SLOT (HandleCurveGlyphWidthEntered()));

    connect(mFillPointCheck, SIGNAL (toggled(bool)), this, SLOT( HandleFillGlyphInterior(bool)));

    mLineOptions->setChecked( true );
    mPointOptions->setChecked( false );
}


void CPlotControlsPanelCurveOptions::HandleLineOptionsChecked( bool checked )
{
	mPointOptions->setChecked( !checked );
    emit LineOptionsChecked( checked, mPlotList->currentRow() );
}

void CPlotControlsPanelCurveOptions::HandlePointOptionsChecked( bool checked )
{
	mLineOptions->setChecked( !checked );
    emit PointOptionsChecked( checked, mPlotList->currentRow() );
}



void CPlotControlsPanelCurveOptions::HandleCurveLineColorSelected()
{
    emit CurveLineColorSelected(this->mLineColorButton->GetColor(), mPlotList->currentRow());
}

void CPlotControlsPanelCurveOptions::HandleCurvePointColorSelected()
{
    emit CurvePointColorSelected(this->mPointColorButton->GetColor(), mPlotList->currentRow());
}

void CPlotControlsPanelCurveOptions::HandleCurveLineOpacityEntered()
{
    bool is_converted=false;
    int opacity_value;
    const QString opacity = this->mLineOpacityValue->text();
    opacity_value = opacity.toInt(&is_converted, 10);

    if (!is_converted)
    {
        return;
    }

    emit CurveLineOpacityEntered(opacity_value, mPlotList->currentRow());
}

void CPlotControlsPanelCurveOptions::HandleCurveLineWidthEntered()
{
    bool is_converted=false;
    int width_value;
    const QString width = this->mLineWidthValue->text();
    width_value = width.toInt(&is_converted, 10);

    if (!is_converted)
    {
        return;
    }

    emit CurveLineWidthEntered(width_value, mPlotList->currentRow());
}


void CPlotControlsPanelCurveOptions::HandleNewPlot(const CDisplayFilesProcessor* curr_proc)
{
    //clears previously installed information
    mPlotList->clear();
    size_t sz = curr_proc->GetXYPlotPropertiesSize();

    for (int i =0;i<sz;i++ )
    {
        CXYPlotProperties* ptr_curr_plot = curr_proc->GetXYPlotProperties(i);
        assert(ptr_curr_plot);
        mPlotList->addItem(t2q(ptr_curr_plot->GetName()));
    }
}

void CPlotControlsPanelCurveOptions::HandleCurrCurveChanged(int index)
{
    this->mPointOptions->setEnabled( true );
    this->mLineOptions->setEnabled( true );
    emit CurrCurveChanged(index);
}

void CPlotControlsPanelCurveOptions::HandleStipplePatternChanged(int pattern)
{
    emit StipplePatternChanged(pattern, mPlotList->currentRow());
}

void CPlotControlsPanelCurveOptions::HandleGlyphPatternChanged(int pattern)
{
    emit GlyphPatternChanged(pattern, mPlotList->currentRow());
}

void CPlotControlsPanelCurveOptions::HandleFillGlyphInterior(bool checked)
{
    emit FillGlyphInterior(checked, mPlotList->currentRow());
}

void CPlotControlsPanelCurveOptions::HandleCurveGlyphWidthEntered()
{
    bool is_converted=false;
    int w_value;
    const QString pointw = this->mPointSizeValue->text();
    w_value = pointw.toInt(&is_converted, 10);

    if (!is_converted)
    {
        return;
    }

    emit CurveGlyphWidthEntered(w_value, mPlotList->currentRow());

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

CMapControlsPanelDataLayers::CMapControlsPanelDataLayers( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 );
    : base_t( parent, f )
{
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

    QLayout *DataLayersOptions_group = AddTopLayout( ELayoutType::Vertical, {
        LayoutWidgets( Qt::Horizontal, { mDataLayer, nullptr, new QLabel( "Number of Labels (Color Bar)" ), mNbLabels } ),
        LayoutWidgets( Qt::Horizontal, { mShowSolidColorCheck, new QLabel( "Show Solid Color" ), mSolidColorEdit, nullptr, mShowContourCheck, new QLabel( "Show Contour" ), mContourEdit } ),
        lineHorizontal,
        LayoutWidgets( Qt::Horizontal, { new QLabel( "Range:" ), nullptr, new QLabel( "Min." ), mMinRange, nullptr, new QLabel( "Max." ), mMaxRange, nullptr, mReset } ),
                                        }, s, m, m, m, m );

    Q_UNUSED( DataLayersOptions_group );

    //DataLayersOptions_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
}



////////////////////////////////////////
//			Map Options Tab
////////////////////////////////////////

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

    AddTopGroupBox( ELayoutType::Horizontal, { ViewOptionsTabs }, "View");
}





///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ViewControlPanels.cpp"
