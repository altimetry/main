#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtils.h"

#include "DataModels/PlotData/BratLookupTable.h"
#include "DataModels/Workspaces/Operation.h"

#include "GUI/ActionsTable.h"
#include "GUI/ControlPanels/ColorMapWidget.h"
#include "new-gui/Common/GUI/TextWidget.h"
#include "DelayExecutionDialog.h"

#include "ExportDialog.h"



//static 
const std::string CExportDialog::smDefaultExtensionAscii = "txt";
//static 
const std::string CExportDialog::smDefaultExtensionNetCdf = "nc";
//static 
const std::string CExportDialog::smDefaultExtensionGeoTiff = "tif";
//static 
const std::string CExportDialog::smDefaultExtensionKML = "kml";


void  CExportDialog::CreateWidgets()
{
    // Pages

	mFormatASCIIPage = new QFrame;
    mFormatASCIIPage->setFrameStyle( QFrame::Sunken );    //mFormatASCIIPage->setFrameShape( QFrame::Box );
	mFormatNetCdfPage = new QFrame;
    mFormatNetCdfPage->setFrameStyle( QFrame::Sunken );    //mFormatNetCdfPage->setFrameShape( QFrame::Box );
	mFormatGeoTiffPage = new QFrame;
    mFormatGeoTiffPage->setFrameStyle( QFrame::Sunken );    //mFormatGeoTiffPage->setFrameShape( QFrame::Box );


    // ASCII

	mDeliverDataAsPeriod = new QCheckBox( "Deliver date as period (duration)" );
	mDeliverDataAsPeriod->setToolTip( "Deliver date as a duration if check, otherwise deliver date as 'yyyy-mm-dd hh:mm:ss' format" );
	mExpandArrays = new QCheckBox( "Expand arrays so far as possible" );
	mExpandArrays->setToolTip( "Expand arrays so far as possible, i.e. if all export array fields have the same dimension" );
	mOnlyDumpExpressions = new QCheckBox( "Only dump expressions (no data computation)" );

	mASCIINumberPrecisionLineEdit = new QLineEdit;
	mASCIINumberPrecisionLineEdit->setValidator( new QRegExpValidator( QRegExp( "[0-9]+" ) ) );
	mASCIINumberPrecisionLineEdit->setMaxLength( 2 );
	QString sprecision = n2s<std::string>( smDefaultDecimalPrecision ).c_str();
	mASCIINumberPrecisionLineEdit->setToolTip(
		"Precision specifies the minimum number of digits to be written after the decimal point.\n"
		"This precision is applied on all fields, except for date string representation.\n"
		"When no precision is specified, the decimal precision is " + sprecision + ".\n"
		"The maximum valid precision depends on the operating system (usually 16).\n" );

	LayoutWidgets( 
	{ 
		mExpandArrays, mDeliverDataAsPeriod, nullptr,
		mOnlyDumpExpressions, LayoutWidgets( Qt::Horizontal, { new QLabel( "Ascii number precision" ), mASCIINumberPrecisionLineEdit, nullptr }, nullptr, 0,0,0,0,0)
	}, 
	mFormatASCIIPage, 6,6,6,6,6 );


    // NetCdf

	// ...


	// GeoTiff

	KMLAlongTrackDataCheck = new QCheckBox( "With Along Track Data" );
	KMLFieldsDataCheck = new QCheckBox( "With Field Image Data" );
	mCreateGoogleKMLFileGroup = CreateGroupBox( ELayoutType::Vertical, { KMLFieldsDataCheck, KMLAlongTrackDataCheck }, "Create GoogleEarth KML" );
	mCreateGoogleKMLFileGroup->setCheckable( true );
	mCreateGeoTiffFilesCheck = new QCheckBox( "Create GeoTIFF" );
	mColorMapWidget = new CColorMapWidget( true, false, nullptr );
	mCreateGoogleKMLFileGroup->setStyleSheet("QGroupBox { font-weight: normal; } ");		//necessary to break inheritance from enveloping group font

	auto *color_table_group = CreateGroupBox( ELayoutType::Horizontal, { mColorMapWidget }, "", nullptr, 4, 4, 4, 4, 4 );		//mColorMapWidget already has group titles
	color_table_group->setStyleSheet("QGroupBox { font-weight: normal; } ");		//necessary to break inheritance from enveloping group font

	LayoutWidgets( Qt::Horizontal,
	{
		LayoutWidgets( Qt::Vertical,
		{
			mCreateGeoTiffFilesCheck,
			nullptr,
			mCreateGoogleKMLFileGroup
		}, 
		nullptr, 2, 2, 2, 2, 2 ),
		color_table_group
	},
	mFormatGeoTiffPage, 6, 6, 6, 6, 6 );


	// Stack pages

	mStackedWidget = new CStackedWidget( this, { 
        { mFormatASCIIPage, "Ascii", CActionInfo::FormatTip("Ascii format options"), "://images/OSGeo/export_ascii.png", true },
        { mFormatNetCdfPage, "NetCdf", CActionInfo::FormatTip("NetCdf format options"), "://images/OSGeo/export_netcdf.png", true },
        { mFormatGeoTiffPage, "KML/GeoTIFF", CActionInfo::FormatTip("GeoTiff format options"), "://images/OSGeo/export_geotiff.png", true }
	} );

	auto *b0 = dynamic_cast<QToolButton*>( mStackedWidget->Button( eASCII ) );
	b0->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	auto *b1 = dynamic_cast<QToolButton*>( mStackedWidget->Button( eNETCDF ) );
	b1->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	mFormatGeoTiffButton = dynamic_cast<QToolButton*>( mStackedWidget->Button( eGEOTIFF ) );
	mFormatGeoTiffButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );

	auto *row = CreateButtonRow( true, Qt::Horizontal, { b0, b1, mFormatGeoTiffButton } );
	auto *options_group = CreateGroupBox( ELayoutType::Vertical, { row, nullptr, mStackedWidget }, "Format", nullptr, 6,6,6,6,6 );


    // Delay execution

	mDelayExecutionCheckBox = new QCheckBox( "Schedule" );
	mDelayExecutionButton = new QPushButton( "Properties..." );

    auto *delay_group = CreateGroupBox( ELayoutType::Horizontal, 
	{ 
		nullptr, mDelayExecutionCheckBox, nullptr, mDelayExecutionButton, nullptr
	}, 
	"Scheduled Execution", nullptr, 6, 6, 6, 6, 6 );


    // Output file

    mOutputPathLineEdit = new QLineEdit();
    SetReadOnlyEditor( mOutputPathLineEdit, true );
    mBrowseButton = new QPushButton( "Browse..." );
	mBrowseButton->setToolTip( CActionInfo::FormatTip( "Select a output file." ).c_str() );

    auto *output_group = CreateGroupBox( ELayoutType::Horizontal, { mOutputPathLineEdit, mBrowseButton }, "Output File", nullptr, 6, 6, 6, 6, 6 );


    // Help

    auto help = new CTextWidget;
    help->SetHelpProperties(
                "Ascii: can be seen in a built-in text viewer through 'Edit Ascii Export'.\n"
                "NetCDF: can be used as source data in a new dataset.\n"
                "GeoTiff: available if the axes of the operation are longitude and latitude."
         ,0 , 6, Qt::AlignCenter );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Maximum );


    //	Buttons (Execute, Delay Execution, Cancel)

    mButtonBox = new QDialogButtonBox();
    mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
    mButtonBox->setOrientation( Qt::Horizontal );
    mButtonBox->setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );    
	mExecuteButton = mButtonBox->button( QDialogButtonBox::Ok );
    mExecuteButton->setText( "Execute" );
    mExecuteButton->setDefault( false );


    //	All

	options_group->setStyleSheet("QGroupBox { font-weight: bold; } ");
	output_group->setStyleSheet("QGroupBox { font-weight: bold; } ");
	delay_group->setStyleSheet("QGroupBox { font-weight: bold; } ");

    QBoxLayout *main_l =
            LayoutWidgets( Qt::Vertical,
                            {
								options_group,
                                output_group,
								delay_group,
								nullptr,
                                help_group,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "Export...");

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );
    setMaximumHeight( height() );

    Wire();
}


