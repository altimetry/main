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


#ifndef __BratGui_H__
#define __BratGui_H__

#include "new-gui/brat/stdafx.h"

// Include wxWindows' headers

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/config.h"  // (to let wxWidgets choose a wxConfig class for your platform)
#include "wx/confbase.h"  // (base config class)
#include "wx/fileconf.h" // (wxFileConfig class)
//#include "wx/msw/regconf.h" // (wxRegConfig class)

#include "dnd_copy.xpm"
#include "dnd_move.xpm"
#include "dnd_none.xpm"

#include "InternalFiles.h"

using namespace brathl;

//#include "BratGui_wdr.h"
#include "wxGuiInterface.h"		// (to let wxWidgets choose a wxConfig class for your platform)
#include "new-gui/brat/Workspaces/Workspace.h"
#include "new-gui/brat/Workspaces/TreeWorkspace.h"
#include "GuiFrame.h"


class CBratSettings;



#if defined(_WIN64) || defined(__LP64__) || defined(__x86_64__) 
	const wxString BRATGUI_TITLE_BASE = "Brat Interface (64 bit)";
#else
	const wxString BRATGUI_TITLE_BASE = "Brat Interface (32 bit)";
#endif
#if defined(DEBUG) || defined(_DEBUG)
	const wxString BRATGUI_TITLE = BRATGUI_TITLE_BASE + " [Debug]";
#else
	const wxString BRATGUI_TITLE = BRATGUI_TITLE_BASE;
#endif

const wxString BRAT_USER_MANUAL = wxString::Format("brat_user_manual_%s.pdf", BRAT_VERSION);
#ifdef __WXMAC__
const wxString BRAT_DOC_SUBDIR = "data";
#else
const wxString BRAT_DOC_SUBDIR = "doc";
#endif


const std::string DATASETS_PAGE_NAME = "Datasets";
const std::string OPERATIONS_PAGE_NAME = "Operations";
const std::string DISPLAY_PAGE_NAME = "Views";
const std::string LOG_PAGE_NAME = "Logs";

#ifdef WIN32
const std::string BRATHL_ICON_FILENAME = "BratIcon.ico";
//const wxString BRATCREATEYFX_EXE = "BratCreateYFX.exe";
//const wxString BRATCREATEZFXY_EXE = "BratCreateZFXY.exe";
//const wxString BRATDISPLAY_EXE = "BratDisplay.exe";
//const wxString BRATEXPORTASCII_EXE = "BratExportAscii.exe";
//const wxString BRATEXPORTGEOTIFF_EXE = "BratExportGeoTiff.exe";
//const wxString BRATSHOWSTATS_EXE = "BratStats.exe";
//const wxString BRATSCHEDULER_EXE = "BratScheduler.exe";
#else
const std::string BRATHL_ICON_FILENAME = "BratIcon.bmp";
//const wxString BRATCREATEYFX_EXE = "BratCreateYFX";
//const wxString BRATCREATEZFXY_EXE = "BratCreateZFXY";
//const wxString BRATDISPLAY_EXE = "BratDisplay";
//const wxString BRATEXPORTASCII_EXE = "BratExportAscii";
//const wxString BRATEXPORTGEOTIFF_EXE = "BratExportGeoTiff";
//const wxString BRATSHOWSTATS_EXE = "BratStats";
//const wxString BRATSCHEDULER_EXE = "BratScheduler";
#endif



inline void DetermineCharSize(wxWindow* wnd, int32_t& width, int32_t& height)
{
  wxStaticText w(wnd, -1, "W", wxDefaultPosition, wxSize(-1,-1));
  w.GetSize(&width, &height);
//  w.Destroy();
}

inline bool RemoveFile( const wxString& name )
{
	if ( !wxFileExists( name ) )
		return true;
	try
	{
		return wxRemoveFile( name );
	}
	catch ( ... )		//Nothing to do
	{}
	return false;
}

inline bool RenameFile( const wxString& oldName, const wxString& newName )
{
	if ( !wxFileExists( oldName ) )
		return true;
	try
	{
		return wxRenameFile( oldName, newName );
	}
	catch ( ... )	//Nothing to do
	{}
	return false;
}

inline void CStringArrayToWxArray( CStringArray& from, wxArrayString& to )
{
	for ( CStringArray::iterator it = from.begin(); it != from.end(); it++ )
		to.Add( ( *it ).c_str() );
}

inline void CStringListToWxArray( const CStringList& from, wxArrayString& to )
{
	for ( CStringList::const_iterator it = from.begin(); it != from.end(); it++ )
		to.Add( it->c_str() );
}
inline void CProductListToWxArray( CProductList& from, wxArrayString& to )
{
	for ( CProductList::iterator it = from.begin(); it != from.end(); it++ )
		to.Add( ( *it ).c_str() );
}
  
