/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDataArrayPlotData.cxx
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
#include "vtkDataArrayPlotData.h"


//----------------------------------------------------------------------------
vtkDataArrayPlotData::vtkDataArrayPlotData()
{
  this->XDataArray = NULL;
  this->YDataArray = NULL;
  this->ZDataArray = NULL;
  this->NumberOfItems = 0;
}

//----------------------------------------------------------------------------
vtkDataArrayPlotData::~vtkDataArrayPlotData()
{
  if (this->XDataArray != NULL)
    {
    this->XDataArray->UnRegister(this);
    }
  if (this->YDataArray != NULL)
    {
    this->YDataArray->UnRegister(this);
    }
  if (this->ZDataArray != NULL)
    {
    this->ZDataArray->UnRegister(this);
    }
}

//----------------------------------------------------------------------------
void vtkDataArrayPlotData::SetXDataArray(vtkDataArray *dataArray)
{
  if (this->XDataArray != dataArray)
    {
    if (dataArray != NULL)
      {
      dataArray->Register(this);
      }
    if (this->XDataArray != NULL)
      {
      this->XDataArray->UnRegister(this);
      }
    this->XDataArray = dataArray;
    this->Modified();
    }
}

//----------------------------------------------------------------------------
void vtkDataArrayPlotData::SetYDataArray(vtkDataArray *dataArray)
{
  if (this->YDataArray != dataArray)
    {
    if (dataArray != NULL)
      {
      dataArray->Register(this);
      }
    if (this->YDataArray != NULL)
      {
      this->YDataArray->UnRegister(this);
      }
    this->YDataArray = dataArray;
    this->Modified();
    }
}

//----------------------------------------------------------------------------
void vtkDataArrayPlotData::SetZDataArray(vtkDataArray *dataArray)
{
  if (this->ZDataArray != dataArray)
    {
    if (dataArray != NULL)
      {
      dataArray->Register(this);
      }
    if (this->ZDataArray != NULL)
      {
      this->ZDataArray->UnRegister(this);
      }
    this->ZDataArray = dataArray;
    this->Modified();
    }
}

//----------------------------------------------------------------------------
// NumberOfItems is set to the minimum number of tuples of all data arrays.
// Only data arrays that are not null pointers are checked. If both the X, Y
// and Z data arrays are null pointers then NumberOfItems is set to 0.
void vtkDataArrayPlotData::Execute()
{
  vtkIdType numberOfTuples;

  vtkDebugMacro(<<"Updating NumberOfItems");

  this->NumberOfItems = -1;

  if (this->XDataArray != NULL)
    {
    numberOfTuples = this->XDataArray->GetNumberOfTuples();
    if (this->NumberOfItems == -1)
      {
      this->NumberOfItems = numberOfTuples;
      }
    }
  if (this->YDataArray != NULL)
    {
    numberOfTuples = this->YDataArray->GetNumberOfTuples();
    if (this->NumberOfItems == -1 || this->NumberOfItems > numberOfTuples)
      {
      this->NumberOfItems = numberOfTuples;
      }
    }
  if (this->ZDataArray != NULL)
    {
    numberOfTuples = this->ZDataArray->GetNumberOfTuples();
    if (this->NumberOfItems == -1 || this->NumberOfItems > numberOfTuples)
      {
      this->NumberOfItems = numberOfTuples;
      }
    }
  if (this->NumberOfItems == -1)
    {
    this->NumberOfItems = 0;
    }

  this->vtkPlotData::Execute();
}

//----------------------------------------------------------------------------
double vtkDataArrayPlotData::GetXValue(int i)
{
  if (this->XDataArray != NULL)
    {
    return this->XDataArray->GetTuple1(i);
    }
  return 0.0;
}

//----------------------------------------------------------------------------
double vtkDataArrayPlotData::GetYValue(int i)
{
  if (this->YDataArray != NULL)
    {
    return this->YDataArray->GetTuple1(i);
    }
  return 0.0;
}

//----------------------------------------------------------------------------
double vtkDataArrayPlotData::GetZValue(int i)
{
  if (this->ZDataArray != NULL)
    {
    return this->ZDataArray->GetTuple1(i);
    }
  return 0.0;
}

//----------------------------------------------------------------------------
int vtkDataArrayPlotData::GetNumberOfItems()
{
  return this->NumberOfItems;
}

//----------------------------------------------------------------------------
unsigned long vtkDataArrayPlotData::GetMTime()
{
  unsigned long mTime = this->vtkPlotData::GetMTime();
  unsigned long time;

  vtkDebugMacro(<<"Returning modification time");

  if (this->XDataArray != NULL)
    {
    time = this->XDataArray->GetMTime();
    mTime = time > mTime ? time : mTime;
    }
  if (this->YDataArray != NULL)
    {
    time = this->YDataArray->GetMTime();
    mTime = time > mTime ? time : mTime;
    }
  if (this->ZDataArray != NULL)
    {
    time = this->ZDataArray->GetMTime();
    mTime = time > mTime ? time : mTime;
    }

  return mTime;
}
