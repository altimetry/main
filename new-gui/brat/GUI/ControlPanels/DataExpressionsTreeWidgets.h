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
#if !defined GUI_CONTROL_PANELS_DATA_EXPRESSIONS_TREE_H
#define GUI_CONTROL_PANELS_DATA_EXPRESSIONS_TREE_H


#include "GenericTreeWidget.h"

class CTextWidget;





class CFieldsTreeWidget : public CGenericTreeWidget
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// types

	using base_t = CGenericTreeWidget;

	friend class CDataExpressionsTreeWidget;


	// instance variables

	QAction *mSortAscending = nullptr;
	QAction *mSortDescending = nullptr;


	//...domain data

	CProduct *mProduct = nullptr;

	//construction / destruction

	QTreeWidgetItem* SetRootItem( CField *field );
public:
	CFieldsTreeWidget( QWidget *parent );

	virtual ~CFieldsTreeWidget()
	{}

	//

    CProduct* Product() { return mProduct; }

	void InsertProduct( CProduct *product );


	const CField* ItemField( const QTreeWidgetItem *item ) const
	{
		return const_cast<CFieldsTreeWidget*>( this )->ItemField( const_cast<QTreeWidgetItem*>( item ) );
	}


	void SelectRecord( const std::string &record );		//see COperationPanel::GetOperationRecord

	std::string	GetCurrentRecord();						//cannot be const	//CFieldsTreeCtrl::GetCurrentRecord()

	std::string	GetCurrentFieldDescription() const;


protected:

	CField* ItemField( QTreeWidgetItem *item );
	
	void SetItemField( QTreeWidgetItem *item, CField *field );


	QTreeWidgetItem* GetFirstRecordItem();

	const QTreeWidgetItem* GetFirstRecordItem() const
	{
		return const_cast<CFieldsTreeWidget*>( this )->GetFirstRecordItem();
	}


	virtual void CustomContextMenu( QTreeWidgetItem *item ) override
    {
        Q_UNUSED( item );
    }

	QTreeWidgetItem* InsertField( QTreeWidgetItem *parent, CObjectTreeNode *node );


protected slots:

    void HandleSortAscending();
    void HandleSortDescending();
};








class CDataExpressionsTreeWidget : public CGenericTreeWidget
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// types

	using base_t = CGenericTreeWidget;


	// static members

public:
	static bool SelectRecord( QWidget *parent, COperation *operation, CProduct *product );


private:
	// instance variables

	CFieldsTreeWidget *mDragSource = nullptr;

	QTreeWidgetItem *mItemX = nullptr;
	QTreeWidgetItem *mItemY = nullptr;
	QTreeWidgetItem *mItemData = nullptr;
	QTreeWidgetItem *mItemSelectionCriteria = nullptr;

	QAction *mInsertExprAction = nullptr;
	QAction *mInsertFieldAction = nullptr;
	QAction *mInsertFormulaAction = nullptr;
	QAction *mDeleteExprAction = nullptr;
	QAction *mRenameExprAction = nullptr;
	QAction *mSaveasFormulaAction = nullptr;
	QAction *mInsertFunctionAction = nullptr;

	CTextWidget *mExpressionTextWidget = nullptr;
	QToolButton *mAssignExpressionButton = nullptr;
    QToolButton *mCheckSyntaxButton = nullptr;


	//...domain

	CWorkspaceFormula *&mWFormula;
	CStringMap &mMapFormulaString;
	COperation *mCurrentOperation = nullptr;
	CFormula *mCurrentFormula = nullptr;		//mUserFormula
	bool mIsMap = true;

	//construction / destruction

	void MakeRootItems();
public:
	CDataExpressionsTreeWidget( CWorkspaceFormula *&wkspc, CStringMap &map_formula, bool is_map, QWidget *parent, CFieldsTreeWidget *drag_source );

	virtual ~CDataExpressionsTreeWidget()
	{}

	//access

	bool IsMap() const { return mIsMap; }

	
	CTextWidget *ExpressionTextWidget() { return mExpressionTextWidget; }
	QToolButton *AssignExpressionButton() { return mAssignExpressionButton; }
    QToolButton *CheckSyntaxButton() { return mCheckSyntaxButton; }

	
	CFormula* SelectedFormula()			//from COperationTreeCtrl::GetCurrentFormula()
	{
		return SelectedItem() ? ItemFormula( SelectedItem() ) : nullptr;
	}


	std::string ParentItemTitle( const CFormula *formula );


	//operations

	bool CanSwitchType( std::string &error_msg ) const;
	bool SwitchType();
    void SetType( bool map );

    void InsertOperation( COperation *operation );

	bool SelectRecord( CProduct *product );

	void SelectX();


	//menu/button operations

	void InsertFunction();
	void InsertAlgorithm();
	void InsertFormula();
	void SaveAsFormula();

	void ShowAliases();


protected:
	virtual void CustomContextMenu( QTreeWidgetItem *item ) override;

	CMapTypeField::ETypeField ItemType( QTreeWidgetItem *item );


	CFormula* ItemFormula( QTreeWidgetItem *item );

	const CFormula* ItemFormula( const QTreeWidgetItem *item ) const
	{
		return const_cast<CDataExpressionsTreeWidget*>( this )->ItemFormula( const_cast<QTreeWidgetItem*>( item ) );
	}

	std::string ItemUnit( const QTreeWidgetItem *item ) const;



	void SetItemType( QTreeWidgetItem *item, CMapTypeField::ETypeField type );

	void SetItemFormula( QTreeWidgetItem *item, CFormula *formula );


	CMapTypeOp::ETypeOp GetOperationType();


	QTreeWidgetItem* FindParentItem( const CFormula *formula );

	QTreeWidgetItem* FindParentRootTypeItem( QTreeWidgetItem *from );
	void DeleteFormula( QTreeWidgetItem *item );

	bool InsertFormula( QTreeWidgetItem *parent, CFormula *formula );

	void FormulaChanged();


	void AddField( QTreeWidgetItem *parent, CField *field );
	void InsertField( CField *field );

	void AddEmptyExpression( QTreeWidgetItem *parent );

	void FormulaNameEdited( QTreeWidgetItem *item, std::string label );
	void FormulaUnitEdited( QTreeWidgetItem *item, std::string label  );
	void SetUnit( std::string label  );


	//drag and drop

	virtual void dragEnterEvent( QDragEnterEvent *event ) override;
	virtual void dropEvent( QDropEvent *event ) override;

signals:
	void FormulaInserted( CFormula *formula );
	void SelectedFormulaChanged( CFormula *formula );

protected slots:
	void HandleSelectionChanged();
	void HandleAssignExpression();
	void HandleExpressionTextChanged();
	void HandleItemChanged( QTreeWidgetItem *item, int column );
    void HandleItemActivated( QTreeWidgetItem *item, int column );

    void HandlemInsertExpr();
    void HandleInsertField();
    void HandleInsertFunction();
    void HandleInsertFormula();
    void HandlemSaveasFormula();
    void HandleDeleteExpr();
	void HandleRenameExpr();
    void HandleCheckSyntax();
};



#endif	//GUI_CONTROL_PANELS_DATA_EXPRESSIONS_TREE_H
