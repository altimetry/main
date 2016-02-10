/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkXYPlotActor.cxx
  Language:  C++
  Date:      -
  Version:   S&T

    *** THIS FILE WAS MODIFIED  ***
    *** THIS FILE WAS MODIFIED BY SCIENCE & TECHNOLOGY BV ***

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkXYPlotActor.h"

#include "vtkActor2DCollection.h"
#include "vtkAppendPolyData.h"
#include "PlotData/vtkNewAxisActor2D.h"
#include "vtkDataObjectCollection.h"
#include "vtkDataSetCollection.h"
#include "vtkFloatArray.h"
#include "vtkGlyph2D.h"
#include "vtkGlyphSource2D.h"
#include "vtkIntArray.h"
#include "vtkLegendBoxActor.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPlane.h"
#include "vtkPlanes.h"
#include "vtkPlotData.h"
#include "vtkPlotDataCollection.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkViewport.h"


#include "PlotData/vtkPointLocatorBrat.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

vtkStandardNewMacro(vtkXYPlotActor);

vtkCxxSetObjectMacro(vtkXYPlotActor,TitleTextProperty,vtkTextProperty);
vtkCxxSetObjectMacro(vtkXYPlotActor,AxisLabelTextProperty,vtkTextProperty);
vtkCxxSetObjectMacro(vtkXYPlotActor,AxisTitleTextProperty,vtkTextProperty);

//----------------------------------------------------------------------------
// Instantiate object
vtkXYPlotActor::vtkXYPlotActor()
{
  this->PositionCoordinate->SetCoordinateSystemToNormalizedViewport();
  this->PositionCoordinate->SetValue(0.015, 0.025);
  this->Position2Coordinate->SetCoordinateSystemToNormalizedViewport();
  this->Position2Coordinate->SetReferenceCoordinate(this->PositionCoordinate);
  this->Position2Coordinate->SetValue(0.97, 0.95);

  this->Title = NULL;
  this->XTitle = new char[7];
  sprintf(this->XTitle, "%s", "X Axis");
  this->YTitle = new char[7];
  sprintf(this->YTitle, "%s", "Y Axis");

  this->NumberOfXLabels = 6;
  this->NumberOfYLabels = 6;

  this->BaseNumberOfXLabels = this->NumberOfXLabels;
  this->BaseNumberOfYLabels = this->NumberOfYLabels;
  this->ComputedNumberOfXLabels = -1;
  this->ComputedNumberOfYLabels = -1;

  this->TitleTextProperty = vtkTextProperty::New();
  this->TitleTextProperty->SetBold(1);
  this->TitleTextProperty->SetItalic(1);
  this->TitleTextProperty->SetShadow(1);
  this->TitleTextProperty->SetFontFamilyToArial();
  this->TitleTextProperty->SetColor(0, 0, 0);

  this->AxisLabelTextProperty = vtkTextProperty::New();
  this->AxisLabelTextProperty->ShallowCopy(this->TitleTextProperty);

  this->AxisTitleTextProperty = vtkTextProperty::New();
  this->AxisTitleTextProperty->ShallowCopy(this->AxisLabelTextProperty);

  this->LabelXFormat = new char[10]; 
  sprintf(this->LabelXFormat, "%s", "%-#.4g");
  this->LabelYFormat = new char[10]; 
  sprintf(this->LabelYFormat, "%s", "%-#.4g");

  this->LogX = 0;
  this->LogY = 0;
  this->BaseX = 10;
  this->BaseY = 10;
  this->MinLogValue = 1/VTK_LARGE_FLOAT;
  
  this->XRange[0] = 1.0;
  this->XRange[1] = 0.0;
  this->YRange[0] = 1.0;
  this->YRange[1] = 0.0;

  // The Base and Computed Range are initialy invalid and
  // will be set to valid values at the first Render
  this->BaseXRange[0] = 1.0;
  this->BaseXRange[1] = 0.0;
  this->BaseYRange[0] = 1.0;
  this->BaseYRange[1] = 0.0;

  this->ComputedXRange[0] = 1.0;
  this->ComputedXRange[1] = 0.0;
  this->ComputedYRange[0] = 1.0;
  this->ComputedYRange[1] = 0.0;

  this->TitleMapper = vtkTextMapper::New();
  this->TitleMapper->GetTextProperty()->ShallowCopy(this->TitleTextProperty);
  this->TitleActor = vtkActor2D::New();
  this->TitleActor->SetMapper(this->TitleMapper);
  this->TitleActor->GetPositionCoordinate()->SetCoordinateSystemToViewport();
  
  this->XAxis = vtkNewAxisActor2D::New();
  this->XAxis->GetLabelTextProperty()->ShallowCopy(this->AxisLabelTextProperty);
  this->XAxis->GetTitleTextProperty()->ShallowCopy(this->AxisTitleTextProperty);
  this->XAxis->GetPositionCoordinate()->SetCoordinateSystemToViewport();
  this->XAxis->GetPosition2Coordinate()->SetCoordinateSystemToViewport();
  this->XAxis->SetProperty(this->GetProperty());
  //this->XAxis->DebugOn();

  this->YAxis = vtkNewAxisActor2D::New();
  this->YAxis->GetLabelTextProperty()->ShallowCopy(this->AxisLabelTextProperty);
  this->YAxis->GetTitleTextProperty()->ShallowCopy(this->AxisTitleTextProperty);
  this->YAxis->GetPositionCoordinate()->SetCoordinateSystemToViewport();
  this->YAxis->GetPosition2Coordinate()->SetCoordinateSystemToViewport();
  this->YAxis->SetProperty(this->GetProperty());
  //this->YAxis->DebugOn();
  
  this->PlotData = vtkPlotDataCollection::New();
  this->PlotActors = vtkActor2DCollection::New();

  this->Legend = 0;
  this->LegendActor = vtkLegendBoxActor::New();
  this->LegendActor->GetPositionCoordinate()->
    SetCoordinateSystemToNormalizedViewport();
  this->LegendActor->GetPositionCoordinate()->
    SetReferenceCoordinate(this->PositionCoordinate);
  this->LegendActor->GetPositionCoordinate()->SetValue(0.75, 0.65);
  this->LegendActor->GetPosition2Coordinate()->
    SetCoordinateSystemToNormalizedViewport();
  this->LegendActor->GetPosition2Coordinate()->
    SetReferenceCoordinate(this->LegendActor->GetPositionCoordinate());
  this->LegendActor->GetPosition2Coordinate()->SetValue(0.15, 0.20);
  this->LegendActor->BorderOn();

  // TODO: Allocate number of entries for Legend Actor

  vtkGlyphSource2D* glyphSource = vtkGlyphSource2D::New();
  glyphSource->SetGlyphTypeToNone();
  glyphSource->DashOn();
  this->DefaultLegendSymbol = glyphSource->GetOutput();
  this->DefaultLegendSymbol->Register(this);
  glyphSource->Delete();

  // The Inner PlotBounds are initialy invalid and
  // will be set to valid values at the first Render
  this->InnerPlotBounds[0] = 1.0;
  this->InnerPlotBounds[1] = 0.0;
  this->InnerPlotBounds[2] = 1.0;
  this->InnerPlotBounds[3] = 0.0;

  this->OuterPlotBounds[0] = 1.0;
  this->OuterPlotBounds[1] = 0.0;
  this->OuterPlotBounds[2] = 1.0;
  this->OuterPlotBounds[3] = 0.0;

  this->CachedViewportSize[0] = 0;
  this->CachedViewportSize[1] = 0;
}

