#include "new-gui/brat/stdafx.h"

#include <qgsvectordataprovider.h>

#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/QtStringUtils.h"
#include "ApplicationLogger.h"

#include "PlotValues.h"
#include "DataModels/DisplayFilesProcessor.h"

#include "DataModels/PlotData/WorldPlot.h"
#include "DataModels/PlotData/WorldPlotData.h"

#include "BratViews.h"



//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//									CBratMapView
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


//void CWorldPlotRenderer::AddData( CWorldPlotData* pdata )
//{
//	pdata->SetRenderer( m_vtkRend );
//
//	m_actors.Insert( pdata );
//	m_transformations->AddItem( pdata->GetTransform() );
//	pdata->GetVtkTransform()->SetTransform( pdata->GetTransform() );
//
//	ResetTextActor();
//}

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
void CBratMapView::PlotTrack( const double *x, const double *y, size_t size, QColor color )		//color = Qt::red 
{
    QgsFeatureList flist;

    for ( auto i = 0u; i < size; ++ i )
    {

#if defined (USE_FEATURES) //(***)
        createPointFeature( flist, x[ i ], y[ i ], color.red() );		//use color.red() as value: not relevant
#else
        addRBPoint( x[ i ], y[ i ], QColor( (long)v[ i ] ), mMainLayer );
#endif
    }

#if defined (USE_FEATURES)
    auto memL = AddMemoryLayer( "", CreatePointSymbol( 0.7, color ) );	//(*)	//note that you can use strings like "red" instead!!!
    memL->dataProvider()->addFeatures( flist );
    //memL->updateExtents();
    //refresh();
#endif
}
void CBratMapView::PlotTrack( const double *x, const double *y, const double *z, size_t size, QColor color )		//color = Qt::red 
{
    QgsFeatureList flist;

    for ( auto i = 0u; i < size; ++ i )
    {

#if defined (USE_FEATURES) //(***)
		createPointFeature( flist, x[ i ], y[ i ], z[ i ] );
#else
        addRBPoint( x[ i ], y[ i ], QColor( (long)v[ i ] ), mMainLayer );
#endif
    }

#if defined (USE_FEATURES)
    auto memL = AddMemoryLayer( "", CreatePointSymbol( 0.7, color ) );	//(*)	//note that you can use strings like "red" instead!!!
    memL->dataProvider()->addFeatures( flist );
    //memL->updateExtents();
    //refresh();
#endif
}




void CBratMapView::AddData( CWorldPlotData *pdata, size_t index )
{
    Q_UNUSED( index );					   	//index is NOT the index in maps

	CWorldPlotData* geo_map = pdata;
	const CWorldPlotInfo &maps = geo_map->PlotInfo();					assert__( maps.size() == 1 );	//simply to check if ever...	
	mMapDataCollection.push_back( geo_map );
	Plot( maps, &geo_map->m_plotProperty );
}
void CBratMapView::Plot( const CWorldPlotInfo &maps, CWorldPlotProperties *props )
{
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
		createPointFeature( flist, map.mXaxis.at( x ), map.mYaxis.at( y ), map.mValues[ i ] );
#else
		addRBPoint( geo_map->lons.at( x ), geo_map->lats.at( y ), QColor( (long)(geo_map->vals[ i ]) ), mMainLayer );
#endif  //USE_FEATURES
	}

#if defined (USE_FEATURES)
    auto memL = AddMemoryLayer( props->m_name, 0.333, map.mMinHeightValue, map.mMaxHeightValue, props->m_numContour );
	memL->dataProvider()->addFeatures( flist );
	//memL->updateExtents();
	//refresh();
#endif  //USE_FEATURES

	return;

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
}





//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//									CBrat3DPlotView
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


CBrat3DPlotView::CBrat3DPlotView( QWidget *parent )		//parent = nullptr 
	: base_t( parent )
	, mZfxyPlotData( new CObArray )
{
	mZfxyPlotData->SetDelete(false);
}



