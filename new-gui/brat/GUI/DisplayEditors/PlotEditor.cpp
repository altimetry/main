#include "new-gui/brat/stdafx.h"

#include "DataModels/Workspaces/Display.h"
#include "DataModels/DisplayFilesProcessor.h"

#include "GUI/ControlPanels/ViewControlPanels.h"
#include "GUI/DisplayWidgets/BratViews.h"

#include "PlotEditor.h"


void CPlotEditor::CreateWidgets() 	//parent = nullptr
{
	mTabData = new CPlotControlsPanelEdit( this );
	mTabCurveOptions = new CPlotControlsPanelCurveOptions( this );
	mTabAxisOptions = new CPlotControlsPanelAxisOptions( this );
	mTabPlots = new CViewControlsPanelPlots( this );

	AddTab( mTabData, "Edit" );
	AddTab( mTabCurveOptions, "Curve Options" );
	AddTab( mTabAxisOptions, "Axis Options" );
	AddTab( mTabPlots, "Plots" );

    //    Set Header Label Texts
    mTabPlots->mPlotInfoTable->setRowCount(1);
    mTabPlots->mPlotInfoTable->setColumnCount(8);
    mTabPlots->mPlotInfoTable->setHorizontalHeaderLabels(
                QString("Plot name;Expression name;View type;Unit;dim.1;dim.2;Operation name;Dataset").split(";"));

	mPlot2DView = new CBrat2DPlotView( this );
	mPlot3DView = new CBrat3DPlotView( this );
	AddView( mPlot2DView, false );
	AddView( mPlot3DView, true );

	Wire();
}

void CPlotEditor::Wire()
{
	connect( mTabAxisOptions, SIGNAL( LogarithmicScaleX( bool ) ), this, SLOT( HandleLogarithmicScaleX( bool ) ) );
	connect( mTabAxisOptions, SIGNAL( LogarithmicScaleY( bool ) ), this, SLOT( HandleLogarithmicScaleY( bool ) ) );
	connect( mTabAxisOptions, SIGNAL( LogarithmicScaleZ( bool ) ), this, SLOT( HandleLogarithmicScaleZ( bool ) ) );

	connect( mTabCurveOptions, SIGNAL( LineOptionsChecked( bool ) ), this, SLOT( HandleLineOptionsChecked( bool ) ) );
	connect( mTabCurveOptions, SIGNAL( PointOptionsChecked( bool ) ), this, SLOT( HandlePointOptionsChecked( bool ) ) );
}

CPlotEditor::CPlotEditor( CModel *model, COperation *operation, QWidget *parent ) 	//parent = nullptr
	: base_t( model, operation, parent )
{
	assert__( !mOperation->IsMap() );

    CreateWidgets();
}


CPlotEditor::CPlotEditor( const CDisplayFilesProcessor *proc, CPlot* plot, QWidget *parent ) 	//parent = nullptr
	: base_t( false, proc, parent )
{
    Q_UNUSED(proc);     Q_UNUSED(plot);         //mPlot2DView->CreatePlot( proc, plot );

    CreateWidgets();
}

CPlotEditor::CPlotEditor( const CDisplayFilesProcessor *proc, CZFXYPlot* plot, QWidget *parent ) 	//parent = nullptr
	: base_t( false, proc, parent )
{
    CreateWidgets();

    Q_UNUSED(proc);     Q_UNUSED(plot);     //mPlot3DView->CreatePlot( proc, plot );
}




//virtual 
CPlotEditor::~CPlotEditor()
{}

//virtual 
void CPlotEditor::Show2D( bool checked )
{
	assert__( mPlot2DView );

	mPlot2DView->setVisible( checked );
}
//virtual 
void CPlotEditor::Show3D( bool checked )
{
	assert__( mPlot3DView );

	mPlot3DView->setVisible( checked );
}


