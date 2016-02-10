/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkProj2DFilter.cxx
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
#if defined (__unix__)
#if defined (__DEPRECATED)          //avoid linux warning in vtk include
#undef __DEPRECATED
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif	// __unix__

#if defined (_WIN32) || defined (WIN32) //avoid collisions with QGIS projPJ
#define projPJ projPJ2
#endif

#include "vtkProj2DFilter.h"

#include <cmath>
#include <cstring>

#include "libbrathl/Tools.h"
using namespace brathl;

#include "vtkCell.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkIdList.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"

// Remove conflict with windows system header files
#define PVALUE PROJ_PVALUE

#include "projects.h"

// Find the arc distance in degrees between two points p and q, where p and q
// are given in (x,y,z) coordinates
static double arcdistancexyz(double px, double py, double pz, double qx,
                             double qy, double qz)
{
  double R = sqrt((px * px + py * py + pz * pz) * (qx * qx + qy * qy + qz * qz));
  if (R == 0)
    {
    // If either or both p and q are (0,0,0) then return 0
    return 0;
    }

  // the arc-distance is the arccos of the normalized inproduct of p and q
  return RAD_TO_DEG * acos((px * qx + py * qy + pz * qz) / R);
}

// Find the arc distance in degrees between two points p and q, where p and q
// are given in (longitude,latitude) angles (phi,tau)
static double arcdistance(double phi_p, double tau_p, double phi_q,
                          double tau_q)
{
  // pp = phi_p (longitude)
  // tp = tau_p (latitude)
  // pq = phi_q (longitude)
  // tq = tau_q (latitude)

  double pp = phi_p * DEG_TO_RAD;
  double tp = tau_p * DEG_TO_RAD;
  double pq = phi_q * DEG_TO_RAD;
  double tq = tau_q * DEG_TO_RAD;

  double cpp = cos(pp);
  double spp = sin(pp);
  double ctp = cos(tp);
  double stp = sin(tp);
  double cpq = cos(pq);
  double spq = sin(pq);
  double ctq = cos(tq);
  double stq = sin(tq);

  double px = cpp * ctp;
  double py = spp * ctp;
  double pz = stp;

  double qx = cpq * ctq;
  double qy = spq * ctq;
  double qz = stq;

  // the arc-distance is the arccos of the inproduct of p and q
  return RAD_TO_DEG * acos(px * qx + py * qy + pz * qz);
}

// calculates the midpoint over the great circle, or shortest path
static void midpoint(double phi_p, double tau_p, double phi_q,
                           double tau_q, double *phi_u, double *tau_u){

  // pp = phi_p (longitude)
  // tp = tau_p (latitude)
  // pq = phi_q (longitude)
  // tq = tau_q (latitude)


  double pp = phi_p * DEG_TO_RAD;
  double tp = tau_p * DEG_TO_RAD;
  double pq = phi_q * DEG_TO_RAD;
  double tq = tau_q * DEG_TO_RAD;

  double dLon = pq - pp;

  double Bx = cos(tq) * cos(dLon);
  double By = cos(tq) * sin(dLon);

  *tau_u = RAD_TO_DEG * atan2(sin(tp) + sin(tq),
                           sqrt( (cos(tp)+Bx)*(cos(tp)+Bx) + (By*By) ));
  *phi_u = RAD_TO_DEG * ( pp + atan2(By, cos(tp) + Bx) );

}

static inline int isVisible(double phi_p, double tau_p){
   return !(phi_p == HUGE_VAL || tau_p == HUGE_VAL);
}

static inline int isVisible(double point[3]){
    return isVisible(point[0], point[1]);
}

// Calculate the longitude (phi_u) and latitude (tau_u) of the point u
// that lies on the shortest path through p and q. Goes through the
// projection via binary, recursive search.
// The projection library has to be initialized BEFORE calling this
// function
static double cuttingpoint(double phi_p, double tau_p, double phi_q,
                           double tau_q, double *phi_u, double *tau_u, PJ *projRef )
{
   projUV projData;
   double phi_m, tau_m;  // midpoint
   double arcdist = 0;

   midpoint(phi_p, tau_p, phi_q, tau_q, &phi_m, &tau_m);

   projData.u = phi_m * DEG_TO_RAD;
   projData.v = tau_m * DEG_TO_RAD;

   projData = pj_fwd(projData, projRef);

   if ( isVisible(projData.u, projData.v) )
   {
       // the midpoint is visible
       // distance: midpoint->next_invisible
       arcdist = arcdistance(phi_m, tau_m, phi_q, tau_q);
       *phi_u = phi_m;
       *tau_u = tau_m;

       // use these as endpoints for next binary search
       // phi_q, tau_q are the same
       phi_p = phi_m;
       tau_p = tau_m;

   }
   else
   {
       // the midpoint is invisible (goes over the horizon)
       // distance:  last_visible->midpoint
       arcdist = arcdistance(phi_p, tau_p, phi_m, tau_m);
       *phi_u = phi_p;
       *tau_u = tau_p;

       // use these as endpoints for next binary search
       // phi_p, tau_p are the same
       phi_q = phi_m;
       tau_q = tau_m;
   }

   if ( arcdist > (1.0e-5) )
   {
        // we can search further
        cuttingpoint(phi_p, tau_p, phi_q, tau_q, phi_u, tau_u, projRef);
   }

   return arcdist;
}

// Calculate the latitude (tau_u) of the point u that lies on the greatcircle
// through p and q where the longitude of point u is given (phi_u)
static double cuttingpoint(double phi_p, double tau_p, double phi_q,
                           double tau_q, double phi_u)
{
  // pp = phi_p (longitude)
  // tp = tau_p (latitude)
  // pq = phi_q (longitude)
  // tq = tau_q (latitude)

  double pp = phi_p * DEG_TO_RAD;
  double tp = tau_p * DEG_TO_RAD;
  double pq = phi_q * DEG_TO_RAD;
  double tq = tau_q * DEG_TO_RAD;
  double pu = phi_u * DEG_TO_RAD;

  double cpp = cos(pp);
  double spp = sin(pp);
  double ctp = cos(tp);
  double stp = sin(tp);
  double cpq = cos(pq);
  double spq = sin(pq);
  double ctq = cos(tq);
  double stq = sin(tq);
  double cpu = cos(pu);
  double spu = sin(pu);

  double px = cpp * ctp;
  double py = spp * ctp;
  double pz = stp;

  double qx = cpq * ctq;
  double qy = spq * ctq;
  double qz = stq;

  // n = p x q (cross product)
  double nx =   py * qz - pz * qy;
  double ny = -(px * qz - pz * qx);
  double nz =   px * qy - py * qx;

  // calculate ||n||
  double norm_n = sqrt(nx*nx+ny*ny+nz*nz);

  // if ||n|| == 0 then p and q are opposite or identical points and
  // we can't interpolate -> just return latitude of p (tau_p)
  if (norm_n == 0)
    {
    return tau_p;
    }

  // normalize n
  nx = nx / norm_n;
  ny = ny / norm_n;
  nz = nz / norm_n;

  // calculate cos(phi_n), sin(phi_n), cos(tau_n), sin(tau_n)
  double stn = nz;
  double ctn = sqrt(1 - stn * stn); // because stn^2 + ctn^2 = 1 and ctn >= 0
  double cpn, spn;
  if (ctn == 0)
    {
    // we are at one of the poles -> chose phi_n = 0
    cpn = 1;
    spn = 0;
    }
  else
    {
    cpn = nx / ctn;
    spn = ny / ctn;
    }

  /*
   * find tau_u for the point u that has the required longitude (phi_u)
   * we transform the creatcircle through p and q to the creatcircle
   * that runs through the poles and (-90, 0) and (0, 90) (i.e. the y-z plane)
   * The transformation using rotation matrixes Ry and Rz should thus follow:
   * (nx, ny, nz) = Rz * Ry * (1, 0, 0) (this is the transformation of the
   * normals)
   * This results in
   *
   *      / cpn -spn 0 \
   * Rz = | spn  cpn 0 |
   *      \  0    0  1 /
   *
   *      / ctn 0 -stn \
   * Ry = |  0  1   0  |
   *      \ stn 0  ctn /
   *
   * and
   *
   *           / ctn*cpn -spn -stn*cpn \
   * Rz * Ry = | ctn*spn  cpn -stn*spn |
   *           \   stn     0     ctn   /
   *
   * The inverse transformation will then be
   *
   *                /  ctn*cpn  ctn*spn stn \
   * (Rz * Ry)^-1 = |   -spn      cpn    0  |
   *                \ -stn*cpn -stn*spn ctn /
   *
   * (0, cos(alpha_u), sin(alpha_u)) = (Rz * Ry)^-1 * u can be reduced to
   * tan(alpha_u) = (cpu*cpn+spu*spn)/(stn*(cpu*spn-spu*cpn)) =>
   */
  double alpha_u = atan2(cpu * cpn + spu * spn, stn * (cpu * spn - spu * cpn));
  // since y = -(cpu*spn+spu*spn)/stn and x = -cpu*spn+spu*cpn, we need to
  // compensate for the sign of -stn when we move it from the y to x in the atan
  // so if -stn < 0 then alpha_u = -alpha_u
  if (-stn < 0)
    {
    alpha_u = -alpha_u;
    }

  // calculate tau_u
  double sau = sin(alpha_u);
  double uz = sau * ctn;
  double tu = asin(uz);

  return tu * RAD_TO_DEG;
}

// Calculate N intermediate points that lie on the greatcircle through p and q
// (given in carthesian coordinates) the resulting points are stored in the
// arrays ux, uz and uz (wich need to be allocated by the caller of this
// function).
// The function returns the amount of intermediate points that were created
// (this is zero if a failure occured).
// The returned points will not include p and/or q
static int intermediatepointsxyz(double px, double py, double pz, double qx,
                                 double qy, double qz, int numPoints,
                                 double *ux, double *uy, double *uz)
{
  const double pi = 3.14159265358979;
  int i;

  // determine radius of p and q
  double pr = sqrt(px * px + py * py + pz * pz);
  double qr = sqrt(qx * qx + qy * qy + qz * qz);
  if (pr == 0 || qr == 0)
    {
    return 0;
    }
  // normalize p and q
  px /= pr;
  py /= pr;
  pz /= pr;
  qx /= qr;
  qy /= qr;
  qz /= qr;

  // n = p x q (cross product)
  double nx =   py * qz - pz * qy;
  double ny = -(px * qz - pz * qx);
  double nz =   px * qy - py * qx;

  // calculate ||n||
  double norm_n = sqrt(nx*nx+ny*ny+nz*nz);

  // if ||n|| == 0 then p and q are opposite or identical points and
  // we can't interpolate -> return 0 (no intermediate points created)
  if (norm_n == 0)
    {
    return 0;
    }

  // normalize n
  nx = nx / norm_n;
  ny = ny / norm_n;
  nz = nz / norm_n;

  //  calculate cos(phi_n), sin(phi_n), cos(tau_n), sin(tau_n)
  double stn = nz;
  double ctn = sqrt(1 - stn * stn); // because stn^2 + ctn^2 = 1 and ctn >= 0
  double cpn, spn;
  if (ctn == 0)
    {
    // we are at one of the poles -> chose phi_n = 0
    cpn = 1;
    spn = 0;
    }
  else
    {
    cpn = nx / ctn;
    spn = ny / ctn;
    }

  /*
   * find alpha_p and alpha_q
   * we transform the creatcircle through p and q to the creatcircle
   * that runs through the poles and (-90, 0) and (0, 90) (i.e. the y-z plane)
   * The transformation using rotation matrixes Ry and Rz should thus follow:
   * (nx, ny, nz) = Rz * Ry * (1, 0, 0) (this is the transformation of the
   * normals)
   * This results in
   *
   *      / cpn -spn 0 \
   * Rz = | spn  cpn 0 |
   *      \  0    0  1 /
   *
   *      / ctn 0 -stn \
   * Ry = |  0  1   0  |
   *      \ stn 0  ctn /
   *
   * and
   *
   *           / ctn*cpn -spn -stn*cpn \
   * Rz * Ry = | ctn*spn  cpn -stn*spn |
   *           \   stn     0     ctn   /
   *
   * The inverse transformation will then be
   *
   *                /  ctn*cpn  ctn*spn stn \
   * (Rz * Ry)^-1 = |   -spn      cpn    0  |
   *                \ -stn*cpn -stn*spn ctn /
   *
   * (0, cos(alpha_p), sin(alpha_p)) = (Rz * Ry)^-1 * p =>
   */
  double alpha_p = atan2(-px * stn * cpn - py * stn * spn + pz * ctn,
                         -px * spn + py * cpn);

  // do the same for q
  double alpha_q = atan2(-qx * stn * cpn - qy * stn * spn + qz * ctn,
                         -qx * spn + qy * cpn);

  // adapt alpha_q such that we take the shortest route from p to q
  // (alpha has a range of [-pi, pi])
  if (alpha_q - alpha_p < pi)
    {
    alpha_q = alpha_q + 2 * pi;
    }
  if (alpha_q - alpha_p > pi)
    {
    alpha_q = alpha_q - 2 * pi;
    }

  // now vary alpha from alpha_p to alpha_q (not including p and q)
  for (i = 0; i < numPoints; i++)
    {
    // calculate intermediate point u
    double alpha_u = alpha_p + (i + 1) * (alpha_q - alpha_p) / (numPoints + 1);

    // u = (Rz * Ry) * (0, cos(alpha_u), sin(alpha_u))
    double cau = cos(alpha_u);
    double sau = sin(alpha_u);

    // radius is linear interpolation between pr and qr
    double ur = pr + (i + 1) * (qr - pr) / (numPoints + 1);

    ux[i] = ur * (-cau * spn - sau * stn * cpn);
    uy[i] = ur * ( cau * cpn - sau * stn * spn);
    uz[i] = ur * ( sau * ctn);
    }

    return numPoints;
}

