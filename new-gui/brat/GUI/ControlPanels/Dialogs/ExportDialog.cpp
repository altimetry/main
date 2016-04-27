#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtils.h"
#include "GUI/ActionsTable.h"
#include "GUI/DisplayWidgets/TextWidget.h"

#include "ExportDialog.h"





void  CExportDialog::CreateWidgets()
{
    // Output file

    QLineEdit *OutputPath = new QLineEdit();
    mBrowseButton = new QPushButton( "Browse..." );
	mBrowseButton->setToolTip( CActionInfo::FormatTip( "Select a output file." ).c_str() );

    auto *output_group = CreateGroupBox( ELayoutType::Horizontal, { OutputPath, mBrowseButton }, "Output File", nullptr, 6, 6, 6, 6, 6 );

    // Pages

	mFormatASCIIPage = new QFrame;
    mFormatASCIIPage->setFrameStyle( QFrame::Sunken );    //mFormatASCIIPage->setFrameShape( QFrame::Box );
	mFormatNetCdfPage = new QFrame;
    mFormatNetCdfPage->setFrameStyle( QFrame::Sunken );    //mFormatNetCdfPage->setFrameShape( QFrame::Box );
	mFormatGeoTiffPage = new QFrame;
    mFormatGeoTiffPage->setFrameStyle( QFrame::Sunken );    //mFormatGeoTiffPage->setFrameShape( QFrame::Box );


    // Ascii

	mDeliverDataAsPeriod = new QCheckBox( "Deliver date as period (duration)" );
	mDeliverDataAsPeriod->setToolTip( "Deliver date as a duration if check, otherwise deliver date as 'yyyy-mm-dd hh:mm:ss' format" );
	mExpandArrays = new QCheckBox( "Expand arrays so far as possible" );
	mExpandArrays->setToolTip( "Expand arrays so far as possible, i.e. if all export array fields have the same dimension" );
	mExecuteOperationAscii = new QCheckBox( "Execute operation before export" );
	mOnlyDumpExpressions = new QCheckBox( "Only dump expressions (no data computation)" );

	mASCIINumberPrecision = new QLineEdit;

	LayoutWidgets( 
	{ 
		mExecuteOperationAscii, mExpandArrays, nullptr, 
		mDeliverDataAsPeriod, mOnlyDumpExpressions, nullptr, 
		LayoutWidgets( Qt::Horizontal, { new QLabel( "Ascii number precision:" ), mASCIINumberPrecision, nullptr }, nullptr, 0,0,0,0,0)
	}, 
	mFormatASCIIPage, 6,6,6,6,6 );


    // NetCdf

	mExecuteOperationNetCdf  = new QCheckBox( "Execute operation before export" );
	LayoutWidgets( Qt::Vertical, 
	{ 
		mExecuteOperationNetCdf 
	}, 
	mFormatNetCdfPage, 6,6,6,6,6 );


	// GeoTiff

	mCreateGoogleKMLFile = new QCheckBox( "Create GoogleEarth KML file" );
	mColorTableCombo = new QComboBox;
	mColorRangeMin = new QLineEdit;
	mColorRangeMin->setMaximumWidth( 80 );
	mColorRangeMax = new QLineEdit;
	mColorRangeMax->setMaximumWidth( 80 );
	mCalculateMinMax = new QPushButton( "Calculate" );

	auto *color_range_group = CreateGroupBox( ELayoutType::Vertical, 
	{
		LayoutWidgets( Qt::Horizontal, { new QLabel("min"), mColorRangeMin, nullptr, new QLabel("max"), mColorRangeMax } , nullptr ,6,6,6,6,6 ),
		mCalculateMinMax,
	}
	,"Color Range", nullptr, 4,4,4,4,4 );
	color_range_group->setStyleSheet("QGroupBox { font-weight: normal; } ");		//necessary to break inheritance from enveloping group font

	auto *color_table_group = CreateGroupBox( ELayoutType::Horizontal, { mColorTableCombo }, "Color table", nullptr, 4, 4, 4, 4, 4 );
	color_table_group->setStyleSheet("QGroupBox { font-weight: normal; } ");		//necessary to break inheritance from enveloping group font

	LayoutWidgets( Qt::Horizontal,
	{
		LayoutWidgets( Qt::Vertical,
		{
			color_table_group,
			nullptr,
			mCreateGoogleKMLFile,
		}, 
		nullptr, 2, 2, 2, 2, 2 ),
		nullptr,
		color_range_group,
	},
	mFormatGeoTiffPage, 6, 6, 6, 6, 6 );


	// Stack pages

	mStackedWidget = new CStackedWidget( this, { 
		{ mFormatASCIIPage, "Ascii", CActionInfo::FormatTip("Ascii format options"), "://images/alpha-numeric/__q.png", true }, 
		{ mFormatNetCdfPage, "NetCdf", CActionInfo::FormatTip("NetCdf format options"), "://images/alpha-numeric/__r.png", true },
		{ mFormatGeoTiffPage, "GeoTiff", CActionInfo::FormatTip("GeoTiff format options"), "://images/alpha-numeric/__s.png", true }
	} );

	auto *b0 = dynamic_cast<QToolButton*>( mStackedWidget->Button( 0 ) );
	b0->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	auto *b1 = dynamic_cast<QToolButton*>( mStackedWidget->Button( 1 ) );
	b1->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	auto *b2 = dynamic_cast<QToolButton*>( mStackedWidget->Button( 2 ) );
	b2->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );

	auto *row = CreateButtonRow( true, Qt::Horizontal, { b0, b1, b2 } );
	auto *options_group = CreateGroupBox( ELayoutType::Vertical, { row, nullptr, mStackedWidget }, "Format", nullptr, 6,6,6,6,6 );


    // Help

    auto help = new CTextWidget;
    help->SetHelpProperties(
				"Ascii: can be seen in a built-in text viewer through 'Edit Ascii Export'\n"
				"     NetCDF: can be used as source data in a new dataset     \n"
				"GeoTiff: available if the axis of the operation are longitude and latitude."
         ,0 , 6, Qt::AlignCenter );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Maximum );


    //	Buttons (Execute, Delay Execution, Cancel)

    mButtonBox = new QDialogButtonBox();
    mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
    mButtonBox->setOrientation( Qt::Horizontal );
    mButtonBox->setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Save | QDialogButtonBox::Cancel );    
    mButtonBox->button( QDialogButtonBox::Ok )->setText( "Delay execution..." );
    mButtonBox->button( QDialogButtonBox::Save )->setText( "Execute" );
    mButtonBox->button( QDialogButtonBox::Ok )->setDefault( false );
    mButtonBox->button( QDialogButtonBox::Save )->setDefault( false );


    //	All

	options_group->setStyleSheet("QGroupBox { font-weight: bold; } ");
	output_group->setStyleSheet("QGroupBox { font-weight: bold; } ");

    QBoxLayout *main_l =
            LayoutWidgets( Qt::Vertical,
                            {
								options_group,
                                output_group,
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


void CExportDialog::Wire()
{
    //	Setup things

    connect( mButtonBox, SIGNAL( clicked( QAbstractButton* ) ), this, SLOT( HandleButtonClicked( QAbstractButton* ) ) );
    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}


CExportDialog::CExportDialog( QWidget *parent )
    : base_t( parent )
{

    CreateWidgets();
}

CExportDialog::~CExportDialog()
{}


//slot
void CExportDialog::HandleButtonClicked( QAbstractButton *button )
{
	if ( button == mButtonBox->button( QDialogButtonBox::Ok ) )
		DelayExecution();
	else
	if ( button == mButtonBox->button( QDialogButtonBox::Save ) )
		Execute();

}


void CExportDialog::DelayExecution()
{

}
void CExportDialog::Execute()
{

}


//virtual
void  CExportDialog::accept()
{
    base_t::accept();
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ExportDialog.cpp"
