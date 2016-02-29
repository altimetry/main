#ifndef GUI_DISPLAY_WIDGETS_BRAT_VIEWS_H
#define GUI_DISPLAY_WIDGETS_BRAT_VIEWS_H

#include "DataModels/PlotData/ZFXYPlotData.h"
#include "DataModels/PlotData/XYPlotData.h"
#include "DataModels/PlotData/WorldPlotData.h"


#include "MapWidget.h"
#include "3DPlotWidget.h"
#include "2DPlotWidget.h"




 //abstract traits for views specialization
//
//	PLOT is the top v3 data class, inheriting CPlotBase
//
//	widget_type _		the GUI view's base class; the view supplies it's specialized data to this base class
//	properties_type -	brat v3 specialized "property" class: a set of view properties
//	data_type -			v3 class with the processed data, that makes the link to vtk
//
template< class PLOT >
struct plot_traits;




template< class PLOT >
class CBratView : public plot_traits< PLOT >::widget_type
{
	//types

	using base_t = typename plot_traits< PLOT >::widget_type;

protected:

	//instance data

	typename plot_traits< PLOT >::properties_type mPlotProperties;

	//construction / destruction

public:
    CBratView( QWidget *parent = nullptr ) 
		: base_t( parent )
	{}

	virtual ~CBratView()
	{}

	//instance member functions

	virtual void CreatePlot( const typename plot_traits< PLOT >::properties_type *props, PLOT *plot ) = 0;

protected:

	virtual void AddData( typename plot_traits< PLOT >::data_type *pdata ) = 0;
};









//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//									CBratMapView
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


// traits for map view specialization
//
template<>
struct plot_traits< CWPlot >
{
	using widget_type = CMapWidget;
	using properties_type = CWorldPlotProperties;
	using data_type = CWorldPlotData;
};




class CBratMapView : public CBratView< CWPlot >
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    using base_t = CBratView< CWPlot >;

public:
    CBratMapView( QWidget *parent = nullptr ) 
		: base_t( parent )
	{}

	virtual ~CBratMapView()
	{}

	//operations

	void Plot( const CWorldPlotInfo &maps );


	// CBratView interface

	virtual void CreatePlot( const CWorldPlotProperties *props, CWPlot *wplot ) override;

protected:

	virtual void AddData( CWorldPlotData *pdata ) override;
};







//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//									CBrat3DPlotView
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


// traits for 3D view specialization
//
template<>
struct plot_traits< CZFXYPlot >
{
	using widget_type = C3DPlotWidget;
	using properties_type = CZFXYPlotProperties;
	using data_type = CZFXYPlotData;
};





class CBrat3DPlotView : public CBratView< CZFXYPlot >
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    using base_t = CBratView< CZFXYPlot >;


	bool mMultiFrame = false;
	bool mHasClut = false;
	CObList mZfxyPlotData;		//see L:\project\archive\master-A\source\display\ZFXYPlotPanel.cpp for uses of this

public:
    CBrat3DPlotView( QWidget *parent = nullptr ) 
		: base_t( parent )
	{
		mZfxyPlotData.SetDelete(false);
	}

	virtual ~CBrat3DPlotView()
	{}


	// CBratView interface

	virtual void CreatePlot( const CZFXYPlotProperties *props, CZFXYPlot* zfxyplot ) override;

protected:

	virtual void AddData( CZFXYPlotData* pdata ) override;
};







//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//									CBrat2DPlotView
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


// traits for 2D view specialization
//
template<>
struct plot_traits< CPlot >
{
	using widget_type = C2DPlotWidget;
	using properties_type = CXYPlotProperties;
	using data_type = CXYPlotData;
};





class CBrat2DPlotView : public CBratView< CPlot >
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    using base_t = CBratView< CPlot >;

	CXYPlotDataCollection *mPlotDataCollection = nullptr;
	bool mMultiFrame = false;
	bool mPanelMultiFrame = false;	//2 variables in v3: one in the window, the other in the panel

public:
	CBrat2DPlotView( QWidget *parent = nullptr );

	virtual ~CBrat2DPlotView()
	{
		delete mPlotDataCollection;
	}


	// CBratView interface

	virtual void CreatePlot( const CXYPlotProperties *props, CPlot *plot ) override;

protected:

	virtual void AddData( CXYPlotData* pdata ) override;
};







#endif			// GUI_DISPLAY_WIDGETS_BRAT_VIEWS_H
