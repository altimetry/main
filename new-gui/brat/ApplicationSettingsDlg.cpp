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
#include "stdafx.h"

#include "process/rads/RadsSettings.h"
#include "new-gui/Common/System/Service/qtservice.h"
#include "new-gui/Common/System/OsProcess.h"
#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/System/Service/qtservice.h"
#include "new-gui/Common/GUI/TextWidget.h"
#include "GUI/ActionsTable.h"
#include "GUI/DisplayWidgets/Dialogs/ViewMapsLayerDialog.h"
#include "BratApplication.h"
#include "BratSettings.h"
#include "ApplicationSettingsDlg.h"



void CApplicationSettingsDlg::CreateWidgets()
{
	// *** Pages ***

	//	ApplicationPaths Page

    mDataDirectoryLineEdit = new QLineEdit( this );
	mBrowseDataDirectoryPushButton = new QPushButton( "Browse..." );
	mUsePortablePathsCheckBox = new QCheckBox( "Use portable paths" );
	mUsePortablePathsCheckBox->setToolTip( "If checked, datasets in the data directory are stored as relative paths,\nallowing workspaces to be copied across machines" );

	auto *data_l =
		LayoutWidgets( Qt::Vertical, {
			new QLabel( "Default Data Directory" ),
			LayoutWidgets( Qt::Horizontal, { mDataDirectoryLineEdit, mBrowseDataDirectoryPushButton }, nullptr, 2, 2, 2, 2, 2 ),
			mUsePortablePathsCheckBox
		}, nullptr, 2, 2, 2, 2, 2 );


    mProjectsDirectoryLineEdit = new QLineEdit( this );
    mProjectsDirectoryLineEdit->setObjectName( QString::fromUtf8("ExternalDataDirectory_lineEdit") );
	mBrowseWorkspacesDirectoryPushButton = new QPushButton( "Browse..." );

	auto *wkspc_l = 
		LayoutWidgets( Qt::Vertical, {
			new QLabel( "Default Workspaces Directory" ),
			LayoutWidgets( Qt::Horizontal, { mProjectsDirectoryLineEdit, mBrowseWorkspacesDirectoryPushButton }, nullptr, 2, 2, 2, 2, 2 )
		}, nullptr, 2, 2, 2, 2, 2 );


	mApplicationPathsPage = CreateGroupBox( ELayoutType::Vertical, { nullptr, data_l, nullptr, wkspc_l, nullptr }, "Application Paths", this, 6, 6, 6, 6, 6 );


	//StartupOptions Page

	mLoadLastProjectAtAtartupCheckBox = new QCheckBox( "Load last workspace of previous session" );

	mUseVectorLayer = new QRadioButton( "Use a vector layer" );
	mUseRasterLayer = new QRadioButton( "Use a raster layer file or URI" );
    mUseRasterLayerWMS = new QRadioButton( "Use a raster layer URI (WMS)" );
    auto *main_layers_group = CreateGroupBox( ELayoutType::Vertical,
                                        {
                                            mUseVectorLayer,
                                            mUseRasterLayer,
                                            mUseRasterLayerWMS
                                        },
                                        "Main Map (requires restart)", this );

	mMapViewsDefaultLayerDialogPushButton = CreateToolButton( "Views...", "://images/OSGeo/map-settings.png", CActionInfo::FormatTip( "Views Options", "Edit view maps default base layer" ) );

	mLayerWMSLineEdit = new QLineEdit;
	mRasterLayerBrowseButton = new QPushButton("Browse...");
	mRasterLayerLineEdit = new QLineEdit;
	auto raster_label = new QLabel( "Raster Layer" );
    auto raster_l = LayoutWidgets( Qt::Vertical, 
    { 
        raster_label,                          
        LayoutWidgets( Qt::Horizontal, { mRasterLayerLineEdit, mRasterLayerBrowseButton }, nullptr, 2, 2, 2, 2, 2 ),                               
    }, nullptr, 2, 2, 2, 2, 2 );

	mVectorSimplifyMethodCheck = new QCheckBox( "Simplify vector layer geometry" );
	mVectorSimplifyMethodCheck->setToolTip( t2q( CActionInfo::FormatTip( "Simplify Vector Geometry", "Check to improve performance, leave unchecked to improve map projections" ) ) );
	mResetLayerOptionsPushButton = new QPushButton("Reset");
	mResetLayerOptionsPushButton->setToolTip( t2q( CActionInfo::FormatTip( "Reset Layer Options", "Return layer options to their default values" ) ) );

	auto *layers_group = CreateGroupBox( ELayoutType::Vertical,
	{
		LayoutWidgets( Qt::Horizontal, { main_layers_group, CreateGroupBox( ELayoutType::Grid, {mMapViewsDefaultLayerDialogPushButton}, "", this ) }, nullptr, 2, 2, 2, 2, 2 ),
		mVectorSimplifyMethodCheck,
		raster_l,
		LayoutWidgets( Qt::Vertical, { new QLabel( "Raster Layer (WMS)" ), mLayerWMSLineEdit }, nullptr, 2, 2, 2, 2, 2 ),
		LayoutWidgets( Qt::Horizontal, { nullptr, mResetLayerOptionsPushButton }, nullptr, 2, 2, 2, 2, 2 ),
	}, 
	"Maps Base Layers",
	nullptr, 2, 4, 4, 4, 4 );
	layers_group->setAlignment( Qt::AlignCenter );


#if defined(DEBUG) || defined(_DEBUG)

	mDesktopManagerSdiCheckbox = new QCheckBox( "Single Document Interface desktop manager" );
	mDesktopManagerSdiCheckbox->setChecked( mSettings.mDesktopManagerSdi );

	mStartupOptionsPage = CreateGroupBox( ELayoutType::Vertical, 
	{ 
		nullptr, mLoadLastProjectAtAtartupCheckBox, nullptr, layers_group, nullptr, mDesktopManagerSdiCheckbox, nullptr
	}
	, "Startup Options", this, 6, 6, 6, 6, 6 );
#else

	mStartupOptionsPage = CreateGroupBox( ELayoutType::Vertical, 
	{ 
        nullptr, mLoadLastProjectAtAtartupCheckBox, nullptr, layers_group, nullptr,
	}
	, "Startup Options", this, 6, 6, 6, 6, 6 );
#endif


	//RADS Page

	//...administration box: install / enable

	mRadsInstallButton = CActionInfo::CreateToolButton( eAction_InstallRadsService, true ); 	assert__( mRadsInstallButton->isCheckable() );
	mRadsInstallButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	mRadsStartButton = CActionInfo::CreateToolButton( eAction_StartRadsService, true ); 		assert__( mRadsStartButton->isCheckable() );
	mRadsStartButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	mRadsStartButton_Test = CActionInfo::CreateToolButton( eAction_StartRadsService, true ); 	assert__( mRadsStartButton_Test->isCheckable() );
	mRadsStartButton_Test->setText( "" );
    mRadsRefreshConnectionButton = CreateToolButton( "Refresh", ":/images/themes/default/mActionRefresh.png", 
		CActionInfo::FormatTip( "Link to RadsService", "Refreshes the link with the RADS service if started/stopped outside BRAT" ) );

	QLabel *picon = new QLabel;
#if defined (Q_OS_WIN)
	QIcon icon = QApplication::style()->standardIcon( QStyle::SP_VistaShield );
	if ( icon.isNull() )
		icon = QIcon( "://images/uac-icon-vista.png" );
	QPixmap pixmap = icon.pixmap( icon.actualSize( QSize( icon_size, icon_size ) ) );
	picon->setPixmap( pixmap );
	if ( !mIsElevatedProcess )
		picon->setToolTip( "To install and enable the RADS service you need to run BRAT as administrator." );
	picon->setEnabled( mIsElevatedProcess );
#endif

    auto *admin_box_l = LayoutWidgets( Qt::Horizontal,
	{
		picon, nullptr, mRadsInstallButton, nullptr, mRadsStartButton, nullptr  //, mRadsRefreshConnectionButton
	}, nullptr, 2, 2, 2, 2, 2 );
    auto *admin_box = new QFrame;
    admin_box->setLayout( admin_box_l );
    admin_box->setFrameShape( QFrame::Box );
    


	//...settings box

	mRadsOutputLineEdit = new QLineEdit;
	mBrowseRadsOutputPushButton = new QPushButton( "Browse..." );
	mRadsOutputBox = CreateGroupBox( ELayoutType::Horizontal,
	{
		LayoutWidgets( Qt::Horizontal, { mRadsOutputLineEdit, mBrowseRadsOutputPushButton }, nullptr, 2, 2, 2, 2, 2 )
	}, "Destination Path", nullptr );
	mRadsOutputBox->setToolTip( "Directory to download the RADS data.\nSelect a location where all BRAT users can read/write files." );
	//mRadsOutputBox->setAlignment( Qt::AlignCenter );

	mRadsViewLogFile = new QPushButton( "Log..." );
	mRadsViewRADSConfigurationFile = new QPushButton( "Configuration..." );
	mRadsViewRADSServiceSettingsFile = new QPushButton( "Service Settings..." );
	mRadsSpin = new QSpinBox;
	mRadsSpin->setMinimum( CSharedRadsSettings::smMinNumberOfDays );
	mRadsSpin->setMaximum( CSharedRadsSettings::smMaxNumberOfDays );
	mRadsMissionsList = new QListWidget;

	mRadsPauseButton = CActionInfo::CreateToolButton( eAction_PauseRadsService, true ); 		assert__( mRadsPauseButton->isCheckable() );
	mRadsExecuteNow = CActionInfo::CreateToolButton( eAction_ExecuteOrStopRadsService, true ); 	assert__( mRadsExecuteNow->isCheckable() );
	mRadsExecuteNow->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );


	mNextSyncLabel = new QLabel;
	mNextSyncLabel->setToolTip( "This information is reliable only if the service is running" );
	mNextSyncLabel->setAlignment( Qt::AlignCenter );
	mRsyncStatusLabel = new QLabel;
	mRsyncStatusLabel->setAlignment( Qt::AlignCenter );

	auto *sync_date_box = CreateGroupBox( ELayoutType::Vertical,
	{
		LayoutWidgets( Qt::Horizontal, { nullptr, new QLabel( "Synchronize every" ), mRadsSpin, new QLabel( "days" ), nullptr }, nullptr, 2,2,2,2,2 ),
		nullptr,
		mNextSyncLabel, 
		mRadsExecuteNow,
		mRsyncStatusLabel,
		nullptr
	}, 
	"Schedule", nullptr, 6,6,6,6,6 );
	sync_date_box->setAlignment( Qt::AlignCenter );
	sync_date_box->layout()->setAlignment( mRadsExecuteNow, Qt::AlignCenter );

	auto *missions_box = CreateGroupBox( ELayoutType::Horizontal,
	{
		mRadsMissionsList
	}, 
	"Missions", nullptr, 6,6,6,6,6 );
	missions_box->setAlignment( Qt::AlignCenter );
	missions_box->layout()->setAlignment( mRadsMissionsList, Qt::AlignCenter );

	auto *sync_settings_l = LayoutWidgets( Qt::Vertical,
	{
		mRadsOutputBox,
		LayoutWidgets( Qt::Horizontal, 
		{ 
			sync_date_box,
			missions_box
		}, nullptr, 6,2,2,2,2 )
	}, 
	nullptr, 6,2,2,2,2 );


	mRadsCommandsBox = CreateGroupBox( ELayoutType::Vertical, 
	{
		CreateButtonRow( false, Qt::Horizontal, { mRadsStartButton_Test, mRadsPauseButton, mRadsViewLogFile, mRadsViewRADSConfigurationFile, mRadsViewRADSServiceSettingsFile }, 2, 2 ),
	},
	"Service Control", nullptr, 2,2,2,2,2 );

