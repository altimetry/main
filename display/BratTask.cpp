/*
* Copyright (C) 2005-2010 Collecte Localisation Satellites (CLS), France (31)
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
    #pragma implementation "BratTask.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "TraceLog.h" 
#include "Exception.h"

#include "wxBratTools.h"

#include "BratTask.h"
#include "SchedulerTaskConfig.h"

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;




DEFINE_EVENT_TYPE(wxEVT_BRAT_TASK_PROCESS)

const wxString CBratTaskFunction::m_TASK_FUNC_COPYFILE = "CopyFile";

const wxString CBratTask::FormatISODateTime = "%Y-%m-%d %H:%M:%S";

const wxString CBratTask::m_BRAT_STATUS_ENDED_LABEL = "ended";
const wxString CBratTask::m_BRAT_STATUS_ERROR_LABEL = "error";
const wxString CBratTask::m_BRAT_STATUS_PENDING_LABEL = "pending";
const wxString CBratTask::m_BRAT_STATUS_PROCESSING_LABEL = "in progress";
const wxString CBratTask::m_BRAT_STATUS_WARNING_LABEL = "warning";

//-------------------------------------------------------------
//------------------- CBratTaskFunction class --------------------
//-------------------------------------------------------------

//----------------------------------------
CBratTaskFunction::CBratTaskFunction()
{
  Init();
}
//----------------------------------------
CBratTaskFunction::CBratTaskFunction(const wxString& name, BratTaskFunctionCallableN* call)
{
  Init();
  m_name = name;
  m_call = call;
}
//----------------------------------------
CBratTaskFunction::CBratTaskFunction(const CBratTaskFunction& o)
{
  Init();
  Set(o);
}
//----------------------------------------

CBratTaskFunction::~CBratTaskFunction()
{
}
//----------------------------------------
void CBratTaskFunction::Init()
{
  m_call = NULL;
}

//----------------------------------------
const CBratTaskFunction& CBratTaskFunction::operator=(const CBratTaskFunction& o)
{
  if (this == &o)
  {
    return *this;
  }

  Set(o);

  return *this;
}
//----------------------------------------
void CBratTaskFunction::Set(const CBratTaskFunction& o)
{
  m_name = o.m_name;
  m_call = o.m_call;
}

//----------------------------------------
void CBratTaskFunction::GetParamsAsString(wxString& value)
{
  value = m_params.ToString().c_str();
}
//----------------------------------------
void CBratTaskFunction::Execute()
{
  try
  {
    m_call(m_params);
  }
  catch(CException& e)
  {
    wxString msg = wxString::Format("Unable to execute function '%s' - Native error: %s.", m_name.c_str(), e.GetMessage().c_str());
    throw CException(msg.c_str(), BRATHL_ERROR);
  }
  catch(exception& e)
  {
    wxString msg = wxString::Format("Unable to execute function '%s' - Native error: %s.", m_name.c_str(), e.what());
    throw CException(msg.c_str(), BRATHL_ERROR);

  }
  catch(...)
  {
    wxString msg = wxString::Format("Unable to execute function '%s' - Native error is unknow (perhaps runtime error)", m_name.c_str());
    throw CException(msg.c_str(), BRATHL_ERROR);

  }
}
//----------------------------------------

void CBratTaskFunction::CopyFile(CVectorBratAlgorithmParam& arg)
{
  if (arg.size() != 2)
  {
    wxString msg = wxString::Format("CBratTaskFunction::CopyFile - Unable to execute  Brat task - Expected number of parameters is 2, but found %d",
                                    static_cast<int32_t>(arg.size()));
  
    throw CException(msg.c_str(), BRATHL_ERROR);
  }

  wxString p1 = arg.at(0).GetValueAsString().c_str();
  wxString p2 = arg.at(1).GetValueAsString().c_str();

  bool bOk = wxCopyFile(p1, p2);

  if (!bOk)
    {
      wxString msg = wxString::Format("Unable to copy file '%s' to '%s'" ,
                        p1.c_str(),
                        p2.c_str());

      throw CException(msg.c_str(), BRATHL_ERROR);
    }

}

//----------------------------------------
void CBratTaskFunction::Dump(ostream& fOut /*= cerr*/)
{
  if (! CTrace::IsTrace())
  {
    return;
  }

  fOut << "==> Dump a CBratTaskFunction Object at "<< this << endl;
  fOut << "m_name: " << m_name << endl;
  fOut << "m_call: " << m_call << endl;
  fOut << "m_params: " << endl;
  m_params.Dump(fOut);
  fOut << "==> END Dump a CBratTaskFunction Object at "<< this << endl;

}//----------------------------------------


