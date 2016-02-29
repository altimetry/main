#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/ApplicationSettings.h"

#include "DisplayWidgets/TextWidget.h"

#include "TabbedDock.h"


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
	mTabWidget->setMovable( true );

	std::vector< QObject* > v;
	if ( top_widget )
		v = { top_widget, mTabWidget };
	else
		v = { mTabWidget };

	LayoutWidgets( Qt::Vertical, v, mDockContents, Spacing, ContentsMargin, ContentsMargin, ContentsMargin, ContentsMargin );

	setWidget( mDockContents );
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


QWidget* CTabbedDock::AddTab( QWidget *tab_widget, const QString &title )
{
	QWidget *tab = new QWidget();
	SetObjectName( tab, "tab" );

	LayoutWidgets( Qt::Vertical, { tab_widget }, tab, Spacing, ContentsMargin, ContentsMargin, ContentsMargin, ContentsMargin );

	mTabWidget->addTab( tab, title );

	assert__( Tab( TabIndex( tab ) ) == tab );

	return tab;
}


QWidget* CTabbedDock::TabWidget( int index )
{
	QWidget *tab = Tab( index );					assert__( tab && tab->layout() );
	return tab->layout()->itemAt( 0 )->widget();
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_TabbedDock.cpp"
