/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInteractorStyleWPlot.cxx
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
#include "MapProjection.h"


#include "vtkActor2D.h"
#include "vtkCommand.h"
#include "vtkCoordinate.h"
#include "vtkIndent.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkOutlineSource.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkTransformCollection.h"

#include "Tools.h"
#include "Trace.h"
#include "Exception.h"
using namespace brathl;

#include "vtkInteractorStyleWPlot.h"




#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif
//-------------------------------------------------------------
//------------------- vtkWPlotState class --------------------
//-------------------------------------------------------------
vtkStandardNewMacro(vtkWPlotState);

//----------------------------------------------------------------------------
vtkWPlotState::vtkWPlotState()
{

}
//----------------------------------------------------------------------------
vtkWPlotState::~vtkWPlotState()
{

}
//----------------------------------------------------------------------------
void vtkWPlotState::PrintSelf(ostream& os, vtkIndent indent)
{

  os << indent << "MidPoint: (" << this->MidPoint[0] << ", "
     << this->MidPoint[1] << ")" << endl;
  os << indent << "Size: (" << this->Size[0] << ", "
     << this->Size[1] << ")" << endl;

     //KAVOK: now we have a zoom parameter
  os << indent << "Zoom: (" << this->ZoomScale << endl;
}

//-------------------------------------------------------------
//------------------- vtkWPlotProjectionState class --------------------
//-------------------------------------------------------------
vtkStandardNewMacro(vtkWPlotProjectionState);

//----------------------------------------------------------------------------
vtkWPlotProjectionState::vtkWPlotProjectionState()
{
  this->CurrentState = this->States.size() - 1;

}
//----------------------------------------------------------------------------
vtkWPlotProjectionState::~vtkWPlotProjectionState()
{

}

//----------------------------------------------------------------------------
void vtkWPlotProjectionState::SaveState(vtkInteractorStyleWPlot* parent)
{
   double mid[2];

  vtkWPlotState* state = vtkWPlotState::New();
  state->SetSize(parent->GetSize());

  //KAVOK: added to preserve the state
  parent->GetViewMidPoint(mid[0], mid[1]);
  state->SetMidPoint(mid);
  state->SetZoomScale(parent->GetZoomScale());

  this->States.Insert(state);

  this->CurrentState = this->States.size() - 1;

}
//----------------------------------------------------------------------------
void vtkWPlotProjectionState::RestoreState(vtkInteractorStyleWPlot* parent)
{
  RestoreState(parent, CurrentState);

}
//----------------------------------------------------------------------------
void vtkWPlotProjectionState::RestoreState(vtkInteractorStyleWPlot* parent, int32_t index)
{
   double *mid;

  if (index < 0)
  {
    return;
  }

  vtkWPlotState* state = this->GetState(index);
  parent->SetSize(state->GetSize());

  mid = state->GetMidPoint();

  //KAVOK: to preserve the state of these properties
  parent->SetViewMidPoint(mid[0], mid[1]);
  parent->SetZoomScale(state->GetZoomScale());

}

//----------------------------------------------------------------------------
void vtkWPlotProjectionState::ClearStates()
{
  if (this->States.size() <= 0)
  {
    return;
  }

  // In this loop, delete all array elements except first element
  vtkObArray::reverse_iterator rev_it;
  for (rev_it = this->States.rbegin() + 1; rev_it != this->States.rend() ; rev_it++)
  {
    this->States.Erase(rev_it.base());
  }

  this->CurrentState = this->States.size() - 1;
}
//----------------------------------------------------------------------------
vtkWPlotState* vtkWPlotProjectionState::GetState(int32_t index)
{
  if ((index < 0) || (index >= static_cast<int32_t>(this->States.size())))
  {
    throw CException(CTools::Format("ERROR in  vtkWPlotProjectionState::GetState : Invalid index %d - Max. index %ld",
                                    index, (long)this->States.size() - 1),
                     BRATHL_LOGIC_ERROR);

  }
  vtkWPlotState* state = dynamic_cast<vtkWPlotState*>(this->States.at(index));
  if (state == NULL)
  {
    throw CException(CTools::Format("ERROR in  vtkWPlotProjectionState::GetState : dynamic_cast<vtkWPlotState*>(this->States.at(%d) returns NULL pointer - "
                                    "state array seems to contain objects other than those of the class vtkWPlotState",
                                    index),
                     BRATHL_LOGIC_ERROR);

  }

  return state;
}
//----------------------------------------------------------------------------
void vtkWPlotProjectionState::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "Projection: (" << this->Projection << endl;
}


