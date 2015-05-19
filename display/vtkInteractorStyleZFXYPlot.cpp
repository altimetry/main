/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInteractorStyleZFXYPlot.cxx
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
#include "vtkInteractorStyleZFXYPlot.h"

#include "vtkTools.h"
#include "vtkActor2D.h"
#include "vtkCommand.h"
#include "vtkIndent.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkOutlineSource.h"
#include "vtkZFXYPlotFilter.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPropCollection.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
//#include "vtkPlotData.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif


vtkStandardNewMacro(vtkInteractorStyleZFXYPlot);

//----------------------------------------------------------------------------
vtkInteractorStyleZFXYPlot::vtkInteractorStyleZFXYPlot()
{
  this->OutlineActor = NULL;
  this->UseTimers = 1;
}

//----------------------------------------------------------------------------
vtkInteractorStyleZFXYPlot::~vtkInteractorStyleZFXYPlot()
{
  if (this->OutlineActor != NULL)
    {
    this->OutlineActor->Delete();
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleZFXYPlot::FindPokedXYPlotActor(int x, int y)
{
  vtkDebugMacro(<< "Poking vtkZFXYPlotActor at (" << x << ", " << y << ")");

  this->FindPokedRenderer(x, y);

  this->CurrentZFXYPlotActor = NULL;
  this->CurrentZFXYPlotElement = -1;
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
    while ((prop = props->GetNextProp()) != NULL && CurrentZFXYPlotActor == NULL)
      {
      vtkZFXYPlotActor* plotActor = vtkZFXYPlotActor::SafeDownCast(prop);
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
            this->CurrentZFXYPlotActor = plotActor;
            }
          }
        }
      }
    if (this->CurrentZFXYPlotActor)
      {
      vtkDebugMacro(<< "Picked plotactor " << (void*)this->CurrentZFXYPlotActor);
      if (this->CurrentZFXYPlotActor->IsInPlot(x, y))
        {
        vtkDebugMacro(<< "We are in the plot area");
        this->CurrentZFXYPlotElement = VTKIS_ZFXYPLOT_PLOT_AREA;
        //vtkDebugMacro(<< "Nearest PlotData " <<
        //this->CurrentZFXYPlotActor->FindPlotData(x, y));
        this->CurrentZFXYPlotActor->FindPlotData(x, y);
        }
      else if (this->CurrentZFXYPlotActor->IsXAxis(x, y))
        {
        vtkDebugMacro(<< "We are at the X Axis");
        this->CurrentZFXYPlotElement = VTKIS_ZFXYPLOT_X_AXIS;
        }
      else if (this->CurrentZFXYPlotActor->IsYAxis(x, y))
        {
        vtkDebugMacro(<< "We are at the Y Axis");
        this->CurrentZFXYPlotElement = VTKIS_ZFXYPLOT_Y_AXIS;
        }
      }
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleZFXYPlot::Pan()
{
  int x;
  int y;

  this->Interactor->GetEventPosition(x, y);

  switch (this->CurrentZFXYPlotElement)
    {
    case VTKIS_ZFXYPLOT_PLOT_AREA:
      if (x != this->PrevPos[0])
        {
        this->CurrentZFXYPlotActor->PanXRange((x - this->PrevPos[0]) / 
          (this->ViewportBounds[1] - this->ViewportBounds[0]));
        }
      if (y != this->PrevPos[1])
        {
        this->CurrentZFXYPlotActor->PanYRange((y - this->PrevPos[1]) / 
          (this->ViewportBounds[3] - this->ViewportBounds[2]));
        }
      if (x != this->PrevPos[0] || y != this->PrevPos[1])
        {
        this->Interactor->Render();
        }
      break;
    case VTKIS_ZFXYPLOT_X_AXIS:
      if (x != this->PrevPos[0])
        {
        this->CurrentZFXYPlotActor->PanXRange((x - this->PrevPos[0]) / 
          (this->ViewportBounds[1] - this->ViewportBounds[0]));
        this->Interactor->Render();
        }
    break;
    case VTKIS_ZFXYPLOT_Y_AXIS:
      if (y != this->PrevPos[1])
        {
        this->CurrentZFXYPlotActor->PanYRange((y - this->PrevPos[1]) / 
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
void vtkInteractorStyleZFXYPlot::OutlineZoom()
{
  int x;
  int y;

  this->Interactor->GetEventPosition(x, y);

  switch (this->CurrentZFXYPlotElement)
    {
    case VTKIS_ZFXYPLOT_PLOT_AREA:
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
void vtkInteractorStyleZFXYPlot::Zoom()
{
  int x;
  int y;

  this->Interactor->GetEventPosition(x, y);

  switch (this->CurrentZFXYPlotElement)
    {
    case VTKIS_ZFXYPLOT_PLOT_AREA:
      if (x != this->PrevPos[0])
        {
        this->CurrentZFXYPlotActor->ZoomInAtXValue(this->StartPos[0],
          exp(0.01 * (x - this->PrevPos[0])));
        }
      if (y != this->PrevPos[1])
        {
        this->CurrentZFXYPlotActor->ZoomInAtYValue(this->StartPos[1],
          exp(0.01 * (y - this->PrevPos[1])));
        }
      if (x != this->PrevPos[0] || y != this->PrevPos[1])
        {
        this->Interactor->Render();
        }
      break;
    case VTKIS_ZFXYPLOT_X_AXIS:
      if (x != this->PrevPos[0])
        {
        this->CurrentZFXYPlotActor->ZoomInAtXValue(this->StartPos[0],
          exp(0.01 * (x - this->PrevPos[0])));
        this->Interactor->Render();
        }
      break;
    case VTKIS_ZFXYPLOT_Y_AXIS:
      if (y != this->PrevPos[1])
        {
        this->CurrentZFXYPlotActor->ZoomInAtYValue(this->StartPos[1],
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
void vtkInteractorStyleZFXYPlot::StartPan()
{
  int pos[2];

  this->Interactor->GetEventPosition(pos);

  vtkDebugMacro(<< "Starting Pan");

  if (this->State != VTKIS_START) 
    {
    return;
    }

  switch (this->CurrentZFXYPlotElement)
    {
    case VTKIS_ZFXYPLOT_PLOT_AREA:
    case VTKIS_ZFXYPLOT_X_AXIS:
    case VTKIS_ZFXYPLOT_Y_AXIS:
      this->CurrentZFXYPlotActor->GetInnerPlotBounds(this->ViewportBounds);
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
void vtkInteractorStyleZFXYPlot::StartOutlineZoom()
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

  switch (this->CurrentZFXYPlotElement)
    {
    case VTKIS_ZFXYPLOT_PLOT_AREA:
      this->CurrentZFXYPlotActor->GetInnerPlotBounds(this->ViewportBounds);
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
void vtkInteractorStyleZFXYPlot::StartZoom()
{
  double* range;
  int pos[2];

  this->Interactor->GetEventPosition(pos);

  vtkDebugMacro(<< "Starting Zoom");

  if (this->State != VTKIS_START) 
    {
    return;
    }

  switch (this->CurrentZFXYPlotElement)
    {
    case VTKIS_ZFXYPLOT_PLOT_AREA:
    case VTKIS_ZFXYPLOT_X_AXIS:
    case VTKIS_ZFXYPLOT_Y_AXIS:
      this->CurrentZFXYPlotActor->GetInnerPlotBounds(this->ViewportBounds);
      this->StartPos[0] = pos[0];
      this->StartPos[1] = pos[1];
      range = this->CurrentZFXYPlotActor->GetComputedXRange();
      vtkTools::ViewportToData(this->StartPos[0], this->ViewportBounds,
                                  range, this->CurrentZFXYPlotActor->GetLogX());
      range = this->CurrentZFXYPlotActor->GetComputedYRange();
      vtkTools::ViewportToData(this->StartPos[1], &(this->ViewportBounds[2]),
                                  range, this->CurrentZFXYPlotActor->GetLogY());
      this->PrevPos[0] = pos[0];
      this->PrevPos[1] = pos[1];
      this->StartState(VTKIS_ZOOM);
      break;
    default:
      break;
   }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleZFXYPlot::EndOutlineZoom()
{
  if (this->State != VTKIS_OUTLINEZOOM) 
    {
    return;
    }

  if (CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_PLOT_AREA)
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
      range = this->CurrentZFXYPlotActor->GetComputedXRange();
      newRange[0] = zoomBounds[0];
      vtkTools::ViewportToData(newRange[0], this->ViewportBounds,
                                  range, this->CurrentZFXYPlotActor->GetLogX());
      newRange[1] = zoomBounds[1];
      vtkTools::ViewportToData(newRange[1], this->ViewportBounds,
                                  range, this->CurrentZFXYPlotActor->GetLogX());
      this->CurrentZFXYPlotActor->ZoomToXRange(newRange);
      range = this->CurrentZFXYPlotActor->GetComputedYRange();
      newRange[0] = zoomBounds[2];
      vtkTools::ViewportToData(newRange[0], &(this->ViewportBounds[2]),
                                  range, this->CurrentZFXYPlotActor->GetLogY());
      newRange[1] = zoomBounds[3];
      vtkTools::ViewportToData(newRange[1], &(this->ViewportBounds[2]),
                                  range, this->CurrentZFXYPlotActor->GetLogY());
      this->CurrentZFXYPlotActor->ZoomToYRange(newRange);
      }
    this->CurrentRenderer->RemoveActor2D(this->OutlineActor);
    this->OutlineActor->Delete();
    this->OutlineActor = NULL;
    this->Interactor->Render();
    }
  this->StopState();

  this->InvokeEvent(vtkInteractorStyleZFXYPlot::EndZoomEvent, NULL);

}

//----------------------------------------------------------------------------
void vtkInteractorStyleZFXYPlot::OnChar()
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
      if (this->CurrentZFXYPlotActor != 0 && this->State == VTKIS_START)
        {
        double range[2];
        this->CurrentZFXYPlotActor->GetBaseXRange(range);
        this->CurrentZFXYPlotActor->ZoomToXRange(range);
        this->CurrentZFXYPlotActor->GetBaseYRange(range);
        this->CurrentZFXYPlotActor->ZoomToYRange(range);
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
      if (this->CurrentZFXYPlotActor != 0 && this->State == VTKIS_START)
        {
        switch (this->CurrentZFXYPlotElement)
          {
          case VTKIS_ZFXYPLOT_X_AXIS:
            if (keycode == 'A')
              {
              this->CurrentZFXYPlotActor->ZoomToInnerXRange();
              }
            else
              {
              this->CurrentZFXYPlotActor->ZoomToOuterXRange();
              }
            this->Interactor->Render();
            break;
          case VTKIS_ZFXYPLOT_Y_AXIS:
            if (keycode == 'A')
              {
              this->CurrentZFXYPlotActor->ZoomToInnerYRange();
              }
            else
              {
              this->CurrentZFXYPlotActor->ZoomToOuterYRange();
              }
            this->Interactor->Render();
            break;
          case VTKIS_ZFXYPLOT_PLOT_AREA:
            if (keycode == 'A')
              {
              this->CurrentZFXYPlotActor->ZoomToInnerXRange();
              this->CurrentZFXYPlotActor->ZoomToInnerYRange();
              }
            else
              {
              this->CurrentZFXYPlotActor->ZoomToOuterXRange();
              this->CurrentZFXYPlotActor->ZoomToOuterYRange();
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
      if (this->CurrentZFXYPlotActor != 0 && this->State == VTKIS_START)
        {
        switch (this->CurrentZFXYPlotElement)
          {
          case VTKIS_ZFXYPLOT_X_AXIS:
            this->CurrentZFXYPlotActor->SetLogX(
              !this->CurrentZFXYPlotActor->GetLogX());
            this->Interactor->Render();
            break;
          case VTKIS_ZFXYPLOT_Y_AXIS:
            this->CurrentZFXYPlotActor->SetLogY(
              !this->CurrentZFXYPlotActor->GetLogY());
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
void vtkInteractorStyleZFXYPlot::EndZoom()
{
  vtkInteractorStyle::EndZoom();
  this->InvokeEvent(vtkInteractorStyleZFXYPlot::EndZoomEvent, NULL);

}
//----------------------------------------------------------------------------
void vtkInteractorStyleZFXYPlot::EndPan()
{
  vtkInteractorStyle::EndPan();
  this->InvokeEvent(vtkInteractorStyleZFXYPlot::EndPanEvent, NULL);

}

//----------------------------------------------------------------------------
void vtkInteractorStyleZFXYPlot::OnTimer()
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
void vtkInteractorStyleZFXYPlot::OnLeftButtonDown()
{
  this->FindPokedXYPlotActor(this->Interactor->GetEventPosition()[0],
                             this->Interactor->GetEventPosition()[1]);

  if (this->CurrentZFXYPlotActor != NULL)
    {
    if (this->Interactor->GetControlKey())
      {
      if (this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_X_AXIS ||
          this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_Y_AXIS ||
          this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_PLOT_AREA)
        {
        this->StartZoom();
        }
      }
    else
      {
      if (this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_X_AXIS ||
          this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_Y_AXIS ||
          this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_PLOT_AREA)
        {
        this->StartPan();
        }
      }
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleZFXYPlot::OnLeftButtonUp()
{
  if (this->CurrentZFXYPlotActor != NULL)
    {
    if (this->Interactor->GetControlKey())
      {
      if (this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_X_AXIS ||
          this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_Y_AXIS ||
          this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_PLOT_AREA)
        {
        this->EndZoom();
        }
      }
    else
      {
      if (this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_X_AXIS ||
          this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_Y_AXIS ||
          this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_PLOT_AREA)
        {
        this->EndPan();
        }
      }
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleZFXYPlot::OnMiddleButtonDown()
{
  this->FindPokedXYPlotActor(this->Interactor->GetEventPosition()[0],
                             this->Interactor->GetEventPosition()[1]);

  if (this->CurrentZFXYPlotActor != NULL)
    {
    if (this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_PLOT_AREA)
      {
      this->StartOutlineZoom();
      }
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleZFXYPlot::OnMiddleButtonUp()
{
  if (this->CurrentZFXYPlotActor != NULL)
    {
    if (this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_PLOT_AREA)
      {
      this->EndOutlineZoom();
      }
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleZFXYPlot::OnRightButtonDown()
{
  this->FindPokedXYPlotActor(this->Interactor->GetEventPosition()[0],
                             this->Interactor->GetEventPosition()[1]);

  if (this->CurrentZFXYPlotActor != NULL)
    {
    if (this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_X_AXIS ||
        this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_Y_AXIS ||
        this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_PLOT_AREA)
      {
      this->StartZoom();
      }
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleZFXYPlot::OnRightButtonUp()
{
  if (this->CurrentZFXYPlotActor != NULL)
    {
    if (this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_X_AXIS ||
        this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_Y_AXIS ||
        this->CurrentZFXYPlotElement == VTKIS_ZFXYPLOT_PLOT_AREA)
      {
      this->EndZoom();
      }
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleZFXYPlot::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkInteractorStyle::PrintSelf(os,indent);
  
  os << indent << "CurrentZFXYPlotActor: " << this->CurrentZFXYPlotActor << endl;
  os << indent << "CurrentZFXYPlotElement: " << this->CurrentZFXYPlotElement
     << endl;
  os << indent << "StartPos: (" << this->StartPos[0] << ", "
     << this->StartPos[1] << ")" << endl;
  os << indent << "PrevPos: (" << this->PrevPos[0] << ", "
     << this->PrevPos[1] << ")" << endl;
  os << indent << "ViewportBounds: (" << this->ViewportBounds[0] << ", "
     << this->ViewportBounds[1] << ", " << this->ViewportBounds[2] << ", "
     << this->ViewportBounds[3] << ")" << endl;
}
