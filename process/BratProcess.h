/*
* Copyright (C) 2005 Collecte Localisation Satellites (CLS), France (31)
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
#include "Stl.h"

#include "List.h"
#include "FileParams.h"
#include "InternalFiles.h"

#include "ParametersDictionary.h"

#include "Product.h"
#include "ProductNetCdf.h"
#include "ProductNetCdfCF.h"
#include "Trace.h"
//#include "ProcessCommonTools.h"

using namespace brathl;
//using namespace processes;

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
public:

  CBratProcess();
    
  virtual ~CBratProcess();

  enum MergeDataMode {
	  pctFIRST,	// Must also be first of enum
	  pctMIN,
	  pctMAX,
	  pctMEAN,	
	  pctSTDDEV,
	  pctCOUNT,
	  pctSUM,
	  pctSUBSTRACT,
	  pctPRODUCT,
	  pctLAST		// Must also be last of enum
  };

	enum OutsideMode {
	  pctSTRICT,
	  pctRELAXED,
	  pctBLACK_HOLE
  };

  enum PositionMode {
	  pctEXACT,
	  pctNEAREST
  };


  CStringArray* GetInputFiles() { return &m_inputFiles; };
  void SetInputFiles(const CStringArray& value ) { m_inputFiles = value; };

  CExpression* GetSelect() { return &m_select; };
  void SetSelect(const CExpression& value ) { m_select = value; };

  int32_t GetNbFields() { return m_fields.size(); };

  bool CheckCommandLineOptions(int argc, char	**argv, const string& helpString, const KeywordHelp	*keywordList);  
  
  void ConstructProduct(const string& fileName, bool createVirtualField = true);
  void ConstructProduct(CStringArray& fileName, bool createVirtualField = true);
  void ConstructProduct(CStringList& fileName, bool createVirtualField = true);

  virtual bool Initialize(string& msg);
  virtual int32_t Execute(string& msg) = 0;

  void AdjustValidMinMax(double value);

  void IncrementOffsetValues(uint32_t incr = 1);
  void IncrementValue(CIntArray& vect, uint32_t incr = 1);


  bool IsProductNetCdf() { return CBratProcess::IsProductNetCdf(m_product); };
  bool IsProductNetCdfCF() { return CBratProcess::IsProductNetCdfCF(m_product); };
  
  
  CStringMap* GetFieldSpecificUnits() { return &m_fieldSpecificUnit; };

  string GetFieldSpecificUnit(const string& key);

  void SetFieldSpecificUnit(const string& key, const string& value);
  
  void SetFieldSpecificUnits(const CStringMap& fieldSpecificUnit);
  void SetFieldSpecificUnits(const CStringMap* fieldSpecificUnit);


public:

  static bool IsProductNetCdf(CBratObject* ob);
  static bool IsProductNetCdfCF(CBratObject* ob);
  
  static CProductNetCdf* GetProductNetCdf(CBratObject* ob, bool withExcept = true);
  static CProductNetCdfCF* GetProductNetCdfCF(CBratObject* ob, bool withExcept = true);
  
  static CObArrayOb* GetObArrayOb(CBratObject* ob, bool withExcept = true);
  static CDoubleArrayOb* GetDoubleArrayOb(CBratObject* ob, bool withExcept = true);


  static CBratProcess::MergeDataMode GetDataMode
		(CFileParams	&params,
		 int32_t	minOccurences	= 0,
		 int32_t	maxOccurences	= 1,
		 const string	&keyword	= "DATA_MODE",
		 int32_t	index		= 0,
		 MergeDataMode defaultValue	= pctMEAN);

  static CBratProcess::MergeDataMode GetDataMode
		(CFileParams	&params,
		 const string	&prefix,
		 int32_t	minOccurences	= 0,
		 int32_t	maxOccurences	= 1,
		 int32_t	index		= 0,
		 MergeDataMode defaultValue	= pctMEAN);

  static string DataModeStr (MergeDataMode mode);

  static int32_t GetFileList
		(CFileParams		&params,
		 const string		&keyword,
		 CStringArray		&names,
		 const string		&traceDescription,
		 int32_t		minOccurences	= 1,
		 int32_t		maxOccurences	= -1,
		 bool			printTrace	= true);


  void AdjustDefinition(CFileParams& params, CExpression& field, bool printTrace = true);


 int32_t GetDefinition
		(CFileParams		&params,
		 const string		&prefix,
		 CExpression    	&field,
		 string			*name,
		 NetCDFVarKind		*kind,
		 CUnit			*unit,
		 string			*title,
		 string			*comment,
		 string			*dataFormat,
		 uint32_t               *group,
 		 const string		&traceDescription,
     //CProduct* product,
		 int32_t		index		= 0,
		 int32_t		minOccurences	= 1,
		 int32_t		maxOccurences	= 1,
		 bool			printTrace	= true);
 

   int32_t GetDefinition
		(CFileParams		&params,
		 const string		&prefix,
		 CExpression    	&field,
		 string			*name,
		 NetCDFVarKind		*kind,
		 CUnit			*unit,
		 string			*title,
		 string			*comment,
		 string			*dataFormat,
		 uint32_t               *group,
		 double			&Min,
		 double			&Max,
		 uint32_t		&Count,
		 double			&Step,
		 const string		&traceDescription,
		 int32_t		index	= 0,
		 int32_t		minOccurences	= 1,
		 int32_t		maxOccurences	= 1,
		 bool			printTrace	= true);


 static int32_t GetVarDef
		(CFileParams		&params,
		 const string		&prefix,
		 CExpression    	&field,
		 string			*name,
		 NetCDFVarKind		*kind,
		 CUnit			*unit,
		 string			*title,
		 string			*comment,
		 string			*dataFormat,
		 uint32_t               *group,
 		 const string		&traceDescription,
		 int32_t		index		= 0,
		 int32_t		minOccurences	= 1,
		 int32_t		maxOccurences	= 1,
		 bool			printTrace	= true);

  static int32_t GetVarDef
		(CFileParams		&params,
		 const string		&prefix,
		 CExpression    	&field,
		 string			*name,
		 NetCDFVarKind		*kind,
		 CUnit			*unit,
		 string			*title,
		 string			*comment,
		 string			*dataFormat,
		 uint32_t               *group,
		 double			&Min,
		 double			&Max,
		 uint32_t		&Count,
		 double			&Step,
		 const string		&traceDescription,
		 int32_t		index	= 0,
		 int32_t		minOccurences	= 1,
		 int32_t		maxOccurences	= 1,
		 bool			printTrace	= true);
  
  static int32_t GetMergedDataSlices(CBratProcess::MergeDataMode mode);
  /*
  ** Checks the command line parameters (with -h -k or parameter file)
  ** and return true if command line is invalid
  ** Last entry of KeywordList MUST BE NULL or result is unpredictible
  ** (and maybe a core dump)
  */
  static bool CheckCommandLineOptions
		                (int			argc,
		                 char			**argv,
		                 const string		&helpString,
                     const KeywordHelp	*keywordList,
		                 string			&commandFileName);

  /*
  ** Print help about a list of keywords which can be foun in a parameter file
  ** definitions are in ParametersDictionary.h
  */
  static void PrintParameterHelp
		  (ostream		&where,
      const KeywordHelp	*keywordList);

  /*
  ** Internal function used to print keyword/type definition
  */
  static void PrintDescription
		  (ostream	&where,
		   const char	*description,
		   bool		firstLeading,
		   const char	*defaultValue);

  static void GetFilterDefinitions
		        (CFileParams		&params,
		         const string		&prefix,
		         bool			*smooth,
		         bool			*extrapolate,
		         int32_t		index	= 0,
		         int32_t		occurences = 1,
		         bool			printTrace = true);

  static void GetLoessCutoff
		        (CFileParams		&params,
		         int32_t		*xCutoff,
		         int32_t		*yCutoff,
		         int32_t		index	= 0,
		         int32_t		occurences = 1,
		         bool			printTrace = true);

  static CBratProcess::OutsideMode GetOutsideMode
		        (CFileParams	&params,
		         int32_t	minOccurences	= 0,
		         int32_t	maxOccurences	= 1,
		         const string	&keyword	= "OUTSIDE_MODE",
		         int32_t	index		= 0,
             OutsideMode	defaultValue		= CBratProcess::pctSTRICT);

  static string OutsideModeStr(CBratProcess::OutsideMode mode);

  // true if outside
  static bool CheckOutsideValue
		                (double		&value,
		                 double		min,
		                 double		max,
		                 double		step,
		                 CBratProcess::OutsideMode	mode);

  static CBratProcess::PositionMode GetPositionMode
		            (CFileParams	&params,
		             int32_t	minOccurences	= 0,
		             int32_t	maxOccurences	= 1,
		             const string	&keyword	= "POSITION_MODE",
		             int32_t	index		= 0,
                 PositionMode	defaultValue		= CBratProcess::pctNEAREST);

  static string PositionModeStr(CBratProcess::PositionMode mode);

  static bool CheckPositionValue
	                (double		value,
	                 double		min,
	                 double		step,
	                 uint32_t	count,
	                 CBratProcess::PositionMode	mode,
	                 uint32_t	&position);

  static double CheckLongitudeValue
		            (double			value,
		             double			min,
		             NetCDFVarKind	kind);

  static bool IsLongitudeCircular
		            (double			min,
		             double			max,
		             NetCDFVarKind	kind,
                 CUnit* unit,
                 double compareEpsilon = CTools::m_CompareEpsilon);


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

  static const CParameter::KWValueListEntry	DataModeKeywords[];
  static const CParameter::KWValueListEntry	FilterKeywords[];
  static const CParameter::KWValueListEntry	OutSideModeKeywords[];
  static const CParameter::KWValueListEntry	PositionModeKeywords[];

  static const int32_t NB_MAX_Y;

  static void MergeDataValue
		    (double& data,
		     double value,
		     double*	countValue,
		     double*	meanValue,
		     CBratProcess::MergeDataMode	mode);


  static void FinalizeMergingOfDataValues
		    (double& data,
		     double*	countValue,
		     double*	meanValue,
		     CBratProcess::MergeDataMode	mode);


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
  void LoadProductDictionary(const string& filename, bool createVirtualField = true);
  void ResizeArrayDependOnFields(uint32_t size);

  void NetCdfProductInitialization();
  void AddCritSelectionComplementDimensionsFromNetCdf();

  virtual void CheckDimensions(const CExpression& expr, CUIntArray& commonDimensions);


  void GetOrderedDimNames(const vector<CExpression>& fields, CStringArray& commonDimensions);
  void GetOrderedDimNames(const CExpression& expr, CStringArray& commonDimensions);
  void GetOrderedDimNames(const CStringArray& fieldNames, CStringArray& commonDimensionNames);
  void GetOrderedDimNames(const string& fieldName, CStringArray& commonDimensionNames);

  void GetFieldWithNoXYCommonDim();
  /*
  void GetNetCdfDimensions(const vector<CExpression>& expressions, CStringArray& commonDimNames);
  void GetNetCdfDimensions(const CExpression& expr, CStringArray& commonDimNames);
  void GetNetCdfDimensions(const CStringArray& fields, CStringArray& commonDimNames);
  */
  void SetExpandArray(const CExpression& expr, bool expandArray);
  void SetExpandArray(const CStringArray* fields, bool expandArray);

  static string GetIndexName(long nbElts);

  CObArray* GetFieldIndexes(const string& fieldName);

  virtual void AddFieldIndexes(uint32_t indexExpr, CNetCDFVarDef* varDef, const CUIntArray& commonDimensions);
  virtual void AddFieldIndexes(const string& fieldName, CNetCDFVarDef* varDef);
  virtual void AddFieldIndexes(CField* field, CNetCDFVarDef* varDef);
  virtual void AddFieldIndexes(CFieldIndex* fieldIndex, CNetCDFVarDef* varDef);

  virtual void GetParameters(const string &commandFileName) = 0;
  virtual void GetParameters() = 0;
  
  void GetSelectParameter(CFileParams& params);

  virtual void BuildListFieldsToRead();
  virtual void BeforeBuildListFieldsToRead();
  virtual void AfterBuildListFieldsToRead();

  virtual void ReplaceFieldDefinition();
  virtual void ReplaceAxisDefinition();
  
  virtual void SetDimCoordAxesToFieldWithoutDim(const CExpression& expr);
  virtual void SetDimCoordAxesToFieldWithoutDim(const CStringArray& fieldNames);

  //virtual void CheckNetCdfFields(CStringArray& fields);
  CExternalFilesNetCDF* OpenExternalFilesNetCDF(const string& fileName);

  virtual void AddDimensionsFromNetCdf(CStringArray& fields);
  virtual void AddVarsFromNetCdf();
  virtual bool IsOutputAxis(const string& name) { return false; };

  string GetExpressionNewName(const string& name);


  virtual void SubstituteAxisDim(const CStringArray& fieldDims, CStringArray& fieldDimsOut);
  virtual void OnAddDimensionsFromNetCdf();
  
  // Creates, opens and read parameters file, then get the list of input files.
  virtual void LoadParams(const string& name, uint32_t mode = CFile::modeRead|CFile::typeBinary);
  
  void CheckFileParams();

  /*
  ** Add a new value to data and computes the partial result
  ** according to the operation asked
  */
  void MergeDataValue
		    (double* data,
		     double* values,
		     uint32_t nbValues,
         uint32_t indexExpr,
		     double* countValues,
		     double* meanValues);

  void MergeDataValue
	      (double& data,
	       double value,
	       uint32_t nbValues,
         uint32_t indexExpr,
	       double* countValue,
	       double* meanValue);

  void FinalizeMergingOfDataValues
		    (double* data,
         uint32_t indexExpr,
         uint32_t nbValues,
		     double* countValues,
	  	   double* meanValues);


