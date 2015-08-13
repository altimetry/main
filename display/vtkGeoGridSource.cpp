/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGeoGridSource.cxx
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
#include "vtkGeoGridSource.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkFloatArray.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"

double convert(double val, double period)
{
    double turns;

    // normalize
    val /= period;

    if ( val < 0.0 )
    {
        // calculate the amount of complete turns around the period
        turns = floor(val + 0.5);

        // get the fractional part
        val -= turns;

        val = 1.0 + val;
    }
    else
    {
        // calculate the amount of complete turns around the period
        //turns = round(val - 0.5);
        turns = ceil(val - 0.5);

        // get the fractional part
        val -= turns;
    }

    return val*period;
}

double min_angle(double val1, double val2, double period)
{
    double dif;

    val1 = convert(val1, period);
    val2 = convert(val2, period);

    dif = fabs(val2-val1);
    if ( dif < period/2 )
    {
        return dif;
    }
    else
    {
        return (period - dif);
    }
}

int between(double dlimit, double ulimit, double val, double period)
{

    dlimit = convert(dlimit, period);
    ulimit = convert(ulimit, period);
    val = convert(val, period);

    if ( ulimit <= dlimit )
    {
        return (val <= ulimit && val >= 0) || ( val >= dlimit && val <= period);
    }
    else
    {
        return (val >= dlimit && val <= ulimit);
    }
}

//----------------------------------------------------------------------------
vtkGeoGridSource::vtkGeoGridSource()
{
    this->LongGraticule = 30;
    this->LatGraticule = 30;
    this->PointDistance = 5;
    this->CreateParallelsForPoles = 1;
    this->LatitudeLimitMagnitude = 180.0;
    this->InitLat = -90.0;
    this->EndLat = 90.0;
    this->InitLong = -180.0;
    this->EndLong = 180.0;
    this->MidLat = 0.0;
    this->MidLong = 0.0;

    vtkCallbackCommand* callback = vtkCallbackCommand::New();
    callback->SetCallback(&vtkGeoGridSource::Test);
    this->AddObserver(vtkCommand::LeftButtonPressEvent, callback);
    callback->Delete();

}

//----------------------------------------------------------------------------
vtkGeoGridSource::~vtkGeoGridSource()
{
}

//----------------------------------------------------------------------------
void vtkGeoGridSource::Test(vtkObject* obj, unsigned long, void*, void*)
{
    //vtkGeoGridSource *source = dynamic_cast<vtkGeoGridSource*>(obj);
    vtkGeoGridSource *source = vtkGeoGridSource::SafeDownCast(obj);
    source->FindData(0,0);
}
//----------------------------------------------------------------------------
void vtkGeoGridSource::Execute()
{
    vtkPolyData *output;
    vtkPoints *points;
    vtkCellArray *cells;
    vtkFloatArray *color;
    vtkIdType cell;
    vtkIdType point;
    int numMeridians; // Line with constant longitude value
    int numParallels; // Line with constant latitude value
    int numPointsPerMeridian;
    int numPointsPerParallel;
    int numLines;
    int numPoints;
    double i, j;
    double cellId;
    int bmLat, bmLong;

    // this is for the viewing range
    //double InitLat, EndLat;
    //double InitLong, EndLong;

    output = (vtkPolyData *)this->GetOutput();
    points = vtkPoints::New();
    cells = vtkCellArray::New();
    color = vtkFloatArray::New();
    output->SetPoints(points);
    points->Delete();
    output->SetLines(cells);
    cells->Delete();
    output->GetCellData()->SetScalars(color);
    color->Delete();

    if (this->LatitudeLimitMagnitude != 180)
    {
        this->CreateParallelsForPoles = 0;
    }

    if ( InitLong == EndLong )
    {
        this->LongGraticule = 30;
    }

    if ( InitLat == EndLat )
    {
        this->LatGraticule = 30;
    }

    numMeridians = (int)(360.0 / this->LongGraticule);

    numParallels = (int)(180.0 / this->LatGraticule);
    numParallels++;


    bmLat = between(InitLat, EndLat, MidLat, 90);
    bmLong = between(InitLong, EndLong, MidLong, 360);

    // clear the gridLInesInfo
    gridLinesInfo.clear();

    numPointsPerMeridian =  (int)(this->LatitudeLimitMagnitude  / this->PointDistance) + 1;
    numPointsPerParallel =  (int)(360.0 / this->PointDistance) + 1;

    numLines = numMeridians + numParallels;
    numPoints = numMeridians * numPointsPerMeridian +
                numParallels * numPointsPerParallel;

    cells->Allocate(100);
    points->Allocate(3600);

    // Meridians (constant longitude)
    for (i = 0; i < numMeridians; i++)
        //for (i = -180; i <= 180; i+=this->Graticule)
    {
        double longitude;

        longitude = (i * this->LongGraticule) - 180;


        // is this longitude between Init/End as the middle point?
        // remember it's and angle, so the direction of "middle"
        // depends on the middle longitude (bmLong)
        if ( between(InitLong, EndLong, longitude, 360) != bmLong )
            continue;


        cell = cells->InsertNextCell(numPointsPerMeridian);

        // The scalar value in the tuple is associated with the color in the final plot
        // therefore, since we want to use this value as an ID to retrieve the lat/lon
        // information, we will all sorts of colors in the final plot.  If we use negative
        // values, then it will all be painted black :) and we still have unique ID's for
        // every grid line.  Not the most elegant solution...
        cellId = -cell;
        color->InsertTuple1(cell, cellId);

        GeoGridLineInfo cInfo = { longitude, (double)cell, GRIDLINE_LONG };
        gridLinesInfo[cellId] = cInfo;

        std::list<double> latitudes;
        for (j = 0; j < numPointsPerMeridian; j++)
        {
            double latitude;
            latitude = j * this->PointDistance - (this->LatitudeLimitMagnitude / 2);

            latitudes.push_back(latitude);
        }

        // now we add more additional points if necessary
        if ( this->LatGraticule < this->PointDistance )
        {
            for ( double lat = InitLat; lat < EndLat; lat += this->LatGraticule/2)
            {
                latitudes.push_back(lat);
            }
        }

        //we order the latitudes
        latitudes.sort();
        for ( std::list<double>::iterator it = latitudes.begin(); it != latitudes.end(); it++ )
        {
            //now we insert into the cell
            point = points->InsertNextPoint(longitude, *it, 0.0);
            cells->InsertCellPoint(point);
        }
        cells->UpdateCellCount(latitudes.size());
    }


    // Parallels (constant latitude)
    // first index is  this->CreateParallelsForPoles (1 -> create for South, 0 -> skip South
    int firstIndexParallels = ((this->CreateParallelsForPoles == 0) ? 1 : 0);

    //for (j = -90; j <= 90; j+=this->Graticule)
    for (j = firstIndexParallels; j < numParallels - firstIndexParallels; j++)
    {
        double latitude;

        latitude = j * this->LatGraticule - 90.0;

        // latitude is easier, we just see if it falls within
        if ( latitude < InitLat || latitude > EndLat )
            continue;

        cell = cells->InsertNextCell(numPointsPerParallel);

        // same reasoning as above
        cellId = -cell;
        color->InsertTuple1(cell, cellId);

        GeoGridLineInfo cInfo = { latitude, (double)cell, GRIDLINE_LAT };
        gridLinesInfo[cellId] = cInfo;

        std::list<double> longitudes;
        for (i = 0; i < numPointsPerParallel; i++)
        {
            double longitude;

            longitude = i * this->PointDistance - 180.0;

            longitudes.push_back(longitude);

        }

        // now we add more additional points if necessary
        if ( this->LongGraticule < this->PointDistance )
        {
            for ( double longi = InitLong; longi < EndLong; longi += this->LongGraticule/2)
            {

                longitudes.push_back(longi);
            }
        }

        longitudes.sort();
        for ( std::list<double>::iterator it = longitudes.begin(); it != longitudes.end(); it++ )
        {
            //now we insert into the cell
            point = points->InsertNextPoint(*it, latitude, 0.0);
            cells->InsertCellPoint(point);
        }
        cells->UpdateCellCount(longitudes.size());

    }
}

