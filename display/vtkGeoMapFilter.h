/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGeoMapSource.h
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
#ifndef __vtkGeoMapFilter_h
#define __vtkGeoMapFilter_h

#include "vtkPolyDataSource.h"

class vtkCellArray;
class vtkDoubleArray;
class vtkPoints;
class vtkPolyData;
class vtkShortArray;

class VTK_EXPORT vtkGeoMapFilter : public vtkPolyDataSource
{
public:
  vtkTypeMacro(vtkGeoMapFilter,vtkPolyDataSource);
  void PrintSelf(ostream& os, vtkIndent indent);
  static vtkGeoMapFilter *New();

  virtual void SetValues(vtkDoubleArray*);
  vtkGetObjectMacro(Values,vtkDoubleArray);

  virtual void SetHeights(vtkDoubleArray*);
  vtkGetObjectMacro(Heights,vtkDoubleArray);

  virtual void SetBitarray(vtkShortArray*);
  vtkGetObjectMacro(Bitarray,vtkShortArray);

  virtual void SetValidMercatorLatitudes(vtkShortArray*);
  vtkGetObjectMacro(ValidMercatorLatitudes,vtkShortArray);

  virtual void SetLongitudes(vtkDoubleArray*);
  vtkGetObjectMacro(Longitudes,vtkDoubleArray);

  virtual void SetLatitudes(vtkDoubleArray*);
  vtkGetObjectMacro(Latitudes,vtkDoubleArray);

  vtkSetMacro(OffsetLatitude,double);
  vtkGetMacro(OffsetLatitude,double);

  vtkSetMacro(OffsetLongitude,double);
  vtkGetMacro(OffsetLongitude,double);

  vtkSetMacro(MinMappedValue,double);
  vtkGetMacro(MinMappedValue,double);

  vtkSetMacro(MaxMappedValue,double);
  vtkGetMacro(MaxMappedValue,double);

  vtkSetMacro(Factor,double);
  vtkGetMacro(Factor,double);

  vtkSetMacro(Radius,double);
  vtkGetMacro(Radius,double);

  vtkSetMacro(MapWidth,int);
  vtkGetMacro(MapWidth,int);

  vtkSetMacro(MapHeight,int);
  vtkGetMacro(MapHeight,int);

  vtkSetMacro(Projection,int);
  vtkGetMacro(Projection,int);

protected:
  vtkGeoMapFilter();
  virtual ~vtkGeoMapFilter();

  bool IsValidPoint(int32_t index);

  virtual void Execute();

//protected:
 private:

  vtkDoubleArray*       Values;

protected:
  vtkDoubleArray*       Heights;
  vtkDoubleArray*       Longitudes;
  vtkDoubleArray*       Latitudes;
  vtkShortArray*        Bitarray;
  vtkShortArray*        ValidMercatorLatitudes;

  double                OffsetLatitude;
  double                OffsetLongitude;

  double                MinMappedValue;
  double                MaxMappedValue;

  double                Factor;
  double                Radius;

  int                   MapWidth;
  int                   MapHeight;

  int                   Projection;
};

#endif
