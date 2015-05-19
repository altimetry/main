/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDataArrayPlotData.h
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
#ifndef __vtkDataArrayPlotData_h
#define __vtkDataArrayPlotData_h

#include "vtkDataArray.h"
#include "vtkPlotData.h"

class VTK_EXPORT vtkDataArrayPlotData : public vtkPlotData
{
public:
  vtkTypeMacro(vtkDataArrayPlotData, vtkPlotData);
  static vtkDataArrayPlotData *New() { return new vtkDataArrayPlotData; }

  void SetXDataArray(vtkDataArray *dataArray);
  vtkDataArray *GetXDataArray() { return this->XDataArray; }

  void SetYDataArray(vtkDataArray *dataArray);
  vtkDataArray *GetYDataArray() { return this->YDataArray; }

  void SetZDataArray(vtkDataArray *dataArray);
  vtkDataArray *GetZDataArray() { return this->ZDataArray; }

  virtual double GetXValue(int i);
  virtual double GetYValue(int i);
  virtual double GetZValue(int i);
  virtual int GetNumberOfItems();

  // Description:
  // Override GetMTime because we depend on vtkDataArray objects
  unsigned long GetMTime();

protected:
  vtkDataArrayPlotData();
  virtual ~vtkDataArrayPlotData();
 
  // Description:
  // Override Execute so we can update NumberOfItems;
  virtual void Execute();

  vtkDataArray *XDataArray;
  vtkDataArray *YDataArray;
  vtkDataArray *ZDataArray;

  int NumberOfItems;

private:
  vtkDataArrayPlotData(const vtkDataArrayPlotData&);  // Not implemented.
  void operator=(const vtkDataArrayPlotData&);  // Not implemented.
};


#endif
