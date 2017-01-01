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
#ifndef GUI_CONTROL_PANELS_DATASET_BROWSERCONTROLS_BASE_H
#define GUI_CONTROL_PANELS_DATASET_BROWSERCONTROLS_BASE_H


#include "DesktopControlPanel.h"
#include "BratApplication.h"


class CTextWidget;


enum EDatasetType
{
	eStandard,
	eRADS
};


template< EDatasetType >
struct CDatasetType;


template<>
struct CDatasetType< eStandard >
{
	using type = CDataset;
};


template<>
struct CDatasetType< eRADS >
{
	using type = CRadsDataset;
};




/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser
/////////////////////////////////////////////////////////////////////////////////////

class CDatasetsBrowserControlsBase : public CDesktopControlsPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	//types

    using base_t = CDesktopControlsPanel;


	//instance data

protected:
	QTreeWidget *mDatasetTree = nullptr;	//must be initialized in derived classes ctor

	QToolButton *mNewDataset = nullptr;		//must be initialized in derived classes ctor
	QToolButton *mDeleteDataset = nullptr;	//must be initialized in derived classes ctor

   	CTextWidget *mFileDesc = nullptr;
	QGroupBox   *mFileDescGroup = nullptr;
	QListWidget *mFieldList = nullptr;
	CTextWidget *mFieldDesc = nullptr;


	//...doamin data

    CBratApplication &mApp;
	const CApplicationPaths &mBratPaths;
	CWorkspaceDataset *mWDataset = nullptr;

   

    //construction / destruction

	// To be called by derived classes ctors
	//
	void AddSpecializedWgdets( QWidget *buttons_row, const QString &datasets_tree_title, const QString &files_description_title );

	// To be called by derived classes Wire
	//
	void Wire();

public:
    explicit CDatasetsBrowserControlsBase( CBratApplication &app, CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CDatasetsBrowserControlsBase();


	// overrides

protected:

	// derived classes virtual interface

	virtual EDatasetType DatasetType() const = 0;

	virtual QString TreeItemSelectionChanged( QTreeWidgetItem *tree_item ) = 0;		//return current dataset name, if any

	virtual QTreeWidgetItem *AddDatasetToTree( const QString &dataset_name ) = 0;

	// Called
	//	- after assigning new workspace 
	//	- after clearing tree
	//	- before invoking AddDatasetToTree iteratively to refill tree
	//
	virtual void PrepareWorkspaceChange() = 0;


	// protected access

	CDataset* WorkspaceDataset( const QString &name );	//get dataset from workspace with correct type


	// operations

	virtual void FillFieldList( CDataset *current_dataset, const std::string &current_file );
	virtual void ClearFieldList();

	bool RenameDataset( QTreeWidgetItem *dataset_item );

signals:
	// Notify to redraw tracks. Item selection may not change, thus, is not catched by HandleTreeItemChanged (RCCC)
	//
	void CurrentDatasetChanged( const CDataset *dataset );

	// Notify about the change (dataset added, renamed, deleted or changed composition: files added or removed)
	//
	void DatasetsChanged( const CDataset *dataset );


public slots:

	void HandleWorkspaceChanged( CWorkspaceDataset *wksd );


protected slots:

	void HandleFieldChanged();
	void HandleSelectionChanged();	//calls TreeItemSelectionChanged after ClearFieldList; emits CurrentDatasetChanged if dataset selection changed 

	void HandleNewDataset();
    void HandleDeleteDataset();
	void HandleDatasetExpanded();								// Resizes DatasetTree when datasets are expanded
	void HandleItemChanged( QTreeWidgetItem *item, int col );	// Rename dataset
    
};






#endif // GUI_CONTROL_PANELS_DATASET_BROWSERCONTROLS_BASE_H
