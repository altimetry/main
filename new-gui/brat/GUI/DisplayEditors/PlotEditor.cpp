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
#include "new-gui/brat/stdafx.h"

#include "BratLogger.h"

#include "DataModels/MapTypeDisp.h"
#include "DataModels/Workspaces/Display.h"
#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/PlotData/Plots.h"

#include "GUI/ControlPanels/ColorButton.h"
#include "GUI/ControlPanels/Views/ViewControlPanels.h"
#include "GUI/DisplayWidgets/3DPlotWidget.h"
#include "GUI/DisplayWidgets/2DPlotWidget.h"
#include "new-gui/Common/GUI/TextWidget.h"

#include "PlotEditor.h"


void CPlotEditor::CreateWidgets() 	//parent = nullptr
{
	base_t::CreateWidgets<CViewControlsPanelGeneralPlots>();

	mTabCurveOptions = new CPlotControlsPanelCurveOptions( this );
	mTabAxisOptions = new CPlotControlsPanelAxisOptions( this );
    mTabAnimateOptions = new CPlotControlsPanelAnimateOptions( this );

	AddTab( mTabCurveOptions, "Data Options" );
	AddTab( mTabAxisOptions, "Axis Options" );
    AddTab( mTabAnimateOptions, "Animation" );

	//Even if ResetViews destroys them before first use, 
	//	it is important to start with valid views
	//
	mPlot2DView = new C2DPlotWidget( this );
	mPlot3DView = new C3DPlotWidget( this );
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

    //...labels
    connect( mTabAxisOptions->mXAxisLabel, SIGNAL( returnPressed() ), this, SLOT( HandleXAxisLabelChanged( ) ) );
    connect( mTabAxisOptions->mYAxisLabel, SIGNAL( returnPressed() ), this, SLOT( HandleYAxisLabelChanged( ) ) );
    connect( mTabAxisOptions->mZAxisLabel, SIGNAL( returnPressed() ), this, SLOT( HandleZAxisLabelChanged( ) ) );

    //...ticks
    connect( mTabAxisOptions->mXNbTicks, SIGNAL( returnPressed() ), this, SLOT( HandleXNbTicksChanged( ) ) );
    connect( mTabAxisOptions->mYNbTicks, SIGNAL( returnPressed() ), this, SLOT( HandleYNbTicksChanged( ) ) );
    connect( mTabAxisOptions->mZNbTicks, SIGNAL( returnPressed() ), this, SLOT( HandleZNbTicksChanged( ) ) );

    //...scale
	connect( mTabAxisOptions->mX2DScaleSpin, SIGNAL( valueChanged( double ) ), this, SLOT( HandleX2DScaleSpinChanged( double ) ) );
	connect( mTabAxisOptions->mY2DScaleSpin, SIGNAL( valueChanged( double ) ), this, SLOT( HandleY2DScaleSpinChanged( double ) ) );
	mTabAxisOptions->mZ2DScaleSpin->setEnabled( false );	//do not allow user to change the color axis...

	connect( mTabAxisOptions->mX3DScaleSpin, SIGNAL( valueChanged( double ) ), this, SLOT( HandleX3DScaleSpinChanged( double ) ) );
	connect( mTabAxisOptions->mY3DScaleSpin, SIGNAL( valueChanged( double ) ), this, SLOT( HandleY3DScaleSpinChanged( double ) ) );
	connect( mTabAxisOptions->mZ3DScaleSpin, SIGNAL( valueChanged( double ) ), this, SLOT( HandleZ3DScaleSpinChanged( double ) ) );

    //...digits
    connect( mTabAxisOptions->mXNbDigits, SIGNAL (returnPressed()), this, SLOT( HandleXAxisNbDigitsChanged()));
    connect( mTabAxisOptions->mYNbDigits, SIGNAL (returnPressed()), this, SLOT( HandleYAxisNbDigitsChanged()));
    connect( mTabAxisOptions->mZNbDigits, SIGNAL (returnPressed()), this, SLOT( HandleZAxisNbDigitsChanged()));

    //...logarithmic
	//connect( mTabAxisOptions->mX_axis->mLogScaleCheck, SIGNAL( toggled( bool ) ), this, SLOT( HandleLogarithmicScaleX( bool ) ) );
	//connect( mTabAxisOptions->mY_axis->mLogScaleCheck, SIGNAL( toggled( bool ) ), this, SLOT( HandleLogarithmicScaleY( bool ) ) );
	connect( mTabAxisOptions->mZLogScaleCheck, SIGNAL( toggled( bool ) ), this, SLOT( HandleLogarithmicScaleZ( bool ) ) );


    // spectrogram / 3d

	connect( mTabCurveOptions->mColorMapWidget, SIGNAL( ShowContourToggled( bool ) ), this, SLOT( HandleShowContourChecked( bool ) ) );
	connect( mTabCurveOptions->mColorMapWidget, SIGNAL( ContoursEditReturnPressed() ), this, SLOT( HandleNumberOfContoursChanged() ) );
	connect( mTabCurveOptions->mColorMapWidget, SIGNAL( ShowSolidColorToggled( bool ) ), this, SLOT( HandleShowSolidColorChecked( bool ) ) );
	connect( mTabCurveOptions->mColorMapWidget, SIGNAL( CurrentIndexChanged( int ) ), this, SLOT( HandleColorTablesIndexChanged( int ) ) );


    // histogram

	connect( mTabCurveOptions->mNumberOfBins, SIGNAL( returnPressed() ), this, SLOT( HandleNumberOfBinsChanged( ) ) );

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


	// animation

	connect( mTabAnimateOptions->mAnimateButton, SIGNAL( toggled( bool ) ), this, SLOT( HandleAnimateButtonToggled( bool ) ) );
	connect( mTabAnimateOptions->mLoopButton, SIGNAL( toggled( bool ) ), this, SLOT( HandleAnimateLoop( bool ) ) );
	connect( mTabAnimateOptions->mMoveBackwardButton, SIGNAL( toggled( bool ) ), this, SLOT( HandleAnimateBackwardToggled( bool ) ) );
	connect( mTabAnimateOptions->mMoveForwardButton, SIGNAL( toggled( bool ) ), this, SLOT( HandleAnimateForwardToggled( bool ) ) );
	connect( mTabAnimateOptions->mStopButton, SIGNAL( clicked() ), this, SLOT( HandleStopAnimationButton() ) );
	connect( mTabAnimateOptions->mFrameIndexSpin, SIGNAL( valueChanged( int ) ), this, SLOT(HandleFrameIndexSpinChanged( int ) ) );
}



CPlotEditor::CPlotEditor( CModel *model, const COperation *op, const std::string &display_name ) 	//display_name = ""
	: base_t( false, model, op, display_name )
{
    CreateWidgets();

	Start( display_name );
}


CPlotEditor::CPlotEditor( const CDisplayDataProcessor *proc, CYFXPlot* plot )
	: base_t( false, proc )
{
    Q_UNUSED(proc);     Q_UNUSED(plot);

    CreateWidgets();

	Start( "" );
}

CPlotEditor::CPlotEditor( const CDisplayDataProcessor *proc, CZFXYPlot* plot )
	: base_t( false, proc )
{
    Q_UNUSED(proc);     Q_UNUSED(plot);

    CreateWidgets();

	Start( "" );
}

CPlotEditor::CPlotEditor( const CDisplayFilesProcessor *proc, CYFXPlot* plot )
	: base_t( false, proc )
{
    Q_UNUSED( plot );
	BRAT_MSG_NOT_IMPLEMENTED( "Critical Error: using CDisplayFilesProcessor" )
}

CPlotEditor::CPlotEditor( const CDisplayFilesProcessor *proc, CZFXYPlot* plot )
	: base_t( false, proc )
{
    Q_UNUSED( plot );
	BRAT_MSG_NOT_IMPLEMENTED( "Critical Error: using CDisplayFilesProcessor" )
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
	//if ( checked && !mPlot3DView->isVisible() && parentWidget() )
	//	parentWidget()->resize( minimumSizeHint());
}
//virtual 
void CPlotEditor::Show3D( bool checked )
{
	assert__( mPlot3DView );

	mPlot3DView->setVisible( checked );
	//if ( checked && !mPlot2DView->isVisible() && parentWidget() )
	//	parentWidget()->resize( minimumSizeHint());
}

//virtual 
void CPlotEditor::Recenter()
{
	mPlot2DView->Home();
	if ( mPlot3DView )
		mPlot3DView->Home();
}


//virtual 
void CPlotEditor::Export2Image( bool save_2d, bool save_3d, const std::string path2d, const std::string path3d, EImageExportType type )
{
	assert__( !save_2d || mPlot2DView && !save_3d || mPlot3DView );

	QString extension = CDisplayData::ImageType2String( type ).c_str();
	QString format = type == CDisplayData::ePnm ? "ppm" : extension;

	if ( save_2d && !mPlot2DView->Save2Image( path2d.c_str(), format, extension ) )
		SimpleWarnBox( "There was an error saving file " + path2d );

	if ( save_3d && !mPlot3DView->Save2Image( path3d.c_str(), format, extension ) )
		SimpleWarnBox( "There was an error saving file " + path3d );
}




//virtual 
bool CPlotEditor::ResetViews( bool reset_2d, bool reset_3d, bool enable_2d, bool enable_3d )
{
	assert__( enable_2d );		//2d displays all types

	const int min_width = enable_3d ? min_plot_widget_width / 2 : min_plot_widget_width;

	if ( reset_2d && mPlot2DView )
	{
		auto *p = mPlot2DView;
		mPlot2DView = nullptr;
		if ( !RemoveView( p, false, true ) )
			return false;
	}
	if ( enable_2d && !mPlot2DView )
	{
		mPlot2DView = new C2DPlotWidget( this );
		mPlot2DView->setMinimumSize( min_width, min_plot_widget_height );
		AddView( mPlot2DView, false );
		connect( mPlot2DView, SIGNAL( ScaleDivChanged( int, double, QString ) ), this, SLOT( Handle2DScaleChanged( int, double, QString ) ) );
	}

	if ( reset_3d && mPlot3DView )
	{
		auto *p = mPlot3DView;
		mPlot3DView = nullptr;
		if ( !RemoveView( p, true, true ) )
			return false;
	}
	if ( enable_3d && !mPlot3DView )
	{
		mPlot3DView = new C3DPlotWidget( this );
		mPlot3DView->setMinimumSize( min_width, min_plot_widget_height );
		AddView( mPlot3DView, true );
		connect( mPlot3DView, SIGNAL( ScaleChanged( double, double, double ) ), this, SLOT( Handle3DScaleChanged( double, double, double ) ) );
	}
	return true;
}


