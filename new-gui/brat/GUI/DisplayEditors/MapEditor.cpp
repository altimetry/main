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

#include "DataModels/Model.h"
#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/PlotData/MapProjection.h"
#include "DataModels/PlotData/WorldPlot.h"
#include "DataModels/PlotData/WorldPlotData.h"

#include "GUI/ActionsTable.h"
#include "GUI/ControlPanels/Views/ViewControlPanels.h"

#include "GUI/DisplayWidgets/GlobeWidget.h"				//these 3 includes must be done in this  order to avoid macro definition collisions
#include "GUI/DisplayWidgets/MapWidget.h"
#include "DataModels/Workspaces/Display.h"

#include "BratLogger.h"
#include "BratSettings.h"

#include "MapEditor.h"



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
	mMapView = new CMapWidget( mModel->Settings().mViewsLayerBaseType, this );
	mMapView->ConnectParentRenderWidgets( mProgressBar, mRenderSuppressionCBox );
	mMapView->ConnectParentMeasureActions( mMeasureButton, mActionMeasure, mActionMeasureArea );
	mMapView->ConnectParentGridAction( mActionDecorationGrid );
	mMapView->ConnectParentMapTipsAction( mActionMapTips );
	mActionMapTips->setChecked( true );
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
	AddTab( mTabDataLayers, "Data Layers" );

	//mTabView = new CMapControlsPanelView( this );	//TODO comment in after implementation
	//AddTab( mTabView, "View" );					//TODO comment in after implementation

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
	QAction *after = mExport2ImageAction;
	AddToolBarSeparator( after );

	mActionDecorationGrid = CMapWidget::CreateGridAction( mGraphicsToolBar );
	mGraphicsToolBar->insertAction( after, mActionDecorationGrid );

	mActionMapTips = CMapWidget::CreateMapTipsAction( mGraphicsToolBar );
	mGraphicsToolBar->insertAction( after, mActionMapTips );

	mMeasureButton = CMapWidget::CreateMapMeasureActions( mGraphicsToolBar, mActionMeasure, mActionMeasureArea );
	mGraphicsToolBar->insertWidget( after, mMeasureButton );


    //    Map
    ResetViews( true, true, true, false );		//creates map and connects map actions; hides it
	mMapView->setVisible( true );

	// add menu button for projections: should be done after view created

	AddToolBarSeparator( after );
	mProjectionsGroup = CreateProjectionsActions();
	mToolProjection = AddMenuButton( eActionGroup_Projections, mProjectionsGroup->actions(), after );
	AddToolBarSeparator( after );

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

	// layers

	//...select layers

	connect( mTabDataLayers->mFieldsList, SIGNAL( currentRowChanged( int ) ), this, SLOT( HandleCurrentFieldChanged( int ) ) );

	//... layer options

	connect( mTabDataLayers->mColorMapWidget, SIGNAL( ShowContourToggled( bool ) ), this, SLOT( HandleShowContourChecked( bool ) ) );
	connect( mTabDataLayers->mColorMapWidget, SIGNAL( ContourColorSelected() ), this, SLOT( HandleContourColorSelected() ) );
	connect( mTabDataLayers->mColorMapWidget, SIGNAL( ShowSolidColorToggled( bool ) ), this, SLOT( HandleShowSolidColorChecked( bool ) ) );
	connect( mTabDataLayers->mColorMapWidget, SIGNAL( CurrentIndexChanged( int ) ), this, SLOT( HandleColorTablesIndexChanged( int ) ) );
	connect( mTabDataLayers->mColorMapWidget, SIGNAL( ContoursEditReturnPressed() ), this, SLOT( HandleNumberOfContoursChanged() ) );
	connect( mTabDataLayers->mColorMapWidget, SIGNAL( ContourWidthReturnPressed() ), this, SLOT( HandleContourWidthChanged() ) );

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

	//LOG_FATAL( QString( "2D" ) + ( checked ? " ==> CHECKED" : "==> unchecked" ) );

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

	//LOG_FATAL( QString( "3D" ) + ( checked ? " ==> CHECKED" : "==> unchecked" ) );

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