//----------------------------------------------------------------------------
vtkXYPlotActor::~vtkXYPlotActor()
{
  this->XAxis->Delete();
  this->YAxis->Delete();
  
  this->PlotData->Delete();
  this->PlotActors->Delete();
  this->LegendActor->Delete();

  this->TitleMapper->Delete();
  this->TitleActor->Delete();

  if (this->Title)
    {
    delete [] this->Title;
    }
  
  if (this->XTitle)
    {
    delete [] this->XTitle;
    }
  
  if (this->YTitle)
    {
    delete [] this->YTitle;
    }
  
  if (this->LabelXFormat) 
    {
    delete [] this->LabelXFormat;
    }

  if (this->LabelYFormat) 
    {
    delete [] this->LabelYFormat;
    }

  if (this->DefaultLegendSymbol)
    {
    this->DefaultLegendSymbol->Delete();
    }

  this->SetTitleTextProperty(NULL);
  this->SetAxisLabelTextProperty(NULL);
  this->SetAxisTitleTextProperty(NULL);
} 

//----------------------------------------------------------------------------
// Add a data object to the std::list of data to plot.
void vtkXYPlotActor::AddInput(vtkPlotData *plotData, vtkProperty2D* property)
{
  if (plotData == NULL)
    {
    vtkErrorMacro(<<"Trying to add an empty object");
    return;
    }

  // Check if plotdata is not already in the std::list
  if (this->PlotData->IsItemPresent(plotData) == 0)
    {
    this->PlotData->AddItem(plotData);
    vtkPolyDataMapper2D* plotMapper = vtkPolyDataMapper2D::New();
    plotMapper->SetInput(plotData->GetOutput());
    vtkActor2D* plotActor = vtkActor2D::New();
    plotActor->SetMapper(plotMapper);
    plotActor->PickableOn();
    plotMapper->Delete();
    if (property!=0)
      {
      plotActor->SetProperty(property);
      }
    this->PlotActors->AddItem(plotActor);
    plotActor->Delete();
    this->Modified();
    }
}

//----------------------------------------------------------------------------
// Remove a data object from the std::list of data to plot.
void vtkXYPlotActor::RemoveInput(vtkPlotData *plotData)
{
  int location = this->PlotData->IsItemPresent(plotData);
  if (location!=0)
    {
    this->PlotData->RemoveItem(location);
    this->PlotActors->RemoveItem(location);
    this->Modified();
    }
}

//----------------------------------------------------------------------------
vtkPlotData* vtkXYPlotActor::GetPlotDataFromActor(vtkActor2D* plotActor)
{
  vtkPlotData* plotData = NULL;
  int location = this->PlotActors->IsItemPresent(plotActor);
  if (location!=0)
    {
    plotData = this->PlotData->GetItem(location);
    }
  return plotData;
}

//----------------------------------------------------------------------------
void vtkXYPlotActor::SetMinLogValue(double value)
{
  if (value > 0 && value < VTK_LARGE_FLOAT)
    {
    this->MinLogValue = value;
    }
}

//----------------------------------------------------------------------------
void vtkXYPlotActor::SetLogX(int logX)
{
  vtkDebugMacro(<< this->GetClassName() << " (" << this << "): "
    << "setting LogX to " << logX);
  if (this->LogX != logX)
    {
    if (logX == 1)
      {
      // Check whether the range does not contain values <= 0
      if (this->ComputedXRange[0] > this->ComputedXRange[1])
        {
        // Computed range is not yet calculated -> use Range
        if (this->XRange[0] <= 0)
          {
          return;
          }
        }
      else
        {
        if (this->ComputedXRange[0] <= 0)
          {
          return;
          }
        }
      } 
    this->LogX = logX;
    this->Modified();
    // reset number of computed labels
    this->ComputedNumberOfXLabels = -1;
    }
}

//----------------------------------------------------------------------------
void vtkXYPlotActor::SetLogY(int logY)
{
  vtkDebugMacro(<< this->GetClassName() << " (" << this << "): "
    << "setting LogY to " << logY);
  if (this->LogY != logY)
    {
    if (logY == 1)
      {
      // Check whether the range does not contain values <= 0
      if (this->ComputedYRange[0] > this->ComputedYRange[1])
        {
        // Computed range is not yet calculated -> use Range
        if (this->YRange[0] <= 0)
          {
          return;
          }
        }
      else
        {
        if (this->ComputedYRange[0] <= 0)
          {
          return;
          }
        }
      } 
    this->LogY = logY;
    this->Modified();
    // reset number of computed labels
    this->ComputedNumberOfYLabels = -1;
    }
}

//----------------------------------------------------------------------------
// Plot scalar data for each input dataset.
int vtkXYPlotActor::RenderOverlay(vtkViewport *viewport)
{
  int renderedSomething = 0;

  vtkActor2D *plotActor;
  this->PlotActors->InitTraversal();
  while ((plotActor=this->PlotActors->GetNextItem()) != 0)
    {
    renderedSomething += plotActor->RenderOverlay(viewport);
    }

  renderedSomething += this->XAxis->RenderOverlay(viewport);
  renderedSomething += this->YAxis->RenderOverlay(viewport);

  if ( this->Title != 0 )
    {
    renderedSomething += this->TitleActor->RenderOverlay(viewport);
    }

  if ( this->Legend )
    {
    renderedSomething += this->LegendActor->RenderOverlay(viewport);
    }

  return renderedSomething;
}

