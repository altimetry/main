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



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CRadsDatasetsTreeWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

CRadsDatasetsTreeWidget::CRadsDatasetsTreeWidget( const CApplicationPaths &brat_paths, QWidget *parent )	//parent = nullptr 
	: base_t( parent )
	, mBratPaths( brat_paths )
	, mRadsServiceSettings( mBratPaths )
{
    QStringList labels;
    labels << tr("Dataset") << tr("Mission");
    setHeaderLabels(labels);
    setHeaderHidden( false );
	header()->setStretchLastSection(false);
	header()->setSectionResizeMode(QHeaderView::ResizeToContents);

	//setRootIsDecorated( false );	the problem with this: the highest level items have no node icon, can only be expanded/collapsed by double-clicking
	setDragEnabled( true );
	setDragDropMode( DragOnly );
	setToolTip( "Drag fields to the data expressions tree" );

	setSortingEnabled( true );

	mSortAscending = AddMenuAction( "Sort &Ascending" );
	mSortDescending = AddMenuAction( "Sort &Descending" );

    connect( mSortAscending,		SIGNAL(triggered()),	this, SLOT( HandleSortAscending() ) );
    connect( mSortDescending,		SIGNAL(triggered()),	this, SLOT( HandleSortDescending() ) );

	std::string rads_error_msg;
	bool result = mRadsServiceSettings.mValidRadsMissions;
	if ( !result )
		rads_error_msg = "An error occurred reading missions from rads configuration file.\nPlease check " + brat_paths.mRadsConfigurationFilePath;
	else
	{
		result = mRadsServiceSettings.LoadConfig();
		if ( !result )
			rads_error_msg = "Could not read RADS Service configuration file.";
	}
	if ( !result )
	{
		SimpleErrorBox( rads_error_msg + "\nRADS settings will not be available." );
		setEnabled( false );
	}
}


