#include "new-gui/brat/stdafx.h"


#include "new-gui/Common/GUI/TextWidget.h"

#include "DataDisplayPropertiesDialog.h"


void  CDataDisplayPropertiesDialog::CreateWidgets()
{
    //	Create

    mNorthComponent = new QRadioButton( "North Component" );
    mEastComponent = new QRadioButton( "East Component" );
    mVectorBox = CreateGroupBox( ELayoutType::Vertical,
    {
        mNorthComponent,
        mEastComponent
    },
    "Vector Plot Component", nullptr );
    mVectorBox->setCheckable(true);
    mVectorBox->setChecked(false);


    //	... Help

    auto help = new CTextWidget;
    QString name = t2q( mFormula->GetName() );
    help->SetHelpProperties(
                "Edit the expression '" + name + "' display properties.\n"
                "Vector box is available only if the operation has more than 1 data field.",
        0 , 6 );
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
                               mVectorBox,
                               WidgetLine(this, Qt::Horizontal),
                               help_group,
                               WidgetLine(this, Qt::Horizontal),
                               mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

    Wire();
}


void  CDataDisplayPropertiesDialog::Wire()
{
	size_t data_count = 0;
    const CMapFormula &formulas = *mOperation->GetFormulas();
	for ( CMapFormula::const_iterator it = formulas.begin(); it != formulas.end(); it++ )
	{
		CFormula *f = dynamic_cast<CFormula*>( it->second );
		if ( f == nullptr )
			continue;

		if ( f->GetFieldType() == CMapTypeField::eTypeOpAsField )
			data_count++;
	}

	mVectorBox->setEnabled( data_count > 1 );


    //	Setup things

    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}


CDataDisplayPropertiesDialog::CDataDisplayPropertiesDialog( QWidget *parent, CFormula *formula, COperation *operation )
    : base_t( parent )
    , mFormula( formula )
    , mOperation( operation )
{
	assert__( mFormula->GetFieldType() == CMapTypeField::eTypeOpAsField );

    CreateWidgets();

    setWindowTitle( ( "'" + formula->GetName() + "'' Display Properties").c_str() );
}


CDataDisplayPropertiesDialog::~CDataDisplayPropertiesDialog()
{}



//virtual
void  CDataDisplayPropertiesDialog::accept()
{
    if ( mVectorBox->isChecked() )
    {
        const CMapFormula &formulas = *mOperation->GetFormulas();
        for ( CMapFormula::const_iterator it = formulas.begin(); it != formulas.end(); it++ )
        {
            CFormula *f = dynamic_cast<CFormula*>( it->second );
            if ( f == nullptr || f == mFormula )
                continue;

            if ( ( f->IsNorthComponent() && mNorthComponent->isChecked() ) ||
                 ( f->IsEastComponent() && mEastComponent->isChecked() ) )
            {
                std::string s = mNorthComponent->isChecked() ? "north" : "east";
                SimpleErrorBox( "There is already a data field selected as " + s + " component." );
                return;
            }

            mFormula->SetNorthComponent( mNorthComponent->isChecked() );
            mFormula->SetEastComponent( mEastComponent->isChecked() );
        }
    }
    else
    {
        mFormula->SetNorthComponent( mNorthComponent->isChecked() );
        mFormula->SetEastComponent( mEastComponent->isChecked() );
    }
    base_t::accept();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DataDisplayPropertiesDialog.cpp"
