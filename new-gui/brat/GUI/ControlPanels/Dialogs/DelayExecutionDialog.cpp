#include "new-gui/brat/stdafx.h"


#include "GUI/DisplayWidgets/TextWidget.h"

#include "DelayExecutionDialog.h"


void  CDelayExecutionDialog::CreateWidgets()
{
    //	Content

    mExecDateEdit = new QDateTimeEdit;							//mExecDateEdit->setCalendarPopup(true);
    mExecDateEdit->setDisplayFormat("yyyy.MM.dd hh:mm:ss");
    mExecDateEdit->setMinimumDateTime( mDateTime );
	mTaskLabelLineEdit = new QLineEdit;
	mTaskLabelLineEdit->setText( mTaskLabel.c_str() );

	auto *content_l = LayoutWidgets( Qt::Horizontal,
	{
		LayoutWidgets( Qt::Vertical, { new QLabel( "Execution date and time" ), mExecDateEdit }, nullptr, 2, 2, 2, 2, 2 ),
		LayoutWidgets( Qt::Vertical, { new QLabel( "Task label (optional)" ), mTaskLabelLineEdit }, nullptr, 2, 2, 2, 2, 2 ),
	}, 
	nullptr, 6, 6, 6, 6, 6 );


    //	... Help

    auto help = new CTextWidget;
    help->SetHelpProperties(
    "This dialog schedules the active operation at a time given by the user.\n"
    "Note that the 'scheduler' must be running in order to have the tasks executed.\n"
    "Once scheduled, the operation can be viewed or removed within the 'scheduler' interface.",
	0, 2 );
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
								content_l,
								WidgetLine( this, Qt::Horizontal),
                                help_group,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "Delay Execution");

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


CDelayExecutionDialog::CDelayExecutionDialog( std::string &label, QDateTime &dt, QWidget *parent )
    : base_t( parent )
	, mTaskLabel( label )
	, mDateTime( dt )
{
    CreateWidgets();
}

CDelayExecutionDialog::~CDelayExecutionDialog()
{}



//virtual
void  CDelayExecutionDialog::accept()
{
	mTaskLabel = q2a( mTaskLabelLineEdit->text() );
	mDateTime = mExecDateEdit->dateTime();
    base_t::accept();
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DelayExecutionDialog.cpp"