QTreeWidgetItem* CRadsDatasetsTreeWidget::AddDatasetToTree( const QString &dataset_name )
{
	assert__( mWDataset );

	CRadsDataset *d = mWDataset->GetDataset< CRadsDataset >( q2a( dataset_name ) );				assert__( d );

	QIcon dataset_icon = QIcon(":/images/OSGeo/dataset.png");

	QTreeWidgetItem *dataset_item = new QTreeWidgetItem();
	dataset_item->setText(0, dataset_name );
	dataset_item->setToolTip(0, dataset_name );
	dataset_item->setIcon(0, dataset_icon);
	dataset_item->setFlags( dataset_item->flags() | Qt::ItemIsEditable );
	addTopLevelItem( dataset_item );
    
    QComboBox *cb = new QComboBox;
    auto const &selected_user_missions = d->Missions();
    int index = 0, selected_index = -1;
    auto &missions = mRadsServiceSettings.AllAvailableMissions();
	for ( auto const &mission : missions )
    {
        cb->addItem(mission.mName.c_str());
        bool selected = std::find( selected_user_missions.begin(), selected_user_missions.end(), mission ) != selected_user_missions.end();
		if ( selected )
            selected_index = index;
		index++;
    }
    cb->setCurrentIndex( selected_index );
	QFrame *frame = new QFrame;

	QString sheet =
		//"QComboBox:item {			\
		//	padding-left: 20px;		\
		//}							\
		//QComboBox:item:selected {	\
		//	padding-left: 20px;		\
		//	border: 2px solid black;\
		//}							\
		//QComboBox:item:checked {	\
		//	padding-left: 20px;		\
		//	font-weight: bold;		\
		//}";


"\
QComboBox:hover,\
QComboBox::drop-down:hover,\
QPushButton:hover {\
	border-image: url(:/images/button-hover.png) 16;\
}													   \
QComboBox:on,\
QPushButton:pressed {	\
	color: lightgray;			 \
	border-image: url(:/images/button-pressed.png) 16;\
	padding-top: -15px;									\
	padding-bottom: -17px;								 \
}\
QComboBox,\
QLineEdit,			\
QListView {			 \
	color: rgb(127, 0, 63);\
	background-color: rgb(255, 255, 241);\
	selection-color: white;				  \
	selection-background-color: rgb(191, 31, 127);\
	border: 2px groove gray;\
	border-radius: 5px; \
	padding: 2px 4px;\
	}\
";
//
	//QComboBox::drop-down {\
	//	subcontrol-origin: padding;\
	//	subcontrol-position: center right;\
	//	width: 11px;\
	//	height: 6px; \
	//	background: none;\
	//}				  \
	//QComboBox {\
	//	padding-right: 15px;\
	//}\
	//QListView {\
	//	padding: 5px 4px;\
	//}\
//QComboBox:!editable,	\
//	QPushButton {		\
//	color: white;			\
//	font: bold 10pt;		\
//	border-image: url(:/images/button.png) 16;\
//	border-width: 16px;\
//	padding: -16px 0px;\
//	min-height: 32px;\
//	min-width: 60px;\
//}\

	cb->setStyleSheet( sheet );
	cb->setSizeAdjustPolicy( QComboBox::AdjustToContents );

	//LayoutWidgets( Qt::Horizontal, { cb }, frame, 2, 2, 2, 2, 2 );
    setItemWidget( dataset_item, 1, cb );

	// Fill tree with new dataset products	//FillFileTree( dataset_item );
	QIcon file_icon = QIcon(":/images/OSGeo/file.png");

	for ( auto const &mission : missions )
	{
		QTreeWidgetItem* item = new QTreeWidgetItem;
		item->setText( 0, t2q( mission.mName ) );
		item->setFlags( item->flags() | Qt::ItemIsUserCheckable );
		bool selected = std::find( selected_user_missions.begin(), selected_user_missions.end(), mission ) != selected_user_missions.end();
		item->setCheckState( 0, selected ? Qt::Checked : Qt::Unchecked );
		item->setData( 0, Qt::UserRole, t2q( mission.mAbbr ) );
		dataset_item->addChild( item );
	}

	cb->view()->setMinimumHeight( 6 + missions.size() * ( cb->fontMetrics().lineSpacing() + 2 ) );
	//cb->model()->setData(cb->model()->index(0, 0), QSize(100, 100), Qt::SizeHintRole);
	//resizeColumnToContents(1);
    
	return dataset_item;
}



bool CRadsDatasetsTreeWidget::MissionStateChanged( CWorkspaceOperation *wkso, const QString &dataset_name, QTreeWidgetItem *item )
{
	assert__( mWDataset && wkso );

	CRadsDataset *d = mWDataset->GetDataset< CRadsDataset >( q2a( dataset_name ) );				assert__( d );

	const std::string rads_server_address = ReadRadsServerAddress( mBratPaths.mRadsConfigurationFilePath );
	const std::string local_dir = FormatRadsLocalOutputPath( mBratPaths.UserDataDirectory() );
	const std::string mission_name = q2a( item->text( 0 ) );

	auto const &missions = mRadsServiceSettings.AllAvailableMissions();
	CRadsMission mission = { mission_name, FindRadsMissionAbbr( mission_name, missions ) };
	if ( mission.mAbbr.empty() )
	{
		SimpleErrorBox( "Invalid RADS mission name." );
		return false;
	}

	bool result = false;
	if ( item->checkState( 0 ) == Qt::Checked )
	{
		// Get current files class and type
		std::string old_product_class = d->ProductClass();
		std::string old_product_type = d->ProductType();


		std::string errors;
		result = d->AddMission( rads_server_address, local_dir, mission, errors );
		if ( !errors.empty() )
		{

		}


		// Check new files class and type
		const bool is_same_product_class_and_type = 
			str_icmp( old_product_class, d->ProductClass() ) &&
			str_icmp( old_product_type, d->ProductType() );

		CStringArray operation_names;
		bool used_by_operations = wkso->UseDataset( d->GetName(), &operation_names );
		if ( !is_same_product_class_and_type && used_by_operations )
		{
			std::string str = operation_names.ToString( "\n", false );
			SimpleWarnBox( 
				"Warning: Files contained in the dataset '"
				+ d->GetName()
				+ "' have been changed from '"
				+ old_product_class
				+ "/"
				+ old_product_type
				+ "' to '"
				+ d->ProductClass()
				+ "/"
				+ d->ProductType()
				+ "' product class/type.\n\nThis dataset is used by the operations below:\n"
				+ str
				+ "\n\nBe sure field's names used in these operations match the fields of the dataset files"
			);
		}
	}
	else
	{
		result = d->RemoveMission( rads_server_address, local_dir, mission );
	}

	return result;
}



