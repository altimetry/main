#include "stdafx.h"

#include "Workspaces/Workspace.h"
#include "Model.h"

#include "BratMainWindow.h"



//explicit 
CModel::CModel()
{}

//virtual 
CModel::~CModel()
{
	mTree.Clear();
}	


//static 
CWorkspace* CModel::GetRootWorkspace( CTreeWorkspace &tree )
{
	return tree.GetRootData();
}

CWorkspace* CModel::GetCurrentRootWorkspace()
{
	if ( mCurrentTree == nullptr )
		mCurrentTree = &mTree;

	return GetRootWorkspace( *mCurrentTree );
}


//static 
template< class WKSPC >
WKSPC* CModel::GetWorkspace( CTreeWorkspace &tree )
{
	CWorkspace* wks = GetRootWorkspace( tree );
	std::string workspaceKey = !wks ? "" : ( wks->GetKey() + CWorkspace::m_keyDelimiter + WKSPC::NAME );

	return dynamic_cast< WKSPC* >( tree.FindWorkspace( workspaceKey ) );
}


CWorkspace* CModel::CreateWorkspace( const std::string& name, const std::string& path, std::string &error_msg )
{
    return mTree.CreateReset( name, path, error_msg );
}

CWorkspace* CModel::CreateTree( CTreeWorkspace &tree, const std::string& path, std::string &error_msg )
{
    return tree.LoadReset( path, error_msg );
}


//static 
CWorkspace* CModel::LoadWorkspace( CTreeWorkspace &tree, const std::string& path, std::string &error_msg )
{
	CWorkspace *wks = CreateTree( tree, path, error_msg );
	CWorkspace *failed_wks = nullptr;
	if ( !tree.LoadConfig( 
		failed_wks,
		GetWorkspace< CWorkspaceDataset >( tree ),
		GetWorkspace< CWorkspaceOperation >( tree ),
		GetWorkspace< CWorkspaceDisplay >( tree ), 
		error_msg 
		) )
	{
		error_msg += ( "\nUnable to load workspace '" + ( failed_wks ? failed_wks->GetName() : "" ) + "'." );
		delete wks;
		wks = nullptr;
	}

	return wks;
}

CWorkspace* CModel::LoadWorkspace( const std::string& path, std::string &error_msg )
{
	return LoadWorkspace( mTree, path, error_msg );
}


void CModel::Reset()
{
	mTree.Clear();
}

void CModel::ResetImportTree()
{
	mImportTree.Clear();
}



bool CModel::LoadImportFormulas( const std::string& path, std::vector< std::string > &v, bool predefined, bool user, std::string &error_msg )
{
	CTreeWorkspace import_tree;
	CWorkspace* wks = CreateTree( import_tree, path, error_msg );
	if ( !wks )
	{
		assert__( !error_msg.empty() );
		return false;
	}

	const CWorkspaceFormula* wksFormula =  import_tree.LoadConfigFormula(
		GetWorkspace< CWorkspaceDataset >( import_tree ),
		GetWorkspace< CWorkspaceOperation >( import_tree ),
		GetWorkspace< CWorkspaceDisplay >( import_tree ), 
		error_msg 
		);

	if ( !wksFormula )
	{
		assert__( !error_msg.empty() );
		return false;
	}

	wksFormula->GetFormulaNames( v, predefined, user );

	import_tree.Clear();

	return true;
}







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


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_Model.cpp"
