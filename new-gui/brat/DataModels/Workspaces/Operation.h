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
#if !defined(WORKSPACES_OPERATION_H)
#define WORKSPACES_OPERATION_H

#include <fstream>

#include "libbrathl/brathl.h"
#include "libbrathl/Product.h" 

#include "Dataset.h"
#include "Formula.h"
#include "new-gui/Common/ApplicationPaths.h"

class CWorkspaceDataset;
class CWorkspaceOperation;
class CBratFilter;



const std::string LOGFILE_EXTENSION = "log";
const std::string COMMANDFILE_EXTENSION = "par";
const std::string EXPORTASCII_COMMANDFILE_EXTENSION = "export.par";
const std::string EXPORTGEOTIFF_COMMANDFILE_EXTENSION = "geotiff_export.par";
const std::string SHOWSTAT_COMMANDFILE_EXTENSION = "stats.par";


#if !defined(BRAT_V3)

#include "new-gui/Common/QtUtilsIO.h"

inline std::string GetFileName( const std::string &path )
{
	return GetBaseFilenameFromPath( path );
}

inline void SetFileExtension( std::string &path, const std::string &extension )
{
	std::string dir = GetDirectoryFromPath( path );
	std::string name = GetFileName( path );
	path = dir + "/" + name + "." + extension;
}

inline std::string NormalizedAbsolutePath( const std::string &path, const std::string &dir )
{
	std::string d = NormalizedPath( dir );
	std::string f = NormalizedPath( path );
	if ( QDir::isAbsolutePath( t2q( f ) ) )
		return f;
	else
		return dir + "/" + path;
}

#endif



inline std::string& clean_path( std::string &path )
{
	return path;
}
inline std::string normalize( std::string &path, const std::string &dir )
{
	path = NormalizedAbsolutePath( path, dir );
	return path;
}




class CCmdFile : non_copyable
{
	std::fstream mFile;

public:
	CCmdFile( const std::string &path ) :
		mFile( path, std::ios::binary | std::ios::out | std::ios::trunc )
	{}

	virtual ~CCmdFile()
	{
		mFile.close();		// just in case
	}

	bool IsOk() const { return !!mFile; }

	
	///////////////////////////////////////////////////

	void WriteLn( const std::string& text = "" )
	{
		assert__( !!mFile );

		mFile << text << "\n";
	}

	void Comment( const std::string& text )
	{
		assert__( !!mFile );

		WriteLn( "#" + text );
	}
};



class COperation : public CBratObject, public non_copyable
{
	friend class CWorkspaceSettings;

public:
	using const_iterator = std::map< std::string, CBratObject* >::const_iterator;

protected:
	static std::string m_execYFXName;
	static std::string m_execZFXYName;
	static std::string m_execExportAsciiName;
	static std::string m_execExportGeoTiffName;
	static std::string m_execShowStatsName;
	static std::string m_execBratSchedulerName;


	CProduct *m_product = nullptr;
	CDataset *mDataset = nullptr;
	const CDataset *mOriginalDataset = nullptr;				//v4
	const CBratFilter *mFilter = nullptr;					//v4
	std::string m_record;									//CFormula* m_formula;
	CFormula *m_select = new CFormula(ENTRY_SELECT, false);
	CMapFormula m_formulas;
	CMapTypeOp::ETypeOp m_type = CMapTypeOp::eTypeOpYFX;
	int32_t m_dataMode = CMapDataMode::GetInstance().GetDefault();

	std::string m_output;
	std::string m_exportAsciiOutput;
	std::string m_showStatsOutput;

	std::string m_cmdFile;
	std::string m_exportAsciiCmdFile;
	std::string m_showStatsCmdFile;

public:
	const int32_t m_verbose = 2;

protected:
	std::string m_logFile;

	bool m_exportAsciiDateAsPeriod = false;
	bool m_exportAsciiExpandArray = false;
	bool m_exportAsciiNoDataComputation = false;
	bool m_executeAgain = false;

	int32_t m_exportAsciiNumberPrecision = defaultValue<int32_t>();

private:
	std::string m_name;

public:

	COperation( const std::string name ) : m_name( name )
	{
		m_select->SetFieldType( CMapTypeField::eTypeOpAsSelect );
	}

	static COperation* Copy( const COperation &o, CWorkspaceOperation *wkso, CWorkspaceDataset *wksd );

	void Clear();


	/// Destructor
	virtual ~COperation()
	{
		delete m_select;
		delete mDataset;
	}

	virtual const_iterator begin() const { return m_formulas.begin(); }

