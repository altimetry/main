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

#ifndef __BratTask_H__
#define __BratTask_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "BratTask.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/xml/xml.h> // (wxXmlDocument, wxXmlNode, wxXmlProperty classes)
#include "wx/filename.h"

#include "brathl.h" 
#include "BratAlgorithmBase.h" 

#include "Stl.h"

using namespace brathl;


BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_BRAT_TASK_PROCESS, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()


class CBratTask;


//-------------------------------------------------------------
//------------------- CBratTaskProcessEvent class --------------------
//-------------------------------------------------------------

class CBratTaskProcessEvent : public wxCommandEvent
{
public:
  CBratTaskProcessEvent(wxWindowID id, wxLongLong_t uid)
    : wxCommandEvent(wxEVT_BRAT_TASK_PROCESS, id)
  {
    m_uid = uid;
  };

  CBratTaskProcessEvent(const CBratTaskProcessEvent& event)
    : wxCommandEvent(wxEVT_BRAT_TASK_PROCESS, event.m_id)
  {
    m_uid = event.m_uid;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CBratTaskProcessEvent(*this); 
    };

  wxLongLong_t m_uid;

};
typedef void (wxEvtHandler::*CBratTaskProcessEventFunction)(CBratTaskProcessEvent&);




//-------------------------------------------------------------
//------------------- CMapBratTask class --------------------
//-------------------------------------------------------------
typedef map<wxLongLong_t, CBratTask*> mapbrattask; 


class CMapBratTask : public mapbrattask
{
public:
  CMapBratTask(bool bDelete = true);  
  virtual ~CMapBratTask();

  virtual CBratTask* Insert(wxLongLong_t key, CBratTask* ob, bool withExcept = true);
  virtual void Insert(const CMapBratTask* map, bool bRemoveAll = true);

  virtual bool Remove(wxLongLong_t id);
  virtual bool Remove(CMapBratTask::iterator it);


  virtual void RemoveAll();

  virtual void Dump(ostream& fOut = cerr); 

  virtual CBratTask* Find(wxLongLong_t id) const;

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};


protected:

  void Init();

protected:

  bool m_bDelete;



};
//-------------------------------------------------------------
//------------------- CVectorBratTask class --------------------
//-------------------------------------------------------------

typedef vector<CBratTask*> vectorbrattask; 

class CVectorBratTask : public vectorbrattask
{
public:
  CVectorBratTask(bool bDelete = true);  
  virtual ~CVectorBratTask();

  virtual void Insert(CBratTask* ob, bool bEnd = true);
  virtual void Insert(const CVectorBratTask* vector, bool bRemoveAll = true,  bool bEnd = true);
  virtual CVectorBratTask::iterator InsertAt(CVectorBratTask::iterator where, CBratTask* ob);

  virtual void RemoveAll();

  virtual void Dump(ostream& fOut = cerr); 

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

protected:

  void Init();

protected:

  bool m_bDelete;



};

//-------------------------------------------------------------
//------------------- CBratTaskFunction class --------------------
//-------------------------------------------------------------

typedef void BratTaskFunctionCallableN(CVectorBratAlgorithmParam& arg);

class CBratTaskFunction
{

public:
  CBratTaskFunction();
  CBratTaskFunction(const wxString& name, BratTaskFunctionCallableN* call);
  CBratTaskFunction(const CBratTaskFunction& o);

  virtual ~CBratTaskFunction();

  void SetName(const wxString& value) { m_name = value;};
  wxString GetName() { return m_name;};
  
  BratTaskFunctionCallableN* GetCall() { return m_call;};
  void SetCall(BratTaskFunctionCallableN* value) { m_call = value;};
  
  CVectorBratAlgorithmParam* GetParams() { return &m_params;};
  void GetParamsAsString(wxString& value);
  
  void Execute();

  virtual void Dump(ostream& fOut = cerr); 

  const CBratTaskFunction& operator=(const CBratTaskFunction& o);

  static void CopyFile(CVectorBratAlgorithmParam& arg);

public:

  static const wxString m_TASK_FUNC_COPYFILE;

protected:
  void Init();
  void Set(const CBratTaskFunction& o);

protected:

  wxString m_name;
  BratTaskFunctionCallableN* m_call;
  CVectorBratAlgorithmParam m_params;

};
//-------------------------------------------------------------
//------------------- CMapBratTaskFunction class --------------------
//-------------------------------------------------------------
typedef map<wxString, CBratTaskFunction*> mapbrattaskfunction; 

class CMapBratTaskFunction: public mapbrattaskfunction
{
protected:
  CMapBratTaskFunction(bool bDelete = true);

public:

  virtual ~CMapBratTaskFunction();

  static CMapBratTaskFunction&  GetInstance();