//----------------------------------------------------------------------------
// Plot scalar data for each input dataset.
int vtkXYPlotActor::RenderOpaqueGeometry(vtkViewport *viewport)
{
  unsigned long plotDataMTime = 0;
  vtkPlotData *plotData;
  vtkActor2D *plotActor;
  int renderedSomething = 0;

  if (this->PlotData->GetNumberOfItems()>0)
    {
    vtkDebugMacro(<<"Plotting input data");
    }

  // Update plotdata and calculate modification time
  this->PlotData->InitTraversal();
  while ((plotData=this->PlotData->GetNextItem()) != 0)
    {
    plotData->Update();
    unsigned long mTime = plotData->GetMTime();
    if (mTime > plotDataMTime)
      {
      plotDataMTime = mTime;
      }
    }

  if (this->Title && !this->TitleTextProperty)
    {
    vtkErrorMacro(<< "Need a title text property to render plot title");
    return 0;
    }

  // Check modification time to see whether we have to rebuild.
  // Pay attention that GetMTime() has been redefined (see below)

  int *viewportSize=viewport->GetSize();
  if (plotDataMTime > this->BuildTime || this->GetMTime() > this->BuildTime ||
      viewportSize[0] != this->CachedViewportSize[0] ||
      viewportSize[1] != this->CachedViewportSize[1] ||
      (this->TitleTextProperty->GetMTime() > this->BuildTime) ||
      (this->AxisLabelTextProperty->GetMTime() > this->BuildTime) ||
      (this->AxisTitleTextProperty->GetMTime() > this->BuildTime))
    {
    double xrange[2], yrange[2];
    int stringSize[2];

    vtkDebugMacro(<<"Rebuilding plot");
    this->CachedViewportSize[0] = viewportSize[0];
    this->CachedViewportSize[1] = viewportSize[1];

    // manage legend
    vtkDebugMacro(<<"Rebuilding legend");
    if (this->Legend)
      {
      this->LegendActor->SetNumberOfEntries(this->PlotData->GetNumberOfItems());
      this->PlotData->InitTraversal();
      this->PlotActors->InitTraversal();
      for (int i=0; i<this->PlotData->GetNumberOfItems(); i++)
        {
        plotData = this->PlotData->GetNextItem();
        plotActor = this->PlotActors->GetNextItem();
        if (plotData->GetPlotPoints())
          {
          this->LegendActor->SetEntrySymbol(i, plotData->GetPlotSymbol());
          }
        else
          {
          this->LegendActor->SetEntrySymbol(i, DefaultLegendSymbol);
          }
        this->LegendActor->SetEntryString(i, plotData->GetPlotLabel());
        this->LegendActor->SetEntryColor(i, plotActor->GetProperty()->GetColor());
        }
      this->LegendActor->SetPadding(2);
      this->LegendActor->GetProperty()->DeepCopy(this->GetProperty());
      this->LegendActor->ScalarVisibilityOff();
      }

    // Rebuid text props
    // Perform shallow copy here since each individual axis can be
    // accessed through the class API (i.e. each individual axis text prop
    // can be changed). Therefore, we can not just assign pointers otherwise
    // each individual axis text prop would point to the same text prop.

    if (this->AxisLabelTextProperty->GetMTime() > this->BuildTime)
      {
      if (this->XAxis->GetLabelTextProperty())
        {
        this->XAxis->GetLabelTextProperty()->ShallowCopy(
          this->AxisLabelTextProperty);
        }
      if (this->YAxis->GetLabelTextProperty())
        {
        this->YAxis->GetLabelTextProperty()->ShallowCopy(
          this->AxisLabelTextProperty);
        }
      }
    
    if ( this->AxisTitleTextProperty->GetMTime() > this->BuildTime)
      {
      if (this->XAxis->GetTitleTextProperty())
        {
        this->XAxis->GetTitleTextProperty()->ShallowCopy(
          this->AxisTitleTextProperty);
        }
      if (this->YAxis->GetTitleTextProperty())
        {
        this->YAxis->GetTitleTextProperty()->ShallowCopy(
          this->AxisTitleTextProperty);
        }
      }

    // setup x-axis
    vtkDebugMacro(<<"Rebuilding x-axis");

    this->XAxis->SetTitle(this->XTitle);
    this->XAxis->SetLabelFormat(this->LabelXFormat);
    this->XAxis->SetBase(this->BaseX);
    this->XAxis->AdjustRangeOff();
    this->XAxis->SetProperty(this->GetProperty());

    if (this->BaseNumberOfXLabels != this->NumberOfXLabels)
      {
      this->BaseNumberOfXLabels = this->NumberOfXLabels;
      this->ComputedNumberOfXLabels = -1;
      }

    CalculateBaseXRange(xrange);

    if (xrange[0] != this->BaseXRange[0] || xrange[1] != this->BaseXRange[1])
      {
      this->BaseXRange[0] = xrange[0];
      this->BaseXRange[1] = xrange[1];
      // The base range has changed, so reset the zoom and pan range
      this->SetComputedXRange(xrange, 1);
      }
    
    if (this->ComputedNumberOfXLabels == -1)
      {
      this->XAxis->AdjustTicksOn();
      this->XAxis->SetNumberOfLabels(this->BaseNumberOfXLabels);
      }
    else
      {
      this->XAxis->AdjustTicksOff();
      this->XAxis->SetNumberOfLabels(this->ComputedNumberOfXLabels);
      }
    this->XAxis->SetRange(this->ComputedXRange[0], this->ComputedXRange[1]);
    this->XAxis->SetLog(this->LogX);

    // setup y-axis
    vtkDebugMacro(<<"Rebuilding y-axis");

    this->YAxis->SetTitle(this->YTitle);
    this->YAxis->SetLabelFormat(this->LabelYFormat);
    this->YAxis->SetBase(this->BaseY);
    this->YAxis->AdjustRangeOff();
    this->YAxis->SetNumberOfLabels(this->NumberOfYLabels);
    this->YAxis->SetProperty(this->GetProperty());

    if (this->BaseNumberOfYLabels != this->NumberOfYLabels)
      {
      this->BaseNumberOfYLabels = this->NumberOfYLabels;
      this->ComputedNumberOfYLabels = -1;
      }

    CalculateBaseYRange(yrange);

    if (yrange[0] != this->BaseYRange[0] || yrange[1] != this->BaseYRange[1])
      {
      this->BaseYRange[0] = yrange[0];
      this->BaseYRange[1] = yrange[1];
      // The base range has changed, so reset the zoom and pan range
      this->SetComputedYRange(yrange, 1);
      }
    
    if (this->ComputedNumberOfYLabels == -1)
      {
      this->YAxis->AdjustTicksOn();
      this->YAxis->SetNumberOfLabels(this->BaseNumberOfYLabels);
      }
    else
      {
      this->YAxis->AdjustTicksOff();
      this->YAxis->SetNumberOfLabels(this->ComputedNumberOfYLabels);
      }
    this->YAxis->SetRange(this->ComputedYRange[1], this->ComputedYRange[0]);
    this->YAxis->SetLog(this->LogY);

    this->PlaceAxes(viewport); // Also sets the inner plot bounds

    // manage title
    if (this->TitleTextProperty->GetMTime() > this->BuildTime)
      {
      this->TitleMapper->GetTextProperty()->ShallowCopy(
        this->TitleTextProperty);
      }
    if (this->Title)
      {
      this->TitleMapper->SetInput(this->Title);

      vtkNewAxisActor2D::SetFontSize(viewport,
                                  this->TitleMapper,
                                  viewportSize,
                                  this->TitleMapper->GetNumberOfLines(),
                                  stringSize);

      this->TitleActor->GetPositionCoordinate()->SetValue(
        this->InnerPlotBounds[0] + 0.5 *
        (this->InnerPlotBounds[1] - this->InnerPlotBounds[0]) -
        0.5 * stringSize[0], this->InnerPlotBounds[3] + 0.5 * stringSize[1]);

      this->TitleActor->SetProperty(this->GetProperty());
      }

    // Update bounds and logarithmic axes setting in plot data
    this->PlotData->InitTraversal();
    while ((plotData=this->PlotData->GetNextItem()) != 0)
      {
      plotData->SetViewportBounds(this->InnerPlotBounds[0],
        this->InnerPlotBounds[1], this->InnerPlotBounds[2],
        this->InnerPlotBounds[3], 0, 0);
      plotData->SetXRange(this->ComputedXRange);
      plotData->SetYRange(this->ComputedYRange);
      plotData->SetLogX(this->LogX && this->ComputedXRange[0] > 0);
      plotData->SetLogY(this->LogY && this->ComputedYRange[0] > 0);
      }
    
    this->BuildTime.Modified();
    }//if need to rebuild the plot


  vtkDebugMacro(<<"Rendering Plot Actors");
  this->PlotActors->InitTraversal();
  while ((plotActor=this->PlotActors->GetNextItem()) != 0)
    {
    renderedSomething += plotActor->RenderOpaqueGeometry(viewport);
    }

  vtkDebugMacro(<<"Rendering Axes");
  renderedSomething += this->XAxis->RenderOpaqueGeometry(viewport);
  renderedSomething += this->YAxis->RenderOpaqueGeometry(viewport);

  if (this->Title)
    {
    vtkDebugMacro(<<"Rendering Title Actor");
    renderedSomething += this->TitleActor->RenderOpaqueGeometry(viewport);
    }

  if (this->Legend)
    {
    vtkDebugMacro(<<"Rendering Legend Actor");
    renderedSomething += this->LegendActor->RenderOpaqueGeometry(viewport);
    }

  return renderedSomething;
}

