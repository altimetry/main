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
#include "new-gui/brat/ApplicationPaths.h"

#include "../MapTypeDisp.h"


class CWorkspaceDataset;
class CWorkspaceOperation;
class CBratFilter;
namespace brathl 
{
	class CInternalFiles;
};





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




class CCmdFile : protected CLogFile
{
	using base_t = CLogFile;

public:
	CCmdFile( const std::string &path ) 
		: base_t( false, path )
	{}

	virtual ~CCmdFile()
	{}

	
	///////////////////////////////////////////////////

	void WriteLn( const std::string& text = "" )
	{
		LogLn( text );
	}

	void Comment( const std::string& text )
	{
		assert__( !!mFile );

		WriteLn( "#" + text );
	}
};



class COperation : public CBratObject, public non_copyable
{
	//types

	friend class CWorkspaceSettings;
	friend class COperationCmdFile;

public:
	using const_iterator = std::map< std::string, CBratObject* >::const_iterator;

	enum EExecutionType
	{
		eOperation,
		eExportNetCDF,
		eExportASCII,
		eExportGeoTIFF,
		eStatistics,

		EExecutionType_size
	};


	//static data

protected:
	static std::string m_execYFXName;
	static std::string m_execZFXYName;
	static std::string m_execExportAsciiName;
	static std::string m_execExportGeoTiffName;
	static std::string m_execShowStatsName;
	static std::string m_execBratSchedulerName;


	//instance data

//	CProduct *m_product = nullptr;							//v4.1	m_product -> mFilteredProduct
	CProduct *mFilteredProduct = nullptr;					//v4.1	m_product -> mFilteredProduct
	CDataset *mFilteredDataset = nullptr;	   				//v4.1: mDataset -> mFilteredDataset
	const CDataset *mOriginalDataset = nullptr;				//v4
	const CBratFilter *mFilter = nullptr;					//v4
	std::string m_record;									//CFormula* m_formula;
	CFormula *mSelectionCriteria = nullptr;
	CMapFormula m_formulas;
	CMapTypeOp::ETypeOp m_type = CMapTypeOp::eTypeOpYFX;

	std::string m_output;
	std::string m_exportAsciiOutput;
	std::string m_exportNetcdfOutput;
	std::string m_exportGeoTIFFOutput;
	std::string m_showStatsOutput;

	std::string m_cmdFile;
	std::string m_exportAsciiCmdFile;
	std::string m_exportGeoTIFFCmdFile;
	std::string m_showStatsCmdFile;

	std::string mScheduledTaskName;
	std::string mScheduledExportNetcdfTaskName;
	std::string mScheduledExportAsciiTaskName;
	std::string mScheduledExportGeoTIFFTaskName;

public:
	const int32_t m_verbose = 2;

protected:
	std::string m_logFile;

	bool mCreateKMLTrackData = false;
    bool mCreateKMLFieldsData = false;
	bool mCreateGeoTIFFs = true;
	std::string mColorTable;
#if defined(BRAT_V3)
	double mGeoTIFFRangeMin = defaultValue<double>();
	double mGeoTIFFRangeMax = defaultValue<double>();
#endif
	std::string mBratLogoPath;

	bool m_exportAsciiDateAsPeriod = false;
	bool m_exportAsciiExpandArray = false;
	bool m_exportAsciiNoDataComputation = false;
	bool m_executeAgain = false;

	int32_t m_exportAsciiNumberPrecision = defaultValue<int32_t>();

private:
	std::string m_name;

	//construction / destruction 

public:

	COperation( const std::string name ) : m_name( name )
	{
		CreateSelectionCriteria();
	}

	static COperation* Copy( const COperation &o, CWorkspaceOperation *wkso, CWorkspaceDataset *wksd );

	void Clear();

	CFormula* CreateSelectionCriteria( CFormula *f = nullptr )
	{
		delete mSelectionCriteria;
		mSelectionCriteria = f ? new CFormula( *f ) : new CFormula( ENTRY_SELECT, false );
		mSelectionCriteria->SetName( ENTRY_SELECT );
		mSelectionCriteria->SetFieldType( CMapTypeField::eTypeOpAsSelect );
		return mSelectionCriteria;
	}

	virtual ~COperation()
	{
		delete mSelectionCriteria;
		delete mFilteredDataset;
	}


	//remaining...

	virtual const_iterator begin() const { return m_formulas.begin(); }

	virtual const_iterator end() const { return m_formulas.end(); }

	bool IsRadsDataset() const;

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

	static CDataset* FindDataset( const std::string& datasetName, CWorkspaceDataset *wks );