// Calculate N intermediate points that lie on the greatcircle through p and q
// (given in longitude/latitude coordinates) the resulting points are stored
// in the arrays phi_u and tau_u (wich need to be allocated by the caller of
// this function).
// The function returns the amount of intermediate points that were created
// (this is zero if a failure occured).
// The returned points will not include p and/or q
static int intermediatepoints(double phi_p, double tau_p, double phi_q,
                              double tau_q, int numPoints, double *phi_u,
                              double *tau_u)
{
  const double pi = 3.14159265358979;
  int i;

  // pp = phi_p (longitude)
  // tp = tau_p (latitude)
  // pq = phi_q (longitude)
  // tq = tau_q (latitude)

  double pp = phi_p * DEG_TO_RAD;
  double tp = tau_p * DEG_TO_RAD;
  double pq = phi_q * DEG_TO_RAD;
  double tq = tau_q * DEG_TO_RAD;

  double cpp = cos(pp);
  double spp = sin(pp);
  double ctp = cos(tp);
  double stp = sin(tp);
  double cpq = cos(pq);
  double spq = sin(pq);
  double ctq = cos(tq);
  double stq = sin(tq);

  double px = cpp * ctp;
  double py = spp * ctp;
  double pz = stp;

  double qx = cpq * ctq;
  double qy = spq * ctq;
  double qz = stq;

  // n = p x q (cross product)
  double nx =   py * qz - pz * qy;
  double ny = -(px * qz - pz * qx);
  double nz =   px * qy - py * qx;

  // calculate ||n||
  double norm_n = sqrt(nx*nx+ny*ny+nz*nz);

  // if ||n|| == 0 then p and q are opposite or identical points and
  // we can't interpolate -> return 0 (no intermediate points created)
  if (norm_n == 0)
    {
    return 0;
    }

  // normalize n
  nx = nx / norm_n;
  ny = ny / norm_n;
  nz = nz / norm_n;

  //  calculate cos(phi_n), sin(phi_n), cos(tau_n), sin(tau_n)
  double stn = nz;
  double ctn = sqrt(1 - stn * stn); // because stn^2 + ctn^2 = 1 and ctn >= 0
  double cpn, spn;
  if (ctn == 0)
    {
    // we are at one of the poles -> chose phi_n = 0
    cpn = 1;
    spn = 0;
    }
  else
    {
    cpn = nx / ctn;
    spn = ny / ctn;
    }

  /*
   * find alpha_p and alpha_q
   * we transform the creatcircle through p and q to the creatcircle
   * that runs through the poles and (-90, 0) and (0, 90) (i.e. the y-z plane)
   * The transformation using rotation matrixes Ry and Rz should thus follow:
   * (nx, ny, nz) = Rz * Ry * (1, 0, 0) (this is the transformation of the
   * normals)
   * This results in
   *
   *      / cpn -spn 0 \
   * Rz = | spn  cpn 0 |
   *      \  0    0  1 /
   *
   *      / ctn 0 -stn \
   * Ry = |  0  1   0  |
   *      \ stn 0  ctn /
   *
   * and
   *
   *           / ctn*cpn -spn -stn*cpn \
   * Rz * Ry = | ctn*spn  cpn -stn*spn |
   *           \   stn     0     ctn   /
   *
   * The inverse transformation will then be
   *
   *                /  ctn*cpn  ctn*spn stn \
   * (Rz * Ry)^-1 = |   -spn      cpn    0  |
   *                \ -stn*cpn -stn*spn ctn /
   *
   * (0, cos(alpha_p), sin(alpha_p)) = (Rz * Ry)^-1 * p =>
   */
  double alpha_p = atan2(-px * stn * cpn - py * stn * spn + pz * ctn,
                         -px * spn + py * cpn);

  // do the same for q
  double alpha_q = atan2(-qx * stn * cpn - qy * stn * spn + qz * ctn,
                         -qx * spn + qy * cpn);

  // adapt alpha_q such that we take the shortest route from p to q
  // (alpha has a range of [-pi, pi])
  if (alpha_q - alpha_p < pi)
    {
    alpha_q = alpha_q + 2 * pi;
    }
  if (alpha_q - alpha_p > pi)
    {
    alpha_q = alpha_q - 2 * pi;
    }

  // now vary alpha from alpha_p to alpha_q (not including p and q)
  for (i = 0; i < numPoints; i++)
    {
    // calculate intermediate point u
    double alpha_u = alpha_p + (i + 1) * (alpha_q - alpha_p) / (numPoints + 1);

    // u = (Rz * Ry) * (0, cos(alpha_u), sin(alpha_u))
    double cau = cos(alpha_u);
    double sau = sin(alpha_u);

    double ux = -cau * spn - sau * stn * cpn;
    double uy =  cau * cpn - sau * stn * spn;
    double uz =  sau * ctn;

    // calculate phi_u and tau_u
    double tu = asin(uz);
    // atan2 automatically 'does the right thing' ((ux,uy)=(0,0) -> pu=0)
    double pu = atan2(uy, ux);

    tau_u[i] = tu * RAD_TO_DEG;
    phi_u[i] = pu * RAD_TO_DEG;
    }

    return numPoints;
}

static void CreateInterPoints(double *p1, double *projP1, double *p2,
                              double *projP2, double *extent,
                              vtkPoints *newPoints, vtkPointData *pointData,
                              vtkPointData *newPointData, vtkIdType firstPt,
                              vtkIdList *idList, PJ *projRef,
                              double interpolationDistance,
                              int cylindricalProjection)
{
  static int depth = 0;
  double distance;

  if (interpolationDistance <= 0)
    {
    return;
    }

  if (depth > 3)
    {
    // prevent endless recursion;
    return;
    }

  distance = sqrt((projP1[0] - projP2[0]) * (projP1[0] - projP2[0]) +
                  (projP1[1] - projP2[1]) * (projP1[1] - projP2[1]));
  if (distance > interpolationDistance)
    {
    double *phi_u;
    double *tau_u;
    int numPoints;
    int result;

    numPoints = (int)(distance / interpolationDistance) + 1;

    if ( (phi_u = new double[numPoints]) == NULL)
      {
      return;
      }
    if ( (tau_u = new double[numPoints]) == NULL)
      {
      return;
      }

    result = intermediatepoints(p1[0], p1[1], p2[0], p2[1],
                                numPoints, phi_u, tau_u);
    if (result == numPoints)
      {
      double prevP[3];
      double projPrevP[3];
      int k;

      prevP[0] = p1[0];
      prevP[1] = p1[1];
      prevP[2] = p1[2];
      projPrevP[0] = projP1[0];
      projPrevP[1] = projP1[1];
      projPrevP[2] = projP1[2];

      for (k = 0; k < numPoints; k++)
        {
        double interP[3];
        projUV projData;

        interP[0] = phi_u[k];
        interP[1] = tau_u[k];
        interP[2] = projP1[2] + (k + 1) *
                    (projP2[2] - projP1[2]) / (numPoints + 1);
        projData.u = phi_u[k] * DEG_TO_RAD;
        projData.v = tau_u[k] * DEG_TO_RAD;
        projData = pj_fwd(projData, projRef);
        if (projData.u != HUGE_VAL && projData.v != HUGE_VAL)
          {
          double projInterP[3];
          vtkIdType projPt;

          projInterP[0] = (projData.u - extent[0]) / (extent[1] - extent[0]);
          projInterP[1] = (projData.v - extent[2]) / (extent[3] - extent[2]);
          projInterP[2] = interP[2];
          if (cylindricalProjection)
            {
            if ((projP1[0] < 0.5 && projP2[0] < 0.5 && projInterP[0] > 0.5) ||
                (projP1[0] > 0.5 && projP2[0] > 0.5 && projInterP[0] < 0.5))
              {
              // Map projected point to correct edge
              projInterP[0] = 1.0 - projInterP[0];
              }
            }
          // recursively call this function to check whether our interpolated
          // points are now within the required margins
          depth++;
          CreateInterPoints(prevP, projPrevP, interP, projInterP, extent, newPoints, pointData, newPointData, firstPt,
                            idList, projRef, interpolationDistance, cylindricalProjection);
          depth--;
          projPt = newPoints->InsertNextPoint(projInterP);
          newPointData->CopyData(pointData, firstPt, projPt);
          idList->InsertNextId(projPt);
          prevP[0] = interP[0];
          prevP[1] = interP[1];
          prevP[2] = interP[2];
          projPrevP[0] = projInterP[0];
          projPrevP[1] = projInterP[1];
          projPrevP[2] = projInterP[2];
          }
        }
      if (!(prevP[0] == p1[0] && prevP[1] == p1[1]))
        {
        // recursively call this function to check whether our interpolated
        // points are now within the required margins
        depth++;
        CreateInterPoints(prevP, projPrevP, p2, projP2, extent, newPoints, pointData, newPointData, firstPt,
                          idList, projRef, interpolationDistance, cylindricalProjection);
        depth--;
        }
      }

    delete [] tau_u;
    delete [] phi_u;
  }
}

//
// The Projections currently only support Verts, Lines, and Polys.
// Strips are thus currently not supported.
//

vtkProj2DFilter::vtkProj2DFilter()
{
    this->CenterLatitude = 0.0;
    this->CenterLongitude = 0.0;
    this->Eps = 0.00001;
    this->InterpolationDistance = 0.005;
    this->AzimuthalIgnorePolyDistance = 7;
    this->Projection = VTK_PROJ2D_PLATE_CAREE;
}

double vtkProj2DFilter::GetXYRatio()
{
  double extent[6];

  GetExtent(this->Projection, extent);

  //return (extent[3] - extent[2]) / (extent[1] - extent[0]);
  return (extent[1] - extent[0]) / (extent[3] - extent[2]);
}

void vtkProj2DFilter::Execute()
{
  vtkPolyData *input;

  vtkDebugMacro(<<"Performing projection on polygonal data")

  // attributes Input, Output, and Points can be NULL
  // attributes Verts, Lines, Polys, Strips, PointData, and CellData are
  // never NULL (but can be empty)
  input = this->GetInput();

  if (input == NULL || input->GetPoints() == NULL)
    {
    return;
    }

/*
  printf("vtkProj2DFilter::Execute   ");

  if ( input->GetPoints())
    printf("Points[%d] ", input->GetPoints()->GetNumberOfPoints());

  if ( input->GetVerts()->GetNumberOfCells() > 0)
    printf("Verts[%d] ", input->GetVerts()->GetNumberOfCells());

  if ( input->GetLines()->GetNumberOfCells() > 0)
    printf("Lines[%d] ", input->GetLines()->GetNumberOfCells());

  printf("\n");

  fflush(NULL);
*/

  // Because vtkPolyData can't handle cellData properly when using a
  // combination of Verts, Lines, Polys, and/or Strips we restrict this
  // filter to vtkPolyData with only one type of cells
  if ((input->GetVerts()->GetNumberOfCells() > 0) +
      (input->GetLines()->GetNumberOfCells() > 0) +
      (input->GetPolys()->GetNumberOfCells() > 0) +
      (input->GetStrips()->GetNumberOfCells() > 0) > 1)
    {
    vtkErrorMacro(<<"this filter does not work on polydata with different types of cells");
    return;
    }

  switch (this->Projection)
    {
    case VTK_PROJ2D_LAMBERT_CYLINDRICAL:
    case VTK_PROJ2D_PLATE_CAREE:
    case VTK_PROJ2D_MOLLWEIDE:
    case VTK_PROJ2D_ROBINSON:
    case VTK_PROJ2D_MERCATOR:
      this->PerformCylindricalProjection();
      break;
    case VTK_PROJ2D_LAMBERT_AZIMUTHAL:
    case VTK_PROJ2D_AZIMUTHAL_EQUIDISTANT:
    case VTK_PROJ2D_STEREOGRAPHIC:
    case VTK_PROJ2D_TMERCATOR:
      this->PerformAzimuthalProjection();
      break;
    case VTK_PROJ2D_ORTHO:
    case VTK_PROJ2D_NEAR_SIGHTED:
      this->PerformSemiProjection();
      break;
    case VTK_PROJ2D_3D:
      this->Perform3DProjection();
      break;
    default:
      vtkErrorMacro(<<"unknown projection");
      return;
    }
}

void vtkProj2DFilter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Projection: ";
  switch (this->Projection)
    {
    case VTK_PROJ2D_LAMBERT_CYLINDRICAL:
      os << "Lambert Cylindrical Equal Area" << std::endl;
      break;
    case VTK_PROJ2D_PLATE_CAREE:
      os << "Plate Caree" << std::endl;
      break;
    case VTK_PROJ2D_MOLLWEIDE:
      os << "Mollweide" << std::endl;
      break;
    case VTK_PROJ2D_ROBINSON:
      os << "Robinson" << std::endl;
      break;
    case VTK_PROJ2D_LAMBERT_AZIMUTHAL:
      os << "Lambert Azimuthal Equal Area" << std::endl;
      break;
    case VTK_PROJ2D_AZIMUTHAL_EQUIDISTANT:
      os << "Azimuthal Equidistant" << std::endl;
      break;
    case VTK_PROJ2D_3D:
      os << "3D" << std::endl;
      break;
    case VTK_PROJ2D_MERCATOR:
      os << "Mercator" << std::endl;
      break;
    case VTK_PROJ2D_TMERCATOR:
      os << "Transverse Mercator" << std::endl;
      break;
    case VTK_PROJ2D_ORTHO:
      os << "Orthographic" << std::endl;
      break;
    case VTK_PROJ2D_NEAR_SIGHTED:
      os << "Near-Sighted Perspective" << std::endl;
      break;
     case VTK_PROJ2D_STEREOGRAPHIC:
      os << "Stereographic" << std::endl;
      break;
    default:
      os << "unknown (" << this->Projection << ")" << std::endl;
      break;
    }
  os << indent << "Center Latitude: " << this->CenterLatitude << std::endl;
  os << indent << "Center Longitude: " << this->CenterLongitude << std::endl;
}

// extent = (min_x, max_x, min_y, max_y, min_z, max_z)
void vtkProj2DFilter::GetExtent(int projection,  double extent[6])
{
  switch (projection)
    {
    case VTK_PROJ2D_LAMBERT_CYLINDRICAL:
      extent[0] = -3.1416;
      extent[1] =  3.1416;
      extent[2] = -1.0;
      extent[3] =  1.0;
      extent[4] =  0.0;
      extent[5] =  0.0;
      break;
    case VTK_PROJ2D_PLATE_CAREE:
      extent[0] = -3.1416;
      extent[1] =  3.1416;
      extent[2] = -1.5710;
      extent[3] =  1.5710;
      extent[4] =  0.0;
      extent[5] =  0.0;
      break;
    case VTK_PROJ2D_MOLLWEIDE:
      extent[0] = -2.83;
      extent[1] =  2.83;
      extent[2] = -1.415;
      extent[3] =  1.415;
      extent[4] =  0.0;
      extent[5] =  0.0;
      break;
    case VTK_PROJ2D_ROBINSON:
      extent[0] = -2.6667;
      extent[1] =  2.6667;
      extent[2] = -1.3525;
      extent[3] =  1.3525;
      extent[4] =  0.0;
      extent[5] =  0.0;
      break;
    case VTK_PROJ2D_LAMBERT_AZIMUTHAL:
      extent[0] = -2.0;
      extent[1] =  2.0;
      extent[2] = -2.0;
      extent[3] =  2.0;
      extent[4] =  0.0;
      extent[5] =  0.0;
      break;
    case VTK_PROJ2D_AZIMUTHAL_EQUIDISTANT:
      extent[0] = -3.1416;
      extent[1] =  3.1416;
      extent[2] = -3.1416;
      extent[3] =  3.1416;
      extent[4] =  0.0;
      extent[5] =  0.0;
      break;
    case VTK_PROJ2D_3D:
      extent[0] = -1.0;
      extent[1] =  1.0;
      extent[2] = -1.0;
      extent[3] =  1.0;
      extent[4] = -1.0;
      extent[5] =  1.0;
      break;
    case VTK_PROJ2D_MERCATOR:
    case VTK_PROJ2D_TMERCATOR:
      extent[0] = -3.1416;
      extent[1] =  3.1416;
      extent[2] = -3.1416;
      extent[3] =  3.1416;
      extent[4] =  0.0;
      extent[5] =  0.0;
      break;
    case VTK_PROJ2D_ORTHO:
      extent[0] = -1.0472;  // pi / 3.0;
      extent[1] =  1.0472;
      extent[2] = -1.0472;
      extent[3] =  1.0472;
      extent[4] =  0.0;
      extent[5] =  0.0;
      break;
    case VTK_PROJ2D_NEAR_SIGHTED:
      extent[0] = -1.0472;  // pi / 3.0;
      extent[1] =  1.0472;
      extent[2] = -1.0472;
      extent[3] =  1.0472;
      extent[4] =  0.0;
      extent[5] =  0.0;
      break;
     case VTK_PROJ2D_STEREOGRAPHIC:
      extent[0] = -9.4248;  //  pi x 3.0
      extent[1] =  9.4248;
      extent[2] = -9.4248;
      extent[3] =  9.4248;
      extent[4] =  0.0;
      extent[5] =  0.0;
      break;
    default:
      //vtkErrorMacro("Unknown cylindrical projection");
      return;
    }
}

