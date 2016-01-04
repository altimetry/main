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

#ifndef WORKSPACES_WORKSPACE_H
#define WORKSPACES_WORKSPACE_H

#include "new-gui/Common/QtUtilsIO.h"

#include "libbrathl/BratObject.h"

#include "new-gui/brat/Workspaces/Constants.h"
#include "new-gui/brat/Workspaces/Dataset.h"
#include "new-gui/brat/Workspaces/Formula.h"


using namespace brathl;


struct CBitSet32;
class COperation;
class CDisplay;



const int IMPORT_DATASET_INDEX = 0;
const int IMPORT_FORMULA_INDEX = 1;
const int IMPORT_OPERATION_INDEX = 2;
const int IMPORT_DISPLAY_INDEX = 3;



class CWorkspaceOperation;
class CWorkspaceDataset;
class CWorkspaceDisplay;


//-------------------------------------------------------------
//------------------- CWorkspace class --------------------
//-------------------------------------------------------------

class CWorkspace : public CBratObject
{
	friend class CConfiguration;
	friend class CWorkspaceSettings;

	static const std::string m_configName;

public:
	static const std::string m_keyDelimiter;

protected:
	CConfiguration *m_config = nullptr;

	std::string m_configFileName;
	std::string m_path;

	std::string m_key;
	std::string m_name;

	int m_level = -1;

	CBitSet32* m_importBitSet = nullptr;

	bool m_ctrlDatasetFiles = true;

public:
	// constructors and destructors
	CWorkspace()
	{}
public:
	CWorkspace( const std::string& name, const std::string& path, bool createPath = true )
	{
		SetName( name );
		SetPath( path, createPath );
	}

	virtual ~CWorkspace()
	{
		delete m_config;
	}

	void SetName( const std::string& name ) { m_name = name; }
	const std::string& GetName() const { return m_name; }

//protected:
//	bool SetPath( const wxFileName& path, bool createPath = true );
public:
	bool SetPath( const std::string& path, bool createPath = true );

	void SetLevel( int32_t value ) { m_level = value; }

	bool Rmdir();

	const std::string& GetPath() const { return m_path; }
	//std::string GetPathName(/* int32_t flags = wxPATH_GET_VOLUME, wxPathFormat format = wxPATH_NATIVE */)
	//{
	//	return m_path.GetPath( wxPATH_GET_VOLUME, wxPATH_NATIVE ).ToStdString();
	//	//return m_path.GetPath( flags, format ).ToStdString();
	//}

protected:
	CConfiguration* GetConfig() const { return m_config; }

public:	
	void SetKey( const std::string& key ) { m_key = key; }
	const std::string& GetKey() { return m_key; }
	std::string GetRootKey();

	bool HasName() const { return !m_name.empty(); }
	bool HasPath() const { return !m_path.empty(); }
	bool IsPath() const { return IsDir( m_path ); }
	bool IsConfigFile() const { return IsFile( m_configFileName ); }
	bool IsRoot() const { return m_level == 1; }

	virtual bool SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush = true );
	virtual bool LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso );
	virtual bool Import( CWorkspace* wks, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso )
	{
        UNUSED( wks );
        UNUSED( errorMsg );
        UNUSED( wksd );
        UNUSED( wkso );

        return true;
	}

	void SetImportBitSet( CBitSet32* value ) { m_importBitSet = value; }

	bool GetCtrlDatasetFiles() { return m_ctrlDatasetFiles; }
	void SetCtrlDatasetFiles( bool value ) { m_ctrlDatasetFiles = value; }

	virtual void Dump( std::ostream& fOut = std::cerr );

protected:
	void InitConfig();

	bool SaveCommonConfig( bool flush = true );
	bool LoadCommonConfig();
	//bool RmdirRecurse();
	bool DeleteConfigFile();
};

//-------------------------------------------------------------
//------------------- CWorkspaceDataset class --------------------
//-------------------------------------------------------------

class CWorkspaceDataset : public CWorkspace
{
	using base_t = CWorkspace;

