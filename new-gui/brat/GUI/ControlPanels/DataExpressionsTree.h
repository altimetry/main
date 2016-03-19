#if !defined GUI_CONTROL_PANELS_DATA_EXPRESSIONS_TREE_H
#define GUI_CONTROL_PANELS_DATA_EXPRESSIONS_TREE_H



class CAbstractTree : public QTreeWidget
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

	using base_t = QTreeWidget;

protected:

	// static members

	static QTreeWidgetItem* SetItemBold( QTreeWidgetItem *item, bool bold );	


	// instance variables

    QIcon mGroupIcon;
    QIcon mKeyIcon;

	QMenu *mContextMenu = nullptr;

	//...domain data


	//construction / destruction


public:
	CAbstractTree( QWidget *parent );

	virtual ~CAbstractTree()
	{}


	//remaining methods

	QTreeWidgetItem* SelectedItem()
	{
		auto items = selectedItems();		assert__( selectionMode() != SingleSelection || items.size() <= 1 );
		if ( items.size() > 0 )
			return items[ 0 ];

		return nullptr;
	}


	//remaining methods for bases classes

protected:

	template< typename FUNCTION >
    QTreeWidgetItem* FindItem( FUNCTION f );


	template< typename DATA >
	QTreeWidgetItem* MakeRootItem( const std::string &name, DATA data = nullptr, bool bold = true );

	template< typename PARENT, typename DATA >
	QTreeWidgetItem* MakeItem( PARENT *parent, const std::string &name, DATA data = nullptr, bool bold = false, bool folder = false );	


	template< typename DATA >
	DATA ItemData( QTreeWidgetItem *item )
	{
		return item->data( 0, Qt::UserRole ).value< DATA >();
	}

	template< typename DATA >
	void SetItemData( QTreeWidgetItem *item, DATA *data )
	{
		if ( !data )
			return;

		item->setData( 0, Qt::UserRole, QVariant::fromValue( *data ) );
	}


	// context menu

	virtual void CustomContextMenu( QTreeWidgetItem *item ) = 0;

	QAction* AddMenuAction( const std::string &name );

	QAction* AddMenuSeparator();

	void SetMenuTitle( const std::string &title )
	{
		mContextMenu->setTitle( title.c_str() );
	}

protected slots:

	void HandleCustomContextMenu( const QPoint &point )
	{
		QModelIndex index = indexAt( point );
		if ( index.isValid() )				// && index.row() % 2 == 0
		{
			CustomContextMenu( itemFromIndex( index ) );
			mContextMenu->exec( mapToGlobal( point ) );
		}
	}
};






class CFieldsTreeWidget : public CAbstractTree
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

	using base_t = CAbstractTree;


	// instance variables

	//...domain data

	CProduct *mProduct = nullptr;

	//construction / destruction

	QTreeWidgetItem* SetRootItem( CField *field );
public:
	CFieldsTreeWidget( QWidget *parent );

	virtual ~CFieldsTreeWidget()
	{}

	//

	const CProduct* Product() const { return mProduct; }

	void InsertProduct( CProduct *product );


	const CField* ItemField( QTreeWidgetItem *item ) const
	{
		return const_cast<CFieldsTreeWidget*>( this )->ItemField( item ) ;
	}


	void SelectRecord( const std::string &record );		//see COperationPanel::GetOperationRecord


protected:

	CField* ItemField( QTreeWidgetItem *item );
	
	void SetItemField( QTreeWidgetItem *item, CField *field );

	QTreeWidgetItem* GetFirstRecordItem();

	virtual void CustomContextMenu( QTreeWidgetItem *item ) override
    {
        Q_UNUSED( item );
    }

	QTreeWidgetItem* InsertField( QTreeWidgetItem *parent, CObjectTreeNode *node );
};








class CDataExpressionsTreeWidget : public CAbstractTree
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

	using base_t = CAbstractTree;


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
	QAction *mSortAscending = nullptr;
	QAction *mSortDescending = nullptr;


	//...domain

	COperation *mCurrentOperation = nullptr;
	bool mIsMap = true;

	//construction / destruction

	void MakeRootItems();
public:
	CDataExpressionsTreeWidget( bool is_map, QWidget *parent, CFieldsTreeWidget *drag_source );

	virtual ~CDataExpressionsTreeWidget()
	{}

	//

	bool IsMap() const { return mIsMap; }
	bool CanSwitchType( std::string &error_msg ) const;
	bool SwitchType();

	void InsertOperation( COperation *operation );

	bool SelectRecord( CProduct *product );

	void SelectX();


	CFormula* SelectedFormula()			//from COperationTreeCtrl::GetCurrentFormula()
	{
		return SelectedItem() ? ItemFormula( SelectedItem() ) : nullptr;
	}


	std::string ParentItemTitle( const CFormula *formula );


	void FormulaChanged( const CFormula *formula );


protected:
	virtual void CustomContextMenu( QTreeWidgetItem *item ) override;

	CMapTypeField::ETypeField ItemType( QTreeWidgetItem *item );


	CFormula* ItemFormula( QTreeWidgetItem *item );

	const CFormula* ItemFormula( const QTreeWidgetItem *item ) const
	{
		return const_cast<CDataExpressionsTreeWidget*>( this )->ItemFormula( const_cast<QTreeWidgetItem*>( item ) );
	}


	void SetItemType( QTreeWidgetItem *item, CMapTypeField::ETypeField type );

	void SetItemFormula( QTreeWidgetItem *item, CFormula *formula );


	CMapTypeOp::ETypeOp GetOperationType();


	QTreeWidgetItem* FindParentItem( const CFormula *formula );

	QTreeWidgetItem* FindParentRootTypeItem( QTreeWidgetItem *from );
	void DeleteFormula( QTreeWidgetItem *item );

	bool InsertFormula( QTreeWidgetItem *parent, CFormula *formula );

	void AddField( QTreeWidgetItem *parent, CField *field );


	//drag and drop

	virtual void dragEnterEvent( QDragEnterEvent *event ) override;
	virtual void dropEvent( QDropEvent *event ) override;

signals:
	void FormulaInserted( CFormula *formula );
	void SelectedFormulaChanged( CFormula *formula );

protected slots:
	void HandleSelectionChanged();

    void HandlemInsertExpr();
    void HandleInsertField();
    void HandleInsertFunction();
    void HandleInsertFormula();
    void HandlemSaveasFormula();
    void HandleDeleteExpr();
	void HandleRenameExpr();
    void HandleSortAscending();
    void HandleSortDescending();
};



#endif	//GUI_CONTROL_PANELS_DATA_EXPRESSIONS_TREE_H
