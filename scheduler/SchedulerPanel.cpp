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

// Temporary hack to prevent the inclusion of Windows XML 
// headers, which collide with xerces
//
#if defined (WIN32) || defined (_WIN32)
//avoid later inclusion of Microsoft XML stuff, which causes name collisions with xerces
#define DOMDocument MsDOMDocument
#include <msxml.h>
#include <urlmon.h>
#undef DOMDocument
#endif

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "SchedulerPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "BratSchedulerApp.h"
#include "SchedulerPanel.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"


const std::string LOG_PAGE_NAME = "Logs";


// WDR: class implementations

//----------------------------------------------------------------------------
// CSchedulerPanel
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CSchedulerPanel,wxPanel)

// WDR: event table for CSchedulerPanel

BEGIN_EVENT_TABLE(CSchedulerPanel,wxPanel)
    EVT_WINDOW_DESTROY(CSchedulerPanel::OnDestroy )
END_EVENT_TABLE()

CSchedulerPanel::CSchedulerPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
  m_pLog = NULL;
  m_pLogPrev = NULL;

  wxBoxSizer *boxSizer = new wxBoxSizer( wxHORIZONTAL );

  wxNotebook *mainNoteBook = new wxNotebook( this, ID_MAINNOTEBOOK, wxDefaultPosition, wxDefaultSize, 0 );

  // Create first LogPanel to set logger 
  CLogPanel *logPanel = new CLogPanel( mainNoteBook, ID_LOGPANEL, wxDefaultPosition, wxDefaultSize, 0 );
  m_pLog = new wxLogTextCtrl(logPanel->GetLogMess());
  m_pLogPrev = wxLog::SetActiveTarget(m_pLog);

  try
  {
    CPendingPanel *pendingPanel = new CPendingPanel( mainNoteBook, ID_PENDINGPANEL, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE|wxSP_3DSASH|wxCLIP_CHILDREN );
    CProcessingPanel *processingPanel = new CProcessingPanel( mainNoteBook, ID_PROCESSINGPANEL, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE|wxCLIP_CHILDREN ); 
    CEndedPanel *endedPanel = new CEndedPanel( mainNoteBook, ID_ENDEDPANEL, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE|wxCLIP_CHILDREN );

    mainNoteBook->AddPage(pendingPanel, PENDING_PAGE_NAME);
    mainNoteBook->AddPage(processingPanel, PROCESSING_PAGE_NAME);
    mainNoteBook->AddPage(endedPanel, ENDED_PAGE_NAME);
    mainNoteBook->AddPage(logPanel, LOG_PAGE_NAME);
  }
  catch(CException &e)
  {
    boxSizer->Add( mainNoteBook, 1, wxGROW|wxALL, 5 );

    SetSizer( boxSizer );

    throw e;
  }

  boxSizer->Add( mainNoteBook, 1, wxGROW|wxALL, 5 );

  SetSizer( boxSizer );

}
//----------------------------------------
CSchedulerPanel::~CSchedulerPanel()
{
}
//----------------------------------------
void CSchedulerPanel::OnDestroy( wxWindowDestroyEvent &event )
{
  wxLog::SetActiveTarget(m_pLogPrev);

  if (m_pLog != NULL)
  {
    delete m_pLog;
    m_pLog = NULL;
  }
}

// WDR: handler implementations for CSchedulerPanel




