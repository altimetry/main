/*
* 
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
#if !defined(_BratObject_h_)
#define _BratObject_h_


#include "Stl.h"
/** \addtogroup tools Tools
  @{ */

/** 
  object base class 

 \version 1.0
*/
#include <typeinfo>
#include "Exception.h"

using namespace brathl;

namespace brathl
{

class CBratObject
{
public:

  ///Ctor
  CBratObject() {};
  ///Dtor
  virtual ~CBratObject() {};
  virtual CBratObject* Clone() 
  { 
    string className = typeid(*this).name();
    string msg = "ERROR - A 'Clone' method must be defined in class '";
    msg.append(className.c_str());
    msg.append("'");
    throw CException(msg, BRATHL_LOGIC_ERROR);
  };

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr) 
  {
    fOut << "==> Dump a CBratObject Object at "<< this << endl;
    fOut << "==> END Dump a CBratObject Object at "<< this << endl;
    fOut << endl;
  };

};

/** @} */

}

#endif // !defined(_Object_h_)