//CField* CRadsDatasetsTreeWidget::ItemField( QTreeWidgetItem *item )
//{
//	return ItemData< CField* >( item );
//}
//
//
//void CRadsDatasetsTreeWidget::SetItemField( QTreeWidgetItem *item, CField *field )
//{
//	SetItemData( item, &field );
//}


std::string	CRadsDatasetsTreeWidget::GetCurrentFieldDescription() const
{
	std::string desc;

    //auto *item = SelectedItem();	// can return null
	//if ( item )
	//{
	//	auto *field = ItemField( item );
	//	if ( field )
	//		desc = field->GetDescription();
	//}

	return desc;
}


//QTreeWidgetItem* CRadsDatasetsTreeWidget::SetRootItem( CField *field )
//{
//	auto *item = invisibleRootItem();
//	SetItemField( item, field );
//	item->setExpanded( true );
//	return item;
//}


QTreeWidgetItem* CRadsDatasetsTreeWidget::GetFirstRecordItem()
{
	auto *root = invisibleRootItem();
	return root->childCount() > 0 ? root->child( 0 ) : nullptr;
}

void CRadsDatasetsTreeWidget::SelectRecord( const std::string &record )		//see COperationPanel::GetOperationRecord
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


//QTreeWidgetItem* CRadsDatasetsTreeWidget::InsertField( QTreeWidgetItem *parent, CObjectTreeNode *node )
//{
//	CField *field = dynamic_cast<CField*>( node->GetData() );			assert__( field );
//
//	if ( ( typeid( *field ) == typeid( CFieldIndex ) ) && field->IsVirtual() )
//	{
//		return parent;
//	}
//
//	if ( node->GetLevel() <= 1 )
//	{
//		return SetRootItem( field );
//	}
//
//
//	std::string str = field->GetName();
//	if ( !field->GetUnit().empty() )
//	{
//		str += ( " (" + field->GetUnit() + ")" );
//	}
//
//	// v3 note: at depth 1 elements won't have any more children
//
//	const bool bold = 
//		( typeid( *field ) == typeid( CFieldRecord ) ) || 
//		( typeid( *field ) == typeid( CFieldArray ) && !field->IsFixedSize() );
//
//	size_t num_children = node->ChildCount();
//	bool hasChildren = num_children > 0;
//
//	QTreeWidgetItem *id = MakeItem( parent, str, field, bold, hasChildren );
//
//	QTreeWidgetItem *return_id;
//
//	if ( hasChildren )
//	{
//		return_id = id;
//	}
//	else
//	{
//		return_id = parent;
//		CObjectTreeNode* parent_node = node->GetParent();
//		while ( parent_node != nullptr )
//		{
//			if ( parent_node->m_current == parent_node->GetChildren().end() )
//			{
//				return_id = return_id->parent();
//				if ( !return_id )
//					return_id = invisibleRootItem();
//				parent_node = parent_node->GetParent();
//			}
//			else
//			{
//				break;
//			}
//		}
//	}
//
//	return return_id;
//}
//

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
