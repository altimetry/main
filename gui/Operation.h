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

#include "brathl.h"

#include "new-gui/brat/Workspaces/Dataset.h"
#include "new-gui/brat/Workspaces/Formula.h"
#include "Product.h" 


class CWorkspaceDataset;
class CWorkspaceOperation;



const std::string LOGFILE_EXTENSION = "log";
const std::string COMMANDFILE_EXTENSION = "par";
const std::string EXPORTASCII_COMMANDFILE_EXTENSION = "export.par";
const std::string EXPORTGEOTIFF_COMMANDFILE_EXTENSION = "geotiff_export.par";
const std::string SHOWSTAT_COMMANDFILE_EXTENSION = "stats.par";


class COperation : public CBratObject
{
protected:

	CProduct* m_product;
	CDataset* m_dataset;
	std::string m_record;
	//CFormula* m_formula;
	CFormula* m_select;
	CMapFormula m_formulas;
	int32_t m_type;
	int32_t m_dataMode;

	wxFileName m_output;
	wxFileName m_exportAsciiOutput;
	wxFileName m_showStatsOutput;

	wxFileName m_cmdFile;
	wxFileName m_exportAsciiCmdFile;
	wxFileName m_showStatsCmdFile;

	//wxFile m_file;

public:
	const int32_t m_verbose = 2;

protected:
	wxFileName m_logFile;

	bool m_exportAsciiDateAsPeriod;
	bool m_exportAsciiExpandArray;
	bool m_exportAsciiNoDataComputation;
	bool m_executeAgain;
	//bool m_delayExecution;

	int32_t m_exportAsciiNumberPrecision;

private:
	std::string m_name;

public:

	/// Empty COperation ctor
	COperation( const std::string name );

	COperation( COperation& o, CWorkspaceDataset *wks, CWorkspaceOperation *wkso );

	/// Destructor
	virtual ~COperation();

	std::string GetName() { return m_name; }
	void SetName( const std::string& value ) { m_name = value; }

	bool HasFormula() { return GetFormulaCount() > 0; }

	bool HasFilters();

	int32_t GetFormulaCount() { return m_formulas.size(); }
	int32_t GetFormulaCountDataFields();

	bool RenameFormula( CFormula* formula, const std::string &newName );

	bool SaveConfig( CConfiguration *config, CWorkspaceOperation *wkso );
	bool LoadConfig( CConfiguration *config, std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso );

	CDataset* FindDataset( const std::string& datasetName, CWorkspaceDataset *wks );

	std::string GetRecord() const { return m_record; }
	void SetRecord( const std::string& value ) { m_record = value; }

	int32_t GetType() const { return m_type; }
	void SetType( int32_t value ) { m_type = value; }

	std::string GetDataModeAsString() { return CMapDataMode::GetInstance().IdToName( m_dataMode ); }
	int32_t GetDataMode() { return m_dataMode; }
	void SetDataMode( int32_t value ) { m_dataMode = value; }

	CDataset* GetDataset() { return m_dataset; }
	const CDataset* GetDataset() const { return m_dataset; }
	void SetDataset( CDataset* value ) { m_dataset = value; }
	std::string GetDatasetName();

	CProduct* GetProduct() { return m_product; }
	void SetProduct( CProduct* value ) { m_product = value; }

	const CMapFormula* GetFormulas() const { return &m_formulas; }
	CMapFormula* GetFormulas() { return &m_formulas; }

	CFormula* GetFormula( const std::string& name );
	CFormula* GetFormula( int32_t type );
	CFormula* GetFormula( CMapFormula::iterator it );
	const CFormula* GetFormula( CMapFormula::const_iterator it );

	CFormula* NewUserFormula( CField* field, int32_t typeField, bool addToMap = true, CProduct* product = NULL );
	CFormula* NewUserFormula( const std::string& name = "", int32_t typeField = CMapTypeField::eTypeOpAsField, const std::string& strUnit = "", bool addToMap = true, CProduct* product = NULL );