void vtkProj2DFilter::Perform3DProjection()
{
  vtkPolyData *input;
  vtkPolyData *output;
  vtkPoints *points;
  vtkPoints *newPoints;
  vtkCellArray *verts;
  vtkCellArray *lines;
  vtkCellArray *polys;
  vtkPointData *pointData;
  vtkPointData *newPointData;
  //vtkDataArray *newVectors;
  vtkCellData *cellData;
  vtkCellData *newCellData;
  vtkIdType id;
  vtkIdType numPoints;
  vtkDataArray *vectors = NULL;

  vtkDebugMacro(<<"Performing 3D projection on polygonal data")

  input = this->GetInput();
  points = input->GetPoints();
  verts = input->GetVerts();
  lines = input->GetLines();
  polys = input->GetPolys();
  pointData = input->GetPointData();
  vectors = pointData->GetVectors();
  cellData = input->GetCellData();

  if ( !vectors || !(vectors->GetNumberOfTuples() > 0) ) {
      vectors = NULL;
  }


  output = this->GetOutput();

  // create the vtkCellArray in output
  output->Allocate(input);
  newPointData = output->GetPointData();
  //newVectors = vtkDoubleArray::New();
  //newVectors->SetNumberOfComponents(3);
  newCellData = output->GetCellData();
  //newVectors->Delete();

  // points need to be transfered and projected
  numPoints = points->GetNumberOfPoints();
  newPoints = vtkPoints::New();
  newPoints->SetDataTypeToFloat();
  newPoints->SetNumberOfPoints(numPoints);

  for (id = 0; id < numPoints; ++id)
    {
        double sinLatitude, cosLatitude, sinLongitude, cosLongitude;
        double pt[3];
        double npt[3];
        double R;

        points->GetPoint(id, pt);
        R = 1;
        if (pt[2] > 0)
         {
         // use z value as radius
         R = pt[2];
         }
        sinLongitude = sin(pt[0] * DEG_TO_RAD);
        cosLongitude = cos(pt[0] * DEG_TO_RAD);
        sinLatitude = sin(pt[1] * DEG_TO_RAD);
        cosLatitude = cos(pt[1] * DEG_TO_RAD);
        npt[0] = R * cosLongitude * cosLatitude;
        npt[1] = R * sinLongitude * cosLatitude;
        npt[2] = R * sinLatitude;
        newPoints->SetPoint(id, npt);


        //recalculate std::vector data

        if ( vectors )
        {
            double magnitude =1;
            double magnitudeTmp =1;
            double t =0;
            double nPoint[3];
            double pointOnPlane[3];
            double newVector[3];

            double *tuple =  vectors->GetTuple3(id);
            if ( tuple )
            {
                // only 2 dimensions for magnitude
                magnitude = sqrt(tuple[0]*tuple[0] + tuple[1]*tuple[1]);

                // this is where the original std::vector points in 2D untransformed space
                nPoint[0] = pt[0] + tuple[0];
                nPoint[1] = pt[1] + tuple[1];
                nPoint[2] = pt[2];

                //where is the endpoint now in the sphere?
                sinLongitude = sin(nPoint[0] * DEG_TO_RAD);
                cosLongitude = cos(nPoint[0] * DEG_TO_RAD);
                sinLatitude = sin(nPoint[1] * DEG_TO_RAD);
                cosLatitude = cos(nPoint[1] * DEG_TO_RAD);
                nPoint[0] = R * cosLongitude * cosLatitude;
                nPoint[1] = R * sinLongitude * cosLatitude;
                nPoint[2] = R * sinLatitude;

               // calculate the plane tangent to the sphere at this point
               // since the sphere is centered at the origin...the
               // perpendicular plane at the point is
               // pt[0] X + pt[1] Y + pt[2] Z = R^2


               // since our basis for determining the angle is the endpoint (nPoint)
               // we find a perpendicular from the nPoint that intersects
               // the plane.  And we want the point of intersection.
               // L = P + tv   -> v = std::vector normal to plane,
               //              -> P is point in the line (nPoint)
               // and we want to find at which t we intersect the plane:

               t = (R*R - npt[0]*nPoint[0] - npt[1]*nPoint[1] - npt[2]*nPoint[2])
                        /(npt[0]*npt[0] + npt[1]*npt[1] + npt[2]*npt[2]);

               // point on the plane is
               pointOnPlane[0] = nPoint[0] + npt[0]*t;
               pointOnPlane[1] = nPoint[1] + npt[1]*t;
               pointOnPlane[2] = nPoint[2] + npt[2]*t;

               //calculate new std::vector
               newVector[0] = pointOnPlane[0] - npt[0];
               newVector[1] = pointOnPlane[1] - npt[1];
               newVector[2] = pointOnPlane[2] - npt[2];

               // normalize
               magnitudeTmp = sqrt(newVector[0]*newVector[0] + newVector[1]*newVector[1] + newVector[2]*newVector[2]);
               newVector[0] /= magnitudeTmp;
               newVector[1] /= magnitudeTmp;
               newVector[2] /= magnitudeTmp;

               // restore to original magnitude
               newVector[0] *= magnitude;
               newVector[1] *= magnitude;
               newVector[2] *= magnitude;

               //save the std::vector data
               vectors->SetTuple3(id, newVector[0], newVector[1], newVector[2]);
               //printf("F: %g, %g, %g -> %g, %g, %g\n", nPoint[0], nPoint[1], nPoint[2], newVector[0], newVector[1], newVector[2]);
            }
        }

    }

  output->SetPoints(newPoints);
  newPoints->Delete();

  // pointData is first copied and later extended as needed
  newPointData->DeepCopy(pointData);


  // Vertices are copied
  if (verts->GetNumberOfCells() > 0)
    {
    vtkCellArray *newVerts = vtkCellArray::New();
    output->SetVerts(newVerts);
    newVerts->Delete();
    newVerts->DeepCopy(verts);
    newCellData->DeepCopy(cellData);
    }


  // Lines are interpolated if necessary
  if (lines->GetNumberOfCells() > 0)
    {
    vtkCellArray *newLines = vtkCellArray::New();
    output->SetLines(newLines);
    newLines->Delete();
    if (this->InterpolationDistance <= 0)
      {
      newLines->DeepCopy(lines);
      newCellData->DeepCopy(cellData);
      }
    else
      {
      vtkIdList *idList;
      vtkIdType numLines;
      vtkIdType cellId;

      newCellData->CopyAllocate(cellData);

      numLines = lines->GetNumberOfCells();

      idList = vtkIdList::New();
      idList->Allocate(1000);

      lines->InitTraversal();
      for (cellId = 0; cellId < numLines; cellId++)
        {
        vtkIdType npts = 0;
        vtkIdType *pts;

        lines->GetNextCell(npts, pts);

        if (npts > 0)
          {
          double p1[3];
          int j;

          newPoints->GetPoint(pts[0], p1);
          idList->Reset();
          idList->InsertNextId(pts[0]);

          for (j = 1; j < npts; j++)
            {
            double p2[3];
            double distance;

            newPoints->GetPoint(pts[j], p2);
            distance = arcdistancexyz(p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);
            if (distance > 360 * this->InterpolationDistance)
              {
              int numIntermediatePoints;
              int result;
              double *ux;
              double *uy;
              double *uz;

              numIntermediatePoints = (int)(distance / (360 * this->InterpolationDistance)) ;
              if ( (ux = new double[numIntermediatePoints]) == NULL)
                {
                idList->Delete();
                return;
                }
              if ( (uy = new double[numIntermediatePoints]) == NULL)
                {
                idList->Delete();
                return;
                }
              if ( (uz = new double[numIntermediatePoints]) == NULL)
                {
                idList->Delete();
                return;
                }
              result = intermediatepointsxyz(p1[0], p1[1], p1[2], p2[0], p2[1],
                                             p2[2], numIntermediatePoints, ux,
                                             uy, uz);
              if (result == numIntermediatePoints)
                {
                int k;

                for (k = 0; k < numIntermediatePoints; k++)
                  {
                  vtkIdType newPt;

                  newPt = newPoints->InsertNextPoint(ux[k], uy[k], uz[k]);
                  newPointData->CopyData(pointData, pts[j - 1], newPt);
                  idList->InsertNextId(newPt);
                  }
                }

              delete [] ux;
              delete [] uy;
              delete [] uz;
              }
            idList->InsertNextId(pts[j]);
            p1[0] = p2[0];
            p1[1] = p2[1];
            p1[2] = p2[2];
            }

          if (idList->GetNumberOfIds() > 1)
            {
            vtkIdType newCellId;

            newCellId = output->InsertNextCell(VTK_LINE, idList);
            newCellData->CopyData(cellData, cellId, newCellId);
            }
          }
        }
      idList->Delete();
      }
    }

  // Polys are interpolated if necessary
  if (polys->GetNumberOfCells() > 0)
    {
    vtkCellArray *newPolys = vtkCellArray::New();
    output->SetPolys(newPolys);
    newPolys->Delete();
    if (this->InterpolationDistance <= 0)
      {
      newPolys->DeepCopy(polys);
      newCellData->DeepCopy(cellData);
      }
    else
      {
      vtkIdList *idList;
      vtkIdType numPolys;
      vtkIdType cellId;

      newCellData->CopyAllocate(cellData);

      numPolys = polys->GetNumberOfCells();

      idList = vtkIdList::New();
      idList->Allocate(1000);

      polys->InitTraversal();
      for (cellId = 0; cellId < numPolys; cellId++)
        {
        vtkIdType npts = 0;
        vtkIdType *pts = NULL;

        polys->GetNextCell(npts, pts);

        if (npts > 0)
          {
          double p1[3];
          int j;

          newPoints->GetPoint(pts[0], p1);
          idList->Reset();
          idList->InsertNextId(pts[0]);

          for (j = 1; j < npts; j++)
            {
            double p2[3];
            double distance;

            newPoints->GetPoint(pts[j], p2);
            distance = arcdistancexyz(p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);
            if (distance > 360 * this->InterpolationDistance)
              {
              int numIntermediatePoints;
              int result;
              double *ux;
              double *uy;
              double *uz;

              numIntermediatePoints = (int)(distance / (360 * this->InterpolationDistance)) ;
              if ( (ux = new double[numIntermediatePoints]) == NULL)
                {
                idList->Delete();
                return;
                }
              if ( (uy = new double[numIntermediatePoints]) == NULL)
                {
                idList->Delete();
                return;
                }
              if ( (uz = new double[numIntermediatePoints]) == NULL)
                {
                idList->Delete();
                return;
                }
              result = intermediatepointsxyz(p1[0], p1[1], p1[2], p2[0], p2[1],
                                             p2[2], numIntermediatePoints, ux,
                                             uy, uz);
              if (result == numIntermediatePoints)
                {
                int k;

                for (k = 0; k < numIntermediatePoints; k++)
                  {
                  vtkIdType newPt;

                  newPt = newPoints->InsertNextPoint(ux[k], uy[k], uz[k]);
                  newPointData->CopyData(pointData, pts[j - 1], newPt);
                  idList->InsertNextId(newPt);
                  }
                }

              delete [] ux;
              delete [] uy;
              delete [] uz;
              }
            idList->InsertNextId(pts[j]);
            p1[0] = p2[0];
            p1[1] = p2[1];
            p1[2] = p2[2];
            }

          if (idList->GetNumberOfIds() > 1)
            {
            vtkIdType newCellId;

            newCellId = output->InsertNextCell(VTK_POLYGON, idList);
            newCellData->CopyData(cellData, cellId, newCellId);
            }
          }
        }
      idList->Delete();
      }
    }
}

