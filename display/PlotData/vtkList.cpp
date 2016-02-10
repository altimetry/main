/*
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

#include <cstdlib>
#include <cstdio>
#include <cstring> 

#include <string> 

#include "new-gui/Common/tools/TraceLog.h" 
#include "new-gui/Common/tools/Exception.h" 
#include "libbrathl/Tools.h"
#if defined (__DEPRECATED)          //avoid linux warning in vtk include
#undef __DEPRECATED
#endif
#if defined (__unix__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include "vtkList.h"

//-------------------------------------------------------------
//------------------- vtkObArray class --------------------
//-------------------------------------------------------------
//----------------------------------------

void vtkObArray::Dump(std::ostream& fOut /* = std::cerr */)
{

  if (CTrace::IsTrace() == false)
   {
      return;
   }

   vtkObArray::iterator it;
   int i = 0;

   fOut << "==> Dump a vtkObArray Object ast "<< this << " with " <<  size() << " elements" << std::endl;

   for ( it = this->begin( ); it != this->end( ); it++ )
   {
     fOut << "vtkObArray[" << i << "] at " << (*it) << std::endl;  
     //(*it)->Dump(fOut);
     i++;
   }

   fOut << "==> END Dump a vtkObArray Object at "<< this << " with " <<  size() << " elements" << std::endl;
    
 
   fOut << std::endl;
   

}

