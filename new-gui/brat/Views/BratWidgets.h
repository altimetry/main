#ifndef GRAPHICS_BRAT_WIDGETS_H
#define GRAPHICS_BRAT_WIDGETS_H


#include "display/PlotData/WorldPlotProperty.h"
#include "display/PlotData/ZFXYPlotData.h"
#include "display/PlotData/XYPlotData.h"


#include "MapWidget.h"
#include "3DPlotWidget.h"
#include "2DPlotWidget.h"


class CmdLineProcessor;

class CWPlot;
class CWorldPlotProperty;
class CWorldPlotData;



template< class PLOT >
struct plot_traits;




template< class PLOT >
class CBratWidget : public plot_traits< PLOT >::widget_type
{
	//types

	using base_t = typename plot_traits< PLOT >::widget_type;

protected:

	//instance data

	typename plot_traits< PLOT >::properties_type mPlotProperty;

	//construction / destruction

public:
    CBratWidget( QWidget *parent = nullptr ) 
		: base_t( parent )
	{}

	virtual ~CBratWidget()
	{}

	//instance member functions

	virtual void CreatePlot( const CmdLineProcessor *proc, PLOT *plot ) = 0;

protected:

	virtual void AddData( typename plot_traits< PLOT >::data_type *pdata ) = 0;
};







//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//									CBratMapWidget
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

template<>
struct plot_traits< CWPlot >
{
	using widget_type = CMapWidget;
	using properties_type = CWorldPlotProperty;
	using data_type = CWorldPlotData;
};



class CBratMapWidget : public CBratWidget< CWPlot >
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    using base_t = CBratWidget< CWPlot >;

public:
    CBratMapWidget( QWidget *parent = nullptr ) 
		: base_t( parent )
	{}

	virtual ~CBratMapWidget()
	{}


	virtual void CreatePlot( const CmdLineProcessor *proc, CWPlot *wplot ) override;

protected:

	virtual void AddData( CWorldPlotData *pdata ) override;
};







//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//									CBrat3DPlotWidget
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


template<>
struct plot_traits< CZFXYPlot >
{
	using widget_type = C3DPlotWidget;
	using properties_type = CZFXYPlotProperty;
	using data_type = CZFXYPlotData;
};





class CBrat3DPlotWidget : public CBratWidget< CZFXYPlot >
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    using base_t = CBratWidget< CZFXYPlot >;


	bool mMultiFrame = false;
	bool mHasClut = false;
	CObList mZfxyPlotData;		//see L:\project\archive\master-A\source\display\ZFXYPlotPanel.cpp for uses of this

public:
    CBrat3DPlotWidget( QWidget *parent = nullptr ) 
		: base_t( parent )
	{
		mZfxyPlotData.SetDelete(false);
	}

	virtual ~CBrat3DPlotWidget()
	{}


	virtual void CreatePlot( const CmdLineProcessor *proc, CZFXYPlot* zfxyplot ) override;

protected:

	virtual void AddData( CZFXYPlotData* pdata ) override;
};







//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//									CBrat2DPlotWidget
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


template<>
struct plot_traits< CPlot >
{
	using widget_type = C2DPlotWidget;
	using properties_type = CXYPlotProperty;
	using data_type = CXYPlotData;
};





class CBrat2DPlotWidget : public CBratWidget< CPlot >
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    using base_t = CBratWidget< CPlot >;

	CXYPlotDataCollection *mPlotDataCollection = nullptr;
	bool mMultiFrame = false;
	bool mPanelMultiFrame = false;	//2 variables in v3: one in the window, the other in the panel

public:
	CBrat2DPlotWidget( QWidget *parent = nullptr );

	virtual ~CBrat2DPlotWidget()
	{
		delete mPlotDataCollection;
	}


	virtual void CreatePlot( const CmdLineProcessor *proc, CPlot *plot ) override;

protected:

	virtual void AddData( CXYPlotData* pdata ) override;
};







#endif			// GRAPHICS_BRAT_WIDGETS_H
