#include "new-gui/brat/stdafx.h"

#include "DatasetFilterControls.h"



/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Filter
/////////////////////////////////////////////////////////////////////////////////////



//explicit
CDatasetFilterControls::CDatasetFilterControls( CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0
    : base_t( manager, parent, f )
	, mMap( manager->Map() )
{

    // I. "Where" Description group
    //
    //    I.1 List of Areas
    QListWidget *areas_list = CreateBooleanList( this, { { "Lake Baikal", true }, { "Black Sea" }, { "User Area 1", true }, { "User Area 2" } } );
    areas_list->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    QGroupBox *areas_box = CreateGroupBox( ELayoutType::Horizontal, { areas_list }, "Areas", this );


    //    I.2 Buttons for region selection
    mSavedRegionsCombo = new QComboBox;
    mSavedRegionsCombo->setToolTip( "List of saved regions" );

    auto mKML     = new QPushButton( "KML" );
    auto mMask    = new QPushButton( "Mask" );

    QBoxLayout *buttons_regions = LayoutWidgets( Qt::Horizontal, { mSavedRegionsCombo, mKML, mMask } );

    //    I.3 Coordinates (max and min values)
    mMaxLat = new QLineEdit(this);
    mMaxLon = new QLineEdit(this);
    mMinLat = new QLineEdit(this);
    mMinLon = new QLineEdit(this);

    mMaxLat->setText("0.0");
    mMaxLon->setText("0.0");
    mMinLat->setText("0.0");
    mMinLon->setText("0.0");

    QBoxLayout *coord_values_l = LayoutWidgets( Qt::Vertical, {
                                             LayoutWidgets( Qt::Horizontal, { new QLabel( "Max Lat (deg)" ), mMaxLat } ),
                                             LayoutWidgets( Qt::Horizontal, { new QLabel( "Min Lat (deg)" ), mMinLat } )
                                               } );

    QBoxLayout *coord_values_r = LayoutWidgets( Qt::Vertical, {
                                             LayoutWidgets( Qt::Horizontal, { new QLabel( "Max Lon (deg)" ), mMaxLon } ),
                                             LayoutWidgets( Qt::Horizontal, { new QLabel( "Min Lon (deg)" ), mMinLon } )
                                               } );

    QBoxLayout *coord_values = LayoutWidgets( Qt::Horizontal, { coord_values_l, nullptr, coord_values_r }, nullptr );


    //    I.4 Adding previous widgets to this...

    QGroupBox *regions_coord = CreateGroupBox(ELayoutType::Vertical, {buttons_regions, coord_values} );
    regions_coord->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    AddTopGroupBox( ELayoutType::Horizontal, { areas_box, regions_coord }, "Where", 4, 4, 4, 4, 4 );


    // II. "When" Description group
    //
    //    II.1 Dates, Cycles and Pass (start and stop values)

    auto mStartDate  = new QLineEdit(this);
    auto mStopDate   = new QLineEdit(this);
    auto mStartCycle = new QLineEdit(this);
    auto mStopCycle  = new QLineEdit(this);
    auto mStartPass  = new QLineEdit(this);
    auto mStopPass   = new QLineEdit(this);

    mStartDate->setText("yyyy/mm/dd T hh:mm");
    mStopDate->setText("yyyy/mm/dd T hh:mm");
    mStartCycle->setText("0");
    mStopCycle->setText("0");
    mStartPass->setText("0");
    mStopPass->setText("0");

    QBoxLayout *dates_box = LayoutWidgets( Qt::Vertical, {
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Start Date" ), mStartDate } ),
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Stop Date" ),  mStopDate  } )
                                                } );

    QBoxLayout *cycles_box = LayoutWidgets( Qt::Vertical, {
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Start Cycle" ), mStartCycle } ),
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Stop Cycle" ),  mStopCycle  } )
                                                } );

    QBoxLayout *pass_box = LayoutWidgets( Qt::Vertical, {
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Start Pass" ), mStartPass } ),
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Stop Pass" ),  mStopPass  } )
                                                } );

    QBoxLayout *dateCyclePassValues_box = LayoutWidgets( Qt::Horizontal, {dates_box, nullptr, cycles_box, nullptr, pass_box});


    //   II.2 One-Click Time Filtering

    QFrame *lineHorizontal = WidgetLine( nullptr, Qt::Horizontal );
    auto one_click_label  = new QLabel( "One-Click Time Filtering" );

    //    Checkable menu items --> ATTENTION: are exclusive checkable menu items??
    auto last_month  = new QCheckBox( "Last Month" );
    auto last_year   = new QCheckBox( "Last Year" );
    auto last_cycle  = new QCheckBox( "Last Cycle" );
    QGroupBox *monthYearCycleGroup = CreateGroupBox(ELayoutType::Vertical, {last_month, last_year, last_cycle} );

    QFrame *lineVertical_1 = WidgetLine( nullptr, Qt::Vertical );
    QFrame *lineVertical_2 = WidgetLine( nullptr, Qt::Vertical );

    auto mRelativeStart  = new QLineEdit(this);
    auto mRelativeStop   = new QLineEdit(this);
    mRelativeStart->setText("0");
    mRelativeStop ->setText("0");
    QBoxLayout *relative_times = LayoutWidgets( Qt::Vertical, {
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Relative Start" ), mRelativeStart } ),
                                                 LayoutWidgets( Qt::Horizontal, { new QLabel( "Relative Stop" ),  mRelativeStop  } )
                                                } );

    auto reference_date       = new QCheckBox( "Reference Date" );
    auto reference_date_text  = new QLineEdit(this);
    reference_date_text->setText("yyyy/mm/dd");
    QBoxLayout *refDateBox = LayoutWidgets( Qt::Vertical, { reference_date, reference_date_text} );

    //    Adding previous widgets to this...
    AddTopGroupBox( ELayoutType::Vertical, {
                        dateCyclePassValues_box,
                        lineHorizontal,
                        one_click_label,
                        LayoutWidgets( Qt::Horizontal, { monthYearCycleGroup, lineVertical_1, relative_times, lineVertical_2, refDateBox }
						) },"When", 4, 4, 4, 4, 4 );


    // III. "Total Nb Records Selected" Description group
    //
    auto mTotalRecordsSelected  = new QLineEdit(this);
    mTotalRecordsSelected->setText("0");
    mTotalRecordsSelected->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    QBoxLayout *TotalRecords_box = LayoutWidgets( Qt::Horizontal, { new QLabel( "Total Nb Records selected" ), mTotalRecordsSelected });
    TotalRecords_box->setAlignment(Qt::AlignRight);

    //    Adding previous widgets to this...
    AddTopGroupBox( ELayoutType::Horizontal, { TotalRecords_box});


    // III. a. v3 Inherited Selection Criteria... (TBD)
    //
    auto mDefineSelectionCriteria = new QPushButton( "Define..." );
    auto mApplySelectionCriteria = new QPushButton( "Apply..." );
    auto mShowSelectionReport = new QPushButton( "Report..." );

    AddTopGroupBox( ELayoutType::Horizontal, { mDefineSelectionCriteria, mApplySelectionCriteria, mShowSelectionReport }, "Selection Criteria" );

    AddTopWidget( WidgetLine( nullptr, Qt::Horizontal ) );


    // IV. Filter name description group
    //
    auto mFilterName  = new QLineEdit(this);
    mFilterName ->setText("Filter Name");

    auto mNewFilter     = new QPushButton( "New Filter" );
    auto mDeleteFilter  = new QPushButton( "Delete Filter" );
    auto mSaveFilter    = new QPushButton( "Save Filter" );

    mOpenFilterCombo = new QComboBox;
    mOpenFilterCombo->setToolTip( "Open a Filter" );

    AddTopLayout( ELayoutType::Horizontal, {  mFilterName, mNewFilter, mDeleteFilter, mSaveFilter, mOpenFilterCombo } );


    AddTopSpace( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );

	Wire();
}


void CDatasetFilterControls::Wire()
{
	connect( mMap, SIGNAL( CurrentLayerSelectionChanged() ), this, SLOT( CurrentLayerSelectionChanged() ) );
}


void CDatasetFilterControls::CurrentLayerSelectionChanged()
{
	QgsRectangle box = mMap->CurrentLayerSelectedBox();

	mMaxLat->setText( n2q( box.yMaximum() ) );
    mMaxLon->setText( n2q( box.xMaximum() ) );
    mMinLat->setText( n2q( box.yMinimum() ) );
    mMinLon->setText( n2q( box.xMinimum() ) );
}




#include "DataModels/DisplayFilesProcessor.h"
#include "libbrathl/InternalFilesZFXY.h"
#include "DataModels/PlotData/WorldPlot.h"
#include "DataModels/PlotData/WorldPlotData.h"


//public slots:
void CDatasetFilterControls::WorkspaceChanged( CWorkspaceDataset *wksd )
{
    mWks = wksd;
}


void CDatasetFilterControls::DatasetChanged( CDataset *dataset )
{
    Q_UNUSED( dataset );
}

void CDatasetFilterControls::FileChanged( const QString &path )
{
    return;

	mMap->RemoveLayers();
	if ( !path.isEmpty() )
	try 
	{
		CDisplayFilesProcessor proc;
		proc.Process( q2t<std::string>( path ) );
		auto &plots = proc.plots();
		for ( auto *plot : plots )
		{
			CWPlot* wplot = dynamic_cast< CWPlot* >( plot );
			if ( wplot == nullptr )
				continue;

			mMap->CreatePlot( proc.GetWorldPlotProperties( 0 ), wplot );
		}
		/*
		CInternalFilesZFXY* zfxy = dynamic_cast< CInternalFilesZFXY* >( proc.Prepare( q2a( path ) ) );
		CExpressionValue varLat, varLon;
		CWorldPlotInfo maps;
		maps.AddMap();
		CWorldPlotData::GetMapLatLon( zfxy, maps().mPlotWidth, maps().mPlotHeight, varLat, varLon );

		bool firstDimIsLat = false;
		CStringArray varDimNames;
		zfxy->GetVarDims( "SLA", varDimNames );
		if ( ( zfxy->GetVarKind( varDimNames.at( 0 ) ) == Latitude ) &&
			( zfxy->GetVarKind( varDimNames.at( 1 ) ) == Longitude ) )
		{
			firstDimIsLat = true;
		}
		else if ( ( zfxy->GetVarKind( varDimNames.at( 0 ) ) == Longitude ) &&
			( zfxy->GetVarKind( varDimNames.at( 1 ) ) == Latitude ) )
		{
			firstDimIsLat = false;
		}

		for ( int32_t iY = 0; iY < maps().mPlotHeight; iY++ )
		{
			auto lat = varLat.GetValues()[ iY ];
			maps.AddY( lat );
			for ( int32_t iX = 0; iX < maps().mPlotWidth; iX++ )
			{
				maps.AddmValidMercatorLatitude( CTools::AreValidMercatorLatitude( lat ) );
				maps.AddBit( true );
				maps.AddValue( iY );
			}
		}
		for ( int32_t iX = 0; iX < maps().mPlotWidth; iX++ )
		{
			maps.AddX( CTools::NormalizeLongitude( -180.0, varLon.GetValues()[ iX ] ) );
		}
		mMap->Plot( maps );
		*/
	}
	catch ( const CException &e )
	{
		SimpleErrorBox( e.Message() );
	}
	catch ( ... )
	{
		SimpleErrorBox( "Unknown error" );
	}
}






///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DatasetFilterControls.cpp"
