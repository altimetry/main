#include "new-gui/brat/stdafx.h"

#include <qmetatype.h>


#include "libbrathl/Product.h"

#include "new-gui/Common/QtUtils.h"

#include "DataModels/Workspaces/Formula.h"
#include "DataModels/Workspaces/Operation.h"
#include "DataModels/MapTypeDisp.h"

#include "GUI/ActionsTable.h"

#include "DataExpressionsTree.h"



Q_DECLARE_METATYPE( CMapTypeField::ETypeField )

Q_DECLARE_METATYPE( CFormula* )

Q_DECLARE_METATYPE( CField* )


QDataStream& operator << ( QDataStream &out, CField *const &rhs ) 
{
	out.writeRawData( reinterpret_cast<const char*>( &rhs ), sizeof( rhs ) );
	return out;
}

QDataStream& operator >> ( QDataStream &in, CField *&rhs ) 
{
	in.readRawData( reinterpret_cast<char*>( &rhs ), sizeof( rhs ) );
	return in;
}


int RegisterMetaTypeStreamOperators()
{
	qRegisterMetaTypeStreamOperators< CField* >( "CField*" );
	return 0;
}
static int dummy = RegisterMetaTypeStreamOperators();





/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						CAbstractTree
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

CAbstractTree::CAbstractTree( QWidget *parent )
{
    Q_UNUSED( parent );

	SetObjectName( this, "FieldsTree" );

    mGroupIcon.addPixmap( style()->standardPixmap( QStyle::SP_DirClosedIcon ), QIcon::Normal, QIcon::Off );
    mGroupIcon.addPixmap( style()->standardPixmap( QStyle::SP_DirOpenIcon ), QIcon::Normal, QIcon::On );
    mKeyIcon.addPixmap( style()->standardPixmap( QStyle::SP_FileIcon ) );	//SP_FileIcon

	mContextMenu = new QMenu( this );				//setContextMenuPolicy( Qt::ActionsContextMenu );
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect( this, SIGNAL( customContextMenuRequested( const QPoint & ) ), this, SLOT( HandleCustomContextMenu( const QPoint & ) ) );
}


template< typename PARENT, typename DATA >
QTreeWidgetItem* CAbstractTree::MakeItem( PARENT *parent, const std::string &name, DATA data, bool bold, bool folder )		//data = nullptr, bold = false, bool folder = false )
{
	QTreeWidgetItem *item = new QTreeWidgetItem( parent );
	item->setText( 0, name.c_str() );
	if ( bold )
	{
		QFont font = item->font( 0 );
		font.setBold( true );
		item->setFont( 0, font );
	}
	SetItemData( item, &data );
	item->setIcon( 0, folder ? mGroupIcon : mKeyIcon );
	item->setExpanded( folder );
	return item;
}


template< typename DATA >
QTreeWidgetItem* CAbstractTree::MakeRootItem( const std::string &name, DATA data, bool bold )	//data = nullptr, bool bold = true );
{
	return MakeItem( this, name, data, bold, true );
}






/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						CFieldsTree
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

CFieldsTree::CFieldsTree( QWidget *parent )
	: base_t( parent )
{
    QStringList labels;
    labels << tr("Fields");
    setHeaderLabels(labels);

	setRootIsDecorated( false );
	setDragEnabled( true );
	setDragDropMode( DragOnly );
	setToolTip( "Drag fields to the data expressions tree" );

    //wxMenu* item1 = new wxMenu( _("Menu for fields tree") );
    //item1->Append( ID_ASXMENU, _("Set as &X"), _("Set field as X") );
    //item1->Append( ID_ASYMENU, _("Set as &Y"), _("Set field as Y") );
    //item1->Append( ID_ASDATAMENU, _("Set as &Data"), _("Add field as data") );
    //item1->Append( ID_ASSELECTMENU, _("Insert into  Selection &criteria"), _("Insert field into selection criteria") );
    //item1->AppendSeparator();
    //wxMenuItem *item2 = new wxMenuItem( item1, ID_FIELDSSORTMENU, _("Sort &Ascending"), _("Sort branch in ascending order") );
    //item2->SetBitmap( BitmapsList( 1 ) );
    //item1->Append( item2 );
    //wxMenuItem *item3 = new wxMenuItem( item1, ID_FIELDSSORTREVMENU, _("Sort &Descending"), _("Sort branch in ascending order") );
    //item3->SetBitmap( BitmapsList( 0 ) );
    //item1->Append( item3 );
    //item1->AppendSeparator();
    //item1->Append( ID_FIELD_ATTR_CHANGE_UNIT, _("Change &unit"), _("Allow to change the unit of this field (only for netcdf variable attributes)") );
    //item0->Append( item1, _("Menu for fields tree") );
}



