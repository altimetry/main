#include "new-gui/brat/stdafx.h"


#include "GUI/DisplayWidgets/TextWidget.h"

#include "ShowInfoDialog.h"




CShowInfoDialog::CShowInfoDialog( QWidget *parent )
    : base_t( parent )
{
    Q_UNUSED( parent );

    CreateWidgets();
}


CShowInfoDialog::~CShowInfoDialog()
{}


void  CShowInfoDialog::CreateWidgets()
{
    //	Create

    //	... Help

    auto help = new CTextWidget;
    help->setHelpProperties(
        "Provides information about the original units (the ones defined in the data products) and the\n"
        " units used during computation or selection."
         , 6, Qt::AlignJustify );
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


    setWindowTitle( "Show 'FIELD_NAME' information...");

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

    Wire();
}


void  CShowInfoDialog::Wire()
{
    //	Setup things

    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}



//virtual
void  CShowInfoDialog::accept()
{

    base_t::accept();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ShowInfoDialog.cpp"
