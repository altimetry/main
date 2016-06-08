#include "stdafx.h"

#include "new-gui/Common/GUI/TextWidget.h"

#include "EditTasksFileDialog.h"



void  CEditTasksFileDialog::CreateWidgets()
{
    //	Text

	mTextEdit = new CTextWidget;
	mTextEdit->SetSizeHint( mTextEdit->sizeHint().width(), 2 * mTextEdit->sizeHint().height() );
	auto *content_l = LayoutWidgets( Qt::Vertical, { mTextEdit }, nullptr, 2, 2, 2, 2, 2 );


    //	... Help

    auto help = new CTextWidget;
	std::string help_str = mTaskName.empty() ? "Scheduled tasks configuration file." : mTaskName + " log file content.";
    help->SetHelpProperties( help_str.c_str(), 0 , 6 );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


    //	... Buttons

    mButtonBox = new QDialogButtonBox( this );
    mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
    mButtonBox->setOrientation( Qt::Horizontal );
    mButtonBox->setStandardButtons( QDialogButtonBox::Close );
    mButtonBox->button( QDialogButtonBox::Close )->setDefault( true );

    QBoxLayout *main_l =
            LayoutWidgets( Qt::Vertical,
                            {
								content_l,
								WidgetLine( nullptr, Qt::Horizontal ),
                                help_group,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

   Wire();
}


void  CEditTasksFileDialog::Wire()
{
	mTextEdit->readFromFile( t2q( mPath ) );
	mTextEdit->MoveTo( 0, false );
	mTextEdit->SetReadOnlyEditor( true );

    //	connect

    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}


CEditTasksFileDialog::CEditTasksFileDialog( const std::string &task_name, const std::string &path, QWidget *parent )
    : base_t( parent )
	, mTaskName( task_name )
	, mPath( path )
{
    CreateWidgets();

	std::string title = mTaskName.empty() ? "Scheduler Tasks File: " : mTaskName + " Log: ";

	setWindowTitle( t2q( title + mPath ) );
}


CEditTasksFileDialog::~CEditTasksFileDialog()
{}


//virtual
void  CEditTasksFileDialog::accept()
{
    base_t::accept();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_EditTasksFileDialog.cpp"
