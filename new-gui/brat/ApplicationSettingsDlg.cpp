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
	mRadsPathBrowseButton = new QPushButton( "Browse..." );
	mRadsViewLogFile = new QPushButton( "Log..." );
	mRadsSpin = new QSpinBox;
	mRadsSpin->setMinimum( 1 );
	mRadsSpin->setMaximum( 31 );
	mRadsMissionsList = new QListWidget;

	mRadsInstallButton = CreateToolButton( "", "", CActionInfo::FormatTip("Install") ); 
	mRadsUninstallButton = CreateToolButton( "", "", CActionInfo::FormatTip("Uninstall") ); 
	mRadsStartButton = CreateToolButton( "", ":/images/themes/default/media/media-seek-forward-16.png", CActionInfo::FormatTip("Start") ); 
	mRadsStopButton = CreateToolButton( "", ":/images/themes/default/media/media-stop-16.png", CActionInfo::FormatTip("Stop") ); 
	mRadsPauseButton = CreateToolButton( "", ":/images/themes/default/media/media-pause-16.png", CActionInfo::FormatTip("Pause") ); 
	mRadsResumeButton = CreateToolButton( "", ":/images/themes/default/media/media-play-16.png", CActionInfo::FormatTip("Resume") ); 

	auto *sync_settings_l = LayoutWidgets( Qt::Vertical,
	{
		LayoutWidgets( Qt::Horizontal, { new QLabel( "Destination Path" ), mRadsOutputEdit, mRadsPathBrowseButton }, nullptr, 2,2,2,2,2 ),
		LayoutWidgets( Qt::Horizontal, { new QLabel( "Sync data every" ), mRadsSpin, new QLabel( "days for mission(s)" ), mRadsMissionsList }, nullptr, 2,2,2,2,2 ),
	}, 
	nullptr, 2,2,2,2,2 );

	mRadsCommandsBox = CreateGroupBox( ELayoutType::Vertical, 
	{
		CreateButtonRow( true, Qt::Horizontal, { mRadsInstallButton, mRadsUninstallButton, mRadsViewLogFile }, 2, 2 ),
		CreateButtonRow( true, Qt::Horizontal, { mRadsStartButton, mRadsStopButton }, 2, 2 ),
		CreateButtonRow( true, Qt::Horizontal, { mRadsPauseButton, mRadsResumeButton }, 2, 2 )
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

	connect( mBrowseDataDirectoryPushButton, SIGNAL( clicked() ), this, SLOT(  HandleBrowseDataDirectory() ) );
	connect( mBrowseWorkspacesDirectoryPushButton, SIGNAL( clicked() ), this, SLOT(  HandleBrowseProjectsPath() ) );


	//StartupOptions Page

	mLoadLastProjectAtAtartupCheckBox->setChecked( mSettings.mLoadLastWorkspaceAtStartUp );

    mUseVectorLayer->setChecked( mSettings.MainLayerBaseType() == CMapWidget::ELayerBaseType::eVectorLayer );
    mUseRasterLayer->setChecked( mSettings.MainLayerBaseType() == CMapWidget::ELayerBaseType::eRasterLayer );
    mUseRasterLayerURI->setChecked( mSettings.MainLayerBaseType() == CMapWidget::ELayerBaseType::eRasterURL );

    mViewsUseVectorLayer->setChecked( mSettings.ViewsLayerBaseType() == CMapWidget::ELayerBaseType::eVectorLayer );
    mViewsUseRasterLayer->setChecked( mSettings.ViewsLayerBaseType() == CMapWidget::ELayerBaseType::eRasterLayer );
    mViewsUseRasterLayerURI->setChecked( mSettings.ViewsLayerBaseType() == CMapWidget::ELayerBaseType::eRasterURL );

	mVectorSimplifyMethodCheck->setChecked( mSettings.mVectorSimplifyMethod );

    connect( mUseRasterLayer, SIGNAL( toggled( bool ) ), this,  SLOT( HandleMainLayerTypeChanged( bool ) ) );
    connect( mUseVectorLayer, SIGNAL( toggled( bool ) ), this,  SLOT( HandleMainLayerTypeChanged( bool ) ) );
    connect( mUseRasterLayerURI, SIGNAL( toggled( bool ) ), this,  SLOT( HandleMainLayerTypeChanged( bool ) ) );

    connect( mViewsUseRasterLayer, SIGNAL( toggled( bool ) ), this,  SLOT( HandleViewsLayerTypeChanged( bool ) ) );
    connect( mViewsUseVectorLayer, SIGNAL( toggled( bool ) ), this,  SLOT( HandleViewsLayerTypeChanged( bool ) ) );
    connect( mViewsUseRasterLayerURI, SIGNAL( toggled( bool ) ), this,  SLOT( HandleViewsLayerTypeChanged( bool ) ) );

    mLayerURLLineEdit->setText( settings_paths->URLRasterLayerPath().c_str() );

    HandleMainLayerTypeChanged( false );		//argument not used
    HandleViewsLayerTypeChanged( false );		//idem


	//RADS Page

	mRadsOutputEdit->setText( settings_paths->UserDataDirectory().c_str() );				//TODO mSettings.RadsPeriod() 
	connect( mRadsPathBrowseButton, SIGNAL( clicked() ), this, SLOT(  HandleRadsPathBrowse() ) );

	mRadsSpin->setValue( 7 );																//TODO mSettings.RadsPeriod() 

	std::vector< std::string > missions = { "Sentinel", "Jason 1", "Jason 2", "Cryosat" };	//TODO: mSettings.RadsMissionsList
	for ( auto &mission : missions )
	{
		QListWidgetItem* item = new QListWidgetItem;
		item->setText( t2q( mission )  );
		item->setFlags( item->flags() | Qt::ItemIsUserCheckable );
		item->setCheckState( Qt::Unchecked );		//Qt::Checked
		mRadsMissionsList->addItem( item );
	}
	// Sort items (ascending order)
	mRadsMissionsList->sortItems();
	mRadsMissionsList->setMaximumHeight( mRadsMissionsList->sizeHintForRow( 0 ) * 5 );

	bool installed = mRadsController->isInstalled();
	bool running = mRadsController->isRunning();
	mRadsStopButton->setChecked( installed && running );		mRadsStartButton->setChecked( installed && !running );
	mRadsPauseButton->setChecked( installed && running );		mRadsResumeButton->setChecked( installed && running );
	EnableRadsButtons();

	connect( mRadsInstallButton, SIGNAL( clicked() ), this, SLOT(  HandleRadsInstall() ) );
	connect( mRadsUninstallButton, SIGNAL( clicked() ), this, SLOT(  HandleRadsUninstall() ) );
	connect( mRadsStartButton, SIGNAL( clicked() ), this, SLOT(  HandleRadsStart() ) );
	connect( mRadsStopButton, SIGNAL( clicked() ), this, SLOT(  HandleRadsStop() ) );
	connect( mRadsPauseButton, SIGNAL( clicked() ), this, SLOT(  HandleRadsPause() ) );
	connect( mRadsResumeButton, SIGNAL( clicked() ), this, SLOT(  HandleRadsResume() ) );
	connect( mRadsViewLogFile, SIGNAL( clicked() ), this, SLOT(  HandleViewLogFile() ) );


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
	, mRadsController( new QtServiceController( RADS_SERVICE_NAME ) )
	, mSettings( options )
{
	CreateWidgets();
}

//virtual 
CApplicationSettingsDlg::~CApplicationSettingsDlg()
{
	delete mRadsController;
}





//	ApplicationPaths_page

void CApplicationSettingsDlg::HandleBrowseDataDirectory()
{
    QString dir = BrowseDirectory( this, "Select Default Data Directory", mDataDirectoryLineEdit->text() );
    if ( !dir.isEmpty() )
        mDataDirectoryLineEdit->setText( dir );
}

void CApplicationSettingsDlg::HandleBrowseProjectsPath()
{
    QString dir = BrowseDirectory( this, "Select Default Workspaces Directory", mProjectsDirectoryLineEdit->text() );
    if ( !dir.isEmpty() )
        mProjectsDirectoryLineEdit->setText( dir );
}


//	StartupOptions_page

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



//	RADS page

void CApplicationSettingsDlg::HandleRadsPathBrowse()
{
	QString dir = BrowseDirectory( this, "Select RADS Output Directory", mRadsOutputEdit->text() );
	if ( !dir.isEmpty() )
		mRadsOutputEdit->setText( dir );
}

void CApplicationSettingsDlg::EnableRadsButtons()
{
	const bool installed = mRadsController->isInstalled();

	mRadsInstallButton->setChecked( installed );				
	mRadsUninstallButton->setChecked( !installed );

	mRadsStopButton->setEnabled( installed );		
	mRadsStartButton->setEnabled( installed );
	mRadsPauseButton->setEnabled( installed );		
	mRadsResumeButton->setEnabled( installed );
}

void CApplicationSettingsDlg::DisplayRadsError( const std::string & action )
{
	SimpleErrorBox( "Could not " + action + " RADS service" );
}

void CApplicationSettingsDlg::HandleRadsInstall()
{
	assert__( !mRadsController->isInstalled() );

	if ( !CBratApplication::InstallRadsService() )
		DisplayRadsError( "install" );

	EnableRadsButtons();
}
void CApplicationSettingsDlg::HandleRadsUninstall()
{
	assert__( mRadsController->isInstalled() );

	if ( !mRadsController->uninstall() )
		DisplayRadsError( "uninstall" );

	EnableRadsButtons();
}
void CApplicationSettingsDlg::HandleRadsStart()
{
	assert__( !mRadsController->isRunning() );

	if ( !mRadsController->start() )
		DisplayRadsError( "start" );
}
void CApplicationSettingsDlg::HandleRadsStop()
{
	assert__( mRadsController->isRunning() );

	if ( !mRadsController->stop() )
		DisplayRadsError( "stop" );
}
void CApplicationSettingsDlg::HandleRadsPause()
{
	assert__( mRadsController->isRunning() );

	if ( !mRadsController->pause() )
		DisplayRadsError( "pause" );
}
void CApplicationSettingsDlg::HandleRadsResume()
{
	assert__( !mRadsController->isRunning() );

	if ( !mRadsController->resume() )
		DisplayRadsError( "resume" );
}
void CApplicationSettingsDlg::HandleViewLogFile()
{
	BRAT_NOT_IMPLEMENTED;
}



//	ApplicationStyles_page




///////////////////////////////////////////////////////////////////////////////////
//
//						Validation and Conclusion
//
///////////////////////////////////////////////////////////////////////////////////



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


	//	3. Application Styles

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
