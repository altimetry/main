#include "new-gui/brat/stdafx.h"

#include "DataModels/MapTypeDisp.h"
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

	AddTab( mTabCurveOptions, "Data Options" );
	AddTab( mTabAxisOptions, "Axis Options" );

	mTabAxisOptions->mX_axis->setEnabled( false );				//TODO delete when properly implemented
	mTabAxisOptions->mY_axis->setEnabled( false );				//TODO delete when properly implemented
	mTabAxisOptions->mZ_axis->mAxisLabel->setEnabled( false );	//TODO delete when properly implemented
	mTabAxisOptions->mZ_axis->mAxisMax->setEnabled( false );	//TODO delete when properly implemented
	mTabAxisOptions->mZ_axis->mAxisMin->setEnabled( false );	//TODO delete when properly implemented
	mTabAxisOptions->mZ_axis->mNbTicks->setEnabled( false );	//TODO delete when properly implemented
	mTabAxisOptions->mZ_axis->mBase->setEnabled( false );		//TODO delete when properly implemented
	mTabAxisOptions->mZ_axis->mBase->setEnabled( false );		//TODO delete when properly implemented
	mTabAxisOptions->mZ_axis->mNbDigits->setEnabled( false );	//TODO delete when properly implemented
	mTabAxisOptions->mZ_axis->mReset->setEnabled( false );		//TODO delete when properly implemented
	mTabAxisOptions->mZ_axis->mHelpGroup->setVisible( true );	//TODO delete when properly implemented
	

	//Even if ResetViews destroys them before first use, 
	//	it is important to start with valid views
	//
	mPlot2DView = new CBrat2DPlotView( this );
	mPlot3DView = new CBrat3DPlotView( this );
	AddView( mPlot2DView, false );
	AddView( mPlot3DView, true );

	Wire();
}


void CPlotEditor::Wire()
{
	//fill before connecting

	for ( int t = 0; t < CPlotEditor::EPlotTypes_size; ++t )
	{
		QListWidgetItem *item = new QListWidgetItem( CPlotEditor::PlotTypeName( ( CPlotEditor::EPlotType )t ).c_str() );
		TabGeneral()->mPlotTypesList->addItem( item );
	}
	TabGeneral()->mPlotTypesList->setMaximumWidth( TabGeneral()->mPlotTypesList->sizeHintForColumn( 0 ) + 10 );
	TabGeneral()->mPlotTypesList->adjustSize();


	auto const &names = CMapStipplePattern::Names();
	for ( auto const &s : names )
		mTabCurveOptions->mStipplePattern->addItem( s.c_str() );

	auto const &v = CMapPointGlyph::Names();
	for ( auto const &s : v )
		mTabCurveOptions->mPointGlyph->addItem( s.c_str() );


	//connect

	connect( TabGeneral()->mPlotTypesList, SIGNAL( currentRowChanged( int ) ), this, SLOT( HandlePlotTypeChanged( int ) ) );

	// axis

    connect( mTabAxisOptions->mX_axis->mLogScaleCheck, SIGNAL( toggled( bool ) ), this, SLOT( HandleLogarithmicScaleX( bool ) ) );
    connect( mTabAxisOptions->mY_axis->mLogScaleCheck, SIGNAL( toggled( bool ) ), this, SLOT( HandleLogarithmicScaleY( bool ) ) );
    connect( mTabAxisOptions->mZ_axis->mLogScaleCheck, SIGNAL( toggled( bool ) ), this, SLOT( HandleLogarithmicScaleZ( bool ) ) );

    connect( mTabAxisOptions->mX_axis->mReset, SIGNAL( clicked( bool ) ), this, SLOT( HandleResetAxis(  ) ) );
    connect( mTabAxisOptions->mY_axis->mReset, SIGNAL( clicked( bool ) ), this, SLOT( HandleResetAxis(  ) ) );

    connect( mTabAxisOptions->mX_axis->mAxisMax, SIGNAL( returnPressed() ), this, SLOT( HandleXAxisMaxScaleChanged(  ) ) );
    connect( mTabAxisOptions->mX_axis->mAxisMin, SIGNAL( returnPressed() ), this, SLOT( HandleXAxisMinScaleChanged(  ) ) );
    connect( mTabAxisOptions->mY_axis->mAxisMax, SIGNAL( returnPressed() ), this, SLOT( HandleYAxisMaxScaleChanged(  ) ) );
    connect( mTabAxisOptions->mY_axis->mAxisMin, SIGNAL( returnPressed() ), this, SLOT( HandleYAxisMinScaleChanged(  ) ) );

    //SetNbTicks:

    connect( mTabAxisOptions->mX_axis->mNbTicks, SIGNAL( returnPressed() ), this, SLOT( HandleXNbTicksChanged( ) ) );
    connect( mTabAxisOptions->mY_axis->mNbTicks, SIGNAL( returnPressed() ), this, SLOT( HandleYNbTicksChanged( ) ) );

    //connect( mPlot2DView->axisWidget(QwtPlot::xBottom), SIGNAL( scaleDivChanged() ), this, SLOT( RespondUpdateAxis() ));

    // spectrogram

	connect( mTabCurveOptions->mShowContour, SIGNAL( toggled( bool ) ), this, SLOT( HandleShowContourChecked( bool ) ) );
	connect( mTabCurveOptions->mShowSolidColor, SIGNAL( toggled( bool ) ), this, SLOT( HandleShowSolidColorChecked( bool ) ) );


	// curves

	//...select curve
	connect( mTabCurveOptions->mFieldsList, SIGNAL( currentRowChanged( int ) ), this, SLOT( HandleCurrentFieldChanged( int ) ) );

	//...enable/disable line/points
	connect( mTabCurveOptions->mPointOptions, SIGNAL( toggled( bool ) ), this, SLOT( HandlePointOptionsChecked( bool ) ) );
	connect( mTabCurveOptions->mLineOptions, SIGNAL( toggled( bool ) ), this, SLOT( HandleLineOptionsChecked( bool ) ) );

	//...color
	connect( mTabCurveOptions->mLineColorButton, SIGNAL( ColorChanged() ), this, SLOT( HandleCurveLineColorSelected() ) );
	connect( mTabCurveOptions->mPointColorButton, SIGNAL( ColorChanged() ), this, SLOT( HandleCurvePointColorSelected() ) );

	//...pattern/glyph
	connect( mTabCurveOptions->mStipplePattern, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleStipplePatternChanged( int ) ) );
	connect( mTabCurveOptions->mPointGlyph, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleGlyphPatternChanged( int ) ) );

	//...opacity/fill point
	connect( mTabCurveOptions->mFillPointCheck, SIGNAL( toggled( bool ) ), this, SLOT( HandleFillGlyphInterior( bool ) ) );
	connect( mTabCurveOptions->mLineOpacityValue, SIGNAL( returnPressed() ), this, SLOT( HandleCurveLineOpacityEntered() ) );
	//
	//DO NOT delete - alternatives to returnPressed
	//connect( mTabCurveOptions->mLineOpacityValue, SIGNAL( textChanged(const QString &) ), this, SLOT( HandleCurveLineOpacityEntered() ) );
	//connect( mTabCurveOptions->mLineOpacityValue, SIGNAL( editingFinished() ), this, SLOT( HandleCurveLineOpacityEntered() ) );

	//...width/size
	connect( mTabCurveOptions->mLineWidthValue, SIGNAL( returnPressed() ), this, SLOT( HandleCurveLineWidthEntered() ) );
	connect( mTabCurveOptions->mPointSizeValue, SIGNAL( returnPressed() ), this, SLOT( HandleCurveGlyphWidthEntered() ) );
}