void CBrat3DPlotView::AddData( CZFXYPlotData* pdata, size_t index )
{
    Q_UNUSED( index );

	assert__( pdata != nullptr );

#if defined(BRAT_V3)		// TODO replace by callback device to inform progress
	std::string szMsg = std::string::Format( "Rendering %s ...", GetTitle().c_str() );
	int32_t maxProgbar = 4;
	wxProgressDialog* dlg = new wxProgressDialog( szMsg,
		"Adding data to plot...",
		maxProgbar, this, wxPD_SMOOTH | wxPD_APP_MODAL | wxPD_AUTO_HIDE );

	dlg->SetSize( -1, -1, 350, -1 );
#endif

	int32_t nFrames = pdata->GetNrMaps();

	// v3 note
    // Only pop-up the animation toolbar if this is the first
	// multi-frame dataset and the user has not specified any
	// explicit option so far.
	if ( ( !mMultiFrame ) && ( nFrames > 1 ) )	//v4: this variable was private and assigned only here
	{
		mMultiFrame = true;
		// TODO translate this to v4
		//m_menuBar->Enable( ID_MENU_VIEW_SLIDER, true );
		//ShowAnimationToolbar( true );

	}
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

	// TODO m_plotPanel->AddData( pdata, dlg );
	// TODO taken from void CZFXYPlotPanel::AddData( CZFXYPlotData* pdata, wxProgressDialog* dlg )

#if defined(BRAT_V3)		// TODO replace by callback device to inform progress
	std::string szMsg = std::string::Format( "Displaying data: %s ...", pdata->GetDataTitle().c_str() );
	if ( dlg != nullptr )
		dlg->Update( 0, szMsg );
#endif

	//v4 pdata->SetRenderer( m_vtkRend );
	mZfxyPlotData->Insert( pdata );

	// v4
	const C3DPlotInfo &maps = pdata->Maps();	assert__( maps.size() == 1 );	//simply to check if ever...
	const C3DPlotParameters &values = maps[0];
	AddSurface( values,
		pdata->GetXRangeMin(), pdata->GetXRangeMax(), pdata->GetYRangeMin(), pdata->GetYRangeMax(), 
		pdata->GetComputedRangeMin(),
		pdata->GetComputedRangeMax());

    SetPlotTitle( q2a( windowTitle() ) );

    CZFXYPlotProperties *props = pdata->GetPlotProperties();
	SetAxisTitles( props->m_xLabel, props->m_yLabel, props->m_name );		//TODO check proper name for z axis, or none

	//v4 m_plotActor->AddInput( pdata );


#if defined (BRAT_V3)									  //TODO this file is never executed in v3, but remains here as a remark for something whose port is missing
	if ( pdata->GetColorBarRenderer() != nullptr )
	{
		//v4 m_vtkWidget->GetRenderWindow()->AddRenderer( pdata->GetColorBarRenderer()->GetVtkRenderer() );
	}
#endif

	std::string textLayer = pdata->GetDataName();

	//v4 GetZfxylayerchoice()->Append( textLayer, static_cast<void*>( pdata ) );
	//v4 m_plotPropertyTab->SetCurrentLayer( 0 );


#if defined(BRAT_V3)		// TODO replace by callback device to inform progress
	if ( dlg != nullptr )
		dlg->Update( 1 );
#endif

	//v4 m_animationToolbar->SetMaxFrame( nFrames );


#if defined(BRAT_V3)		// TODO replace by callback device to inform progress
	if ( dlg != nullptr )
		dlg->Update( 3 );
#endif

	//v4 m_plotPropertyTab->UpdateTitleControls();
	//v4 UpdateRender();

#if defined(BRAT_V3)		// TODO replace by callback device to inform progress
	dlg->Destroy();
#endif
}




//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//									CBrat2DPlotView
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


CBrat2DPlotView::CBrat2DPlotView( QWidget *parent )	//parent = nullptr 
	: base_t( parent )
	, mPlotDataCollection( new CXYPlotDataCollection )
	, mZfxyPlotData( new CObArray )
{
	mZfxyPlotData->SetDelete(false);

	QwtLegend *legend = AddLegend( RightLegend );		Q_UNUSED( legend );
}