//-----------------------------------------------------------------------------
// Description:
// Does this prop have some translucent polygonal geometry?
int vtkXYPlotActor::HasTranslucentPolygonalGeometry()
{
    return 0;
}

//----------------------------------------------------------------------------
// Release any graphics resources that are being consumed by this actor.
// The parameter window could be used to determine which graphic
// resources to release.
void vtkXYPlotActor::ReleaseGraphicsResources(vtkWindow *win)
{
  vtkActor2D *plotActor;
  this->PlotActors->InitTraversal();
  while ((plotActor=this->PlotActors->GetNextItem()) != 0)
    {
    plotActor->ReleaseGraphicsResources(win);
    }
  this->XAxis->ReleaseGraphicsResources(win);
  this->YAxis->ReleaseGraphicsResources(win);
  this->TitleActor->ReleaseGraphicsResources(win);
  this->LegendActor->ReleaseGraphicsResources(win);
}

//----------------------------------------------------------------------------
unsigned long vtkXYPlotActor::GetMTime()
{
  unsigned long mtime, mtime2;
  mtime = this->vtkActor2D::GetMTime();

  if (this->Legend)
    {
    mtime2 = this->LegendActor->GetMTime();
    if (mtime2 > mtime)
      {
      mtime = mtime2;
      }
    }

  return mtime;
}

