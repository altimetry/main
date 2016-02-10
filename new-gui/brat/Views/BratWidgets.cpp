#include "new-gui/brat/stdafx.h"

#include <qgsvectordataprovider.h>


#include "new-gui/Common/QtStringUtils.h"
#include "new-gui/brat/ApplicationLogger.h"

#include "new-gui/brat/DataModels/CmdLineProcessor.h"

#include "display/PlotData/WPlot.h"
#include "display/PlotData/GeoMap.h"


#include "BratWidgets.h"






//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//									CBratMapWidget
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


void CBratMapWidget::CreatePlot( const CmdLineProcessor *proc, CWPlot* wplot )
{
	wplot->GetInfo();

	mPlotProperty = *proc->GetWorldPlotProperty( 0 );
	this->setWindowTitle( t2q( wplot->MakeTitle() ) );

	// for geostrophic velocity
	CPlotField * northField =NULL;
	CPlotField * eastField =NULL;
	//for ( CObArray::iterator itField = wplot->m_fields.begin(); itField != wplot->m_fields.end(); itField++ )
	for ( auto &itField : wplot->m_fields )
	{
		CPlotField* field = CPlotField::GetPlotField( itField );
		if ( field->m_internalFiles.empty() )
			continue;

		if ( field->m_worldProps->m_northComponent && northField == NULL ) {
			northField = field;
			continue;
		}
		else
		if ( field->m_worldProps->m_eastComponent && eastField == NULL ) {
			eastField = field;
			continue;
		}

		// otherwise just add it as regular data
		CGeoMap *geoMap = new CGeoMap( field );
		AddData( geoMap );
	}

	// we have a Vector Plot!
	if ( northField != NULL && eastField != NULL ) {

		CGeoVelocityMap *gvelocityMap = new CGeoVelocityMap( northField, eastField );
		gvelocityMap->SetIsGlyph( true );
		AddData( gvelocityMap );
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
void CBratMapWidget::AddData( CWorldPlotData* pdata )
{
	CGeoMap* geoMap = dynamic_cast<CGeoMap*>( pdata );

	auto IsValidPoint = [&geoMap]( int32_t i )
	{
		bool bOk = geoMap->v4_bits[ i ];

		//	  if (Projection == VTK_PROJ2D_MERCATOR)
		//	  {
		bOk &= geoMap->v4_valids[ i ];
		//	  }
		//
		return bOk;
	};


	auto const size = geoMap->v4_vals.size();
	QgsFeatureList flist;

#if defined (USE_POINTS)	//(**)

	for ( auto i = 0u; i < size; ++ i )
	{
		if ( !IsValidPoint( i ) )
			continue;

		auto x = i % geoMap->v4_lons.size(); // ( x * geoMap->lats.size() ) + i;
		auto y = i / geoMap->v4_lons.size(); // ( x * geoMap->lats.size() ) + i;

#if defined (USE_FEATURES) //(***)
		createPointFeature( flist, geoMap->v4_lons.at( x ), geoMap->v4_lats.at( y ), geoMap->v4_vals[ i ] );
		//createPointFeature( flist, geoMap->lons.at( x ), geoMap->lats.at( y ), QColor( 0, (unsigned char)(geoMap->vals[ i ]), 0 ) );
#else
		addRBPoint( geoMap->lons.at( x ), geoMap->lats.at( y ), QColor( (long)(geoMap->vals[ i ]) ), mMainLayer );
#endif
	}

#if defined (USE_FEATURES)
	auto memL = addMemoryLayer( createPointSymbol( 0.5, Qt::cyan ) );	//(*)	//note that you can use strings like "red" instead!!!
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
	//if ( pdata->GetColorBarRenderer() != NULL )
	//	m_vtkWidget->GetRenderWindow()->AddRenderer( pdata->GetColorBarRenderer()->GetVtkRenderer() );
	//m_plotRenderer->AddData( pdata );


	//femm: the less important part
	//CGeoMap* geoMap = dynamic_cast<CGeoMap*>( pdata );
	//if ( geoMap != NULL )
	//{
	//	wxString textLayer = wxString::Format( "%s", geoMap->GetDataName().c_str() );

	//	m_plotPropertyTab->GetLayerChoice()->Append( textLayer, static_cast<void*>( geoMap ) );
	//	m_plotPropertyTab->SetCurrentLayer( 0 );
	//}

	//int32_t nFrames = 1;
	//if ( geoMap != NULL )
	//	nFrames = geoMap->GetNrMaps();

	//m_animationToolbar->SetMaxFrame( nFrames );
}





//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//									CBrat3DPlotWidget
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////




void CBrat3DPlotWidget::CreatePlot( const CmdLineProcessor *proc, CZFXYPlot *zfxyplot )
{
	zfxyplot->GetInfo();

	mPlotProperty = *proc->GetZFXYPlotProperty( 0 );
	this->setWindowTitle( t2q( zfxyplot->MakeTitle() ) );

	//CZFXYPlotFrame* frame = new CZFXYPlotFrame( NULL, -1, title, zfxyPlotProperty, pos, size );

	for ( auto &itField : zfxyplot->m_fields )
	{
		CPlotField* field = CPlotField::GetPlotField( itField );

		if ( field->m_internalFiles.empty() )
			continue;

		CZFXYPlotData *geoMap = new CZFXYPlotData( zfxyplot, field );

		AddData( geoMap );			//frame->AddData( geoMap );
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

void CBrat3DPlotWidget::AddData( CZFXYPlotData* pdata )
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
	if ( ( !mHasClut ) && ( pdata->GetLookupTable() != NULL ) )
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
	if ( dlg != NULL )
		dlg->Update( 0, szMsg );
#endif

	//v4 pdata->SetRenderer( m_vtkRend );
	mZfxyPlotData.Insert( pdata );

	// v4
	const C3DPlotInfo &maps = pdata->Maps();	assert__( maps.size() == 1 );	//simply to check if ever...
	const C3DPlotValues &values = maps[0];
	AddSurface( values,
		pdata->GetXRangeMin(), pdata->GetXRangeMax(), pdata->GetYRangeMin(), pdata->GetYRangeMax(), 
		pdata->GetComputedRangeMin(),
		pdata->GetComputedRangeMax());

    SetPlotTitle( q2a( windowTitle() ) );

	SetAxisTitles( mPlotProperty.m_xLabel, mPlotProperty.m_yLabel, mPlotProperty.m_name );		//TODO check proper name for z axis, or none

	//v4 m_plotActor->AddInput( pdata );


	if ( pdata->GetColorBarRenderer() != NULL )
	{
		//v4 m_vtkWidget->GetRenderWindow()->AddRenderer( pdata->GetColorBarRenderer()->GetVtkRenderer() );
	}

	std::string textLayer = pdata->GetDataName();

	//v4 GetZfxylayerchoice()->Append( textLayer, static_cast<void*>( pdata ) );
	//v4 m_plotPropertyTab->SetCurrentLayer( 0 );


#if defined(BRAT_V3)		// TODO replace by callback device to inform progress
	if ( dlg != NULL )
		dlg->Update( 1 );
#endif

	//v4 m_animationToolbar->SetMaxFrame( nFrames );


#if defined(BRAT_V3)		// TODO replace by callback device to inform progress
	if ( dlg != NULL )
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
//									CBrat2DPlotWidget
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <qwt_plot_magnifier.h>

CBrat2DPlotWidget::CBrat2DPlotWidget( QWidget *parent )	//parent = nullptr 
	: base_t( parent )
	, mPlotDataCollection( new CXYPlotDataCollection )
{
	QwtLegend *legend = AddLegend( RightLegend );

	//QwtPlotZoomer *zoomer = AddZoomer( Qt::darkBlue );
	auto *zoomer = new QwtPlotMagnifier(canvas());
	QwtPlotPanner *panner = AddPanner();

	Q_UNUSED( zoomer );		Q_UNUSED( panner );		Q_UNUSED( legend );
}


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
void CBrat2DPlotWidget::CreatePlot( const CmdLineProcessor *proc, CPlot *plot )
{
    Q_UNUSED( proc );

	clear();

	// v4 taken from CBratDisplayApp::CreateXYPlot(CPlot* plot, wxSize& size, wxPoint& pos) and callees

    plot->GetInfo();

	//mPlotProperty = *proc->GetXYPlotProperty( 0 );		v4: nothing similar to this, similar to zfxy and lat-lon, was done here

	this->setWindowTitle( t2q( plot->MakeTitle() ) );

	//CXYPlotFrame* frame = new CXYPlotFrame( NULL, -1, title, pos, size );

	int32_t nrFields = (int32_t)plot->m_fields.size();

	for ( int32_t iField = 0; iField < nrFields; iField++ )
	{
		CPlotField* field = plot->GetPlotField( iField );
		CInternalFiles* yfx = field->GetInternalFiles( 0 );
		CXYPlotData* plotData = new CXYPlotData( "", field->m_xyProps );

		plotData->Create( yfx, plot, iField );

		AddData( plotData );		//frame->AddData( plotData );
	}

	//frame->Raise();

	//frame->ShowPropertyPanel();
	//frame->Show( TRUE );

	//pos = frame->GetPosition();

}
void CBrat2DPlotWidget::AddData( CXYPlotData *pdata )
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

	if ( mPlotDataCollection == NULL )
		return;

	CXYPlotProperty* props = pdata->GetPlotProperty();

	mPlotDataCollection->Insert( pdata );

	//m_plotActor->AddInput( pdata->GetVtkDataArrayPlotData(), props->GetVtkProperty2D() );		v4 commented out
	//vtkDataArrayPlotData*	  vtkProperty2D* 

	double xrMin = 0;
	double xrMax = 0;
	double yrMin = 0;
	double yrMax = 0;

	if ( ( isDefaultValue( props->GetXMin() ) ) ||
		( isDefaultValue( props->GetXMax() ) ) )
	{
		mPlotDataCollection->GetXRange( xrMin, xrMax );
	}

	if ( ( isDefaultValue( props->GetYMin() ) ) ||
		( isDefaultValue( props->GetYMax() ) ) )
	{
		mPlotDataCollection->GetYRange( yrMin, yrMax );
	}

	if ( isDefaultValue( props->GetXMin() ) == false )
	{
		xrMin = props->GetXMin();
	}
	if ( isDefaultValue( props->GetXMax() ) == false )
	{
		xrMax = props->GetXMax();
	}

	if ( isDefaultValue( props->GetYMin() ) == false )
	{
		yrMin = props->GetYMin();
	}
	if ( isDefaultValue( props->GetYMax() ) == false )
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

	// v4
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

	setAxisScale( xBottom, xrMin, xrMax );
	setAxisScale( yLeft, yrMin, yrMax );

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
//  if (plotData == NULL)
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

#include "moc_BratWidgets.cpp"
