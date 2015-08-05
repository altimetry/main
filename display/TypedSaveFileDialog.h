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


#ifndef __TypedSaveFileDialog_H__
#define __TypedSaveFileDialog_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "TypedSaveFileDialog.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "brathl.h"

#include "List.h"
using namespace brathl;

#include "wx/filename.h"

//#include "BratDisplay_wdr.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CTypedSaveFileDialog
//----------------------------------------------------------------------------

class CTypedSaveFileDialog : public wxDialog
{
public:
  // constructors and destructors
  CTypedSaveFileDialog();
  CTypedSaveFileDialog( wxWindow *parent, const wxString& initialDir, const CStringMap& info, 
                        wxWindowID id = -1, const wxString &title = "Save...",
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxDEFAULT_DIALOG_STYLE );

  virtual ~CTypedSaveFileDialog();
  
  bool Create( wxWindow *parent, const wxString& initialDir, const CStringMap& info, 
               wxWindowID id = -1, const wxString &title = "Save...",
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxDEFAULT_DIALOG_STYLE );

  // WDR: method declarations for CTypedSaveFileDialog

public:
  wxFileName m_fileName;
  //string m_ext;

private:

  long m_ID_LOCFIELD;
  long m_ID_BROWSE;
  long m_ID_TYPECHOICE;
  
  CStringMap m_info;

  wxFileName m_currentDir;

  wxStaticText* m_locLabel;
  wxTextCtrl* m_locField;
  wxButton* m_browseButton;
  wxStaticText* m_typeLabel;
  wxChoice* m_typeChoice;
  wxButton* m_cancelButton;
  wxButton* m_saveButton;
  wxBoxSizer* m_hSizer1;
  wxBoxSizer* m_hSizer2;
  wxBoxSizer* m_hSizer3;
  wxBoxSizer* m_vSizer;


  // WDR: member variable declarations for CTypedSaveFileDialog
    
private:
  void Init();
  
  bool CreateControls();
  void CreateLayout();
  void InstallToolTips();
  void InstallEventListeners();
  void InfoToArray(wxArrayString& array);
  bool CtrlExt();

  // WDR: handler declarations for CTypedSaveFileDialog
  void OnCloseWindow( wxCloseEvent &event );
  void OnTypeChoice( wxCommandEvent &event );
  void OnLocFieldChar( wxCommandEvent &event );
  void OnLocFieldEnter( wxCommandEvent &event );
  void OnCancel( wxCommandEvent &event );
  void OnSaveEvent( wxCommandEvent &event );
  void OnSave();
  void OnBrowse( wxCommandEvent &event );


private:
  DECLARE_CLASS(CTypedSaveFileDialog)
  DECLARE_EVENT_TABLE()
};




#endif
