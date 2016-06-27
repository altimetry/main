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

#include "Constants.h"
#include "Dataset.h"
#include "Formula.h"


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
class CMapDisplayData;



//-------------------------------------------------------------
//------------------- CWorkspace class --------------------
//-------------------------------------------------------------

class CWorkspace : public CBratObject
{
	// types

	using base_t = CBratObject;

	friend class CWorkspaceSettings;
	friend class CTreeWorkspace;

	std::map< std::string, CBratObject* > m_dummy;
protected:
	using const_iterator = std::map< std::string, CBratObject* >::const_iterator;


	// static members

	static const std::string m_configName;

public:
	static const std::string m_keyDelimiter;

	// For clients to foresee which path will be used by a new workspace
	//
	static std::string MakeNewWorkspacePath( const std::string& name, const std::string& parent_path )
	{
		return parent_path + "/" + name;
	}


protected:
	CWorkspaceSettings *m_config = nullptr;

	std::string m_configFileName;
	std::string m_path;

	std::string m_key;
	std::string m_name;

	int m_level = -1;

	CBitSet32* m_importBitSet = nullptr;

	bool m_ctrlDatasetFiles = true;

protected:
	// construction and destruction

	CWorkspace() = delete;

	//	Create Constructor: to be used only for 1st time creation of new, empty, workspaces
	//
	CWorkspace( const std::string& name, const std::string& path )
	{
		SetName( name );
		SetPath( MakeNewWorkspacePath( name, path ), true );
	}

public:
	//	Load Constructor: to be used only for instantiation of existing workspaces, previously saved in path
	//
	CWorkspace( const std::string& path )
	{
		SetPath( path, false );
	}

	virtual ~CWorkspace();

	const std::string& GetName() const { return m_name; }

	const std::string& GetPath() const { return m_path; }

	const std::string& GetKey() { return m_key; }

//protected:		//TODO public only because of old BratGui

	bool IsConfigFile() const { return IsFile( m_configFileName ); }
	bool IsRoot() const { return m_level == 1; }
	void SetName( const std::string& name ) { m_name = name; }

protected:
	bool SetPath( const std::string& path, bool createPath = true );
	void SetKey( const std::string& key ) { m_key = key; }

	std::string GetRootKey();

public:

	void SetLevel( int32_t value ) { m_level = value; }

	bool Rmdir();


public:	
	virtual const_iterator begin() const { return m_dummy.begin(); }

	virtual const_iterator end() const { return m_dummy.end(); }

	virtual bool SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush = true );
	virtual bool LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso );
	virtual bool Import(CWorkspace* wksi, std::string &errorMsg, CWorkspaceDataset *wks_data, CWorkspaceDisplay *wks_disp, CWorkspaceOperation *wks_op)
	{
        UNUSED( wksi );
        UNUSED( wks_data );
        UNUSED( errorMsg );
        UNUSED( wks_disp );
        UNUSED( wks_op );

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

	friend class CWorkspaceSettings;
	friend class CTreeWorkspace;

public:
	static const std::string NAME;

protected:
	CObMap m_datasets;

protected:
	// construction and destruction

	CWorkspaceDataset( const std::string& path )
		: CWorkspace( NAME, path )
	{}

public:

	virtual ~CWorkspaceDataset()
	{
		m_datasets.RemoveAll();
	}

	// 

	bool HasDataset() { return GetDatasetCount() > 0; }

	bool CheckFiles( std::string &errorMsg );			// femm: Apparently not used...


	virtual const_iterator begin() const override { return m_datasets.begin(); }

	virtual const_iterator end() const override { return m_datasets.end(); }

	virtual bool SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush = true ) override;
	virtual bool LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso ) override;
	virtual bool Import( CWorkspace* wksi, std::string &errorMsg, CWorkspaceDataset *wks_data, CWorkspaceDisplay *wks_disp, CWorkspaceOperation *wks_op ) override;

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

	friend class CWorkspaceSettings;
	friend class CTreeWorkspace;

public:
	static const std::string NAME;

protected:
	CMapFormula m_formulas;

	std::vector< std::string > m_formulasToImport;

protected:
	// construction and destruction

	CWorkspaceFormula( const std::string &internal_data_path, std::string &errorMsg, const std::string& path )
		: base_t( NAME, path ), m_formulas( internal_data_path, errorMsg, true )
	{}

