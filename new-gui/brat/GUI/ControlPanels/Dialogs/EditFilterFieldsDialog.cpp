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

#include "DataModels/Workspaces/Operation.h"

#include "new-gui/Common/GUI/TextWidget.h"

#include "EditFilterFieldsDialog.h"


///////////////////////////////////////////
//  CEditFilterFieldsDialog
///////////////////////////////////////////

void  CEditFilterFieldsDialog::CreateWidgets()
{
    //	Create

	mLongitudeLineEdit = new QLineEdit;
	auto *lon_l = LayoutWidgets( Qt::Vertical, { new QLabel( "Longitude" ), mLongitudeLineEdit }, nullptr, 2, 2, 2, 2, 2 );

	mLatitudeLineEdit = new QLineEdit;
	auto *lat_l = LayoutWidgets( Qt::Vertical, { new QLabel( "Latitude" ), mLatitudeLineEdit }, nullptr, 2, 2, 2, 2, 2 );

	mTimeLineEdit = new QLineEdit;
	auto *time_l = LayoutWidgets( Qt::Vertical, { new QLabel( "Time" ), mTimeLineEdit }, nullptr, 2, 2, 2, 2, 2 );

	mResetPushButton = new QPushButton( "Reset" );
	mResetPushButton->setToolTip( "Reset variables to their default aliases names" );
	auto *reset_l = LayoutWidgets( Qt::Horizontal, { nullptr, mResetPushButton }, nullptr, 2, 2, 2, 2, 2 );

	auto *content = LayoutWidgets( Qt::Vertical,
	{
								lon_l,
								lat_l,
								time_l,
								reset_l
	}, 
	nullptr, 2, 2, 2, 2, 2 );




	//	... Help

    auto help = new CTextWidget;
    help->SetHelpProperties(
        " Edit the names of the variables used in the construction of the operation filter. \n"
        " This is necessary only if longitude, latitude or time are used in the operation  \n"
		" and any of the variables has a name different from the respective alias (default). \n"
		" Use the Reset button to revert to the default aliases names."
		,0 , 6 );
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
								content,
								//lon_l,
								//lat_l,
								//time_l,
								//comment_l,
                                help_group,
                                mButtonBox
                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "Filter Variables");

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

    Wire();
}


CEditFilterFieldsDialog::CEditFilterFieldsDialog( COperation *operation, QWidget *parent )
    : base_t( parent )
	, mOperation( operation )
{
	assert__( mOperation );

    CreateWidgets();
}

CEditFilterFieldsDialog::~CEditFilterFieldsDialog()
{}


void  CEditFilterFieldsDialog::Wire()
{
	Reset();

	connect( mResetPushButton, SIGNAL( clicked() ), this, SLOT( HandleResetPushButton() ) );
    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}


void CEditFilterFieldsDialog::Reset()
{
	auto names = mOperation->FilterVariablesNames();

	mLongitudeLineEdit->setText( names[ eFilterVariableLon ].c_str() );
	mLatitudeLineEdit->setText( names[ eFilterVariableLat ].c_str() );
	mTimeLineEdit->setText( names[ eFilterVariableTime ].c_str() );
}


void  CEditFilterFieldsDialog::HandleResetPushButton()
{
	mOperation->SetFilterVariablesNames();
	Reset();
}


//virtual
void  CEditFilterFieldsDialog::accept()
{
	QString lon_name = mLongitudeLineEdit->text();
	QString lat_name = mLatitudeLineEdit->text();
	QString time_name = mTimeLineEdit->text();
	std::string error_msg;
	if ( lon_name.isEmpty() )
	{
		error_msg = "Longitude ";
		mLongitudeLineEdit->setFocus();
	}
	else
	if ( lat_name.isEmpty() )
	{
		error_msg = "Latitude ";
		mLongitudeLineEdit->setFocus();
	}
	else
	if ( time_name.isEmpty() )
	{
		error_msg = "Time";
		mLongitudeLineEdit->setFocus();
	}
	if ( !error_msg.empty() )
	{
		SimpleErrorBox( error_msg + " variable cannot be empty." );
		return;
	}

	mOperation->SetFilterVariablesNames( { q2a( lon_name ), q2a( lat_name ), q2a( time_name ) } );
	if ( !error_msg.empty() )
	{
		SimpleErrorBox( error_msg );
		mTimeLineEdit->setFocus();
		return;
	}

	base_t::accept();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////


#include "moc_EditFilterFieldsDialog.cpp"