//----------------------------------------------------------------------------
void vtkXYPlotActor::PrintSelf(std::ostream& os, vtkIndent indent)
{
  vtkActor2D::PrintSelf(os, indent);

  os << indent << "Input PlotData Objects:" << std::endl;
  this->PlotData->PrintSelf(os, indent.GetNextIndent());
  
  if (this->TitleTextProperty)
    {
    os << indent << "Title Text Property:\n";
    this->TitleTextProperty->PrintSelf(os,indent.GetNextIndent());
    }
  else
    {
    os << indent << "Title Text Property: (none)\n";
    }

  if (this->AxisTitleTextProperty)
    {
    os << indent << "Axis Title Text Property:\n";
    this->AxisTitleTextProperty->PrintSelf(os,indent.GetNextIndent());
    }
  else
    {
    os << indent << "Axis Title Text Property: (none)\n";
    }

  if (this->AxisLabelTextProperty)
    {
    os << indent << "Axis Label Text Property:\n";
    this->AxisLabelTextProperty->PrintSelf(os,indent.GetNextIndent());
    }
  else
    {
    os << indent << "Axis Label Text Property: (none)\n";
    }

  os << indent << "InnerPlotBounds: ("
     << this->InnerPlotBounds[0] << ", " 
     << this->InnerPlotBounds[1] << ", " 
     << this->InnerPlotBounds[2] << ", " 
     << this->InnerPlotBounds[3] << ")" << std::endl;

  os << indent << "Title: " << (this->Title ? this->Title : "(none)") << std::endl;

  os << indent << "X Title: " 
     << (this->XTitle ? this->XTitle : "(none)") << std::endl;
  os << indent << "Y Title: " 
     << (this->YTitle ? this->YTitle : "(none)") << std::endl;
 
  os << indent << "X Range: ";
  if ( this->XRange[0] > this->XRange[1] )
    {
    os << indent << "(Automatically Computed)" << std::endl;
    }
  else
    {
    os << "(" << this->XRange[0] << ", " << this->XRange[1] << ")" << std::endl;
    }

  os << indent << "Y Range: ";
  if ( this->XRange[0] > this->YRange[1] )
    os << indent << "(Automatically Computed)" << std::endl;
  else
    os << "(" << this->YRange[0] << ", " << this->YRange[1] << ")" << std::endl;

  os << indent << "Logarithmic X Axis: " << (this->LogX ? "On" : "Off") << std::endl;
  os << indent << "Logarithmic Y Axis: " << (this->LogY ? "On" : "Off") << std::endl;

  os << indent << "Base for X Axis: " << this->BaseX << std::endl;
  os << indent << "Base for Y Axis: " << this->BaseY << std::endl;

  os << indent << "Minimum Logarithmic Value: " << this->MinLogValue << std::endl;

  os << indent << "Number Of X Labels: " << this->NumberOfXLabels << std::endl;
  os << indent << "Number Of Y Labels: " << this->NumberOfYLabels << std::endl;

  os << indent << "Label X Format: " << this->LabelXFormat << std::endl;
  os << indent << "Label Y Format: " << this->LabelYFormat << std::endl;

  os << indent << "Legend: " << (this->Legend ? "On" : "Off") << std::endl;
  LegendActor->Print(os);
}

//----------------------------------------------------------------------------
// Position the axes taking into account the expected padding due to labels
// and titles. We want the result to fit in the box specified. This method
// knows something about how the vtkAxisActor2D functions, so it may have 
// to change if that class changes dramatically.
//
void vtkXYPlotActor::PlaceAxes(vtkViewport *viewport)
{
  int titleSize[2], titleSizeX[2], titleSizeY[2], labelSizeX[2], labelSizeY[2];
  double labelFactorX, labelFactorY;
  double fontFactorX, fontFactorY;
  double tickOffsetX, tickOffsetY;
  double tickLengthX, tickLengthY;
  int* absolutePositionCoordinate;
  int* absolutePosition2Coordinate;
  char str1[512], str2[512];

  fontFactorY = this->YAxis->GetFontFactor();
  fontFactorX = this->XAxis->GetFontFactor();

  labelFactorY = this->YAxis->GetLabelFactor();
  labelFactorX = this->XAxis->GetLabelFactor();

  // Create a dummy text mapper for getting font sizes

  vtkTextMapper *textMapper = vtkTextMapper::New();
  vtkTextProperty *tprop = textMapper->GetTextProperty();

  // Estimate the padding around the X and Y axes

  if (this->Title != NULL)
    {
    if (this->TitleTextProperty)
      {
      tprop->ShallowCopy(this->TitleTextProperty);
      }
    textMapper->SetInput(this->Title);
    vtkNewAxisActor2D::SetFontSize(viewport,
                                   textMapper,
                                   this->CachedViewportSize,
                                   textMapper->GetNumberOfLines(),
                                   titleSize);
    }
  else
    {
    titleSize[0] = 0;
    titleSize[1] = 0;
    }

  tprop->ShallowCopy(this->XAxis->GetTitleTextProperty());
  textMapper->SetInput(this->XAxis->GetTitle());
  vtkNewAxisActor2D::SetFontSize(viewport,
                                 textMapper,
                                 this->CachedViewportSize,
                                 fontFactorX * textMapper->GetNumberOfLines(),
                                 titleSizeX);

  tprop->ShallowCopy(this->YAxis->GetTitleTextProperty());
  textMapper->SetInput(this->YAxis->GetTitle());
  vtkNewAxisActor2D::SetFontSize(viewport,
                                 textMapper,
                                 this->CachedViewportSize,
                                 fontFactorY * textMapper->GetNumberOfLines(),
                                 titleSizeY);

  // At this point the thing to do would be to actually ask the Y axis
  // actor to return the largest label.
  // In the meantime, let's try with the min and max

  sprintf(str1, this->YAxis->GetLabelFormat(),
          this->YAxis->GetTickRange()[0]);
  sprintf(str2, this->YAxis->GetLabelFormat(),
          this->YAxis->GetTickRange()[1]);
  tprop->ShallowCopy(this->YAxis->GetLabelTextProperty());
  textMapper->SetInput(strlen(str1) > strlen(str2) ? str1 : str2);
  vtkNewAxisActor2D::SetFontSize(viewport,
                              textMapper,
	                      this->CachedViewportSize,
                              labelFactorY * fontFactorY,
                              labelSizeY);

  // We only care for the height of the label in the X axis, so let's
  // use the min

  sprintf(str1, this->XAxis->GetLabelFormat(),
          this->XAxis->GetTickRange()[0]);
  tprop->ShallowCopy(this->XAxis->GetLabelTextProperty());
  textMapper->SetInput(str1);
  vtkNewAxisActor2D::SetFontSize(viewport,
                              textMapper,
                              this->CachedViewportSize,
                              labelFactorX * fontFactorX,
                              labelSizeX);

  // Get tick sizes

  tickOffsetX = this->XAxis->GetTickOffset();
  tickOffsetY = this->YAxis->GetTickOffset();
  tickLengthX = this->XAxis->GetTickLength();
  tickLengthY = this->YAxis->GetTickLength();

  // Estimate the size of the axes
  // These calculations are based on the exact same calculations that
  // are used in vtkAxisActor2D. So if vtkAxisActor2D changes its
  // behavior the calculations below may need to be refined.

  absolutePositionCoordinate =
    this->PositionCoordinate->GetComputedViewportValue(viewport);
  absolutePosition2Coordinate =
    this->Position2Coordinate->GetComputedViewportValue(viewport);
  // left X value
  this->InnerPlotBounds[0] = absolutePositionCoordinate[0] +
    titleSizeY[0] + tickOffsetY + tickLengthY + 1.2 * labelSizeY[0];
  // right X value
  this->InnerPlotBounds[1] = absolutePosition2Coordinate[0] -
    labelSizeX[0] / 2;
  // lower Y value
  this->InnerPlotBounds[2] = absolutePositionCoordinate[1] +
    titleSizeX[1] + tickOffsetX + tickLengthX + 1.2 * labelSizeX[1];
  // upper Y value
  this->InnerPlotBounds[3] = absolutePosition2Coordinate[1] -
    1.5 * titleSize[1];

  // Save the boundaries of the plot in viewport coordinates

  this->OuterPlotBounds[0] = absolutePositionCoordinate[0];
  this->OuterPlotBounds[1] = absolutePosition2Coordinate[0];
  this->OuterPlotBounds[2] = absolutePositionCoordinate[1];
  this->OuterPlotBounds[3] = absolutePosition2Coordinate[1];

  // Now specify the location of the axes

  this->XAxis->GetPositionCoordinate()->SetValue(
    this->InnerPlotBounds[0], this->InnerPlotBounds[2]);
  this->XAxis->GetPosition2Coordinate()->SetValue(
    this->InnerPlotBounds[1], this->InnerPlotBounds[2]);
  this->YAxis->GetPositionCoordinate()->SetValue(
    this->InnerPlotBounds[0], this->InnerPlotBounds[3]);
  this->YAxis->GetPosition2Coordinate()->SetValue(
    this->InnerPlotBounds[0], this->InnerPlotBounds[2]);

  textMapper->Delete();
}

