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
#ifndef GUI_CONTROL_PANELS_DATASETBROWSERCONTROLS_H
#define GUI_CONTROL_PANELS_DATASETBROWSERCONTROLS_H


#include "DatasetsBrowserControlsBase.h"







/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser
/////////////////////////////////////////////////////////////////////////////////////

class CDatasetsBrowserControls : public CDatasetsBrowserControlsBase
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

    using base_t = CDatasetsBrowserControlsBase;


	//instance data


    QToolButton *mAddDir = nullptr;
    QToolButton *mAddFiles = nullptr;
    QToolButton *mRemove = nullptr;
    QToolButton *mClear = nullptr;


	//...doamin data


    //construction / destruction

    void Wire();

public:
    explicit CDatasetsBrowserControls( CBratApplication &app, CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CDatasetsBrowserControls();


	// overrides

protected:

	virtual void UpdatePanelSelectionChange() override;


	virtual EDatasetType DatasetType() const override
	{
		return eStandard;
	}

	virtual QString TreeItemSelectionChanged( QTreeWidgetItem *tree_item ) override;

	virtual void FillFieldList( CDataset *current_dataset, const std::string &current_file ) override;

	virtual QTreeWidgetItem *AddDatasetToTree( const QString &dataset_name ) override;

	// Called
	//	- after assigning new workspace 
	//	- after clearing tree
	//	- before invoking AddDatasetToTree iteratively to refill tree
	//
	virtual void PrepareWorkspaceChange() override;
	
	
    // access

    // operations

	void AddFiles( QStringList &paths_list );					//called by HandleAddFiles and HandleAddDir

	void FillFileTree( QTreeWidgetItem *current_dataset_item );	//populates tree with file nodes; called by AddFiles and AddDatasetToTree


protected slots:

    void HandleAddFiles();
    void HandleAddDir();
    void HandleRemoveFile();
    void HandleClearFiles();

};






#endif // GUI_CONTROL_PANELS_DATASETBROWSERCONTROLS_H
