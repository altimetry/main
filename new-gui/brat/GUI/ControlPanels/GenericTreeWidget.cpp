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

#include "GenericTreeWidget.h"



static QIcon DefaultItemIcon()
{
	QTreeWidgetItem item;
	return item.icon( 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						CGenericTreeWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

CGenericTreeWidget::CGenericTreeWidget( QWidget *parent, bool default_icons )	//default_icons = true
{
    Q_UNUSED( parent );

	SetObjectName( this, "FieldsTree" );

	if ( default_icons )
	{
		mGroupIcon = DefaultItemIcon();
		mKeyIcon = DefaultItemIcon();
	}
	else 
	{
		mGroupIcon.addPixmap( style()->standardPixmap( QStyle::SP_DirClosedIcon ), QIcon::Normal, QIcon::Off );
		mGroupIcon.addPixmap( style()->standardPixmap( QStyle::SP_DirOpenIcon ), QIcon::Normal, QIcon::On );
		mKeyIcon.addPixmap( style()->standardPixmap( QStyle::SP_FileIcon ) );	//SP_FileIcon
	}


	mContextMenu = new QMenu( this );				//setContextMenuPolicy( Qt::ActionsContextMenu );
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect( this, SIGNAL( customContextMenuRequested( const QPoint & ) ), this, SLOT( HandleCustomContextMenu( const QPoint & ) ) );
}


QTreeWidgetItem* CGenericTreeWidget::SetItemBold( QTreeWidgetItem *item, bool bold )
{
	QFont font = item->font( 0 );
	font.setBold( bold );
	item->setFont( 0, font );
	return item;
}






///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_GenericTreeWidget.cpp"
