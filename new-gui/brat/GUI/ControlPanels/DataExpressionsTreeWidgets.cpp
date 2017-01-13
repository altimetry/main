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

#include <qmetatype.h>


#include "libbrathl/Product.h"
#include "libbrathl/ProductNetCdf.h"

#include "new-gui/Common/QtUtils.h"

#include "DataModels/Workspaces/Workspace.h"
#include "DataModels/Workspaces/Function.h"
#include "DataModels/Workspaces/Formula.h"
#include "DataModels/Workspaces/Operation.h"
#include "DataModels/MapTypeDisp.h"

#include "GUI/ActionsTable.h"
#include "new-gui/Common/GUI/TextWidget.h"
#include "Dialogs/InsertFunctionDialog.h"
#include "Dialogs/InsertAlgorithmDialog.h"
#include "Dialogs/SelectRecordDialog.h"
#include "Dialogs/FormulaDialog.h"
#include "Dialogs/SaveAsFormulaDialog.h"
#include "Dialogs/ShowAliasesDialog.h"

#include "DataExpressionsTreeWidgets.h"



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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						CFieldsTreeWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

CFieldsTreeWidget::CFieldsTreeWidget( QWidget *parent )
	: base_t( parent )
{
    QStringList labels;
    labels << tr("Fields");
    setHeaderLabels(labels);

	//setRootIsDecorated( false );	the problem with this: the highest level items have no node icon, can only be expanded/collapsed by double-clicking
	setDragEnabled( true );
	setDragDropMode( DragOnly );
	setToolTip( "Drag fields to the data expressions tree" );

	setSortingEnabled( true );

	mSortAscending = AddMenuAction( "Sort &Ascending" );
	mSortDescending = AddMenuAction( "Sort &Descending" );

    connect( mSortAscending,		SIGNAL(triggered()),	this, SLOT( HandleSortAscending() ) );
    connect( mSortDescending,		SIGNAL(triggered()),	this, SLOT( HandleSortDescending() ) );
}


//virtual 
CFieldsTreeWidget::~CFieldsTreeWidget()
{
	delete mProduct;
}




CField* CFieldsTreeWidget::ItemField( QTreeWidgetItem *item )
{
	return ItemData< CField* >( item );
}


void CFieldsTreeWidget::SetItemField( QTreeWidgetItem *item, CField *field )
{
	SetItemData( item, &field );
}


std::string	CFieldsTreeWidget::GetCurrentFieldDescription() const
{
	std::string desc;

	auto *item = SelectedItem();	// can return null
	if ( item )
	{
		auto *field = ItemField( item );
		if ( field )
			desc = field->GetDescription();
	}

	return desc;
}


void CFieldsTreeWidget::InsertProduct( CProduct *product )
{
	delete mProduct;
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

	HandleSortAscending();
}


QTreeWidgetItem* CFieldsTreeWidget::SetRootItem( CField *field )
{
	auto *item = invisibleRootItem();
	SetItemField( item, field );
	item->setExpanded( true );
	return item;
}


QTreeWidgetItem* CFieldsTreeWidget::GetFirstRecordItem()
{
	auto *root = invisibleRootItem();
	return root->childCount() > 0 ? root->child( 0 ) : nullptr;
}

void CFieldsTreeWidget::SelectRecord( const std::string &record )		//see COperationPanel::GetOperationRecord
{
	auto items = findItems( record.c_str(), Qt::MatchExactly );
	QTreeWidgetItem *item = items.size() > 0 ? items[ 0 ] : nullptr;
	if ( !item )
	{
		item = GetFirstRecordItem();
    }									//assert__( item );

	setCurrentItem( item );
	if ( item )
		item->setExpanded( true );
}


std::string	CFieldsTreeWidget::GetCurrentRecord() //CFieldsTreeCtrl::GetCurrentRecord()
{
    std::string record_name;			//assert__( mProduct );
	if ( mProduct )
	{
		QTreeWidgetItem *item = SelectedItem();
		if ( !item )
		{
			item = GetFirstRecordItem();
		}

		CField* field = ItemField( item );
		if ( field != nullptr )
		{
			record_name = field->GetRecordName();	  //method cannot be const because of this call
		}

		if ( record_name.empty() && mProduct->IsNetCdf() )
		{
			record_name = CProductNetCdf::m_virtualRecordName.c_str();
		}
	}
	return record_name;
}



