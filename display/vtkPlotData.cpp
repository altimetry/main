/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPlotData.cxx
  Language:  C++
  Date:      -
  Version:   S&T

    *** THIS FILE WAS MODIFIED  ***
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

#include "vtkTools.h"
#include "vtkPlotData.h"

#include "vtkAppendPolyData.h"
#include "vtkCell.h"
#include "vtkCellArray.h"
#include "vtkGlyph2D.h"
#include "vtkGlyphSource2D.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"


//----------------------------------------------------------------------------
// Instantiate object
vtkPlotData::vtkPlotData()
{
  this->PlotLines = 1;
  this->PlotPoints = 0;

  this->LogX = 0;
  this->LogY = 0;
  this->LogZ = 0;

  this->PlotLabel = 0;

  this->XRange[0] = 1.0;
  this->XRange[1] = 0.0;
  this->YRange[0] = 1.0;
  this->YRange[1] = 0.0;
  this->ZRange[0] = 1.0;
  this->ZRange[1] = 0.0;

  this->ViewportBounds[0] = 1.0;
  this->ViewportBounds[1] = 0.0;
  this->ViewportBounds[2] = 1.0;
  this->ViewportBounds[3] = 0.0;
  this->ViewportBounds[4] = 1.0;
  this->ViewportBounds[5] = 0.0;

  this->PlotPointsData = vtkPolyData::New();
  this->PlotLinesData = vtkPolyData::New();
  //this->PlotPolysData = vtkPolyData::New();

  this->GlyphSize = 0.01;

  this->GlyphSource = vtkGlyphSource2D::New();
  this->GlyphSource->SetGlyphTypeToCircle();
  this->GlyphSource->FilledOff();

  this->PlotGlyph = vtkGlyph3D::New();
  this->PlotGlyph->SetSource(this->GlyphSource->GetOutput());
  this->PlotGlyph->SetInput(this->PlotPointsData);
  this->PlotGlyph->SetScaleModeToDataScalingOff();

  this->PlotAppend = vtkAppendPolyData::New();
  this->PlotAppend->AddInput(this->PlotLinesData);
  //this->PlotAppend->AddInput(this->PlotPolysData);
  this->PlotAppend->AddInput(this->PlotGlyph->GetOutput());

  this->PlotGlyph->Delete();
  this->PlotPointsData->Delete();
  this->PlotLinesData->Delete();
  //this->PlotPolysData->Delete();
}

//----------------------------------------------------------------------------
vtkPlotData::~vtkPlotData()
{
  if (this->PlotLabel)
    {
    delete [] this->PlotLabel;
    }
  this->PlotAppend->Delete();

  if (this->GlyphSource!= NULL)
    {
    this->GlyphSource->Delete();
    this->GlyphSource = NULL;
    }

} 

