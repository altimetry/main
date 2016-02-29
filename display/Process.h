
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

#ifndef __Process_H__
#define __Process_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/process.h"
#include "wx/filename.h"
#include "wx/thread.h"
#include "wx/listctrl.h"

#include "wxList.h"
#include "List.h"
using namespace brathl;


BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_PROCESS_TERM, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()


class CProcess;
class CBratTask;

// WDR: class declarations

//-------------------------------------------------------------
//------------------- CProcessTermEvent class --------------------
//-------------------------------------------------------------

class CProcessTermEvent : public wxCommandEvent
{
public:
  CProcessTermEvent(wxWindowID id, const wxString& name, int32_t type, int32_t pid, int32_t exitcode = 0)
    : wxCommandEvent(wxEVT_PROCESS_TERM, id)
  {
    m_name = name;
    m_type = type;
    m_pid = pid;
    m_exitcode = exitcode;
    m_taskUid = -1;
  };
  CProcessTermEvent(wxWindowID id, const wxString& name,  wxLongLong_t taskUid, int32_t type, int32_t pid, int32_t exitcode = 0)
    : wxCommandEvent(wxEVT_PROCESS_TERM, id)
  {
    m_name = name;
    m_type = type;
    m_pid = pid;
    m_exitcode = exitcode;
    m_taskUid = taskUid;
  };

  CProcessTermEvent(const CProcessTermEvent& event)
    : wxCommandEvent(wxEVT_PROCESS_TERM, event.m_id)
  {
    m_name = event.m_name;
    m_type = event.m_type;
    m_pid = event.m_pid;
    m_exitcode = event.m_exitcode;
    m_taskUid = event.m_taskUid;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CProcessTermEvent(*this); 
    };

  wxString m_name;
  int32_t m_type;
  int32_t m_pid;
  int32_t m_exitcode;
  wxLongLong_t m_taskUid;
};
typedef void (wxEvtHandler::*CProcessTermEventFunction)(CProcessTermEvent&);


//-------------------------------------------------------------
//------------------- CProcess class --------------------
//-------------------------------------------------------------


class CProcess : public wxProcess
{

protected:
	wxString m_name;
	wxWindow *m_parent;
	wxString m_cmd;

	int32_t m_running;
	bool m_killed;
	std::string m_output;
	int32_t m_type;

	int32_t m_executeFlags;

	long m_currentPid;


public:
	CProcess( const wxString& name, wxWindow *parent, const wxString& cmd, const std::string *output = NULL, int32_t type = -1 );

	virtual ~CProcess();

protected:

	virtual void SetCmd( const wxString& value ) { m_cmd = value; };
public:

	virtual wxString GetName() { return m_name; };
	virtual void SetName( const wxString& value ) { m_name = value; };

	int32_t GetExecuteFlags() { return m_executeFlags; };
	void SetExecuteFlags( int32_t value ) { m_executeFlags = value; };

	virtual wxString GetCmd() { return m_cmd; };

	virtual int32_t Execute();
	virtual int32_t Kill( wxSignal sig = wxSIGTERM );

	// instead of overriding this virtual function we might as well process the
	// event from it in the frame class - this might be more convenient in some
	// cases
	virtual void OnTerminate( int pid, int status );

	virtual void OnTerminate( int status );

	virtual bool HasInput() { return false; };

	bool IsRunning() { return m_running > 0; };
	bool IsEnded() { return m_running < 0; };
	bool IsReady() { return m_running == 0; };
	bool IsKilled() { return m_killed; };

	int32_t AskForPid();

	static void EvtProcessTermCommand( wxEvtHandler& evtHandler, const CProcessTermEventFunction& method,
		wxObject* userData = NULL, wxEvtHandler* eventSink = NULL );

	static void DisconnectEvtProcessTermCommand( wxEvtHandler& evtHandler );

protected:
	virtual int32_t ExecuteSync();
	virtual int32_t ExecuteAsync();

};

//-------------------------------------------------------------
//------------------- CPipedProcess class --------------------
//-------------------------------------------------------------

// A specialization of CProcess for redirecting the output
class CPipedProcess : public CProcess
{
protected:

	wxTextCtrl *m_logCtrl = nullptr;
	wxFile *m_logFile = nullptr;
	wxFileName *m_logFileName = nullptr;

public:
	CPipedProcess( const wxString& name, wxWindow *parent, const wxString& cmd, wxTextCtrl* logCtrl = NULL, const std::string *output = NULL, int32_t type = -1 );

	virtual ~CPipedProcess();

	virtual void OnTerminate( int pid ) override;
	virtual void OnTerminate( int pid, int status ) override;

	virtual bool HasInput() override;

	void SetLogFile( const wxString& value );
	void SetLogFile( const wxFileName& value );
	void SetLogFile( wxFileName* value );

	wxFile* GetLogFile() { return m_logFile; };

	wxFileName* GetLogFileName() { return m_logFileName; };

protected:
	void DeleteLogFile();
	void WriteToLogFile( const wxString& msg );
};


//-------------------------------------------------------------
//------------------- CBratTaskProcess class --------------------
//-------------------------------------------------------------

class CBratTaskProcess : public CPipedProcess
{
public:
  CBratTaskProcess(CBratTask* bratTask, wxWindow *parent);

  virtual ~CBratTaskProcess();

  virtual int32_t Execute() override;

  virtual void OnTerminate(int pid) override;
  virtual void OnTerminate(int pid, int status) override;
  
  virtual bool HasInput() override;
  
  CBratTask* GetBratTask() { return m_bratTask; };


protected:

  virtual int32_t ExecuteSync() override;
  virtual int32_t ExecuteAsync() override;
  
  virtual int32_t ExecuteFunction(CBratTask* bratTask);
  
  virtual int32_t ExecuteAsyncSubordinateTasks();

  void InstallEventListeners();
  void DeInstallEventListeners();

  void OnTerminateReport(int pid, int status, CBratTask* bratTask, const wxString& taskLabel = "TASK");
  
  void OnProcessTerm(CProcessTermEvent& event);

protected:
  
  CBratTask* m_bratTask;
  int32_t m_subordinateTaskIndex;

};
//-------------------------------------------------------------
//------------------- CMapProcess class --------------------
//-------------------------------------------------------------

class CMapProcess: public wxObMap
{
public:
  /// CIntMap ctor
  CMapProcess(bool bDelete = false);

  /// CIntMap dtor
  virtual ~CMapProcess();

  bool ValidName(const char* name);
  bool ValidName(const std::string& name);
  
  void NamesToArrayString(wxArrayString& array);
  void NamesToComboBox(wxComboBox& combo);



protected:

public:


private :


};




#endif
