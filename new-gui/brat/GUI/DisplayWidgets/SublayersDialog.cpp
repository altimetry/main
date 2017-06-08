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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA	 02110-1301, USA.
*/
#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtils.h"

#include "new-gui/Common/GUI/TextWidget.h"
#include "SublayersDialog.h"



//static 
const QString CSubLayerIdentifier::delim = ":";



void CSublayersDialog::CreateWidgets()
{
	mLayersTable = new QTreeWidget( this );
	mLayersTable->setObjectName( QString::fromUtf8( "mLayersTable" ) );
	mLayersTable->setSelectionMode( QAbstractItemView::SingleSelection );
	mLayersTable->setSelectionBehavior( QAbstractItemView::SelectRows );

    //	... Help

    auto help = new CTextWidget;
	help->SetHelpProperties( "Select sub-layer from list", 0, 2 );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


    //	... Buttons

	QDialogButtonBox *buttonBox = new QDialogButtonBox( this );
	buttonBox->setObjectName( QString::fromUtf8( "buttonBox" ) );
	buttonBox->setOrientation( Qt::Horizontal );
	buttonBox->setStandardButtons( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );

	LayoutWidgets( Qt::Vertical, { mLayersTable, help_group, buttonBox }, this, 6, 9, 9, 9, 9 );


    //	Wire

	QMetaObject::connectSlotsByName( this );

	connect( buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
	connect( buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}




CSublayersDialog::CSublayersDialog( QWidget* parent, const std::vector<CSubLayerIdentifier> &list, Qt::WindowFlags fl )	//delim = ":" , Qt::WindowFlags fl = 0 
	: base_t( parent, fl )
	, mList( list )
{
	CreateWidgets();

	setWindowTitle( tr( "Select vector layer to open..." ) );
	mLayersTable->setHeaderLabels( QStringList() << tr( "Layer ID" ) << tr( "Layer name" ) << tr( "Number of features" ) << tr( "Geometry type" ) );

	for ( auto const &id: mList )
	{
		assert__( id.IsPlolygonType() );

		auto *item = new QTreeWidgetItem( id.IdElements() );
		item->setData( 0, Qt::UserRole, QVariant::fromValue(&id) );	//using QVariant::fromValue is critical to retrieve value later
		mLayersTable->addTopLevelItem( item );
	}

	for ( int i = 0; i < mLayersTable->columnCount(); i++ )
		mLayersTable->resizeColumnToContents( i );

	int width = 0;
	for ( int i = 0; i < mLayersTable->columnCount(); ++i )
		width += 2 + mLayersTable->columnWidth( i );
	width += qApp->style()->pixelMetric( QStyle::PM_ScrollBarExtent );

	mLayersTable->setMinimumWidth( width );

	adjustSize();
}


CSublayersDialog::~CSublayersDialog()
{}





const CSubLayerIdentifier* CSublayersDialog::selection()
{
	auto selected_items = mLayersTable->selectedItems();		assert__( selected_items.size() <= 1 );
	if ( selected_items.empty() )
		return nullptr;

	return selected_items.at( 0 )->data( 0, Qt::UserRole ).value<const CSubLayerIdentifier*>();
}


// override exec() instead of using showEvent()
// because in some case we don't want the dialog to appear
// 
int CSublayersDialog::exec()
{
	// make sure three are sub-layers to choose
	//
	if ( mLayersTable->topLevelItemCount() == 0 )
	{
		SimpleWarnBox( "No layers were found with polygon geometry types." );
		return base_t::Rejected;
	}

	return base_t::exec();
}


//virtual
void CSublayersDialog::accept()
{
	auto items = mLayersTable->selectedItems();
	if ( items.size() <= 0 )
		SimpleErrorBox( "You must select a layer from the sub-layers list." );
	else
		base_t::accept();
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_SublayersDialog.cpp"
