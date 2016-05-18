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

#include "new-gui/Common/QtUtilsIO.h"

#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/Workspaces/Display.h"
#include "DataModels/PlotData/MapProjection.h"
#include "DataModels/PlotData/WorldPlot.h"
#include "DataModels/PlotData/WorldPlotData.h"

#include "GUI/ActionsTable.h"
#include "GUI/ControlPanels/Views/ViewControlPanels.h"

#include "GUI/DisplayWidgets/GlobeWidget.h"				//these 3 includes must be done in this  order to avoid macro definition collisions
#include "GUI/DisplayWidgets/MapWidget.h"
#include "DataModels/PlotData/BratLookupTable.h"

#include "ApplicationLogger.h"

#include "MapEditor.h"



void CMapEditor::CreateMapActions()
{
	AddToolBarSeparator();
	mActionStatisticsMean = AddToolBarAction( this, eAction_MapEditorMean );
	mActionStatisticsStDev = AddToolBarAction( this, eAction_MapEditorStDev );
	mActionStatisticsLinearRegression = AddToolBarAction( this, eAction_MapEditorLinearRegression );

	// add menu button for projections

	AddToolBarSeparator();
	mProjectionsGroup = CreateProjectionsActions();
	mToolProjection = AddMenuButton( eActionGroup_Projections, mProjectionsGroup->actions() );
}


void CMapEditor::ResetAndWireNewMap()
{
	if ( mMapView )
	{
		mMapView->setRenderFlag( false );
		mCoordinatesFormat = mMapView->CoordinatesFormat();
		QWidget *p = mMapView;
		mMapView = nullptr;
		RemoveView( p, false, false );
	}
	mMapView = new CMapWidget( this );
	mMapView->ConnectParentRenderWidgets( mProgressBar, mRenderSuppressionCBox );
	mMapView->ConnectParentMeasureActions( mMeasureButton, mActionMeasure, mActionMeasureArea );
	mMapView->ConnectParentGridAction( mActionDecorationGrid );
	mMapView->ConnectCoordinatesWidget( mCoordsEdit, mCoordinatesFormatButton, mCoordinatesFormat );
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

	mTabView->setEnabled( false );			//TODO delete after implementation

	// map widget managed

	const int index = 0;
	CMapWidget::CreateRenderWidgets( statusBar(), mProgressBar, mRenderSuppressionCBox );

#if !defined(_DEBUG) && !defined(DEBUG)
	mRenderSuppressionCBox->setVisible( false );
	mRenderSuppressionCBox->setEnabled( false );
#endif

	statusBar()->insertPermanentWidget( index, mRenderSuppressionCBox, 0 );
	statusBar()->insertPermanentWidget( index, mProgressBar, 1 );

	//coords status bar widget
	CMapWidget::CreateCoordinatesWidget( statusBar(), mCoordsEdit, mCoordinatesFormatButton );
	statusBar()->addPermanentWidget( mCoordinatesFormatButton, 0 );
	statusBar()->addPermanentWidget( mCoordsEdit, 0 );

	//graphics bar
	mMeasureButton = CMapWidget::CreateMapMeasureActions( mGraphicsToolBar, mActionMeasure, mActionMeasureArea );
	mGraphicsToolBar->addAction( CActionInfo::CreateAction( mGraphicsToolBar, eAction_Separator ) );
	mGraphicsToolBar->addWidget( mMeasureButton );

	mActionDecorationGrid = CMapWidget::CreateGridAction( mGraphicsToolBar );
	mGraphicsToolBar->addAction( mActionDecorationGrid );

    //    Map
    ResetViews( true, true, true, false );		//creates map and connects map actions; hides it
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

	connect( mTabDataLayers->mMinRange, SIGNAL( returnPressed() ), this, SLOT( HandleFieldMinRangeEntered() ) );
	connect( mTabDataLayers->mMaxRange, SIGNAL( returnPressed() ), this, SLOT( HandleFieldMaxRangeEntered() ) );

	//... layer options
	connect( mTabDataLayers->mColorMapWidget, SIGNAL( ShowContourToggled( bool ) ), this, SLOT( HandleShowContourChecked( bool ) ) );
	connect( mTabDataLayers->mColorMapWidget, SIGNAL( ShowSolidColorToggled( bool ) ), this, SLOT( HandleShowSolidColorChecked( bool ) ) );
	connect( mTabDataLayers->mColorMapWidget, SIGNAL( CurrentIndexChanged( int ) ), this, SLOT( HandleColorTablesIndexChanged( int ) ) );

	mMainSplitter->setHandleWidth( 0 );
}

