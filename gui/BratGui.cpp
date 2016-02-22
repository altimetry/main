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
#include "new-gui/brat/stdafx.h"
#include "new-gui/Common/ConfigurationKeywords.h"
#include "new-gui/Common/+UtilsIO.h"
#include "new-gui/brat/BratSettings.h"
#include "display/wxInterface.h"

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// Include private headers

#include <locale.h>

#include "wx/mimetype.h"

//#ifdef WIN32
//  #include <crtdbg.h>
//#endif


#include "new-gui/Common/tools/Trace.h"
#include "List.h"
#include "Tools.h"
#include "new-gui/Common/tools/Exception.h"
#include "new-gui/brat/DataModels/MapTypeDisp.h"
#include "InternalFilesFactory.h"
#include "InternalFiles.h"
#include "InternalFilesYFX.h"
#include "InternalFilesZFXY.h"
#include "Product.h"
#include "BratAlgorithmBase.h"
using namespace brathl;

#include "BratGui.h"
#include "DirTraverser.h"

//#ifdef WIN32
//#include <crtdbg.h>
//#endif



wxString FindFile( const wxString& fileName )
{
	wxString fileNameFound;

	CDirTraverserExistFile traverserExistFile( fileName );

	wxFileName path;
	if ( wxGetApp().GetExecPathName().IsEmpty() )
	{
		wxString fileTmp = wxFileSelector( wxString::Format( "Where is the file %s ?", fileName.c_str() ),
			wxGetCwd(),
			fileName );
		if ( fileTmp.IsEmpty() )
		{
			return "";
		}
		return fileTmp;
	}

	path.AssignDir( wxGetApp().GetExecPathName() );

	wxArrayString dirs = path.GetDirs();

	for ( int32_t i = dirs.GetCount() - 1; i > 0; i-- )
		path.RemoveDir( i );

	wxDir dir;

	dir.Open( path.GetFullPath() );
	dir.Traverse( traverserExistFile );

	if ( traverserExistFile.m_found )
	{
		fileNameFound = traverserExistFile.m_fileName.GetFullPath();
	}
	return fileNameFound;
}



// WDR: class implementations


//------------------------------------------------------------------------------
// MainApp
//------------------------------------------------------------------------------

IMPLEMENT_APP(CBratGuiApp)


#define NEW_CONFIGURATION_STUFF


CBratGuiApp::CBratGuiApp():
	  m_config( nullptr )
	, m_currentTree( nullptr )
{}

