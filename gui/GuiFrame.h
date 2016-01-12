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


#ifndef __GuiFrame_H__
#define __GuiFrame_H__

// Include wxWindows' headers

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/docview.h"

#include "BratGui_wdr.h"

#include "GuiPanel.h"
#include "FieldsTreeCtrl.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CGuiFrame
//----------------------------------------------------------------------------

class CGuiFrame : public wxFrame
{
public:
	// constructors and destructors
	CGuiFrame( wxWindow *parent, wxWindowID id, const wxString &title,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize( 1024, 600 ),
		long style = wxDEFAULT_FRAME_STYLE );

	~CGuiFrame();

	// WDR: method declarations for CGuiFrame
	CDisplayDataTreeCtrl* GetDispavailtreectrl()  { return (CDisplayDataTreeCtrl*)FindWindow( ID_DISPAVAILTREECTRL ); }
	COperationTreeCtrl* GetOperationtreectrl()  { return (COperationTreeCtrl*)FindWindow( ID_OPERATIONTREECTRL ); }
	CFieldsTreeCtrl* GetFieldstreectrl()  { return (CFieldsTreeCtrl*)FindWindow( ID_FIELDSTREECTRL ); }
	wxNotebook* GetMainnotebook()  { return (wxNotebook*)FindWindow( ID_MAINNOTEBOOK ); }

	CWorkspace* CreateTree( const std::string& name, const std::string& path );	//taken from BratGui, with changes
	CWorkspace* LoadTree( const std::string& path );	//taken from BratGui CreateTree, with changes
	//bool CreateWorkspace();
	bool SaveWorkspace();
	//bool OpenWorkspace();
	bool LoadWorkspace( const std::string& path );
	bool OpenWorkspace( const wxString& path, bool askUser = false );
	bool ImportWorkspace();
	bool RenameWorkspace();
	bool DeleteWorkspace();

	void LoadWorkspace();

	void DoEmptyWorkspace();
	void EnableCtrlWorkspace();
	void ResetWorkspace();

	void SetTitle();

	CGuiPanel* GetGuiPanel() { return m_guiPanel; };

	wxMenu* GetMenuFieldsTreeCtrl() { return m_menuFieldsTreeCtrl; };
	wxMenu* GetMenuOperationTreeCtrl() { return m_menuOperationTreeCtrl; };
	wxMenu* GetMenuDisplayTreeCtrl() { return m_menuDisplayTreeCtrl; };



private:
	// WDR: method declarations for CGuiFrame
	void CreateMainMenuBar();
	void CreatePopupMenuBar();
	void CreateMainToolBar();
	void ConfigToFileHistory();
	void AddWorkspaceToHistory( const wxString& name );
	void RemoveWorkspaceFromHistory( const wxString& name );
	void LoadFromWorkspaceHistory( int32_t fileNum );
	void FileHistoryToConfig();
	bool AskToSave();

	void DoSortFieldCtrl( bool reverse = false );
	void DoSortOpCtrl( bool reverse = false );
	void DoSortDispTreeCtrl( bool reverse = false );



private:
	// WDR: member variable declarations for CGuiFrame
	CGuiPanel* m_guiPanel;
	wxFileHistory m_wksHistory;

	wxMenuBar* m_menuBar;
	wxMenu* m_menuWorkspace;
	wxMenu* m_menuHistory;

	wxMenuBar* m_menuBarPopup;
	wxMenu* m_menuFieldsTreeCtrl;
	wxMenu* m_menuOperationTreeCtrl;
	wxMenu* m_menuDisplayTreeCtrl;


private:
	// WDR: handler declarations for CGuiFrame
	void OnFieldAttrChangeUnit( wxCommandEvent &event );
	void OnCollapseAvailabledispDataTree( wxCommandEvent &event );
	void OnRenameExpression( wxCommandEvent &event );
	void OnExpandAvailabledispDataTree( wxCommandEvent &event );
	void OnAddToSelected( wxCommandEvent &event );
	void OnDeleteExpression( wxCommandEvent &event );
	void OnSaveAsFormula( wxCommandEvent &event );
	void OnInsertFormula( wxCommandEvent &event );
	void OnInsertFunction( wxCommandEvent &event );
	void OnInsertField( wxCommandEvent &event );
	void OnInsertEmptyExpression( wxCommandEvent &event );
	void OnInsertIntoSelect( wxCommandEvent &event );
	void OnSetAsData( wxCommandEvent &event );
	void OnSetAsY( wxCommandEvent &event );
	void OnSetAsX( wxCommandEvent &event );
	void OnSortRevDispTreeCtrl( wxCommandEvent &event );
	void OnSortDispTreeCtrl( wxCommandEvent &event );
	void OnSortRevOpCtrl( wxCommandEvent &event );
	void OnSortOpCtrl( wxCommandEvent &event );
	void OnSortRevFieldCtrl( wxCommandEvent &event );
	void OnSortFieldCtrl( wxCommandEvent &event );
	void OnUserManual( wxCommandEvent &event );
	void OnDuplicateOperationUpdateUI( wxUpdateUIEvent &event );
	void OnDuplicateOperation( wxCommandEvent &event );
	void OnDeleteViewUpdateUI( wxUpdateUIEvent &event );
	void OnDeleteView( wxCommandEvent &event );
	void OnNewViewUpdateUI( wxUpdateUIEvent &event );
	void OnNewView( wxCommandEvent &event );
	void OnDeleteOperationUpdateUI( wxUpdateUIEvent &event );
	void OnDeleteOperation( wxCommandEvent &event );
	void OnNewOperationUpdateUI( wxUpdateUIEvent &event );
	void OnNewOperation( wxCommandEvent &event );
	void OnDeleteDatasetUpdateUI( wxUpdateUIEvent &event );
	void OnDeleteDataset( wxCommandEvent &event );
	void OnNewDatasetUpdateUI( wxUpdateUIEvent &event );
	void OnNewDataset( wxCommandEvent &event );
	void OnImportWorkspaceUpdateUI( wxUpdateUIEvent &event );
	void OnSaveWorkspaceUpdateUI( wxUpdateUIEvent &event );
	void OnSaveWorkspace( wxCommandEvent &event );
	void OnDeleteWorkspaceUpdateUI( wxUpdateUIEvent &event );
	void OnRenameWorkspaceUpdateUI( wxUpdateUIEvent &event );
	void OnDeleteWorkspace( wxCommandEvent &event );
	void OnRenameWorkspace( wxCommandEvent &event );
	void OnImportWorkspace( wxCommandEvent &event );
	void OnOpenWorkspace( wxCommandEvent &event );
	void OnNewWorkspace( wxCommandEvent &event );
	void OnAbout( wxCommandEvent &event );
	void OnQuit( wxCommandEvent &event );
	void OnCloseWindow( wxCloseEvent &event );
	void OnSize( wxSizeEvent &event );
	void OnUpdateUI( wxUpdateUIEvent &event );
	void OnFileHistory( wxCommandEvent &event );

private:
	DECLARE_EVENT_TABLE()
};


#endif
