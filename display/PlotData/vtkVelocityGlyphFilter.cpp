/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVelocityGlyphFilter.cxx
  Language:  C++
  Date:      -
  Version:   S&T

    *** THIS FILE WAS CREATED BY SCIENCE & TECHNOLOGY BV ***

  Copyright (C) 2002-2010 Science & Technology BV, The Netherlands.

  This file is part of BRAT.

  BRAT is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  BRAT is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with BRAT; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

=========================================================================*/

#include <algorithm>

#include "../libbrathl/Tools.h"
#include "../libbrathl/Trace.h"
#include "../libbrathl/LatLonPoint.h"
#include "../libbrathl/Exception.h"
using namespace brathl;

#include "vtkProj2DFilter.h"
#include "vtkVelocityGlyphFilter.h"
#include "vtkGeoGridSource.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkShortArray.h"
#include "vtkArrowSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

#include "math.h"

#if defined min
#undef min
#endif

#if defined max
#undef max
#endif

vtkStandardNewMacro(vtkVelocityGlyphFilter);

vtkCxxSetObjectMacro(vtkVelocityGlyphFilter,ValuesNorth,vtkDoubleArray);
vtkCxxSetObjectMacro(vtkVelocityGlyphFilter,ValuesEast,vtkDoubleArray);

vtkVelocityGlyphFilter::vtkVelocityGlyphFilter() : vtkGeoMapFilter()
{
    this->ValuesNorth = NULL;
    this->ValuesEast = NULL;

    this->MinVE = 0;
    this->MaxVE = 1000;  // this can be any value
    this->MinVN = 0;
    this->MaxVN = 1000;  // this can be any value

    this->MaxV = 1;  // this can be any value

    this->InitLat = -90;
    this->EndLat = 90;
    this->InitLon = -180;
    this->EndLon = 180;
    this->MidLat = 0;
    this->MidLon = 0;

    IsGlyph = false;

    GroupRatio = 5;

    this->OffsetLatitude = 10;
    this->OffsetLongitude = 10;
}

vtkVelocityGlyphFilter::~vtkVelocityGlyphFilter()
{
    if (this->ValuesNorth != NULL)
    {
        this->ValuesNorth->Delete();
    }

    if (this->ValuesEast != NULL)
    {
        this->ValuesEast->Delete();
    }
}