void CExportDialog::CalculateMinMax()
{
	WaitCursor wait;

	CFormula *formula = mOperation->GetFormula( CMapTypeField::eTypeOpAsField );

	CProduct *product_tmp = CProduct::Construct( *(const_cast<CDataset*>(mOperation->OriginalDataset())->GetProductList() ) );

	std::string error_msg;
	CExpression expr;
	if ( !CFormula::SetExpression( formula->GetDescription( true, mAliases, product_tmp->GetAliasesAsString() ), expr, error_msg ) )
	{
		wait.Restore();
		SimpleWarnBox( "Unable to calculate min/max. Expression can't be set:\n'" + error_msg + "'" );
		return;		//v3 didn't return
	}

	product_tmp->GetValueMinMax( expr, mOperation->GetRecord(), mColorRangeMin, mColorRangeMax, *formula->GetUnit() );

	delete product_tmp;
}


void CExportDialog::Wire()
{
	//ASCII related

	QString sprecision = n2s<std::string>( mASCIINumberPrecision ).c_str();
	mASCIINumberPrecisionLineEdit->setText( sprecision );

	//GeoTIFF related

	CalculateMinMax();

	if ( mIsGeoImage )
	{
		mFormatGeoTiffButton->setEnabled( true );

		mCreateGoogleKMLFileGroup->setChecked( mCreateKMLTrackData || mCreateKMLFieldsData );
		KMLAlongTrackDataCheck->setChecked( mCreateKMLTrackData );
		KMLFieldsDataCheck->setChecked( mCreateKMLFieldsData );
		mCreateGeoTiffFilesCheck->setChecked( mCreateGeoTIFFs );
		mColorMapWidget->SetLUT( mLUT, mColorRangeMin, mColorRangeMax );
		connect( mColorMapWidget, SIGNAL( CurrentIndexChanged( int ) ), this, SLOT( HandleColorTablesIndexChanged( int ) ) );
	}
	else
	{
		mFormatGeoTiffPage->setVisible( false );
		mFormatGeoTiffPage->setEnabled( false );
		mFormatGeoTiffButton->setEnabled( false );
	}

	//schedule

	mDelayExecutionCheckBox->setChecked( mDelayExecution );


	//output path

	mCurrentAsciiOutputFileName = mOperation->GetExportAsciiOutputPath();
	mCurrentGeoTIFFOutputFileName = mOperation->GetExportGeoTIFFOutputPath();
	mCurrentNetcdfOutputFileName = mOperation->GetExportNetcdfOutputPath();;


	//set ASCII page

	mExportType = eASCII;
	HandleExportType( eASCII );


	// Connect

	connect( KMLFieldsDataCheck, SIGNAL( toggled( bool ) ), this, SLOT( HandleKMLFieldsDataChecked( bool ) ) );
	connect( KMLAlongTrackDataCheck, SIGNAL( toggled( bool ) ), this, SLOT( HandleKMLAlongTrackDataChecked( bool ) ) );
	connect( mCreateGeoTiffFilesCheck, SIGNAL( toggled( bool ) ), this, SLOT( HandleCreateGeoTiffFilesChecked( bool ) ) );
	connect( mCreateGoogleKMLFileGroup, SIGNAL( toggled( bool ) ), this, SLOT( HandleCreateGoogleKMLFileChecked( bool ) ) );

	connect( mBrowseButton, SIGNAL( clicked() ), this, SLOT( HandleChangeExportPath() ) );
	connect( mStackedWidget, SIGNAL( currentChanged( int ) ), this, SLOT( HandleExportType( int ) ) );
	connect( mDelayExecutionButton, SIGNAL( clicked() ), this, SLOT( HandleDelayExecution() ) );
	connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
	connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}