wxString FindFile( const wxString& fileName );

inline void GetRecordNames( CStringArray& array, CProduct* product )
{
	if ( product == NULL )
		return;

	product->GetRecords( array );
}

inline void GetRecordNames( wxArrayString& array, CProduct* product )
{
	CStringArray arrayTmp;
	GetRecordNames( arrayTmp, product );
	CStringArrayToWxArray( arrayTmp, array );
}

inline void GetRecordNames( wxComboBox& combo, CProduct* product )
{
	CStringArray array;
	GetRecordNames( array, product );
	for ( CStringArray::iterator it = array.begin(); it != array.end(); it++ )
		combo.Append( ( *it ).c_str() );
}

inline void GetRecordNames( wxListBox& listBox, CProduct* product )
{
	CStringArray array;
	GetRecordNames( array, product );
	for ( CStringArray::iterator it = array.begin(); it != array.end(); it++ )
		listBox.Append( ( *it ).c_str() );
}








// WDR: class declarations

//----------------------------------------------------------------------------
// BratApp
//----------------------------------------------------------------------------

class CBratGuiApp : public wxApp
{

	/////////////////////////////////////////////////
	//	data
	/////////////////////////////////////////////////
public:
	CTreeWorkspace m_tree;
	CTreeWorkspace m_treeImport;

private:
	CTreeWorkspace* m_currentTree = nullptr;
	wxFileConfig* m_config = nullptr;

	wxString m_userManualViewer;
	wxString m_userManual;
	wxString m_lastWksPath;
	wxString m_lastColorTable;
	wxString m_lastDataPath;
	wxString m_lastPageReached;

	CGuiFrame* m_frame = nullptr;

	wxFileName m_execName;

	/////////////////////////////////////////////////
	//	construction / destruction
	/////////////////////////////////////////////////
public:
	CBratGuiApp() : m_config( nullptr ), m_currentTree( nullptr )
	{}

	virtual bool OnInit();
	virtual int OnExit();

	/////////////////////////////////////////////////
	//	GUI guetters
	/////////////////////////////////////////////////
	static wxBitmapType GetIconType()
	{
		//return wxBITMAP_TYPE_BMP;
		//return wxBITMAP_TYPE_JPEG;
		//return wxBITMAP_TYPE_GIF;
#ifdef WIN32
		return wxBITMAP_TYPE_ICO;
#else
		return wxBITMAP_TYPE_BMP;
#endif
	}

	static wxString GetIconFileName()
	{
		return BRATHL_ICON_FILENAME.c_str();
	}
	static wxString GetIconFile()
	{
		return CTools::FindDataFile( (const char *)GetIconFileName().c_str() ).c_str();
	}

	wxNotebook* GetMainnotebook()  { return  m_frame->GetMainnotebook(); }
	CGuiFrame* GetGuiFrame()  { return  m_frame; }
	CGuiPanel* GetGuiPanel()  { return  m_frame->GetGuiPanel(); }
	CLogPanel* GetLogPanel()  { return  m_frame->GetGuiPanel()->GetLogPanel(); }
	//CDisplayPanel* GetDisplayPanel()  { return  m_frame->GetGuiPanel()->GetDisplayPanel(); }
	CDisplayPanel* GetDisplayPanel()  { return  m_frame->GetGuiPanel()->GetDisplayPanel(); }

	COperationPanel* GetOperationPanel()  { return  m_frame->GetGuiPanel()->GetOperationPanel(); }

	wxMenu* GetMenuFieldsTreeCtrl()  { return  m_frame->GetMenuFieldsTreeCtrl(); }
	wxMenu* GetMenuOperationTreeCtrl()  { return  m_frame->GetMenuOperationTreeCtrl(); }
	wxMenu* GetMenuDisplayTreeCtrl() { return m_frame->GetMenuDisplayTreeCtrl(); }


	wxFileName* GetExecName() { return &m_execName; }

	wxString GetExecPathName( int32_t flags = wxPATH_GET_VOLUME, wxPathFormat format = wxPATH_NATIVE )
#ifdef __WXMAC__
	{return (m_execName.IsRelative() ? 
	m_execName.GetPath(flags, format) + "/BratGui.app/Contents/MacOS" :
	m_execName.GetPath(flags, format));}
#else
	{return m_execName.GetPath( flags, format ); }
#endif


	/////////////////////////////////////////////////
	//	configuration
	/////////////////////////////////////////////////
private:
	wxFileConfig* GetConfig() { return m_config; }
    CBratSettings *mAppSettings = nullptr;

	bool SaveConfig( bool flush = true );
	bool SaveConfigSelectionCriteria( bool flush = true );

	bool LoadConfig();
	bool LoadConfigSelectionCriteria();

public:
	void LoadFileHistory( std::vector<std::string> &v );
	void SaveFileHistory( const std::vector<std::string> &v );