void vtkProj2DFilter::PerformAzimuthalProjection()
{
  vtkPolyData *input;
  vtkPolyData *output;
  vtkPoints *points;
  vtkPoints *newPoints;
  vtkCellArray *verts;
  vtkCellArray *lines;
  vtkCellArray *polys;
  vtkPointData *pointData;
  vtkPointData *newPointData;
  vtkCellData *cellData;
  vtkCellData *newCellData;
  vtkIdType id;
  vtkIdType numPoints;
   vtkDataArray *vectors = NULL;
  PJ *projRef;
  projUV projData;
  char centerLatitudeParam[100];
  char centerLongitudeParam[100];
  double extent[6];
  double cuttingLatitude;
  double cuttingLongitude;

  vtkDebugMacro(<<"Performing Azimuthal projection on polygonal data")

  input = this->GetInput();
  points = input->GetPoints();
  verts = input->GetVerts();
  lines = input->GetLines();
  polys = input->GetPolys();
  pointData = input->GetPointData();
  cellData = input->GetCellData();

  vectors = pointData->GetVectors();
  if ( !vectors || !(vectors->GetNumberOfTuples() > 0) )
    vectors = NULL;

  output = this->GetOutput();

  // create the vtkCellArray in output
  output->Allocate(input);
  newPointData = output->GetPointData();
  newCellData = output->GetCellData();

  // points need to be transfered and projected
  numPoints = points->GetNumberOfPoints();
  newPoints = vtkPoints::New();
  newPoints->SetDataTypeToFloat();
  newPoints->SetNumberOfPoints(numPoints);

  if (this->CenterLatitude == 0.0)
    {
    cuttingLatitude = this->CenterLatitude;
    }
  else
    {
    cuttingLatitude = -this->CenterLatitude;
    }
  cuttingLongitude = this->CenterLongitude + 180.0;
  if (cuttingLongitude >= 180.0)
    {
    cuttingLongitude -= 360.0;
    }

  static const char *parameters[] = {
    "",
    "",
    "",
    "R=1.0",
    "ellps=WGS84",
    "no_defs",
    ""
  };

  switch (this->Projection)
    {
    case VTK_PROJ2D_LAMBERT_AZIMUTHAL:
      parameters[0] = "proj=laea";
      break;
    case VTK_PROJ2D_AZIMUTHAL_EQUIDISTANT:
      parameters[0] = "proj=aeqd";
      break;
    case VTK_PROJ2D_TMERCATOR:
      parameters[0] = "proj=tmerc";
      break;
    case VTK_PROJ2D_STEREOGRAPHIC:
      parameters[0] = "proj=stere";
      parameters[6] = "k=2";
      break;
    default:
      vtkErrorMacro("Unknown azimuthal projection");
      return;
    }
  sprintf(centerLatitudeParam, "lat_0=%7.3f", this->CenterLatitude);
  sprintf(centerLongitudeParam, "lon_0=%7.3f", this->CenterLongitude);
  parameters[1] = centerLatitudeParam;
  parameters[2] = centerLongitudeParam;

  projRef = pj_init(sizeof(parameters)/sizeof(char *), const_cast<char**>( parameters) );
  if (projRef == 0)
    {
    vtkErrorMacro(<<"Could not initialize PROJ library ("
                  << pj_strerrno(pj_errno) << ")");
    return;
    }

  GetExtent(this->Projection, extent);

  for (id = 0; id < numPoints; ++id)
    {
    double pt[3];
    double npt[3];

    points->GetPoint(id, pt);
    while (pt[0] >= 180)
      {
      pt[0] -= 360;
      }
    while (pt[0] < -180)
      {
      pt[0] += 360;
      }
    projData.u = pt[0] * DEG_TO_RAD;
    projData.v = pt[1] * DEG_TO_RAD;
    projData = pj_fwd(projData, projRef);
    npt[0] = (projData.u - extent[0]) / (extent[1] - extent[0]);
    npt[1] = (projData.v - extent[2]) / (extent[3] - extent[2]);
    newPoints->SetPoint(id, npt);


     // transform the std::vector data
     if ( vectors )
     {
         double magnitude =1;
         //double magnitudeTmp =1;
         double nPoint[3];
         int nleftSide =0, leftSide =0;
         double *tuple =  vectors->GetTuple3(id);
         magnitude = sqrt(tuple[0]*tuple[0] + tuple[1]*tuple[1]);

         // this is where the original std::vector points in 2D untransformed space
         nPoint[0] = pt[0] + tuple[0];
         nPoint[1] = pt[1] + tuple[1];

         //now transform this point
         while (nPoint[0] >= 180)
          {
          nPoint[0] -= 360;
          }
        while (nPoint[0] < -180)
          {
          nPoint[0] += 360;
          }


        if (this->CenterLongitude > cuttingLongitude)
        {
            nleftSide = (nPoint[0] >= cuttingLongitude && nPoint[0] < this->CenterLongitude);
            leftSide = (pt[0] >= cuttingLongitude && pt[0] < this->CenterLongitude);
        }
        else
        {
            nleftSide = (nPoint[0] >= cuttingLongitude || nPoint[0] < this->CenterLongitude);
            leftSide = (pt[0] >= cuttingLongitude || pt[0] < this->CenterLongitude);
        }

        if ( nleftSide != leftSide )
        {
            vectors->SetTuple3(id, 0, 0, 0);
            continue;
        }



        projData.u = nPoint[0] * DEG_TO_RAD;
        projData.v = nPoint[1] * DEG_TO_RAD;
        projData = pj_fwd(projData, projRef);
        nPoint[0] = (projData.u - extent[0]) / (extent[1] - extent[0]);
        nPoint[1] = (projData.v - extent[2]) / (extent[3] - extent[2]);

        if ( !isVisible(nPoint[0], nPoint[1]) )
        {
            vectors->SetTuple3(id, 0, 0, 0);
            continue;
        }

        // calculate the new std::vector
        nPoint[0] -= npt[0];
        nPoint[1] -= npt[1];

        // The following code would re-scale the std::vector, but
        // we don't want that right now

        //magnitudeTmp = sqrt(nPoint[0]*nPoint[0] + nPoint[1]*nPoint[1]);
        //nPoint[0] /= magnitudeTmp;
        //nPoint[1] /= magnitudeTmp;

        //restore to original magnitude
        //nPoint[0] *= magnitude;
        //nPoint[1] *= magnitude;


        //save the std::vector data
        vectors->SetTuple3(id, nPoint[0], nPoint[1], 0);

     }
    }

  output->SetPoints(newPoints);
  newPoints->Delete();

  // pointData is first copied and later extended as needed
  newPointData->DeepCopy(pointData);

  // Vertices are filtered for Verts that correspond with the cutting point
  if (verts->GetNumberOfCells() > 0)
    {
    vtkCellArray *newVerts = vtkCellArray::New();
    vtkIdType numVerts;
    vtkIdType cellId;

    // initialize newCellData for CopyData invocations
    newCellData->CopyAllocate(cellData);

    output->SetVerts(newVerts);
    newVerts->Delete();
    numVerts = verts->GetNumberOfCells();

    verts->InitTraversal();
    for (cellId = 0; cellId < numVerts; cellId++)
      {
      vtkIdType npts = 0;
      vtkIdType *pts = NULL;

      verts->GetNextCell(npts, pts);

      // we ignore cells that do not contain vertices (i.e. only one point)
      if (npts == 1)
        {
        double pt[3];

        newPoints->GetPoint(pts[0], pt);
        if (pt[0] != HUGE_VAL && pt[1] != HUGE_VAL)
          {
          vtkIdType newCellId;
          newCellId = output->InsertNextCell(VTK_VERTEX, 1, pts);
          newCellData->CopyData(cellData, cellId, newCellId);
          }
        }
      }
    }


  // Lines need to be filtered for points that correspond with the cutting point
  if (lines->GetNumberOfCells() > 0)
    {
    vtkCellArray *newLines = vtkCellArray::New();
    vtkIdList *idList;
    vtkIdType numLines;
    vtkIdType cellId;

    // initialize newCellData for CopyData invocations
    newCellData->CopyAllocate(cellData);

    output->SetLines(newLines);
    newLines->Delete();
    numLines = lines->GetNumberOfCells();

    idList = vtkIdList::New();
    idList->Allocate(1000);

    lines->InitTraversal();
    for(cellId = 0; cellId < numLines; cellId++)
      {
      vtkIdType npts = 0;
      vtkIdType *pts = NULL;

      lines->GetNextCell(npts, pts);

      if (npts > 0)
        {
        double p1[3];
        double projP1[3];
        vtkIdType newCellId;
        int j;

        points->GetPoint(pts[0], p1);
        newPoints->GetPoint(pts[0], projP1);
        while (p1[0] >= 180)
          {
          p1[0] -= 360;
          }
        while (p1[0] < -180)
          {
          p1[0] += 360;
          }
        idList->Reset();
        if (projP1[0] != HUGE_VAL && projP1[1] != HUGE_VAL)
          {
          idList->InsertNextId(pts[0]);
          }

        for (j = 1; j < npts; j++)
          {
          double p2[3];
          double projP2[3];

          points->GetPoint(pts[j], p2);
          newPoints->GetPoint(pts[j], projP2);
          while (p2[0] >= 180)
            {
            p2[0] -= 360;
            }
          while (p2[0] < -180)
            {
            p2[0] += 360;
            }
          if (projP2[0] == HUGE_VAL || projP2[1] == HUGE_VAL)
            {
            // Skip this point and start a new line
            if (idList->GetNumberOfIds() > 1)
              {
              newCellId = output->InsertNextCell(VTK_LINE, idList);
              newCellData->CopyData(cellData, cellId, newCellId);
              }
            idList->Reset();
            }
          else
            {
            // check for interpolation
            if (projP1[0] != HUGE_VAL && projP1[1] != HUGE_VAL)
              {
              CreateInterPoints(p1, projP1, p2, projP2, extent, newPoints,
                                pointData, newPointData, pts[j - 1], idList,
                                projRef, this->InterpolationDistance, 0);
              }
            idList->InsertNextId(pts[j]);
            }

          p1[0] = p2[0];
          p1[1] = p2[1];
          p1[2] = p2[2];
          projP1[0] = projP2[0];
          projP1[1] = projP2[1];
          projP1[2] = projP2[2];
          }

        if (idList->GetNumberOfIds() > 1)
          {
          newCellId = output->InsertNextCell(VTK_LINE, idList);
          newCellData->CopyData(cellData, cellId, newCellId);
          }
        }
      }
    idList->Delete();
    }


  // Polys need to be filtered for points that correspond with the cutting point
  // and polys that lie within the AzimuthalIgnorePolyDistance distance of the
  // cutting point will be filtered out
  if (polys->GetNumberOfCells() > 0)
    {
    vtkCellArray *newPolys = vtkCellArray::New();
    vtkIdList *idList;
    vtkIdType numPolys;
    vtkIdType cellId;

    // initialize newCellData for CopyData invocations
    newCellData->CopyAllocate(cellData);

    output->SetPolys(newPolys);
    newPolys->Delete();
    numPolys = polys->GetNumberOfCells();

    idList = vtkIdList::New();
    idList->Allocate(1000);

    polys->InitTraversal();
    for (cellId = 0; cellId < numPolys; cellId++)
      {
      double mindistance = this->AzimuthalIgnorePolyDistance;
      vtkIdType npts = 0;
      vtkIdType *pts = NULL;

      polys->GetNextCell(npts, pts);

      if (npts > 0)
        {
        vtkIdType newCellId;
        double p1[3];
        double projP1[3];
        double distance;
        int lastId = -1;
        int j;

        points->GetPoint(pts[0], p1);
        distance = arcdistance(p1[0], p1[1], cuttingLongitude, cuttingLatitude);
        if (distance < mindistance)
          {
          mindistance = distance;
          }
        newPoints->GetPoint(pts[0], projP1);
        idList->Reset();
        if (projP1[0] != HUGE_VAL && projP1[1] != HUGE_VAL)
          {
          idList->InsertNextId(pts[0]);
          lastId = 0;
          }

        for (j = 1; j < npts; j++)
          {
          double projP2[3];

          newPoints->GetPoint(pts[j], projP2);
          if (projP2[0] != HUGE_VAL && projP2[1] != HUGE_VAL)
            {
            double p2[3];

            points->GetPoint(pts[j], p2);
            distance = arcdistance(p2[0], p2[1], cuttingLongitude, cuttingLatitude);
            if (distance < mindistance)
              {
              mindistance = distance;
              }
            if (lastId >= 0)
              {
              CreateInterPoints(p1, projP1, p2, projP2, extent, newPoints,
                                pointData, newPointData, pts[lastId], idList,
                                projRef, this->InterpolationDistance, 0);
              }
            idList->InsertNextId(pts[j]);

            p1[0] = p2[0];
            p1[1] = p2[1];
            p1[2] = p2[2];
            projP1[0] = projP2[0];
            projP1[1] = projP2[1];
            projP1[2] = projP2[2];
            lastId = j;
            }
          }

        if (idList->GetNumberOfIds() > 1 &&
            mindistance >= this->AzimuthalIgnorePolyDistance)
          {
          vtkIdType id1, id2;
          id1 = idList->GetId(0);
          id2 = idList->GetId(idList->GetNumberOfIds() - 1);
          // if the first and last id are not the same interpolate between
          // these points
          if (id1 != id2)
            {
            double firstP[3];
            double lastP[3];
            double projFirstP[3];
            double projLastP[3];
            points->GetPoint(id1, firstP);
            points->GetPoint(id2, lastP);
            newPoints->GetPoint(id1, projFirstP);
            newPoints->GetPoint(id2, projLastP);
            if (firstP[0] != lastP[0] || firstP[1] != lastP[1])
              {
              CreateInterPoints(lastP, projLastP, firstP, projFirstP, extent,
                                newPoints, pointData, newPointData, id2,
                                idList, projRef, this->InterpolationDistance, 0);
              }
            }
          newCellId = output->InsertNextCell(VTK_POLYGON, idList);
          newCellData->CopyData(cellData, cellId, newCellId);
          }
        }
      }
    idList->Delete();
    }

  pj_free(projRef);
}

