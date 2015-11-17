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
    #pragma implementation "wxBratTools.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wxBratTools.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"


//-------------------------------------------------------------
//------------------- wxBratTools class --------------------
//-------------------------------------------------------------

//----------------------------------------
wxBratTools::wxBratTools()
{
}
//----------------------------------------
wxBratTools::~wxBratTools()
{
}
//----------------------------------------
wxLongLong_t wxBratTools::wxStringTowxLongLong_t(const wxString& str)
{
  wxLongLong_t value = 0 ;
  long valueHigh = 0 ;
  long valueLow = 0 ;

  if (str.IsEmpty())
  {
    return value;
  }
  
  wxString s = str;
  
  int factor = ((s.at(0) == '-') ? -1 : 1);
  if (factor < 0)
  {
    s.Remove(static_cast<size_t>(0), static_cast<size_t>(1));
  }

  int length = s.Length() ;

  if (length <= 9) 
  {
    if (!s.ToLong(&valueLow)) 
    {
      valueLow = 0 ;
    }

    value = valueLow ;
    return (value * factor) ;
  }

  // length > 9

  wxString low = s.Right(9) ;
  wxString high = s.Left(length - 9) ;

  if (!low.ToLong(&valueLow)) 
  {
    valueLow = 0 ;
  }

  if (!high.ToLong(&valueHigh))
  {
    valueHigh = 0 ;
  }

  // assign first to avoid precision loss!
  //wxLongLong ll(valueHigh, valueLow);
  //ll *= factor;
  //value = ll.GetValue();
  value = valueHigh ;
  value *= 1000000000 ;
  value += valueLow ;
  value *= factor;
  return value;
}
//----------------------------------------------------
wxString wxBratTools::MutexErrorToString(wxMutexError error)
{
  wxString msg;
  switch (error)
  {
    case wxMUTEX_NO_ERROR: msg = "operation completed successfully"; break;
    case wxMUTEX_INVALID: msg = "mutex hasn't been initialized"; break;
    case wxMUTEX_DEAD_LOCK: msg = "mutex is already locked by the calling thread"; break;
    case wxMUTEX_BUSY: msg = "mutex is already locked by another thread"; break;
    case wxMUTEX_UNLOCKED: msg = "attempt to unlock a mutex which is not locked"; break;
    case wxMUTEX_MISC_ERROR: msg = "any other error"; break;
    default: msg = "any other error"; break;
  }
  return msg;
}
//----------------------------------------------------
bool wxBratTools::ParseTime(const wxString& value, wxTimeSpan& ts, wxString& error) 
{
  wxDateTime dt;
  
  bool bOk = ParseTime(value, dt, error);
  
  if (!bOk)
  {
    return bOk;
  }

  wxTimeSpan tsTemp(dt.GetHour(), dt.GetMinute());
  ts = tsTemp;

  //ts = wxTimeSpan::Hours(dt.GetHour());
  //ts += wxTimeSpan::Hours(dt.GetMinute());

  return bOk;

}
//----------------------------------------------------
bool wxBratTools::ParseTime(const wxString& value, wxDateTime& dt, wxString& error) 
{
  bool bOk = true;

  wxLogBuffer logBuffer;
  wxLog* initialLog = wxLog::SetActiveTarget(&logBuffer);

  const wxChar* result = dt.ParseTime(value);
  if (result == NULL)
  {
    error = logBuffer.GetBuffer();
    bOk = false;
  }

  wxLog::SetActiveTarget(initialLog);
  return bOk;

}
//----------------------------------------------------
bool wxBratTools::ParseDate(const wxString& value, wxDateTime& dt, wxString& error) 
{
  bool bOk = true;

  wxLogBuffer logBuffer;
  wxLog* initialLog = wxLog::SetActiveTarget(&logBuffer);

  const wxChar* result = dt.ParseDate(value);
  if (result == NULL)
  {
    error = logBuffer.GetBuffer();
    bOk = false;
  }

  wxLog::SetActiveTarget(initialLog);
  return bOk;

}
//----------------------------------------------------
bool wxBratTools::ParseDateTime(const wxString& value, wxDateTime& dt, wxString& error) 
{
  bool bOk = true;

  wxLogBuffer logBuffer;
  wxLog* initialLog = wxLog::SetActiveTarget(&logBuffer);

  const wxChar* result = dt.ParseDateTime(value);
  if (result == NULL)
  {
    error = logBuffer.GetBuffer();
    bOk = false;
  }

  wxLog::SetActiveTarget(initialLog);
  return bOk;

}
//----------------------------------------
bool wxBratTools::RemoveFile(const wxString& name)
{
  bool bOk = true;

  bOk = wxFileExists(name);
  if (bOk == false)
  {
    return true;
  }

  try
  {
    bOk = wxRemoveFile(name);
  }
  catch (...)
  {
    //Nothing to do
    bOk = false;
  }
  return bOk;
}

//----------------------------------------
bool wxBratTools::RenameFile(const wxString& oldName, const wxString& newName)
{
  bool bOk = true;

  bOk = wxFileExists(oldName);
  if (bOk == false)
  {
    return true;
  }

  try
  {
    bOk = wxRenameFile(oldName, newName);
  }
  catch (...)
  {
    //Nothing to do
    bOk = false;
  }

  return bOk;
}
//----------------------------------------
void wxBratTools::CStringArrayToWxArray(CStringArray& from, wxArrayString& to)
{
  CStringArray::iterator it;
  for (it = from.begin() ; it != from.end() ; it++)
  {
    to.Add((*it).c_str());
  }
}
//----------------------------------------
void wxBratTools::CStringListToWxArray(CStringList& from, wxArrayString& to)
{
  CStringList::iterator it;
  for (it = from.begin() ; it != from.end() ; it++)
  {
    to.Add((*it).c_str());
  }
}