QTreeWidgetItem* CFieldsTreeWidget::InsertField( QTreeWidgetItem *parent, CObjectTreeNode *node )
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


void CFieldsTreeWidget::HandleSortAscending()
{
	sortByColumn( 0, Qt::AscendingOrder );
}


void CFieldsTreeWidget::HandleSortDescending()
{
	sortByColumn( 0, Qt::DescendingOrder );
}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						CDataExpressionsTreeWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CDataExpressionsTreeWidget::MakeRootItems()
{
	mItemX = MakeRootItem( mIsMap ? "Lon" : "X", CMapTypeField::eTypeOpAsX );
	mItemY = MakeRootItem( mIsMap ? "Lat" : "Y (optional)", CMapTypeField::eTypeOpAsY );
	mItemData = MakeRootItem( "Data", CMapTypeField::eTypeOpAsField );
	mItemSelectionCriteria = MakeRootItem( "Selection criteria (optional)", CMapTypeField::eTypeOpAsSelect, false );
}


QAction* CGenericTreeWidget::AddMenuAction( const std::string &name )
{
    QAction *a = new QAction( name.c_str(), mContextMenu );
	mContextMenu->addAction( a );
	return a;
}
QAction* CGenericTreeWidget::AddMenuSeparator()
{
	QAction *a = CActionInfo::CreateAction( mContextMenu, eAction_Separator );
	mContextMenu->addAction( a );
	return a;
}


CDataExpressionsTreeWidget::CDataExpressionsTreeWidget( CWorkspaceFormula *&wkspc, CStringMap &map_formula, bool is_map, QWidget *parent, CFieldsTreeWidget *drag_source )
	: base_t( parent, false )
	, mDragSource( drag_source )
	, mWFormula( wkspc )
	, mMapFormulaString( map_formula )
	, mIsMap( is_map )
	, mExpressionTextWidget( new CTextWidget )
	, mAssignExpressionButton( CActionInfo::CreateToolButton( eAction_AssignExpression ) )
    , mCheckSyntaxButton( CreateToolButton( "", ":/images/OSGeo/check.png", "Check syntax" ) )
{
	mExpressionTextWidget->SetSizeHint( mExpressionTextWidget->sizeHint().width(), 3 * fontMetrics().lineSpacing() );

    QStringList labels;
    labels << tr("Data Expressions") << tr("Units");
    setHeaderLabels(labels);

	MakeRootItems();

	setDragDropMode( DropOnly );
	viewport()->setAcceptDrops(true);
	setDropIndicatorShown(true);

	auto *root_item = invisibleRootItem();
    root_item->setFlags( root_item->flags() & ~Qt::ItemIsDropEnabled );

	connect( this, SIGNAL( itemSelectionChanged() ), this, SLOT( HandleSelectionChanged() ) );
	connect( mAssignExpressionButton, SIGNAL( clicked() ), this, SLOT( HandleAssignExpression() ) );
	connect( mExpressionTextWidget, SIGNAL( textChanged() ), this, SLOT( HandleExpressionTextChanged() ) );
    connect( mCheckSyntaxButton, SIGNAL( clicked() ), this, SLOT( HandleCheckSyntax() ) );

	connect( this, SIGNAL( itemChanged( QTreeWidgetItem *, int ) ), this, SLOT( HandleItemChanged( QTreeWidgetItem *, int ) ) );
    connect( this, SIGNAL( itemActivated( QTreeWidgetItem *, int ) ), this, SLOT( HandleItemActivated( QTreeWidgetItem *, int ) ) );


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

    connect( mInsertExprAction,		SIGNAL(triggered()),	this, SLOT( HandlemInsertExpr() ) );
    connect( mInsertFieldAction,	SIGNAL(triggered()),	this, SLOT( HandleInsertField() ) );
    connect( mInsertFunctionAction,	SIGNAL(triggered()),	this, SLOT( HandleInsertFunction() ) );
    connect( mInsertFormulaAction,	SIGNAL(triggered()),	this, SLOT( HandleInsertFormula() ) );
    connect( mSaveasFormulaAction,	SIGNAL(triggered()),	this, SLOT( HandlemSaveasFormula() ) );
    connect( mDeleteExprAction,		SIGNAL(triggered()),	this, SLOT( HandleDeleteExpr() ) );
	connect( mRenameExprAction,		SIGNAL(triggered()),	this, SLOT( HandleRenameExpr() ) );
}


