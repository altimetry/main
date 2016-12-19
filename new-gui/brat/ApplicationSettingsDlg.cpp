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
#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/System/Service/qtservice.h"
#include "new-gui/Common/GUI/TextWidget.h"
#include "GUI/ActionsTable.h"
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

	auto *rads_info_label = new CTextWidget;
	rads_info_label->SetHelpProperties(
		"If you enable RADS, data will be downloaded to\nthe \"rads\" folder in this directory."
		,0 , 6 );

	auto *data_l =
		LayoutWidgets( Qt::Vertical, {
			new QLabel( "Default Data Directory" ),
			LayoutWidgets( Qt::Horizontal, { mDataDirectoryLineEdit, mBrowseDataDirectoryPushButton }, nullptr, 2, 2, 2, 2, 2 ),
			LayoutWidgets( Qt::Horizontal, { rads_info_label, nullptr, mUsePortablePathsCheckBox }, nullptr, 2, 2, 2, 2, 2 ),
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
	mUseRasterLayer = new QRadioButton( "Use a raster layer" );
    mUseRasterLayerURI = new QRadioButton( "Use a raster layer URI" );
    auto *main_layers_group = CreateGroupBox( ELayoutType::Vertical,
                                        {
                                            mUseVectorLayer,
                                            mUseRasterLayer,
                                            mUseRasterLayerURI
                                        },
                                        "Main Map Base Layer (requires restart)", this );

	mViewsUseVectorLayer = new QRadioButton( "Use vector layers" );
	mViewsUseRasterLayer = new QRadioButton( "Use raster layers" );
	mViewsUseRasterLayerURI = new QRadioButton( "Use a raster layer URI" );
    auto *views_layers_group = CreateGroupBox( ELayoutType::Vertical,
                                        {
                                            mViewsUseVectorLayer,
                                            mViewsUseRasterLayer,
											mViewsUseRasterLayerURI
                                        },
                                        "View Maps Base Layer", this );

    mLayerURLLineEdit = new QLineEdit;
	mVectorSimplifyMethodCheck = new QCheckBox( "Simplify vector layer geometry" );
	mVectorSimplifyMethodCheck->setToolTip( "Check to improve performance, leave unchecked to improve map projections" );

	auto *layers_l = LayoutWidgets( Qt::Vertical,
	{
		LayoutWidgets( Qt::Horizontal, { main_layers_group, views_layers_group }, nullptr, 2, 2, 2, 2, 2 ),
		LayoutWidgets( Qt::Horizontal, { new QLabel( "Raster Layer URI" ), mLayerURLLineEdit }, nullptr, 2, 2, 2, 2, 2 ),
		mVectorSimplifyMethodCheck
	}, 
	nullptr, 2, 2, 2, 2, 2 );


#if defined(DEBUG) || defined(_DEBUG)

	mDesktopManagerSdiCheckbox = new QCheckBox( "Single Document Interface desktop manager" );
	mDesktopManagerSdiCheckbox->setChecked( mSettings.mDesktopManagerSdi );

	mStartupOptionsPage = CreateGroupBox( ELayoutType::Vertical, 
	{ 
		nullptr, mLoadLastProjectAtAtartupCheckBox, nullptr, layers_l, nullptr, mDesktopManagerSdiCheckbox, nullptr
	}
	, "Startup Options", this, 6, 6, 6, 6, 6 );
#else

	mStartupOptionsPage = CreateGroupBox( ELayoutType::Vertical, 
	{ 
        nullptr, mLoadLastProjectAtAtartupCheckBox, nullptr, layers_l, nullptr,
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

	QLabel *picon = new QLabel;
#if defined (Q_OS_WIN)
	QIcon icon = QApplication::style()->standardIcon( QStyle::SP_VistaShield );
	QPixmap pixmap = icon.pixmap( icon.actualSize( QSize( icon_size, icon_size ) ) );
	picon->setPixmap( pixmap );
	picon->setToolTip( "To install and activate the RADS service you may need to run brat as administrator." );
#endif

	auto *admin_box = CreateGroupBox( ELayoutType::Horizontal,
	{
		picon, nullptr, mRadsInstallButton, nullptr, mRadsStartButton, nullptr
	}, "", nullptr, 2, 2, 2, 2, 2 );


	//...settings box

	mRadsOutputEdit = new QLineEdit;
	SetReadOnlyEditor( mRadsOutputEdit, true );
	mRadsOutputEdit->setToolTip( "Change location by changing the default BRAT data directory" );
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
		LayoutWidgets( Qt::Horizontal, { new QLabel( "Destination Path" ), mRadsOutputEdit }, nullptr, 2,2,2,2,2 ),
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
		admin_box,
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



    // Help				TODO
    //
    auto help = new CTextWidget;
    help->SetHelpProperties(
                "Application global options"
         ,0 , 6 );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


	// Dialog buttons

    mButtonBox = new QDialogButtonBox( this );
    mButtonBox->setObjectName(QString::fromUtf8("mButtonBox"));
    mButtonBox->setMinimumSize(QSize(495, 25));
    mButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);


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
	mUseRasterLayerURI->setChecked( mSettings.MainLayerBaseType() == CMapWidget::ELayerBaseType::eRasterURL );

	mViewsUseVectorLayer->setChecked( mSettings.ViewsLayerBaseType() == CMapWidget::ELayerBaseType::eVectorLayer );
	mViewsUseRasterLayer->setChecked( mSettings.ViewsLayerBaseType() == CMapWidget::ELayerBaseType::eRasterLayer );
	mViewsUseRasterLayerURI->setChecked( mSettings.ViewsLayerBaseType() == CMapWidget::ELayerBaseType::eRasterURL );

	mVectorSimplifyMethodCheck->setChecked( mSettings.mVectorSimplifyMethod );

	connect( mUseRasterLayer, SIGNAL( toggled( bool ) ), this, SLOT( HandleMainLayerTypeChanged( bool ) ) );
	connect( mUseVectorLayer, SIGNAL( toggled( bool ) ), this, SLOT( HandleMainLayerTypeChanged( bool ) ) );
	connect( mUseRasterLayerURI, SIGNAL( toggled( bool ) ), this, SLOT( HandleMainLayerTypeChanged( bool ) ) );

	connect( mViewsUseRasterLayer, SIGNAL( toggled( bool ) ), this, SLOT( HandleViewsLayerTypeChanged( bool ) ) );
	connect( mViewsUseVectorLayer, SIGNAL( toggled( bool ) ), this, SLOT( HandleViewsLayerTypeChanged( bool ) ) );
	connect( mViewsUseRasterLayerURI, SIGNAL( toggled( bool ) ), this, SLOT( HandleViewsLayerTypeChanged( bool ) ) );

	mLayerURLLineEdit->setText( settings_paths->URLRasterLayerPath().c_str() );

	HandleMainLayerTypeChanged( false );		//argument not used
	HandleViewsLayerTypeChanged( false );		//idem


	//RADS Page

	if ( CheckRadsConfigStatus() )							//can disable page
	{
		// Do not use mRadsServiceSettings.OutputDirectory(); this is not a stored path, it is always constructed
		//	based on UserDataDirectory; besides, if this is the first time, UserDataDirectory will not be configured
		//	inside rads settings
		//
		mRadsOutputEdit->setText( CRadsSettings::FormatRadsLocalOutputFolder( settings_paths->UserDataDirectory() ).c_str() );
		mRadsSpin->setValue( mRadsServiceSettings.NumberOfDays() );

		size_t max_len = 0;
		auto const &user_missions = mRadsServiceSettings.MissionNames();
		for ( auto const &mission : mRadsServiceSettings.AllAvailableMissions() )
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
		SetMaximumVisibleItems( mRadsMissionsList, 7 );
		mRadsMissionsList->setMaximumWidth( max_len * mRadsMissionsList->fontMetrics().width('W') + qApp->style()->pixelMetric( QStyle::PM_ScrollBarExtent ) );

		const bool installed = mRadsController.isInstalled();
		const bool running = installed && mRadsController.isRunning();
		mRadsInstallButton->setChecked( installed );
		mRadsStartButton->setChecked( running );
		mRadsStartButton_Test->setChecked( running );
		mRadsPauseButton->setChecked( running );
		EnableRadsButtons();

		connect( mRadsInstallButton, SIGNAL( toggled( bool ) ), this, SLOT( HandleRadsInstall( bool ) ) );
		connect( mRadsStartButton, SIGNAL( toggled( bool ) ), this, SLOT( HandleRadsUserStart( bool ) ) );
		connect( mRadsStartButton_Test, SIGNAL( toggled( bool ) ), this, SLOT( HandleRadsStart( bool ) ) );
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
				mApp.RsyncIsActive() ? 
				CBratApplication::ERadsNotification::eNotificationRsyncRunnig : 
				CBratApplication::ERadsNotification::eNotificationRsyncStopped )
			: CBratApplication::ERadsNotification::eNotificationUnknown );
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


CApplicationSettingsDlg::CApplicationSettingsDlg( CBratApplication &app, QWidget *parent )
	: QDialog( parent )
	, mApp( app )
	, mSettings( mApp.Settings() )
	, mRadsController( mApp.RadsServiceController() )
	, mRadsServiceSettings( mApp.mRadsServiceSettings )
{
	CreateWidgets();
	setWindowTitle( "BRAT Options" );
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
		mRadsOutputEdit->setText( CRadsSettings::FormatRadsLocalOutputFolder( q2a( dir ) ).c_str() );
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

void CApplicationSettingsDlg::HandleMainLayerTypeChanged( bool )
{
    mLayerURLLineEdit->setEnabled( mUseRasterLayerURI->isChecked() || mViewsUseRasterLayerURI->isChecked() );
	mVectorSimplifyMethodCheck->setEnabled( mUseVectorLayer->isChecked() || mViewsUseVectorLayer->isChecked() );
}
void CApplicationSettingsDlg::HandleViewsLayerTypeChanged( bool )
{
    mLayerURLLineEdit->setEnabled( mUseRasterLayerURI->isChecked() || mViewsUseRasterLayerURI->isChecked() );
	mVectorSimplifyMethodCheck->setEnabled( mUseVectorLayer->isChecked() || mViewsUseVectorLayer->isChecked() );
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


void CApplicationSettingsDlg::HandleRsyncStatusChanged( CBratApplication::ERadsNotification notification )
{
	QString text = mRsyncStatusLabel->text();			//preserve existing text in case notification is not related
	QString exec_now_text = mRadsExecuteNow->text();	//
	mRadsExecuteNow->blockSignals( true );

	switch ( notification )
	{
		case CBratApplication::eNotificationRsyncRunnig:
		{
			text = "The RADS service is synchronizing data";
			exec_now_text = "Stop synchronization...";
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
			exec_now_text = "Synchronize now...";
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

	mRadsExecuteNow->blockSignals( false );
	mRadsExecuteNow->setText( exec_now_text );

	mRsyncStatusLabel->setText( text );
}


void CApplicationSettingsDlg::EnableRadsButtons()
{
	const bool installed = mRadsController.isInstalled();		assert__( !installed || mRadsInstallButton->isChecked() );		assert__( installed || !mRadsInstallButton->isChecked() );
	const bool running = installed && mRadsController.isRunning();

	mRadsInstallButton->setEnabled( !running );
	//mRadsInstallButton->setVisible( !installed );
	mRadsInstallButton->setText( installed ? "Uninstall service" : "Install service"  );
	mRadsSettingsBox->setEnabled( installed );

	mRadsStartButton->setText( running ? "Disable service" : "Enable service" );
	mRadsStartButton->setEnabled( installed );
	mRadsStartButton_Test->setEnabled( installed );

	mRadsPauseButton->setEnabled( running );
	mRadsExecuteNow->setEnabled( running && !mApp.RsyncIsActive() );
}


void CApplicationSettingsDlg::DisplayRadsError( const std::string & action )
{
	SimpleErrorBox( "Could not " + action + " RADS service" );
}


void CApplicationSettingsDlg::HandleRadsInstall( bool toggled )
{
	const bool installed = mRadsController.isInstalled();
	std::string error_token;
	bool user_canceled = false;

	if ( toggled )
	{
		if ( !installed )
		{
			// do not call ValidateAndSaveRadsValues here: the settings must still be configured by the user
			if ( !user_canceled && !mApp.InstallRadsService( this ) )
				error_token = "install";
		}
	}
	else
	{
		if ( installed )
		{
			user_canceled = !SimpleQuestion( "Are you sure you want to uninstall the RADS service?" );
			if ( !user_canceled && !mApp.UninstallRadsService() )
				error_token = "uninstall";			//Windows 1072: "The specified service has been marked for deletion."
		}
	}

	if ( !error_token.empty() )
		DisplayRadsError( error_token );


	QTimer::singleShot( 2000, this, [this, error_token, user_canceled]() 
	{
		if ( !error_token.empty() || user_canceled )
		{
			mRadsInstallButton->blockSignals( true );
			mRadsInstallButton->setChecked( mRadsController.isInstalled() );
			mRadsInstallButton->blockSignals( false );
		}
		else
		{
			if ( mRadsController.isInstalled() && !mRadsController.isRunning() )
				SimpleMsgBox(
					"Before starting periodic data synchronizations with the RADS server, please select your options in '" 
					+ mRadsSettingsBox->title()
					+ "' and activate the service with the '" 
					+ mRadsStartButton->text() 
					+ "' button when ready." 
				);
		}

		EnableRadsButtons();
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
			user_canceled = validate_and_save && !ValidateAndSaveRadsValues( true );
			if ( !user_canceled )
			{
				WaitCursor wait;

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
				WaitCursor wait;

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
		EnableRadsButtons();

	return error_token.empty() && !user_canceled;
}

void CApplicationSettingsDlg::HandleRadsStart( bool toggled )
{
	RadsStart( toggled, true, true );
}

// The service stars with rads client paused. In production mode
//	it should be immediately active.
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
		if ( !mApp.PauseRadsService() )
			error_token = "pause";
	}
	else
	{
		user_canceled = validate_and_save && !ValidateAndSaveRadsValues( true );

		if ( !user_canceled && !mApp.ResumeRadsService() )
			error_token = "resume";
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

	if ( toggled )
	{
		if ( !ValidateAndSaveRadsValues( true ) )
			return;

		if ( !mApp.SendRadsServiceCommand( eRadsService_ExecNow ) )
			error_token = "send synchronization command to";
	}
	else
	{
		user_canceled = !SimpleQuestion( "Are you sure you want to stop the current synchronization with RADS server?" );
		if ( !user_canceled && !mApp.SendRadsServiceCommand( eRadsService_StopExec ) )
			error_token = "send stop synchronization command to";
	}

	if ( !error_token.empty() )
		DisplayRadsError( error_token );

	if ( user_canceled )	//
	{
		mRadsExecuteNow->blockSignals( true );
		mRadsExecuteNow->setChecked( !toggled );
		mRadsExecuteNow->blockSignals( false );
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
};


// main function body


// Returns false only when the user cancels
//
bool CApplicationSettingsDlg::ValidateAndSaveRadsValues( bool ask_user )
{
	if ( ask_user && !SimpleQuestion( "This will save your current RADS settings.\nDo you want to proceed?" ) )
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
				const std::string &abbr = q2a( item->data( Qt::UserRole ).toString() );		assert__( FindRadsMissionAbbr( q2a( item->text() ), mRadsServiceSettings.AllAvailableMissions() ) == abbr );
				if ( !missions_str.empty() )
					missions_str += missions_separator;
				missions_str += abbr;
			}
		}

		int ndays = mRadsSpin->value();
		const bool running = mRadsController.isRunning();

		std::string rads_dir = CRadsSettingsBase::FormatRadsLocalOutputFolder( mSettings.BratPaths().UserDataDirectory() );
		if ( !IsDir( rads_dir ) && !AskCreateDir( "RADS data download directory", rads_dir ) )
		{
			return false;
		}

		bool cmd_success = !running || mApp.SendRadsServiceCommand( eRadsService_LockConfig );
		if ( cmd_success )
		{
			// This validity (valid_values truth) must be ensured by the programmer, not the user
			//
			bool valid_values = mRadsServiceSettings.SetApplicationParameterValues( missions_str, ndays, mSettings.BratPaths().UserDataDirectory() );		assert__( valid_values );
			cmd_success = 
				valid_values
				&&
				( !running || mApp.SendRadsServiceCommand( eRadsService_UnlockConfig ) );
		}
		if ( !cmd_success )
		{
			std::string msg = "Could not write or synchronize configuration settings with RADS service.\n";
			msg += "Some parameter values may not be updated.";
			SimpleErrorBox( msg );
			return false;
		}

		assert__( mRadsServiceSettings.OutputDirectory() == CRadsSettings::FormatRadsLocalOutputFolder( mSettings.BratPaths().UserDataDirectory() ) );
	}

	return true;
}



// IMPORTANT: call this only after validating user data path
//
bool CApplicationSettingsDlg::ValidateAndAssign()
{
	//	1. Application Paths

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
    mSettings.SetUserDataDirectory( mUsePortablePathsCheckBox->isChecked(), user_dir );
    mSettings.SetWorkspacesDirectory( workspace_dir );


	//	2. Startup Options

	mSettings.mLoadLastWorkspaceAtStartUp = mLoadLastProjectAtAtartupCheckBox->isChecked();

    if ( mUseRasterLayerURI->isChecked() || mViewsUseRasterLayerURI->isChecked() )
    {
		std::string url = q2a( mLayerURLLineEdit->text() );
		if ( url.empty() )
		{
			mLayerURLLineEdit->setFocus();
			SimpleErrorBox( "Layer URL cannot be empty" );
			return false;
		}
        mSettings.SetURLRasterLayerPath( url );
    }

    if ( mUseVectorLayer->isChecked() )
        mSettings.mMainLayerBaseType = CMapWidget::ELayerBaseType::eVectorLayer;
    else
    if ( mUseRasterLayer->isChecked() )
        mSettings.mMainLayerBaseType = CMapWidget::ELayerBaseType::eRasterLayer;
    else
    if ( mUseRasterLayerURI->isChecked() )
        mSettings.mMainLayerBaseType = CMapWidget::ELayerBaseType::eRasterURL;
    else
        assert__( false );

    if ( mViewsUseVectorLayer->isChecked() )
        mSettings.mViewsLayerBaseType = CMapWidget::ELayerBaseType::eVectorLayer;
    else
    if ( mViewsUseRasterLayer->isChecked() )
        mSettings.mViewsLayerBaseType = CMapWidget::ELayerBaseType::eRasterLayer;
	else
    if ( mViewsUseRasterLayerURI->isChecked() )
        mSettings.mViewsLayerBaseType = CMapWidget::ELayerBaseType::eRasterURL;
    else
        assert__( false );

	mSettings.mVectorSimplifyMethod = mVectorSimplifyMethodCheck->isChecked();


#if defined(DEBUG) || defined(_DEBUG)
	assert__( mDesktopManagerSdiCheckbox );
	mSettings.mDesktopManagerSdi = mDesktopManagerSdiCheckbox->isChecked();
#endif


	//	3. RADS

	
	ValidateAndSaveRadsValues( false );	//Can we disregard possible failure on RADS parameters save an let the dialog close?


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