public:

	virtual ~CWorkspaceFormula()
	{}

	virtual const_iterator begin() const override { return m_formulas.begin(); }

	virtual const_iterator end() const override { return m_formulas.end(); }

	virtual bool SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush = true ) override;
	virtual bool LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso ) override;
	virtual bool Import(CWorkspace* wksi, std::string &errorMsg, CWorkspaceDataset *wks_data, CWorkspaceDisplay *wks_disp, CWorkspaceOperation *wks_op) override;

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
	void GetFormulaNames( std::vector< std::string >& array, bool getPredefined = true, bool getUser = true ) const;

	//TODO GetFormulasToImport exists only becaus eof old BratGui
	std::vector< std::string >& GetFormulasToImport() { return m_formulasToImport; }
	void SetFormulasToImport( const std::vector< std::string >& array );

	bool IsFormulaToImport( const std::string& name );

	void AmendFormulas( const CStringArray& key, CProduct* product, const std::string& record );

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

	friend class CWorkspaceSettings;
	friend class CTreeWorkspace;

public:
	static const std::string NAME;
	
	static const std::string& QuickOperationName()
	{
		static const std::string s =  "QuickOperation";
		return s;
	}

protected:
	CObMap m_operations;

protected:
	// construction and destruction

	CWorkspaceOperation( const std::string& path )
		: CWorkspace( NAME, path )
	{}

public:

	virtual ~CWorkspaceOperation()
	{
		m_operations.RemoveAll();
	}

	virtual const_iterator begin() const override { return m_operations.begin(); }

	virtual const_iterator end() const override { return m_operations.end(); }

	virtual bool SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush = true ) override;
	virtual bool LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso ) override;
	virtual bool Import(CWorkspace* wksi, std::string &errorMsg, CWorkspaceDataset *wks_data, CWorkspaceDisplay *wks_disp, CWorkspaceOperation *wks_op) override;

	void GetOperationNames( std::vector< std::string >& array );

	bool HasOperation() const { return GetOperationCount() > 0; }

	std::string GetOperationNewName();
	std::string GetOperationCopyName( const std::string& baseName );

	COperation* GetOperation( const std::string& name );
	COperation* GetQuickOperation() { return GetOperation( QuickOperationName() ); }
	const COperation* GetOperation( const std::string& name ) const;

	CObMap* GetOperations() { return &m_operations; }
	const CObMap* GetOperations() const { return &m_operations; }

	size_t GetOperationCount() const { return m_operations.size(); }

    bool InsertOperation( const std::string& name );
    bool InsertOperation( const std::string& name, COperation* operationToCopy, CWorkspaceDataset *wksds );

	bool RenameOperation( COperation* operation, const std::string& newName );

	bool UseDataset( const std::string& name, CStringArray* operation_names = nullptr );
	bool UseFilter( const std::string& name, CStringArray* operation_names = nullptr );

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

	friend class CWorkspaceSettings;
	friend class CTreeWorkspace;

public:
	static const std::string NAME;

protected:
	CObMap m_displays;

protected:
	// construction and destruction

	CWorkspaceDisplay( const std::string& path )
		: CWorkspace( NAME, path )
	{}

public:
	virtual ~CWorkspaceDisplay()
	{
		m_displays.RemoveAll();
	}

	virtual const_iterator begin() const override { return m_displays.begin(); }

	virtual const_iterator end() const override { return m_displays.end(); }

	virtual bool SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, CWorkspaceDisplay *wksd, bool flush = true ) override;
	virtual bool LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso ) override;
	virtual bool Import(CWorkspace* wksi, std::string &errorMsg, CWorkspaceDataset *wks_data, CWorkspaceDisplay *wks_disp, CWorkspaceOperation *wks_op) override;

	bool UseOperation( const std::string& name, std::string &errorMsg, CStringArray* displayNames = nullptr );

	void GetDisplayNames( std::vector< std::string >& array );

	bool HasDisplay() { return GetDisplayCount() > 0; }

	std::string GetDisplayNewName( const COperation *operation );

	CDisplay* GetDisplay( const std::string& name );

	size_t GetDisplayCount()
	{
		return m_displays.size();
	}

	CObMap* GetDisplays() { return &m_displays; }
	const CObMap* GetDisplays() const { return &m_displays; }

    bool InsertDisplay( const std::string& name );

    CDisplay* CloneDisplay( const CDisplay *display, const COperation *operation, const CWorkspaceOperation *wkso );

	bool DeleteDisplay( CDisplay* display );

	bool RenameDisplay( CDisplay* display, const std::string& newName );


    std::vector< CDisplay*> CreateDisplays4Operation( const COperation *operation, CMapDisplayData *dataList, bool split_plots );


    virtual void Dump( std::ostream& fOut = std::cerr ) override;

private:
	bool SaveConfigDisplay( std::string &errorMsg, CWorkspaceDisplay *wksd );
	bool LoadConfigDisplay( std::string &errorMsg, CWorkspaceDisplay *wksd, CWorkspaceOperation *wkso );
};



#endif		// WORKSPACES_WORKSPACE_H