	friend class CConfiguration;
	friend class CWorkspaceSettings;

public:
	static const std::string NAME;

protected:
	CObMap m_datasets;

public:
	// constructors and destructors
	CWorkspaceDataset()
	{}
	//CWorkspaceDataset( const std::string& name, const wxFileName& path, bool createPath = true )
	//	: CWorkspace( name, path, createPath )
	//{}
	CWorkspaceDataset( const std::string& name, const std::string& path, bool createPath = true )
		: CWorkspace( name, path, createPath )
	{}

	virtual ~CWorkspaceDataset()
	{
		Close();
		m_datasets.RemoveAll();
	}

	void Close();

	bool HasDataset() { return GetDatasetCount() > 0; }

	bool CheckFiles( std::string &errorMsg );			// femm: Apparently not used...


	virtual bool SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush = true ) override;
	virtual bool LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso ) override;
	virtual bool Import( CWorkspace* wks, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso ) override;

	bool RenameDataset( CDataset* dataset, const std::string& newName );
	bool DeleteDataset( CDataset* dataset );

	std::string GetDatasetNewName();

	size_t GetDatasetCount() const
	{
		return m_datasets.size();
	}
	CDataset* GetDataset( const std::string& name );
	const CObMap* GetDatasets() const { return &m_datasets; }

	void GetDatasetNames( std::vector< std::string >& array ) const;

    bool InsertDataset( const std::string& name );

    virtual void Dump( std::ostream& fOut = std::cerr ) override;

protected:
	bool SaveConfigDataset( std::string &errorMsg );
	bool LoadConfigDataset( std::string &errorMsg );
};

//-------------------------------------------------------------
//------------------- CWorkspaceFormula class --------------------
//-------------------------------------------------------------

class CWorkspaceFormula : public CWorkspace
{
	using base_t = CWorkspace;

	friend class CConfiguration;

public:
	static const std::string NAME;

protected:
	CMapFormula m_formulas;

	std::vector< std::string > m_formulasToImport;

public:
	// constructors and destructors
	CWorkspaceFormula( std::string &errorMsg ) : base_t(), m_formulas( errorMsg, true )
	{}
	//CWorkspaceFormula( const std::string& name, const wxFileName& path, bool createPath = true )
	//	: CWorkspace( name, path, createPath )
	//{}
	CWorkspaceFormula( std::string &errorMsg, const std::string& name, const std::string& path, bool createPath = true )
		: base_t( name, path, createPath ), m_formulas( errorMsg, true )
	{}

	virtual ~CWorkspaceFormula()
	{}

	virtual bool SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush = true ) override;
	virtual bool LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso ) override;
	virtual bool Import( CWorkspace* wks, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso ) override;

	bool AddFormula( CFormula& formula, std::string &errorMsg );
	void RemoveFormula( const std::string& name );

	std::string GetDescFormula( const std::string& name, bool alias = false );
	std::string GetCommentFormula( const std::string& name );
	CFormula* GetFormula( const std::string& name );
	CFormula* GetFormula( CMapFormula::iterator it );
	const CFormula* GetFormula( CMapFormula::const_iterator it ) const;

	size_t GetFormulaCount() { return m_formulas.size(); }
	CMapFormula* GetFormulas() { return &m_formulas; }
	const CMapFormula* GetFormulas() const { return &m_formulas; }

	void GetFormulaNames( CStringMap& stringMap, bool getPredefined = true, bool getUser = true );
	void GetFormulaNames( std::vector< std::string >& array, bool getPredefined = true, bool getUser = true );

	std::vector< std::string >& GetFormulasToImport() { return m_formulasToImport; }
	void SetFormulasToImport( const std::vector< std::string >& array );

	bool IsFormulaToImport( const std::string& name );

	void AmendFormulas( const CStringArray& key, CProduct* product, const std::string& record );

	bool SaveConfigPredefinedFormula();

    virtual void Dump( std::ostream& fOut = std::cerr ) override;

