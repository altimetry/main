#include "new-gui/brat/stdafx.h"

#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/Workspaces/Display.h"

#include "GUI/DisplayEditors/PlotEditor.h"

#include "ViewControlPanelsCommon.h"



////////////////////////////////////////
//		Common General Tab					
////////////////////////////////////////


void CViewControlsPanelGeneral::CreateWidgets()
{
	// I. Top Buttons Row

	mNewDisplayButton = CreateToolButton( "", ":/images/OSGeo/new.png", "Create a new view" );
    mRenameDisplayButton = CreateToolButton( "", ":/images/OSGeo/edit.png", "Rename the selected view" );
	mDeleteDisplayButton = CreateToolButton( "", ":/images/OSGeo/workspace-delete.png", "Delete the selected view" );
	mExecuteDisplay = CreateToolButton( "Refresh", ":/images/OSGeo/execute.png", "Plot the view" );
	mExecuteDisplay->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );


	QWidget *top_buttons_row = CreateButtonRow( false, Qt::Horizontal, 
	{ 
		mNewDisplayButton, mRenameDisplayButton, mDeleteDisplayButton, nullptr, mExecuteDisplay
	} );

	// II. Left Layout

    mDisplaysCombo = new QComboBox;
    mDisplaysCombo->setToolTip( "Open a view" );
	mDisplaysCombo->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );

    mPlotTitle = new QLineEdit(this);

	auto *plots_group = CreateGroupBox( ELayoutType::Horizontal, 
	{ 
		LayoutWidgets( Qt::Vertical, { mDisplaysCombo }, nullptr, s, m, m, m, m ),
	}, 
	"Operation Views", nullptr, s, m, m, m, m );

    auto *plots_l = LayoutWidgets( Qt::Vertical, 
	{ 
		top_buttons_row, plots_group, CreateGroupBox( ELayoutType::Horizontal, { new QLabel("Title"), mPlotTitle }, "", nullptr ), nullptr
	}, 
	nullptr, 4, 4, 4, 4, 4 );


	mViewsLayout = LayoutWidgets( Qt::Horizontal, { plots_l }, nullptr, s, m, m, m, m );
}


CViewControlsPanelGeneral::CViewControlsPanelGeneral( QWidget *parent, Qt::WindowFlags f )		//parent = nullptr, f = 0
    : base_t( parent, f )
{
	CreateWidgets();
}




////////////////////////////////////////
//			Plots General Tab
////////////////////////////////////////



void CViewControlsPanelGeneralPlots::CreateWidgets()
{
	layout()->removeItem(mViewsLayout);

    mPlotTypesList = new QListWidget;
    mPlotTypesList->setToolTip( "Select a plot type" );
	mPlotTypesList->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );

    auto *types_group = CreateGroupBox( ELayoutType::Vertical, { mPlotTypesList }, "2D Plot Type", nullptr, s, m, m, m, m );
	types_group->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );

    auto *views_group = CreateGroupBox( ELayoutType::Horizontal, { mViewsLayout, types_group }, "Operation Display", nullptr, s, m, m, m, m );

    mLinkToPlot = new QComboBox;
    auto *link_group = CreateGroupBox( ELayoutType::Vertical, { mLinkToPlot }, "Link to Plot", nullptr, s, m, m, m, m );
	link_group->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Maximum );


	AddTopLayout( ELayoutType::Horizontal, 
	{ 
		views_group, LayoutWidgets( Qt::Vertical, { link_group, nullptr }, nullptr )
	}, 
	s, m, m, m, m );

}


CViewControlsPanelGeneralPlots::CViewControlsPanelGeneralPlots( QWidget *parent, Qt::WindowFlags f )		//parent = nullptr, f = 0
    : base_t( parent, f )
{
	CreateWidgets();
}



////////////////////////////////////////
//			Maps General Tab
////////////////////////////////////////


void CViewControlsPanelGeneralMaps::CreateWidgets()
{
	layout()->removeItem( mViewsLayout );

	AddTopLayout( ELayoutType::Horizontal, { mViewsLayout, nullptr }, s, m, m, m, m );
}


CViewControlsPanelGeneralMaps::CViewControlsPanelGeneralMaps( QWidget *parent, Qt::WindowFlags f )		//parent = nullptr, f = 0
    : base_t( parent, f )
{
	CreateWidgets();
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ViewControlPanelsCommon.cpp"
