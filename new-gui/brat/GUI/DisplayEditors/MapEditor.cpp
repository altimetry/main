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
#include "DataModels/PlotData/Plots.h"
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


CMapEditor::CMapEditor( CDisplayDataProcessor *proc, CGeoPlot* wplot )
	: base_t( true, proc )
{
	CreateWidgets();

	Start( "" );

	std::vector< CGeoPlot* > wplots;
	wplots.push_back( wplot );

	//CreatePlotData( wplots );	ChangeView();				//of course it won't work
}


CMapEditor::CMapEditor( CDisplayFilesProcessor *proc, CGeoPlot* wplot )
	: base_t( true, proc )
{
	BRAT_MSG_NOT_IMPLEMENTED( "Critical Error: using CDisplayFilesProcessor" )
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
		mDisplaying3D = false;
	}
}


//virtual 
bool CMapEditor::ResetViews( bool reset_2d, bool reset_3d, bool enable_2d, bool enable_3d )
{
	KillGlobe();		Q_UNUSED( reset_3d );		Q_UNUSED( enable_2d );		Q_UNUSED( enable_3d );
	if ( reset_2d )
	{
		ResetAndWireNewMap();
		mDisplaying2D = true;
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
void CMapEditor::SetPlotTitle()
{
	mMapView->setWindowTitle( t2q( mDisplay->Title() ) );
}



//virtual 
bool CMapEditor::Test()
{
	assert__( mCurrentPlotGeo );

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

	if ( !mCurrentPlotGeo->Empty() )
	{
		CWorldPlotData *pdata = mCurrentPlotGeo->PlotData( index );
		const CMapValues &arrays = *pdata;
		const CMapPlotParameters &array = arrays( 0 );
		field = pdata->FieldName();

		all_mx = mx = array.mMinX;
		all_Mx = Mx = array.mMaxX; 
		all_my = my = array.mMinY;
		all_My = My = array.mMaxX;
		all_mz = mz = array.mMinHeightValue;
		all_Mz = Mz = array.mMaxHeightValue;

		if ( arrays.size() > 1 )
			error_msg += "Unexpected World number of frames greater than 1\n";

		nframes = arrays.size();					assert__( nframes == 1 );	//#frames; simply to check if ever...
		int iframe = 0;
		nvalues = arrays.GetDataSize( iframe );
        for ( ; iframe < (int)nframes; ++iframe )
		{
			if ( arrays.GetDataSize( iframe ) != nvalues )
				error_msg += "3D frames with different size\n";
		}

		nvalues0 = arrays.GetDataCountIf( 0, []( const double &v ) { return v == 0; } );
		ndefault_values = arrays.GetDataCountIf( 0, []( const double &v ) { return isDefaultValue( v ); } );
		nan_values = arrays.GetDataCountIf( 0, []( const double &v ) { return std::isnan( v ); } );
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
	WaitCursor wait;				  	assert__( mDisplay->IsZLatLonType() );

	try
	{
		//	I . Assign current plot type
		//without meaning in maps

		//	III . Get plots (v3 plot definitions) from display file

		std::vector< CGeoPlot* > wplots = GetPlotsFromDisplayData< CGeoPlot >( mDisplay );		assert__( wplots.size() && CurrentDisplayDataProcessor() );

		//	IV . Reset views and select general tab 

		if ( !ResetViews( wplots.size() > 0, true, wplots.size() > 0, false ) )
			throw CException( "A previous map is still processing. Please try again later." );
		SelectTab( mTabGeneral->parentWidget() );

		assert__( wplots.size() <= 1 );		//forces redesign if false

		//	II . Reset "current" pointers: TODO check if can be done after I. or if plots II can be done just before V

		delete mCurrentPlotGeo;
		mCurrentPlotGeo = nullptr;
		mCurrentPlotData = nullptr;

		//	V . Process the plot definitions: create plot data and plots

		for ( auto *plot : wplots )
		{			
			mCurrentPlotGeo = plot;				assert__( mCurrentPlotGeo != nullptr );

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

bool CMapEditor::UpdateCurrentPointers( int field_index )
{
	mCurrentPlotData = nullptr;
	mCurrentBratLookupTable = nullptr;

	if ( field_index >= 0 )
	{
		assert__( mOperation && mDisplay && mCurrentPlotGeo );

		mCurrentPlotData = mCurrentPlotGeo->PlotData( field_index );								assert__( mCurrentPlotData && *mCurrentPlotData && mCurrentPlotData->size() == 1 );	//simply to check if ever...
        mCurrentPlotVelocityData = dynamic_cast< CWorldPlotVelocityData* >( mCurrentPlotData );
		mCurrentPlotData = GetDisplayData( field_index, mCurrentPlotGeo );
		mCurrentBratLookupTable = mCurrentPlotData->ColorTablePtr();								assert__( mCurrentBratLookupTable );
		mCurrentBratLookupTable->ExecMethod( mCurrentPlotData->ColorPalette() );
	}

	return mCurrentPlotData != nullptr && mCurrentBratLookupTable != nullptr;
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
	assert__( mMapView && CurrentDisplayDataProcessor() );

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
    const size_t nfields = mCurrentPlotGeo->Size();
	for ( size_t ifield = 0; ifield < nfields; ++ifield )
	{
		bool result = UpdateCurrentPointers( ifield );			assert__( result );		Q_UNUSED( result );

		mCurrentBratLookupTable->GetLookupTable()->SetTableRange( mCurrentPlotData->CurrentMinValue(), mCurrentPlotData->CurrentMaxValue() );		//field_data->GetLookupTable()->GetTableRange()

		const CMapPlotParameters &array = mCurrentPlotData->at( 0 );
		auto const size = array.mValues.size();
		QgsFeatureList flist;

#if defined (USE_POINTS)	//(**)

		for ( auto i = 0u; i < size; ++ i )
		{
			if ( !IsValidPoint( array, i ) )
				continue;

			auto x = i % array.mXaxis.size(); // ( x * field_data->lats.size() ) + i;
			auto y = i / array.mXaxis.size(); // ( x * field_data->lats.size() ) + i;

#if defined (USE_FEATURES) //(***)

            ////////// TODO RCCC  //////////////////////////////////////////////////
            if ( mCurrentPlotVelocityData )
            {
                double magnitude = sqrt( array.mValues[ i ]*array.mValues[ i ] + array.mValuesEast[ i ]*array.mValuesEast[ i ] ) ;
                double angle = atan2(array.mValuesEast[ i ], array.mValues[ i ]) * 180 / M_PI;
                if ( angle < 0 ) {  angle += 360;  }

                mMapView->CreateVectorFeature( flist, array.mXaxis.at( x ), array.mYaxis.at( y ), angle, magnitude );
            }
            else
            {
                //mMapView->CreatePointDataFeature( flist, map.mXaxis.at( x ), map.mYaxis.at( y ), map.mValues[ i ] );
                std::string errorMsg;
                double step_x = const_cast<CFormula*>( mOperation->GetFormula( CMapTypeField::eTypeOpAsX ))->GetStepAsDouble( errorMsg );
                double step_y = const_cast<CFormula*>( mOperation->GetFormula( CMapTypeField::eTypeOpAsY ))->GetStepAsDouble( errorMsg );
                mMapView->CreatePolygonDataFeature( flist, array.mXaxis.at( x ), array.mYaxis.at( y ), array.mValues[ i ], step_x, step_y );

            }
            ///////////////////////////////////////////////////////////////////////
#else
			addRBPoint( field_data->lons.at( x ), field_data->lats.at( y ), QColor( (long)(field_data->vals[ i ]) ), mMainLayer );
#endif  //USE_FEATURES
		}

#if defined (USE_FEATURES)

		//AddDataLayer( props->m_name, 0.333, map.mMinHeightValue, map.mMaxHeightValue, props->m_numContour, flist );  // TODO - RCCC Uncomment this to use Points

        if( mCurrentPlotVelocityData )
        {
             mMapView->AddArrowDataLayer( mCurrentPlotData->FieldName(), flist );
        }
        else
        {			
            mMapView->AddDataLayer( mCurrentPlotData->FieldName(), 0.1, array.mMinHeightValue, array.mMaxHeightValue, mCurrentBratLookupTable->GetLookupTable(), flist,
				mCurrentPlotData->DataUnit()->IsDate(), RefDateFromUnit( *mCurrentPlotData->DataUnit() ) );
        }

#endif 


#else		//(**)

		QgsPolyline points;
		for ( auto i = 0; i < size; ++ i ) 
		{
			if ( !IsValidPoint(i) )
				continue;

			auto x = i % field_data->lons.size();
			auto y = i / field_data->lons.size();

			points.append( QgsPoint( field_data->lons.at( x ), field_data->lats.at( y ) ) );
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

		// ( ... )

		//int32_t nFrames = 1;
		//if ( field_data != nullptr )
		//	nFrames = field_data->GetNrMaps();
		//m_animationToolbar->SetMaxFrame( nFrames );


		QListWidgetItem *item = new QListWidgetItem( t2q( mCurrentPlotData->FieldName() ) );		//cannot assert__( field_data->FieldName(0) == mCurrentPlotUnit.Name() );
		item->setToolTip( item->text() );
		mTabDataLayers->mFieldsList->addItem( item );

		mMapView->SetDataLayerVisible( ifield, mCurrentPlotData->WithSolidColor(), mDisplaying2D );

		if ( mCurrentPlotData->WithContour() )
		{
			mMapView->AddContourLayer( ifield, mCurrentPlotData->FieldName(), mCurrentPlotData->ContourLineWidth(), mCurrentPlotData->ContourLineColor().GetQColor(),
				mCurrentPlotData->NumContours(), array );
		}
	}

	SetPlotTitle();

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

	bool result = UpdateCurrentPointers( field_index );			assert__( result );		Q_UNUSED( result );

	mTabDataLayers->mColorMapWidget->blockSignals( true );
	// color table and range should be retrieved here from plot, but maps (still) know nothing about LUTs or data ranges
	const CMapPlotParameters &map = mCurrentPlotData->at( 0 );
	mTabDataLayers->mColorMapWidget->SetLUT( mCurrentBratLookupTable, map.mMinHeightValue, map.mMaxHeightValue );	

	mTabDataLayers->mColorMapWidget->SetContourColor( mCurrentPlotData->ContourLineColor().GetQColor() );
	mTabDataLayers->mColorMapWidget->blockSignals( false );

	mTabDataLayers->mColorMapWidget->SetShowSolidColor( mMapView->IsDataLayerVisible( field_index ) );
	mTabDataLayers->mColorMapWidget->SetShowContour( mMapView->IsContourLayerVisible( field_index ) );
	// more values that should be retrieved here from plot, but these values are (still) hard/impossible to retrieve from map canvas
	mTabDataLayers->mColorMapWidget->SetNumberOfContours( mCurrentPlotData->NumContours() );
	mTabDataLayers->mColorMapWidget->SetContoursWidth( mCurrentPlotData->ContourLineWidth() );

	mTabDataLayers->mColorMapWidget->setEnabled( !mMapView->IsArrowLayer( field_index ) );
}



///////////////////////////////////////////////////////
//	From widgets to properties, from properties to plot
///////////////////////////////////////////////////////


void CMapEditor::HandleShowSolidColorChecked( bool checked )
{
	assert__( mMapView && mCurrentPlotData );

	WaitCursor wait;

	mCurrentPlotData->SetWithSolidColor( checked );

	mMapView->SetDataLayerVisible( mTabDataLayers->mFieldsList->currentRow(), mCurrentPlotData->WithSolidColor(), mDisplaying2D );
}


void CMapEditor::HandleShowContourChecked( bool checked )
{
	int field_index = mTabDataLayers->mFieldsList->currentRow();	assert__( field_index >= 0 && field_index < mCurrentPlotGeo->Size() && mCurrentPlotData );

	mCurrentPlotData->SetWithContour( checked );

	if ( mMapView->HasContourLayer( field_index ) )
	{
		mMapView->SetContourLayerVisible( field_index, checked, mDisplaying2D );
		return;
	}

	if ( !checked )
		return;

    WaitCursor wait;

	const CMapPlotParameters &array = mCurrentPlotData->at( 0 );

	//CBratLookupTable t;
	//t.ExecMethod( "Rainbow" );
	mMapView->AddContourLayer( field_index, mCurrentPlotData->FieldName(), mCurrentPlotData->ContourLineWidth(), mCurrentPlotData->ContourLineColor().GetQColor(), 
		mCurrentPlotData->NumContours(), array );	//t.GetLookupTable(), 
}


void CMapEditor::HandleNumberOfContoursChanged()
{
	WaitCursor wait;												assert__( mMapView && mCurrentPlotData );

	int field_index = mTabDataLayers->mFieldsList->currentRow();

	mCurrentPlotData->SetNumContours( mTabDataLayers->mColorMapWidget->NumberOfContours() );

	const CMapPlotParameters &array = mCurrentPlotData->at( 0 );

	mMapView->SetNumberOfContours( field_index, mCurrentPlotData->NumContours(), array );
}


void CMapEditor::HandleContourColorSelected()
{
	WaitCursor wait;												assert__( mMapView && mCurrentPlotData );

	int field_index = mTabDataLayers->mFieldsList->currentRow();

	mCurrentPlotData->SetContourLineColor( mTabDataLayers->mColorMapWidget->ContourColor() );

	mMapView->SetContoursProperties( field_index, mCurrentPlotData->ContourLineColor().GetQColor(), mCurrentPlotData->ContourLineWidth() );
}


void CMapEditor::HandleContourWidthChanged()
{
	WaitCursor wait;												assert__( mMapView && mCurrentPlotData );

	int field_index = mTabDataLayers->mFieldsList->currentRow();

	mCurrentPlotData->SetContourLineWidth( mTabDataLayers->mColorMapWidget->ContoursWidth() );

	mMapView->SetContoursProperties( field_index, mCurrentPlotData->ContourLineColor().GetQColor(), mCurrentPlotData->ContourLineWidth() );
}


void CMapEditor::HandleColorTablesIndexChanged( int index )
{
	WaitCursor wait;												assert__( mMapView && mCurrentPlotData );

	int field_index = mTabDataLayers->mFieldsList->currentRow();	assert__( field_index >= 0 && field_index < mCurrentPlotGeo->Size() );

	const CMapPlotParameters &array = mCurrentPlotData->at( 0 );

	mCurrentPlotData->SetColorPalette( mCurrentBratLookupTable->GetCurrentFunction() );
	mCurrentPlotData->SetCurrentMinValue( mCurrentBratLookupTable->GetLookupTable()->GetTableRange()[0] );
	mCurrentPlotData->SetCurrentMaxValue( mCurrentBratLookupTable->GetLookupTable()->GetTableRange()[1] );

	//NOTE: no need to assign anything to mCurrentBratLookupTable

	mMapView->ChangeDataRenderer( field_index, 0., array.mMinHeightValue, array.mMaxHeightValue, mCurrentBratLookupTable->GetLookupTable() );	
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
