#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/+Utils.h"
#include "new-gui/Common/QtUtils.h"

#include "DisplayEditor.h"



///////////////////////////////////////////////////////////////////////////////////////////
//	Base
///////////////////////////////////////////////////////////////////////////////////////////

QLayout* CAbstractDisplayEditor::AddWorkingWidget( QWidget *w, QLayout *l )	//l = nullptr 
{
	if ( !l )
		l = mWorkingDockLayout;
	l->addWidget( w );
	return l;
}
QLayout* CAbstractDisplayEditor::CreateWorkingLayout( Qt::Orientation orientation )
{
	QLayout *l = createLayout( nullptr/*mWorkingDock*/, orientation );
	mWorkingDockLayout->addLayout( l );
	return l;
}

void CAbstractDisplayEditor::CreateWorkingDock()
{
	//Unfortunately, it seems it must be hard-coded
	static const int min_width = 200;
	static const int min_height = 100;

	setDockOptions( dockOptions() & ~AnimatedDocks );	//TODO: DELETE AFTER CHECKING WINDOWS BLUE SCREENS

	//dock
	mWorkingDock = new QDockWidget( this );									//note that docks already have layouts and Qt warns about that
    mWorkingDock->setWindowTitle( QString( "Working Panel" ) + "[*]" );
	mWorkingDock->setMinimumSize( min_width, min_height );
	mWorkingDock->setFeatures( mWorkingDock->features() & ~( QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable ) );

	//dock main widget (content container)
    mWorkingDockContent = new QWidget( mWorkingDock );
	mWorkingDockLayout = qobject_cast<QVBoxLayout*>( createLayout( mWorkingDock, Qt::Vertical ) );		assert__( mWorkingDockLayout );
	mWorkingDockContent->setLayout( mWorkingDockLayout );
	
	mWorkingDock->setWidget( mWorkingDockContent );
	addDockWidget( Qt::DockWidgetArea::LeftDockWidgetArea, mWorkingDock );
	mWorkingDock->setVisible( true );

	//Sample widgets: TODO: delete
	AddWorkingWidget( new QPushButton( "Do It", mWorkingDockContent ),
		AddWorkingWidget( new QTextEdit/*QListView*/( mWorkingDockContent ), 
		CreateWorkingLayout( Qt::Horizontal ) ) );

	AddWorkingWidget( new QComboBox( mWorkingDockContent ),
		AddWorkingWidget( new QCheckBox( "Don't do it", mWorkingDockContent ), 
		CreateWorkingLayout( Qt::Horizontal ) ) );
}


void CAbstractDisplayEditor::CreateGraphicsBar()
{
    mToolBar = new QToolBar( this );
    mToolBar->setIconSize( QSize( 32, 32 ) );

	QAction *a = createAction( mWorkingDock, "f", "F tool tip", "://images/alpha-numeric/f.png" );
	mToolBar->addAction( a );
    mToolBar->addSeparator();

	// add button group

	QActionGroup *group = new QActionGroup( this );

	a = createAction( group, "2D", "Display 2D view", "://images/alpha-numeric/2.png", "://images/alpha-numeric/2.png" );
	a->setCheckable( true );
	connect( a, SIGNAL( toggled( bool ) ), this, SLOT( Handle2D_slot( bool ) ) );

	a = createAction( group, "3D", "Display 3D view", "://images/alpha-numeric/3.png", "://images/alpha-numeric/3.png" );
	a->setCheckable( true );
	connect( a, SIGNAL( toggled( bool ) ), this, SLOT( Handle3D_slot( bool ) ) );

	a = createAction( group, "Text", "Display text", "://images/alpha-numeric/1.png", "://images/alpha-numeric/1.png" );
	a->setCheckable( true );
	connect( a, SIGNAL( toggled( bool ) ), this, SLOT( HandleText_slot( bool ) ) );

	a = createAction( group, "Text", "Display text", "://images/alpha-numeric/0.png", "://images/alpha-numeric/0.png" );
	a->setCheckable( true );
	connect( a, SIGNAL( toggled( bool ) ), this, SLOT( HandleLog_slot( bool ) ) );

	mToolBar->addActions( group->actions() );
    mToolBar->addSeparator();

	// add menu button

	const int menu_size = 3;
	QAction *menu_actions[ menu_size ] ={
		new QAction( QPixmap( "://images/alpha-numeric/5.png" ), "five", this ),
		new QAction( QPixmap( "://images/alpha-numeric/6.png" ), "six", this ),
		new QAction( QPixmap( "://images/alpha-numeric/7.png" ), "seven", this )
	};
	mToolBar->addWidget( CreateMenuButton( menu_actions, menu_size ) );

	// add the bar

	mToolBar->setAllowedAreas( Qt::RightToolBarArea );
    addToolBar( Qt::RightToolBarArea, mToolBar );
}

