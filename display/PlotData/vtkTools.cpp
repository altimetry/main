/*
* 
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

#include "libbrathl/Tools.h"
using namespace brathl;

#if defined (__unix__)
#if defined (__DEPRECATED)          //avoid linux warning in vtk include
#undef __DEPRECATED
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif	// __unix__

#include "vtkTools.h"

#include "vtkByteSwap.h"

const int vtkTools::VTK_LEFT = -1;
const int vtkTools::VTK_MIDDLE = 0;
const int vtkTools::VTK_RIGHT = 1;


//----------------------------------------------------------------------------
vtkTools::vtkTools()
{

}
//----------------------------------------------------------------------------
vtkTools::~vtkTools()
{

} 
//----------------------------------------------------------------------------
void vtkTools::DeleteObject(vtkObject* vtkOb)
{
  if (vtkOb != NULL)
    {
    vtkOb->Delete();
    }

}

//----------------------------------------------------------------------------
void vtkTools::DataToViewport(double &x,
                                 double viewportBounds[2],
                                 double dataBounds[2], int logX)
{
  if (viewportBounds[0]<viewportBounds[1])
    {
    if (dataBounds[0]==dataBounds[1])
      {
      x = (viewportBounds[0]+viewportBounds[1])/2;
      }
    else
      {
      if (logX)
        { // Use logarithmic mapping
        if (dataBounds[0]>0 && dataBounds[1]>0)
          {
          x = viewportBounds[0] + (viewportBounds[1] - viewportBounds[0]) *
            (log(x) - log(dataBounds[0])) /
            (log(dataBounds[1]) - log(dataBounds[0]));
          }
        }
      else
        { // Use linear mapping
        x = viewportBounds[0] + (viewportBounds[1] - viewportBounds[0]) *
          (x - dataBounds[0]) / (dataBounds[1] - dataBounds[0]);
        }
      }
    }
}

//----------------------------------------------------------------------------
void vtkTools::ViewportToData(double &x,
                                 double viewportBounds[2],
                                 double dataBounds[2], int logX)
{
  if (viewportBounds[0]==viewportBounds[1] || dataBounds[0]==dataBounds[1])
    {
    x = dataBounds[0];
    }
  else if (viewportBounds[0]<viewportBounds[1])
    {
    if (logX)
      { // Use logarithmic mapping
      if (dataBounds[0]>0 && dataBounds[1]>0)
        {
        x = exp(log(dataBounds[0]) +
          (log(dataBounds[1]) - log(dataBounds[0])) *
          (x - viewportBounds[0]) / (viewportBounds[1] - viewportBounds[0]));
        }
      }
    else
      { // Use linear mapping
      x = dataBounds[0] + (dataBounds[1] - dataBounds[0]) *
        (x - viewportBounds[0]) / (viewportBounds[1] - viewportBounds[0]);
      }
    }
}

//----------------------------------------------------------------------------

int vtkTools::IsNaN(const double x)
{
  if (isDefaultValue(x))
  {
    return 1;
  }

  unsigned int e_mask, f_mask;
  double as_double = x;
  unsigned int *as_int;

  // We match on the higher 4 bytes of the double
  vtkByteSwap::Swap8BE(&as_double);
  as_int = (unsigned int *)&as_double;
  vtkByteSwap::Swap4BE(as_int);

  e_mask = 0x7ff0;
  e_mask <<= 16;

  if ((*as_int & e_mask) != e_mask)
    {
    return 0;       /* e != 2047 */
    }

  f_mask = 1;
  f_mask <<= 20;
  f_mask--;

  /* number is NaN if f does not equal zero. */
  return (*as_int & f_mask) != 0;
}
//----------------------------------------------------------------------------
// Determine whether the value is a valid number
int vtkTools::IsInvalid(const double x, const int log)
{
  if (vtkTools::IsNaN(x))
    {
    return 1;
    }
  if (log && x <= 0)
    {
    return 1;
    }
  return 0;
}
//----------------------------------------------------------------------------
// Computes the instersection of the line starting in p1 and ending in p2
// with the bounding box defined by bounds (=[xmin,xmax,ymin,ymax,zmin,zmax])
// If the line intersects with the bounding box the function returns a non-zero
// value and the intersection point is stored in 'intersectionPoint'.
int vtkTools::ComputeIntersection(double bounds[6],
                                double p1[3],
                                double p2[3],
                                double intersectionPoint[3])
{
  double direction[3];
  double t;
  double maxT[3];
  double candidatePlane[3];
  int quadrant[3];
  int thePlane = 0;
  int inside = 1;
  int i;

  direction[0] = p2[0] - p1[0];
  direction[1] = p2[1] - p1[1];
  direction[2] = p2[2] - p1[2];

  // Find closest planes
  for (i = 0; i < 3; i++) 
    {
    if (p1[i] < bounds[2 * i]) 
      {
      quadrant[i] = vtkTools::VTK_LEFT;
      candidatePlane[i] = bounds[2 * i];
      inside = 0;
      }
    else if (p1[i] > bounds[2 * i + 1]) 
      {
      quadrant[i] = vtkTools::VTK_RIGHT;
      candidatePlane[i] = bounds[2 * i + 1];
      inside = 0;
      }
    else 
      {
      quadrant[i] = vtkTools::VTK_MIDDLE;
      }
    }

  // Check whether origin of ray is inside bbox
  if (inside) 
    {
    intersectionPoint[0] = p1[0];
    intersectionPoint[1] = p1[1];
    intersectionPoint[2] = p1[2];
    t = 0;
    return 1;
    }
  
  // Calculate parametric distances to plane
  for (i = 0; i < 3; i++)
    {
    if (quadrant[i] != VTK_MIDDLE && direction[i] != 0.0)
      {
      maxT[i] = (candidatePlane[i] - p1[i]) / direction[i];
      }
    else
      {
      maxT[i] = -1.0;
      }
    }

  // Find the largest parametric value of intersection
  for (i = 0; i < 3; i++)
    {
    if (maxT[thePlane] < maxT[i])
      {
      thePlane = i;
      }
    }

  // Check for valid intersection along line
  if (maxT[thePlane] > 1.0 || maxT[thePlane] < 0.0)
    {
    return 0;
    }
  else
    {
    t = maxT[thePlane];
    }

  // Intersection point along line is okay.  Check bounding box.
  for (i = 0; i < 3; i++) 
    {
    if (thePlane != i) 
      {
      intersectionPoint[i] = p1[i] + maxT[thePlane] * direction[i];
      if (intersectionPoint[i] < bounds[2 * i] ||
          intersectionPoint[i] > bounds[2 * i + 1])
        {
        return 0;
        }
      } 
    else 
      {
      intersectionPoint[i] = candidatePlane[i];
      }
    }

  return 1;
}