GeoGridLineInfo vtkGeoGridSource::GetGridLineInfo(double id)
{
    return gridLinesInfo[id];
}

//----------------------------------------------------------------------------
vtkPolyData* vtkGeoGridSource::FindData(double x,
                                        double y)
{
    double minDistance = VTK_LARGE_FLOAT;
    vtkPolyData* nearestPolyData = 0;
    vtkPolyData* output = (vtkPolyData *)this->GetOutput();
    double distance;
    vtkFloatingPointType* xyz;
    vtkIdType point;
    this->Update();
    point = output->FindPoint(x, y, 0);
    if (point>=0)
    {
        xyz = output->GetPoint(point);
        distance = (x-xyz[0])*(x-xyz[0]) + (y-xyz[1])*(y-xyz[1]);
        if (distance<minDistance)
        {
            minDistance = distance;
            nearestPolyData = output;
        }
    }
    return nearestPolyData;
}

//----------------------------------------------------------------------------
void vtkGeoGridSource::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);

    os << indent << "Longitude Graticule: " << this->LongGraticule << endl;
    os << indent << "Latitude Graticule: " << this->LatGraticule << endl;
    os << indent << "Point distance: " << this->PointDistance << endl;
    os << indent << "Create parallels for poles: "
    << this->CreateParallelsForPoles << endl;

    os << indent << "Outputxx: " << endl;
    vtkPolyData *output = (vtkPolyData *)this->GetOutput();
    output->Print(os);
    os << indent << "Pointsxx: " << endl;
    /*
    vtkPoints* points = output->GetPoints();
    points->Print(os);
    os << indent << "Datasxx: " << endl;
    vtkDataArray* dataArray = points->GetData();
    dataArray->Print(os);
    vtkIdType nOfT = dataArray->GetNumberOfTuples();
    int nOfC = dataArray->GetNumberOfComponents();

    for(int i = 0 ; i < nOfT ; i++)
    {
      os << "dataArray[" << i << "]:";
      double* d = dataArray->GetTuple(i);
      for (int j = 0 ; j < nOfC ; j++)
      {
        os << " " << d[j];
      }
      os << endl;
    }
    */
    vtkFloatingPointType* xyz;
    os << indent << "Pointxx: " << endl;
    for (int j = 0 ; j < output->GetNumberOfPoints() ; j++)
    {
        xyz = output->GetPoint(j);
        vtkIdType p = output->FindPoint(xyz);
        os << indent
        << " Point  " << j
        << " x: " << xyz[0]
        << " y: " << xyz[1]
        //<< " z: " << xyz[2]
        << " find point : " << p
        << endl;
    }


}