CMapTypeField::ETypeField CDataExpressionsTreeWidget::ItemType( QTreeWidgetItem *item )
{
	return item->data( 0, Qt::UserRole ).value< CMapTypeField::ETypeField >();
}
void CDataExpressionsTreeWidget::SetItemType( QTreeWidgetItem *item, CMapTypeField::ETypeField type )
{
	item->setData( 0, Qt::UserRole, QVariant::fromValue( type ) );
}


CFormula* CDataExpressionsTreeWidget::ItemFormula( QTreeWidgetItem *item )
{
	return item->data( 0, Qt::UserRole ).value< CFormula* >();
}
void CDataExpressionsTreeWidget::SetItemFormula( QTreeWidgetItem *item, CFormula *formula )
{
	item->setData( 0, Qt::UserRole, QVariant::fromValue( formula ) );
}


std::string CDataExpressionsTreeWidget::ItemUnit( const QTreeWidgetItem *item ) const
{
	const CFormula *formula = ItemFormula( item );
	if ( formula )
		return formula->GetUnitAsText();

	return "";
}



void CDataExpressionsTreeWidget::SelectX()
{
	setCurrentItem( mItemX );
}


void CDataExpressionsTreeWidget::FormulaChanged()					//COperationTreeCtrl::OnFormulaChanged(CFormula* formula)		
{
    //assert__( mCurrentOperation );		//cannot assert mCurrentFormula
    if ( !mCurrentOperation )
        return;

	if ( mCurrentOperation->IsSelect( mCurrentFormula ) )
	{
		std::string value = mCurrentFormula->GetDescription( true );
		SetItemBold( mItemSelectionCriteria, !value.empty() );		//mItemSelectionCriteria == GetSelectRootId
	}
}


QTreeWidgetItem* CDataExpressionsTreeWidget::FindParentRootTypeItem( QTreeWidgetItem *from )
{
	if ( !from || from == mItemX || from == mItemY || from == mItemData || from == mItemSelectionCriteria )
		return from;

	if ( from == invisibleRootItem() )
		return nullptr;

	QTreeWidgetItem *parent = from->parent();

	//FindParentRootTypeItem( parent );		//???

	return parent;
}


QTreeWidgetItem* CDataExpressionsTreeWidget::FindParentItem( const CFormula *formula )
{
	QTreeWidgetItem *item = FindItem(
		[ this, &formula ]( const QTreeWidgetItem *item )
		{
			return ItemFormula( item ) == formula;
		} 
	);

	if ( !item )
		return nullptr;

	return FindParentRootTypeItem( item );
}


std::string CDataExpressionsTreeWidget::ParentItemTitle( const CFormula *formula )
{
	std::string s;
	QTreeWidgetItem *item = FindParentItem( formula );
	if ( item )
		s = q2a( item->text( 0 ) );
	return s;
}




// There are 2 possible types:
//
//		eTypeOpYFX
//		eTypeOpZFXY
//
//	If, besides being eTypeOpZFXY, x is lon(lat) and y is lat(lon)
//	the operation IsMap
//
CMapTypeOp::ETypeOp CDataExpressionsTreeWidget::GetOperationType()
{
	assert__( mItemX && mItemY );

	bool hasX = mItemX->childCount() > 0;
	bool hasY = mItemY->childCount() > 0;

	return ( hasX && hasY ) ? CMapTypeOp::eTypeOpZFXY : CMapTypeOp::eTypeOpYFX;
}