CField* CFieldsTree::ItemField( QTreeWidgetItem *item )
{
	return ItemData< CField* >( item );
}


void CFieldsTree::SetItemField( QTreeWidgetItem *item, CField *field )
{
	SetItemData( item, &field );
}


void CFieldsTree::InsertProduct( CProduct *product )
{
	mProduct = product;

	clear();

	if ( mProduct == nullptr )
		return;

	CTreeField* tree = mProduct->GetTreeField();
	if ( tree->GetRoot() == nullptr )
		return;

	QTreeWidgetItem *parent = nullptr;
	tree->SetWalkDownRootPivot();
	do
	{
		parent = InsertField( parent, tree->GetWalkCurrent() );
	} 
	while ( tree->SubTreeWalkDown() );
}


QTreeWidgetItem* CFieldsTree::SetRootItem( CField *field )
{
	auto *item = invisibleRootItem();
	SetItemField( item, field );
	item->setExpanded( true );
	return item;
}


QTreeWidgetItem* CFieldsTree::GetFirstRecordItem()
{
	auto *root = invisibleRootItem();
	return root->childCount() > 0 ? root->child( 0 ) : nullptr;
}

void CFieldsTree::SelectRecord( const std::string &record )		//see COperationPanel::GetOperationRecord
{
	auto items = findItems( record.c_str(), Qt::MatchExactly );
	QTreeWidgetItem *item = items.size() > 0 ? items[ 0 ] : nullptr;
	if ( !item )
	{
		item = GetFirstRecordItem();
	}

	assert__( item );

	setCurrentItem( item );
	item->setExpanded( true );
}