//-------------------------------------------------------------
//------------------- CMapBratTaskFunction class --------------------
//-------------------------------------------------------------

CMapBratTaskFunction::CMapBratTaskFunction(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;
  Init();
}


//----------------------------------------
CMapBratTaskFunction::~CMapBratTaskFunction()
{
  RemoveAll();
}
//----------------------------------------
void CMapBratTaskFunction::Init()
{
  this->Insert(CBratTaskFunction::m_TASK_FUNC_COPYFILE, new CBratTaskFunction(CBratTaskFunction::m_TASK_FUNC_COPYFILE, CBratTaskFunction::CopyFile)); 
}

//----------------------------------------
CMapBratTaskFunction& CMapBratTaskFunction::GetInstance()
{
 static CMapBratTaskFunction instance;

 return instance;
}
//----------------------------------------
CBratTaskFunction* CMapBratTaskFunction::Insert(const wxString& key, CBratTaskFunction* ob, bool withExcept /* = true */)
{
  

  pair <CMapBratTaskFunction::iterator,bool> pairInsert;


  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object
  pairInsert = (mapbrattaskfunction::insert(CMapBratTaskFunction::value_type(key, ob)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in CMapBratTaskFunction::Insert - try to insert an task that already exists. Check that no task have the same id", BRATHL_INCONSISTENCY_ERROR);
    Dump(*CTrace::GetDumpContext());

    throw(e);
  }
  
  CMapBratTaskFunction::iterator it = (pairInsert.first);
  return (*it).second;;

}
//----------------------------------------
CBratTaskFunction* CMapBratTaskFunction::Find(const wxString& id) const
{
  CMapBratTaskFunction::const_iterator it = mapbrattaskfunction::find(id);
  if (it == end())
  {
    return NULL;
  }
  else
  {
    return (*it).second;
  }

}

//----------------------------------------
bool CMapBratTaskFunction::Remove(const wxString& id)
{
  CMapBratTaskFunction::iterator it = mapbrattaskfunction::find(id);
  
  return Remove(it);
}
//----------------------------------------
bool CMapBratTaskFunction::Remove(CMapBratTaskFunction::iterator it)
{
  if (it == end())
  {
    return false;
  }

  if (m_bDelete)
  {
    CBratTaskFunction* ob = it->second;  
    if (ob != NULL)
    {
      delete  ob;
    }
  }

  mapbrattaskfunction::erase(it);

  return true;
}
//----------------------------------------
void CMapBratTaskFunction::RemoveAll()
{
  
  CMapBratTaskFunction::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      CBratTaskFunction* ob = it->second;
      if (ob != NULL)
      {
        delete  ob;
      }
    }
  }

  mapbrattaskfunction::clear();

}

//----------------------------------------
void CMapBratTaskFunction::Dump(ostream& fOut /* = cerr */)
{

   if (CTrace::IsTrace() == false)
   { 
      return;
   }

   fOut << "==> Dump a CMapBratTaskFunction Object at "<< this << " with " <<  size() << " elements" << endl;

   CMapBratTaskFunction::iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      CBratTaskFunction *ob = it->second;
      fOut << "CMapBratTaskFunction Key is = " << (*it).first << endl;
      fOut << "CMapBratTaskFunction Value is = " << endl;
      ob->Dump(fOut);
   }

   fOut << "==> END Dump a CMapBratTaskFunction Object at "<< this << " with " <<  size() << " elements" << endl;

}


//-------------------------------------------------------------
//------------------- CBratTask class --------------------
//-------------------------------------------------------------

//----------------------------------------
CBratTask::CBratTask()
{
  Init();
}
  
//----------------------------------------
CBratTask::CBratTask(const CBratTask &o)
{
  Init();
  Set(o); 
}
  

//----------------------------------------
CBratTask::~CBratTask()
{
}