#if !defined(DEBUG_RADS_SERVICE)
	mRadsCommandsBox->setVisible( false );
#endif

	mRadsSettingsBox = CreateGroupBox( ELayoutType::Vertical, 
	{ 
		nullptr, sync_settings_l, mRadsCommandsBox, nullptr 
	}, 
	"Service Settings", this, 6, 6, 6, 6, 6 );

	mRadsOptionsPage = CreateGroupBox( ELayoutType::Vertical, 
	{ 
		LayoutWidgets( Qt::Horizontal, { admin_box, mRadsRefreshConnectionButton }, nullptr, 6,2,2,2,2),
        //admin_box,
		mRadsSettingsBox
	}, 
	"RADS", this, 6, 6, 6, 6, 6 );


	//ApplicationStyles Page

    mStylesListWidget = new QListWidget( this );
    mDefaultStyleCheckBox = new QCheckBox( "Use the default style", this );

	mApplicationStylesPage = CreateGroupBox( ELayoutType::Vertical, 
	{ 
		nullptr, mStylesListWidget, mDefaultStyleCheckBox, nullptr 
	}, 
	"Application Styles", this, 6, 6, 6, 6, 6 );



	// Stack pages

	mStackedWidget = new CStackedWidget( this, { 
        { mApplicationPathsPage, "Paths", CActionInfo::FormatTip("Paths\nDefault application paths selection"), "://images/brat_paths.png", true },
		{ mStartupOptionsPage, "Startup", CActionInfo::FormatTip("Startup\nApplication start-up behavior"), "://images/OSGeo/tools.png", true },
		{ mRadsOptionsPage, "RADS", CActionInfo::FormatTip("RADS\nRADS data synchronization control"), "://images/rads.gif", true },
		{ mApplicationStylesPage, "Style", CActionInfo::FormatTip("Styles\nApplication visual options"), "://images/brat_style.png", true }
	} );


	auto *row = CreateButtonRow( true, Qt::Vertical, { mStackedWidget->Button( 0 ), mStackedWidget->Button( 1 ), mStackedWidget->Button( 2 ), mStackedWidget->Button( 3 ) } );
	auto *row_group = CreateGroupBox( ELayoutType::Vertical, { row } );

	auto *content_l = LayoutWidgets( Qt::Horizontal, { row_group, mStackedWidget }, nullptr, 6, 6, 6, 6, 6 );



    // Help
    //
    auto help = new CTextWidget;
    help->SetHelpProperties(
                "Application global options"
         ,0 , 6 );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


	// Dialog buttons

    mButtonBox = new QDialogButtonBox( this );
	mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
	mButtonBox->setOrientation( Qt::Horizontal );
    mButtonBox->setStandardButtons( QDialogButtonBox::Cancel| QDialogButtonBox::Ok );
    //mButtonBox->button( QDialogButtonBox::Ok )->setDefault( true );

	// Main layout

	LayoutWidgets( Qt::Vertical, { content_l, help_group, mButtonBox }, this, 6, 6, 6, 6, 6 );

	Wire();
}


