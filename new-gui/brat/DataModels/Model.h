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
#if !defined DATA_MODELS_MODEL_H
#define DATA_MODELS_MODEL_H

#include <QtCore>

#include "Workspaces/TreeWorkspace.h"
#include "Filters/BratFilters.h"


class CBratSettings;
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


	/////////////////
	// static members
	/////////////////

	static CModel *smInstance;
	static CBratFilters *smBratFilters;
	static bool smValidFilters;


	static CWorkspace* RootWorkspace( CTreeWorkspace &tree );

	template< class WKSPC >
	static WKSPC* Workspace( CTreeWorkspace &tree );

	template< class WKSPC >
	static const WKSPC* Workspace( const CTreeWorkspace &tree );

    static CWorkspace* CreateTree( CTreeWorkspace &tree, const std::string& path, std::string &error_msg );

	static CWorkspace* LoadWorkspace( CTreeWorkspace &tree, const std::string& path, std::string &error_msg );

	static bool SaveWorkspace( CTreeWorkspace &tree, std::string &error_msg );


public:

	static bool BratFiltersValid() { return smValidFilters; }


private:

	/////////////////
	// instance data
	/////////////////

	CTreeWorkspace mTree;
	const CBratSettings &mSettings;


	/////////////////////////////
	// construction / destruction
	/////////////////////////////

	explicit CModel( const CBratSettings &settings );

	virtual ~CModel();

public:
	static CModel& CreateInstance( const CBratSettings &settings );

	static void DestroyInstance();


	/////////////
	// access 
	/////////////

	//...workspaces

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


	//...settings && paths

	const CBratSettings& Settings() const { return mSettings; }

	const CApplicationPaths& BratPaths() const;

	//...filters

	//always check BratFiltersValid() before using BratFilters()

	CBratFilters& BratFilters();				//throws

	const CBratFilters& BratFilters() const
	{ 
		return const_cast<CModel*>( this )->BratFilters(); 
	}



	/////////////
	// operations
	/////////////


	//...workspaces

	static bool LoadImportFormulas( const std::string& path, std::vector< std::string > &v, bool predefined, bool user, std::string &error_msg );

	
    CWorkspace* CreateWorkspace( const std::string& name, const std::string& path, std::string &error_msg );


	CWorkspace* LoadWorkspace( const std::string& path, std::string &error_msg );


	bool SaveWorkspace( std::string &error_msg )
	{
		return SaveWorkspace( mTree, error_msg );
	}


	bool ImportWorkspace( const std::string& path, 
		bool datasets, bool formulas, bool operations, bool views, const std::vector< std::string > &vformulas, std::string &error_msg );

	
	void Reset();


	//

	template< class DISPLAY >
	std::vector< DISPLAY* > OperationDisplays( const std::string &name );

	std::vector<const CDisplay*> OperationDisplays( const std::string &name ) const;

	std::vector<CDisplay*> OperationDisplays( const std::string &name );

	bool DeleteOperationDisplays( const std::string &name );
};



#endif	//DATA_MODELS_MODEL_H
