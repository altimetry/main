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
#ifndef __vtkZFXYPlotFilter_h
#define __vtkZFXYPlotFilter_h

#include "vtkPolyDataSource.h"
#include "vtkGlyphSource2D.h"
#include "vtkGlyph3D.h"
#include "vtkAppendPolyData.h"

#include "brathl.h"

class vtkCellArray;
class vtkDoubleArray;
class vtkPoints;
class vtkPolyData;
class vtkShortArray;


class VTK_EXPORT vtkZFXYPlotFilter : public vtkPolyDataSource
{
public:
  vtkTypeMacro(vtkZFXYPlotFilter,vtkPolyDataSource);
  void PrintSelf(ostream& os, vtkIndent indent);
  static vtkZFXYPlotFilter *New();

  virtual void SetValues(vtkDoubleArray*);
  vtkGetObjectMacro(Values,vtkDoubleArray);

  virtual void SetZDataArray(vtkDoubleArray*);
  vtkGetObjectMacro(ZDataArray,vtkDoubleArray);

  virtual void SetBitarray(vtkShortArray*);
  vtkGetObjectMacro(Bitarray,vtkShortArray);
 
  
  virtual void SetXDataArray(vtkDoubleArray*);
  vtkGetObjectMacro(XDataArray,vtkDoubleArray);

  virtual void SetYDataArray(vtkDoubleArray*);
  vtkGetObjectMacro(YDataArray,vtkDoubleArray);

  vtkSetMacro(OffsetY,double);
  vtkGetMacro(OffsetY,double);

  vtkSetMacro(OffsetX,double);
  vtkGetMacro(OffsetX,double);

  vtkSetMacro(MinMappedValue,double);
  vtkGetMacro(MinMappedValue,double);

  vtkSetMacro(MaxMappedValue,double);
  vtkGetMacro(MaxMappedValue,double);

  vtkSetMacro(PlotWidth,int);
  vtkGetMacro(PlotWidth,int);

  vtkSetMacro(PlotHeight,int);  
  vtkGetMacro(PlotHeight,int);  

  // Description:
  // Set these ranges to clip the data in the X, Y and/or Z dimension.
  // If for a dimension range[0]>range[1] then the range is ignored and
  // no clipping is performed in that dimension.
  // For a 2D plot the z dimension is ignored.
  vtkSetVector2Macro(XRange, float);
  vtkSetVector2Macro(XRange, double);
  vtkGetVector2Macro(XRange, double);
  vtkSetVector2Macro(YRange, float);
  vtkSetVector2Macro(YRange, double);
  vtkGetVector2Macro(YRange, double);
  vtkSetVector2Macro(ZRange, float);
  vtkSetVector2Macro(ZRange, double);
  vtkGetVector2Macro(ZRange, double);


  vtkSetVector2Macro(XDataRange, float);
  vtkSetVector2Macro(XDataRange, double);
  vtkGetVector2Macro(XDataRange, double);
  vtkSetVector2Macro(YDataRange, float);
  vtkSetVector2Macro(YDataRange, double);
  vtkGetVector2Macro(YDataRange, double);
  vtkSetVector2Macro(ZDataRange, float);
  vtkSetVector2Macro(ZDataRange, double);
  vtkGetVector2Macro(ZDataRange, double);

  // Description:
  // Set/Get the geometry bounding box in viewport coordinats in the form
  // (xmin, xmax, ymin, ymax, zmin, zmax). The datavalues will be linearly
  // mapped from (range[0], range[1]) to (min, max) for each dimension.
  // If for a dimension min>max then for that dimension the data values
  // are used as viewport coordinates and no mapping is performed.
  // For a 2D plot the z dimension is ignored.
  vtkSetVector6Macro(ViewportBounds, float);
  vtkSetVector6Macro(ViewportBounds, double);
  vtkGetVector6Macro(ViewportBounds, double);
  
  // Description:
  // This will compute the actual range of the underlying data.
  // It will not modify the XRange, YRange and ZRange values.
  // If there are no elements the range will be [1,0]
  // If in a derived class a specific dimension is not used the range for
  // that dimension should be set to [0,0] and the GetValue method should
  // return 0.0.

  virtual void ComputeRange(double range[2], int dim);
  void ComputeXRange(double range[2])
    { this->ComputeRange(range, 0); };
  void ComputeYRange(double range[2])
    { this->ComputeRange(range, 1); };
  void ComputeZRange(double range[2])
    { this->ComputeRange(range, 2); };

