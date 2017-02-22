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

#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/GUI/TextWidget.h"
#include "new-gui/brat/DataModels/Filters/BratFilters.h"

#include "RadsPhasesDialog.h"



void CRadsPhasesDialog::Setup()
{
	int selected = -1, index = 0;
    std::for_each( mPhases.begin(), mPhases.end(), [this, &selected, &index]( const std::string &phase )
	{
		QListWidgetItem *item = new QListWidgetItem;
		item->setText( phase.c_str() );
		if ( std::find( mSelectedPhases.begin(), mSelectedPhases.end(), phase ) != mSelectedPhases.end() )
			item->setSelected( true );
		mPhasesListWidget->addItem( item );
		index++;
	} 
	);
	mPhasesListWidget->setCurrentRow( selected );

	connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}


void CRadsPhasesDialog::CreateWidgets()
{
	mPhasesListWidget = new QListWidget;
	mPhasesListWidget->setSelectionMode( QAbstractItemView::MultiSelection );

	auto *widgets_l = CreateGroupBox( ELayoutType::Vertical,
	{
		new QLabel( mMissionName.c_str() ), 
		mPhasesListWidget, 
	},
	"", nullptr, 2, 2, 2, 2, 2 );


    //... Help

    mHelpText = new CTextWidget;
	mHelpText->SetHelpProperties( "Select the phases of mission " + t2q( mMissionName ), 1, 6, Qt::AlignCenter, true );
	mHelpText->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	mHelpText->adjustSize();
	auto help_group = CreateGroupBox( ELayoutType::Vertical, { mHelpText }, "", nullptr, 6, 6, 6, 6, 6 );

    //... Buttons

    mButtonBox = new QDialogButtonBox( this );
    mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
    mButtonBox->setOrientation( Qt::Horizontal );
    mButtonBox->setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    mButtonBox->button( QDialogButtonBox::Ok )->setDefault( true );


    //... All

	QSplitter *splitter = CreateSplitter( nullptr, Qt::Vertical, { widgets_l, help_group } );

    QBoxLayout *main_l = LayoutWidgets( Qt::Vertical,
                            {
								splitter,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "Mission Phases");

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

	Setup();
}


CRadsPhasesDialog::CRadsPhasesDialog( const std::vector< std::string > &list, std::vector< std::string > &selected_list, 
	const std::string &name, QWidget *parent )

	: base_t( parent )
	, mPhases ( list )
	, mSelectedPhases( selected_list )
	, mMissionName( name )
{
	CreateWidgets();
}


CRadsPhasesDialog::~CRadsPhasesDialog()
{}



//virtual
void CRadsPhasesDialog::accept()
{
	QList<QListWidgetItem*> items = mPhasesListWidget->selectedItems();
	if ( items.empty() )
	{
		SimpleErrorBox( "At least one phase must be selected." );
		return;
	}

	mSelectedPhases.clear();
	for ( auto const *item : items )
	{
		mSelectedPhases.push_back( q2a( item->text() ) );
	}

	base_t::accept();
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_RadsPhasesDialog.cpp"
