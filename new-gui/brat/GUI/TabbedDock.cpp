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

#include "new-gui/Common/ApplicationSettings.h"

#include "new-gui/Common/GUI/TextWidget.h"

#include "TabbedDock.h"


CExpandableTabWidget::CExpandableTabWidget( QWidget *parent )		//parent = nullptr 
	: QTabWidget( parent )
{
	tabBar()->setExpanding( true );
}




static const int ContentsMargin = 1;
static const int Spacing = 6;


void CTabbedDock::CreateContents( QWidget *top_widget )	//top_widget = nullptr 
{
	//note that docks already have layouts and Qt warns about that

	setObjectName( QString::fromUtf8( "mMainOutputDock" ) );
	setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea );

	mDockContents = new QWidget();
	mDockContents->setObjectName( QString::fromUtf8( "mDockContents" ) );

	mTabWidget = new CExpandableTabWidget( mDockContents );
	mTabWidget->setObjectName( QString::fromUtf8( "mTabWidget" ) );
	mTabWidget->setMovable( false );

	std::vector< QObject* > v;
	if ( top_widget )
		v = { top_widget, mTabWidget };
	else
		v = { mTabWidget };

	LayoutWidgets( Qt::Vertical, v, mDockContents, Spacing, ContentsMargin, ContentsMargin, ContentsMargin, ContentsMargin );

	setWidget( mDockContents );

	//Wire

	connect( mTabWidget, SIGNAL( currentChanged( int ) ), this, SIGNAL( TabSelected( int ) ), Qt::QueuedConnection );
}

CTabbedDock::CTabbedDock( QWidget *top_widget, const QString &title, QWidget *parent, Qt::WindowFlags f )		//parent = nullptr, Qt::WindowFlags f = 0 
    : base_t( title, parent, f )
{
	CreateContents( top_widget );
}

CTabbedDock::CTabbedDock( const QString &title, QWidget *parent, Qt::WindowFlags f )		//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( title, parent, f )
{
	CreateContents();
}


void CTabbedDock::SetTabShape( QTabWidget::TabShape s )
{
	mTabWidget->setTabShape( s );
}


void CTabbedDock::SetTabPosition( QTabWidget::TabPosition p )
{
	mTabWidget->setTabPosition( p );
}


QWidget* CTabbedDock::AddTab( QWidget *tab_widget, const QString &title )
{
	mTabWidget->addTab( tab_widget, title );	assert__( Tab( TabIndex( tab_widget ) ) == tab_widget );

	return tab_widget;
}


QWidget* CTabbedDock::TabWidget( int index )
{
	QWidget *tab = Tab( index );	assert__( tab );
	return tab;
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_TabbedDock.cpp"
