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

#include "common/QtUtilsIO.h"

#include "DataModels/Workspaces/Workspace.h"
#include "new-gui/Common/GUI/TextWidget.h"
#include "DataModels/Model.h"

#include "WorkspaceDialog.h"


static const std::string* MakeErrorMessages()
{
	static std::string error_messages[ CTreeWorkspace::EValidationErrors_size ];

	error_messages[ CTreeWorkspace::eNoError ]						= "";
	error_messages[ CTreeWorkspace::eError_EmptyName ]				= "Workspace has no name.\nPlease set a name for the workspace.";
	error_messages[ CTreeWorkspace::eError_EmptyPathName ]			= "Invalid pathname %s.\nPlease set a valid pathname\n(if the pathname doesn't exist, it will be created).";
	error_messages[ CTreeWorkspace::eError_NotExistingPath ]		= "Pathname %s doesn't exist.\nPlease set a correct pathname.";
	error_messages[ CTreeWorkspace::eError_InvalidConfig ]			= "Path %s doesn't contain any workspace informations or is not the root folder.\nPlease set a correct pathname.";
	error_messages[ CTreeWorkspace::eError_WorkspaceAlreadyExists ]	= "Path %s seems to already contain a workspace with the same name.\nPlease set a correct pathname.";
	error_messages[ CTreeWorkspace::eError_SelfImporting ]			= "You can't import a workspace from itself.";

	return error_messages;
}


static inline std::string ErrorMessage( CTreeWorkspace::EValidationError e, const std::string &arg = "" )
{
	static const std::string *error_messages = MakeErrorMessages();
	
	const std::string to_replace = arg.empty() ? " %s" : "%s";

	return replace( error_messages[ e ], to_replace, arg );
}



QString CWorkspaceDialog::CreateHelpText()
{
	// TODO this is only indicative text to show how help content can be formed; the widget will adjust to content

	QString help_text =
        "A workspace is stored as a structured set of files in a directory tree."
		"";

	switch ( mTask )
	{
		case CTreeWorkspace::eNew:									// append text
			help_text +=
                "\nData is first saved in a sub-directory named "
				"after the workspace, the workspace root.\n"
				"Enter in Location the directory where you "
				"want the new sub-directory to be created.";
			break;
		case CTreeWorkspace::eOpen:
			help_text =	
				"Enter in Location the directory containing "
				"the root of an existing workspace.";				// override text
			break;
		case CTreeWorkspace::eImport:
			help_text +=
				"\nUse the Import Options to select the foreign "
				"objects to be merged in the workspace.";			// append text
			break;
		case CTreeWorkspace::eRename:
			help_text +=
				"\nThe workspace will be renamed independently of "
				"its directory.";									// append text
			break;
		case CTreeWorkspace::eDelete:
			help_text =	
				"The workspace will be deleted as well as "
				"all data in its directory on disk.";				// override text
			break;
		default:
			assert__( false );
			break;
	}
	return help_text;
}



