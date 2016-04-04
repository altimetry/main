#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtils.h"

#include "libbrathl/Expression.h"

#include "DataModels/Workspaces/Function.h"
#include "GUI/DisplayWidgets/TextWidget.h"

#include "InsertFunctionDialog.h"



void CInsertFunctionDialog::Setup()
{
    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );

	connect( mFunctionCategories, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleFunctionCategoriesIndexChanged( int ) ) );
	connect( mFunctionList, SIGNAL( currentRowChanged( int ) ), this, SLOT( HandleFunctionListRowChanged( int ) ) );

	//GetCategory( CMapFunction::GetInstance(), *GetFunctionCat() );
	mFunctionCategories->clear();
	mFunctionCategories->addItem( "All", -1 );
	for ( int category = MathTrigo; category <= Geographical; category++ )
	{
		mFunctionCategories->addItem( CFunction::GetCategoryAsString( category ).c_str(), category );
	}

	//int32_t sel = 0;
	//GetFunctionCat()->SetSelection( 0 );
	//FunctionCat( sel );
}


void CInsertFunctionDialog::CreateWidgets()
{
	mFunctionCategories = new QComboBox;
	mFunctionList = new QListWidget;

	auto *widgets_l = CreateGroupBox( ELayoutType::Vertical,
	{
		new QLabel( "Select a Category" ), mFunctionCategories, new QLabel( "Select a Function" ), mFunctionList
	},
	"", nullptr, 2, 2, 2, 2, 2 );


    //... Help

    mHelpText = new CTextWidget;
	//mHelpText->setHelpProperties( "" , 6, Qt::AlignCenter );
    auto help_group = CreateGroupBox( ELayoutType::Vertical, { mHelpText }, "", nullptr, 6, 6, 6, 6, 6 );
    mHelpText->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

    //... Buttons

    mButtonBox = new QDialogButtonBox( this );
    mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
    mButtonBox->setOrientation( Qt::Horizontal );
    mButtonBox->setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    mButtonBox->button( QDialogButtonBox::Ok )->setDefault( true );


    //... All

	QSplitter *splitter = CreateSplitter( nullptr, Qt::Vertical, { widgets_l, help_group } );

    QBoxLayout *main_l = LayoutWidgets( Qt::Vertical,
                            {
								splitter,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "Insert Function");

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

	Setup();
}


CInsertFunctionDialog::CInsertFunctionDialog( QWidget *parent )
	: base_t( parent )
{
	CreateWidgets();
}


CInsertFunctionDialog::~CInsertFunctionDialog()
{}



void CInsertFunctionDialog::FillFunctionList( int category )
{
	mFunctionList->clear();

	//NamesToListBox( CMapFunction::GetInstance(), *GetFunctionList(), category );
	auto &mapf = CMapFunction::GetInstance();
	for ( CMapFunction::const_iterator it = mapf.begin(); it != mapf.end(); it++ )
	{
		CFunction* value = dynamic_cast< CFunction* >( it->second );
		if ( value != nullptr )
		{
			if ( category != -1 )
			{
				if ( value->GetCategory() != category )
				{
					continue;
				}
			}
			QListWidgetItem *item = new QListWidgetItem( value->GetName().c_str(), mFunctionList );
            Q_UNUSED( item );
		}
	}

	//int sel = 0;
	//GetFunctionList()->SetSelection( sel );
	//FunctionList( sel );
	mFunctionList->setCurrentRow( 0 );
}


void CInsertFunctionDialog::HandleFunctionCategoriesIndexChanged( int index )		//CFunctionsDlg::FunctionCat( int32_t sel )
{
	mFunctionList->clear();
	if ( index < 0 )
		return;

	int category = static_cast< int >( mFunctionCategories->itemData( index ).toInt() );
	FillFunctionList( category );
}


void CInsertFunctionDialog::HandleFunctionListRowChanged( int index )
{
	mHelpText->setText( "" );
	if ( index < 0 )
		return;

	std::string func_name = q2a( mFunctionList->item( index )->text() );
	std::string func_desc = CMapFunction::GetInstance().GetDescFunc( func_name );
	//if ( !EndsWith( func_desc, std::string( "\n" ) ) )
	//	func_desc += "\n";
    mHelpText->SetHelpProperties( func_desc.c_str(), 0, 6, Qt::AlignCenter, true );
    //mHelpText->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}


//virtual
void CInsertFunctionDialog::accept()
{
	mResultFunction = q2a( mFunctionList->currentItem()->text() );

    base_t::accept();
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_InsertFunctionDialog.cpp"