void vtkXYPlotActor::CalculateBaseXRange(double xrange[2])
{
  // Retrieve data range
  if (this->XRange[0] <= this->XRange[1])
    {
    xrange[0] = this->XRange[0];
    xrange[1] = this->XRange[1];
    }
  else
    {
    vtkPlotData *plotData;
    xrange[0] = VTK_LARGE_FLOAT;
    xrange[1] = -VTK_LARGE_FLOAT;
    this->PlotData->InitTraversal();
    while ((plotData=this->PlotData->GetNextItem()) != 0)
      {
      double plotDataRange[2];
      plotData->ComputeXRange(plotDataRange);
      if (plotDataRange[0]<=plotDataRange[1])
        {
        if (plotDataRange[0] < xrange[0])
          {
          xrange[0] = plotDataRange[0];
          }
        if (plotDataRange[1] > xrange[1])
          {
          xrange[1] = plotDataRange[1];
          }
        }
      }

    if (xrange[0]>xrange[1])
      {
      xrange[0] = 0;
      xrange[1] = 0;
      }
    }
}

void vtkXYPlotActor::CalculateBaseYRange(double yrange[2])
{
  // Retrieve data range
  if (this->YRange[0] <= this->YRange[1])
    {
    yrange[0] = this->YRange[0];
    yrange[1] = this->YRange[1];
    }
  else
    {
    vtkPlotData *plotData;
    yrange[0] = VTK_LARGE_FLOAT;
    yrange[1] = -VTK_LARGE_FLOAT;
    this->PlotData->InitTraversal();
    while ((plotData=this->PlotData->GetNextItem()) != 0)
      {
      double plotDataRange[2];
      plotData->ComputeYRange(plotDataRange);
      if (plotDataRange[0]<=plotDataRange[1])
        {
        if (plotDataRange[0] < yrange[0])
          {
          yrange[0] = plotDataRange[0];
          }
        if (plotDataRange[1] > yrange[1])
          {
          yrange[1] = plotDataRange[1];
          }
        }
      }

    if (yrange[0]>yrange[1])
      {
      yrange[0] = 0;
      yrange[1] = 0;
      }
    }
}

void vtkXYPlotActor::AdjustXRangeAndLabels()
{
  double newRange[2];
  double interval;
  int numTicks;
  if (this->ComputedXRange[0] > this->ComputedXRange[1])
    {
    // Not rendered yet, so calculate and use base range
    double xrange[2];
    CalculateBaseXRange(xrange);
    vtkNewAxisActor2D::ComputeRange(xrange, newRange,
      this->NumberOfXLabels, this->BaseX, this->LogX, numTicks, interval);
    }
  else
    {
    vtkNewAxisActor2D::ComputeRange(this->ComputedXRange, newRange,
      this->NumberOfXLabels, this->BaseX, this->LogX, numTicks, interval);
    }
  this->SetXRange(newRange);
  this->SetNumberOfXLabels(numTicks);
}

void vtkXYPlotActor::AdjustYRangeAndLabels()
{
  double newRange[2];
  double interval;
  int numTicks;
  if (this->ComputedYRange[0] > this->ComputedYRange[1])
    {
    // Not rendered yet, so calculate and use base range
    double yrange[2];
    CalculateBaseYRange(yrange);
    vtkNewAxisActor2D::ComputeRange(yrange, newRange,
      this->NumberOfYLabels, this->BaseY, this->LogY, numTicks, interval);
    }
  else
    {
    vtkNewAxisActor2D::ComputeRange(this->ComputedYRange, newRange,
      this->NumberOfYLabels, this->BaseY, this->LogY, numTicks, interval);
    }
  this->SetYRange(newRange);
  this->SetNumberOfYLabels(numTicks);
}

void vtkXYPlotActor::SetComputedXRange(double range[2],
                                       int resetNumberOfLabels)
{
  vtkDebugMacro(<<"Settting ComputedXRange to (" << range[0] << ", " << range[1] << ")");
  this->ComputedXRange[0] = range[0];
  this->ComputedXRange[1] = range[1];
  if (resetNumberOfLabels)
    {
    this->ComputedNumberOfXLabels = -1;
    }
  if (this->LogX && this->ComputedXRange[0] <= 0)
    {
    this->LogX = 0;
    }
  this->InvokeEvent("UserEvent", (void *)"ComputedXRangeEvent");
}

void vtkXYPlotActor::SetComputedYRange(double range[2],
                                       int resetNumberOfLabels)
{
  vtkDebugMacro(<<"Settting ComputedYRange to (" << range[0] << ", " << range[1] << ")");
  this->ComputedYRange[0] = range[0];
  this->ComputedYRange[1] = range[1];
  if (resetNumberOfLabels)
    {
    this->ComputedNumberOfYLabels = -1;
    }
  if (this->LogY && this->ComputedYRange[0] <= 0)
    {
    this->LogY = 0;
    }
  this->InvokeEvent("UserEvent", (void *)"ComputedYRangeEvent");
}