void CWorkspaceDialog::CreateGUI()
{
	//	Create


	//	... Name

    auto name_wrapper = new QWidget;									
	mNameText = new QLineEdit( this );
	mNameText->setToolTip( "Enter the name of the workspace." );
    auto name_line = WidgetLine( nullptr, Qt::Horizontal );				//hl_name->addItem( CreateSpace() );
	auto hl_name = LayoutWidgets( Qt::Horizontal, { mNameText, name_line }, nullptr, 6, 0, 0, 0, 0 );
	LayoutWidgets( Qt::Vertical, { new QLabel( "Name" ), hl_name }, name_wrapper, 0, 0, 0, 0, 0 );
    name_wrapper->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );


	//	... Path

    auto path_wrapper = new QWidget;									
	mPathText = new QLineEdit( this );
	mPathText->setToolTip( "Enter path to the workspace" );	
    mBrowseDirButton = new QPushButton( "Browse..." );
	mBrowseDirButton->setToolTip( "Select the workspace location in the file system" );
	auto hl_path = LayoutWidgets( Qt::Horizontal, { mPathText, mBrowseDirButton }, nullptr, 6, 0, 0, 0, 0 );
	LayoutWidgets( Qt::Vertical, { new QLabel( "Location" ), hl_path }, path_wrapper, 0, 0, 0, 0, 0 );
    path_wrapper->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );


	//	... Import Options

    mFormulasList = new QListWidget;						// formulas_list->setMinimumWidth( formulas_list->sizeHintForColumn( 0 ) );
	mFormulasList->setSelectionMode( QAbstractItemView::MultiSelection );
    mFormulasList->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
    auto formulas_list_l = LayoutWidgets( Qt::Vertical, { mFormulasList }, nullptr, 0,0,0,0,0 );
    auto selected_all_button = new QPushButton( "Select All" );
	auto deselected_all_button = new QPushButton( "De-select All" );
	auto formulas_l = LayoutWidgets( Qt::Vertical, { selected_all_button, deselected_all_button }, nullptr, 6, 6, 6, 6, 6 );
    mFormulasGroup = ::CreateGroupBox( ELayoutType::Horizontal,
	{ 
        formulas_list_l, formulas_l
	},
	"Formulas", nullptr, 6, 6, 6, 6, 6 
	);
    mFormulasGroup->setCheckable( true );
    mFormulasGroup->setChecked( false );

	mDatasetsCheck = new QCheckBox( "Datasets", this );
	mOperationsCheck = new QCheckBox( "Operations", this );
	mViewsCheck = new QCheckBox( "Views", this );
    auto import_group = CreateGroupBox< QgsCollapsibleGroupBox >( ELayoutType::Vertical,
	{ 
		mDatasetsCheck, mFormulasGroup, mOperationsCheck, mViewsCheck
	}, 
	"Import Options", nullptr, 6, 6, 6, 6, 6 
	);
    import_group->setSyncGroup( "SyncGroup" );
    import_group->setCollapsed( true );
    import_group->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
	connect( import_group, SIGNAL( collapsedStateChanged( bool ) ), this, SLOT( collapsedStateChanged( bool ) ) );


	//	... Help

	auto help = new CTextWidget;
	help->SetHelpProperties( CreateHelpText(), 0, 6, Qt::AlignCenter );
    auto help_group = CreateGroupBox( ELayoutType::Horizontal, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


	//	... Buttons

	mButtonBox = new QDialogButtonBox( this );
	mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
	mButtonBox->setOrientation( Qt::Horizontal );
	mButtonBox->setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );	//| QDialogButtonBox::Reset 
    mButtonBox->button( QDialogButtonBox::Ok )->setDefault( true );


	//	Layout everything

    QBoxLayout *l =
            LayoutWidgets( Qt::Vertical,
                           {
                               name_wrapper,
                               path_wrapper,
                               import_group,

                           }, nullptr, 6, 6, 6, 6, 6 );


    QBoxLayout *main_l =
            LayoutWidgets( Qt::Vertical,
                            {
                                l,
                                help_group,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );


	//	Set state according to task

	QString ok_text;
	switch ( mTask )
	{
		case CTreeWorkspace::eNew:
			mNameText->setFocus();
			import_group->setVisible( false );
			ok_text = "Create";
			break;
		case CTreeWorkspace::eOpen:
			mBrowseDirButton->setFocus();
			name_wrapper->setEnabled( false );
		    import_group->setVisible( false );
			ok_text = "Open";
			break;
		case CTreeWorkspace::eImport:
			mBrowseDirButton->setFocus();
			name_wrapper->setEnabled( false );
			mPathText->setEnabled( false );			//force usage of browse button so we can detect new import source; and minimize path input errors
			ok_text = "Import";
			break;
		case CTreeWorkspace::eRename:
			mNameText->setFocus();
			path_wrapper->setEnabled( false );		//TODO this can change if rename is construed as a save as
			import_group->setVisible( false );
			ok_text = "Rename";
			break;
		case CTreeWorkspace::eDelete:
			name_wrapper->setEnabled( false );
			path_wrapper->setEnabled( false );
			import_group->setVisible( false );
			ok_text = "Delete";
			break;
		default:
			assert__( false );
			break;
	}
    mButtonBox->button( QDialogButtonBox::Ok )->setText( ok_text );


    setWindowTitle( "Workspace Dialog - " + ok_text );


    //	Wire things

	connect( mBrowseDirButton, SIGNAL( clicked() ), this, SLOT( on_button_browse_clicked() ) );
	connect( selected_all_button, SIGNAL( clicked() ), this, SLOT( on_button_select_all_clicked() ) );
	connect( deselected_all_button, SIGNAL( clicked() ), this, SLOT( on_button_deselect_all_clicked() ) );

	connect( mDatasetsCheck, SIGNAL( toggled( bool ) ), this, SLOT( on_button_datasets_toggled( bool ) ) );
	connect( mFormulasGroup, SIGNAL( toggled( bool ) ), this, SLOT( on_button_formulas_toggled( bool ) ) );
	connect( mOperationsCheck, SIGNAL( toggled( bool ) ), this, SLOT( on_button_operations_toggled( bool ) ) );
	connect( mViewsCheck, SIGNAL( toggled( bool ) ), this, SLOT( on_button_views_toggled( bool ) ) );
	 
	connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
	connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );


    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );
    if ( mTask != CTreeWorkspace::eImport )
        setMaximumHeight( height() );
    adjustSize();


	assert__( layout() == main_l );				Q_UNUSED( main_l );	//for release builds
}


CWorkspaceDialog::CWorkspaceDialog( QWidget *parent, CTreeWorkspace::EValidationTask task, const CWorkspace *wks, const QString &initial_dir, const CModel &model ) 
	: QDialog(parent), mTask(task), mWks( wks ), mInitialDir(initial_dir), mModel( model )
{
	CreateGUI();

	if ( mWks && mTask != CTreeWorkspace::eImport )
	{
		mNameText->setText( t2q( mWks->GetName() ) );
		mPathText->setText( t2q( mWks->GetPath() ) );
	}
	else
		mPathText->setText( mInitialDir );
}


CWorkspaceDialog::~CWorkspaceDialog()
{}


