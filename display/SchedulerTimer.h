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

#ifndef __SchedulerTimer_H__
#define __SchedulerTimer_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "SchedulerTimer.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
//#include "wx/timer.h"

//#include "BratScheduler_wdr.h"
#include "wx/filename.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CSchedulerTimer
//----------------------------------------------------------------------------

class CSchedulerTimer: public wxTimer
{
public:
  // constructors and destructors
  CSchedulerTimer();
  CSchedulerTimer(wxEvtHandler *owner, int id = wxID_ANY);

  virtual ~CSchedulerTimer();
  
  virtual void Notify();


private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(CSchedulerTimer)
};


class CCheckFileChangeEvent;

typedef void (wxEvtHandler::*CCheckFileChangeEventFunction)(CCheckFileChangeEvent&);

//----------------------------------------------------------------------------
// CCheckFileChange
//----------------------------------------------------------------------------

class CCheckFileChange
{
public:
  // constructors and destructors
  CCheckFileChange();
  CCheckFileChange(const wxString& fileTocheck);
  CCheckFileChange(const CCheckFileChange& o);

  virtual ~CCheckFileChange();
  
  void SetFileName(const wxString& value) {m_fileName.Assign(value);};
  void SetFileName(const wxFileName& value) {m_fileName = value;};
  const wxFileName* GetFileName() {return &m_fileName;};
  
  const wxDateTime* GetFileLastMod() { return &m_fileLastMod;};
  
  void SetFileDirty(bool value ) {m_fileDirty = value;};
  bool IsFileDirty() {return m_fileDirty;};

  void GetFileCurrentMod();

  const CCheckFileChange& operator=(const CCheckFileChange& o);
    
  static void EvtCheckFileChangeCommand(wxEvtHandler& evtHandler, const CCheckFileChangeEventFunction& method,
                                               wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  
  static void DisconnectEvtCheckFileChangeCommand(wxEvtHandler& evtHandler);
    

protected:

  void Set(const CCheckFileChange& o);

protected:
  wxFileName m_fileName;
  wxDateTime m_fileCurrentMod;
  wxDateTime m_fileLastMod;
  bool m_fileDirty;

};

//-------------------------------------------------------------
//------------------- CCheckFileChangeEvent class --------------------
//-------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_CHECK_FILE_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()

class CCheckFileChangeEvent : public wxCommandEvent
{
public:
  CCheckFileChangeEvent(wxWindowID id)
    : wxCommandEvent(wxEVT_CHECK_FILE_CHANGED, id)
  {
  };

  CCheckFileChangeEvent(wxWindowID id, const CCheckFileChange& checkFileChange)
    : wxCommandEvent(wxEVT_CHECK_FILE_CHANGED, id)
  {
    m_checkFileChange = checkFileChange;
  };

  CCheckFileChangeEvent(const CCheckFileChangeEvent& event)
    : wxCommandEvent(wxEVT_CHECK_FILE_CHANGED, event.m_id)
  {
    m_checkFileChange = event.m_checkFileChange;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CCheckFileChangeEvent(*this); 
    };

  CCheckFileChange m_checkFileChange;

};

//----------------------------------------------------------------------------
// CCheckFileChangeTimer
//----------------------------------------------------------------------------

class CCheckFileChangeTimer: public wxTimer
{
public:
  // constructors and destructors
  CCheckFileChangeTimer();
  CCheckFileChangeTimer(const wxString& fileTocheck, wxEvtHandler *owner, int id = wxID_ANY);

  virtual ~CCheckFileChangeTimer();
  
  void SetFileName(const wxString& value) {m_checkFileChange.SetFileName(value);};
  void SetFileName(const wxFileName& value) {m_checkFileChange.SetFileName(value);};
  const wxFileName* GetFileName() {return m_checkFileChange.GetFileName();};
  
  const wxDateTime* GetFileLastMod() { return m_checkFileChange.GetFileLastMod();};
  
  void SetFileDirty(bool value ) {m_checkFileChange.SetFileDirty(value);};
  bool IsFileDirty() {return m_checkFileChange.IsFileDirty();};
  void GetFileCurrentMod() {return m_checkFileChange.GetFileCurrentMod();};
  
  CCheckFileChange* GetCheckFileChange() {return &m_checkFileChange;};

  virtual void Notify();

protected:

protected:
  CCheckFileChange m_checkFileChange;

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(CCheckFileChangeTimer)
};




#endif
