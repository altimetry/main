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

	AddTab( mTabCurveOptions, "Curve Options" );
	AddTab( mTabAxisOptions, "Axis Options" );

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

	connect( mTabAxisOptions, SIGNAL( LogarithmicScaleX( bool ) ), this, SLOT( HandleLogarithmicScaleX( bool ) ) );
	connect( mTabAxisOptions, SIGNAL( LogarithmicScaleY( bool ) ), this, SLOT( HandleLogarithmicScaleY( bool ) ) );
	connect( mTabAxisOptions, SIGNAL( LogarithmicScaleZ( bool ) ), this, SLOT( HandleLogarithmicScaleZ( bool ) ) );


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



CPlotEditor::CPlotEditor( CModel *model, const COperation *op, const std::string &display_name, QWidget *parent ) 	//display_name = "" parent = nullptr
	: base_t( false, model, op, display_name, parent )
{
    CreateWidgets();

	Start( display_name );
}


CPlotEditor::CPlotEditor( const CDisplayFilesProcessor *proc, CPlot* plot, QWidget *parent ) 	//parent = nullptr
	: base_t( false, proc, parent )
{
    Q_UNUSED(proc);     Q_UNUSED(plot);

    CreateWidgets();
}

CPlotEditor::CPlotEditor( const CDisplayFilesProcessor *proc, CZFXYPlot* plot, QWidget *parent ) 	//parent = nullptr
	: base_t( false, proc, parent )
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
{
	BRAT_NOT_IMPLEMENTED
}
//virtual 
void CPlotEditor::DeleteButtonClicked()
{
	BRAT_NOT_IMPLEMENTED
}
//virtual 
void CPlotEditor::OneClick()
{
	BRAT_NOT_IMPLEMENTED
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
		mPlot3DView->SetLogarithmicScale( log );		//TODO or eliminate
}

void CPlotEditor::HandleLogarithmicScaleY( bool log )
{
	WaitCursor wait;

	if ( mPlot3DView )
		mPlot3DView->SetLogarithmicScale( log );		//TODO or eliminate
}

void CPlotEditor::HandleLogarithmicScaleZ( bool log )
{
	WaitCursor wait;

	if ( mPlot3DView )
		mPlot3DView->SetLogarithmicScaleZ( log );
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
	mTabCurveOptions->mFieldsList->setSelectionMode( mTabCurveOptions->mFieldsList->NoSelection );
	const size_t size = mDataArrayZFXY_2D->size();				assert__( size == mCurrentDisplayFilesProcessor->GetZFXYPlotPropertiesSize()  && size == mDataArrayZFXY_3D->size() );
	for ( size_t i = 0; i < size; ++i )
	{
		mPropertiesZFXY_2D = dynamic_cast<CZFXYPlotData*>( mDataArrayZFXY_2D->at( i ) )->GetPlotProperties();	assert__( mPropertiesZFXY_2D );
		mPropertiesZFXY_3D = dynamic_cast<CZFXYPlotData*>( mDataArrayZFXY_3D->at( i ) )->GetPlotProperties();	assert__( mPropertiesZFXY_3D );

		mTabCurveOptions->mFieldsList->addItem( t2q( mPropertiesZFXY_2D->m_name ) );							assert__( mPropertiesZFXY_2D->m_name == mPropertiesZFXY_3D->m_name );			

		mPlot2DView->ShowContour( i, mPropertiesZFXY_2D->m_withContour );		assert__( mPropertiesZFXY_2D->m_withContour == mPropertiesZFXY_3D->m_withContour );
		mPlot2DView->ShowSolidColor( i, mPropertiesZFXY_2D->m_solidColor );		assert__( mPropertiesZFXY_2D->m_solidColor == mPropertiesZFXY_3D->m_solidColor );

		mPlot3DView->ShowContour( mPropertiesZFXY_3D->m_withContour );		//in fact, in qwtplot3d this affects the axis (all fields) not only the current iteration one
		mPlot3DView->ShowSolidColor( mPropertiesZFXY_3D->m_solidColor );	//idem
	}
	
	//NOTE 1. assign property values from plots to widgets here, because no single field selection is allowed, so the respective handler won't be called for 3D
	//NOTE 2. assuming mPlot2DView was assigned in the loop the same values as mPlot3DView

	mTabCurveOptions->mShowContour->setChecked( mPlot2DView->HasContour( 0 ) );
	mTabCurveOptions->mShowSolidColor->setChecked( mPlot2DView->HasSolidColor( 0 ) );

	mTabCurveOptions->mLineOptions->setEnabled( false );
	mTabCurveOptions->mPointOptions->setEnabled( false );
	mTabCurveOptions->mSpectrogramOptions->setEnabled( true );

	mTabCurveOptions->mFieldsList->setCurrentRow( -1 );		//no selection allowed
}


