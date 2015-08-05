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
    #pragma implementation "PlotField.h"
#endif

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Trace.h"
#include "Tools.h"
#include "Exception.h"
using namespace brathl;


//#include "BratDisplay.h"
#include "PlotField.h"

//-------------------------------------------------------------
//------------------- CPlotField class --------------------
//-------------------------------------------------------------

CPlotField::CPlotField(const wxString& name)
    : m_internalFiles(false)
{
  Init();
  m_name = name;
}
//----------------------------------------
CPlotField::~CPlotField()
{
}

//----------------------------------------
void CPlotField::Init()
{
  m_xyProps = NULL;
  m_worldProps = NULL;
  m_zfxyProps = NULL;
}

//----------------------------------------
CPlotField* CPlotField::GetPlotField(CBratObject* ob)
{
  CPlotField* field = dynamic_cast<CPlotField*>(ob);
  if (field == NULL)
  {
    CException e("CPlotField::CPlotField - dynamic_cast<CPlotField*>(ob) returns NULL"
                 "object seems not to be an instance of CPlotField",
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);

  }

  return field;

}
//----------------------------------------
CInternalFiles* CPlotField::GetInternalFiles(int32_t index)
{
  int32_t nCount = m_internalFiles.size();

  if ( (index < 0) || (index >= nCount))
  {
    string msg = CTools::Format("CPlotField::GetInternalFile - invalid index %d"
                                "- Valid range is [0, %d]",
                                index, nCount);
    CException e(msg,
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  CInternalFiles* f = dynamic_cast<CInternalFiles*>(m_internalFiles[index]);
  if (f == NULL)
  {
    CException e(CTools::Format("CPlotField::GetInternalFile - dynamic_cast<CInternalFiles*>(m_internalFiles[%d]) returns NULL"
                                "- Valid range is [0, %d]"
                                "- object seems not to be an instance of CInternalFiles",
                                index, nCount),
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);

  }

  return f;
}
//----------------------------------------
CInternalFilesYFX* CPlotField::GetInternalFilesYFX(int32_t index)
{
  int32_t nCount = m_internalFiles.size();

  CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>(GetInternalFiles(index));
  if (yfx == NULL)
  {
    CException e(CTools::Format("CPlotField::GetInternalFilesYFX - dynamic_cast<CInternalFilesYFX*>(m_internalFiles[%d]) returns NULL"
                                "- Valid range is [0, %d]"
                                "- object seems not to be an instance of CInternalFilesYFX",
                                index, nCount),
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);

  }

  return yfx;
}
