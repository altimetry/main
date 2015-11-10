/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInteractorStyleXYPlot.h
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
#ifndef __vtkInteractorStyleXYPlot_h
#define __vtkInteractorStyleXYPlot_h

#include "vtkInteractorStyle.h"

class vtkActor2D;
class vtkIndent;
class vtkOutlineSource;
class vtkRenderer;
class vtkXYPlotActor;

// This interactorstyle only uses VTKIS_NONE, VTKIS_ZOOM, and VTKIS_PAN
// and has an aditional mode VTKIS_OUTLINEZOOM

// Define XYPlotActor region types
#define VTKIS_XYPLOT_PLOT_AREA     0
#define VTKIS_XYPLOT_X_AXIS        1
#define VTKIS_XYPLOT_Y_AXIS        2
#define VTKIS_XYPLOT_TITLE         3
#define VTKIS_XYPLOT_LEGEND        4

// Define our own VTKIS types
#define VTKIS_OUTLINEZOOM        100

class VTK_EXPORT vtkInteractorStyleXYPlot : public vtkInteractorStyle
{
public:
  static vtkInteractorStyleXYPlot *New();
  vtkTypeMacro(vtkInteractorStyleXYPlot, vtkInteractorStyle);
  void PrintSelf(std::ostream& os, vtkIndent indent);

  // Description:
  // Event bindings controlling the effects of pressing mouse buttons
  // or moving the mouse.
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();
  virtual void OnMiddleButtonDown();
  virtual void OnMiddleButtonUp();
  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  virtual void OnChar();
  virtual void OnTimer(void);

protected:
  vtkInteractorStyleXYPlot();
  virtual ~vtkInteractorStyleXYPlot();

  void Pan();
  void OutlineZoom();
  void Zoom();
 
  void StartPan();
  void StartOutlineZoom();
  void StartZoom();
  void EndOutlineZoom();

  void FindPokedXYPlotActor(int x, int y);

protected:
  vtkXYPlotActor*       CurrentXYPlotActor;
  int                   CurrentXYPlotElement;

  vtkOutlineSource*     OutlineSource;
  vtkActor2D*           OutlineActor;

  double  StartPos[2]; // viewport coor. for pan, data coor. for zoom
  double  PrevPos[2];
  double  ViewportBounds[4];

private:
  vtkInteractorStyleXYPlot(const vtkInteractorStyleXYPlot&);  // Not implemented.
  void operator=(const vtkInteractorStyleXYPlot&);  // Not implemented.
};

#endif