//----------------------------------------
const CBratTask& CBratTask::operator=(const CBratTask &o)
{
  if (this == &o)
  {
    return *this;
  }

  Set(o);

  return *this;
}
//----------------------------------------
void CBratTask::Init()
{
  m_uid = -1;
  m_status = CBratTask::BRAT_STATUS_PENDING;
}
//----------------------------------------
void CBratTask::Set(const CBratTask &o) 
{
  m_uid = o.m_uid;
  m_name = o.m_name;
  m_cmd = o.m_cmd;
  m_at = o.m_at;
  m_status = o.m_status;
  m_function = o.m_function;
  m_logFile = o.m_logFile;

  m_subordinateTasks.Insert(&o.m_subordinateTasks);

}
//----------------------------------------
void CBratTask::EvtBratTaskProcessCommand(wxEvtHandler& evtHandler, const CBratTaskProcessEventFunction& method,
                                               wxObject* userData, wxEvtHandler* eventSink)
{
  evtHandler.Connect(wxEVT_BRAT_TASK_PROCESS,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
//----------------------------------------
void CBratTask::DisconnectEvtBratTaskProcessCommand(wxEvtHandler& evtHandler)
{
  evtHandler.Disconnect(wxEVT_BRAT_TASK_PROCESS);
}
//----------------------------------------
void CBratTask::SetUid(const wxString& value)
{
  m_uid = wxBratTools::wxStringTowxLongLong_t(value);
}
//----------------------------------------
void CBratTask::ExecuteFunction()
{
  m_function.Execute();
}

//----------------------------------------
wxString CBratTask::TaskStatusToString(CBratTask::bratTaskStatus status)
{
  wxString value = "";
  switch (status) 
  {
    case CBratTask::BRAT_STATUS_PENDING: value = CBratTask::m_BRAT_STATUS_PENDING_LABEL; break;
    case CBratTask::BRAT_STATUS_PROCESSING: value = CBratTask::m_BRAT_STATUS_PROCESSING_LABEL; break;
    case CBratTask::BRAT_STATUS_ENDED: value = CBratTask::m_BRAT_STATUS_ENDED_LABEL; break;
    case CBratTask::BRAT_STATUS_ERROR: value = CBratTask::m_BRAT_STATUS_ERROR_LABEL; break;
    case CBratTask::BRAT_STATUS_WARNING: value = CBratTask::m_BRAT_STATUS_WARNING_LABEL; break;
    default: value = wxString::Format("CBratTask::TaskStatusToString: unknown status %d.",
               static_cast<uint32_t>(status)); break;

  }

  return value; 
}

//----------------------------------------
CBratTask::bratTaskStatus CBratTask::StringToTaskStatus(const wxString& status)
{
  CBratTask::bratTaskStatus value;

  if (status.CmpNoCase(CBratTask::m_BRAT_STATUS_PENDING_LABEL) == 0)
  {
    value = CBratTask::BRAT_STATUS_PENDING;
  }
  else if (status.CmpNoCase(CBratTask::m_BRAT_STATUS_PROCESSING_LABEL) == 0)
  {
    value = CBratTask::BRAT_STATUS_PROCESSING;
  }
  else if (status.CmpNoCase(CBratTask::m_BRAT_STATUS_ENDED_LABEL) == 0)
  {
    value = CBratTask::BRAT_STATUS_ENDED;
  }
  else if (status.CmpNoCase(CBratTask::m_BRAT_STATUS_ERROR_LABEL) == 0)
  {
    value = CBratTask::BRAT_STATUS_ERROR;
  }
  else if (status.CmpNoCase(CBratTask::m_BRAT_STATUS_WARNING_LABEL) == 0)
  {
    value = CBratTask::BRAT_STATUS_WARNING;
  }
  else 
  {
    wxString msg = wxString::Format("CBratTask::StringToTaskStatus: unknown status label '%s'.", status.c_str());
    throw CException(msg.c_str(), BRATHL_INCONSISTENCY_ERROR);
  }


  return value; 
}
//----------------------------------------
void CBratTask::Dump(ostream& fOut /*= cerr*/)
{
  if (! CTrace::IsTrace())
  {
    return;
  }
 
  fOut << "==> Dump a CBratTask Object at "<< this << endl;
  
  fOut << "m_uid: " << this->GetUidValue() << endl;
  fOut << "m_at: " << m_at.GetValue().GetValue() << "(" << GetAtAsString() << ")" << endl;
  fOut << "m_status: " << m_status << endl;
  fOut << "m_cmd: " << m_cmd << endl;
  fOut << "m_function: " << endl;
  m_function.Dump(fOut);
  fOut << "m_subordinateTasks: " << endl;
  m_subordinateTasks.Dump(fOut);

  fOut << "==> END Dump a CBratTask Object at "<< this << endl;

}
//-------------------------------------------------------------
//------------------- CMapBratTask class --------------------
//-------------------------------------------------------------

CMapBratTask::CMapBratTask(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;
  Init();

}


//----------------------------------------
CMapBratTask::~CMapBratTask()
{
  RemoveAll();
}

//----------------------------------------
void CMapBratTask::Insert(const CMapBratTask* map, bool bRemoveAll /* = true */)
{
  if (map == NULL)
  {
    return;
  }

  if (bRemoveAll)
  {
    this->RemoveAll();
  }


  CMapBratTask::const_iterator it;

  for (it = map->begin() ; it != map->end() ; it++)
  {
    CBratTask* ob = (it->second)->Clone();
    Insert(it->first, ob);
  }

}

//----------------------------------------
CBratTask* CMapBratTask::Insert(wxLongLong_t key, CBratTask* ob, bool withExcept /* = true */)
{
  

  pair <CMapBratTask::iterator,bool> pairInsert;


  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object
  pairInsert = (mapbrattask::insert(CMapBratTask::value_type(key, ob)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in CMapBratTask::Insert - try to insert an task that already exists. Check that no task have the same id", BRATHL_INCONSISTENCY_ERROR);
    Dump(*CTrace::GetDumpContext());

    throw(e);
  }
  
  CMapBratTask::iterator it = (pairInsert.first);
  return (*it).second;;

}
//----------------------------------------
void CMapBratTask::Init()
{
}

//----------------------------------------
CBratTask* CMapBratTask::Find(wxLongLong_t id) const
{
  CMapBratTask::const_iterator it = mapbrattask::find(id);
  if (it == end())
  {
    return NULL;
  }
  else
  {
    return (*it).second;
  }

}

//----------------------------------------
bool CMapBratTask::Remove(wxLongLong_t id)
{
  CMapBratTask::iterator it = mapbrattask::find(id);
  
  return Remove(it);
}
//----------------------------------------
bool CMapBratTask::Remove(CMapBratTask::iterator it)
{
  if (it == end())
  {
    return false;
  }

  if (m_bDelete)
  {
    CBratTask* ob = it->second;  
    if (ob != NULL)
    {
      delete  ob;
    }
  }

  mapbrattask::erase(it);

  return true;
}
//----------------------------------------
void CMapBratTask::RemoveAll()
{
  
  CMapBratTask::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      CBratTask* ob = it->second;
      if (ob != NULL)
      {
        delete  ob;
      }
    }
  }

  mapbrattask::clear();

}
//----------------------------------------
void CMapBratTask::Dump(ostream& fOut /* = cerr */)
{

   if (CTrace::IsTrace() == false)
   { 
      return;
   }

   fOut << "==> Dump a CMapBratTask Object at "<< this << " with " <<  size() << " elements" << endl;

   CMapBratTask::iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      CBratTask *ob = it->second;
      fOut << "CMapBratTask Key is = " << (*it).first << endl;
      fOut << "CMapBratTask Value is = " << endl;
      ob->Dump(fOut);
   }

   fOut << "==> END Dump a CMapBratTask Object at "<< this << " with " <<  size() << " elements" << endl;

}