void vtkProj2DFilter::PerformCylindricalProjection()
{
  vtkPolyData *input;
  vtkPolyData *output;
  vtkPoints *points;
  vtkPoints *newPoints;
  vtkCellArray *verts;
  vtkCellArray *lines;
  vtkCellArray *polys;
  vtkPointData *pointData;
  vtkPointData *newPointData;
  vtkCellData *cellData;
  vtkCellData *newCellData;
  vtkIdType id;
  vtkIdType numPoints;
  PJ *projRef;
  projUV projData;
  char centerLongitudeParam[100];
  double extent[6];
  double cuttingLongitude;

  vtkDataArray *vectors = NULL;

  vtkDebugMacro(<<"Performing Cylindrical projection on polygonal data")

  input = this->GetInput();
  points = input->GetPoints();
  verts = input->GetVerts();
  lines = input->GetLines();
  polys = input->GetPolys();
  pointData = input->GetPointData();
  cellData = input->GetCellData();

  vectors = pointData->GetVectors();
  if ( !vectors || !(vectors->GetNumberOfTuples() > 0) )
    vectors = NULL;


  output = this->GetOutput();

  // create the vtkCellArray in output
  output->Allocate(input);
  newPointData = output->GetPointData();
  newCellData = output->GetCellData();

  // points need to be transfered and projected
  numPoints = points->GetNumberOfPoints();
  newPoints = vtkPoints::New();
  newPoints->SetDataTypeToFloat();
  newPoints->SetNumberOfPoints(numPoints);

  cuttingLongitude = this->CenterLongitude + 180.0;
  if (cuttingLongitude >= 180.0)
    {
    cuttingLongitude -= 360.0;
    }

  static const char *parameters[] = {
    "",
    "",
    "",
    "R=1.0",
    "ellps=WGS84",
    "no_defs",
    ""
  };

  switch (this->Projection)
    {
    case VTK_PROJ2D_LAMBERT_CYLINDRICAL:
      parameters[0] = "proj=cea";
      break;
    case VTK_PROJ2D_PLATE_CAREE:
      parameters[0] = "proj=eqc";
      break;
    case VTK_PROJ2D_MOLLWEIDE:
      parameters[0] = "proj=moll";
      break;
    case VTK_PROJ2D_ROBINSON:
      parameters[0] = "proj=robin";
      break;
    case VTK_PROJ2D_MERCATOR:
      parameters[0] = "proj=merc";
      break;
    case VTK_PROJ2D_STEREOGRAPHIC:
      parameters[0] = "proj=stere";
      parameters[6] = "k=2";
      break;
    default:
      vtkErrorMacro("Unknown cylindrical projection");
      return;
    }
  sprintf(centerLongitudeParam, "lon_0=%7.3f", this->CenterLongitude);
  parameters[1] = centerLongitudeParam;

  projRef = pj_init(sizeof(parameters)/sizeof(char *), const_cast<char**>( parameters) );
  if (projRef == 0)
    {
    vtkErrorMacro(<<"Could not initialize PROJ library ("
                  << pj_strerrno(pj_errno) << ")");
    return;
    }

  GetExtent(this->Projection, extent);

  for (id = 0; id < numPoints; id++)
    {
    double pt[3];
    double npt[3];
    int leftSide;

    points->GetPoint(id, pt);
    while (pt[0] >= 180)
      {
      pt[0] -= 360;
      }
    while (pt[0] < -180)
      {
      pt[0] += 360;
      }
    // if projection is mercator and latitude is +/-90,
    // then subtract 1.0e-7 from latitude value to avoid infinite number
    if (this->Projection == VTK_PROJ2D_MERCATOR)
      {
      if (areEqual(pt[1], 90.0))
        {
          pt[1] = pt[1] - CTools::m_deltaLatitudeMercator;
        }
      else if (areEqual(pt[1], -90.0))
        {
          pt[1] = pt[1] + CTools::m_deltaLatitudeMercator;
        }
      }
    if (this->CenterLongitude > cuttingLongitude)
      {
      leftSide = (pt[0] >= cuttingLongitude && pt[0] < this->CenterLongitude);
      }
    else
      {
      leftSide = (pt[0] >= cuttingLongitude || pt[0] < this->CenterLongitude);
      }
    projData.u = pt[0] * DEG_TO_RAD;
    projData.v = pt[1] * DEG_TO_RAD;
    projData = pj_fwd(projData, projRef);
    if (std::numeric_limits<double>::infinity() == projData.u)
    {
      pt[1] = pt[1] - (1.0e-7);
    }
    npt[0] = (projData.u - extent[0]) / (extent[1] - extent[0]);
    npt[1] = (projData.v - extent[2]) / (extent[3] - extent[2]);
    if ((leftSide && npt[0] > 0.5) || (!leftSide && npt[0] < 0.5))
      {
      // Fix rounding to the wrong side of the edge by PROJ
      npt[0] = 1.0 - npt[0];
      }

    npt[2] =0;
    newPoints->SetPoint(id, npt);

     // transform the std::vector data
     if ( vectors )
     {
         double magnitude =1;
         //double magnitudeTmp =1;
         double nPoint[3];
         int nleftSide = 0;
         double *tuple =  vectors->GetTuple3(id);
         magnitude = sqrt(tuple[0]*tuple[0] + tuple[1]*tuple[1]);

         // this is where the original std::vector points in 2D untransformed space
         nPoint[0] = pt[0] + tuple[0];
         nPoint[1] = pt[1] + tuple[1];
         nPoint[2] = 0;

         //now transform this point
         while (nPoint[0] >= 180)
          {
          nPoint[0] -= 360;
          }
        while (nPoint[0] < -180)
          {
          nPoint[0] += 360;
          }


        if (this->CenterLongitude > cuttingLongitude)
        {
            nleftSide = (nPoint[0] >= cuttingLongitude && nPoint[0] < this->CenterLongitude);
        }
        else
        {
            nleftSide = (nPoint[0] >= cuttingLongitude || nPoint[0] < this->CenterLongitude);
        }

        // if the are in different sides of the projection, forget it
        if ( nleftSide != leftSide ) {
            vectors->SetTuple3(id, 0, 0, 0);
            continue;
        }



        projData.u = nPoint[0] * DEG_TO_RAD;
        projData.v = nPoint[1] * DEG_TO_RAD;
        projData = pj_fwd(projData, projRef);
        nPoint[0] = (projData.u - extent[0]) / (extent[1] - extent[0]);
        nPoint[1] = (projData.v - extent[2]) / (extent[3] - extent[2]);
        nPoint[2] =0;

        // if the end point is out of the projection, we forget it
        if ( !isVisible(nPoint[0], nPoint[1]) )
        {
            vectors->SetTuple3(id, 0, 0, 0);
            continue;
        }


        // calculate the new std::vector
        nPoint[0] -= npt[0];
        nPoint[1] -= npt[1];
        nPoint[2] = 0;

        // We don't want to re-scale the std::vector now..

        // normalize this std::vector
        /*magnitudeTmp = sqrt(nPoint[0]*nPoint[0] + nPoint[1]*nPoint[1]);
        nPoint[0] /= magnitudeTmp;
        nPoint[1] /= magnitudeTmp;

        //restore to original magnitude
        nPoint[0] *= magnitude;
        nPoint[1] *= magnitude;*/


        //save the std::vector data
        vectors->SetTuple3(id, nPoint[0], nPoint[1], 0);

     }

  }
  output->SetPoints(newPoints);
  newPoints->Delete();

  // pointData is first copied and later extended as needed
  newPointData->DeepCopy(pointData);

  // Vertices are passed unmodified
  if (verts->GetNumberOfCells() > 0)
    {
    vtkCellArray *newVerts = vtkCellArray::New();
    output->SetVerts(newVerts);
    newVerts->Delete();
    newVerts->DeepCopy(verts);
    newCellData->DeepCopy(cellData);
    }

  // Lines need to be cut when passing a projection boundary
  if (lines->GetNumberOfCells() > 0)
    {
    vtkCellArray *newLines = vtkCellArray::New();
    vtkIdList *idList;
    vtkIdType numLines;
    vtkIdType cellId;

    // initialize newCellData for CopyData invocations
    newCellData->CopyAllocate(cellData);

    output->SetLines(newLines);
    newLines->Delete();
    numLines = lines->GetNumberOfCells();

    idList = vtkIdList::New();
    idList->Allocate(1000);

    lines->InitTraversal();
    for (cellId = 0; cellId < numLines; cellId++)
      {
      vtkIdType npts = 0;
      vtkIdType *pts = NULL;

      lines->GetNextCell(npts, pts);

      if (npts > 0)
        {
        vtkIdType newCellId;
        double p1[3];
        double projP1[3];
        int j;

        points->GetPoint(pts[0], p1);
        newPoints->GetPoint(pts[0], projP1);
        p1[2] =0;
        projP1[2] =0;
        while (p1[0] >= 180)
          {
          p1[0] -= 360;
          }
        while (p1[0] < -180)
          {
          p1[0] += 360;
          }
        idList->Reset();
        idList->InsertNextId(pts[0]);

        for (j = 1; j < npts; j++)
          {
          double p2[3];
          double projP2[3];

          points->GetPoint(pts[j], p2);
          newPoints->GetPoint(pts[j], projP2);
          p2[2] =0;
          projP2[2] =0;
          while (p2[0] >= 180)
            {
            p2[0] -= 360;
            }
          while (p2[0] < -180)
            {
            p2[0] += 360;
            }

          // check if both points are on the cutting edge
          if (p1[0] == cuttingLongitude && p2[0] == cuttingLongitude)
            {
            double projSecondP1[3];
            double projSecondP2[3];
            vtkIdList *idList2;
            vtkIdType secondPt;

            // the line segments coincides with the cutting meridian ->
            // keep the line segment and add a second line at the other side
            CreateInterPoints(p1, projP1, p2, projP2, extent, newPoints, pointData, newPointData, pts[j - 1], idList,
                              projRef, this->InterpolationDistance, 1);
            idList->InsertNextId(pts[j]);

            idList2 = vtkIdList::New();
            idList2->Allocate(100);

            projSecondP1[0] = 1.0 - projP1[0];
            projSecondP1[1] = projP1[1];
            projSecondP1[2] = projP1[2];
            secondPt = newPoints->InsertNextPoint(projSecondP1);
            newPointData->CopyData(pointData, pts[j - 1], secondPt);
            idList2->InsertNextId(secondPt);

            projSecondP2[0] = 1.0 - projP2[0];
            projSecondP2[1] = projP2[1];
            projSecondP2[2] = projP2[2];
            secondPt = newPoints->InsertNextPoint(projSecondP2);
            newPointData->CopyData(pointData, pts[j], secondPt);
            CreateInterPoints(p1, projSecondP1, p2, projSecondP2, extent, newPoints, pointData, newPointData,
                              pts[j - 1], idList2, projRef, this->InterpolationDistance, 1);
            idList2->InsertNextId(secondPt);

            newCellId = output->InsertNextCell(VTK_LINE, idList2);
            newCellData->CopyData(cellData, cellId, newCellId);

            idList2->Delete();
            }
          else
            {
            int leftSide[2];
            int splitLineSegment = 0;

            // check whether both points lie in the range
            // [cuttingLongitude, CenterLongitude] or
            // [CenterLongitude, cuttingLongitude]
            if (this->CenterLongitude > cuttingLongitude)
              {
              leftSide[0] = (p1[0] >= cuttingLongitude && p1[0] <= this->CenterLongitude);
              leftSide[1] = (p2[0] >= cuttingLongitude && p2[0] <= this->CenterLongitude);
              }
            else
              {
              leftSide[0] = (p1[0] >= cuttingLongitude || p1[0] <= this->CenterLongitude);
              leftSide[1] = (p2[0] >= cuttingLongitude || p2[0] <= this->CenterLongitude);
              }
            if (leftSide[0] != leftSide[1])
              {
              double d1, d2;
              // check whether the shortest path between both points crosses
              // the cuttingLongitude meridian (or the CenterLongitude meridian)
              d1 = fabs(p1[0] - cuttingLongitude);
              if (d1 >= 180)
                {
                d1 = 360 - d1;
                }
              d2 = fabs(p2[0] - cuttingLongitude);
              if (d2 >= 180)
                {
                d2 = 360 - d2;
                }
              splitLineSegment = (d1 + d2 <= 180);
              }
            if (splitLineSegment)
              {
              // points are on different sides of the cutting edge ->
              // introduce extra points at cutting edge (both sides);
              // terminate this Line and start a new one
              vtkIdType edgePt;
              double edge[3];
              double projEdgeP1[3];
              double projEdgeP2[3];

              edge[0] = cuttingLongitude;
              edge[1] = cuttingpoint(p1[0], p1[1], p2[0], p2[1], cuttingLongitude);
              edge[2] = (p1[2] + p2[2]) / 2;  // TODO: Use proper weighting
              projData.u = edge[0] * DEG_TO_RAD;
              projData.v = edge[1] * DEG_TO_RAD;
              projData = pj_fwd(projData, projRef);
              projEdgeP1[0] = (projData.u - extent[0]) / (extent[1] - extent[0]);
              projEdgeP1[1] = (projData.v - extent[2]) / (extent[3] - extent[2]);
              projEdgeP1[2] = edge[2];
              projEdgeP2[0] = projEdgeP1[0];
              projEdgeP2[1] = projEdgeP1[1];
              projEdgeP2[2] = projEdgeP1[2];
              if ((leftSide[0] && projEdgeP1[0] > 0.5) || (!leftSide[0] && projEdgeP1[0] < 0.5))
                {
                projEdgeP1[0] = 1.0 - projEdgeP1[0];
                }
              if ((leftSide[1] && projEdgeP2[0] > 0.5) || (!leftSide[1] && projEdgeP2[0] < 0.5))
                {
                projEdgeP2[0] = 1.0 - projEdgeP2[0];
                }
              edgePt = newPoints->InsertNextPoint(projEdgeP1);
              newPointData->CopyData(pointData, pts[j - 1], edgePt);
              CreateInterPoints(p1, projP1, edge, projEdgeP1, extent, newPoints, pointData, newPointData, pts[j - 1],
                                idList, projRef, this->InterpolationDistance, 1);
              idList->InsertNextId(edgePt);

              newCellId = output->InsertNextCell(VTK_LINE, idList);
              newCellData->CopyData(cellData, cellId, newCellId);

              idList->Reset();

              edgePt = newPoints->InsertNextPoint(projEdgeP2);
              newPointData->CopyData(pointData, pts[j], edgePt);
              idList->InsertNextId(edgePt);
              CreateInterPoints(edge, projEdgeP2, p2, projP2, extent, newPoints, pointData, newPointData, pts[j],
                                idList, projRef, this->InterpolationDistance, 1);
              }
            else
              {
              CreateInterPoints(p1, projP1, p2, projP2, extent, newPoints, pointData, newPointData, pts[j - 1], idList,
                                projRef, this->InterpolationDistance, 1);
              }
            idList->InsertNextId(pts[j]);
            }

          p1[0] = p2[0];
          p1[1] = p2[1];
          p1[2] = p2[2];
          projP1[0] = projP2[0];
          projP1[1] = projP2[1];
          projP1[2] = projP2[2];
          }

        if (idList->GetNumberOfIds() > 1)
          {
          newCellId = output->InsertNextCell(VTK_LINE, idList);
          newCellData->CopyData(cellData, cellId, newCellId);
          }
        }
      }
    idList->Delete();
    }

  // Polys need to be divided into sub-polys if they cross a projection boundary
  if (polys->GetNumberOfCells() > 0)
    {
    vtkCellArray *newPolys = vtkCellArray::New();
    vtkIdList *idList[2];
    vtkIdType numPolys;
    vtkIdType cellId;

    // initialize newCellData for CopyData invocations
    newCellData->CopyAllocate(cellData);

    output->SetPolys(newPolys);
    newPolys->Delete();
    numPolys = polys->GetNumberOfCells();

    // std::list 0 is for polys where the first point is in the range
    // [cuttingLongitude, CenterLongitude] or [CenterLongitude, cuttingLongitude]
    // (depending on which of the two longitudes is the smallest)
    // std::list 1 is for the other polys
    idList[0] = vtkIdList::New();
    idList[0]->Allocate(1000);
    idList[1] = vtkIdList::New();
    idList[1]->Allocate(1000);

    polys->InitTraversal();
    for (cellId = 0; cellId < numPolys; cellId++)
      {
      vtkIdType npts = 0;
      vtkIdType *pts = NULL;

      polys->GetNextCell(npts, pts);

      if (npts > 0)
        {
        vtkIdType newCellId;
        int currentList;
        int leftSide[2];
        // We need to keep track of the unprojected first and last point
        // of the idLists (in order to create a proper interpolation between
        // the last and first point if that is necessary)
        double listFirstP[2][3];
        double listLastP[2][3];
        double p1[3];
        double projP1[3];
        int j;

        points->GetPoint(pts[0], p1);
        newPoints->GetPoint(pts[0], projP1);
        p1[2] =0;
        projP1[2] =0;
        while (p1[0] >= 180)
          {
          p1[0] -= 360;
          }
        while (p1[0] < -180)
          {
          p1[0] += 360;
          }
        idList[0]->Reset();
        idList[1]->Reset();

        if (this->CenterLongitude > cuttingLongitude)
          {
          leftSide[0] = (p1[0] >= cuttingLongitude && p1[0] <= this->CenterLongitude);
          }
        else
          {
          leftSide[0] = (p1[0] >= cuttingLongitude || p1[0] <= this->CenterLongitude);
          }
        currentList = leftSide[0] ? 0 : 1;

        idList[currentList]->InsertNextId(pts[0]);
        listFirstP[currentList][0] = p1[0];
        listFirstP[currentList][1] = p1[1];
        listFirstP[currentList][2] = p1[2];
        listLastP[currentList][0] = p1[0];
        listLastP[currentList][1] = p1[1];
        listLastP[currentList][2] = p1[2];

        for (j = 1; j < npts; j++)
          {
          double p2[3];
          double projP2[3];
          int splitPolySegment = 0;

          points->GetPoint(pts[j], p2);
          newPoints->GetPoint(pts[j], projP2);
          p2[2] =0;
          projP2[2] =0;
          while (p2[0] >= 180)
            {
            p2[0] -= 360;
            }
          while (p2[0] < -180)
            {
            p2[0] += 360;
            }

          // check whether both points lie in the range
          // [cuttingLongitude, CenterLongitude] or
          // [CenterLongitude, cuttingLongitude]
          if (this->CenterLongitude > cuttingLongitude)
            {
            leftSide[1] = (p2[0] >= cuttingLongitude && p2[0] <= this->CenterLongitude);
            }
          else
            {
            leftSide[1] = (p2[0] >= cuttingLongitude || p2[0] <= this->CenterLongitude);
            }
          if (leftSide[0] != leftSide[1])
            {
            double d1, d2;
            // check whether the shortest path between both points crosses
            // the cuttingLongitude meridian (or the CenterLongitude meridian)
            d1 = fabs(p1[0] - cuttingLongitude);
            if (d1 >= 180)
              {
              d1 = 360 - d1;
              }
            d2 = fabs(p2[0] - cuttingLongitude);
            if (d2 >= 180)
              {
              d2 = 360 - d2;
              }
            splitPolySegment = (d1 + d2 <= 180);
            }
          if (splitPolySegment)
            {
            // points are on different sides of the cutting edge ->
            // introduce extra points at both sides (in projected space) of cutting edge
            double edge[3];
            double projEdgeP1[3];
            double projEdgeP2[3];
            vtkIdType edgePt;

            edge[0] = cuttingLongitude;
            edge[1] = cuttingpoint(p1[0], p1[1], p2[0], p2[1], cuttingLongitude);
            edge[2] = (p1[2] + p2[2]) / 2;  // TODO: Use proper weighting
            projData.u = edge[0] * DEG_TO_RAD;
            projData.v = edge[1] * DEG_TO_RAD;
            projData = pj_fwd(projData, projRef);
            projEdgeP1[0] = (projData.u - extent[0]) / (extent[1] - extent[0]);
            projEdgeP1[1] = (projData.v - extent[2]) / (extent[3] - extent[2]);
            projEdgeP1[2] = edge[2];
            projEdgeP2[0] = projEdgeP1[0];
            projEdgeP2[1] = projEdgeP1[1];
            projEdgeP2[2] = projEdgeP1[2];
            if ((leftSide[0] && projEdgeP1[0] > 0.5) || (!leftSide[0] && projEdgeP1[0] < 0.5))
              {
              projEdgeP1[0] = 1.0 - projEdgeP1[0];
              }
            if ((leftSide[1] && projEdgeP2[0] > 0.5) || (!leftSide[1] && projEdgeP2[0] < 0.5))
              {
              projEdgeP2[0] = 1.0 - projEdgeP2[0];
              }
            edgePt = newPoints->InsertNextPoint(projEdgeP1);
            newPointData->CopyData(pointData, pts[j - 1], edgePt);
            CreateInterPoints(p1, projP1, edge, projEdgeP1, extent, newPoints, pointData, newPointData, pts[j - 1],
                              idList[currentList], projRef, this->InterpolationDistance, 1);
            idList[currentList]->InsertNextId(edgePt);

            if ((currentList == 0 && leftSide[1]) ||
                (currentList == 1 && !leftSide[1]))
              {
              // we've made a trip around the world, so we don't switch to
              // the other std::list;
              // we are going to connect both sides via one of the poles
              double polar[3];
              double projPolarP1[3];
              double projPolarP2[3];
              vtkIdType polarPt;

              polar[0] = edge[0];
              polar[1] = (edge[1] >= 0 ? 90 : -90);
              polar[2] = edge[2];
              projData.u = polar[0] * DEG_TO_RAD;
              projData.v = polar[1] * DEG_TO_RAD;
              projData = pj_fwd(projData, projRef);
              projPolarP1[0] = (projData.u - extent[0]) / (extent[1] - extent[0]);
              projPolarP1[1] = (projData.v - extent[2]) / (extent[3] - extent[2]);
              projPolarP1[2] = projEdgeP1[2];
              projPolarP2[0] = projPolarP1[0];
              projPolarP2[1] = projPolarP1[1];
              projPolarP2[2] = projPolarP1[2];
              if ((leftSide[0] && projPolarP1[0] > 0.5) || (!leftSide[0] && projPolarP1[0] < 0.5))
                {
                projPolarP1[0] = 1.0 - projPolarP1[0];
                }
              if ((leftSide[1] && projPolarP2[0] > 0.5) || (!leftSide[1] && projPolarP2[0] < 0.5))
                {
                projPolarP2[0] = 1.0 - projPolarP2[0];
                }
              polarPt = newPoints->InsertNextPoint(projPolarP1);
              newPointData->CopyData(pointData, pts[j - 1], polarPt);
              CreateInterPoints(edge, projEdgeP1, polar, projPolarP1, extent, newPoints, pointData, newPointData,
                                pts[j - 1], idList[currentList], projRef, this->InterpolationDistance, 1);
              idList[currentList]->InsertNextId(polarPt);
              polarPt = newPoints->InsertNextPoint(projPolarP2);
              newPointData->CopyData(pointData, pts[j], polarPt);
              CreateInterPoints(polar, projPolarP2, edge, projEdgeP2, extent, newPoints, pointData, newPointData,
                                pts[j], idList[currentList], projRef, this->InterpolationDistance, 1);
              idList[currentList]->InsertNextId(polarPt);
              }
            else
              {
              // switch poly
              listLastP[currentList][0] = edge[0];
              listLastP[currentList][1] = edge[1];
              listLastP[currentList][2] = edge[2];
              currentList = 1 - currentList;
              if (idList[currentList]->GetNumberOfIds() == 0)
                {
                listFirstP[currentList][0] = edge[0];
                listFirstP[currentList][1] = edge[1];
                listFirstP[currentList][2] = edge[2];
                }
              }

            edgePt = newPoints->InsertNextPoint(projEdgeP2);
            newPointData->CopyData(pointData, pts[j], edgePt);
            idList[currentList]->InsertNextId(edgePt);
            CreateInterPoints(edge, projEdgeP2, p2, projP2, extent, newPoints, pointData, newPointData, pts[j],
                              idList[currentList], projRef, this->InterpolationDistance, 1);
            }
          else
            {
            CreateInterPoints(p1, projP1, p2, projP2, extent, newPoints, pointData, newPointData, pts[j - 1],
                              idList[currentList], projRef, this->InterpolationDistance, 1);
            }

          idList[currentList]->InsertNextId(pts[j]);

          p1[0] = p2[0];
          p1[1] = p2[1];
          p1[2] = p2[2];
          projP1[0] = projP2[0];
          projP1[1] = projP2[1];
          projP1[2] = projP2[2];
          leftSide[0] = leftSide[1];
          }
        listLastP[currentList][0] = p1[0];
        listLastP[currentList][1] = p1[1];
        listLastP[currentList][2] = p1[2];

        if (idList[0]->GetNumberOfIds() > 1)
          {
          vtkIdType id1, id2;
          id1 = idList[0]->GetId(0);
          id2 = idList[0]->GetId(idList[0]->GetNumberOfIds() - 1);
          // if the first and last id are not the same interpolate between
          // these points
          if (id1 != id2)
            {
            double projFirstP[3];
            double projLastP[3];
            newPoints->GetPoint(id1, projFirstP);
            newPoints->GetPoint(id2, projLastP);
            projFirstP[2] =0;
            projLastP[2] =0;

            if (listFirstP[0][0] != listLastP[0][0] ||
                listFirstP[0][1] != listLastP[0][1])
              {
              CreateInterPoints(listLastP[0], projLastP, listFirstP[0], projFirstP, extent, newPoints, pointData,
                                newPointData, id2, idList[0], projRef, this->InterpolationDistance, 1);
              }
            }
          newCellId = output->InsertNextCell(VTK_POLYGON, idList[0]);
          newCellData->CopyData(cellData, cellId, newCellId);
          }
        if (idList[1]->GetNumberOfIds() > 1)
          {
          vtkIdType id1, id2;
          id1 = idList[1]->GetId(0);
          id2 = idList[1]->GetId(idList[1]->GetNumberOfIds() - 1);
          // if the first and last id are not the same interpolate between
          // these points
          if (id1 != id2)
            {
            double projFirstP[3];
            double projLastP[3];
            projFirstP[2] =0;
            projLastP[2] =0;
            newPoints->GetPoint(id1, projFirstP);
            newPoints->GetPoint(id2, projLastP);
            if (listFirstP[1][0] != listLastP[1][0] ||
                listFirstP[1][1] != listLastP[1][1])
              {
              CreateInterPoints(listLastP[1], projLastP, listFirstP[1], projFirstP, extent, newPoints, pointData,
                                newPointData, id2, idList[1], projRef, this->InterpolationDistance, 1);
              }
            }
          newCellId = output->InsertNextCell(VTK_POLYGON, idList[1]);
          newCellData->CopyData(cellData, cellId, newCellId);
          }
        }
      }
    idList[0]->Delete();
    idList[1]->Delete();
    }

  pj_free(projRef);
}