//virtual 
void CPlotEditor::ResetViews()
{
	// TODO delete this after testing 2D plots clear method
	//if ( mPlot2DView )
	//	mPlot2DView->Clear();
	if ( mPlot2DView )
	{
		auto *p = mPlot2DView;
		mPlot2DView = nullptr;
		RemoveView( p, false );
	}
	mPlot2DView = new CBrat2DPlotView( this );
	AddView( mPlot2DView, false );
	mPlot2DView->setVisible( false );

	if ( mPlot3DView )
	{
		auto *p = mPlot3DView;
		mPlot3DView = nullptr;
		RemoveView( p, false );
	}
	mPlot3DView = new CBrat3DPlotView( this );
	AddView( mPlot3DView, true );
	mPlot3DView->setVisible( false );
}


//virtual 
void CPlotEditor::NewButtonClicked()
{
	NOT_IMPLEMENTED
}


//virtual 
void CPlotEditor::PlotChanged( int index )
{
	if ( index < 0 )
		return;

	WaitCursor wait;

	try
	{
		ResetViews();

		std::vector< CPlot* > yfx_plots;
		std::vector< CZFXYPlot* > zfxy_plots;

		if ( mDisplay->IsYFXType() )
			yfx_plots = GetDisplayPlots< CPlot >( mDisplay );
		else
		if ( mDisplay->IsZYFXType() )
			zfxy_plots = GetDisplayPlots< CZFXYPlot >( mDisplay );
		else
			assert__( false );								assert__( ( yfx_plots.size() || zfxy_plots.size() ) && mCurrentDisplayFilesProcessor );


		for ( auto *yfxplot : yfx_plots )
		{
			assert__( yfxplot != nullptr );

			mPlot2DView->CreatePlot( mCurrentDisplayFilesProcessor->GetXYPlotProperties( 0 ), yfxplot );
			mPlot2DView->setVisible( true );
		}

		for ( auto *zfxyplot : zfxy_plots )
		{
			assert__( zfxyplot != nullptr );

			mPlot3DView->CreatePlot( mCurrentDisplayFilesProcessor->GetZFXYPlotProperties( 0 ), zfxyplot );
			mPlot3DView->setVisible( true );
		}

		if ( !mDisplay->IsZYFXType() )
			mPlot3DView->setVisible( false );

		if ( !mDisplay->IsYFXType() )
			mPlot2DView->setVisible( false );

		return;
	}
	catch ( CException &e )
	{
		SimpleErrorBox( e.what() );
	}
	catch ( std::exception &e )
	{
		SimpleErrorBox( e.what() );
	}
	catch ( ... )
	{
		SimpleErrorBox( "Unexpected error encountered" );
	}

	throw;
}


//virtual 
void CPlotEditor::DatasetsIndexChanged( int index )
{
    Q_UNUSED(index);

    NOT_IMPLEMENTED
}
//virtual 
void CPlotEditor::FiltersIndexChanged( int index )
{
    Q_UNUSED(index);

    NOT_IMPLEMENTED
}
//virtual 
void CPlotEditor::OneClick()
{
	NOT_IMPLEMENTED
}





void CPlotEditor::HandleLogarithmicScaleX( bool log )
{
	assert__( mPlot3DView );

	WaitCursor wait;

	mPlot3DView->SetXLogarithmicScale( log );
}

void CPlotEditor::HandleLogarithmicScaleY( bool log )
{
	assert__( mPlot3DView );

	WaitCursor wait;

	mPlot3DView->SetYLogarithmicScale( log );
}

void CPlotEditor::HandleLogarithmicScaleZ( bool log )
{
	assert__( mPlot3DView );

	WaitCursor wait;

	mPlot3DView->SetZLogarithmicScale( log );
}


void CPlotEditor::HandleLineOptionsChecked( bool checked )
{
	if ( mPlot2DView && checked )
	{
		mPlot2DView->SetLineCurves();
	}
	// TODO 3D

}
void CPlotEditor::HandlePointOptionsChecked( bool checked )
{
	if ( mPlot2DView && checked )
	{
		mPlot2DView->SetPointCurves();
	}
	// TODO 3D

}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_PlotEditor.cpp"