void vtkXYPlotActor::ZoomToXRange(double range[2])
{
  if (range[0] <= range[1] && range[0] > -VTK_LARGE_FLOAT &&
      range[1] < VTK_LARGE_FLOAT)
    {
    this->SetComputedXRange(range, 1);
    this->Modified();
    }
}

void vtkXYPlotActor::ZoomToYRange(double range[2])
{
  if (range[0] <= range[1] && range[0] > -VTK_LARGE_FLOAT &&
      range[1] < VTK_LARGE_FLOAT)
    {
    this->SetComputedYRange(range, 1);
    this->Modified();
    }
}

void vtkXYPlotActor::ZoomToOuterXRange()
{
  double newRange[2];
  double interval;
  vtkNewAxisActor2D::ComputeRange(this->ComputedXRange, newRange,
    this->NumberOfXLabels, this->BaseX, this->LogX,
    this->ComputedNumberOfXLabels, interval);
  this->SetComputedXRange(newRange, 0);
  this->Modified();
}

void vtkXYPlotActor::ZoomToOuterYRange()
{
  double newRange[2];
  double interval;
  vtkNewAxisActor2D::ComputeRange(this->ComputedYRange, newRange,
    this->NumberOfYLabels, this->BaseY, this->LogY,
    this->ComputedNumberOfYLabels, interval);
  this->SetComputedYRange(newRange, 0);
  this->Modified();
}

void vtkXYPlotActor::ZoomToInnerXRange()
{
  double newRange[2];
  double interval;
  vtkNewAxisActor2D::ComputeInnerRange(this->ComputedXRange, newRange,
    this->NumberOfXLabels, this->BaseX, this->LogX,
    this->ComputedNumberOfXLabels, interval);
  this->SetComputedXRange(newRange, 0);
  this->Modified();
}

void vtkXYPlotActor::ZoomToInnerYRange()
{
  double newRange[2];
  double interval;
  vtkNewAxisActor2D::ComputeInnerRange(this->ComputedYRange, newRange,
    this->NumberOfYLabels, this->BaseY, this->LogY,
    this->ComputedNumberOfYLabels, interval);
  this->SetComputedYRange(newRange, 0);
  this->Modified();
}

void vtkXYPlotActor::ZoomInAtXValue(double x,
                                    double zoomFactor)
{
  double newRange[2];
  if (this->LogX && x > 0 && this->ComputedXRange[0] > 0)
    {
    newRange[0] = exp(log(x) - (log(x) - log(this->ComputedXRange[0])) /
      zoomFactor);
    newRange[1] = exp(log(x) + (log(this->ComputedXRange[1]) - log(x)) /
      zoomFactor);
    this->SetComputedXRange(newRange, 1);
    this->Modified();
    }
  else
    {
    newRange[0] = this->ComputedXRange[0] + (x - this->ComputedXRange[0]) *
                  (zoomFactor - 1) / zoomFactor;
    newRange[1] = newRange[0] + (this->ComputedXRange[1] -
                  this->ComputedXRange[0]) / zoomFactor;
    this->SetComputedXRange(newRange, 1);
    this->Modified();
    }
}

void vtkXYPlotActor::ZoomInAtYValue(double y,
                                    double zoomFactor)
{
  double newRange[2];
  if (this->LogY && y > 0 && this->ComputedYRange[0] > 0)
    {
    newRange[0] = exp(log(y) - (log(y) - log(this->ComputedYRange[0])) /
      zoomFactor);
    newRange[1] = exp(log(y) + (log(this->ComputedYRange[1]) - log(y)) /
      zoomFactor);
    this->SetComputedYRange(newRange, 1);
    this->Modified();
    }
    else
    {
    newRange[0] = this->ComputedYRange[0] + (y - this->ComputedYRange[0]) *
                  (zoomFactor - 1) / zoomFactor;
    newRange[1] = newRange[0] + (this->ComputedYRange[1] -
                  this->ComputedYRange[0]) / zoomFactor;
    this->SetComputedYRange(newRange, 1);
    this->Modified();
    }
}

void vtkXYPlotActor::PanXRange(double panFactor)
{
  double newRange[2];
  if (this->LogX && this->ComputedXRange[0] > 0)
    {
    newRange[0] = exp(log(this->ComputedXRange[0]) - panFactor *
      (log(this->ComputedXRange[1]) - log(this->ComputedXRange[0])));
    newRange[1] = exp(log(this->ComputedXRange[1]) - panFactor *
      (log(this->ComputedXRange[1]) - log(this->ComputedXRange[0])));
    this->SetComputedXRange(newRange, 1);
    this->Modified();
    }
  else
    {
    newRange[0] = this->ComputedXRange[0] - panFactor *
                  (this->ComputedXRange[1] - this->ComputedXRange[0]);
    newRange[1] = this->ComputedXRange[1] - panFactor *
                  (this->ComputedXRange[1] - this->ComputedXRange[0]);
    this->SetComputedXRange(newRange, 1);
    this->Modified();
    }
}

void vtkXYPlotActor::PanYRange(double panFactor)
{
  double newRange[2];
  if (this->LogY && this->ComputedYRange[0] > 0)
    {
    newRange[0] = exp(log(this->ComputedYRange[0]) - panFactor *
      (log(this->ComputedYRange[1]) - log(this->ComputedYRange[0])));
    newRange[1] = exp(log(this->ComputedYRange[1]) - panFactor *
      (log(this->ComputedYRange[1]) - log(this->ComputedYRange[0])));
    this->SetComputedYRange(newRange, 1);
    this->Modified();
    }
  else
    {
    newRange[0] = this->ComputedYRange[0] - panFactor *
                  (this->ComputedYRange[1] - this->ComputedYRange[0]);
    newRange[1] = this->ComputedYRange[1] - panFactor *
                  (this->ComputedYRange[1] - this->ComputedYRange[0]);
    this->SetComputedYRange(newRange, 1);
    this->Modified();
    }
}

int vtkXYPlotActor::IsInPlot(double x, double y)
{
  return (x>=this->InnerPlotBounds[0] &&
          x<=this->InnerPlotBounds[1] &&
          y>=this->InnerPlotBounds[2] &&
          y<=this->InnerPlotBounds[3]);
}