void CPlotEditor::Reset2DProperties( const CXYPlotProperties *props, CPlot *plot )
{
	assert__( mPlot2DView && mCurrentDisplayFilesProcessor );

	QwtPlotMagnifier *mag = mPlot2DView->AddMagnifier();			Q_UNUSED( mag );
	QwtPlotPanner *panner = mPlot2DView->AddPanner();				Q_UNUSED( panner );

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

	mPlot2DView->EnableAxisY2( mPlotType == eXYY );

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
			mPlot2DView->SetCurveLineColor( i, mPropertiesXY->GetColor() );
			mPlot2DView->SetCurveLineOpacity( i, mPropertiesXY->GetOpacity() );
			mPlot2DView->SetCurveLinePattern( i, mPropertiesXY->GetStipplePattern() );
			mPlot2DView->SetCurveLineWidth( i, mPropertiesXY->GetLineWidth() );

			mPlot2DView->SetCurvePointColor( i, mPropertiesXY->GetPointColor() );
			mPlot2DView->SetCurvePointFilled( i, mPropertiesXY->GetFilledPoint() );
			mPlot2DView->SetCurvePointGlyph( i, mPropertiesXY->GetPointGlyph() );
			mPlot2DView->SetCurvePointSize( i, mPropertiesXY->GetPointSize() );
		}
	}

	mTabCurveOptions->mFieldsList->setCurrentRow( 0 );
}



///////////////////////////
//	From plot to widgets
///////////////////////////

void CPlotEditor::HandleCurrentFieldChanged( int index )
{
	assert__( mPlot2DView /*&& !mDataArrayZFXY_2D && !mDataArrayZFXY_3D */);

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

		mTabCurveOptions->mShowContour->setChecked( mPlot2DView->HasContour( index ) );
		mTabCurveOptions->mShowSolidColor->setChecked( mPlot2DView->HasSolidColor( index ) );

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
}




///////////////////////////////////////////////////////
//	From widgets to properties, from properties to plot
///////////////////////////////////////////////////////


//spectrogram  properties

void CPlotEditor::HandleShowContourChecked( bool checked )
{
	assert__( mPlot2DView && mPlot3DView && mDataArrayZFXY_2D && mPropertiesZFXY_3D && mDataArrayZFXY_2D && mPropertiesZFXY_3D );

	//mPropertiesZFXY_2D->m_withContour = checked;	this is done in loop below, for all mPropertiesZFXY_2D (if more than one will be supported)
	//mPropertiesZFXY_3D->m_withContour = checked;	idem

	const size_t size = mDataArrayZFXY_2D->size();		assert__( size == mCurrentDisplayFilesProcessor->GetZFXYPlotPropertiesSize() && size == mDataArrayZFXY_3D->size() );
	for ( size_t i = 0; i < size; ++i )
	{
		auto *current_properties_2D = dynamic_cast<CZFXYPlotData*>( mDataArrayZFXY_2D->at( i ) )->GetPlotProperties();			assert__( mPropertiesZFXY_2D );
		auto *current_properties_3D = dynamic_cast<CZFXYPlotData*>( mDataArrayZFXY_3D->at( i ) )->GetPlotProperties();			assert__( mPropertiesZFXY_3D );

		current_properties_2D->m_withContour = checked;
		current_properties_3D->m_withContour = checked;

		mPlot2DView->ShowContour( i, current_properties_2D->m_withContour );
		mPlot3DView->ShowContour( current_properties_3D->m_withContour );	//in fact, in qwtplot3d this affects the axis (all fields) not only the current iteration one
	}
}

void CPlotEditor::HandleShowSolidColorChecked( bool checked )
{
	assert__( mPlot2DView && mPlot3DView && mDataArrayZFXY_2D && mPropertiesZFXY_3D && mDataArrayZFXY_2D && mPropertiesZFXY_3D );

	//mPropertiesZFXY_2D->m_solidColor = checked;	this is done in loop below, for all mPropertiesZFXY_2D (if more than one will be supported)
	//mPropertiesZFXY_3D->m_solidColor = checked;	idem

	const size_t size = mDataArrayZFXY_2D->size();		assert__( size == mCurrentDisplayFilesProcessor->GetZFXYPlotPropertiesSize() && size == mDataArrayZFXY_3D->size() );
	for ( size_t i = 0; i < size; ++i )
	{
		auto *current_properties_2D = dynamic_cast<CZFXYPlotData*>( mDataArrayZFXY_2D->at( i ) )->GetPlotProperties();			assert__( mPropertiesZFXY_2D );
		auto *current_properties_3D = dynamic_cast<CZFXYPlotData*>( mDataArrayZFXY_3D->at( i ) )->GetPlotProperties();			assert__( mPropertiesZFXY_3D );

		current_properties_2D->m_solidColor = checked;
		current_properties_3D->m_solidColor = checked;

		mPlot2DView->ShowSolidColor( i, current_properties_2D->m_solidColor );	//in fact, in qwtplot3d this affects the axis (all fields) not only the current iteration one
		mPlot3DView->ShowSolidColor( current_properties_3D->m_solidColor );		//idem
	}
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
