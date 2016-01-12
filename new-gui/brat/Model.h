#if !defined MODEL_H
#define MODEL_H

#include <QtCore>

#include "Workspaces/TreeWorkspace.h"


class CModel : public QObject
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif
		
	// types

	using base_t = QObject;


	// static members

	static CWorkspace* GetRootWorkspace( CTreeWorkspace &tree );

	template< class WKSPC >
	static WKSPC* GetWorkspace( CTreeWorkspace &tree );

    static CWorkspace* CreateTree( CTreeWorkspace &tree, const std::string& path, std::string &error_msg );

	static CWorkspace* LoadWorkspace( CTreeWorkspace &tree, const std::string& path, std::string &error_msg );


	// instance data

	CTreeWorkspace mTree;
	CTreeWorkspace mImportTree;
	CTreeWorkspace* mCurrentTree = nullptr;


	// construction / destruction

public:
	explicit CModel();

	virtual ~CModel();


	// access 

	const CTreeWorkspace& Tree() const { return  mTree; }

	template< class WKSPC >
	WKSPC* GetCurrentWorkspace()
	{
		if ( mCurrentTree == nullptr )
			mCurrentTree = &mTree;

		return GetWorkspace< WKSPC >( *mCurrentTree );
	}



	CWorkspace* GetCurrentRootWorkspace();

	const CWorkspace* GetCurrentRootWorkspace() const
	{
		return const_cast< CModel* >( this )->GetCurrentRootWorkspace();
	}


	// operations

	static bool LoadImportFormulas( const std::string& path, std::vector< std::string > &v, bool predefined, bool user, std::string &error_msg );



    CWorkspace* CreateWorkspace( const std::string& name, const std::string& path, std::string &error_msg );

	CWorkspace* LoadWorkspace( const std::string& path, std::string &error_msg );

	bool SaveWorkspace( std::string &error_msg )
	{
		return mTree.SaveConfig(
			error_msg,
			GetCurrentWorkspace< CWorkspaceOperation >(),
			GetCurrentWorkspace< CWorkspaceDisplay >() );
	}

	void Reset();

	void ResetImportTree();
};



#endif	//MODEL_H
