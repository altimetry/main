/*
* This file is part of BRAT 
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "new-gui/brat/stdafx.h"


#include "new-gui/Common/GUI/TextWidget.h"

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
    "The 'scheduler' must be running in order to have the tasks executed.\n"
    "The operation can be viewed or removed within the 'scheduler' interface.", 0, 2 );
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