// If IsPlot(==!IsMap) can only change if x/y are empty or lon/lat or lat/lon
// If IsMap, it can always change
//
bool CDataExpressionsTreeWidget::CanSwitchType( std::string &error_msg ) const
{
	if ( !mCurrentOperation || mIsMap )
		return true;

	const CFormula* xFormula = mCurrentOperation->GetFormula( CMapTypeField::eTypeOpAsX );
	const CFormula* yFormula = mCurrentOperation->GetFormula( CMapTypeField::eTypeOpAsY );

	bool xIsLon = !xFormula || xFormula->IsLonDataType();
	bool xIsLat = !xFormula || xFormula->IsLatDataType();
	bool yIsLon = !yFormula || yFormula->IsLonDataType();
	bool yIsLat = !yFormula || yFormula->IsLatDataType();

	bool can_be_map = 
		( xIsLon && yIsLat ) ||
		( xIsLat && yIsLon );

	if ( !can_be_map )
		error_msg += "X and Y expressions must be cleared or replaced by longitude/latitude or latitude/longitude fields";

	return can_be_map;
}


void CDataExpressionsTreeWidget::SetType( bool map )
{
    mIsMap = map;

    mItemX->setText( 0, mIsMap ? "Lon" : "X" );
    mItemY->setText( 0, mIsMap ? "Lat" : "Y (optional)" );

    if ( mCurrentOperation )
        mCurrentOperation->SetType( GetOperationType() );
}

bool CDataExpressionsTreeWidget::SwitchType()
{
	std::string error_msg;
	if ( !CanSwitchType(error_msg) )
		return false;

    SetType( !mIsMap );

	//assert__( !mIsMap || mCurrentOperation->IsMap() );

	return true;
}


void CDataExpressionsTreeWidget::InsertOperation( COperation *operation )
{
	mCurrentOperation = operation;

	clear();
	MakeRootItems();

    SetType( !mCurrentOperation || mCurrentOperation->IsMap() );

    if ( mCurrentOperation == nullptr )
	{
		emit SelectedFormulaChanged( nullptr );
		return;
	}

	CMapFormula* formulas = mCurrentOperation->GetFormulas();
	for ( CMapFormula::iterator it = formulas->begin(); it != formulas->end(); it++ )
	{
		CFormula* formula = mCurrentOperation->GetFormula( it );
		if ( formula == nullptr )
			continue;

		switch ( formula->GetFieldType() )
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

            default:
                assert__( false );
        }
	}

	InsertFormula( mItemSelectionCriteria, mCurrentOperation->GetSelect() );

	mCurrentFormula = SelectedFormula();
	mExpressionTextWidget->setText( mCurrentFormula ? mCurrentFormula->GetDescription().c_str() : "" );

    resizeColumnToContents(0);
    resizeColumnToContents(1);
	SelectX();
}


void CDataExpressionsTreeWidget::HandleAssignExpression()
{
	if ( mCurrentFormula )
	{
		mCurrentFormula->SetDescription( q2a( mExpressionTextWidget->toPlainText() ) );
		FormulaChanged();
	}
}


void CDataExpressionsTreeWidget::HandleExpressionTextChanged()
{
	//mAssignExpressionButton->setEnabled( !mExpressionTextWidget->isEmpty() );		allow empty assignments
}


void CDataExpressionsTreeWidget::HandleSelectionChanged()
{
	QTreeWidgetItem *item = SelectedItem();		//cannot assert__( item );
	mCurrentFormula = nullptr;
	QString formula_text;
	if ( item )
	{
		mCurrentFormula = ItemFormula( item );
		if ( mCurrentFormula )
			formula_text = mCurrentFormula->GetDescription().c_str();
	}

	mExpressionTextWidget->setText( formula_text );
	FormulaChanged();

	emit SelectedFormulaChanged( mCurrentFormula );
}


//static 
std::pair< bool, std::string > CDataExpressionsTreeWidget::SelectRecord( QWidget *parent, CProduct *product )
{
	if ( product )
	{
		CSelectRecordDialog dlg( parent, product );	//product cannot be const, product iteration is not read-only...
		if ( dlg.exec() == QDialog::Accepted )
		{
			return{ true, dlg.SelectedRecord() };
		}
	}

	return{ false, empty_string() };
}