void CBrat2DPlotView::AddData( CXYPlotData *pdata, size_t index, bool histogram )
{
    Q_UNUSED( index );

	//v4 taken from CXYPlotFrame::AddData(CXYPlotData* data)

            //v3 comment
			//uniquified_name = self.usg.uniquify(data.GetName())
			//if not uniquified_name.endswith(" 1"):
			//data.SetName(uniquified_name)

	// v3 note Only pop-up the animation toolbar if this is the first
	// multi-frame dataset and the user has not specified any
	// explicit option so far.

	if ( !mMultiFrame )	//v4: this variable was private and assigned only here
	{
		int32_t nFrames = pdata->GetNumberOfFrames();
		if ( nFrames > 1 )
		{
			mMultiFrame = true;
			// TODO translate this to v4
			// m_menuBar->Enable( ID_MENU_VIEW_SLIDER, true );
			// ShowAnimationToolbar( true );
		}
	}

	//m_plotPanel->AddData( data );
	//TODO taken from void CXYPlotPanel::AddData( CXYPlotData* pdata )

	if ( mPlotDataCollection == nullptr )
		return;

	CXYPlotProperties* props = pdata->GetPlotProperties();

	mPlotDataCollection->Insert( pdata );

	//m_plotActor->AddInput( pdata->GetVtkDataArrayPlotData(), props->GetVtkProperty2D() );		v4 commented out
	//vtkDataArrayPlotData*	  vtkProperty2D* 

	double xrMin = 0;
	double xrMax = 0;
	double yrMin = 0;
	double yrMax = 0;

	if ( isDefaultValue( props->GetXMin() ) || isDefaultValue( props->GetXMax() ) )
	{
        mPlotDataCollection->GetXRange( xrMin, xrMax );
	}

	if ( isDefaultValue( props->GetYMin() ) || isDefaultValue( props->GetYMax() ) )
	{
        mPlotDataCollection->GetYRange( yrMin, yrMax );
	}

	if ( !isDefaultValue( props->GetXMin() ) )
	{
		xrMin = props->GetXMin();
	}
	if ( !isDefaultValue( props->GetXMax() ) )
	{
		xrMax = props->GetXMax();
	}

	if ( !isDefaultValue( props->GetYMin() ) )
	{
		yrMin = props->GetYMin();
	}
	if ( !isDefaultValue( props->GetYMax() ) )
	{
		yrMax = props->GetYMax();
	}

	/*v4 commented out; for an example of a SetRange method, see //(*) below
	SetXRange( xrMin, xrMax );			***
	SetYRange( yrMin, yrMax );			***

	SetBaseX( props->GetXBase() );
	SetBaseY( props->GetYBase() );

	SetLogX( props->GetXLog() );
	SetLogY( props->GetYLog() );

	SetXTitle( props->GetXLabel() );	***
	SetYTitle( props->GetYLabel() );	***

	SetTitle( props->GetTitle() );		***

	if ( isDefaultValue( props->GetXNumTicks() ) == false )
	{
		m_plotActor->SetNumberOfXLabels( props->GetXNumTicks() );
	}

	if ( isDefaultValue( props->GetYNumTicks() ) == false )
	{
		m_plotActor->SetNumberOfYLabels( props->GetYNumTicks() );
	}
	*/

	int32_t nFrames = pdata->GetNumberOfFrames();
	mPanelMultiFrame |= ( nFrames > 1 );

	// v4: all following code is v4
    SetPlotTitle( q2a( windowTitle() ) );

    // Set axis titles
    SetAxisTitles( props->GetXLabel(), props->GetYLabel() );

    // Insert new curves
	QwtPlotCurve *curve = nullptr;
	CHistogram *histo = nullptr;
	if ( histogram )
		histo = AddHistogram( props->GetName(), color_cast< QColor >( props->GetColor() ), pdata->GetQwtArrayPlotData() );
	else
		curve = AddCurve( props->GetName(), color_cast< QColor >( props->GetColor() ), pdata->GetQwtArrayPlotData() );

    // Markers example
    //  ...a horizontal line at y = 0...
    //  ...a vertical line at x = 2 * pi
	//
    //QwtPlotMarker *mY = AddMarker( "y = 0", QwtPlotMarker::HLine, 0.0 );
    //QwtPlotMarker *mX = AddMarker( "x = 2 pi", QwtPlotMarker::VLine, 2.0 * M_PI );
    //mX->setLinePen( QPen( Qt::black, 0, Qt::DashDotLine ) );								Q_UNUSED( mY );

	SetAxisScales( xrMin, xrMax, yrMin, yrMax );

	replot();

	Q_UNUSED( curve );		Q_UNUSED( histo );
}

//(*)
//
//void CXYPlotPanel::SetYRange( double min, double max )
//{
//	m_currentYMin = min;
//	m_currentYMax = max;
//	m_plotActor->SetYRange( min, max );
//}


//void vtkXYPlotActor::AddInput(vtkPlotData *plotData, vtkProperty2D* property)
//{
//  if (plotData == nullptr)
//    {
//    vtkErrorMacro(<<"Trying to add an empty object");
//    return;
//    }
//
//  // Check if plotdata is not already in the list
//  if (PlotData->IsItemPresent(plotData) == 0)
//    {
//    PlotData->AddItem(plotData);
//    vtkPolyDataMapper2D* plotMapper = vtkPolyDataMapper2D::New();
//    plotMapper->SetInput(plotData->GetOutput());
//    vtkActor2D* plotActor = vtkActor2D::New();
//    plotActor->SetMapper(plotMapper);
//    plotActor->PickableOn();
//    plotMapper->Delete();
//    if (property!=0)
//      {
//      plotActor->SetProperty(property);
//      }
//    PlotActors->AddItem(plotActor);
//    plotActor->Delete();
//    Modified();
//    }
//}




