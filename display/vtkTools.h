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

#ifndef __vtkTools_h
#define __vtkTools_h

#include "vtkObject.h"

class vtkTools 
{
public:

  static void DeleteObject(vtkObject* vtkOb);

  // Description:
  // Calculate the viewport coordinate in one dimension for a datapoint.
  // viewportBounds is one dimension of the area in which the plotdata is
  // drawn (the area between te axes) in viewport coordinates.
  // dataBounds[0] is the data value at viewportBounds[0] and dataBounds[1]
  // the data value at viewportBounds[1].
  // If viewportBounds[0]>viewportBounds[1] the coordinate is left unchanged.
  // If dataBounds[0]==dataBounds[1] the center of the viewportBounds is
  // returned.
  // If logX is true and dataBounds[0]<=0 or dataBounds[1]<=0 then the
  // coordinate is left unchanged.
  static void DataToViewport(double &x,
                             double viewportBounds[2],
                             double dataBounds[2], int logX);

  static void DataToViewport2D(double xy[2],
                               double viewportBounds[4],
                               double dataBounds[4], int logXY[2])
  {
    vtkTools::DataToViewport(xy[0], viewportBounds, dataBounds, logXY[0]);
    vtkTools::DataToViewport(xy[1], &(viewportBounds[2]), &(dataBounds[2]), logXY[1]);
  }

  static void DataToViewport3D(double xyz[3],
                               double viewportBounds[6],
                               double dataBounds[6],
                               int logXYZ[3])
  {
    vtkTools::DataToViewport(xyz[0], viewportBounds, dataBounds, logXYZ[0]);
    vtkTools::DataToViewport(xyz[1], &(viewportBounds[2]), &(dataBounds[2]), logXYZ[1]);
    vtkTools::DataToViewport(xyz[2], &(viewportBounds[4]), &(dataBounds[4]), logXYZ[2]);
  }

  // Description:
  // Calculate one dimension of viewport coordinate to datapoint mapping.
  // ViewportBounds is one dimension of the area in which the plotdata is
  // drawn (the area between te axes) in viewport coordinates.
  // dataBounds[0] is the data value at viewportBounds[0] and dataBounds[1]
  // the data value at viewportBounds[1].
  // If viewportBounds[0]>viewportBounds[1] the datapoint is left unchanged.
  // If viewportBounds[0]==viewportBounds[1] or dataBounds[0]==dataBounds[1]
  // then x will be set to the value of dataBounds[0].
  // If logX is true and dataBounds[0]<=0 or dataBounds[1]<=0 then the
  // datapoint is left unchanged.
  static void ViewportToData(double &x,
                             double viewportBounds[2],
                             double dataBounds[2], int logX);

  static void ViewportToData2D(double xy[2],
                               double viewportBounds[4],
                               double dataBounds[4], int logXY[2])
  {
    ViewportToData(xy[0], viewportBounds, dataBounds, logXY[0]);
    ViewportToData(xy[1], &(viewportBounds[2]), &(dataBounds[2]), logXY[1]);
  }

  static void ViewportToData3D(double xyz[3],
                               double viewportBounds[6],
                               double dataBounds[6],
                               int logXYZ[3])
  {
    ViewportToData(xyz[0], viewportBounds, dataBounds, logXYZ[0]);
    ViewportToData(xyz[1], &(viewportBounds[2]), &(dataBounds[2]), logXYZ[1]);
    ViewportToData(xyz[2], &(viewportBounds[4]), &(dataBounds[4]), logXYZ[2]);
  }

  static int IsNaN(const double x);
  static int IsInvalid(const double x, const int log);

  static int ComputeIntersection(double bounds[6],
                                double p1[3],
                                double p2[3],
                                double intersectionPoint[3]);

public:
  static const int VTK_LEFT;
  static const int VTK_MIDDLE;
  static const int VTK_RIGHT;


protected:
  vtkTools();
  ~vtkTools();

private:

  vtkTools(const vtkTools&);  // Not implemented.
  void operator=(const vtkTools&);  // Not implemented.
};


#endif
