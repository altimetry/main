/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGSHHSReader.h
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
#ifndef __vtkGSHHSReader_h
#define __vtkGSHHSReader_h

#include <cstdio>

#include "brathl.h"
#include "vtkPolyDataSource.h"

class VTK_EXPORT vtkGSHHSReader : public vtkPolyDataSource
{
public:
  vtkTypeMacro(vtkGSHHSReader, vtkPolyDataSource);
  static vtkGSHHSReader *New() { return new vtkGSHHSReader; }
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify file name of GSHHS file.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  // Specify the maximum level of data to be read.
  // Level 1 data comprises the mayor coastlines. Higher level data are
  // rivers and lakes.
  vtkSetMacro(MaxLevel, int);
  vtkGetMacro(MaxLevel, int);

protected:
  vtkGSHHSReader();
  virtual ~vtkGSHHSReader();
  void Execute();

  char *FileName;
  int MaxLevel;

private:
  vtkGSHHSReader(const vtkGSHHSReader&);  // Not implemented.
  void operator=(const vtkGSHHSReader&);  // Not implemented.

  int readlong(FILE *f, int32_t &value);
  int readshort(FILE *f, int16_t &value);
};

#endif