protected:
	bool LoadConfigFormula( std::string &errorMsg );
	bool SaveConfigFormula();
};

//-------------------------------------------------------------
//------------------- CWorkspaceOperation class --------------------
//-------------------------------------------------------------

class CWorkspaceOperation : public CWorkspace
{
	using base_t = CWorkspace;

	friend class CConfiguration;

public:
	static const std::string NAME;

protected:
	CObMap m_operations;

public:
	// constructors and destructors
	CWorkspaceOperation()
	{}
	//CWorkspaceOperation( const std::string& name, const wxFileName& path, bool createPath = true )
	//	: CWorkspace( name, path, createPath )
	//{}
	CWorkspaceOperation( const std::string& name, const std::string& path, bool createPath = true )
		: CWorkspace( name, path, createPath )
	{}
	virtual ~CWorkspaceOperation()
	{
		m_operations.RemoveAll();
	}

	virtual bool SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush = true ) override;
	virtual bool LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso ) override;
	virtual bool Import( CWorkspace* wks, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso ) override;

	void GetOperationNames( std::vector< std::string >& array );

	bool HasOperation() const { return GetOperationCount() > 0; }

	std::string GetOperationNewName();
	std::string GetOperationCopyName( const std::string& baseName );

	COperation* GetOperation( const std::string& name );

	CObMap* GetOperations() { return &m_operations; }
	const CObMap* GetOperations() const { return &m_operations; }

	size_t GetOperationCount() const { return m_operations.size(); }

    bool InsertOperation( const std::string& name );
    bool InsertOperation( const std::string& name, COperation* operationToCopy, CWorkspaceOperation *wkso );

	bool RenameOperation( COperation* operation, const std::string& newName );

	bool UseDataset( const std::string& name, std::string &errorMsg, CStringArray* operationNames = nullptr );

	bool DeleteOperation( COperation* operation );


    virtual void Dump( std::ostream& fOut = std::cerr ) override;

protected:
	bool SaveConfigOperation( std::string &errorMsg );
	bool LoadConfigOperation( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso  );
};

//-------------------------------------------------------------
//------------------- CWorkspaceDisplay class --------------------
//-------------------------------------------------------------

class CWorkspaceDisplay : public CWorkspace
{
	using base_t = CWorkspace;

	friend class CConfiguration;

public:
	static const std::string NAME;

protected:
	CObMap m_displays;

public:
	// constructors and destructors
	CWorkspaceDisplay()
	{}
	//CWorkspaceDisplay( const std::string& name, const wxFileName& path, bool createPath = true )
	//	: CWorkspace( name, path, createPath )
	//{}
	CWorkspaceDisplay( const std::string& name, const std::string& path, bool createPath = true )
		: CWorkspace( name, path, createPath )
	{}
	virtual ~CWorkspaceDisplay()
	{
		m_displays.RemoveAll();
	}

	virtual bool SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush = true ) override;
	virtual bool LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso ) override;
	virtual bool Import( CWorkspace* wks, std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso ) override;

	bool UseOperation( const std::string& name, std::string &errorMsg, CStringArray* displayNames = nullptr );

	void GetDisplayNames( std::vector< std::string >& array );

	bool HasDisplay() { return GetDisplayCount() > 0; }

	std::string GetDisplayNewName();

	CDisplay* GetDisplay( const std::string& name );

	size_t GetDisplayCount()
	{
		return m_displays.size();
	}

	CObMap* GetDisplays() { return &m_displays; }
	const CObMap* GetDisplays() const { return &m_displays; }

    bool InsertDisplay( const std::string& name );

	bool DeleteDisplay( CDisplay* display );

	bool RenameDisplay( CDisplay* display, const std::string& newName );

    virtual void Dump( std::ostream& fOut = std::cerr ) override;

private:
	bool SaveConfigDisplay( std::string &errorMsg, CWorkspaceDisplay *wksd );
	bool LoadConfigDisplay( std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso );
};



#endif		// WORKSPACES_WORKSPACE_H