void CApplicationSettingsDlg::Wire()
{
	const CApplicationPaths* settings_paths = &mSettings.BratPaths();

	//	ApplicationPaths Page

	mDataDirectoryLineEdit->setText( settings_paths->UserDataDirectory().c_str() );				//for data portable paths
	mProjectsDirectoryLineEdit->setText( settings_paths->WorkspacesDirectory().c_str() );

	mUsePortablePathsCheckBox->setChecked( settings_paths->UsePortablePaths() );

	connect( mBrowseDataDirectoryPushButton, SIGNAL( clicked() ), this, SLOT( HandleBrowseDataDirectory() ) );
	connect( mBrowseWorkspacesDirectoryPushButton, SIGNAL( clicked() ), this, SLOT( HandleBrowseProjectsPath() ) );


	//StartupOptions Page

	mLoadLastProjectAtAtartupCheckBox->setChecked( mSettings.mLoadLastWorkspaceAtStartUp );

	mUseVectorLayer->setChecked( mSettings.MainLayerBaseType() == CMapWidget::ELayerBaseType::eVectorLayer );
	mUseRasterLayer->setChecked( mSettings.MainLayerBaseType() == CMapWidget::ELayerBaseType::eRasterLayer );
	mUseRasterLayerWMS->setChecked( mSettings.MainLayerBaseType() == CMapWidget::ELayerBaseType::eRasterLayerWMS );

	mVectorSimplifyMethodCheck->setChecked( mSettings.mVectorSimplifyMethod );

	connect( mUseRasterLayer, SIGNAL( toggled( bool ) ), this, SLOT( HandleMainLayerTypeChanged( bool ) ) );
	connect( mRasterLayerBrowseButton, &QPushButton::clicked, this, &CApplicationSettingsDlg::HandleBrowseRasterLayerFile );
	connect( mUseVectorLayer, SIGNAL( toggled( bool ) ), this, SLOT( HandleMainLayerTypeChanged( bool ) ) );
	connect( mUseRasterLayerWMS, SIGNAL( toggled( bool ) ), this, SLOT( HandleMainLayerTypeChanged( bool ) ) );
	connect( mResetLayerOptionsPushButton, &QPushButton::clicked, this, &CApplicationSettingsDlg::HandleResetLayerOptions );

	connect( mMapViewsDefaultLayerDialogPushButton, &QToolButton::clicked, this, &CApplicationSettingsDlg::HandleViewsDefaultLayerDialog );

	mRasterLayerLineEdit->setText( settings_paths->RasterLayerPath().c_str() );
	mLayerWMSLineEdit->setText( settings_paths->WMSRasterLayerPath().c_str() );

	HandleMainLayerTypeChanged( false );		//argument not used


	//RADS Page

	if ( CheckRadsConfigStatus() )				//can disable page
	{
		mRadsOutputLineEdit->setText( mRadsServiceSettings.UserWritableDirectory().c_str() );
		mRadsSpin->setValue( mRadsServiceSettings.NumberOfDays() );

		size_t max_len = 0;
		auto const &user_missions = mRadsServiceSettings.MissionNames();
		for ( auto const &mission : mRadsServiceSettings.AllMissions() )
		{
			max_len = std::max( max_len, mission.mName.length() );
			QListWidgetItem* item = new QListWidgetItem;
			item->setText( t2q( mission.mName ) );
			item->setFlags( item->flags() | Qt::ItemIsUserCheckable );
			bool selected = std::find( user_missions.begin(), user_missions.end(), mission.mAbbr ) != user_missions.end();
			item->setCheckState( selected ? Qt::Checked : Qt::Unchecked );
			item->setData( Qt::UserRole, t2q( mission.mAbbr ) );
			mRadsMissionsList->addItem( item );
		}
		// Sort items (ascending order)
		mRadsMissionsList->sortItems();
		SetMaximumVisibleItems( mRadsMissionsList, 6 );
		mRadsMissionsList->setMaximumWidth( max_len * mRadsMissionsList->fontMetrics().width('W') + qApp->style()->pixelMetric( QStyle::PM_ScrollBarExtent ) );

		const bool installed = mRadsController.isInstalled();
		const bool running = installed && mRadsController.isRunning();
		mRadsInstallButton->setChecked( installed );
		mRadsStartButton->setChecked( running );
		mRadsStartButton_Test->setChecked( running );
		mRadsPauseButton->setChecked( running );
		UpdateRadsButtons();

		connect( mRadsInstallButton, SIGNAL( toggled( bool ) ), this, SLOT( HandleRadsInstall( bool ) ) );
		connect( mRadsStartButton, SIGNAL( toggled( bool ) ), this, SLOT( HandleRadsUserStart( bool ) ) );
		connect( mRadsStartButton_Test, SIGNAL( toggled( bool ) ), this, SLOT( HandleRadsStart_Test( bool ) ) );
        connect( mRadsRefreshConnectionButton, SIGNAL( clicked() ), this, SLOT( HandleRadsRefreshConnection() ) );
		connect( mBrowseRadsOutputPushButton, SIGNAL( clicked() ), this, SLOT( HandleBrowseRadsOutputDirectory() ) );
		connect( mRadsPauseButton, SIGNAL( toggled( bool ) ), this, SLOT( HandleRadsPause( bool ) ) );
		connect( mRadsViewLogFile, SIGNAL( clicked() ), this, SLOT( HandleViewLogFile() ) );
		connect( mRadsViewRADSConfigurationFile, SIGNAL( clicked() ), this, SLOT( HandleViewRADSConfigurationFile() ) );
		connect( mRadsViewRADSServiceSettingsFile, SIGNAL( clicked() ), this, SLOT( HandleViewRADSServiceSettingsFile() ) );
		connect( mRadsExecuteNow, SIGNAL( toggled( bool ) ), this, SLOT( HandleRadsExecuteNow( bool ) ) );
		connect( mRadsSpin, (void (QSpinBox::*)(int))&QSpinBox::valueChanged, this, &CApplicationSettingsDlg::HandleRadsSpinValueChanged );

		connect( &mApp, &CBratApplication::RadsNotification, this, &CApplicationSettingsDlg::HandleRsyncStatusChanged, Qt::QueuedConnection );

		HandleRadsSpinValueChanged( mRadsSpin->value() );
		HandleRsyncStatusChanged( 
			running ? (
				mApp.IsRsyncActive() ? 
				CBratApplication::ERadsNotification::eNotificationRsyncRunnig : 
				CBratApplication::ERadsNotification::eNotificationRsyncStopped )
			: CBratApplication::ERadsNotification::eNotificationUnknown );

#if defined (Q_OS_WIN)
		mRadsOptionsPage->setEnabled( mIsElevatedProcess );
#endif
	}


    //	Application Styles

	mStylesListWidget->setSelectionMode( QAbstractItemView::SingleSelection );
	const std::vector< std::string > &styles = CBratSettings::getStyles();
	FillList( mStylesListWidget, styles, (int)CBratSettings::getStyleIndex( mSettings.mAppStyle ), true );

    mDefaultStyleCheckBox->setChecked( mSettings.mUseDefaultStyle );
	mStylesListWidget->setDisabled( mDefaultStyleCheckBox->isChecked() );
	connect( mDefaultStyleCheckBox, SIGNAL( clicked( bool ) ), mStylesListWidget, SLOT( setDisabled( bool ) ) );


    //button box

    connect( mButtonBox, SIGNAL(accepted()), this, SLOT(accept()) );
    connect( mButtonBox, SIGNAL(rejected()), this, SLOT(reject()) );


    adjustSize();
    setMinimumWidth( width() );
    setMaximumHeight( height() );
}


