
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

#include <cstdlib>
#include <cstdio>
#include <cstring> 
#include <string> 


#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include "new-gui/Common/tools/TraceLog.h" 
#include "Tools.h" 
#include "new-gui/Common/tools/Exception.h" 
#include "ProductAop.h" 

using namespace brathl;

namespace brathl
{



CProductAop::CProductAop()
{
  Init();
}


//----------------------------------------

CProductAop::CProductAop(const std::string& fileName)
      : CProduct(fileName)
{
  Init();
}


//----------------------------------------
CProductAop::CProductAop(const CStringList& fileNameList)
      : CProduct(fileNameList)

{
  Init();
}

//----------------------------------------

CProductAop::~CProductAop()
{

}

//----------------------------------------
void CProductAop::InitCriteriaInfo()
{
  CProduct::InitCriteriaInfo();
}
//----------------------------------------
void CProductAop::InitDateRef()
{
  m_refDate = REF19500101;
}

//----------------------------------------
void CProductAop::Init()
{
  m_label = "Altimeter Ocean Pathfinder product";

  InitDateRef();
  InitCriteriaInfo();

}
//----------------------------------------
void CProductAop::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CProductAop Object at "<< this << std::endl;

  //------------------
  CProduct::Dump(fOut);
  //------------------

  fOut << "==> END Dump a CProductAop Object at "<< this << std::endl;

  fOut << std::endl;

}


} // end namespace
