/*
* 
*
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

#ifndef __vtkZFXYPlotActor_h
#define __vtkZFXYPlotActor_h

#include "List.h"
using namespace brathl;

#include "vtkActor2D.h"
#include "vtkPolyData.h"

#include "vtkZFXYPlotFilter.h"

#include "ZFXYPlotData.h"


class vtkActor2DCollection;
class vtkAppendPolyData;
class vtkNewAxisActor2D;
class vtkDataObjectCollection;
class vtkGlyph2D;
class vtkGlyphSource2D;
class vtkIntArray;
class vtkLegendBoxActor;
class vtkPlanes;
class vtkZFXYPlotFilterCollection;
class vtkZFXYPlotFilter;
class vtkPolyDataMapper2D;
class vtkTextMapper;
class vtkTextProperty;

class CZFXYPlotProperty;

class VTK_EXPORT vtkZFXYPlotActor : public vtkActor2D
{
public:
  vtkTypeMacro(vtkZFXYPlotActor,vtkActor2D);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Instantiate object with autorange computation; bold, italic, and shadows
  // on; arial font family; the number of labels set to 5 for the x and y
  // axes; a label format of "%-#.4g"; and x coordinates computed from point
  // ids.
  static vtkZFXYPlotActor *New();

  //--------------------------------------------------------------------------
  // Description:
  // Add a plotdata object to the list of plotdata.
  void AddInput(CZFXYPlotData* pdata);
  
  void AddInput(vtkZFXYPlotFilter* plotData);

  // Description:
  // Remove a data object from the list of plotdata.
  // This will automatically remove the corresponding Mapper2D and Actor2D.
  void RemoveInput(vtkZFXYPlotFilter *plotData);

  void RemoveInput(CZFXYPlotData* pdata);
  void RemoveInput();

  // Description:
  // Replace the current item by the newIndex'th item
  void ReplaceInput(uint32_t newIndex);

  // Description:
  // Retrieve the plotData that is used as data for a certain plotActor.
  // If the actor is not a plotActor or if the plotData could not be found
  // 0 will be returned.
  vtkZFXYPlotFilter* GetPlotDataFromActor(vtkActor2D* plotActor);

  void AddActor(vtkActor2D* plotActor);
  void RemoveActor(vtkActor2D* plotActor);

  //--------------------------------------------------------------------------
  // Description:
  // Get the bounds of the plot area (without axis) in viewport coordinates.
  vtkGetVector4Macro(InnerPlotBounds, double);

  //--------------------------------------------------------------------------
  // Description:
  // Set/Get the title of the x-y plot, and the title along the
  // x and y axes.
  vtkSetStringMacro(Title);
  vtkGetStringMacro(Title);
  vtkSetStringMacro(XTitle);
  vtkGetStringMacro(XTitle);
  vtkSetStringMacro(YTitle);
  vtkGetStringMacro(YTitle);

  // Description:
  // Set the plot range (range of independent and dependent variables)
  // to plot. Data outside of the range will be clipped. If the plot
  // range of either the x or y variables is set to (v1,v2), where
  // v1 > v2, then the range will be computed automatically (automatic
  // computation is the default behavior).
  vtkSetVector2Macro(XRange, float);
  vtkSetVector2Macro(XRange, double);
  vtkGetVectorMacro(XRange, double, 2);
  vtkSetVector2Macro(YRange, float);
  vtkSetVector2Macro(YRange, double);
  vtkGetVectorMacro(YRange, double, 2);
  void SetPlotRange(double xmin, double ymin, double xmax, double ymax)
    {this->SetXRange(xmin,xmax); this->SetYRange(ymin,ymax);}
  
  // Description:
  // Get the plot range before zoom and pan were applied
  // This range is calculated by taking, if they are set, the XRange
  // (or YRange) values or otherwise calculating the ranges from the datasets.
  vtkGetVectorMacro(BaseXRange, double, 2);
  vtkGetVectorMacro(BaseYRange, double, 2);
  vtkSetVector2Macro(BaseXRange, double);
  vtkSetVector2Macro(BaseYRange, double);

  // Description:
  // Get the plot range as it is actually being plot.
  // The actual range is calculated by applying zoom and pan to the base range
  // (BaseXRange and BaseYRange).
  vtkGetVectorMacro(ComputedXRange, double, 2);
  vtkGetVectorMacro(ComputedYRange, double, 2);

  // Description:
  // Enable/Disable use of a logarithmic x-axis.
  void SetLogX(int logX);
  vtkGetMacro(LogX, int);
  vtkBooleanMacro(LogX, int);

  // Description:
  // Enable/Disable use of a logarithmic y-axis.
  void SetLogY(int logY);
  vtkGetMacro(LogY, int);
  vtkBooleanMacro(LogY, int);

  // Description:
  // Set the base for the linear and logarithmic tick calculation for
  // the x-axis.
  // BaseX should be > 1.0
  vtkSetMacro(BaseX, double);
  vtkGetMacro(BaseX, double);

  // Description:
  // Set the base for the linear and logarithmic tick calculation for
  // the y-axis.
  // BaseY should be > 1.0
  vtkSetMacro(BaseY, double);
  vtkGetMacro(BaseY, double);

  // Description:
  // Set the minimal value (>0) used when using logarithmic axis.
  // All datavalues below this value will be ignored.
  // The default value for MinLogValue is 1/VTK_LARGE_FLOAT
  void SetMinLogValue(double value);
  vtkGetMacro(MinLogValue, double);

  // Description:
  // Set/Get the number of annotation labels to show along the x and y axes.
  // This values is a suggestion: the number of labels may vary depending
  // on the particulars of the data. The convenience method 
  // SetNumberOfLabels() sets the number of x and y labels to the same value.
  vtkSetClampMacro(NumberOfXLabels, int, 0, 50);
  vtkGetMacro(NumberOfXLabels, int);
  vtkSetClampMacro(NumberOfYLabels, int, 0, 50);
  vtkGetMacro(NumberOfYLabels, int);
  void SetNumberOfLabels(int num)
    {this->SetNumberOfXLabels(num); this->SetNumberOfYLabels(num);}


  // Description:
  // Set/Get the format with which to print the labels on x-axis
  vtkSetStringMacro(LabelXFormat);
  vtkGetStringMacro(LabelXFormat);

  // Description:
  // Set/Get the format with which to print the labels on y-axis
  vtkSetStringMacro(LabelYFormat);
  vtkGetStringMacro(LabelYFormat);
  
  // Description:
  // Adjust the current XRange (or YRange) and current NumberOfXLabels
  // (or NumberOfYLabels) such that we get nice labels.
  // The behaviour is similar to the ZoomToOuterXRange (or ZoomToOuterYRange)
  // method, but in this case the user definable range and numberOfLabels
  // values are modified, which means that BaseXRange (or BaseYRange) and
  // BaseNumberOfXLabels (or BaseNumberOfYLabels) will also change.
  void AdjustXRangeAndLabels();
  void AdjustYRangeAndLabels();

  // Description:
  // Set/Get the title text property.
  virtual void SetTitleTextProperty(vtkTextProperty *p);
  vtkGetObjectMacro(TitleTextProperty,vtkTextProperty);
  
  // Description:
  // Set/Get the title text property of all axes. Note that each axis can
  // be controlled individually through the GetX/YAxisActor2D() methods.
  virtual void SetAxisTitleTextProperty(vtkTextProperty *p);
  vtkGetObjectMacro(AxisTitleTextProperty,vtkTextProperty);
  
  // Description:
  // Set/Get the labels text property of all axes. Note that each axis can
  // be controlled individually through the GetX/YAxisActor2D() methods.
  virtual void SetAxisLabelTextProperty(vtkTextProperty *p);
  vtkGetObjectMacro(AxisLabelTextProperty,vtkTextProperty);

  //--------------------------------------------------------------------------
  // Description:
  // Set the zoom and pan amounts such that the new data range equals the
  // given data range.
  void ZoomToXRange(double min, double max);
  void ZoomToXRange(double range[2]);
  void ZoomToYRange(double min, double max);
  void ZoomToYRange(double range[2]);

  // Description:
  // Auto adjust and zoom the current range such that it becomes a 'nice'
  // range and encapsulates the current range
  void ZoomToOuterXRange();
  void ZoomToOuterYRange();

  // Description:
  // Auto adjust and zoom the current range such that it becomes a 'nice'
  // range and is encapsulated by the current range
  void ZoomToInnerXRange();
  void ZoomToInnerYRange();

  // Description:
  // This sets the zoom and pan amounts such that it looks as if we are
  // zooming in at the data value x (or y).
  // The zoomFactor is equal to the zoom amount if linear axis are used.
  // In case of logarithmic axis the appropriate zoom value is calculated
  // from the zoomFactor by applying the zoomfactor to the log10() of the
  // data values.
  // Zooming is done relative to the current zoomed and panned range.
  void ZoomInAtXValue(double x, double zoomFactor);
  void ZoomInAtYValue(double y, double zoomFactor);

  // Description:
  // This sets the zoom and pan amounts such that it looks as if the whole
  // data range was panned. For linear axis the panFactor is equal to the pan
  // amount and no zooming is done. In case of logarithmic axis the panning
  // is done on the log10() of the data values.
  // Panning is done relative to the current zoomed and panned range.
  void PanXRange(double panFactor);
  void PanYRange(double panFactor);

  //--------------------------------------------------------------------------
  // Description:
  // Retrieve a handle to the legend box. This is usefull if you would like
  // to change the default behavior of the legend box.
  vtkLegendBoxActor *GetLegendBoxActor() {return this->LegendActor;}

  // Description:
  // Enable/Disable the creation of a legend.
  vtkSetMacro(Legend, int);
  vtkGetMacro(Legend, int);
  vtkBooleanMacro(Legend, int);

  // Description:
  // Set/Get the symbol that will be used if PlotPoints is off but
  // PlotLines is on for a PlotData input object
  vtkSetObjectMacro(DefaultLegendSymbol, vtkPolyData);
  vtkGetObjectMacro(DefaultLegendSymbol, vtkPolyData);

  //--------------------------------------------------------------------------
  // Description:
  // Is the specified viewport position within the inner plot area?
  int IsInPlot(double x, double y);

  // Description:
  // Is the specified viewport position within the X axis bounds?
  int IsXAxis(double x, double y);

  // Description:
  // Is the specified viewport position within the Y axis bounds?
  int IsYAxis(double x, double y);

  // Description:
  // Find the closest PlotData near the viewport position (x,y).
  // A pointer to the PlotData will be returned when a PlotData object is
  // found, otherwise 0 will be returned.
  vtkZFXYPlotFilter* FindPlotData(double x, double y);
  

  int FindPoint(vtkZFXYPlotFilter* plotData, double x, double y, double z);

  //--------------------------------------------------------------------------
  // Description:
  // Take into account the modified time of internal helper classes.
  unsigned long GetMTime();
  
//BTX  
  // Description:
  // WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
  // DO NOT USE THIS METHOD OUTSIDE OF THE RENDERING PROCESS.
  // Draw the x-y plot.
  int RenderOpaqueGeometry(vtkViewport*);
  int RenderOverlay(vtkViewport*);
  int RenderTranslucentGeometry(vtkViewport *) {return 0;}

  // Description:
  // Release any graphics resources that are being consumed by this actor.
  // The parameter window could be used to determine which graphic
  // resources to release.
  void ReleaseGraphicsResources(vtkWindow *);
//ETX  


  void LutChanged();
  void Update();

  void RemoveActors();
  void RemoveActors(CZFXYPlotData* pdata);
  void RemoveActorsSolidColor(CZFXYPlotData* pdata);
  void RemoveActorsContour(CZFXYPlotData* pdata);
  void RemoveActorsContourLabel(CZFXYPlotData* pdata);

  void AddActors();

  void AddActorsSolidColor();
  void AddActorsContour();

  void AddActors(CZFXYPlotData* pdata);

  void AddActorsContour(CZFXYPlotData* pdata);
  void AddActorsContourLabel(CZFXYPlotData* pdata);
  void AddActorsSolidColor(CZFXYPlotData* pdata);

  void GetXPlotDataRange(double range[2],uint32_t frame);
  void GetXPlotDataRange(double& min, double& max, uint32_t frame);

  void GetXPlotDataRange(double range[2]);
  void GetXPlotDataRange(double& min, double& max);

  void GetYPlotDataRange(double range[2],uint32_t frame);
  void GetYPlotDataRange(double& min, double& max, uint32_t frame);

  void GetYPlotDataRange(double range[2]);
  void GetYPlotDataRange(double& min, double& max);



protected:
  vtkZFXYPlotActor();
  ~vtkZFXYPlotActor();

  void SetComputedXRange(double range[2],
                         int resetNumberOfLabels);
  void SetComputedYRange(double range[2],
                         int resetNumberOfLabels);

  void CalculateBaseXRange(double xrange[2]);
  void CalculateBaseYRange(double yrange[2]);

  void SetPlotProperties(CZFXYPlotProperty* props);


protected:

  vtkTimeStamp  BuildTime;

  //vtkRenderer* Renderer;

  //The data drawn within the axes. Each curve is one polydata.
  //color is controlled by scalar data. The curves are appended
  //together, possibly glyphed with point symbols.
  vtkZFXYPlotFilterCollection *PlotData;
  vtkActor2DCollection  *PlotActors;
  
  CObArray ZFXYData;

  // Inner bounds in absolute viewport coordinates
  double InnerPlotBounds[4];
  // Outer bounds in absolute viewport coordinates
  double OuterPlotBounds[4];
  int CachedViewportSize[2]; // Viewport size in abosulte coordinates

  char *Title;
  vtkTextMapper *TitleMapper;
  vtkActor2D    *TitleActor;
  vtkTextProperty *TitleTextProperty;

  char *XTitle;
  char *YTitle;
  double XRange[2]; // user defined range before pan/zoom
  double YRange[2]; // user defined range before pan/zoom
  double BaseXRange[2];  // cached range before pan/zoom
  double BaseYRange[2];  // cached range before pan/zoom
  double ComputedXRange[2];  // range actually used by plot (with pan/zoom)
  double ComputedYRange[2];  // range actually used by plot (with pan/zoom)
  int LogX;
  int LogY;
  double BaseX; // the base for the linear and logarithmic tick calculation
  double BaseY; // the base for the linear and logarithmic tick calculation
  double MinLogValue;
  int NumberOfXLabels; // user defined number of labels/ticks
  int NumberOfYLabels; // user defined number of labels/ticks
  int BaseNumberOfXLabels; // cached number of labels/ticks before pan/zoom
  int BaseNumberOfYLabels; // cached number of labels/ticks before pan/zoom
  // if number of labels should be calculated automatically the value 
  // of ComputedNumberOfX/YLabels is -1
  int ComputedNumberOfXLabels; // number of labels/ticks after inner/outer zoom
  int ComputedNumberOfYLabels; // number of lables/ticks after inner/outer zoom
  char *LabelXFormat;
  char *LabelYFormat;

  vtkNewAxisActor2D *XAxis;
  vtkNewAxisActor2D *YAxis;

  vtkTextProperty *AxisTitleTextProperty;
  vtkTextProperty *AxisLabelTextProperty;

  // Legends and plot symbols. The legend also keeps track of
  // the symbols and such.
  int Legend;
  // The PositionCoordinate of the legend is set to normalized viewport and
  // uses the PositionCoordinate of the vtkZFXYPlotActor as reference.
  // The PositionCoordinate2 of the legend is set to normalized viewport and
  // has the PositionCoordinate of the legend as reference coordinate.
  vtkLegendBoxActor *LegendActor;
  // This is the symbol that will be used if PlotPoints is off but
  // PlotLines is on for a PlotData input object
  vtkPolyData* DefaultLegendSymbol;

  void PlaceAxes(vtkViewport *viewport);
  
private:
  vtkZFXYPlotActor(const vtkZFXYPlotActor&);  // Not implemented.
  void operator=(const vtkZFXYPlotActor&);  // Not implemented.
};

#endif
