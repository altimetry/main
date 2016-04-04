#include "new-gui/brat/stdafx.h"


#include "GUI/DisplayWidgets/TextWidget.h"

#include "ShowAliasesDialog.h"




CShowAliasesDialog::CShowAliasesDialog( QWidget *parent )
    : base_t( parent )
{
    Q_UNUSED( parent );

    CreateWidgets();
}

CShowAliasesDialog::~CShowAliasesDialog()
{}


void  CShowAliasesDialog::CreateWidgets()
{
    //	Create

    //	... Help

    auto help = new CTextWidget;
    help->SetHelpProperties(
        "Provides information about the aliases available for the chosen dataset. Aliases are\n"
        " equivalents that you can use instead of the fields name."
         ,0 , 6, Qt::AlignJustify );
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
                                help_group,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "Show 'PRODUCT_NAME' aliases...");

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

    Wire();
}


void  CShowAliasesDialog::Wire()
{
    //	Setup things

    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}



//virtual
void  CShowAliasesDialog::accept()
{

    base_t::accept();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ShowAliasesDialog.cpp"
