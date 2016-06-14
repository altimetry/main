#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtils.h"
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
    mDataDirectoryLineEdit->setObjectName(QString::fromUtf8("DataDirectory_lineEdit"));
	mBrowseDataDirectoryPushButton = new QPushButton( "Browse..." );
	mUsePortablePathsCheckBox = new QCheckBox( "Use portable paths" );

	auto *data_l =
		LayoutWidgets( Qt::Vertical, {
			new QLabel( "Default Data Directory" ),
			LayoutWidgets( Qt::Horizontal, { mDataDirectoryLineEdit, mBrowseDataDirectoryPushButton }, nullptr, 2, 2, 2, 2, 2 ),
			mUsePortablePathsCheckBox
		}, nullptr, 2, 2, 2, 2, 2 );


    mProjectsDirectoryLineEdit = new QLineEdit( this );
    mProjectsDirectoryLineEdit->setObjectName(QString::fromUtf8("ExternalDataDirectory_lineEdit"));
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
    mUseURLRasterLayer = new QRadioButton( "Use a raster layer URI" );
    mLayerURLLineEdit = new QLineEdit;
    auto *main_layers_group = CreateGroupBox( ELayoutType::Vertical,
                                        {
                                            mUseVectorLayer,
                                            mUseRasterLayer,
                                            mUseURLRasterLayer,
                                            mLayerURLLineEdit
                                        },
                                        "Main Map Base Layer (requires restart)", this );

	mViewsUseVectorLayer = new QRadioButton( "Use vector layers" );
	mViewsUseRasterLayer = new QRadioButton( "Use raster layers" );
    auto *views_layers_group = CreateGroupBox( ELayoutType::Vertical,
                                        {
                                            mViewsUseVectorLayer,
                                            mViewsUseRasterLayer,
                                        },
                                        "View Maps Base Layer", this );


	auto *layers_l = LayoutWidgets( Qt::Horizontal, 
	{ 
		main_layers_group, views_layers_group 
	}
	, nullptr, 2, 2, 2, 2, 2 );



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
        { mApplicationStylesPage, "Style", CActionInfo::FormatTip("Styles\nApplication visual options"), "://images/brat_style.png", true }
	} );


	auto *row = CreateButtonRow( true, Qt::Vertical, { mStackedWidget->Button( 0 ), mStackedWidget->Button( 1 ), mStackedWidget->Button( 2 ) } );
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

    mDataDirectoryLineEdit->setText( settings_paths->mPortableBasePath.c_str() );				//for data portable paths
    mProjectsDirectoryLineEdit->setText( settings_paths->mWorkspacesDir.c_str() );

    mUsePortablePathsCheckBox->setChecked( settings_paths->UsePortablePaths() );

	connect( mBrowseDataDirectoryPushButton, SIGNAL( clicked() ), this, SLOT(  HandleBrowseDataDirectory() ) );
	connect( mBrowseWorkspacesDirectoryPushButton, SIGNAL( clicked() ), this, SLOT(  HandleBrowseProjectsPath() ) );


	//StartupOptions Page

	mLoadLastProjectAtAtartupCheckBox->setChecked( mSettings.mLoadLastWorkspaceAtStartUp );

    mUseVectorLayer->setChecked( mSettings.MainLayerBaseType() == CMapWidget::ELayerBaseType::eVectorLayer );
    mUseRasterLayer->setChecked( mSettings.MainLayerBaseType() == CMapWidget::ELayerBaseType::eRasterLayer );
    mUseURLRasterLayer->setChecked( mSettings.MainLayerBaseType() == CMapWidget::ELayerBaseType::eRasterURL );

    mViewsUseVectorLayer->setChecked( mSettings.ViewsLayerBaseType() == CMapWidget::ELayerBaseType::eVectorLayer );
    mViewsUseRasterLayer->setChecked( mSettings.ViewsLayerBaseType() == CMapWidget::ELayerBaseType::eRasterLayer );

    connect( mUseRasterLayer, SIGNAL( toggled( bool ) ), this,  SLOT( HandleMainLayerTypeChanged( bool ) ) );
    connect( mUseVectorLayer, SIGNAL( toggled( bool ) ), this,  SLOT( HandleMainLayerTypeChanged( bool ) ) );
    connect( mUseURLRasterLayer, SIGNAL( toggled( bool ) ), this,  SLOT( HandleMainLayerTypeChanged( bool ) ) );

    connect( mViewsUseRasterLayer, SIGNAL( toggled( bool ) ), this,  SLOT( HandleViewsLayerTypeChanged( bool ) ) );
    connect( mViewsUseVectorLayer, SIGNAL( toggled( bool ) ), this,  SLOT( HandleViewsLayerTypeChanged( bool ) ) );

    mLayerURLLineEdit->setText( settings_paths->mURLRasterLayerPath.c_str() );

    HandleMainLayerTypeChanged( false );		//argument not used
    HandleViewsLayerTypeChanged( false );		//idem


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
}


CApplicationSettingsDlg::CApplicationSettingsDlg( CBratSettings &options, QWidget *parent ) : QDialog( parent ), mSettings( options )
{
	CreateWidgets();
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
    mLayerURLLineEdit->setEnabled( mUseURLRasterLayer->isChecked() );
}
void CApplicationSettingsDlg::HandleViewsLayerTypeChanged( bool )
{
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

    if ( !IsDir( user_dir ) && !ask_create_dir( "User working directory", user_dir ) )
    {
		return false;
    }

    if ( !IsDir(workspace_dir) && !ask_create_dir( "Workspaces directory.", workspace_dir ) )
    {
		return false;
    }

    //Its safe now to apply these procedures
    mSettings.SetUserBasePath( mUsePortablePathsCheckBox->isChecked(), user_dir );
    mSettings.SetWorkspacesDirectory( workspace_dir );


	//	2. Startup Options

	mSettings.mLoadLastWorkspaceAtStartUp = mLoadLastProjectAtAtartupCheckBox->isChecked();
    if ( mUseVectorLayer->isChecked() )
        mSettings.mMainLayerBaseType = CMapWidget::ELayerBaseType::eVectorLayer;
    else
    if ( mUseRasterLayer->isChecked() )
        mSettings.mMainLayerBaseType = CMapWidget::ELayerBaseType::eRasterLayer;
    else
    if ( mUseURLRasterLayer->isChecked() )
    {
        mSettings.mMainLayerBaseType = CMapWidget::ELayerBaseType::eRasterURL;
		std::string url = q2a( mLayerURLLineEdit->text() );
		if ( url.empty() )
		{
			mLayerURLLineEdit->setFocus();
			SimpleErrorBox( "Layer URL cannot be empty" );
			return false;
		}
        mSettings.SetURLRasterLayerPath( url );
    }
    else
        assert__( false );

    if ( mViewsUseVectorLayer->isChecked() )
        mSettings.mViewsLayerBaseType = CMapWidget::ELayerBaseType::eVectorLayer;
    else
    if ( mViewsUseRasterLayer->isChecked() )
        mSettings.mViewsLayerBaseType = CMapWidget::ELayerBaseType::eRasterLayer;
    else
        assert__( false );


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
