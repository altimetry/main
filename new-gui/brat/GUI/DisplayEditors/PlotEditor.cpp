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

	//connect( mTabAxisOptions->mX_axis->mLogScaleCheck, SIGNAL( toggled( bool ) ), this, SLOT( HandleLogarithmicScaleX( bool ) ) );
	//connect( mTabAxisOptions->mY_axis->mLogScaleCheck, SIGNAL( toggled( bool ) ), this, SLOT( HandleLogarithmicScaleY( bool ) ) );
	//connect( mTabAxisOptions->mZ_axis->mLogScaleCheck, SIGNAL( toggled( bool ) ), this, SLOT( HandleLogarithmicScaleZ( bool ) ) );


    // spectrogram

	connect( mTabCurveOptions->mShowContour, SIGNAL( toggled( bool ) ), this, SLOT( HandleShowContourChecked( bool ) ) );
	connect( mTabCurveOptions->mShowSolidColor, SIGNAL( toggled( bool ) ), this, SLOT( HandleShowSolidColorChecked( bool ) ) );


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
	, mDataArrayXY( new CXYPlotDataCollection )
	, mDataArrayZFXY( new CObArray )
{
    CreateWidgets();

	Start( display_name );
}


CPlotEditor::CPlotEditor( const CDisplayFilesProcessor *proc, CPlot* plot )
	: base_t( false, proc )
	, mDataArrayXY( new CXYPlotDataCollection )
	, mDataArrayZFXY( new CObArray )
{
    Q_UNUSED(proc);     Q_UNUSED(plot);

    CreateWidgets();

	Start( "" );
}

CPlotEditor::CPlotEditor( const CDisplayFilesProcessor *proc, CZFXYPlot* plot )
	: base_t( false, proc )
	, mDataArrayXY( new CXYPlotDataCollection )
	, mDataArrayZFXY( new CObArray )
{
    Q_UNUSED(proc);     Q_UNUSED(plot);

    CreateWidgets();

	Start( "" );
}




//virtual 
CPlotEditor::~CPlotEditor()
{
	delete mDataArrayXY;
	delete mDataArrayZFXY;
}





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
bool CPlotEditor::ResetViews( bool reset_2d, bool reset_3d, bool enable_2d, bool enable_3d )
{
	assert__( enable_2d );		//2d displays all types

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


template< typename T >
inline std::string TF( const std::string &s, T n )
{
	return s + " == " + n2s<std::string>( n ) + "\n";
};

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
	size_t nframes, nvalues, nvalues0, ndefault_values;
	nframes = nvalues = nvalues0 = ndefault_values = 0;
	std::string error_msg, field;

	if ( !mDataArrayXY->empty() )
	{
		assert__( mDataArrayZFXY->empty() );

		CXYPlotData *pdata = mDataArrayXY->Get( index );
		const CQwtArrayPlotData &data = *pdata->GetQwtArrayPlotData();
		field = pdata->GetName();

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
	}
	if ( !mDataArrayZFXY->empty() )
	{
		assert__( mDataArrayXY->empty() );

		CZFXYPlotData *pdata = dynamic_cast<CZFXYPlotData*>( mDataArrayZFXY->at( index ) );
		const C3DPlotInfo &data = pdata->Maps();
		field = pdata->GetDataName();

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
	}

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

	if ( !mDataArrayXY->empty() )
	{
		Recreate2DPlots();
	}
	else
	if ( !mDataArrayZFXY->empty() )
	{
		Recreate3DPlots( true, false );
	}
	else
		assert( false );
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
	return CreatePlotData( type );
}