QTreeWidgetItem* CFieldsTree::InsertField( QTreeWidgetItem *parent, CObjectTreeNode *node )
{
	CField *field = dynamic_cast<CField*>( node->GetData() );			assert__( field );

	if ( ( typeid( *field ) == typeid( CFieldIndex ) ) && field->IsVirtual() )
	{
		return parent;
	}

	if ( node->GetLevel() <= 1 )
	{
		return SetRootItem( field );
	}


	std::string str = field->GetName();
	if ( !field->GetUnit().empty() )
	{
		str += ( " (" + field->GetUnit() + ")" );
	}

	// v3 note: at depth 1 elements won't have any more children

	const bool bold = 
		( typeid( *field ) == typeid( CFieldRecord ) ) || 
		( typeid( *field ) == typeid( CFieldArray ) && !field->IsFixedSize() );

	size_t num_children = node->ChildCount();
	bool hasChildren = num_children > 0;

	QTreeWidgetItem *id = MakeItem( parent, str, field, bold, hasChildren );

	QTreeWidgetItem *return_id;

	if ( hasChildren )
	{
		return_id = id;
	}
	else
	{
		return_id = parent;
		CObjectTreeNode* parent_node = node->GetParent();
		while ( parent_node != nullptr )
		{
			if ( parent_node->m_current == parent_node->GetChildren().end() )
			{
				return_id = return_id->parent();
				if ( !return_id )
					return_id = invisibleRootItem();
				parent_node = parent_node->GetParent();
			}
			else
			{
				break;
			}
		}
	}

	return return_id;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						CDataExpressionsTree
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CDataExpressionsTree::MakeRootItems()
{
	mItemX = MakeRootItem( mIsMap ? "Lon" : "X", CMapTypeField::eTypeOpAsX );
	mItemY = MakeRootItem( mIsMap ? "Lat" : "Y (optional)", CMapTypeField::eTypeOpAsY );
	mItemData = MakeRootItem( "Data", CMapTypeField::eTypeOpAsField );
	mItemSelectionCriteria = MakeRootItem( "Selection criteria (optional)", CMapTypeField::eTypeOpAsField, false );
}


QAction* CAbstractTree::AddMenuAction( const std::string &name )
{
    QAction *a = new QAction( name.c_str(), mContextMenu );
	mContextMenu->addAction( a );
	return a;
}
QAction* CAbstractTree::AddMenuSeparator()
{
	QAction *a = CActionInfo::CreateAction( mContextMenu, eAction_Separator );
	mContextMenu->addAction( a );
	return a;
}


CDataExpressionsTree::CDataExpressionsTree( bool is_map, QWidget *parent, CFieldsTree *drag_source )
	: base_t( parent )
	, mDragSource( drag_source )
	, mIsMap( is_map )
{
    QStringList labels;
    labels << tr("Data Expressions");
    setHeaderLabels(labels);

	MakeRootItems();

	setDragDropMode( DropOnly );
	viewport()->setAcceptDrops(true);
	setDropIndicatorShown(true);

	auto *root_item = invisibleRootItem();
    root_item->setFlags( root_item->flags() & ~Qt::ItemIsDropEnabled );


	//Tree context menu

	//QActions are automatically deleted: if put as data members and deleted in dtor, issue access violation

	mInsertExprAction = AddMenuAction( "Insert &empty expression" );
    mInsertFieldAction = AddMenuAction( "Insert '%s' f&ield" );
	mInsertFunctionAction = AddMenuAction( "Insert &function..." );
	mInsertFormulaAction = AddMenuAction( "Insert f&ormula..." );
	AddMenuSeparator();
	mSaveasFormulaAction = AddMenuAction( "&Save as formula..." );
	AddMenuSeparator();
	mDeleteExprAction = AddMenuAction( "&Delete expression" );
	mRenameExprAction = AddMenuAction( "&Rename expression" );
	AddMenuSeparator();
	mSortAscending = AddMenuAction( "Sort &Ascending" );
	mSortDescending = AddMenuAction( "Sort &Descending" );

    connect( mInsertExprAction,		SIGNAL(triggered()),	this, SLOT( HandlemInsertExpr() ) );
    connect( mInsertFieldAction,	SIGNAL(triggered()),	this, SLOT( HandleInsertField() ) );
    connect( mInsertFunctionAction,	SIGNAL(triggered()),	this, SLOT( HandleInsertFunction() ) );
    connect( mInsertFormulaAction,	SIGNAL(triggered()),	this, SLOT( HandleInsertFormula() ) );
    connect( mSaveasFormulaAction,	SIGNAL(triggered()),	this, SLOT( HandlemSaveasFormula() ) );
    connect( mDeleteExprAction,		SIGNAL(triggered()),	this, SLOT( HandleDeleteExpr() ) );
	connect( mRenameExprAction,		SIGNAL(triggered()),	this, SLOT( HandleRenameExpr() ) );
    connect( mSortAscending,		SIGNAL(triggered()),	this, SLOT( HandleSortAscending() ) );
    connect( mSortDescending,		SIGNAL(triggered()),	this, SLOT( HandleSortDescending() ) );
}


CMapTypeField::ETypeField CDataExpressionsTree::ItemType( QTreeWidgetItem *item )
{
	return item->data( 0, Qt::UserRole ).value< CMapTypeField::ETypeField >();
}
void CDataExpressionsTree::SetItemType( QTreeWidgetItem *item, CMapTypeField::ETypeField type )
{
	item->setData( 0, Qt::UserRole, QVariant::fromValue( type ) );
}


CFormula* CDataExpressionsTree::ItemFormula( QTreeWidgetItem *item )
{
	return item->data( 0, Qt::UserRole ).value< CFormula* >();
}
void CDataExpressionsTree::SetItemFormula( QTreeWidgetItem *item, CFormula *formula )
{
	item->setData( 0, Qt::UserRole, QVariant::fromValue( formula ) );
}


void CDataExpressionsTree::SelectX()
{
	setCurrentItem( mItemX );
}


// There are 2 possible types:
//
//		eTypeOpYFX
//		eTypeOpZFXY
//
//	If, besides being eTypeOpZFXY, x is lon(lat) and y is lat(lon)
//	the operation IsMap
//
CMapTypeOp::ETypeOp CDataExpressionsTree::GetOperationType()
{
	assert__( mItemX && mItemY );

	bool hasX = mItemX->childCount() > 0;
	bool hasY = mItemY->childCount() > 0;

	return ( hasX && hasY ) ? CMapTypeOp::eTypeOpZFXY : CMapTypeOp::eTypeOpYFX;
}


// If IsPlot(==!IsMap) can only change if x/y are empty or lon/lat or lat/lon
// If IsMap, it can always change
//
bool CDataExpressionsTree::CanSwitchType( std::string &error_msg ) const
{
	if ( !mOperation || mIsMap )
		return true;

	const CFormula* xFormula = mOperation->GetFormula( CMapTypeField::eTypeOpAsX );
	const CFormula* yFormula = mOperation->GetFormula( CMapTypeField::eTypeOpAsY );

	bool can_be_map = ( xFormula->IsLonDataType() && yFormula->IsLatDataType() ) ||
		( xFormula->IsLatDataType() && yFormula->IsLonDataType() );

	if ( !can_be_map )
		error_msg += "X and Y expressions must be cleared or replaced by longitude/latitude or latitude/longitude fields";

	return can_be_map;
}


bool CDataExpressionsTree::SwitchType()
{
	std::string error_msg;
	if ( !CanSwitchType(error_msg) )
		return false;

	mIsMap = !mIsMap;

	mItemX = MakeRootItem( mIsMap ? "Lon" : "X", CMapTypeField::eTypeOpAsX );
	mItemY = MakeRootItem( mIsMap ? "Lat" : "Y (optional)", CMapTypeField::eTypeOpAsY );

	mOperation->SetType( GetOperationType() );

	assert__( !mIsMap || mOperation->IsMap() );

	return true;
}


void CDataExpressionsTree::InsertOperation( COperation *operation )
{
	mOperation = operation;

	clear();
	MakeRootItems();

	if ( mOperation == nullptr )
		return;

	CMapFormula* formulas = mOperation->GetFormulas();
	for ( CMapFormula::iterator it = formulas->begin(); it != formulas->end(); it++ )
	{
		CFormula* formula = mOperation->GetFormula( it );
		if ( formula == nullptr )
			continue;

		switch ( formula->GetType() )
		{
			case CMapTypeField::eTypeOpAsX:
				InsertFormula( mItemX, formula );
				break;
			case CMapTypeField::eTypeOpAsY:
				InsertFormula( mItemY, formula );
				break;
			case CMapTypeField::eTypeOpAsField:
				InsertFormula( mItemData, formula );
				break;
		}
	}

	InsertFormula( mItemSelectionCriteria, mOperation->GetSelect() );
}


QTreeWidgetItem* CDataExpressionsTree::FindParentRootTypeItem( QTreeWidgetItem *from )
{
	if ( !from || from == mItemX || from == mItemY || from == mItemData || from == mItemSelectionCriteria )
		return from;

	if ( from == invisibleRootItem() )
		return nullptr;

	QTreeWidgetItem *parent = from->parent();

	//FindParentRootTypeItem( parent );		//???

	return parent;
}


void CDataExpressionsTree::DeleteFormula( QTreeWidgetItem *item )
{
	assert__( item );

	//std::string name;
	CFormula *formula = ItemFormula( item );
	if ( formula != nullptr )
	{
		if ( item != mItemSelectionCriteria )
		{
			SetItemFormula( item, nullptr );
		}
		mOperation->DeleteFormula( formula->GetName() );
	}

	//CDeleteDataExprEvent ev( GetId(), name );
	//if ( m_owner != nullptr )
	//{
	//	wxPostEvent( m_owner, ev );
	//}

	//Refresh();
}


// false return means not inserted, not necessarily because of an error
//
bool CDataExpressionsTree::InsertFormula( QTreeWidgetItem *parent, CFormula *formula )		//based on Add(const wxTreeItemId& parentId, CFormula* formula)
{
	assert__( formula && mOperation && parent );

	QTreeWidgetItem *the_parent = FindParentRootTypeItem( parent );
	if ( !the_parent )
		return false;

	//DeInstallEventListeners();

	int nChildren = the_parent->childCount();
	int max = CMapTypeField::GetInstance().GetMaxAllowed( ItemType( the_parent ) );

	if ( max > 0 )
	{
		if ( nChildren >= max )
		{
			if ( the_parent != mItemSelectionCriteria )
			{
				QTreeWidgetItem *last_child_id = the_parent->child( the_parent->childCount() - 1 );	//GetLastChild in wxWidgets parlance
				//DeleteFormulaAll(the_parent);
				DeleteFormula( last_child_id );
				//DeleteChildren(the_parent);
				delete last_child_id;
			}
		}
	}

	if ( the_parent == mItemSelectionCriteria )
	{
		mOperation->SetSelect( formula );
		SetItemFormula( mItemSelectionCriteria, mOperation->GetSelect() );
		setCurrentItem( mItemSelectionCriteria );
	}
	else
	{
		QTreeWidgetItem *inserted = MakeItem( the_parent, formula->GetName(), formula );
		the_parent->setExpanded( true );
		mOperation->SetType( GetOperationType() );
		setCurrentItem( inserted );
	}

	if ( the_parent == mItemX || the_parent == mItemY )
	{
		formula->SetStepToDefaultAsNecessary();
	}


	//Refresh();


	//CNewDataExprEvent ev( GetId(), formula->GetName() );
	//if ( m_owner != nullptr )
	//{
	//	wxPostEvent( m_owner, ev );
	//}

	//ConnectToolTipEvent();

	return true;
}


void CDataExpressionsTree::AddField( QTreeWidgetItem *parent, CField *field )
{
	assert__( field && mOperation && parent );

	QTreeWidgetItem *the_parent = FindParentRootTypeItem( parent );
	std::string operationRecord = mOperation->GetRecord();
	std::string fieldRecord = field->GetRecordName();

	if ( !mOperation->HasFormula() )
	{
		mOperation->SetRecord( fieldRecord );
	}

	//DeInstallEventListeners();

	//COperationTreeItemData* itemDataParent = dynamic_cast<COperationTreeItemData*>( GetItemData( theParentId ) );
	//wxTreeItemId selectRootId = GetSelectRootId();

	auto type = ItemType( the_parent );
	//if ( type != nullptr )
	//{
		std::string error_msg;
		CFormula *formula = mOperation->NewUserFormula( error_msg, field, type, the_parent != mItemSelectionCriteria, mDragSource->Product() );
		if ( !error_msg.empty() )
			SimpleWarnBox( error_msg );

		if ( InsertFormula( the_parent, formula ) )
			emit FormulaInserted( formula );
	//}

	//ConnectToolTipEvent();
}


void CDataExpressionsTree::dragEnterEvent( QDragEnterEvent *event )
{
	if ( mDragSource == event->source() )
	{
		auto *item = itemAt( event->pos() );
		QModelIndex dropIndex = indexAt( event->pos() );
		if ( item && ( itemAt( dropIndex.row(), 0 ) == item ) /*&& !item->parent() && ( item->childCount() == 0 ) && item != invisibleRootItem()*/ )
		{
			event->acceptProposedAction();
			qDebug() << "dragEnterEvent " << item;
		}
	}
}


void CDataExpressionsTree::dropEvent( QDropEvent *event )
{
	if ( mDragSource == event->source() && event->dropAction() == Qt::CopyAction )
	{
		auto *item = itemAt( event->pos() );

		qDebug() << "dropEvent " << item;		// items are different !!!!

        auto const &formats = event->mimeData()->formats();

		if ( item && item != invisibleRootItem() && !item->parent() && item != invisibleRootItem() && formats.size() > 0 )
		{
			//TODO if childCount == 1, replace 
			//if ( item->childCount() > 0 )

			QByteArray itemData = event->mimeData()->data( formats[0] );		//"application/x-qabstractitemmodeldatalist"
			QDataStream stream( &itemData, QIODevice::ReadOnly );
			int r, c;
			QMap< int, QVariant > v;
			stream >> r >> c >> v;

			qDebug() << v.value( 32 ).userType();
			qDebug() << v.value( 32 ).typeName();

			CField *field = v.value( 32 ).value<CField*>();			
			if ( field )
				AddField( item, field );

			//base_t::dropEvent( event );
			event->acceptProposedAction();
			return;
		}
	}

	event->ignore();
	return;
}


//virtual 
void CDataExpressionsTree::CustomContextMenu( QTreeWidgetItem *item )
{
	// Leaf field selected in fields tree
	//
	QTreeWidgetItem *fields_tree_item = mDragSource->SelectedItem();
	CField* field = nullptr;
	if ( fields_tree_item && fields_tree_item->childCount() == 0 )
	{
		field = mDragSource->ItemField( fields_tree_item );
	}

	// Menu title
	//
	std::string name;
	CFormula* formula = ItemFormula( item );
	if ( formula == nullptr )
	{
		name = q2a( item->text(0) );
	}
	else
	{
		name = formula->GetName();
	}
	SetMenuTitle( "Menu for " + name );


	//mInsertFieldAction
	//
	std::string text;
	bool enable = false;
	if ( field != nullptr )
	{
		if ( formula == nullptr )
		{
			text = "Insert '" + field->GetName() + "' f&ield as a new expression";
			enable = true;
		}
		else
		{
			text = "Insert '" + field->GetName() + "' f&ield into expression";
			enable = true;
		}
	}
	else
	{
		text = "Insert f&ield";
		enable = false;
	}
	mInsertFieldAction->setText( text.c_str() );
	mInsertFieldAction->setEnabled( enable );


	//mDeleteExprAction
	//
	if ( formula != nullptr )
	{
		text = "&Delete '" + q2a( item->text(0) ) + "' expression";
		enable = true;
	}
	else
	{
		text = "&Delete expression";
		enable = false;
	}
	mDeleteExprAction->setText( text.c_str() );
	mDeleteExprAction->setEnabled( enable );


	//mRenameExprAction
	//
	if ( formula != nullptr )
	{
		if ( !COperation::IsSelect( formula->GetName() ) )
		{
			text = "&Rename '" + q2a( item->text(0) ) + "' expression";
			enable = true;
		}
	}
	else
	{
		text = "&Rename expression";
		enable = false;
	}
	mRenameExprAction->setText( text.c_str() );
	mRenameExprAction->setEnabled( enable );


	//mSaveasFormulaAction
	//
	mSaveasFormulaAction->setEnabled( formula != nullptr && !formula->GetDescription().empty() );


	//mInsertFunctionAction
	//
	mInsertFunctionAction->setEnabled( formula != nullptr );

	//mInsertFormulaAction
	//
	mInsertFormulaAction->setEnabled( formula != nullptr );
}


void CDataExpressionsTree::HandlemInsertExpr()
{
	NOT_IMPLEMENTED;
}
void CDataExpressionsTree::HandleInsertField()
{
	NOT_IMPLEMENTED;
}
void CDataExpressionsTree::HandleInsertFunction()
{
	NOT_IMPLEMENTED;
}
void CDataExpressionsTree::HandleInsertFormula()
{
	NOT_IMPLEMENTED;
}
void CDataExpressionsTree::HandlemSaveasFormula()
{
	NOT_IMPLEMENTED;
}
void CDataExpressionsTree::HandleDeleteExpr()		//DeleteCurrentFormula()
{
	auto *item = SelectedItem();
	DeleteFormula( item );
	if ( item != mItemSelectionCriteria )
		delete item;

	mOperation->SetType( GetOperationType() );
}
void CDataExpressionsTree::HandleRenameExpr()
{
	NOT_IMPLEMENTED;
}
void CDataExpressionsTree::HandleSortAscending()
{
	NOT_IMPLEMENTED;
}
void CDataExpressionsTree::HandleSortDescending()
{
	NOT_IMPLEMENTED;
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DataExpressionsTree.cpp"