  virtual double GetValueX(int i);
  virtual double GetValueY(int i);
  virtual double GetValueZ(int i);
  virtual int GetNumberOfItems(int dim);


    // Description:
  // Set the label of the data. This label is used within legends
  vtkSetStringMacro(PlotLabel);
  vtkGetStringMacro(PlotLabel);

  // Description:
  // Set the glyph symbol that is used when plotting points
  // Use output from e.g. vtkGlyphSource2D as input
  void SetPlotSymbol(vtkPolyData *input);
  vtkPolyData *GetPlotSymbol();

  // Description:
  // Set/Get the relative size of the glyph.
  vtkSetMacro(GlyphSize, double);
  vtkGetMacro(GlyphSize, double);

  // Description:
  // Set/Get whether to plot the lines
  vtkSetMacro(PlotLines, int);
  vtkGetMacro(PlotLines, int);
  vtkBooleanMacro(PlotLines, int);

  // Description:
  // Set/Get whether to plot the points (= glyphs)
  vtkSetMacro(PlotPoints, int);
  vtkGetMacro(PlotPoints, int);
  vtkBooleanMacro(PlotPoints, int);
  
  // Description:
  // Set/Get whether to plot the polys 
  vtkSetMacro(PlotPolys, int);
  vtkGetMacro(PlotPolys, int);
  vtkBooleanMacro(PlotPolys, int);

  // Description:
  // Set/Get whether a logarithmic axis is used. When on (and
  // bounds[0]!=bounds[1]), a value will be scaled logarithmic between
  // their data boundaries as follows:
  // x_new = bounds[0] + log(x-bounds[0]+1) * (bounds[1] - bounds[0]) /
  //         log(bounds[1]-bounds[0]+1)
  // If bounds[0]==bounds[1], no conversion will be done
  vtkSetMacro(LogX, int);
  vtkGetMacro(LogX, int);
  vtkSetMacro(LogY, int);
  vtkGetMacro(LogY, int);
  vtkSetMacro(LogZ, int);
  vtkGetMacro(LogZ, int);

  vtkGlyphSource2D* GetGlyphSource();
  
  vtkSetMacro(Id, int);
  vtkGetMacro(Id, int);



protected:
  vtkZFXYPlotFilter();
  virtual ~vtkZFXYPlotFilter();

  bool IsValidPoint(int32_t index);

  void GetOffset(int32_t x, int32_t y, double& offsetX, double& offsetY);

  void Execute();


  void ComputePlotPoints(vtkPoints* pts, double dataBounds[6],
                         int viewportMappingNeeded, int dataClippingNeeded);
  void ComputePlotLines(vtkPoints* pts, double dataBounds[6],
                        int viewportMappingNeeded);
  void ComputePlotLinesWithClipping(vtkPoints* pts,
                                    double dataBounds[6],
                                    int viewportMappingNeeded);


  void ComputePlotPolysPoints(vtkPoints *points, vtkShortArray*  innerPlotPoint, double dataBounds[6], 
                              int viewportMappingNeeded, 
                              int dataClippingNeeded,
                              vtkPoints *mappedPoints);


  double ComputeGlyphScale();

protected:
  int Id;
  int PlotLines;
  int PlotPoints;
  int PlotPolys;

  int LogX;
  int LogY;
  int LogZ;


  vtkDoubleArray*       Values;
  vtkDoubleArray*       ZDataArray;
  vtkDoubleArray*       XDataArray;
  vtkDoubleArray*       YDataArray;
  vtkShortArray*        Bitarray;

  vtkShortArray*        InnerPlotPoint;

  double                OffsetY;
  double                OffsetX;
    
  double                MinMappedValue;
  double                MaxMappedValue;

  int                   PlotWidth;
  int                   PlotHeight;

  double ViewportBounds[6];
  
  double XRange[2];
  double YRange[2];
  double ZRange[2];

  double XDataRange[2];
  double YDataRange[2];
  double ZDataRange[2];


  // The data drawn within the axes. A curve is one polydata.
  // color is controlled by scalar data. The curves are appended
  // together, possibly glyphed with point symbols.
  char* PlotLabel;
  double GlyphSize;
  vtkGlyph3D* PlotGlyph;
  vtkPolyData* PlotPointsData;
  vtkPolyData* PlotLinesData;
  vtkPolyData* PlotPolysData;
  vtkAppendPolyData* PlotAppend;

  vtkGlyphSource2D* GlyphSource;

};

#endif