//-------------------------------------------------------------
//------------------- vtkInteractorStyleWPlot class --------------------
//-------------------------------------------------------------

vtkStandardNewMacro(vtkInteractorStyleWPlot);

//----------------------------------------------------------------------------
vtkInteractorStyleWPlot::vtkInteractorStyleWPlot()
{
  this->UseTimers = 1;

  this->Size[0] = 1.0;
  this->Size[1] = 1.0;
  this->MidPoint[0] = 0.5;
  this->MidPoint[1] = 0.5;

  this->RatioVector[0] = 1.0;
  this->RatioVector[1] = 1.0;

  this->ZoomTranslateX = 0.0;
  this->ZoomTranslateY = 0.0;

  //set same ZoomScale as in VISAN
  this->ZoomScale = 2.5;

  this->OutlineActor = NULL;

  this->ViewportSize[0] = 1;
  this->ViewportSize[1] = 1;

  this->TransformCollection = vtkTransformCollection::New();

  this->AutoAdjustCameraClippingRangeOn();

}

//----------------------------------------------------------------------------
vtkInteractorStyleWPlot::~vtkInteractorStyleWPlot()
{
  if (this->TransformCollection != NULL)
    {
    this->TransformCollection->Delete();
    }

  if (this->OutlineActor != NULL)
    {
    this->OutlineActor->Delete();
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::SetTransformCollection(vtkTransformCollection* collection)
{
  if (collection != NULL)
    {
    collection->Register(this);
    if (this->TransformCollection != NULL)
      {
      TransformCollection->UnRegister(this);
      }
    this->TransformCollection = collection;
    }
}

//----------------------------------------------------------------------------
vtkTransformCollection* vtkInteractorStyleWPlot::GetTransformCollection()
{
  return this->TransformCollection;
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::SetRatio(double xRatio, double yRatio)
{

   vtkDebugMacro(<< "Setting Ratio to:" << xRatio << "," << yRatio);

  this->Size[0] *= (xRatio / this->RatioVector[0]);
  this->Size[1] *= (yRatio / this->RatioVector[1]);

  this->RatioVector[0] = xRatio;
  this->RatioVector[1] = yRatio;

  this->SetTransformation();

}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::SetViewportSize(int width, int height)
{
  vtkDebugMacro(<< "Setting Viewport Size  to:" << width << "," << height);

  this->ViewportSize[0] = width;
  this->ViewportSize[1] = height;

}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::MapPointToMidPointTranslateVector(double x, double y)
{

  this->ZoomTranslateX = (1 - 2 * ((x - this->minX()) / (this->Size[0])));
  this->ZoomTranslateY = (1 - 2 * ((y - this->minY()) / (this->Size[1])));
}

//-----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::SetTransformation()
{
  vtkTransform* transform;

  this->TransformCollection->InitTraversal();
  transform = this->TransformCollection->GetNextItem();
  while (transform)
    {
    transform->Identity();
    transform->Translate(this->minX(), this->minY(), 0);
    transform->Scale(this->Size[0], this->Size[1], 0);
    transform = this->TransformCollection->GetNextItem();
    }
  this->Interactor->Render();
}

//----------------------------------------------------------------------------
double vtkInteractorStyleWPlot::maxX()
{
  return this->MidPoint[0] + this->Size[0] / 2;
}

//----------------------------------------------------------------------------
double vtkInteractorStyleWPlot::minX()
{
  return this->MidPoint[0] - this->Size[0] / 2;
}

//----------------------------------------------------------------------------
double vtkInteractorStyleWPlot::maxY()
{
  return this->MidPoint[1] + this->Size[1] / 2;
}

//----------------------------------------------------------------------------
double vtkInteractorStyleWPlot::minY()
{
  return this->MidPoint[1] - this->Size[1] / 2;
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::StartOutlineZoom(int x, int y)
{
  /*********************************************************
   This code almost works, but shows a drift in the y axis
   commented until a fix is found.
  ********************************************************/

 /*
  vtkFloatingPointType color[3];
  vtkPolyDataMapper2D* outlineMapper;

  this->OutlineSource = vtkOutlineSource::New();
  this->OutlineSource->SetBounds(x, x, y, y, 0, 0);

  outlineMapper = vtkPolyDataMapper2D::New();
  outlineMapper->SetInput(this->OutlineSource->GetOutput());

  this->OutlineActor = vtkActor2D::New();
  this->OutlineActor->SetMapper(outlineMapper);

  if (this->CurrentRenderer != NULL)
  {
    this->CurrentRenderer->GetBackground(color);
  }
  color[0] = 1.0 - color[0];
  color[1] = 1.0 - color[1];
  color[2] = 1.0 - color[2];

  this->OutlineActor->GetProperty()->SetColor(color);
  this->OutlineActor->GetProperty()->SetLineStipplePattern(0xCCCC);

  if (this->CurrentRenderer != NULL)
  {
    this->CurrentRenderer->AddActor2D(this->OutlineActor);
  }
  this->OutlineSource->Delete();
  outlineMapper->Delete();

  this->StartPos[0] = x;
  this->StartPos[1] = y;

  this->PrevPos[0] = x;
  this->PrevPos[1] = y;

  this->StartState(VTKIS_OUTLINEZOOM);
  */

}
//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::StartOutlineZoom()
{

  /*********************************************************
   This code almost works, but shows a drift in the y axis
   commented until a fix is found.
  ********************************************************/

 /*
  // from VISAN
  vtkFloatingPointType color[3];
  vtkPolyDataMapper2D* outlineMapper;
  int x;
  int y;

  this->Interactor->GetEventPosition(x, y);
  this->FindPokedRenderer(x, y);
  y += ((double)this->ViewportSize[1]) / this->ZoomScale;


  vtkDebugMacro(<< "Starting Outline Zoom");

  if (this->State != VTKIS_START)
    {
    return;
    }

  this->OutlineSource = vtkOutlineSource::New();
  this->OutlineSource->SetBounds(x, x, y, y, 0, 0);

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

  this->StartPos[0] = x;
  this->StartPos[1] = y;

  this->PrevPos[0] = x;
  this->PrevPos[1] = y;


  this->StartState(VTKIS_OUTLINEZOOM);
  this->OutlineSource->Update();
  */

}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::EndOutlineZoom()
{

 /*********************************************************
   This code almost works, but shows a drift in the y axis
   commented until a fix is found.
  ********************************************************/

/*
   //from VISAN code
  vtkFloatingPointType bounds[6];

  if (this->State != VTKIS_OUTLINEZOOM)
    {
    return;
    }

  this->OutlineSource->GetBounds(bounds);

  if (bounds[0] != bounds[1] && bounds[2] != bounds[3])
    {
    double factor;
    double mPoint[2];
    double viewCenter[2];

    this->CurrentRenderer->RemoveActor2D(this->OutlineActor);
    this->OutlineActor->Delete();
    this->OutlineActor = NULL;

    // scale to fractions of the viewport
    bounds[0] /= this->ViewportSize[0];
    bounds[1] /= this->ViewportSize[0];
    bounds[2] /= this->ViewportSize[1];
    bounds[3] /= this->ViewportSize[1];

    // multiplicative scale length - both scales were constrained to be the same
    factor = 1.0 / fabs(bounds[1] - bounds[0]);

    // center of the zoom box in window fraction
    mPoint[0] = (bounds[0] + bounds[1]) / 2.0;
    mPoint[1] = (bounds[2] + bounds[3]) / 2.0;

    // compute the view center point (in normalized projection units)
    viewCenter[0] = 0.5 - (this->MidPoint[0] - mPoint[0]) / this->Size[0];
    viewCenter[1] = 0.5 - (this->MidPoint[1] - mPoint[1]) / this->Size[1];

    // resize (and adjust the Zoom)
    this->Size[0] *= factor;
    this->Size[1] *= factor;
    this->ZoomScale *= factor;

    this->SetViewMidPoint(viewCenter[0], viewCenter[1]);

    }
  this->StopState();

  // this has to go here
  this->InvokeEvent(vtkInteractorStyleWPlot::EndZoomEvent, &ZoomScale);
  */
}


//----------------------------------------------------------------------------
// code ported from VISAN
void vtkInteractorStyleWPlot::Pan()
{
  int x;
  int y;

  this->Interactor->GetEventPosition(x, y);

  if (x != this->PrevPos[0] || y != this->PrevPos[1])
  {
      double deltaX = x - this->PrevPos[0]; // +ve left
      double deltaY = y - this->PrevPos[1]; // +ve up
      double tmpMidpoint;
      bool updated = false;

      // change in terms of fractions of the visible range - this is the
      // intended increase in the mipoint.
      deltaX /= this->ViewportSize[0];
      deltaY /= this->ViewportSize[1];

      // only pan x when a point in the extent stays in the center of the window.
      tmpMidpoint = this->MidPoint[0];
      this->MidPoint[0] += deltaX;
      if (this->minX() > 0.5)
        this->MidPoint[0] -= this->minX() - 0.5;
      if (this->maxX() < 0.5)
        this->MidPoint[0] += 0.5 - this->maxX();
      updated = (tmpMidpoint != this->MidPoint[0]);

      // only pan y when y-border stays within window
      tmpMidpoint = this->MidPoint[1];
      this->MidPoint[1] += deltaY;
      if (this->minY() > 0.5)
        this->MidPoint[1] -= this->minY() - 0.5;
      if (this->maxY() < 0.5)
        this->MidPoint[1] += 0.5 - this->maxY();
      updated = (tmpMidpoint != this->MidPoint[1]);

      if (updated)
      {
          this->SetTransformation();
      }
      this->PrevPos[0] = x;
      this->PrevPos[1] = y;

  }

}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::OutlineZoom(int x, int y)
{
    return;
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::OutlineZoom()
{
 /*********************************************************
  This code almost works, but shows a drift in the y axis
  commented until a fix is found.
  ********************************************************/

/*
  int x;
  int y;

  this->Interactor->GetLastEventPosition(x, y);
  y += ((double)this->ViewportSize[1]) / 2.0;

  if (x != this->PrevPos[0] || y != this->PrevPos[1])
    {
    double fY = ((double)this->ViewportSize[0]) / this->ViewportSize[1];
    double dX = x - this->StartPos[0];
    double dY = y - this->StartPos[1];
    vtkFloatingPointType bounds[6];

    this->OutlineSource->GetBounds(bounds);

    if (fabs(dX / dY) > fY)
      {
      // the box should be larger in HEIGHT, so take x as base and calc y
      if (dY * dX < 0)
        {
        // dy is negative or dx is negative and dy is not (so: dx*fy is negative and shouldn't be
        dY = dX / fY;
        dY *= -1;
        }
      else
        {
        dY = dX / fY;
        }
      }
    else
      {
      // the box should be larger in WIDTH, so take y as base and calc x
      if (dY * dX < 0)
        {
        // dy is negative and dx is not (so: dy*fy is negative and shouldn't be
        dX = dY * fY;
        dX *= -1;
        }
      else
        {
        dX = dY * fY;
        }
      }

    bounds[1] = x;//bounds[0] + dX;
    bounds[3] = bounds[2] + dY;
    this->OutlineSource->SetBounds(bounds);
    this->OutlineSource->Update();

    //this->Interactor->Render();

    this->PrevPos[0] = x;
    this->PrevPos[1] = y;
    }
    */
}
//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::Zoom(int x, int y)
{
    return;

}

//----------------------------------------------------------------------------
// code ported from VISAN
void vtkInteractorStyleWPlot::Zoom()
{

  int x;
  int y;

    this->Interactor->GetEventPosition(x, y);

    if (y != this->StartPos[1])
    {

        double factor = 1.0 + ((double)(y - this->StartPos[1])) / this->ViewportSize[1];
        // clamp it
        if (factor > 1.25)
        {
            factor = 1.25;
        }
        else if (factor < 1.0/1.25)
        {
            factor = 1.0/1.25;
        }

        factor *= this->ZoomScale; // increase or decrease the zoom factor

        // make an attempt to preserve the view center point
        SetZoomScale(factor);
    }
}
//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::ZoomTo(int x1, int y1, int x2, int y2)
{

  // calculate the center bounded by these limits
  int xcenter = (x1 + x2)/2;
  int ycenter = (y1 + y2)/2;

  SetViewMidPoint(xcenter, ycenter);

}
//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::SetViewMidPoint(double x, double y)
{

  this->MidPoint[0] = 0.5 - this->Size[0] * (x - 0.5);
  this->MidPoint[1] = 0.5 - this->Size[1] * (y - 0.5);

  // clamp the view
  if (this->minX() > 0.5)
    this->MidPoint[0] -= this->minX() - 0.5;
  if (this->maxX() < 0.5)
    this->MidPoint[0] += 0.5 - this->maxX();
  if (this->minY() > 0.5)
    this->MidPoint[1] -= this->minY() - 0.5;
  if (this->maxY() < 0.5)
    this->MidPoint[1] += 0.5 - this->maxY();

  this->SetTransformation();

}
//---------------------------------------------------------------------------
void vtkInteractorStyleWPlot::GetViewMidPoint(double &x, double &y)
{
  x = 0.5 - (this->MidPoint[0] - 0.5) / this->Size[0];
  y = 0.5 - (this->MidPoint[1] - 0.5) / this->Size[1];
}

//----------------------------------------------------------------------------
//from VISAN code
void vtkInteractorStyleWPlot::SetZoomScale(double zoomS)
{
  double tmpX, tmpY;

  if (zoomS < 1.0)
    this->ZoomScale = 1.0;
  else
    this->ZoomScale = zoomS;

  if ( this->ZoomScale > 1e6 )
    this->ZoomScale = 1e6;


  // try and preserve the current window center
  tmpX = 0.5 + (0.5 - this->MidPoint[0]) / this->Size[0]; // current center in X
  tmpY = 0.5 + (0.5 - this->MidPoint[1]) / this->Size[1]; // current center in Y

  this->Size[0] = this->ZoomScale * this->RatioVector[0];
  this->Size[1] = this->ZoomScale * this->RatioVector[1];


  SetViewMidPoint(tmpX, tmpY);
  this->InvokeEvent(vtkInteractorStyleWPlot::EndZoomEvent, &ZoomScale);
}
//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::StartPan()
{
  int x;
  int y;

  if (this->State != VTKIS_START)
    {
    return;
    }

  vtkDebugMacro(<< "Starting Pan");

  this->Interactor->GetEventPosition(x, y);
  //y += this->ViewportSize[1];  //KAVOK

  this->PrevPos[0] = x;
  this->PrevPos[1] = y;

  this->StartState(VTKIS_PAN);
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::StartZoom(int x, int y)
{
    return;

  this->StartPos[0] = x;
  this->StartPos[1] = y;

  this->MapPointToMidPointTranslateVector(((double)x) / this->ViewportSize[0],
                                          ((double)y) / this->ViewportSize[1]);

}
//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::StartZoom()
{
  int x;
  int y;

  if (this->State != VTKIS_START)
    {
    return;
    }

  vtkDebugMacro(<< "Starting Zoom");

  this->Interactor->GetEventPosition(x, y);
  //y += this->ViewportSize[1]; // KAVOK
  //StartZoom(x, y);  //KAVOK

  this->StartPos[0] = x;
  this->StartPos[1] = y;

  this->StartState(VTKIS_ZOOM);

}
//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::EndZoom()
{
  vtkInteractorStyle::EndZoom();
  this->InvokeEvent(vtkInteractorStyleWPlot::EndZoomEvent, &ZoomScale);

}
//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::EndPan()
{
  vtkInteractorStyle::EndPan();
  this->InvokeEvent(vtkInteractorStyleWPlot::EndPanEvent, NULL);

}
//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::OnChar()
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
    case 'r':
      // Reset the pan and zoom values
      // Only allow resetting if we are not panning/zooming.
      if (this->State == VTKIS_START)
        {

        if ( this->OutlineActor != NULL )
        {
            this->CurrentRenderer->RemoveActor2D(this->OutlineActor);
            this->OutlineActor->Delete();
            this->OutlineActor = NULL;
        }

        this->Size[0] = this->RatioVector[0];
        this->Size[1] = this->RatioVector[1];
        this->MidPoint[0] = 0.5;
        this->MidPoint[1] = 0.5;

        //KAVOK: set same ZoomScale as in VISAN
        this->SetZoomScale(1.0);

        this->SetTransformation();

        this->Interactor->Render();

        // this is not elegant, but how else can I force the pipeline to be called again?
        EndPan();

        }

      break;
    case 'U':
    case 'u':
      this->Interactor->UserCallback();
      break;
    default:
      break;
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::OnTimer()
{
  vtkRenderWindowInteractor *rwi = this->Interactor;

  switch (this->State)
    {
    case VTKIS_PAN:
      this->Pan();
      rwi->CreateTimer(VTKI_TIMER_UPDATE);
      break;
    case VTKIS_ZOOM:
      this->Zoom();
      rwi->CreateTimer(VTKI_TIMER_UPDATE);
      break;
    case VTKIS_OUTLINEZOOM:
      this->OutlineZoom();
      rwi->CreateTimer(VTKI_TIMER_UPDATE);
    case VTKIS_TIMER:
      rwi->Render();
      rwi->CreateTimer(VTKI_TIMER_UPDATE);
      break;
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::OnLeftButtonDown()
{
  if (this->Interactor->GetControlKey())
    {
    this->StartOutlineZoom();
    }
  else
    {
    this->StartPan();
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::OnLeftButtonUp()
{

  switch (this->State)
  {
    case VTKIS_OUTLINEZOOM:
      this->EndOutlineZoom();
      break;

    case VTKIS_PAN:
      this->EndPan();
      break;
  }

}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::OnRightButtonDown()
{
  this->StartZoom();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::OnRightButtonUp()
{
  this->EndZoom();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::OnMiddleButtonDown()
{
  this->StartOutlineZoom();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::OnMiddleButtonUp()
{
  this->EndOutlineZoom();
}

//----------------------------------------------------------------------------
vtkWPlotProjectionState* vtkInteractorStyleWPlot::GetProjState(int32_t projection)
{
  string projName = CMapProjection::GetInstance()->IdToName(projection);

  vtkWPlotProjectionState* projState = dynamic_cast<vtkWPlotProjectionState*>(this->ProjStates.Exists(projName));
  if (projState == NULL)
  {
    projState = vtkWPlotProjectionState::New();
    this->ProjStates.Insert(projName, projState);
  }

  return projState;
}
//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::SaveState(int32_t projection)
{
  string projName = CMapProjection::GetInstance()->IdToName(projection);

  vtkWPlotProjectionState* projState = this->GetProjState(projection);

  projState->SaveState(this);

}
//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::RestoreState(int32_t projection)
{
  vtkWPlotProjectionState* projState = this->GetProjState(projection);

  projState->RestoreState(this);

  this->SetTransformation();

  this->InvokeEvent(vtkInteractorStyleWPlot::EndRestoreStateEvent, NULL);


}
//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::RestoreState(int32_t projection, int32_t index)
{
  vtkWPlotProjectionState* projState = this->GetProjState(projection);

  projState->RestoreState(this, index);

  this->SetTransformation();

  this->InvokeEvent(vtkInteractorStyleWPlot::EndRestoreStateEvent, NULL);

}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::ClearStates(int32_t projection)
{
  vtkWPlotProjectionState* projState = this->GetProjState(projection);

  projState->ClearStates();

}
//----------------------------------------------------------------------------
vtkWPlotState* vtkInteractorStyleWPlot::GetState(int32_t projection, int32_t index)
{
  vtkWPlotProjectionState* projState = this->GetProjState(projection);

  return projState->GetState(index);
}

//----------------------------------------------------------------------------
bool vtkInteractorStyleWPlot::HasState(int32_t projection)
{
  return CountState(projection) > 0;
}
//----------------------------------------------------------------------------
uint32_t vtkInteractorStyleWPlot::CountState(int32_t projection)
{
  vtkWPlotProjectionState* projState = this->GetProjState(projection);

  return projState->CountState();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleWPlot::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkInteractorStyle::PrintSelf(os,indent);

  os << indent << "MidPoint: (" << this->MidPoint[0] << ", "
     << this->MidPoint[1] << ")" << endl;
  os << indent << "PrevPos: (" << this->PrevPos[0] << ", "
     << this->PrevPos[1] << ")" << endl;
  os << indent << "RatioVector: (" << this->RatioVector[0] << ", "
     << this->RatioVector[1] << ")" << endl;
  os << indent << "Size: (" << this->Size[0] << ", "
     << this->Size[1] << ")" << endl;
  os << indent << "StartPos: (" << this->StartPos[0] << ", "
     << this->StartPos[1] << ")" << endl;
  os << indent << "ViewportSize: (" << this->ViewportSize[0] << ", "
     << this->ViewportSize[1] << ")" << endl;
  //KAVOK
  os << indent << "ZoomScale: (" << this->ZoomScale << endl;
}
