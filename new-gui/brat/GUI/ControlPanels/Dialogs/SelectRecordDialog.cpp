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

#include "libbrathl/Product.h"

#include "DataModels/Workspaces/Workspace.h"

#include "new-gui/Common/GUI/TextWidget.h"

#include "SelectRecordDialog.h"


using namespace brathl;



///////////////////////////////////////////
//  CSelectRecordDialog
///////////////////////////////////////////

CSelectRecordDialog::CSelectRecordDialog( QWidget *parent, CProduct *product )
    : base_t( parent )
	, mProduct( product )
{
	assert__( mProduct );

    CreateWidgets();
}

CSelectRecordDialog::~CSelectRecordDialog()
{}


void CSelectRecordDialog::CreateWidgets()
{
    //	Create

	mRecordsList = new QListWidget;
	auto *content = LayoutWidgets( Qt::Vertical, { new QLabel( "Records" ), mRecordsList }, nullptr, 2, 2, 2, 2, 2 );

	//	... Help

    auto help = new CTextWidget;
    help->SetHelpProperties( 
		"Select an item from the 'Records' list."
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
								content,
                                help_group,
                                mButtonBox
                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "Record Selection" );

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

    Wire();
}


void  CSelectRecordDialog::Wire()
{
    //	Setup things

    connect( mRecordsList, SIGNAL( currentRowChanged( int ) ), this, SLOT( HandleRecordsListSelection( int ) ) );

    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );

	CStringArray v;
    mProduct->GetRecords( v );
	for ( auto &s : v )
	{
		mRecordsList->addItem( s.c_str() );
	}
}


void CSelectRecordDialog::HandleRecordsListSelection( int index )
{
	if ( index < 0 )
		return;

	mSelectedRecord = q2a ( mRecordsList->item( index )->text() );
}


//virtual
void  CSelectRecordDialog::accept()
{
	if ( mSelectedRecord.empty() )
	{
		SimpleErrorBox( "Please, select a record." );
		return;
	}

    base_t::accept();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////


#include "moc_SelectRecordDialog.cpp"
