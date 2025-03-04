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

#include "ShowAliasesDialog.h"


QTableWidgetItem* SetItemBold( QTableWidgetItem *item, bool bold )
{
	QFont font = item->font();
	font.setBold( bold );
	item->setFont( font );
	return item;
}


QWidget* CShowAliasesDialog::MakeSelectCell()
{
	QWidget *w = new QWidget;
	QHBoxLayout *l = new QHBoxLayout( w );
	mSelected.push_back( new QCheckBox );
	l->addWidget( mSelected.back() );
	l->setAlignment( Qt::AlignCenter );
	w->setLayout( l );
	return w;
}



void CShowAliasesDialog::CreateWidgets()
{
    //	Create

	mAliasesTable = new QTableWidget( this );
	mAliasesTable->setColumnCount( EColums_size );
	mAliasesTable->setSelectionBehavior( QAbstractItemView::SelectRows );

	mAliasesTable->setHorizontalHeaderItem( eValue, new QTableWidgetItem( "Value" ) );
	SetItemBold( mAliasesTable->horizontalHeaderItem( eValue ), true );

	mAliasesTable->setHorizontalHeaderItem( eSyntax, new QTableWidgetItem( "Syntax" ) );
	SetItemBold( mAliasesTable->horizontalHeaderItem( eSyntax ), true );

	mAliasesTable->setHorizontalHeaderItem( eDescription, new QTableWidgetItem( "Description" ) );
	SetItemBold( mAliasesTable->horizontalHeaderItem( eDescription ), true );

	mAliasesTable->setHorizontalHeaderItem( eSelect, new QTableWidgetItem( "Select (*)" ) );
	SetItemBold( mAliasesTable->horizontalHeaderItem( eSelect ), true );

	auto *table_l = LayoutWidgets( Qt::Vertical, { mAliasesTable }, nullptr, 2, 2, 2, 2, 2 );
	table_l->setAlignment( Qt::AlignCenter );

	mHeaderLabel = new QLabel;
	mFooterLabel = new QLabel( "(*) Check the alias you want to add to the current expression.\nSelected aliases will be added when the window is closed." );
	mHeaderLabel->setAlignment( Qt::AlignCenter );
	mFooterLabel->setAlignment( Qt::AlignCenter );

    //	... Help

    auto help = new CTextWidget;
    help->SetHelpProperties(
        "Provides information about the aliases available for the chosen dataset. Aliases are\n"
        " equivalents that you can use instead of the fields name."
         ,0 , 6 );
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
				mHeaderLabel,
				table_l,
				mFooterLabel,
				help_group,
				mButtonBox
			}, 
			this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "Show 'PRODUCT_NAME' aliases...");

    //	Wrap up dimensions

    Wire();
}


void  CShowAliasesDialog::Wire()
{
    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}


CShowAliasesDialog::CShowAliasesDialog( QWidget *parent, const COperation *operation, CFormula *formula )
	: base_t( parent )
{
	CreateWidgets();

	CProductInfo pi( operation->OriginalDataset() );	assert__( pi.IsValid() );	//assuming that dialog is called only when there are fields and so the product was previously open

	std::string class_and_type = pi.ProductClassAndType();

	setWindowTitle( ( "Show '" + class_and_type + "' aliases..." ).c_str() );

	mHeaderLabel->setText( QString( "The list below shows the available aliases for the product\n" ) + class_and_type.c_str() );

	CStringArray aliasesArray;
	pi.AliasKeys( aliasesArray );
	std::sort( aliasesArray.begin(), aliasesArray.end(), CTools::StringCompare );

	bool hasOpFieldSelected = formula != nullptr;

	mAliasesTable->setColumnHidden( eSelect, !hasOpFieldSelected );
	mFooterLabel->setVisible( hasOpFieldSelected );

	size_t size = aliasesArray.size();
	mAliasesTable->setRowCount( (int)size );

	for ( int i = 0; i < (int)size; i++ )
	{
		std::string name = aliasesArray.at( i );

		mAliasesTable->setVerticalHeaderItem( i, new QTableWidgetItem( name.c_str() ) );
		SetItemBold( mAliasesTable->verticalHeaderItem( i ), true );

		mAliasesTable->setItem( i, eValue, new QTableWidgetItem( pi.AliasExpandedValue( name ).c_str() ) );
		mAliasesTable->item( i, eValue )->setFlags( mAliasesTable->item( i, eValue )->flags() & ~Qt::ItemIsEditable );

		mAliasesTable->setItem( i, eSyntax, new QTableWidgetItem( std::string( "%{" + name + "}").c_str() ) );
		mAliasesTable->item( i, eSyntax )->setFlags( mAliasesTable->item( i, eSyntax )->flags() & ~Qt::ItemIsEditable );

		const CProductAlias* product_alias = pi.Alias( name );
		if ( product_alias != nullptr )
		{
			mAliasesTable->setItem( i, eDescription, new QTableWidgetItem( product_alias->GetDescription().c_str() ) );
			mAliasesTable->item( i, eDescription )->setFlags( mAliasesTable->item( i, eDescription )->flags() & ~Qt::ItemIsEditable );
		}

		if ( hasOpFieldSelected )
		{
			mAliasesTable->setItem( i, eSelect, new QTableWidgetItem( "" ) );
			mAliasesTable->setCellWidget( i, eSelect, MakeSelectCell() );
		}
	}

    //mAliasesTable->resizeColumnToContents( eSyntax );
    mAliasesTable->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

    setMinimumWidth( width() );
    adjustSize();
}


CShowAliasesDialog::~CShowAliasesDialog()
{}



//virtual
void  CShowAliasesDialog::accept()
{
    base_t::accept();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ShowAliasesDialog.cpp"
