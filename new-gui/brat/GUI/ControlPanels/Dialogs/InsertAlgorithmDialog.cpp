#include "new-gui/brat/stdafx.h"

#include "libbrathl/BratAlgorithmBase.h"

#include "GUI/DisplayWidgets/TextWidget.h"

#include "InsertAlgorithmDialog.h"


Q_DECLARE_METATYPE( CBratAlgorithmBase* )



CInsertAlgorithmDialog::CInsertAlgorithmDialog( QWidget *parent )
    : base_t( parent )
{
    Q_UNUSED( parent );

    CreateWidgets();
}

CInsertAlgorithmDialog::~CInsertAlgorithmDialog()
{}


enum EParameters
{
	eParameterName,
	eParameterDescription,
	eParameterFormat,
	eParameterUnit,

	EParameters_size
};


void  CInsertAlgorithmDialog::CreateWidgets()
{
    //	Create

	mAlgorithmsList = new QListWidget;

	QWidget *algo_list_w = new QWidget;
	LayoutWidgets( Qt::Vertical, { new QLabel( "Algorithms" ), mAlgorithmsList }, algo_list_w, 2, 2, 2, 2, 2 );

	mDescriptionTextWidget = new CTextWidget;

	mParametersTable = new QTableWidget;
	mParametersTable->setColumnCount( EParameters_size );
	mParametersTable->setHorizontalHeaderLabels( QStringList() << "Name" << "Description" << "Format" << "Unit" );
	mParametersTable->setSortingEnabled( true );
	mParametersTable->setSelectionBehavior( QAbstractItemView::SelectRows );
	mParametersTable->setAlternatingRowColors( true );
    mParametersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	mParametersTable->setWordWrap( true );
	QFont font = mParametersTable->horizontalHeader()->font();
	font.setBold( true );
	mParametersTable->horizontalHeader()->setFont( font );


	mOutputUnitTextWidget = new QLineEdit;

	QWidget *algo_desc_w = new QWidget;
	LayoutWidgets( Qt::Vertical, 
	{ 
		new QLabel( "Description" ),
		mDescriptionTextWidget,
		new QLabel( "Output Unit" ),
		mOutputUnitTextWidget

	}, algo_desc_w, 2, 2, 2, 2, 2 );


	auto *content = CreateSplitter( nullptr, Qt::Horizontal, { algo_list_w, algo_desc_w } );

	//auto *content = LayoutWidgets( Qt::Horizontal, 
	//{
	//	algo_list,
	//	algo_desc

	//}, nullptr, 2, 2, 2, 2, 2 );


    //	... Help

    auto help = new CTextWidget;
	help->setHelpProperties(
		//"Algorithms provide means of computing complex operations. They are pre-defined and compiled within BRAT. They\n"
		//" include an algorithm name and a number of input parameters (depending on the algorithm) to be filled in by the user.\n"
		//" Note that, as in the all of BRAT, computations are done in SI units. If the field(s) you are using have a unit\n"
		//" defined, BRAT will take care of the conversion. However, beware if there is no unit really defined ('count').\n"
		//" BRAT will then consider the data as in S.I."
		"As in whole BRAT, computations are done using SI units.\n"
		"Conversions are possible when the unit is defined (basically, in the data fields, e.g. cm can be converted in m).\n"
		"Note that, however, they are not possible when using expressions where the unit might be different from SI\n"
		"(e.g. cm instead of m), while not formally defined.\n"
		, 6 );
	auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
	help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


    //	... Buttons

    mButtonBox = new QDialogButtonBox( this );
    mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
    mButtonBox->setOrientation( Qt::Horizontal );
    mButtonBox->setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    mButtonBox->button( QDialogButtonBox::Ok )->setDefault( true );


    QBoxLayout *main_l =
            LayoutWidgets( Qt::Vertical,
                            {
								content,
		new QLabel( "Input Parameters" ),
		mParametersTable,
                                help_group,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "Insert Algorithm");

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

    Wire();
}


void  CInsertAlgorithmDialog::Wire()
{
    //	Setup things

    connect( mAlgorithmsList, SIGNAL( currentRowChanged( int ) ), this, SLOT( HandleAlgorithmsListSelection( int ) ) );

    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );

	FillAlgorithmsList();

	mAlgorithmsList->setCurrentRow( 0 );
}


void CInsertAlgorithmDialog::FillAlgorithmsList()
{
	CMapBratAlgorithm *algorithms = CBratAlgorithmBaseRegistry::GetAlgorithms();
	if ( algorithms == nullptr )
	{
		return;
	}

	for ( CMapBratAlgorithm::iterator it = algorithms->begin(); it != algorithms->end(); it++ )
	{
		CBratAlgorithmBase* ob = it->second;
		if ( ob != nullptr )
		{
			QListWidgetItem *item = new QListWidgetItem( ob->GetName().c_str(), mAlgorithmsList );
			item->setData( Qt::UserRole, QVariant::fromValue( ob ) );
		}
	}

	mAlgorithmsList->setResizeMode( mAlgorithmsList->Adjust );
}


void CInsertAlgorithmDialog::HandleAlgorithmsListSelection( int index )
{
	mDescriptionTextWidget->clear();
	mOutputUnitTextWidget->clear();
	mCurrentAlgorithm = nullptr;

	if ( index < 0 )
		return;

	QListWidgetItem *item = mAlgorithmsList->item( index );
	mCurrentAlgorithm = item->data( Qt::UserRole ).value< CBratAlgorithmBase* >();

	mDescriptionTextWidget->setPlainText( mCurrentAlgorithm->GetDescription().c_str() );
	mOutputUnitTextWidget->setText( mCurrentAlgorithm->GetOutputUnit().c_str() );

	int n = (int)mCurrentAlgorithm->GetNumInputParam();
	mParametersTable->setRowCount( (int)n );

	for ( int index = 0; index < n; index++ )
	{
		auto *
		item = new QTableWidgetItem( mCurrentAlgorithm->GetParamName( index ).c_str() );
		mParametersTable->setItem( index, eParameterName, item );

		item = new QTableWidgetItem( mCurrentAlgorithm->GetInputParamDescWithDefValueLabel( index ).c_str() );
		item->setToolTip( mCurrentAlgorithm->GetInputParamDescWithDefValueLabel( index ).c_str() );
		item->setTextAlignment(Qt::AlignCenter);
		mParametersTable->setItem( index, eParameterDescription, item );

		item = new QTableWidgetItem( mCurrentAlgorithm->GetInputParamFormatAsString( index ).c_str() );
		item->setTextAlignment(Qt::AlignCenter);
		mParametersTable->setItem( index, eParameterFormat, item );

		item = new QTableWidgetItem( mCurrentAlgorithm->GetInputParamUnit( index ).c_str() );
		item->setTextAlignment(Qt::AlignCenter);
		mParametersTable->setItem( index, eParameterUnit, item );
	}

	//mParametersTable->resizeColumnToContents( eParameterDescription );
	mParametersTable->resizeColumnToContents( eParameterUnit );

	//mParametersTable->horizontalHeader()->setStretchLastSection( true );
	mParametersTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	mParametersTable->horizontalHeader()->setHighlightSections( true );

	mParametersTable->resizeColumnToContents( eParameterName );
	mParametersTable->resizeColumnToContents( eParameterFormat );

	mParametersTable->resizeRowsToContents();
}





//virtual
void  CInsertAlgorithmDialog::accept()
{
	assert__( mCurrentAlgorithm );

    base_t::accept();
}

///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_InsertAlgorithmDialog.cpp"
