/*
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
#ifndef _ProcessCommonTools_h_
#define _ProcessCommonTools_h_

#include "netcdf.h"
#include <string>
#include "FileParams.h"
#include "InternalFiles.h"

#include "BratProcess.h"
using namespace brathl;
using namespace processes;

namespace processes
{

#ifndef MIN
#define MIN(X,Y)	((X) <= (Y) ? (X) : (Y))
#endif

#ifndef MAX
#define MAX(X,Y)	((X) >= (Y) ? (X) : (Y))
#endif





/*
** Print help about a list of keywords which can be found in a parameter file
** definitions are in ParametersDictionary.h
*/
void PrintParameterHelp
		(std::ostream		&where,
		 const KeywordHelp	*KeywordList);

/*
** Checks the command line parameters (with -h -k or parameter file)
** and return true if command line is invalid
** Last entry of KeywordList MUST BE NULL or result is unpredictable
** (and maybe a core dump)
*/
bool CheckCommandLineOptions
		(int			argc,
		 char			**argv,
		 const std::string		&HelpString,
		 const KeywordHelp	*KeywordList,
		 std::string			&CommandFileName);


int32_t GetFileList
		(CFileParams		&Params,
		 const std::string		&Keyword,
		 CStringArray		&Names,
		 const std::string		&TraceDescription,
		 int32_t		MinOccurences	= 1,
		 int32_t		MaxOccurences	= -1,
		 bool			PrintTrace	= true);

int32_t GetVarDef
		(CFileParams		&Params,
		 const std::string		&Prefix,
		 CExpression    	&Field,
		 std::string			*Name,
		 NetCDFVarKind		*Kind,
		 CUnit			*Unit,
		 std::string			*Title,
		 std::string			*Comment,
		 std::string			*DataFormat,
		 uint32_t               *Group,
		 const std::string		&TraceDescription,
		 int32_t		Index		= 0,
		 int32_t		MinOccurences	= 1,
		 int32_t		MaxOccurences	= 1,
		 bool			PrintTrace	= true);

int32_t GetVarDef
		(CFileParams		&Params,
		 const std::string		&Prefix,
		 CExpression    	&Field,
		 std::string			*Name,
		 NetCDFVarKind		*Kind,
		 CUnit			*Unit,
		 std::string			*Title,
		 std::string			*Comment,
		 std::string			*DataFormat,
		 uint32_t               *Group,
		 double			&Min,
		 double			&Max,
		 uint32_t		&Count,
		 double			&Step,
		 const std::string		&TraceDescription,
		 int32_t		Index		= 0,
		 int32_t		MinOccurences	= 1,
		 int32_t		MaxOccurences	= 1,
		 bool			PrintTrace	= true);

void GetFilterDefinitions
		(CFileParams		&Params,
		 const std::string		&Prefix,
		 bool			*Smooth,
		 bool			*Extrapolate,
		 int32_t		Index		= 0,
		 int32_t		Occurences	= 1,
		 bool			PrintTrace	= true);

void GetLoessCutoff
		(CFileParams		&Params,
		 int32_t		*XCutoff,
		 int32_t		*YCutoff,
		 int32_t		Index		= 0,
		 int32_t		Occurences	= 1,
		 bool			PrintTrace	= true);

CBratProcess::EMergeDataMode GetDataMode_NOT_USED
		(CFileParams	&Params,
		 int32_t	MinOccurences	= 0,
		 int32_t	MaxOccurences	= 1,
		 const std::string	&Keyword	= "DATA_MODE",
		 int32_t	Index		= 0,
		 CBratProcess::EMergeDataMode	Default		= CBratProcess::pctMEAN);

CBratProcess::EMergeDataMode GetDataMode_NOT_USED
		(CFileParams	&Params,
		 const std::string	&Prefix,
		 int32_t	MinOccurences	= 0,
		 int32_t	MaxOccurences	= 1,
		 int32_t	Index		= 0,
		 CBratProcess::EMergeDataMode	Default		= CBratProcess::pctMEAN);

std::string DataModeStr_NOT_USED
		(CBratProcess::EMergeDataMode	Mode);


CBratProcess::OutsideMode GetOutsideMode
		(CFileParams	&Params,
		 int32_t	MinOccurences	= 0,
		 int32_t	MaxOccurences	= 1,
		 const std::string	&Keyword	= "OUTSIDE_MODE",
		 int32_t	Index		= 0,
		 CBratProcess::OutsideMode	Default		= CBratProcess::pctSTRICT);

std::string OutsideModeStr
		(CBratProcess::OutsideMode	Mode);

// true if outside
bool CheckOutsideValue
		(double		&Value,
		 double		Min,
		 double		Max,
		 double		Step,
		 CBratProcess::OutsideMode	Mode);

CBratProcess::PositionMode GetPositionMode
		(CFileParams	&Params,
		 int32_t	MinOccurences	= 0,
		 int32_t	MaxOccurences	= 1,
		 const std::string	&Keyword	= "POSITION_MODE",
		 int32_t	Index		= 0,
		 CBratProcess::PositionMode	Default		= CBratProcess::pctNEAREST);

std::string PositionModeStr
		(CBratProcess::PositionMode	Mode);

bool CheckPositionValue
		(double		Value,
		 double		Min,
		 double		Step,
		 uint32_t	Count,
		 CBratProcess::PositionMode	Mode,
		 uint32_t	&Position);

double CheckLongitudeValue
		(double			Value,
		 double			Min,
		 NetCDFVarKind		Kind);

bool IsLongitudeCircular
		(double			Min,
		 double			Max,
		 NetCDFVarKind		Kind);


int32_t GetMergedDataSlices_NOT_USED
		(CBratProcess::EMergeDataMode		Mode);
/*
void InitMergedData
		(double			*Data,
		 uint32_t		DataCount,
		 uint32_t		DataSlices,
		 MergeDataMode		Mode);
*/
void InitMergedData_NOT_USED
		(double			*Data,
		 uint32_t		DataCount);

void MergeDataValue_NOT_USED
		(double			*Data,
		 double			Value,
		 uint32_t		CountValueOffset,
		 uint32_t		MeanValueOffset,
		 CBratProcess::EMergeDataMode		Mode);

void FinalizeMergingOfDataValues_NOT_USED
		(double			*Data,
		 uint32_t		CountValueOffset,
		 uint32_t		MeanValueOffset,
		 CBratProcess::EMergeDataMode		Mode);

// If Result is NULL, Data is modified upon exit.
// If Result is not NULL it must point to an area
// big enough to contain at least NbX*NbY data.
void LoessFilterGrid
		(double		*Data,
		 double		*Result,
		 int32_t	NbX,
		 bool		CircularX,
		 int32_t	WaveLengthX,
		 int32_t	NbY,
		 bool		CircularY,
		 int32_t	WaveLengthY,
		 bool		SmoothData,
		 bool		ExtrapolData	
		 );


}

#endif // Already included .h
