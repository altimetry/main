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


class CApplicationPaths;



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CRadsDatasetsTreeWidget Items Class
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////



class CRadsDatasetsTreeWidgetItem : public QObject, public QTreeWidgetItem
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

	using qobject_base_t = QObject;
	using base_t = QTreeWidgetItem;


	//instance data

	QFrame *mComboFrame = nullptr;
	QComboBox *mCombo = nullptr;


	//construction / destruction

public:
	CRadsDatasetsTreeWidgetItem( const std::vector< CRadsMission > &missions, CRadsDataset *dataset, QTreeWidget *view );

	virtual ~CRadsDatasetsTreeWidgetItem()
	{}


	//access 

	int currentIndex() const { return mCombo->currentIndex(); }

	void setCurrentIndex( int index ) const { mCombo->setCurrentIndex( index ); }


	QString DatasetName() const { return text( 0 ); }

	QString CurrentMission() const { return mCombo->currentText(); }


	//overrides / signals / slots

	
	// Disambiguate parent() function
	//	- this is an erase, not an override (base function is not virtual)
	//
	QTreeWidgetItem* parent() const { return base_t::parent(); }	


	void SetMissionToolTip();
	void SetToolTip( int column, const QString &atoolTip );


protected:

	virtual bool operator<(const QTreeWidgetItem &other) const override;


signals:
	void itemChanged( CRadsDatasetsTreeWidgetItem*, int );


protected slots:

	void currentIndexChanged( int index );

};




/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CRadsDatasetsTreeWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


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


	// instance variables

	QAction *mSortAscending = nullptr;
	QAction *mSortDescending = nullptr;


	//...domain data

	const std::vector< CRadsMission >& mAllAvailableMissions;
	CWorkspaceDataset *mWDataset = nullptr;


	//construction / destruction

public:
	CRadsDatasetsTreeWidget( const std::vector< CRadsMission >& all_available_missions, QWidget *parent = nullptr );

	virtual ~CRadsDatasetsTreeWidget()
	{}


	//operations

	void WorkspaceChanged( CWorkspaceDataset *wksd )
	{
		mWDataset = wksd;
	}

	QTreeWidgetItem* AddDatasetToTree( const QString &dataset_name );


	//overrides / signals / slots

protected:
	virtual bool eventFilter( QObject *o, QEvent *e ) override;


	virtual void CustomContextMenu( QTreeWidgetItem *item ) override
    {
        Q_UNUSED( item );
    }


signals:

	void currentIndexChanged( CRadsDatasetsTreeWidgetItem *item, int index );


protected slots:

    void HandleSortAscending();
    void HandleSortDescending();
};





#endif	//GUI_CONTROL_PANELS_RADS_DATASETS_TREE_WIDGET_H