//virtual 
void CMapEditor::Export2Image( bool save_2d, bool save_3d, const std::string path2d, const std::string path3d, EImageExportType type )
{
	assert__( !save_2d || mMapView && !save_3d || mGlobeView );

	QString extension = CDisplayData::ImageType2String( type ).c_str();
	QString format = type == CDisplayData::ePnm ? "ppm" : extension;

	if ( save_2d && !mMapView->Save2Image( path2d.c_str(), format, extension ) )
		SimpleWarnBox( "There was an error saving file " + path3d );

	if ( save_3d && !mGlobeView->Save2Image( path3d.c_str(), format, extension ) )
		SimpleWarnBox( "There was an error saving file " + path3d );
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
		qApp->processEvents();///////////////////////////////////////////////////// ADDED FOR TESTING
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
	int index = mTabDataLayers->mFieldsList->currentRow();
	if ( index < 0 )
	{
		LOG_WARN( "Test: No data" );
		return false;
	}

	double all_mx, all_Mx, all_my, all_My, all_mz, all_Mz, mx, Mx, my, My, mz, Mz;
	all_mx = all_Mx = all_my = all_My = all_mz = all_Mz = mx = Mx = my = My = mz = Mz = 0.;
	size_t nframes, nvalues, nvalues0, ndefault_values, nan_values;
	nframes = nvalues = nvalues0 = ndefault_values = nan_values = 0;
	std::string error_msg, field;

	if ( !mDataArrayMap.empty() )
	{
		CWorldPlotData *pdata = GetFieldData( index, mDataArrayMap );
		const CMapValues &data = pdata->PlotValues();
		const CMapPlotParameters &map = data( 0 );		Q_UNUSED( map );
		CPlotUnitData unit;
		unit = GetDisplayData( index, mOperation, mDisplay, mDataArrayMap );		Q_UNUSED( unit );
		field = unit.Name();

		all_mx = mx = data[ 0 ].mMinX;
		all_Mx = Mx = data[ 0 ].mMaxX; 
		all_my = my = data[ 0 ].mMinY;
		all_My = My = data[ 0 ].mMaxX;
		all_mz = mz = data[ 0 ].mMinHeightValue;
		all_Mz = Mz = data[ 0 ].mMaxHeightValue;

		if ( data.size() > 1 )
			error_msg += "Unexpected World number of frames greater than 1\n";

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
		+ TF( "nan_values", nan_values )
		+ error_msg
		;

	LOG_FATAL( msg );

	return true;
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



bool CMapEditor::ChangeView()
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

			const CWorldPlotProperties *props = mCurrentDisplayFilesProcessor->GetWorldPlotProperties( map_index++ );	Q_UNUSED( props );  //mPlotPropertiesMap = *props;	// *proc->GetWorldPlotProperties( 0 );
			mMapView->setWindowTitle( t2q( wplot->MakeTitle() ) );

			// for Geo-strophic velocity
			//
			CPlotField * northField =nullptr;
			CPlotField * eastField =nullptr;

			//size_t index = 0;							//index is NOT the index in maps
			for ( auto *field : wplot->mFields )
			{

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

CWorldPlotData* CMapEditor::UpdateCurrentPointers( int field_index )
{
	mPropertiesMap = nullptr;
	mCurrentPlotUnit = nullptr;
	mCurrentBratLookupTable = nullptr;

	if ( field_index < 0 )
		return nullptr;

	assert__( mOperation && mDisplay );

	mPropertiesMap = GetProperties( field_index, mDataArrayMap );
	mCurrentPlotUnit = GetDisplayData( field_index, mOperation, mDisplay, mDataArrayMap );
	mCurrentBratLookupTable = mPropertiesMap->mLUT;
	mCurrentBratLookupTable->ExecMethod( mCurrentPlotUnit.GetColorPalette() );

	assert__( mPropertiesMap && mCurrentPlotUnit && mCurrentBratLookupTable );

	return GetFieldData( field_index, mDataArrayMap );
}



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

	auto IsValidPoint = []( const CMapPlotParameters &map, int32_t i )
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
    const size_t array_size = mDataArrayMap.size();								//assert__( size == mCurrentDisplayFilesProcessor->GetWorldPlotPropertiesSize() );
	for ( size_t i = 0; i < array_size; ++i )
	{
		CWorldPlotData *geo_map = UpdateCurrentPointers( i );			assert__( mPropertiesMap );
        CWorldPlotVelocityData *geo_velocity_map = dynamic_cast<CWorldPlotVelocityData*>( geo_map );
		
		const CMapValues &maps = geo_map->PlotValues();					assert__( maps.size() == 1 );	//simply to check if ever...	

		mCurrentBratLookupTable->GetLookupTable()->SetTableRange( mCurrentPlotUnit.GetCurrentMinValue(), mCurrentPlotUnit.GetCurrentMaxValue() );		//geo_map->GetLookupTable()->GetTableRange()

		const CMapPlotParameters &map = maps( 0 );
		auto const size = map.mValues.size();
		QgsFeatureList flist;

#if defined (USE_POINTS)	//(**)

		for ( auto i = 0u; i < size; ++ i )
		{
			if ( !IsValidPoint( map, i ) )
				continue;

			auto x = i % map.mXaxis.size(); // ( x * geo_map->lats.size() ) + i;
			auto y = i / map.mXaxis.size(); // ( x * geo_map->lats.size() ) + i;

#if defined (USE_FEATURES) //(***)

            ////////// TODO RCCC  //////////////////////////////////////////////////
            if ( geo_velocity_map )
            {
                double magnitude = sqrt( map.mValues[ i ]*map.mValues[ i ] + map.mValuesEast[ i ]*map.mValuesEast[ i ] ) ;
                double angle = atan2(map.mValuesEast[ i ], map.mValues[ i ]) * 180 / M_PI;
                if ( angle < 0 ) {  angle += 360;  }

                mMapView->CreateVectorFeature( flist, map.mXaxis.at( x ), map.mYaxis.at( y ), angle, magnitude );
            }
            else
            {
                //mMapView->CreatePointDataFeature( flist, map.mXaxis.at( x ), map.mYaxis.at( y ), map.mValues[ i ] );
                std::string errorMsg;
                double step_x = const_cast<CFormula*>( mOperation->GetFormula( CMapTypeField::eTypeOpAsX ))->GetStepAsDouble( errorMsg );
                double step_y = const_cast<CFormula*>( mOperation->GetFormula( CMapTypeField::eTypeOpAsY ))->GetStepAsDouble( errorMsg );
                mMapView->CreatePolygonDataFeature( flist, map.mXaxis.at( x ), map.mYaxis.at( y ), map.mValues[ i ], step_x, step_y );

            }
            ///////////////////////////////////////////////////////////////////////
#else
			addRBPoint( geo_map->lons.at( x ), geo_map->lats.at( y ), QColor( (long)(geo_map->vals[ i ]) ), mMainLayer );
#endif  //USE_FEATURES
		}

#if defined (USE_FEATURES)

		//AddDataLayer( props->m_name, 0.333, map.mMinHeightValue, map.mMaxHeightValue, props->m_numContour, flist );  // TODO - RCCC Uncomment this to use Points

        if( geo_velocity_map )
        {
             mMapView->AddArrowDataLayer( mPropertiesMap->Name(), flist );
        }
        else
        {			
            mMapView->AddDataLayer( mPropertiesMap->Name(), 0.1, map.mMinHeightValue, map.mMaxHeightValue, mCurrentBratLookupTable->GetLookupTable(), flist,
				geo_map->GetDataUnit()->IsDate(), RefDateFromUnit( *geo_map->GetDataUnit() ) );
        }

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

		// ( ... )

		//int32_t nFrames = 1;
		//if ( geo_map != nullptr )
		//	nFrames = geo_map->GetNrMaps();

		//m_animationToolbar->SetMaxFrame( nFrames );

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////

		QListWidgetItem *item = new QListWidgetItem( t2q( mCurrentPlotUnit.Name() ) );		//cannot assert__( geo_map->FieldName(0) == mCurrentPlotUnit.Name() );
		item->setToolTip( item->text() );
		mTabDataLayers->mFieldsList->addItem( item );

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

	UpdateCurrentPointers( -1 );

	if ( field_index < 0 )
		return;

	CWorldPlotData *world_data = UpdateCurrentPointers( field_index );			assert__( world_data );

	mTabDataLayers->mColorMapWidget->blockSignals( true );
	// TODO color table and range should be retrieved here from plot, but maps still know nothing about LUTs or data ranges
	// TODO create a current CMapPlotParameters data member
	const CMapValues &maps = world_data->PlotValues();
	const CMapPlotParameters &map = maps( 0 );
	mTabDataLayers->mColorMapWidget->SetLUT( mCurrentBratLookupTable, map.mMinHeightValue, map.mMaxHeightValue );	

	assert__( VirtuallyEqual( map.mMinHeightValue, mCurrentPlotUnit.GetAbsoluteMinValue() ) );
	assert__( VirtuallyEqual( map.mMaxHeightValue, mCurrentPlotUnit.GetAbsoluteMaxValue() ) );

	mTabDataLayers->mColorMapWidget->SetContourColor( mPropertiesMap->m_contourLineColor.GetQColor() );
	mTabDataLayers->mColorMapWidget->blockSignals( false );

	mTabDataLayers->mColorMapWidget->SetShowSolidColor( mMapView->IsDataLayerVisible( field_index ) );
	mTabDataLayers->mColorMapWidget->SetShowContour( mMapView->IsContourLayerVisible( field_index ) );
	mTabDataLayers->mColorMapWidget->SetNumberOfContours( mPropertiesMap->m_numContour );
	mTabDataLayers->mColorMapWidget->SetContoursWidth( mPropertiesMap->mContourLineWidth );
}



///////////////////////////////////////////////////////
//	From widgets to properties, from properties to plot
///////////////////////////////////////////////////////


void CMapEditor::HandleShowSolidColorChecked( bool checked )
{
	assert__( mMapView && mPropertiesMap );

	WaitCursor wait;

	mCurrentPlotUnit.SetSolidColor( checked );
    mPropertiesMap->m_solidColor = checked;

	mMapView->SetDataLayerVisible( mTabDataLayers->mFieldsList->currentRow(), mPropertiesMap->m_solidColor, mDisplaying2D );
}


void CMapEditor::HandleShowContourChecked( bool checked )
{
	int field_index = mTabDataLayers->mFieldsList->currentRow();
	assert__( field_index >= 0 && field_index < (int)mDataArrayMap.size() );

	mCurrentPlotUnit.SetContour( checked );

	if ( mMapView->HasContourLayer( field_index ) )
	{
		mMapView->SetContourLayerVisible( field_index, checked, mDisplaying2D );
		return;
	}

	if ( !checked )
		return;

    WaitCursor wait;

	const CMapValues &maps = GetFieldData( field_index, mDataArrayMap )->PlotValues();
	const CMapPlotParameters &map = maps( 0 );

	CBratLookupTable t;
	t.ExecMethod( "Rainbow" );
	mMapView->AddContourLayer( field_index, mPropertiesMap->Name(), mPropertiesMap->mContourLineWidth, mPropertiesMap->m_contourLineColor.GetQColor(), 
		t.GetLookupTable(), mPropertiesMap->m_numContour, map );
}


void CMapEditor::HandleNumberOfContoursChanged()
{
	WaitCursor wait;												assert__( mMapView && mPropertiesMap );

	int field_index = mTabDataLayers->mFieldsList->currentRow();

	mPropertiesMap->m_numContour = mTabDataLayers->mColorMapWidget->NumberOfContours();

	const CMapValues &maps = GetFieldData( field_index, mDataArrayMap )->PlotValues();
	const CMapPlotParameters &map = maps( 0 );

	mMapView->SetNumberOfContours( field_index, mPropertiesMap->m_numContour, map );
}


void CMapEditor::HandleContourColorSelected()
{
	WaitCursor wait;												assert__( mMapView && mPropertiesMap );

	int field_index = mTabDataLayers->mFieldsList->currentRow();

	mPropertiesMap->m_contourLineColor = mTabDataLayers->mColorMapWidget->ContourColor();

	mMapView->SetContoursProperties( field_index, mPropertiesMap->m_contourLineColor.GetQColor(), mPropertiesMap->mContourLineWidth );
}


void CMapEditor::HandleContourWidthChanged()
{
	WaitCursor wait;												assert__( mMapView && mPropertiesMap );

	int field_index = mTabDataLayers->mFieldsList->currentRow();

	mPropertiesMap->mContourLineWidth = mTabDataLayers->mColorMapWidget->ContoursWidth();

	mMapView->SetContoursProperties( field_index, mPropertiesMap->m_contourLineColor.GetQColor(), mPropertiesMap->mContourLineWidth );
}


void CMapEditor::HandleColorTablesIndexChanged( int index )
{
	WaitCursor wait;												assert__( mMapView && mPropertiesMap );

	int field_index = mTabDataLayers->mFieldsList->currentRow();	assert__( field_index >= 0 && field_index < (int)mDataArrayMap.size() );

	const CMapValues &maps = GetFieldData( field_index, mDataArrayMap )->PlotValues();
	const CMapPlotParameters &map = maps( 0 );

	mCurrentPlotUnit.SetColorPalette( mCurrentBratLookupTable->GetCurrentFunction() );
	mCurrentPlotUnit.SetCurrentMinValue( mCurrentBratLookupTable->GetLookupTable()->GetTableRange()[0] );
	mCurrentPlotUnit.SetCurrentMaxValue( mCurrentBratLookupTable->GetLookupTable()->GetTableRange()[1] );

	//NOTE: no need to assign anything to mPropertiesMap

	mMapView->ChangeDataRenderer( field_index, 0., map.mMinHeightValue, map.mMaxHeightValue, mCurrentBratLookupTable->GetLookupTable() );	
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