int vtkXYPlotActor::IsXAxis(double x, double y)
{
  vtkDebugMacro(<<"XAxis check " << this->InnerPlotBounds[0] << ", "
    << this->InnerPlotBounds[1] << ", " << this->OuterPlotBounds[2] << ", "
    << this->InnerPlotBounds[2]);
  return (x>=this->InnerPlotBounds[0] && x<=this->InnerPlotBounds[1] &&
          y>=this->OuterPlotBounds[2] && y<=this->InnerPlotBounds[2]);
}

int vtkXYPlotActor::IsYAxis(double x, double y)
{
  vtkDebugMacro(<<"YAxis check " << this->OuterPlotBounds[1] << ", "
    << this->InnerPlotBounds[0] << ", " << this->InnerPlotBounds[2] << ", "
    << this->InnerPlotBounds[3]);
  return (x>=this->OuterPlotBounds[0] && x<=this->InnerPlotBounds[0] &&
          y>=this->InnerPlotBounds[2] && y<=this->InnerPlotBounds[3]);
}



int vtkXYPlotActor::FindPoint(vtkPlotData* plotData, double x, double y, double z) 
{

  double xyz[3];
  xyz[0] = x; xyz[1] = y; xyz[2] = z;

  vtkPointSet* pointSet = plotData->GetOutput();

    if ( !pointSet->GetPoints() )
    {
    return -1;
    }
  
  vtkPointLocatorBrat *locator = vtkPointLocatorBrat::New();
  locator->Register(pointSet);
  locator->Delete();
  locator->SetDataSet(pointSet);

  if ( pointSet->GetPoints()->GetMTime() > locator->GetMTime() )
    {
    locator->SetDataSet(pointSet);
    }

  return locator->FindClosestPoint(xyz);


}


vtkPlotData* vtkXYPlotActor::FindPlotData(double x,
                                          double y)
{
  double minDistance = VTK_LARGE_FLOAT;
  vtkPlotData* nearestPlotData = 0;
  vtkPlotData* plotData;

  this->PlotData->InitTraversal();
  while((plotData=this->PlotData->GetNextItem())!=0)
    {
    double distance;
    vtkFloatingPointType* xyz;
    vtkIdType point;
    plotData->Update();
    // There is a bug in vtkPointLocator 
    //=================================
    // in BuildLocator method which can potentially access a negative index into the Hashtable array 
    // (buckets) if there are points in the dataset which fall outside the 
    // bounds of the dataset. 
    // So don't use plotData->GetOutput()->FindPoint(x, y, 0), but use FindPoint(plotData, x, y, 0);
    //point = plotData->GetOutput()->FindPoint(x, y, 0);
    point = FindPoint(plotData, x, y, 0);

    if (point>=0)
      {
      xyz = plotData->GetOutput()->GetPoint(point);
      distance = (x-xyz[0])*(x-xyz[0]) + (y-xyz[1])*(y-xyz[1]);
      if (distance<minDistance)
        {
        minDistance = distance;
        nearestPlotData = plotData;
        }
      }
    }
  return nearestPlotData;
}

static void PrintStringOnOneLine(FILE *f, const char *str)
{
  if (str != NULL)
    {
    const char *c = str;
    while (*c != '\0')
      {
      if (*c == '\n')
        {
        fprintf(f, " ");
        }
      else
        {
        fprintf(f, "%c", *c);
        }
      c++;
      }
    }
}

int vtkXYPlotActor::ExportToGnuplotFile(const char* filename)
{
  FILE *file;
  int n;

  file = fopen(filename, "w");
  if (file == NULL)
    {
    return -1;
    }

  fprintf(file, "set encoding iso_8859_1;\n");
  fprintf(file, "set xrange [%f:%f];\n", this->ComputedXRange[0],
          this->ComputedXRange[1]);
  fprintf(file, "set yrange [%f:%f];\n", this->ComputedYRange[0],
          this->ComputedYRange[1]);
  if (this->XTitle)
    {
    fprintf(file, "set xlabel \"");
    PrintStringOnOneLine(file, this->XTitle);
    fprintf(file, "\";\n");
    }
  if (this->YTitle)
    {
    fprintf(file, "set ylabel \"");
    PrintStringOnOneLine(file, this->YTitle);
    fprintf(file, "\";\n");
    }
  if (this->Title)
    {
    fprintf(file, "set title \"");
    PrintStringOnOneLine(file, this->Title);
    fprintf(file, "\";\n");
    }
  n = this->PlotData->GetNumberOfItems();
  if (n > 0)
  {
    int i;
    vtkPlotData *plotData;

    fprintf(file, "plot");
    i = 0;
    this->PlotData->InitTraversal();
    plotData = this->PlotData->GetNextItem();
    while (plotData != 0)
      {
      if (plotData->GetPlotLines() || plotData->GetPlotPoints())
        {
        fprintf(file, " '-' using (column(1)):(column(2))");
        if (plotData->GetPlotLabel() != 0)
          {
          fprintf(file, " title '");
          PrintStringOnOneLine(file, plotData->GetPlotLabel());
          fprintf(file, "'");
          }
        else
          {
          fprintf(file, " notitle");
          }
        if (plotData->GetPlotLines())
          {
          if (plotData->GetPlotPoints())
            {
            fprintf(file, " with linespoints");
            }
          else
            {
            fprintf(file, " with lines");
            }
          }
        else
          {
          fprintf(file, " with points");
          }
        if (plotData->GetPlotLines())
          {
          fprintf(file, " linetype %i", i + 1);
          }
        if (plotData->GetPlotPoints())
          {
          fprintf(file, " pointtype %i", i + 1);
          }

        plotData = this->PlotData->GetNextItem();

        if (plotData != 0)
          {
          fprintf(file, ",");
          }
        }
      else
        {
        plotData = this->PlotData->GetNextItem();
        }
      i++;
      }
    fprintf(file, "\n");

    this->PlotData->InitTraversal();
    plotData = this->PlotData->GetNextItem();
    while (plotData != 0)
      {
      if (plotData->GetPlotLines() || plotData->GetPlotPoints())
        {
        for (i = 0; i < plotData->GetNumberOfItems(); i++)
          {
          fprintf(file, " %f %f\n",
                  plotData->GetXValue(i),
                  plotData->GetYValue(i));
          }
        fprintf(file, "e\n");
        }
      plotData = this->PlotData->GetNextItem();
      }
    }

  fclose(file);
  return 0;
}
