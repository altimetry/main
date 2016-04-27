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
	mExecuteDisplay = CreateToolButton( "Reset", ":/images/OSGeo/execute.png", "Rebuild the view(s)" );
	mExecuteDisplay->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );


	QWidget *top_buttons_row = CreateButtonRow( false, Qt::Horizontal, 
	{ 
		mNewDisplayButton, mRenameDisplayButton, mDeleteDisplayButton, nullptr, mExecuteDisplay
	} );

	// II. Left Layout

    mDisplaysCombo = new QComboBox;
    mDisplaysCombo->setToolTip( "Open a view" );
	mDisplaysCombo->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );

	auto title_label = new QLabel( "Title" );
	title_label->setToolTip( "Press enter to assign new title" );
    mPlotTitle = new QLineEdit(this);
    mPlotType = new QLineEdit(this);
	SetReadOnlyEditor( mPlotType, true );

	auto *plots_group = //CreateGroupBox( ELayoutType::Horizontal,
			LayoutWidgets( Qt::Horizontal,
	{
		CreateGroupBox( ELayoutType::Horizontal,
		{
			mDisplaysCombo,
		}, "Operation Views", nullptr ),

			nullptr,
			//CreateGroupBox( ELayoutType::Horizontal,
			LayoutWidgets( Qt::Horizontal,
			{
				title_label, mPlotTitle, nullptr, new QLabel( "Type" ), mPlotType
			} )
	},
	nullptr, s, m, m, m, m );

    auto *plots_l = LayoutWidgets( Qt::Vertical, 
	{ 
		top_buttons_row, 
		plots_group
	}, 
	nullptr, 4, 4, top_margin, 4, top_margin );


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
	mPlotTypesList->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );
	mPlotTypesList->setMaximumHeight( MaxSmallListsHeight() );

    auto *types_group = CreateGroupBox( ELayoutType::Vertical, { mPlotTypesList }, "Plot Type", nullptr, s, m, m, m, m );
	mPlotTypesList->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );

    auto *views_group = LayoutWidgets( Qt::Horizontal, { mViewsLayout }, nullptr, s, m, m, m, m );

    mLinkToPlot = new QComboBox;
    auto *link_group = CreateGroupBox( ELayoutType::Vertical, { mLinkToPlot }, "Link to Plot", nullptr, s, m, m, m, m );
	link_group->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );

	AddTopLayout( ELayoutType::Horizontal, 
	{ 
		views_group, nullptr, types_group, LayoutWidgets( Qt::Vertical, { link_group }, nullptr )
	}, 
	s, m, m, m, m );

	link_group->setEnabled( false );		//TODO	delete when implemented
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