//----------------------------------------
bool CBratGuiApp::OnInit()
{
#ifdef WIN32
  #ifdef _DEBUG
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
  #endif
#endif

#if wxUSE_UNICODE
    mBratPaths = new CApplicationPaths( wxGetApp().argv[0].ToStdString().c_str() );
#else
    mBratPaths = new CApplicationPaths( wxGetApp().argv[0] );
#endif

  // Register Brat algorithms
  CBratAlgorithmBase::RegisterAlgorithms();

  bool bOk = true;


  // To be sure that number have always a decimal point (and not a comma
  // or something else)
  setlocale(LC_NUMERIC, "C");

  m_execName.Assign(wxGetApp().argv[0]);

  //wxString appPath = GetExecPathName();

  CTools::SetInternalDataDir/*ForExecutable*/(mBratPaths->mInternalDataDir/*wxGetApp().argv[0]*/);


  //wxFileName traceFileName;
  //traceFileName.AssignDir(wxGetApp().GetExecPathName());
  //traceFileName.SetFullName("GuiTrace.log");
  //traceFileName.Normalize();
  //CTrace::GetInstance(traceFileName.GetFullPath().c_str());

  //auto_ptr<CTrace>pTrace(CTrace::GetInstance(argc, argv));
  //pTrace->SetTraceLevel(5);

  COperation::SetExecNames( *mBratPaths );
  CDisplayPanel::SetExecDisplayName( mBratPaths->mExecutableDir + "/" + setExecExtension( "BratDisplay" ) );

      //CTools::SetInternalDataDir/*ForExecutable*/(mBratPaths.mInternalDataDir/*wxGetApp().argv[0]*/);

  //CTools::SetDataDir( GetDirectoryFromPath( wxGetApp().argv[0] ) + "/data" );
  //if (!CTools::DirectoryExists(CTools::GetDataDir()))
  //{
  //    std::cerr << "ERROR: " << CTools::GetDataDir() << " is not a valid directory" << std::endl;
  //    ::wxMessageBox(wxString(CTools::GetDataDir().c_str()) + " is not a valid directory -- BRAT cannot continue. \n\nAre you sure your " + BRATHL_ENVVAR + " environment variable is set correctly?", "BRAT ERROR");
  //    return false;
  //}


  std::string errorMsg;
  if (!CTools::LoadAndCheckUdUnitsSystem(errorMsg))
  {
      std::cerr << errorMsg << std::endl;
      ::wxMessageBox(errorMsg.c_str(), "BRAT ERROR");
      return false;
  }


  // Load aliases dictionnary
  CAliasesDictionary::LoadAliasesDictionary(&errorMsg, false);
  if (!(errorMsg.empty())) 
  {
    std::cerr << "WARNING: " << errorMsg << std::endl;
    wxMessageBox(errorMsg.c_str(),
		 "Warning",
		 wxOK | wxCENTRE | wxICON_EXCLAMATION);
  }

  CProduct::CodaInit();
  //CProduct::SetCodaReleaseWhenDestroy(false);

  try {
	  m_config = new wxFileConfig( wxGetApp().GetAppName(), wxEmptyString, wxEmptyString, wxEmptyString, wxCONFIG_USE_LOCAL_FILE );

	  LoadConfig();
  }
  catch ( CException &e ) {
	  wxMessageBox( wxString::Format( "An error occurred while loading Brat configuration (CBratGui::LoadConfig)\nNative std::exception: %s", e.what() ),
		  "Warning",
		  wxOK | wxCENTRE | wxICON_EXCLAMATION );
  }


  m_frame = new CGuiFrame( *mBratPaths, NULL, -1, BRATGUI_TITLE );

  m_frame->Show( TRUE );

  //if (m_lastWksPath.IsEmpty() == false)
//  {
  try {

    bOk = m_frame->OpenWorkspace(m_lastWksPath);
    if (bOk == false)
    {
      wxMessageBox(wxString::Format("Unable to load last used workspace located in '%s' ",
                                    m_lastWksPath.c_str()),
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);

    }

  }
  catch(CException &e) {
    wxMessageBox(wxString(e.what()),
		 "Warning",
		 wxOK | wxCENTRE | wxICON_EXCLAMATION);
  }

    m_frame->SetTitle();
  //}

   GotoLastPageReached();

  return true;
}

