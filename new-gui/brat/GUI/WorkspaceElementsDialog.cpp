#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtils.h"

#include "DataModels/Workspaces/Operation.h"
#include "DataModels/Workspaces/Display.h"
#include "DataModels/Workspaces/Workspace.h"
#include "DataModels/Workspaces/TreeWorkspace.h"
#include "GUI/DisplayWidgets/TextWidget.h"

#include "WorkspaceElementsDialog.h"


// TODO all code until the dialog box member functions definition is 
//	an analysis of the workspace tree types. It works, but given the 
//	intractability of the types design in what concerns abstraction,
//	as shown, even for a simple tree traversal, it will be replaced.


template< typename ELEMENT >
void FillTree_rec( QTreeWidgetItem *parent, const ELEMENT &element )
{
    QTreeWidgetItem *item = new QTreeWidgetItem( parent );
	if ( !element( item, element.value() ) )
	{
		parent->removeChild( item );
		delete item;
		return;
	}

    for ( auto const &child : element )
        FillTree_rec( item, element.child_value( child ) );
}



namespace std
{
	auto begin( const CBratObject & ) -> const int*
	{
		return nullptr;
	}
	auto end( const CBratObject & ) -> const int*
	{
		return nullptr;
	}
};


template< typename VALUE >
const std::string& GenericName( const VALUE &v )
{
	return v.GetName();
}
template<>
const std::string& GenericName( const int &v )
{
    Q_UNUSED( v );

	static std::string s;

	return s;
}
template<>
const std::string& GenericName( const CBratObject &v )
{
    Q_UNUSED( v );

    return GenericName( 0 );
}
template<>
const std::string& GenericName( const CTreeWorkspace &v )
{
    Q_UNUSED( v );

    static const std::string s = "Workspace Tree";
	return s;
}


template< typename VALUE, typename ITERATOR, typename CHILD_NODE >
struct WkspcTreeNodeBase
{
	using iterator_t = ITERATOR;
	using child_value_t = typename std::iterator_traits< ITERATOR >::value_type;
	using value_t = VALUE;
	using child_node_t = CHILD_NODE;

	const value_t *mpv = nullptr;

	WkspcTreeNodeBase( const value_t *pv ) : mpv( pv )
	{}

	virtual const value_t& value() const
	{
		return *mpv;
	}

	virtual bool operator()( QTreeWidgetItem *item, const value_t &v ) const
	{
		item->setText( 0, t2q( GenericName( v ) ) );
		return true;
	}

	virtual const child_node_t child_value( const child_value_t &v ) const = 0;

	virtual iterator_t begin() const {	return std::begin( *mpv );	}
	virtual iterator_t end() const {	return std::end( *mpv );	}
};




template< class WKSPC >
struct workspace_traits;

template<>
struct workspace_traits< CTreeWorkspace >
{
	using child_t = CWorkspace;
};
template<>
struct workspace_traits< CWorkspace >
{
	using child_t = CBratObject;
};
template<>
struct workspace_traits< CWorkspaceDataset >
{
	using child_t = CDataset;
};
template<>
struct workspace_traits< CWorkspaceFormula >
{
	using child_t = CFormula;
};
template<>
struct workspace_traits< CWorkspaceOperation >
{
	using child_t = COperation;
};
template<>
struct workspace_traits< CWorkspaceDisplay >
{
	using child_t = CDisplay;
};


using CAny_iterator = const int*;

template< typename VALUE >
struct WkspcTreeNode : public WkspcTreeNodeBase< VALUE, CAny_iterator, WkspcTreeNode< VALUE > >
{
	using base_t = WkspcTreeNodeBase< VALUE, CAny_iterator, WkspcTreeNode< VALUE > >;
    using value_t = typename base_t::value_t;
    using child_node_t = typename base_t::child_node_t;
    using child_value_t = typename base_t::child_value_t;

	WkspcTreeNode( const value_t *pv ) : base_t( pv )
	{}

	virtual const child_node_t child_value( const child_value_t &v ) const
	{
        Q_UNUSED( v );

        return 0;
	}
};