//----------------------------------------------------------------------------
void vtkPlotData::Execute()
{
  double dataBounds[6];
  int i;
  
  this->PlotGlyph->SetSource(this->GlyphSource->GetOutput());
  
  vtkDebugMacro(<<"Updating vtkPlotData");

  if (!this->PlotPoints && !this->PlotLines)
    {
    vtkDebugMacro(<<"Nothing to plot");
    this->PlotPointsData->SetPoints(0);
    this->PlotLinesData->SetPoints(0);
    this->PlotLinesData->SetLines(0);
    this->PlotAppend->Update();
    this->GetOutput()->ShallowCopy(this->PlotAppend->GetOutput());
    return;
    }

  int viewportMappingNeeded =
    (this->ViewportBounds[0]<=this->ViewportBounds[1] ||
     this->ViewportBounds[2]<=this->ViewportBounds[3] ||
     this->ViewportBounds[4]<=this->ViewportBounds[5]);

  this->GetXRange(dataBounds);
  this->GetYRange(&(dataBounds[2]));
  this->GetZRange(&(dataBounds[4]));
  
  int dataClippingNeeded = (dataBounds[0]<=dataBounds[1] ||
                            dataBounds[2]<=dataBounds[3] ||
                            dataBounds[4]<=dataBounds[5]);

  if (viewportMappingNeeded || dataClippingNeeded)
    {
    // Make sure that 'dataBounds' corresponds with the real bounds
    // (for all dimensions that have no user-specified range)
    if (dataBounds[0]>dataBounds[1])
      {
      if (this->LogX)
        {
        this->ComputeXRangeAbove0(dataBounds);
        }
      else
        {
        this->ComputeXRange(dataBounds);
        }
      }
    if (dataBounds[2]>dataBounds[3])
      {
      if (this->LogY)
        {
        this->ComputeYRangeAbove0(&(dataBounds[2]));
        }
      else
        {
        this->ComputeYRange(&(dataBounds[2]));
        }
      }
    if (dataBounds[4]>dataBounds[5])
      {
      if (this->LogZ)
        {
        this->ComputeZRangeAbove0(&(dataBounds[4]));
        }
      else
        {
        this->ComputeZRange(&(dataBounds[4]));
        }      
      }
    }

  // Check if boundaries are valid when using logarithmic axis
  if ((this->LogX && dataBounds[0]<=0.0) ||
      (this->LogY && dataBounds[2]<=0.0) ||
      (this->LogZ && dataBounds[4]<=0.0))
    {
    vtkDebugMacro(<<"Trying to plot negative values with logarithmic axis.");
    this->PlotPointsData->SetPoints(0);
    this->PlotLinesData->SetPoints(0);
    this->PlotLinesData->SetLines(0);
    this->PlotAppend->Update();
    this->GetOutput()->ShallowCopy(this->PlotAppend->GetOutput());
    return;
    }

  vtkDebugMacro(<<"  Using databounds: (" << dataBounds[0] << ", " <<
                  dataBounds[1] << ", " << dataBounds[2] << ", " <<
                  dataBounds[3] << ", " << dataBounds[4] << ", " <<
                  dataBounds[5] << ")");

  vtkDebugMacro(<<"  Calculating Data Points");
  // The points array will intially contain all data elements.
  vtkPoints* pts = vtkPoints::New(VTK_DOUBLE);
  int numComp = this->GetNumberOfItems();
  for (i = 0; i < numComp; i++)
    {
    double xyz[3];
    xyz[0] = this->GetXValue(i);
    xyz[1] = this->GetYValue(i);
    xyz[2] = this->GetZValue(i);
    //vtkDebugMacro("  Adding point ("<<xyz[0]<<", "<<xyz[1]<<", "<<xyz[2]<<")");
    pts->InsertNextPoint(xyz);
    }

  this->ComputePlotPoints(pts, dataBounds, viewportMappingNeeded, dataClippingNeeded);
  if (dataClippingNeeded)
    {
    this->ComputePlotLinesWithClipping(pts, dataBounds, viewportMappingNeeded);
    }
  else
    {
    this->ComputePlotLines(pts, dataBounds, viewportMappingNeeded);
    }

  pts->Delete();

  // Update the PlotData, PlotGlyph and PlotAppend polydata
  this->PlotAppend->Update();

  // Update the output
  this->GetOutput()->ShallowCopy(this->PlotAppend->GetOutput());
}

//----------------------------------------------------------------------------
void vtkPlotData::ComputePlotPoints(vtkPoints* pts,
                                    double dataBounds[6],
                                    int viewportMappingNeeded,
                                    int dataClippingNeeded)
{
  this->PlotPointsData->SetPoints(0);
  if (this->PlotPoints)
    {
    // PlotPointsData contains only those points that are within the
    // X, Y, and Z ranges.
    vtkDebugMacro(<<"  Calculating PlotPoints with DataClipping");
    vtkPoints* newPts = vtkPoints::New(VTK_DOUBLE);
    int numberOfPoints = 0;
    int logXYZ[3];
    logXYZ[0] = this->LogX;
    logXYZ[1] = this->LogY;
    logXYZ[2] = this->LogZ;
    for (vtkIdType ptId=0; ptId<pts->GetNumberOfPoints(); ptId++)
      {
      double xyz[3];
      pts->GetPoint(ptId, xyz);
      if (!vtkTools::IsInvalid(xyz[0], logXYZ[0]) && !vtkTools::IsInvalid(xyz[1], logXYZ[1]) && (!dataClippingNeeded ||
          (xyz[0] >= dataBounds[0] && xyz[0] <= dataBounds[1] &&
           xyz[1] >= dataBounds[2] && xyz[1] <= dataBounds[3] &&
           xyz[2] >= dataBounds[4] && xyz[2] <= dataBounds[5])))
        {
        if (viewportMappingNeeded)
          {
          vtkTools::DataToViewport3D(xyz, this->ViewportBounds, dataBounds, logXYZ);
          }
        newPts->InsertNextPoint(xyz);
        numberOfPoints++;
        }
      }
    if (numberOfPoints>0)
      {
      this->PlotPointsData->SetPoints(newPts);
      }
    newPts->Delete();
    // Set scale of the Glyph with respect to the viewport bounds
    this->PlotGlyph->SetScaleFactor(ComputeGlyphScale());
  }
}