//virtual 
void CPlotEditor::NewButtonClicked()
{}
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
//virtual 
void CPlotEditor::SetPlotTitle()
{
	if ( mPlot2DView )
	{
		mPlot2DView->setWindowTitle( t2q( mDisplay->Title() ) );
		mPlot2DView->SetPlotTitle( q2a( mPlot2DView->windowTitle() ) );		//in v3 was inside the loop
	}

	if ( mPlot3DView )
	{
		mPlot3DView->setWindowTitle( t2q( mDisplay->Title() ) );
		mPlot3DView->SetPlotTitle( mDisplay->Title() );
	}
}


//virtual 
bool CPlotEditor::Test()
{
	int index = mTabCurveOptions->mFieldsList->currentRow();
	if ( index < 0 )
	{
		LOG_WARN( "Test: No data" );
		return false;
	}

	double all_mx, all_Mx, all_my, all_My, all_mz, all_Mz, mx, Mx, my, My, mz, Mz;
	all_mx = all_Mx = all_my = all_My = all_mz = all_Mz = mx = Mx = my = My = mz = Mz = 0.;
	size_t nframes, nvalues, nvalues0, ndefault_values, nan_values;
	nframes = nvalues = nvalues0 = ndefault_values = nan_values = 0;
	int xticks, yticks, zticks, xdigits, ydigits, zdigits;
	xticks = yticks = zticks = xdigits = ydigits = zdigits = 0;

	std::string error_msg, field;
	CPlotInterfaces *pgeneric = nullptr;

	if ( mCurrentPlotYFX )
	{
		assert__( !mCurrentPlotZFXY );

		pgeneric = mCurrentPlotYFX;
		CXYPlotField *pdata = mCurrentPlotYFX->PlotData( index );
		const CYFXValues &data = *pdata;		//unit = GetDisplayData( index, mOperation, mDisplay, mCurrentPlotYFX );		Q_UNUSED( unit );
		field = pdata->WidgetFieldName();

		data.GetXRange( mx, Mx, data.CurrentFrame() );
		data.GetYRange( my, My, data.CurrentFrame() );
		data.GetXRange( all_mx, all_Mx );
		data.GetYRange( all_my, all_My );

		nframes = data.GetNumberOfFrames();
		int iframe = 0;
		nvalues = data.GetDataSize( iframe );
        for ( ; iframe < (int)nframes; ++iframe )
			if ( data.GetDataSize( iframe ) != nvalues )
				error_msg += "2D frames with different size\n";

		nvalues0 = data.GetDataCountIf( 0, []( const double &v ) { return v == 0; } );
		ndefault_values = data.GetDataCountIf( 0, []( const double &v ) { return isDefaultValue( v ); } );
		nan_values = data.GetDataCountIf( 0, []( const double &v ) { return std::isnan( v ); } );

		zticks = pgeneric->ValueTicks();
		zdigits = pgeneric->ValueDigits();
	}
	if ( mCurrentPlotZFXY )
	{
		assert__( !mCurrentPlotYFX );

		pgeneric = mCurrentPlotZFXY;
		CZFXYPlotField *pdata = mCurrentPlotZFXY->PlotData( index );
		const CZFXYValues &data = *pdata;		//unit = GetDisplayData( index, mOperation, mDisplay, mCurrentPlotZFXY );		Q_UNUSED( unit );
		field = pdata->GetPlotProperties()->FieldName();
		assert__( pdata->WidgetFieldName() == field );

		all_mx = mx = data[ 0 ].mMinX;
		all_Mx = Mx = data[ 0 ].mMaxX; 
		all_my = my = data[ 0 ].mMinY;
		all_My = My = data[ 0 ].mMaxX;
		all_mz = mz = data[ 0 ].mMinHeightValue;
		all_Mz = Mz = data[ 0 ].mMaxHeightValue;

		if ( data.size() > 1 )
			error_msg += "Unexpected 3D number of frames greater than 1\n";

		nframes = data.size();					assert__( nframes == 1 );	//#frames; simply to check if ever...
		int iframe = 0;
		nvalues = data.GetDataSize( iframe );
        for ( ; iframe < (int)nframes; ++iframe )
		{
			if ( data.GetDataSize( iframe ) != nvalues )
				error_msg += "3D frames with different size\n";
		}

		nvalues0 = data.GetDataCountIf( 0, []( const double &v ) { return v == 0; } );
		ndefault_values = data.GetDataCountIf( 0, []( const double &v ) { return isDefaultValue( v ); } );
		nan_values = data.GetDataCountIf( 0, []( const double &v ) { return std::isnan( v ); } );

		zticks = pgeneric->ValueTicks( index );
		zdigits = pgeneric->ValueDigits( index );
	}

	assert__( pgeneric );

	xticks = pgeneric->Xticks();
	yticks = pgeneric->Yticks();
	xdigits = pgeneric->Xdigits();
	ydigits = pgeneric->Ydigits();

	const std::string msg =
		+ "\nData dump for '"
		+ field
		+ "'\n"
		+ TF( "mx", mx )
		+ TF( "Mx", Mx )
		+ TF( "all_mx", all_mx )
		+ TF( "all_Mx", all_Mx )
		+ TF( "my", my )
		+ TF( "My", My )
		+ TF( "all_my", all_my )
		+ TF( "all_My", all_My )
		+ TF( "mz", mz )
		+ TF( "Mz", Mz )
		+ TF( "all_mz", all_mz )
		+ TF( "all_Mz", all_Mz )
		+ TF( "nvalues", nvalues )
		+ TF( "nframes", nframes )
		+ TF( "nvalues0", nvalues0 )
		+ TF( "ndefault_values", ndefault_values )
		+ TF( "nan_values", nan_values )
		+ TF( "--------", "" )
		+ TF( "xticks", xticks, "; " )
		+ TF( "yticks", yticks, "; " )
		+ TF( "zticks", zticks )
		+ TF( "xdigits", xdigits, "; " )
		+ TF( "ydigits", ydigits, "; " )
		+ TF( "zdigits", zdigits )
		+ error_msg
		;

	LOG_FATAL( msg );

	return true;
}



//slot
void CPlotEditor::HandlePlotTypeChanged( int index )
{
	assert__( index >= 0 );

	EPlotType type = (EPlotType)index;			assert__( type < EPlotTypes_size );

	mPlotType = type;
	//
	//	only 2D view must be reset when plot type changes through plot types list
	//
	if ( !ResetViews( true, false, true, false ) )
		throw CException( "A previous plot is still processing. Please try again later." );

	SelectTab( TabGeneral()->parentWidget() );

	if ( mCurrentPlotYFX )
	{
		Recreate2DPlots();
	}
	else
	if ( mCurrentPlotZFXY )
	{
		Recreate3DPlots( true, false );
	}
	else
        assert( false );
}


void CPlotEditor::UpdatePlotTypeList( EPlotType plot_type, bool select )
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



