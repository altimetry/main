/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPlotDataCollection.cxx
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
#include "vtkPlotDataCollection.h"

#include "vtkPoints.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkPlotDataCollection* vtkPlotDataCollection::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkPlotDataCollection");
  if(ret)
    {
    return (vtkPlotDataCollection*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkPlotDataCollection;
}