void CWorkspaceDialog::reset()
{
    //not used: reset not inserted
}


void CWorkspaceDialog::FillImportFormulas()
{
	assert__( mTask == CTreeWorkspace::eImport );

	mFormulasList->clear();

	std::string path, name;
	if ( !Validate( name, path ) )
		return;

	std::string error_msg;
	std::vector< std::string > v;
	if ( !mModel.LoadImportFormulas( path, v, false, true, error_msg ) )
	{
		SimpleErrorBox( error_msg );
	}
	else
	{
		FillList( mFormulasList, v, -1, true );
		if ( mFormulasGroup->isChecked() )
			on_button_select_all_clicked();
		else
			on_button_deselect_all_clicked();
	}
}



//	Handlers

void CWorkspaceDialog::collapsedStateChanged( bool )
{
	adjustSize();
	resize( sizeHint() );
}


void CWorkspaceDialog::on_button_box_clicked( QAbstractButton *button )
{
    if ( button == mButtonBox->button( QDialogButtonBox::Reset ) )
        reset();
}


void CWorkspaceDialog::on_button_browse_clicked()
{
	QString qpath = BrowseDirectory( this, "Select Workspace Directory", mInitialDir );
	if ( qpath.isEmpty() )
		return;

	mInitialDir = qpath;
	mPathText->setText( qpath );

	switch ( mTask )
	{
		case CTreeWorkspace::eOpen:
		case CTreeWorkspace::eImport:
		{
			EValidationError e;
			std::string path = q2a( qpath );
			if ( CTreeWorkspace::Validate( mTask, path, &e, "", mWks ) )
			{
				CWorkspace wks( path );
				std::string error_msg;
				if ( wks.LoadConfig( error_msg, nullptr, nullptr, nullptr ) )	//wxGetApp().GetCurrentWorkspaceDataset(), 	//wxGetApp().GetCurrentWorkspaceDisplay(), 	//wxGetApp().GetCurrentWorkspaceOperation() 
				{
					mNameText->setText( t2q( wks.GetName() ) );
					if ( mTask == CTreeWorkspace::eImport )
						FillImportFormulas();
				}
				else
					SimpleErrorBox( error_msg );
			}
			else
			{
				SimpleErrorBox( ErrorMessage( e, path ) );
			}
			
			break;
		}

		case CTreeWorkspace::eNew:
		case CTreeWorkspace::eRename:
		case CTreeWorkspace::eDelete:
			break;

		default:
			assert__( false );
	}
}



void CWorkspaceDialog::on_button_datasets_toggled( bool toggled )
{
	if ( !toggled )
	{
		mFormulasGroup->setChecked( false );
		mOperationsCheck->setChecked( false );
		mViewsCheck->setChecked( false );
	}
}
void CWorkspaceDialog::on_button_formulas_toggled( bool toggled )
{
	if ( toggled )
		mFormulasList->selectAll();
	else
		mFormulasList->clearSelection();

	if ( !toggled )
	{
		mOperationsCheck->setChecked( false );
		mViewsCheck->setChecked( false );
	}
}
void CWorkspaceDialog::on_button_operations_toggled( bool toggled )
{
	if ( toggled )
	{
		mFormulasGroup->setChecked( true );
		mDatasetsCheck->setChecked( true );
	}
	else
	{
		mViewsCheck->setChecked( false );
	}
}
void CWorkspaceDialog::on_button_views_toggled( bool toggled )
{
	if ( toggled )
	{
		mOperationsCheck->setChecked( true );
		mFormulasGroup->setChecked( true );
		mDatasetsCheck->setChecked( true );
	}
}



void CWorkspaceDialog::on_button_select_all_clicked()
{
	mFormulasList->selectAll();
}


void CWorkspaceDialog::on_button_deselect_all_clicked()
{
	mFormulasList->clearSelection();
}



//	Validation

bool CWorkspaceDialog::Validate( std::string &name, std::string &path )
{
	EValidationError e;
	path = q2a( mPathText->text() );
	name = q2a( mNameText->text() );
	if ( !CTreeWorkspace::Validate( mTask, path, &e, name, mWks ) )
	{
		SimpleErrorBox( ErrorMessage( e, e == CTreeWorkspace::eError_EmptyName ? mName : mPath ) );
		return false;
	}
	return true;
}


//virtual
void CWorkspaceDialog::accept()
{
	std::string path, name;
	if ( !Validate( name, path ) )
	{
		mPath.clear();
		mName.clear();

		mFormulas = false;
		mDatasets = false;
		mOperations = false;
		mViews = false;

		mImportFormulas.clear();
	}
	else
	{
		mPath = NormalizePath( path );
		mName = name;

		mFormulas = mFormulasGroup->isChecked();
		mDatasets = mDatasetsCheck->isChecked();
		mOperations = mOperationsCheck->isChecked();
		mViews = mViewsCheck->isChecked();

		mImportFormulas.clear();
		const QList< QListWidgetItem* > items = mFormulasList->selectedItems();
		for ( auto *item : items )
			mImportFormulas.push_back( q2a( item->text() ) );

		QDialog::accept();
	}
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_WorkspaceDialog.cpp"
