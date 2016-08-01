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
#ifndef GUI_DISPLAY_WIDGETS_3D_PLOT_WIDGET_H
#define GUI_DISPLAY_WIDGETS_3D_PLOT_WIDGET_H

#include <qtimer.h>
#if !defined (GL_EXT_geometry_shader4)
#define GL_EXT_geometry_shader4
#endif

#include <qwt3d_global.h>
#if (QWT3D_MINOR_VERSION > 2)
#include <qwt3d_curve.h>
//typedef Qwt3D::Curve BratSurfacePlot;
#else
#include <qwt3d_surfaceplot.h>
typedef Qwt3D::SurfacePlot SurfacePlot; // VC6/moc issue
typedef Qwt3D::SurfacePlot BratSurfacePlot;
#endif



class CDisplayFilesProcessor;
class CZFXYPlot;
class CZFXYPlotData;

struct CZFXYPlotParameters;

struct CBrat3DFunction;
class CBrat3DPlot;
class QLookupTable;



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


class Plot : public BratSurfacePlot
{
	Q_OBJECT

public:
    Plot( QWidget* pw, int updateinterval = 0 );

public slots:
	void rotate();
};



////////////////////////////////////////////////////////////
//	
////////////////////////////////////////////////////////////


class CBrat3DPlot : public BratSurfacePlot
{
	Q_OBJECT

	// types

	using base_t = BratSurfacePlot;


	//static data

    static const std::vector< Qwt3D::AXIS > smXaxis;
	static const std::vector< Qwt3D::AXIS > smYaxis;
	static const std::vector< Qwt3D::AXIS > smZaxis;


	//instance data

	std::vector<CBrat3DFunction*> mFunctions;		//multiple functions are really not supported so far

	QActionGroup *mStyleGroup = nullptr;
    QActionGroup *mCoordinateStyleGroup = nullptr;
	QAction *mAnimateAction = nullptr;
    QAction *mStandardColorAction = nullptr;
    QAction *mHomeAction = nullptr;

	int mXDigits = 0;		//-1 means date
	int mYDigits = 0;		//-1 means date
	int mZDigits = 0;		//-1 means date

	std::string mXlabel;
	std::string mYlabel;
	std::string mZlabel;

	QTimer mTimer;

	//construction / destruction

	void CreateContextMenu();

public:
	CBrat3DPlot( QWidget *pw );

	virtual ~CBrat3DPlot();


	void SetTitle( const QString &title );


	// axis

	//...labels

	void AxisTitles( std::string &xtitle, std::string &ytitle, std::string &ztitle ) const
	{
		xtitle = AxisTitle( Qwt3D::X1 );
		ytitle = AxisTitle( Qwt3D::Y1 );
		ztitle = AxisTitle( Qwt3D::Z1 );		
	}
	bool SetXYAxisTitles( const std::string &xtitle, const std::string &ytitle )
	{
		return
			SetAxisTitle( Qwt3D::X1, xtitle, mXlabel ) &&
			SetAxisTitle( Qwt3D::Y1, ytitle, mYlabel );
	}
	bool SetZAxisTitle( const std::string &ztitle )
	{
		return 
			SetAxisTitle( Qwt3D::Z1, ztitle, mZlabel );
	}

	//...ticks

    unsigned int XAxisNbTicks() const;
    void SetXAxisTicks( unsigned int nticks );

    unsigned int YAxisNbTicks() const;
    void SetYAxisTicks( unsigned int nticks );

    unsigned int ZAxisNbTicks() const;
    void SetZAxisTicks( unsigned int nticks );


	//...digits / date
	//
	//		- once assigned as date, number of ticks cannot be re-assigned
	//		- if isdate, "digits" is ignored and -1 is assigned internally
	//		- if not isdate and digits <= 0, the default value is used

	int XDigits() const { return mXDigits; }
	int YDigits() const { return mYDigits; }
	int ZDigits() const { return mZDigits; }

	bool XisDateTime() const { return mXDigits == -1; }
	bool YisDateTime() const { return mYDigits == -1; }
	bool ZisDateTime() const { return mZDigits == -1; }

    void SetXDigits( bool isdate, int digits, brathl_refDate date_ref = REF19500101 );
    void SetYDigits( bool isdate, int digits, brathl_refDate date_ref = REF19500101 );
    void SetZDigits( bool isdate, int digits, brathl_refDate date_ref = REF19500101 );

protected:
    void SetDigits( int &this_digits, const std::vector< Qwt3D::AXIS > &axis_ids, bool islog, bool isdate, int digits, brathl_refDate date_ref = REF19500101 );
	
public:

	//...scale

	void Scale( double &xVal, double &yVal, double &zVal );
	void SetScale( double xVal, double yVal, double zVal );

	bool LogarithmicScaleZ() const;
	void SetLogarithmicScaleZ( bool log );


