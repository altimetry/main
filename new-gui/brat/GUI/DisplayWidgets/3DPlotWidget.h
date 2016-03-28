#ifndef GUI_DISPLAY_WIDGETS_3D_PLOT_WIDGET_H
#define GUI_DISPLAY_WIDGETS_3D_PLOT_WIDGET_H

#include <qtimer.h>
#include <qwt3d_surfaceplot.h>


class CDisplayFilesProcessor;
class CZFXYPlot;
class CZFXYPlotData;

struct C3DPlotParameters;

struct CBrat3DFunction;


typedef Qwt3D::SurfacePlot SurfacePlot; // VC6/moc issue



////////////////////////////////////////////////////////////
// TODO delete;		Examples related declarations
////////////////////////////////////////////////////////////



class Bar;	//enrichment example
class Hat2;	//enrichment example



enum Plot3dExample
{
	e_Autoswitching_axes, e_Plot3dExample_first = e_Autoswitching_axes,
	e_Simple_SurfacePlot,
	e_Vertex_Enrichment,

	e_Plot3dExample_size
};

inline Plot3dExample& operator ++( Plot3dExample &ex )
{
	int exi = static_cast<int>( ex ) + 1;

	if ( exi >= e_Plot3dExample_size )
		ex = e_Plot3dExample_first;
	else
		ex = static_cast<Plot3dExample>( exi );

	return ex;// static_cast<Plot3dExample>( static_cast<int>(ex)+ 1 );
}


class Plot : public Qwt3D::SurfacePlot
{
	Q_OBJECT

public:
    Plot( QWidget* pw, int updateinterval = 0 );

public slots:
	void rotate();
};



////////////////////////////////////////////////////////////
//			Internal Qwt3D function 
////////////////////////////////////////////////////////////


class CBrat3DPlot : public Qwt3D::SurfacePlot
{
	Q_OBJECT

	// types

	using base_t = Qwt3D::SurfacePlot;

	//instance data

	CBrat3DFunction *mFunction = nullptr;

	QActionGroup *mStyleGroup = nullptr;
    QActionGroup *mCoordinateStyleGroup = nullptr;
	QAction *mAnimateAction = nullptr;
    QAction *mStandardColorAction = nullptr;
    QAction *mHomeAction = nullptr;

	QTimer mTimer;

	//construction / destruction

	void CreateContextMenu();

public:
	CBrat3DPlot( QWidget *pw );

	virtual ~CBrat3DPlot();

	// 

	void SetLogarithmicScale( bool onlyz, bool log );

	void SetAxisTitles( const std::string &xtitle, const std::string &ytitle, const std::string &ztitle )
	{
		SetAxisTitle( Qwt3D::X1, xtitle );
		SetAxisTitle( Qwt3D::Y1, ytitle );
		SetAxisTitle( Qwt3D::Z1, ztitle );		
	}

	// 

	void AddSurface( const C3DPlotParameters &values, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax );

	void SetPlotStyle( Qwt3D::PLOTSTYLE style = Qwt3D::FILLEDMESH );

	void SetCoordinateStyle( Qwt3D::COORDSTYLE style = Qwt3D::FRAME );

protected:
	void SetAxisTitle( Qwt3D::AXIS axis, const std::string &title );

protected slots:

	void rotate();

	void Animate( int updateinterval = 200 );
	void Reset();
	void StandardColor( bool standard );
	void PlotStyle();
	void CoordinateStyle();
};






////////////////////////////////////////////////////////////
//					3D Plot Class
////////////////////////////////////////////////////////////


class C3DPlotWidget : public QFrame
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// types

	using base_t = QFrame;


	//static data

public:

	static const std::string smFontName;
	static const int smAxisFontSize = 8;
	static const int smTitleFontSize = 10;


protected:

	// instance data

	std::vector<Qwt3D::SurfacePlot*> mPlots;

	QSize m_SizeHint;

	bool mShowContour = true;
	bool mShowMesh = true;

	double level_, width_;			//enrichment example
	Bar *bar = nullptr;				//enrichment example
	Hat2* hat = nullptr;			//enrichment example
	QFrame *frame = nullptr;		//enrichment example
	QSlider *levelSlider = nullptr;	//enrichment example
	QSlider *widthSlider = nullptr;	//enrichment example
	void setupUi();					//enrichment example

	//construction /destruction

	void Autoswitching_axes();	//auto-switching example
	void Simple_SurfacePlot();	//surface example
	void setColor();			//enrichment example
	void Vertex_Enrichment();	//enrichment example

public:
	C3DPlotWidget( QWidget *parent = nullptr );

	virtual ~C3DPlotWidget();

	// access/assignment

	//...create plot

	void AddSurface( const C3DPlotParameters &values, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax );


	//...title

	void SetPlotTitle( const std::string &title );


	//style

    bool HasContour() const;
    void ShowContour( bool show );

    bool HasSolidColor() const;
    void ShowSolidColor( bool show );


	//axis

	void SetAxisTitles( const std::string &xtitle, const std::string &ytitle, const std::string &ztitle );

	void SetLogarithmicScaleZ( bool log );

	void SetLogarithmicScale( bool log );


	// protected operations

protected:
	QSize sizeHint() const override;

	void SetCentralWidget( QWidget *w );

	void SetStyle();

public slots:
	void setLevel( int );		//enrichment example
	void setWidth( int );		//enrichment example
	void barSlot();				//enrichment example
};


// DO NOT delete
//
// qwtplot3d original comments: 
//
//	- Mouse
//
//	key/mousebutton combination for data/coordinatesystem moves inside the widget - default behavior:
//
//	rotate around x axis: Qt::LeftButton 
//	rotate around y axis: Qt::LeftButton | Qt::ShiftButton
//	rotate around z axis: Qt::LeftButton 
//	scale x:              Qt::LeftButton | Qt::AltButton 
//	scale y:              Qt::LeftButton | Qt::AltButton 
//	scale z:              Qt::LeftButton | Qt::AltButton | Qt::ShiftButton
//	zoom:                 Qt::LeftButton | Qt::AltButton | Qt::ControlButton
//	shifting along x:     Qt::LeftButton | Qt::ControlButton 
//	shifting along y:     Qt::LeftButton | Qt::ControlButton
//
//	mouseMoveEvent() evaluates this function - if overridden, their usefulness becomes somehow limited
//
//
//	- Keyboard
//
//	//keybutton combination for data/coordinatesystem moves inside the widget - default behavior:
//
//	//rotate around x axis: [Key_Down, Key_Up] 
//	//rotate around y axis: SHIFT+[Key_Right, Key_Left]
//	//rotate around z axis: [Key_Right, Key_Left] 
//	//scale x:              ALT+[Key_Right, Key_Left] 
//	//scale y:              ALT+[Key_Up, Key_Down] 
//	//scale z:              ALT+SHIFT[Key_Down, Key_Up] 
//	//zoom:                 ALT+CTRL+[Key_Down, Key_Up]
//	//shifting along x:     CTRL+[Key_Right, Key_Left] 
//	//shifting along z:     CTRL+[Key_Down, Key_Up]
//


#endif			// GUI_DISPLAY_WIDGETS_3D_PLOT_WIDGET_H
