/*
* Copyright (C) 2005 Collecte Localisation Satellites (CLS), France (31)
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

#ifndef __vtkPointLocatorBrat_h
#define __vtkPointLocatorBrat_h

#include "vtkPointLocator.h"

class vtkCellArray;
class vtkIdList;
class vtkNeighborPoints;
class vtkPoints;

class VTK_COMMON_EXPORT vtkPointLocatorBrat : public vtkPointLocator
{
public:
  // Description:
  // Construct with automatic computation of divisions, averaging
  // 25 points per bucket.
  static vtkPointLocatorBrat *New();

  void BuildLocator();

protected:
  vtkPointLocatorBrat();
  ~vtkPointLocatorBrat();

private:
  vtkPointLocatorBrat(const vtkPointLocatorBrat&);  // Not implemented.
  void operator=(const vtkPointLocatorBrat&);  // Not implemented.
};

#endif


