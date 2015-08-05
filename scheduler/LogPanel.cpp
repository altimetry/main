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
    #pragma implementation "LogPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "LogPanel.h"

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CLogPanel
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CLogPanel,wxPanel)

// WDR: event table for CLogPanel

BEGIN_EVENT_TABLE(CLogPanel,wxPanel)
    EVT_WINDOW_DESTROY(CLogPanel::OnDestroy )
    EVT_TIMER(wxID_ANY, CLogPanel::OnTimer)
    EVT_BUTTON( ID_LOG_CLEAR, CLogPanel::OnClear )
    EVT_TEXT( ID_LOG_MESS, CLogPanel::OnLogText )
END_EVENT_TABLE()
//----------------------------------------
CLogPanel::CLogPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{

  m_numLines = 0;
  m_numLinesMax = 1000;

#ifdef WIN32
  m_lineEndingLength = 2;
#else
  m_lineEndingLength = 1;
#endif

  m_removeLinesTimer.SetOwner(this);

  wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

  LogInfoPanel(this, false, true);

  item0->Add( this, 0, wxGROW|wxALL, 5 );
  parent->SetSizer( item0 );

  m_removeLinesTimer.Start(1000);

  EnableCtrl();
}
//----------------------------------------
CLogPanel::~CLogPanel()
{
}
//----------------------------------------
void CLogPanel::EnableCtrl()
{
}

//----------------------------------------
void CLogPanel::PartialClear()
{  
  int32_t diff = m_numLines - m_numLinesMax;

  if (diff > 0)
  {
    uint64_t length = 0;
    for (int32_t i = 0 ; i < diff ; i++)
    {

      length += GetLogMess()->GetLineLength(i) + m_lineEndingLength;
    }
    m_numLines -= (diff + 1);
    GetLogMess()->Remove(0, length);
  }

}

//----------------------------------------
// WDR: handler implementations for CLogPanel

void CLogPanel::OnLogText( wxCommandEvent &event )
{
  //m_numLines = GetLogMess()->GetNumberOfLines();
  m_numLines++;
}
//----------------------------------------
void CLogPanel::OnClear( wxCommandEvent &event )
{
  GetLogMess()->Clear();    
  m_numLines = 0;
}
//----------------------------------------
void CLogPanel::OnTimer(wxTimerEvent& event)
{
  PartialClear();
}
//----------------------------------------
void CLogPanel::OnDestroy( wxWindowDestroyEvent &event )
{
}


