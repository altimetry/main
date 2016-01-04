#include "stdafx.h"

#include "Workspaces/Workspace.h"
#include "Model.h"

#include "BratMainWindow.h"



//explicit 
CModel::CModel()
{}

//virtual 
CModel::~CModel()
{}	

///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

void CBratMainWindow::XYPlot()
{
	/*
	CObArray::iterator itGroup;

	wxSize size = wxDefaultSize;
	wxPoint pos = wxDefaultPosition;

	for ( itGroup = m_plots.begin(); itGroup != m_plots.end(); itGroup++ )
	{
		CPlot* plot = dynamic_cast<CPlot*>( *itGroup );
		if ( plot == nullptr )
		{
			continue;
		}

		CWindowHandler::GetSizeAndPosition( size, pos );

		CreateXYPlot( plot, size, pos );
	}
	*/
}
void CBratMainWindow::ZFXYPlot()
{
	/*
	CObArray::iterator itGroup;

	wxSize size = wxDefaultSize;
	wxPoint pos = wxDefaultPosition;

	for ( itGroup = m_plots.begin(); itGroup != m_plots.end(); itGroup++ )
	{
		CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>( *itGroup );
		if ( zfxyplot == nullptr )
		{
			continue;
		}

		CWindowHandler::GetSizeAndPosition( size, pos );

		CreateZFXYPlot( zfxyplot, size, pos );
	}
	*/
}


void CBratMainWindow::CreateTree( CWorkspace* root, CTreeWorkspace& tree )
{
	tree.DeleteTree();

	// Set tree root
	tree.SetRoot( root->GetName(), root, true );

	//WARNING : the sequence of workspaces object creation is significant, because of the interdependence of them

	std::string 

	//FIRSTLY - Create "Datasets" branch
	path = root->GetPath() + "/" + CWorkspaceDataset::NAME;
	CWorkspaceDataset* wksDataSet = new CWorkspaceDataset( CWorkspaceDataset::NAME, path );
	tree.AddChild( wksDataSet->GetName(), wksDataSet );

	//SECOND Create "Formulas" branch
	path = root->GetPath() + "/" + CWorkspaceFormula::NAME;
	std::string errorMsg;
	CWorkspaceFormula* wksFormula = new CWorkspaceFormula( errorMsg, CWorkspaceFormula::NAME, path );
	if ( !errorMsg.empty() )
		SimpleWarnBox( errorMsg );
	tree.AddChild( wksFormula->GetName(), wksFormula );

	//THIRDLY - Create "Operations" branch
	path = root->GetPath() + "/" + CWorkspaceOperation::NAME;
	CWorkspaceOperation* wksOperation = new CWorkspaceOperation( CWorkspaceOperation::NAME, path );
	tree.AddChild( wksOperation->GetName(), wksOperation );

	//FOURTHLY -  Create "Displays" branch
	path = root->GetPath() + "/" + CWorkspaceDisplay::NAME;
	CWorkspaceDisplay* wksDisplay = new CWorkspaceDisplay( CWorkspaceDisplay::NAME, path );
	tree.AddChild( wksDisplay->GetName(), wksDisplay );
}

template< class WKSPC >
WKSPC* CBratMainWindow::GetCurrentWorkspace()
{
	if ( mCurrentTree == nullptr )
		mCurrentTree = &mTree;

	CWorkspace* wks = GetCurrentRootWorkspace();
	std::string workspaceKey = !wks ? "" : ( wks->GetKey() + CWorkspace::m_keyDelimiter + WKSPC::NAME );

	return dynamic_cast< WKSPC* >( mCurrentTree->FindObject( workspaceKey ) );
}

CWorkspace* CBratMainWindow::GetCurrentRootWorkspace()
{
	if ( mCurrentTree == nullptr )
		mCurrentTree = &mTree;

	return mCurrentTree->GetRootData();
}

void CBratMainWindow::EnableCtrlWorkspace()
{
	//m_guiPanel->GetDatasetPanel()->EnableCtrl();
	//m_guiPanel->GetOperationPanel()->EnableCtrl();
	//m_guiPanel->GetDisplayPanel()->EnableCtrl();
}
void CBratMainWindow::LoadWorkspace()
{
	//m_guiPanel->GetDatasetPanel()->LoadDataset();
	//m_guiPanel->GetOperationPanel()->LoadOperation();
	//m_guiPanel->GetDisplayPanel()->LoadDisplay();
}
void CBratMainWindow::ResetWorkspace()
{
	//m_guiPanel->GetDatasetPanel()->Reset();
	//m_guiPanel->GetOperationPanel()->Reset();
	//m_guiPanel->GetDisplayPanel()->Reset();

	//m_guiPanel->GetDatasetPanel()->ClearAll();
	//m_guiPanel->GetOperationPanel()->ClearAll();
	//m_guiPanel->GetDisplayPanel()->ClearAll();
}
void CBratMainWindow::AddWorkspaceToHistory( const std::string& name )
{
    UNUSED( name );

    //m_menuBar->Enable( ID_MENU_FILE_RECENT, true );
	//m_wksHistory.AddFileToHistory( name );
}

void CBratMainWindow::SetTitle( CWorkspace *wks )	//= nullptr 
{
	if ( !wks )
		wks = GetCurrentRootWorkspace();

	setWindowTitle( makeWindowTitle( t2q( wks ? wks->GetName() : "" ) ) );
}

void CBratMainWindow::DoEmptyWorkspace()
{
	mTree.DeleteTree();
	ResetWorkspace();
	EnableCtrlWorkspace();
	SetTitle();
}

bool CBratMainWindow::OpenWorkspace( const QString &qpath )
{
	auto path = q2a( qpath );
	CWorkspace* wks = new CWorkspace( GetFilenameFromPath( path ), path );

	CreateTree( wks );
	CWorkspace *failed_wks = nullptr;
	std::string errorMsg;
	if ( !mTree.LoadConfig( failed_wks, 
		GetCurrentWorkspace<CWorkspaceDataset>(), 
		GetCurrentWorkspace<CWorkspaceOperation>(), 
		GetCurrentWorkspace<CWorkspaceDisplay>(), errorMsg ) )
	{
		SimpleWarnBox( errorMsg + "\nUnable to load workspace '" + ( failed_wks ? failed_wks->GetName() : "" ) + "'." );
		DoEmptyWorkspace();
		return false;
	}

	ResetWorkspace();

	LoadWorkspace();

	EnableCtrlWorkspace();

	if ( wks )
		AddWorkspaceToHistory( wks->GetPath() );

	SetTitle( wks );
    //CTextWidget *editor = CTextWidget::OpenWorkspace( fileName, this );
    //if ( editor )
    //    addEditor( editor );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_Model.cpp"
