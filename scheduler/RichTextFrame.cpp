/*
* Copyright (C) 2005 Collecte Localisation Satellites (CLS), France (31)
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
    #pragma implementation "RichTextFrame.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif


#include "wx/colordlg.h"
#include "wx/fontdlg.h"
#include "wx/numdlg.h"
#include "wx/tokenzr.h"

#include "BratScheduler.h"

#include "RichTextFrame.h"

// WDR: class implementations

/*
 * CRichTextFrame is used to demonstrate rich text behaviour
 */

enum
{
    RICHTEXT_CLOSE = 1000,
    RICHTEXT_LEFT_ALIGN,
    RICHTEXT_RIGHT_ALIGN,
    RICHTEXT_CENTRE,
    RICHTEXT_JUSTIFY,
    RICHTEXT_CHANGE_FONT,
    RICHTEXT_CHANGE_TEXT_COLOUR,
    RICHTEXT_CHANGE_BACKGROUND_COLOUR,
    RICHTEXT_LEFT_INDENT,
    RICHTEXT_RIGHT_INDENT,
    RICHTEXT_TAB_STOPS,
    RICHTEXT_DIALOGS_FIND
};

BEGIN_EVENT_TABLE(CRichTextFrame, wxFrame)
    EVT_IDLE(CRichTextFrame::OnIdle)
    EVT_MENU(RICHTEXT_CLOSE, CRichTextFrame::OnClose)
    EVT_MENU(RICHTEXT_LEFT_ALIGN, CRichTextFrame::OnLeftAlign)
    EVT_MENU(RICHTEXT_RIGHT_ALIGN, CRichTextFrame::OnRightAlign)
    EVT_MENU(RICHTEXT_CENTRE, CRichTextFrame::OnCentre)
    EVT_MENU(RICHTEXT_JUSTIFY, CRichTextFrame::OnJustify)
    EVT_MENU(RICHTEXT_CHANGE_FONT, CRichTextFrame::OnChangeFont)
    EVT_MENU(RICHTEXT_CHANGE_TEXT_COLOUR, CRichTextFrame::OnChangeTextColour)
    EVT_MENU(RICHTEXT_CHANGE_BACKGROUND_COLOUR, CRichTextFrame::OnChangeBackgroundColour)
    EVT_MENU(RICHTEXT_LEFT_INDENT, CRichTextFrame::OnLeftIndent)
    EVT_MENU(RICHTEXT_RIGHT_INDENT, CRichTextFrame::OnRightIndent)
    EVT_MENU(RICHTEXT_TAB_STOPS, CRichTextFrame::OnTabStops)
    EVT_MENU(RICHTEXT_DIALOGS_FIND, CRichTextFrame::ShowFindDialog)

    EVT_FIND(wxID_ANY, CRichTextFrame::OnFindDialog)
    EVT_FIND_NEXT(wxID_ANY, CRichTextFrame::OnFindDialog)
    EVT_FIND_REPLACE(wxID_ANY, CRichTextFrame::OnFindDialog)
    EVT_FIND_REPLACE_ALL(wxID_ANY, CRichTextFrame::OnFindDialog)
    EVT_FIND_CLOSE(wxID_ANY, CRichTextFrame::OnFindDialog)
END_EVENT_TABLE()
//----------------------------------------

CRichTextFrame::CRichTextFrame(wxWindow* parent, const wxString& title):
    wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxSize(600, 400))
{
  m_dlgFind = NULL;

  m_currentPosition = -1;
  m_textCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
          wxDefaultSize, wxTE_MULTILINE|wxTE_RICH|wxTE_READONLY|wxHSCROLL);

  try
  {
    if (!wxGetApp().GetIconFile().IsEmpty())
    {
      SetIcon(wxIcon(wxGetApp().GetIconFile(), ::wxGetApp().GetIconType()));
    }
    else
    {
      ::wxMessageBox(wxString::Format("WARNING: Unable to find Brat icon file %s",
                                      ::wxGetApp().GetIconFileName().c_str())
                     , "BRAT WARNING");
    }
  }
  catch(...)
  {
    // Do nothing
  }

#ifdef WIN32
  m_lineEndingLength = 2;
#else
  m_lineEndingLength = 1;
