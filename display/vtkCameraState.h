/*
* Copyright (C) 2005 Collecte Localisation Satellites (CLS), France (31)
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

#ifndef __vtkCamera_H__
#define __vtkCamera_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "vtkCameraState.h"
#endif


#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "BratObject.h"
using namespace brathl;

#include "vtkCamera.h"

//-------------------------------------------------------------
//------------------- vtkCameraState class --------------------
//-------------------------------------------------------------
class VTK_EXPORT vtkCameraState : public vtkObject
{
public:
  static vtkCameraState *New();
  vtkTypeMacro(vtkCameraState, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetVector3Macro(ViewUp,double);
  vtkGetVector3Macro(ViewUp,double);

  vtkSetVector3Macro(FocalPoint,double);
  vtkGetVector3Macro(FocalPoint,double);

  vtkSetVector3Macro(Position,double);
  vtkGetVector3Macro(Position,double);

  vtkSetVector2Macro(ClippingRange, double);
  vtkGetVector2Macro(ClippingRange, double);

  vtkSetMacro(ViewAngle, double);
  vtkGetMacro(ViewAngle, double);

  vtkSetMacro(ParallelScale, double);
  vtkGetMacro(ParallelScale, double);

protected:

  vtkCameraState();
  virtual ~vtkCameraState();

protected:
  double ViewUp[3];
  double FocalPoint[3];
  double Position[3];
  double ClippingRange[2];
  double ViewAngle;
  double ParallelScale;

};
#endif