  virtual CBratTaskFunction* Insert(const wxString& key, CBratTaskFunction* ob, bool withExcept = true);

  virtual bool Remove(const wxString& id);
  virtual bool Remove(CMapBratTaskFunction::iterator it);

  virtual void RemoveAll();

  virtual void Dump(ostream& fOut = cerr); 

  virtual CBratTaskFunction* Find(const wxString& id) const;

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

protected:

  void Init();

protected:

  bool m_bDelete;



};
//-------------------------------------------------------------
//------------------- CBratTask class --------------------
//-------------------------------------------------------------


class CBratTask
{
public:

  typedef enum {
    BRAT_STATUS_PENDING = 0, 
    BRAT_STATUS_PROCESSING,
    BRAT_STATUS_ENDED,
    BRAT_STATUS_ERROR,
    BRAT_STATUS_WARNING,
  } bratTaskStatus;

public:
  /** Default contructor */
  CBratTask();
  /** Copy contructor */
  CBratTask(const CBratTask	&o);

  CBratTask(wxXmlNode* task);

  /** Destructor */
  virtual ~CBratTask();

 virtual CBratTask* Clone() { return new CBratTask(*this); }

  void SetUid(const wxString& value);
  void SetUid(const wxLongLong& value) {m_uid = value;}
  void SetUid(wxLongLong_t value) {m_uid = value;}
  
  const wxLongLong& GetUid() {return m_uid;}
  
  wxLongLong_t GetUidValue() {return m_uid.GetValue();}
  
  wxString GetUidValueAsString() {return m_uid.ToString();}
  
  //void SetId(const wxString& value) {m_id = value;};
  //const wxString& GetId() {return m_id;};
  
  void SetName(const wxString& value) {m_name = value;}
  wxString GetName() {return m_name;}
  
  void SetAt(const wxDateTime& value) {m_at = value;}
  const wxDateTime& GetAt() {return m_at;}

  wxString GetAtAsString(const wxString& format = CBratTask::FormatISODateTime) {return m_at.Format(format);}
  
  void SetCmd(const wxString& value) {m_cmd = value;}
  const wxString& GetCmd() {return m_cmd;}
  
  void SetStatus(const wxString& value) {m_status = CBratTask::StringToTaskStatus(value);}
  void SetStatus(CBratTask::bratTaskStatus value) {m_status = value;}
  CBratTask::bratTaskStatus GetStatus() {return m_status;}
  wxString GetStatusAsString() {return CBratTask::TaskStatusToString(m_status);}


  void SetLogFile(const wxString& value) {m_logFile.Assign(value);}
  void SetLogFile(const wxFileName& value) {m_logFile = value;}
  
  wxFileName* GetLogFile() {return &m_logFile;}
  wxString GetLogFileAsString() {return m_logFile.GetFullPath();}


  CVectorBratTask* GetSubordinateTasks() {return &m_subordinateTasks; }
  bool HasSubordinateTasks() {return (m_subordinateTasks.size() > 0); }

  CBratTaskFunction* GetBratTaskFunction() { return &m_function; }
  void SetBratTaskFunction(const CBratTaskFunction& value) { m_function = value; }
  void SetBratTaskFunction(CBratTaskFunction* value) 
          { if (value != NULL)
            { m_function = *value; }
          };
  
  bool HasCmd() { return !(m_cmd.IsEmpty()); }
  bool HasFunction() { return !(m_function.GetName().IsEmpty()); }
  
  void ExecuteFunction();

  const CBratTask& operator=(const CBratTask &o);

  static wxString TaskStatusToString(CBratTask::bratTaskStatus status);
  static CBratTask::bratTaskStatus StringToTaskStatus(const wxString& status);

  /** Dump function */
  virtual void Dump(ostream& fOut = cerr);

  static void EvtBratTaskProcessCommand(wxEvtHandler& evtHandler, const CBratTaskProcessEventFunction& method,
                                               wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  
  static void DisconnectEvtBratTaskProcessCommand(wxEvtHandler& evtHandler);

public:
  static const wxString m_BRAT_STATUS_ENDED_LABEL;
  static const wxString m_BRAT_STATUS_ERROR_LABEL;
  static const wxString m_BRAT_STATUS_PENDING_LABEL;
  static const wxString m_BRAT_STATUS_PROCESSING_LABEL;
  static const wxString m_BRAT_STATUS_WARNING_LABEL;

    // datetime in ISO 8601 format
  static const wxString FormatISODateTime;

protected: 
  wxLongLong m_uid;
  //wxString m_id;
  wxString m_name;
  wxString m_cmd;
  CBratTaskFunction m_function;
  wxDateTime m_at;
  bratTaskStatus m_status;

  wxFileName m_logFile;

  CVectorBratTask m_subordinateTasks;

protected:

  void Init();
  void Set(const CBratTask &o);

};



#endif
