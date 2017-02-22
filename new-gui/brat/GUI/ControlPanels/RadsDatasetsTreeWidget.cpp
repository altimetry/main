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
#include "DataModels/Workspaces/RadsDataset.h"
#include "DataModels/Workspaces/Workspace.h"
#include "ApplicationPaths.h"

#include "RadsDatasetsTreeWidget.h"


Q_DECLARE_METATYPE( CRadsDataset* )

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CRadsDatasetsTreeWidget Items Class
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////




CRadsDatasetsTreeWidgetItem::CRadsDatasetsTreeWidgetItem( const std::vector< CRadsMission > &missions, CRadsDataset *dataset, QTreeWidget *view )
	: qobject_base_t( view )
	, base_t( view )
	, mAllmissions( missions )
{
	assert__( view );

	view->blockSignals( true );

	// I. Datasets column

	const QString dataset_name = dataset->GetName().c_str();

	setText( eName, dataset_name );
	setToolTip( eName, dataset_name );
	setIcon( eName, QIcon( ":/images/OSGeo/dataset.png" ) );
	setFlags( flags() | Qt::ItemIsEditable );
	setData( eName, Qt::UserRole, QVariant::fromValue( dataset ) );

	view->addTopLevelItem( this );		//this is necessary before what follows


	// II. Missions column


	// II. 1. Missions combo box

	mCombo = new QComboBox;

	// ...fill with missions

	for ( auto const &mission : mAllmissions )
	{
		mCombo->addItem( mission.mName.c_str() );
	}
	UpdateMission();


	// ...aesthetics

	QString sheet =
		"\
		QComboBox,										\
		QLineEdit,										\
		QListView {										\
			selection-background-color: rgb(0, 0, 0);	\
			selection-color: white;						\
			border: 1px black;							\
			border-radius: 5px;							\
			padding: 2px 2px;							\
			font: bold;									\
			}											\
		QComboBox::drop-down {							\
			height: 12px;								\
			subcontrol-origin: padding;					\
			subcontrol-position: center right;			\
			background: none;							\
		}												\
		";
//width: 11px;								\

	mCombo->setStyleSheet( sheet );
	mCombo->setSizeAdjustPolicy( QComboBox::AdjustToContents );
	mCombo->view()->setMinimumHeight( missions.size() * ( mCombo->fontMetrics().lineSpacing() + 1 ) );

	//...align center
	mCombo->setEditable(true);				//otherwise there is no edit widget
	mCombo->lineEdit()->setReadOnly(true);
	mCombo->lineEdit()->setAlignment( Qt::AlignCenter );
	for( int i = 0; i < mCombo->count(); i++ )
		mCombo->setItemData( i, Qt::AlignCenter, Qt::TextAlignmentRole );

	//...connect embedded combo signal
	connect( mCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( currentIndexChanged( int ) ) );

	//...allow parent view to ignore wheel events
	mCombo->installEventFilter( view );
	mCombo->setFocusPolicy( Qt::StrongFocus );
	mCombo->lineEdit()->installEventFilter( view );


	// II. 2. Put missions combo in column 1

	mComboFrame = new QFrame;
	LayoutWidgets( Qt::Horizontal, { mCombo }, mComboFrame, 4, 4, 4, 4, 4 );
	view->setItemWidget( this, eMissions, mComboFrame );
	SetMissionToolTip();

	view->blockSignals( false );
}


const CRadsDataset* CRadsDatasetsTreeWidgetItem::Dataset()
{
	return data( eName, Qt::UserRole ).value< CRadsDataset* >();
}


void CRadsDatasetsTreeWidgetItem::UpdateMission()
{
	auto const &selected_user_missions = Dataset()->Missions();	//only one...

	//if/when implemented more than 1 mission, selected_index must be turned into vector

	int selected_index = selected_user_missions.size() == 0 ? -1 : mCombo->findText( selected_user_missions[ 0 ].mName.c_str() );
	mCombo->setCurrentIndex( selected_index );

	UpdatePhases();
}

void CRadsDatasetsTreeWidgetItem::UpdatePhases()
{
	const CRadsDataset *rads_dataset = Dataset();					assert__( rads_dataset);
	auto const &selected_user_missions = rads_dataset->Missions();	//only one...
    std::string phases;
    for ( auto const &mission : selected_user_missions )
	{
		phases += Vector2String( mission.mPhases );	//if there really was more than 1 mission, the mission name should be added to each phases substring
		setText( ePhases, phases.c_str() );
	}

	setTextAlignment( ePhases, Qt::AlignCenter );
}


//virtual 
bool CRadsDatasetsTreeWidgetItem::operator< ( const QTreeWidgetItem &o ) const
{
	if ( treeWidget()->sortColumn() == eName )
		return base_t::operator< ( o );

	if ( treeWidget()->sortColumn() == ePhases )
		return false;

	const auto &rads_o = dynamic_cast< const CRadsDatasetsTreeWidgetItem& >( o );

	const QString &s1 = mCombo->currentText();
	const QString &s2 = rads_o.mCombo->currentText();

	return s1 < s2;
}


