/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGeoMapFilter.cxx
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
#include "Tools.h"
#include "Trace.h"
#include "LatLonPoint.h"
#include "Exception.h"
using namespace brathl;

#include "vtkProj2DFilter.h"
#include "vtkGeoMapFilter.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkShortArray.h"

#include "math.h"

// Undefine max/min first (because of windows system header conflict)
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

#define max(x,y)	((x)<(y) ? (y) : (x))
#define min(x,y)	((x)<(y) ? (x) : (y))

vtkStandardNewMacro(vtkGeoMapFilter);

vtkCxxSetObjectMacro(vtkGeoMapFilter,Values,vtkDoubleArray);
vtkCxxSetObjectMacro(vtkGeoMapFilter,Heights,vtkDoubleArray);
vtkCxxSetObjectMacro(vtkGeoMapFilter,Latitudes,vtkDoubleArray);
vtkCxxSetObjectMacro(vtkGeoMapFilter,Longitudes,vtkDoubleArray);
vtkCxxSetObjectMacro(vtkGeoMapFilter,Bitarray,vtkShortArray);
vtkCxxSetObjectMacro(vtkGeoMapFilter,ValidMercatorLatitudes,vtkShortArray);

//----------------------------------------------------------------------------
vtkGeoMapFilter::vtkGeoMapFilter()
{
  this->Values = NULL;
  this->Heights = NULL;
  this->Bitarray = NULL;
  this->ValidMercatorLatitudes = NULL;
  this->Latitudes = NULL;
  this->Longitudes = NULL;
  this->MinMappedValue = 0.0;
  this->MaxMappedValue = 1.0;
  this->OffsetLatitude = 0;
  this->OffsetLongitude = 0;
  this->Factor = 0.0;
  this->Radius = 1.0;
  this->MapWidth = 0;
  this->MapHeight = 0;
  this->Projection = VTK_PROJ2D_3D;
}

//----------------------------------------------------------------------------
vtkGeoMapFilter::~vtkGeoMapFilter()
{
  if (this->Values != NULL)
    {
    this->Values->Delete();
    }

  if (this->Heights != NULL)
    {
    this->Heights->Delete();
    }

  if (this->Bitarray != NULL)
    {
    this->Bitarray->Delete();
    }

  if (this->ValidMercatorLatitudes != NULL)
    {
    this->ValidMercatorLatitudes->Delete();
    }

  if (this->Latitudes != NULL)
    {
    this->Latitudes->Delete();
    }

  if (this->Longitudes != NULL)
    {
    this->Longitudes->Delete();
    }
}
//----------------------------------------------------------------------------
bool vtkGeoMapFilter::IsValidPoint(int32_t index)
{
  if (this->Bitarray == NULL)
  {
    return false;
  }
  if ((Projection == VTK_PROJ2D_MERCATOR) && (this->ValidMercatorLatitudes == NULL))
  {
    return false;
  }

  short* bits = this->Bitarray->GetPointer(0);
  short* validMercatorLatitudes = this->ValidMercatorLatitudes->GetPointer(0);

  if (bits == NULL)
  {
    return false;
  }
  if ((Projection == VTK_PROJ2D_MERCATOR) && (validMercatorLatitudes == NULL))
  {
    return false;
  }

  bool bOk = (bits[index] == 1);

  if (Projection == VTK_PROJ2D_MERCATOR)
  {
    bOk &= (validMercatorLatitudes[index] == 1);
  }

  return bOk;
}
//----------------------------------------------------------------------------