	virtual const_iterator end() const { return m_formulas.end(); }

    static void SetExecNames( const CApplicationPaths &appPath );
	static const std::string& GetExecYFXName() { return m_execYFXName; }
	static const std::string& GetExecZFXYName() { return m_execZFXYName; }
	static const std::string& GetExecExportAsciiName() { return m_execExportAsciiName; }
	static const std::string& GetExecExportGeoTiffName() { return m_execExportGeoTiffName; }
	static const std::string& GetExecShowStatsName() { return m_execShowStatsName; }
	static const std::string& GetExecBratSchedulerName() { return m_execBratSchedulerName; }

	const std::string& GetName() const { return m_name; }
	void SetName( const std::string& value ) { m_name = value; }

	bool HasFormula() const { return GetFormulaCount() > 0; }
	bool HasV3Filters() const { return m_formulas.HasFilters(); }

	size_t GetFormulaCount() const { return m_formulas.size(); }
	size_t GetFormulaCountDataFields();

	bool RenameFormula( CFormula* formula, const std::string &newName );

	bool SaveConfig( CWorkspaceSettings *config, const CWorkspaceOperation *wks ) const;
	bool LoadConfig( CWorkspaceSettings *config, std::string &error_msg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso );

	CDataset* FindDataset( const std::string& datasetName, CWorkspaceDataset *wks );

	std::string GetRecord() const { return m_record; }
	void SetRecord( const std::string& value ) { m_record = value; }

	CMapTypeOp::ETypeOp GetType() const { return (CMapTypeOp::ETypeOp)m_type; }
	void SetType( int32_t value ) { SetType( (CMapTypeOp::ETypeOp)value ); }
	void SetType( CMapTypeOp::ETypeOp value ) { m_type = value; }

	std::string GetDataModeAsString() { return CMapDataMode::GetInstance().IdToName( m_dataMode ); }
	int32_t GetDataMode() const { return m_dataMode; }
	void SetDataMode( int32_t value ) { m_dataMode = value; }

#if !defined(BRAT_V3)
protected:
#endif

	CDataset* Dataset() { return mDataset; }
	void SetDataset();

public:
	const CDataset* OriginalDataset() const { return mOriginalDataset; }
	std::string OriginalDatasetName() const;
	const CDataset* Dataset() const { return mDataset; }
	void SetDataset( const CDataset *dataset );
	const CBratFilter* Filter() const { return mFilter; }
	std::string FilterName() const;
	void SetFilter( const CBratFilter *filter );
	void RemoveFilter();
	void ReapplyFilter();

	CProduct* GetProduct() { return m_product; }
	void SetProduct( CProduct* value ) { m_product = value; }

	const CMapFormula* GetFormulas() const { return &m_formulas; }
	CMapFormula* GetFormulas() { return &m_formulas; }

	CFormula* GetFormula( const std::string& name ) const;
	CFormula* GetFormula( int32_t type );
	const CFormula* GetFormula( int32_t type ) const
	{
		return const_cast<COperation*>( this )->GetFormula( type );
	}
	CFormula* GetFormula( CMapFormula::iterator it );
	const CFormula* GetFormula( CMapFormula::const_iterator it ) const;

	CFormula* NewUserFormula( std::string &error_msg, CField* field, int32_t typeField, bool addToMap = true, const CProduct *product = nullptr );
	CFormula* NewUserFormula( std::string &error_msg, const std::string& name = "", int32_t typeField = CMapTypeField::eTypeOpAsField, 
		const std::string& strUnit = "", bool addToMap = true, const CProduct *product = nullptr );

	bool AddFormula( CFormula& value, std::string &error_msg );
	bool DeleteFormula( const std::string& name );

	std::string GetDescFormula( const std::string& name, bool alias = false );
	//void SetDescFormula(const std::string& name, const std::string& value);

	// output paths ///////////////////////////////////////////////////////////////////////
	//
	const std::string& GetOutputPath() const { return m_output; }				//femm: GetOutputName -> GetOutputPath; old body: {return m_output.GetFullPath();};
	void SetOutput( const std::string& value, CWorkspaceOperation* wks );
	std::string GetOutputPathRelativeToWks( const CWorkspaceOperation *wks ) const;

    const std::string& GetExportAsciiOutputPath() const { return m_exportAsciiOutput; }
	void SetExportAsciiOutput( const std::string& value, CWorkspaceOperation* wks );
	std::string GetExportAsciiOutputPathRelativeToWks( const CWorkspaceOperation *wks ) const;