CMapEditor::CMapEditor( CModel *model, const COperation *op, const std::string &display_name )		//display_name = ""
	: base_t( true, model, op, display_name )
{
	CreateWidgets();

	Start( display_name );

#if defined (TESTING_GLOBE)

	Show3D( true );

#endif
}


CMapEditor::CMapEditor( CDisplayFilesProcessor *proc, CWPlot* wplot )
	: base_t( true, proc )
{
	CreateWidgets();

	Start( "" );

	std::vector< CWPlot* > wplots;
	wplots.push_back( wplot );

	CreatePlotData( wplots );
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
	if ( mMapProcessing )
	{
		LOG_FATAL( "2D reentrant request" );
		return;
	}
	mMapProcessing = true;

	LOG_FATAL( QString( "2D" ) + ( checked ? " ==> CHECKED" : "==> unchecked" ) );

	if ( checked )
	{
		if ( mGlobeView )
		{
			mGlobeView->Pause();		assert__( !mGlobeView->Rendering() );
		}

		mMapView->freeze( false );
		mMapView->refresh();
	}

	mMapView->setVisible( checked );

	mMeasureButton->setEnabled( checked );
	//mActionDecorationGrid->setEnabled( checked );
	if( mToolProjection )
		mToolProjection->setEnabled( checked );

	mMapProcessing = false;
	mDisplaying2D = true;
	mDisplaying3D = false;
}
//virtual 
void CMapEditor::Show3D( bool checked )
{
	if ( mGlobeProcessing )
	{
		LOG_FATAL( "3D reentrant request" );
		return;
	}
	mGlobeProcessing = true;

	LOG_FATAL( QString( "3D" ) + ( checked ? " ==> CHECKED" : "==> unchecked" ) );

	if ( checked )
	{
		while ( mMapView->isDrawing() )
			qApp->processEvents();
		mMapView->freeze( true );

		if ( mGlobeView )
		{
			mGlobeView->Resume( true );
		}
		else
		{
			WaitCursor wait;

            mGlobeView = new CGlobeWidget( this, mMapView );
			AddView( mGlobeView, true );
		}
	}

	mGlobeView->setVisible( checked );

	mGlobeProcessing = false;
	mDisplaying2D = false;
	mDisplaying3D = true;
}


//virtual 
void CMapEditor::Recenter()
{
	if ( mDisplaying2D )
		mMapView->Home();

	if ( mGlobeView && mDisplaying3D )
		mGlobeView->Home();
}


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
	}
}


