#include "new-gui/brat/stdafx.h"

#include "DataModels/Workspaces/Display.h"
#include "DataModels/DisplayFilesProcessor.h"

#include "GUI/ControlPanels/Views/ViewControlPanels.h"
#include "GUI/DisplayWidgets/BratViews.h"

#include "PlotEditor.h"


void CPlotEditor::CreateWidgets() 	//parent = nullptr
{
	base_t::CreateWidgets<CViewControlsPanelGeneralPlots>();

	mTabCurveOptions = new CPlotControlsPanelCurveOptions( this );
	mTabAxisOptions = new CPlotControlsPanelAxisOptions( this );
	mTabPlots = new CViewControlsPanelPlots( this );


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

    connect( mTabCurveOptions, SIGNAL( LineOptionsChecked( bool, int ) ), this, SLOT( HandleLineOptionsChecked( bool, int ) ) );
    connect( mTabCurveOptions, SIGNAL( PointOptionsChecked( bool, int ) ), this, SLOT( HandlePointOptionsChecked( bool, int ) ) );
    connect( mTabCurveOptions, SIGNAL( CurrCurveChanged( int ) ), this, SLOT( OnUpdateCurveOptions( int ) ) );

    connect( mTabCurveOptions, SIGNAL( CurveLineColorSelected(QColor, int) ), this, SLOT( HandleCurveLineColorSelected(QColor, int)));
    connect( mTabCurveOptions, SIGNAL( CurvePointColorSelected(QColor, int) ), this, SLOT( HandleCurvePointColorSelected(QColor, int)));
    //opacity line changed
    connect( mTabCurveOptions, SIGNAL( CurveLineOpacityEntered(int, int) ), this , SLOT ( HandleOpacityChanged(int, int)));

    connect( mTabCurveOptions, SIGNAL( CurveLineWidthEntered(int, int) ), this , SLOT ( HandleWidthChanged(int, int)));

    //stipplepattern
    connect( mTabCurveOptions, SIGNAL( StipplePatternChanged(int, int) ), this , SLOT(HandleStipplePatternChanged(int, int)));

    connect( mTabCurveOptions, SIGNAL( GlyphPatternChanged(int, int) ), this , SLOT(HandleGlyphPatternChanged(int, int)));

    connect( mTabCurveOptions, SIGNAL( CurveGlyphWidthEntered(int, int) ), this , SLOT ( HandleGlyphSizeChanged(int, int)));

    connect( mTabCurveOptions, SIGNAL( FillGlyphInterior(bool, int)), this, SLOT ( HandleFillGlyphInteriorChanged(bool,int)));

    connect( this, SIGNAL(UpdatedCurrPlot(const CDisplayFilesProcessor*)), mTabCurveOptions, SLOT( HandleNewPlot(const CDisplayFilesProcessor*)));
}



