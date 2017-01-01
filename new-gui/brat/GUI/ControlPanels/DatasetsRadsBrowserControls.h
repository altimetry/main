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
#ifndef GUI_CONTROL_PANELS_RADS_BROWSERCONTROLS_H
#define GUI_CONTROL_PANELS_RADS_BROWSERCONTROLS_H


#include "DatasetsBrowserControlsBase.h"
#include "RadsDatasetsTreeWidget.h"






/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser
/////////////////////////////////////////////////////////////////////////////////////

class CRadsBrowserControls : public CDatasetsBrowserControlsBase
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



	//...doamin data

	const CSharedRadsSettings &mRadsServiceSettings;
	QSharedMemory mSharedMemory;
   

    //construction / destruction

    void Wire();

public:
    explicit CRadsBrowserControls( CBratApplication &app, CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CRadsBrowserControls();


protected:

	// overrides

	virtual void UpdatePanelSelectionChange() override
	{}

	virtual EDatasetType DatasetType() const override
	{
		return eRADS;
	}

	virtual QString TreeItemSelectionChanged( QTreeWidgetItem *tree_item ) override;

	virtual QTreeWidgetItem *AddDatasetToTree( const QString &dataset_name ) override;

	// Called
	//	- after assigning new workspace 
	//	- after clearing tree
	//	- before invoking AddDatasetToTree iteratively to refill tree
	//
	virtual void PrepareWorkspaceChange() override;
	
	
    // access

	CRadsDatasetsTreeWidget* RadsDatasetTree();


	// operations

	bool CheckRadsConfigStatus();


protected slots:

	void HandleCurrentIndexChanged( CRadsDatasetsTreeWidgetItem*, int index );			//Changed the mission selection in a dataset
    void HandleRsyncStatusChanged( CBratApplication::ERadsNotification notification );
};






#endif // GUI_CONTROL_PANELS_RADS_BROWSERCONTROLS_H
