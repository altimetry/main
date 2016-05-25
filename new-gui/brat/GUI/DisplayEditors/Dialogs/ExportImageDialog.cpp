#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtils.h"
#include "new-gui/brat/GUI/ActionsTable.h"
#include "new-gui/brat/GUI/DisplayWidgets/TextWidget.h"

#include "ExportImageDialog.h"


void CExportImageDialog::CreateWidgets()
{
    // Output file

    mOutputPathLineEdit = new QLineEdit();
    mBrowseButton = new QPushButton( "Browse..." );
	mBrowseButton->setToolTip( CActionInfo::FormatTip( "Select output file name and location." ).c_str() );

    auto *output_file_group = CreateGroupBox( ELayoutType::Horizontal, { mOutputPathLineEdit, mBrowseButton }, "Output File", nullptr, 6, 6, 6, 6, 6 );

    // Output file

	mExtensionCombo = new QComboBox;
    auto *output_type_group = CreateGroupBox( ELayoutType::Horizontal, { mExtensionCombo }, "File type", nullptr, 6, 6, 6, 6, 6 );


    // Help

    auto help = new CTextWidget;
    help->SetHelpProperties( 
		"Select a file name and location. The extension will\n"
		"automatically be assigned according to the chosen image type."
         ,0 , 6, Qt::AlignCenter );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Maximum );


    //	Buttons (Execute, Delay Execution, Cancel)

    mButtonBox = new QDialogButtonBox();
    mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
    mButtonBox->setOrientation( Qt::Horizontal );
    mButtonBox->setStandardButtons( QDialogButtonBox::Save | QDialogButtonBox::Cancel );    
	mSaveButton = mButtonBox->button( QDialogButtonBox::Save );
    mSaveButton->setDefault( false );

	output_file_group->setStyleSheet("QGroupBox { font-weight: bold; } ");
	output_type_group->setStyleSheet("QGroupBox { font-weight: bold; } ");


    //	All

    QBoxLayout *main_l =
            LayoutWidgets( Qt::Vertical,
                            {
                                output_file_group,
								output_type_group,
								nullptr,
                                help_group,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "Export...");

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

    Wire();
}


void CExportImageDialog::Wire()
{
	for ( int i = 0; i < CDisplayData::EImageExportType_size; ++i )
		mExtensionCombo->addItem( ImageType2String( (EImageExportType)i ).c_str() );

	mExtensionCombo->setCurrentIndex( mOutputFileType );


	// Connect

	connect( mBrowseButton, SIGNAL( clicked() ), this, SLOT( HandleChangeExportPath() ) );
	connect( mExtensionCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleExportExtension( int ) ) );
	connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
	connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}


CExportImageDialog::CExportImageDialog( std::string &ouput_path, QWidget *parent )
    : base_t( parent )
	, mOutputFileName(ouput_path)
{
    CreateWidgets();
}


CExportImageDialog::~CExportImageDialog()
{}


void CExportImageDialog::HandleChangeExportPath()
{
	QString export_path = BrowseNewFile( this, "Export Output File", QString( mOutputFileName.c_str() ) );
	if ( !export_path.isEmpty() )
	{
		mOutputFileName = q2a( export_path );
		mOutputPathLineEdit->setText( export_path );
	}
}


void CExportImageDialog::HandleExportExtension( int index )
{
	assert__( index >= 0 && index < CDisplayData::EImageExportType_size );

	mOutputFileType = (EImageExportType)index;

	std::string extension = ImageType2String( mOutputFileType );

	SetFileExtension( mOutputFileName, extension );
	mOutputPathLineEdit->setText( mOutputFileName.c_str() );
}


bool CExportImageDialog::Check()
{
	return true;
}


//virtual
void  CExportImageDialog::accept()
{
	if ( Check() )
		base_t::accept();
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ExportImageDialog.cpp"