CPlotEditor::CPlotEditor( CModel *model, const COperation *op, const std::string &display_name, QWidget *parent ) 	//display_name = "" parent = nullptr
	: base_t( model, op, display_name, parent )
{
	assert__( !mOperation->IsMap() );

    CreateWidgets();

	Start( display_name );
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
bool CPlotEditor::Refresh()
{
	WaitCursor wait;

	try
	{
		ResetViews();

		std::vector< CPlot* > yfx_plots;
		std::vector< CZFXYPlot* > zfxy_plots;

		if ( mDisplay->IsYFXType() )
        {
            yfx_plots = GetDisplayPlots< CPlot >( mDisplay );

            //shall not be null!!
            assert(mCurrentDisplayFilesProcessor!=nullptr);
            emit UpdatedCurrPlot(mCurrentDisplayFilesProcessor);
        }
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

		return true;

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

	return false;
}


//virtual 
void CPlotEditor::OperationChanged( int index )
{
    Q_UNUSED(index);

	//This will automatically trigger a display change, so we don't need to take additional measures (so far)
}
//virtual 
void CPlotEditor::FilterChanged( int index )
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


void CPlotEditor::HandleLineOptionsChecked( bool checked, int index )
{
	if ( mPlot2DView && checked )
	{
        mPlot2DView->SetTargetCurveStyleLine(index);
	}
	// TODO 3D

}
void CPlotEditor::HandlePointOptionsChecked( bool checked, int index )
{
	if ( mPlot2DView && checked )
	{
        mPlot2DView->SetTargetCurveStyleDots(index);
	}
	// TODO 3D

}


void CPlotEditor::HandleCurveLineColorSelected( QColor new_color, int index )
{
    if ( mPlot2DView )
    {
        mPlot2DView->SetTargetCurvePenColor(index, new_color);
    }
}

void CPlotEditor::HandleCurvePointColorSelected( QColor new_color, int index )
{
    if ( mPlot2DView )
    {
        mPlot2DView->SetTargetCurveGlyphColor(index, new_color);
    }
}

void CPlotEditor::HandleOpacityChanged(int new_value, int index)
{
    if ( mPlot2DView )
    {
        mPlot2DView->SetTargetCurvePenAlpha(index, new_value);

    }
}

void CPlotEditor::HandleWidthChanged(int new_value, int index)
{
    if ( mPlot2DView )
    {
        mPlot2DView->SetTargetCurvePenWidth(index, new_value);

    }
}


void CPlotEditor::HandleStipplePatternChanged(int pattern, int index)
{
    if ( mPlot2DView )
    {
        mPlot2DView->SetTargetCurveLinePattern(index, pattern);
    }
}

void CPlotEditor::HandleGlyphPatternChanged(int pattern, int index)
{
    if ( mPlot2DView )
    {
        mPlot2DView->SetTargetCurvePointSymbol(index, pattern);
    }
}

void CPlotEditor::HandleGlyphSizeChanged(int new_value, int index)
{
    if ( mPlot2DView )
    {
        mPlot2DView->SetTargetCurvePointSize(index, new_value);
    }
}

void CPlotEditor::HandleFillGlyphInteriorChanged(bool checked, int index)
{
    if ( mPlot2DView )
    {
        mPlot2DView->SetInteriorBrush(checked, index);
    }
}

//A new curve was selected by user:
//the model queries information about the curve!
//and updates the tab accordingly
void CPlotEditor::OnUpdateCurveOptions( int index )
{
    int alpha, pattern, lwidth;
    QColor cPenColor;

    if (index < 0)
    {
        return;
    }
    if (this->mPlot2DView->IsTargetCurveStyleLine(index))
    {
        this->mTabCurveOptions->SetPlotTypeLine();

        // fetch current curve brush color
        cPenColor = this->mPlot2DView->GetTargetCurvePenColor(index);

        this->mTabCurveOptions->SetLineColor(cPenColor);

        // Opacity
        alpha = this->mPlot2DView->GetTargetCurvePenAlpha(index);

        this->mTabCurveOptions->SetLineOpacity(QString::number(alpha));

        //sttiple pattern

        pattern = this->mPlot2DView->GetTargetCurveLinePattern(index);

        this->mTabCurveOptions->SetStipplePattern(pattern);

        //width
        lwidth = this->mPlot2DView->GetTargetCurvePenWidth(index);
        this->mTabCurveOptions->SetLineWidth(lwidth);


    }
    else if (this->mPlot2DView->IsTargetCurveStyleDots(index))
    {
        this->mTabCurveOptions->SetPlotTypeDot();

        cPenColor = this->mPlot2DView->GetTargetCurvePenColor(index);

        this->mTabCurveOptions->SetPointColor(cPenColor);

        // glyph pattern update

        int gly_patt = this->mPlot2DView->GetTargetCurvePointSymbol(index);

        this->mTabCurveOptions->SetGlyphPattern(gly_patt);

        int gly_size = this->mPlot2DView->GetTargetCurvePointSize(index);

        this->mTabCurveOptions->SetGlyphSize(QString::number(gly_size));

    }
    else
    {
        return;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_PlotEditor.cpp"