void vtkVelocityGlyphFilter::Execute()
{
    vtkPolyData *output;
    vtkPoints *points;
    vtkCellArray *lines;
    vtkFloatArray *colors;
    std::vector<vtkIdType> ptIds;
    vtkDoubleArray *vectors;

    vtkIdType ptId;
    double *valuesNorth = NULL, *valuesEast = NULL;
    double *heights = NULL;
    double *latitudes = NULL;
    double *longitudes = NULL;
    double interpolatedHeight;
    int numValidPoints;
    int lat;
    int lon;
    int val_id;
    int nIds = 0;
    int useHeights;

    int bmLat, bmLon;

    useHeights = (this->Heights != NULL && this->Factor > 0 && this->MaxMappedValue > this->MinMappedValue);

    if (this->ValuesNorth == NULL || this->ValuesEast == NULL || this->Bitarray == NULL ||
            this->Latitudes == NULL || this->Longitudes == NULL || this->ValidMercatorLatitudes == NULL)
    {
        vtkErrorMacro(<< "One or more data arrays are empty");
        return;
    }
    if (this->ValuesNorth->GetNumberOfTuples() != this->MapHeight * this->MapWidth
            || this->ValuesNorth->GetNumberOfTuples() != this->ValuesEast->GetNumberOfTuples())
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

    valuesNorth = this->ValuesNorth->GetPointer(0);
    valuesEast = this->ValuesEast->GetPointer(0);


    if (useHeights)
    {
        heights = this->Heights->GetPointer(0);
    }

    latitudes = this->Latitudes->GetPointer(0);
    longitudes = this->Longitudes->GetPointer(0);

    if (valuesNorth == NULL || valuesEast == NULL || this->Bitarray->GetPointer(0) == NULL || (useHeights && heights == NULL) ||
            latitudes == NULL || longitudes == NULL)
    {
        vtkErrorMacro(<< "One or more data array pointers are empty");
        return;
    }

    output = (vtkPolyData *)this->GetOutput();
    // clean up stuff
    // to free memory
    output->Initialize();

    points = vtkPoints::New(VTK_DOUBLE);
    lines = vtkCellArray::New();
    colors = vtkFloatArray::New();
    output->SetPoints(points);
    points->Delete();
    output->SetLines(lines);
    lines->Delete();
    output->GetCellData()->SetScalars(colors);
    colors->Delete();

    // std::vector stuff
    vectors = vtkDoubleArray::New();
    vectors->SetNumberOfComponents(3);
    output->GetPointData()->SetVectors(vectors);
    vectors->Delete();

    nIds = (this->MapHeight + 1) * this->MapWidth;

    bmLat = between(InitLat, EndLat, MidLat, 90);
    bmLon = between(InitLon, EndLon, MidLon, 360);

    // Determine Southpole value
    numValidPoints = 0;
    interpolatedHeight = 0.0;

    for (lon = 0; lon < this->MapWidth; lon++)
    {
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
            interpolatedHeight = this->Radius + this->Factor * (std::min(std::max(interpolatedHeight / numValidPoints, this->MinMappedValue), 
					this->MaxMappedValue) - this->MinMappedValue) / (this->MaxMappedValue - this->MinMappedValue);
        }
        else
        {
            interpolatedHeight = this->Radius;
        }


        for (lon = 0; lon < this->MapWidth; lon++)
        {
            int lonGroup;
            int pointCount =0;
            double tmpEast = 0;
            double tmpNorth = 0;
            double midLon =0;

            for ( lonGroup = 0; (lon + lonGroup) < this->MapWidth && lonGroup <= GroupRatio; lonGroup++ )
            {
                // bounding
                if ( between(InitLon, EndLon, longitudes[lon+lonGroup], 360) != bmLon )
                        continue;

                midLon += longitudes[lon + lonGroup];

                tmpEast += valuesEast[lon + lonGroup];
                tmpNorth += valuesNorth[0];

                pointCount++;
            }

            lon += (lonGroup -1);

            if ( pointCount > 0 )
            {
                midLon /= pointCount;
                tmpEast /= pointCount;
                tmpNorth /= pointCount;

                ptId = points->InsertNextPoint(midLon,
                                               latitudes[0],
                                               interpolatedHeight);
                ptIds.push_back(ptId);

                vectors->InsertTuple3(ptId, - this->OffsetLongitude* (tmpEast/MaxV), 0, interpolatedHeight );

                CTrace::Tracer("Southpole\tInsert pointIds=%ld at longitudes=%f, latitudes=%f",
                               (long int)ptId, midLon, latitudes[0]);
            }
        }
    }

     CTrace::Tracer("%ld southpole points l\n", (long int)output->GetPoints()->GetNumberOfPoints());


    int nMaxHeight = this->MapHeight;
    bool fullLat = false;
    if ( CTools::AreEqual(latitudes[0],-90.0) && CTools::AreEqual(latitudes[this->MapHeight - 1], 90.0) )
    {
        nMaxHeight = this->MapHeight - 1;
        fullLat = true;
    }


    //Determine Intermediate values
    val_id =0;
    for (lat = 0; lat < this->MapHeight - 1; lat++)
    {
        int groupLat =0;
        for (lon = 0; lon < this->MapWidth; lon++)
        {
            double midlat =0;
            double midlon =0;
            int pointCount = 0;
            int groupLon =0;
            double tmpEast =0, tmpNorth =0;

            // grouping
            for (groupLat =0; (lat + groupLat) < this->MapHeight - 1 && groupLat <= GroupRatio; groupLat++)
            {

                for (groupLon = 0; (lon + groupLon) < this->MapWidth && groupLon <= GroupRatio; groupLon++)
                {
                    // bounding... for efficiency and avoid rendering overhead

                    // this probably should go outside this inner loop..
                    if ( latitudes[lat + groupLat] < InitLat
                                    || latitudes[lat + groupLat] > EndLat  )
                        continue;

                    if ( between(InitLon, EndLon, longitudes[lon+groupLon], 360) != bmLon )
                        continue;


                    val_id = (lat + groupLat) * this->MapWidth + (lon + groupLon);
                    interpolatedHeight =0;
                    if (IsValidPoint(val_id))
                    {
                        numValidPoints++;
                        if (useHeights)
                        {
                            interpolatedHeight += heights[val_id];
                        }


                        if (useHeights)
                        {
                            interpolatedHeight = this->Radius + this->Factor * (std::min(std::max(interpolatedHeight / numValidPoints, this->MinMappedValue), this->MaxMappedValue) - this->MinMappedValue) / (this->MaxMappedValue - this->MinMappedValue);
                        }
                        else
                        {
                            interpolatedHeight = this->Radius;
                        }

                        midlat += latitudes[lat + groupLat];
                        midlon += longitudes[lon + groupLon];

                        tmpEast += valuesEast[val_id];
                        tmpNorth += valuesNorth[val_id];

                        pointCount++;
                    }
                }
            }

            if ( pointCount > 0 )
            {
                //mid point
                midlat /= pointCount;
                midlon /= pointCount;

                tmpEast /= pointCount;
                tmpNorth /= pointCount;

                ptId = points->InsertNextPoint(midlon, midlat, interpolatedHeight);
                ptIds.push_back(ptId);


                vectors->InsertTuple3(ptId, this->OffsetLongitude*(tmpEast/MaxV),
                                      this->OffsetLatitude*(tmpNorth/MaxV), interpolatedHeight );

                CTrace::Tracer("Intermediate\tInsert pointIds=%ld at longitudes=%f, latitudes=%f",
                               (long int)ptId,midlon, midlat);
            }

            // calculate roper lat/lon based on the grouping
            lon += (groupLon-1);
        }
        lat += (groupLat -1);
    }
    CTrace::Tracer("%ld - %ld intermediate points l\n", (long int)output->GetPoints()->GetNumberOfPoints(), (long int)ptIds.size());

    // Determine Northpole value

    val_id = (this->MapHeight - 1) * this->MapWidth;
    numValidPoints = 0;
    interpolatedHeight = 0.0;

    for (lon = 0; lon < this->MapWidth; lon++)
    {
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
            interpolatedHeight = this->Radius + this->Factor * (std::min(std::max(interpolatedHeight / numValidPoints, this->MinMappedValue), this->MaxMappedValue) - this->MinMappedValue) / (this->MaxMappedValue - this->MinMappedValue);
        }
        else
        {
            interpolatedHeight = this->Radius;
        }

        for (lon = 0; lon < this->MapWidth; lon++)
        {
            int lonGroup;
            int pointCount =0;
            double midLon = 0;
            double tmpEast =0, tmpNorth =0;

            for ( lonGroup = 0; (lon + lonGroup) < this->MapWidth && lonGroup <= GroupRatio; lonGroup++ )
            {
                // bounding
                if ( between(InitLon, EndLon, longitudes[lon+lonGroup], 360) != bmLon )
                        continue;

                midLon += longitudes[lon];

                tmpEast += valuesEast[lon];
                tmpNorth += valuesNorth[0];

                pointCount++;
            }

            lon += (lonGroup -1);

            if ( pointCount > 0 )
            {
                midLon /= pointCount;
                tmpEast /= pointCount;
                tmpNorth /= pointCount;

                ptId = points->InsertNextPoint(midLon,
                                               latitudes[this->MapHeight-1],
                                               interpolatedHeight);
                ptIds.push_back(ptId);

                vectors->InsertTuple3(ptId, - this->OffsetLongitude* (tmpEast/MaxV),
                                      0, interpolatedHeight);

                CTrace::Tracer("Northpole\tInsert pointIds=%ld at longitudes=%f, latitudes=%f",
                               (long int)ptId, midLon, latitudes[this->MapHeight-1]);
            }
        }
    }


    // Create the lines
    CTrace::Tracer("%ld total points l\n", (long int)output->GetPoints()->GetNumberOfPoints());

    if ( !IsGlyph )
    {
        for ( std::vector<vtkIdType>::const_iterator it = ptIds.begin();  it != ptIds.end(); it++)
        {
            double pt[3];
            double *tuple;
            double magnitude =0;
            vtkIdType nptId = *it;
            points->GetPoint(nptId, pt);

            //insert the next point for the line
            tuple = vectors->GetTuple3(nptId);

            // vectors only in 2D.. the last one is height
            ptId = points->InsertNextPoint(pt[0] + tuple[0],
                                           pt[1] + tuple[1], pt[2]);

            // otherwise vtkProj2DFilter crashes, because it does not
            // know which points have vectors and which don't!
            vectors->InsertTuple3(ptId, 0, 0, 0 );

            CTrace::Tracer("lat %f lon %f val_id %d",
                           pt[0], pt[1], val_id);


            vtkIdType polyId = lines->InsertNextCell(2);
            magnitude = sqrt(tuple[0]*tuple[0] + tuple[1]*tuple[1]);
            colors->InsertTuple1(polyId, magnitude);

            CTrace::Tracer("---->InsertNexCell values[%d]=%f", val_id, magnitude);

            lines->InsertCellPoint(nptId);
            CTrace::Tracer("\tpointIds=%d",val_id);

            lines->InsertCellPoint(ptId);
            CTrace::Tracer("\tpointIds=%ld",(long int)ptId);
        }
    }
}

