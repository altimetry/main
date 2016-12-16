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

#include "DatasetInterpolationDialog.h"



void CDatasetInterpolationDialog::Setup()
{
	mDateTimeEdit->setMinimumDateTime( CBratFilter::brat2q( CDate() ) );
	mDateTimeEdit->setDateTime( mDataModeDIDateTime );
	int selected = -1, index = 0;
    std::for_each( mFieldMNamesList.begin(), mFieldMNamesList.end(), [this, &selected, &index]( const std::string &name )
	{
		QListWidgetItem *item = new QListWidgetItem;
		item->setText( name.c_str() );
		if ( name == mDataModeDITimeName )
			selected = index;
		mFieldsList->addItem( item );
		index++;
	} 
	);
	mFieldsList->setCurrentRow( selected );

	connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}


void CDatasetInterpolationDialog::CreateWidgets()
{
	mFieldsList = new QListWidget;

	mDateTimeEdit = new QDateTimeEdit;							//mDateTimeEdit->setCalendarPopup(true);
	mDateTimeEdit->setDisplayFormat("yyyy.MM.dd hh:mm:ss");

	auto *widgets_l = CreateGroupBox( ELayoutType::Vertical,
	{
		new QLabel( "Time Field" ), mFieldsList, new QLabel( "Interpolation Date" ), mDateTimeEdit
	},
	"", nullptr, 2, 2, 2, 2, 2 );


    //... Help

    mHelpText = new CTextWidget;
	mHelpText->SetHelpProperties( "Select which of the product fields represents time and enter a date for Dataset Interpolation" , 1, 6, Qt::AlignCenter, true );
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


    setWindowTitle( "Dataset Interpolation");

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

	Setup();
}


CDatasetInterpolationDialog::CDatasetInterpolationDialog( const std::vector< std::string > &list, const std::string &name, const QDateTime &dt, QWidget *parent )
	: base_t( parent )
	, mFieldMNamesList ( list )
	, mDataModeDITimeName( name )
	, mDataModeDIDateTime( dt )
{
	CreateWidgets();
}


CDatasetInterpolationDialog::~CDatasetInterpolationDialog()
{}



//virtual
void CDatasetInterpolationDialog::accept()
{
	mDataModeDITimeName = q2a( mFieldsList->currentItem()->text() );
	mDataModeDIDateTime = mDateTimeEdit->dateTime();
	base_t::accept();
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DatasetInterpolationDialog.cpp"