CExportDialog::CExportDialog( const std::string logo_path, CWorkspaceOperation *wkso, COperation *operation, const CStringMap *aliases, QWidget *parent )
    : base_t( parent )
	, mWOperation( wkso )
	, mOperation( operation )
	, mAliases( aliases )
	, mLUT( new CBratLookupTable )
	, mIsGeoImage( operation->IsMap() && operation->GetFormulaCountDataFields() > 0 )
	, mDateTime( QDateTime::currentDateTime() )
	, mLogoPath( logo_path )
{
	assert__( mWOperation && mOperation );

	mOperation->GetExportGeoTiffProperties( mCreateKMLFieldsData, mCreateKMLTrackData, mCreateGeoTIFFs, mColorTable );
	if ( mColorTable.empty() )
		mLUT->ExecMethodDefaultColorTable();
	else
		mLUT->ExecMethod( mColorTable );

    CreateWidgets();
}


CExportDialog::~CExportDialog()
{
	delete mLUT;
}


// Ensure at least one of tiff/kml is checked
//
void CExportDialog::HandleCreateGeoTiffFilesChecked( bool checked )
{
	if ( !checked && !mCreateGoogleKMLFileGroup->isChecked() )
	{
		mCreateGeoTiffFilesCheck->setChecked( true );
		return;
	}

	HandleExportType( mExportType );
}
void CExportDialog::HandleCreateGoogleKMLFileChecked( bool checked )
{
	if ( !checked && !mCreateGeoTiffFilesCheck->isChecked() )
	{
		mCreateGoogleKMLFileGroup->setChecked( true );		
		return;
	}

	// If checked, ensure at least kml-fields is checked
	//
	if ( checked && !KMLFieldsDataCheck->isChecked() && !KMLAlongTrackDataCheck->isChecked())
		KMLFieldsDataCheck->setChecked( true );		

	HandleExportType( mExportType );
}
// Ensure at least one of kml-fields/kml-tracks is checked
//
void CExportDialog::HandleKMLFieldsDataChecked( bool checked )
{
	if ( !checked && !KMLAlongTrackDataCheck->isChecked() )
		KMLFieldsDataCheck->setChecked( true );		
}
void CExportDialog::HandleKMLAlongTrackDataChecked( bool checked )
{
	if ( !checked && !KMLFieldsDataCheck->isChecked() )
		KMLAlongTrackDataCheck->setChecked( true );		
}



