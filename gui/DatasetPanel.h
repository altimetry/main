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

#ifndef __DatasetPanel_H__
#define __DatasetPanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "DatasetPanel.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "brathl.h"

#include "Product.h"
using namespace brathl;

#include "BratGui_wdr.h"

#include "Dictionary.h"
#include "Dataset.h"

class CDatasetPanel;

// WDR: class declarations
//----------------------------------------------------------------------------
// CDatasetDropTarget
//----------------------------------------------------------------------------
class CDatasetDropTarget : public wxFileDropTarget
{
public:
    CDatasetDropTarget(CDatasetPanel *pOwner);

    virtual bool OnDropFiles(wxCoord x, wxCoord y,
                             const wxArrayString& filenames);

private:
    //wxListBox *m_pOwner;
    CDatasetPanel *m_pOwner;
};

//----------------------------------------------------------------------------
// CDatasetPanel
//----------------------------------------------------------------------------

class CDatasetPanel: public wxSplitterWindow
{
public:
  // constructors and destructors
  CDatasetPanel( wxWindow *parent, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = 0 );
  virtual ~CDatasetPanel();

  void LoadDataset();
  void NewDataset();
  void DeleteDataset();

  void EnableCtrl();

  void ApplySelectionCriteria(CStringList& filesIn, CStringList& filesOut);

  void AddDir(wxArrayString& dirName);
  void AddDir(wxString& dirName);

  void AddFiles(wxArrayString& paths);

  bool CheckFiles();

  void ClearAll();

  void PostDatasetFilesChangeEvent();

  bool CheckForEmptyDataset();


  bool HasDataset() {return m_currentDataset >= 0;};

  void Reset();

  // WDR: method declarations for CDatasetPanel
    wxButton* GetDsdefinecrit()  { return (wxButton*) FindWindow( ID_DSDEFINECRIT ); }
    wxCheckBox* GetDsapplycrit()  { return (wxCheckBox*) FindWindow( ID_DSAPPLYCRIT ); }
  wxButton* GetDsdelete()  { return (wxButton*) FindWindow( ID_DSDELETE ); }
  wxButton* GetDsnew()  { return (wxButton*) FindWindow( ID_DSNEW ); }
  wxTextCtrl* GetFiledesc()  { return (wxTextCtrl*) FindWindow( ID_FILEDESC ); }
  wxStaticText* GetFiledescLabel()  { return (wxStaticText*) FindWindow( ID_FILEDESC_LABEL ); }
  wxButton* GetDsadddir()  { return (wxButton*) FindWindow( ID_DSADDDIR ); }
  wxTextCtrl* GetFielddesc()  { return (wxTextCtrl*) FindWindow( ID_FIELDDESC ); }
  CDictionaryList* GetDictlist()  { return (CDictionaryList*) FindWindow( ID_DICTLIST ); }
  wxStaticText* GetProductLabel()  { return (wxStaticText*) FindWindow( ID_PRODUCT_LABEL ); }
  wxButton* GetDscheck()  { return (wxButton*) FindWindow( ID_DSCHECK ); }
  wxButton* GetDsclear()  { return (wxButton*) FindWindow( ID_DSCLEAR ); }
  wxButton* GetDssort()  { return (wxButton*) FindWindow( ID_DSSORT ); }
  wxButton* GetDsdown()  { return (wxButton*) FindWindow( ID_DSDOWN ); }
  wxButton* GetDsup()  { return (wxButton*) FindWindow( ID_DSUP ); }
  wxButton* GetDsaddfiles()  { return (wxButton*) FindWindow( ID_DSADDFILES ); }
  wxComboBox* GetDsnames()  { return (wxComboBox*) FindWindow( ID_DSNAMES ); }
  wxListBox* GetDsfilelist()  { return (wxListBox*) FindWindow( ID_DSFILELIST ); }

    
private:
  bool m_dsNameDirty;
  int32_t m_currentDataset;
  int32_t m_currentFile;
  wxString m_currentFilename;

  CDataset* m_dataset;
  bool m_revertSortFile;

  CProduct* m_product;

  wxPanel* m_leftPanel;
  wxPanel* m_rightPanel;
  // WDR: member variable declarations for CDatasetPanel

  wxFileName m_logFileName;
    
private:

  void DsNameChanged();
 
  CDataset* GetCurrentDataset();
  bool SetCurrentDataset();

  void GetFiles( CStringArray& array );
  void GetFiles( wxArrayString& array );

  void FillFileList();

  void ClearDict();
  void FillDict();

  void DeleteProduct();

  void RemoveFileFromList(int32_t pos);

  //void ShowFieldDescription();

  // WDR: handler declarations for CDatasetPanel
    void OnShowSelectionReport( wxCommandEvent &event );
    void OnDefineCriteria( wxCommandEvent &event );
    void OnApplyCriteria( wxCommandEvent &event );
    void OnDeleteDataset( wxCommandEvent &event );
    void OnNewDataset( wxCommandEvent &event );
    void OnFileRemove( wxCommandEvent &event );
    void OnAddDir( wxCommandEvent &event );
  //void OnFieldDeselected( wxListEvent &event );
  //void OnFieldSelected( wxListEvent &event );
  void OnListBox( wxCommandEvent &event );
  void OnCheckFiles( wxCommandEvent &event );
  void OnFileClear( wxCommandEvent &event );
  void OnFileSort( wxCommandEvent &event );
  void OnFileDown( wxCommandEvent &event );
  void OnFileUp( wxCommandEvent &event );

  void OnListChar(wxKeyEvent &event );
  //void OnComboChar( CComboCharEventFunction &event );
  void OnComboChar( wxKeyEvent &event );
  //void OnComboKillFocus( CComboKillFocusEventFunction &event );
  void OnComboKillFocus( wxFocusEvent &event );
  void OnComboTextEnter( wxCommandEvent &event );
  void OnComboText( wxCommandEvent &event );
  void OnComboBox( wxCommandEvent &event );
  void OnAddFiles( wxCommandEvent &event );
  void OnWindowCreate(wxWindowCreateEvent& evt);

private:
    DECLARE_CLASS(CDatasetPanel)
    DECLARE_EVENT_TABLE()
};




#endif