// (*) Do NOT use mSettings or mApp here, it is not assigned yet. The ctor follows the member 
// variables declaration order and when arrives to mViewMapsLayerBaseType uses mSettings
// as defined, but mSettings comes only later in member variables list, and has only 
// default values (if exists at all)
// 
CApplicationSettingsDlg::CApplicationSettingsDlg( CBratApplication &app, QWidget *parent )
	: QDialog( parent )
	, mApp( app )
	, mSettings( mApp.Settings() )
	, mViewMapsLayerBaseType( app.Settings().mViewsLayerBaseType )		//(*)
	, mRadsController( app.RadsServiceController() )
	, mRadsServiceSettings( app.mRadsServiceSettings )
#if defined (Q_OS_WIN)
	, mIsElevatedProcess( IsElevatedProcess() )
#endif
{
	CreateWidgets();
	setWindowTitle( QCoreApplication::applicationName() + " Options" );
}

//virtual 
CApplicationSettingsDlg::~CApplicationSettingsDlg()
{}




//////////////////////////
//	ApplicationPaths_page
//////////////////////////


void CApplicationSettingsDlg::HandleBrowseDataDirectory()
{
    QString dir = BrowseDirectory( this, "Select Default Data Directory", mDataDirectoryLineEdit->text() );
	if ( !dir.isEmpty() )
	{
		mDataDirectoryLineEdit->setText( dir );
	}
}