void CBrat2DPlotView::AddData( CZFXYPlotData* pdata, size_t index )
{
    assert__( pdata != nullptr );

#if defined(BRAT_V3)		// TODO replace by callback device to inform progress
    std::string szMsg = std::string::Format( "Rendering %s ...", GetTitle().c_str() );
    int32_t maxProgbar = 4;
    wxProgressDialog* dlg = new wxProgressDialog( szMsg,
        "Adding data to plot...",
        maxProgbar, this, wxPD_SMOOTH | wxPD_APP_MODAL | wxPD_AUTO_HIDE );

    dlg->SetSize( -1, -1, 350, -1 );
#endif

    int32_t nFrames = pdata->GetNrMaps();

    // v3 note
    // Only pop-up the animation toolbar if this is the first
    // multi-frame dataset and the user has not specified any
    // explicit option so far.
    if ( ( !mMultiFrame ) && ( nFrames > 1 ) )	//v4: this variable was private and assigned only here
    {
        mMultiFrame = true;
        // TODO translate this to v4
        //m_menuBar->Enable( ID_MENU_VIEW_SLIDER, true );
        //ShowAnimationToolbar( true );

    }
#if defined (BRAT_V3)									  //TODO this file is never executed in v3, but remains here as a remark for something whose port is missing
    if ( !mHasClut && pdata->GetLookupTable() != nullptr )
#else
//    if ( !mHasClut )
#endif
//    {
//        mHasClut = true;
//        // TODO translate this to v4
//        //m_menuBar->Enable( ID_MENU_VIEW_COLORBAR, m_hasClut );
//        //m_menuBar->Enable( ID_MENU_VIEW_CLUTEDIT, m_hasClut );
//        //v3 commented out ShowColorBar(data->m_plotProperty.m_showColorBar);
//    }

    // TODO m_plotPanel->AddData( pdata, dlg );
    // TODO taken from void CZFXYPlotPanel::AddData( CZFXYPlotData* pdata, wxProgressDialog* dlg )

#if defined(BRAT_V3)		// TODO replace by callback device to inform progress
    std::string szMsg = std::string::Format( "Displaying data: %s ...", pdata->GetDataTitle().c_str() );
    if ( dlg != nullptr )
        dlg->Update( 0, szMsg );
#endif

    //v4 pdata->SetRenderer( m_vtkRend );
    mZfxyPlotData->Insert( pdata );

    // v4
    const C3DPlotInfo *maps = dynamic_cast<const C3DPlotInfo*>( &pdata->Maps() );		assert__( maps->size() == 1 );	//simply to check if ever...
	CZFXYPlotProperties *props = pdata->GetPlotProperties();							assert( pdata->GetDataName() == props->m_name );

    //mSpectogramData = new SpectrogramData();
    //mSpectogramData->InjectData( values.mXaxis, values.mYaxis, values.mBits, values.mValues );
    //Spectogram( parentWidget() );

	CreateSurface( pdata->GetPlotProperties()->m_name, *maps, props->m_minContourValue, props->m_maxContourValue, props->m_numContour, index );

    SetPlotTitle( q2a( windowTitle() ) );

    SetAxisTitles( pdata->GetPlotProperties()->m_xLabel, pdata->GetPlotProperties()->m_yLabel, pdata->GetPlotProperties()->m_name );

    //v4 m_plotActor->AddInput( pdata );


#if defined (BRAT_V3)									  //TODO this file is never executed in v3, but remains here as a remark for something whose port is missing
    if ( pdata->GetColorBarRenderer() != nullptr )
    {
        //v4 m_vtkWidget->GetRenderWindow()->AddRenderer( pdata->GetColorBarRenderer()->GetVtkRenderer() );
    }
#endif

    std::string textLayer = pdata->GetDataName();

    //v4 GetZfxylayerchoice()->Append( textLayer, static_cast<void*>( pdata ) );
    //v4 m_plotPropertyTab->SetCurrentLayer( 0 );


#if defined(BRAT_V3)		// TODO replace by callback device to inform progress
    if ( dlg != nullptr )
        dlg->Update( 1 );
#endif

    //v4 m_animationToolbar->SetMaxFrame( nFrames );


#if defined(BRAT_V3)		// TODO replace by callback device to inform progress
    if ( dlg != nullptr )
        dlg->Update( 3 );
#endif

    //v4 m_plotPropertyTab->UpdateTitleControls();
    //v4 UpdateRender();

#if defined(BRAT_V3)		// TODO replace by callback device to inform progress
    dlg->Destroy();
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_BratViews.cpp"
