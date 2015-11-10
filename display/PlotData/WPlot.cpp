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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "WPlot.h"
#endif

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "../libbrathl/Trace.h"
#include "../libbrathl/Tools.h"
#include "../libbrathl/Exception.h"
#include "../libbrathl/InternalFiles.h"
#include "../libbrathl/InternalFilesZFXY.h"
using namespace brathl;

#include "PlotField.h"
#include "WPlot.h"


//-------------------------------------------------------------
//------------------- CWPlot class --------------------
//-------------------------------------------------------------
CWPlot::CWPlot(uint32_t groupNumber)
      : CPlotBase(groupNumber)

{
  Init();

}


//----------------------------------------
CWPlot::~CWPlot()
{

}
//----------------------------------------
void CWPlot::Init()
{
}
//----------------------------------------
void CWPlot::GetInfo()
{

  CPlotField* field = CPlotField::GetPlotField(*(m_fields.begin()));

  if (field == NULL)
  {
    return;
  }

  CWorldPlotProperty* worldProps = field->m_worldProps;

  if (worldProps != NULL)
  {
	  //femmTODO: uncomment next line
    //m_title = worldProps->m_title;
  }

  if (m_title.empty())
  {
    if (field->m_internalFiles.size() > 0)
    {
      CInternalFilesZFXY* zfxy = CWPlot::GetInternalFilesZFXY(*(field->m_internalFiles.begin()));
      m_title = zfxy->GetTitle("").c_str();
    }
  }

}
//----------------------------------------
CInternalFiles* CWPlot::GetInternalFiles(CBratObject* ob, bool withExcept /* = true */)
{
  return CWPlot::GetInternalFilesZFXY(ob);
}
//----------------------------------------
CInternalFilesZFXY* CWPlot::GetInternalFilesZFXY(CBratObject* ob, bool withExcept /* = true */)
{
  CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>(ob);
  if (zfxy == NULL)
  {
    if (withExcept)
    {
      CException e("CWPlot::GetInternalFilesZFXY - Non-ZFXY data found - dynamic_cast<CInternalFilesZFXY*>(ob) returns NULL",
                   BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

  }
  if (zfxy->IsGeographic() == false)
  {
    if (withExcept)
    {
      CException e("CWPlot::GetInternalFilesZFXY  - a ZFXY data seems to be NOT geographical  - zfxy->IsGeographic() is true",
                   BRATHL_LOGIC_ERROR);
      CTrace::Tracer("%s", e.what());
      throw e;
    }
  }

  return zfxy;

}




