#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtils.h"
#include "new-gui/brat/GUI/ActionsTable.h"
#include "new-gui/Common/GUI/TextWidget.h"

#include "ExportImageDialog.h"


void CExportImageDialog::CreateWidgets( bool enable2D, bool enable3D )
{
    // Output type

	mExtensionCombo = new QComboBox;
    m2DCheck = new QCheckBox( "2D" );
    m3DCheck = new QCheckBox( "3D" );
    auto *output_type_group = CreateGroupBox( ELayoutType::Horizontal, { mExtensionCombo, nullptr, m2DCheck, m3DCheck }, "Image and File Type", nullptr, 6, 6, 6, 6, 6 );


    // Output file

    mOutputPathLineEdit2D = new QLineEdit();
    mOutputPathLineEdit2D->setReadOnly( true );
    mBrowseButton2D = new QPushButton( "Browse..." );
    mOutputPathLineEdit3D = new QLineEdit();
    mOutputPathLineEdit3D->setReadOnly( true );
    mBrowseButton3D = new QPushButton( "Browse..." );
	mBrowseButton2D->setToolTip( CActionInfo::FormatTip( "Select output file name and location." ).c_str() );
	mBrowseButton3D->setToolTip( CActionInfo::FormatTip( "Select output file name and location." ).c_str() );

    mOutputFileGroup2D = CreateGroupBox( ELayoutType::Horizontal, { mOutputPathLineEdit2D, mBrowseButton2D }, "2D Output File", nullptr, 6, 6, 6, 6, 6 );
    mOutputFileGroup3D = CreateGroupBox( ELayoutType::Horizontal, { mOutputPathLineEdit3D, mBrowseButton3D }, "3D Output File", nullptr, 6, 6, 6, 6, 6 );


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

	mOutputFileGroup2D->setStyleSheet("QGroupBox { font-weight: bold; } ");
	mOutputFileGroup3D->setStyleSheet("QGroupBox { font-weight: bold; } ");
	output_type_group->setStyleSheet("QGroupBox { font-weight: bold; } ");


    //	All

    QBoxLayout *main_l =
            LayoutWidgets( Qt::Vertical,
                            {
								output_type_group,
                                mOutputFileGroup2D,
                                mOutputFileGroup3D,
								nullptr,
                                help_group,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    Wire( enable2D, enable3D );
}


void CExportImageDialog::Wire( bool enable2D, bool enable3D )
{
	for ( int i = 0; i < CDisplayData::EImageExportType_size; ++i )
		mExtensionCombo->addItem( CDisplayData::ImageType2String( (EImageExportType)i ).c_str() );

	// Connect

    connect( m2DCheck, SIGNAL( toggled( bool ) ), this, SLOT( Handle2DChecked( bool ) ) );
    connect( m3DCheck, SIGNAL( toggled( bool ) ), this, SLOT( Handle3DChecked( bool ) ) );
	connect( mBrowseButton2D, SIGNAL( clicked() ), this, SLOT( HandleChangeExportPath() ) );
	connect( mBrowseButton3D, SIGNAL( clicked() ), this, SLOT( HandleChangeExportPath() ) );
	connect( mExtensionCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleExportExtension( int ) ) );
	connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
	connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );

	m2DCheck->setChecked( enable2D );
	m3DCheck->setChecked( enable3D );
	Handle2DChecked( enable2D );
	Handle3DChecked( enable3D );
	m2DCheck->setEnabled( enable2D && enable3D );	//enable only if there is also the other alternative
	m3DCheck->setEnabled( enable3D && enable2D );	//idem

	mExtensionCombo->setCurrentIndex( mOutputFileType );
	//HandleExportExtension( 0 );

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );
    setMaximumHeight( height() );
}


CExportImageDialog::CExportImageDialog( bool enable2D, bool enable3D, std::string &ouput_path2d, std::string &ouput_path3d, QWidget *parent )
    : base_t( parent )
	, mOutputFileName2D( ouput_path2d )
	, mOutputFileName3D( ouput_path3d )
{
	assert__( enable2D || enable3D );

    CreateWidgets( enable2D, enable3D );

    setWindowTitle( "Export to Image");
}


CExportImageDialog::~CExportImageDialog()
{}


void CExportImageDialog::HandleChangeExportPath()
{
	std::string *output = nullptr;
	std::string title = "Export Output File";
	QLineEdit *edit = nullptr;
	if ( sender() == mBrowseButton2D )
	{
		title += " 2D";
		output = &mOutputFileName2D;
		edit = mOutputPathLineEdit2D;
	}
	else
	if ( sender() == mBrowseButton3D )
	{
		title += " 3D";
		output = &mOutputFileName3D;
		edit = mOutputPathLineEdit3D;
	}
	else
		assert__( false );

	QString export_path = BrowseNewFile( this, title.c_str(), QString( output->c_str() ) );
	if ( !export_path.isEmpty() )
	{
		*output = q2a( export_path );
		edit->setText( export_path );
	}
}


void CExportImageDialog::HandleExportExtension( int index )
{
	assert__( index >= 0 && index < CDisplayData::EImageExportType_size );

	mOutputFileType = (EImageExportType)index;

	std::string extension = CDisplayData::ImageType2String( mOutputFileType );

	SetFileExtension( mOutputFileName2D, extension );
	mOutputPathLineEdit2D->setText( mOutputFileName2D.c_str() );
	SetFileExtension( mOutputFileName3D, extension );
	mOutputPathLineEdit3D->setText( mOutputFileName3D.c_str() );
}


void CExportImageDialog::Handle2DChecked( bool checked )
{
	mOutputFileGroup2D->setEnabled( checked );
	mSaveButton->setEnabled( checked || m3DCheck->isChecked() );
}
void CExportImageDialog::Handle3DChecked( bool checked )
{
	mOutputFileGroup3D->setEnabled( checked );
	mSaveButton->setEnabled( checked || m2DCheck->isChecked() );
}


bool CExportImageDialog::Check()
{
    if ( Save2D() && IsFile( mOutputFileName2D ) )
    {
        if ( !SimpleQuestion( "Do you want to overwrite existing file " + mOutputFileName2D + " ?" ) )
            return false;
    }

    if ( Save3D() && IsFile( mOutputFileName3D ) )
    {
        if ( !SimpleQuestion( "Do you want to overwrite existing file " + mOutputFileName3D + " ?" ) )
            return false;
    }

    if ( ( Save2D() && mOutputFileName2D.empty() ) || ( Save3D() && mOutputFileName3D.empty() ) )
	{
		SimpleErrorBox( "The output path of the file(s) to save must be specified." );
		return false;
	}
	if ( mOutputFileName2D == mOutputFileName3D )
	{
		SimpleErrorBox( "Cannot save both images to the same file" );
		return false;
	}
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