void CRadsDatasetsTreeWidgetItem::SetMissionToolTip()
{
	QString tip;
	const CRadsDataset *rads_dataset = Dataset();					assert__( rads_dataset);
	if ( rads_dataset )
	{
		const QString mission_name = CurrentMission();
		if ( rads_dataset->HasMission( q2a( mission_name ) ) )
		{
			if ( rads_dataset->IsEmpty() )
			{
				tip = "No files found for the selected phases of mission " + mission_name;
			}
			else
			{
				tip = n2q( rads_dataset->Size() ) + " files found for the selected phases of mission " + mission_name;
			}
		}
	}
	SetToolTip( eMissions, tip );
}


void CRadsDatasetsTreeWidgetItem::SetToolTip( int column, const QString &atoolTip )
{
	setToolTip( column, atoolTip );
	setData( column, Qt::ToolTipRole, atoolTip );

	if ( column == eMissions )
	{
		mCombo->setToolTip( atoolTip );
		mCombo->lineEdit()->setToolTip( atoolTip );
		mComboFrame->setToolTip( atoolTip );
	}
}


//slot
void CRadsDatasetsTreeWidgetItem::currentIndexChanged( int index )
{
	// Cannot SetMissionToolTip here, because only answering this signal
	//	will the dataset be assigned with the mission

	emit itemChanged( this, index );
}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CRadsDatasetsTreeWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////



// Assumes a checked and valid all_available_missions
//
CRadsDatasetsTreeWidget::CRadsDatasetsTreeWidget( const std::vector< CRadsMission > &all_available_missions, QWidget *parent )	//parent = nullptr 
	: base_t( parent )
	, mAllAvailableMissions( all_available_missions )
{
    setHeaderLabels( QStringList() << tr("Dataset") << tr("Mission") << tr("Phases") );
    setHeaderHidden( false );
	header()->setStretchLastSection( true );
	header()->setSectionResizeMode( QHeaderView::Interactive );
	header()->setDefaultAlignment( Qt::AlignCenter );
	header()->setSectionsMovable( false );

	header()->viewport()->installEventFilter( this );


	// Do not leave left space for expandable nodes, items occupy the whole row length
	//	which is what we want here: highest level items have no children. If they had,
	//	without node icons could only be expanded/collapsed by double-clicking,
	//
	setRootIsDecorated( false );
	setDragEnabled( false );

	setSortingEnabled( true );

	mSortAscending = AddMenuAction( "Sort &Ascending" );
	mSortDescending = AddMenuAction( "Sort &Descending" );

    connect( mSortAscending,		SIGNAL(triggered()),	this, SLOT( HandleSortAscending() ) );
    connect( mSortDescending,		SIGNAL(triggered()),	this, SLOT( HandleSortDescending() ) );
}


QTreeWidgetItem* CRadsDatasetsTreeWidget::AddDatasetToTree( const QString &dataset_name )
{
	assert__( mWDataset );

	CRadsDataset *wkspcd = mWDataset->GetDataset< CRadsDataset >( q2a( dataset_name ) );			assert__( wkspcd );

	CRadsDatasetsTreeWidgetItem *dataset_item = new CRadsDatasetsTreeWidgetItem( mAllAvailableMissions, wkspcd, this );
	connect( dataset_item, &CRadsDatasetsTreeWidgetItem::itemChanged, this, &CRadsDatasetsTreeWidget::currentIndexChanged );	//specialized RADS tree item signal, from missions combo
    
	resizeColumnToContents( 0 );

	return dataset_item;
}


// Tree item events to be processed by the container tree
//
bool CRadsDatasetsTreeWidget::eventFilter( QObject *o, QEvent *e ) 
{
	if ( qobject_cast< QComboBox* >( o ) )
	{
		switch( e->type() )
		{
			// Prevent mouse wheel on combo

			case QEvent::Wheel:
				e->ignore();
				return true;
				break;

			default:            
				return false;
				break;
		}
	}
	else
	if ( qobject_cast< QLineEdit* >( o ) )
	{
		switch( e->type() )
		{
			// Prevent arrow keys on combo line edit

			case QEvent::KeyPress:
			case QEvent::KeyRelease:
				e->ignore();
				return true;
				break;

			// Allow selection by clicking in combo line edit

			case QEvent::MouseButtonPress:
			case QEvent::MouseButtonRelease:
				e->ignore();
				return true;
				break;

			default:            
				return false;
				break;
		}
	}
	else
	if ( header()->viewport() == o )
	{
		QMouseEvent *me = dynamic_cast<QMouseEvent*>( e );
		if ( me )
		{
			// Prevent phases column header mouse clicks

			if ( e->type() != QEvent::MouseMove && header()->logicalIndexAt( me->pos() ) == CRadsDatasetsTreeWidgetItem::ePhases )
			{
				e->ignore();
				return true;
			}
		}
	}

	return base_t::eventFilter( o, e );
}



void CRadsDatasetsTreeWidget::HandleSortAscending()
{
	sortByColumn( 0, Qt::AscendingOrder );
}


void CRadsDatasetsTreeWidget::HandleSortDescending()
{
	sortByColumn( 0, Qt::DescendingOrder );
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_RadsDatasetsTreeWidget.cpp"