void vtkProj2DFilter::PerformSemiProjection()
{
  vtkPolyData *input;
  vtkPolyData *output;
  vtkPoints *points;
  vtkPoints *newPoints;
  vtkPoints *finalpoints;
  vtkCellArray *verts;
  vtkCellArray *lines;
  vtkCellArray *polys;
  vtkPointData *pointData;
  vtkPointData *newPointData;
  vtkCellData *cellData;
  vtkCellData *newCellData;
  vtkIdType id;
  vtkIdType numPoints;
  PJ *projRef;
  projUV projData;
  char centerLongitudeParam[100];
  char centerLatitudeParam[100];
  double extent[6];
  double cuttingLongitude;
  vtkDataArray *vectors = NULL;

  vtkDebugMacro(<<"Performing Semi projection on polygonal data")

  input = this->GetInput();
  points = input->GetPoints();
  verts = input->GetVerts();
  lines = input->GetLines();
  polys = input->GetPolys();
  pointData = input->GetPointData();
  cellData = input->GetCellData();

  vectors = pointData->GetVectors();
  if ( !vectors || !(vectors->GetNumberOfTuples() > 0) )
    vectors = NULL;

  output = this->GetOutput();

  // create the vtkCellArray in output
  output->Allocate(input);
  newPointData = output->GetPointData();
  newCellData = output->GetCellData();

  // points need to be transfered and projected
  numPoints = points->GetNumberOfPoints();
  newPoints = vtkPoints::New();
  newPoints->SetDataTypeToFloat();
  newPoints->SetNumberOfPoints(numPoints);

  cuttingLongitude = this->CenterLongitude + 180.0;
  if (cuttingLongitude >= 180.0)
    {
    cuttingLongitude -= 360.0;
    }

  static const char *parameters[] = {
    "",
    "",
    "",
    "R=1.0",
    "ellps=WGS84",
    "no_defs",
    ""
  };

  switch (this->Projection)
    {
    case VTK_PROJ2D_ORTHO:
      parameters[0] = "proj=ortho";
      break;
    case VTK_PROJ2D_NEAR_SIGHTED:
      parameters[0] = "proj=nsper";
      parameters[6] = "h=8";
      break;
    case VTK_PROJ2D_STEREOGRAPHIC:
      parameters[0] = "proj=stere";
      parameters[6] = "k=2";
      break;
    default:
      vtkErrorMacro("Unknown semi projection");
      return;
    }
  sprintf(centerLongitudeParam, "lon_0=%7.3f", this->CenterLongitude);
  sprintf(centerLatitudeParam, "lat_0=%7.3f", this->CenterLatitude);
  parameters[1] = centerLatitudeParam;
  parameters[2] = centerLongitudeParam;

  projRef = pj_init(sizeof(parameters)/sizeof(char *), const_cast<char**>( parameters) );
  if (projRef == 0)
    {
    vtkErrorMacro(<<"Could not initialize PROJ library ("
                  << pj_strerrno(pj_errno) << ")");
    return;
    }


  GetExtent(this->Projection, extent);

    for (id = 0; id < numPoints; ++id)
  {
    double pt[3];
    double npt[3];

    points->GetPoint(id, pt);
    while (pt[0] >= 180)
      {
      pt[0] -= 360;
      }
    while (pt[0] < -180)
      {
      pt[0] += 360;
      }
    projData.u = pt[0] * DEG_TO_RAD;
    projData.v = pt[1] * DEG_TO_RAD;
    projData = pj_fwd(projData, projRef);
    npt[0] = (projData.u - extent[0]) / (extent[1] - extent[0]);
    npt[1] = (projData.v - extent[2]) / (extent[3] - extent[2]);

    if ( !isVisible(projData.u, projData.v) )
    {
        npt[0] = npt[1] = HUGE_VAL;
    }

    newPoints->SetPoint(id, npt);


    // transform the std::vector data
     if ( vectors && isVisible(npt[0], npt[1]))
     {
         double magnitude =1;
         double nPoint[3];
         double *tuple =  vectors->GetTuple3(id);
         magnitude = sqrt(tuple[0]*tuple[0] + tuple[1]*tuple[1]);

         // this is where the original std::vector points in 2D untransformed space
         nPoint[0] = pt[0] + tuple[0];
         nPoint[1] = pt[1] + tuple[1];

         //now transform this point
         while (nPoint[0] >= 180)
          {
          nPoint[0] -= 360;
          }
        while (nPoint[0] < -180)
          {
          nPoint[0] += 360;
          }

        projData.u = nPoint[0] * DEG_TO_RAD;
        projData.v = nPoint[1] * DEG_TO_RAD;
        projData = pj_fwd(projData, projRef);
        nPoint[0] = (projData.u - extent[0]) / (extent[1] - extent[0]);
        nPoint[1] = (projData.v - extent[2]) / (extent[3] - extent[2]);

        if ( !isVisible(nPoint[0], nPoint[1]) )
        {
            vectors->SetTuple3(id, 0, 0, 0);
            continue;
        }

        // calculate the new std::vector
        nPoint[0] -= npt[0];
        nPoint[1] -= npt[1];

        // we don't want to rescale the std::vector..

        // normalize this std::vector
        /*magnitudeTmp = sqrt(nPoint[0]*nPoint[0] + nPoint[1]*nPoint[1]);
        nPoint[0] /= magnitudeTmp;
        nPoint[1] /= magnitudeTmp;

        //restore to original magnitude
        nPoint[0] *= magnitude;
        nPoint[1] *= magnitude;*/


        //save the std::vector data
        vectors->SetTuple3(id, nPoint[0], nPoint[1], 0);

     } else if ( vectors && !isVisible(npt[0], npt[1]) ) {

         // otherwise these are plotted in the origin
         // they'll still be plotted, but with 0 magnitude
         vectors->SetTuple3(id, 0, 0, 0);
     }
  }

  output->SetPoints(newPoints);
  newPoints->Delete();

  // pointData is first copied and later extended as needed
  newPointData->DeepCopy(pointData);


  // Vertices are passed unmodified
  // we will ignore points that we do not see
  if (verts->GetNumberOfCells() > 0)
    {
        vtkCellArray *newVerts = vtkCellArray::New();
        vtkIdList *idList;
        vtkIdType numVerts;
        vtkIdType cellId;

        //initialize newCellData for CopyData invocations
        newCellData->CopyAllocate(cellData);

        output->SetVerts(newVerts);
        newVerts->Delete();
        numVerts = verts->GetNumberOfCells();

        idList = vtkIdList::New();
        idList->Allocate(1000);

        verts->InitTraversal();
        for (cellId = 0; cellId < numVerts; cellId++)
        {
            vtkIdType npts = 0;
            vtkIdType *pts = NULL;
            verts->GetNextCell(npts, pts);
            idList->Reset();

            if (npts > 0)
            {
                vtkIdType newCellId;
                int j;

                for (j = 0; j < npts; j++)
                {
                    double projP1[3];
                    newPoints->GetPoint(pts[j], projP1);

                    if ( isVisible(projP1) )
                    {
                        idList->InsertNextId(pts[j]);
                    }
                }

                if ( idList->GetNumberOfIds() > 1 )
                {
                    newCellId = output->InsertNextCell(VTK_VERTEX, idList);
                    newCellData->CopyData(cellData, cellId, newCellId);
                }
            }
        }

    idList->Delete();
    }


   // Lines need to be cut when passing a projection boundary
  if (lines->GetNumberOfCells() > 0)
    {
    vtkCellArray *newLines = vtkCellArray::New();
    vtkIdList *idList;
    vtkIdType numLines;
    vtkIdType cellId;

    // initialize newCellData for CopyData invocations
    newCellData->CopyAllocate(cellData);

    output->SetLines(newLines);
    newLines->Delete();
    numLines = lines->GetNumberOfCells();

    idList = vtkIdList::New();
    idList->Allocate(1000);

    lines->InitTraversal();
    for (cellId = 0; cellId < numLines; cellId++)
      {
      vtkIdType npts = 0;
      vtkIdType *pts = NULL;

      lines->GetNextCell(npts, pts);

      if (npts > 0)
        {
        vtkIdType newCellId;
        double p1[3];
        double projP1[3];
        int j;

        points->GetPoint(pts[0], p1);
        newPoints->GetPoint(pts[0], projP1);
        while (p1[0] >= 180)
          {
          p1[0] -= 360;
          }
        while (p1[0] < -180)
          {
          p1[0] += 360;
          }
        idList->Reset();


        if ( isVisible(projP1) )
            {
            // we see this point, so include it in the new set of points
            // otherwise this point is over the horizon -> we don't see it -> discard it
            idList->InsertNextId(pts[0]);
            }


        for (j = 1; j < npts; j++)
          {
          double p2[3];
          double projP2[3];
          double cutpoint[3];
          double projcutpoint[3];
          vtkIdType cutptid;

          points->GetPoint(pts[j], p2);
          newPoints->GetPoint(pts[j], projP2);
          while (p2[0] >= 180)
            {
            p2[0] -= 360;
            }
          while (p2[0] < -180)
            {
            p2[0] += 360;
            }


         if ( !isVisible(projP1) && isVisible(projP2))
         {
              // so this is the first point we see from the line, so we create a new line segment
              idList->Reset();

              // we calculate the cutting point between the two
              // first the visible point, then the invisible one

              cuttingpoint(p2[0], p2[1], p1[0], p1[1], &cutpoint[0], &cutpoint[1], projRef);

              projData.u = cutpoint[0] * DEG_TO_RAD;
              projData.v = cutpoint[1] * DEG_TO_RAD;
              projData = pj_fwd(projData, projRef);

              projcutpoint[0] = (projData.u - extent[0]) / (extent[1] - extent[0]);
              projcutpoint[1] = (projData.v - extent[2]) / (extent[3] - extent[2]);
			  projcutpoint[2] = 0;

              // add new points from the cutting point to the
              // visible point
              cutptid = newPoints->InsertNextPoint(projcutpoint);
              newPointData->CopyData(pointData, pts[j], cutptid);
              idList->InsertNextId(cutptid);

              CreateInterPoints(cutpoint, projcutpoint, p2, projP2, extent,
                                newPoints, pointData, newPointData, pts[j-1], idList,
                                projRef, this->InterpolationDistance, 1);


              // no we add this point to the line, because it's visible
              idList->InsertNextId(pts[j]);

          }
          else if ( isVisible(projP1) && !isVisible(projP2))
          {
              // this is the first point we DON'T see anymore, so this will be the end of this line

              // we calculate the cutting point between the two
              // first the visible point, then the invisible one
              cuttingpoint(p1[0], p1[1], p2[0], p2[1], &cutpoint[0], &cutpoint[1], projRef);

              projData.u = cutpoint[0] * DEG_TO_RAD;
              projData.v = cutpoint[1] * DEG_TO_RAD;
              projData = pj_fwd(projData, projRef);

              projcutpoint[0] = (projData.u - extent[0]) / (extent[1] - extent[0]);
              projcutpoint[1] = (projData.v - extent[2]) / (extent[3] - extent[2]);
			  projcutpoint[2] = 0;

              // add new points from the cutting point to the
              // visible point
              cutptid = newPoints->InsertNextPoint(projcutpoint);
              newPointData->CopyData(pointData, pts[j-1], cutptid);

              CreateInterPoints(p1, projP1, cutpoint, projcutpoint, extent,
                                newPoints, pointData, newPointData, pts[j - 1], idList,
                                projRef, this->InterpolationDistance, 1);

              idList->InsertNextId(cutptid);

              // copy the data to this new line sgment
              newCellId = output->InsertNextCell(VTK_LINE, idList);
              newCellData->CopyData(cellData, cellId, newCellId);

              idList->Reset();

          }
          else if ( isVisible(projP2) )
          {
              // both are visible, so add them!
            CreateInterPoints(p1, projP1, p2, projP2, extent, newPoints, pointData, newPointData, pts[j - 1], idList,
                                projRef, this->InterpolationDistance, 1);

            idList->InsertNextId(pts[j]);

          }
          else
          {
              // none are visible, ignore these points
          }


          // follow the line
          p1[0] = p2[0];
          p1[1] = p2[1];
          p1[2] = p2[2];
          projP1[0] = projP2[0];
          projP1[1] = projP2[1];
          projP1[2] = projP2[2];
        } // for

        if (idList->GetNumberOfIds() > 1)
        {
                    newCellId = output->InsertNextCell(VTK_LINE, idList);
                    newCellData->CopyData(cellData, cellId, newCellId);
        }

       } // if this line has points
    } // for: iteration over lines
    idList->Delete();
   }


  // Polys need to be cut if they cross a projection boundary
  // the section that is over the horizon (not visible)
  // is discarted (not copied to the output)
  if (polys->GetNumberOfCells() > 0)
    {
    vtkCellArray *newPolys = vtkCellArray::New();
    vtkIdList *idList;
    vtkIdType numPolys;
    vtkIdType cellId;

    // initialize newCellData for CopyData invocations
    newCellData->CopyAllocate(cellData);

    output->SetPolys(newPolys);
    newPolys->Delete();
    numPolys = polys->GetNumberOfCells();

    // std::list of points for this poly
    idList = vtkIdList::New();
    idList->Allocate(1000);

    polys->InitTraversal();
    for (cellId = 0; cellId < numPolys; cellId++)
      {
      vtkIdType npts = 0;
      vtkIdType *pts = NULL;

      polys->GetNextCell(npts, pts);

      if (npts > 0)
        {
        vtkIdType newCellId;

        double p1[3];
        double projP1[3];

        // the first and last visible points of the polygon
        double first[3] = {HUGE_VAL, HUGE_VAL, HUGE_VAL};
        double projFirst[3] = {HUGE_VAL, HUGE_VAL, HUGE_VAL};
        double last[3] = {HUGE_VAL, HUGE_VAL, HUGE_VAL};
        double projLast[3] = {HUGE_VAL, HUGE_VAL, HUGE_VAL};

        int j;

        points->GetPoint(pts[0], p1);
        newPoints->GetPoint(pts[0], projP1);
        while (p1[0] >= 180)
          {
          p1[0] -= 360;
          }
        while (p1[0] < -180)
          {
          p1[0] += 360;
          }
        idList->Reset();

         if ( isVisible(projP1) )
         {
            idList->InsertNextId(pts[0]);

            // this is our first visible point
            first[0] = p1[0];
            first[1] = p1[1];
            first[2] = p1[2];
         }

        for (j = 1; j < npts; j++)
          {
          double p2[3];
          double projP2[3];
          double cutpoint[3];
          double projcutpoint[3];

          points->GetPoint(pts[j], p2);
          newPoints->GetPoint(pts[j], projP2);
          while (p2[0] >= 180)
            {
            p2[0] -= 360;
            }
          while (p2[0] < -180)
            {
            p2[0] += 360;
            }

            // going from a nonvisible(over the edge) to a visible point
            if ( !isVisible(projP1) && isVisible(projP2) )
            {

              // find out the border point
              // we calculate the cutting point between the two
              // first the visible point, then the invisible one

              cuttingpoint(p2[0], p2[1], p1[0], p1[1], &cutpoint[0], &cutpoint[1], projRef);

              projData.u = cutpoint[0] * DEG_TO_RAD;
              projData.v = cutpoint[1] * DEG_TO_RAD;
              projData = pj_fwd(projData, projRef);

              projcutpoint[0] = (projData.u - extent[0]) / (extent[1] - extent[0]);
              projcutpoint[1] = (projData.v - extent[2]) / (extent[3] - extent[2]);

                // 2 check to see if this the first visible point
                // of the poly
                if ( first[0] == HUGE_VAL)
                {
                        //ohh, then this is our first visible point!
                        first[0] = cutpoint[0];
                        first[1] = cutpoint[1];
                        first[2] = cutpoint[2];

                        projFirst[0] = projcutpoint[0];
                        projFirst[1] = projcutpoint[1];
                        projFirst[2] = projcutpoint[2];
                }

                // 3 connect with the last border point
                // if there is one
                if ( last[0] != HUGE_VAL )
                {
                    // from the last borderpoint to the cutpoint
                     CreateInterPoints(last, projLast, cutpoint, projcutpoint, extent,
                                newPoints, pointData, newPointData, pts[j - 1], idList,
                                projRef, this->InterpolationDistance, 1);


                }

              // add new points from the cutting point to this
              // visible point
              CreateInterPoints(cutpoint, projcutpoint, p2, projP2, extent,
                                newPoints, pointData, newPointData, pts[j - 1], idList,
                                projRef, this->InterpolationDistance, 1);


                idList->InsertNextId(pts[j]);
            }

            // going from a visible point to one over the edge
            else if ( isVisible(projP1) && !isVisible(projP2) )
            {
                // 1 find the border point
               /*cuttingpoint(p1[0], p1[1], p2[0], p2[1], &cutpoint[0], &cutpoint[1], projRef);

               projData.u = cutpoint[0] * DEG_TO_RAD;
               projData.v = cutpoint[1] * DEG_TO_RAD;
               projData = pj_fwd(projData, projRef);

               projcutpoint[0] = (projData.u - extent[0]) / (extent[1] - extent[0]);
               projcutpoint[1] = (projData.v - extent[2]) / (extent[3] - extent[2]);

               // add new points from the cutting point to the
              // visible point
              CreateInterPoints(p1, projP1, cutpoint, projcutpoint, extent,
                                newPoints, pointData, newPointData, pts[j - 1], idList,
                                projRef, this->InterpolationDistance, 1);


                // this border point becomes the last border point
                last[0] = cutpoint[0];
                last[1] = cutpoint[1];
                projLast[0] = projcutpoint[0];
                projLast[1] = projcutpoint[1];*/

            }

            // both points are visible
            else if (isVisible(projP2) )
            {
                 // interpolate between the points
                CreateInterPoints(p1, projP1, p2, projP2, extent, newPoints, pointData, newPointData, pts[j - 1], idList,
                                projRef, this->InterpolationDistance, 1);

                idList->InsertNextId(pts[j]);
            }
            else
            {
              // none are visible, ignore these points
            }

          p1[0] = p2[0];
          p1[1] = p2[1];
          p1[2] = p2[2];
          projP1[0] = projP2[0];
          projP1[1] = projP2[1];
          projP1[2] = projP2[2];

        }

        if (idList->GetNumberOfIds() > 1)
          {
          vtkIdType id1, id2;
          id1 = idList->GetId(0);
          id2 = idList->GetId(idList->GetNumberOfIds() - 1);
          // if the first and last id are not the same interpolate between
          // these points
          if (id1 != id2)
            {
            double firstP[3], lastP[3];
            double projFirstP[3], projLastP[3];

            points->GetPoint(id1, firstP);
            points->GetPoint(id2, lastP);

            newPoints->GetPoint(id1, projFirstP);
            newPoints->GetPoint(id2, projLastP);

            if (firstP[0] != lastP[0] ||
                firstP[1] != lastP[1])
              {
              CreateInterPoints(lastP, projLastP, firstP, projFirstP, extent, newPoints, pointData,
                                newPointData, id2, idList, projRef, this->InterpolationDistance, 1);
              }
            }
            newCellId = output->InsertNextCell(VTK_POLYGON, idList);
            newCellData->CopyData(cellData, cellId, newCellId);
          }
        }
      }
    idList->Delete();
    }


  // cleanup points.. remove the ones that are not visible
  finalpoints =  vtkPoints::New();
  finalpoints->SetDataTypeToFloat();
  numPoints = newPoints->GetNumberOfPoints();
  for ( int j =0; j < numPoints; j++ )
  {
      double pt[3];

      newPoints->GetPoint(j, pt);

      if ( isVisible(pt) )
      {
          finalpoints->InsertPoint(j, pt);
      }
  }
  output->SetPoints(finalpoints);
  finalpoints->Delete();

  pj_free(projRef);
}

