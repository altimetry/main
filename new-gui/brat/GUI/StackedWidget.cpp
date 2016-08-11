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
