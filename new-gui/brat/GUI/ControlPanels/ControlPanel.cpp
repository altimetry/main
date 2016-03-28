#include "new-gui/brat/stdafx.h"

#include "libbrathl/TreeField.h"
#include "libbrathl/Field.h"

#include "ApplicationLogger.h"

#include "GUI/DisplayWidgets/TextWidget.h"
#include "GUI/DisplayWidgets/2DPlotWidget.h"
#include "GUI/DisplayWidgets/3DPlotWidget.h"

#include "ControlPanel.h"


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//								Specialized Widgets
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

CStackedWidget::CStackedWidget( QWidget *parent, const std::vector< PageInfo > &widgets, int iselected )		//iselected = 0 
	: base_t( parent )
{
    assert__( (size_t)iselected < widgets.size() );

	SetObjectName( this, "StackedWidget" );

	mGroup = new QButtonGroup( parent );

	for ( auto &w : widgets )
	{
		addWidget( w.mWidget );
		QAbstractButton *b = nullptr;
		if ( w.mUseToolButtons )
		{
			b = CreateToolButton( w.mName, w.mIconPath, w.mTip );
		}
		else
		{
			b = CreatePushButton( w.mName, w.mIconPath, w.mTip );
		}
		b->setParent( parent );
		b->setCheckable( true );
        connect( b, SIGNAL( toggled( bool ) ), this, SLOT( buttonToggled( bool ) ) );
		mGroup->addButton( b );
	}

    SetCurrentIndex( iselected );
}



void CStackedWidget::SetCurrentIndex( int index )
{
    Button( index )->setChecked( true );
}


// slot
void CStackedWidget::buttonToggled( bool checked )
{
    QAbstractButton *b = qobject_cast< QAbstractButton* >( sender() );      assert__( b );
    if ( checked )
    {
        auto index = mGroup->buttons().indexOf( b );
        setCurrentIndex( index );
        emit PageChanged( index );
    }
}







/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//							Control Panels Base Class
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

//explicit 
CControlPanel::CControlPanel( QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( parent, f )
{
	mMainLayout = static_cast< decltype( mMainLayout ) >( CreateLayout( this, Qt::Vertical, smSpacing, smLeft, smTop, smRight, smBottom ) );
}


QListWidget* CControlPanel::CreateBooleanList( QWidget *parent, std::initializer_list< BooleanListItemInfo > il )
{
	QListWidget *l = new QListWidget( parent );
	for ( auto &i : il )
	{
		QListWidgetItem* item = new QListWidgetItem( t2q( i.mName ), l );
		item->setFlags( item->flags() | Qt::ItemIsUserCheckable );		
		item->setCheckState( i.mChecked ? Qt::Checked : Qt::Unchecked );
	}
	return l;
}


QObject* CControlPanel::AddTopWidget( QObject *ob )
{
	auto w = qobject_cast<QWidget*>( ob );
	auto l = qobject_cast<QLayout*>( ob );		assert__( w || l );
	if ( w )
		mMainLayout->addWidget( w );
	else
		mMainLayout->addLayout( l );

	return ob;
}


//	spacing = smSpacing, left = smLeft, top = smTop, right = smRight, bottom = smBottom
//
QLayout* CControlPanel::AddTopLayout( ELayoutType o, const std::vector< QObject* > &v, int spacing, int left, int top, int right, int bottom )		 
{
	QLayout *l = nullptr;
	switch ( o )
	{
		case ELayoutType::Horizontal:
		case ELayoutType::Vertical:
			l = LayoutWidgets( static_cast< Qt::Orientation >( o ), v, nullptr, spacing, left, top, right, bottom  );
			break;
		case ELayoutType::Grid:
			l = LayoutWidgets( v, nullptr, spacing, left, top, right, bottom  );
			break;
		default:
			assert__( false );
	}	
	AddTopWidget( l );

	return l;
}


//	title = "", parent = nullptr, spacing = smSpacing, left = smLeft, top = smTop, right 00000= smRight, bottom = smBottom
//
QGroupBox* CControlPanel::AddTopGroupBox( ELayoutType o, const std::vector< QObject* > &v, const QString &title, 
	int spacing, int left, int top, int right, int bottom )		 
{
	QGroupBox *group = CreateGroupBox( o, v, title, this, spacing, left, top, right, bottom  );
	AddTopWidget( group );
	return group;
}


QSpacerItem* CControlPanel::AddTopSpace( int w, int h, QSizePolicy::Policy hData, QSizePolicy::Policy vData )
{
    auto spacer = CreateSpace( w, h, hData, vData );
    mMainLayout->addItem( spacer );
    return spacer;
}


QSplitter* CControlPanel::AddTopSplitter( Qt::Orientation o, const std::vector< QWidget* > &v, bool collapsible, const QList< int > sizes )	//collapsible = false, const QList< int > sizes 
{
	QSplitter *splitter = CreateSplitter( this, o, v, collapsible, sizes );
	AddTopWidget( splitter );
	return splitter;
}






/////////////////////////////////////////////////////////////////////////////////////
//					Control Panel Specialized for Desktop
/////////////////////////////////////////////////////////////////////////////////////



//explicit 
CDesktopControlsPanel::CDesktopControlsPanel( CModel &model, CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( parent, f ), mModel( model ), mDesktopManager( manager )
{}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ControlPanel.cpp"
