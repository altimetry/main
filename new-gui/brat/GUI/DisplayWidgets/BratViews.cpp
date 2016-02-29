#include "new-gui/brat/stdafx.h"

#include <qgsvectordataprovider.h>

#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/QtStringUtils.h"
#include "ApplicationLogger.h"

#include "DataModels/DisplayFilesProcessor.h"

#include "DataModels/PlotData/WorldPlot.h"
#include "DataModels/PlotData/WorldPlotData.h"

#include "BratViews.h"



//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//									CBratMapView
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


void CBratMapView::CreatePlot( const CWorldPlotProperties *props, CWPlot* wplot )
{
	//wplot->GetInfo();		 assumed done by caller

	mPlotProperties = *props;	// *proc->GetWorldPlotProperties( 0 );
	this->setWindowTitle( t2q( wplot->MakeTitle() ) );

	// for Geo-strophic velocity
	//
	CPlotField * northField =nullptr;
	CPlotField * eastField =nullptr;

	for ( auto &itField : wplot->m_fields )
	{
		CPlotField* field = CPlotField::GetPlotField( itField );

#if defined(Q_OS_WIN)
        assert__(field->m_worldProps == props );
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
		AddData( new CWorldPlotData( field ) );					   		//v4 note: CWorldPlotData ctor is only invoked here
	}

	// we have a Vector Plot!
	if ( northField != nullptr && eastField != nullptr ) 
	{
		AddData( new CWorldPlotVelocityData( northField, eastField ) );	//v4 note: CWorldPlotVelocityData ctor is only invoked here
	}
	else if ( northField != eastField ) 
	{
		CException e( "CBratDisplayApp::CreateWPlot - incomplete std::vector plot components", BRATHL_INCONSISTENCY_ERROR );
		LOG_TRACE( e.what() );
		throw e;
	}
}
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
void CBratMapView::AddData( CWorldPlotData* pdata )
{
	CWorldPlotData* geoMap = dynamic_cast<CWorldPlotData*>( pdata );
	const CWorldPlotInfo &maps = geoMap->PlotInfo();					assert__( maps.size() == 1 );	//simply to check if ever...
	Plot( maps );
}
void CBratMapView::Plot( const CWorldPlotInfo &maps )
{

	auto IsValidPoint = [&maps]( int32_t i )
	{
		bool bOk = maps(0).mBits[ i ];

		//	  if (Projection == VTK_PROJ2D_MERCATOR)
		//	  {
		bOk &= maps(0).mValidMercatorLatitudes[ i ];
		//	  }
		//
		return bOk;
	};


	auto const size = maps(0).mValues.size();
	QgsFeatureList flist;

#if defined (USE_POINTS)	//(**)

	for ( auto i = 0u; i < size; ++ i )
	{
		if ( !IsValidPoint( i ) )
			continue;

		auto x = i % maps(0).mXaxis.size(); // ( x * geoMap->lats.size() ) + i;
		auto y = i / maps(0).mXaxis.size(); // ( x * geoMap->lats.size() ) + i;

#if defined (USE_FEATURES) //(***)
		createPointFeature( flist, maps(0).mXaxis.at( x ), maps(0).mYaxis.at( y ), maps(0).mValues[ i ] );
		//createPointFeature( flist, geoMap->lons.at( x ), geoMap->lats.at( y ), QColor( 0, (unsigned char)(geoMap->vals[ i ]), 0 ) );
#else
		addRBPoint( geoMap->lons.at( x ), geoMap->lats.at( y ), QColor( (long)(geoMap->vals[ i ]) ), mMainLayer );
#endif
	}

#if defined (USE_FEATURES)
	auto memL = AddMemoryLayer( createPointSymbol( 0.5, Qt::cyan ) );	//(*)	//note that you can use strings like "red" instead!!!
	memL->dataProvider()->addFeatures( flist );
	//memL->updateExtents();
	//refresh();
#endif

	return;

#else		//(**)

	QgsPolyline points;
	for ( auto i = 0; i < size; ++ i ) 
	{
		if ( !IsValidPoint(i) )
			continue;

		auto x = i % geoMap->lons.size();
		auto y = i / geoMap->lons.size();

		points.append( QgsPoint( geoMap->lons.at( x ), geoMap->lats.at( y ) ) );
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


	//femm: the less important part
	//CWorldPlotData* geoMap = dynamic_cast<CWorldPlotData*>( pdata );
	//if ( geoMap != nullptr )
	//{
	//	wxString textLayer = wxString::Format( "%s", geoMap->GetDataName().c_str() );

	//	m_plotPropertyTab->GetLayerChoice()->Append( textLayer, static_cast<void*>( geoMap ) );
	//	m_plotPropertyTab->SetCurrentLayer( 0 );
	//}

	//int32_t nFrames = 1;
	//if ( geoMap != nullptr )
	//	nFrames = geoMap->GetNrMaps();

	//m_animationToolbar->SetMaxFrame( nFrames );
}





//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//									CBrat3DPlotView
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////




void CBrat3DPlotView::CreatePlot( const CZFXYPlotProperties *props, CZFXYPlot *zfxyplot )
{
	//zfxyplot->GetInfo();	 assumed done by caller

	mPlotProperties = *props;	// *proc->GetZFXYPlotProperties( 0 );
	this->setWindowTitle( t2q( zfxyplot->MakeTitle() ) );

	//CZFXYPlotFrame* frame = new CZFXYPlotFrame( nullptr, -1, title, zfxyPlotProperty, pos, size );

	for ( auto &itField : zfxyplot->m_fields )
	{
		CPlotField* field = CPlotField::GetPlotField( itField );

		assert__(field->m_zfxyProps == props );

		if ( field->m_internalFiles.empty() )
			continue;

		//frame->AddData( geoMap );
		AddData( new CZFXYPlotData( zfxyplot, field ) );	// v4 note: CZFXYPlotData ctor only invoked here
	}

	//frame->UpdateView();
	//frame->Raise();
	//frame->Show( TRUE );

	// ---- Fix for bug on Linux to resize correctly VTK widget.
	//wxSize sizefFrame = frame->GetSize();
	//sizefFrame += wxSize( 1, 1 );
	//frame->SetSize( sizefFrame );
	// ----

	//pos = frame->GetPosition();
}

void CBrat3DPlotView::AddData( CZFXYPlotData* pdata )
{
	assert__( pdata != nullptr );

#if defined(BRAT_V3)		// TODO replace by callback device to inform progress
	std::string szMsg = std::string::Format( "Rendering %s ...", this->GetTitle().c_str() );
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
	mZfxyPlotData.Insert( pdata );

	// v4
	const C3DPlotInfo &maps = pdata->Maps();	assert__( maps.size() == 1 );	//simply to check if ever...
	const C3DPlotParameters &values = maps[0];
	AddSurface( values,
		pdata->GetXRangeMin(), pdata->GetXRangeMax(), pdata->GetYRangeMin(), pdata->GetYRangeMax(), 
		pdata->GetComputedRangeMin(),
		pdata->GetComputedRangeMax());

    SetPlotTitle( q2a( windowTitle() ) );

	SetAxisTitles( mPlotProperties.m_xLabel, mPlotProperties.m_yLabel, mPlotProperties.m_name );		//TODO check proper name for z axis, or none

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
{
	QwtLegend *legend = AddLegend( RightLegend );		Q_UNUSED( legend );

	QwtPlotMagnifier *mag = AddMagnifier();				Q_UNUSED( mag );	//QwtPlotZoomer *zoomer = AddZoomer( Qt::darkBlue );		Q_UNUSED( zoomer );

	QwtPlotPanner *panner = AddPanner();				Q_UNUSED( panner );
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
void CBrat2DPlotView::CreatePlot( const CXYPlotProperties *props, CPlot *plot )
{
	//clear();		// TODO: currently the editor deletes the widget; decide which action to take on new plot

	// v4 taken from CBratDisplayApp::CreateXYPlot(CPlot* plot, wxSize& size, wxPoint& pos) and callees

    //plot->GetInfo(); 	 assumed done by caller

	mPlotProperties = *props;		// *proc->GetXYPlotProperty( 0 );		v4: nothing similar to this, similar to zfxy and lat - lon, was done here

	this->setWindowTitle( t2q( plot->MakeTitle() ) );

	//CXYPlotFrame* frame = new CXYPlotFrame( nullptr, -1, title, pos, size );

	int32_t nrFields = (int32_t)plot->m_fields.size();

	for ( int32_t iField = 0; iField < nrFields; iField++ )
	{
		//auto field = plot->GetPlotField(iField);
		//assert__(field->m_xyProps == props );

		//frame->AddData( plotData );
		AddData( new CXYPlotData( plot, iField ) );		//v4 note: CXYPlotData ctor only invoked here
	}

	//frame->Raise();

	//frame->ShowPropertyPanel();
	//frame->Show( TRUE );

	//pos = frame->GetPosition();

}
void CBrat2DPlotView::AddData( CXYPlotData *pdata )
{
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
	auto const vtk_color = props->GetColor();
	QwtPlotCurve *curve = AddCurve( props->GetTitle(), QColor(vtk_color.Red()*255, vtk_color.Green()*255, vtk_color.Blue()*255), pdata->GetQwtArrayPlotData() );

    // Markers
    //  ...a horizontal line at y = 0...
    //  ...a vertical line at x = 2 * pi
	//
    QwtPlotMarker *mY = AddMarker( "y = 0", QwtPlotMarker::HLine, 0.0 );
    QwtPlotMarker *mX = AddMarker( "x = 2 pi", QwtPlotMarker::VLine, 2.0 * M_PI );
    mX->setLinePen( QPen( Qt::black, 0, Qt::DashDotLine ) );

	SetAxisScales( xrMin, xrMax, yrMin, yrMax );

	replot();

	Q_UNUSED( curve );		Q_UNUSED( mY );
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
//  if (this->PlotData->IsItemPresent(plotData) == 0)
//    {
//    this->PlotData->AddItem(plotData);
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
//    this->PlotActors->AddItem(plotActor);
//    plotActor->Delete();
//    this->Modified();
//    }
//}







///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_BratViews.cpp"