protected:

  CExpression	m_xField;
  NetCDFVarKind	m_xType;
  CUnit m_xUnit;
  string m_xName;
  string m_xTitle;
  string m_xComment;

  CExpression	m_yField;
  NetCDFVarKind	m_yType;
  CUnit m_yUnit;
  string m_yName;
  string m_yTitle;


  CStringArray m_inputFiles;
  CExpression m_select;

  vector<CExpression> m_fields;
  vector<NetCDFVarKind> m_types;
  vector<CUnit> m_units;
  
  CStringArray m_names;
  CStringArray m_titles;
  CStringArray m_comments;


  string m_outputFileName;
  string m_outputTitle;
  
  string m_recordName;

  CProduct* m_product;

  CStringList m_listFieldsToRead;
  
  CObMap m_mapFieldIndexesToRead;
  
  CArrayStringMap m_mapFieldIndexesByExpr;

  bool m_expandArray;

  MergeDataMode m_dataModeGlobal;
  vector<MergeDataMode> m_dataMode;

  string m_commandFileName;

  bool m_alwaysFalse;
  bool m_alwaysTrue;

  double m_validMin;
  double m_validMax;

  CInternalFiles* m_internalFiles;

  CStringArray m_netCdfAxisDims;
  CStringArray m_netCdfComplementDims;

  CIntArray m_countOffsets;
  CIntArray m_meanOffsets;

  uint32_t m_nbDataAllocated;

  CStringArray m_fieldDefinitionToReplace;

  CStringMap m_fieldSpecificUnit;
  
  CStringArray m_fieldWithNoXYCommonDim;

  CFileParams* m_fileParams;

protected:



};



}

#endif
