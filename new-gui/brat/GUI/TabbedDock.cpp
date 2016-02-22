#include "new-gui/brat/stdafx.h"

#include "new-gui/brat/Views/TextWidget.h"
#include "new-gui/Common/ApplicationSettings.h"

#include "TabbedDock.h"


static int ContentsMargin = 1;		//11


void CTabbedDock::CreateContents( QWidget *top_widget )	//top_widget = nullptr 
{
	//note that docks already have layouts and Qt warns about that

	setObjectName( QString::fromUtf8( "mMainOutputDock" ) );
	setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea );

	mDockContents = new QWidget();
	mDockContents->setObjectName( QString::fromUtf8( "mDockContents" ) );
	//QHBoxLayout *main_hl = new QHBoxLayout( mDockContents );
	//main_hl->setSpacing( 6 );
	//main_hl->setContentsMargins( ContentsMargin, ContentsMargin, ContentsMargin, ContentsMargin );
	//main_hl->setObjectName( QString::fromUtf8( "main_hl" ) );

	mTabWidget = new CExpandableTabWidget( mDockContents );
	mTabWidget->setObjectName( QString::fromUtf8( "mTabWidget" ) );
	mTabWidget->setMovable( true );
	//main_hl->addWidget( mTabWidget );

	std::vector< QObject* > v;
	if ( top_widget )
		v = { top_widget, mTabWidget };
	else
		v = { mTabWidget };

	LayoutWidgets( Qt::Vertical, v, mDockContents, 6, 2, 2, 2, 2 );

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


QWidget* CTabbedDock::TabWidget( int index )
{
	QWidget *tab = Tab( index );					assert__( tab && tab->layout() );
	return tab->layout()->itemAt( 0 )->widget();
}


bool CTabbedDock::ReadSettings( const std::string &group )
{
    Q_UNUSED( group );

    return AppSettingsStatus() == QSettings::NoError;
}

bool CTabbedDock::WriteSettings( const std::string &group )
{
    Q_UNUSED( group );

	return AppSettingsStatus() == QSettings::NoError;
}




//////////////////////////////////////////////////////////////////////////
//						Workspace Tabbed Dock
//////////////////////////////////////////////////////////////////////////

QWidget* CWorkspaceTabbedDock::CreateTopFrame()
{
	//WARNING:  don't use "this" here (it is called before base invocation)

	auto ldatasets = new QLabel( "Datasets" );		ldatasets->setAlignment( Qt::AlignCenter );
	auto lfilters = new QLabel("Filters ");			lfilters->setAlignment( Qt::AlignCenter );
	auto loperations = new QLabel("Operations");	loperations->setAlignment( Qt::AlignCenter );
	auto datasets = new QComboBox;
	auto filters = new QComboBox;
	auto operations = new QComboBox;

	QFrame *frame = new QFrame;
	frame->setWindowTitle( "QFrame::Box" );
	frame->setFrameStyle( QFrame::Panel );
	frame->setFrameShadow( QFrame::Sunken );
	frame->setObjectName("OperationsFrame");
	frame->setStyleSheet("#OperationsFrame { border: 1px solid black; }");
	//LayoutWidgets( Qt::Vertical, { all_grid_l, mOperationName, op_hl }, frame, s, m, m, m, m );
    LayoutWidgets( { ldatasets, lfilters, loperations, nullptr, datasets, filters, operations }, frame, 6, 2, 2, 2, 2 );

	return frame;
}


//explicit 
CWorkspaceTabbedDock::CWorkspaceTabbedDock( const QString &title, QWidget *parent, Qt::WindowFlags f )		//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( CreateTopFrame(), title, parent, f )
{}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_TabbedDock.cpp"
