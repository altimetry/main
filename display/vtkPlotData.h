/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPlotData.h
  Language:  C++
  Date:      -
  Version:   S&T

    *** THIS FILE WAS MODIFIED BY Collecte Localisation Satellites (CLS) ***
    *** THIS FILE WAS CREATED BY SCIENCE & TECHNOLOGY BV ***

  Copyright (C) 2002-2005 Science & Technology BV, The Netherlands.

  This file is part of VISAN.

  VISAN is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  VISAN is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
                         
  You should have received a copy of the GNU General Public License
  along with VISAN; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

=========================================================================*/
#ifndef __vtkPlotData_h
#define __vtkPlotData_h

#include "vtkPolyDataSource.h"
#include "vtkGlyphSource2D.h"

class vtkAppendPolyData;
class vtkGlyph3D;
class vtkPoints;

// Undefine any macro called GetYValue (Windows seems to have such a macro)
#ifdef GetYValue
#undef GetYValue
#endif

class VTK_EXPORT vtkPlotData : public vtkPolyDataSource
{
public:
  vtkTypeMacro(vtkPlotData, vtkPolyDataSource);
  virtual void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual double GetXValue(int i) = 0;
  virtual double GetYValue(int i) = 0;
  virtual double GetZValue(int i) = 0;
  virtual int GetNumberOfItems() = 0;

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
  // This will compute the actual range of the underlying data.
  // It will not modify the XRange, YRange and ZRange values.
  // If there are no elements the range will be [1,0]
  // If in a derived class a specific dimension is not used the range for
  // that dimension should be set to [0,0] and the GetValue method should
  // return 0.0.
  virtual void ComputeRange(double range[2], int dim, int log = 0);
  void ComputeXRange(double range[2])
    { this->ComputeRange(range, 0); }
  void ComputeYRange(double range[2])
    { this->ComputeRange(range, 1); }
  void ComputeZRange(double range[2])
    { this->ComputeRange(range, 2); }
  // Description:
  // This will compute the range of the underlying data for values > 0.
  // This range will be used for plots with a logarithmic axis.
  // If there are no elements with a value > 0 the range will be [1,0]
  // If in a derived class a specific dimension is not used the range for
  // that dimension should be set to [1,0] (i.e. there are no elements with
  // a value > 0).
  virtual void ComputeRangeAbove0(double range[2], int dim);
  void ComputeXRangeAbove0(double range[2]) { this->ComputeRangeAbove0(range, 0); }
  void ComputeYRangeAbove0(double range[2]) { this->ComputeRangeAbove0(range, 1); }
  void ComputeZRangeAbove0(double range[2]) { this->ComputeRangeAbove0(range, 2); }

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

  vtkGlyphSource2D* GetGlyphSource();

public:


protected:
  vtkPlotData();
  ~vtkPlotData();

  virtual void Execute();

  void ComputePlotPoints(vtkPoints* pts, double dataBounds[6],
                         int viewportMappingNeeded, int dataClippingNeeded);
  void ComputePlotLines(vtkPoints* pts, double dataBounds[6],
                        int viewportMappingNeeded);
  void ComputePlotLinesWithClipping(vtkPoints* pts,
                                    double dataBounds[6],
                                    int viewportMappingNeeded);
  double ComputeGlyphScale();

protected:
  int PlotLines;
  int PlotPoints;

  int LogX;
  int LogY;
  int LogZ;

  double XRange[2];
  double YRange[2];
  double ZRange[2];

  double ViewportBounds[6];

  // The data drawn within the axes. A curve is one polydata.
  // color is controlled by scalar data. The curves are appended
  // together, possibly glyphed with point symbols.
  char* PlotLabel;
  double GlyphSize;
  vtkGlyph3D* PlotGlyph;
  vtkPolyData* PlotPointsData;
  vtkPolyData* PlotLinesData;
  //vtkPolyData* PlotPolysData;
  vtkAppendPolyData* PlotAppend;

  vtkGlyphSource2D* GlyphSource;

private:
  vtkPlotData(const vtkPlotData&);  // Not implemented.
  void operator=(const vtkPlotData&);  // Not implemented.
};


#endif
