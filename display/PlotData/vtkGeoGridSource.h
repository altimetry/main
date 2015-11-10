/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGeoGridSource.h
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
#ifndef __vtkGeoGridSource_h
#define __vtkGeoGridSource_h

#include "vtkPolyDataSource.h"
#include "vtkPolyLine.h"
#include <map>
#include <list>

enum GridLineType { GRIDLINE_LAT = 1, GRIDLINE_LONG };

typedef struct {

    double val;
    double id;
    GridLineType lineType;


} GeoGridLineInfo;


class VTK_EXPORT vtkGeoGridSource : public vtkPolyDataSource
{
public:
  vtkTypeMacro(vtkGeoGridSource, vtkPolyDataSource);
  static vtkGeoGridSource *New() { return new vtkGeoGridSource; }
  void PrintSelf(std::ostream& os, vtkIndent indent);

  // Description:
  // Sets the distance (in degrees) between gridlines. The same distance
  // (graticule) is used for meridians and parallels. The default is 30
  // degrees between gridlines.
  // The graticule should be a divisor of 180 or you may get unexpected
  // results.
  vtkSetClampMacro(LongGraticule, double, 0.00001, 30);
  vtkGetMacro(LongGraticule, double);

  vtkSetClampMacro(LatGraticule, double, 0.00001, 30);
  vtkGetMacro(LatGraticule, double);

  // Description:
  // Sets the distance (in degrees) between the points making up a gridline.
  // The default is 1 degree between points.
  // The point distance should be a divisor of 180 or you may get unexpected
  // results.
  vtkSetClampMacro(PointDistance, double, 0.001, 90);
  vtkGetMacro(PointDistance, double);

  // Description:
  // This option determines whether the poles (+90 latitude and -90 latitude)
  // should have parallels.
  // If the graticule is not a divisor of 90 this options will be ignored
  // (since in that case a pole wouldn't have a parallel anyway).
  // By default parallels for poles are created.
  vtkSetMacro(CreateParallelsForPoles, int);
  vtkGetMacro(CreateParallelsForPoles, int);
  vtkBooleanMacro(CreateParallelsForPoles, int);

  // Description:
  // Used to tell what part of the global grid to generate
  // when zooming in, too many grid lines may be un-allocatable
  // in memory
  vtkSetClampMacro(InitLat, double, -90.0, 90.0);
  vtkGetMacro(InitLat, double);
  vtkSetClampMacro(EndLat, double, -90.0, 90.0);
  vtkGetMacro(EndLat, double);
  vtkSetClampMacro(InitLong, double, -180.0, 180.0);
  vtkGetMacro(InitLong, double);
  vtkSetClampMacro(EndLong, double, -180.0, 180.0);
  vtkGetMacro(EndLong, double);
  vtkSetClampMacro(MidLong, double, -180.0, 180.0);
  vtkGetMacro(MidLong, double);
  vtkSetClampMacro(MidLat, double, -180.0, 180.0);
  vtkGetMacro(MidLat, double);


  // Description:
  // This option determines whether the limit magnitude of the latitude
  // By default full latitute is 180
  vtkSetClampMacro(LatitudeLimitMagnitude, double, 90, 180);
  vtkGetMacro(LatitudeLimitMagnitude, double);

  vtkPolyData* FindData(double x, double y);
  static void Test(vtkObject* obj, unsigned long, void*, void*);

  GeoGridLineInfo GetGridLineInfo(double);


protected:
  vtkGeoGridSource();
  virtual ~vtkGeoGridSource();
  void Execute();

  double InitLat, EndLat;
  double InitLong, EndLong;
  double MidLat, MidLong;
  double LatitudeLimitMagnitude;
  double LongGraticule, LatGraticule;
  double PointDistance;
  int CreateParallelsForPoles;

  std::map<double, GeoGridLineInfo> gridLinesInfo;

private:
  vtkGeoGridSource(const vtkGeoGridSource&);  // Not implemented.
  void operator=(const vtkGeoGridSource&);  // Not implemented.
};

double convert(double val, double period);
double min_angle(double val1, double val2, double period);
int between(double dlimit, double ulimit, double val, double period);

#endif
