/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGSHHSReader.cxx
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
#include <cassert>

#include "brathl_config.h"
#include "../libbrathl/Tools.h"

#include "vtkGSHHSReader.h"


#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkFloatArray.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"

using namespace brathl;

int vtkGSHHSReader::readlong(FILE *f, int32_t &value)
{
  assert(sizeof(value) == 4);

  if (fread(&value, 4, 1, f) != 1)
  {
    if (feof(f))
    {
      fclose(f);
      return 1;
    }
    vtkErrorMacro(<<"Could not read long value from GSHHS file " << FileName);
    fclose(f);
    return -1;
  }
  
#ifndef WORDS_BIGENDIAN
  CTools::SwapValue(value);
#endif

  return 0;
}
#define READLONG(X)	if (readlong(f, (X)) != 0) return;

int vtkGSHHSReader::readshort(FILE *f, int16_t &value)
{
  assert(sizeof(value) == 2);

  if (fread(&value, 2, 1, f) != 1)
  {
    if (feof(f))
    {
      fclose(f);
      return 1;
    }
    vtkErrorMacro(<<"Could not read short value from GSHHS file " << FileName);
    fclose(f);
    return -1;
  }
  
#ifndef WORDS_BIGENDIAN
  CTools::SwapValue(value);
#endif

  return 0;
}
#define READSHORT(X)	if (readshort(f, (X)) != 0) return;


//----------------------------------------------------------------------------
vtkGSHHSReader::vtkGSHHSReader()
{
  this->FileName = NULL;
  this->MaxLevel = VTK_INT_MAX;
}

//----------------------------------------------------------------------------
vtkGSHHSReader::~vtkGSHHSReader()
{
  if (this->FileName != NULL)
  {
    delete [] this->FileName;
  }
}

//----------------------------------------------------------------------------
void vtkGSHHSReader::Execute()
{

  vtkPolyData *output;
  vtkPoints *points;
  vtkCellArray *lines;
  vtkFloatArray *color;
  FILE *f;
  int32_t id;

  if (this->FileName == NULL)
  {
    vtkErrorMacro(<<"A File Name must be specified.");
    return;
  }

  f = fopen(this->FileName, "rb");
  if (f == NULL)
  {
    vtkErrorMacro(<<"Could not open GSHHS file " << FileName);
    return;
  }

  vtkDebugMacro("Reading gshhs file " << FileName);

  output = (vtkPolyData *)this->GetOutput();
  points = vtkPoints::New();
  lines = vtkCellArray::New();
  color = vtkFloatArray::New();
  output->SetPoints(points);
  points->Delete();
  output->SetLines(lines);
  //output->SetPolys(lines);
  lines->Delete();
  output->GetCellData()->SetScalars(color);
  color->Delete();

  // read id of first polygon
  READLONG(id);

  // read until we encounter end-of-file or until we encounter an error
  for (;;)
  {
    double firstLatitude, firstLongitude;
    double lastLatitude, lastLongitude;
    int32_t numPoints;
    int32_t level;
    int32_t extent[4];
    int32_t area;
    int32_t version;
    int16_t greenwich;
    int16_t source;
    int32_t i;
    int32_t   lonint;
    int32_t   latint;

    // read number of points
    READLONG(numPoints);

    // read level
    READLONG(level);

    vtkDebugMacro(" Polygon #" << id << ", " << numPoints << " points of level " << level);

    // read extent
    READLONG(extent[0]);
    READLONG(extent[1]);
    READLONG(extent[2]);
    READLONG(extent[3]);
    // read area
    READLONG(area);
    // Version of file
    READLONG(version);
    // read greenwich
    READSHORT(greenwich);
    // read source
    READSHORT(source);



    if ((level > this->MaxLevel) || (numPoints <= 1))
    {
      fseek(f, numPoints*sizeof(latint)*2, SEEK_CUR);
      vtkDebugMacro(<< "polygon skipped");
    }
    else
    {
      vtkIdType cell;

      cell = lines->InsertNextCell(numPoints);
      color->InsertTuple1(cell, 0.0);
      firstLatitude = 0.0;	// Not necessary but avoids compiler complaint
      firstLongitude = 0.0;
      lastLatitude = 0.0;
      lastLongitude = 0.0;

      // read points
      for (i = 0; i < numPoints; ++i)
      {
	// read point
	READLONG(lonint);
	READLONG(latint);

        vtkIdType point;

        lastLongitude = lonint / 1000000.0;
        if (lastLongitude > 180)
        {
          lastLongitude -= 360;
        }
        lastLatitude = latint / 1000000.0;

        point = points->InsertNextPoint(lastLongitude, lastLatitude, 0.0);
        lines->InsertCellPoint(point);

        if (i == 0)
        {
          firstLongitude = lastLongitude;
          firstLatitude = lastLatitude;
        }
      }

      vtkDebugMacro(<< numPoints << " points read");
      if (numPoints > 1)
      {
        if (firstLongitude != lastLongitude || firstLatitude != lastLatitude)
        {
          vtkIdType point;

          point = points->InsertNextPoint(firstLongitude, firstLatitude, 0.0);
          lines->InsertCellPoint(point);
          lines->UpdateCellCount(numPoints + 1);
          vtkDebugMacro("extra endpoint added");
        }
      }
    }

    // read id of next polygon
    READLONG(id);
  }
}

//----------------------------------------------------------------------------
void vtkGSHHSReader::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "File Name: "
     << (this->FileName ? this->FileName : "(none)") << std::endl;
  os << indent << "Maximum Level: " << this->MaxLevel << std::endl;
}