template< typename WKSPC_CHILD >
bool process_( QTreeWidgetItem *item, const WKSPC_CHILD *value )
{
	if ( !value )
		return false;

	item->setText( 0, t2q( GenericName( *value ) ) );

	return true;
}

bool process( QTreeWidgetItem *item, const CDataset *value ) 
{
	if ( !process_( item, value ) )
		return false;
	return true;
}
bool process( QTreeWidgetItem *item, const CFormula *value ) 
{
	if ( !process_( item, value ) )
		return false;
	return true;
}

bool process( QTreeWidgetItem *item, const COperation *value )
{
    Q_UNUSED( item );

    if ( !value )
		return false;
	return true;
}


bool process( QTreeWidgetItem *item, const CDisplay *value ) 
{
	if ( !process_( item, value ) )
		return false;
	return true;
}

using CBratObject_iterator = CAny_iterator;

template<>
struct WkspcTreeNode< CBratObject > : public WkspcTreeNodeBase< CBratObject, CBratObject_iterator, WkspcTreeNode< CBratObject > >
{
	using base_t = WkspcTreeNodeBase< CBratObject, CBratObject_iterator, WkspcTreeNode< CBratObject > >;

	WkspcTreeNode( const value_t *pv ) : base_t( pv )
	{}

    virtual bool operator()( QTreeWidgetItem *item, const value_t &v ) const override
	{
		auto formula = dynamic_cast< const CFormula* >( &v );
		auto operation = dynamic_cast< const COperation* >( &v );
		auto dataset = dynamic_cast< const CDataset* >( &v );
		auto display = dynamic_cast< const CDisplay* >( &v );

		process( item, formula )	||
		process( item, operation )	||
		process( item, dataset )	||
		process( item, display );

		return true;
	}

	virtual const child_node_t child_value( const child_value_t &pair ) const override
	{
        Q_UNUSED( pair );

        return 0;
	}
};


using CWorkspace_iterator = std::map< std::string, CBratObject* >::const_iterator;

template<>
struct WkspcTreeNode< COperation > : public WkspcTreeNodeBase< COperation, CWorkspace_iterator, WkspcTreeNode< CBratObject > >
{
	using base_t = WkspcTreeNodeBase< COperation, CWorkspace_iterator, WkspcTreeNode< CBratObject > >;

	WkspcTreeNode( const value_t *pv ) : base_t( pv )
	{}

	virtual const child_node_t child_value( const child_value_t &pair ) const override
	{
		return child_node_t( pair.second );
	}
};



template< class WKSPC >
struct WkspcNode : public WkspcTreeNodeBase< WKSPC, CWorkspace_iterator, WkspcTreeNode< typename workspace_traits< WKSPC >::child_t > >
{
	using base_t = WkspcTreeNodeBase< WKSPC, CWorkspace_iterator, WkspcTreeNode< typename workspace_traits< WKSPC >::child_t > >;
    using value_t = typename base_t::value_t;
    using child_node_t = typename base_t::child_node_t;
    using child_value_t = typename base_t::child_value_t;

	WkspcNode( const value_t *pv ) : base_t( pv )
	{}

	virtual const child_node_t child_value( const child_value_t &pair ) const override
	{
		auto data = dynamic_cast< const typename workspace_traits< WKSPC >::child_t* >( pair.second );		assert__(data);
		return child_node_t( data );
	}
};


template<>
struct WkspcTreeNode< CWorkspace > : public WkspcTreeNodeBase< CWorkspace, CWorkspace_iterator, WkspcTreeNode< CBratObject > >
{
	using base_t = WkspcTreeNodeBase< CWorkspace, CWorkspace_iterator, WkspcTreeNode< CBratObject > >;

	WkspcTreeNode( const value_t *pv ) : base_t( pv )
	{}