void CApplicationSettingsDlg::HandleBrowseProjectsPath()
{
    QString dir = BrowseDirectory( this, "Select Default Workspaces Directory", mProjectsDirectoryLineEdit->text() );
    if ( !dir.isEmpty() )
        mProjectsDirectoryLineEdit->setText( dir );
}



//////////////////////////
//	StartupOptions_page
//////////////////////////

void CApplicationSettingsDlg::HandleBrowseRasterLayerFile()
{
	QString path = BrowseFile( this, "Select Local Raster Layer File", mRasterLayerLineEdit->text() );
	if ( !path.isEmpty() )
	{
		mRasterLayerLineEdit->setText( path );
	}
}

void CApplicationSettingsDlg::HandleMainLayerTypeChanged( bool )
{
	mRasterLayerLineEdit->setEnabled( mUseRasterLayer->isChecked() || mViewMapsLayerBaseType == CMapWidget::ELayerBaseType::eRasterLayer );
	mRasterLayerBrowseButton->setEnabled( mUseRasterLayer->isChecked() || mViewMapsLayerBaseType == CMapWidget::ELayerBaseType::eRasterLayer );
    mLayerWMSLineEdit->setEnabled( mUseRasterLayerWMS->isChecked() || mViewMapsLayerBaseType == CMapWidget::ELayerBaseType::eRasterLayerWMS );
	mVectorSimplifyMethodCheck->setEnabled( mUseVectorLayer->isChecked() || mViewMapsLayerBaseType == CMapWidget::ELayerBaseType::eVectorLayer );
}

void CApplicationSettingsDlg::HandleViewsDefaultLayerDialog()
{
	CViewMapsLayerDialog dlg( mViewMapsLayerBaseType, true, true, this );
	if ( dlg.exec() == QDialog::Accepted )
	{
		auto type = dlg.LayerBaseType();		//assert__( type != CMapWidget::ELayerBaseType::eRasterLayerWMS || !url.empty() );

		mViewMapsLayerBaseType = type;

		HandleMainLayerTypeChanged( false );	//boolean not used
	}
}


void CApplicationSettingsDlg::HandleResetLayerOptions()
{
	mUseVectorLayer->setChecked( mSettings.smDefaultLayerBaseType == CMapWidget::ELayerBaseType::eVectorLayer );
	mUseRasterLayer->setChecked( mSettings.smDefaultLayerBaseType == CMapWidget::ELayerBaseType::eRasterLayer );
	mUseRasterLayerWMS->setChecked( mSettings.smDefaultLayerBaseType == CMapWidget::ELayerBaseType::eRasterLayerWMS );
	
	mRasterLayerLineEdit->setText( t2q( mSettings.BratPaths().DefaulLocalFileRasterLayerPath() ) );
	mLayerWMSLineEdit->setText( mSettings.BratPaths().smDefaultWMSRasterLayerPath.c_str() );
	mVectorSimplifyMethodCheck->setChecked( mSettings.smVectorSimplifyMethod );
}


//////////////////////////
//	RADS page
//////////////////////////

bool CApplicationSettingsDlg::CheckRadsConfigStatus()
{
	// mValidRadsMissions variable records the status of reading BRADS configuration file (not the service settings file); note
	//	that it is const, therefore initialized in ctor, not depending of LoadConfig call (this one yes, uses the service settings file)
	//
	std::string rads_error_msg;
	bool result = mRadsServiceSettings.mValidRadsMissions;
	if ( !result )
		rads_error_msg = "An error occurred reading missions from rads configuration file.\nPlease check " + mSettings.BratPaths().mRadsConfigurationFilePath.mPath;
	else
	{
		result = mRadsServiceSettings.Status() == QSettings::Status::NoError;
		if ( !result )
			rads_error_msg = "Error reading RADS Service configuration file '" + mRadsServiceSettings.FilePath() + "'";
		else
		{
			//result = mSharedMemory.create( sizeof( CRadsSharedMemory ) );
			//if ( !result )
			//{
			//	rads_error_msg = "Unable to communicate with RADS service.";
			//}
		}
	}

	if ( !result )
	{
		SimpleErrorBox( rads_error_msg + "\nRADS settings will not be available." );
		mRadsOptionsPage->setEnabled( false );
	}

	return result;
}



void CApplicationSettingsDlg::HandleRadsRefreshConnection()
{
    mApp.ResetRadsSocketConnection();
	UpdateRadsButtons();
}


void CApplicationSettingsDlg::SetExecNowText()
{
	static const QString synchronize_now = "Synchronize now...";
	static const QString stop_synchronization ="Stop synchronization";

	mRadsExecuteNow->setText( mRadsExecuteNow->isChecked() ? stop_synchronization : synchronize_now );
	;
}


