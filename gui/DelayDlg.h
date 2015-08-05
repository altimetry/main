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

#ifndef __DelayDlg_H__
#define __DelayDlg_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "DelayDlg.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "BratGui_wdr.h"

#include "wxBratTools.h"
#include "wxDatePickerCtrl.h"
#include "TimeCtrl.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CDelayDlg
//----------------------------------------------------------------------------

class CDelayDlg: public wxDialog
{
public:
    // constructors and destructors
    CDelayDlg( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE );
    virtual ~CDelayDlg();
    
    // WDR: method declarations for CDelayDlg
    wxTextCtrl* GetTaskName()  { return (wxTextCtrl*) FindWindow( ID_TASK_NAME ); }
    CTimeCtrl* GetTimePicker()  { return (CTimeCtrl*) FindWindow( ID_TIME_PICKER ); }
    wxDatePickerCtrl* GetDatePicker()  { return (wxDatePickerCtrl*) FindWindow( ID_DATE_PICKER ); }
    
    void GetDateTime(wxDateTime& dateTime);

private:
    // WDR: member variable declarations for CDelayDlg
    
private:
    // WDR: handler declarations for CDelayDlg
    void OnOk( wxCommandEvent &event );
    void OnCancel( wxCommandEvent &event );

private:
    DECLARE_CLASS(CDelayDlg)
    DECLARE_EVENT_TABLE()
};




#endif
