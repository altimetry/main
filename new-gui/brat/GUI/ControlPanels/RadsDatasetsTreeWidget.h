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
#if !defined GUI_CONTROL_PANELS_RADS_DATASETS_TREE_WIDGET_H
#define GUI_CONTROL_PANELS_RADS_DATASETS_TREE_WIDGET_H


#include "process/rads/RadsSettings.h"
#include "GenericTreeWidget.h"


class CTextWidget;
class CApplicationPaths;





class CRadsDatasetsTreeWidget : public CGenericTreeWidget
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

	const CApplicationPaths &mBratPaths;
	CSharedRadsSettings mRadsServiceSettings;
	CWorkspaceDataset *mWDataset = nullptr;


	//construction / destruction

	//QTreeWidgetItem* SetRootItem( CField *field );
public:
	CRadsDatasetsTreeWidget( const CApplicationPaths &brat_paths, QWidget *parent = nullptr );

	virtual ~CRadsDatasetsTreeWidget()
	{}

	//

	void WorkspaceChanged( CWorkspaceDataset *wksd )
	{
		mWDataset = wksd;
	}

	QTreeWidgetItem* AddDatasetToTree( const QString &dataset_name );

	bool MissionStateChanged( CWorkspaceOperation *wkso, const QString &dataset_name, QTreeWidgetItem *item );

	//const CField* ItemField( const QTreeWidgetItem *item ) const
	//{
	//	return const_cast<CRadsDatasetsTreeWidget*>( this )->ItemField( const_cast<QTreeWidgetItem*>( item ) );
	//}


	void SelectRecord( const std::string &record );		//see COperationPanel::GetOperationRecord

	std::string	GetCurrentFieldDescription() const;


protected:

	//CField* ItemField( QTreeWidgetItem *item );
	
	//void SetItemField( QTreeWidgetItem *item, CField *field );


	QTreeWidgetItem* GetFirstRecordItem();

	const QTreeWidgetItem* GetFirstRecordItem() const
	{
		return const_cast<CRadsDatasetsTreeWidget*>( this )->GetFirstRecordItem();
	}


	virtual void CustomContextMenu( QTreeWidgetItem *item ) override
    {
        Q_UNUSED( item );
    }

protected slots:

    void HandleSortAscending();
    void HandleSortDescending();
};





#endif	//GUI_CONTROL_PANELS_RADS_DATASETS_TREE_WIDGET_H