//----------------------------------------------------------------------------
void vtkPlotData::ComputePlotLines(vtkPoints* pts,
                                   double dataBounds[6],
                                   int viewportMappingNeeded)
{
  this->PlotLinesData->SetLines(0);
  if (this->PlotLines)
    {
    vtkDebugMacro(<<"  Calculating PlotLines without DataClipping");
    int numberOfDataPoints = pts->GetNumberOfPoints();
    if (numberOfDataPoints>0)
      {
      int logXYZ[3];
      int numberOfLines = 0;
      int numberOfPoints = 0; // Number of non-NaN points
      vtkPoints* newPts = vtkPoints::New(VTK_DOUBLE);
      vtkCellArray* lines = vtkCellArray::New(); 
      logXYZ[0] = this->LogX;
      logXYZ[1] = this->LogY;
      logXYZ[2] = this->LogZ;
      //lines->InsertNextCell(numberOfDataPoints);
      for (vtkIdType ptId=0; ptId<numberOfDataPoints; ptId++)
        {
        double xyz[3];
        pts->GetPoint(ptId, xyz);
        if (!vtkTools::IsInvalid(xyz[0], logXYZ[0]) && !vtkTools::IsInvalid(xyz[1], logXYZ[1]))
          {
          if (viewportMappingNeeded)
            {
            vtkTools::DataToViewport3D(xyz, this->ViewportBounds, dataBounds, logXYZ);
            }
          vtkIdType newPtId = newPts->InsertNextPoint(xyz);
          if (numberOfPoints == 0)
            {
            lines->InsertNextCell(0);
            }
          lines->InsertCellPoint(newPtId);
          numberOfPoints++;
          }
        else if (numberOfPoints > 0)
          {
          // finish line segment
          lines->UpdateCellCount(numberOfPoints);
          numberOfLines++;
          numberOfPoints = 0;
          }
        }

      // Finish last line segment
      if (numberOfPoints>0)
        {
        lines->UpdateCellCount(numberOfPoints);
        numberOfLines++;
        }
      if (numberOfLines > 0)
        {
        this->PlotLinesData->SetPoints(newPts);
        this->PlotLinesData->SetLines(lines);
        }
      newPts->Delete();
      lines->Delete();
      }
    }
}