	std::string GetRecord() const { return m_record; }
	void SetRecord( const std::string& value ) { m_record = value; }

	CMapTypeOp::ETypeOp GetType() const { return m_type; }
	void SetType( CMapTypeOp::ETypeOp value ) { m_type = value; }


#if defined(BRAT_V3)
	CDataset* Dataset() { return mDataset; }
#else
protected:
#endif

	bool CreateFilteredDataset( std::string &error_msg, CProgressInterface *progress );
	bool RemoveFilteredDataset();
	void CopyFilteredDatasetAndProduct( const COperation &o );
	CDataset* NewFilteredDataset() const;

public:
	const CDataset* OriginalDataset() const { return mOriginalDataset; }
	std::string OriginalDatasetName() const;
	bool SetOriginalDataset( const CWorkspaceDataset *wks, const std::string dataset_name, std::string &error_msg );
	void RemoveOriginalDataset();

	std::pair< bool, const CDataset* > FilteredDataset( std::string &error_msg, CProgressInterface *progress );
	bool HasFilteredDataset() const { return mFilteredDataset; }

	const CBratFilter* Filter() const { return mFilter; }
	std::string FilterName() const;
	bool SetFilter( const CBratFilter *filter, std::string &error_msg );
	void RemoveFilter();	//always succeeds
	bool ReapplyFilter();	//obsolete from v4.0: simply calls RemoveFilteredDataset

	//void SetProduct( CProduct* value ) { m_product = value; }

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
	void RemoveFormulas();

	CFormula* NewUserFormula( std::string &error_msg, CField* field, CMapTypeField::ETypeField typeField, bool addToMap = true, const CProductInfo &pi = CProductInfo::smInvalidProduct );
	CFormula* NewUserFormula( std::string &error_msg, const std::string& name = "", CMapTypeField::ETypeField typeField = CMapTypeField::eTypeOpAsField, 
		const std::string& strUnit = "", bool addToMap = true, const CProductInfo &pi = CProductInfo::smInvalidProduct );

	bool AddFormula( CFormula& value, std::string &error_msg );
	bool DeleteFormula( const std::string& name );

	std::string GetDescFormula( const std::string& name, bool alias = false );
	//void SetDescFormula(const std::string& name, const std::string& value);

	//v4: formerly static void CDisplay::GetDisplayType( const COperation* operation, CUIntArray& displayTypes, CInternalFiles** pf = nullptr );
	//
	std::vector< CMapTypeDisp::ETypeDisp > GetDisplayTypes( CInternalFiles** pf = nullptr ) const;


	void GetExportGeoTiffProperties( bool &createKMLFields, bool &createKMLTrack, bool &createGeoTIFFs, std::string &colorTable )
	{
		createKMLFields = mCreateKMLFieldsData;
		createKMLTrack = mCreateKMLTrackData;
		createGeoTIFFs = mCreateGeoTIFFs;
		colorTable = mColorTable;
//#if defined(BRAT_V3)
//		rangeMin = mGeoTIFFRangeMin;
//		rangeMax = mGeoTIFFRangeMax;
//#endif
	}

	void SetExportGeoTiffProperties( bool createKMLFields, bool createKMLTrack, bool createGeoTIFFs, std::string colorTable, const std::string &logo_path )
	{
		mCreateKMLFieldsData = createKMLFields;
		mCreateKMLTrackData = createKMLTrack;
		mCreateGeoTIFFs = createGeoTIFFs;
		mColorTable = colorTable;
//#if defined(BRAT_V3)
//		mGeoTIFFRangeMin = rangeMin;
//		mGeoTIFFRangeMax = rangeMax;
//#endif
		mBratLogoPath = logo_path;
	}

	// output paths ///////////////////////////////////////////////////////////////////////
	//
	void InitOutputs( CWorkspaceOperation *wks );
	void InitOperationOutput( CWorkspaceOperation *wks );
	void InitExportAsciiOutput( CWorkspaceOperation *wks );
	void InitExportNetcdfOutput( CWorkspaceOperation *wks );
	void InitExportGeoTIFFOutput( CWorkspaceOperation *wks );
	void InitShowStatsOutput( CWorkspaceOperation *wks );

	std::string GetOutputPath( EExecutionType type ) const
	{
		switch ( type )
		{
			case eOperation:
				return GetOutputPath();
			case eExportNetCDF:
				return GetExportNetcdfOutputPath();
			case eExportASCII:
				return GetExportAsciiOutputPath();
			case eExportGeoTIFF:
				return GetExportGeoTIFFOutputPath();
			case eStatistics:
				return GetShowStatsOutputPath();
			default:
				assert__( false );
		}
		return empty_string<std::string>();
	}