    virtual bool operator()( QTreeWidgetItem *item, const value_t &v ) const override
	{
        auto wks = dynamic_cast< const CWorkspace* >( &v );			assert__(wks);              Q_UNUSED( wks );

		auto formula = dynamic_cast< const CWorkspaceFormula* >( &v );
		auto operation = dynamic_cast< const CWorkspaceOperation* >( &v );
		auto dataset = dynamic_cast< const CWorkspaceDataset* >( &v );
		auto display = dynamic_cast< const CWorkspaceDisplay* >( &v );

		if ( dataset )
			FillTree_rec( item->parent(), WkspcNode< CWorkspaceDataset >( dataset ) );
		else
		if ( formula )
			FillTree_rec( item->parent(), WkspcNode< CWorkspaceFormula >( formula ) );
		else
		if ( operation )
			FillTree_rec( item->parent(), WkspcNode< CWorkspaceOperation >( operation ) );
		else
		if ( display )
			FillTree_rec( item->parent(), WkspcNode< CWorkspaceDisplay >( display ) );
		else
			return base_t::operator()( item, v );
			
		return false;
	}

	virtual const child_node_t child_value( const child_value_t &pair ) const override
	{
		return child_node_t( pair.second );
	}
};


using CTreeWorkspace_iterator = std::map< std::string, CObjectTreeNode* >::const_iterator;

template<>
struct WkspcTreeNode< CTreeWorkspace > : 
	public WkspcTreeNodeBase< CTreeWorkspace, CTreeWorkspace_iterator, WkspcTreeNode< workspace_traits< CTreeWorkspace>::child_t > >
{
	using base_t = WkspcTreeNodeBase< CTreeWorkspace, CTreeWorkspace_iterator, WkspcTreeNode< workspace_traits< CTreeWorkspace>::child_t > >;

	WkspcTreeNode( const value_t *pv ) : base_t( pv )
	{}

	virtual const child_node_t child_value( const std::map< std::string, CObjectTreeNode* >::value_type &pair ) const override
	{
		auto pv = pair.second->GetData();

		auto wks = dynamic_cast< const CWorkspace* >( pv );				assert__(wks);

		return child_node_t( wks );
	}
};


// TODO end of the analysis exercise - see TODO at top of file



void CWorkspaceElementsDialog::CreateWidgets()
{
	static const QStringList header_labels = QStringList() << tr( "Name" ) << tr( "Type" ) << tr( "Value" );

	//	Create

	//	... Tree

	mTreeWidget = new QTreeWidget( this );
    mTreeWidget->setColumnCount( header_labels.size() );
    mTreeWidget->setHeaderLabels( header_labels );
    mTreeWidget->header()->setStretchLastSection( true );


	//	... Help

	auto help = new CTextWidget;
	help->SetHelpProperties( "Tree of elements composing the workspace " + t2q( mTree.GetRootData()->GetName() ), 0, 6, Qt::AlignCenter );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


	//	... Buttons

	mButtonBox = new QDialogButtonBox( this );
	mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
	mButtonBox->setOrientation( Qt::Horizontal );
	mButtonBox->setStandardButtons( QDialogButtonBox::Close );
    mButtonBox->button( QDialogButtonBox::Close )->setDefault( true );


    QBoxLayout *main_l =
            LayoutWidgets( Qt::Vertical,
                            {
                                mTreeWidget,
								help_group,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "Workspace Elements Tree - " + t2q( mTree.GetRootData()->GetName() ) );

    //	Wrap up dimensions

    adjustSize();
	setMinimumWidth( width() );

	Wire();
}

void CWorkspaceElementsDialog::Wire()
{
    //	Setup things

	connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
	connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );

	//

	QTreeWidgetItem	*parent = mTreeWidget->invisibleRootItem();
    parent->setText( 0, "Root" );
	FillTree_rec( parent, WkspcTreeNode< CTreeWorkspace >( &mTree ) );		// TODO see TODO at top of file
    mTreeWidget->expandAll();
    mTreeWidget->resizeColumnToContents(0);
    mTreeWidget->resizeColumnToContents(1);
    mTreeWidget->resizeColumnToContents(2);
}


CWorkspaceElementsDialog::CWorkspaceElementsDialog( QWidget *parent, const CTreeWorkspace &wks )
	: base_t( parent ), mTree( wks )
{
	assert__( mTree.GetRootData() );

	CreateWidgets();
}


CWorkspaceElementsDialog::~CWorkspaceElementsDialog()
{}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_WorkspaceElementsDialog.cpp"
