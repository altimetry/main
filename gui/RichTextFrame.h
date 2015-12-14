/*
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


#ifndef __RichTextFrame_H__
#define __RichTextFrame_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/fdrepdlg.h>


// WDR: class declarations


class CRichTextFrame: public wxFrame
{
public:
    CRichTextFrame(wxWindow* parent, const wxString& title);

    wxTextCtrl* GetTextCtrl() {return m_textCtrl;};

    void ShowFindDialog( wxCommandEvent& event );

    static wxString DecodeFindDialogEventFlags(int flags);
// Event handlers

    void OnFindDialog(wxFindDialogEvent& event);
    void OnClose(wxCommandEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnLeftAlign(wxCommandEvent& event);
    void OnRightAlign(wxCommandEvent& event);
    void OnJustify(wxCommandEvent& event);
    void OnCentre(wxCommandEvent& event);
    void OnChangeFont(wxCommandEvent& event);
    void OnChangeTextColour(wxCommandEvent& event);
    void OnChangeBackgroundColour(wxCommandEvent& event);
    void OnLeftIndent(wxCommandEvent& event);
    void OnRightIndent(wxCommandEvent& event);
    void OnTabStops(wxCommandEvent& event);

private:
    wxTextCtrl *m_textCtrl;
    long m_currentPosition;

    wxFindReplaceData m_findData;
    wxFindReplaceDialog *m_dlgFind;

    DECLARE_EVENT_TABLE()
};





#endif
