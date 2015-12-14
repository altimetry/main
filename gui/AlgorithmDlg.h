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

#ifndef __AlgorithmDlg_H__
#define __AlgorithmDlg_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "BratAlgorithmBase.h"
using namespace brathl;

#include "BratGui_wdr.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CAlgorithmDlg
//----------------------------------------------------------------------------

class CAlgorithmDlg: public wxDialog
{
public:
  // constructors and destructors
  CAlgorithmDlg( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
  virtual ~CAlgorithmDlg();
  
  // WDR: method declarations for CAlgorithmDlg
    wxTextCtrl* GetAlgoOutputUnit()  { return (wxTextCtrl*) FindWindow( ID_ALGO_OUTPUT_UNIT ); }
  wxTextCtrl* GetAlgoDescr()  { return (wxTextCtrl*) FindWindow( ID_ALGO_DESCR ); }
  wxListBox* GetAlgoList()  { return (wxListBox*) FindWindow( ID_ALGO_LIST ); }
  wxGrid* GetAlgoInputParamGrid()  { return (wxGrid*) FindWindow( ID_ALGO_INPUT_PARAM_GRID ); }

public:
  CBratAlgorithmBase* m_algo;
    
private:
  // WDR: member variable declarations for CAlgorithmDlg
  wxSizer* m_item0;
  wxSize m_gridSize;
    
private:
    void FillAlgoList();
    void FillInputParametersGrid(CBratAlgorithmBase* algo);
  
  // WDR: handler declarations for CAlgorithmDlg
    void OnCancel( wxCommandEvent &event );
    void OnOk( wxCommandEvent &event );
    void OnAlgorithmList( wxCommandEvent &event );


private:
  DECLARE_CLASS(CAlgorithmDlg)
  DECLARE_EVENT_TABLE()
};




#endif