void CExportDialog::HandleExportType( int index )
{
	assert__( index >= 0 && index < EExportFormat_size && ( index != eGEOTIFF || mIsGeoImage ) );

	mExportType = (EExportFormat)index;

	std::string extension;
	std::string currentoutputfilename;
	switch ( mExportType )
	{
		case eASCII:
			currentoutputfilename = mCurrentAsciiOutputFileName;
			extension = smDefaultExtensionAscii;
			break;
		case eNETCDF:
			currentoutputfilename = mCurrentNetcdfOutputFileName;
			extension = smDefaultExtensionNetCdf;
			break;
		case eGEOTIFF:
			currentoutputfilename = mCurrentGeoTIFFOutputFileName;
			extension = mCreateGeoTiffFilesCheck->isChecked() ? smDefaultExtensionGeoTiff : smDefaultExtensionKML;
			break;
		default:
			assert__( false );
	}

	SetFileExtension( currentoutputfilename, extension );
	mOutputPathLineEdit->setText( currentoutputfilename.c_str() );
}


void CExportDialog::HandleChangeExportPath()
{
	std::string currentoutputfilename;
	switch ( mExportType )
	{
		case eASCII:
			currentoutputfilename = mCurrentAsciiOutputFileName;
			break;
		case eNETCDF:
			currentoutputfilename = mCurrentNetcdfOutputFileName;
			break;
		case eGEOTIFF:
			currentoutputfilename = mCurrentGeoTIFFOutputFileName;
			break;
		default:
			assert__( false );
	}

	QString export_path = BrowseNewFile( this, "Export Output File", QString( currentoutputfilename.c_str() ) );
	if ( !export_path.isEmpty() )
	{
		currentoutputfilename = q2a( export_path );
		switch ( mExportType )
		{
			case eASCII:
				mCurrentAsciiOutputFileName = currentoutputfilename;
				break;
			case eNETCDF:
				mCurrentNetcdfOutputFileName = currentoutputfilename;
				break;
			case eGEOTIFF:
				mCurrentGeoTIFFOutputFileName = currentoutputfilename;
				break;
            default:
                assert__( false );
        }

		mOutputPathLineEdit->setText( export_path );
	}
}