	wxString GetLastColorTable() { return m_lastColorTable; }
	void SetLastColorTable( const wxString& value ) { m_lastColorTable = value; }

	wxString BuildUserManualPath();
	void ViewUserManual();

	wxString GetUserManual() { return m_userManual; }
	void SetUserManual( const wxString& value ) { m_userManual = value; }

	wxString GetLastDataPath() { return m_lastDataPath; }
	void SetLastDataPath( const wxString& value ) { m_lastDataPath = value; }

	wxString GetLastPageReached() { return m_lastPageReached; }
	void SetLastPageReached( const wxString& value ) { m_lastPageReached = value; }

	/////////////////////////////////////////////////
	//	workspace
	/////////////////////////////////////////////////

private:
	CTreeWorkspace* GetCurrentTree() { return m_currentTree; }

public:
	void SetCurrentTree( CTreeWorkspace* value ) { m_currentTree = value; }

	template< class WKSPC >
	WKSPC* GetWorkspace()
	{
		CWorkspace* wks = GetCurrentWorkspace();
		if ( !wks )								   	//admissible when no workspace loaded
			return nullptr;

        std::string workspaceKey = m_currentTree->ChildKey( WKSPC::NAME );

		return dynamic_cast<WKSPC*>( m_currentTree->FindWorkspace( workspaceKey ) );
	}

	CWorkspace* GetCurrentWorkspace()
	{
		if ( m_currentTree == NULL )
		{
			m_currentTree = &m_tree;
		}
		return m_currentTree->GetRootData();
	}
	CWorkspaceDataset* GetCurrentWorkspaceDataset() { return GetWorkspace< CWorkspaceDataset >(); }
	CWorkspaceOperation* GetCurrentWorkspaceOperation() { return GetWorkspace< CWorkspaceOperation >(); }
	CWorkspaceFormula* GetCurrentWorkspaceFormula() { return GetWorkspace< CWorkspaceFormula >(); }
	CWorkspaceDisplay* GetCurrentWorkspaceDisplay() { return GetWorkspace< CWorkspaceDisplay >(); }

public:

	bool CanDeleteDataset( const wxString& name, CStringArray* operationNames = nullptr );
	bool CanDeleteOperation( const wxString& name, CStringArray* displayNames = nullptr );
	bool CanDeleteDisplay( const wxString& name );

	/////////////////////////////////////////////////
	//	workspace GUI
	/////////////////////////////////////////////////

	int32_t GotoLastPageReached();
	int32_t GotoPage( int32_t pos );
	int32_t GotoPage( const wxString& name );

	wxString GetCurrentPageText();

	bool IsCurrentLogPage()
	{
		return GetCurrentPageText().CmpNoCase( LOG_PAGE_NAME ) == 0;
	}

	int32_t GotoDatasetPage() { return GotoPage( DATASETS_PAGE_NAME ); }
	int32_t GotoOperationPage() { return GotoPage( OPERATIONS_PAGE_NAME ); }
	int32_t GotoDisplayPage() { return GotoPage( DISPLAY_PAGE_NAME ); }
	int32_t GotoLogPage() { return GotoPage( LOG_PAGE_NAME ); }

	bool IsNewViewEnable()
	{
		return 
			GetCurrentWorkspace() != NULL &&
			GetCurrentWorkspaceOperation() != NULL &&
			GetCurrentWorkspaceOperation()->HasOperation();
	}
	bool IsNewDatasetEnable()
	{
		return GetCurrentWorkspace() != NULL;
	}
	bool IsNewOperationEnable()
	{
		return 
			GetCurrentWorkspace() != NULL &&
			GetCurrentWorkspaceDataset() != NULL &&
			GetCurrentWorkspaceDataset()->HasDataset();
	}

	bool IsDeleteViewEnable()
	{
		return GetCurrentWorkspace() != NULL && GetGuiPanel()->GetDisplayPanel()->HasDisplay();
	}

	bool IsDeleteDatasetEnable()
	{
		return GetCurrentWorkspace() != NULL && GetGuiPanel()->GetDatasetPanel()->HasDataset();
	}

	bool IsDeleteOperationEnable()
	{
		return GetCurrentWorkspace() != NULL && GetGuiPanel()->GetOperationPanel()->HasOperation();
	}

	/////////////////////////////////////////////////
	//	
	/////////////////////////////////////////////////

	static void EvtFocus( wxWindow& window, int32_t eventType, const wxFocusEventFunction& method,
		wxObject* userData = nullptr, wxEvtHandler* eventSink = nullptr );
	static void EvtChar( wxWindow& window, int32_t eventType, const wxCharEventFunction& method,
		wxObject* userData = nullptr, wxEvtHandler* eventSink = nullptr );
};




DECLARE_APP(CBratGuiApp)



#endif