//----------------------------------------------------------------------------
void vtkPlotData::ComputePlotLinesWithClipping(vtkPoints* pts,
                                            double dataBounds[6],
                                            int viewportMappingNeeded)
{
  this->PlotLinesData->SetLines(0);
  if (this->PlotLines)
    {
    vtkDebugMacro(<<"  Calculating PlotLines with DataClipping");
    // The lines array contains the line segments that results form clipping
    // the original line with the X, Y, and Z ranges. Each linesegment is
    // contained within one cell and contains both the points from the dataset
    // and possible new intersection endpoints.
    int numberOfDataPoints = pts->GetNumberOfPoints();
    if (numberOfDataPoints>0)
      {
      vtkPoints* newPts = vtkPoints::New(VTK_DOUBLE);
      vtkCellArray* lines = vtkCellArray::New(); 
      int numberOfLines = 0;
      int numberOfPoints = 0; // Number of points within a line segment
      int logXYZ[3];
      double xyz1[3];
      double xyz2[3];
      logXYZ[0] = this->LogX;
      logXYZ[1] = this->LogY;
      logXYZ[2] = this->LogZ;
      pts->GetPoint(0, xyz1);
      // NaN values are considered outside the bounds
      int isnan1 = vtkTools::IsInvalid(xyz1[0], logXYZ[0]) || vtkTools::IsInvalid(xyz1[1], logXYZ[1]);
      int withinBounds1 = (!isnan1 &&
         xyz1[0] >= dataBounds[0] && xyz1[0] <= dataBounds[1] &&
         xyz1[1] >= dataBounds[2] && xyz1[1] <= dataBounds[3] &&
         xyz1[2] >= dataBounds[4] && xyz1[2] <= dataBounds[5]);
      for (vtkIdType ptId=0; ptId<numberOfDataPoints-1; ptId++)
        {
        pts->GetPoint(ptId+1, xyz2);
        int isnan2 = vtkTools::IsInvalid(xyz2[0], logXYZ[0]) || vtkTools::IsInvalid(xyz2[1], logXYZ[1]);
        int withinBounds2 = (!isnan2 &&
          xyz2[0] >= dataBounds[0] && xyz2[0] <= dataBounds[1] &&
          xyz2[1] >= dataBounds[2] && xyz2[1] <= dataBounds[3] &&
          xyz2[2] >= dataBounds[4] && xyz2[2] <= dataBounds[5]);
        if (withinBounds1)
          {
          double xyz3[3];
          if (!withinBounds2 && !isnan2)
            {
            // calculate intersection
            vtkTools::ComputeIntersection(dataBounds, xyz2, xyz1, xyz3);
            }
          // add xyz1 to points
          if (viewportMappingNeeded)
            {
            vtkTools::DataToViewport3D(xyz1, this->ViewportBounds, dataBounds,
                                   logXYZ);
            }
          vtkIdType newPtId = newPts->InsertNextPoint(xyz1);
          // add xyz1 to lines
          if (numberOfPoints == 0)
            {
            lines->InsertNextCell(0);
            }
          lines->InsertCellPoint(newPtId);
          numberOfPoints++;
          if (!withinBounds2) // Ending line
            {
            if (!isnan2) // Leaving the boundaries
              {
              // add intersection point to points
              if (viewportMappingNeeded)
                {
                vtkTools::DataToViewport3D(xyz3, this->ViewportBounds, dataBounds,
                                      logXYZ);
                }
              newPtId = newPts->InsertNextPoint(xyz3);
              // add intersection point to lines
              lines->InsertCellPoint(newPtId);
              numberOfPoints++;
              }
            // finish line segment
            lines->UpdateCellCount(numberOfPoints);
            numberOfLines++;
            numberOfPoints = 0;
            }
          }
        else if (!isnan1)
          {
          if (withinBounds2) // Entering the boundaries
            {
            double xyz3[3];
            // calculate intersection
            vtkTools::ComputeIntersection(dataBounds, xyz1, xyz2, xyz3);
            // add intersection point to points
            if (viewportMappingNeeded)
              {
              vtkTools::DataToViewport3D(xyz3, this->ViewportBounds, dataBounds,
                                     logXYZ);
              }
            vtkIdType newPtId = newPts->InsertNextPoint(xyz3);
            // add intersection point to lines starting a new line segment
            lines->InsertNextCell(0);
            lines->InsertCellPoint(newPtId);
            numberOfPoints = 1;
            }
          else if (!isnan2) // Staying outside the boundaries
            {
            // check if linesegment doesn't intersect with boundaries
            double xyz3[3];
            if(vtkTools::ComputeIntersection(dataBounds, xyz1, xyz2, xyz3))
              {
              double xyz4[3];
              // Calculate second intersectionpoint
              vtkTools::ComputeIntersection(dataBounds, xyz2, xyz1, xyz4);
              // add first intersection point to points
              if (viewportMappingNeeded)
                {
                vtkTools::DataToViewport3D(xyz3, this->ViewportBounds, dataBounds,
                                       logXYZ);
                }
              vtkIdType newPtId = newPts->InsertNextPoint(xyz3);
              // add first intersection point to lines
              lines->InsertNextCell(2);
              lines->InsertCellPoint(newPtId);
              // Only add the second intersection point to points if the
              // first and second intersection point are not the same
              // (i.e. we don't intersect one of the axes or corner points of
              // the bounding box)
              if (viewportMappingNeeded)
                {
                vtkTools::DataToViewport3D(xyz4, this->ViewportBounds, dataBounds,
                                       logXYZ);
                }
              if (xyz3[0]!=xyz4[0] || xyz3[1]!=xyz4[1] || xyz3[2]!=xyz4[2])
                {
                newPtId = newPts->InsertNextPoint(xyz4);
                }
              // add second intersection point to lines
              lines->InsertCellPoint(newPtId);
              // go to next line
              numberOfLines++;
              numberOfPoints = 0;
              }
            }
          }
        xyz1[0] = xyz2[0];
        xyz1[1] = xyz2[1];
        xyz1[2] = xyz2[2];
        withinBounds1 = withinBounds2;
        isnan1 = isnan2;
        }
      if (withinBounds1)
        {
        // Add last point to points
        if (viewportMappingNeeded)
          {
          vtkTools::DataToViewport3D(xyz1, this->ViewportBounds, dataBounds,
                                 logXYZ);
          }
        vtkIdType newPtId = newPts->InsertNextPoint(xyz1);
        // Add last point to lines
        if (numberOfPoints == 0)
          {
          lines->InsertNextCell(0);
          }
        lines->InsertCellPoint(newPtId);
        numberOfPoints++;
        }
      // Finish last line segment
      if (numberOfPoints>0)
        {
        lines->UpdateCellCount(numberOfPoints);
        numberOfLines++;
        }
      if (numberOfLines>0)
        {
        this->PlotLinesData->SetPoints(newPts);
        this->PlotLinesData->SetLines(lines);
        }
      newPts->Delete();
      lines->Delete();
      }
    }
}
  
