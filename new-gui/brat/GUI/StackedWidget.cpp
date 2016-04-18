#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtils.h"

#include "StackedWidget.h"

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




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_StackedWidget.cpp"
