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

#include "new-gui/Common/System/Service/qtservice.h"
#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/System/Service/qtservice.h"
#include "GUI/ActionsTable.h"
#include "new-gui/Common/GUI/TextWidget.h"
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

	mRadsOutputEdit = new QLineEdit;
	SetReadOnlyEditor( mRadsOutputEdit, true );
	mRadsViewLogFile = new QPushButton( "Log..." );
	mRadsSpin = new QSpinBox;
	mRadsSpin->setMinimum( 1 );
	mRadsSpin->setMaximum( 31 );
	mRadsMissionsList = new QListWidget;

	mRadsInstallButton = CActionInfo::CreateToolButton( eAction_InstallRadsService, true ); 	assert__( mRadsInstallButton->isCheckable() );
	mRadsStartButton = CActionInfo::CreateToolButton( eAction_StartRadsService, true ); 		assert__( mRadsStartButton->isCheckable() );
	mRadsPauseButton = CActionInfo::CreateToolButton( eAction_PauseRadsService, true ); 		assert__( mRadsPauseButton->isCheckable() );
	mRadsExecuteNow = CActionInfo::CreateToolButton( eAction_ExecuteRadsService, true ); 		

	auto *sync_settings_l = LayoutWidgets( Qt::Vertical,
	{
		LayoutWidgets( Qt::Horizontal, { new QLabel( "Destination Path" ), mRadsOutputEdit }, nullptr, 2,2,2,2,2 ),
		LayoutWidgets( Qt::Horizontal, { new QLabel( "Sync data every" ), mRadsSpin, new QLabel( "days for mission(s)" ), mRadsMissionsList }, nullptr, 2,2,2,2,2 ),
	}, 
	nullptr, 2,2,2,2,2 );

	mRadsCommandsBox = CreateGroupBox( ELayoutType::Vertical, 
	{
		CreateButtonRow( false, Qt::Horizontal, { mRadsInstallButton, mRadsStartButton, mRadsPauseButton, mRadsExecuteNow, mRadsViewLogFile }, 2, 2 ),
	},
	"Service Control", nullptr, 2,2,2,2,2 );

	mRadsOptionsPage = CreateGroupBox( ELayoutType::Vertical, 
	{ 
		nullptr, sync_settings_l, mRadsCommandsBox, nullptr 
	}, 
	"RADS Settings", this, 6, 6, 6, 6, 6 );


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

	std::string rads_error_msg, user_missions_str;
	unsigned ndays = 1;

	bool result = mRadsServiceSettings.mValidRadsMissions;
	if ( !result )
		rads_error_msg = "An error occurred reading missions from rads configuration file.\nPlease check " + settings_paths->mRadsConfigurationFilePath;
	else
	{
		result = mRadsServiceSettings.LoadConfig();
		if ( !result )
			rads_error_msg = "Could not read RADS Service configuration file.";
	}
	if ( !result )
	{
		SimpleErrorBox( rads_error_msg + "\nRADS settings will not be available." );
		mRadsOptionsPage->setEnabled( false );
	}
	else
	{
		mRadsOutputEdit->setText( FormatRadsLocalOutputPath( settings_paths->UserDataDirectory() ).c_str() );
		mRadsSpin->setValue( ndays );

		auto const &user_missions = mRadsServiceSettings.MissionNames();
		for ( auto const &mission : mRadsServiceSettings.AllAvailableMissions() )
		{
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

		const bool installed = mRadsController->isInstalled();
		const bool running = installed && mRadsController->isRunning();
		mRadsInstallButton->setChecked( installed );
		mRadsStartButton->setChecked( running );
		mRadsPauseButton->setChecked( running );
		EnableRadsButtons();

		connect( mRadsInstallButton, SIGNAL( toggled( bool ) ), this, SLOT( HandleRadsInstall( bool ) ) );
		connect( mRadsStartButton, SIGNAL( toggled( bool ) ), this, SLOT( HandleRadsStart( bool ) ) );
		connect( mRadsPauseButton, SIGNAL( toggled( bool ) ), this, SLOT( HandleRadsPause( bool ) ) );
		connect( mRadsViewLogFile, SIGNAL( clicked() ), this, SLOT( HandleViewLogFile() ) );
		connect( mRadsExecuteNow, SIGNAL( clicked() ), this, SLOT( HandleRadsExecuteNow() ) );
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


CApplicationSettingsDlg::CApplicationSettingsDlg( CBratSettings &options, QWidget *parent )
	: QDialog( parent )
	, mSettings( options )
	, mRadsController( new QtServiceController( RADS_SERVICE_NAME ) )	//, mRadsServiceSettings( mSettings.BratPaths().mRadsServiceIniFilePath )
	, mRadsServiceSettings( mSettings.BratPaths() )
{
	CreateWidgets();
}

//virtual 
CApplicationSettingsDlg::~CApplicationSettingsDlg()
{
	delete mRadsController;
}




//////////////////////////
//	ApplicationPaths_page
//////////////////////////


void CApplicationSettingsDlg::HandleBrowseDataDirectory()
{
    QString dir = BrowseDirectory( this, "Select Default Data Directory", mDataDirectoryLineEdit->text() );
	if ( !dir.isEmpty() )
	{
		mDataDirectoryLineEdit->setText( dir );
		mRadsOutputEdit->setText( FormatRadsLocalOutputPath( q2a( dir ) ).c_str() );
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

void CApplicationSettingsDlg::EnableRadsButtons()
{
	const bool installed = mRadsController->isInstalled();		assert__( !installed || mRadsInstallButton->isChecked() );
                                                                //assert__( installed || !mRadsInstallButton->isChecked() );
	const bool running = installed && mRadsController->isRunning();
	mRadsStartButton->setEnabled( installed );
	mRadsPauseButton->setEnabled( running );
	mRadsExecuteNow->setEnabled( running );
}


void CApplicationSettingsDlg::DisplayRadsError( const std::string & action )
{
	SimpleErrorBox( "Could not " + action + " RADS service" );
}


void CApplicationSettingsDlg::HandleRadsInstall( bool toggled )
{
	const bool installed = mRadsController->isInstalled();
	std::string error_token;

	if ( toggled )
	{
		if ( !ValidateAndSaveRadsValues( true ) )
			return;
		if ( !installed && !CBratApplication::InstallRadsService() )
			error_token = "install";
	}
	else
	{
		if ( installed )
		{
			HandleRadsStart( false );
			if ( !mRadsController->uninstall() )
				error_token = "uninstall";			//Windows 1072: "The specified service has been marked for deletion."
		}
	}

	if ( !error_token.empty() )
	{
		DisplayRadsError( error_token );
		mRadsInstallButton->setChecked( mRadsController->isInstalled() );
	}

	EnableRadsButtons();
}


void CApplicationSettingsDlg::HandleRadsStart( bool toggled )
{
	assert__( mRadsController->isInstalled() );

	const bool running = mRadsController->isRunning();
	std::string error_token;

	if ( toggled )
	{
		if ( !ValidateAndSaveRadsValues( true ) )
			return;
		if ( !running && !mRadsController->start() )
			error_token = "start";					//Windows 1069: "The service did not start due to a logon failure."
	}
	else
	{
		if ( running && !mRadsController->stop() )
			error_token = "stop";
	}

    QBratThread::sleep( 2 );    //give some time for isRunning to return an accurate value
    
	if ( !error_token.empty() )
	{
		DisplayRadsError( error_token );
		mRadsStartButton->setChecked( mRadsController->isRunning() );
	}

	EnableRadsButtons();
}


void CApplicationSettingsDlg::HandleRadsPause( bool toggled )
{
	assert__( mRadsController->isInstalled() );

	std::string error_token;
	//TODO:  the controller has no flag for paused state

	if ( toggled )
	{
		if ( !mRadsController->pause() )
			error_token = "pause";
	}
	else
	{
		if ( !ValidateAndSaveRadsValues( true ) )
			return;
		if ( !mRadsController->resume() )
			error_token = "resume";
	}

	if ( !error_token.empty() )
	{
		DisplayRadsError( error_token );
		mRadsPauseButton->blockSignals( true );
		mRadsPauseButton->setChecked( !toggled );	//Guessing: without flag for paused state, we can only assume that, on failure, the current state did not change
		mRadsPauseButton->blockSignals( false );
	}
}


void CApplicationSettingsDlg::HandleRadsExecuteNow()
{
	if ( !ValidateAndSaveRadsValues( true ) )
		return;
	if ( !mRadsController->sendCommand( eRadsService_ExecNow ) )
		DisplayRadsError( "execute" );
}


void CApplicationSettingsDlg::HandleViewLogFile()
{
	QDesktopServices::openUrl( QUrl( mSettings.BratPaths().mRadsServiceLogFilePath.c_str() ) );
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
					missions_str += " ";
				missions_str += abbr;
			}
		}

		int ndays = mRadsSpin->value();
		const bool running = mRadsController->isRunning();

		bool cmd_success = !running || mRadsController->sendCommand( eRadsService_LockConfig );
		if ( cmd_success )
		{
			cmd_success = 
				mRadsServiceSettings.SetApplicationParameters( missions_str, ndays, q2a( mRadsOutputEdit->text() ) )
				&&
				( !running || mRadsController->sendCommand( eRadsService_UnlockConfig ) );
		}
		if ( !cmd_success )
		{
			std::string msg = "Could not synchronize configuration settings with RadsService.\n";
			msg += "Some parameter values may not be updated.";
			SimpleErrorBox( msg );
			return false;
		}
	}

	return true;
}



bool CApplicationSettingsDlg::ValidateAndAssign()
{
	// lambda

	auto ask_create_dir = []( const std::string &dir_name, const std::string &dir ) -> bool
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

	//	1. Application Paths

    std::string user_dir = q2a(mDataDirectoryLineEdit->text());
    std::string workspace_dir = q2a(mProjectsDirectoryLineEdit->text());

    if ( !IsDir( user_dir ) && !ask_create_dir( "User data working directory", user_dir ) )
    {
		return false;
    }

    if ( !IsDir(workspace_dir) && !ask_create_dir( "Workspaces directory.", workspace_dir ) )
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