#endif


  wxMenuBar* menuBar = new wxMenuBar;
  wxMenu* fileMenu = new wxMenu;
  fileMenu->Append(RICHTEXT_CLOSE, _("Close\tCtrl+W"));
  menuBar->Append(fileMenu, _("File"));

  wxMenu* editMenu = new wxMenu;
  editMenu->AppendCheckItem(RICHTEXT_DIALOGS_FIND, _T("&Find...\tCtrl-F"));
  editMenu->AppendSeparator();
  editMenu->Append(RICHTEXT_LEFT_ALIGN, _("Left Align"));
  editMenu->Append(RICHTEXT_RIGHT_ALIGN, _("Right Align"));
  editMenu->Append(RICHTEXT_CENTRE, _("Centre"));
  editMenu->Append(RICHTEXT_JUSTIFY, _("Justify"));
  editMenu->AppendSeparator();
  editMenu->Append(RICHTEXT_CHANGE_FONT, _("Change Font"));
  editMenu->Append(RICHTEXT_CHANGE_TEXT_COLOUR, _("Change Text Colour"));
  editMenu->Append(RICHTEXT_CHANGE_BACKGROUND_COLOUR, _("Change Background Colour"));
  editMenu->AppendSeparator();
  editMenu->Append(RICHTEXT_LEFT_INDENT, _("Left Indent"));
  editMenu->Append(RICHTEXT_RIGHT_INDENT, _("Right Indent"));
  editMenu->Append(RICHTEXT_TAB_STOPS, _("Tab Stops"));
  menuBar->Append(editMenu, _("Edit"));

  SetMenuBar(menuBar);
#if wxUSE_STATUSBAR
  CreateStatusBar();
#endif // wxUSE_STATUSBAR
}
//----------------------------------------

// Event handlers
//----------------------------------------

void CRichTextFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
  Close(true);
}
//----------------------------------------

void CRichTextFrame::OnLeftAlign(wxCommandEvent& WXUNUSED(event))
{
  wxTextAttr attr;
  attr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);

  long start, end;
  m_textCtrl->GetSelection(& start, & end);
  m_textCtrl->SetStyle(start, end, attr);

  m_currentPosition = -1;
}
//----------------------------------------

void CRichTextFrame::OnRightAlign(wxCommandEvent& WXUNUSED(event))
{
  wxTextAttr attr;
  attr.SetAlignment(wxTEXT_ALIGNMENT_RIGHT);

  long start, end;
  m_textCtrl->GetSelection(& start, & end);
  m_textCtrl->SetStyle(start, end, attr);

  m_currentPosition = -1;
}
//----------------------------------------

void CRichTextFrame::OnJustify(wxCommandEvent& WXUNUSED(event))
{
  wxTextAttr attr;
  attr.SetAlignment(wxTEXT_ALIGNMENT_JUSTIFIED);

  long start, end;
  m_textCtrl->GetSelection(& start, & end);
  m_textCtrl->SetStyle(start, end, attr);

  m_currentPosition = -1;
}
//----------------------------------------

void CRichTextFrame::OnCentre(wxCommandEvent& WXUNUSED(event))
{
  wxTextAttr attr;
  attr.SetAlignment(wxTEXT_ALIGNMENT_CENTRE);

  long start, end;
  m_textCtrl->GetSelection(& start, & end);
  m_textCtrl->SetStyle(start, end, attr);

  m_currentPosition = -1;
}
//----------------------------------------

void CRichTextFrame::OnChangeFont(wxCommandEvent& WXUNUSED(event))
{
  wxFontData data;

  wxFontDialog dialog(this, data);

  if (dialog.ShowModal() == wxID_OK)
  {
      wxFontData retData = dialog.GetFontData();
      wxFont font = retData.GetChosenFont();

      wxTextAttr attr;
      attr.SetFont(font);

      long start, end;
      m_textCtrl->GetSelection(& start, & end);
      m_textCtrl->SetStyle(start, end, attr);

      m_currentPosition = -1;
  }
}
//----------------------------------------

void CRichTextFrame::OnChangeTextColour(wxCommandEvent& WXUNUSED(event))
{
  wxColourData data;
  data.SetColour(* wxBLACK);
  data.SetChooseFull(true);
  for (int i = 0; i < 16; i++)
  {
      wxColour colour((unsigned char)(i*16), (unsigned char)(i*16), (unsigned char)(i*16));
      data.SetCustomColour(i, colour);
  }

  wxColourDialog dialog(this, &data);
  dialog.SetTitle(_T("Choose the text colour"));
  if (dialog.ShowModal() == wxID_OK)
  {
      wxColourData retData = dialog.GetColourData();
      wxColour col = retData.GetColour();

      wxTextAttr attr;
      attr.SetTextColour(col);

      long start, end;
      m_textCtrl->GetSelection(& start, & end);
      m_textCtrl->SetStyle(start, end, attr);

      m_currentPosition = -1;
  }
}
//----------------------------------------

