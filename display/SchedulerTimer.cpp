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
    #pragma implementation "SchedulerTimer.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "SchedulerTaskConfig.h"

#include "SchedulerTimer.h"

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CSchedulerTimer
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CSchedulerTimer,wxTimer)

//----------------------------------------

CSchedulerTimer::CSchedulerTimer()
{
}
//----------------------------------------

CSchedulerTimer::CSchedulerTimer(wxEvtHandler *owner, int id /* = wxID_ANY */)
    : wxTimer(owner, id)
{
}
//----------------------------------------

CSchedulerTimer::~CSchedulerTimer()
{
}
//----------------------------------------
void CSchedulerTimer::Notify()
{
  CSchedulerTaskConfig* schedulerTaskConfig = CSchedulerTaskConfig::GetInstance();

  wxDateTime now = wxDateTime::Now();

  std::vector<wxLongLong_t> vectorBratTaskToProcess;

  schedulerTaskConfig->GetMapPendingBratTaskToProcess(now, &vectorBratTaskToProcess);

  std::vector<wxLongLong_t>::const_iterator it;

  for (it = vectorBratTaskToProcess.begin() ; it != vectorBratTaskToProcess.end() ; it++)
  {
    CBratTaskProcessEvent ev(GetId(), *it);
    wxPostEvent(this->GetOwner(), ev);
  }

}
//----------------------------------------------------------------------------
// CCheckFileChange
//----------------------------------------------------------------------------

//----------------------------------------
DEFINE_EVENT_TYPE(wxEVT_CHECK_FILE_CHANGED)
//----------------------------------------

CCheckFileChange::CCheckFileChange()
{
}
//----------------------------------------

CCheckFileChange::CCheckFileChange(const wxString& fileTocheck)
{
  SetFileName(fileTocheck);
}
//----------------------------------------
CCheckFileChange::~CCheckFileChange()
{
}
//----------------------------------------
CCheckFileChange::CCheckFileChange(const CCheckFileChange& o)
{
  Set(o);
}

//----------------------------------------
const CCheckFileChange& CCheckFileChange::operator=(const CCheckFileChange& o)
{
  if (this == &o)
  {
    return *this;
  }

  Set(o);

  return *this;
}
//----------------------------------------
void CCheckFileChange::Set(const CCheckFileChange& o)
{
  m_fileName = o.m_fileName;
  m_fileCurrentMod = o.m_fileCurrentMod;
  m_fileLastMod = o.m_fileLastMod;
  m_fileDirty = o.m_fileDirty;
}
//----------------------------------------
void CCheckFileChange::GetFileCurrentMod()
{
  if (!m_fileName.FileExists())
  {
    return;
  }

  m_fileName.GetTimes(NULL, &m_fileCurrentMod, NULL);

  if (! m_fileLastMod.IsValid())
  {
    m_fileLastMod = m_fileCurrentMod;
    m_fileDirty = false;
  }

  if (! m_fileDirty)
  {
    m_fileDirty = (m_fileCurrentMod != m_fileLastMod);
    m_fileLastMod = m_fileCurrentMod;    
  }

  //wxString dt1 = m_fileCurrentMod.Format(CBratTask::FormatISODateTime);
  //wxString dt2 = m_fileLastMod.Format(CBratTask::FormatISODateTime);
}

//----------------------------------------
void CCheckFileChange::EvtCheckFileChangeCommand(wxEvtHandler& evtHandler, const CCheckFileChangeEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  evtHandler.Connect(wxEVT_CHECK_FILE_CHANGED,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CCheckFileChange::DisconnectEvtCheckFileChangeCommand(wxEvtHandler& evtHandler)
{
  evtHandler.Disconnect(wxEVT_CHECK_FILE_CHANGED);
}
//----------------------------------------------------------------------------
// CCheckFileChangeTimer
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CCheckFileChangeTimer,wxTimer)

//----------------------------------------

CCheckFileChangeTimer::CCheckFileChangeTimer()
{
}
//----------------------------------------

CCheckFileChangeTimer::CCheckFileChangeTimer(const wxString& fileTocheck, wxEvtHandler *owner, int id /* = wxID_ANY */)
    : wxTimer(owner, id)
{
  SetFileName(fileTocheck);
}
//----------------------------------------

CCheckFileChangeTimer::~CCheckFileChangeTimer()
{
}


//----------------------------------------
void CCheckFileChangeTimer::Notify()
{

  //int id = this->GetId();

  GetFileCurrentMod();

  if (IsFileDirty())
  {
    CCheckFileChangeEvent ev(wxID_ANY, m_checkFileChange);
    wxPostEvent(this->GetOwner(), ev);
  }

}