//-----------------------------------------------------------------------------------------------
void vtkProj2DFilter::normalizedDeprojection2D(int projection, double centerLat, double centerLon,
                                             double x, double y, double &lat, double &lon)
{
    double extent[6];
    PJ *projRef;
    projUV projData;
    char centerLatitudeParam[100];
    char centerLongitudeParam[100];

    static const char *parameters[] = {
      "",
      "",
      "",
      "R=1.0",
      "ellps=WGS84",
      "no_defs",
      ""
    };

    switch (projection)
    {
        case VTK_PROJ2D_LAMBERT_CYLINDRICAL:
            parameters[0] = "proj=cea";
            break;
        case VTK_PROJ2D_PLATE_CAREE:
            parameters[0] = "proj=eqc";
            break;
        case VTK_PROJ2D_MOLLWEIDE:
            parameters[0] = "proj=moll";
            break;
        case VTK_PROJ2D_ROBINSON:
            parameters[0] = "proj=robin";
            break;
        case VTK_PROJ2D_LAMBERT_AZIMUTHAL:
            parameters[0] = "proj=laea";
            break;
        case VTK_PROJ2D_AZIMUTHAL_EQUIDISTANT:
            parameters[0] = "proj=aeqd";
            break;
        case VTK_PROJ2D_MERCATOR:
            parameters[0] = "proj=merc";
            break;
        case VTK_PROJ2D_TMERCATOR:
            parameters[0] = "proj=tmerc";
            break;
        case VTK_PROJ2D_ORTHO:
            parameters[0] = "proj=ortho";
            break;
        case VTK_PROJ2D_NEAR_SIGHTED:
            parameters[0] = "proj=nsper";
            parameters[6] = "h=8";
            break;
        case VTK_PROJ2D_STEREOGRAPHIC:
            parameters[0] = "proj=stere";
            parameters[6] = "k=2";
            break;
        case VTK_PROJ2D_3D:
        default:
            // this is meaningles ...
            lat = lon = 0.0;
            return;
            break;
    }

    sprintf(centerLatitudeParam, "lat_0=%7.3f", centerLat);
    sprintf(centerLongitudeParam, "lon_0=%7.3f", centerLon);
    parameters[1] = centerLatitudeParam;
    parameters[2] = centerLongitudeParam;

    // initialize the projection library
    projRef = pj_init(sizeof(parameters)/sizeof(char *), const_cast<char**>( parameters) );
    if (projRef == 0)
    {
        return;
    }

    vtkProj2DFilter::GetExtent(projection, extent);

    // denormalize the projection point
    projData.u = x * (extent[1] - extent[0]) + extent[0];
    projData.v = y * (extent[3] - extent[2]) + extent[2];

    // map the projected point to lat,lon
    projData = pj_inv(projData, projRef);

    // convert to degrees and consider the range.
    lon = projData.u * RAD_TO_DEG;
    lat = projData.v * RAD_TO_DEG;

    pj_free(projRef);
}