void CRichTextFrame::OnChangeBackgroundColour(wxCommandEvent& WXUNUSED(event))
{
  wxColourData data;
  data.SetColour(* wxWHITE);
  data.SetChooseFull(true);
  for (int i = 0; i < 16; i++)
  {
      wxColour colour((unsigned char)(i*16), (unsigned char)(i*16), (unsigned char)(i*16));
      data.SetCustomColour(i, colour);
  }

  wxColourDialog dialog(this, &data);
  dialog.SetTitle(_T("Choose the text background colour"));
  if (dialog.ShowModal() == wxID_OK)
  {
      wxColourData retData = dialog.GetColourData();
      wxColour col = retData.GetColour();

      wxTextAttr attr;
      attr.SetBackgroundColour(col);

      long start, end;
      m_textCtrl->GetSelection(& start, & end);
      m_textCtrl->SetStyle(start, end, attr);

      m_currentPosition = -1;
  }
}
//----------------------------------------

void CRichTextFrame::OnLeftIndent(wxCommandEvent& WXUNUSED(event))
{
  wxString indentStr = wxGetTextFromUser
                       (
                          _("Please enter the left indent in tenths of a millimetre."),
                          _("Left Indent"),
                          wxEmptyString,
                          this
                       );
  if (!indentStr.IsEmpty())
  {
      int indent = wxAtoi(indentStr);

      wxTextAttr attr;
      attr.SetLeftIndent(indent);

      long start, end;
      m_textCtrl->GetSelection(& start, & end);
      m_textCtrl->SetStyle(start, end, attr);

      m_currentPosition = -1;
  }
}
//----------------------------------------

void CRichTextFrame::OnRightIndent(wxCommandEvent& WXUNUSED(event))
{
  wxString indentStr = wxGetTextFromUser
                       (
                          _("Please enter the right indent in tenths of a millimetre."),
                          _("Right Indent"),
                          wxEmptyString,
                          this
                       );
  if (!indentStr.IsEmpty())
  {
      int indent = wxAtoi(indentStr);

      wxTextAttr attr;
      attr.SetRightIndent(indent);

      long start, end;
      m_textCtrl->GetSelection(& start, & end);
      m_textCtrl->SetStyle(start, end, attr);

      m_currentPosition = -1;
  }
}
//----------------------------------------

void CRichTextFrame::OnTabStops(wxCommandEvent& WXUNUSED(event))
{
  wxString tabsStr = wxGetTextFromUser
                       (
                          _("Please enter the tab stop positions in tenths of a millimetre, separated by spaces.\nLeave empty to reset tab stops."),
                          _("Tab Stops"),
                          wxEmptyString,
                          this
                       );

  wxArrayInt tabs;

  wxStringTokenizer tokens(tabsStr, _T(" "));
  while (tokens.HasMoreTokens())
  {
      wxString token = tokens.GetNextToken();
      tabs.Add(wxAtoi(token));
  }

  wxTextAttr attr;
  attr.SetTabs(tabs);

  long start, end;
  m_textCtrl->GetSelection(& start, & end);
  m_textCtrl->SetStyle(start, end, attr);

  m_currentPosition = -1;
}
//----------------------------------------

void CRichTextFrame::OnIdle(wxIdleEvent& WXUNUSED(event))
{
  long insertionPoint = m_textCtrl->GetInsertionPoint();
  if (insertionPoint != m_currentPosition)
  {
#if wxUSE_STATUSBAR
      wxTextAttr attr;
      if (m_textCtrl->GetStyle(insertionPoint, attr))
      {
          wxString msg;
          wxString facename(wxT("unknown"));
          if (attr.GetFont().Ok())
          {
              facename = attr.GetFont().GetFaceName();
          }
          wxString alignment(wxT("unknown alignment"));
          if (attr.GetAlignment() == wxTEXT_ALIGNMENT_CENTRE)
              alignment = wxT("centred");
          else if (attr.GetAlignment() == wxTEXT_ALIGNMENT_RIGHT)
              alignment = wxT("right-aligned");
          else if (attr.GetAlignment() == wxTEXT_ALIGNMENT_LEFT)
              alignment = wxT("left-aligned");
          else if (attr.GetAlignment() == wxTEXT_ALIGNMENT_JUSTIFIED)
              alignment = wxT("justified");
          msg.Printf(wxT("Facename: %s, wxColour(%d, %d, %d), %s"),
              (const wxChar*) facename,
              attr.GetTextColour().Red(), attr.GetTextColour().Green(), attr.GetTextColour().Blue(),
              (const wxChar*) alignment);

          if (attr.HasFont())
          {
              if (attr.GetFont().GetWeight() == wxBOLD)
                  msg += wxT(" BOLD");
              else if (attr.GetFont().GetWeight() == wxNORMAL)
                  msg += wxT(" NORMAL");

              if (attr.GetFont().GetStyle() == wxITALIC)
                  msg += wxT(" ITALIC");

              if (attr.GetFont().GetUnderlined())
                  msg += wxT(" UNDERLINED");
          }

          SetStatusText(msg);
      }
#endif // wxUSE_STATUSBAR
      m_currentPosition = insertionPoint;
  }
}

