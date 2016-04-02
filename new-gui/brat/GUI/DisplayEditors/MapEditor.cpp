#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtilsIO.h"

#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/Workspaces/Display.h"
#include "DataModels/PlotData/MapProjection.h"
#include "DataModels/PlotData/WorldPlot.h"

#include "GUI/ActionsTable.h"
#include "GUI/ControlPanels/Views/ViewControlPanels.h"
#include "GUI/DisplayWidgets/GlobeWidget.h"				//include map before globe to avoid macro definition collisions
#include "GUI/DisplayWidgets/BratViews.h"

#include "MapEditor.h"


void CMapEditor::CreateMapActions()
{
	AddToolBarSeparator();
	mActionStatisticsMean = AddToolBarAction( this, eAction_MapEditorMean );
	mActionStatisticsStDev = AddToolBarAction( this, eAction_MapEditorStDev );
	mActionStatisticsLinearRegression = AddToolBarAction( this, eAction_MapEditorLinearRegression );

	// add menu button for projections

	AddToolBarSeparator();
	mProjectionsGroup = ProjectionsActions( this );
	mToolProjection = AddMenuButton( eActionGroup_Projections, mProjectionsGroup->actions() );
}


void CMapEditor::CreateAndWireNewMap()
{
	if ( mMapView )
	{
		mMapView->setRenderFlag( false );
		QWidget *p = mMapView;
		mMapView = nullptr;
		RemoveView( p, false, false );
	}
	mMapView = new CBratMapView( this );
	mMapView->ConnectParentRenderWidgets( mProgressBar, mRenderSuppressionCBox );
	mMapView->ConnectParentMeasureActions( mMeasureButton, mActionMeasure, mActionMeasureArea );
	mMapView->ConnectParentGridAction( mActionDecorationGrid );
	AddView( mMapView, false );
	mMapView->setVisible( false );
}

//create and insert widgets

void CMapEditor::CreateWidgets()
{
	base_t::CreateWidgets<CViewControlsPanelGeneralMaps>();

    //    Specialized Tabs
	mTabDataLayers = new CMapControlsPanelDataLayers( this );
	mTabView = new CMapControlsPanelView( this );

	AddTab( mTabDataLayers, "Data Layers" );
	AddTab( mTabView, "View" );

	// map widget managed

	const int index = 0;
	CMapWidget::CreateRenderWidgets( statusBar(), mProgressBar, mRenderSuppressionCBox );
	statusBar()->insertPermanentWidget( index, mRenderSuppressionCBox, 0 );
	statusBar()->insertPermanentWidget( index, mProgressBar, 1 );

	mMeasureButton = CMapWidget::CreateMapMeasureActions( mGraphicsToolBar, mActionMeasure, mActionMeasureArea );
	mGraphicsToolBar->addAction( CActionInfo::CreateAction( mGraphicsToolBar, eAction_Separator ) );
	mGraphicsToolBar->addWidget( mMeasureButton );

	mActionDecorationGrid = CMapWidget::CreateGridAction( mGraphicsToolBar );
	mGraphicsToolBar->addAction( mActionDecorationGrid );

    //    Map
	ResetViews( true, false );		//creates map and connects map actions; hides it
	mMapView->setVisible( true );

	// Tool-bar actions

	CreateMapActions();

	Wire();
}

void CMapEditor::Wire()
{
	for ( auto a : mProjectionsGroup->actions() )
	{
		if ( a->isSeparator() )
			continue;

		assert__( a->isCheckable() );
		connect( a, SIGNAL( triggered() ), this, SLOT( HandleProjection() ) );
	}

	connect( mActionStatisticsMean, SIGNAL( triggered() ), this, SLOT( HandleStatisticsMean() ) );
	connect( mActionStatisticsStDev, SIGNAL( triggered() ), this, SLOT( HandleStatisticsStDev() ) );
	connect( mActionStatisticsLinearRegression, SIGNAL( triggered() ), this, SLOT( HandleStatisticsLinearRegression() ) );

	// layers

	//...select layers

	connect( mTabDataLayers->mFieldsList, SIGNAL( currentRowChanged( int ) ), this, SLOT( HandleCurrentFieldChanged( int ) ) );


	//...show/hide layers
	connect( mTabDataLayers->mShowSolidColorCheck, SIGNAL( toggled( bool ) ), this, SLOT( HandleShowSolidColor( bool ) ) );
}

