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

#include <cmath>
#include <cstdlib>

#include "common/tools/Exception.h"
#include "FileParams.h"
#include "InternalFilesZFXY.h"

#include "Field.h"
#include "BratProcess.h"
#include "BratProcessZFXY.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;
using namespace processes;

namespace processes
{

const KeywordHelp CBratProcessZFXY::ZFXYKeywordList[]	= {
	KeywordHelp(kwFILE,		1, 0),
	KeywordHelp(kwOUTPUT),
	KeywordHelp(kwOUTPUT_TITLE,	0, 1, "\"\""),
	KeywordHelp(kwSELECT,		0, 0, "1"),
	KeywordHelp(kwRECORD),
	KeywordHelp(kwDATA_MODE,	0, 1, "MEAN"),
	KeywordHelp(kwPOSITION_MODE,	0, 1, "NEAREST"),
	KeywordHelp(kwOUTSIDE_MODE,	0, 1, "STRICT"),
	KeywordHelp(kwX),
	KeywordHelp(kwX_NAME),
	KeywordHelp(kwX_TYPE),
	KeywordHelp(kwX_UNIT),
	KeywordHelp(kwX_TITLE),
	KeywordHelp(kwX_INTERVALS,	1, 1, "180 for lat 360 for lon"),
	KeywordHelp(kwX_MIN,		1, 1, "-90 for lat, -180 for lon"),
	KeywordHelp(kwX_MAX,		1, 1, "90 for lat, 180 for lon"),
	KeywordHelp(kwX_LOESS_CUTOFF,	1, 1, "0"),
	KeywordHelp(kwY),
	KeywordHelp(kwY_INTERVALS,	1, 1, "180 for lat 360 for lon"),
	KeywordHelp(kwY_NAME),
	KeywordHelp(kwY_TYPE),
	KeywordHelp(kwY_UNIT),
	KeywordHelp(kwY_TITLE),
	KeywordHelp(kwY_MIN,		1, 1, "-90 for lat, -180 for lon"),
	KeywordHelp(kwY_MAX,		1, 1, "90 for lat, 180 for lon"),
	KeywordHelp(kwY_LOESS_CUTOFF,	1, 1, "0"),
	KeywordHelp(kwFIELD,		1, base_t::NB_MAX_Y, NULL, 24),
	KeywordHelp(kwFIELD_NAME,	-24),
	KeywordHelp(kwFIELD_DATA_MODE,	-24),
	KeywordHelp(kwFIELD_TYPE,	-24),
	KeywordHelp(kwFIELD_UNIT,	-24),
	KeywordHelp(kwFIELD_TITLE,	-24),
	KeywordHelp(kwFIELD_FILTER,	-24),
	KeywordHelp(kwALIAS_NAME,	0, 0, "None", 14),
	KeywordHelp(kwALIAS_VALUE,	-14),
	KeywordHelp(kwUNIT_ATTR_NAME,	0, 0, "None", 14),
	KeywordHelp(kwUNIT_ATTR_VALUE,	-14),
	KeywordHelp(kwVERBOSE,		0, 1, "0"),
 	KeywordHelp(kwLOGFILE,		0, 1, ""),
	KeywordHelp(""),
};

//-------------------------------------------------------------
//------------------- CBratProcessZFXY class --------------------
//-------------------------------------------------------------

CBratProcessZFXY::CBratProcessZFXY( const std::string &python_dir )
	: base_t( python_dir )
{
  Init();
}
//----------------------------------------
    
CBratProcessZFXY::~CBratProcessZFXY()
{

  DeleteData();

}
//----------------------------------------
void CBratProcessZFXY::Init()
{
  //m_nbMaxDataSlices = 1;
  m_nbDataAllocated = 0;

  setDefaultValue(m_xMin);
  setDefaultValue(m_xMax);
  setDefaultValue(m_xCount);
  setDefaultValue(m_xStep);
  setDefaultValue(m_xLoessCutoff);
  m_xCircular = false;

  setDefaultValue(m_yMin);
  setDefaultValue(m_yMax);
  setDefaultValue(m_yCount);
  setDefaultValue(m_yStep);
  setDefaultValue(m_yLoessCutoff);
  m_yCircular = false;

  m_positionMode = base_t::pctNEAREST;
  m_outsideMode = base_t::pctSTRICT;

  setDefaultValue(m_nbDataByGrid);

  m_xData = NULL;
  m_yData = NULL;


}

//----------------------------------------
void CBratProcessZFXY::DeleteData()
{
	delete m_xData;
	m_xData = NULL;

	delete m_yData;
	m_yData = NULL;
}

//----------------------------------------
void CBratProcessZFXY::ResizeArrayDependOnFields(uint32_t size)
{
  base_t::ResizeArrayDependOnFields(size);

  m_smooth.resize(size);
  m_extrapolate.resize(size);
    
}

//----------------------------------------
void CBratProcessZFXY::AddFieldIndexes(CFieldIndex* fieldIndex, CNetCDFVarDef* varDef)
{
  if (varDef == NULL)
  {
    return;
  }

  if (fieldIndex == NULL)
  {
    return;
  }

  base_t::AddFieldIndexes(fieldIndex, varDef);

  /*
  IncrementOffsetValues();

  m_countOffsets.push_back(-1);
  m_meanOffsets.push_back(-1);

  //-----------------
  m_nbDataAllocated++;
  //-----------------
*/
}
//----------------------------------------
void CBratProcessZFXY::GetParameters(const std::string& commandFileName)
{
  m_commandFileName = commandFileName;
  GetParameters();
}
//----------------------------------------
void CBratProcessZFXY::GetParameters()
{
	LoadParams( m_commandFileName );
	CheckFileParams();

	CFileParams& params = *m_fileParams;

	// Verify keyword occurrences
	uint32_t nbFields = params.CheckCount( kwFIELD, 1, base_t::NB_MAX_Y );

	params.CheckCount( kwOUTPUT );
	params.CheckCount( kwRECORD );


	// Get keyword values

	  //CTrace *p =
	CTrace::GetInstance();

	//---------

	params.m_mapParam[ kwOUTPUT ]->GetValue( m_outputFileName );
	CTrace::Tracer( 1, base_t::PCT_StrFmt, "Output file", m_outputFileName.c_str() );

	m_internalFiles = new CInternalFilesZFXY( m_outputFileName.c_str(), Replace );
	m_internalFiles->Open();

	//---------

	if ( params.CheckCount( kwOUTPUT_TITLE, 0 ) == 1 )
	{
		params.m_mapParam[ kwOUTPUT_TITLE ]->GetValue( m_outputTitle );
	}

	CTrace::Tracer( 1, base_t::PCT_QStrFmt, "Output title", m_outputTitle.c_str() );

	//---------

	params.m_mapParam[ kwRECORD ]->GetValue( m_recordName );
	CTrace::Tracer( 1, base_t::PCT_StrFmt, "Data set name", m_recordName.c_str() );

	//---------

	if ( params.CheckCount( kwEXPAND_ARRAY, 0, 1 ) == 1 )
	{
		params.m_mapParam[ kwEXPAND_ARRAY ]->GetValue( m_expandArray );
	}
	CTrace::Tracer( 1, PCT_IntFmt, "Expand array", m_expandArray );

	//---------

	GetSelectParameter( params );

	//---------

	m_dataModeGlobal	= GetDataMode( params );
	CTrace::Tracer( 1, base_t::PCT_StrFmt, "Default data mode", base_t::DataModeStr( m_dataModeGlobal ).c_str() );

	//---------

	m_positionMode	= base_t::GetPositionMode( params );
	CTrace::Tracer( 1, base_t::PCT_StrFmt, "Position mode", base_t::PositionModeStr( m_positionMode ).c_str() );

	//---------

	m_outsideMode	= base_t::GetOutsideMode( params );
	CTrace::Tracer( 1, base_t::PCT_StrFmt, "Outside mode", base_t::OutsideModeStr( m_outsideMode ).c_str() );

	//---------

	//If XMin/XMax and YMin/YMax are always expressed in base unit.
	//If XMin/XMax and YMin/YMax account for a date, they are expressed in number of seconds since 1950-01-01 00;:00:00.0
  /*
	base_t::GetVarDef(params,
				kwX,
				m_xField,
				&m_xName,
				&m_xType,
				&m_xUnit,
				&m_xTitle,
				&m_xComment,
				NULL,
				NULL,
				m_xMin,
				m_xMax,
				m_xCount,
				m_xStep,
				"X Data");
  */
	uint32_t hack = 0;
	this->GetDefinition( params,
		kwX,
		m_xField,
		&m_xName,
		&m_xType,
		&m_xUnit,
		&m_xTitle,
		&m_xComment,
		NULL,
		NULL,
		m_xMin,
		m_xMax,
		hack,
		m_xStep,
		"X Data" );
	m_xCount = hack;

	m_xCircular	= base_t::IsLongitudeCircular( m_xMin, m_xMax, m_xType, &m_xUnit, CLatLonPoint::m_LONGITUDE_COMPARE_EPSILON );

	this->GetDefinition( params,
		kwY,
		m_yField,
		&m_yName,
		&m_yType,
		&m_yUnit,
		&m_yTitle,
		&m_yComment,
		NULL,
		NULL,
		m_yMin,
		m_yMax,
		hack,
		m_yStep,
		"Y Data" );
	m_yCount = hack;

	m_yCircular	= base_t::IsLongitudeCircular( m_yMin, m_yMax, m_yType, &m_yUnit, CLatLonPoint::m_LONGITUDE_COMPARE_EPSILON );

	ResizeArrayDependOnFields( nbFields );

	bool oneFilter	= false;

	for ( uint32_t index = 0; index < nbFields; index++ )
	{
		this->GetDefinition( params,
			kwFIELD,
			m_fields[ index ],
			&m_names[ index ],
			&m_types[ index ],
			&m_units[ index ],
			&m_titles[ index ],
			&m_comments[ index ],
			NULL,
			NULL,
			"Value",
			index,
			nbFields );

		bool smoothTmp;
		bool extrapolateTmp;

		base_t::GetFilterDefinitions( params, kwFIELD, &smoothTmp, &extrapolateTmp, index, nbFields );

		m_smooth[ index ]	= smoothTmp;

		m_extrapolate[ index ]	= extrapolateTmp;

		oneFilter	|= smoothTmp || extrapolateTmp;

		m_dataMode[ index ] = GetDataMode( params, kwFIELD, 0, nbFields, index, m_dataModeGlobal );
		if ( m_dataMode[ index ] == pctTIME )
		{
			mDataInterpolationTimeFieldName[ index ] = GetDataModeDITimeName( params, kwFIELD, 0, nbFields, index );
			mDataInterpolationDateTime[ index ] = GetDataModeDIDateTime( params, kwFIELD, 0, nbFields, index );
			mDataInterpolationDistanceWeighting[ index ] = GetDataModeDIDistanceWeighting( params, kwFIELD, 0, nbFields, index );
			mDataInterpolationTimeWeighting[ index ] = GetDataModeDITimeWeighting( params, kwFIELD, 0, nbFields, index );
		}
	}


	if ( oneFilter )
	{
		base_t::GetLoessCutoff( params, &m_xLoessCutoff, &m_yLoessCutoff );
	}



	CTrace::Tracer( 1, "Allocating and initialising working structures" );

	m_nbDataByGrid	= m_xCount * m_yCount;

	int32_t nbDataSlices = 0;
	uint32_t accruedDataSlices = 0;

	for ( uint32_t index = 0; index < nbFields; index++ )
	{
		nbDataSlices = GetMergedDataSlices( m_dataMode[ index ] );	//default is 1; 2 for pctMEAN and pctTIME; 3 for pctSTDDEV

		m_countOffsets[ index ] = -1;
		m_meanOffsets[ index ] = -1;

		switch ( nbDataSlices )
		{
			//-------
			case 2:
				//-------
				m_countOffsets[ index ] = nbFields + accruedDataSlices;
				break;
				//-------
			case 3:
				//-------
				m_countOffsets[ index ] = nbFields + accruedDataSlices;
				m_meanOffsets[ index ] = nbFields + accruedDataSlices + 1;
				break;
				//-------
			default:
				//-------
				break;
		}

		accruedDataSlices += ( nbDataSlices - 1 );

		// add :  nbDataSlices to number of data to allocate
		// 
		m_nbDataAllocated	+= nbDataSlices;
	}


	if ( m_nbDataAllocated != ( nbFields + accruedDataSlices ) )
	{
		throw CException( CTools::Format( "ERROR: CBratProcessZFXY::GetParameters() - number of data to allocate (%d) != number of fields (%d) + data slices (%d)\n",
			m_nbDataAllocated,
			nbFields,
			accruedDataSlices ),
			BRATHL_LOGIC_ERROR );
	}

	DeleteFileParams();
}


//----------------------------------------
bool CBratProcessZFXY::CheckCommandLineOptions(int	argc, char	**argv)
{
  return base_t::CheckCommandLineOptions(argc, argv,
      			      "This program creates data files corresponding to Z=F(X,Y) function",
	      		      ZFXYKeywordList);

}

//----------------------------------------
void CBratProcessZFXY::BeforeBuildListFieldsToRead()
{

}
//----------------------------------------
void CBratProcessZFXY::AfterBuildListFieldsToRead()
{

  AddComplementDimensionsFromNetCdf();

  m_listFieldsToRead.InsertUnique(m_xField.GetFieldNames());  
  m_listFieldsToRead.InsertUnique(m_yField.GetFieldNames());  

}
//----------------------------------------
CMatrixDoublePtr* CBratProcessZFXY::NewMatrixDoublePtr()
{
  CMatrixDoublePtr* matrix = NULL;
  try
  {
    matrix = new CMatrixDoublePtr(m_xCount, m_yCount);       
  }
  catch (CException& e)
  {
    m_grids.RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CException(CTools::Format("ERROR in CBratProcessZFXY::NewMatrixDoublePtr(). Reason: %s\n",
					  e.what()),
            BRATHL_ERROR);
  }
  catch (std::bad_alloc& e) // If memory allocation (new) failed...
  {
    m_grids.RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CMemoryException(CTools::Format("ERROR: CBratProcessZFXY::NewMatrixDoublePtr() - Unable to allocate new memory to store the result.\nNative error: '%s'\n",
                                  e.what()));
  }
  catch (...)
  {
    m_grids.RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CMemoryException(CTools::Format("ERROR: CBratProcessZFXY::NewMatrixDoublePtr(). It may be that it does not remain enough memory to allocate %ld grids of %ld x %ld values (= %ld values)"
                                           " or try to allocate more than your system architecture can ever handle",
					  (long)m_nbDataAllocated,
					  (long)m_xCount,
					  (long)m_yCount,
					  (long)(m_nbDataByGrid * m_nbDataAllocated)));
  }


  if (matrix == NULL)
  {
    m_grids.RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CMemoryException(CTools::Format("ERROR: CBratProcessZFXY::NewMatrixDoublePtr(). It may be that it does not remain enough memory to allocate %ld grids of %ld x %ld values (= %ld values)"
                                           " or try to allocate more than your system architecture can ever handle",
					  (long)m_nbDataAllocated,
					  (long)m_xCount,
					  (long)m_yCount,
					  (long)(m_nbDataByGrid * m_nbDataAllocated)));

  }
  return matrix;

}

//----------------------------------------
CMatrixDouble* CBratProcessZFXY::NewMatrixDouble()
{
  CMatrixDouble* matrix = NULL;
  try
  {
    matrix = new CMatrixDouble(m_xCount, m_yCount);       
  }
  catch (CException& e)
  {
    m_grids.RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CException(CTools::Format("ERROR in CBratProcessZFXY::NewMatrixDouble(). Reason: %s\n",
					  e.what()),
            BRATHL_ERROR);
  }
  catch (std::bad_alloc& e) // If memory allocation (new) failed...
  {
    m_grids.RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CMemoryException(CTools::Format("ERROR: CBratProcessZFXY::NewMatrixDouble() - Unable to allocate new memory to store the result.\nNative error: '%s'\n"
                                           " or try to allocate more than your system architecture can ever handle",
                                  e.what()));
  }
  catch (...)
  {
    m_grids.RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CMemoryException(CTools::Format("ERROR: CBratProcessZFXY::NewMatrixDouble(). It may be that it does not remain enough memory to allocate %ld grids of %ld x %ld values (= %ld values)"
                                           " or try to allocate more than your system architecture can ever handle",
					  (long)m_nbDataAllocated,
					  (long)m_xCount,
					  (long)m_yCount,
					  (long)m_nbDataByGrid * m_nbDataAllocated));
  }


  if (matrix == NULL)
  {
    m_grids.RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CMemoryException(CTools::Format("ERROR: CBratProcessZFXY::NewMatrixDouble(). It may be that it does not remain enough memory to allocate %ld grids of %ld x %ld values (= %ld values)"
                                           " or try to allocate more than your system architecture can ever handle",
					  (long)m_nbDataAllocated,
					  (long)m_xCount,
					  (long)m_yCount,
					  (long)(m_nbDataByGrid * m_nbDataAllocated)));

  }
  return matrix;

}

//----------------------------------------
CMatrix* CBratProcessZFXY::CreateGrid( uint32_t index, const CExpressionValue& exprValue )
{

	CMatrix* matrix = NULL;

	if ( exprValue.GetNbDimensions() <= 0 )
	{
		matrix = CreateGridDouble( index );
	}
	else
	{
		matrix = CreateGridDoublePtr( index, exprValue );
	}

	return matrix;

}
//----------------------------------------
CMatrixDoublePtr* CBratProcessZFXY::CreateGridDoublePtr(uint32_t index, const CExpressionValue& exprValue)
{
  CTrace::Tracer(1, CTools::Format("Allocating and initialising gridded data structures of field '%s'", 
                                    m_names[index].c_str()));

  CStringArray fieldDims;
  CStringArray complementFieldDims;
  // Applies only to Netcdf products
  if (base_t::IsProductNetCdf() )
  {
    CProductNetCdf* productNetCdf = GetProductNetCdf(m_product);
    productNetCdf->GetNetCdfDimensions(m_fields.at(index), fieldDims, m_recordName);
    m_netCdfAxisDims.Complement(fieldDims, complementFieldDims);

//    fieldDims.Dump(*(CTrace::GetInstance()->GetDumpContext()));

  }
  else
  {
    const CStringArray* exprFieldIndexes = m_mapFieldIndexesByExpr.Exists(m_names.at(index));
    if (exprFieldIndexes != NULL)
    {
      complementFieldDims.Insert(*exprFieldIndexes);
    }
  }


//  complementFieldDims.Dump(*(CTrace::GetInstance()->GetDumpContext()));

  //int32_t exprNbDims = exprValue.GetNbDimensions() - 2;
  //int32_t nbComplementFieldDims = complementFieldDims.size();

/*
  if (exprNbDims == 0)
  {
    throw CException(CTools::Format("ERROR in CBratProcessZFXY::CreateGridDoublePtr(). "
                                    "Number of data dimension is zero and it should be > 0 "
                                    "Expression %s (index is %d, value is %s)\n",
					                          m_names.at(index).c_str(),
                                    index,
                                    m_fields.at(index).AsString().c_str()),
            BRATHL_LOGIC_ERROR);


  }
  */
  /*
  if (nbComplementFieldDims == 0)
  {
    throw CException(CTools::Format("ERROR in CBratProcessZFXY::CreateGridDoublePtr(). "
                                    "Except X/Y dimensions, at least one other dimension should have been found "
                                    "for expression %s (index is %d, value is %s)\n",
					                          m_names.at(index).c_str(),
                                    index,
                                    m_fields.at(index).AsString().c_str()),
            BRATHL_LOGIC_ERROR);


  }
  if ((nbComplementFieldDims != exprNbDims) && (nbComplementFieldDims != 0))
  {
    throw CException(CTools::Format("ERROR in CBratProcessZFXY::CreateGridDoublePtr(). "
                                    "Dimensions are inconsistency: number of data dimension is %d and number of field dimension is %d."
                                    "Expression %s (index is %d, value is %s)\n",
                                    exprNbDims,
                                    nbComplementFieldDims,
					                          m_names.at(index).c_str(),
                                    index,
                                    m_fields.at(index).AsString().c_str()),
            BRATHL_LOGIC_ERROR);


  }
  */

  CMatrixDoublePtr* matrix = NULL;

  try
  {    
    //-------------
    matrix = NewMatrixDoublePtr();
    //-------------
    matrix->SetName(m_names[index]);
    matrix->SetXName(m_xName);
    matrix->SetYName(m_yName);
    
    //---------------------------------
    matrix->InitMatrixDimsData(exprValue.GetDimensions(), base_t::MergeIdentifyUnsetData);
    //---------------------------------
    
    matrix->SetMatrixDataDimIndexes(complementFieldDims);
    
    //---------------------------------
    m_grids[index] = matrix;
    //---------------------------------
    
    int32_t countOffset = m_countOffsets[index];

    if (countOffset > 0)
    {
      //-------------
      matrix = NewMatrixDoublePtr();
      //-------------
      matrix->SetName(CTools::Format("%s_count", m_names[index].c_str()));
      matrix->SetXName(m_xName);
      matrix->SetYName(m_yName);

      //---------------------------------
      matrix->InitMatrixDimsData(exprValue.GetDimensions(), base_t::MergeIdentifyUnsetData);
      //---------------------------------
      
      matrix->SetMatrixDataDimIndexes(complementFieldDims);

      //---------------------------------
      m_grids[countOffset] = matrix;
      //---------------------------------

    }
    
    int32_t meanOffset = m_meanOffsets[index];

    if (meanOffset > 0)
    {
      //-------------
      matrix = NewMatrixDoublePtr();
      //-------------
      matrix->SetName(CTools::Format("%s_mean", m_names[index].c_str()));
      matrix->SetXName(m_xName);
      matrix->SetYName(m_yName);
      
      //---------------------------------
      matrix->InitMatrixDimsData(exprValue.GetDimensions(), base_t::MergeIdentifyUnsetData);
      //---------------------------------

      matrix->SetMatrixDataDimIndexes(complementFieldDims);

      //---------------------------------
      m_grids[meanOffset] = matrix;
      //---------------------------------

    }
  }
  catch (CException& e)
  {
    m_grids.RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CException(CTools::Format("ERROR in CBratProcessZFXY::CreateGridDoublePtr(). Reason: %s\n",
					  e.what()),
            BRATHL_ERROR);
  }
  catch (std::bad_alloc& e) // If memory allocation (new) failed...
  {
    m_grids.RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CMemoryException(CTools::Format("ERROR: CBratProcessZFXY::CreateGridDoublePtr() - Unable to allocate new memory to store the result.\nNative error: '%s'\n"
                                           " or try to allocate more than your system architecture can ever handle",
                                  e.what()));
  }
  catch (...)
  {
    m_grids.RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CMemoryException(CTools::Format("ERROR: CBratProcessZFXY::CreateGridDoublePtr(). Not enough memory to allocate %ld grids of %ld x %ld values (= %ld values)"
                                           " or try to allocate more than your system architecture can ever handle",
					  (long)m_nbDataAllocated,
					  (long)m_xCount,
					  (long)m_yCount,
					  (long)(m_nbDataByGrid * m_nbDataAllocated)));
  }

  return CBratProcessZFXY::GetMatrixDoublePtr(index); 
}
//----------------------------------------
CMatrixDouble* CBratProcessZFXY::CreateGridDouble(uint32_t index)
{
  CTrace::Tracer(1, CTools::Format("Allocating and initialising gridded data structures of field '%s'", 
                                    m_names[index].c_str()));


  CMatrixDouble* matrix = NULL;

  try
  {    
    //-------------
    matrix = NewMatrixDouble();
    //-------------
    matrix->SetName(m_names[index]);
    matrix->SetXName(m_xName);
    matrix->SetYName(m_yName);

    m_grids[index] = matrix;
    
    int32_t countOffset = m_countOffsets[index];

    if (countOffset > 0)
    {
      //-------------
      matrix = NewMatrixDouble();
      //-------------
      matrix->SetName(CTools::Format("%s_count", m_names[index].c_str()));
      matrix->SetXName(m_xName);
      matrix->SetYName(m_yName);
      
      m_grids[countOffset] = matrix;

    }
    
    int32_t meanOffset = m_meanOffsets[index];

    if (meanOffset > 0)
    {
      //-------------
      matrix = NewMatrixDouble();
      //-------------
      matrix->SetName(CTools::Format("%s_mean", m_names[index].c_str()));
      matrix->SetXName(m_xName);
      matrix->SetYName(m_yName);
      
      m_grids[meanOffset] = matrix;

    }
  }
  catch (CException& e)
  {
    m_grids.RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CException(CTools::Format("ERROR in CBratProcessZFXY::CreateGridDouble(). Reason: %s\n",
					  e.what()),
            BRATHL_ERROR);
  }
  catch (std::bad_alloc& e) // If memory allocation (new) failed...
  {
    m_grids.RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CMemoryException(CTools::Format("ERROR: CBratProcessZFXY::CreateGridDouble() - Unable to allocate new memory to store the result.\nNative error: '%s'\n"
                                           " or try to allocate more than your system architecture can ever handle",
                                  e.what()));
  }
  catch (...)
  {
    m_grids.RemoveAll(); // free memory  to be able to allocate new for error msg
    throw CMemoryException(CTools::Format("ERROR: CBratProcessZFXY::CreateGridDouble(). Not enough memory to allocate %ld grids of %ld x %ld values (= %ld values)"
                                           " or try to allocate more than your system architecture can ever handle",
					  (long)m_nbDataAllocated,
					  (long)m_xCount,
					  (long)m_yCount,
					  (long)(m_nbDataByGrid * m_nbDataAllocated)));
  }


  return CBratProcessZFXY::GetMatrixDouble(index); 

}



//----------------------------------------
void CBratProcessZFXY::InitGrids()
{
	try
	{
		uint32_t index = 0;

		m_grids.resize(m_nbDataAllocated, NULL);

		DeleteData();
		m_xData = new double[m_xCount];
		m_yData = new double[m_yCount];

		for (size_t index = 0; index < m_xCount; index++)
		{
			m_xData[index]	= m_xMin + m_xStep * static_cast<double>(index);
		}

		for (size_t index = 0; index < m_yCount; index++)
		{
			m_yData[index]	= m_yMin + m_yStep * index;
		}
	}
	catch( ... )
	{
		throw CMemoryException( "ERROR: CBratProcessZFXY::InitGrids() out of memory allocating data structures," );
	}
}

//----------------------------------------
void CBratProcessZFXY::AddComplementDimensionsFromNetCdf()
{
  // Applies only to Netcdf products
  if (! base_t::IsProductNetCdf() )
  {
   return;
  }
  CProductNetCdf* productNetCdf = GetProductNetCdf(m_product);

  // Get dimension's names of the X field
  m_netCdfAxisDims.RemoveAll();
  productNetCdf->GetNetCdfDimensions(m_xField, m_netCdfAxisDims, m_recordName);
  // and add dimension's names of the Y field
  productNetCdf->GetNetCdfDimensions(m_yField, m_netCdfAxisDims, m_recordName);

  // Get dimension's names of all the data fields
  CStringArray fieldDims;
  CStringArray fieldDimsWithoutAlgo;
  productNetCdf->GetNetCdfDimensions(m_fields, fieldDims, m_recordName);
  productNetCdf->GetNetCdfDimensionsWithoutAlgo(m_fields, fieldDimsWithoutAlgo, m_recordName);
  
  m_netCdfAxisDims.Dump(*(CTrace::GetInstance()->GetDumpContext()));
  fieldDims.Dump(*(CTrace::GetInstance()->GetDumpContext()));
  
  //Gets the dimension's name which are not dimensions of the X field
  m_netCdfComplementDims.RemoveAll();
  m_netCdfAxisDims.Complement(fieldDims, m_netCdfComplementDims);

  m_netCdfComplementDims.Dump(*(CTrace::GetInstance()->GetDumpContext()));

  // Add dimensions which are not dimensions of the X / Y fields
  if (fieldDimsWithoutAlgo.size() > 0)
  {
    AddDimensionsFromNetCdf(m_netCdfComplementDims);
  }

  AddVarsFromNetCdf();

}


//----------------------------------------
CMatrix* CBratProcessZFXY::GetMatrix( size_t index )
{
	CMatrix* matrix = index < m_grids.size() ? m_grids.at( index ) : NULL;
	if ( !matrix )
	{
		throw CException( CTools::Format( "CBratProcessZFXY::GetMatrix - matrix at index %d is NULL.", index ), BRATHL_LOGIC_ERROR );
	}
	return matrix;
}


CMatrixDoublePtr* CBratProcessZFXY::GetMatrixDoublePtr( size_t index )
{
	CMatrixDoublePtr* matrix = dynamic_cast<CMatrixDoublePtr*>( GetMatrix( index ) );
	if ( !matrix )
	{
		throw CException( CTools::Format( "CBratProcessZFXY::GetMatrixDoublePtr - matrix at index %d is NULL.\nObject is not an instance of CMatrixDoublePtr",
			index ),
			BRATHL_LOGIC_ERROR );
	}
	return matrix;

}

CMatrixDouble* CBratProcessZFXY::GetMatrixDouble( size_t index )
{
	CMatrixDouble* matrix = dynamic_cast<CMatrixDouble*>( m_grids.at( index ) );
	if ( !matrix )
	{
		throw CException( CTools::Format( "CBratProcessZFXY::GetMatrixDouble - matrix at index %d is NULL.\nObject is not an instance of CMatrixDouble",
			index ),
			BRATHL_LOGIC_ERROR );
	}

	return matrix;
}



//----------------------------------------
int32_t CBratProcessZFXY::Execute( std::string& msg )
{
	UNUSED( msg );

	int32_t result = BRATHL_SUCCESS;

	CDate startExec;
	startExec.SetDateNow();

	if ( m_product == NULL )
	{
		throw CException( "PROGRAM ERROR: product is NULL (CBratProcessZFXY::Execute) - Perhaps, initialization has not been called",
			BRATHL_LOGIC_ERROR );
	}

	InitGrids();

	size_t nbFiles = m_inputFiles.size();
	size_t cptFile = 0;

	for ( CStringArray::iterator itFile = m_inputFiles.begin(); itFile != m_inputFiles.end(); itFile++ )
	{
		cptFile++;

		CTrace::Tracer( 1, "File %d/%d - Reading record data from %s ... and registering data ...",
			cptFile, nbFiles, ( *itFile ).c_str() );

		// open file an set record name and list of field to read
		// 
		m_product->SetFieldSpecificUnits( m_fieldSpecificUnit );
		m_product->Open( *itFile, m_recordName );

		NetCdfProductInitialization();

		SetExpandArray( m_xField, true );
		SetExpandArray( m_yField, true );

		ReplaceAxisDefinition();

		ReplaceFieldDefinition();

        m_product->SetListFieldToRead( m_listFieldsToRead, false );

		// Get the number of record for the default record name (set in Open method of CProduct above)
		// 
		int_t nRecords = m_product->GetNumberOfRecords();	//can be < 0

		for ( int_t iRecord = 0; iRecord < nRecords; iRecord++ )
		{
			//Read fields for the record name  (list of field and record name are set in Open method of CProduct above)
			// 
            m_product->ReadBratRecord( iRecord );
			RegisterData();
		}

		m_product->Close();
	}

	if ( m_product->GetSkippedRecordCount() > 0 )
	{
		CTrace::Tracer( 1, CTools::Format( "WARNING - %d input data records have been skipped due to inconsistency between two measures",
			m_product->GetSkippedRecordCount() ) );
	}


	result = WriteData();

	CDate endExec;
	endExec.SetDateNow();

	CTrace::Tracer( 1, CTools::Format( "Processing time: %.3f seconds (%.2f minutes)\n",
		( endExec - startExec ),
		( endExec - startExec ) / 60.0 ) );


	return result;
}

//----------------------------------------

void CBratProcessZFXY::RegisterData()
{

	if ( m_product == NULL )
	{
		throw CException( "PROGRAM ERROR: product is NULL (CBratProcessZFXY::RegisterData) - Perhaps, initialization has not been called",
			BRATHL_LOGIC_ERROR );
	}

	CDataSet* dataSet = NULL;
	CRecordSet* recordSet = NULL;

	CExpressionValue exprValue;

	double xValue;
	double yValue;

	setDefaultValue( xValue );

	//  OneMeasure	Measure;

	dataSet = m_product->GetDataSet();

	//---------------------------------
	// Picks up data from the recordset
	//---------------------------------
	for ( CObArray::iterator itDataSet = dataSet->begin(); itDataSet != dataSet->end(); itDataSet++ )
	{
		recordSet = dataSet->GetRecordSet( itDataSet );

		///bool firstRecord = (record == dataSet->front());

		//---------------------------------
		// Tests condition expression (select expression)
		//---------------------------------
		if ( m_alwaysTrue == false )
		{
			recordSet->ExecuteExpression( m_select, m_recordName, exprValue, m_product );
			if ( exprValue.IsTrue() != 1 )
			{
				//data doesn't match SELECT options,
				//-------------
				continue;
				//-------------
			}

		}


		//---------------------------------
		// Reads X value
		//---------------------------------
		recordSet->ExecuteExpression( m_xField, m_recordName, exprValue, m_product );

		size_t nbValues = exprValue.GetNbValues();

		if ( nbValues == 0 )
		{
			continue;
		}

		if ( nbValues == 1 )
		{
			xValue = exprValue.GetValues()[ 0 ];

			if ( isDefaultValue( xValue ) )
			{
				//---------
				continue;
				//---------
			}

			xValue = base_t::CheckLongitudeValue( xValue, m_xMin, m_xType );
		}
		else
		{
			throw CException( "X value must be scalar not a std::vector or a matrix",
				BRATHL_LIMIT_ERROR );
		}


		//---------------------------------
		// Reads Y value
		//---------------------------------
		recordSet->ExecuteExpression( m_yField, m_recordName, exprValue, m_product );

		nbValues	= exprValue.GetNbValues();

		if ( nbValues == 0 )
		{
			continue;
		}

		if ( nbValues == 1 )
		{
			yValue = exprValue.GetValues()[ 0 ];

			if ( isDefaultValue( yValue ) )
			{
				//---------
				continue;
				//---------
			}

			yValue = base_t::CheckLongitudeValue( yValue, m_yMin, m_yType );
		}
		else
		{
			throw CException( "Y value must be scalar not a std::vector or a matrix",
				BRATHL_LIMIT_ERROR );
		}

		//-----------------------------
		// Searches data x an y indexes
		//-----------------------------
		uint32_t xPos = 0;
		uint32_t yPos = 0;

		if ( CheckOutsideValue( xValue, m_xMin, m_xMax, m_xStep, m_outsideMode ) )
		{
			continue;
		}

		if ( CheckOutsideValue( yValue, m_yMin, m_yMax, m_yStep, m_outsideMode ) )
		{
			continue;
		}

		if ( CheckPositionValue( xValue, m_xMin, m_xStep, m_xCount, m_positionMode, xPos ) )
		{
			continue;
		}

		if ( CheckPositionValue( yValue, m_yMin, m_yStep, m_yCount, m_positionMode, yPos ) )
		{
			continue;
		}


		//---------------------
		// Gets data
		//---------------------

		uint32_t indexExpr = 0;
		/*
			for (indexExpr = 0 ; indexExpr < m_fields.size() ; indexExpr++)
			{
			  CMatrix* matrix = GetMatrix(indexExpr, false);
			  if (matrix = NULL)
			  {
				recordSet->ExecuteExpression(m_fields.at(indexExpr), m_recordName, exprValue, m_product);

				matrix = CreateGrid(indexExpr, exprValue);
			  }
			  //---------------------------------
			  // Finds each data (expression) dimensions
			  //---------------------------------

			  CMatrixDoublePtr* matrix = CBratProcessZFXY::GetMatrixDoublePtr(m_grids.at(indexExpr));

			  if (matrix->IsMatrixDataSet())
			  {
				//---------------------------------
				// matrix data have already been set
				//---------------------------------
				break;
				//---------------------------------
			  }

			  p->Tracer(1, CTools::Format("Allocating and initialising gridded data structures of field '%s'", matrix->GetName().c_str()));

			  recordSet->ExecuteExpression(m_fields.at(indexExpr), m_recordName, exprValue, m_product);

			  //---------------------------------
			  matrix->InitMatrixDimsData(exprValue.GetDimensions(), base_t::MergeIdentifyUnsetData);
			  //---------------------------------

			  int32_t countOffset = m_countOffsets.at(indexExpr);
			  int32_t meanOffset = m_meanOffsets.at(indexExpr);

			  if (countOffset > 0)
			  {
				matrix = CBratProcessZFXY::GetMatrixDoublePtr(m_grids.at(countOffset));

			  //---------------------------------
				matrix->InitMatrixDimsData(exprValue.GetDimensions(), base_t::MergeIdentifyUnsetData);
			  //---------------------------------
			  }

			  if (meanOffset > 0)
			  {
				matrix = CBratProcessZFXY::GetMatrixDoublePtr(m_grids.at(meanOffset));

			  //---------------------------------
				matrix->InitMatrixDimsData(exprValue.GetDimensions(), base_t::MergeIdentifyUnsetData);
			  //---------------------------------

			  }
			}
		*/
		//---------------------------------
		// Reads data values (expression result)
		//---------------------------------
		for ( indexExpr = 0; indexExpr < m_fields.size(); indexExpr++ )
		{
			recordSet->ExecuteExpression( m_fields.at( indexExpr ), m_recordName, exprValue, m_product );

			CMatrix *matrix =
				indexExpr < m_grids.size() ?
				m_grids.at( indexExpr ) :
				nullptr;

			if ( matrix == NULL )
			{
				//recordSet->ExecuteExpression(m_fields.at(indexExpr), m_recordName, exprValue, m_product);

				matrix = CreateGrid( indexExpr, exprValue );
			}


			//int32_t countOffset = m_countOffsets.at(indexExpr);
			//int32_t meanOffset = m_meanOffsets.at(indexExpr);


			double* dataValues = NULL;
			double* countValues = NULL;
			double* meanValues = NULL;

			GetDataValuesFromMatrix( indexExpr, xPos, yPos, dataValues, countValues, meanValues, nbValues );


			if ( base_t::IsProductNetCdf() )
			{
				CNetCDFCoordinateAxis* coordVar = dynamic_cast<CNetCDFCoordinateAxis*>( m_internalFiles->GetNetCDFVarDef( m_names[ indexExpr ] ) );
				if ( coordVar != NULL )
				{
					std::string msg;
					bool areValuesSimilar = CheckValuesSimilar( indexExpr, exprValue.GetValues(), nbValues, xPos, yPos, msg );
					if ( ! areValuesSimilar )
					{
						CTrace::Tracer( 2, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
						CTrace::Tracer( 2, msg );
						CTrace::Tracer( 2, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

					}
				}
			}


			CMatrixDouble *matrixDouble = dynamic_cast<CMatrixDouble*>( matrix );
			CMatrixDoublePtr *matrixDoublePtr = dynamic_cast<CMatrixDoublePtr*>( matrix );

			assert__( ( matrixDouble && !matrixDoublePtr ) || ( !matrixDouble && matrixDoublePtr ) );

			double* auxParams = ComputeMergeDataValueParameters( recordSet, indexExpr, xValue, yValue );
			if ( matrixDouble != NULL )
			{
				MergeDataValue( *dataValues, exprValue.GetValues()[ 0 ], countValues, meanValues, auxParams, m_dataMode[ indexExpr ] );
			}
			else if ( matrixDoublePtr != NULL )
			{
				MergeDataValue( dataValues, exprValue.GetValues(), nbValues, indexExpr, countValues, meanValues, auxParams );
			}
		}
	}

	//p->Tracer(1,"End registering data");
}

double* CBratProcessZFXY::ComputeMergeDataValueParameters(CRecordSet* recordSet, uint32_t index, double xPos, double yPos) {
    double* auxData = NULL;
    EMergeDataMode mode = m_dataMode[index];

    switch (mode) {
        case CBratProcess::pctTIME:
            // init. output array
        {
            auxData = new double[7];

            // get interp. target time
            CDate target = mDataInterpolationDateTime.at(index);
            double secs;
            target.Convert2Second(secs);
            auxData[0] = secs;

            // get actual time value
            CExpressionValue exprValue;
            std::string field = mDataInterpolationTimeFieldName.at(index);
            CExpression fieldExpr(field);
            recordSet->ExecuteExpression(fieldExpr, m_recordName, exprValue, m_product);

            auxData[1] = exprValue.GetValues()[0];

            // calc. distance from cell centre
            double y_err = CTools::Deg2Rad(fmod((yPos - m_yMin), m_yStep));
            double x_err = CTools::Deg2Rad(fmod((xPos - m_xMin), m_xStep)) * cos(y_err / 2.0);
            double dist = sqrt(x_err*x_err + y_err*y_err) * 6378137.0; // earth rad. metres

            auxData[2] = dist;

            auxData[3] = mDataInterpolationTimeWeighting.at(index);
            auxData[4] = m_tFactor;
            auxData[5] = mDataInterpolationDistanceWeighting.at(index);
            auxData[6] = m_dFactor;
            break;
        }
        default:
            break;
    }
    return auxData;
}

//----------------------------------------
void CBratProcessZFXY::GetDataValuesFromMatrix(size_t indexExpr, uint32_t xPos, uint32_t yPos, double *&dataValues, size_t& nbValues)
{
  /*
  //---------------------------------------
  CMatrixDouble* matrixDouble = CBratProcessZFXY::GetMatrixDouble(m_grids.at(indexExpr), false);      
  //---------------------------------------

  if (matrixDouble != NULL)
  {
    GetDataValuesFromMatrix(matrixDouble, indexExpr, xPos, yPos, dataValues, nbValues);
    return;
  }

  //---------------------------------------
  CMatrixDoublePtr* matrixDoublePtr = CBratProcessZFXY::GetMatrixDoublePtr(m_grids.at(indexExpr), false);      
  //---------------------------------------

  if (matrixDoublePtr != NULL)
  {
    GetDataValuesFromMatrix(matrixDoublePtr, indexExpr, xPos, yPos, dataValues, nbValues);
  }
  */
  
  //---------------------------------------
  CMatrix* matrix = GetMatrix(indexExpr);      
  //---------------------------------------

  dataValues = matrix->At(xPos, yPos);

  if (dataValues == NULL)
  {
    throw CException(CTools::Format("ERROR: CBratProcessZFXY::GetDataValuesFromMatrix() - data values not found\n. Expression index is %d. Matrix '%s' at (%d , %d).\n"
                                    "Expression is: '%s'",
                                    indexExpr, 
                                    matrix->GetName().c_str(),
                                    xPos,
                                    yPos,
                                    m_fields.at(indexExpr).AsString().c_str()),
			               BRATHL_LOGIC_ERROR);
  }

  nbValues = matrix->GetMatrixNumberOfValuesData();

}
/*
//----------------------------------------
void CBratProcessZFXY::GetDataValuesFromMatrix(CMatrixDoublePtr* matrix, uint32_t indexExpr, uint32_t xPos, uint32_t yPos, double *&dataValues, uint32_t& nbValues)
{

  dataValues = matrix->At(xPos, yPos);

  if (dataValues == NULL)
  {
    throw CException(CTools::Format("ERROR: CBratProcessZFXY::GetDataValuesFromMatrix() - data values not found\n. Expression index is %d. Matrix '%s' at (%d , %d).\n"
                                    "Expression is: '%s'",
                                    indexExpr, 
                                    matrix->GetName().c_str(),
                                    xPos,
                                    yPos,
                                    m_fields.at(indexExpr).AsString().c_str()),
			               BRATHL_LOGIC_ERROR);
  }

  nbValues = matrix->GetMatrixNumberOfValuesData();

}
//----------------------------------------
void CBratProcessZFXY::GetDataValuesFromMatrix(CMatrixDouble* matrix, uint32_t indexExpr, uint32_t xPos, uint32_t yPos, double *&dataValues, uint32_t& nbValues)
{

  dataValues = matrix->At(xPos, yPos);

  if (dataValues == NULL)
  {
    throw CException(CTools::Format("ERROR: CBratProcessZFXY::GetDataValueFromMatrix() - data values not found\n. Expression index is %d. Matrix '%s' at (%d , %d).\n"
                                    "Expression is: '%s'",
                                    indexExpr, 
                                    matrix->GetName().c_str(),
                                    xPos,
                                    yPos,
                                    m_fields.at(indexExpr).AsString().c_str()),
			               BRATHL_LOGIC_ERROR);
  }

  nbValues = 1;

}
*/
//----------------------------------------
void CBratProcessZFXY::GetDataValuesFromMatrix(size_t indexExpr, uint32_t xPos, uint32_t yPos, double *&dataValues, double *&countValues, double *&meanValues, 
	size_t& nbValues)
{

  
  GetDataValuesFromMatrix(indexExpr, xPos, yPos, dataValues, nbValues);

  countValues = NULL;
  meanValues = NULL;

  CMatrix* matrix = NULL;

  int32_t countOffset = m_countOffsets.at(indexExpr);
  int32_t meanOffset = m_meanOffsets.at(indexExpr);

  if (countOffset > 0)
  {
    //---------------------------------------
    matrix = CBratProcessZFXY::GetMatrix(countOffset);      
    //---------------------------------------
  
    countValues = matrix->At(xPos, yPos);

    if (countValues == NULL)
    {
      throw CException(CTools::Format("ERROR: CBratProcessZFXY::GetDataValueFromMatrix() - count values array not found\n. Expression index is %d. Offset is %d. Matrix '%s' at (%d , %d).\n"
                                      "Expression is: '%s'",
                                      indexExpr, 
                                      countOffset,
                                      matrix->GetName().c_str(),
                                      xPos,
                                      yPos,
                                      m_fields.at(indexExpr).AsString().c_str()),
			                 BRATHL_LOGIC_ERROR);
    }
    else //  countValues != NULL)
    {
      size_t colsCount = matrix->GetMatrixNumberOfValuesData();
      if (nbValues != colsCount)
      {
        throw CException(CTools::Format("ERROR: CBratProcessZFXY::GetDataValueFromMatrix() - data values array size (%d) is not equal to count values array size (%d).\n"
                                        "Expression is: '%s'",
                                        nbValues, 
                                        colsCount,
                                        m_fields.at(indexExpr).AsString().c_str()),
			                   BRATHL_LOGIC_ERROR);

      }
    }
  }

  if (meanOffset > 0)
  {
    //---------------------------------------
    matrix = CBratProcessZFXY::GetMatrix(meanOffset);      
    //---------------------------------------

    meanValues = matrix->At(xPos, yPos);
  
    if (meanValues == NULL)
    {
      throw CException(CTools::Format("ERROR: CBratProcessZFXY::GetDataValueFromMatrix() - mean values array not found\n. Expression index is %d. Offset is %d.\n Matrix '%s' at (%d , %d).\n"
                                      "Expression is: '%s'",
                                      indexExpr, 
                                      meanOffset,
                                      matrix->GetName().c_str(),
                                      xPos,
                                      yPos,
                                      m_fields.at(indexExpr).AsString().c_str()),
			                 BRATHL_LOGIC_ERROR);
    }
    else   // (meanValues != NULL)
    {
      size_t colsMean = matrix->GetMatrixNumberOfValuesData();
      if (nbValues != colsMean)
      {
        throw CException(CTools::Format("ERROR: CBratProcessZFXY::GetDataValueFromMatrix() - data values array size (%d) is not equal to  mean values array size (%d)\n"
                                        "Expression is: '%s'",
                                        nbValues, 
                                        colsMean,
                                        m_fields.at(indexExpr).AsString().c_str()),
			                   BRATHL_LOGIC_ERROR);

      }
    }
  }
}


//----------------------------------------
int32_t CBratProcessZFXY::WriteData()
{
	int32_t result = BRATHL_SUCCESS;

	size_t countMatrixFilled = 0;
	const size_t size = m_fields.size();
	for ( size_t indexExpr = 0; indexExpr < size; indexExpr++ )
	{
		CMatrix* matrix =
			indexExpr < m_grids.size() ?
			m_grids.at( indexExpr ) :
			nullptr;

		if ( matrix == nullptr )
		{
			CTrace::Tracer( 1, ">>>>>>>>>>>>>>>>>>>> WARNING >>>>>>>>>>>>>>>>>>>>>>>>>" );
			CTrace::Tracer( 1, "No data have been selected for '%s' expression ", m_names[ indexExpr ].c_str() );
			CTrace::Tracer( 1, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
			result = BRATHL_WARNING;
			continue;
		}

		countMatrixFilled++;
	}

	if ( countMatrixFilled == 0 )
	{

		CTrace::Tracer( 1, ">>>>>>>>>>>>>>>>>>>> WARNING >>>>>>>>>>>>>>>>>>>>>>>>>" );
		CTrace::Tracer( 1, "There is no data to write" );
		CTrace::Tracer( 1, "Output file '%s' have not been created", m_outputFileName.c_str() );
		CTrace::Tracer( 1, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
		m_internalFiles->Close();
		CFile::Delete( m_outputFileName );

		result = BRATHL_WARNING;
		return result;
	}

	CTrace::Tracer( 1, "Creating output file '%s'", m_outputFileName.c_str() );

	if ( m_internalFiles == NULL )
	{
		throw CException( "ERROR: CBratProcessZFXY::WriteData() - Unable to create NetCDF file because m_internalFiles is NULL.\n",
			BRATHL_LOGIC_ERROR );

	}

	//CInternalFilesYFX	internalFilesYFX(m_outputFileName.c_str(), Replace);
	//m_internalFiles->Open();

	m_internalFiles->WriteFileTitle( m_outputTitle );

	CTrace::Tracer( 1, "Writing data" );


	//-----------------

	// Warning : if data account for a date, they are expressed in numbers of seconds since 1950-01-01 00:00:00.0
	// m_xData and/or m_yData have to be converted to unit expressed in the file
	//if unit is set as 'xxxx since YYYY-MM-DD HH:MM:SS.MS'.

	CUnit unit = m_xUnit;
	unit.SetConversionFromBaseUnit();

	for ( uint32_t iXValue = 0; iXValue < m_xCount; iXValue++ )
	{
		m_xData[ iXValue ]	= unit.Convert( m_xData[ iXValue ] );
	}


	//---------------------------------------------
	// Get X min and X max  (X is sorted)
	//---------------------------------------------
	setDefaultValue( m_validMin );
	setDefaultValue( m_validMax );

	if ( m_xCount > 0 )
	{
		m_validMin = m_xData[ 0 ];
		m_validMax = m_xData[ m_xCount - 1 ];
	}


	//---------------------------------------------
	// Create X netcdf dimension and variable
	//---------------------------------------------
	CNetCDFCoordinateAxis* addedXCoordVar = dynamic_cast<CNetCDFCoordinateAxis*>( m_internalFiles->GetNetCDFVarDef( m_xName ) );

	bool addDimToXCoord = false;

	if ( addedXCoordVar == NULL )
	{
		CNetCDFCoordinateAxis xCoordVar( m_xName );

		addedXCoordVar = dynamic_cast<CNetCDFCoordinateAxis*>( m_internalFiles->AddNetCDFVarDef( xCoordVar ) );
		addDimToXCoord = true;
	}

	addedXCoordVar->SetUnit( m_xUnit );
	addedXCoordVar->SetDimKind( m_xType );

	addedXCoordVar->AddAttribute( new CNetCDFAttrString( LONG_NAME_ATTR, m_xTitle ) );
	addedXCoordVar->AddAttribute( new CNetCDFAttrString( COMMENT_ATTR, m_xComment ) );

	addedXCoordVar->SetValidMinMax( m_validMin, m_validMax );

	CNetCDFDimension* addedXDim = m_internalFiles->GetNetCDFDim( m_xName );

	if ( addedXDim == NULL )
	{
		CNetCDFDimension xDim( m_xName, m_xCount );

		addedXDim = m_internalFiles->AddNetCDFDim( xDim );

		addedXCoordVar->AddNetCDFDim( *addedXDim );
		addedXDim->AddCoordinateVariable( addedXCoordVar );
	}
	else
	{
		addedXDim->SetLength( m_xCount );
		addedXCoordVar->ReplaceNetCDFDim( *addedXDim );

	}


	CUIntArray dimensions;
	dimensions.Insert( m_xCount );

	CExpressionValue xValues( FloatType, dimensions, m_xData, false );;
	xValues.SetName( "xValues" );


	//-----------------

	unit = m_yUnit;
	unit.SetConversionFromBaseUnit();

	for ( uint32_t iYValue = 0; iYValue < m_yCount; iYValue++ )
	{
		m_yData[ iYValue ]	= unit.Convert( m_yData[ iYValue ] );
	}


	//---------------------------------------------
	// Get Y min and Y max  (Y is sorted)
	//---------------------------------------------
	setDefaultValue( m_validMin );
	setDefaultValue( m_validMax );

	if ( m_yCount > 0 )
	{
		m_validMin = m_yData[ 0 ];
		m_validMax = m_yData[ m_yCount - 1 ];
	}




	//---------------------------------------------
	// Create Y netcdf dimension and variable
	//---------------------------------------------
	CNetCDFCoordinateAxis* addedYCoordVar = dynamic_cast<CNetCDFCoordinateAxis*>( m_internalFiles->GetNetCDFVarDef( m_yName ) );

	bool addDimToYCoord = false;

	if ( addedYCoordVar == NULL )
	{
		CNetCDFCoordinateAxis yCoordVar( m_yName );

		addedYCoordVar = dynamic_cast<CNetCDFCoordinateAxis*>( m_internalFiles->AddNetCDFVarDef( yCoordVar ) );
		addDimToYCoord = true;
	}

	addedYCoordVar->SetUnit( m_yUnit );
	addedYCoordVar->SetDimKind( m_yType );

	addedYCoordVar->AddAttribute( new CNetCDFAttrString( LONG_NAME_ATTR, m_yTitle ) );
	addedYCoordVar->AddAttribute( new CNetCDFAttrString( COMMENT_ATTR, m_yComment ) );

	addedYCoordVar->SetValidMinMax( m_validMin, m_validMax );

	CNetCDFDimension* addedYDim = m_internalFiles->GetNetCDFDim( m_yName );

	if ( addedYDim == NULL )
	{
		CNetCDFDimension yDim( m_yName, m_yCount );

		addedYDim = m_internalFiles->AddNetCDFDim( yDim );

		addedYCoordVar->AddNetCDFDim( *addedYDim );
		addedYDim->AddCoordinateVariable( addedYCoordVar );
	}
	else
	{
		addedYDim->SetLength( m_yCount );
		addedYCoordVar->ReplaceNetCDFDim( *addedYDim );

	}



	dimensions.RemoveAll();
	dimensions.Insert( m_yCount );

	CExpressionValue yValues( FloatType, dimensions, m_yData, false );;
	yValues.SetName( "yValues" );



	//----------------



	//////////////std::string dimType = xCoordVar.GetDimType();




  //  OutputFile.CreateDim(XType, XName, DataWritten, XUnit.GetText(), XTitle, 
  //                       XComment, validMin, validMax);


	//-----------------------------
	// Merge data
	//-----------------------------
	CIntMap mapVarIndexExpr;

	// 2 loops for netcdf performance
	for ( size_t indexExpr = 0; indexExpr < m_fields.size(); indexExpr++ )
	{
		setDefaultValue( m_validMin );
		setDefaultValue( m_validMax );

		double* dataValues = NULL;
		double* countValues = NULL;
		double* meanValues = NULL;

		unit = m_units[ indexExpr ];
		unit.SetConversionFromBaseUnit();

		bool applyFilter = m_smooth[ indexExpr ] || m_extrapolate[ indexExpr ];

		CMatrix *matrix =
			indexExpr < m_grids.size() ?
			m_grids.at( indexExpr ) :
			nullptr;

		if ( matrix == NULL )
		{
			continue;
		}



		//---------------------------------------
		CTrace::Tracer( 1, CTools::Format( "\t==> Merge data of field '%s'", matrix->GetName().c_str() ) );

		size_t nbValues = 0;
		const bool is_matrix_double_ptr = dynamic_cast<CMatrixDoublePtr*>( matrix ) != nullptr;		assert__( is_matrix_double_ptr || dynamic_cast<CMatrixDouble*>( matrix ) );

		for ( size_t xPos = 0; xPos < m_xCount; xPos++ )
		{
			for ( size_t yPos = 0; yPos < m_yCount; yPos++ )
			{
				GetDataValuesFromMatrix( indexExpr, xPos, yPos, dataValues, countValues, meanValues, nbValues );

				if ( is_matrix_double_ptr )
				{
					FinalizeMergingOfDataValues( dataValues,
						indexExpr,
						nbValues,
						countValues,
						meanValues );

					for ( size_t indexValues = 0; indexValues < nbValues; indexValues++ )
					{
						//---------------------------------------------
						// converts to asked unit
						//---------------------------------------------
						if ( !isDefaultValue( dataValues[ indexValues ] ) )
						{
							dataValues[ indexValues ]	= unit.Convert( dataValues[ indexValues ] );
						}
					}

				}
				else
				{
					FinalizeMergingOfDataValues( *dataValues,
						countValues,
						meanValues,
						m_dataMode[ indexExpr ] );
					//---------------------------------------------
					// converts to asked unit
					//---------------------------------------------
					if ( !isDefaultValue( *dataValues ) )
					{
						*dataValues	= unit.Convert( *dataValues );
					}
				}

			} //for (size_t yPos ....

		} //for (size_t xPos ....

		if ( applyFilter )
		{
			CTrace::Tracer( 1, CTools::Format( "\t==> Apply filters to field '%s'", matrix->GetName().c_str() ) );

			CBratProcessZFXY::LoessFilterGrid( *matrix, NULL,
				m_xCircular, m_xLoessCutoff,
				m_yCircular, m_yLoessCutoff,
				m_smooth[ indexExpr ],
				m_extrapolate[ indexExpr ] );

		}

		//---------------------------------------------
		// compute variable min and max
		//---------------------------------------------
		CTrace::Tracer( 1, CTools::Format( "\t==> Compute min./max. values of field '%s'", matrix->GetName().c_str() ) );
		matrix->GetMinMaxValues( m_validMin, m_validMax );


		CNetCDFVarDef* addedVarDef = m_internalFiles->GetNetCDFVarDef( m_names[ indexExpr ] );
		if ( addedVarDef == NULL )
		{
			CNetCDFVarDef varDef( m_names[ indexExpr ], m_units[ indexExpr ] );

			varDef.AddAttribute( new CNetCDFAttrString( LONG_NAME_ATTR, m_titles[ indexExpr ] ) );
			varDef.AddAttribute( new CNetCDFAttrString( COMMENT_ATTR, m_comments[ indexExpr ] ) );

			addedVarDef = m_internalFiles->AddNetCDFVarDef( varDef );

		}

		addedVarDef->SetValidMinMax( m_validMin, m_validMax );

		if ( base_t::IsProductNetCdf() )
		{
			//=============
			if ( ! addedVarDef->IsCoordinateAxis() )
			{
				CNetCDFDimension* varDim = NULL;
				//-------------
				if ( addedVarDef->HasCommonDims( m_xName ) )
				{
					varDim = addedVarDef->GetNetCDFDim( addedXDim->GetName() );
					if ( varDim == NULL )
					{
						addedVarDef->InsertNetCDFDim( *addedXDim, 0 ); // X first dim
					}
					else
					{
						varDim->SetLength( m_xCount );
						addedVarDef->ReplaceNetCDFDim( *varDim );
					}
				}
				//-------------
				if ( addedVarDef->HasCommonDims( m_yName ) )
				{
					varDim = addedVarDef->GetNetCDFDim( addedYDim->GetName() );
					if ( varDim == NULL )
					{
						addedVarDef->InsertNetCDFDim( *addedYDim, 1 ); // Y second dim
					}
					else
					{
						varDim->SetLength( m_yCount );
						addedVarDef->ReplaceNetCDFDim( *varDim );
					}
				}
				//-------------

			}
			//=============
		}
		else if ( ! addedVarDef->IsCoordinateAxis() ) // not a CProductNetCdf object
		{
			//-------------
			CNetCDFDimension* varDim = addedVarDef->GetNetCDFDim( addedXDim->GetName() );
			if ( varDim == NULL )
			{
				addedVarDef->InsertNetCDFDim( *addedXDim, 0 ); // X first dim
			}
			else
			{
				varDim->SetLength( m_xCount );
				addedVarDef->ReplaceNetCDFDim( *varDim );
			}

			//-------------
			varDim = addedVarDef->GetNetCDFDim( addedYDim->GetName() );
			if ( varDim == NULL )
			{
				addedVarDef->InsertNetCDFDim( *addedYDim, 1 ); // Y second dim
			}
			else
			{
				varDim->SetLength( m_yCount );
				addedVarDef->ReplaceNetCDFDim( *varDim );
			}
		}

		mapVarIndexExpr.Insert( addedVarDef->GetName(), indexExpr, false );


	} //for (uint32_t indexExpr = 0;...

	CTrace::Tracer( 1, "\t==> Write Netcdf file structure" );

	//---------------------------------------------
	// Create netcdf dimensions
	 //---------------------------------------------
	m_internalFiles->WriteDimensions();
	//---------------------------------------------
	// Create netcdf variables
	 //---------------------------------------------
	m_internalFiles->WriteVariables();

	//---------------------------------------------
	// Write netcdf dimension's data or variables that depend neither on X nor on Y
	//---------------------------------------------
	CTrace::Tracer( 1, "\t==> Write Netcdf data values" );

	CObMap* mapNetCdfVarDefs = m_internalFiles->GetNetCDFVarDefs();
	CObMap::const_iterator itVarDef;

	for ( itVarDef = mapNetCdfVarDefs->begin(); itVarDef != mapNetCdfVarDefs->end(); itVarDef++ )
	{
		CNetCDFVarDef* varDef = dynamic_cast<CNetCDFVarDef*>( itVarDef->second );

		if ( varDef == NULL )
		{
			//-----------
			continue;
			//-----------
		}

		CNetCDFCoordinateAxis* coordAxis = dynamic_cast<CNetCDFCoordinateAxis*>( varDef );

		std::string coordAxisName = ( ( coordAxis != NULL ) ? coordAxis->GetName() : "" );

		if ( ! base_t::IsProductNetCdf() && ( coordAxis == NULL ) )
		{
			//-----------
			continue;
			//-----------
		}

		if ( base_t::IsProductNetCdf() && ( coordAxis == NULL )
			&& ( varDef->HasCommonDims( m_xName ) || varDef->HasCommonDims( m_yName ) ) )
		{
			//-----------
			continue;
			//-----------
		}

		//if (! (coordAxis->GetAxis().empty()) )
		if ( coordAxisName.compare( m_xName ) == 0 )
		{
			m_internalFiles->WriteData( coordAxis, &xValues );
			//-----------
			continue;
			//-----------
		}
		else if ( coordAxisName.compare( m_yName ) == 0 )
		{
			m_internalFiles->WriteData( coordAxis, &yValues );
			//-----------
			continue;
			//-----------
		}
		else
		{
			// Is it a CFieldIndex object ? (only for "product" objects that are not CProductNetCdf object)
			// ==> Write variable data.
			CExpressionValue dataWritten;
			dataWritten.SetName( "dataWritten2" );

			CFieldIndex* fieldIndex = dynamic_cast<CFieldIndex*>( m_mapFieldIndexesToRead.Exists( varDef->GetName() ) );

			if ( fieldIndex != NULL )
			{

				CDoubleArray vect;
				fieldIndex->Read( vect );
				dataWritten.SetNewValue( vect );

			}
			else if ( base_t::IsProductNetCdf() )
			{
				// It's a CProductNetCdf, and variable depend neither on X nor on Y
				// ==> Write variable data.

				int32_t indexVar = mapVarIndexExpr.Exists( varDef->GetName() );

				if ( isDefaultValue( indexVar ) )
				{
					throw CException( CTools::Format( "ERROR: CBratProcessZFXY::WriteData() - variable index not found\n. Coordinate axis name is '%s'.\n",
						varDef->GetName().c_str() ),
						BRATHL_LOGIC_ERROR );
				}


				double* dataValues = NULL;
				size_t nbValues = 0;

				//------------
				GetDataValuesFromMatrix( indexVar, 0, 0, dataValues, nbValues );
				//------------

				dataWritten.SetNewValue( FloatType, *( varDef->GetDims() ), dataValues );

			}

			//----------------------------------
			m_internalFiles->WriteData( varDef, &dataWritten );
			//----------------------------------

		}

	}

	//---------------------------------------------
	// Write netcdf variable's data (which ones have common dim with X and/or Y)
	//---------------------------------------------


	for ( itVarDef = mapNetCdfVarDefs->begin(); itVarDef != mapNetCdfVarDefs->end(); itVarDef++ )
	{
		CNetCDFVarDef* varDef = dynamic_cast<CNetCDFVarDef*>( itVarDef->second );

		if ( varDef == NULL )
		{
			//-----------
			continue;
			//-----------
		}

		CNetCDFCoordinateAxis* coordAxis = dynamic_cast<CNetCDFCoordinateAxis*>( varDef );

		// Axis variables have already been written
		if ( coordAxis != NULL )
		{
			//-----------
			continue;
			//-----------
		}

		// Variables of a CProductNetCdf object that don't depend on axes have already been written
		if ( base_t::IsProductNetCdf()
			&& ! varDef->HasCommonDims( m_xName ) && ! varDef->HasCommonDims( m_yName ) )
		{
			//-----------
			continue;
			//-----------
		}



		int32_t indexVar = mapVarIndexExpr.Exists( varDef->GetName() );

		/*
		long nbData = varDef->GetNbData();

		CExpressionValue dataWritten;
		dataWritten.SetName("dataWritten1");

		dataWritten.SetNewValue(FloatType, *(varDef->GetDims()), NULL);

		double* data = dataWritten.GetValues();
		long iData = 0;

		//---------------------------------------
		CMatrix* matrix = GetMatrix(indexVar);
		//---------------------------------------


		for (uint32_t xPos = 0 ; xPos < m_xCount ; xPos++)
		{
		  for (uint32_t yPos = 0 ; yPos < m_yCount ; yPos++)
		  {

			uint32_t nbValues = 0;
			double* dataValues = NULL;
			//------------
			GetDataValuesFromMatrix(indexVar, xPos, yPos, dataValues, nbValues);
			//------------

			//---------------------------------------------
			if (CBratProcessZFXY::IsMatrixDoublePtr(matrix))
			//---------------------------------------------
			{
			  uint32_t indexValues = 0;

			  for (indexValues = 0 ; indexValues < nbValues; indexValues++)
			  {
				data[iData] = dataValues[indexValues];
				//--------
				iData++;
				//--------

			  }
			}
			//---------------------------------------------
			else if (CBratProcessZFXY::IsMatrixDouble(matrix))
			//---------------------------------------------
			{
			  data[iData] = *dataValues;
			  //--------
			  iData++;
			  //--------

			}


		  } //for (uint32_t yPos ....

		} //for (uint32_t xPos ....

		*/

		//---------------------------------------
		CMatrix *matrix =
			indexVar < m_grids.size() ?
			m_grids.at( indexVar ) :
			nullptr;
		//---------------------------------------

		if ( matrix == NULL )
		{
			continue;
		}

		//CProductNetCdfCF* productNetCdfCF = GetProductNetCdfCF(m_product, false);

	//    m_product->Dump(*(CTrace::GetInstance()->GetDumpContext()));
		//--------------------------
		m_internalFiles->WriteData( varDef, matrix );
		//--------------------------

		//delete []data;
		//data = NULL;

	}

	CTrace::Tracer( 1, "Data written" );
	CTrace::Tracer( 1, "Output file '%s' created", m_outputFileName.c_str() );

	m_internalFiles->Close();

	return result;
}


//----------------------------------------
//CObArrayOb* CBratProcessZFXY::GetOneMeasure(double key, bool withExcept /*= true*/)
//{   
//  return base_t::GetObArrayOb(m_measures.Exists(key), withExcept);
//}
//----------------------------------------
void CBratProcessZFXY::ResizeDataValues(CDoubleArrayOb* dataValues, uint32_t nbValues)
{   
  if (dataValues == NULL)
  {
    return;
  }

  int32_t size = (nbValues == 0 ? 1 : nbValues);
  dataValues->resize(size, base_t::MergeIdentifyUnsetData);
}
//----------------------------------------
void CBratProcessZFXY::SubstituteAxisDim( const CStringArray& fieldDims, CStringArray& fieldDimsOut )
{
	// Replace dimensions which have the same name of the X/Y fields by the X/Y expression name 

	CStringArray arrayTmp;

	for ( CStringArray::const_iterator it = m_netCdfAxisDims.begin(); it != m_netCdfAxisDims.end(); it++ )
	{
		uint32_t index = fieldDims.FindIndex( *it );
		if ( isDefaultValue( index ) )
		{
			continue;
		}

		if ( arrayTmp.size() <= index )
		{
			arrayTmp.resize( index + 1 );
		}

		arrayTmp[ index ] = *it;
	}

	int_t first = 0;
	for ( ; first < (int_t)arrayTmp.size(); first++ )
	{
		if ( ! arrayTmp.at( first ).empty() )
		{
			break;
		}

	}

	int_t last = last = arrayTmp.size() - 1;	//can be negative, cannot be size_t, 
	for ( ; last >= 0; last-- )
	{
		if ( ! arrayTmp.at( last ).empty() )
		{
			break;
		}

	}

	CStringArray axisDimsSameRangeAsField;

	for ( int_t i = first; i <= last; i++ )		//i must have same type as first and last
	{
		std::string str = arrayTmp.at( i );

		if ( str.empty() )
		{
			str = fieldDims.at( i );
		}

		if ( str.empty() )
		{
			continue;
		}

		axisDimsSameRangeAsField.Insert( str );
	}

	CStringArray arrayReplaceBy;
	arrayReplaceBy.resize( axisDimsSameRangeAsField.size() );

	if ( arrayReplaceBy.size() < 2 )
	{
		arrayReplaceBy.resize( 2 );
	}

	arrayReplaceBy[ 0 ] = m_xName;
	arrayReplaceBy[ arrayReplaceBy.size() - 1 ] = m_yName;

	for ( int_t i = 1; i < (int_t)arrayReplaceBy.size() - 1; i++ )
	{
		arrayReplaceBy[ i ] = axisDimsSameRangeAsField[ i ];
	}

	std::string replaceBy = arrayReplaceBy.ToString( ",", false );

	fieldDims.Replace( axisDimsSameRangeAsField, replaceBy, fieldDimsOut, true, true );

	/*
	CStringArray fieldDimsOutTmp;

	fieldDimsOut.Replace(m_netCdfAxisDims, m_xName, fieldDimsOutTmp, true, true);

	fieldDimsOut = fieldDimsOutTmp;

	fieldDimsOut.Replace(m_netCdfAxisDims, m_yName, fieldDimsOutTmp, true, true);

	fieldDimsOut = fieldDimsOutTmp;
	*/

}

//----------------------------------------
void CBratProcessZFXY::OnAddDimensionsFromNetCdf()
{
  base_t::OnAddDimensionsFromNetCdf();

}


//----------------------------------------
bool CBratProcessZFXY::CheckValuesSimilar( uint32_t indexExpr, double* dataValues, size_t nbValues, uint32_t xPos, uint32_t yPos, std::string& msg )
{
	CMatrix* matrix = GetMatrix( indexExpr );
	CMatrixDoublePtr* matrix_double_ptr = dynamic_cast<CMatrixDoublePtr*>( matrix );	assert__( matrix_double_ptr || dynamic_cast<CMatrixDouble*>( matrix ) );

	//---------------------------------------------
	if ( matrix_double_ptr )
	//---------------------------------------------
	{
		if ( matrix_double_ptr->GetMatrixDimsData()->size() <= 0 )
		{
			return true;
		}
	}

	size_t cols = matrix->GetMatrixNumberOfValuesData();
	if ( cols <= 0 )
	{
		return true;
	}

	if ( cols != nbValues )
	{
		return false;
	}


	double* dataValuesRef = matrix->At( xPos, yPos );
	bool bSimilarValues = true;

	//---------------------------------------------
	if ( matrix_double_ptr )
	//---------------------------------------------
	{
		for ( uint32_t indexValues = 0; indexValues < cols; indexValues++ )
		{
			if ( dataValuesRef[ indexValues ] == base_t::MergeIdentifyUnsetData )
			{
				break;
			}
			if ( !areEqual( dataValuesRef[ indexValues ], dataValues[ indexValues ] ) )
			{
				bSimilarValues = false;
				break;
			}
		}
	}
	//---------------------------------------------
	else
	//---------------------------------------------
	{
		if ( !areEqual( *dataValuesRef, *dataValues ) )
		{
			bSimilarValues = false;
		}
	}


	std::string format = "WARNING - Regarding to X / Y values %.15g / %.15g, values contained in %s dimension are not similar with read previous values.\n"
		"Only the first values will be stored in the NetCdf output file.\n"
		"Expressions below that depend on %s dimension might be inconsistent:\n%s";

	CNetCDFDimension* netCDFdim = m_internalFiles->GetNetCDFDim( m_names[ indexExpr ] );
	if ( netCDFdim == NULL )
	{
		throw CException( CTools::Format( "ERROR - CBratProcessZFXY::CheckValuesSimilar - Unable to find dimension '%s' in output Netcdf file.",
			m_names[ indexExpr ].c_str() ),
			BRATHL_LOGIC_ERROR );
	}

	CStringArray strArray;
	strArray.Insert( m_names[ indexExpr ] );

	CStringArray complement;
	CStringArray* coordVars = netCDFdim->GetCoordinateVariables();

	strArray.Complement( *coordVars, complement );

	std::string coordVarAsString = complement.ToString( ", ", false );

	msg = CTools::Format( format.c_str(),
		m_xData[ xPos ],
		m_yData[ yPos ],
		m_names[ indexExpr ].c_str(),
		m_names[ indexExpr ].c_str(),
		coordVarAsString.c_str() );
	;

	return bSimilarValues;
}



//----------------------------------------
// If result is NULL, Data is modified upon exit.
// If result is not NULL it must point to an area
// big enough to contain at least nbX*nbY data.

bool CBratProcessZFXY::LoessFilterGrid(double	*data, double	*result,
		                                   int_t nbX, bool circularX, int_t waveLengthX,
		                                   int_t nbY, bool circularY, int_t waveLengthY,
		                                   bool smoothData, bool extrapolData)
{
	// index limits for inner loops
  int_t	minX;
  int_t	maxX;
	// in case of a grid covering the
				// equator (circular grid)
  int_t	minY;
  int_t	maxY;

  bool		doDist;
  double	dist	= 0.0;

  // Calculates the position of a data inside the data array
  #define GRIDINDEX(X, Y) ( ( ((X) + nbX) % nbX ) * nbY + ((Y) + nbY) % nbY )

  waveLengthX	/= 2;

  if ((waveLengthX < 0) || (waveLengthX > (nbX / 2)))
  {
    waveLengthX	= 0;
  }

  waveLengthY	/= 2;

  if ((waveLengthY < 0) || (waveLengthY > (nbY/2)))
  {
    waveLengthY	= 0;
  }

  doDist = (waveLengthX > 0) && (waveLengthY > 0);

  if (circularX)
  {
    // The grid covers the whole earth in longitude so we can have indexes	
    /// outside the array bounds. GRIDINDEX take this into account		
    minX	= -nbX;
    maxX	= 2 * nbX;
  }
  else
  {
    minX	= 0;
    maxX	= nbX;
  }

  if (circularY)
  {
    // The grid covers the whole earth in longitude so we can have indexes
    // outside the array bounds. GRIDINDEX take this into account
    minY	= -nbY;
    maxY	= 2 * nbY;
  }
  else
  {
    minY	= 0;
    maxY	= nbY;
  }

  int_t	nbData	= nbX * nbY;
  
  double	*buffer	= (result != NULL ? result : new double[nbData]);

  try // To catch exceptions for data integrity
  {

    // Initializes result with input data
    memcpy(buffer, data, sizeof(*buffer) * nbData);

    for (int_t xIndex=0; xIndex < nbX; xIndex++)
    {
      for (int_t yIndex=0; yIndex < nbY; yIndex++)
      {
      	int_t	indexInGrid	= GRIDINDEX(xIndex, yIndex);

	      bool	isDefault	= isDefaultValue( data[indexInGrid] );

        if ((smoothData && (! isDefault)) || (extrapolData && isDefault))
        {
          // Smoothing with pixels around the current one
	        double smooth	= 0.0;
	        double weight	= 0.0;
	        
          for (int_t xLocal = MAX(minX, xIndex-waveLengthX) ; xLocal < MIN(maxX, xIndex+waveLengthX+1); xLocal++)
	        {
	          double sqrNormDistX	= static_cast<double>(xLocal-xIndex) / static_cast<double>(waveLengthX);
	          
            sqrNormDistX *= sqrNormDistX;

	          for (int_t yLocal = MAX(minY, yIndex-waveLengthY) ; yLocal < MIN(maxY, yIndex + waveLengthY + 1); yLocal++)
	          {
	            int_t	localIndexInGrid	= GRIDINDEX(xLocal, yLocal);

	            if (! isDefaultValue(data[localIndexInGrid]))
	            {
		            // Weighting
		            if (doDist)
		            {
	                double normDistY	= static_cast<double>(yLocal-yIndex) / static_cast<double>(waveLengthY);
		              
                  dist = sqrt( sqrNormDistX + (normDistY * normDistY) );
		            }

		            double wij = (dist <= 1.0 ? pow((1.0 - pow(dist, 3.0)),3.0) : 0.0);

		            smooth += wij * data[localIndexInGrid];
		            weight += wij;
	            }
	          }
	        }

	        if (!isZero(weight))
          {
	          buffer[indexInGrid]	= smooth / weight;
          }
        
        } // end  if ((SmoothData && (! isDefault)) || (ExtrapolData && isDefault))
      }
    }

    // Report computed result
    if (result == NULL)
    {
      memcpy(data, buffer, sizeof(*data)*nbData);
    }
  }
  catch (CException& e)
  {
    if (result == NULL) 
    {
      delete []buffer;
      buffer = NULL;
    }

    throw e;
  }
  catch (...)
  {
    if (result == NULL) 
    {
      delete []buffer;
      buffer = NULL;
    }

    throw;
  }
 
  if (result == NULL) 
  {
    delete []buffer;
    buffer = NULL;
  }
  
  return true;

}
//----------------------------------------
bool CBratProcessZFXY::LoessFilterGrid( CMatrix& matrix, CMatrix *result,
	bool circularX, int_t waveLengthX,
	bool circularY, int_t waveLengthY,
	bool smoothData, bool extrapolData )
{
	CMatrixDoublePtr *matrix_double_ptr = dynamic_cast<CMatrixDoublePtr*>( &matrix );
	CMatrixDouble *matrix_double = dynamic_cast<CMatrixDouble*>( &matrix );				assert__( matrix_double_ptr || matrix_double );

	if ( !matrix_double_ptr )
	{
		CMatrixDouble *resultDouble = dynamic_cast<CMatrixDouble*>( result );

		if ( result && !resultDouble )
		{
			return false;
		}

		return CBratProcessZFXY::LoessFilterGrid( *matrix_double, resultDouble,
			circularX, waveLengthX,
			circularY, waveLengthY,
			smoothData, extrapolData );

	}

	if ( matrix_double_ptr )
	{
		CMatrixDoublePtr *resultDoublePtr = dynamic_cast<CMatrixDoublePtr*>( result );

		if ( result && !resultDoublePtr )
		{
			return false;
		}

		return CBratProcessZFXY::LoessFilterGrid( *matrix_double_ptr, resultDoublePtr,
			circularX, waveLengthX,
			circularY, waveLengthY,
			smoothData, extrapolData );
	}

	return false;
}


//----------------------------------------
// If result is NULL, matrix is modified upon exit.
// If result is not NULL it must point to an matrix with the same numbers of rows and columns than matrix

bool CBratProcessZFXY::LoessFilterGrid( CMatrixDouble& matrix, CMatrixDouble *result,
	bool circularX, int_t waveLengthX,
	bool circularY, int_t waveLengthY,
	bool smoothData, bool extrapolData )
{
	int_t nbX = matrix.GetNumberOfRows();
	int_t nbY = matrix.GetNumberOfCols();

	// index limits for inner loops
	int_t	minX;
	int_t	maxX;
	// in case of a grid covering the
				// equator (circular grid)
	int_t	minY;
	int_t	maxY;

	bool		doDist;
	double	dist	= 0.0;

	// Calculates the position of a data inside the data array
	//#define GRIDINDEX(X, Y) ( ( ((X) + nbX) % nbX ) * nbY + ((Y) + nbY) % nbY )

	waveLengthX	/= 2;

	if ( ( waveLengthX < 0 ) || ( waveLengthX > ( nbX / 2 ) ) )
	{
		waveLengthX	= 0;
	}

	waveLengthY	/= 2;

	if ( ( waveLengthY < 0 ) || ( waveLengthY > ( nbY / 2 ) ) )
	{
		waveLengthY	= 0;
	}

	doDist = ( waveLengthX > 0 ) && ( waveLengthY > 0 );

	if ( circularX )
	{
		// The grid covers the whole earth in longitude so we can have indexes
		// outside the array bounds. GRIDINDEX take this into account		
		minX	= -nbX;
		maxX	= 2 * nbX;
	}
	else
	{
		minX	= 0;
		maxX	= nbX;
	}

	if ( circularY )
	{
		// The grid covers the whole earth in longitude so we can have indexes
		// outside the array bounds. GRIDINDEX take this into account
		minY	= -nbY;
		maxY	= 2 * nbY;
	}
	else
	{
		minY	= 0;
		maxY	= nbY;
	}

	//int32_t	nbData	= nbX * nbY;

	CMatrixDouble *buffer = result ? result : new CMatrixDouble( matrix.GetNumberOfRows(), matrix.GetNumberOfCols() );
	CMatrixDouble* matrixTmp = &matrix;

	try // To catch exceptions for data integrity
	{
		// Initializes result with input data
		*buffer = *matrixTmp;

		for ( int32_t xIndex = 0; xIndex < nbX; xIndex++ )
		{
			for ( int32_t yIndex = 0; yIndex < nbY; yIndex++ )
			{
				//int32_t	indexInGrid	= GRIDINDEX(xIndex, yIndex);
				double * values = matrixTmp->At( xIndex, yIndex );
				if ( values == NULL )
				{
					continue;
				}

				double value = values[ 0 ];

				bool isDefault	= isDefaultValue( value );

				if ( ( smoothData && ( ! isDefault ) ) || ( extrapolData && isDefault ) )
				{
					// Smoothing with pixels around the current one
					double smooth	= 0.0;
					double weight	= 0.0;

					for ( int_t xLocal = MAX( minX, ( xIndex - waveLengthX ) ); xLocal < MIN( maxX, ( xIndex + waveLengthX + 1 ) ); xLocal++ )
					{
						double sqrNormDistX	= static_cast<double>( xLocal - xIndex ) / static_cast<double>( waveLengthX );

						sqrNormDistX *= sqrNormDistX;
						int_t xx = ( ( xLocal + nbX ) % nbX );

						for ( int_t yLocal = MAX( minY, ( yIndex - waveLengthY ) ); yLocal < MIN( maxY, ( yIndex + waveLengthY + 1 ) ); yLocal++ )
						{
							int_t yy = ( ( yLocal + nbY ) % nbY );

							//int32_t	localIndexInGrid	= GRIDINDEX(xLocal, yLocal);
							//int32_t	localIndexInGrid2	= GRIDINDEX(xx, yy);

							double * valuesLocal = matrixTmp->At( xx, yy );

							if ( valuesLocal == NULL )
							{
								continue;
							}

							double valueLocal = valuesLocal[ 0 ];

							if ( ! isDefaultValue( valueLocal ) )
							{
								// Weighting
								if ( doDist )
								{
									double normDistY	= static_cast<double>( yLocal - yIndex ) / static_cast<double>( waveLengthY );

									dist = sqrt( sqrNormDistX + ( normDistY * normDistY ) );
								}

								double wij = ( dist <= 1.0 ? pow( ( 1.0 - pow( dist, 3.0 ) ), 3.0 ) : 0.0 );

								smooth += wij * valueLocal;
								weight += wij;
							}
						}
					}

					if ( !isZero( weight ) )
					{
						double * bufferValues = buffer->At( xIndex, yIndex );
						if ( bufferValues != NULL )
						{
							bufferValues[ 0 ] = smooth / weight;
						}
					}

				} // end  if ((SmoothData && (! isDefault)) || (ExtrapolData && isDefault))
			}
		}

		// Report computed result
		if ( result == NULL )
		{
			*matrixTmp = *buffer;
		}
	}
	catch ( CException& e )
	{
		if ( result == NULL )
		{
			delete buffer;
			buffer = NULL;
		}

		throw e;
	}
	catch ( ... )
	{
		if ( result == NULL )
		{
			delete buffer;
			buffer = NULL;
		}

		throw;
	}

	if ( result == NULL )
	{
		delete buffer;
		buffer = NULL;
	}

	return true;
}

//----------------------------------------
// If result is NULL, matrix is modified upon exit.
// If result is not NULL it must point to an matrix with the same numbers of rows and columns than matrix

bool CBratProcessZFXY::LoessFilterGrid( CMatrixDoublePtr& matrix, CMatrixDoublePtr *result,
	bool circularX, int_t waveLengthX,
	bool circularY, int_t waveLengthY,
	bool smoothData, bool extrapolData )
{

	size_t numOfValuePerPoint = matrix.GetMatrixNumberOfValuesData();

	if ( numOfValuePerPoint != 1 )
	{
		CTrace::Tracer( 1, CTools::Format( "WARNING - The filters are not applied to '%s' expression because the number of values per point (x/y) is not equal to 1 (number is %d)",
			matrix.GetName().c_str(), numOfValuePerPoint ) );

		return false;
	}


	int_t nbX = matrix.GetNumberOfRows();
	int_t nbY = matrix.GetNumberOfCols();

	// index limits for inner loops
	int_t	minX;
	int_t	maxX;
	// in case of a grid covering the
				// equator (circular grid)
	int_t	minY;
	int_t	maxY;

	bool		doDist;
	double	dist	= 0.0;

	// Calculates the position of a data inside the data array
	//#define GRIDINDEX(X, Y) ( ( ((X) + nbX) % nbX ) * nbY + ((Y) + nbY) % nbY )

	waveLengthX	/= 2;

	if ( ( waveLengthX < 0 ) || ( waveLengthX > ( nbX / 2 ) ) )
	{
		waveLengthX	= 0;
	}

	waveLengthY	/= 2;

	if ( ( waveLengthY < 0 ) || ( waveLengthY > ( nbY / 2 ) ) )
	{
		waveLengthY	= 0;
	}

	doDist = ( waveLengthX > 0 ) && ( waveLengthY > 0 );

	if ( circularX )
	{
		// The grid covers the whole earth in longitude so we can have indexes
		// outside the array bounds. GRIDINDEX take this into account		
		minX	= -nbX;
		maxX	= 2 * nbX;
	}
	else
	{
		minX	= 0;
		maxX	= nbX;
	}

	if ( circularY )
	{
		// The grid covers the whole earth in longitude so we can have indexes
		// outside the array bounds. GRIDINDEX take this into account
		minY	= -nbY;
		maxY	= 2 * nbY;
	}
	else
	{
		minY	= 0;
		maxY	= nbY;
	}

	//int32_t	nbData	= nbX * nbY;

	CMatrixDoublePtr *buffer = NULL;
	if ( result != NULL )
	{
		buffer = result;
	}
	else
	{
		buffer = new CMatrixDoublePtr( matrix.GetNumberOfRows(), matrix.GetNumberOfCols() );
	}

	CMatrixDoublePtr *matrixTmp = &matrix;


	try // To catch exceptions for data integrity
	{
		// Initializes result with input data
		*buffer = *matrixTmp;

		for ( int_t xIndex = 0; xIndex < nbX; xIndex++ )
		{
			for ( int_t yIndex = 0; yIndex < nbY; yIndex++ )
			{
				//int_t	indexInGrid	= GRIDINDEX(xIndex, yIndex);
				double * values = matrixTmp->At( xIndex, yIndex );
				if ( values == NULL )
				{
					continue;
				}

				double value = values[ 0 ];

				bool isDefault	= isDefaultValue( value );

				if ( ( smoothData && ( ! isDefault ) ) || ( extrapolData && isDefault ) )
				{
					// Smoothing with pixels around the current one
					double smooth	= 0.0;
					double weight	= 0.0;

					for ( int_t xLocal = MAX( minX, ( xIndex - waveLengthX ) ); xLocal < MIN( maxX, ( xIndex + waveLengthX + 1 ) ); xLocal++ )
					{
						double sqrNormDistX	= static_cast<double>( xLocal - xIndex ) / static_cast<double>( waveLengthX );

						sqrNormDistX *= sqrNormDistX;
						int_t xx = ( ( xLocal + nbX ) % nbX );

						for ( int_t yLocal = MAX( minY, ( yIndex - waveLengthY ) ); yLocal < MIN( maxY, ( yIndex + waveLengthY + 1 ) ); yLocal++ )
						{
							int_t yy = ( ( yLocal + nbY ) % nbY );

							//int32_t	localIndexInGrid	= GRIDINDEX(xLocal, yLocal);
							//int32_t	localIndexInGrid2	= GRIDINDEX(xx, yy);

							double * valuesLocal = matrixTmp->At( xx, yy );

							if ( valuesLocal == NULL )
							{
								continue;
							}

							double valueLocal = valuesLocal[ 0 ];

							if ( ! isDefaultValue( valueLocal ) )
							{
								// Weighting
								if ( doDist )
								{
									double normDistY	= static_cast<double>( yLocal - yIndex ) / static_cast<double>( waveLengthY );

									dist = sqrt( sqrNormDistX + ( normDistY * normDistY ) );
								}

								double wij = ( dist <= 1.0 ? pow( ( 1.0 - pow( dist, 3.0 ) ), 3.0 ) : 0.0 );

								smooth += wij * valueLocal;
								weight += wij;
							}
						}
					}

					if ( !isZero( weight ) )
					{
						double * bufferValues = buffer->At( xIndex, yIndex );
						if ( bufferValues != NULL )
						{
							bufferValues[ 0 ] = smooth / weight;
						}
					}

				} // end  if ((SmoothData && (! isDefault)) || (ExtrapolData && isDefault))
			}
		}

		// Report computed result
		if ( result == NULL )
		{
			*matrixTmp = *buffer;
		}
	}
	catch ( CException& e )
	{
		if ( result == NULL )
		{
			delete buffer;
			buffer = NULL;
		}

		throw e;
	}
	catch ( ... )
	{
		if ( result == NULL )
		{
			delete buffer;
			buffer = NULL;
		}

		throw;
	}

	if ( result == NULL )
	{
		delete buffer;
		buffer = NULL;
	}

	return true;
}



}