	bool AddFormula( CFormula& value );
	bool DeleteFormula( const std::string& name );

	std::string GetDescFormula( const std::string& name, bool alias = false );
	//void SetDescFormula(const std::string& name, const std::string& value);

	const wxFileName* GetOutput() const { return &m_output; }
	std::string GetOutputName() const { return m_output.GetFullPath().ToStdString(); }
	std::string GetOutputNameRelativeToWks( CWorkspaceOperation *wks );

	void SetOutput( const wxFileName& value, CWorkspaceOperation* wks );
	void SetOutput( const std::string& value, CWorkspaceOperation* wks );

	const wxFileName* GetExportAsciiOutput() const { return &m_exportAsciiOutput; }
	std::string GetExportAsciiOutputName() { return m_exportAsciiOutput.GetFullPath().ToStdString(); }
	std::string GetExportAsciiOutputNameRelativeToWks( CWorkspaceOperation *wks );

	void SetExportAsciiOutput( const wxFileName& value, CWorkspaceOperation* wks );
	void SetExportAsciiOutput( const std::string& value, CWorkspaceOperation* wks );


	const wxFileName* GetShowStatsOutput() const { return &m_showStatsOutput; }
	std::string GetShowStatsOutputName() { return m_showStatsOutput.GetFullPath().ToStdString(); }
	std::string GetShowStatsOutputNameRelativeToWks( CWorkspaceOperation *wks );

	void SetShowStatsOutput( const wxFileName& value, CWorkspaceOperation* wks );
	void SetShowStatsOutput( const std::string& value, CWorkspaceOperation* wks  );

	std::string GetTaskName();
	wxFileName* GetCmdFile() { return &m_cmdFile; }
	std::string GetCmdFileFullName() { return m_cmdFile.GetFullPath().ToStdString(); }
	void SetCmdFile( CWorkspaceOperation* wks );

	std::string GetExportAsciiTaskName();
	wxFileName* GetExportAsciiCmdFile() { return &m_exportAsciiCmdFile; }
	std::string GetExportAsciiCmdFileFullName() { return m_exportAsciiCmdFile.GetFullPath().ToStdString(); }
	void SetExportAsciiCmdFile( CWorkspaceOperation *wks );

	std::string GetShowStatsTaskName();
	wxFileName* GetShowStatsCmdFile() { return &m_showStatsCmdFile; }
	std::string GetShowStatsCmdFileFullName() { return m_showStatsCmdFile.GetFullPath().ToStdString(); }
	void SetShowStatsCmdFile( CWorkspaceOperation *wks );


	std::string GetCommentFormula( const std::string& name );
	void SetCommentFormula( const std::string &name, const std::string &value );

	const CFormula* GetSelect() const { return m_select; }
	CFormula* GetSelect() { return m_select; }
	void SetSelect( CFormula* value );

	bool IsYFX();
	bool IsZFXY();
	bool IsMap();

	bool IsSelect( CFormula* value );
	static bool IsSelect( const std::string& name );

	std::string GetSelectName();
	std::string GetSelectDescription();
	//  void SetSelectDesc(const std::string& value);

	std::string GetFormulaNewName();
	std::string GetFormulaNewName( const std::string& prefix );

	wxFileName* GetSystemCommand();
	std::string GetFullCmd();

	wxFileName* GetExportAsciiSystemCommand();
	std::string GetExportAsciiFullCmd();

	wxFileName* GetShowStatsSystemCommand();
	std::string GetShowStatsFullCmd();


	bool IsExportAsciiDateAsPeriod() const { return m_exportAsciiDateAsPeriod; }
	void SetExportAsciiDateAsPeriod( bool value ) { m_exportAsciiDateAsPeriod = value; }

	bool IsExportAsciiExpandArray() const { return m_exportAsciiExpandArray; }
	void SetExportAsciiExpandArray( bool value ) { m_exportAsciiExpandArray = value; }