//static 
bool CDataExpressionsTreeWidget::SelectAndAssignRecord( QWidget *parent, COperation *operation, CProduct *product )
{
	auto record = SelectRecord( parent, product );
	if ( record.first )
	{
		operation->SetRecord( record.second );
	}

	return record.first;
}


bool CDataExpressionsTreeWidget::SelectAndAssignRecord( CProduct *product )
{
	assert__( mCurrentOperation );

	return CDataExpressionsTreeWidget::SelectAndAssignRecord( this, mCurrentOperation, product );
}




void CDataExpressionsTreeWidget::DeleteFormula( QTreeWidgetItem *item )
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
		mCurrentOperation->DeleteFormula( formula->GetName() );
	}

	HandleSelectionChanged();		//used to refresh text edit

	//CDeleteDataExprEvent ev( GetId(), name );
	//if ( m_owner != nullptr )
	//{
	//	wxPostEvent( m_owner, ev );
	//}

	//Refresh();
}


// false return means not inserted, not necessarily because of an error
//
bool CDataExpressionsTreeWidget::InsertFormula( QTreeWidgetItem *parent, CFormula *formula )		//based on Add(const wxTreeItemId& parentId, CFormula* formula)
{
	assert__( formula && mCurrentOperation && parent );

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
		mCurrentOperation->SetSelect( formula );
		SetItemFormula( mItemSelectionCriteria, mCurrentOperation->GetSelect() );
		mItemSelectionCriteria->setText( 1, formula->GetUnitAsText().c_str() );		//TODO check this
		setCurrentItem( mItemSelectionCriteria );
	}
	else
	{
		QTreeWidgetItem *inserted = MakeEditableLeaf( the_parent, formula->GetName(), formula );
		inserted->setText( 1, formula->GetUnitAsText().c_str() );
		the_parent->setExpanded( true );
		mCurrentOperation->SetType( GetOperationType() );
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


void CDataExpressionsTreeWidget::AddField( QTreeWidgetItem *parent, CField *field )
{
	assert__( field && mCurrentOperation && parent );

	QTreeWidgetItem *the_parent = FindParentRootTypeItem( parent );
	std::string operationRecord = mCurrentOperation->GetRecord();
	std::string fieldRecord = field->GetRecordName();

	if ( !mCurrentOperation->HasFormula() )
	{
		mCurrentOperation->SetRecord( fieldRecord );
	}

	//DeInstallEventListeners();

	//COperationTreeItemData* itemDataParent = dynamic_cast<COperationTreeItemData*>( GetItemData( theParentId ) );
	//wxTreeItemId selectRootId = GetSelectRootId();

	auto type = ItemType( the_parent );
	//if ( type != nullptr )
	//{
	std::string error_msg;
	CFormula *formula = mCurrentOperation->NewUserFormula( error_msg, field, type, the_parent != mItemSelectionCriteria, CProductInfo( mDragSource->Product() ) );
	if ( !error_msg.empty() )
		SimpleWarnBox( error_msg );

	if ( InsertFormula( the_parent, formula ) )
	{
		std::string	error_msg;
		mCurrentOperation->ComputeInterval( error_msg );

        if ( !error_msg.empty() )
            SimpleErrorBox( error_msg );

		emit FormulaInserted( formula );
	}
	//}

	//ConnectToolTipEvent();
}


void CDataExpressionsTreeWidget::InsertField( CField *field )
{
	assert__( mCurrentOperation && field );

	auto *item = SelectedItem();			assert__( item );

	if ( mCurrentFormula == nullptr )
	{
		AddField( item, field );
	}
	else
	{
		if ( mCurrentOperation->GetRecord().compare( field->GetRecordName() ) != 0 )
		{
			mExpressionTextWidget->append( field->GetFullNameWithRecord().c_str() );
		}
		else
		{
			mExpressionTextWidget->append( field->GetFullName().c_str() );
		}

		HandleAssignExpression();
	}
}


void CDataExpressionsTreeWidget::AddEmptyExpression( QTreeWidgetItem *parent )
{
	assert__( mCurrentOperation && parent );

	std::string record = mCurrentOperation->GetRecord();

	if ( record.empty() )
	{
		CProduct *product = const_cast<CProduct*>( mDragSource->Product() );		//ATTENTION to this
		if ( !SelectAndAssignRecord( product ) )
			return;
	}

	auto *the_parent = FindParentRootTypeItem( parent );

	if ( the_parent )
	{
		auto type = ItemType( the_parent );

		std::string error_msg;
		CFormula *formula = mCurrentOperation->NewUserFormula( error_msg, mCurrentOperation->GetFormulaNewName(), type, "", the_parent != mItemSelectionCriteria );

		if ( !error_msg.empty() )
			SimpleWarnBox( error_msg );

		if ( !formula )
			return;

		InsertFormula( the_parent, formula );
	}
}


void CDataExpressionsTreeWidget::dragEnterEvent( QDragEnterEvent *event )
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


void CDataExpressionsTreeWidget::dropEvent( QDropEvent *event )
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
			qDebug() << formats[0];

			QByteArray itemData = event->mimeData()->data( formats[0] );		//"application/x-qabstractitemmodeldatalist"
			QDataStream stream( &itemData, QIODevice::ReadOnly );
			int r, c;
			QMap< int, QVariant > v;
			stream >> r >> c >> v;

			qDebug() << v.value( Qt::UserRole ).userType();
			qDebug() << v.value( Qt::UserRole ).typeName();

			CField *field = v.value( Qt::UserRole ).value<CField*>();			
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
void CDataExpressionsTreeWidget::CustomContextMenu( QTreeWidgetItem *item )
{
	// Leaf field selected in fields tree
	//
	QTreeWidgetItem *fields_tree_item = mDragSource->SelectedItem();
	const CField* field = nullptr;
	if ( fields_tree_item && fields_tree_item->childCount() == 0 )
	{
		field = ((const CFieldsTreeWidget*)mDragSource)->ItemField( fields_tree_item );
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
	if ( formula != nullptr && !COperation::IsSelect( formula->GetName() ) )
	{
		text = "&Rename '" + q2a( item->text(0) ) + "' expression";
		enable = true;
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


void CDataExpressionsTreeWidget::HandlemInsertExpr()
{
	assert__( mCurrentOperation );

	auto *item = SelectedItem();

	AddEmptyExpression( item );

	//SetCurrentFormula();

	//EnableCtrl();
}
void CDataExpressionsTreeWidget::HandleInsertField()
{
	auto *item = mDragSource->SelectedItem();			assert__( item );

	CField *field = mDragSource->ItemField( item );		assert__( item );

	InsertField( field );
}


void CDataExpressionsTreeWidget::ShowAliases()
{
	assert__( mCurrentOperation );

    CShowAliasesDialog dlg( this, mCurrentOperation, mCurrentFormula );
    dlg.exec();

	bool hasOpFieldSelected = mCurrentFormula != nullptr;
	if ( hasOpFieldSelected )
	{
		std::string text;
		const std::vector<QCheckBox*>& checked = dlg.CheckedItems();
        size_t num_rows = checked.size();

		bool hasSelectedItems = false;
        for ( int i = 0; i < (int)num_rows; i++ )
		{
			if ( checked[i]->isChecked() )
			{
				hasSelectedItems = true;
				text += dlg.AliasSyntax( i );
				text += " ";
			}
		}
		if ( hasSelectedItems )
		{
			mExpressionTextWidget->append( text.c_str() );	//		GetOptextform()->WriteText( text );
			HandleAssignExpression();						//		SetTextFormula();
		}
	}
}


void CDataExpressionsTreeWidget::HandleCheckSyntax()
{
    assert__( mCurrentOperation && mDragSource->Product() );

	CProductInfo pi( mDragSource->Product() );
	bool result = pi.IsValid();
	if ( !result )
	{
		LOG_WARN( pi.ErrorMessages() );
	}
	else
	{
		std::string value_out, msg, value = q2a( mExpressionTextWidget->toPlainText() );

		if ( mCurrentOperation->IsSelect( mCurrentFormula ) )
		{
			result = CFormula::CheckExpression( mWFormula, value, mCurrentOperation->GetRecord(), msg, nullptr, &mMapFormulaString, pi, &value_out );
			if ( !result )
				SimpleWarnBox( msg );
		}
		else
		{
			std::string unit = mCurrentFormula->GetUnitAsText();	// = GetOpunit()->GetValue();
			result = CFormula::CheckExpression( mWFormula, value, mCurrentOperation->GetRecord(), msg, &unit, &mMapFormulaString, pi, &value_out );
			if ( !result )
				SimpleWarnBox( msg );
		}

		if ( !value_out.empty() )
		{
			if ( value != value_out )
			{
				mExpressionTextWidget->setPlainText( value_out.c_str() );
			}
		}
	}

    if ( result )
    {
        LOG_WARN( "Data expression OK." );
    }
    else
    {
		mExpressionTextWidget->setFocus();
    }
}


void CDataExpressionsTreeWidget::InsertAlgorithm()
{
	assert__( mCurrentOperation );

	CInsertAlgorithmDialog dlg( this );
	if ( dlg.exec() == QDialog::Accepted )
	{
		mExpressionTextWidget->insertPlainText( dlg.CurrentAlgorithm()->GetSyntax().c_str() );
		mCurrentFormula->SetDescription( q2a( mExpressionTextWidget->toPlainText() ) );				////SetTextFormula();

		//TODO how to deal/assign with field units		
		//SetUnitText();
	}
}
void CDataExpressionsTreeWidget::InsertFunction()
{
	assert__( mCurrentFormula );

	CInsertFunctionDialog dlg( this );
	if ( dlg.exec() == QDialog::Accepted )
	{
		std::string name = dlg.ResultFunction();
		std::string function_syntax = CMapFunction::GetInstance().GetSyntaxFunc( name );
		
		mExpressionTextWidget->insertPlainText( function_syntax.c_str() );
		mCurrentFormula->SetDescription( q2a( mExpressionTextWidget->toPlainText() ) );
		FormulaChanged();
	}
}
void CDataExpressionsTreeWidget::HandleInsertFunction()
{
	InsertFunction();
}


void CDataExpressionsTreeWidget::InsertFormula()
{
	CFormulaDialog dlg( mWFormula, this );
	if ( dlg.exec() == QDialog::Accepted )
	{
		//GetFormulaSyntax( GetOptextform(), GetOpunit() );
		std::string name = dlg.CurrentFormula()->GetName();
		std::string syntax = mWFormula->GetDescFormula( name, dlg.AsAlias() );
		mExpressionTextWidget->insertPlainText( syntax.c_str() );
		//auto *item = SelectedItem();
		//item->setText( 1, dlg.CurrentFormula()->GetUnitAsText().c_str() );
		mCurrentFormula->SetDescription( q2a( mExpressionTextWidget->toPlainText() ) );		//SetTextFormula();
		SetUnit( dlg.CurrentFormula()->GetUnitAsText() );									//SetUnitText();
	}
}
void CDataExpressionsTreeWidget::HandleInsertFormula()
{
	InsertFormula();
}


void CDataExpressionsTreeWidget::SaveAsFormula()
{
	assert__( mCurrentFormula );

	CFormula new_formula( *mCurrentFormula );

    CSaveAsFormula dlg( &new_formula, this );
    if ( dlg.exec() == QDialog::Accepted )
    {
		std::string error_msg;
		if ( !mWFormula->AddFormula( new_formula, error_msg ) )
			SimpleErrorBox( error_msg );
		else
		{
			if (mWFormula->GetFormulaCount() > 0)
				mWFormula->GetFormulaNames( mMapFormulaString );
		}
    }
}
void CDataExpressionsTreeWidget::HandlemSaveasFormula()
{
	SaveAsFormula();
}


void CDataExpressionsTreeWidget::HandleDeleteExpr()		//DeleteCurrentFormula()
{
	auto *item = SelectedItem();
	DeleteFormula( item );
	if ( item != mItemSelectionCriteria )
		delete item;

	mCurrentOperation->SetType( GetOperationType() );
}


void CDataExpressionsTreeWidget::FormulaNameEdited( QTreeWidgetItem *item, std::string label )
{
	assert__( mCurrentOperation );

	CFormula *formula = ItemFormula( item );			assert__( formula && formula == mCurrentFormula );

	CFormula* other_formula = mCurrentOperation->GetFormula( label );
	if ( other_formula != nullptr )
	{
		// Compare pointer, if same, don't warn
		if ( other_formula != formula )
		{
			SimpleErrorBox( "Unable to rename.\nAnother data expression is already named '" + label + "'." );
			item->setText( 0, formula->GetName().c_str() );
			return;
		}
	}

	mCurrentOperation->RenameFormula( formula, label );
}


void CDataExpressionsTreeWidget::SetUnit( std::string unit_value )
{
	QTreeWidgetItem *item = SelectedItem();
	std::string default_value = ItemUnit( item );	//same as take unit as string from formula; using ItemUnit just to be similar to v3
	if ( unit_value.empty() )
	{
		unit_value = default_value;
	}

	//std::string formula_value = q2a( mExpressionTextWidget->toPlainText() );
	//trim( formula_value );
	//auto *item = FindItem( formula_value, true, false );

	std::string error_msg;
	mCurrentFormula->SetUnit( unit_value, error_msg, default_value, true );		//there was a withMsg parameter in v3
	if ( !error_msg.empty() )
		SimpleWarnBox( error_msg );

	item->setText( 1, mCurrentFormula->GetUnitAsText().c_str() );

	if ( !mCurrentFormula->ControlUnitConsistency( error_msg ) )
	{
		SimpleWarnBox( error_msg );
	}

	//SetResolutionLabels();
	//ShowResolutionAndFilterSizer( m_operation->IsZFXY() );
}


void CDataExpressionsTreeWidget::FormulaUnitEdited( QTreeWidgetItem *item, std::string unit_value )
{
	assert__( mCurrentOperation );

	CFormula *formula = ItemFormula( item );		assert__( item == SelectedItem() && formula && formula == mCurrentFormula );		Q_UNUSED( formula );

	if ( item == SelectedItem() )		//it must be
		SetUnit( unit_value );
}


void CDataExpressionsTreeWidget::HandleItemChanged( QTreeWidgetItem *item, int column )
{
	QWidget *w = itemWidget( item, column );
	if ( !w )
		return;

	std::string label = q2a( item->text( column ) );

	trim( label );

	CFormula *formula = ItemFormula( item );			assert__( formula && formula == mCurrentFormula );

	//bool bOk = textLightValidator.IsValid( label );
	//if ( !bOk )
	//{
	//	wxMessageBox( std::string::Format( "Names of data expressions must only contain the following characters:\n"
	//		"%s\n",
	//		textLightValidator.GetDigits().c_str() ),
	//		"Warning",
	//		wxOK | wxICON_EXCLAMATION );

	//	event.Veto();
	//	return;
	//}

	//if ( !id )
	//{
	//	event.Veto();
	//	return;
	//}

	if ( label.empty() )
	{
		if ( column == 0 )
			item->setText( 0, formula->GetName().c_str() );
		else
			item->setText( 1, formula->GetUnitAsText().c_str() );

		return;
	}

	if ( column == 0 )
		FormulaNameEdited( item, label );
	else
	if ( column == 1 )
		FormulaUnitEdited( item, label );
}


void CDataExpressionsTreeWidget::HandleItemActivated( QTreeWidgetItem *item, int column )
{
    if ( item != mItemSelectionCriteria || column != 1 )
        return;

    mItemSelectionCriteria->setFlags( mItemSelectionCriteria->flags() | Qt::ItemIsEditable );
    editItem( mItemSelectionCriteria, 1 );
    mItemSelectionCriteria->setFlags( mItemSelectionCriteria->flags() &~ Qt::ItemIsEditable );
}


void CDataExpressionsTreeWidget::HandleRenameExpr()		//COperationPanel::RenameExpression()
{
	assert__( mCurrentOperation );

	auto *item = SelectedItem();		assert__( item );

	editItem( item );

	//GetOperationtreectrl()->EditLabel( id );

	//EnableCtrl();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DataExpressionsTreeWidgets.cpp"
