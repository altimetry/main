/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInteractorStyleXYPlot.cxx
  Language:  C++
  Date:      -
  Version:   S&T

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
#include "vtkInteractorStyleXYPlot.h"

#include "PlotData/vtkTools.h"
#include "vtkActor2D.h"
#include "vtkCommand.h"
#include "vtkIndent.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkOutlineSource.h"
#include "vtkPlotData.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPropCollection.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkXYPlotActor.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif


vtkStandardNewMacro(vtkInteractorStyleXYPlot);

//----------------------------------------------------------------------------
vtkInteractorStyleXYPlot::vtkInteractorStyleXYPlot()
{
  this->OutlineActor = NULL;
  this->UseTimers = 1;
}

//----------------------------------------------------------------------------
vtkInteractorStyleXYPlot::~vtkInteractorStyleXYPlot()
{
  if (this->OutlineActor != NULL)
    {
    this->OutlineActor->Delete();
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleXYPlot::FindPokedXYPlotActor(int x, int y)
{
  vtkDebugMacro(<< "Poking vtkXYPlotActor at (" << x << ", " << y << ")");

  this->FindPokedRenderer(x, y);

  this->CurrentXYPlotActor = NULL;
  this->CurrentXYPlotElement = -1;
  if (this->CurrentRenderer)
    {
    vtkPropCollection* props;
    vtkProp* prop;
#if VTK_MAJOR_VERSION >= 5
    props = this->CurrentRenderer->GetViewProps();
#else
    props = this->CurrentRenderer->GetProps();
#endif
    props->InitTraversal();
    while ((prop = props->GetNextProp()) != NULL && CurrentXYPlotActor == NULL)
      {
      vtkXYPlotActor* plotActor = vtkXYPlotActor::SafeDownCast(prop);
      if (plotActor!=0)
        {
        // Check if current mouse position is within plotactor bounds
        int* p = plotActor->GetPositionCoordinate()->
          GetComputedViewportValue(CurrentRenderer);
        if (x >= p[0] && y >= p[1])
          {
          p = plotActor->GetPosition2Coordinate()->
            GetComputedViewportValue(CurrentRenderer);
          if (x <= p[0] && y <= p[1])
            {
            this->CurrentXYPlotActor = plotActor;
            }
          }
        }
      }
    if (this->CurrentXYPlotActor)
      {
      vtkDebugMacro(<< "Picked plotactor " << (void*)this->CurrentXYPlotActor);
      if (this->CurrentXYPlotActor->IsInPlot(x, y))
        {
        vtkDebugMacro(<< "We are in the plot area");
        this->CurrentXYPlotElement = VTKIS_XYPLOT_PLOT_AREA;
        //vtkDebugMacro(<< "Nearest PlotData " <<
        //this->CurrentXYPlotActor->FindPlotData(x, y));
        this->CurrentXYPlotActor->FindPlotData(x, y);
        }
      else if (this->CurrentXYPlotActor->IsXAxis(x, y))
        {
        vtkDebugMacro(<< "We are at the X Axis");
        this->CurrentXYPlotElement = VTKIS_XYPLOT_X_AXIS;
        }
      else if (this->CurrentXYPlotActor->IsYAxis(x, y))
        {
        vtkDebugMacro(<< "We are at the Y Axis");
        this->CurrentXYPlotElement = VTKIS_XYPLOT_Y_AXIS;
        }
      }
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleXYPlot::Pan()
{
  int x;
  int y;

  this->Interactor->GetEventPosition(x, y);

  switch (this->CurrentXYPlotElement)
    {
    case VTKIS_XYPLOT_PLOT_AREA:
      if (x != this->PrevPos[0])
        {
        this->CurrentXYPlotActor->PanXRange((x - this->PrevPos[0]) / 
          (this->ViewportBounds[1] - this->ViewportBounds[0]));
        }
      if (y != this->PrevPos[1])
        {
        this->CurrentXYPlotActor->PanYRange((y - this->PrevPos[1]) / 
          (this->ViewportBounds[3] - this->ViewportBounds[2]));
        }
      if (x != this->PrevPos[0] || y != this->PrevPos[1])
        {
        this->Interactor->Render();
        }
      break;
    case VTKIS_XYPLOT_X_AXIS:
      if (x != this->PrevPos[0])
        {
        this->CurrentXYPlotActor->PanXRange((x - this->PrevPos[0]) / 
          (this->ViewportBounds[1] - this->ViewportBounds[0]));
        this->Interactor->Render();
        }
    break;
    case VTKIS_XYPLOT_Y_AXIS:
      if (y != this->PrevPos[1])
        {
        this->CurrentXYPlotActor->PanYRange((y - this->PrevPos[1]) / 
          (this->ViewportBounds[3] - this->ViewportBounds[2]));
        this->Interactor->Render();
        }
    default:
    break;
    }
  this->PrevPos[0] = x;
  this->PrevPos[1] = y;
}

//----------------------------------------------------------------------------
void vtkInteractorStyleXYPlot::OutlineZoom()
{
  int x;
  int y;

  this->Interactor->GetEventPosition(x, y);

  switch (this->CurrentXYPlotElement)
    {
    case VTKIS_XYPLOT_PLOT_AREA:
      if (x != this->PrevPos[0] || y != this->PrevPos[1])
        {
        vtkFloatingPointType bounds[6];
        this->OutlineSource->GetBounds(bounds);
        bounds[1] = x;
        bounds[3] = y;
        this->OutlineSource->SetBounds(bounds);
        this->OutlineSource->Update();
        this->Interactor->Render();
        }
      break;
    default:
      break;
    }
  this->PrevPos[0] = x;
  this->PrevPos[1] = y;
}

//----------------------------------------------------------------------------
void vtkInteractorStyleXYPlot::Zoom()
{
  int x;
  int y;

  this->Interactor->GetEventPosition(x, y);

  switch (this->CurrentXYPlotElement)
    {
    case VTKIS_XYPLOT_PLOT_AREA:
      if (x != this->PrevPos[0])
        {
        this->CurrentXYPlotActor->ZoomInAtXValue(this->StartPos[0],
          exp(0.01 * (x - this->PrevPos[0])));
        }
      if (y != this->PrevPos[1])
        {
        this->CurrentXYPlotActor->ZoomInAtYValue(this->StartPos[1],
          exp(0.01 * (y - this->PrevPos[1])));
        }
      if (x != this->PrevPos[0] || y != this->PrevPos[1])
        {
        this->Interactor->Render();
        }
      break;
    case VTKIS_XYPLOT_X_AXIS:
      if (x != this->PrevPos[0])
        {
        this->CurrentXYPlotActor->ZoomInAtXValue(this->StartPos[0],
          exp(0.01 * (x - this->PrevPos[0])));
        this->Interactor->Render();
        }
      break;
    case VTKIS_XYPLOT_Y_AXIS:
      if (y != this->PrevPos[1])
        {
        this->CurrentXYPlotActor->ZoomInAtYValue(this->StartPos[1],
          exp(0.01 * (y - this->PrevPos[1])));
        this->Interactor->Render();
        }
      break;
    default:
      break;
    }
  this->PrevPos[0] = x;
  this->PrevPos[1] = y;
}

//----------------------------------------------------------------------------
void vtkInteractorStyleXYPlot::StartPan()
{
  int pos[2];

  this->Interactor->GetEventPosition(pos);

  vtkDebugMacro(<< "Starting Pan");

  if (this->State != VTKIS_START) 
    {
    return;
    }

  switch (this->CurrentXYPlotElement)
    {
    case VTKIS_XYPLOT_PLOT_AREA:
    case VTKIS_XYPLOT_X_AXIS:
    case VTKIS_XYPLOT_Y_AXIS:
      this->CurrentXYPlotActor->GetInnerPlotBounds(this->ViewportBounds);
      this->StartPos[0] = pos[0];
      this->StartPos[1] = pos[1];
      this->PrevPos[0] = pos[0];
      this->PrevPos[1] = pos[1];
      this->StartState(VTKIS_PAN);
      break;
    default:
      break;
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleXYPlot::StartOutlineZoom()
{
  vtkFloatingPointType color[3];
  vtkPolyDataMapper2D* outlineMapper;
  int pos[2];

  this->Interactor->GetEventPosition(pos);

  vtkDebugMacro(<< "Starting Zoom (using an outline)");

  if (this->State != VTKIS_START) 
    {
    return;
    }

  switch (this->CurrentXYPlotElement)
    {
    case VTKIS_XYPLOT_PLOT_AREA:
      this->CurrentXYPlotActor->GetInnerPlotBounds(this->ViewportBounds);
      this->OutlineSource = vtkOutlineSource::New();
      this->OutlineSource->SetBounds(pos[0], pos[0], pos[1], pos[1], 0, 0);
      outlineMapper = vtkPolyDataMapper2D::New();
      outlineMapper->SetInput(this->OutlineSource->GetOutput());
      this->OutlineActor = vtkActor2D::New();
      this->OutlineActor->SetMapper(outlineMapper);
      this->CurrentRenderer->GetBackground(color);
      color[0] = 1.0 - color[0];
      color[1] = 1.0 - color[1];
      color[2] = 1.0 - color[2];
      this->OutlineActor->GetProperty()->SetColor(color);
      this->OutlineActor->GetProperty()->SetLineStipplePattern(0xCCCC);
      this->CurrentRenderer->AddActor2D(this->OutlineActor);
      this->OutlineSource->Delete();
      outlineMapper->Delete();
      this->PrevPos[0] = pos[0];
      this->PrevPos[1] = pos[1];
      this->StartState(VTKIS_OUTLINEZOOM);
      break;
    default:
      break;
   }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleXYPlot::StartZoom()
{
  double* range;
  int pos[2];

  this->Interactor->GetEventPosition(pos);

  vtkDebugMacro(<< "Starting Zoom");

  if (this->State != VTKIS_START) 
    {
    return;
    }

  switch (this->CurrentXYPlotElement)
    {
    case VTKIS_XYPLOT_PLOT_AREA:
    case VTKIS_XYPLOT_X_AXIS:
    case VTKIS_XYPLOT_Y_AXIS:
      this->CurrentXYPlotActor->GetInnerPlotBounds(this->ViewportBounds);
      this->StartPos[0] = pos[0];
      this->StartPos[1] = pos[1];
      range = this->CurrentXYPlotActor->GetComputedXRange();
      vtkTools::ViewportToData(this->StartPos[0], this->ViewportBounds,
                                  range, this->CurrentXYPlotActor->GetLogX());
      range = this->CurrentXYPlotActor->GetComputedYRange();
      vtkTools::ViewportToData(this->StartPos[1], &(this->ViewportBounds[2]),
                                  range, this->CurrentXYPlotActor->GetLogY());
      this->PrevPos[0] = pos[0];
      this->PrevPos[1] = pos[1];
      this->StartState(VTKIS_ZOOM);
      break;
    default:
      break;
   }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleXYPlot::EndOutlineZoom()
{
  if (this->State != VTKIS_OUTLINEZOOM) 
    {
    return;
    }

  if (CurrentXYPlotElement == VTKIS_XYPLOT_PLOT_AREA)
    {
    vtkFloatingPointType zoomBounds[6];
    this->OutlineSource->GetBounds(zoomBounds);
    // Only zoom if the axis of the zoombox are not zero 
    if (zoomBounds[0] != zoomBounds[1] && zoomBounds[2] != zoomBounds[3])
      {
      double newRange[2];
      double* range;
      if (zoomBounds[0] > zoomBounds[1])
        {
        double temp = zoomBounds[0];
        zoomBounds[0] = zoomBounds[1];
        zoomBounds[1] = temp;
        }
      if (zoomBounds[2] > zoomBounds[3])
        {
        double temp = zoomBounds[2];
        zoomBounds[2] = zoomBounds[3];
        zoomBounds[3] = temp;
        }
      range = this->CurrentXYPlotActor->GetComputedXRange();
      newRange[0] = zoomBounds[0];
      vtkTools::ViewportToData(newRange[0], this->ViewportBounds,
                                  range, this->CurrentXYPlotActor->GetLogX());
      newRange[1] = zoomBounds[1];
      vtkTools::ViewportToData(newRange[1], this->ViewportBounds,
                                  range, this->CurrentXYPlotActor->GetLogX());
      this->CurrentXYPlotActor->ZoomToXRange(newRange);
      range = this->CurrentXYPlotActor->GetComputedYRange();
      newRange[0] = zoomBounds[2];
      vtkTools::ViewportToData(newRange[0], &(this->ViewportBounds[2]),
                                  range, this->CurrentXYPlotActor->GetLogY());
      newRange[1] = zoomBounds[3];
      vtkTools::ViewportToData(newRange[1], &(this->ViewportBounds[2]),
                                  range, this->CurrentXYPlotActor->GetLogY());
      this->CurrentXYPlotActor->ZoomToYRange(newRange);
      }
    this->CurrentRenderer->RemoveActor2D(this->OutlineActor);
    this->OutlineActor->Delete();
    this->OutlineActor = NULL;
    this->Interactor->Render();
    }
  this->StopState();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleXYPlot::OnChar()
{
  // Completely overrule the default key behaviour of vtkInteractorStyle
  char keycode = this->Interactor->GetKeyCode();

  switch (keycode)
    {
    case 'Q':
    case 'q':
    case 'E':
    case 'e':
      this->Interactor->ExitCallback();
      break;
    case 'R':
    case 'r': // Reset the pan and zoom values
      this->FindPokedXYPlotActor(this->Interactor->GetEventPosition()[0],
                                 this->Interactor->GetEventPosition()[1]);
      // Only allow resetting if we are not panning/zooming.
      if (this->CurrentXYPlotActor != 0 && this->State == VTKIS_START)
        {
        double range[2];
        this->CurrentXYPlotActor->GetBaseXRange(range);
        this->CurrentXYPlotActor->ZoomToXRange(range);
        this->CurrentXYPlotActor->GetBaseYRange(range);
        this->CurrentXYPlotActor->ZoomToYRange(range);
        this->Interactor->Render();
        }
      break;
    case 'U':
    case 'u':
      this->Interactor->UserCallback();
      break;
    case 'A':
    case 'a': // Adjust labels on axes to 'nice' range
      this->FindPokedXYPlotActor(this->Interactor->GetEventPosition()[0],
                                 this->Interactor->GetEventPosition()[1]);
      if (this->CurrentXYPlotActor != 0 && this->State == VTKIS_START)
        {
        switch (this->CurrentXYPlotElement)
          {
          case VTKIS_XYPLOT_X_AXIS:
            if (keycode == 'A')
              {
              this->CurrentXYPlotActor->ZoomToInnerXRange();
              }
            else
              {
              this->CurrentXYPlotActor->ZoomToOuterXRange();
              }
            this->Interactor->Render();
            break;
          case VTKIS_XYPLOT_Y_AXIS:
            if (keycode == 'A')
              {
              this->CurrentXYPlotActor->ZoomToInnerYRange();
              }
            else
              {
              this->CurrentXYPlotActor->ZoomToOuterYRange();
              }
            this->Interactor->Render();
            break;
          case VTKIS_XYPLOT_PLOT_AREA:
            if (keycode == 'A')
              {
              this->CurrentXYPlotActor->ZoomToInnerXRange();
              this->CurrentXYPlotActor->ZoomToInnerYRange();
              }
            else
              {
              this->CurrentXYPlotActor->ZoomToOuterXRange();
              this->CurrentXYPlotActor->ZoomToOuterYRange();
              }
            this->Interactor->Render();
            break;
          default:
            break;
          }
        }
      break;
    case 'L':
    case 'l': // Switch on/off logarithmic axes
      this->FindPokedXYPlotActor(this->Interactor->GetEventPosition()[0],
                                 this->Interactor->GetEventPosition()[1]);
      if (this->CurrentXYPlotActor != 0 && this->State == VTKIS_START)
        {
        switch (this->CurrentXYPlotElement)
          {
          case VTKIS_XYPLOT_X_AXIS:
            this->CurrentXYPlotActor->SetLogX(
              !this->CurrentXYPlotActor->GetLogX());
            this->Interactor->Render();
            break;
          case VTKIS_XYPLOT_Y_AXIS:
            this->CurrentXYPlotActor->SetLogY(
              !this->CurrentXYPlotActor->GetLogY());
            this->Interactor->Render();
            break;
          default:
            break;
          }
        }
      break;
    default:
      break;
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleXYPlot::OnTimer()
{
  vtkRenderWindowInteractor *rwi = this->Interactor;

  switch (this->State)
    {
    case VTKIS_PAN:
      this->Pan();
      rwi->CreateTimer(VTKI_TIMER_UPDATE);
      break;
    case VTKIS_OUTLINEZOOM:
      this->OutlineZoom();
      rwi->CreateTimer(VTKI_TIMER_UPDATE);
      break;
    case VTKIS_ZOOM:
      this->Zoom();
      rwi->CreateTimer(VTKI_TIMER_UPDATE);
      break;
    case VTKIS_TIMER:
      rwi->Render();
      rwi->CreateTimer(VTKI_TIMER_UPDATE);
      break;
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleXYPlot::OnLeftButtonDown()
{
  this->FindPokedXYPlotActor(this->Interactor->GetEventPosition()[0],
                             this->Interactor->GetEventPosition()[1]);

  if (this->CurrentXYPlotActor != NULL)
    {
    if (this->Interactor->GetControlKey())
      {
      if (this->CurrentXYPlotElement == VTKIS_XYPLOT_X_AXIS ||
          this->CurrentXYPlotElement == VTKIS_XYPLOT_Y_AXIS ||
          this->CurrentXYPlotElement == VTKIS_XYPLOT_PLOT_AREA)
        {
        this->StartZoom();
        }
      }
    else
      {
      if (this->CurrentXYPlotElement == VTKIS_XYPLOT_X_AXIS ||
          this->CurrentXYPlotElement == VTKIS_XYPLOT_Y_AXIS ||
          this->CurrentXYPlotElement == VTKIS_XYPLOT_PLOT_AREA)
        {
        this->StartPan();
        }
      }
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleXYPlot::OnLeftButtonUp()
{
  if (this->CurrentXYPlotActor != NULL)
    {
    if (this->Interactor->GetControlKey())
      {
      if (this->CurrentXYPlotElement == VTKIS_XYPLOT_X_AXIS ||
          this->CurrentXYPlotElement == VTKIS_XYPLOT_Y_AXIS ||
          this->CurrentXYPlotElement == VTKIS_XYPLOT_PLOT_AREA)
        {
        this->EndZoom();
        }
      }
    else
      {
      if (this->CurrentXYPlotElement == VTKIS_XYPLOT_X_AXIS ||
          this->CurrentXYPlotElement == VTKIS_XYPLOT_Y_AXIS ||
          this->CurrentXYPlotElement == VTKIS_XYPLOT_PLOT_AREA)
        {
        this->EndPan();
        }
      }
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleXYPlot::OnMiddleButtonDown()
{
  this->FindPokedXYPlotActor(this->Interactor->GetEventPosition()[0],
                             this->Interactor->GetEventPosition()[1]);

  if (this->CurrentXYPlotActor != NULL)
    {
    if (this->CurrentXYPlotElement == VTKIS_XYPLOT_PLOT_AREA)
      {
      this->StartOutlineZoom();
      }
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleXYPlot::OnMiddleButtonUp()
{
  if (this->CurrentXYPlotActor != NULL)
    {
    if (this->CurrentXYPlotElement == VTKIS_XYPLOT_PLOT_AREA)
      {
      this->EndOutlineZoom();
      }
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleXYPlot::OnRightButtonDown()
{
  this->FindPokedXYPlotActor(this->Interactor->GetEventPosition()[0],
                             this->Interactor->GetEventPosition()[1]);

  if (this->CurrentXYPlotActor != NULL)
    {
    if (this->CurrentXYPlotElement == VTKIS_XYPLOT_X_AXIS ||
        this->CurrentXYPlotElement == VTKIS_XYPLOT_Y_AXIS ||
        this->CurrentXYPlotElement == VTKIS_XYPLOT_PLOT_AREA)
      {
      this->StartZoom();
      }
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleXYPlot::OnRightButtonUp()
{
  if (this->CurrentXYPlotActor != NULL)
    {
    if (this->CurrentXYPlotElement == VTKIS_XYPLOT_X_AXIS ||
        this->CurrentXYPlotElement == VTKIS_XYPLOT_Y_AXIS ||
        this->CurrentXYPlotElement == VTKIS_XYPLOT_PLOT_AREA)
      {
      this->EndZoom();
      }
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleXYPlot::PrintSelf(std::ostream& os, vtkIndent indent)
{
  vtkInteractorStyle::PrintSelf(os,indent);
  
  os << indent << "CurrentXYPlotActor: " << this->CurrentXYPlotActor << std::endl;
  os << indent << "CurrentXYPlotElement: " << this->CurrentXYPlotElement
     << std::endl;
  os << indent << "StartPos: (" << this->StartPos[0] << ", "
     << this->StartPos[1] << ")" << std::endl;
  os << indent << "PrevPos: (" << this->PrevPos[0] << ", "
     << this->PrevPos[1] << ")" << std::endl;
  os << indent << "ViewportBounds: (" << this->ViewportBounds[0] << ", "
     << this->ViewportBounds[1] << ", " << this->ViewportBounds[2] << ", "
     << this->ViewportBounds[3] << ")" << std::endl;
}
