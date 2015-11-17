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
    #pragma implementation "DelayDlg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "DelayDlg.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CDelayDlg
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(CDelayDlg,wxDialog)

// WDR: event table for CDelayDlg

BEGIN_EVENT_TABLE(CDelayDlg,wxDialog)
    EVT_BUTTON( wxID_OK, CDelayDlg::OnOk )
    EVT_BUTTON( wxID_CANCEL, CDelayDlg::OnCancel )
END_EVENT_TABLE()
//----------------------------------------

CDelayDlg::CDelayDlg( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
    // WDR: dialog function DelayDlg for CDelayDlg
  //----------------------------
    DelayDlg( this, TRUE ); 
  //----------------------------
}
//----------------------------------------
CDelayDlg::~CDelayDlg()
{
}
//----------------------------------------
void CDelayDlg::GetDateTime(wxDateTime& dateTime)
{
  dateTime = GetDatePicker()->GetValue();
  dateTime.ResetTime();

  wxTimeSpan timeSpan;
  wxString error;
  
  bool bOk = wxBratTools::ParseTime(GetTimePicker()->GetValue(), timeSpan, error);
  
  if (bOk)
  {
    dateTime += timeSpan;
  }
}


//----------------------------------------
// WDR: handler implementations for CDelayDlg

//----------------------------------------
void CDelayDlg::OnOk(wxCommandEvent &event)
{
    event.Skip();
}

//----------------------------------------
void CDelayDlg::OnCancel(wxCommandEvent &event)
{
    event.Skip();
}