void CApplicationSettingsDlg::HandleRsyncStatusChanged( CBratApplication::ERadsNotification notification )
{
    static bool processing = false;

    // This function can be re-entered. Ensure sequential processing,
    //  furthermore because displayed dialogs have undefined time (<= auto_close_time)
    //  to close but the thread's event loop keeps running.
    //
    if ( processing )
    {
        QTimer::singleShot( 2000, this,
        [this, notification]()
        {
            HandleRsyncStatusChanged( notification );
        }
        );
        return;
    }

    boolean_locker_t l ( processing );
    
	QString text = mRsyncStatusLabel->text();			//preserve existing text in case notification is not related
	mRadsExecuteNow->blockSignals( true );

	switch ( notification )
	{
		case CBratApplication::eNotificationRsyncRunnig:
		{
			text = "The RADS service is synchronizing data";
			mRadsExecuteNow->setChecked( true );
		}
		break;

		case CBratApplication::eNotificationRsyncStopped:
		{
#if defined(DEBUG) || defined(_DEBUG)
			text = "The RADS service is idle.";
#else
			text = "";
#endif
			mRadsExecuteNow->setChecked( false );
		}
		break;

		case CBratApplication::eNotificationUnknown:
		{
#if defined(DEBUG) || defined(_DEBUG)
			text = "Cannot detect RADS service activity";
#else
			text = "";
#endif
		}
		//break;	no break!

		case CBratApplication::eNotificationConfigSaved:
		{
			if ( CheckRadsConfigStatus() )							//can disable page
				HandleRadsSpinValueChanged( mRadsSpin->value() );
		}
		break;

		default:        
			assert__( false );
	}

	SetExecNowText();
	mRadsExecuteNow->blockSignals( false );

	mRsyncStatusLabel->setText( text );
	mRadsOutputBox->setEnabled( !mApp.IsRsyncActive() );
}


void CApplicationSettingsDlg::UpdateRadsButtons()
{
	const bool installed = mRadsController.isInstalled();		assert__( !installed || mRadsInstallButton->isChecked() );		assert__( installed || !mRadsInstallButton->isChecked() );
	const bool running = installed && mRadsController.isRunning();

	mRadsInstallButton->setEnabled( !running );
	//mRadsInstallButton->setVisible( !installed );
	mRadsInstallButton->setText( installed ? "Uninstall service" : "Install service"  );
	mRadsSettingsBox->setEnabled( installed );
	mRadsRefreshConnectionButton->setEnabled( installed );

    mRadsStartButton->setText( running ? "Stop service" : "Start service" );
	mRadsStartButton->setEnabled( installed );
	mRadsStartButton_Test->setEnabled( installed );

	mRadsPauseButton->setEnabled( running );
	mRadsExecuteNow->setEnabled( running );
	SetExecNowText();
}


void CApplicationSettingsDlg::DisplayRadsError( const std::string & action )
{
	SimpleErrorBox( "Could not " + action + " RADS service" );
}


void CApplicationSettingsDlg::HandleRadsInstall( bool toggled )
{
	const bool was_installed = mRadsController.isInstalled();
	std::string error_token;
	bool user_canceled = false;

	if ( toggled )
	{
		if ( !was_installed )
		{
			// Do not display WaitCursor, installation opens dialogs
			// Do not call ValidateAndAssignRadsValues here: the settings must still be configured by the user
			if ( !user_canceled && !mApp.InstallRadsService( this ) )
				error_token = "install";
		}
	}
	else
	{
		if ( was_installed )
		{
			user_canceled = !SimpleQuestion( "Are you sure you want to uninstall the RADS service?" );
			if ( !user_canceled )
			{
				WaitCursor wait;

				if ( !mApp.UninstallRadsService() )
					error_token = "uninstall";			//Windows 1072: "The specified service has been marked for deletion."
			}
		}
	}

	if ( !error_token.empty() )
		DisplayRadsError( error_token );


	WaitCursor::GlobalWait();

	QTimer::singleShot( 2000, this, [this, was_installed, error_token, user_canceled]() 
	{
		WaitCursor::GlobalRestore();

		if ( !error_token.empty() || user_canceled )
		{
			mRadsInstallButton->blockSignals( true );
			mRadsInstallButton->setChecked( mRadsController.isInstalled() );
			mRadsInstallButton->blockSignals( false );
		}
		else
		{
			if ( mRadsController.isInstalled() && !was_installed && !mRadsController.isRunning() )
				SimpleMsgBox(
					QString( "The RADS service was installed successfully.\n\n" )
					+ "Before starting periodic data synchronizations with the RADS server, please select your options in '" 
					+ mRadsSettingsBox->title()
                    + "'. The service will start automatically when the system restarts.\nYou can start the service now with the '"
                    + mRadsStartButton->text()
                    + "' button."
				);
		}

		UpdateRadsButtons();
	} );
}


// Returns false if user canceled or operation failed
//
bool CApplicationSettingsDlg::RadsStart( bool toggled, bool validate_and_save, bool check_buttons )
{
	assert__( mRadsController.isInstalled() );

	const bool running = mRadsController.isRunning();
	std::string error_token;
	bool user_canceled = false;

	if ( toggled )
	{
		if ( !running )
		{
			user_canceled = validate_and_save && !ValidateAndAssignRadsValues( true );
			if ( !user_canceled )
			{
				WaitCursor wait( true );

				if ( !mApp.StartRadsService() )
					error_token = "start";					//Windows 1069: "The service did not start due to a logon failure."
			}
		}
	}
	else
	{
		if ( running )
		{
			user_canceled = !SimpleQuestion( "This will stop any downloads from the RADS server.\nDo you want to proceed?" );
			if ( !user_canceled )
			{
				WaitCursor wait( true );

				if ( !mApp.StopRadsService() )
					error_token = "stop";
			}
		}
	}

	if ( !error_token.empty() )
		DisplayRadsError( error_token );

	if ( !error_token.empty() || user_canceled )
	{
		mRadsStartButton->blockSignals( true );
		mRadsStartButton_Test->blockSignals( true );
		mRadsStartButton->setChecked( mRadsController.isRunning() );
		mRadsStartButton_Test->setChecked( mRadsController.isRunning() );
		mRadsStartButton->blockSignals( false );
		mRadsStartButton_Test->blockSignals( false );
	}

	if ( check_buttons )
		UpdateRadsButtons();

	return error_token.empty() && !user_canceled;
}