//-----------------------------------------------------------------------------------------------
// added from VISAN code  KAVOK
void vtkProj2DFilter::normalizedProjection2D(int projection, double centerLat, double centerLon,
                                           double lat, double lon, double &x, double &y)
{
    double extent[6];
    PJ *projRef;
    projUV projData;
    char centerLatitudeParam[100];
    char centerLongitudeParam[100];

    static const char *parameters[] = {
      "",
      "",
      "",
      "R=1.0",
      "ellps=WGS84",
      "no_defs",
      ""
    };

    switch (projection)
    {

        case VTK_PROJ2D_LAMBERT_CYLINDRICAL:
            parameters[0] = "proj=cea";
            break;
        case VTK_PROJ2D_PLATE_CAREE:
            parameters[0] = "proj=eqc";
            break;
        case VTK_PROJ2D_MOLLWEIDE:
            parameters[0] = "proj=moll";
            break;
        case VTK_PROJ2D_ROBINSON:
            parameters[0] = "proj=robin";
            break;
        case VTK_PROJ2D_LAMBERT_AZIMUTHAL:
            parameters[0] = "proj=laea";
            break;
        case VTK_PROJ2D_AZIMUTHAL_EQUIDISTANT:
            parameters[0] = "proj=aeqd";
            break;
        case VTK_PROJ2D_MERCATOR:
            parameters[0] = "proj=merc";
            break;
        case VTK_PROJ2D_TMERCATOR:
            parameters[0] = "proj=tmerc";
            break;
        case VTK_PROJ2D_ORTHO:
            parameters[0] = "proj=ortho";
            break;
        case VTK_PROJ2D_NEAR_SIGHTED:
            parameters[0] = "proj=nsper";
            parameters[6] = "h=8";
            break;
        case VTK_PROJ2D_STEREOGRAPHIC:
            parameters[0] = "proj=stere";
            parameters[6] = "k=2";
            break;
        case VTK_PROJ2D_3D:
        default:
            // this is meaningles ...
            x = y = 0.0;
            return;
            break;
    }

    sprintf(centerLatitudeParam, "lat_0=%7.3f", centerLat);
    sprintf(centerLongitudeParam, "lon_0=%7.3f", centerLon);
    parameters[1] = centerLatitudeParam;
    parameters[2] = centerLongitudeParam;

    // initialize the projection library
    projRef = pj_init(sizeof(parameters)/sizeof(char *), const_cast<char**>( parameters) );
    if (projRef == 0)
    {
        return;
    }

    vtkProj2DFilter::GetExtent(projection, extent);

    projData.u = lon * DEG_TO_RAD;
    projData.v = lat * DEG_TO_RAD;
    projData = pj_fwd(projData, projRef);


    // normalize the projection
    x = (projData.u - extent[0]) / (extent[1] - extent[0]);
    y = (projData.v - extent[2]) / (extent[3] - extent[2]);

    pj_free(projRef);
}

void vtkProj2DFilter::calcCenterLatLon(int projection, double centerLat, double centerLon,
                                           double lat, double lon, double &x, double &y)
{
    switch ( projection ) {
        case VTK_PROJ2D_ORTHO:
        case VTK_PROJ2D_NEAR_SIGHTED:

            // the near sighted does not cover all the hemisphere as the orthographic
            // we need to handle it in 4 hemispheres.
            if ( lat > 45 )
                x = 90;
            else if ( lat < -45 )
                x = -90;
            else
                x = 0;


            if ( lon > 45 )
                y = 90;
            else if ( lon > 135 )
                y = 180;
            else if ( lon < -45 )
                y = -90;
            else if ( lon < -135 )
                y = -180;
            else
                y =0;

        break;
        default:
            x = centerLat;
            y = centerLon;
    }
}


////////////////////////////////////////////////
//
//  vtkSimpleVectorProjectFilter
//
///////////////////////////////////////////////

vtkSimpleVectorProjFilter::vtkSimpleVectorProjFilter() : vtkProj2DFilter ()
{
}

void vtkSimpleVectorProjFilter::Execute(){

  vtkPolyData *input;

  vtkDebugMacro(<<"Performing projection on polygonal data")

  // attributes Input, Output, and Points can be NULL
  // attributes Verts, Lines, Polys, Strips, PointData, and CellData are
  // never NULL (but can be empty)
  input = this->GetInput();

  if (input == NULL || input->GetPoints() == NULL)
    {
    return;
    }

  // Because vtkPolyData can't handle cellData properly when using a
  // combination of Verts, Lines, Polys, and/or Strips we restrict this
  // filter to vtkPolyData with only one type of cells
  if ((input->GetVerts()->GetNumberOfCells() > 0) +
      (input->GetLines()->GetNumberOfCells() > 0) +
      (input->GetPolys()->GetNumberOfCells() > 0) +
      (input->GetStrips()->GetNumberOfCells() > 0) > 1)
    {
    vtkErrorMacro(<<"this filter does not work on polydata with different types of cells");
    return;
    }


    if ( this->Projection == VTK_PROJ2D_3D )
    {
        PerformSimple3D();
    }
    else
    {
        PerformSimple2D();
    }

}


void vtkSimpleVectorProjFilter::PerformSimple2D()
{

  vtkPolyData *input;
  vtkPolyData *output;
  vtkPoints *points;
  vtkPoints *newPoints;
  vtkPointData *pointData;
  vtkPointData *newPointData;
  vtkCellData *cellData;
  vtkCellData *newCellData;
  vtkIdType id;
  vtkIdType numPoints;
  PJ *projRef;
  projUV projData;
  char centerLatitudeParam[100];
  char centerLongitudeParam[100];
  double extent[6];
  double cuttingLatitude;
  double cuttingLongitude;

  vtkDebugMacro(<<"Performing Point projection conversion")

  input = this->GetInput();
  points = input->GetPoints();

  pointData = input->GetPointData();
  cellData = input->GetCellData();

  output = this->GetOutput();

  // create the vtkCellArray in output
  output->Allocate(input);
  newPointData = output->GetPointData();
  newCellData = output->GetCellData();

  // points need to be transfered and projected
  numPoints = points->GetNumberOfPoints();
  newPoints = vtkPoints::New();
  newPoints->SetDataTypeToFloat();
  newPoints->SetNumberOfPoints(numPoints);

  if (this->CenterLatitude == 0.0)
    {
    cuttingLatitude = this->CenterLatitude;
    }
  else
    {
    cuttingLatitude = -this->CenterLatitude;
    }
  cuttingLongitude = this->CenterLongitude + 180.0;
  if (cuttingLongitude >= 180.0)
    {
    cuttingLongitude -= 360.0;
    }

  static const char *parameters[] = {
    "",
    "",
    "",
    "R=1.0",
    "ellps=WGS84",
    "no_defs",
    ""
  };

  switch (this->Projection)
    {
     case VTK_PROJ2D_LAMBERT_CYLINDRICAL:
            parameters[0] = "proj=cea";
            break;
        case VTK_PROJ2D_PLATE_CAREE:
            parameters[0] = "proj=eqc";
            break;
        case VTK_PROJ2D_MOLLWEIDE:
            parameters[0] = "proj=moll";
            break;
        case VTK_PROJ2D_ROBINSON:
            parameters[0] = "proj=robin";
            break;
        case VTK_PROJ2D_LAMBERT_AZIMUTHAL:
            parameters[0] = "proj=laea";
            break;
        case VTK_PROJ2D_AZIMUTHAL_EQUIDISTANT:
            parameters[0] = "proj=aeqd";
            break;
        case VTK_PROJ2D_MERCATOR:
            parameters[0] = "proj=merc";
            break;
        case VTK_PROJ2D_TMERCATOR:
            parameters[0] = "proj=tmerc";
            break;
        case VTK_PROJ2D_ORTHO:
            parameters[0] = "proj=ortho";
            break;
        case VTK_PROJ2D_NEAR_SIGHTED:
            parameters[0] = "proj=nsper";
            parameters[6] = "h=8";
            break;
        case VTK_PROJ2D_STEREOGRAPHIC:
            parameters[0] = "proj=stere";
            parameters[6] = "k=2";
            break;
        default:
            vtkErrorMacro("Unknown 2D projection");
            return;
    }

  sprintf(centerLatitudeParam, "lat_0=%7.3f", this->CenterLatitude);
  sprintf(centerLongitudeParam, "lon_0=%7.3f", this->CenterLongitude);
  parameters[1] = centerLatitudeParam;
  parameters[2] = centerLongitudeParam;

  projRef = pj_init(sizeof(parameters)/sizeof(char *), const_cast<char**>( parameters) );
  if (projRef == 0)
    {
    vtkErrorMacro(<<"Could not initialize PROJ library ("
                  << pj_strerrno(pj_errno) << ")");
    return;
    }


  if ( pointData->GetNumberOfComponents() != 3 )
  {
      vtkErrorMacro(<<"Point data does not contain proper std::vector data.");
      return;
  }

  GetExtent(this->Projection, extent);

  newPointData->DeepCopy(pointData);
  vtkDataArray *vectorData = newPointData->GetVectors();


  for (id = 0; id < numPoints; ++id)
    {
    double pt[3];
    double *vdata;
    double magnitude = 1;

    points->GetPoint(id, pt);
    vdata = vectorData->GetTuple3(id);
    magnitude = sqrt(vdata[0]*vdata[0] + vdata[1]*vdata[1]);


    while (pt[0] >= 180)
      {
      pt[0] -= 360;
      }
    while (pt[0] < -180)
      {
      pt[0] += 360;
      }

    // convert the std::vector direction as well
    while (vdata[0] >= 180)
      {
      vdata[0] -= 360;
      }
    while (vdata[0] < -180)
      {
      vdata[0] += 360;
      }

    // we need to convert the vdata to an actual point in the projection
    vdata[0] += pt[0];
    vdata[1] += pt[1];



    projData.u = pt[0] * DEG_TO_RAD;
    projData.v = pt[1] * DEG_TO_RAD;
    projData = pj_fwd(projData, projRef);
    pt[0] = (projData.u - extent[0]) / (extent[1] - extent[0]);
    pt[1] = (projData.v - extent[2]) / (extent[3] - extent[2]);
    newPoints->SetPoint(id, pt);


    // conversion of std::vector
    projData.u = vdata[0] * DEG_TO_RAD;
    projData.v = vdata[1] * DEG_TO_RAD;
    projData = pj_fwd(projData, projRef);
    vdata[0] = (projData.u - extent[0]) / (extent[1] - extent[0]);
    vdata[1] = (projData.v - extent[2]) / (extent[3] - extent[2]);

    //return to std::vector, and not absolute point
    if ( vdata[0] != HUGE_VAL && vdata[1] != HUGE_VAL )
    {
        double n_magnitude = 1;

        vdata[0] -= pt[0];
        vdata[1] -= pt[1];

        // we have the direction, now to original magnitude
        n_magnitude = sqrt(vdata[0]*vdata[0] + vdata[1]*vdata[1]);
        vdata[0] *= (magnitude/n_magnitude);
        vdata[1] *= (magnitude/n_magnitude);

    }
    else
    {
        vdata[0] = 0;
        vdata[1] = 0;
    }

    vectorData->SetTuple3(id, vdata[0], vdata[1], vdata[2]);

    }

  output->SetPoints(newPoints);
  newPoints->Delete();

  // pointData is first copied and later extended as needed
  //newPointData->DeepCopy(pointData);


  pj_free(projRef);
}


void vtkSimpleVectorProjFilter::PerformSimple3D()
{
  vtkPolyData *input;
  vtkPolyData *output;
  vtkPoints *points;
  vtkPoints *newPoints;
  vtkPointData *pointData;
  vtkPointData *newPointData;
  vtkCellData *cellData;
  vtkCellData *newCellData;
  vtkIdType id;
  vtkIdType numPoints;
  //vtkDoubleArray *newVectorData;

  vtkDebugMacro(<<"Performing Simple 3D projection")

  input = this->GetInput();
  points = input->GetPoints();
  pointData = input->GetPointData();
  cellData = input->GetCellData();

  output = this->GetOutput();

  // create the vtkCellArray in output
  output->Allocate(input);
  newPointData = output->GetPointData();
  newCellData = output->GetCellData();

  // points need to be transfered and projected
  numPoints = points->GetNumberOfPoints();
  newPoints = vtkPoints::New();
  newPoints->SetDataTypeToFloat();
  newPoints->SetNumberOfPoints(numPoints);

  newPointData->DeepCopy(pointData);

  //newVectorData = vtkDoubleArray::New();
  //newVectorData->SetNumberOfComponents(3);
  vtkDataArray *vectorData =newPointData->GetVectors();
  //newPointData->SetVectors(newVectorData);



  for (id = 0; id < numPoints; ++id)
    {
    double sinLatitude, cosLatitude, sinLongitude, cosLongitude;
    double pt[3];
    double R;
    double *vdata;
    double magnitude = 1;

    points->GetPoint(id, pt);
    vdata = vectorData->GetTuple3(id);
    magnitude = sqrt(vdata[0]*vdata[0] + vdata[1]*vdata[1]);

     // we need to convert the vdata to an actual point in the projection
    vdata[0] += pt[0];
    vdata[1] += pt[1];


    R = 1;
    if (pt[2] > 0)
     {
     // use z value as radius
     R = pt[2];
     }


    sinLongitude = sin(pt[0] * DEG_TO_RAD);
    cosLongitude = cos(pt[0] * DEG_TO_RAD);
    sinLatitude = sin(pt[1] * DEG_TO_RAD);
    cosLatitude = cos(pt[1] * DEG_TO_RAD);
    pt[0] = R * cosLongitude * cosLatitude;
    pt[1] = R * sinLongitude * cosLatitude;
    pt[2] = R * sinLatitude;
    newPoints->SetPoint(id, pt);

    // convert the std::vector direction
    sinLongitude = sin(vdata[0] * DEG_TO_RAD);
    cosLongitude = cos(vdata[0] * DEG_TO_RAD);
    sinLatitude = sin(vdata[1] * DEG_TO_RAD);
    cosLatitude = cos(vdata[1] * DEG_TO_RAD);
    vdata[0] = R * cosLongitude * cosLatitude;
    vdata[1] = R * sinLongitude * cosLatitude;
    vdata[2] = R * sinLatitude;

    //return to std::vector, and not absolute point
    double n_magnitude = 1;

    vdata[0] -= pt[0];
    vdata[1] -= pt[1];
    vdata[2] -= pt[2];

        // we have the direction, now to original magnitude
    n_magnitude = sqrt(vdata[0]*vdata[0] + vdata[1]*vdata[1] + vdata[2]*vdata[2]);
    vdata[0] *= (magnitude/n_magnitude);
    vdata[1] *= (magnitude/n_magnitude);
    vdata[2] *= (magnitude/n_magnitude);


    vectorData->SetTuple3(id, vdata[0], vdata[1], vdata[2]);

    }

  output->SetPoints(newPoints);
  newPoints->Delete();
  //newVectorData->Delete();

  // pointData is first copied and later extended as needed
  //newPointData->DeepCopy(pointData);

}