//----------------------------------------

void CRichTextFrame::ShowFindDialog( wxCommandEvent& WXUNUSED(event) )
{
    if ( m_dlgFind )
    {
        delete m_dlgFind;
        m_dlgFind = NULL;
    }
    else
    {
        m_findData.SetFlags(m_findData.GetFlags() | wxFR_DOWN);
        m_dlgFind = new wxFindReplaceDialog
                        (
                            this,
                            &m_findData,
                            _T("Find..."),
                            // just for testing
                            wxFR_NOWHOLEWORD
                        );

        m_dlgFind->Show(true);
    }
}

//----------------------------------------
wxString CRichTextFrame::DecodeFindDialogEventFlags(int flags)
{
    wxString str;
    str << (flags & wxFR_DOWN ? _T("down") : _T("up")) << _T(", ")
        << (flags & wxFR_WHOLEWORD ? _T("whole words only, ") : _T(""))
        << (flags & wxFR_MATCHCASE ? _T("") : _T("not "))
        << _T("case sensitive");

    return str;
}
//----------------------------------------
void CRichTextFrame::OnFindDialog(wxFindDialogEvent& event)
{
    wxEventType type = event.GetEventType();

    if ( type == wxEVT_COMMAND_FIND || type == wxEVT_COMMAND_FIND_NEXT )
    {
      /*
        wxLogMessage(wxT("Find %s'%s' (flags: %s)"),
                     type == wxEVT_COMMAND_FIND_NEXT ? wxT("next ") : wxT(""),
                     event.GetFindString().c_str(),
                     DecodeFindDialogEventFlags(event.GetFlags()).c_str());
        */
        wxString sSearch = event.GetFindString();
        wxString sText = m_textCtrl->GetValue();
        //sText.Replace(wxT( "\n" ), wxT( " \n" ) ) ;
        int iStart = -1;
        long from = m_textCtrl->GetInsertionPoint();
        long to = 0;
        m_textCtrl->GetSelection(&from, &to);
        bool matchCase = ((event.GetFlags() & wxFR_MATCHCASE) == wxFR_MATCHCASE);
        bool down = ((event.GetFlags() & wxFR_DOWN) == wxFR_DOWN);
        if (matchCase)
        {
          if (down)
          {
            from = to + 1;
            iStart =  sText.find (sSearch, from);
          }
          else
          {
            from = from - 1;
            iStart =  sText.rfind (sSearch, from);
          }
        }
        else
        {
          if (down)
          {
            from = to + 1;
            iStart =  CTools::FindNoCase(sText.c_str(), sSearch.c_str(), from);
          }
          else
          {
            from = from - 1;
            iStart =  CTools::RFindNoCase(sText.c_str(), sSearch.c_str(), from);
          }
        }
        if (iStart < 0)
        {
          wxMessageBox(wxString::Format("Cannot find the string '%s'", 
                                        sSearch.c_str()),
                      "Find text...",
                      wxOK | wxICON_EXCLAMATION,
                      this);
          event.GetDialog()->SetFocus();
        }
        else
        {
          long x;
          long y;
          m_textCtrl->PositionToXY(iStart, &x, &y);
          iStart += ((m_lineEndingLength - 1) * y);
          int iEnd = iStart + sSearch.Length();
          m_textCtrl->SetFocus();
          m_textCtrl->SetInsertionPoint(iEnd+1);
          m_textCtrl->SetSelection (iStart, iEnd);

        }

    }
    else if ( type == wxEVT_COMMAND_FIND_CLOSE )
    {
        wxFindReplaceDialog *dlg = event.GetDialog();

        if ( dlg == m_dlgFind )
        {
            m_dlgFind = NULL;
        }

        dlg->Destroy();
    }
    else
    {
        //wxLogError(wxT("Unknown find dialog event!"));
    }
}