void CApplicationSettingsDlg::HandleRadsStart_Test( bool toggled )
{
	RadsStart( toggled, true, true );
}

// Assuming the service stars with rads client paused. In production mode
//	it is should be immediately active.
//
void CApplicationSettingsDlg::HandleRadsUserStart( bool toggled )
{
	if ( RadsStart( toggled, true, true ) && toggled )
		RadsPause( false, false );
}


bool CApplicationSettingsDlg::RadsPause( bool toggled, bool validate_and_save )
{
	assert__( mRadsController.isInstalled() );

	std::string error_token;
	bool user_canceled = false;	

	if ( toggled )
	{
		WaitCursor wait;

		if ( !mApp.PauseRadsService() )
			error_token = "pause";
	}
	else
	{
		user_canceled = validate_and_save && !ValidateAndAssignRadsValues( true );
		if ( !user_canceled )
		{
			WaitCursor wait;

			if ( !mApp.ResumeRadsService() )
				error_token = "resume";
		}
	}

	if ( !error_token.empty() )
		DisplayRadsError( error_token );

	if ( !error_token.empty() || user_canceled )
	{
		mRadsPauseButton->blockSignals( true );
		mRadsPauseButton->setChecked( !toggled );	//Guessing: without a flag for paused state, we can only assume that, on failure, the current state did not change
		mRadsPauseButton->blockSignals( false );
	}

	return !error_token.empty() || user_canceled;
}

void CApplicationSettingsDlg::HandleRadsPause( bool toggled )
{
	RadsPause( toggled, true );
}


void CApplicationSettingsDlg::HandleRadsExecuteNow( bool toggled )
{
	std::string error_token;
	bool user_canceled = false;	
	const bool was_checked = !mRadsExecuteNow->isChecked();

	if ( toggled )
	{
		user_canceled = !ValidateAndAssignRadsValues( true );
		if ( !user_canceled )
		{
			WaitCursor wait;

			if ( !mApp.SendRadsServiceCommand( eRadsService_ExecNow ) )
				error_token = "send synchronization command to";
		}
	}
	else
	{
		user_canceled = !SimpleQuestion( "Are you sure you want to stop the current synchronization with RADS server?" );
		if ( !user_canceled )
		{
			WaitCursor wait;

			if ( !mApp.SendRadsServiceCommand( eRadsService_StopExec ) )
				error_token = "send stop synchronization command to";
		}
	}

	if ( !error_token.empty() )
		DisplayRadsError( error_token );

	if ( user_canceled )	//text will be corrected in UpdateRadsButtons
	{
		mRadsExecuteNow->blockSignals( true );
		mRadsExecuteNow->setChecked( was_checked );
		mRadsExecuteNow->blockSignals( false );
	}

    UpdateRadsButtons();
}


void CApplicationSettingsDlg::HandleBrowseRadsOutputDirectory()
{
	QString dir = BrowseDirectory( this, "Select RADS Data Directory", mRadsOutputLineEdit->text() );
	if ( !dir.isEmpty() )
	{
		if ( mApp.IsRsyncActive() )
		{
			//It can happen that a notification of rsync activity arrives while the dialog box is open
			//
			SimpleWarnBox( "The data directory cannot be changed because a data synchronization with RADS has started.\nPlease, try again when it finishes." );
		}
		else
		{
			mRadsOutputLineEdit->setText( dir );
		}
	}
}


void CApplicationSettingsDlg::HandleRadsSpinValueChanged( int i )
{
	assert__( i = mRadsSpin->value() );

	mNextSyncLabel->setText( "Next synchronization will be scheduled for\n" 
		+ mRadsServiceSettings.NextSyncDateForPeriodWithDays( i ).toString( CSharedRadsSettings::RsyncDateFormat() ) );
}


void CApplicationSettingsDlg::HandleViewLogFile()
{
	QDesktopServices::openUrl( QUrl( mRadsServiceSettings.LogFilePath().c_str() ) );
}


void CApplicationSettingsDlg::HandleViewRADSConfigurationFile()
{
	QDesktopServices::openUrl( QUrl( mSettings.BratPaths().mRadsConfigurationFilePath.mPath.c_str() ) );
}


void CApplicationSettingsDlg::HandleViewRADSServiceSettingsFile()
{
	assert__( mRadsServiceSettings.FilePath() == CSharedRadsSettings::RadsSettingsFilePath() );

	QDesktopServices::openUrl( QUrl( mRadsServiceSettings.FilePath().c_str() ) );
}




//////////////////////////
//	ApplicationStyles_page
//////////////////////////

// ( ... )


///////////////////////////////////////////////////////////////////////////////////
//
//						Validation and Conclusion
//
///////////////////////////////////////////////////////////////////////////////////


bool AskCreateDir( const std::string &dir_name, const std::string &dir )
{
	QMessageBox msg_userdir;
	msg_userdir.setText( t2q( dir_name + " does not exist." ) );
	msg_userdir.setInformativeText( t2q( "Would you like to create '" + dir + "' ?" ) );
	msg_userdir.setStandardButtons(QMessageBox::Ok |  QMessageBox::Cancel);
	msg_userdir.setDefaultButton(QMessageBox::Ok);
	int user_option = msg_userdir.exec();
	if (user_option != QMessageBox::Ok)
	{
		return false;
	}
	if (!MakeDirectory(dir))
	{
		SimpleErrorBox( "Could not create path " + dir );
		return false;
	}
	return true;
}


