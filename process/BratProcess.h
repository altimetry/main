/*
* 
*
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
#if !defined(_BratProcess_h_)
#define _BratProcess_h_

#include <cstdio>
#include <cstdlib>

#include "libbrathl/List.h"
#include "libbrathl/FileParams.h"
#include "libbrathl/InternalFiles.h"

#include "ParametersDictionary.h"

#include "libbrathl/Product.h"
#include "libbrathl/ProductNetCdf.h"
#include "libbrathl/ProductNetCdfCF.h"
#include "common/tools/Trace.h"
//#include "ProcessCommonTools.h"

using namespace brathl;
//using namespace processes;



const std::string CHECKED_DATASET_CMD_LINE_PARAMETER = "-rads";

const std::string DATA_MODE_SUFFIX = "_DATA_MODE";
const std::string DATA_MODE_DI_TIME_NAME_SUFFIX = "_DATA_MODE_DI_TIME_NAME";
const std::string DATA_MODE_DI_DATE_TIME_SUFFIX = "_DATA_MODE_DI_DATE_TIME";


namespace processes
{

#ifndef MIN
#define MIN(X,Y)	((X) <= (Y) ? (X) : (Y))
#endif

#ifndef MAX
#define MAX(X,Y)	((X) >= (Y) ? (X) : (Y))
#endif







//-------------------------------------------------------------
//------------------- CBratProcess class --------------------
//-------------------------------------------------------------

	class CBratProcess : public CBratObject
	{
		// types

        DECLARE_BASE_TYPE( CBratObject )

	public:

		//...MergeDataMode

		enum EMergeDataMode {
			pctFIRST,		// Must also be first of enum (v3 note)
			pctMIN,
			pctMAX,
			pctMEAN,
			pctSTDDEV,
			pctCOUNT,
			pctSUM,
			pctSUBSTRACT,
			pctPRODUCT,
			pctTIME,		// Dataset Interpolation
			pctLAST,		// Must also be last of enum (v3 note)

			eMergeDataMode_size
		};


	private:
		static const std::string* DataModeNames()
		{
			static std::string names_list[ eMergeDataMode_size ];

			names_list[ pctFIRST ] =		"FIRST";
			names_list[ pctMIN ] =			"MIN";
			names_list[ pctMAX ] =			"MAX";
			names_list[ pctMEAN ] =			"MEAN";
			names_list[ pctSTDDEV ] =		"STDDEV";
			names_list[ pctCOUNT ] =		"COUNT";
			names_list[ pctSUM ] =			"SUM";
			names_list[ pctSUBSTRACT ] =	"SUBTRACTION";
			names_list[ pctPRODUCT ] =		"PRODUCT";
			names_list[ pctTIME ] =			"TIME";
			names_list[ pctLAST ] =			"LAST";

			return names_list;
		}

	public:

		static const std::string& DataModeStr( EMergeDataMode mode )
		{
			static const std::string* names_list = DataModeNames();		//call DataModeNames only once
			static const std::vector< std::string > names_vector( &names_list[pctFIRST], &names_list[pctLAST + 1] );

			assert__( names_vector.size() == eMergeDataMode_size );

			assert__( mode >= pctFIRST && mode <= pctLAST );

			return names_vector[ mode ];
		}

	private:
		static const CParameter::KWValueListEntry* DataModeKeywords()
		{
			static const CParameter::KWValueListEntry list[ eMergeDataMode_size + 1 ] =	//+1: null terminator
			{
				{ DataModeStr( pctFIRST ).c_str(),		pctFIRST },
				{ DataModeStr( pctMIN ).c_str(),		pctMIN },
				{ DataModeStr( pctMAX ).c_str(),		pctMAX },
				{ DataModeStr( pctMEAN ).c_str(),		pctMEAN },
				{ DataModeStr( pctSTDDEV ).c_str(),		pctSTDDEV },
				{ DataModeStr( pctCOUNT ).c_str(),		pctCOUNT },
				{ DataModeStr( pctSUM ).c_str(),		pctSUM },
				{ DataModeStr( pctSUBSTRACT ).c_str(),	pctSUBSTRACT },
				{ DataModeStr( pctPRODUCT ).c_str(),	pctPRODUCT },
				{ DataModeStr( pctTIME ).c_str(),		pctTIME },
				{ DataModeStr( pctLAST ).c_str(),		pctLAST },
				{ nullptr, 0 }
			};

			return list;
		}

	public:


		//...OutsideMode

		enum OutsideMode {
			pctSTRICT,
			pctRELAXED,
			pctBLACK_HOLE
		};


		//...PositionMode

		enum PositionMode {
			pctEXACT,
			pctNEAREST
		};


		// instance data

	protected:

		CExpression	m_xField;
		NetCDFVarKind	m_xType;
		CUnit m_xUnit;
		std::string m_xName;
		std::string m_xTitle;
		std::string m_xComment;

		CExpression	m_yField;
		NetCDFVarKind	m_yType;
		CUnit m_yUnit;
		std::string m_yName;
		std::string m_yTitle;


		CStringArray m_inputFiles;
		CExpression m_select;

		std::vector<CExpression> m_fields;
		std::vector<NetCDFVarKind> m_types;
		std::vector<CUnit> m_units;

		CStringArray m_names;
		CStringArray m_titles;
		CStringArray m_comments;


		std::string m_outputFileName;
		std::string m_outputTitle;

		std::string m_recordName;

		CProduct* m_product;

		CStringList m_listFieldsToRead;

		CObMap m_mapFieldIndexesToRead;

		CArrayStringMap m_mapFieldIndexesByExpr;

		bool m_expandArray;

		EMergeDataMode m_dataModeGlobal;
		std::vector< EMergeDataMode > m_dataMode;
		std::vector< std::string > mDataInterpolationTimeFieldName;
		std::vector< CDate > mDataInterpolationDateTime;

		std::string m_commandFileName;

		bool m_alwaysFalse;
		bool m_alwaysTrue;

		double m_validMin;
		double m_validMax;

		CInternalFiles* m_internalFiles;

		CStringArray m_netCdfAxisDims;
		CStringArray m_netCdfComplementDims;

		CIntArray m_countOffsets;
		CIntArray m_meanOffsets;
        CIntArray m_weightOffsets;

        double m_tParam;
        double m_dParam;
        double m_tFactor;
        double m_dFactor;

		uint32_t m_nbDataAllocated;

		CStringArray m_fieldDefinitionToReplace;

		CStringMap m_fieldSpecificUnit;

		CStringArray m_fieldWithNoXYCommonDim;

		CFileParams* m_fileParams;

        bool mCheckedDataset;


		// construction / destruction

	public:

		CBratProcess();

		virtual ~CBratProcess();


		// ...

		CStringArray* GetInputFiles() { return &m_inputFiles; };
		void SetInputFiles( const CStringArray& value ) { m_inputFiles = value; };

		CExpression* GetSelect() { return &m_select; };
		void SetSelect( const CExpression& value ) { m_select = value; };

		size_t GetNbFields() { return m_fields.size(); };

		bool CheckCommandLineOptions( int argc, char	**argv, const std::string& helpString, const KeywordHelp	*keywordList );

		void ConstructProduct( const std::string& fileName, bool createVirtualField = true );
		void ConstructProduct( CStringArray& fileName, bool createVirtualField, bool check_only_first_files = false );
		void ConstructProduct( CStringList& fileName, bool createVirtualField = true );

		virtual bool Initialize( std::string& msg );
		virtual int32_t Execute( std::string& msg ) = 0;

		void AdjustValidMinMax( double value );

		void IncrementOffsetValues( uint32_t incr = 1 );
		void IncrementValue( CIntArray& vect, uint32_t incr = 1 );


		bool IsProductNetCdf() { return CBratProcess::IsProductNetCdf( m_product ); };
		bool IsProductNetCdfCF() { return CBratProcess::IsProductNetCdfCF( m_product ); };


		CStringMap* GetFieldSpecificUnits() { return &m_fieldSpecificUnit; };

		std::string GetFieldSpecificUnit( const std::string& key );

		void SetFieldSpecificUnit( const std::string& key, const std::string& value );

		void SetFieldSpecificUnits( const CStringMap& fieldSpecificUnit );
		void SetFieldSpecificUnits( const CStringMap* fieldSpecificUnit );


	public:

		static bool IsProductNetCdf( CBratObject* ob );
		static bool IsProductNetCdfCF( CBratObject* ob );

		static CProductNetCdf* GetProductNetCdf( CBratObject* ob, bool withExcept = true );
		static CProductNetCdfCF* GetProductNetCdfCF( CBratObject* ob, bool withExcept = true );

		static CObArrayOb* GetObArrayOb( CBratObject* ob, bool withExcept = true );
		static CDoubleArrayOb* GetDoubleArrayOb( CBratObject* ob, bool withExcept = true );


		static EMergeDataMode GetDataMode
		( CFileParams	&params,
			int32_t	minOccurences	= 0,
			int32_t	maxOccurences	= 1,
			const std::string	&keyword	= "DATA_MODE",
			int32_t	index		= 0,
			EMergeDataMode defaultValue	= pctMEAN );

		static EMergeDataMode GetDataMode
		( CFileParams	&params,
			const std::string	&prefix,
			int32_t	minOccurences	= 0,
			int32_t	maxOccurences	= 1,
			int32_t	index		= 0,
			EMergeDataMode defaultValue	= pctMEAN );

		static std::string GetDataModeDTTimeName
		( CFileParams	&params,
			const std::string	&prefix,
			int32_t	minOccurences	= 0,
			int32_t	maxOccurences	= 1,
			int32_t	index		= 0 );

		static CDate GetDataModeDTDateTime
		( CFileParams	&params,
			const std::string	&prefix,
			int32_t	minOccurences	= 0,
			int32_t	maxOccurences	= 1,
			int32_t	index		= 0 );

		static int32_t GetFileList
		( CFileParams		&params,
			const std::string		&keyword,
			CStringArray		&names,
			const std::string		&traceDescription,
			int32_t		minOccurences	= 1,
			int32_t		maxOccurences	= -1,
			bool			printTrace	= true );


		void AdjustDefinition( CFileParams& params, CExpression& field, bool printTrace = true );


		int32_t GetDefinition
		( CFileParams		&params,
			const std::string		&prefix,
			CExpression    	&field,
			std::string			*name,
			NetCDFVarKind		*kind,
			CUnit			*unit,
			std::string			*title,
			std::string			*comment,
			std::string			*dataFormat,
			uint32_t               *group,
			const std::string		&traceDescription,
			//CProduct* product,
			int32_t		index		= 0,
			int32_t		minOccurences	= 1,
			int32_t		maxOccurences	= 1,
			bool			printTrace	= true );


		int32_t GetDefinition
		( CFileParams		&params,
			const std::string		&prefix,
			CExpression    	&field,
			std::string			*name,
			NetCDFVarKind		*kind,
			CUnit			*unit,
			std::string			*title,
			std::string			*comment,
			std::string			*dataFormat,
			uint32_t               *group,
			double			&Min,
			double			&Max,
			uint32_t		&Count,
			double			&Step,
			const std::string		&traceDescription,
			int32_t		index	= 0,
			int32_t		minOccurences	= 1,
			int32_t		maxOccurences	= 1,
			bool			printTrace	= true );


		static int32_t GetVarDef
		( CFileParams		&params,
			const std::string		&prefix,
			CExpression    	&field,
			std::string			*name,
			NetCDFVarKind		*kind,
			CUnit			*unit,
			std::string			*title,
			std::string			*comment,
			std::string			*dataFormat,
			uint32_t               *group,
			const std::string		&traceDescription,
			int32_t		index		= 0,
			int32_t		minOccurences	= 1,
			int32_t		maxOccurences	= 1,
			bool			printTrace	= true );

		static int32_t GetVarDef
		( CFileParams		&params,
			const std::string		&prefix,
			CExpression    	&field,
			std::string			*name,
			NetCDFVarKind		*kind,
			CUnit			*unit,
			std::string			*title,
			std::string			*comment,
			std::string			*dataFormat,
			uint32_t               *group,
			double			&Min,
			double			&Max,
			uint32_t		&Count,
			double			&Step,
			const std::string		&traceDescription,
			int32_t		index	= 0,
			int32_t		minOccurences	= 1,
			int32_t		maxOccurences	= 1,
			bool			printTrace	= true );

		static int32_t GetMergedDataSlices( EMergeDataMode mode );
		/*
		** Checks the command line parameters (with -h -k or parameter file)
		** and return true if command line is invalid
		** Last entry of KeywordList MUST BE NULL or result is unpredictible
		** (and maybe a core dump)
		*/
		static bool CheckCommandLineOptions
		( int			argc,
			char			**argv,
			const std::string		&helpString,
			const KeywordHelp	*keywordList,
			std::string			&commandFileName,
			bool &checked_dataset
		);

		/*
		** Print help about a list of keywords which can be foun in a parameter file
		** definitions are in ParametersDictionary.h
		*/
		static void PrintParameterHelp
		( std::ostream		&where,
			const KeywordHelp	*keywordList );

		/*
		** Internal function used to print keyword/type definition
		*/
		static void PrintDescription
		( std::ostream	&where,
			const char	*description,
			bool		firstLeading,
			const char	*defaultValue );

		static void GetFilterDefinitions
		( CFileParams		&params,
			const std::string		&prefix,
			bool			*smooth,
			bool			*extrapolate,
			int32_t		index	= 0,
			int32_t		occurences = 1,
			bool			printTrace = true );

		static void GetLoessCutoff
		( CFileParams		&params,
			int32_t		*xCutoff,
			int32_t		*yCutoff,
			int32_t		index	= 0,
			int32_t		occurences = 1,
			bool			printTrace = true );

		static CBratProcess::OutsideMode GetOutsideMode
		( CFileParams	&params,
			int32_t	minOccurences	= 0,
			int32_t	maxOccurences	= 1,
			const std::string	&keyword	= "OUTSIDE_MODE",
			int32_t	index		= 0,
			OutsideMode	defaultValue		= CBratProcess::pctSTRICT );

		static std::string OutsideModeStr( CBratProcess::OutsideMode mode );

		// true if outside
		static bool CheckOutsideValue
		( double		&value,
			double		min,
			double		max,
			double		step,
			CBratProcess::OutsideMode	mode );

		static CBratProcess::PositionMode GetPositionMode
		( CFileParams	&params,
			int32_t	minOccurences	= 0,
			int32_t	maxOccurences	= 1,
			const std::string	&keyword	= "POSITION_MODE",
			int32_t	index		= 0,
			PositionMode	defaultValue		= CBratProcess::pctNEAREST );

		static std::string PositionModeStr( CBratProcess::PositionMode mode );

		static bool CheckPositionValue
		( double		value,
			double		min,
			double		step,
			uint32_t	count,
			CBratProcess::PositionMode	mode,
			uint32_t	&position );

		static double CheckLongitudeValue
		( double			value,
			double			min,
			NetCDFVarKind	kind );

		static bool IsLongitudeCircular
		( double			min,
			double			max,
			NetCDFVarKind	kind,
			CUnit* unit,
			double compareEpsilon = CTools::m_CompareEpsilon );


	public:

		static const char* PCT_HeaderFmt;
		static const char* PCT_HeaderCountFmt;
		static const char* PCT_StrFmt;
		static const char* PCT_FltFmt;
		static const char* PCT_IntFmt;
		static const char* PCT_QStrFmt;
		static const char* PCT_StrFmt2;
		static const char* PCT_FltFmt2;
		static const char* PCT_IntFmt2;
		static const char* PCT_QStrFmt2;
		static const char* PCT_FltFmt2BaseUnit;

		static const CParameter::KWValueListEntry	FilterKeywords[];
		static const CParameter::KWValueListEntry	OutSideModeKeywords[];
		static const CParameter::KWValueListEntry	PositionModeKeywords[];

		static const int32_t NB_MAX_Y;

		static void MergeDataValue
		( double& data,
			double value,
			double*	countValue,
			double*	meanValue,
            double* weightValue,
			EMergeDataMode	mode );


		static void FinalizeMergingOfDataValues
		( double& data,
			double*	countValue,
			double*	meanValue,
            double* weightValue,
			EMergeDataMode	mode );


		/*
		** Value used to distinguish invalid/missing data (known as default value
		** and set by set by CTools::SetDefaultValue) and the fact that no data
		** has yet been taken into account.
		*/
		static const double MergeIdentifyUnsetData;



	protected:

		void Init();
		void DeleteProduct();
		void DeleteInternalFile();
		void DeleteFileParams();
		void LoadProductDictionary( const std::string& filename, bool createVirtualField = true );
		void ResizeArrayDependOnFields( uint32_t size );

		void NetCdfProductInitialization();
		void AddCritSelectionComplementDimensionsFromNetCdf();

		virtual void CheckDimensions( const CExpression& expr, CUIntArray& commonDimensions );


		void GetOrderedDimNames( const std::vector<CExpression>& fields, CStringArray& commonDimensions );
		void GetOrderedDimNames( const CExpression& expr, CStringArray& commonDimensions );
		void GetOrderedDimNames( const CStringArray& fieldNames, CStringArray& commonDimensionNames );
		void GetOrderedDimNames( const std::string& fieldName, CStringArray& commonDimensionNames );

		void GetFieldWithNoXYCommonDim();
		/*
		void GetNetCdfDimensions(const std::vector<CExpression>& expressions, CStringArray& commonDimNames);
		void GetNetCdfDimensions(const CExpression& expr, CStringArray& commonDimNames);
		void GetNetCdfDimensions(const CStringArray& fields, CStringArray& commonDimNames);
		*/
		void SetExpandArray( const CExpression& expr, bool expandArray );
		void SetExpandArray( const CStringArray* fields, bool expandArray );

		static std::string GetIndexName( long nbElts );

		CObArray* GetFieldIndexes( const std::string& fieldName );

		virtual void AddFieldIndexes( uint32_t indexExpr, CNetCDFVarDef* varDef, const CUIntArray& commonDimensions );
		virtual void AddFieldIndexes( const std::string& fieldName, CNetCDFVarDef* varDef );
		virtual void AddFieldIndexes( CField* field, CNetCDFVarDef* varDef );
		virtual void AddFieldIndexes( CFieldIndex* fieldIndex, CNetCDFVarDef* varDef );

		virtual void GetParameters( const std::string &commandFileName ) = 0;
		virtual void GetParameters() = 0;

		void GetSelectParameter( CFileParams& params );

		virtual void BuildListFieldsToRead();
		virtual void BeforeBuildListFieldsToRead();
		virtual void AfterBuildListFieldsToRead();

		virtual void ReplaceFieldDefinition();
		virtual void ReplaceAxisDefinition();

		virtual void SetDimCoordAxesToFieldWithoutDim( const CExpression& expr );
		virtual void SetDimCoordAxesToFieldWithoutDim( const CStringArray& fieldNames );

		//virtual void CheckNetCdfFields(CStringArray& fields);
		CExternalFilesNetCDF* OpenExternalFilesNetCDF( const std::string& fileName );

		virtual void AddDimensionsFromNetCdf( CStringArray& fields );
		virtual void AddVarsFromNetCdf();
		virtual bool IsOutputAxis( const std::string& ) { return false; };

		std::string GetExpressionNewName( const std::string& name );


		virtual void SubstituteAxisDim( const CStringArray& fieldDims, CStringArray& fieldDimsOut );
		virtual void OnAddDimensionsFromNetCdf();

		// Creates, opens and read parameters file, then get the list of input files.
		virtual void LoadParams( const std::string& name, uint32_t mode = CFile::modeRead | CFile::typeBinary );

		void CheckFileParams();

		/*
		** Add a new value to data and computes the partial result
		** according to the operation asked
		*/
		void MergeDataValue
		( double* data,
			double* values,
			uint32_t nbValues,
			uint32_t indexExpr,
			double* countValues,
            double* meanValues,
            double* weightValues);

		//void MergeDataValue
		//( double& data,
		//	double value,
		//	uint32_t nbValues,
		//	uint32_t indexExpr,
		//	double* countValue,
		//	double* meanValue );

		void FinalizeMergingOfDataValues
		( double* data,
			uint32_t indexExpr,
			uint32_t nbValues,
			double* countValues,
            double* meanValues,
            double* weightValues);

	};

}

#endif
