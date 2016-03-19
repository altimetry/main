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
#if !defined(DATA_MODELS_WORKSPACES_FUNCTION_H)
#define DATA_MODELS_WORKSPACES_FUNCTION_H

#include "libbrathl/brathl.h"
#include "libbrathl/BratObject.h"
#include "libbrathl/Expression.h"

using namespace brathl;



class CWorkspaceSettings;


//-------------------------------------------------------------
//------------------- CFunction class --------------------
//-------------------------------------------------------------

class CFunction : public CBratObject
{
protected:

	std::string m_name;
	std::string m_description;
	int32_t m_nbParams;
	int32_t m_category;

public:
	CFunction( const std::string& name, const std::string& description, int32_t category = MathTrigo, int32_t nbParams = 1 );

	/// Destructor
	virtual ~CFunction();

	std::string GetSyntax() const;

    std::string GetName() { return m_name; }
    void SetName( const std::string& value ) { m_name = value; }

    std::string GetDescription() { return m_description; }
    void SetDescription( const std::string& value ) { m_description = value; }

    int32_t GetNbparams() { return m_nbParams; }
    void SetNbParams( int32_t value ) { m_nbParams = value; }

    int32_t GetCategory() { return m_category; }
	std::string GetCategoryAsString();
    void SetCategory( int32_t value ) { m_category = value; }

	static std::string GetCategoryAsString( int32_t category );

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr );
	virtual void DumpFmt( std::ostream& fOut = std::cerr );
};


//-------------------------------------------------------------
//------------------- CMapFunction class --------------------
//-------------------------------------------------------------

class CMapFunction : public CObMap
{
	friend class CWorkspaceSettings;
protected:

	CWorkspaceSettings *m_config = nullptr;
	static const std::string m_configFilename;

protected:

	void DeleteConfig();
	void Init();

public:
	/// CIntMap ctor
	CMapFunction();

	/// CIntMap dtor
	virtual ~CMapFunction();

	static CMapFunction&  GetInstance();

	std::string GetDescFunc( const std::string& name );
	std::string GetSyntaxFunc( const std::string& name );


	bool ValidName( const char* name );
	bool ValidName( const std::string& name );

	std::string GetFunctionExtraDescr( const std::string& pathSuff );

	void SetExtraDescr();

	static std::string GetFunctionExtraDescr( CWorkspaceSettings *config, const std::string& pathSuff = "" );
	static void SaveFunctionDescrTemplate( const std::string &internal_data_path, CWorkspaceSettings *config, bool flush = true );

	virtual void Dump( std::ostream& fOut  = std::cerr );
	virtual void DumpFmt( std::ostream& fOut  = std::cerr );
};



/** @} */


#endif		//DATA_MODELS_WORKSPACES_FUNCTION_H
