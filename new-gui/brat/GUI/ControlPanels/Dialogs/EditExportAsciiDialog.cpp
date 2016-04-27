#include "new-gui/brat/stdafx.h"

#include "GUI/DisplayWidgets/TextWidget.h"

#include "EditExportAsciiDialog.h"



CEditExportAsciiDialog::CEditExportAsciiDialog( QWidget *parent )
    : base_t( parent )
{

    CreateWidgets();
}

CEditExportAsciiDialog::~CEditExportAsciiDialog()
{}


void  CEditExportAsciiDialog::CreateWidgets()
{
    //	Create

    //	... Help

    auto help = new CTextWidget;
    help->SetHelpProperties(
                "It can be used to visualize and edit the exported Ascii files."
         ,0 , 6, Qt::AlignCenter );
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


    setWindowTitle( "Edit Export Ascii...");

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

   Wire();
}


void  CEditExportAsciiDialog::Wire()
{
    //	Setup things

    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}



//virtual
void  CEditExportAsciiDialog::accept()
{

    base_t::accept();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_EditExportAsciiDialog.cpp"