CPlotEditor::CPlotEditor( CModel *model, const COperation *op, const std::string &display_name ) 	//display_name = ""
	: base_t( false, model, op, display_name )
{
    CreateWidgets();

	Start( display_name );
}


CPlotEditor::CPlotEditor( const CDisplayFilesProcessor *proc, CPlot* plot )
	: base_t( false, proc )
{
    Q_UNUSED(proc);     Q_UNUSED(plot);

    CreateWidgets();
}

CPlotEditor::CPlotEditor( const CDisplayFilesProcessor *proc, CZFXYPlot* plot )
	: base_t( false, proc )
{
    CreateWidgets();

    Q_UNUSED(proc);     Q_UNUSED(plot);
}




//virtual 
CPlotEditor::~CPlotEditor()
{}





//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//						General Processing
//
//	- editor setup for operation/display/plot-type changes
//	- all other editable properties rely on a correct/consistent setup here
//
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


CViewControlsPanelGeneralPlots* CPlotEditor::TabGeneral() 
{ 
	return dynamic_cast<CViewControlsPanelGeneralPlots*>( mTabGeneral ); 
}



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
void CPlotEditor::Recenter()
{
	mPlot2DView->Home();
	if ( mPlot3DView )
		mPlot3DView->Home();
}


//virtual 
bool CPlotEditor::ResetViews( bool enable_2d, bool enable_3d )
{
	assert__( enable_2d );		//2d displays all types

	if ( mPlot2DView )
	{
		auto *p = mPlot2DView;
		mPlot2DView = nullptr;
		if ( !RemoveView( p, false, true ) )
			return false;
	}
	if ( enable_2d )
	{
		mPlot2DView = new CBrat2DPlotView( this );
		AddView( mPlot2DView, false );
	}

	if ( mPlot3DView )
	{
		auto *p = mPlot3DView;
		mPlot3DView = nullptr;
		if ( !RemoveView( p, true, true ) )
			return false;
	}
	if ( enable_3d )
	{
		mPlot3DView = new CBrat3DPlotView( this );
		AddView( mPlot3DView, true );
	}
	return true;
}


//virtual 
void CPlotEditor::NewButtonClicked()
{
	BRAT_NOT_IMPLEMENTED
}
//virtual 
void CPlotEditor::RenameButtonClicked()
{}
//virtual 
void CPlotEditor::DeleteButtonClicked()
{}
//virtual 
void CPlotEditor::OneClick()
{
	BRAT_NOT_IMPLEMENTED
}

//JOFF
void CPlotEditor::mousePressEvent(QMouseEvent * mouse_event)
{
    //Q_UNUSED(mouse_event);
    //RespondUpdateAxis();
    base_t::mousePressEvent( mouse_event );
}
void CPlotEditor::mouseReleaseEvent(QMouseEvent * mouse_event)
{
    //Q_UNUSED(mouse_event);
    //RespondUpdateAxis();
    base_t::mouseReleaseEvent( mouse_event );
}
void CPlotEditor::mouseMoveEvent(QMouseEvent * mouse_event)
{
//    Q_UNUSED(mouse_event);
//    RespondUpdateAxis();
    base_t::mouseMoveEvent( mouse_event );
}
void CPlotEditor::wheelEvent(QWheelEvent *mouse_event)
{
    //RespondUpdateAxis();
    //event->accept();
    base_t::wheelEvent( mouse_event );
}






