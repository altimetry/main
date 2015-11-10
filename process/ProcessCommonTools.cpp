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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA	 02110-1301, USA.
*/

#include <cmath>
#include <cstdlib>

#include <string>

#include "brathl.h"

#include "Date.h"
#include "Exception.h"
#include "Expression.h"
#include "Tools.h"
#include "Trace.h"
#include "LatLonPoint.h"

// Included inside namespace since it is local data only
// Definitions of ProcessCommonTools.h used (keyword names)
#include "ParametersDictionary.h"

#include "ProcessCommonTools.h"
using namespace brathl;
using namespace processes;

namespace processes
{




/*
** Print help about a list of keywords which can be foun in a parameter file
** definitions are in ParametersDictionary.h
*/
void PrintParameterHelp
		(std::ostream		&where,
		 const KeywordHelp	*keywordList)
{

  CBratProcess::PrintParameterHelp (where, keywordList);

}



/*
** Checks the command line parameters (with -h -k or parameter file)
** and return true if command line is invalid
** Last entry of KeywordList MUST BE NULL or result is unpredictible
** (and maybe a core dump)
*/
bool CheckCommandLineOptions
		(int			argc,
		 char			**argv,
		 const std::string		&helpString,
		 const KeywordHelp	*keywordList,
		 std::string			&commandFileName)
{
  return  CBratProcess::CheckCommandLineOptions(argc, argv, helpString, keywordList, commandFileName);


}

//----------------------------------------
int32_t GetFileList
		(CFileParams		&params,
		 const std::string		&keyword,
		 CStringArray		&names,
		 const std::string		&traceDescription,
		 int32_t		minOccurences	/*= 1*/,
		 int32_t		maxOccurences	/*= -1*/,
		 bool			printTrace	/*= true*/)
{
  return CBratProcess::GetFileList( params, keyword, names, traceDescription, minOccurences, maxOccurences, printTrace);
}

//----------------------------------------
int32_t GetVarDef
		(CFileParams		&params,
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
		 int32_t		index		/*= 0*/,
		 int32_t		minOccurences	/*= 1*/,
		 int32_t		maxOccurences	/*= 1*/,
		 bool			printTrace	/*= true*/)
{

  return  CBratProcess::GetVarDef(params, 
                                  prefix, 
                                  field, 
                                  name, 
                                  kind, 
                                  unit, 
                                  title, 
                                  comment, 
                                  dataFormat, 
                                  group, 
                                  traceDescription, 
                                  index, 
                                  minOccurences, 
                                  maxOccurences, 
                                  printTrace);

}

//----------------------------------------
int32_t GetVarDef
		(CFileParams		&params,
		 const std::string		&prefix,
		 CExpression    	&field,
		 std::string			*name,
		 NetCDFVarKind		*kind,
		 CUnit			*unit,
		 std::string			*title,
		 std::string			*comment,
		 std::string			*dataFormat,
		 uint32_t               *group,
		 double			&min,
		 double			&max,
		 uint32_t		&count,
		 double			&step,
		 const std::string		&traceDescription,
		 int32_t		index		/*= 0*/,
		 int32_t		minOccurences	/*= 1*/,
		 int32_t		maxOccurences	/*= 1*/,
		 bool			printTrace	/*= true*/)
{

  return  CBratProcess::GetVarDef(params, 
                                  prefix,
  	                              field,
	                                name,
		                              kind,
		                              unit,
		                              title,
		                              comment,
		                              dataFormat,
		                              group,
		                              min,
		                              max,
		                              count,
		                              step,
		                              traceDescription,
		                              index, 
		                              minOccurences,
		                              maxOccurences,
		                              printTrace);


}

//----------------------------------------
void GetFilterDefinitions
		(CFileParams		&params,
		 const std::string		&prefix,
		 bool			*smooth,
		 bool			*extrapolate,
		 int32_t		index		/*= 0*/,
		 int32_t		occurences	/*= 1*/,
		 bool			printTrace	/*= true*/)
{
  CBratProcess::GetFilterDefinitions(params, prefix, smooth, extrapolate, index, occurences, printTrace);
}

//----------------------------------------
void GetLoessCutoff
		(CFileParams		&params,
		 int32_t		*xCutoff,
		 int32_t		*yCutoff,
		 int32_t		index		/*= 0*/,
		 int32_t		occurences	/*= 1*/,
		 bool			printTrace	/*= true*/)
{
  CBratProcess::GetLoessCutoff(params, xCutoff, yCutoff, index,	occurences, printTrace);
}
//----------------------------------------

CBratProcess::MergeDataMode GetDataMode
		(CFileParams	&params,
		 int32_t	minOccurences	/*= 0*/,
		 int32_t	maxOccurences	/*= 1*/,
		 const std::string	&keyword	/*= "DATA_MODE"*/,
		 int32_t	index		/*= 0*/,
		 CBratProcess::MergeDataMode	Default		/*= CBratProcess::pctMEAN*/)
{
  return CBratProcess::GetDataMode(params, minOccurences, maxOccurences, keyword, index, Default);

}

//----------------------------------------

CBratProcess::MergeDataMode GetDataMode
		(CFileParams	&params,
		 const std::string	&prefix,
		 int32_t	minOccurences	/*= 0*/,
		 int32_t	maxOccurences	/*= 1*/,
		 int32_t	index		/*= 0*/,
		 CBratProcess::MergeDataMode	Default		/*= CBratProcess::pctMEAN*/)
{
  return CBratProcess::GetDataMode(params, prefix, minOccurences, maxOccurences, index, Default);

}
//----------------------------------------

std::string DataModeStr
		(CBratProcess::MergeDataMode Mode)
{
  return CBratProcess::DataModeStr(Mode);

}

//----------------------------------------

CBratProcess::OutsideMode GetOutsideMode
		(CFileParams	&params,
		 int32_t	minOccurences	/*= 0*/,
		 int32_t	maxOccurences	/*= 1*/,
		 const std::string	&keyword	/*= "OUTSIDE_MODE"*/,
		 int32_t	index		/*= 0*/,
		 CBratProcess::OutsideMode	defaultValue		/*= pctSTRICT*/)
{
  return CBratProcess::GetOutsideMode(params, minOccurences, maxOccurences, keyword, index, defaultValue);
}

//----------------------------------------
std::string OutsideModeStr
		(CBratProcess::OutsideMode mode)
{
  return CBratProcess::OutsideModeStr(mode);
}
//----------------------------------------
bool CheckOutsideValue
		(double		&value,
		 double		min,
		 double		max,
		 double		step,
     CBratProcess::OutsideMode	mode)
{
  return CBratProcess::CheckOutsideValue(value, min, max, step, mode);
}

//----------------------------------------
CBratProcess::PositionMode GetPositionMode
		(CFileParams	&params,
		 int32_t	minOccurences	/*= 0*/,
		 int32_t	maxOccurences	/*= 1*/,
		 const std::string	&keyword	/*= "POSITION_MODE"*/,
		 int32_t	index		/*= 0*/,
		 CBratProcess::PositionMode	defaultValue		/*= pctNEAREST*/)
{

  return CBratProcess::GetPositionMode(params, minOccurences, maxOccurences, keyword, index, defaultValue);
}
//----------------------------------------
std::string PositionModeStr
		(CBratProcess::PositionMode	mode)
{
  return CBratProcess::PositionModeStr(mode);
}
//----------------------------------------
bool CheckPositionValue
		(double		value,
		 double		min,
		 double		step,
		 uint32_t	count,
		 CBratProcess::PositionMode	mode,
		 uint32_t	&position)
{
  return CBratProcess::CheckPositionValue(value, min, step, count, mode, position);
}

//----------------------------------------
double CheckLongitudeValue
		(double			value,
		 double			min,
		 NetCDFVarKind		kind)
{
  return CBratProcess::CheckLongitudeValue( value, min, kind);
}
//----------------------------------------
bool IsLongitudeCircular
		(double			min,
		 double			max,
		 NetCDFVarKind		kind)
{
  return CBratProcess::IsLongitudeCircular(min, max, kind, NULL);
}





















/*
** Determines how many values are needed to compute the incremental operation
** defined by 'Mode'
*/
int32_t GetMergedDataSlices
		(CBratProcess::MergeDataMode mode)
{
  return CBratProcess::GetMergedDataSlices(mode);
}


/*
** Initialises data for use with the merging routines
*/
/*
void InitMergedData
		(double			*Data,
		 uint32_t		DataCount,
		 uint32_t		DataSlices,
		 MergeDataMode		Mode)
{
  double	InitValue	= MergeIdentifyUnsetData;

  for (uint32_t IndexValues=0; IndexValues<DataCount; IndexValues++)
  {
    Data[IndexValues]	= InitValue;
  }
}
*/
void InitMergedData
		(double			*Data,
		 uint32_t		DataCount)
{
  double	InitValue	= CBratProcess::MergeIdentifyUnsetData;

  for (uint32_t IndexValues = 0; IndexValues < DataCount; IndexValues++)
  {
    Data[IndexValues]	= InitValue;
  }
}


/*
** Add a new value to data and computes the partial result
** according to the operation asked
*/
void MergeDataValue
		(double			*Data,
		 double			Value,
		 uint32_t		CountValueOffset,
		 uint32_t		MeanValueOffset,
		 CBratProcess::MergeDataMode		Mode)
{
  if (CTools::IsDefaultValue(Value))
    return;

  double Dummy	= 0.0;

  switch (Mode)
  {
    case CBratProcess::pctFIRST:
	    if (*Data == CBratProcess::MergeIdentifyUnsetData)
	      *Data	= Value;
	    break;
    case CBratProcess::pctLAST:
	    if (! CTools::IsDefaultValue(Value))
	      *Data	= Value;
	    break;
    case CBratProcess::pctMIN:
	    if (*Data == CBratProcess::MergeIdentifyUnsetData)
	      *Data	= Value;
	    else
	      *Data	= CTools::Min(*Data, Value);
	    break;
    case CBratProcess::pctMAX:
	    if (*Data == CBratProcess::MergeIdentifyUnsetData)
	      *Data	= Value;
	    else
	      *Data	= CTools::Max(*Data, Value);
	    break;
    case CBratProcess::pctSUM:
	    if (*Data == CBratProcess::MergeIdentifyUnsetData)
	      *Data	= Value;
	    else
	      *Data	= CTools::Plus(*Data, Value);
	    break;
    case CBratProcess::pctSUBSTRACT:
	    if (*Data == CBratProcess::MergeIdentifyUnsetData)
	      *Data	= Value;
	    else
	      *Data	= CTools::Minus(*Data, Value);
	    break;
    case CBratProcess::pctPRODUCT:
	    if (*Data == CBratProcess::MergeIdentifyUnsetData)
	      *Data	= Value;
	    else
	      *Data	= CTools::Multiply(*Data, Value);
	    break;
    case CBratProcess::pctMEAN:
	    if (*Data == CBratProcess::MergeIdentifyUnsetData)
	    {
	      Data[CountValueOffset]	= 0;
	    }

	    CTools::DoIncrementalStats(Value,
				       Data[CountValueOffset],
				       *Data,
				       Dummy,
				       Dummy,
				       Dummy);
	    break;
    case CBratProcess::pctSTDDEV:
	    if (*Data == CBratProcess::MergeIdentifyUnsetData)
	    {
	      Data[CountValueOffset]	= 0;
	    }

	    CTools::DoIncrementalStats(Value,
				       Data[CountValueOffset],
				       Data[MeanValueOffset],
				       *Data,
				       Dummy,
				       Dummy);
	    break;
    case CBratProcess::pctCOUNT:
	    *Data	= (*Data == CBratProcess::MergeIdentifyUnsetData ?
				1.0 :
				*Data + 1);
	    break;
    default:
	    throw CException(CTools::Format("PROGRAM ERROR: DataMode %d unknown",
					    Mode),
			     BRATHL_LOGIC_ERROR);
  }
}


/*
** All values for data have been integrated. Computes
** the final value.
*/
void FinalizeMergingOfDataValues
		(double			*Data,
		 uint32_t		CountValueOffset,
		 uint32_t		MeanValueOffset,
		 CBratProcess::MergeDataMode		Mode)
{
  double Dummy	= 0.0;

  switch (Mode)
  {
    case CBratProcess::pctFIRST:
    case CBratProcess::pctLAST:
    case CBratProcess::pctMIN:
    case CBratProcess::pctMAX:
    case CBratProcess::pctCOUNT:
    case CBratProcess::pctSUM:
    case CBratProcess::pctSUBSTRACT:
    case CBratProcess::pctPRODUCT:
	    if (*Data == CBratProcess::MergeIdentifyUnsetData)
	    {
	      CTools::SetDefaultValue(*Data);
	    }
	    break;
    case CBratProcess::pctMEAN:
	    if (*Data == CBratProcess::MergeIdentifyUnsetData)
	    {
	      CTools::SetDefaultValue(*Data);
	    }
	    else
	    {
	      CTools::FinalizeIncrementalStats(Data[CountValueOffset],
					       *Data,
					       Dummy,
					       Dummy,
					       Dummy);
	    }
	    break;
    case CBratProcess::pctSTDDEV:
	    if (*Data == CBratProcess::MergeIdentifyUnsetData)
	    {
	      CTools::SetDefaultValue(*Data);
	    }
	    else
	    {
	      CTools::FinalizeIncrementalStats(Data[CountValueOffset],
					       Data[MeanValueOffset],
					       *Data,
					       Dummy,
					       Dummy);
	    }
	    break;
    default:
	    throw CException(CTools::Format("PROGRAM ERROR: DataMode %d unknown",
					    Mode),
			     BRATHL_LOGIC_ERROR);
  }
}


// If result is NULL, Data is modified upon exit.
// If result is not NULL it must point to an area
// big enough to contain at least NbX*NbY data.
void LoessFilterGrid
		(double		*Data,
		 double		*result,
		 int32_t	NbX,
		 bool		CircularX,
		 int32_t	WaveLengthX,
		 int32_t	NbY,
		 bool		CircularY,
		 int32_t	WaveLengthY,
		 bool		SmoothData,
		 bool		ExtrapolData
		 )
{
  int32_t	MinX, MaxX;	// index limits for inner loops
  int32_t	MinY, MaxY;	// in case of a grid covering the
				// equator (circular grid)
  bool		DoDist;
  double	Dist	= 0.0;

  // Calculates the position of a data inside the data array
  #define GRIDINDEX(X, Y) ((((X)+NbX)%NbX)*NbY+((Y)+NbY)%NbY)

  WaveLengthX	/= 2;
  if ((WaveLengthX<0) || (WaveLengthX > (NbX/2)))
    WaveLengthX	= 0;
  WaveLengthY	/= 2;
  if ((WaveLengthY<0) || (WaveLengthY > (NbY/2)))
    WaveLengthY	= 0;

  DoDist	= (WaveLengthX > 0) && (WaveLengthY > 0);

  if (CircularX)
  {
    /* The grid covers the whole earth in longitude so we can have indexes	*/
    /* outside the array bounds. GRIDINDEX take this into account		*/
    MinX	= -NbX;
    MaxX	= 2*NbX;
  }
  else
  {
    MinX	= 0;
    MaxX	= NbX;
  }
  if (CircularY)
  {
    /* The grid covers the whole earth in longitude so we can have indexes	*/
    /* outside the array bounds. GRIDINDEX take this into account		*/
    MinY	= -NbY;
    MaxY	= 2*NbY;
  }
  else
  {
    MinY	= 0;
    MaxY	= NbY;
  }

  int32_t	NbData	= NbX*NbY;
  double	*Buffer	= (result != NULL ? result : new double[NbData]);

  try // To catch exceptions for data integrity
  {

    // Initializes result with input data
    memcpy(Buffer, Data, sizeof(*Buffer)*NbData);

    for (int32_t XIndex=0; XIndex < NbX; XIndex++)
    {
      for (int32_t YIndex=0; YIndex < NbY; YIndex++)
      {
	int32_t	IndexInGrid	= GRIDINDEX(XIndex, YIndex);
	bool	IsDefault	= CTools::IsDefaultValue(Data[IndexInGrid]);

        if ((SmoothData && (! IsDefault)) ||
	    (ExtrapolData && IsDefault))
        {
          // Smoothing with pixels around the current one
	  double Smooth	= 0.0;
	  double Weight	= 0.0;
	  for (int32_t XLocal=MAX(MinX, XIndex-WaveLengthX);
		       XLocal< MIN(MaxX, XIndex+WaveLengthX+1);
		       XLocal++)
	  {
	    double SqrNormDistX	= static_cast<double>(XLocal-XIndex)/
				  static_cast<double>(WaveLengthX);
	    SqrNormDistX	*= SqrNormDistX;

	    for (int32_t YLocal=MAX(MinY, YIndex-WaveLengthY);
			 YLocal<MIN(MaxY, YIndex+WaveLengthY+1);
			 YLocal++)
	    {
	      int32_t	LocalIndexInGrid	= GRIDINDEX(XLocal, YLocal);
	      if (! CTools::IsDefaultValue(Data[LocalIndexInGrid]))
	      {
		// Weighting
		if (DoDist)
		{
	          double NormDistY	= static_cast<double>(YLocal-YIndex)/
					  static_cast<double>(WaveLengthY);
		  Dist	=  sqrt(SqrNormDistX+(NormDistY*NormDistY));
		}
		double Wij	= (Dist <= 1.0 ? pow((1.0-pow(Dist, 3.0)),3.0) : 0.0);
		Smooth		+= Wij*Data[LocalIndexInGrid];
		Weight		+= Wij;
	      }
	    }
	  }
	  if (! CTools::IsZero(Weight))
	    Buffer[IndexInGrid]	= Smooth/Weight;
        }
      }
    }

    // Report computed result
    memcpy(Data, Buffer, sizeof(*Data)*NbData);
  }
  catch (...)
  {
    if (result != NULL) delete []Buffer;
    throw;
  }
  if (result != NULL) delete []Buffer;
}




}
