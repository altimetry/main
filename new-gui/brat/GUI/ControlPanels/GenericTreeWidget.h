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
#if !defined GUI_CONTROL_PANELS_GENERIC_TREE_WIDGET_H
#define GUI_CONTROL_PANELS_GENERIC_TREE_WIDGET_H



class CTextWidget;



class CGenericTreeWidget : public QTreeWidget
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// types

	using base_t = QTreeWidget;

protected:

	// static members

	static QTreeWidgetItem* SetItemBold( QTreeWidgetItem *item, bool bold );	


	// instance variables

private:

	QIcon mGroupIcon;
    QIcon mKeyIcon;

	QMenu *mContextMenu = nullptr;

	//...domain data


	//construction / destruction


public:
	CGenericTreeWidget( QWidget *parent, bool default_icons = true );

	virtual ~CGenericTreeWidget()
	{}


	//remaining methods

	QTreeWidgetItem* SelectedItem()
	{
		auto items = selectedItems();		assert__( selectionMode() != SingleSelection || items.size() <= 1 );
		if ( items.size() > 0 )
			return items[ 0 ];

		return nullptr;
	}

	// can return null
	//
	const QTreeWidgetItem* SelectedItem() const
	{
		return const_cast<CGenericTreeWidget*>( this )->SelectedItem();
	}


	//remaining methods for bases classes

protected:

	template< typename FUNCTION >
    QTreeWidgetItem* FindItem( FUNCTION f );


	template< typename DATA >
	QTreeWidgetItem* MakeRootItem( const std::string &name, DATA data = nullptr, bool bold = true );

	template< typename PARENT, typename DATA >
	QTreeWidgetItem* MakeItem( PARENT *parent, const std::string &name, DATA data = nullptr, bool bold = false, bool folder = false );	

	template< typename PARENT, typename DATA >
	QTreeWidgetItem* MakeEditableLeaf( PARENT *parent, const std::string &name, DATA data = nullptr, bool bold = false );	


	template< typename DATA >
	DATA ItemData( QTreeWidgetItem *item )
	{
		return item->data( 0, Qt::UserRole ).value< DATA >();
	}

	template< typename DATA >
	void SetItemData( QTreeWidgetItem *item, DATA *data )
	{
		if ( !data )
			return;

		item->setData( 0, Qt::UserRole, QVariant::fromValue( *data ) );
	}


	// context menu

	virtual void CustomContextMenu( QTreeWidgetItem *item ) = 0;

	QAction* AddMenuAction( const std::string &name );

	QAction* AddMenuSeparator();

	void SetMenuTitle( const std::string &title )
	{
		mContextMenu->setTitle( title.c_str() );
	}

protected slots:

	void HandleCustomContextMenu( const QPoint &point )
	{
		QModelIndex index = indexAt( point );
		if ( index.isValid() )				// && index.row() % 2 == 0
		{
			CustomContextMenu( itemFromIndex( index ) );
			mContextMenu->exec( mapToGlobal( point ) );
		}
	}
};






template< typename PARENT, typename DATA >
QTreeWidgetItem* CGenericTreeWidget::MakeItem( PARENT *parent, const std::string &name, DATA data, bool bold, bool folder )		//data = nullptr, bold = false, bool folder = false )
{
	QTreeWidgetItem *item = new QTreeWidgetItem( parent );
	item->setText( 0, name.c_str() );
	if ( bold )
		SetItemBold( item, bold );
	SetItemData( item, &data );
	item->setIcon( 0, folder ? mGroupIcon : mKeyIcon );
	item->setExpanded( folder );
	return item;
}


template< typename PARENT, typename DATA >
QTreeWidgetItem* CGenericTreeWidget::MakeEditableLeaf( PARENT *parent, const std::string &name, DATA data, bool bold )		//data = nullptr, bool bold = false 
{
	auto item = MakeItem( parent, name, data, bold, false );
	item->setFlags( item->flags() | Qt::ItemIsEditable );
	return item;
}


template< typename DATA >
QTreeWidgetItem* CGenericTreeWidget::MakeRootItem( const std::string &name, DATA data, bool bold )	//data = nullptr, bool bold = true );
{
	return MakeItem( this, name, data, bold, true );
}


template< typename FUNCTION >
QTreeWidgetItem* CGenericTreeWidget::FindItem( FUNCTION f )
{
	//TODO code for a future templated find item with predicate

	QTreeWidgetItemIterator it( this );
	while ( *it )
	{
		if ( f( *it ) )
			return *it;
		++it;
	}

	return nullptr;
}



#endif	//GUI_CONTROL_PANELS_GENERIC_TREE_WIDGET_H
