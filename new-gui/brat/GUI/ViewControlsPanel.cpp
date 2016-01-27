#include "new-gui/brat/stdafx.h"

//#include "new-gui/brat/Workspaces/Workspace.h"

//#include "new-gui/brat/Views/TextWidget.h"
//#include "new-gui/brat/Views/2DPlotWidget.h"
//#include "new-gui/brat/Views/3DPlotWidget.h"

#include "ViewControlsPanel.h"


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//								Graphics Panels
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//	 Axis Tab (used to build the tab page with axis options)
////////////////////////////////////////////////////////////////
CAxisTab::CAxisTab( QWidget *parent, Qt::WindowFlags f )
    : base_t( parent, f)
{

    // 1. Axis label and Scale
    auto mAxisLabel = new QLineEdit();
    mAxisLabel->setText("Axis label text");
    mAxisLabel->setToolTip("Axis label");

    auto mLogScaleCheck = new QCheckBox();

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

    QBoxLayout *Ranges1_layout = ::LayoutWidgets( Qt::Vertical, { mCurrent1, mRange1 }, nullptr, 2, 2, 2, 2, 2);
    QBoxLayout *Ranges2_layout = ::LayoutWidgets( Qt::Vertical, { mCurrent2, mRange2 }, nullptr, 2, 2, 2, 2, 2);
    QBoxLayout *Range_labels   = ::LayoutWidgets( Qt::Vertical, { new QLabel( "Current" ), new QLabel( "Range" ) }, nullptr, 2, 2, 2, 2, 2);


    QGroupBox *FallBackRange_group = CreateGroupBox(ELayoutType::Horizontal, {
                                                mFromDataCheck, new QLabel( "From Data" ), nullptr, mFromUserCheck, new QLabel( "From User" ),
                                                Ranges1_layout,
                                                nullptr,
                                                Range_labels,
                                                Ranges2_layout
                                                                           }, "Fallback Range");

    // 4. Axis Color Options
    mAxisColorButton = new QPushButton();
    mAxisColorButton->setFixedSize(20, 20);

    auto mAxisOpacityValue   = new QLineEdit(this);
    mAxisOpacityValue->setText("0.1");

    QComboBox *mAxisStipplePattern = new QComboBox;
    mAxisStipplePattern->setToolTip("Stipple pattern");

    auto mAxisWidthValue = new QLineEdit(this);
    mAxisWidthValue->setText("0.5");

    QGroupBox *AxisColorOptions = CreateGroupBox( ELayoutType::Horizontal, { new QLabel( "Plot Color" ),
                                                                       mAxisColorButton,
                                                                       nullptr,
                                                                       new QLabel( "Opacity" ),
                                                                       mAxisOpacityValue,
                                                                       nullptr,
                                                                       new QLabel( "Stipple pattern"),
                                                                       mAxisStipplePattern,
                                                                       nullptr,
                                                                       new QLabel( "Width" ),
                                                                       mAxisWidthValue
                                                                      }, "", nullptr, 4, 4, 4, 4, 4 );

    connect( mAxisColorButton, SIGNAL( clicked() ), this, SLOT( SetAxisColor() ) );

    // 5. Creating Tab Group
    QGroupBox *TabGroup = CreateGroupBox( ELayoutType::Vertical, {
                               ::LayoutWidgets( Qt::Horizontal, { mAxisLabel, nullptr,  mLogScaleCheck, new QLabel( "Logarithmic Scale" ) }, nullptr, 2, 2, 2, 2, 2 ),
                               ::LayoutWidgets( Qt::Horizontal, { new QLabel( "Nb of Ticks:" ), mNbTicks, nullptr, new QLabel( "Base:" ), mBase, nullptr, new QLabel( "Nb of Digits:" ), mNbDigits}, nullptr, 2, 2, 2, 2, 2 ),
                               FallBackRange_group,
                               AxisColorOptions
                                    }, "", nullptr, 4, 4, 4, 4, 4 );

    AddWidget( this, TabGroup );
}

void CAxisTab::SetAxisColor()
{
    QColor color = QColorDialog::getColor(mAxisColor, this);

    if (color.isValid())
    {
        mAxisColor = color;
        mAxisColorButton->setPalette(QPalette(mAxisColor));
    }
}


/////////////////////////////////////////////////////////////////////////////////////
//								Plot View
/////////////////////////////////////////////////////////////////////////////////////

