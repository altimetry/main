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
#include "new-gui/brat/stdafx.h"
#include "display/wxInterface.h"
#include "new-gui/brat/stdafx.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif


#include "wx/generic/gridctrl.h"

#include "BratGui.h"
#include "AlgorithmDlg.h"

// WDR: class implementations

const int32_t NB_COLS = 4;
const int32_t NUM_COL_NAME = 0;
const int32_t NUM_COL_DESCR = 1;
const int32_t NUM_COL_FORMAT = 2;
const int32_t NUM_COL_UNIT = 3;
//----------------------------------------------------------------------------
// CAlgorithmDlg
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CAlgorithmDlg,wxDialog)

// WDR: event table for CAlgorithmDlg

BEGIN_EVENT_TABLE(CAlgorithmDlg,wxDialog)
    EVT_LISTBOX( ID_ALGO_LIST, CAlgorithmDlg::OnAlgorithmList )
    EVT_BUTTON( wxID_OK, CAlgorithmDlg::OnOk )
    EVT_BUTTON( wxID_CANCEL, CAlgorithmDlg::OnCancel )
END_EVENT_TABLE()

CAlgorithmDlg::CAlgorithmDlg( wxWindow *parent, wxWindowID id, const wxString &title,
                              const wxPoint &position, const wxSize& size, long style ) :
      wxDialog( parent, id, title, position, size, style )
{


  // WDR: dialog function AlgorithmDlg for CAlgorithmDlg
  //--------------------------------
   m_item0 = AlgorithmDlg( this, TRUE ); 
  //--------------------------------


  int32_t charwidth, charheight;
  DetermineCharSize(this, charwidth, charheight);

  GetAlgoInputParamGrid()->CreateGrid(0, NB_COLS );
  //GetAlgoInputParamGrid()->AutoSizeColumns();
  GetAlgoInputParamGrid()->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_TOP);
  GetAlgoInputParamGrid()->SetRowLabelSize( 2 * charwidth);
  GetAlgoInputParamGrid()->EnableEditing(false);

  GetAlgoInputParamGrid()->SetColLabelValue(NUM_COL_NAME, "Name");

  //GetAlgoInputParamGrid()->SetColLabelValue(0, "Range");
  GetAlgoInputParamGrid()->SetColLabelValue(NUM_COL_DESCR, "Description");
  //GetAlgoInputParamGrid()->SetColSize(NUM_COL_DESCR, 33 * charwidth);

  GetAlgoInputParamGrid()->SetColLabelValue(NUM_COL_FORMAT, "Format");
  GetAlgoInputParamGrid()->SetColLabelValue(NUM_COL_UNIT, "Unit");

  FillAlgoList();

  GetAlgoInputParamGrid()->SetMinSize(GetAlgoInputParamGrid()->GetBestSize());
  GetAlgoInputParamGrid()->Fit();
  
  this->Layout();
  
  wxSize s = glbAlgoInputParamSizerH->GetSize();
  m_gridSize.SetWidth(s.GetWidth());

  s = glbAlgoInputParamSizerV->GetSize();
  m_gridSize.SetHeight(s.GetHeight());

  m_algo = NULL;
}

//----------------------------------------
CAlgorithmDlg::~CAlgorithmDlg()
{
}

//----------------------------------------
void CAlgorithmDlg::FillAlgoList()
{
  
  CMapBratAlgorithm* algorithms = CBratAlgorithmBaseRegistry::GetAlgorithms();
  if (algorithms == NULL)
  {
    return;
  }

  CMapBratAlgorithm::iterator it;

  for (it = algorithms->begin() ; it != algorithms->end() ; it++)
  {
    CBratAlgorithmBase* ob = it->second;
    if (ob != NULL)
    {
      GetAlgoList()->Append(ob->GetName().c_str());
    }
  }    

}


//----------------------------------------
void CAlgorithmDlg::FillInputParametersGrid(CBratAlgorithmBase* algo)
{
  if (algo == NULL)
  {
    return;
  }

  int32_t charwidth, charheight;
  DetermineCharSize(this, charwidth, charheight);

  //GetAlgoInputParamGrid()->BeginBatch();

  uint32_t nParam = algo->GetNumInputParam();

  if (GetAlgoInputParamGrid()->GetNumberRows() > 0)
  {
    GetAlgoInputParamGrid()->DeleteRows(0, GetAlgoInputParamGrid()->GetNumberRows());
  }

  GetAlgoInputParamGrid()->AppendRows(nParam);
  
  for (uint32_t i = 0 ; i < nParam ; i++)
  {
    //GetAlgoInputParamGrid()->SetCellValue(i, 0, CTools::IntToStr(i).c_str());
    GetAlgoInputParamGrid()->SetCellValue(i, NUM_COL_NAME, algo->GetParamName(i).c_str());
    
    GetAlgoInputParamGrid()->SetCellValue(i, NUM_COL_DESCR, algo->GetInputParamDescWithDefValueLabel(i).c_str());
    GetAlgoInputParamGrid()->SetCellRenderer(i , NUM_COL_DESCR, new wxGridCellAutoWrapStringRenderer);

    GetAlgoInputParamGrid()->SetCellValue(i, NUM_COL_FORMAT, algo->GetInputParamFormatAsString(i).c_str());
    
    GetAlgoInputParamGrid()->SetCellValue(i, NUM_COL_UNIT, algo->GetInputParamUnit(i).c_str());
    GetAlgoInputParamGrid()->SetCellRenderer(i , NUM_COL_UNIT, new wxGridCellAutoWrapStringRenderer);
  }

  GetAlgoInputParamGrid()->Fit();
  

  //GetAlgoInputParamGrid()->Refresh();
  //GetAlgoInputParamGrid()->ForceRefresh();

  //GetAlgoInputParamGrid()->EndBatch();

  GetAlgoInputParamGrid()->SetMinSize(m_gridSize);

  glbAlgoInputParamSizerV->Layout();
  glbAlgoInputParamSizerH->Layout();
  m_item0->Layout();
  m_item0->SetSizeHints( this );  
}
//----------------------------------------
// WDR: handler implementations for CAlgorithmDlg
//----------------------------------------

void CAlgorithmDlg::OnCancel( wxCommandEvent &event )
{
  event.Skip();    
}

//----------------------------------------
void CAlgorithmDlg::OnOk( wxCommandEvent &event )
{
  event.Skip();        
}
//----------------------------------------

void CAlgorithmDlg::OnAlgorithmList( wxCommandEvent &event )
{
  int32_t sel = event.GetInt();

  GetAlgoDescr()->Clear();
  GetAlgoOutputUnit()->Clear();
  
  GetAlgoInputParamGrid()->ClearGrid();

  if (sel < 0)
  {
    return;
  }

  wxString name = GetAlgoList()->GetString(static_cast<uint32_t>(sel));

  m_algo = CBratAlgorithmBaseRegistry::Find(name.ToStdString());

  FillInputParametersGrid(m_algo);
  
  if (m_algo != NULL)
  {
    GetAlgoDescr()->SetValue(m_algo->GetDescription().c_str());
    GetAlgoOutputUnit()->SetValue(m_algo->GetOutputUnit().c_str());
  }
    
}