	const std::string& GetOutputPath() const { return m_output; }				//femm: GetOutputName -> GetOutputPath; old body: {return m_output.GetFullPath();};
	void SetOutput( const std::string& value, CWorkspaceOperation* wks );
	std::string GetOutputPathRelativeToWks( const CWorkspaceOperation *wks ) const;

    const std::string& GetExportNetcdfOutputPath() const { return m_exportNetcdfOutput; }
	void SetExportNetcdfOutput( const std::string& value, CWorkspaceOperation* wks );
	std::string GetExportNetcdfOutputPathRelativeToWks( const CWorkspaceOperation *wks ) const;

    const std::string& GetExportAsciiOutputPath() const { return m_exportAsciiOutput; }
	void SetExportAsciiOutput( const std::string& value, CWorkspaceOperation* wks );
	std::string GetExportAsciiOutputPathRelativeToWks( const CWorkspaceOperation *wks ) const;

	const std::string& GetExportGeoTIFFOutputPath() const { return m_exportGeoTIFFOutput; }
	void SetExportGeoTIFFOutput( const std::string& value, CWorkspaceOperation* wks );
	std::string GetExportGeoTIFFOutputPathRelativeToWks( const CWorkspaceOperation *wks ) const;

	const std::string& GetShowStatsOutputPath() const { return m_showStatsOutput; }
	void SetShowStatsOutput( const std::string& value, CWorkspaceOperation* wks );
	std::string GetShowStatsOutputPathRelativeToWks( const CWorkspaceOperation *wks ) const;
	//
	///////////////////////////////////////////////////////////////////////////////////////


public:
	// build command files ////////////////////////////////////////////////////////////////
	//
	bool BuildCmdFile( EExecutionType type, CWorkspaceFormula *wks, CWorkspaceOperation *wkso, std::string &error_msg, CProgressInterface *progress )
	{
		switch ( type )
		{
			case eOperation:
				return BuildCmdFile( wks, wkso, error_msg, progress );
			case eExportNetCDF:
				assert__( false );
				break;
			case eExportASCII:
				return BuildExportAsciiCmdFile( wks, wkso, error_msg );
			case eExportGeoTIFF:
				return BuildExportGeoTIFFCmdFile( wks, wkso, error_msg );
			case eStatistics:
				return BuildShowStatsCmdFile( wks, wkso, error_msg );
			default:
				assert__( false );
		}
		return false;
	}
protected:
	bool BuildCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso, std::string &error_msg, CProgressInterface *progress );
	bool BuildExportAsciiCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso, std::string &error_msg );
	bool BuildExportGeoTIFFCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso, std::string &error_msg );
	bool BuildShowStatsCmdFile( CWorkspaceFormula *wks, CWorkspaceOperation *wkso, std::string &error_msg );
	//
	///////////////////////////////////////////////////////////////////////////////////////

public:
	// tasks names ////////////////////////////////////////////////////////////////////////
	//
	std::string GetTaskName( EExecutionType type, bool delayed = false ) const
	{
		switch ( type )
		{
			case eOperation:
			{
				if ( !delayed || mScheduledTaskName.empty() )
					return GetTaskName();
				else
					return mScheduledTaskName;
			}
			break;

			case eExportNetCDF:
			{
				if ( !delayed || mScheduledExportNetcdfTaskName.empty() )
					return GetExportNetcdfTaskName();
				else
					return mScheduledExportNetcdfTaskName;
			}
			break;
												   
			case eExportASCII:
			{
				if ( !delayed || mScheduledExportAsciiTaskName.empty() )
					return GetExportAsciiTaskName();
				else
					return mScheduledExportAsciiTaskName;
			}
			break;

			case eExportGeoTIFF:
			{
				if ( !delayed || mScheduledExportGeoTIFFTaskName.empty() )
					return GetExportGeoTIFFTaskName();
				else
					return mScheduledExportGeoTIFFTaskName;
			}
			break;

			case eStatistics:
			{
				return GetShowStatsTaskName();
			}
			break;

			default:
				assert__( false );
		}
		return empty_string<std::string>();
	}

	void SetScheduledTaskName( const std::string &name ){ mScheduledTaskName = name; }
	void SetScheduledExportNetcdfTaskName( const std::string &name ){ mScheduledExportNetcdfTaskName = name; }
	void SetScheduledExportAsciiTaskName( const std::string &name ){ mScheduledExportAsciiTaskName = name; }
	void SetScheduledExportGeoTIFFTaskName( const std::string &name ){ mScheduledExportGeoTIFFTaskName = name; }