//
//	CXYPlotField		is a	CBratObject							
//					has a	vtkDataArrayPlotData
//								has a vtkDoubleArrayBrathl *XDataArray;
//								has a vtkDoubleArrayBrathl *YDataArray;						
//								has a vtkDoubleArrayBrathl *ZDataArray;						
//										
//	CPlotArray	has a 	vtkObArray (array of frames)
//
//
//	CXYPlotField		=> Create	=> SetData( 
//	(has a vtkDataArrayPlotData)			double* xArray, const CUIntArray& xDims, 
//											double* yArray, const CUIntArray& yDims,
//											const CUIntArray& xCommonDimIndex,
//											const CUIntArray& yCommonDimIndex ) => ...
//
//	... => CPlotArray::SetFrameData for each frame, 
//		and at the end:
//	... => SetRawData( CPlotArray* x, CPlotArray* y ) => Update() => m_vtkDataArrayPlotData::SetDataArrays( vtkDoubleArrayBrathl, vtkDoubleArrayBrathl )
//
//virtual 
bool CPlotEditor::ChangeView()
{
	EPlotType type = mDisplay->IsYFXType() ? eXY : eXYZ;
    UpdatePlotTypeList( type, false );


	WaitCursor wait;

	try
	{
		//	I . Assign current plot type 

		mPlotType = type;							assert__( mPlotType != eHistogram );
		
		//	II . Reset "current" pointers

		delete mCurrentPlotYFX;
		mCurrentPlotYFX = nullptr;
		mCurrentPlotFieldYFX = nullptr;

		delete mCurrentPlotZFXY;
		mCurrentPlotZFXY = nullptr;
		mCurrentPlotFieldZFXY = nullptr;

		//	III . Get plots (v3 plot definitions) from display file

		std::vector< CYFXPlot* > yfx_plots;
		std::vector< CZFXYPlot* > zfxy_plots;
		if ( mDisplay->IsYFXType() )
        {
            yfx_plots = GetPlotsFromDisplayData< CYFXPlot >( mDisplay );            //yfx_plots = GetPlotsFromDisplayData< CYFXPlot >( mOperation );
        }
        else
		if ( mDisplay->IsZYFXType() || mDisplay->IsZLatLonType() )
		{
            zfxy_plots = GetPlotsFromDisplayData< CZFXYPlot >( mDisplay );            //zfxy_plots = GetPlotsFromDisplayData< CZFXYPlot >( mOperation );
		}
		else
			assert__( false );						
		
		assert__( ( yfx_plots.size() || zfxy_plots.size() ) && CurrentDisplayDataProcessor() );
		assert__( yfx_plots.size() <= 1 && zfxy_plots.size() <= 1 );		//forces redesign if false


		//	IV . Reset views and select general tab 

		if ( !ResetViews( true, mPlotType != eHistogram, true, zfxy_plots.size() > 0 ) )
			throw CException( "A previous plot is still processing. Please try again later." );
		SelectTab( TabGeneral()->parentWidget() );


		//	V . Process the plot definitions: create plot data and plots

		for ( auto *yfxplot : yfx_plots )
		{
			mCurrentPlotYFX = yfxplot;				assert__( mCurrentPlotYFX != nullptr && ( mPlotType == eXY || mPlotType == eHistogram ) );

            Recreate2DPlots();
		}

		
		for ( auto *zfxyplot : zfxy_plots )
		{
			mCurrentPlotZFXY = zfxyplot;			assert__( mCurrentPlotZFXY != nullptr && ( mPlotType == eXYZ || mPlotType == eHistogram ) );

			Recreate3DPlots( true, true );
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

	
    ResetViews( true, true, true, false );

	return false;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//											Graphics Processing
//
//	I.		From data to plot (plot creation)
//	II.		From plots to widgets (handle field selection change && assign "current" pointers)
//	III.	From widgets to data, from data to plot (handle properties change)
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////



bool CPlotEditor::UpdateCurrentPointers( int field_index )
{
	mCurrentPlotFieldYFX = nullptr;
	mCurrentPlotFieldZFXY = nullptr;

	if ( field_index < 0 )
        return false;

	assert__( mOperation && mDisplay );

	if ( mCurrentPlotZFXY )
	{
		assert__( field_index < (int)mCurrentPlotZFXY->Size() );

		mCurrentPlotFieldZFXY = mCurrentPlotZFXY->PlotData( field_index );	assert__( mCurrentPlotFieldZFXY && *mCurrentPlotFieldZFXY && mCurrentPlotFieldZFXY->size() == 1 );	//simply to check if ever...
	}
	else
	{
		assert__( field_index < (int)mCurrentPlotYFX->Size() );

		mCurrentPlotFieldYFX = mCurrentPlotYFX->PlotData( field_index );		assert__( mCurrentPlotFieldYFX && *mCurrentPlotFieldYFX );			//size >= 1 for animated
	}

	return mCurrentPlotFieldYFX != nullptr || mCurrentPlotFieldZFXY != nullptr;
}



//helper

double CPlotEditor::CreateHistogram()
{
	CWidgetField *pdata = mCurrentPlotFieldYFX ? static_cast<CWidgetField*>( mCurrentPlotFieldYFX ) : static_cast<CWidgetField*>( mCurrentPlotFieldZFXY );		assert__( pdata );

	const CYFXValues *yfx_arrays = dynamic_cast< CYFXValues* >( pdata );
	const CZFXYValues *zfxy_arrays = dynamic_cast< CZFXYValues* >( pdata );

	
	double max_freq;
	if ( zfxy_arrays )
		mPlot2DView->SetSingleHistogram( pdata->WidgetFieldName(), color_cast<QColor>( pdata->LineColor() ), *zfxy_arrays, max_freq, pdata->NumberOfBins(),
			pdata->DataUnit()->IsDate(), RefDateFromUnit(*pdata->DataUnit()));
	else
		mPlot2DView->PushHistogram( pdata->WidgetFieldName(), color_cast<QColor>( pdata->LineColor() ), *yfx_arrays, max_freq, pdata->NumberOfBins(),
			pdata->DataUnit()->IsDate(), RefDateFromUnit(*pdata->DataUnit()));

	return max_freq;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//									I. From properties to plot (plot creation)
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////
// I.1.	z=f(x,y)
//////////////////


void CPlotEditor::Recreate3DPlots( bool build2d, bool build3d )
{
	assert__( mPlot2DView && CurrentDisplayDataProcessor() && mCurrentPlotZFXY );

	mMultiFrame = false;
	int n_frames = 1;
	mTabCurveOptions->mFieldsList->clear();

	const size_t size = mCurrentPlotZFXY->Size();			assert__( size == CurrentDisplayDataProcessor()->GetZFXYPlotPropertiesSize() );
	for ( size_t i = 0; i < size; ++i )
	{
		/////////////////////////////////////////////////
		//				update pointers
		/////////////////////////////////////////////////

		bool result = UpdateCurrentPointers( i );			assert__( result );		Q_UNUSED( result );

		if ( n_frames == 1 )
			n_frames = mCurrentPlotFieldZFXY->size();
#if defined (DEBUG) || defined(_DEBUG)				//make sure the number of frames is the same for all data
		else
			assert__( n_frames == mCurrentPlotFieldZFXY->size() );
#endif

		/////////////////////////////////////////////////
		//		create field plot, one widget per plot
		/////////////////////////////////////////////////

		// void CZFXYPlotPanel::AddData( CZFXYPlotField* pdata, wxProgressDialog* dlg )

		double m, M;
		mCurrentPlotFieldZFXY->GetDataRange( m, M );

		if ( build2d )
		{
			if ( mPlotType == eHistogram )
			{
				//2d Histograms: assign axis titles

				double max_freq = CreateHistogram();

				//axis scale ranges

				mPlot2DView->SetPlotAxisRanges( i, m, M, 0, max_freq );
			}
			else
			{
				//2d Rasters: assign axis scale ranges, color table

				mPlot2DView->PushRaster( 
					mCurrentPlotFieldZFXY->WidgetFieldName(), *mCurrentPlotFieldZFXY, mCurrentPlotFieldZFXY->MinContourValue(),
					mCurrentPlotFieldZFXY->MaxContourValue(), mCurrentPlotFieldZFXY->NumContours(), 
					*mCurrentPlotFieldZFXY->ColorTablePtr()->GetLookupTable()
					);

				//axis titles

				mPlot2DView->SetSpectrogramAxisTitles( i, mCurrentPlotZFXY->Xtitle(), mCurrentPlotZFXY->Ytitle(), mCurrentPlotZFXY->ValueTitle( i ) );
			}
		}

		if ( build3d )
		{
			//3d: assign color table

			mPlot3DView->PushPlot( mCurrentPlotFieldZFXY->at( 0 ), new QLookupTable( *mCurrentPlotFieldZFXY->ColorTablePtr()->GetLookupTable() ) );

			//axis titles

			mPlot3DView->SetAllXYAxisTitles( mCurrentPlotZFXY->Xtitle(), mCurrentPlotZFXY->Ytitle() );	//could be out of the loop
			mPlot3DView->SetZAxisTitle( i, mCurrentPlotZFXY->ValueTitle( i ) );
		}

		/////////////////////////////////////////////////
		//				add field to list
		/////////////////////////////////////////////////

		mTabCurveOptions->mFieldsList->addItem( MakeFieldItem( mCurrentPlotFieldZFXY ) );


		/////////////////////////////////////////////////
		//		assign properties to widget
		/////////////////////////////////////////////////

		if ( mPlotType != eHistogram )
		{
			//ticks
			//
			// In 3D works per current plot (field) so are really assigned here, but with 2D... is global; we also assign 
			//...in this loop to keep consistency (if 2d widget is improved in this respect) but real 2d assignment must be made in field change handler

			unsigned int x_nticks = mCurrentPlotZFXY->Xticks(), y_nticks = mCurrentPlotZFXY->Yticks(), z_nticks = mCurrentPlotZFXY->ValueTicks( i );

			assert__( x_nticks > 0 && !isDefaultValue( x_nticks ) && y_nticks > 0 && !isDefaultValue( y_nticks ) && z_nticks > 0 && !isDefaultValue( z_nticks ) );

			if ( build2d )
			{
				mPlot2DView->SetXAxisTicks( x_nticks );
				mPlot2DView->SetYAxisTicks( y_nticks );
				mPlot2DView->SetY2AxisTicks( z_nticks );
			}
			if ( build3d )
			{
				mPlot3DView->SetXAxisTicks( x_nticks );
				mPlot3DView->SetYAxisTicks( y_nticks );
				mPlot3DView->SetZAxisTicks( z_nticks );
			}

			//digits
			//
			// similar to ticks
			//
			const auto x_digits = mCurrentPlotZFXY->Xdigits(), y_digits = mCurrentPlotZFXY->Ydigits(), z_digits = mCurrentPlotZFXY->ValueDigits( i );

			if ( build3d )
			{
                mPlot3DView->SetXAxisDigits( mCurrentPlotZFXY->XUnit().IsDate(), x_digits, RefDateFromUnit(mCurrentPlotZFXY->XUnit()) );
                mPlot3DView->SetYAxisDigits( mCurrentPlotZFXY->YUnit().IsDate(), y_digits, RefDateFromUnit(mCurrentPlotZFXY->YUnit()) );
                mPlot3DView->SetZAxisDigits( mCurrentPlotFieldZFXY->DataUnit()->IsDate(), z_digits, RefDateFromUnit(*mCurrentPlotFieldZFXY->DataUnit()) );
			}
			if ( build2d )
			{
                mPlot2DView->SetXAxisDigits( mCurrentPlotZFXY->XUnit().IsDate(), x_digits, RefDateFromUnit(mCurrentPlotZFXY->XUnit()) );
                mPlot2DView->SetYAxisDigits( mCurrentPlotZFXY->YUnit().IsDate(), y_digits, RefDateFromUnit(mCurrentPlotZFXY->YUnit()) );
                mPlot2DView->SetY2AxisDigits( mCurrentPlotFieldZFXY->DataUnit()->IsDate(), z_digits, RefDateFromUnit(*mCurrentPlotFieldZFXY->DataUnit()) );
			}


			//solid color/contour

			if ( build2d )
			{
				mPlot2DView->ShowContour( i, mCurrentPlotFieldZFXY->WithContour() );
				mPlot2DView->ShowSolidColor( i, mCurrentPlotFieldZFXY->WithSolidColor() );
			}

			if ( build3d )
			{
				//TODO	mPlot3DView->ShowContour( i, mCurrentPlotFieldZFXY->m_withContour );		
				mPlot3DView->ShowSolidColor( i, mCurrentPlotFieldZFXY->WithSolidColor() );	//in fact, in qwtplot3d this affects the axis (all fields) not only the current iteration one
			}
		}

		//plot title; must be done per plot; also updates window title

		SetPlotTitle();
	}


	//animation

	mMultiFrame = mPlotType != eHistogram && n_frames > 1;
	mTabAnimateOptions->setEnabled( mMultiFrame );				assert__( !mMultiFrame );		//TODO check this for 3d


	//interaction
	if ( mPlotType == eHistogram )
	{
		mPlot2DView->AddMagnifier();
		mPlot2DView->AddPanner();

		mTabCurveOptions->SwitchToHistogram( false );
	}
	else
	{
		// 2D Raster Interaction
		//
		// LeftButton for the zooming
		// MidButton for the panning
		// RightButton: zoom out by 1
		// Ctrl+RighButton: zoom out to full size
		//
		mPlot2DView->AddMagnifier();		//mPlot2DView->AddZoomer(); was the default
		mPlot2DView->AddPanner();

		mTabCurveOptions->SwitchTo3D();
		mTabAxisOptions->SwitchTo3D();
	}


	//GO: select field
	if ( mTabCurveOptions->mFieldsList->currentRow() != 0 )
		mTabCurveOptions->mFieldsList->setCurrentRow( 0 );
	else
		HandleCurrentFieldChanged( 0 );
}



//////////////////
// I.2.	y=f(x)
//////////////////

//plot->GetInfo(); 	 assumed done by GetPlotsFromDisplayData
//
void CPlotEditor::Recreate2DPlots()
{
	assert__( mPlot2DView && CurrentDisplayDataProcessor() && mCurrentPlotYFX );

	mTabCurveOptions->mFieldsList->clear();
	mTabCurveOptions->mFieldsList->setSelectionMode( mTabCurveOptions->mFieldsList->SingleSelection );

	int n_frames = 1;

	//for scale (common to all plots)

	double xaxisMin, xaxisMax, yaxisMin, yaxisMax;
	xaxisMax = yaxisMax = std::numeric_limits<double>::lowest();
	xaxisMin = yaxisMin = std::numeric_limits<double>::max();

	int nrFields = (int)mCurrentPlotYFX->Size();					assert__( nrFields == CurrentDisplayDataProcessor()->GetXYPlotPropertiesSize() );
	for ( int i = 0; i < nrFields; i++ )
	{
		/////////////////////////////////////////////////
		//				update pointers
		/////////////////////////////////////////////////

		bool result = UpdateCurrentPointers( i );			assert__( result );		Q_UNUSED( result );

		//==>CXYPlotFrame::AddData(CXYPlotField* data) ==>CXYPlotPanel::AddData( CXYPlotField* pdata )

		if ( n_frames == 1 )
			n_frames = mCurrentPlotFieldYFX->GetNumberOfFrames();
#if defined (DEBUG) || defined(_DEBUG)				//make sure the number of frames is the same for all data
		else
			assert__( n_frames == mCurrentPlotFieldYFX->GetNumberOfFrames() );
#endif

		/////////////////////////////////////////////////
		//	create field plot, all plots in same widget
		/////////////////////////////////////////////////

		double yrMin = 0;
		double yrMax = 0;
		mCurrentPlotFieldYFX->GetYRange( yrMin, yrMax );

		QwtPlotCurve *curve = nullptr;
		if ( mPlotType == eHistogram )
		{
			//2d Histograms: assigns (fixed, predefined) axis titles

			yaxisMin = std::min( yaxisMin, 0. );				//y axis is a frequency
			yaxisMax = std::max( yaxisMax, CreateHistogram() );	//y axis is a frequency
			xaxisMin = std::min( xaxisMin, yrMin );				//y value intervals in x axis
			xaxisMax = std::max( xaxisMax, yrMax );				//y value intervals in x axis
		}
		else
		{
			//2d curves

			curve = mPlot2DView->PushCurve( mCurrentPlotFieldYFX->WidgetFieldName(), color_cast<QColor>( mCurrentPlotFieldYFX->LineColor() ), *mCurrentPlotFieldYFX );

			double xrMin = 0;
			double xrMax = 0;
			mCurrentPlotFieldYFX->GetXRange( xrMin, xrMax );
			xaxisMin = std::min( xaxisMin, xrMin );
			xaxisMax = std::max( xaxisMax, xrMax );
			yaxisMin = std::min( yaxisMin, yrMin );
			yaxisMax = std::max( yaxisMax, yrMax );
		}

		/////////////////////////////////////////////////
		//				add field to list
		/////////////////////////////////////////////////

		mTabCurveOptions->mFieldsList->addItem( MakeFieldItem( mCurrentPlotFieldYFX ) );


		/////////////////////////////////////////////////
		//		assign field properties to widget
		/////////////////////////////////////////////////

		if ( mPlotType != eHistogram )
		{
			//curve line
			mPlot2DView->SetCurveLineColor( i, mCurrentPlotFieldYFX->LineColor() );
			mPlot2DView->SetCurveLineOpacity( i, mCurrentPlotFieldYFX->Opacity() );
			mPlot2DView->SetCurveLinePattern( i, mCurrentPlotFieldYFX->StipplePattern() );
			mPlot2DView->SetCurveLineWidth( i, mCurrentPlotFieldYFX->LineWidth() );

			//curve point
			mPlot2DView->SetCurvePointColor( i, mCurrentPlotFieldYFX->PointColor() );
			mPlot2DView->SetCurvePointFilled( i, mCurrentPlotFieldYFX->FilledPoint() );
			mPlot2DView->SetCurvePointGlyph( i, mCurrentPlotFieldYFX->PointGlyph() );
			mPlot2DView->SetCurvePointSize( i, mCurrentPlotFieldYFX->PointSize() );
		}

		//plot title; must be done per plot (although in 2d all fields are the in same plot); also updates window title

		SetPlotTitle();
	}


	/////////////////////////////////////////////////
	//		assign properties to widget
	/////////////////////////////////////////////////


	// axis titles

	if ( mPlotType != eHistogram )
		mPlot2DView->SetAllCurvesAxisTitles( mCurrentPlotYFX->Xtitle(), mCurrentPlotYFX->ValueTitle() );
    mTabAxisOptions->mZAxisLabel->setEnabled( false );


	// axis scale ranges (common to all plots)

	for ( int i = 0; i < nrFields; i++ )
		mPlot2DView->SetPlotAxisRanges( i, xaxisMin, xaxisMax, yaxisMin, yaxisMax );

    if ( mPlotType != eHistogram )
    {
        // axis ticks
        //
        // using last assignment in loop to mCurrentPlotFieldYFX: use field data for axis properties, which are common to all...
        //
        unsigned int x_nticks = mCurrentPlotYFX->Xticks(), y_nticks = mCurrentPlotYFX->Yticks();		assert__( x_nticks > 0 && !isDefaultValue( x_nticks ) && y_nticks > 0 && !isDefaultValue( y_nticks ) );

        mPlot2DView->SetXAxisTicks( x_nticks );
        mPlot2DView->SetYAxisTicks( y_nticks );

        //axis digits
        //
        // similar comment as for ticks
        //
        unsigned x_digits = mCurrentPlotYFX->Xdigits(), y_digits = mCurrentPlotYFX->Ydigits();		assert__( x_digits > 0 && !isDefaultValue( x_digits ) && y_digits > 0 && !isDefaultValue( y_digits ) );

        mPlot2DView->SetXAxisDigits( nrFields == 1 && mCurrentPlotYFX->XUnit().IsDate(), x_digits, RefDateFromUnit( mCurrentPlotYFX->XUnit() ) );
        mPlot2DView->SetYAxisDigits( nrFields == 1 && mCurrentPlotYFX->YUnit().IsDate(), y_digits, RefDateFromUnit( mCurrentPlotYFX->YUnit() ) );
    }

	//animation

	mMultiFrame = mPlotType != eHistogram && n_frames > 1;
	mTabAnimateOptions->setEnabled( mMultiFrame );
	if ( mMultiFrame )
	{
		mTabAnimateOptions->SetRange( n_frames -1 );
		mTabAnimateOptions->mSpeedEdit->setText( "100" );
		mTabAnimateOptions->mMoveForwardButton->setChecked( true ), 
		mTabAnimateOptions->mLoopButton->setChecked( true ), 

		connect( mPlot2DView, SIGNAL( FrameChanged( size_t ) ), mTabAnimateOptions, SLOT( HandleFrameChanged( size_t ) ) );
		connect( mPlot2DView, SIGNAL( AnimationStopped() ), this, SLOT( AnimationStopped() ) );
	}


	//interaction

	//Create these only after invoking CreatePlot
	//
	mPlot2DView->AddMagnifier();
	mPlot2DView->AddPanner();


	if ( mPlotType == eHistogram )
	{
		mTabCurveOptions->SwitchToHistogram( true );
	}
	else
	{
		mTabCurveOptions->SwitchTo2D();
		mTabAxisOptions->SwitchTo2D();
	}


	//GO: select field
	if ( mTabCurveOptions->mFieldsList->currentRow() != 0 )
		mTabCurveOptions->mFieldsList->setCurrentRow( 0 );
	else
		HandleCurrentFieldChanged( 0 );
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//									II. From plot to widgets	(&& assign "current" pointers)
//
//					- update widgets state and content, reflecting views state
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CPlotEditor::HandleCurrentFieldChanged( int index )
{
	assert__( mPlot2DView );

	/////////////////////////////////////////////////
	//			update widgets state
	/////////////////////////////////////////////////

	bool enable_yfx = index >= 0 && !mCurrentPlotZFXY && mPlotType != eHistogram;
	mTabCurveOptions->mLineOptions->setEnabled( enable_yfx );
	mTabCurveOptions->mPointOptions->setEnabled( enable_yfx );
	mTabCurveOptions->mColorMapWidget->EnableOnlySolidColor( false );		//restore
	mTabCurveOptions->mSpectrogramOptions->setEnabled( index >= 0 && mCurrentPlotZFXY && mPlotType != eHistogram );

	//always prevent label editing when viewing an histogram

    mTabAxisOptions->mXAxisLabel->setEnabled( mPlotType != eHistogram );
    mTabAxisOptions->mYAxisLabel->setEnabled( mPlotType != eHistogram );
    mTabAxisOptions->mZAxisLabel->setEnabled( mCurrentPlotZFXY && mPlotType != eHistogram );
	mTabAxisOptions->mZLogScaleCheck->setEnabled( false );


	/////////////////////////////////////////////////
	//				update pointers
	/////////////////////////////////////////////////

	UpdateCurrentPointers( -1 );
	if ( index < 0 )
		return;

	bool result = UpdateCurrentPointers( index );			assert__( result );		Q_UNUSED( result );
	
	CWidgetField *pdata = mCurrentPlotFieldYFX ? static_cast<CWidgetField*>( mCurrentPlotFieldYFX ) : static_cast<CWidgetField*>( mCurrentPlotFieldZFXY );	assert__( pdata );
	CPlotInterfaces *pplot = mCurrentPlotFieldYFX ? static_cast<CPlotInterfaces*>( mCurrentPlotYFX ) : static_cast<CPlotInterfaces*>( mCurrentPlotZFXY );	assert__( pplot );

	/////////////////////////////////////////////////
	//for ZFXY, changing field changes displayed plot
	/////////////////////////////////////////////////

	if ( mCurrentPlotZFXY )
	{
		//update 2d current plot

		if ( mPlotType == eHistogram )
		{
			//updates plot axis titles & scale

			mPlot2DView->SetCurrentHistogram( index );
		}
		else
		{
			//updates plot axis titles & scale
			
			mPlot2DView->SetCurrentRaster( index );
		}

		//update 3d current plot

		mPlot3DView->SetCurrentPlot( index );
	}


	/////////////////////////////////////////////////
	// YFX and ZFXY
	/////////////////////////////////////////////////

    //axis

	//...labels
	std::string xtitle, ytitle, ztitle;
	if ( mPlotType == eHistogram && !mPlot3DView )						//viewing only histogram
	{
		xtitle = C2DPlotWidget::smHistogramXtitle;
		ytitle = C2DPlotWidget::smHistogramYtitle;
	}
	else
	if ( mPlot3DView )
		mPlot3DView->CurrentAxisTitles( xtitle, ytitle, ztitle );
	else
		mPlot2DView->CurrentAxisTitles( &xtitle, &ytitle, &ztitle );

    mTabAxisOptions->mXAxisLabel->setText( xtitle.c_str() );
    mTabAxisOptions->mYAxisLabel->setText( ytitle.c_str() );
    mTabAxisOptions->mZAxisLabel->setText( ztitle.c_str() );

    //bins

    if ( mPlotType == eHistogram )
    {
        mTabCurveOptions->mNumberOfBins->setText( n2s( mPlot2DView->NumberOfBins( index ) ).c_str() );
        mTabAxisOptions->mXNbDigits->setEnabled( !mPlot2DView->XAxisIsDateTime() );
        mTabAxisOptions->mYNbDigits->setEnabled( !mPlot2DView->YAxisIsDateTime() );
        mTabAxisOptions->mZNbDigits->setEnabled( false );
    }
    else
    {
        // ticks and digits NOTE: 2d and 3d widgets adjust these parameter values, and that can result not only in values different
        //	from the assignment made but also different in the 2d and 3d case. So, unlike the general policy followed of always showing
        //	the "real" values used by the plot, and because here there can be 2 different "real" values, in these cases we use the value
        //	in the plot field structures (and used in assignment), not the values reported by the widgets.

        //...ticks
        // 2d does not keep axis ticks per plot, and as a consequence:
        //		1. read values from plot data
        //
        int xticks = pplot->Xticks();	//mPlot3DView ? mPlot3DView->XAxisTicks() : pplot->Xticks();
        int yticks = pplot->Yticks();	//mPlot3DView ? mPlot3DView->YAxisTicks() : pplot->Yticks();
        int zticks = mCurrentPlotYFX ? mCurrentPlotYFX->ValueTicks() : mCurrentPlotZFXY->ValueTicks( index );		//mPlot3DView ? mPlot3DView->ZAxisTicks() : pplot->Zticks();
        //
        //		2. we must assign to 2d widget here, breaking parameter editors update policy
        //
        mPlot2DView->SetXAxisTicks( xticks );
        mPlot2DView->SetYAxisTicks( yticks );
        mPlot2DView->SetY2AxisTicks( zticks );
        //
        mTabAxisOptions->mXNbTicks->setText( n2s( xticks ).c_str() );
        mTabAxisOptions->mYNbTicks->setText( n2s( yticks ).c_str() );
        mTabAxisOptions->mZNbTicks->setText( n2s( zticks ).c_str() );

        //...digits
        // similar problem and solution as for ticks, with the additional need to also specify units besides digits
        //
        int xdigits = pplot->Xdigits();		//mPlot3DView ? mPlot3DView->XDigits() : pplot->Xdigits();
        int ydigits = pplot->Ydigits();		//mPlot3DView ? mPlot3DView->YDigits() : pplot->Ydigits();
        int zdigits = mCurrentPlotYFX ? mCurrentPlotYFX->ValueDigits() : mCurrentPlotZFXY->ValueDigits( index );	//mPlot3DView ? mPlot3DView->ZDigits() : pplot->Zdigits();
        //
        const CUnit &xunit = mCurrentPlotZFXY ? mCurrentPlotZFXY->XUnit() : mCurrentPlotYFX->XUnit();
        const CUnit &yunit = mCurrentPlotZFXY ? mCurrentPlotZFXY->YUnit() : mCurrentPlotYFX->YUnit();
        //
        mPlot2DView->SetXAxisDigits( xunit.IsDate(), xdigits, RefDateFromUnit( xunit ) );
        mPlot2DView->SetYAxisDigits( yunit.IsDate(), ydigits, RefDateFromUnit( yunit ) );
        mPlot2DView->SetY2AxisDigits( pdata->DataUnit()->IsDate(), zdigits, RefDateFromUnit( *pdata->DataUnit() ) );
        //
        mTabAxisOptions->mXNbDigits->setText( n2s( xdigits ).c_str() );
        mTabAxisOptions->mYNbDigits->setText( n2s( ydigits ).c_str() );
        mTabAxisOptions->mZNbDigits->setText( n2s( zdigits ).c_str() );
        //
        // disable digits assignment for dates case; now we can return to read from widgets instead of data
        //
        mTabAxisOptions->mXNbDigits->setEnabled( !mPlot2DView->XAxisIsDateTime() );
        mTabAxisOptions->mYNbDigits->setEnabled( !mPlot2DView->YAxisIsDateTime() );
        mTabAxisOptions->mZNbDigits->setEnabled( mPlot3DView && !mPlot3DView->ZAxisIsDateTime() );
    }


	//...scales (with logarithmic)
	//scales are sensitive to user interaction so widget assignment is automatically done by remaining handlers in this file's section
	double xVal, yVal, zVal;
	if ( mPlot3DView )
	{
		mPlot3DView->Scale( xVal, yVal, zVal );
		mTabAxisOptions->mX3DScaleSpin->setValue( xVal );
		mTabAxisOptions->mY3DScaleSpin->setValue( yVal );
		mTabAxisOptions->mZ3DScaleSpin->setValue( zVal );

		mTabAxisOptions->mZLogScaleCheck->setEnabled( mCurrentPlotZFXY->ZLogarithmsEnabled() );
		mTabAxisOptions->mZLogScaleCheck->setChecked( mPlot3DView->AxisZLogarithmicScale() );
	}

	///////////////////////////////////////////////
	//solid color, contours, color table: only ZFXY
	///////////////////////////////////////////////

	if ( mCurrentPlotZFXY )
	{
		if ( mPlotType != eHistogram )
		{
			//TODO	no contours in 3D widget:	assert__( mPlot2DView->HasContour() == mPlot3DView->HasContour() );
			assert__( mPlot2DView->HasSolidColor() == mPlot3DView->HasSolidColor() );

			mTabCurveOptions->mColorMapWidget->SetShowContour( mPlotType != eHistogram && mPlot2DView->HasContour() );
			mTabCurveOptions->mColorMapWidget->SetNumberOfContours( mPlot2DView->NumberOfContours() );
			mTabCurveOptions->mColorMapWidget->EnableOnlySolidColor( mCurrentPlotFieldZFXY->IsVectorComponent() );
		}

		//must use 3d widget; 2d can be on histogram mode

		mTabCurveOptions->mColorMapWidget->SetShowSolidColor( mPlot3DView->HasSolidColor() );

		// TODO color table properties should be retrieved here from plot
		//mCurrentPlotFieldZFXY->SetAbsoluteRangeValues( array.mMinHeightValue, array.mMaxHeightValue );

		auto const &array = mCurrentPlotFieldZFXY->at( 0 );		Q_UNUSED( array );
		assert__( array.mMinHeightValue == mCurrentPlotFieldZFXY->DataMinValue() && array.mMaxHeightValue == mCurrentPlotFieldZFXY->DataMaxValue() );

		mTabCurveOptions->mColorMapWidget->SetLUT( mCurrentPlotFieldZFXY->ColorTablePtr(), mCurrentPlotFieldZFXY->DataMinValue(), mCurrentPlotFieldZFXY->DataMaxValue() );
	
		return;
	}

	///////////////////////////////////////////////
	// line & point options: only YFX-not-histogram
	///////////////////////////////////////////////

	if ( mPlotType == eHistogram )
	{		
		return;
	}

	//curve line
	mTabCurveOptions->mLineColorButton->SetColor( mPlot2DView->CurveLineColor( index ) );
	mTabCurveOptions->mLineOpacityValue->setText( n2s( mPlot2DView->CurveLineOpacity( index ) ).c_str() );
	mTabCurveOptions->mStipplePattern->setCurrentIndex( mPlot2DView->CurveLinePattern( index ) );
	mTabCurveOptions->mLineWidthValue->setText( n2s( mPlot2DView->CurveLineWidth( index ) ).c_str() );

	//curve point
	mTabCurveOptions->mPointColorButton->SetColor( mPlot2DView->CurvePointColor( index ) );
	mTabCurveOptions->mFillPointCheck->setChecked( mPlot2DView->IsCurvePointFilled( index ) );
	mTabCurveOptions->mPointGlyph->setCurrentIndex( mPlot2DView->CurvePointGlyph( index, mCurrentPlotFieldYFX->PointGlyph() ) );
	mTabCurveOptions->mPointSizeValue->setText( n2s( mPlot2DView->CurvePointSize( index ) ).c_str() );

	//enable/disable according to data
	mTabCurveOptions->mLineOptions->setChecked( mPlotType != eHistogram && mCurrentPlotFieldYFX->Lines() );
	mTabCurveOptions->mPointOptions->setChecked( mCurrentPlotFieldYFX->Points() );
}


void CPlotEditor::Handle3DScaleChanged( double xVal, double yVal, double zVal )
{
	mTabAxisOptions->mX3DScaleSpin->setValue( xVal );
	mTabAxisOptions->mY3DScaleSpin->setValue( yVal );
	mTabAxisOptions->mZ3DScaleSpin->setValue( zVal );

	//LOG_TRACEstd( "3d scale changed " + n2s( xVal ) + " - " + n2s( yVal ) + " - " + n2s( zVal ) );
}


void CPlotEditor::Handle2DScaleChanged( int iaxis, double factor, QString range )
{
	QLineEdit *edit = nullptr;
	QDoubleSpinBox *spin = nullptr;
	switch ( iaxis )
	{
		case 0:
			edit = mTabAxisOptions->mXAxisMinMax;
			spin = mTabAxisOptions->mX2DScaleSpin;
			break;
		case 1:
			edit = mTabAxisOptions->mYAxisMinMax;
			spin = mTabAxisOptions->mY2DScaleSpin;
			break;
		case 2:
			edit = mTabAxisOptions->mZAxisMinMax;
			spin = mTabAxisOptions->mZ2DScaleSpin;
			break;
		default:
			assert__( false );
	}

	// This would be enough to prevent infinite loop (infinite because spin signal can also call this handler):
	//		if ( factor != spin->value() ) spin->setValue( factor );
	// But we prefer to block spin signals preventing unnecessary "echo" from the spin while re-plotting (which 
	// also updates axis scales and calls this several times)
	//
		spin->blockSignals( true );
		spin->setValue( factor );
		spin->blockSignals( false );

	edit->setText( range );	

	// LOG_TRACEstd( "2d scale changed: axis" + n2s( iaxis ) + " - factor: " + n2s( factor ) );
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//						III. From widgets to properties, from properties to plot
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////
//spectrogram  properties
/////////////////////////

void CPlotEditor::HandleShowContourChecked( bool checked )
{
	assert__( mPlot2DView && mPlot3DView && mCurrentPlotZFXY && mCurrentPlotFieldZFXY );

	WaitCursor wait;

	mCurrentPlotFieldZFXY->SetWithContour( checked );

	if ( mPlotType != eHistogram )
		mPlot2DView->ShowContour( mCurrentPlotFieldZFXY->WithContour() );
}


void CPlotEditor::HandleShowSolidColorChecked( bool checked )
{
	assert__( mPlot2DView && mPlot3DView && mCurrentPlotFieldZFXY && mCurrentPlotZFXY );

	WaitCursor wait;

	mCurrentPlotFieldZFXY->SetWithSolidColor( checked );

	if ( mPlotType != eHistogram )
		mPlot2DView->ShowSolidColor( mCurrentPlotFieldZFXY->WithSolidColor() );
	mPlot3DView->ShowSolidColor( mCurrentPlotFieldZFXY->WithSolidColor() );
}


void CPlotEditor::HandleColorTablesIndexChanged( int index )
{
    assert__( mPlot2DView && mPlot3DView && mCurrentPlotFieldZFXY && mCurrentPlotZFXY );        Q_UNUSED( index );

	WaitCursor wait;

	CWidgetField *pdata = mCurrentPlotFieldYFX ? static_cast< CWidgetField* >( mCurrentPlotFieldYFX ) : static_cast< CWidgetField* >( mCurrentPlotFieldZFXY );

	//no need to assign... check only

	assert__( mTabCurveOptions->mColorMapWidget->ColorRangeMin() == pdata->CurrentMinValue() && mTabCurveOptions->mColorMapWidget->ColorRangeMax() == pdata->CurrentMaxValue() );

	if ( mPlotType != eHistogram )
		mPlot2DView->SetRasterColorMap( *pdata->ColorTablePtr()->GetLookupTable() );
	mPlot3DView->SetColorMap( new QLookupTable( *pdata->ColorTablePtr()->GetLookupTable() ) );
}


void CPlotEditor::HandleNumberOfContoursChanged()
{
	assert__( mPlot2DView && mPlot3DView && mCurrentPlotFieldZFXY && mCurrentPlotZFXY );

	mCurrentPlotFieldZFXY->SetNumContours( mTabCurveOptions->mColorMapWidget->NumberOfContours() );

	if ( mPlotType != eHistogram )
		mPlot2DView->SetNumberOfContours( mCurrentPlotFieldZFXY->MinContourValue(), mCurrentPlotFieldZFXY->MaxContourValue(), mCurrentPlotFieldZFXY->NumContours() );
}



/////////////////////////
// line properties
/////////////////////////

void CPlotEditor::HandleCurveLineColorSelected()
{
	assert__( mPlot2DView && mCurrentPlotFieldYFX );

    QColor selected_color = mTabCurveOptions->mLineColorButton->GetColor();

    mCurrentPlotFieldYFX->SetLineColor( selected_color );

	mPlot2DView->SetCurveLineColor( mTabCurveOptions->mFieldsList->currentRow(), color_cast< QColor >( mCurrentPlotFieldYFX->LineColor() ) );
}

void CPlotEditor::HandleCurveLineOpacityEntered()
{
	assert__( mPlot2DView && mCurrentPlotFieldYFX );

    bool is_converted=false;
    const QString opacity = mTabCurveOptions->mLineOpacityValue->text();
    mCurrentPlotFieldYFX->SetOpacity( opacity.toInt(&is_converted, 10) );			//TODO VALIDATE

	mPlot2DView->SetCurveLineOpacity( mTabCurveOptions->mFieldsList->currentRow(), mCurrentPlotFieldYFX->Opacity() );
}

void CPlotEditor::HandleStipplePatternChanged( int pattern )
{
	assert__( pattern >= eFirstStipplePattern && pattern < EStipplePattern_size );

    mCurrentPlotFieldYFX->SetStipplePattern( (EStipplePattern)pattern );
	mPlot2DView->SetCurveLinePattern( mTabCurveOptions->mFieldsList->currentRow(), mCurrentPlotFieldYFX->StipplePattern() );
}

void CPlotEditor::HandleCurveLineWidthEntered()
{
	assert__( mPlot2DView && mCurrentPlotFieldYFX );

    bool is_converted=false;
    const QString width = mTabCurveOptions->mLineWidthValue->text();
	mCurrentPlotFieldYFX->SetLineWidth( width.toInt( &is_converted, 10 ) );			//TODO VALIDATE
	mPlot2DView->SetCurveLineWidth( mTabCurveOptions->mFieldsList->currentRow(), mCurrentPlotFieldYFX->LineWidth() );
}


/////////////////////////
// curve properties
/////////////////////////

void CPlotEditor::HandleCurvePointColorSelected()
{
	assert__( mPlot2DView && mCurrentPlotFieldYFX );

    QColor selected_color = mTabCurveOptions->mPointColorButton->GetColor();
    mCurrentPlotFieldYFX->SetPointColor( selected_color );
	mPlot2DView->SetCurvePointColor( mTabCurveOptions->mFieldsList->currentRow(), mCurrentPlotFieldYFX->PointColor() );
}

void CPlotEditor::HandleFillGlyphInterior( bool checked )
{
	assert__( mPlot2DView && mCurrentPlotFieldYFX );

    mCurrentPlotFieldYFX->SetFilledPoint( checked );

	mPlot2DView->SetCurvePointFilled( mTabCurveOptions->mFieldsList->currentRow(), mCurrentPlotFieldYFX->FilledPoint() );
}

void CPlotEditor::HandleGlyphPatternChanged( int pattern )
{
	assert__( mPlot2DView && mCurrentPlotFieldYFX && pattern >= eFirstPointGlyph && pattern < EPointGlyph_size );

    mCurrentPlotFieldYFX->SetPointGlyph( (EPointGlyph)pattern );

	mPlot2DView->SetCurvePointGlyph( mTabCurveOptions->mFieldsList->currentRow(), mCurrentPlotFieldYFX->PointGlyph() );
}

void CPlotEditor::HandleCurveGlyphWidthEntered()
{
	assert__( mPlot2DView && mCurrentPlotFieldYFX );

    bool is_converted=false;
    const QString pointw = mTabCurveOptions->mPointSizeValue->text();		//TODO validate
    int w_value = pointw.toInt(&is_converted, 10);

    mCurrentPlotFieldYFX->SetPointSize( w_value );
	mPlot2DView->SetCurvePointSize( mTabCurveOptions->mFieldsList->currentRow(), mCurrentPlotFieldYFX->PointSize() );
}



/////////////////////////
// line/curve properties
/////////////////////////

//enable/disable line/curve properties

void CPlotEditor::HandleLineOptionsChecked( bool checked )
{
	assert__( mPlot2DView  && mCurrentPlotFieldYFX );

    mCurrentPlotFieldYFX->SetLines( checked );
	mPlot2DView->EnableCurveLine( mTabCurveOptions->mFieldsList->currentRow(), mCurrentPlotFieldYFX->Lines() );
}

void CPlotEditor::HandlePointOptionsChecked( bool checked )
{
	assert__( mPlot2DView && mCurrentPlotFieldYFX );

    mCurrentPlotFieldYFX->SetPoints( checked );

	mPlot2DView->EnableCurvePoints( mTabCurveOptions->mFieldsList->currentRow(), mCurrentPlotFieldYFX->Points(), mCurrentPlotFieldYFX->PointGlyph() );
}




/////////////////////////
// animation properties
/////////////////////////


void CPlotEditor::HandleAnimateButtonToggled( bool toggled )
{
	assert__( mPlot2DView  && mCurrentPlotFieldYFX );

    mDisplay->SetWithAnimation( toggled );

	bool ok;
	QString qspeed = mTabAnimateOptions->mSpeedEdit->text();
	unsigned int speed = qspeed.toUInt( &ok);
	if ( !ok )
		speed = 100;

	mPlot2DView->Animate( toggled ? speed : 0 );
}


void CPlotEditor::HandleStopAnimationButton()
{
	assert__( mPlot2DView  && mCurrentPlotFieldYFX );

	mPlot2DView->StopAnimation();
	mTabAnimateOptions->mAnimateButton->setChecked( false );
}


void CPlotEditor::HandleAnimateLoop( bool toggled )
{
	assert__( mPlot2DView  && mCurrentPlotFieldYFX );

	mTabAnimateOptions->mFrameIndexSpin->setWrapping( toggled );

	mPlot2DView->SetLoop( toggled );
}


void CPlotEditor::HandleAnimateBackwardToggled( bool toggled )
{
	assert__( mPlot2DView  && mCurrentPlotFieldYFX );

	mPlot2DView->SetMovingForward( !toggled );
}


void CPlotEditor::HandleAnimateForwardToggled( bool toggled )
{
	assert__( mPlot2DView  && mCurrentPlotFieldYFX );

	mPlot2DView->SetMovingForward( toggled );
}


void CPlotEditor::AnimationStopped()
{
	assert__( mPlot2DView  && mCurrentPlotFieldYFX );

	mTabAnimateOptions->mAnimateButton->setChecked( false );
}


void CPlotEditor::HandleFrameIndexSpinChanged( int i )
{
	assert__( mPlot2DView  && mCurrentPlotFieldYFX );

	// If signal was initially sent by spin, call mPlot2DView ->SetFrame; if not, 
	//	the spin itself is being updated by the mPlot2DView signal (connected above)
	//
	if ( mTabAnimateOptions->mFrameIndexSpin->isEnabled() )		
		mPlot2DView->SetFrame( i );

	SetAnimationDescr( i );
}


void CPlotEditor::SetAnimationDescr( int frame )
{
	assert__( mPlot2DView  && mCurrentPlotFieldYFX );

	mTabAnimateOptions->mInformation->setText( "" );

	std::string descr;

	const size_t ndata = mCurrentPlotYFX->Size();

	for ( size_t i = 0; i < ndata; i++ )
	{
		CXYPlotField* data = mCurrentPlotYFX->PlotData( i );

		if ( data->GetVarComplement().GetNbValues() <= 0 )
		{
			continue;
		}

		CFieldNetCdf& field = data->GetFieldComplement();
		std::string field_descr = field.GetMostExplicitName();
		std::string str_unit = field.GetUnit();

		CUnit* unit = field.GetNetCdfUnit();

		size_t index = frame;
		if ( index >= data->GetVarComplement().GetNbValues() )
		{
			index = data->GetVarComplement().GetNbValues() - 1;
		}

		if ( unit->IsDate() )
		{
			CDate d1950_01_01 = unit->ConvertToBaseUnit( data->GetVarComplement().GetValues()[ index ] );
			descr += (
				"'"
				+ field_descr
				+ "' value for above plot: "
				+ d1950_01_01.AsString( "", true )
				+ " (field '"
				+ data->GetPlotProperties()->FieldName()
				+ "')\n" );
		}
		else
		{
			descr += (
				"'"
				+ field_descr
				+ "' value for above plot: "
				+ n2s( data->GetVarComplement().GetValues()[ index ] )
				+ " "
				+ str_unit
				+ " (field '"
				+ data->GetPlotProperties()->FieldName()
				+ "')\n" );
		}


		size_t i_other_fields = 0;

		const CObArray& other_fields = data->GetOtherFields();
		const CObArray& other_data = data->GetOtherVars();

		for ( i_other_fields = 0; i_other_fields < other_fields.size(); i_other_fields++ )
		{
			CFieldNetCdf* field = dynamic_cast<CFieldNetCdf*>( other_fields.at( i_other_fields ) );
			if ( field == nullptr )
			{
				continue;
			}

			std::string field_descr = field->GetMostExplicitName();
			std::string str_unit = field->GetUnit();

			CUnit* unit = field->GetNetCdfUnit();

			size_t index = frame;
			CExpressionValue* exprValue = CExpressionValue::GetExpressionValue( other_data.at( i_other_fields ) );

			if ( index >= exprValue->GetNbValues() )
			{
				index = exprValue->GetNbValues() - 1;
			}

			if ( unit->IsDate() )
			{
				CDate d1950_01_01 = unit->ConvertToBaseUnit( exprValue->GetValues()[ index ] );

				descr += (
					"'"
					+ field_descr
					+ "' value for above plot: "
					+ d1950_01_01.AsString( "", true )
					+ " (field '"
					+ data->GetPlotProperties()->FieldName()
					+ "')\n" );
			}
			else
			{
				double value = exprValue->GetValues()[ index ];
				std::string value_str;

				if ( isDefaultValue( value ) )
				{
					value_str = "No value (default value)";
				}
				else
				{
					value_str = n2s(value) + " " + str_unit;
				}

				descr += ( 
					"'"
					+ field_descr
					+ "' value for above plot: "
					+ value_str
					+ " (field '"
					+ data->GetPlotProperties()->FieldName()
					+ "')\n" );
			}
		}
	}

	mTabAnimateOptions->mInformation->setText( descr.c_str() );
}





/////////////////////////
// histogram properties
/////////////////////////

void CPlotEditor::HandleNumberOfBinsChanged()
{
	CWidgetField *pdata = mCurrentPlotFieldYFX ? static_cast< CWidgetField* >( mCurrentPlotFieldYFX ) : static_cast< CWidgetField* >( mCurrentPlotFieldZFXY );

	assert__( mPlot2DView && pdata );

    const QString qbins = mTabCurveOptions->mNumberOfBins->text();
	bool is_converted = false;
	unsigned bins = qbins.toInt( &is_converted, 10 );
	if ( !is_converted )
	{
		bins = mPlot2DView->NumberOfBins( mTabCurveOptions->mFieldsList->currentRow() );
		mTabCurveOptions->mNumberOfBins->blockSignals( true );
		mTabCurveOptions->mNumberOfBins->setText( n2s( bins ).c_str() );
		mTabCurveOptions->mNumberOfBins->blockSignals( false );
		LOG_WARN( "Invalid number of bins" );
		return;
	}	

	pdata->SetNumberOfBins( bins );
	HandlePlotTypeChanged( int( eHistogram ) );
}




//////////////////////////////////////////////////////////////////////////
//	axis  properties
//////////////////////////////////////////////////////////////////////////

//////////////////
//	...logarithmic

//void CPlotEditor::HandleLogarithmicScaleX( bool log )
//{
//	WaitCursor wait;
//
//	if ( mPlot3DView )
//    {
//        mPlot3DView->SetLogarithmicScale( log );		//TODO or eliminate}
//    }
//    if (mPlot2DView)
//    {
//        mPlot2DView->SetLogScale(QwtPlot::xBottom, log);
//        mCurrentPlotFieldYFX->SetXLog(log);
//    }
//}
//
//void CPlotEditor::HandleLogarithmicScaleY( bool log )
//{
//	WaitCursor wait;
//
//    if ( mPlot3DView )
//    {
//        mPlot3DView->SetLogarithmicScale( log );		//TODO or eliminate
//    }
//    if (mPlot2DView)
//    {
//        mPlot2DView->SetLogScale(QwtPlot::yLeft, log);
//        mCurrentPlotFieldYFX->SetYLog(log);
//    }
//}

void CPlotEditor::HandleLogarithmicScaleZ( bool log )
{
	WaitCursor wait;

	if ( mPlot3DView )
	{
		mPlot3DView->SetAxisZLogarithmicScale( log );
    }
    //if (mPlot2DView)
    //{
    //    mPlot2DView->SetLogScale(QwtPlot::yRight, log);
    //}
}


//////////////////
//	...ticks


static inline int ProcessNumber( QLineEdit *edit, unsigned int def_value, const std::string &property_name, unsigned int max_value )
{
	bool ok_conv = false;
	QString new_nb_str = edit->text();
	int new_nb = new_nb_str.toInt( &ok_conv );
    if ( !ok_conv || new_nb > (int)max_value )
	{
		new_nb = def_value;
		edit->blockSignals( true );
		edit->setText( n2s( new_nb ).c_str() );
		edit->blockSignals( false );
		LOG_WARN( "Invalid number for " + property_name );
		return -1;
	}
	return new_nb;
}




void CPlotEditor::HandleXNbTicksChanged()
{
	assert__( mPlot2DView );

	int new_nb = ProcessNumber( mTabAxisOptions->mXNbTicks, mPlot2DView->XAxisTicks(), "ticks", min_plot_widget_width / 5 );	// TODO arbitrary value
	if ( new_nb < 0 )
		return;

	CPlotInterfaces *pplot = mCurrentPlotFieldYFX ? static_cast<CPlotInterfaces*>( mCurrentPlotYFX ) : static_cast<CPlotInterfaces*>( mCurrentPlotZFXY );	assert__( pplot );
	
	if ( mPlotType != eHistogram || mCurrentPlotZFXY )	//assign to data only if another plot besides histogram is being shown
		pplot->SetXticks( new_nb );

	if ( mPlot2DView )
	{
		mPlot2DView->SetXAxisTicks( pplot->Xticks() );
	}
	if ( mPlot3DView )
	{
		mPlot3DView->SetXAxisTicks( pplot->Xticks() );
	}
}


void CPlotEditor::HandleYNbTicksChanged()
{
	assert__( mPlot2DView );

	int new_nb = ProcessNumber( mTabAxisOptions->mYNbTicks, mPlot2DView->YAxisTicks(), "ticks", min_plot_widget_width / 5 );	// TODO arbitrary value
	if ( new_nb < 0 )
		return;

	CPlotInterfaces *pplot = mCurrentPlotFieldYFX ? static_cast<CPlotInterfaces*>( mCurrentPlotYFX ) : static_cast<CPlotInterfaces*>( mCurrentPlotZFXY );	assert__( pplot );

	if ( mPlotType != eHistogram || mCurrentPlotZFXY )	//assign to data only if another plot besides histogram is being shown
		pplot->SetYticks( new_nb );

	if ( mPlot2DView )
	{
		mPlot2DView->SetYAxisTicks( pplot->Yticks() );
	}
	if ( mPlot3DView )
	{
		mPlot3DView->SetYAxisTicks( pplot->Yticks() );
	}
}


void CPlotEditor::HandleZNbTicksChanged()
{
	assert__( mPlot3DView );

	int new_nb = ProcessNumber( mTabAxisOptions->mZNbTicks, mPlot3DView->ZAxisTicks(), "ticks", min_plot_widget_width / 5 );	// TODO arbitrary value
	if ( new_nb < 0 )
		return;

	const int index = mTabCurveOptions->mFieldsList->currentRow();

	if ( mCurrentPlotYFX )
	{
		if ( mPlotType != eHistogram )					//assign to data only if another plot besides histogram is being shown
			mCurrentPlotYFX->SetValueTicks( new_nb );
	}
	else
	{
		mCurrentPlotZFXY->SetValueTicks( index, new_nb );
	}

	mPlot2DView->SetY2AxisTicks( new_nb );
	mPlot3DView->SetZAxisTicks( new_nb );
}



//////////////////
//	...digits

void CPlotEditor::HandleXAxisNbDigitsChanged()
{
	assert__( mPlot2DView );

	int new_nb = ProcessNumber( mTabAxisOptions->mXNbDigits, mPlot2DView->XAxisDigits(), "digits", 30 );	// TODO arbitrary value
	if ( new_nb < 0 )
		return;

	CPlotInterfaces *pplot = mCurrentPlotFieldYFX ? static_cast<CPlotInterfaces*>( mCurrentPlotYFX ) : static_cast<CPlotInterfaces*>( mCurrentPlotZFXY );	assert__( pplot );

	if ( mPlotType != eHistogram || mCurrentPlotZFXY )	//assign to data only if another plot besides histogram is being shown
		pplot->SetXdigits( new_nb );

	if ( mPlot2DView )
	{
        mPlot2DView->SetXAxisDigits( mPlot2DView->XAxisIsDateTime(), pplot->Xdigits() );
    }
	if ( mPlot3DView )
    {
        mPlot3DView->SetXAxisDigits( mPlot3DView->XAxisIsDateTime(), pplot->Xdigits() );
    }
}

void CPlotEditor::HandleYAxisNbDigitsChanged()
{
	assert__( mPlot2DView );

	int new_nb = ProcessNumber( mTabAxisOptions->mYNbDigits, mPlot2DView->YAxisDigits(), "digits", 30 );	// TODO arbitrary value
	if ( new_nb < 0 )
		return;

	CPlotInterfaces *pplot = mCurrentPlotFieldYFX ? static_cast<CPlotInterfaces*>( mCurrentPlotYFX ) : static_cast<CPlotInterfaces*>( mCurrentPlotZFXY );	assert__( pplot );

	if ( mPlotType != eHistogram || mCurrentPlotZFXY )	//assign to data only if another plot besides histogram is being shown
		pplot->SetYdigits( new_nb );

	if ( mPlot2DView )
	{
        mPlot2DView->SetYAxisDigits( mPlot2DView->YAxisIsDateTime(), pplot->Ydigits() );
    }
	if ( mPlot3DView )
    {
        mPlot3DView->SetYAxisDigits( mPlot3DView->YAxisIsDateTime(), pplot->Ydigits() );
    }
}


void CPlotEditor::HandleZAxisNbDigitsChanged()
{
	assert__( mPlot3DView );

	int new_nb = ProcessNumber( mTabAxisOptions->mZNbDigits, mPlot3DView->ZAxisDigits(), "digits", 30 );	// TODO arbitrary value
	if ( new_nb < 0 )
		return;

	const int index = mTabCurveOptions->mFieldsList->currentRow();

	if ( mCurrentPlotYFX )
	{
		if ( mPlotType != eHistogram )					//assign to data only if another plot besides histogram is being shown
			mCurrentPlotYFX->SetValueDigits( new_nb );
	}
	else
	{
		mCurrentPlotZFXY->SetValueDigits( index, new_nb );
	}

	mPlot2DView->SetY2AxisDigits( mPlot2DView->Y2AxisIsDateTime(), new_nb );
	mPlot3DView->SetZAxisDigits( mPlot3DView->ZAxisIsDateTime(), new_nb );
}





//////////////////
//	...labels


bool CPlotEditor::HandleAxisLabelChanged( QLineEdit *label_edit, std::string &xtitle, std::string &ytitle, std::string &ztitle, std::string &new_title )
{
	CWidgetField *pdata = mCurrentPlotFieldYFX ? static_cast< CWidgetField* >( mCurrentPlotFieldYFX ) : static_cast< CWidgetField* >( mCurrentPlotFieldZFXY );

	assert__( mPlotType != eHistogram );		assert__( mPlot2DView || mPlot3DView );		assert__( pdata );      Q_UNUSED( pdata );

	const int index = mTabCurveOptions->mFieldsList->currentRow();

	if ( mPlotType == eHistogram )	//viewing an histogram; HandleAxisLabelChanged should never be called in this case, even if another type of plot is being displayed
	{
		assert__( false );
		return false;
	}
	else
	if ( mPlot3DView )
		mPlot3DView->CurrentAxisTitles( xtitle, ytitle, ztitle );
	else
		mPlot2DView->CurrentAxisTitles( &xtitle, &ytitle, &ztitle );

	QString new_label = label_edit->text();
	if ( new_label.isEmpty() )
	{
		LOG_WARN( "Axis labels cannot be empty" );
		return false;
	}

	new_title = q2a( new_label );

	if ( mPlot2DView )
	{
		if ( mCurrentPlotFieldYFX )
			mPlot2DView->SetAllCurvesAxisTitles( xtitle, ytitle );
		else
			mPlot2DView->SetSpectrogramAxisTitles( index, xtitle, ytitle, ztitle );
	}

	if ( mPlot3DView )
	{
		mPlot3DView->SetAllXYAxisTitles( xtitle, ytitle );
		mPlot3DView->SetZAxisTitle( index, ztitle );
	}

	return true;
}

void CPlotEditor::HandleXAxisLabelChanged()
{
	CPlotInterfaces *pplot = mCurrentPlotYFX ? static_cast< CPlotInterfaces* >( mCurrentPlotYFX ) : static_cast< CPlotInterfaces* >( mCurrentPlotZFXY );

	std::string xtitle, ytitle, ztitle;
	if ( HandleAxisLabelChanged( mTabAxisOptions->mXAxisLabel, xtitle, ytitle, ztitle, xtitle ) )
		pplot->SetXtitle( xtitle );
	else
		mTabAxisOptions->mXAxisLabel->setText( xtitle.c_str() );
}

void CPlotEditor::HandleYAxisLabelChanged()
{
	CPlotInterfaces *pplot = mCurrentPlotYFX ? static_cast< CPlotInterfaces* >( mCurrentPlotYFX ) : static_cast< CPlotInterfaces* >( mCurrentPlotZFXY );

	std::string xtitle, ytitle, ztitle;
	if ( HandleAxisLabelChanged( mTabAxisOptions->mYAxisLabel, xtitle, ytitle, ztitle, ytitle ) )
		pplot->SetYtitle( ytitle );
	else
		mTabAxisOptions->mYAxisLabel->setText( ytitle.c_str() );
}

void CPlotEditor::HandleZAxisLabelChanged()
{
	assert__( mCurrentPlotZFXY );

	const int index = mTabCurveOptions->mFieldsList->currentRow();

	std::string xtitle, ytitle, ztitle;
	if ( HandleAxisLabelChanged( mTabAxisOptions->mZAxisLabel, xtitle, ytitle, ztitle, ztitle ) )
		mCurrentPlotZFXY->SetValueTitle( index, ztitle );
	else
		mTabAxisOptions->mZAxisLabel->setText( ztitle.c_str() );
}




//////////////////
//	...scales						  //TODO display data assignment


void CPlotEditor::HandleX2DScaleSpinChanged( double x )
{
	assert__( mPlot2DView );

	//NOTE this triggers, but allow self notification to update range edit widget:
	//	Handle2DScaleChanged only updates spin if value is different, so this handler
	//	won't be called again
	//
	mPlot2DView->RescaleX( x );
}


void CPlotEditor::HandleY2DScaleSpinChanged( double y )
{
	assert__( mPlot2DView );

	//NOTE this triggers, but allow self notification to update range edit widget:
	//	Handle2DScaleChanged only updates spin if value is different, so this handler
	//	won't be called again
	//
	mPlot2DView->RescaleY( y );
}


void CPlotEditor::HandleX3DScaleSpinChanged( double x )
{
	assert__( mPlot2DView );

	double xVal, yVal, zVal;
	mPlot3DView->Scale( xVal, yVal, zVal );
	mPlot3DView->blockSignals( true );		//do not notify ourselves
	mPlot3DView->SetScale( x, yVal, zVal );
	mPlot3DView->blockSignals( false );
}


void CPlotEditor::HandleY3DScaleSpinChanged( double y )
{
	assert__( mPlot3DView );

	double xVal, yVal, zVal;
	mPlot3DView->Scale( xVal, yVal, zVal );
	mPlot3DView->blockSignals( true );		//do not notify ourselves
	mPlot3DView->SetScale( xVal, y, zVal );
	mPlot3DView->blockSignals( false );
}


void CPlotEditor::HandleZ3DScaleSpinChanged( double z )
{
	assert__( mPlot3DView );

	double xVal, yVal, zVal;
	mPlot3DView->Scale( xVal, yVal, zVal );
	mPlot3DView->blockSignals( true );		//do not notify ourselves
	mPlot3DView->SetScale( xVal, yVal, z );
	mPlot3DView->blockSignals( false );
}





///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_PlotEditor.cpp"