//-------------------------------------------------------------
//------------------- CVectorBratTask class --------------------
//-------------------------------------------------------------

CVectorBratTask::CVectorBratTask(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;
  Init();

}


//----------------------------------------
CVectorBratTask::~CVectorBratTask()
{
  RemoveAll();
}

//----------------------------------------
void CVectorBratTask::Init()
{
}
//----------------------------------------
void CVectorBratTask::Insert(CBratTask* ob, bool bEnd /* = true */)
{
  if (bEnd)
  {  
    vectorbrattask::push_back(ob);   
  }
  else
  {  
    CVectorBratTask::InsertAt(this->begin(), ob);   
  }

}
//----------------------------------------
void CVectorBratTask::Insert(const CVectorBratTask* vector, bool bRemoveAll /* = true */, bool bEnd /* = true */)
{
  if (vector == NULL)
  {
    return;
  }

  if (bRemoveAll)
  {
    this->RemoveAll();
  }


  CVectorBratTask::const_iterator it;

  for (it = vector->begin() ; it != vector->end() ; it++)
  {
    CBratTask* ob = (*it)->Clone();
    Insert(ob, bEnd);
  }

}

//----------------------------------------
CVectorBratTask::iterator CVectorBratTask::InsertAt(CVectorBratTask::iterator where, CBratTask* ob)
{ 
  return vectorbrattask::insert(where, ob);     
}


//----------------------------------------
void CVectorBratTask::RemoveAll()
{
  
  CVectorBratTask::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      CBratTask* ob = *it;
      if (ob != NULL)
      {
        delete  ob;
      }
    }
  }

  vectorbrattask::clear();

}
//----------------------------------------
void CVectorBratTask::Dump(ostream& fOut /* = cerr */)
{

 if (CTrace::IsTrace() == false)
  {
    return;
  }

  CVectorBratTask::const_iterator it;
  int i = 0;

  fOut << "==> Dump a CVectorBratTask Object at "<< this << " with " <<  size() << " elements" << endl;

  for ( it = this->begin( ); it != this->end( ); it++ )
  {
    fOut << "CVectorBratTask[" << i << "]= " << endl;  
    if ((*it) != NULL)
    {
     (*it)->Dump(fOut);
    }
    i++;
  }

  fOut << "==> END Dump a CVectorBratTask Object at "<< this << " with " <<  size() << " elements" << endl;


  fOut << endl;

}