void CPlotEditor::HandlePlotTypeChanged( int index )
{
	assert__( index >= 0 );

	EPlotType type = (EPlotType)index;			assert__( type < EPlotTypes_size );

	Refresh( type );
}


void CPlotEditor::UpdatePlotType( EPlotType plot_type, bool select )
{
	assert__( mDisplay );

	for ( int i = 0; i < TabGeneral()->mPlotTypesList->count(); ++i )
	{
		auto *item = TabGeneral()->mPlotTypesList->item( i );
		item->setFlags( item->flags() | Qt::ItemIsEnabled );
	}

	//CMapTypeDisp::ETypeDisp: eTypeDispYFX, eTypeDispZFXY, eTypeDispZFLatLon

	CMapTypeDisp::ETypeDisp type = mDisplay->GetType();

	//EPlotType: eXY, eXYY, eXYZ, eHistogram,

	if ( type == CMapTypeDisp::eTypeDispYFX )
	{
		auto *itemXYZ = TabGeneral()->mPlotTypesList->item( eXYZ );
		itemXYZ->setFlags( itemXYZ->flags() & ~Qt::ItemIsEnabled );
	}
	else
	if ( type == CMapTypeDisp::eTypeDispZFXY || type == CMapTypeDisp::eTypeDispZFLatLon )
	{
		auto *itemXY = TabGeneral()->mPlotTypesList->item( eXY );
		itemXY->setFlags( itemXY->flags() & ~Qt::ItemIsEnabled );

		auto *itemXYY = TabGeneral()->mPlotTypesList->item( eXYY );
		itemXYY->setFlags( itemXYY->flags() & ~Qt::ItemIsEnabled );

		auto *itemHisto = TabGeneral()->mPlotTypesList->item( eHistogram );
		itemHisto->setFlags( itemHisto->flags() & ~Qt::ItemIsEnabled );
	}
	else
		assert__( false );

	if ( !select )
		TabGeneral()->mPlotTypesList->blockSignals( true );
	TabGeneral()->mPlotTypesList->setCurrentItem( TabGeneral()->mPlotTypesList->item( plot_type ) );
	if ( !select )
		TabGeneral()->mPlotTypesList->blockSignals( false );

	//DO NOT delete: example of selecting without triggering signal; disadvantage: triggers on 1st activation without user intervention
	//
	//TabGeneral()->mPlotTypesList->setItemSelected( TabGeneral()->mPlotTypesList->item( plot_type ), true );
}


//virtual 
void CPlotEditor::OperationChanged( int index )
{
    Q_UNUSED(index);

	TabGeneral()->mPlotTypesList->setEnabled( mOperation );

	//if ( !mOperation )
	//	return;

	//const CMapFormula* formulas = mOperation->GetFormulas();
	//const CMapTypeOp::ETypeOp type = mOperation->GetType();		//ETypeOp: eTypeOpYFX, eTypeOpZFXY

	//for ( CMapFormula::const_iterator it = formulas->cbegin(); it != formulas->cend(); it++ )
	//{
	//	const CFormula* formula = mOperation->GetFormula( it );
	//	if ( formula == nullptr )
	//		continue;

	//	switch ( formula->GetType() )
	//	{
	//		case CMapTypeField::eTypeOpAsX:

	//			break;

	//		case CMapTypeField::eTypeOpAsY:

	//			assert__( type != CMapTypeOp::eTypeOpYFX );

	//			break;

	//		case CMapTypeField::eTypeOpAsField:

	//			if ( type == CMapTypeOp::eTypeOpZFXY )
	//				;
	//			else
	//			{
	//			}
	//			break;
	//	}
	//}
}



//virtual 
bool CPlotEditor::ViewChanged()
{

	EPlotType type = mDisplay->IsYFXType() ? eXY : eXYZ;
	UpdatePlotType( type, false );
	return Refresh( type );
}