	bool IsExportAsciiNoDataComputation() const { return m_exportAsciiNoDataComputation; }
	void SetExportAsciiNoDataComputation( bool value ) { m_exportAsciiNoDataComputation = value; }

	int32_t GetExportAsciiNumberPrecision() const { return m_exportAsciiNumberPrecision; }
	void SetExportAsciiNumberPrecision( int32_t value ) { m_exportAsciiNumberPrecision = value; }

	bool IsExecuteAgain() { return m_executeAgain; }
	void SetExecuteAgain( bool value ) { m_executeAgain = value; }

	//bool IsDelayExecution() {return m_delayExecution;}
	//void SetDelayExecution(bool value) {m_delayExecution = value;}

	bool CtrlLoessCutOff( std::string &msg );

	bool UseDataset( const std::string& name );

	bool RemoveOutput();
	bool RenameOutput( const std::string& oldName );

	bool BuildCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso, std::string &errorMsg );
	bool BuildShowStatsCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso );
	bool BuildExportAsciiCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso );
private:
	//bool BuildCmdFileHeader();
	//bool BuildCmdFileGeneralProperties();
	//bool BuildCmdFileAlias();
	//bool BuildCmdFileSpecificUnit();
	//bool BuildCmdFileDataset();
	//bool BuildCmdFileFromOutputOperation();
	//bool BuildCmdFileFields();
	//bool BuildCmdFileFieldsSpecificZFXY( CFormula* value );
	//bool BuildCmdFileOutput();
	//bool BuildCmdFileSelect();
	//bool BuildCmdFileVerbose();

	//bool BuildShowStatsCmdFileHeader();
	//bool BuildShowStatsCmdFileOutput();
	//bool BuildShowStatsCmdFileFields();

	//bool BuildExportAsciiCmdFileHeader();
	//bool BuildExportAsciiCmdFileGeneralProperties();
	//bool BuildExportAsciiCmdFileAlias();
	//bool BuildExportAsciiCmdFileDataset();
	//bool BuildExportAsciiCmdFileFields();
	//bool BuildExportAsciiCmdFileOutput();
	//bool BuildExportAsciiCmdFileSelect();
	//bool BuildExportAsciiCmdFileVerbose();


	//bool WriteComment( const std::string& text );
	//bool WriteLine( const std::string& text );
	//bool WriteEmptyLine();

public:

	void InitOutput( CWorkspaceOperation *wks );
	void InitExportAsciiOutput( CWorkspaceOperation *wks );
	void InitShowStatsOutput( CWorkspaceOperation *wks );

	bool ComputeInterval( const std::string& formulaName, std::string &errorMsg );
	bool ComputeInterval( CFormula* f, std::string &errorMsg );

	bool ControlDimensions( CFormula* formula, std::string &errorMsg, const CStringMap* aliases = NULL );
	bool ControlResolution( std::string &errorMsg );
	bool Control( CWorkspaceFormula *wks, std::string& msg, bool basicControl = false, const CStringMap* aliases = NULL );

	bool GetXExpression( CExpression& expr, std::string& errorMsg, const CStringMap* aliases = NULL );
	bool GetYExpression( CExpression& expr, std::string& errorMsg, const CStringMap* aliases = NULL );

	bool ControlXYDataFields( std::string &errorMsg, const CStringMap* aliases = NULL );

	void DeleteSelect();

	void ClearLogFile();

	void SetLogFile( const std::string& value );
	void SetLogFile( CWorkspaceOperation *wks );

	const wxFileName* GetLogFile() const { return &m_logFile; }

	//COperation& operator=( COperation& o );

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr );

protected:
	void Init();
	void Copy( COperation& o, CWorkspaceDataset *wks, CWorkspaceOperation *wkso );
};

/** @} */


#endif // !defined(WORKSPACES_OPERATION_H)