//virtual 
bool CMapEditor::ResetViews( bool reset_2d, bool reset_3d, bool enable_2d, bool enable_3d )
{
	KillGlobe();		Q_UNUSED( reset_3d );		Q_UNUSED( enable_2d );		Q_UNUSED( enable_3d );
	if ( reset_2d )
	{
		ResetAndWireNewMap();
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
{}
//virtual 
void CMapEditor::DeleteButtonClicked()
{}
//virtual 
void CMapEditor::OneClick()
{
	BRAT_NOT_IMPLEMENTED
}
//virtual 
bool CMapEditor::Test()
{
	BRAT_NOT_IMPLEMENTED;
	return false;
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
		//	I . Assign current plot type
		//without meaning in maps

		//	II . Reset "current" pointers

		mDataArrayMap.clear();
		mPropertiesMap = nullptr;


		//	III . Get plots (v3 plot definitions) from display file

		std::vector< CWPlot* > wplots;			assert__( mDisplay->IsZLatLonType() );

		wplots = GetPlotsFromDisplayFile< CWPlot >( mDisplay );	assert__( wplots.size() && mCurrentDisplayFilesProcessor );

		//	IV . Reset views and select general tab 

		if ( !ResetViews( wplots.size() > 0, true, wplots.size() > 0, false ) )
			throw CException( "A previous map is still processing. Please try again later." );
		SelectTab( mTabGeneral->parentWidget() );

		return CreatePlotData( wplots );
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


bool CMapEditor::CreatePlotData( const std::vector< CWPlot* > &wplots )
{
	WaitCursor wait;

	try
	{
		//	V . Process the plot definitions: create plot data and plots

		int map_index = 0;						assert__( wplots.size() <= 1 );		//forces redesign if false
		for ( auto *wplot : wplots )
		{
			assert__( wplot != nullptr );

			//ResetProperties( mCurrentDisplayFilesProcessor->GetWorldPlotProperties( map_index++ ), wplot );	//v3 always used hard coded map_index == 0

			const CWorldPlotProperties *props = mCurrentDisplayFilesProcessor->GetWorldPlotProperties( map_index++ );	Q_UNUSED( props );  //mPlotPropertiesMap = *props;	// *proc->GetWorldPlotProperties( 0 );
			mMapView->setWindowTitle( t2q( wplot->MakeTitle() ) );

			// for Geo-strophic velocity
			//
			CPlotField * northField =nullptr;
			CPlotField * eastField =nullptr;

			//size_t index = 0;							//index is NOT the index in maps
			for ( auto &itField : wplot->m_fields )
			{
				CPlotField* field = CPlotField::GetPlotField( itField );

#if defined(_DEBUG) || defined(DEBUG) 
				if ( field->m_worldProps != props )
					LOG_WARN( "field->m_worldProps != props" );
#endif

				if ( field->m_internalFiles.empty() )
					continue;

				if ( field->m_worldProps->m_northComponent && northField == nullptr )
				{
					northField = field;
					continue;
				}
				else
				if ( field->m_worldProps->m_eastComponent && eastField == nullptr )
				{
					eastField = field;
					continue;
				}

				// otherwise just add it as regular data
				mDataArrayMap.push_back( new CWorldPlotData( field ) );		//v4 note: CWorldPlotData ctor is only invoked here
			}

			// we have a Vector Plot!
			if ( northField != nullptr && eastField != nullptr )
			{
				mDataArrayMap.push_back( new CWorldPlotVelocityData( northField, eastField ) );	//v4 note: CWorldPlotVelocityData ctor is only invoked here
			}
			else if ( northField != eastField )
			{
				CException e( "CMapEditor::ViewChanged() - incomplete std::vector plot components", BRATHL_INCONSISTENCY_ERROR );
				LOG_TRACE( e.what() );
				throw e;
			}

			ResetMap();
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

#define USE_POINTS		//(**)
#define USE_FEATURES	//(***)
//
//(*)	Using memory layer because: "OGR error creating feature 0: CreateFeature : unsupported operation on a read-only datasource."
//(**)	Using point and not line features because: 
//			1) (If using main layer and "this" shapefile) "Feature creation error (OGR error: Attempt to write non-linestring (POINT) geometry to ARC type shapefile.)"
//			2) There is no algorithm to "bezier" the points collection to an spline
//			3) cannot color the values over a line (unless with another layer, a point layer, but then, why the line layer?)
//(***) Using features and not rubberbands because these are not projected in the globe
//

void CMapEditor::ResetMap()
{
	assert__( mMapView && mCurrentDisplayFilesProcessor );

	auto IsValidPoint = []( const CWorldPlotParameters &map, int32_t i )
	{
		bool bOk = map.mBits[ i ];

		//	  if (Projection == VTK_PROJ2D_MERCATOR)
		//	  {
		//bOk &= maps(0).mValidMercatorLatitudes[ i ];
		//	  }
		//
		return bOk;
	};


	mTabDataLayers->mFieldsList->clear();
	const size_t size = mDataArrayMap.size();								assert__( size == mCurrentDisplayFilesProcessor->GetWorldPlotPropertiesSize() );
	for ( size_t i = 0; i < size; ++i )
	{
		CWorldPlotData* geo_map = mDataArrayMap[ i ];
		mPropertiesMap = &geo_map->m_plotProperty;							assert__( mPropertiesMap );
		const CWorldPlotInfo &maps = geo_map->PlotInfo();					assert__( maps.size() == 1 );	//simply to check if ever...	

		// TODO brat v3 does not update range
		geo_map->m_plotProperty.mLUT->GetLookupTable()->SetTableRange( geo_map->GetLookupTable()->GetTableRange() );

		auto const size = maps( 0 ).mValues.size();
		const CWorldPlotParameters &map = maps( 0 );
		QgsFeatureList flist;

#if defined (USE_POINTS)	//(**)

		for ( auto i = 0u; i < size; ++ i )
		{
			if ( !IsValidPoint( map, i ) )
				continue;

			auto x = i % map.mXaxis.size(); // ( x * geo_map->lats.size() ) + i;
			auto y = i / map.mXaxis.size(); // ( x * geo_map->lats.size() ) + i;

#if defined (USE_FEATURES) //(***)
			//CreatePointFeature( flist, map.mXaxis.at( x ), map.mYaxis.at( y ), map.mValues[ i ] );  // TODO - RCCC Uncomment this to use Points

			////////// TODO RCCC  ////////////////
			mMapView->CreatePolygonFeature( flist, map.mXaxis.at( x ), map.mYaxis.at( y ), map.mValues[ i ] );
			//////////////////////////////////////
#else
			addRBPoint( geo_map->lons.at( x ), geo_map->lats.at( y ), QColor( (long)(geo_map->vals[ i ]) ), mMainLayer );
#endif  //USE_FEATURES
		}

#if defined (USE_FEATURES)

		//AddDataLayer( props->m_name, 0.333, map.mMinHeightValue, map.mMaxHeightValue, props->m_numContour, flist );  // TODO - RCCC Uncomment this to use Points

		////////// TODO RCCC  ////////////////
		mMapView->AddPolygonDataLayer( 
			mPropertiesMap->m_name, map.mMinHeightValue, map.mMaxHeightValue, mPropertiesMap->mLUT->GetLookupTable(), mPropertiesMap->m_numContour, flist );
		//////////////////////////////////////

#endif 


#else		//(**)

		QgsPolyline points;
		for ( auto i = 0; i < size; ++ i ) 
		{
			if ( !IsValidPoint(i) )
				continue;

			auto x = i % geo_map->lons.size();
			auto y = i / geo_map->lons.size();

			points.append( QgsPoint( geo_map->lons.at( x ), geo_map->lats.at( y ) ) );
		}
#if !defined (USE_FEATURES) //(***)
		auto memL = addMemoryLayer( createLineSymbol( 0.5, Qt::red ) );	//(*)	//note that you can use strings like "red" instead!!!
		createLineFeature( flist, points );						
		memL->dataProvider()->addFeatures( flist );				
		//memL->updateExtents();
		//refresh();
#else
		addRBLine( points, QColor( 0, 255, 0 ), mMainLayer );	
#endif

		return;

#endif

		//femm: This is CWorldPlotPanel::AddData

		//femm: the important part
		//if ( pdata->GetColorBarRenderer() != nullptr )
		//	m_vtkWidget->GetRenderWindow()->AddRenderer( pdata->GetColorBarRenderer()->GetVtkRenderer() );
		//m_plotRenderer->AddData( pdata );


		//CWorldPlotData* geo_map = dynamic_cast<CWorldPlotData*>( pdata );
		//if ( geo_map != nullptr )
		//{
		//	wxString textLayer = wxString::Format( "%s", geo_map->GetDataName().c_str() );

		//	m_plotPropertyTab->GetLayerChoice()->Append( textLayer, static_cast<void*>( geo_map ) );
		//	m_plotPropertyTab->SetCurrentLayer( 0 );
		//}

		//int32_t nFrames = 1;
		//if ( geo_map != nullptr )
		//	nFrames = geo_map->GetNrMaps();

		//m_animationToolbar->SetMaxFrame( nFrames );

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	mTabDataLayers->mColorMapWidget->setEnabled( field_index >= 0 );

	mPropertiesMap = nullptr;
	mCurrentDisplayData = nullptr;
	mBratLookupTable= nullptr;

	if ( field_index < 0 )
		return;

	assert__( field_index < (int)mDataArrayMap.size() );

	mPropertiesMap = &mDataArrayMap.at( field_index )->m_plotProperty;	  			assert__( mPropertiesMap );
	//TODO - fetch correct display data
	mCurrentDisplayData = mDisplay->GetDisplayData( mOperation, mPropertiesMap->m_name );
	mBratLookupTable = mPropertiesMap->mLUT;

	mTabDataLayers->mColorMapWidget->SetShowSolidColor( mMapView->IsLayerVisible( field_index ) );
	
	mTabDataLayers->mColorMapWidget->blockSignals( true );
	mTabDataLayers->mColorMapWidget->SetLUT( mBratLookupTable );	// TODO color table must be assigned to plot and retrieved here from plot
	mTabDataLayers->mColorMapWidget->blockSignals( false );
	// TODO on the other hand do we want a color table per field???

	//TODO - fetch correct display data
	//mTabDataLayers->mMinRange->setText( n2s<std::string>( mCurrentDisplayData->GetMinValue() ).c_str() );
	//mTabDataLayers->mMaxRange->setText( n2s<std::string>( mCurrentDisplayData->GetMaxValue() ).c_str() );


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


void CMapEditor::HandleShowSolidColorChecked( bool checked )
{
	assert__( mMapView && mPropertiesMap );

	WaitCursor wait;

    mPropertiesMap->m_solidColor = checked;

	mMapView->SetLayerVisible( mTabDataLayers->mFieldsList->currentRow(), mPropertiesMap->m_solidColor, mDisplaying2D );
}


void CMapEditor::HandleShowContourChecked( bool checked )
{
	BRAT_NOT_IMPLEMENTED;
}
void CMapEditor::HandleColorTablesIndexChanged( int index )
{
	if ( !ResetViews( true, true, true, false ) )
	{
		SimpleWarnBox( "A previous map is still processing. Please try again later." );
		return;
	}

	//SelectTab( mTabGeneral->parentWidget() );
	ResetMap();
	mMapView->setVisible( true );
}



void CMapEditor::HandleFieldMinRangeEntered()
{
	//assert__( mCurrentDisplayData );

 //   bool is_converted=false;
 //   const QString rmin = mTabDataLayers->mMinRange->text();
	//mCurrentDisplayData->SetMinValue( rmin.toInt( &is_converted, 10 ) );			//TODO VALIDATE

	//RunButtonClicked();
}
void CMapEditor::HandleFieldMaxRangeEntered()
{
	//assert__( mCurrentDisplayData );

 //   bool is_converted=false;
 //   const QString rmax = mTabDataLayers->mMaxRange->text();
	//mCurrentDisplayData->SetMaxValue( rmax.toInt( &is_converted, 10 ) );			//TODO VALIDATE

	//RunButtonClicked();
}






///////////////////
//	Map Projections
///////////////////

static const unsigned default_projection_id = (unsigned)-1;


QActionGroup* CMapEditor::CreateProjectionsActions()
{
	static const std::vector< std::pair< EActionTag, unsigned > > proj_ids_map =
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
		proj_ids_map[size++].first,
		proj_ids_map[size++].first,
		proj_ids_map[size++].first,
		proj_ids_map[size++].first,
		proj_ids_map[size++].first,
		proj_ids_map[size++].first,
		proj_ids_map[size++].first,
		proj_ids_map[size++].first,
		proj_ids_map[size++].first,
		proj_ids_map[size++].first,
		proj_ids_map[size++].first,
		proj_ids_map[size++].first,
	};


	QActionGroup *g = CActionInfo::CreateActionGroup( this, ActionTags, true );
	auto actions = g->actions();

	// CMapProjection::3D is not in ActionTags, default_projection_id and separator are not in CMapProjection
	//
	assert__( size == ActionTags.size() && size == CMapProjection::GetInstance()->size() + 1 && size == proj_ids_map.size() && size == actions.size() );

	for ( size_t i = 0; i < size; ++i )
	{
		auto *a = actions[ (int) i ];
		if ( a->isSeparator() )
			continue;

		auto id = proj_ids_map[ i ].second;
		if ( (unsigned)id == (unsigned)default_projection_id )
		{
			a->setChecked( true );
			if ( mMapView )
				a->setText( a->text() + " - C.R.S. " + ( mMapView->DefaultProjection().description() ) );
		}
		else
		{
			a->setText( a->text() + " - C.R.S. " + CMapProjection::GetInstance()->IdToCRS( id ).description() );
		}
		a->setData( QVariant::fromValue( id ) );
	}

	return g;
}

void CMapEditor::HandleProjection()
{
	assert__( mMapView );

	Show2D( true );

	auto a = qobject_cast<QAction*>( sender() );				assert__( a && mProjectionsGroup->actions().indexOf( a ) >= 0 );

	unsigned proj_id = a->data().toUInt();

	bool result = false;
	if ( proj_id == PROJ2D_3D )
		Show3D( true );
	else
	if ( proj_id == default_projection_id )
		result = mMapView->SetDefaultProjection();
	else
		result = mMapView->SetProjection( proj_id );

	if ( !result )
	{
		a->setChecked( false );
	}
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_MapEditor.cpp"