void vtkGeoMapFilter::Execute()
{
  vtkPolyData *output;
  vtkPoints *points;
  vtkCellArray *polys;
  vtkFloatArray *colors;
  vtkIdType *pointIds;
  vtkIdType ptId;
  double *values = NULL;
  //short* bits = NULL;
  //short* validMercatorLatitudes = NULL;
  double *heights = NULL;
  double *latitudes = NULL;
  double *longitudes = NULL;
  double interpolatedHeight;
  int numValidPoints;
  int lat;
  int lon;
  int val_id;
  int useHeights;


  useHeights = (this->Heights != NULL && this->Factor > 0 && this->MaxMappedValue > this->MinMappedValue);

  if (this->Values == NULL || this->Bitarray == NULL ||
      this->Latitudes == NULL || this->Longitudes == NULL || this->ValidMercatorLatitudes == NULL)
    {
    vtkErrorMacro(<< "One or more data arrays are empty");
    return;
    }
  if (this->Values->GetNumberOfTuples() != this->MapHeight * this->MapWidth)
    {
    vtkErrorMacro(<< "Input 'Values' contains invalid number of elements");
    return;
    }
  if (this->Bitarray->GetNumberOfTuples() != this->MapHeight * this->MapWidth)
    {
    vtkErrorMacro(<< "Input 'Bitarray' contains invalid number of elements");
    return;
    }
  if (this->ValidMercatorLatitudes->GetNumberOfTuples() != this->MapHeight * this->MapWidth)
    {
    vtkErrorMacro(<< "Input 'InvalidLatitudes' contains invalid number of elements");
    return;
    }
  if (useHeights && this->Heights->GetNumberOfTuples() != this->MapHeight * this->MapWidth)
    {
    vtkErrorMacro(<< "Input 'Heights' contains invalid number of elements");
    return;
    }
  if (this->Latitudes->GetNumberOfTuples() != this->MapHeight)
    {
    vtkErrorMacro(<< "Input 'Latitudes' contains invalid number of elements");
    return;
    }
  if (this->Longitudes->GetNumberOfTuples() != this->MapWidth)
    {
    vtkErrorMacro(<< "Input 'Longitudes' contains invalid number of elements");
    return;
    }

  values = this->Values->GetPointer(0);
  //bits = this->Bitarray->GetPointer(0);
  //validMercatorLatitudes = this->ValidMercatorLatitudes->GetPointer(0);

  if (useHeights)
    {
    heights = this->Heights->GetPointer(0);
    }

  latitudes = this->Latitudes->GetPointer(0);
  longitudes = this->Longitudes->GetPointer(0);

  if (values == NULL || this->Bitarray->GetPointer(0) == NULL || (useHeights && heights == NULL) ||
      latitudes == NULL || longitudes == NULL)
    {
    vtkErrorMacro(<< "One or more data array pointers are empty");
    return;
    }

  output = (vtkPolyData *)this->GetOutput();
  points = vtkPoints::New(VTK_DOUBLE);
  polys = vtkCellArray::New();
  colors = vtkFloatArray::New();
  output->SetPoints(points);
  points->Delete();
  output->SetPolys(polys);
  polys->Delete();
  output->GetCellData()->SetScalars(colors);
  colors->Delete();

  pointIds = new vtkIdType[(this->MapHeight + 1) * this->MapWidth];


  // Determine Southpole value
  numValidPoints = 0;
  interpolatedHeight = 0.0;

  for (lon = 0; lon < this->MapWidth; lon++)
    {
    //if (bits[lon])
    if (IsValidPoint(lon))
      {
      numValidPoints++;
      if (useHeights)
        {
        interpolatedHeight += heights[lon];
        }
      }
    }
  if (numValidPoints > 0)
    {
    if (useHeights)
      {
      interpolatedHeight = this->Radius + this->Factor * (min(max(interpolatedHeight / numValidPoints, this->MinMappedValue), this->MaxMappedValue) - this->MinMappedValue) / (this->MaxMappedValue - this->MinMappedValue);
      }
    else
      {
      interpolatedHeight = this->Radius;
      }
    for (lon = 0; lon < this->MapWidth; lon++)
      {
      //pointIds[lon] = points->InsertNextPoint(longitudes[lon] - this->OffsetLongitude, -90, interpolatedHeight);
      pointIds[lon] = points->InsertNextPoint(longitudes[lon] - this->OffsetLongitude,
                                              latitudes[0],
                                              interpolatedHeight);
      CTrace::Tracer("Southpole\tInsert pointIds[%d]=%ld at longitudes[%d]=%f, latitudes[%d]=%f",
              lon, (long int)pointIds[lon], lon, longitudes[lon]- this->OffsetLongitude,
              0, latitudes[0]);
      }
    }
  else
    {
    for (lon = 0; lon < this->MapWidth; lon++)
      {
      pointIds[lon] = -1;
      }
    }

  int nMaxHeight = this->MapHeight;
  bool fullLat = false;
  if ( CTools::AreEqual(latitudes[0],-90.0) && CTools::AreEqual(latitudes[this->MapHeight - 1], 90.0) )
  {
    nMaxHeight = this->MapHeight - 1;
    fullLat = true;
  }

  int nMaxWidth = this->MapWidth;
  bool fullLon = CTools::Compare(360.0,
                                 (CTools::NormalizeLongitude(longitudes[0], longitudes[1])-longitudes[0]) * nMaxWidth,
                                 CLatLonPoint::m_LONGITUDE_COMPARE_EPSILON) <= 0;


  // Determine Intermediate values
  val_id = 0;
  for (lat = 0; lat < this->MapHeight - 1; lat++)
  //for (lat = 0; lat < nMaxHeight; lat++)
    {
    for (lon = 0; lon < this->MapWidth; lon++)
      {
      int rightShift = (lon == 0 ? this->MapWidth - 1 : -1);
      if (fullLon == false)
      {
        rightShift = (lon == 0 ? 0 : -1);
      }
      numValidPoints = 0;
      interpolatedHeight = 0;
      //if (bits[val_id])
      if (IsValidPoint(val_id))
        {
        numValidPoints++;
        if (useHeights)
          {
          interpolatedHeight += heights[val_id];
          }
        }
      //if (bits[val_id + rightShift])
      if (IsValidPoint(val_id + rightShift))
        {
        numValidPoints++;
        if (useHeights)
          {
          interpolatedHeight += heights[val_id + rightShift];
          }
        }
      //if (bits[val_id + this->MapWidth])
      if (IsValidPoint(val_id + this->MapWidth))
        {
        numValidPoints++;
        if (useHeights)
          {
          interpolatedHeight += heights[val_id + this->MapWidth];
          }
        }
      //if (bits[val_id + this->MapWidth + rightShift])
      if (IsValidPoint(val_id + this->MapWidth + rightShift))
        {
        numValidPoints++;
        if (useHeights)
          {
          interpolatedHeight += heights[val_id + this->MapWidth + rightShift];
          }
        }
      ptId = -1;
      if (numValidPoints > 0)
        {
        if (useHeights)
          {
          interpolatedHeight = this->Radius + this->Factor * (min(max(interpolatedHeight / numValidPoints, this->MinMappedValue), this->MaxMappedValue) - this->MinMappedValue) / (this->MaxMappedValue - this->MinMappedValue);
          }
        else
          {
          interpolatedHeight = this->Radius;
          }
        ptId = points->InsertNextPoint(longitudes[lon] - this->OffsetLongitude, latitudes[lat] + this->OffsetLatitude, interpolatedHeight);
        CTrace::Tracer("Intermediate\tInsert pointIds[%d]=%d at longitudes[%d]=%f, latitudes[%d]=%f",
               val_id + this->MapWidth,
               (int)ptId, lon, longitudes[lon]- this->OffsetLongitude, lat, latitudes[lat] + this->OffsetLatitude);
        }
      pointIds[val_id + this->MapWidth] = ptId;
      val_id++;
      }
    }

  // Determine Northpole value
  val_id = (this->MapHeight - 1) * this->MapWidth;
  numValidPoints = 0;
  interpolatedHeight = 0.0;

  for (lon = 0; lon < this->MapWidth; lon++)
    {
//    if (bits[val_id + lon])
    if (IsValidPoint(val_id + lon))
      {
      numValidPoints++;
      if (useHeights)
        {
        interpolatedHeight += heights[val_id + lon];
        }
      }
    }
  if (numValidPoints > 0)
    {
      if (useHeights)
        {
        interpolatedHeight = this->Radius + this->Factor * (min(max(interpolatedHeight / numValidPoints, this->MinMappedValue), this->MaxMappedValue) - this->MinMappedValue) / (this->MaxMappedValue - this->MinMappedValue);
        }
      else
        {
        interpolatedHeight = this->Radius;
        }
    for (lon = 0; lon < this->MapWidth; lon++)
      {
      //pointIds[val_id + this->MapWidth + lon] = points->InsertNextPoint(longitudes[lon] - this->OffsetLongitude, 90, interpolatedHeight);
      pointIds[val_id + this->MapWidth + lon] = points->InsertNextPoint(longitudes[lon] - this->OffsetLongitude,
                                                                        latitudes[this->MapHeight-1],
                                                                        interpolatedHeight);
      CTrace::Tracer("Northpole\tInsert pointIds[%d]=%ld at longitudes[%d]=%f, latitudes[%d]=%f",
        val_id + this->MapWidth + lon, (long int)pointIds[val_id + this->MapWidth + lon], lon, longitudes[lon]- this->OffsetLongitude,
        this->MapHeight-1, latitudes[this->MapHeight-1]);
      }
    }
  else
    {
    for (lon = 0; lon < this->MapWidth; lon++)
      {
      pointIds[val_id + this->MapWidth + lon] = -1;
      }
    }

  // Create Polys
  val_id = 0;
  for (lat = 0; lat < this->MapHeight; lat++)
    {
    for (lon = 0; lon < this->MapWidth; lon++)
      {
      int rightShift = (lon == this->MapWidth - 1 ? 1 - this->MapWidth : 1);
      if (fullLon == false)
      {
        rightShift = (lon == this->MapWidth - 1 ? 0 : 1);
      }

      CTrace::Tracer("lat %d lon %d rightShift %d val_id %d",
              lat, lon, rightShift, val_id);

      numValidPoints = (pointIds[val_id] != -1) +
          (pointIds[val_id + rightShift] != -1) +
          (pointIds[val_id + this->MapWidth] != -1) +
          (pointIds[val_id + this->MapWidth + rightShift] != -1);


      CTrace::Tracer("numValidPoints%d\tbits[%d]=%d\tpointIds[%d]=%ld\tpointIds[%d]=%ld\tpointIds[%d]=%ld\tpointIds[%d]=%ld",
              numValidPoints,
              val_id, this->Bitarray->GetPointer(0)[val_id],
              val_id, (long int)pointIds[val_id],
              val_id + rightShift, (long int)pointIds[val_id + rightShift],
              val_id + this->MapWidth, (long int)pointIds[val_id + this->MapWidth],
              val_id + this->MapWidth + rightShift, (long int)pointIds[val_id + this->MapWidth + rightShift]
              );

      if ( (numValidPoints >= 3) && IsValidPoint(val_id))
        {
	      vtkIdType polyId = polys->InsertNextCell(numValidPoints);
	      colors->InsertTuple1(polyId, values[val_id]);
        CTrace::Tracer("---->InsertNexCell values[%d]=%f",
               val_id, values[val_id]);
        if (pointIds[val_id] != -1)
          {
	        polys->InsertCellPoint(pointIds[val_id]);
          CTrace::Tracer("\tpointIds[%d]=%ld",
                  val_id, (long int)pointIds[val_id]);
          }
        if (pointIds[val_id + rightShift] != -1)
          {
	        polys->InsertCellPoint(pointIds[val_id + rightShift]);
          CTrace::Tracer("\tpointIds[%d]=%ld",
                  val_id + rightShift, (long int)pointIds[val_id + rightShift]);
          }
        if (pointIds[val_id + this->MapWidth + rightShift] != -1)
          {
	        polys->InsertCellPoint(pointIds[val_id + this->MapWidth + rightShift]);
          CTrace::Tracer("\tpointIds[%d]=%ld",
                 val_id + this->MapWidth + rightShift, (long int)pointIds[val_id + this->MapWidth + rightShift]);
          }
        if (pointIds[val_id + this->MapWidth] != -1)
          {
	        polys->InsertCellPoint(pointIds[val_id + this->MapWidth]);
          CTrace::Tracer("\tpointIds[%d]=%ld",
                  val_id + this->MapWidth, (long int)pointIds[val_id + this->MapWidth]);
          }
        }
      val_id++;
      }
    }
   delete [] pointIds;
}

//----------------------------------------------------------------------------
void vtkGeoMapFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "OffsetLatitude : " << this->OffsetLatitude << endl;
  os << indent << "OffsetLongitude : " << this->OffsetLongitude << endl;
  os << indent << "MinMappedValue : " << this->MinMappedValue << endl;
  os << indent << "MaxMappedValue : " << this->MaxMappedValue << endl;
  os << indent << "Factor : " << this->Factor << endl;
  os << indent << "Radius : " << this->Radius << endl;
  os << indent << "MapWidth : " << this->MapWidth << endl;
  os << indent << "MapHeight : " << this->MapHeight << endl;
}