	const std::string& GetShowStatsOutputPath() const { return m_showStatsOutput; }
	void SetShowStatsOutput( const std::string& value, CWorkspaceOperation* wks );
	std::string GetShowStatsOutputPathRelativeToWks( const CWorkspaceOperation *wks ) const;
	//
	///////////////////////////////////////////////////////////////////////////////////////


	std::string GetTaskName() const;
	const std::string& GetCmdFile() const { return m_cmdFile; }	
	void SetCmdFile( CWorkspaceOperation* wks );

	std::string GetExportAsciiTaskName() const;
	const std::string& GetExportAsciiCmdFile() const { return m_exportAsciiCmdFile; }
	void SetExportAsciiCmdFile( CWorkspaceOperation *wks );

	std::string GetShowStatsTaskName() const;
	const std::string& GetShowStatsCmdFile() const { return m_showStatsCmdFile; }
	void SetShowStatsCmdFile( CWorkspaceOperation *wks );


	std::string GetCommentFormula( const std::string& name ) const;
	void SetCommentFormula( const std::string &name, const std::string &value );

	const CFormula* GetSelect() const { return m_select; }
	CFormula* GetSelect() { return m_select; }
	void SetSelect( CFormula* value );

	bool IsYFX() const;
	bool IsZFXY() const;
	bool IsMap() const;

	bool IsSelect( const CFormula* value ) const;
	static bool IsSelect( const std::string& name )
	{
		return str_icmp( name, ENTRY_SELECT );
	}

	std::string GetSelectName() const;
	std::string GetSelectDescription() const;
	//  void SetSelectDesc(const std::string& value);

	std::string GetFormulaNewName() const;
	std::string GetFormulaNewName( const std::string& prefix ) const;

	const std::string& GetSystemCommand() const;
	std::string GetFullCmd() const;

	const std::string& GetExportAsciiSystemCommand() const;
	std::string GetExportAsciiFullCmd();

	const std::string& GetShowStatsSystemCommand() const;
	std::string GetShowStatsFullCmd() const;


	bool IsExportAsciiDateAsPeriod() const { return m_exportAsciiDateAsPeriod; }
	void SetExportAsciiDateAsPeriod( bool value ) { m_exportAsciiDateAsPeriod = value; }

	bool IsExportAsciiExpandArray() const { return m_exportAsciiExpandArray; }
	void SetExportAsciiExpandArray( bool value ) { m_exportAsciiExpandArray = value; }

	bool IsExportAsciiNoDataComputation() const { return m_exportAsciiNoDataComputation; }
	void SetExportAsciiNoDataComputation( bool value ) { m_exportAsciiNoDataComputation = value; }

	int32_t GetExportAsciiNumberPrecision() const { return m_exportAsciiNumberPrecision; }
	void SetExportAsciiNumberPrecision( int32_t value ) { m_exportAsciiNumberPrecision = value; }

	bool IsExecuteAgain() const { return m_executeAgain; }
	void SetExecuteAgain( bool value ) { m_executeAgain = value; }

	bool CtrlLoessCutOff( std::string &msg );

	bool UseDataset( const std::string& name );

	bool RemoveOutput();
	bool RenameOutput( const std::string& oldPath );

	bool BuildCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso, std::string &error_msg );
	bool BuildShowStatsCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso );
	bool BuildExportAsciiCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso );

public:
	void InitOutput( CWorkspaceOperation *wks );
	void InitExportAsciiOutput( CWorkspaceOperation *wks );
	void InitShowStatsOutput( CWorkspaceOperation *wks );

	//bool ComputeInterval( const std::string& formulaName, std::string &error_msg );
	bool ComputeInterval( std::string &error_msg );
	bool ComputeInterval( CFormula* f, std::string &error_msg );

	bool ControlDimensions( CFormula* formula, std::string &error_msg, const CStringMap* aliases = nullptr );
	bool ControlResolution( std::string &error_msg );
	bool Control( CWorkspaceFormula *wks, std::string& msg, bool basicControl = false, const CStringMap* aliases = nullptr );

	bool GetXExpression( CExpression& expr, std::string& error_msg, const CStringMap* aliases = nullptr ) const;
	bool GetYExpression( CExpression& expr, std::string& error_msg, const CStringMap* aliases = nullptr ) const;

	bool ControlXYDataFields( std::string &error_msg, const CStringMap* aliases = nullptr );

	void ClearLogFile();

	void SetLogFile( const std::string& value );
	void SetLogFile( CWorkspaceOperation *wks );

	const std::string& GetLogFile() const { return m_logFile; }

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr );
};

/** @} */


#endif // !defined(WORKSPACES_OPERATION_H)