CMapEditor::CMapEditor( CModel *model, const COperation *op, const std::string &display_name, QWidget *parent )		//display_name = "" parent = nullptr
	: base_t( true, model, op, display_name, parent )
{
	CreateWidgets();

	Start( display_name );

#if defined (TESTING_GLOBE)

	Show3D( true );

#endif
}


CMapEditor::CMapEditor( CDisplayFilesProcessor *proc, CWPlot* wplot, QWidget *parent )		//parent = nullptr
	: base_t( true, proc, parent )
{
	CreateWidgets();

	mMapView->CreatePlot( proc->GetWorldPlotProperties( 0 ), wplot );
}


//virtual 
CMapEditor::~CMapEditor()
{
	delete mGlobeView;
	delete mMapView;
}


//virtual 
void CMapEditor::closeEvent( QCloseEvent *event )
{
	KillGlobe();
	if ( 
		( !mGlobeView || mGlobeView->close() ) &&
		( !mMapView || mMapView->close() ) 
		)
		event->accept();
	else
		event->ignore();
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//						General Processing
//
//	- editor setup for operation/display/map type changes
//	- all other editable properties rely on a correct/consistent setup here
//
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


CViewControlsPanelGeneralMaps* CMapEditor::TabGeneral() 
{ 
	return dynamic_cast<CViewControlsPanelGeneralMaps*>( mTabGeneral ); 
}


//virtual 
void CMapEditor::Show2D( bool checked )
{
	mMapView->setVisible( checked );
}
//virtual 
void CMapEditor::Show3D( bool checked )
{
	mMapView->freeze( true );
	if ( checked && !mGlobeView )
	{
		WaitCursor wait;

		mGlobeView = new CGlobeWidget( this, mMapView, statusBar() );
		AddView( mGlobeView, true );
	}
	mGlobeView->setVisible( checked );
	mMapView->freeze( false );
}


//virtual 
bool CMapEditor::ResetViews( bool reset_2d, bool reset_3d )
{
	KillGlobe();
	if ( reset_2d )
	{
		CreateAndWireNewMap();
	}
	return true;
}



#include <QProgressDialog>
void f()
{
	const int numFiles = 10000;
	QWidget *parent = nullptr;
	QProgressDialog progress( "Copying files...", "Abort Copy", 0, numFiles, parent );
	progress.setWindowModality( Qt::WindowModal );
	progress.show();
	int i = 0;
	for ( ; i < numFiles; i++ )
	{
		progress.setValue( i );

		if ( progress.wasCanceled() )
			break;
		qDebug() << "... copy one file";
	}
	progress.setValue( numFiles );
	qDebug() << "copied files==" << i;
}


//virtual 
void CMapEditor::NewButtonClicked()
{
	BRAT_NOT_IMPLEMENTED
}
//virtual 
void CMapEditor::RenameButtonClicked()
{
	BRAT_NOT_IMPLEMENTED
}
//virtual 
void CMapEditor::DeleteButtonClicked()
{
	BRAT_NOT_IMPLEMENTED
}
//virtual 
void CMapEditor::OneClick()
{
	BRAT_NOT_IMPLEMENTED
}


void CMapEditor::HandleStatisticsMean()
{
	BRAT_NOT_IMPLEMENTED
}


void CMapEditor::HandleStatisticsStDev()
{
	BRAT_NOT_IMPLEMENTED
}


void CMapEditor::HandleStatisticsLinearRegression()
{
	BRAT_NOT_IMPLEMENTED
}





//virtual 
void CMapEditor::OperationChanged( int index )
{
    Q_UNUSED(index);

	//if ( !mOperation )
	//	return;

	//const CMapFormula* formulas = mOperation->GetFormulas();
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
	//			break;

	//		case CMapTypeField::eTypeOpAsField:
	//			break;
	//	}
	//}
}



bool CMapEditor::ViewChanged()
{
	WaitCursor wait;

	try
	{
		std::vector< CWPlot* > wplots;
		if ( mDisplay->IsZLatLonType() )
			wplots = GetDisplayPlots< CWPlot >( mDisplay );			assert__( wplots.size() && mCurrentDisplayFilesProcessor );


		if ( !ResetViews( wplots.size() > 0, false ) )
			throw CException( "A previous map is still processing. Please try again later." );
		SelectTab( mTabGeneral->parentWidget() );


		int map_index = 0;						assert__( wplots.size() <= 1 );		//forces redesign if false
		for ( auto *wplot : wplots )
		{
			assert__( wplot != nullptr );

			ResetProperties( mCurrentDisplayFilesProcessor->GetWorldPlotProperties( map_index++ ), wplot );	//v3 always used hard coded map_index == 0
		}

		mMapView->setVisible( true );

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
		SimpleErrorBox( "Unexpected error trying to create a map plot." );
	}

	return false;
}







//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//						Properties Processing
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



///////////////////////////
//	From properties to plot
///////////////////////////


void CMapEditor::ResetProperties( const CWorldPlotProperties *props, CWPlot *wplot )
{
	assert__( mMapView && mCurrentDisplayFilesProcessor );

	mMapView->CreatePlot( props, wplot );

	mDataArrayMap = mMapView->MapDataCollection();									assert__( mDataArrayMap );

	mTabDataLayers->mFieldsList->clear();
	const size_t size = mDataArrayMap->size();										assert__( size == mCurrentDisplayFilesProcessor->GetWorldPlotPropertiesSize() );
	for ( size_t i = 0; i < size; ++i )
	{
		mPropertiesMap = &mDataArrayMap->at( i )->m_plotProperty;					assert__( mPropertiesMap );
		mTabDataLayers->mFieldsList->addItem( t2q( mPropertiesMap->m_name ) );

		//mMapView->SetCurveLineColor( i, mPropertiesMap->GetColor() );
		//mMapView->SetCurveLineOpacity( i, mPropertiesMap->GetOpacity() );
		//mMapView->SetCurveLinePattern( i, mPropertiesMap->GetStipplePattern() );
		//mMapView->SetCurveLineWidth( i, mPropertiesMap->GetLineWidth() );

		//mMapView->SetCurvePointColor( i, mPropertiesMap->GetPointColor() );
		//mMapView->SetCurvePointFilled( i, mPropertiesMap->GetFilledPoint() );
		//mMapView->SetCurvePointGlyph( i, mPropertiesMap->GetPointGlyph() );
		//mMapView->SetCurvePointSize( i, mPropertiesMap->GetPointSize() );
	}

	mTabDataLayers->mFieldsList->setCurrentRow( 0 );
}


///////////////////////////
//	From plot to widgets
///////////////////////////


void CMapEditor::HandleCurrentFieldChanged( int field_index )
{
	assert__( mMapView );

	mTabDataLayers->mShowSolidColorCheck->setEnabled( field_index >= 0 && mDataArrayMap );

	mPropertiesMap = nullptr;

	if ( field_index < 0 )
		return;

	assert__( field_index < mDataArrayMap->size() );

	mPropertiesMap = &mDataArrayMap->at( field_index )->m_plotProperty;	  			assert__( mPropertiesMap );

	mTabDataLayers->mShowSolidColorCheck->setChecked( mMapView->IsLayerVisible( field_index ) );
	//mTabDataLayers->mLineColorButton->SetColor( mPlot2DView->CurveLineColor( index ) );
	//mTabDataLayers->mLineOpacityValue->setText( n2s<std::string>( mPlot2DView->CurveLineOpacity( index ) ).c_str() );
	//mTabDataLayers->mStipplePattern->setCurrentIndex( mPlot2DView->CurveLinePattern( index ) );
	//mTabDataLayers->mLineWidthValue->setText( n2s<std::string>( mPlot2DView->CurveLineWidth( index ) ).c_str() );


	//mTabDataLayers->mPointColorButton->SetColor( mPlot2DView->CurvePointColor( index ) );
	//mTabDataLayers->mFillPointCheck->setChecked( mPlot2DView->IsCurvePointFilled( index ) );
	//mTabDataLayers->mPointGlyph->setCurrentIndex( mPlot2DView->CurvePointGlyph( index ) );
	//mTabDataLayers->mPointSizeValue->setText( n2s<std::string>( mPlot2DView->CurvePointSize( index ) ).c_str() );

	//mTabDataLayers->mLineOptions->setChecked( mPlotType != eHistogram && mPropertiesMap->GetLines() );
	//mTabDataLayers->mPointOptions->setChecked( mPropertiesMap->GetPoints() );
}



///////////////////////////////////////////////////////
//	From widgets to properties, from properties to plot
///////////////////////////////////////////////////////


void CMapEditor::KillGlobe()
{
	if ( mGlobeView )
	{
		mGlobeView->ScheduleClose();
		mGlobeView->setParent( nullptr );
		mGlobeView = nullptr;
		m3DAction->blockSignals( true );
		m3DAction->setChecked( false );
		m3DAction->setDisabled( false );
		m3DAction->blockSignals( false );
		qApp->processEvents();
	}
}


void CMapEditor::HandleShowSolidColor( bool checked )
{
	assert__( mMapView && mDataArrayMap && mPropertiesMap );

	WaitCursor wait;

	if ( mGlobeView )
	{
		//KillGlobe();
		//mTabDataLayers->mShowSolidColorCheck->blockSignals( true );
		//mTabDataLayers->mShowSolidColorCheck->setChecked( !checked );
		//mTabDataLayers->mShowSolidColorCheck->blockSignals( false );
		//return;
	}
    mPropertiesMap->m_solidColor = checked;

	mMapView->SetLayerVisible( mTabDataLayers->mFieldsList->currentRow(), mPropertiesMap->m_solidColor );
}








///////////////////
//	Map Projections
///////////////////

static const unsigned default_projection_id = (unsigned)-1;


QActionGroup* ProjectionsActions( QWidget *parent )
{
	static const std::vector< std::pair< EActionTag, unsigned > > ProjIdsMap =
	{
		{ eAction_Projection_LAMBERT_CYLINDRICAL,	PROJ2D_LAMBERT_CYLINDRICAL   },
		{ eAction_Projection_PLATE_CAREE,			PROJ2D_PLATE_CAREE			 },
		{ eAction_Projection_MOLLWEIDE,				PROJ2D_MOLLWEIDE			 },
		{ eAction_Projection_ROBINSON,				PROJ2D_ROBINSON				 },
		{ eAction_Projection_LAMBERT_AZIMUTHAL,		PROJ2D_LAMBERT_AZIMUTHAL	 },
		{ eAction_Projection_AZIMUTHAL_EQUIDISTANT, PROJ2D_AZIMUTHAL_EQUIDISTANT },
		{ eAction_Projection_MERCATOR,				PROJ2D_MERCATOR				 },
		{ eAction_Projection_ORTHO,					PROJ2D_ORTHO				 },
		{ eAction_Projection_NEAR_SIGHTED,			PROJ2D_NEAR_SIGHTED			 },
		{ eAction_Projection_STEREOGRAPHIC,			PROJ2D_STEREOGRAPHIC		 },
		{ eAction_Separator,						default_projection_id		 },		//any id, it doesn't matter
		{ eAction_Projection_Default,				default_projection_id		 },
	};

	static size_t size = 0;

	static const std::vector< EActionTag > ActionTags =
	{
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
		ProjIdsMap[size++].first,
	};


	QActionGroup *g = CActionInfo::CreateActionGroup( parent, ActionTags, true );
	auto actions = g->actions();

	// CMapProjection::3D is not in ActionTags, default_projection_id and separator are not in CMapProjection
	//
	assert__( size == ActionTags.size() && size == CMapProjection::GetInstance()->size() + 1 && size == ProjIdsMap.size() && size == actions.size() );

	for ( size_t i = 0; i < size; ++i )
	{
		auto *a = actions[ (int) i ];
		if ( a->isSeparator() )
			continue;

		auto id = ProjIdsMap[ i ].second;
		if ( (unsigned)id == (unsigned)default_projection_id )
			a->setChecked( true );
		else
		{
			a->setData( QVariant::fromValue( id ) );
			a->setText( a->text() + " - C.R.S. " + CMapProjection::GetInstance()->IdToCRS( id ).description() );
		}
	}

	return g;
}

void CMapEditor::HandleProjection()
{
	assert__( mMapView );

	auto a = qobject_cast<QAction*>( sender() );				assert__( a && mProjectionsGroup->actions().indexOf( a ) >= 0 );

	unsigned proj_id = a->data().toUInt();

	if ( proj_id == PROJ2D_3D )
		Show3D( true );
	else
	if ( proj_id == default_projection_id )
		mMapView->SetDefaultProjection();
	else
		mMapView->SetProjection( proj_id );
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_MapEditor.cpp"
