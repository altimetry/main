#if !defined DATA_MODELS_MODEL_H
#define DATA_MODELS_MODEL_H

#include <QtCore>

#include "Workspaces/TreeWorkspace.h"


class CApplicationPaths;
class CDisplay;


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

	static CModel *smInstance;


	static CWorkspace* RootWorkspace( CTreeWorkspace &tree );

	template< class WKSPC >
	static WKSPC* Workspace( CTreeWorkspace &tree );

	template< class WKSPC >
	static const WKSPC* Workspace( const CTreeWorkspace &tree );

    static CWorkspace* CreateTree( CTreeWorkspace &tree, const std::string& path, std::string &error_msg );

	static CWorkspace* LoadWorkspace( CTreeWorkspace &tree, const std::string& path, std::string &error_msg );


	// instance data

	CTreeWorkspace mTree;
	const CApplicationPaths &mBratPaths;


	// construction / destruction

	explicit CModel( const CApplicationPaths &brat_paths );

public:
	static CModel& CreateInstance( const CApplicationPaths &brat_paths );

	virtual ~CModel();


	// access 

	const CTreeWorkspace& Tree() const { return  mTree; }

	template< class WKSPC >
	WKSPC* Workspace()
	{
		return Workspace< WKSPC >( mTree );
	}


	CWorkspace* RootWorkspace();
	const CWorkspace* RootWorkspace() const
	{
		return const_cast< CModel* >( this )->RootWorkspace();
	}


	// operations

	static bool LoadImportFormulas( const std::string& path, std::vector< std::string > &v, bool predefined, bool user, std::string &error_msg );

	
    CWorkspace* CreateWorkspace( const std::string& name, const std::string& path, std::string &error_msg );


	CWorkspace* LoadWorkspace( const std::string& path, std::string &error_msg );


	bool SaveWorkspace( std::string &error_msg )
	{
		return mTree.SaveConfig(
			error_msg,
			Workspace< CWorkspaceOperation >(),
			Workspace< CWorkspaceDisplay >() );
	}


	bool ImportWorkspace( const std::string& path, 
		bool datasets, bool formulas, bool operations, bool views, const std::vector< std::string > &vformulas, std::string &error_msg );

	
	void Reset();


	//

	std::vector<const CDisplay*> OperationDisplays( const std::string &name ) const;

};



#endif	//DATA_MODELS_MODEL_H