protected:
	std::string GetTaskName() const;
	std::string GetExportAsciiTaskName() const;
	std::string GetExportNetcdfTaskName() const;
	std::string GetExportGeoTIFFTaskName() const;
	std::string GetShowStatsTaskName() const;

public:
	// command files //////////////////////////////////////////////////////////////////////
	//
	const std::string& GetCmdFile( EExecutionType type ) const 
	{
		switch ( type )
		{
			case eOperation:
				return GetCmdFile();
			case eExportNetCDF:
				assert__( false );
				break;
			case eExportASCII:
				return GetExportAsciiCmdFile();
			case eExportGeoTIFF:
				return GetExportGeoTIFFCmdFile();
			case eStatistics:
				return GetShowStatsCmdFile();
			default:
				assert__( false );
		}
		return empty_string<std::string>();
	}	
protected:
	const std::string& GetCmdFile() const { return m_cmdFile; }	
	void SetCmdFile( CWorkspaceOperation* wks );

	const std::string& GetExportAsciiCmdFile() const { return m_exportAsciiCmdFile; }
	void SetExportAsciiCmdFile( CWorkspaceOperation *wks );

	const std::string& GetExportGeoTIFFCmdFile() const { return m_exportGeoTIFFCmdFile; }
	void SetExportGeoTIFFCmdFile( CWorkspaceOperation *wks );

	const std::string& GetShowStatsCmdFile() const { return m_showStatsCmdFile; }
	void SetShowStatsCmdFile( CWorkspaceOperation *wks );
	//
	///////////////////////////////////////////////////////////////////////////////////////

public:
	// full command lines /////////////////////////////////////////////////////////////////
	//
	std::string GetFullCmd( EExecutionType type ) const
	{
		switch ( type )
		{
			case eOperation:
				return GetFullCmd();
			case eExportNetCDF:
				assert__( false );
				break;
			case eExportASCII:
				return GetExportAsciiFullCmd();
			case eExportGeoTIFF:
				return GetExportGeoTIFFFullCmd();
			case eStatistics:
				return GetShowStatsFullCmd();
			default:
				assert__( false );
		}
		return empty_string<std::string>();
	}	

protected:
	const std::string& GetSystemCommand() const;
	std::string GetFullCmd() const;

	const std::string& GetExportAsciiSystemCommand() const;
	std::string GetExportAsciiFullCmd() const;

	std::string GetExportGeoTIFFFullCmd() const;

	const std::string& GetShowStatsSystemCommand() const;
	std::string GetShowStatsFullCmd() const;
	//
	///////////////////////////////////////////////////////////////////////////////////////


public:
	std::string GetCommentFormula( const std::string& name ) const;
	void SetCommentFormula( const std::string &name, const std::string &value );

	const CFormula* GetSelect() const { return mSelectionCriteria; }
	CFormula* GetSelect() { return mSelectionCriteria; }
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


	bool UseDataset( const std::string& name );

	bool RemoveOutput();
	bool RenameOutput( const std::string& oldPath );

public:

	//bool ComputeInterval( const std::string& formulaName, std::string &error_msg );
	bool ComputeInterval( std::string &error_msg );
	bool ComputeInterval( CFormula* f, std::string &error_msg );

	bool Control( CWorkspaceFormula *wks, std::string& msg, const CStringMap* aliases = nullptr );

private:
	bool CtrlLoessCutOff( std::string &msg );
	bool ControlDimensions( const CProductInfo &pi, CFormula* formula, std::string &error_msg, const CStringMap* aliases = nullptr );
	bool ControlResolution( std::string &error_msg );

	bool GetXExpression_NOT_USED( const CProductInfo &pi, CExpression& expr, std::string& error_msg, const CStringMap* aliases = nullptr ) const;
	bool GetYExpression_NOT_USED( const CProductInfo &pi, CExpression& expr, std::string& error_msg, const CStringMap* aliases = nullptr ) const;

	bool ControlXYDataFields_NOT_USED( std::string &error_msg, const CStringMap* aliases = nullptr );

public:

	void ClearLogFile();

	void SetLogFile( const std::string& value );
	void SetLogFile( CWorkspaceOperation *wks );

	const std::string& GetLogFile() const { return m_logFile; }

#if defined(BRAT_V3)

	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr );
#endif
};

/** @} */


#endif // !defined(WORKSPACES_OPERATION_H)