void CAbstractDisplayEditor::CreateStatusBar()
{
}

CAbstractDisplayEditor::CAbstractDisplayEditor( QWidget *parent, QWidget *view )	//parent = nullptr, QWidget *view = nullptr ) 
	: base_t( parent )
{
    setWindowFlags( Qt::Widget );
	setAttribute( Qt::WA_DeleteOnClose );

	CreateWorkingDock();
	CreateGraphicsBar();
	CreateStatusBar();

	if ( view )
		AddView( view );

	//setVisible( true );
}

//virtual 
CAbstractDisplayEditor::~CAbstractDisplayEditor()
{}

void CAbstractDisplayEditor::AddView( QWidget *view )
{
	delete mGraphic;
	mGraphic = view;
	setCentralWidget( mGraphic );
	mGraphic->setParent( this );
}

void CAbstractDisplayEditor::Handle2D_slot( bool checked )
{
	Handle2D( checked );
}
void CAbstractDisplayEditor::Handle3D_slot( bool checked )
{
	Handle3D( checked );
}
void CAbstractDisplayEditor::HandleText_slot( bool checked )
{
	HandleText( checked );
}
void CAbstractDisplayEditor::HandleLog_slot( bool checked )
{
	HandleLog( checked );
}





///////////////////////////////////////////////////////////////////////////////////////////
//	Maps
///////////////////////////////////////////////////////////////////////////////////////////

CMapEditor::CMapEditor( QWidget *parent, QWidget *view )		//parent = nullptr, QWidget *view = nullptr
	: base_t( parent, view )
{}

//virtual 
CMapEditor::~CMapEditor()
{}


//virtual 
void CMapEditor::Handle2D( bool checked )
{
    Q_UNUSED( checked );

	auto v = typeid( &CMapEditor::Handle2D ).name();

    Q_UNUSED( v );

    SimpleMsgBox( "CMapEditor::Handle2D" );
}
//virtual 
void CMapEditor::Handle3D( bool checked )
{
    Q_UNUSED( checked );

    SimpleMsgBox( typeid( &CMapEditor::Handle3D ).name() );
}
//virtual 
void CMapEditor::HandleText( bool checked )
{
    Q_UNUSED( checked );

    SimpleMsgBox( typeid( &CMapEditor::HandleText ).name() );
}
//virtual 
void CMapEditor::HandleLog( bool checked )
{
    Q_UNUSED( checked );

    SimpleMsgBox( typeid( &CMapEditor::HandleLog ).name() );
}



///////////////////////////////////////////////////////////////////////////////////////////
//	Plots
///////////////////////////////////////////////////////////////////////////////////////////

CPlotEditor::CPlotEditor( QWidget *parent, QWidget *view ) 	//parent = nullptr, QWidget *view = nullptr
	: base_t( parent, view )
{}

//virtual 
CPlotEditor::~CPlotEditor()
{}


//virtual 
void CPlotEditor::Handle2D( bool checked )
{
    Q_UNUSED( checked );

    SimpleMsgBox( __func__ );
}
//virtual 
void CPlotEditor::Handle3D( bool checked )
{
    Q_UNUSED( checked );

    SimpleMsgBox( __func__ );
}
//virtual 
void CPlotEditor::HandleText( bool checked )
{
    Q_UNUSED( checked );

    SimpleMsgBox( __func__ );
}
//virtual 
void CPlotEditor::HandleLog( bool checked )
{
    Q_UNUSED( checked );

    SimpleMsgBox( typeid( &CPlotEditor::HandleLog ).name() );
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DisplayEditor.cpp"
