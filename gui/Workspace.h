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

#include "new-gui/Common/QtFileUtils.h"

#include "BratObject.h"

#include "new-gui/brat/Workspaces/Constants.h"
#include "new-gui/brat/Workspaces/Dataset.h"
#include "Formula.h"
#include "Operation.h"
#include "Display.h"


using namespace brathl;


class CBitSet32;




const int IMPORT_DATASET_INDEX = 0;
const int IMPORT_FORMULA_INDEX = 1;
const int IMPORT_OPERATION_INDEX = 2;
const int IMPORT_DISPLAY_INDEX = 3;


//-------------------------------------------------------------
//------------------- CWorkspace class --------------------
//-------------------------------------------------------------

class CWorkspace : public CBratObject
{
	static const std::string m_configName;

public:
	static const std::string m_keyDelimiter;

protected:
	CConfiguration *m_config = nullptr;

	//wxFileName m_configFileName;
	//wxFileName m_path;
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
//protected:
//	CWorkspace( const std::string& name, const wxFileName& path, bool createPath = true )
//	{
//		SetName( name );
//		SetPath( path, createPath );
//	}
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

	const std::string& GetPath() { return m_path; }
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

	virtual bool SaveConfig( bool flush = true );
	virtual bool LoadConfig();
	virtual bool Import( CWorkspace* wks )
	{
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

	bool CheckFiles();


	virtual bool SaveConfig( bool flush = true ) override;
	virtual bool LoadConfig() override;
	virtual bool Import( CWorkspace* wks ) override;

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

	virtual void Dump( std::ostream& fOut = std::cerr );

protected:
	bool SaveConfigDataset();
	bool LoadConfigDataset();
};

//-------------------------------------------------------------
//------------------- CWorkspaceFormula class --------------------
//-------------------------------------------------------------

class CWorkspaceFormula : public CWorkspace
{
protected:
	CMapFormula m_formulas;

	std::vector< std::string > m_formulasToImport;

public:
	// constructors and destructors
	CWorkspaceFormula()
	{}
	//CWorkspaceFormula( const std::string& name, const wxFileName& path, bool createPath = true )
	//	: CWorkspace( name, path, createPath )
	//{}
	CWorkspaceFormula( const std::string& name, const std::string& path, bool createPath = true )
		: CWorkspace( name, path, createPath )
	{}

	virtual ~CWorkspaceFormula()
	{}

	virtual bool SaveConfig( bool flush = true ) override;
	virtual bool LoadConfig() override;
	virtual bool Import( CWorkspace* wks ) override;

	void AddFormula( CFormula& formula );
	void RemoveFormula( const std::string& name );

	std::string GetDescFormula( const std::string& name, bool alias = false );
	std::string GetCommentFormula( const std::string& name );
	CFormula* GetFormula( const std::string& name );
	CFormula* GetFormula( CMapFormula::iterator it );

	int32_t GetFormulaCount() { return m_formulas.size(); }
	CMapFormula* GetFormulas() { return &m_formulas; }

	void GetFormulaNames( CStringMap& stringMap, bool getPredefined = true, bool getUser = true );
	void GetFormulaNames( std::vector< std::string >& array, bool getPredefined = true, bool getUser = true );
	void GetFormulaNames( wxComboBox& combo, bool getPredefined = true, bool getUser = true );
	void GetFormulaNames( wxListBox& lb, bool getPredefined = true, bool getUser = true );

	std::vector< std::string >& GetFormulasToImport() { return m_formulasToImport; }
	void SetFormulasToImport( const std::vector< std::string >& array );

	bool IsFormulaToImport( const std::string& name );

	void AmendFormulas( const CStringArray& key, CProduct* product, const std::string& record );

	bool SaveConfigPredefinedFormula();

	virtual void Dump( std::ostream& fOut = std::cerr );

protected:
	bool LoadConfigFormula();
	bool SaveConfigFormula();
};

//-------------------------------------------------------------
//------------------- CWorkspaceOperation class --------------------
//-------------------------------------------------------------

class CWorkspaceOperation : public CWorkspace
{
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

	virtual bool SaveConfig( bool flush = true ) override;
	virtual bool LoadConfig() override;
	virtual bool Import( CWorkspace* wks ) override;

	void GetOperationNames( std::vector< std::string >& array );
	void GetOperationNames( wxComboBox& combo );

	bool HasOperation() { return GetOperationCount() > 0; }

	std::string GetOperationNewName();
	std::string GetOperationCopyName( const std::string& baseName );

	COperation* GetOperation( const std::string& name )
	{
		return dynamic_cast<COperation*>( m_operations.Exists( name ) );
	}

	CObMap* GetOperations() { return &m_operations; }

	size_t GetOperationCount()
	{
		return m_operations.size();
	}

    bool InsertOperation( const std::string& name );
    bool InsertOperation( const std::string& name, COperation* operationToCopy );

	bool RenameOperation( COperation* operation, const std::string& newName );

	bool UseDataset( const std::string& name, CStringArray* operationNames = nullptr );

	bool DeleteOperation( COperation* operation );


	virtual void Dump( std::ostream& fOut = std::cerr );

protected:
	bool SaveConfigOperation();
	bool LoadConfigOperation();
};

//-------------------------------------------------------------
//------------------- CWorkspaceDisplay class --------------------
//-------------------------------------------------------------

class CWorkspaceDisplay : public CWorkspace
{
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

	virtual bool SaveConfig( bool flush = true ) override;
	virtual bool LoadConfig() override;
	virtual bool Import( CWorkspace* wks ) override;

	bool UseOperation( const std::string& name, CStringArray* displayNames = nullptr );

	void GetDisplayNames( std::vector< std::string >& array );
	void GetDisplayNames( wxComboBox& combo );

	bool HasDisplay() { return GetDisplayCount() > 0; }

	std::string GetDisplayNewName();

	CDisplay* GetDisplay( const std::string& name )
	{
		return dynamic_cast<CDisplay*>( m_displays.Exists( name ) );
	}

	size_t GetDisplayCount()
	{
		return m_displays.size();
	}

	CObMap* GetDisplays() { return &m_displays; }

    bool InsertDisplay( const std::string& name );

	bool DeleteDisplay( CDisplay* display );

	bool RenameDisplay( CDisplay* display, const std::string& newName );

	virtual void Dump( std::ostream& fOut = std::cerr );

private:
	bool SaveConfigDisplay();
	bool LoadConfigDisplay();
};



#endif		// WORKSPACES_WORKSPACE_H