// IMPORTAT: if called with validate_paths false, call this only after validating user data path
//
// Returns false only when 
//	- the user cancels
//	- could not write or synchronize configuration settings with RADS service
//
bool CApplicationSettingsDlg::ValidateAndAssignRadsValues( bool ask_user, bool validate_paths )	//validate_paths = true 
{
	if ( ask_user && !SimpleQuestion( "This will save your current RADS settings.\nDo you want to proceed?" ) )
		return false;

	if ( validate_paths && !ValidateAndAssignPaths() )
		return false;

	if ( mRadsOptionsPage->isEnabled() )
	{
		const int size = mRadsMissionsList->count();
		std::string missions_str;
		for ( int i = 0; i < size; ++i )
		{
			auto *item = mRadsMissionsList->item( i );
			if ( item->checkState() == Qt::Checked )
			{
				const std::string &abbr = q2a( item->data( Qt::UserRole ).toString() );		assert__( FindRadsMissionAbbr( q2a( item->text() ), mRadsServiceSettings.AllMissions() ) == abbr );
				if ( !missions_str.empty() )
					missions_str += missions_separator;
				missions_str += abbr;
			}
		}

		int ndays = mRadsSpin->value();
		const bool running = mRadsController.isRunning();

		std::string rads_dir = q2a( mRadsOutputLineEdit->text() );
		if ( !IsDir( rads_dir ) && !AskCreateDir( "RADS data download directory", rads_dir ) )
		{
			return false;
		}

		bool cmd_success = !running || mApp.SendRadsServiceCommand( eRadsService_LockConfig );
		if ( cmd_success )
		{
			// This validity (valid_values truth) must be ensured by the programmer, not the user
			//
			bool valid_values = mRadsServiceSettings.SetApplicationParameterValues( missions_str, ndays, rads_dir );		assert__( valid_values );
			cmd_success = 
				valid_values
				&&
				( !running || mApp.SendRadsServiceCommand( eRadsService_UnlockConfig ) );
		}
		if ( !cmd_success )
		{
			std::string msg = "Could not write or synchronize configuration settings with the RADS service.\n";
			msg += "Some parameter values may not be updated.";
			SimpleErrorBox( msg );
			return false;
		}

		assert__( IsSubDirectory( mRadsServiceSettings.UserWritableDirectory(), mRadsServiceSettings.DownloadDirectory() ) );
	}

	return true;
}


// Returns false only when the user cancels
//
bool CApplicationSettingsDlg::ValidateAndAssignPaths()
{
	std::string user_dir = q2a(mDataDirectoryLineEdit->text());
	std::string workspace_dir = q2a(mProjectsDirectoryLineEdit->text());

	if ( !IsDir( user_dir ) && !AskCreateDir( "User data working directory", user_dir ) )
	{
		return false;
	}

	if ( !IsDir(workspace_dir) && !AskCreateDir( "Workspaces directory.", workspace_dir ) )
	{
		return false;
	}

	//Its safe now to apply these procedures

	return 
		mSettings.SetUserDataDirectory( mUsePortablePathsCheckBox->isChecked(), user_dir )
		&&
		mSettings.SetWorkspacesDirectory( workspace_dir );
}


bool CApplicationSettingsDlg::ValidateAndAssign()
{
	//	1. Application Paths

	if ( !ValidateAndAssignPaths() )
		return false;


	//	2. Startup Options

	mSettings.mLoadLastWorkspaceAtStartUp = mLoadLastProjectAtAtartupCheckBox->isChecked();

    if ( mUseRasterLayer->isChecked() || mViewMapsLayerBaseType == CMapWidget::ELayerBaseType::eRasterLayer )
    {
		std::string path = q2a( mRasterLayerLineEdit->text() );
		if ( path.empty() )
		{
			mRasterLayerLineEdit->setFocus();
			const std::string used_by = mViewMapsLayerBaseType == CMapWidget::ELayerBaseType::eRasterLayer ? "views" : "main";
			SimpleErrorBox( "The raster layer is used by the " + used_by + " map and cannot be empty" );
			return false;
		}
        mSettings.SetRasterLayerPath( path );
    }

	if ( mUseRasterLayerWMS->isChecked() || mViewMapsLayerBaseType == CMapWidget::ELayerBaseType::eRasterLayerWMS )
	{
		std::string url = q2a( mLayerWMSLineEdit->text() );
		if ( url.empty() )
		{
			mLayerWMSLineEdit->setFocus();
			const std::string used_by = mViewMapsLayerBaseType == CMapWidget::ELayerBaseType::eRasterLayerWMS ? "views" : "main";
			SimpleErrorBox( "The layer URL is used by the " + used_by + " map and cannot be empty" );
			return false;
		}
		mSettings.SetWMSRasterLayerPath( url );
	}

	if ( mUseVectorLayer->isChecked() )
        mSettings.mMainLayerBaseType = CMapWidget::ELayerBaseType::eVectorLayer;
    else
    if ( mUseRasterLayer->isChecked() )
        mSettings.mMainLayerBaseType = CMapWidget::ELayerBaseType::eRasterLayer;
    else
    if ( mUseRasterLayerWMS->isChecked() )
        mSettings.mMainLayerBaseType = CMapWidget::ELayerBaseType::eRasterLayerWMS;
    else
        assert__( false );

	mSettings.mViewsLayerBaseType = mViewMapsLayerBaseType;

	mSettings.mVectorSimplifyMethod = mVectorSimplifyMethodCheck->isChecked();


#if defined(DEBUG) || defined(_DEBUG)
	assert__( mDesktopManagerSdiCheckbox );
	mSettings.mDesktopManagerSdi = mDesktopManagerSdiCheckbox->isChecked();
#endif


	//	3. RADS

	
	if ( mRadsController.isInstalled() )
		ValidateAndAssignRadsValues( false, false );	//Can we disregard possible failure on RADS parameters save an let the dialog close?


	//	4. Application Styles

	if ( mStylesListWidget->currentItem() )
		mSettings.mAppStyle = mStylesListWidget->currentItem()->text();
    mSettings.mUseDefaultStyle = mDefaultStyleCheckBox->isChecked();

    return true;
}


void CApplicationSettingsDlg::accept()
{
    if ( ValidateAndAssign() )
        QDialog::accept();
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ApplicationSettingsDlg.cpp"