bool CPlotEditor::Refresh( EPlotType type )
{
	WaitCursor wait;

	try
	{
		mPlotType = type;
		std::vector< CPlot* > yfx_plots;
		std::vector< CZFXYPlot* > zfxy_plots;
		mDataArrayXY = nullptr;
		mPropertiesXY = nullptr;
		mDataArrayZFXY_2D = nullptr;
		mPropertiesZFXY_2D = nullptr;
		mDataArrayZFXY_3D = nullptr;
		mPropertiesZFXY_3D = nullptr;


		if ( mDisplay->IsYFXType() )
        {
            yfx_plots = GetDisplayPlots< CPlot >( mDisplay );
        }
        else
		if ( mDisplay->IsZYFXType() || mDisplay->IsZLatLonType() )
		{
			zfxy_plots = GetDisplayPlots< CZFXYPlot >( mDisplay );
		}
		else
			assert__( false );					assert__( ( yfx_plots.size() || zfxy_plots.size() ) && mCurrentDisplayFilesProcessor );


		if ( !ResetViews( true, zfxy_plots.size() > 0 ) )
			throw CException( "A previous plot is still processing. Please try again later." );

		SelectTab( TabGeneral()->parentWidget() );

		int yfx_index = 0;						assert__( yfx_plots.size() <= 1 );		//forces redesign if false
		for ( auto *yfxplot : yfx_plots )
		{
			assert__( yfxplot != nullptr );

			Reset2DProperties( mCurrentDisplayFilesProcessor->GetXYPlotProperties( yfx_index++ ), yfxplot );

			yfx_index++;
		}

		
		int zfxy_index = 0;						assert__( zfxy_plots.size() <= 1 );		//forces redesign if false
		for ( auto *zfxyplot : zfxy_plots )
		{
			assert__( zfxyplot != nullptr );
			assert__( mPlotType == eXYZ );

			auto *props = mCurrentDisplayFilesProcessor->GetZFXYPlotProperties( zfxy_index++ );

			Reset2DProperties( props, zfxyplot );
		}

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

	
	ResetViews( true, false );

	return false;
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//						Properties Processing
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
//						Axis Processing
//////////////////////////////////////////////////////////////////////////




void CPlotEditor::HandleLogarithmicScaleX( bool log )
{
	WaitCursor wait;

	if ( mPlot3DView )
    {
        mPlot3DView->SetLogarithmicScale( log );		//TODO or eliminate}
    }

    if (mPlot2DView)
    {
        mPlot2DView->SetLogScale(QwtPlot::xBottom, log);
        mPropertiesXY->SetXLog(log);
    }
}

void CPlotEditor::HandleLogarithmicScaleY( bool log )
{
	WaitCursor wait;

    if ( mPlot3DView )
    {
        mPlot3DView->SetLogarithmicScale( log );		//TODO or eliminate
    }
    if (mPlot2DView)
    {
        mPlot2DView->SetLogScale(QwtPlot::yLeft, log);
        mPropertiesXY->SetYLog(log);
    }
}

void CPlotEditor::HandleLogarithmicScaleZ( bool log )
{
	WaitCursor wait;

	if ( mPlot3DView )
		mPlot3DView->SetLogarithmicScaleZ( log );

    if (mPlot2DView)
    {
        mPlot2DView->SetLogScale(QwtPlot::yRight, log);
    }
}

//reset scales
void CPlotEditor::HandleResetAxis()
{
    double xMin_or, xMax_or, yMin_or, yMax_or;

    xMin_or = mPropertiesXY->GetXMin();
    xMax_or = mPropertiesXY->GetXMax();
    yMin_or = mPropertiesXY->GetYMin();
    yMax_or = mPropertiesXY->GetYMax();

    mPropertiesXY->SetCurrXMin(xMin_or);
    mPropertiesXY->SetCurrXMax(xMax_or);
    mPropertiesXY->SetCurrYMin(yMin_or);
    mPropertiesXY->SetCurrYMax(yMax_or);

    mPlot2DView->SetAxisScales(xMin_or, xMax_or, yMin_or, yMax_or);
    RespondUpdateAxis();
}

void CPlotEditor::HandleXAxisMinScaleChanged()
{
    bool ok_conv=false;
    double x_max = mPropertiesXY->GetCurrXMax();
    QString new_xmin_str = mTabAxisOptions->mX_axis->mAxisMin->text();
    double new_xmin = new_xmin_str.toDouble(&ok_conv);

    if ((!ok_conv) || (new_xmin > x_max))
    {
        RespondUpdateAxis();
        return;
    }

    mPropertiesXY->SetCurrXMin(new_xmin);
    ScaleFromPropertiesTo2DPlot();
}

void CPlotEditor::HandleXAxisMaxScaleChanged()
{
    bool ok_conv=false;
    double x_min = mPropertiesXY->GetCurrXMin();
    QString new_xmax_str = mTabAxisOptions->mX_axis->mAxisMax->text();
    double new_xmax = new_xmax_str.toDouble(&ok_conv);

    if ((!ok_conv) || (new_xmax < x_min))
    {
        RespondUpdateAxis();
        return;
    }

    mPropertiesXY->SetCurrXMax(new_xmax);
    ScaleFromPropertiesTo2DPlot();
}

void CPlotEditor::HandleYAxisMinScaleChanged()
{
    bool ok_conv=false;
    double y_max = mPropertiesXY->GetCurrYMax();
    QString new_ymin_str = mTabAxisOptions->mY_axis->mAxisMin->text();
    double new_ymin = new_ymin_str.toDouble(&ok_conv);

    if ((!ok_conv) || (new_ymin > y_max))
    {
        RespondUpdateAxis();
        return;
    }

    mPropertiesXY->SetCurrYMin(new_ymin);
    ScaleFromPropertiesTo2DPlot();
}
void CPlotEditor::HandleXNbTicksChanged()
{
    bool ok_conv=false;
    QString new_nb_ticks = mTabAxisOptions->mX_axis->mNbTicks->text();
    int new_nb = new_nb_ticks.toInt(&ok_conv);

    if ((!ok_conv) || (new_nb <= 0))
    {
        mTabAxisOptions->mX_axis->mNbTicks->setText(QString::number(mPropertiesXY->GetXNumTicks()));
        return;
    }

    double curr_min = mPropertiesXY->GetCurrXMin();
    double curr_max = mPropertiesXY->GetCurrXMax();
    double delta = (curr_max-curr_min)/(double)new_nb;
    mPropertiesXY->SetXNumTicks(new_nb);
    mPlot2DView->GenScaleX(curr_min, curr_max, delta);
}

void CPlotEditor::HandleYNbTicksChanged()
{
    bool ok_conv=false;
    QString new_nb_ticks = mTabAxisOptions->mY_axis->mNbTicks->text();
    int new_nb = new_nb_ticks.toInt(&ok_conv);

    if ((!ok_conv) || (new_nb <= 0))
    {
        mTabAxisOptions->mY_axis->mNbTicks->setText(QString::number(mPropertiesXY->GetYNumTicks()));
        return;
    }

    double curr_min = mPropertiesXY->GetCurrYMin();
    double curr_max = mPropertiesXY->GetCurrYMax();
    double delta = (curr_max-curr_min)/(double)new_nb;
    mPropertiesXY->SetYNumTicks(new_nb);
    mPlot2DView->GenScaleY(curr_min, curr_max, delta);
}

void CPlotEditor::HandleYAxisMaxScaleChanged()
{
    bool ok_conv=false;
    double y_min = mPropertiesXY->GetCurrYMin();
    QString new_ymax_str = mTabAxisOptions->mY_axis->mAxisMax->text();
    double new_ymax = new_ymax_str.toDouble(&ok_conv);

    if ((!ok_conv) || (new_ymax < y_min))
    {
        RespondUpdateAxis();
        return;
    }

    mPropertiesXY->SetCurrYMax(new_ymax);
    ScaleFromPropertiesTo2DPlot();
}

void CPlotEditor::ScaleFromPropertiesTo2DPlot()
{
    double xMin_or, xMax_or, yMin_or, yMax_or;

    xMin_or = mPropertiesXY->GetCurrXMin();
    xMax_or = mPropertiesXY->GetCurrXMax();
    yMin_or = mPropertiesXY->GetCurrYMin();
    yMax_or = mPropertiesXY->GetCurrYMax();
    mPlot2DView->SetAxisScales(xMin_or, xMax_or, yMin_or, yMax_or);
}

//from plot to widge,properties
void CPlotEditor::RespondUpdateAxis()
{    
    mPropertiesXY->SetCurrXMin(mPlot2DView->GetXMin());
    mPropertiesXY->SetCurrXMax(mPlot2DView->GetXMax());
    mPropertiesXY->SetCurrYMin(mPlot2DView->GetYMin());
    mPropertiesXY->SetCurrYMax(mPlot2DView->GetYMax());

    mTabAxisOptions->mY_axis->mAxisMin->setText(QString::number(mPropertiesXY->GetCurrYMin()));
    mTabAxisOptions->mY_axis->mAxisMax->setText(QString::number(mPropertiesXY->GetCurrYMax()));
    mTabAxisOptions->mX_axis->mAxisMin->setText(QString::number(mPropertiesXY->GetCurrXMin()));
    mTabAxisOptions->mX_axis->mAxisMax->setText(QString::number(mPropertiesXY->GetCurrXMax()));
}


//////////////////////////////////////////////////////////////////////////
//						Curves Processing
//////////////////////////////////////////////////////////////////////////




///////////////////////////
//	From properties to plot
///////////////////////////

void CPlotEditor::Reset2DProperties( const CZFXYPlotProperties *props, CZFXYPlot *zfxyplot )
{
	assert__( mPlot2DView && mCurrentDisplayFilesProcessor );

	mPlot2DView->CreatePlot( props, zfxyplot );
	mPlot3DView->CreatePlot( props, zfxyplot );

	mDataArrayZFXY_2D = mPlot2DView->ZfxyPlotData();			assert__( mDataArrayZFXY_2D );
	mDataArrayZFXY_3D = mPlot3DView->ZfxyPlotData();			assert__( mDataArrayZFXY_3D );

	mTabCurveOptions->mFieldsList->clear();
	const size_t size = mDataArrayZFXY_2D->size();				assert__( size == mCurrentDisplayFilesProcessor->GetZFXYPlotPropertiesSize()  && size == mDataArrayZFXY_3D->size() );
	for ( size_t i = 0; i < size; ++i )
	{
		mPropertiesZFXY_2D = dynamic_cast<CZFXYPlotData*>( mDataArrayZFXY_2D->at( i ) )->GetPlotProperties();	assert__( mPropertiesZFXY_2D );
		mPropertiesZFXY_3D = dynamic_cast<CZFXYPlotData*>( mDataArrayZFXY_3D->at( i ) )->GetPlotProperties();	assert__( mPropertiesZFXY_3D );

		mTabCurveOptions->mFieldsList->addItem( t2q( mPropertiesZFXY_2D->m_name ) );							assert__( mPropertiesZFXY_2D->m_name == mPropertiesZFXY_3D->m_name );			

		mPlot2DView->ShowContour( i, mPropertiesZFXY_2D->m_withContour );		assert__( mPropertiesZFXY_2D->m_withContour == mPropertiesZFXY_3D->m_withContour );
		mPlot2DView->ShowSolidColor( i, mPropertiesZFXY_2D->m_solidColor );		assert__( mPropertiesZFXY_2D->m_solidColor == mPropertiesZFXY_3D->m_solidColor );

		mPlot3DView->ShowContour( i, mPropertiesZFXY_3D->m_withContour );		//in fact, in qwtplot3d this affects the axis (all fields) not only the current iteration one
		mPlot3DView->ShowSolidColor( i, mPropertiesZFXY_3D->m_solidColor );		//idem
	}
	
	// 2D Raster Interaction
	//
    // LeftButton for the zooming
    // MidButton for the panning
    // RightButton: zoom out by 1
    // Ctrl+RighButton: zoom out to full size
	//
    mPlot2DView->AddZoomer();
    mPlot2DView->AddPanner();

	//

	mTabCurveOptions->mLineOptions->setEnabled( false );
	mTabCurveOptions->mPointOptions->setEnabled( false );
	mTabCurveOptions->mSpectrogramOptions->setEnabled( true );

	mTabAxisOptions->mZ_axis->setEnabled( true );
	mTabAxisOptions->mZ_axis->setEnabled( true );
	mTabAxisOptions->SelectTab( 2 );					//TODO delete when properly implemented

	mTabCurveOptions->mFieldsList->setCurrentRow( 0 );
}


void CPlotEditor::Reset2DProperties( const CXYPlotProperties *props, CPlot *plot )
{
	assert__( mPlot2DView && mCurrentDisplayFilesProcessor );

	if ( mPlotType == eHistogram )									   //TODO deal with histograms in 2D and 3D when properly implemented
	{
		BRAT_MSG_NOT_IMPLEMENTED( n2s<std::string>( eHistogram ) );
	}
	else
	if ( mPlotType == eHistogram || mPlotType == eXY )
	{
		mPlot2DView->CreatePlot( props, plot, mPlotType == eHistogram );
	}
	else
	if ( mPlotType == eXYY )
	{
		// TODO display second curve in other units...........

		BRAT_MSG_NOT_IMPLEMENTED( n2s<std::string>( eXYY ) );
	}
	else
		assert__( false );

	//Create these only after invoking CreatePlot
	//
	mPlot2DView->AddMagnifier();
	mPlot2DView->AddPanner();

	mPlot2DView->EnableAxisY2( mPlotType == eXYY );
	mTabAxisOptions->mZ_axis->setEnabled( mPlotType == eXYY );
	mTabAxisOptions->SelectTab( 0 );					//TODO delete when properly implemented

	mDataArrayXY = mPlot2DView->PlotDataCollection();								assert__( mDataArrayXY );

	mTabCurveOptions->mFieldsList->clear();
	mTabCurveOptions->mFieldsList->setSelectionMode( mTabCurveOptions->mFieldsList->SingleSelection );
	const size_t size = mDataArrayXY->size();										assert__( mPlotType != eXYY || size == mCurrentDisplayFilesProcessor->GetXYPlotPropertiesSize() );
	for ( size_t i = 0; i < size; ++i )
	{
		mPropertiesXY = mDataArrayXY->Get( i )->GetPlotProperties();				assert__( mPropertiesXY );
		mTabCurveOptions->mFieldsList->addItem( t2q( mPropertiesXY->GetName() ) );

		if ( mPlotType != eHistogram )
		{
            //Fills the minimum vs maximum
            mPropertiesXY->SetXMin(mPlot2DView->GetXMin());
            mPropertiesXY->SetXMax(mPlot2DView->GetXMax());
            mPropertiesXY->SetYMin(mPlot2DView->GetYMin());
            mPropertiesXY->SetYMax(mPlot2DView->GetYMax());

            //Fills nb Ticks structure
            mPropertiesXY->SetXNumTicks(5);
            mPropertiesXY->SetYNumTicks(5);

            //Fills nb of digits
            mPropertiesXY->SetXNbDigits(mPlot2DView->GetXAxisMantissa());
            mPropertiesXY->SetYNbDigits(mPlot2DView->GetYAxisMantissa());

            //Fills CurrRange
            mPropertiesXY->SetCurrXMin(mPropertiesXY->GetXMin());
            mPropertiesXY->SetCurrXMax(mPropertiesXY->GetXMax());
            mPropertiesXY->SetCurrYMin(mPropertiesXY->GetYMin());
            mPropertiesXY->SetCurrYMax(mPropertiesXY->GetYMax());

			mPlot2DView->SetCurveLineColor( i, mPropertiesXY->GetColor() );
			mPlot2DView->SetCurveLineOpacity( i, mPropertiesXY->GetOpacity() );
			mPlot2DView->SetCurveLinePattern( i, mPropertiesXY->GetStipplePattern() );
			mPlot2DView->SetCurveLineWidth( i, mPropertiesXY->GetLineWidth() );

			mPlot2DView->SetCurvePointColor( i, mPropertiesXY->GetPointColor() );
			mPlot2DView->SetCurvePointFilled( i, mPropertiesXY->GetFilledPoint() );
			mPlot2DView->SetCurvePointGlyph( i, mPropertiesXY->GetPointGlyph() );
			mPlot2DView->SetCurvePointSize( i, mPropertiesXY->GetPointSize() );

            double x_min = mPropertiesXY->GetCurrXMin();
            double x_max = mPropertiesXY->GetCurrXMax();

            double y_min = mPropertiesXY->GetCurrYMin();
            double y_max = mPropertiesXY->GetCurrYMax();

            mPlot2DView->GenScaleX(x_min, x_max, (x_max-x_min)/5);		//TODO 5 ?????
            mPlot2DView->GenScaleY(y_min, y_max, (y_max-y_min)/5);		//TODO 5 ?????

		}
	}

	mTabCurveOptions->mFieldsList->setCurrentRow( 0 );
}



///////////////////////////
//	From plot to widgets
///////////////////////////

void CPlotEditor::HandleCurrentFieldChanged( int index )
{
    int nb_ticks=0;
	assert__( mPlot2DView );

	mTabCurveOptions->mLineOptions->setEnabled( index >= 0 && !mDataArrayZFXY_2D && !mDataArrayZFXY_3D && mPlotType != eHistogram );
	mTabCurveOptions->mPointOptions->setEnabled( index >= 0 && !mDataArrayZFXY_2D && !mDataArrayZFXY_3D && mPlotType != eHistogram );
	mTabCurveOptions->mSpectrogramOptions->setEnabled( index >= 0 && mDataArrayZFXY_2D && mDataArrayZFXY_3D );

	mPropertiesXY = nullptr;
	mPropertiesZFXY_2D = nullptr;
	mPropertiesZFXY_3D = nullptr;

	if ( index < 0 )
		return;

	if ( mDataArrayZFXY_2D )
	{
		assert__( mDataArrayZFXY_3D && index < mDataArrayZFXY_2D->size() && mDataArrayZFXY_2D->size() == mDataArrayZFXY_3D->size() );

		mPropertiesZFXY_2D = dynamic_cast<CZFXYPlotData*>( mDataArrayZFXY_2D->at( index ) )->GetPlotProperties();				assert__( mPropertiesZFXY_2D );
		mPropertiesZFXY_3D = dynamic_cast<CZFXYPlotData*>( mDataArrayZFXY_3D->at( index ) )->GetPlotProperties();				assert__( mPropertiesZFXY_3D );

		mPlot2DView->SetCurrentRaster( index );
		mPlot3DView->SetCurrentPlot( index );

		//TODO temporarily here, while axis work is not done
		mPlot2DView->SetAxisTitles( mPropertiesZFXY_2D->m_xLabel, mPropertiesZFXY_2D->m_yLabel, mPropertiesZFXY_2D->m_name );
		mPlot3DView->SetAxisTitles( mPropertiesZFXY_3D->m_xLabel, mPropertiesZFXY_3D->m_yLabel, mPropertiesZFXY_3D->m_name );

		mTabCurveOptions->mShowContour->setChecked( mPlot2DView->HasContour() );			assert__( mPlot2DView->HasContour() == mPlot3DView->HasContour() );
		mTabCurveOptions->mShowSolidColor->setChecked( mPlot2DView->HasSolidColor() );		assert__( mPlot2DView->HasSolidColor() == mPlot3DView->HasSolidColor() );

		return;
	}

	assert__( index < mDataArrayXY->size() );

	mPropertiesXY = mDataArrayXY->Get( index )->GetPlotProperties();									   				assert__( mPropertiesXY );

	if ( mPlotType == eHistogram )
	{
		return;
	}

	mTabCurveOptions->mLineColorButton->SetColor( mPlot2DView->CurveLineColor( index ) );
	mTabCurveOptions->mLineOpacityValue->setText( n2s<std::string>( mPlot2DView->CurveLineOpacity( index ) ).c_str() );
	mTabCurveOptions->mStipplePattern->setCurrentIndex( mPlot2DView->CurveLinePattern( index ) );
	mTabCurveOptions->mLineWidthValue->setText( n2s<std::string>( mPlot2DView->CurveLineWidth( index ) ).c_str() );


	mTabCurveOptions->mPointColorButton->SetColor( mPlot2DView->CurvePointColor( index ) );
	mTabCurveOptions->mFillPointCheck->setChecked( mPlot2DView->IsCurvePointFilled( index ) );
	mTabCurveOptions->mPointGlyph->setCurrentIndex( mPlot2DView->CurvePointGlyph( index ) );
	mTabCurveOptions->mPointSizeValue->setText( n2s<std::string>( mPlot2DView->CurvePointSize( index ) ).c_str() );

	mTabCurveOptions->mLineOptions->setChecked( mPlotType != eHistogram && mPropertiesXY->GetLines() );
	mTabCurveOptions->mPointOptions->setChecked( mPropertiesXY->GetPoints() );

    assert__( mTabAxisOptions->mX_axis );
    mTabAxisOptions->mX_axis->mAxisLabel->setText(t2q(mPropertiesXY->GetXLabel()));
    mTabAxisOptions->mX_axis->mBase->setText(QString::number(mPropertiesXY->GetXBase()));
    nb_ticks = mPropertiesXY->GetXNumTicks();
    if (nb_ticks < 0)
    {
        mTabAxisOptions->mX_axis->mNbTicks->setText("No ticks");
    }
    else
    {
        mTabAxisOptions->mX_axis->mNbTicks->setText(QString::number(mPropertiesXY->GetXNumTicks()));
    }
    mTabAxisOptions->mX_axis->mLogScaleCheck->setChecked(mPropertiesXY->GetXLog());
    mTabAxisOptions->mX_axis->mNbDigits->setText(QString::number(mPropertiesXY->GetXNbDigits()));

    //Curve's max and min
    mTabAxisOptions->mX_axis->mAxisMin->setText(QString::number(mPropertiesXY->GetXMin()));
    mTabAxisOptions->mX_axis->mAxisMax->setText(QString::number(mPropertiesXY->GetXMax()));

    assert__( mTabAxisOptions->mY_axis );
    mTabAxisOptions->mY_axis->mAxisLabel->setText(t2q(mPropertiesXY->GetYLabel()));
    mTabAxisOptions->mY_axis->mBase->setText(QString::number(mPropertiesXY->GetYBase()));
    nb_ticks = mPropertiesXY->GetYNumTicks();
    if (nb_ticks < 0)
    {
        mTabAxisOptions->mY_axis->mNbTicks->setText("No ticks");
    }
    else
    {
        mTabAxisOptions->mY_axis->mNbTicks->setText(QString::number(mPropertiesXY->GetYNumTicks()));
    }
    mTabAxisOptions->mY_axis->mLogScaleCheck->setChecked(mPropertiesXY->GetYLog());
    mTabAxisOptions->mY_axis->mNbDigits->setText(QString::number(mPropertiesXY->GetYNbDigits()));

    //Curve's max and min
    mTabAxisOptions->mY_axis->mAxisMin->setText(QString::number(mPropertiesXY->GetYMin()));
    mTabAxisOptions->mY_axis->mAxisMax->setText(QString::number(mPropertiesXY->GetYMax()));



}




///////////////////////////////////////////////////////
//	From widgets to properties, from properties to plot
///////////////////////////////////////////////////////


//spectrogram  properties

void CPlotEditor::HandleShowContourChecked( bool checked )
{
	assert__( mPlot2DView && mPlot3DView && mDataArrayZFXY_2D && mPropertiesZFXY_3D && mDataArrayZFXY_2D && mPropertiesZFXY_3D );

	mPropertiesZFXY_2D->m_withContour = checked;
	mPropertiesZFXY_3D->m_withContour = checked;

	mPlot2DView->ShowContour( mPropertiesZFXY_2D->m_withContour );
	mPlot3DView->ShowContour( mPropertiesZFXY_3D->m_withContour );
}

void CPlotEditor::HandleShowSolidColorChecked( bool checked )
{
	assert__( mPlot2DView && mPlot3DView && mDataArrayZFXY_2D && mPropertiesZFXY_3D && mDataArrayZFXY_2D && mPropertiesZFXY_3D );

	mPropertiesZFXY_2D->m_solidColor = checked;
	mPropertiesZFXY_3D->m_solidColor = checked;

	mPlot2DView->ShowSolidColor( mPropertiesZFXY_2D->m_solidColor );
	mPlot3DView->ShowSolidColor( mPropertiesZFXY_3D->m_solidColor );
}



//line properties

void CPlotEditor::HandleCurveLineColorSelected()
{
	assert__( mPlot2DView && mPropertiesXY );

    QColor selected_color = mTabCurveOptions->mLineColorButton->GetColor();

    mPropertiesXY->SetColor( selected_color );

	mPlot2DView->SetCurveLineColor( mTabCurveOptions->mFieldsList->currentRow(), color_cast< QColor >( mPropertiesXY->GetColor() ) );
}

void CPlotEditor::HandleCurveLineOpacityEntered()
{
	assert__( mPlot2DView && mPropertiesXY );

    bool is_converted=false;
    const QString opacity = mTabCurveOptions->mLineOpacityValue->text();
    mPropertiesXY->SetOpacity( opacity.toInt(&is_converted, 10) );			//TODO VALIDATE

	mPlot2DView->SetCurveLineOpacity( mTabCurveOptions->mFieldsList->currentRow(), mPropertiesXY->GetOpacity() );
}

void CPlotEditor::HandleStipplePatternChanged( int pattern )
{
	assert__( pattern >= eFirstStipplePattern && pattern < EStipplePattern_size );

    mPropertiesXY->SetStipplePattern( (EStipplePattern)pattern );
	mPlot2DView->SetCurveLinePattern( mTabCurveOptions->mFieldsList->currentRow(), mPropertiesXY->GetStipplePattern() );
}

void CPlotEditor::HandleCurveLineWidthEntered()
{
	assert__( mPlot2DView && mPropertiesXY );

    bool is_converted=false;
    const QString width = mTabCurveOptions->mLineWidthValue->text();
	mPropertiesXY->SetLineWidth( width.toInt( &is_converted, 10 ) );			//TODO VALIDATE
	mPlot2DView->SetCurveLineWidth( mTabCurveOptions->mFieldsList->currentRow(), mPropertiesXY->GetLineWidth() );
}


//curve properties

void CPlotEditor::HandleCurvePointColorSelected()
{
	assert__( mPlot2DView && mPropertiesXY );

    QColor selected_color = mTabCurveOptions->mPointColorButton->GetColor();
    mPropertiesXY->SetPointColor( selected_color );
	mPlot2DView->SetCurvePointColor( mTabCurveOptions->mFieldsList->currentRow(), mPropertiesXY->GetPointColor() );
}

void CPlotEditor::HandleFillGlyphInterior( bool checked )
{
	assert__( mPlot2DView && mPropertiesXY );

    mPropertiesXY->SetFilledPoint( checked );

	mPlot2DView->SetCurvePointFilled( mTabCurveOptions->mFieldsList->currentRow(), mPropertiesXY->GetFilledPoint() );
}

void CPlotEditor::HandleGlyphPatternChanged( int pattern )
{
	assert__( mPlot2DView && mPropertiesXY && pattern >= eFirstPointGlyph && pattern < EPointGlyph_size );

    mPropertiesXY->SetPointGlyph( (EPointGlyph)pattern );

	mPlot2DView->SetCurvePointGlyph( mTabCurveOptions->mFieldsList->currentRow(), mPropertiesXY->GetPointGlyph() );
}

void CPlotEditor::HandleCurveGlyphWidthEntered()
{
	assert__( mPlot2DView && mPropertiesXY );

    bool is_converted=false;
    const QString pointw = mTabCurveOptions->mPointSizeValue->text();		//TODO validate
    int w_value = pointw.toInt(&is_converted, 10);

    mPropertiesXY->SetPointSize( w_value );
	mPlot2DView->SetCurvePointSize( mTabCurveOptions->mFieldsList->currentRow(), mPropertiesXY->GetPointSize() );
}



//enable/disable line/curve properties

void CPlotEditor::HandleLineOptionsChecked( bool checked )
{
	assert__( mPlot2DView  && mPropertiesXY );

    mPropertiesXY->SetLines( checked );
	mPlot2DView->EnableCurveLine( mTabCurveOptions->mFieldsList->currentRow(), mPropertiesXY->GetLines() );
}

void CPlotEditor::HandlePointOptionsChecked( bool checked )
{
	assert__( mPlot2DView && mPropertiesXY );

    mPropertiesXY->SetPoints( checked );

	mPlot2DView->EnableCurvePoints( mTabCurveOptions->mFieldsList->currentRow(), mPropertiesXY->GetPoints(), mPropertiesXY->GetPointGlyph() );
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_PlotEditor.cpp"