//
//	CXYPlotData		is a	CBratObject							
//					has a	vtkDataArrayPlotData
//								has a vtkDoubleArrayBrathl *XDataArray;
//								has a vtkDoubleArrayBrathl *YDataArray;						
//								has a vtkDoubleArrayBrathl *ZDataArray;						
//										
//	CPlotArray	has a 	vtkObArray (array of frames)
//
//
//	CXYPlotData		=> Create	=> SetData( 
//	(has a vtkDataArrayPlotData)			double* xArray, const CUIntArray& xDims, 
//											double* yArray, const CUIntArray& yDims,
//											const CUIntArray& xCommonDimIndex,
//											const CUIntArray& yCommonDimIndex ) => ...
//
//	... => CPlotArray::SetFrameData for each frame, 
//		and at the end:
//	... => SetRawData( CPlotArray* x, CPlotArray* y ) => Update() => m_vtkDataArrayPlotData::SetDataArrays( vtkDoubleArrayBrathl, vtkDoubleArrayBrathl )
//
bool CPlotEditor::CreatePlotData( EPlotType type )
{
	WaitCursor wait;

	try
	{
		mPlotType = type;							assert__( mPlotType != eHistogram );
		
		std::vector< CPlot* > yfx_plots;
		mDataArrayXY->clear();
		mPropertiesXY = nullptr;

		std::vector< CZFXYPlot* > zfxy_plots;
		mDataArrayZFXY->clear();
		mPropertiesZFXY = nullptr;


		if ( mDisplay->IsYFXType() )
        {
            yfx_plots = GetPlotsFromDisplayFile< CPlot >( mDisplay );
        }
        else
		if ( mDisplay->IsZYFXType() || mDisplay->IsZLatLonType() )
		{
			zfxy_plots = GetPlotsFromDisplayFile< CZFXYPlot >( mDisplay );
		}
		else
			assert__( false );						assert__( ( yfx_plots.size() || zfxy_plots.size() ) && mCurrentDisplayFilesProcessor );


		if ( !ResetViews( true, mPlotType != eHistogram, true, zfxy_plots.size() > 0 ) )
			throw CException( "A previous plot is still processing. Please try again later." );

		SelectTab( TabGeneral()->parentWidget() );


		int yfx_index = 0;							assert__( yfx_plots.size() <= 1 );		//forces redesign if false
		for ( auto *yfxplot : yfx_plots )												//CBratDisplayApp::XYPlot()
		{
			assert__( yfxplot != nullptr );			assert__( mPlotType == eXY || mPlotType == eHistogram );

			//CBratDisplayApp::CreateXYPlot

			mPlot2DView->setWindowTitle( t2q( yfxplot->MakeTitle() ) );
			mPlot2DView->SetPlotTitle( q2a( mPlot2DView->windowTitle() ) );		//in v3 was inside the loop

			int nrFields = (int)yfxplot->m_fields.size();		assert__( nrFields == mCurrentDisplayFilesProcessor->GetXYPlotPropertiesSize() );
			for ( int i = 0; i < nrFields; i++ )
			{
				//==>CXYPlotFrame::AddData(CXYPlotData* data) ==>CXYPlotPanel::AddData( CXYPlotData* pdata )

				mDataArrayXY->AddData( new CXYPlotData( yfxplot, i ) );				//v4 note: CXYPlotData ctor only invoked here
			}

            Recreate2DPlots();

			yfx_index++;
		}

		
		int zfxy_index = 0;							assert__( zfxy_plots.size() <= 1 );		//forces redesign if false
		for ( auto *zfxyplot : zfxy_plots )
		{
			assert__( zfxyplot != nullptr );		assert__( mPlotType == eXYZ || mPlotType == eHistogram );

			auto *props = mCurrentDisplayFilesProcessor->GetZFXYPlotProperties( zfxy_index++ );			//zfxyplot->GetInfo();	 assumed done by caller

			Q_UNUSED( props );  //mPlotProperties3D = *props;	// *proc->GetZFXYPlotProperties( 0 );
			mPlot2DView->setWindowTitle( t2q( zfxyplot->MakeTitle() ) );
			mPlot3DView->setWindowTitle( t2q( zfxyplot->MakeTitle() ) );

			//CZFXYPlotFrame* frame = new CZFXYPlotFrame( nullptr, -1, title, zfxyPlotProperty, pos, size );

			for ( auto &itField : zfxyplot->m_fields )
			{
				CPlotField* field = CPlotField::GetPlotField( itField );

#if defined(_DEBUG) || defined(DEBUG) 
				if ( field->m_zfxyProps != props )
					LOG_WARN( "field->m_worldProps != props" );
#endif
				if ( field->m_internalFiles.empty() )
					continue;

				//frame->AddData( geoMap );
				mDataArrayZFXY->Insert( new CZFXYPlotData( zfxyplot, field ) );		// v4 note: CZFXYPlotData ctor only invoked here
			}

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
	assert__( mPlot2DView && mCurrentDisplayFilesProcessor );

	mMultiFrame = mHasClut = false;
	int n_frames = 1;
	mTabCurveOptions->mFieldsList->clear();

	const size_t size = mDataArrayZFXY->size();				assert__( size == mCurrentDisplayFilesProcessor->GetZFXYPlotPropertiesSize() );
	for ( size_t i = 0; i < size; ++i )
	{
		CZFXYPlotData *pdata = dynamic_cast<CZFXYPlotData*>( mDataArrayZFXY->at( i ) );		assert__( pdata != nullptr );

		if ( n_frames == 1 )
			n_frames = pdata->GetNrMaps();
#if defined (DEBUG) || defined(_DEBUG)				//make sure the number of frames is the same for all data
		else
			assert__( n_frames == pdata->GetNrMaps() );
#endif

#if defined (BRAT_V3)									  //TODO this file is never executed in v3, but remains here as a remark for something whose port is missing
		if ( !mHasClut && pdata->GetLookupTable() != nullptr )
#else
		if ( !mHasClut )
#endif
		{
			mHasClut = true;
			// TODO translate this to v4
			//m_menuBar->Enable( ID_MENU_VIEW_COLORBAR, m_hasClut );
			//m_menuBar->Enable( ID_MENU_VIEW_CLUTEDIT, m_hasClut );
			//v3 commented out ShowColorBar(data->m_plotProperty.m_showColorBar);
		}

		// void CZFXYPlotPanel::AddData( CZFXYPlotData* pdata, wxProgressDialog* dlg )

		mPropertiesZFXY = pdata->GetPlotProperties();											assert__( mPropertiesZFXY );
		mCurrentDisplayData = mDisplay->GetDisplayData( mOperation, pdata->GetDataName() );		assert__( mCurrentDisplayData && pdata->GetDataName() == mPropertiesZFXY->m_name );

		const C3DPlotInfo &maps = pdata->Maps();	assert__( maps.size() == 1 );	//simply to check if ever...
		if ( build2d )
		{
			if ( mPlotType == eHistogram )
			{
				double m, M, max_freq;
				mPlot2DView->PushHistogram( mPropertiesZFXY->m_name, Qt::green, &maps, max_freq, mCurrentDisplayData->GetNumberOfBins() );
				maps.GetDataRange( m, M, 0 );

				//axis scale
				mPlot2DView->SetPlotAxisScales( i, m, M, 0, max_freq );
			}
			else
			{
				//AddRaster assigns axis scale

				mPlot2DView->PushRaster( mPropertiesZFXY->m_name, maps, mPropertiesZFXY->m_minContourValue, mPropertiesZFXY->m_maxContourValue, mPropertiesZFXY->m_numContour );
			}

			mPlot2DView->SetPlotTitle( q2a( windowTitle() ) );

			mPlot2DView->SetAxisTitles( i, mPropertiesZFXY->m_xLabel, mPropertiesZFXY->m_yLabel, mPropertiesZFXY->m_name );
		}

		if ( build3d )
		{
			const C3DPlotParameters &values = maps[ 0 ];
			mPlot3DView->PushPlot( values,
				pdata->GetXRangeMin(), pdata->GetXRangeMax(), pdata->GetYRangeMin(), pdata->GetYRangeMax(),
				pdata->GetComputedRangeMin(),
				pdata->GetComputedRangeMax() );

			mPlot3DView->SetPlotTitle( q2a( windowTitle() ) );

			mPlot3DView->SetAxisTitles( mPropertiesZFXY->m_xLabel, mPropertiesZFXY->m_yLabel, mPropertiesZFXY->m_name );
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////

		mTabCurveOptions->mFieldsList->addItem( t2q( mPropertiesZFXY->m_name ) );

		if ( mPlotType != eHistogram )
		{
			//ticks: in 3D works per current plot (field) so are assigned here, but with 2D...

			unsigned int x_nticks = mPropertiesZFXY->m_xNumTicks, y_nticks = mPropertiesZFXY->m_xNumTicks, z_nticks = mPropertiesZFXY->m_yNumTicks;
			if ( build2d )
			{
				if ( x_nticks > 0 && !isDefaultValue( x_nticks ) )
					mPlot2DView->SetXAxisNbTicks( x_nticks );
				if ( y_nticks > 0 && !isDefaultValue( y_nticks ) )
					mPlot2DView->SetYAxisNbTicks( y_nticks );
			}
			if ( build3d )
			{
				if ( x_nticks > 0 && !isDefaultValue( x_nticks ) )
					mPlot3DView->SetXAxisNbTicks( x_nticks );
				if ( y_nticks > 0 && !isDefaultValue( y_nticks ) )
					mPlot3DView->SetYAxisNbTicks( y_nticks );
				if ( z_nticks > 0 && !isDefaultValue( z_nticks ) )
					mPlot3DView->SetZAxisNbTicks( z_nticks );
			}

			//digits
			//
			// NOTE this approach does not work per field (if any does)
			//
			unsigned x_digits = 3, y_digits = 3;// , z_digits = 3;
			if ( build2d )
			{
				if ( x_digits > 0 && !isDefaultValue( x_digits ) )
					mPlot2DView->SetXAxisMantissa( x_digits );
				if ( y_digits > 0 && !isDefaultValue( y_digits ) )
					mPlot2DView->SetYAxisMantissa( y_digits );
			}
			if ( build3d )
			{
				//if ( x_digits > 0 && !isDefaultValue( x_digits ) )
				//	mPlot3DView->SetXAxisMantissa( x_digits );
				//if ( y_digits > 0 && !isDefaultValue( y_digits ) )
				//	mPlot3DView->SetYAxisMantissa( y_digits );
				//if ( z_digits > 0 && !isDefaultValue( z_digits ) )
				//	mPlot3DView->SetZAxisMantissa( z_digits );
			}


			//color/contour

			if ( build2d )
			{
				mPlot2DView->ShowContour( i, mPropertiesZFXY->m_withContour );
				mPlot2DView->ShowSolidColor( i, mPropertiesZFXY->m_solidColor );
			}

			if ( build3d )
			{
				mPlot3DView->ShowContour( i, mPropertiesZFXY->m_withContour );		//in fact, in qwtplot3d this affects the axis (all fields) not only the current iteration one
				mPlot3DView->ShowSolidColor( i, mPropertiesZFXY->m_solidColor );	//idem
			}
		}
	}


	//animation

	mMultiFrame = mPlotType != eHistogram && n_frames > 1;
	mTabAnimateOptions->setEnabled( mMultiFrame );				assert__( !mMultiFrame );		//TODO check this


	//interaction
	if ( mPlotType == eHistogram )
	{
		//
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
		mPlot2DView->AddZoomer();
		mPlot2DView->AddPanner();

		mTabCurveOptions->SwitchTo3D();
		mTabAxisOptions->SwitchTo3D();
	}

	//GO: select field
	mTabCurveOptions->mFieldsList->setCurrentRow( 0 );
}



//////////////////
// I.2.	y=f(x)
//////////////////

//plot->GetInfo(); 	 assumed done by GetPlotsFromDisplayFile
//
void CPlotEditor::Recreate2DPlots()
{
	assert__( mPlot2DView && mCurrentDisplayFilesProcessor );

	mTabCurveOptions->mFieldsList->clear();
	mTabCurveOptions->mFieldsList->setSelectionMode( mTabCurveOptions->mFieldsList->SingleSelection );

	int n_frames = 1;

	//for histogram scale
	double min_y, max_y, max_freq;
	max_y = max_freq = std::numeric_limits<double>::lowest();
	min_y = std::numeric_limits<double>::max();

	int nrFields = (int)mDataArrayXY->size();					assert__( nrFields == mCurrentDisplayFilesProcessor->GetXYPlotPropertiesSize() );
	for ( int i = 0; i < nrFields; i++ )
	{
		CXYPlotData *pdata = mDataArrayXY->Get( i );

		//==>CXYPlotFrame::AddData(CXYPlotData* data) ==>CXYPlotPanel::AddData( CXYPlotData* pdata )

		if ( n_frames == 1 )
			n_frames = pdata->GetNumberOfFrames();
#if defined (DEBUG) || defined(_DEBUG)				//make sure the number of frames is the same for all data
		else
			assert__( n_frames == pdata->GetNumberOfFrames() );
#endif

		mPropertiesXY = mDataArrayXY->Get( i )->GetPlotProperties();						assert__( mPropertiesXY );
		mCurrentDisplayData = mDisplay->GetDisplayData( mOperation, pdata->GetName() );		assert__( mCurrentDisplayData && pdata->GetName() == mPropertiesXY->GetName() );

		double xrMin = 0;
		double xrMax = 0;
		double yrMin = 0;
		double yrMax = 0;

		if ( !isDefaultValue( mPropertiesXY->GetXMin() ) )
		{
			xrMin = mPropertiesXY->GetXMin();
		}
		if ( !isDefaultValue( mPropertiesXY->GetXMax() ) )
		{
			xrMax = mPropertiesXY->GetXMax();
		}

		if ( !isDefaultValue( mPropertiesXY->GetYMin() ) )
		{
			yrMin = mPropertiesXY->GetYMin();
		}
		if ( !isDefaultValue( mPropertiesXY->GetYMax() ) )
		{
			yrMax = mPropertiesXY->GetYMax();
		}


		// Create Plots
		QwtPlotCurve *curve = nullptr;
		CHistogram *histo = nullptr;
		if ( mPlotType == eHistogram )
		{
			//use xrMin for other purpose
			histo = mPlot2DView->AddHistogram( mPropertiesXY->GetName(), color_cast<QColor>( mPropertiesXY->GetColor() ), pdata->GetQwtArrayPlotData(), xrMin, mCurrentDisplayData->GetNumberOfBins() );
			max_freq = std::max( max_freq, xrMin );
			min_y = std::min( min_y, yrMin );
			max_y = std::max( max_y, yrMax );
		}
		else
			curve = mPlot2DView->AddCurve( mPropertiesXY->GetName(), color_cast<QColor>( mPropertiesXY->GetColor() ), pdata->GetQwtArrayPlotData() );


		// axis labels: assign here, as v3 (assuming common labels)

		if ( mPlotType == eHistogram )
			mPlot2DView->SetAxisTitles( i, mPropertiesXY->GetYLabel(), "Frequency" );
		else
			mPlot2DView->SetAxisTitles( i, mPropertiesXY->GetXLabel(), mPropertiesXY->GetYLabel() );


		// axis scales: assign here, as v3 (assuming common scale)

		if ( mPlotType != eHistogram )
			mPlot2DView->SetPlotAxisScales( i, xrMin, xrMax, yrMin, yrMax );


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		mTabCurveOptions->mFieldsList->addItem( t2q( mPropertiesXY->GetName() ) );

		if ( mPlotType != eHistogram )
		{
			//curve line
			mPlot2DView->SetCurveLineColor( i, mPropertiesXY->GetColor() );
			mPlot2DView->SetCurveLineOpacity( i, mPropertiesXY->GetOpacity() );
			mPlot2DView->SetCurveLinePattern( i, mPropertiesXY->GetStipplePattern() );
			mPlot2DView->SetCurveLineWidth( i, mPropertiesXY->GetLineWidth() );

			//curve point
			mPlot2DView->SetCurvePointColor( i, mPropertiesXY->GetPointColor() );
			mPlot2DView->SetCurvePointFilled( i, mPropertiesXY->GetFilledPoint() );
			mPlot2DView->SetCurvePointGlyph( i, mPropertiesXY->GetPointGlyph() );
			mPlot2DView->SetCurvePointSize( i, mPropertiesXY->GetPointSize() );
		}
	}

	// axis scale 

	if ( mPlotType == eHistogram )
		for ( int i = 0; i < nrFields; i++ )
			mPlot2DView->SetPlotAxisScales( i, min_y, max_y, 0, max_freq );


	//ticks
	//
	// NOTE this approach does not work per field (if any does)
	//
	unsigned x_nticks = mPropertiesXY->GetXNumTicks(), y_nticks = mPropertiesXY->GetYNumTicks();
	if ( x_nticks > 0 && !isDefaultValue( x_nticks ) )
		mPlot2DView->SetXAxisNbTicks( x_nticks );
	if ( y_nticks > 0 && !isDefaultValue( y_nticks ) )
		mPlot2DView->SetYAxisNbTicks( y_nticks );

	//digits
	//
	// NOTE this approach does not work per field (if any does)
	//
	unsigned x_digits = mPropertiesXY->GetXNbDigits(), y_digits = mPropertiesXY->GetYNbDigits();
	if ( x_digits > 0 && !isDefaultValue( x_digits ) )
		mPlot2DView->SetXAxisMantissa( x_digits );
	if ( y_digits > 0 && !isDefaultValue( y_digits ) )
		mPlot2DView->SetYAxisMantissa( y_digits );


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
	mTabCurveOptions->mFieldsList->setCurrentRow( 0 );
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//									II. From plot to widgets
//								(&& assign "current" pointers)
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////



void CPlotEditor::HandleCurrentFieldChanged( int index )
{
	assert__( mPlot2DView );

	mTabCurveOptions->mLineOptions->setEnabled( index >= 0 && mDataArrayZFXY->empty() && mPlotType != eHistogram );
	mTabCurveOptions->mPointOptions->setEnabled( index >= 0 && mDataArrayZFXY->empty() && mPlotType != eHistogram );
	mTabCurveOptions->mSpectrogramOptions->setEnabled( index >= 0 && !mDataArrayZFXY->empty() );

	mCurrentDisplayData = nullptr;
	mPropertiesXY = nullptr;
	mPropertiesZFXY = nullptr;

	if ( index < 0 )
		return;

    //axis

	//...labels
	std::string xtitle, ytitle, ztitle;
	if ( mPlotType == eHistogram )
		mPlot2DView->HistogramAxisTitles( index, xtitle, ytitle, ztitle );
	else
	if ( !mDataArrayZFXY->empty() )
		mPlot2DView->SpectrogramAxisTitles( index, xtitle, ytitle, ztitle );
	else
		mPlot2DView->AxisTitles( &xtitle, &ytitle, &ztitle );

    mTabAxisOptions->mXAxisLabel->setText( xtitle.c_str() );
    mTabAxisOptions->mYAxisLabel->setText( ytitle.c_str() );
    mTabAxisOptions->mZAxisLabel->setText( ztitle.c_str() );

	//...ticks
	mTabAxisOptions->mXNbTicks->setText( n2s<std::string>( mPlot2DView->XAxisNbTicks() ).c_str() );
	mTabAxisOptions->mYNbTicks->setText( n2s<std::string>( mPlot2DView->YAxisNbTicks() ).c_str() );
	mTabAxisOptions->mZNbTicks->setText( n2s<std::string>( mPlot3DView ? mPlot3DView->ZAxisNbTicks() : 0 ).c_str() );

	//...scales
	//scales are sensitive to user interaction so widget assignment is automatically done by remaining handlers in this file's section

	//...digits
	mTabAxisOptions->mXNbDigits->setText( n2s<std::string>( mPlot2DView->GetXAxisMantissa() ).c_str() );
	mTabAxisOptions->mYNbDigits->setText( n2s<std::string>( mPlot2DView->GetYAxisMantissa() ).c_str() );
	//mTabAxisOptions->mZNbDigits->setText( n2s<std::string>( mPlot3DView->GetZAxisMantissa() ).c_str() );


	//bins

	if ( mPlotType == eHistogram )
	{		
		mTabCurveOptions->mNumberOfBins->setText( n2s<std::string>( mPlot2DView->NumberOfBins( index ) ).c_str() );
	}

	if ( !mDataArrayZFXY->empty() )
	{
		assert__( index < (int)mDataArrayZFXY->size() );

		mPropertiesZFXY = dynamic_cast<CZFXYPlotData*>( mDataArrayZFXY->at( index ) )->GetPlotProperties();		assert__( mPropertiesZFXY );
		mCurrentDisplayData = mDisplay->GetDisplayData( mOperation, mPropertiesZFXY->m_name );					assert__( mCurrentDisplayData );

		if ( mPlotType == eHistogram )
		{
			mPlot2DView->SetCurrentHistogram( index );
		}
		else
		{
			mPlot2DView->SetCurrentRaster( index );
		}
		mPlot3DView->SetCurrentPlot( index );

		if ( mPlotType == eHistogram )
		{
			//TODO temporarily here, while axis work is not done
			mPlot2DView->SetAxisTitles( index, mPropertiesZFXY->m_name, "Frequency" );
		}
		else {
			//TODO temporarily here, while axis work is not done
			mPlot2DView->SetAxisTitles( index, mPropertiesZFXY->m_xLabel, mPropertiesZFXY->m_yLabel, mPropertiesZFXY->m_name );

			assert__( mPlot2DView->HasContour() == mPlot3DView->HasContour() );
			assert__( mPlot2DView->HasSolidColor() == mPlot3DView->HasSolidColor() );
		}
		
		//use 3d widget; 2d can be on histogram mode
		mTabCurveOptions->mShowContour->setChecked( mPlot3DView->HasContour() );		
		mTabCurveOptions->mShowSolidColor->setChecked( mPlot3DView->HasSolidColor() );	

		return;
	}

	assert__( index < (int)mDataArrayXY->size() );

	mPropertiesXY = mDataArrayXY->Get( index )->GetPlotProperties();							assert__( mPropertiesXY );
	mCurrentDisplayData = mDisplay->GetDisplayData( mOperation, mPropertiesXY->GetName() );		assert__( mCurrentDisplayData );


	if ( mPlotType == eHistogram )
	{		
		return;
	}

	//curve line
	mTabCurveOptions->mLineColorButton->SetColor( mPlot2DView->CurveLineColor( index ) );
	mTabCurveOptions->mLineOpacityValue->setText( n2s<std::string>( mPlot2DView->CurveLineOpacity( index ) ).c_str() );
	mTabCurveOptions->mStipplePattern->setCurrentIndex( mPlot2DView->CurveLinePattern( index ) );
	mTabCurveOptions->mLineWidthValue->setText( n2s<std::string>( mPlot2DView->CurveLineWidth( index ) ).c_str() );

	//curve point
	mTabCurveOptions->mPointColorButton->SetColor( mPlot2DView->CurvePointColor( index ) );
	mTabCurveOptions->mFillPointCheck->setChecked( mPlot2DView->IsCurvePointFilled( index ) );
	mTabCurveOptions->mPointGlyph->setCurrentIndex( mPlot2DView->CurvePointGlyph( index ) );
	mTabCurveOptions->mPointSizeValue->setText( n2s<std::string>( mPlot2DView->CurvePointSize( index ) ).c_str() );

	//enable/disable according to data
	mTabCurveOptions->mLineOptions->setChecked( mPlotType != eHistogram && mPropertiesXY->GetLines() );
	mTabCurveOptions->mPointOptions->setChecked( mPropertiesXY->GetPoints() );
}


void CPlotEditor::Handle3DScaleChanged( double xVal, double yVal, double zVal )
{
	mTabAxisOptions->mX3DScaleSpin->setValue( xVal );
	mTabAxisOptions->mY3DScaleSpin->setValue( yVal );
	mTabAxisOptions->mZ3DScaleSpin->setValue( zVal );
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

	spin->blockSignals( true );
	spin->setValue( factor );
	spin->blockSignals( false );

	edit->setText( range );	
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
	assert__( mPlot2DView && mPlot3DView && mDataArrayZFXY && mPropertiesZFXY );

	mPropertiesZFXY->m_withContour = checked;

	if ( mPlotType != eHistogram )
		mPlot2DView->ShowContour( mPropertiesZFXY->m_withContour );
	mPlot3DView->ShowContour( mPropertiesZFXY->m_withContour );
}

void CPlotEditor::HandleShowSolidColorChecked( bool checked )
{
	assert__( mPlot2DView && mPlot3DView && mPropertiesZFXY && mDataArrayZFXY );

	mPropertiesZFXY->m_solidColor = checked;

	if ( mPlotType != eHistogram )
		mPlot2DView->ShowSolidColor( mPropertiesZFXY->m_solidColor );
	mPlot3DView->ShowSolidColor( mPropertiesZFXY->m_solidColor );
}



/////////////////////////
// line properties
/////////////////////////

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


/////////////////////////
// curve properties
/////////////////////////

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



/////////////////////////
// line/curve properties
/////////////////////////

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




/////////////////////////
// animation properties
/////////////////////////


void CPlotEditor::HandleAnimateButtonToggled( bool toggled )
{
	assert__( mPlot2DView  && mPropertiesXY );

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
	assert__( mPlot2DView  && mPropertiesXY );

	mPlot2DView->StopAnimation();
	mTabAnimateOptions->mAnimateButton->setChecked( false );
}


void CPlotEditor::HandleAnimateLoop( bool toggled )
{
	assert__( mPlot2DView  && mPropertiesXY );

	mTabAnimateOptions->mFrameIndexSpin->setWrapping( toggled );

	mPlot2DView->SetLoop( toggled );
}


void CPlotEditor::HandleAnimateBackwardToggled( bool toggled )
{
	assert__( mPlot2DView  && mPropertiesXY );

	mPlot2DView->SetMovingForward( !toggled );
}


void CPlotEditor::HandleAnimateForwardToggled( bool toggled )
{
	assert__( mPlot2DView  && mPropertiesXY );

	mPlot2DView->SetMovingForward( toggled );
}


void CPlotEditor::AnimationStopped()
{
	assert__( mPlot2DView  && mPropertiesXY );

	mTabAnimateOptions->mAnimateButton->setChecked( false );
}


void CPlotEditor::HandleFrameIndexSpinChanged( int i )
{
	assert__( mPlot2DView  && mPropertiesXY );

	// If signal was initially sent by spin, call mPlot2DView ->SetFrame; if not, 
	//	the spin itself is being updated by the mPlot2DView signal (connected above)
	//
	if ( mTabAnimateOptions->mFrameIndexSpin->isEnabled() )		
		mPlot2DView->SetFrame( i );

	SetAnimationDescr( i );
}


void CPlotEditor::SetAnimationDescr( int frame )
{
	assert__( mPlot2DView  && mPropertiesXY );

	mTabAnimateOptions->mInformation->setText( "" );

	std::string descr;

	const size_t ndata = mDataArrayXY->size();

	for ( size_t i = 0; i < ndata; i++ )
	{
		CXYPlotData* data = mDataArrayXY->Get( i );

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
				+ data->GetName()
				+ "')\n" );
		}
		else
		{
			descr += (
				"'"
				+ field_descr
				+ "' value for above plot: "
				+ n2s<std::string>( data->GetVarComplement().GetValues()[ index ] )
				+ " "
				+ str_unit
				+ " (field '"
				+ data->GetName()
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
					+ data->GetName()
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
					value_str = n2s<std::string>(value) + " " + str_unit;
				}

				descr += ( 
					"'"
					+ field_descr
					+ "' value for above plot: "
					+ value_str
					+ " (field '"
					+ data->GetName()
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
	assert__( mPlot2DView && mCurrentDisplayData );

    const QString qbins = mTabCurveOptions->mNumberOfBins->text();
	bool is_converted = false;
	unsigned bins = qbins.toInt( &is_converted, 10 );
	if ( !is_converted )
	{
		bins = mPlot2DView->NumberOfBins( mTabCurveOptions->mFieldsList->currentRow() );
		mTabCurveOptions->mNumberOfBins->blockSignals( true );
		mTabCurveOptions->mNumberOfBins->setText( n2s<std::string>( bins ).c_str() );
		mTabCurveOptions->mNumberOfBins->blockSignals( false );
		LOG_WARN( "Invalid number of bins" );
		return;
	}	

	mCurrentDisplayData->SetNumberOfBins( bins );	
	HandlePlotTypeChanged( int( eHistogram ) );
}




//////////////////////////////////////////////////////////////////////////
//	axis  properties
//////////////////////////////////////////////////////////////////////////

//////////////////
//	...logarithmic

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


//////////////////
//	...ticks

static inline int ProcessNbTicks( QLineEdit *edit, unsigned int def_value )
{
	bool ok_conv = false;
	QString new_nb_ticks = edit->text();
	int new_nb = new_nb_ticks.toInt( &ok_conv );
	if ( !ok_conv || new_nb > min_plot_widget_width / 5 )	//TODO arbitrary value
	{
		new_nb = def_value;
		edit->blockSignals( true );
		edit->setText( n2s<std::string>( new_nb ).c_str() );
		edit->blockSignals( false );
		LOG_WARN( "Invalid number of ticks" );
		return -1;
	}
	return new_nb;
}


void CPlotEditor::HandleXNbTicksChanged()
{
	assert__( mPlot2DView || mPlot3DView );		assert__( mPropertiesZFXY || mPropertiesXY );

	int new_nb = ProcessNbTicks( mTabAxisOptions->mXNbTicks, mPlot2DView->XAxisNbTicks() );
	if ( new_nb < 0 )
		return;

	if ( mPlot2DView )
	{
		mPlot2DView->SetXAxisNbTicks( new_nb );
	}
	if ( mPlot3DView )
	{
		mPlot3DView->SetXAxisNbTicks( new_nb );
	}
	
	if (mPropertiesZFXY)
		mPropertiesZFXY->m_xNumTicks = new_nb;
	else
		mPropertiesXY->SetXNumTicks( new_nb );
}


void CPlotEditor::HandleYNbTicksChanged()
{
	assert__( mPlot2DView || mPlot3DView );		assert__( mPropertiesZFXY || mPropertiesXY );

	int new_nb = ProcessNbTicks( mTabAxisOptions->mYNbTicks, mPlot2DView->YAxisNbTicks() );
	if ( new_nb < 0 )
		return;

	if ( mPlot2DView )
	{
		mPlot2DView->SetYAxisNbTicks( new_nb );
	}

	if ( mPlot3DView )
	{
		mPlot3DView->SetYAxisNbTicks( new_nb );
	}

	if (mPropertiesZFXY)
		mPropertiesZFXY->m_yNumTicks = new_nb;
	else
		mPropertiesXY->SetYNumTicks( new_nb );
}


void CPlotEditor::HandleZNbTicksChanged()
{
	assert__( mPlot3DView );		assert__( mPropertiesZFXY );

	int new_nb = ProcessNbTicks( mTabAxisOptions->mZNbTicks, mPlot3DView->ZAxisNbTicks() );
	if ( new_nb < 0 )
		return;

	mPropertiesZFXY->m_zNumTicks = new_nb;
	mPlot3DView->SetZAxisNbTicks( new_nb );
}



//////////////////
//	...digits				 TODO this needs refactoring			//TODO display data assignment

void CPlotEditor::HandleXAxisNbDigitsChanged()
{
    bool ok_conv=false;
    QString new_ndigits = mTabAxisOptions->mXNbDigits->text();
    int ndigits = new_ndigits.toInt(&ok_conv);

	if ( mPlot2DView )
	{
        if ((!ok_conv) || (ndigits < 0))
        {
			if ( mPropertiesXY )
	            mTabAxisOptions->mXNbDigits->setText(QString::number(mPropertiesXY->GetXNbDigits()));
            return;
        }

		if ( mPropertiesXY )
			mPropertiesXY->SetXNbDigits(ndigits);
        //mCurrentDisplayData->SetXMantissa(mPropertiesXY->GetXNbDigits());
        mPlot2DView->SetXAxisMantissa(ndigits);
    }

	if ( mPlot3DView )
    {
        if ((!ok_conv) || (ndigits < 0))
        {
            //mTabAxisOptions->mXNbDigits->setText(QString::number(mPropertiesZFXY->m_xMantissaDigits));
            return;
        }

        //mPropertiesZFXY->m_xMantissaDigits = ndigits;
        mPlot3DView->SetXScaleConf(ndigits);
        ////trigger 3DPlot axis changing functions
    }
}

void CPlotEditor::HandleYAxisNbDigitsChanged()
{
    bool ok_conv=false;
    QString new_ndigits = mTabAxisOptions->mYNbDigits->text();
    int ndigits = new_ndigits.toInt(&ok_conv);

	if ( mPlot2DView )
    {
        if ((!ok_conv) || (ndigits < 0))
        {
			if ( mPropertiesXY )
				mTabAxisOptions->mYNbDigits->setText(QString::number(mPropertiesXY->GetYNbDigits()));
            return;
        }

		if ( mPropertiesXY )
			mPropertiesXY->SetYNbDigits(ndigits);
        //mCurrentDisplayData->SetYMantissa(mPropertiesXY->GetYNbDigits());
        mPlot2DView->SetYAxisMantissa(ndigits);
    }

	if ( mPlot3DView )
    {
        if ((!ok_conv) || (ndigits < 0))
        {
            //mTabAxisOptions->mYNbDigits->setText(QString::number(mPropertiesZFXY->m_yMantissaDigits));
            return;
        }

        //mPropertiesZFXY_3D->m_yMantissaDigits = ndigits;
        mPlot3DView->SetYScaleConf(ndigits);
        ////trigger 3DPlot axis changing functions
    }
}


void CPlotEditor::HandleZAxisNbDigitsChanged()
{
    bool ok_conv=false;
    QString new_ndigits = mTabAxisOptions->mZNbDigits->text();
    int ndigits = new_ndigits.toInt(&ok_conv);
    if (mPlotType == eXYZ)
    {
        if ((!ok_conv) || (ndigits < 0))
        {
            //mTabAxisOptions->mZNbDigits->setText(QString::number(mPropertiesZFXY->m_zMantissaDigits));
            return;
        }

        //mPropertiesZFXY_3D->m_zMantissaDigits = ndigits;
        mPlot3DView->SetZScaleConf(ndigits);
    }
}





//////////////////
//	...labels				 TODO this needs refactoring			//TODO display data assignment


void CPlotEditor::HandleXAxisLabelChanged()
{
	assert__( mPlot2DView || mPlot3DView );		assert__( mPropertiesZFXY || mPropertiesXY );

	int index = mTabCurveOptions->mFieldsList->currentRow();

	std::string xtitle, ytitle, ztitle;
	if ( mPlotType == eHistogram )
		mPlot2DView->HistogramAxisTitles( index, xtitle, ytitle, ztitle );
	else
	if ( !mDataArrayZFXY->empty() )
		mPlot2DView->SpectrogramAxisTitles( index, xtitle, ytitle, ztitle );
	else
		mPlot2DView->AxisTitles( &xtitle, &ytitle, &ztitle );

	QString new_label = mTabAxisOptions->mXAxisLabel->text();
	xtitle = q2a( new_label );
	mCurrentDisplayData->SetXAxisText( 0, xtitle );

	if ( mPlot2DView )
	{
		mPlot2DView->SetAxisTitles( index, xtitle, ytitle, ztitle );
	}

	if ( mPlot3DView )
	{
		mPlot3DView->SetAxisTitles( xtitle, ytitle, ztitle );
	}
}
void CPlotEditor::HandleYAxisLabelChanged()
{
	assert__( mPlot2DView || mPlot3DView );		assert__( mPropertiesZFXY || mPropertiesXY );

	int index = mTabCurveOptions->mFieldsList->currentRow();

	std::string xtitle, ytitle, ztitle;
	if ( mPlotType == eHistogram )
		mPlot2DView->HistogramAxisTitles( index, xtitle, ytitle, ztitle );
	else
	if ( !mDataArrayZFXY->empty() )
		mPlot2DView->SpectrogramAxisTitles( index, xtitle, ytitle, ztitle );
	else
		mPlot2DView->AxisTitles( &xtitle, &ytitle, &ztitle );

	QString new_label = mTabAxisOptions->mYAxisLabel->text();
	ytitle = q2a( new_label );
	//mCurrentDisplayData->SetXAxisText( 1, ytitle );

	if ( mPlot2DView )
	{
		mPlot2DView->SetAxisTitles( index, xtitle, ytitle, ztitle );
	}

	if ( mPlot3DView )
	{
		mPlot3DView->SetAxisTitles( xtitle, ytitle, ztitle );
	}
}
void CPlotEditor::HandleZAxisLabelChanged()
{
	assert__( mPlot2DView || mPlot3DView );		assert__( mPropertiesZFXY || mPropertiesXY );

	int index = mTabCurveOptions->mFieldsList->currentRow();

	std::string xtitle, ytitle, ztitle;
	if ( mPlotType == eHistogram )
		mPlot2DView->HistogramAxisTitles( index, xtitle, ytitle, ztitle );
	else
	if ( !mDataArrayZFXY->empty() )
		mPlot2DView->SpectrogramAxisTitles( index, xtitle, ytitle, ztitle );
	else
		mPlot2DView->AxisTitles( &xtitle, &ytitle, &ztitle );

	QString new_label = mTabAxisOptions->mZAxisLabel->text();
	ztitle = q2a( new_label );
	//mCurrentDisplayData->SetXAxisText( 2, ztitle );

	if ( mPlot2DView )
	{
		mPlot2DView->SetAxisTitles( index, xtitle, ytitle, ztitle );
	}

	if ( mPlot3DView )
	{
		mPlot3DView->SetAxisTitles( xtitle, ytitle, ztitle );
	}
}




//////////////////
//	...scales						  //TODO display data assignment


void CPlotEditor::HandleX2DScaleSpinChanged( double x )
{
	assert__( mPlot2DView );

	mPlot2DView->blockSignals( true );		//do not notify ourselves
	mPlot2DView->RescaleX( x );
	mPlot2DView->blockSignals( false );
}


void CPlotEditor::HandleY2DScaleSpinChanged( double y )
{
	assert__( mPlot2DView );

	mPlot2DView->blockSignals( true );		//do not notify ourselves
	mPlot2DView->RescaleY( y );
	mPlot2DView->blockSignals( false );
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
