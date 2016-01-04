#include "new-gui/brat/stdafx.h"

#include "TabbedDock.h"
#include "new-gui/brat/Views/TextWidget.h"
#include "new-gui/brat/ApplicationSettings.h"


static int ContentsMargin = 1;		//11

//explicit 
CTabbedDock::CTabbedDock( const QString &title, QWidget *parent, Qt::WindowFlags f )		//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( title, parent, f )
{
	//setAttribute( Qt::WA_DeleteOnClose );

	//note that docks already have layouts and Qt warns about that

	setObjectName( QString::fromUtf8( "mMainOutputDock" ) );
	setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea );

	mDockContents = new QWidget();
	mDockContents->setObjectName( QString::fromUtf8( "mDockContents" ) );
	QHBoxLayout *main_hl = new QHBoxLayout( mDockContents );
	main_hl->setSpacing( 6 );
	main_hl->setContentsMargins( ContentsMargin, ContentsMargin, ContentsMargin, ContentsMargin );
	main_hl->setObjectName( QString::fromUtf8( "main_hl" ) );

	mTabWidget = new QTabWidget( mDockContents );
	mTabWidget->setObjectName( QString::fromUtf8( "mTabWidget" ) );
	mTabWidget->setMovable( true );
	main_hl->addWidget( mTabWidget );

	setWidget( mDockContents );
}


QWidget* CTabbedDock::AddTab( QWidget *tab_widget, const QString &title )
{
	static int n = 0;

	const QString sindex = n2s<std::string>( ++n ).c_str();

	QWidget *tab = new QWidget();
	tab->setObjectName( QString::fromUtf8( "tab" ) + sindex );
	QVBoxLayout *vl = new QVBoxLayout( tab );
	vl->setSpacing( 6 );
	vl->setContentsMargins( ContentsMargin, ContentsMargin, ContentsMargin, ContentsMargin );
	vl->setObjectName( QString::fromUtf8( "tab_layout" ) + sindex );
	tab_widget->setParent( tab );
	vl->addWidget( tab_widget );

	mTabWidget->addTab( tab, title );

	assert__( Tab( TabIndex( tab ) ) == tab );

	return tab;
}


bool CTabbedDock::ReadSettings( const std::string &group )
{
	int index;
	if ( AppSettingsReadValues< int >( group,
	{
		{ KEY_TABBED_DOCK_INDEX, index },
	}
	) )
	{
		if (index >= 0 )
			mTabWidget->setCurrentIndex( index );
	}

	return AppSettingsStatus() == QSettings::NoError;
}

bool CTabbedDock::WriteSettings( const std::string &group )
{
	if (
		!AppSettingsWriteValues< int >( group,
		{
			{ KEY_TABBED_DOCK_INDEX, mTabWidget->currentIndex() },
		} ) 
		)
		std::cout << "Unable to save CTabbedDock index." << std::endl;	// TODO: log this

	return AppSettingsStatus() == QSettings::NoError;
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_TabbedDock.cpp"
