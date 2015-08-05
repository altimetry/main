/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkProj2DFilter.h
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
#ifndef __vtkProj2DFilter_h
#define __vtkProj2DFilter_h

#include "vtkPolyDataToPolyDataFilter.h"
#include  "vtkDoubleArray.h"

// (Pseudo) Cylindrical projections:
// - Lambert Cylindrical Equal Area
#define VTK_PROJ2D_LAMBERT_CYLINDRICAL      1
// - Plate Caree
#define VTK_PROJ2D_PLATE_CAREE              2
// - Mollweide
#define VTK_PROJ2D_MOLLWEIDE                3
// - Robinson
#define VTK_PROJ2D_ROBINSON                 4

// Azimuthal projections:
// - Lambert Azimuthal
#define VTK_PROJ2D_LAMBERT_AZIMUTHAL        5
// - Azimuthal Equidistant
#define VTK_PROJ2D_AZIMUTHAL_EQUIDISTANT    6

// 3D projections
#define VTK_PROJ2D_3D                       7

// Mercator projections
#define VTK_PROJ2D_MERCATOR                 8

// Apian Globular
#define VTK_PROJ2D_ORTHO                    9

// Near-Sighted Perspective
#define VTK_PROJ2D_NEAR_SIGHTED             10

// Gnomonic Projection also called central or azimuthal centrographic
#define VTK_PROJ2D_STEREOGRAPHIC            11

// transverse mercator
#define VTK_PROJ2D_TMERCATOR            12

class VTK_EXPORT vtkProj2DFilter : public vtkPolyDataToPolyDataFilter
{
public:
  vtkTypeMacro(vtkProj2DFilter, vtkPolyDataToPolyDataFilter);
  static vtkProj2DFilter *New() { return new vtkProj2DFilter; }
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // The type of projections that needs to be performed
  // Valid projections are:
  // - Lambert Cylindrical Equal Area
  // - Plate Caree
  // - Mollweide
  // - Robinson
  // - Lambert Azimuthal
  // - Apian Globular
  // - 3D
  // The default projection is the Plate Caree projection
  vtkSetClampMacro(Projection, int, 1, 11);
  vtkGetMacro(Projection, int);

  // Description:
  // The latitude value in degrees of the center position of the map.
  // The default is 0 degrees.
  // This property is not used for some projections.
  vtkSetClampMacro(CenterLatitude, double, -90, 90);
  vtkGetMacro(CenterLatitude, double);

  // Description:
  // The longitude value in degrees of the center position of the map.
  // The default is 0 degrees.
  // This property is not used for some projections.
  vtkSetClampMacro(CenterLongitude, double, -180, 180);
  vtkGetMacro(CenterLongitude, double);

  // Description:
  // Geographical positions that can not be properly mapped by a certain
  // projection will be moved a tiny amount to bring it back on the map.
  // The amount such points are shifted is determined by the value of Eps.
  // The default value of Eps is 0.00001.
  // The unit of Eps is 'degrees'.
  vtkSetMacro(Eps, double);
  vtkGetMacro(Eps, double);

  // Description:
  // When the distance between two points for a line or poly in the projected
  // space is larger than this value then one or more intermediate points will
  // be inserted in order to reduce the distance between consecutive points.
  // The default value of InterpolationDistance is 0.005.
  // For cylindrical and azimuthal projections the distance is the carthesian
  // distance within the projected space, which has an extent of [0, 1]x[0, 1].
  // For 3D projections the distance is the arcdistance between two points (in
  // degrees) divided by 360 (so the default InterpolationDistance is 0.005 x
  // 360 = 1.8 degrees). The radius of 3D projected points is ignored when
  // calculating the distance (i.e. distances are taken along a unit sphere).
  vtkSetMacro(InterpolationDistance, double);
  vtkGetMacro(InterpolationDistance, double);

  // Description:
  // When using an Azimuthal projection some polygons that lie near the
  // cutting point may be triangulated incorrectly by VTK. To remove such
  // polygons for every polygon all points are measured against the cutting
  // point. If the arc distance (in degrees) between the farthest point in the
  // polygon and the cutting point is below the AzimuthalIgnorePolyDistance then
  // the polygon is not included in the output.
  // The default value of AzimuthalIgnorePolyDistance is 7 (degrees).
  vtkSetMacro(AzimuthalIgnorePolyDistance, double);
  vtkGetMacro(AzimuthalIgnorePolyDistance, double);

  // Description:
  // Get X/Y ratio of the current projection.
  double GetXYRatio();
  // KAVOK: added from VISAN code
  static void normalizedDeprojection2D(int projection, double centerLat, double centerLon,
                                             double x, double y, double &lat, double &lon);
  static void normalizedProjection2D(int projection, double centerLat, double centerLon,
                                           double lat, double lon, double &x, double &y);
  static void calcCenterLatLon(int projection, double centerLat, double centerLon,
                                           double lat, double lon, double &x, double &y);

protected:
  vtkProj2DFilter();
  virtual ~vtkProj2DFilter() {};
  virtual void Execute();

  static void GetExtent(int projection, double extent[6]);  //change by KAVOK
  void Perform3DProjection();
  void PerformAzimuthalProjection();
  void PerformCylindricalProjection();
  void PerformSemiProjection();

  double Eps;
  int Projection;
  double CenterLatitude;
  double CenterLongitude;
  double InterpolationDistance;
  double AzimuthalIgnorePolyDistance;
private:
  vtkProj2DFilter(const vtkProj2DFilter&);  // Not implemented.
  void operator=(const vtkProj2DFilter&);  // Not implemented.
};


// will not interpolate or create or cut anything
// will just transform points to the projection
// and convert the vector data accordingly
class vtkSimpleVectorProjFilter : public vtkProj2DFilter {

public:
  vtkTypeMacro(vtkSimpleVectorProjFilter, vtkPolyDataToPolyDataFilter);
  static vtkSimpleVectorProjFilter *New() { return new vtkSimpleVectorProjFilter; }

protected:
    vtkSimpleVectorProjFilter();
    virtual ~vtkSimpleVectorProjFilter() {};
    void Execute();

    void PerformSimple2D();
    void PerformSimple3D();

};

/*
class vtkSimpleLineProjFilter : public vtkProj2DFilter {

public:
  vtkTypeMacro(vtkSimpleLineProjFilter, vtkPolyDataToPolyDataFilter);
  static vtkSimpleLineProjFilter *New() { return new vtkSimpleLineProjFilter; }

protected:
    vtkSimpleLineProjFilter();
    virtual ~vtkSimpleLineProjFilter() {};
    void Execute();

    void PerformSimple2D();
    void PerformSimple3D();

};
*/

#endif
