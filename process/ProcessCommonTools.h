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
#ifndef _ProcessCommonTools_h_
#define _ProcessCommonTools_h_

#include <netcdf.h>
#include "Stl.h"
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
** Print help about a list of keywords which can be foun in a parameter file
** definitions are in ParametersDictionary.h
*/
void PrintParameterHelp
		(ostream		&where,
		 const KeywordHelp	*KeywordList);

/*
** Checks the command line parameters (with -h -k or parameter file)
** and return true if command line is invalid
** Last entry of KeywordList MUST BE NULL or result is unpredictible
** (and maybe a core dump)
*/
bool CheckCommandLineOptions
		(int			argc,
		 char			**argv,
		 const string		&HelpString,
		 const KeywordHelp	*KeywordList,
		 string			&CommandFileName);


int32_t GetFileList
		(CFileParams		&Params,
		 const string		&Keyword,
		 CStringArray		&Names,
		 const string		&TraceDescription,
		 int32_t		MinOccurences	= 1,
		 int32_t		MaxOccurences	= -1,
		 bool			PrintTrace	= true);

int32_t GetVarDef
		(CFileParams		&Params,
		 const string		&Prefix,
		 CExpression    	&Field,
		 string			*Name,
		 NetCDFVarKind		*Kind,
		 CUnit			*Unit,
		 string			*Title,
		 string			*Comment,
		 string			*DataFormat,
		 uint32_t               *Group,
		 const string		&TraceDescription,
		 int32_t		Index		= 0,
		 int32_t		MinOccurences	= 1,
		 int32_t		MaxOccurences	= 1,
		 bool			PrintTrace	= true);

int32_t GetVarDef
		(CFileParams		&Params,
		 const string		&Prefix,
		 CExpression    	&Field,
		 string			*Name,
		 NetCDFVarKind		*Kind,
		 CUnit			*Unit,
		 string			*Title,
		 string			*Comment,
		 string			*DataFormat,
		 uint32_t               *Group,
		 double			&Min,
		 double			&Max,
		 uint32_t		&Count,
		 double			&Step,
		 const string		&TraceDescription,
		 int32_t		Index		= 0,
		 int32_t		MinOccurences	= 1,
		 int32_t		MaxOccurences	= 1,
		 bool			PrintTrace	= true);

void GetFilterDefinitions
		(CFileParams		&Params,
		 const string		&Prefix,
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

CBratProcess::MergeDataMode GetDataMode
		(CFileParams	&Params,
		 int32_t	MinOccurences	= 0,
		 int32_t	MaxOccurences	= 1,
		 const string	&Keyword	= "DATA_MODE",
		 int32_t	Index		= 0,
		 CBratProcess::MergeDataMode	Default		= CBratProcess::pctMEAN);

CBratProcess::MergeDataMode GetDataMode
		(CFileParams	&Params,
		 const string	&Prefix,
		 int32_t	MinOccurences	= 0,
		 int32_t	MaxOccurences	= 1,
		 int32_t	Index		= 0,
		 CBratProcess::MergeDataMode	Default		= CBratProcess::pctMEAN);

string DataModeStr
		(CBratProcess::MergeDataMode	Mode);


CBratProcess::OutsideMode GetOutsideMode
		(CFileParams	&Params,
		 int32_t	MinOccurences	= 0,
		 int32_t	MaxOccurences	= 1,
		 const string	&Keyword	= "OUTSIDE_MODE",
		 int32_t	Index		= 0,
		 CBratProcess::OutsideMode	Default		= CBratProcess::pctSTRICT);

string OutsideModeStr
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
		 const string	&Keyword	= "POSITION_MODE",
		 int32_t	Index		= 0,
		 CBratProcess::PositionMode	Default		= CBratProcess::pctNEAREST);

string PositionModeStr
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


int32_t GetMergedDataSlices
		(CBratProcess::MergeDataMode		Mode);
/*
void InitMergedData
		(double			*Data,
		 uint32_t		DataCount,
		 uint32_t		DataSlices,
		 MergeDataMode		Mode);
*/
void InitMergedData
		(double			*Data,
		 uint32_t		DataCount);

void MergeDataValue
		(double			*Data,
		 double			Value,
		 uint32_t		CountValueOffset,
		 uint32_t		MeanValueOffset,
		 CBratProcess::MergeDataMode		Mode);

void FinalizeMergingOfDataValues
		(double			*Data,
		 uint32_t		CountValueOffset,
		 uint32_t		MeanValueOffset,
		 CBratProcess::MergeDataMode		Mode);

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
