#include "new-gui/brat/stdafx.h"

#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/Workspaces/Display.h"

#include "GUI/DisplayEditors/PlotEditor.h"

#include "ViewControlPanelsCommon.h"


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//								General Tab
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////
//			General Tab					
////////////////////////////////////////


void CViewControlsPanelGeneral::Wire()
{
	connect( mExecuteDisplay, SIGNAL( clicked() ), this, SLOT( RunButtonClicked() ), Qt::QueuedConnection );
	connect( mDisplaysCombo, SIGNAL( currentIndexChanged(int) ), this, SIGNAL( CurrentDisplayIndexChanged(int) ), Qt::QueuedConnection );
	connect( mNewDisplayButton, SIGNAL( clicked() ), this, SIGNAL( NewButtonClicked() ), Qt::QueuedConnection );
}

void CViewControlsPanelGeneral::CreateWidgets()
{
	// I. Top Buttons Row

	mNewDisplayButton = CreateToolButton( "", ":/images/OSGeo/new.png", "Create a new view" );
    mRenameDisplayButton = CreateToolButton( "", ":/images/OSGeo/edit.png", "Rename the selected view" );
	mDeleteDisplayButton = CreateToolButton( "", ":/images/OSGeo/workspace-delete.png", "Delete the selected view" );
	mExecuteDisplay = CreateToolButton( "", ":/images/OSGeo/execute.png", "Plot the view" );

	QWidget *top_buttons_row = CreateButtonRow( false, Qt::Horizontal, 
	{ 
		mNewDisplayButton, mRenameDisplayButton, mDeleteDisplayButton, nullptr, mExecuteDisplay
	} );

    mDisplaysCombo = new QComboBox;
    mDisplaysCombo->setToolTip( "Open a view" );

    //    Adding previous widgets to this...
	auto *plots_group = CreateGroupBox( ELayoutType::Horizontal, 
	{ 
		LayoutWidgets( Qt::Vertical, { mDisplaysCombo }, nullptr, s, m, m, m, m )
	}, 
	"Views", nullptr, s, m, m, m, m );

    auto *plots_l = LayoutWidgets( Qt::Vertical, { top_buttons_row, plots_group, nullptr }, nullptr, 4, 4, 4, 4, 4 );

	mViewsLayout = AddTopLayout( ELayoutType::Horizontal, { plots_l, nullptr }, 4, 4, 4, 4, 4 );


	Wire();
}


CViewControlsPanelGeneral::CViewControlsPanelGeneral( QWidget *parent, Qt::WindowFlags f )		//parent = nullptr, f = 0
    : base_t( parent, f )
{
	CreateWidgets();
}



void CViewControlsPanelGeneral::RunButtonClicked()
{
	int index = mDisplaysCombo->currentIndex();
	if ( index >= 0 )
		emit RunButtonClicked( index );
}




////////////////////////////////////////
//			Plots General Tab
////////////////////////////////////////


void CViewControlsPanelGeneralPlots::Wire()
{
}

void CViewControlsPanelGeneralPlots::CreateWidgets()
{
    mSelectPlotCombo = new QComboBox;
    mvarX       = new QComboBox;
    mvarY       = new QComboBox;
    mvarY2      = new QComboBox;
    mvarZ       = new QComboBox;

    mSelectPlotCombo->setToolTip( "Select a plot type" );
    mvarX      ->setToolTip( "X" );
    mvarY      ->setToolTip( "Y" );
    mvarY2     ->setToolTip( "Y2" );
    mvarZ      ->setToolTip( "Z" );

    //    II.3 Link to Plot
    auto mPlotTitle   = new QLineEdit(this);
    mPlotTitle->setText("Plot title");

    mLinkToPlot  = new QComboBox;
    mLinkToPlot->setToolTip("Link to Plot");

    //    Adding previous widgets to this...
    auto *group = CreateGroupBox( ELayoutType::Vertical, {
                                 LayoutWidgets( Qt::Horizontal, { mSelectPlotCombo, nullptr, new QLabel( "Data Expression" ), mvarX, mvarY, mvarY2, mvarZ } ),
                                 LayoutWidgets( Qt::Horizontal, { mPlotTitle, nullptr, mLinkToPlot } )
                                            }, "Data Expression", nullptr, s, m, m, m, m );

	layout()->removeItem(mViewsLayout);

	AddTopLayout( ELayoutType::Horizontal, { mViewsLayout, group }, s, m, m, m, m );

	Wire();
}


CViewControlsPanelGeneralPlots::CViewControlsPanelGeneralPlots( QWidget *parent, Qt::WindowFlags f )		//parent = nullptr, f = 0
    : base_t( parent, f )
{
	CreateWidgets();
}



////////////////////////////////////////
//			Maps General Tab
////////////////////////////////////////


void CViewControlsPanelGeneralMaps::Wire()
{
}

void CViewControlsPanelGeneralMaps::CreateWidgets()
{
    mData = new QComboBox;
    mData->setToolTip( "Data");

    auto *group = CreateGroupBox( ELayoutType::Vertical, { mData }, "Data Expression", nullptr, s, m, m, m, m );

	layout()->removeItem( mViewsLayout );

	AddTopLayout( ELayoutType::Horizontal, { mViewsLayout, group }, s, m, m, m, m );

	Wire();
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