//explicit
CPlotViewControls::CPlotViewControls( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( parent, f )
{

    // I. Information about Dataset, Filter and Operation and OneClick
    //
    //    I.1 Info
    mSelectDataset   = new QComboBox;
    mApplyFilter     = new QComboBox;
    mSelectOperation = new QComboBox;

    mSelectDataset  ->setToolTip( "Select a Dataset" );
    mApplyFilter    ->setToolTip( "Apply a Filter" );
    mSelectOperation->setToolTip( "Select a Operation" );

    auto mDatasetName   = new QLineEdit(this);
    auto mFilterName    = new QLineEdit(this);
    auto mOperationName = new QLineEdit(this);

    mDatasetName  ->setText("Dataset name");
    mFilterName   ->setText("Filter name");
    mOperationName->setText("Operation name");

    QGridLayout *dataFilterOp_layout = LayoutWidgets( {
                                            mSelectDataset, mApplyFilter, mSelectOperation, nullptr,
                                            mDatasetName,   mFilterName,  mOperationName,
                                                      } );
    //    I.2 Save One Click button
    auto mSaveOneClick     = new QPushButton( "Save One Click" );
    QGroupBox *SaveOneClick_group = CreateGroupBox(ELayoutType::Horizontal, {mSaveOneClick});
    SaveOneClick_group->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    //    Adding previous widgets to this...
    AddTopGroupBox( ELayoutType::Horizontal, { dataFilterOp_layout, SaveOneClick_group }, "", 4, 4, 4, 4, 4 );


    // II. Plot selection buttons and info
    //
    //    II.1 Selection buttons
    auto mPlotName   = new QLineEdit(this);
    mPlotName->setText("Plot name");

    auto mNewPlot    = new QPushButton( "New" );
    auto mSavePlot   = new QPushButton( "Save" );
    auto mDeletePlot = new QPushButton( "Delete" );
    auto mRunPlot    = new QPushButton( "Run" );

    mOpenAplot  = new QComboBox;
    mSelectplot = new QComboBox;
    mvarX       = new QComboBox;
    mvarY       = new QComboBox;
    mvarY2      = new QComboBox;
    mvarZ       = new QComboBox;

    mOpenAplot ->setToolTip( "Open a plot" );
    mSelectplot->setToolTip( "Select a plot type");
    mvarX      ->setToolTip( "X" );
    mvarY      ->setToolTip( "Y");
    mvarY2     ->setToolTip( "Y2");
    mvarZ      ->setToolTip( "Z" );

    //    II.2 Table with plot information
    auto mPlotInfoTable = new QTableWidget;
    mPlotInfoTable->setSortingEnabled( true );                             // Enabling sorting
    mPlotInfoTable->setAlternatingRowColors( true );                       // Alternating row colors
    mPlotInfoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);    // Disable editable table items
    //mPlotInfoTable->setSelectionBehavior( QAbstractItemView::SelectRows ); // Whole row selection
    //mPlotInfoTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    mPlotInfoTable->setMinimumWidth( ComputeTableWidth( mPlotInfoTable ) );  // Setting minimum width

    //    Set Header Label Texts
    mPlotInfoTable->setRowCount(1);
    mPlotInfoTable->setColumnCount(8);
    mPlotInfoTable->setHorizontalHeaderLabels(
                QString("Plot name;Expression name;View type;Unit;dim.1;dim.2;Operation name;Dataset").split(";"));

    //    II.3 Link to Plot
    auto mPlotTitle   = new QLineEdit(this);
    mPlotTitle->setText("Plot title");

    mLinkToPlot  = new QComboBox;
    mLinkToPlot->setToolTip("Link to Plot");


    //    Adding previous widgets to this...
    AddTopGroupBox( ELayoutType::Vertical, {
                                 LayoutWidgets( Qt::Horizontal, { mPlotName, mNewPlot, mSavePlot, mDeletePlot, mRunPlot, mOpenAplot } ),
                                 LayoutWidgets( Qt::Horizontal, { mSelectplot, new QLabel( "Add data:" ), mvarX, mvarY, mvarY2, mvarZ } ),
                                 mPlotInfoTable,
                                 LayoutWidgets( Qt::Horizontal, { mPlotTitle, mLinkToPlot } )
                                            }, "", 4, 4, 4, 4, 4 );

    // III. Line Options
    //
    mLineColorButton = new QPushButton();
    mLineColorButton->setFixedSize(20, 20);

    auto mOpacityValue   = new QLineEdit(this);
    mOpacityValue->setText("0.1");

    mStipplePattern = new QComboBox;
    mStipplePattern->setToolTip("Stipple pattern");

    auto mWidthValue = new QLineEdit(this);
    mWidthValue->setText("0.5");

    QGroupBox *LineOptions = AddTopGroupBox( ELayoutType::Horizontal, { new QLabel( "Plot Color" ),
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

    LineOptions->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    connect( mLineColorButton, SIGNAL( clicked() ), this, SLOT( SetLineColor() ) );


    // IV. Point Options
    //
    mPointColorButton = new QPushButton();
    mPointColorButton->setFixedSize(20, 20);

    auto mFillPointCheck   = new QCheckBox(this);

    mPointGlyph = new QComboBox;
    mPointGlyph->setToolTip("Point Glyph");

    auto mSizeValue = new QLineEdit(this);
    mSizeValue->setText("0.5");

    QGroupBox *PointOptions = AddTopGroupBox( ELayoutType::Horizontal, { new QLabel( "Plot Color" ),
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

    PointOptions->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    connect( mPointColorButton, SIGNAL( clicked() ), this, SLOT( SetPointColor() ) );


    // V. Axes Options
    //
    axisOptionsTabs = new QTabWidget();

    axisOptionsTabs->addTab(new CAxisTab(), "X axis");
    axisOptionsTabs->addTab(new CAxisTab(), "Y axis");
    axisOptionsTabs->addTab(new CAxisTab(), "Y2 axis");

    axisOptionsTabs->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    AddTopGroupBox( ELayoutType::Horizontal, { axisOptionsTabs });

}



void CPlotViewControls::SetLineColor()
{
    QColor color = QColorDialog::getColor(mLineColor, this);

    if (color.isValid())
    {
        mLineColor = color;
        mLineColorButton->setPalette(QPalette(mLineColor));
    }
}

void CPlotViewControls::SetPointColor()
{
    QColor color = QColorDialog::getColor(mPointColor, this);

    if (color.isValid())
    {
        mPointColor = color;
        mPointColorButton->setPalette(QPalette(mPointColor));
    }
}



/////////////////////////////////////////////////////////////////////////////////////
//								Map View
/////////////////////////////////////////////////////////////////////////////////////

//explicit
CMapViewControls::CMapViewControls( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( parent, f )
{

    // I. Information about Dataset, Filter and Operation and OneClick
    //
    //    I.1 Info
    mSelectDataset   = new QComboBox;
    mApplyFilter     = new QComboBox;
    mSelectOperation = new QComboBox;

    mSelectDataset  ->setToolTip( "Select a Dataset" );
    mApplyFilter    ->setToolTip( "Apply a Filter" );
    mSelectOperation->setToolTip( "Select a Operation" );

    auto mDatasetName   = new QLineEdit(this);
    auto mFilterName    = new QLineEdit(this);
    auto mOperationName = new QLineEdit(this);

    mDatasetName  ->setText("Dataset name");
    mFilterName   ->setText("Filter name");
    mOperationName->setText("Operation name");

    QGridLayout *dataFilterOp_layout = LayoutWidgets( {
                                            mSelectDataset, mApplyFilter, mSelectOperation, nullptr,
                                            mDatasetName,   mFilterName,  mOperationName,
                                                      } );
    //    I.2 Save One Click button
    auto mSaveOneClick            = new QPushButton( "Save One Click" );
    QGroupBox *SaveOneClick_group = CreateGroupBox(ELayoutType::Horizontal, {mSaveOneClick});
    SaveOneClick_group->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    //    Adding previous widgets to this...
    AddTopGroupBox( ELayoutType::Horizontal, { dataFilterOp_layout, SaveOneClick_group }, "", 4, 4, 4, 4, 4 );


    // II. Plot selection buttons and info
    //
    //    II.1 Selection buttons
    auto mPlotName   = new QLineEdit(this);
    mPlotName->setText("Plot name");

    auto mNewPlot    = new QPushButton( "New" );
    auto mSavePlot   = new QPushButton( "Save" );
    auto mDeletePlot = new QPushButton( "Delete" );
    auto mRunPlot    = new QPushButton( "Run" );

    mOpenAplot  = new QComboBox;
    mAddData    = new QComboBox;

    mOpenAplot ->setToolTip( "Open a plot" );
    mAddData   ->setToolTip( "Add Data");

    QBoxLayout *mAddData_box = LayoutWidgets( Qt::Horizontal, { mAddData });
    mAddData_box->setAlignment(Qt::AlignLeft);

    //    II.2 Table with map plot information
    auto mPlotInfoTable = new QTableWidget;
    mPlotInfoTable->setSortingEnabled( true );                             // Enabling sorting
    mPlotInfoTable->setAlternatingRowColors( true );                       // Alternating row colors
    mPlotInfoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);    // Disable editable table items
    //mPlotInfoTable->setSelectionBehavior( QAbstractItemView::SelectRows ); // Whole row selection
    //mPlotInfoTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    mPlotInfoTable->setMinimumWidth( ComputeTableWidth( mPlotInfoTable ) );  // Setting minimum width

    //    Set Header Label Texts
    mPlotInfoTable->setRowCount(1);
    mPlotInfoTable->setColumnCount(5);
    mPlotInfoTable->setHorizontalHeaderLabels( QString("Plot name;Expression name;Unit;Operation name;Dataset").split(";") );

    //    Adding previous widgets to this...
    AddTopGroupBox( ELayoutType::Vertical, {
                                 LayoutWidgets( Qt::Horizontal, { mPlotName, mNewPlot, mSavePlot, mDeletePlot, mRunPlot, mOpenAplot } ),
                                 mAddData_box,
                                 mPlotInfoTable,
                                            }, "", 4, 4, 4, 4, 4 );

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

    AddTopGroupBox( ELayoutType::Horizontal, { ViewOptionsTabs }, "Data Layers");


}






///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ViewControlsPanel.cpp"