void CExportDialog::HandleDelayExecution()
{
	if ( mDateTime < QDateTime::currentDateTime() )
		mDateTime = QDateTime::currentDateTime();

	std::string mTaskLabel;
	if ( mTaskLabel.empty() )
		switch ( mExportType )
		{
			case eASCII:
				mTaskLabel = mOperation->GetTaskName( EExecutionType::eExportASCII, true );
				break;
			case eNETCDF:
				mTaskLabel = mOperation->GetTaskName( EExecutionType::eOperation, true );
				break;
			case eGEOTIFF:
				mTaskLabel = mOperation->GetTaskName( EExecutionType::eExportGeoTIFF, true );
				break;
			default:
				assert__( false );
		}

    CDelayExecutionDialog dlg( mTaskLabel, mDateTime, this );

	if ( dlg.exec() == QDialog::Accepted )
	{
		switch ( mExportType )
		{
			case eASCII:
				mOperation->SetScheduledExportAsciiTaskName( mTaskLabel );
				break;
			case eNETCDF:
				mOperation->SetScheduledExportNetcdfTaskName( mTaskLabel );
				break;
			case eGEOTIFF:
				mOperation->SetScheduledExportGeoTIFFTaskName( mTaskLabel );
				break;
            default:
                assert__( false );
        }
	}
}


void CExportDialog::HandleColorTablesIndexChanged( int )
{
	mColorRangeMin = mColorMapWidget->ColorRangeMin();
	mColorRangeMax = mColorMapWidget->ColorRangeMax();
}


bool CExportDialog::Validate()
{
	mDelayExecution = mDelayExecutionCheckBox->isChecked();

	SetFileExtension( mCurrentNetcdfOutputFileName, smDefaultExtensionNetCdf );
	mOperation->SetExportNetcdfOutput( mCurrentNetcdfOutputFileName, mWOperation );

	SetFileExtension( mCurrentAsciiOutputFileName, smDefaultExtensionAscii );
	mOperation->SetExportAsciiOutput( mCurrentAsciiOutputFileName, mWOperation );

	SetFileExtension( mCurrentGeoTIFFOutputFileName, smDefaultExtensionGeoTiff );
	mOperation->SetExportGeoTIFFOutput( mCurrentGeoTIFFOutputFileName, mWOperation );

	//	The output file is not updated as it should when the values in the formula tree change; 
	//	so, inspecting IsExecuteAgain and output file existence is not enough to know when execution 
	//	is necessary. Hence, here, we are forced to always execute, because apparently there is no other
	//	mechanism that we could inspect to know if the output (netcdf) file is updated.
	//
	mOperation->SetExecuteAgain( true );

 	switch ( mExportType )
	{
		case eASCII:
		{
			QString sprecision = mASCIINumberPrecisionLineEdit->text();
			bool ok_conv = false;
			mASCIINumberPrecision = sprecision.toInt( &ok_conv );
			if ( !ok_conv )
			{
				mASCIINumberPrecision = smDefaultDecimalPrecision;
				mASCIINumberPrecisionLineEdit->setText( n2s<std::string>( mASCIINumberPrecision ).c_str() );
			}

			mOperation->SetExportAsciiDateAsPeriod( mDeliverDataAsPeriod->isChecked() );
			mOperation->SetExportAsciiExpandArray( mExpandArrays->isChecked() );
			mOperation->SetExportAsciiNoDataComputation( mOnlyDumpExpressions->isChecked() );
			mOperation->SetExportAsciiNumberPrecision( mASCIINumberPrecision );
		}
		break;


		case eNETCDF:
		{
		}
		break;


		case eGEOTIFF:
		{
			mColorRangeMin = mColorMapWidget->ColorRangeMin();	//v4 not used
			mColorRangeMax = mColorMapWidget->ColorRangeMax();	//v4 not used

			mCreateKMLFieldsData = mCreateGoogleKMLFileGroup->isChecked() && KMLFieldsDataCheck->isChecked();
			mCreateKMLTrackData = mCreateGoogleKMLFileGroup->isChecked() && KMLAlongTrackDataCheck->isChecked();
			mCreateGeoTIFFs = mCreateGeoTiffFilesCheck->isChecked();
			mColorTable = q2a( mColorMapWidget->currentText() );

			mOperation->SetExportGeoTiffProperties( mCreateKMLFieldsData, mCreateKMLTrackData, mCreateGeoTIFFs, mColorTable, mLogoPath );
		}
		break;


		default:
			assert__( false );
		break;
	}

	return true;
}


//virtual
void  CExportDialog::accept()
{
	if ( Validate() )
		base_t::accept();
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ExportDialog.cpp"