//----------------------------------------------------------------------------
void vtkPlotData::PrintSelf(std::ostream& os, vtkIndent indent)
{
  os << indent << "Number of Items: " << this->GetNumberOfItems() << std::endl;
  os << indent << "X Range: (" << this->XRange[0] << ", " <<
    this->XRange[1] << ")" << std::endl;
  os << indent << "Y Range: (" << this->YRange[0] << ", " <<
    this->YRange[1] << ")" << std::endl;
  os << indent << "Z Range: (" << this->ZRange[0] << ", " <<
    this->ZRange[1] << ")" << std::endl;
  os << indent << "Log X: " << (this->LogX ? "On" : "Off") << std::endl;
  os << indent << "Log Y: " << (this->LogY ? "On" : "Off") << std::endl;
  os << indent << "Log Z: " << (this->LogZ ? "On" : "Off") << std::endl;
  os << indent << "Viewport Bounds: (" << this->ViewportBounds[0] << ", " <<
    this->ViewportBounds[1] << ", " << this->ViewportBounds[2] << ", " <<
    this->ViewportBounds[3] << ", " << this->ViewportBounds[4] << ", " <<
    this->ViewportBounds[5] << ")" << std::endl;
  os << indent << "Plot Points: " << (this->PlotPoints ? "On" : "Off") << std::endl;
  os << indent << "Plot Lines: " << (this->PlotLines ? "On" : "Off") << std::endl;
  os << indent << "Glyph Size: " << this->GlyphSize << std::endl;
}

//----------------------------------------------------------------------------
void vtkPlotData::ComputeRangeAbove0(double range[2], int dim)
{
  this->ComputeRange(range, dim, 1);
}

//----------------------------------------------------------------------------
void vtkPlotData::ComputeRange(double range[2], int dim, int log /* = 0*/)
{
  int numComp = this->GetNumberOfItems();

  range[0] = VTK_DOUBLE_MAX;
  range[1] = (log ? 0 : VTK_DOUBLE_MIN);

  for (int i=0; i<numComp; i++)
    {
    double x;
    switch(dim)
      {
      case 0:
        x = this->GetXValue(i);
        break;
      case 1:
        x = this->GetYValue(i);
        break;
      case 2:
        x = this->GetZValue(i);
        break;
      default:
        x = 0.0;
      }

    if (!vtkTools::IsInvalid(x, log))
      {
      if (x < range[0])
        {
        range[0] = x;
        }
      if (x > range[1])
        {
        range[1] = x;
        }
      }
    } //over all data objects


  if ( range[0] > range[1] )
    {
    range[0] = 1;
    range[1] = 0;
    }

  vtkDebugMacro(<<"ComputeRange (dim=" << dim << ", log=" << log << "): "
                "range = (" << range[0] << "," << range[1] << ")");
}

//----------------------------------------------------------------------------
void vtkPlotData::SetPlotSymbol(vtkPolyData *input)
{
  if (input != this->PlotGlyph->GetSource())
    {
    this->PlotGlyph->SetSource(input);
    this->Modified();
    }
}

//----------------------------------------------------------------------------
vtkPolyData *vtkPlotData::GetPlotSymbol()
{
  return this->PlotGlyph->GetSource();
}
//----------------------------------------------------------------------------
vtkGlyphSource2D* vtkPlotData::GetGlyphSource()
{
  return this->GlyphSource;
}
//----------------------------------------------------------------------------
double vtkPlotData::ComputeGlyphScale()
{
  vtkPolyData *pd = this->PlotGlyph->GetSource();
  pd->Update();
  return this->GlyphSize * sqrt((double)
        (this->ViewportBounds[1]-this->ViewportBounds[0])*
        (this->ViewportBounds[1]-this->ViewportBounds[0]) +
        (this->ViewportBounds[3]-this->ViewportBounds[2])*
        (this->ViewportBounds[3]-this->ViewportBounds[2]) +
        (this->ViewportBounds[5]-this->ViewportBounds[4])*
        (this->ViewportBounds[5]-this->ViewportBounds[4])) / 
        pd->GetLength();
}

