#include "new-gui/brat/stdafx.h"


#include "GUI/DisplayWidgets/TextWidget.h"

#include "DelayExecutionDialog.h"




CDelayExecutionDialog::CDelayExecutionDialog(QWidget *parent)
    : base_t( parent )
{

    CreateWidgets();
}

CDelayExecutionDialog::~CDelayExecutionDialog()
{}


void  CDelayExecutionDialog::CreateWidgets()
{
    //	Create

    //	... Help

    auto help = new CTextWidget;
    help->SetHelpProperties(
    "The 'Delay execution' schedules the active operation at a time given by the user.\n"
    "Note that 'scheduler' has to be running in order to have the tasks executed.\n"
    "Date and time for the execution, as well as an optional name for the operation can be defined.\n"
    "Once scheduled, such an operation can be viewed or removed within BRAT scheduler interface."
         , 0, 6, Qt::AlignJustify );
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


    setWindowTitle( "Delay Execution...");

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

    Wire();
}


void  CDelayExecutionDialog::Wire()
{
    //	Setup things

    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}



//virtual
void  CDelayExecutionDialog::accept()
{

    base_t::accept();
}

///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DelayExecutionDialog.cpp"
