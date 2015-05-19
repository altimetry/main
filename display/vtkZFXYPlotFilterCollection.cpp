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

#include "vtkZFXYPlotFilterCollection.h"

#include "vtkPoints.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkZFXYPlotFilterCollection* vtkZFXYPlotFilterCollection::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkZFXYPlotFilterCollection");
  if(ret)
    {
    return (vtkZFXYPlotFilterCollection*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkZFXYPlotFilterCollection;
}