	void FunctionRanges( double &xMin, double &xMax, double &yMin, double &yMax, double &zMin, double &zMax ) const;


	// data

	void AddSurface( const CZFXYPlotParameters &values );


	// style

	void SetPlotStyle( Qwt3D::PLOTSTYLE style = Qwt3D::FILLEDMESH );

	void SetCoordinateStyle( Qwt3D::COORDSTYLE style = Qwt3D::FRAME );


	// color

	void SetColorMap( QLookupTable *pcolor_map );


	// protected

protected:
	const std::string& AxisTitle( Qwt3D::AXIS axis ) const;
	bool SetAxisTitle( Qwt3D::AXIS axis, const std::string &title, std::string &data_memeber );

public slots:

	void Reset();

protected slots:

	void rotate();

	void Animate( int updateinterval = 200 );
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


	//statics

public:

	static const std::string smFontName;
	static const int smAxisFontSize = 8;
	static const int smTitleFontSize = 10;


protected:
	static void Save2All( CBrat3DPlot *p, const QString &path );

	// ok
	static bool Save2ps( CBrat3DPlot *p, const QString &path );

	// unsuccessful
	static bool Save2unsupported( CBrat3DPlot *p, const QString &path, const QString &format );
	static bool Save2gif( CBrat3DPlot *p, const QString &path );
	static bool Save2svg( CBrat3DPlot *p, const QString &path );

protected:

	// instance data

	QStackedLayout *mStackedLayout = nullptr;

	std::vector< BratSurfacePlot* > mSurfacePlots;
	CBrat3DPlot *mCurrentPlot = nullptr;

	QSize m_SizeHint;

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


	//...create plot

	//takes ownership of pcolor_map
	void PushPlot( const CZFXYPlotParameters &values, Qwt3D::Color *pcolor_map );


	// access/assignment

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//NOTE: 
	//	- Axis accessors with "All" token in name apply to all plots in internal plots stack
	//
	//	- Otherwise, accessors that receive no index parameter apply to current plot of internal 
	//		plots stack
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////


	//switch plot

	void SetCurrentPlot( int index );


	////////////////////
	//...title
	////////////////////

	void SetPlotTitle( const std::string &title );


	////////////////////
	//color map: takes ownership of pcolor_map
	////////////////////

	void SetColorMap( QLookupTable *pcolor_map );


	////////////////////
	//style
	////////////////////

    bool HasMesh() const;
    void ShowMesh( bool show );

    bool HasSolidColor() const;
    void ShowSolidColor( bool show );

    void ShowMesh( int index, bool show );
    void ShowSolidColor( int index, bool show );


	////////////////////
	//axis
	////////////////////

	//... axis titles
	void CurrentAxisTitles( std::string &xtitle, std::string &ytitle, std::string &ztitle ) const;

	bool SetAllXYAxisTitles( const std::string &xtitle, const std::string &ytitle );
	bool SetZAxisTitle( size_t index, const std::string &ztitle );


	//... axis logarithmic
	bool AxisZLogarithmicScale() const;
	void SetAxisZLogarithmicScale( bool log );

protected:
	//void SetLogarithmicScale( bool log );	//not really supported

public:

	//...axis  ticks

    unsigned int XAxisTicks() const;
    void SetXAxisTicks( unsigned int nbticks );

    unsigned int YAxisTicks() const;
    void SetYAxisTicks( unsigned int nbticks );

    unsigned int ZAxisTicks() const;
    void SetZAxisTicks( unsigned int nbticks );


	//...axis digits / date

	int XAxisDigits() const;
	int YAxisDigits() const;
	int ZAxisDigits() const;

    bool XAxisIsDateTime() const;
    bool YAxisIsDateTime() const;
    bool ZAxisIsDateTime() const;

    void SetXAxisDigits( bool isdate, unsigned int  digits, brathl_refDate date_ref = REF19500101 );
    void SetYAxisDigits( bool isdate, unsigned int  digits, brathl_refDate date_ref = REF19500101 );
    void SetZAxisDigits( bool isdate, unsigned int  digits, brathl_refDate date_ref = REF19500101 );


	////////////////////
	//...scale
	////////////////////

	void Scale( double &xVal, double &yVal, double &zVal );
	void SetScale( double xVal, double yVal, double zVal );

protected:
	void CorrectScale();

public:

	////////////////////
	//...persistence
	//		- see also unsupported format tests in static section
	////////////////////

	bool Save2Image( const QString &path, const QString &format, const QString &extension );


	////////////////////
	//interaction
	////////////////////

	void Home();


	QSize sizeHint() const override;

	///////////////////////
	// protected operations
	///////////////////////

protected:

	void AddWidget( QWidget *w );

	void SetStyle( bool show_contour, bool show_mesh );

signals:
	void ScaleChanged( double xVal, double yVal, double zVal );

protected slots:

	void HandleScaleChanged( double xVal, double yVal, double zVal );

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