//----------------------------------------
int CBratGuiApp::OnExit()
{
	assert__( m_config == NULL || mAppSettings == NULL );

	if ( m_config != NULL || mAppSettings != NULL )
	{
		SaveConfig();
		delete m_config;
		delete mAppSettings;
		m_config = NULL;
		mAppSettings = NULL;
	}

	m_tree.Clear();

	CProduct::CodaRelease();

	return 0;
}
//----------------------------------------
int32_t CBratGuiApp::GotoPage(int32_t pos)
{
  return GetMainnotebook()->SetSelection(pos);
}
//----------------------------------------
int32_t CBratGuiApp::GotoLastPageReached()
{
  if (m_lastPageReached.IsEmpty())
  {
    return -1;
  }

  return GotoPage(m_lastPageReached);
}
//----------------------------------------
int32_t CBratGuiApp::GotoPage(const wxString& name)
{
  for (uint32_t i = 0 ; i < GetMainnotebook()->GetPageCount() ; i++)
  {
    if (GetMainnotebook()->GetPageText(i).CmpNoCase(name) == 0)
    {
      return GotoPage(i);
    }
  }

  return -1;
}
//----------------------------------------
wxString CBratGuiApp::GetCurrentPageText()
{
  int32_t idx = GetMainnotebook()->GetSelection();

  wxString result;

  if ( idx == wxNOT_FOUND)
  {
    return result;
  }


  return GetMainnotebook()->GetPageText(idx);
}
//----------------------------------------
bool CBratGuiApp::CanDeleteOperation( const wxString& name, CStringArray* displayNames /*= NULL*/ )
{
	bool canDelete = true;
	CWorkspaceDisplay* wks = GetCurrentWorkspaceDisplay();
	if ( wks == NULL )
	{
		return true;
	}

	std::string errorMsg;
	canDelete &= ( wks->UseOperation( name.ToStdString(), errorMsg, displayNames ) == false );
	if ( !errorMsg.empty() )
		wxMessageBox( errorMsg, "Error", wxOK | wxCENTRE | wxICON_ERROR );

	return canDelete;
}
//----------------------------------------
bool CBratGuiApp::CanDeleteDisplay(const wxString& name)
{
    UNUSED(name);

  bool canDelete = true;

  return canDelete;
}
//----------------------------------------
bool CBratGuiApp::CanDeleteDataset( const wxString& name, CStringArray* operationNames /*= NULL*/ )
{
	bool canDelete = true;
	CWorkspaceOperation* wks = GetCurrentWorkspaceOperation();
	if ( wks == NULL )
	{
		return true;
	}

	std::string errorMsg;
	canDelete &= ( wks->UseDataset( name.ToStdString(), errorMsg, operationNames ) == false );
	if ( !errorMsg.empty() )
		wxMessageBox( errorMsg, "Error", wxOK | wxCENTRE | wxICON_ERROR );

	return canDelete;
}
//----------------------------------------
wxString CBratGuiApp::BuildUserManualPath()
{
  wxFileName fileName;

  fileName.AssignDir(wxGetApp().GetExecPathName());
  fileName.AppendDir("..");
  fileName.AppendDir(BRAT_DOC_SUBDIR);
  fileName.SetFullName(BRAT_USER_MANUAL);
  fileName.Normalize();

  return fileName.GetFullPath();
}
//----------------------------------------
void CBratGuiApp::ViewUserManual()
{
  wxFileName fileName;

  if (m_userManual.IsEmpty())
  {
    //m_userManual = CBratGuiApp::FindFile(BRAT_USER_MANUAL);
    m_userManual = BuildUserManualPath();
  }
  else
  {
    fileName.Assign(m_userManual);
    wxString fullName = fileName.GetFullName();

    bool versionIsOk= (fullName.Cmp(BRAT_USER_MANUAL) == 0);

    if (!versionIsOk)
    {
      m_userManual = BuildUserManualPath();
    }
  }

  if (m_userManual.IsEmpty() || (!wxFileExists(m_userManual)))
  {
      m_userManual = wxFileSelector(wxString::Format("Where is the file %s ?",  BRAT_USER_MANUAL.c_str()),
                                      wxGetCwd(),
                                      BRAT_USER_MANUAL);
  }

  if (m_userManual.IsEmpty())
  {
    wxMessageBox(wxString::Format("User's manual '%s' is not found. Not path to the file has been selected.",
                                  BRAT_USER_MANUAL.c_str()),
                 "Information",
                  wxOK | wxCENTRE | wxICON_INFORMATION);
    return;

  }
  if (wxFileExists(m_userManual) == false)
  {
    wxMessageBox(wxString::Format("User's manual '%s' doesn't exist.",
                                  m_userManual.c_str()),
                 "Warning",
                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
    return;

  }

  wxString ext = m_userManual.AfterLast(_T('.'));
  wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
  if ( ft == NULL )
  {
    //wxMessageBox(wxString::Format("There is no program associated with '%s' file. "
    //                              "Brat can't open the User Manual from '%s'. "
    //                              "Check your system configuration and/or try to open the User Manuel manually (outside Brat)",
    //                              ext.c_str(),
    //                              m_userManual.c_str()),
    //             "Warning",
    //              wxOK | wxCENTRE | wxICON_EXCLAMATION);

    if (m_userManualViewer.IsEmpty())
    {
      wxMessageBox(wxString::Format("No default '%s' viewer found."
                                       "Please click OK and then select an executable program to view '%s'",
                                       ext.c_str(),
                                       m_userManual.c_str()),
                    "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);

      wxFileName currentDir;
      currentDir.AssignDir(".");
      currentDir.Normalize();

      wxString wildcards = wxString::Format(_T("All files (%s)|%s"),
                                          wxFileSelectorDefaultWildcardStr,
                                          wxFileSelectorDefaultWildcardStr
                                          );

      wxFileDialog execDialog(NULL, wxString::Format("Choose a '%s' viewer executable", ext.c_str()),
                              currentDir.GetPath(), wxEmptyString, wildcards, wxFD_CHANGE_DIR|wxFD_FILE_MUST_EXIST);

      if (execDialog.ShowModal() == wxID_OK) 
      {
        m_userManualViewer = execDialog.GetPath();
      }
    }
    
    if (!(m_userManualViewer.IsEmpty()))
    {
      wxString cmd = m_userManualViewer;
      cmd.Append(" ");
      cmd.Append(m_userManual);
      ::wxExecute(cmd);
    }

    return;
  }

  wxString desc;
  ft->GetDescription(&desc);

  wxString cmd;
  bool bOk = ft->GetOpenCommand(&cmd,
                               wxFileType::MessageParameters(m_userManual, wxEmptyString) );
  delete ft;
  ft = NULL;

  if (bOk == false)
  {
    wxMessageBox(wxString::Format("No command available (%s) to open '%s'",
                                  desc.c_str(),
                                  m_userManual.c_str()),
                 "Warning",
                  wxOK | wxCENTRE | wxICON_EXCLAMATION);

    return;
  }
  
  ::wxExecute(cmd);

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CONFIGURATION
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------
bool CBratGuiApp::SaveConfig( bool flush )
{

#if defined( NEW_CONFIGURATION_STUFF)

	assert__( !m_config && mAppSettings );

  //m_lastWksPath.Empty();

	CWorkspace* wks = GetCurrentWorkspace();
	if ( wks != NULL )
		m_lastWksPath = wks->GetPath();
    m_lastWksPath = wks != nullptr ? wks->GetPath() : "";

	mAppSettings->m_userManual = m_userManual;
	mAppSettings->m_userManualViewer = m_userManualViewer;
	mAppSettings->m_lastDataPath = m_lastDataPath;
	mAppSettings->m_lastPageReached = m_lastPageReached;
	mAppSettings->m_lastWksPath = m_lastWksPath;
	mAppSettings->m_lastColorTable = m_lastColorTable;

    bool result = mAppSettings->SaveConfig();

	if (flush)
		mAppSettings->Sync();

	return result;

#else

	assert__( m_config && !mAppSettings );

  bool bOk = true;
  //if (m_config == NULL)
  //{
  //  return bOk;
  //}

  ////------------------------------------
  m_config->SetPath("/" + GROUP_COMMON);
  ////------------------------------------

  bOk &= m_config->Write(ENTRY_USER_MANUAL, m_userManual);
  if (!m_userManualViewer.IsEmpty())
  {
    bOk &= m_config->Write(ENTRY_USER_MANUAL_VIEWER, m_userManualViewer);
  }
  bOk &= m_config->Write(ENTRY_LAST_DATA_PATH, m_lastDataPath);
  bOk &= m_config->Write(ENTRY_LAST_PAGE_REACHED, m_lastPageReached);
  

  ////------------------------------------
  m_config->SetPath("/" + GROUP_WKS);
  ////------------------------------------
  bOk &= m_config->Write(ENTRY_LAST, m_lastWksPath);

  if (m_lastColorTable.IsEmpty() == false)
  {
    //------------------------------------
    m_config->SetPath("/" + GROUP_COLORTABLE);
    //------------------------------------
    bOk &= m_config->Write(ENTRY_LAST, m_lastColorTable);
  }

  bOk &= SaveConfigSelectionCriteria(false);

  if (flush)
  {
    m_config->Flush();
  }


  return bOk;

#endif
}

void CBratGuiApp::SaveFileHistory( const std::vector<std::string> &v )
{
#if defined( NEW_CONFIGURATION_STUFF )
	assert__( !m_config && mAppSettings );

	mAppSettings->SaveRecentFiles( v );

	mAppSettings->Sync();

#else
	assert__( m_config && !mAppSettings );

	auto brathlFmtEntryRecentWksMacro = []( int index ) ->std::string
	{
		return GROUP_WKS_RECENT + "/" + KEY_WKS_RECENT + n2s<std::string>( index );
	};

	m_config->SetPath( "/" );

	const size_t size = v.size();
	for ( size_t i = 0; i < size; ++i )
	{
		m_config->Write( brathlFmtEntryRecentWksMacro( i ), v[ i ].c_str() );
	}

	m_config->Flush();
#endif

}

//----------------------------------------
bool CBratGuiApp::SaveConfigSelectionCriteria(bool flush)
{
	assert__( m_config && !mAppSettings );

    UNUSED(flush);

  bool bOk = true;
  //if (m_config == NULL)
  //{
  //  return bOk;
  //}

	assert__( m_config );

  wxString configPath;
  CMapProduct& mapProductInstance = CMapProduct::GetInstance();

  CObMap::iterator it;
  for ( it = mapProductInstance.begin(); it != mapProductInstance.end() ; it++ )
  {
    CProduct* product = dynamic_cast<CProduct*>(it->second);
    if (product == NULL)
    {
      continue;
    }
    if (!product->HasCriteriaInfo())
    {
      continue;
    }

    //configPath.Empty();
    //configPath.Append("/");
    //configPath.Append(GROUP_SEL_CRITERIA);
    //configPath.Append(product->GetLabel().c_str());

	configPath = BuildComposedKey( { "/", GROUP_SEL_CRITERIA, product->GetLabel() } ).c_str();

    wxString value;

    //------------------------------------
    m_config->SetPath(configPath);
    //------------------------------------

    if (product->IsSetLatLonCriteria())
    {
      value = product->GetLatLonCriteria()->GetAsText().c_str();
      bOk &= m_config->Write(ENTRY_LAT_LON, value);
    }
    if (product->IsSetDatetimeCriteria())
    {
      value = product->GetDatetimeCriteria()->GetAsText().c_str();
      bOk &= m_config->Write(ENTRY_DATETIME, value);
    }
    if (product->IsSetCycleCriteria())
    {
      value = product->GetCycleCriteria()->GetAsText().c_str();
      bOk &= m_config->Write(ENTRY_CYCLE, value);
    }
    if (product->IsSetPassIntCriteria())
    {
      value = product->GetPassIntCriteria()->GetAsText().c_str();
      bOk &= m_config->Write(ENTRY_PASS_NUMBER, value);
    }
    if (product->IsSetPassStringCriteria())
    {
      value = product->GetPassStringCriteria()->GetAsText().c_str();
      bOk &= m_config->Write(ENTRY_PASS_STRING, value);
    }
  }

  return bOk;

}
//----------------------------------------
bool CBratGuiApp::LoadConfig()
{
	assert__( m_config && !mAppSettings );

#if wxUSE_UNICODE
    static CApplicationPaths brat_paths( wxGetApp().argv[0].ToStdString().c_str() );
#else
    static CApplicationPaths brat_paths( wxGetApp().argv[0] );
#endif
    mAppSettings = new CBratSettings( brat_paths, m_config->GetLocalFile( wxGetApp().GetAppName() ).GetFullPath().ToStdString() );

#if defined(NEW_CONFIGURATION_STUFF)

	delete m_config;
	m_config = nullptr;
	if ( !mAppSettings->LoadConfig() )
		throw CException( "Unable to create new configuration type", BRATHL_LOGIC_ERROR );

	m_userManual = mAppSettings->m_userManual;
	m_userManualViewer = mAppSettings->m_userManualViewer;
	m_lastDataPath = mAppSettings->m_lastDataPath;
	m_lastPageReached = mAppSettings->m_lastPageReached;

	m_lastWksPath = mAppSettings->m_lastWksPath;

	m_lastColorTable = mAppSettings->m_lastColorTable;

	return true;

#else
	delete mAppSettings;
	mAppSettings = nullptr;
	if ( !bOk )
		return false;

	//if ( m_config == NULL )		//???femm
	//	return bOk;

	assert__( m_config );

	m_config->SetPath( "/" + GROUP_COMMON );

	bOk &= m_config->Read( ENTRY_USER_MANUAL, &m_userManual );
	bOk &= m_config->Read( ENTRY_USER_MANUAL_VIEWER, &m_userManualViewer );
	bOk &= m_config->Read( ENTRY_LAST_DATA_PATH, &m_lastDataPath );
	bOk &= m_config->Read( ENTRY_LAST_PAGE_REACHED, &m_lastPageReached, DATASETS_PAGE_NAME );

	m_config->SetPath( "/" + GROUP_WKS );

	bOk &= m_config->Read( ENTRY_LAST, &m_lastWksPath );

	m_config->SetPath( "/" + GROUP_COLORTABLE );

	bOk &= m_config->Read( ENTRY_LAST, &m_lastColorTable );

	bOk &= LoadConfigSelectionCriteria();

	return bOk;
#endif
}

void CBratGuiApp::LoadFileHistory( std::vector<std::string> &v)
{
#if defined( NEW_CONFIGURATION_STUFF )
	assert__( !m_config && mAppSettings );

	mAppSettings->LoadRecentFiles( v );

#else
	assert__( m_config && !mAppSettings );

  m_config->SetPath("/" + GROUP_WKS_RECENT);

  long maxEntries = m_config->GetNumberOfEntries();
  bool bOk = false;
  wxString entry;
  wxString valueString;
  long i = 0;

  do
  {
    bOk = m_config->GetNextEntry(entry, i);
    if (bOk)
    {
      valueString = m_config->Read(entry);
	  v.push_back( valueString.ToStdString() );		// AddWorkspaceToHistory( valueString );
    }
  }
  while (bOk);

  assert__( v.size() == maxEntries );

  //m_menuBar->Enable(ID_MENU_FILE_RECENT, maxEntries > 0);
#endif
}

//----------------------------------------
bool CBratGuiApp::LoadConfigSelectionCriteria()
{
	assert__( m_config && !mAppSettings );

  bool bOk = true;
  //if (m_config == NULL)
  //{
  //  return bOk;
  //}
	assert__( m_config );


  try {

    CMapProduct& mapProductInstance = CMapProduct::GetInstance();
    mapProductInstance.AddCriteriaToProducts();

    wxString configPath;

    CObMap::iterator it;
    for (it = mapProductInstance.begin() ; it != mapProductInstance.end() ; it++)
    {
      CProduct* product = dynamic_cast<CProduct*>(it->second);
      if (product == NULL)
      {
        continue;
      }
      if (!product->HasCriteriaInfo())
      {
        continue;
      }

      //configPath.Empty();
      //configPath.Append("/");
      //configPath.Append(GROUP_SEL_CRITERIA);
      //configPath.Append(product->GetLabel().c_str());

	  configPath = BuildComposedKey( { "/", GROUP_SEL_CRITERIA, product->GetLabel() } ).c_str();

      wxString value;

      //------------------------------------
      m_config->SetPath(configPath);
      //------------------------------------

      if (product->HasLatLonCriteria())
      {
        value = m_config->Read(ENTRY_LAT_LON);
        if (!value.IsEmpty())
        {
          product->GetLatLonCriteria()->Set((const char *)value.c_str());
        }
      }
      if (product->HasDatetimeCriteria())
      {
        value = m_config->Read(ENTRY_DATETIME);
        if (!value.IsEmpty())
        {
          product->GetDatetimeCriteria()->SetFromText((const char *)value.c_str());
        }
      }
      if (product->HasCycleCriteria())
      {
        value = m_config->Read(ENTRY_CYCLE);
        if (!value.IsEmpty())
        {
          product->GetCycleCriteria()->SetFromText((const char *)value.c_str());
        }
      }
      if (product->HasPassIntCriteria())
      {
        value = m_config->Read(ENTRY_PASS_NUMBER);
        if (!value.IsEmpty())
        {
          product->GetPassIntCriteria()->SetFromText((const char *)value.c_str());
        }
      }
      if (product->HasPassStringCriteria())
      {
        value = m_config->Read(ENTRY_PASS_STRING);
        if (!value.IsEmpty())
        {
          product->GetPassStringCriteria()->Set((const char *)value.c_str());
        }
      }
    }
  }
  catch(CException &e) {
    wxString msg = wxString::Format("An error occured while loading criteria selection configuration (CBratGuiApp::LoadConfigSelectionCriteria)\nNavive execption: %s", e.what());
    throw CException((const char *)msg.c_str(), e.error());
  }

  return bOk;

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CONFIGURATION - end
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------
////----------------------------------------
//void CBratGuiApp::CreateTree( CWorkspace* root, CTreeWorkspace& tree )
//{
//	tree.DeleteTree();
//
//	// Set tree root
//	tree.SetRoot( root->GetName(), root, true );
//
//	//WARNING : the sequence of workspaces object creation is significant, because of the interdependence of them
//
//	std::string 
//
//	//FIRSTLY - Create "Datasets" branch
//	path = root->GetPath() + "/" + CWorkspaceDataset::NAME;
//	CWorkspaceDataset* wksDataSet = new CWorkspaceDataset( CWorkspaceDataset::NAME, path );
//	tree.AddChild( wksDataSet->GetName(), wksDataSet );
//
//	//SECOND Create "Formulas" branch
//	path = root->GetPath() + "/" + CWorkspaceFormula::NAME;
//	std::string errorMsg;
//	CWorkspaceFormula* wksFormula = new CWorkspaceFormula( errorMsg, CWorkspaceFormula::NAME, path );
//	if ( !errorMsg.empty() )
//		wxMessageBox( errorMsg, "Warning", wxOK | wxCENTRE | wxICON_INFORMATION );
//
//	tree.AddChild( wksFormula->GetName(), wksFormula );
//
//	//THIRDLY - Create "Operations" branch
//	path = root->GetPath() + "/" + CWorkspaceOperation::NAME;
//	CWorkspaceOperation* wksOperation = new CWorkspaceOperation( CWorkspaceOperation::NAME, path );
//	tree.AddChild( wksOperation->GetName(), wksOperation );
//
//	//FOURTHLY -  Create "Displays" branch
//	path = root->GetPath() + "/" + CWorkspaceDisplay::NAME;
//	CWorkspaceDisplay* wksDisplay = new CWorkspaceDisplay( CWorkspaceDisplay::NAME, path );
//	tree.AddChild( wksDisplay->GetName(), wksDisplay );
//}
//----------------------------------------

void CBratGuiApp::EvtFocus(wxWindow& window,  int32_t eventType, const wxFocusEventFunction& method,
                                wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(eventType,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}

//----------------------------------------
void CBratGuiApp::EvtChar(wxWindow& window,  int32_t eventType, const wxCharEventFunction& method,
                                wxObject* userData, wxEvtHandler* eventSink)
{
  window.Connect(eventType,
                 (wxObjectEventFunction)
                 (wxEventFunction)
                 method,
                 userData,
                 eventSink);
}
