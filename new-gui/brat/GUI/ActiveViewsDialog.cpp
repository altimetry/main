#include "new-gui/brat/stdafx.h"


#include "new-gui/Common/QtUtils.h"

#include "DataModels/Model.h"
#include "DataModels/Workspaces/Workspace.h"
#include "DataModels/Workspaces/Display.h"

#include "new-gui/Common/GUI/TextWidget.h"

#include "ActiveViewsDialog.h"



void CActiveViewsDialog::CreateWidgets()
{
	static const QStringList header_labels = QStringList() << tr( "Name" ) << tr( "Type" ) << tr( "Value" );

	//	Create

	//	... Tree

	mViewsListWidget = new QListWidget( this );
	mViewsListWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
	mViewsListWidget->setSelectionMode( QAbstractItemView::SingleSelection );
	mViewsListWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );

	//	... Help

	auto help = new CTextWidget;
	help->SetHelpProperties( "Select a view from all view windows currently opened", 0, 6, Qt::AlignCenter );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


	//	... Buttons

	mCloseDialogButton = new QPushButton( "Done", this );
	mCloseWindowButton = new QPushButton( "Close", this );
	mSelectWindowButton = new QPushButton( "Select", this );

	auto *buttons = LayoutWidgets( Qt::Horizontal, { mSelectWindowButton, mCloseWindowButton, nullptr, mCloseDialogButton }, nullptr, 2, 2, 2, 2, 2 );

    QBoxLayout *main_l =
            LayoutWidgets( Qt::Vertical,
                            {
                                mViewsListWidget,
								help_group,
                                buttons

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "Active Views" );

    //	Wrap up dimensions

    adjustSize();
	setMinimumWidth( width() );

	Wire();
}

void CActiveViewsDialog::Wire()
{
    //	Setup things

	connect( mCloseDialogButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( mCloseWindowButton, SIGNAL( clicked() ), this, SLOT( HandleCloseWindow() ) );
	connect( mSelectWindowButton, SIGNAL( clicked() ), this, SLOT( HandleSelectWindow() ) );
	connect( mSelectWindowButton, SIGNAL( clicked() ), this, SLOT( accept() ) );

	RefreshWindowsList();
}


void CActiveViewsDialog::RefreshWindowsList()
{
	mSubWindows = mDesktopManager->SubWindowList();			//for some reason this fails (crashes) in the initializer list...

	mViewsListWidget->clear();
	for ( auto *sub_window : mSubWindows )
	{
		//QListWidgetItem *item = 
		new QListWidgetItem( sub_window->windowTitle(), mViewsListWidget );
	}
	mViewsListWidget->setCurrentRow( 0 );
	mCloseWindowButton->setEnabled( mSubWindows.size() > 0 );
	mSelectWindowButton->setEnabled( mSubWindows.size() > 0 );
}



CActiveViewsDialog::CActiveViewsDialog( QWidget *parent, CDesktopManagerBase *manager )
	: base_t( parent )
	, mDesktopManager( manager )
{
	assert__( manager != nullptr );

	CreateWidgets();

	assert__( mSubWindows.size() > 0 );
}


CActiveViewsDialog::~CActiveViewsDialog()
{}


void CActiveViewsDialog::HandleCloseWindow()
{
	int index = mViewsListWidget->currentRow();
	if ( index >= 0 )
	{
		auto *sub_window = mSubWindows[ index ];
		if ( sub_window->close() )
			RefreshWindowsList();
		else
			SimpleWarnBox( "Window " + q2a( sub_window->windowTitle() ) + " could not be closed." );
	}
}


void CActiveViewsDialog::HandleSelectWindow()
{
	int index = mViewsListWidget->currentRow();
	if ( index >= 0 )
	{
		auto *sub_window = mSubWindows[ index ];
		sub_window->setVisible( true );
		sub_window->showNormal();
        sub_window->activateWindow();   //linux
    }
}


//virtual 
QSize CActiveViewsDialog::sizeHint() const
{
	int delta_height = base_t::sizeHint().height() - mViewsListWidget->sizeHint().height() + 4;		//+ 4: guessing borders
    QSize s( base_t::sizeHint().width(), 
		delta_height + (int)mDesktopManager->SubWindowList().count() * ( mViewsListWidget->fontMetrics().height() + 2 ) );	//+2: guessing items spacing
	return s;
}


//virtual 
void CActiveViewsDialog::accept()
{
	base_t::accept();
}






///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ActiveViewsDialog.cpp"
