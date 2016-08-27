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
#include "stdafx.h"

#if defined (_MSC_VER)

#include <iostream>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#include <signal.h>

#if defined (WIN32)
#undef min
#undef max
#endif

#include "../+Utils.h"

#include "Exception.h"
#include "ExceptionWin.h"


#pragma warning ( disable : 4786 )	//added; 'identifier' : identifier was truncated to 'number' characters in the debug information



enum EExceptIndex
{
	e_EXCEPTION_ACCESS_VIOLATION,				
	e_EXCEPTION_DATATYPE_MISALIGNMENT,			
	e_EXCEPTION_BREAKPOINT,						
	e_EXCEPTION_SINGLE_STEP,					
	e_EXCEPTION_ARRAY_BOUNDS_EXCEEDED,			
	e_EXCEPTION_FLT_DENORMAL_OPERAND,			
	e_EXCEPTION_FLT_DIVIDE_BY_ZERO,				
	e_EXCEPTION_FLT_INEXACT_RESULT,				
	e_EXCEPTION_FLT_INVALID_OPERATION,			
	e_EXCEPTION_FLT_OVERFLOW,					
	e_EXCEPTION_FLT_STACK_CHECK,				
	e_EXCEPTION_FLT_UNDERFLOW,					
	e_EXCEPTION_INT_DIVIDE_BY_ZERO,				
	e_EXCEPTION_INT_OVERFLOW,					
	e_EXCEPTION_PRIV_INSTRUCTION,				
	e_EXCEPTION_IN_PAGE_ERROR,					
	e_EXCEPTION_ILLEGAL_INSTRUCTION,			
	e_EXCEPTION_NONCONTINUABLE_EXCEPTION,		
	e_EXCEPTION_STACK_OVERFLOW,					
	e_EXCEPTION_INVALID_DISPOSITION,			
	e_EXCEPTION_GUARD_PAGE,						
	e_EXCEPTION_INVALID_HANDLE,					
	e_CONTROL_C_EXIT,							
	e_0xE06D7363,								
	e_default,									
	EExceptIndex_size
};

#define MAKE_MESSAGE( x ) \
	#x,

static const char *msgs[ EExceptIndex_size ] =
{
	MAKE_MESSAGE( e_EXCEPTION_ACCESS_VIOLATION				)
	MAKE_MESSAGE( e_EXCEPTION_DATATYPE_MISALIGNMENT			)
	MAKE_MESSAGE( e_EXCEPTION_BREAKPOINT					)
	MAKE_MESSAGE( e_EXCEPTION_SINGLE_STEP					)
	MAKE_MESSAGE( e_EXCEPTION_ARRAY_BOUNDS_EXCEEDED			)
	MAKE_MESSAGE( e_EXCEPTION_FLT_DENORMAL_OPERAND			)
	MAKE_MESSAGE( e_EXCEPTION_FLT_DIVIDE_BY_ZERO			)
	MAKE_MESSAGE( e_EXCEPTION_FLT_INEXACT_RESULT			)
	MAKE_MESSAGE( e_EXCEPTION_FLT_INVALID_OPERATION			)
	MAKE_MESSAGE( e_EXCEPTION_FLT_OVERFLOW					)
	MAKE_MESSAGE( e_EXCEPTION_FLT_STACK_CHECK				)
	MAKE_MESSAGE( e_EXCEPTION_FLT_UNDERFLOW					)
	MAKE_MESSAGE( e_EXCEPTION_INT_DIVIDE_BY_ZERO			)
	MAKE_MESSAGE( e_EXCEPTION_INT_OVERFLOW					)
	MAKE_MESSAGE( e_EXCEPTION_PRIV_INSTRUCTION				)
	MAKE_MESSAGE( e_EXCEPTION_IN_PAGE_ERROR					)
	MAKE_MESSAGE( e_EXCEPTION_ILLEGAL_INSTRUCTION			)
	MAKE_MESSAGE( e_EXCEPTION_NONCONTINUABLE_EXCEPTION		)
	MAKE_MESSAGE( e_EXCEPTION_STACK_OVERFLOW				)
	MAKE_MESSAGE( e_EXCEPTION_INVALID_DISPOSITION			)
	MAKE_MESSAGE( e_EXCEPTION_GUARD_PAGE					)
	MAKE_MESSAGE( e_EXCEPTION_INVALID_HANDLE				)
	MAKE_MESSAGE( e_CONTROL_C_EXIT							)
	"Microsoft C++ Exception",
	"UNKNOWN_EXCEPTION"
};


void
trans_func( unsigned int u, _EXCEPTION_POINTERS* pExp )
{
    UNUSED( pExp );

	EExceptIndex e;
	switch(u){
	case EXCEPTION_ACCESS_VIOLATION :			e = e_EXCEPTION_ACCESS_VIOLATION;
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:		e = e_EXCEPTION_DATATYPE_MISALIGNMENT;
		break;
	case EXCEPTION_BREAKPOINT:					e = e_EXCEPTION_BREAKPOINT;
		break;
	case EXCEPTION_SINGLE_STEP:					e = e_EXCEPTION_SINGLE_STEP;
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:		e = e_EXCEPTION_ARRAY_BOUNDS_EXCEEDED;
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:		e = e_EXCEPTION_FLT_DENORMAL_OPERAND;
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:			e = e_EXCEPTION_FLT_DIVIDE_BY_ZERO;
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:			e = e_EXCEPTION_FLT_INEXACT_RESULT;
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:		e = e_EXCEPTION_FLT_INVALID_OPERATION;
		break;
	case EXCEPTION_FLT_OVERFLOW:				e = e_EXCEPTION_FLT_OVERFLOW;
		break;
	case EXCEPTION_FLT_STACK_CHECK:				e = e_EXCEPTION_FLT_STACK_CHECK;
		break;
	case EXCEPTION_FLT_UNDERFLOW:				e = e_EXCEPTION_FLT_UNDERFLOW;
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:			e = e_EXCEPTION_INT_DIVIDE_BY_ZERO; 
		break;
	case EXCEPTION_INT_OVERFLOW:				e = e_EXCEPTION_INT_OVERFLOW;
		break;
	case EXCEPTION_PRIV_INSTRUCTION:			e = e_EXCEPTION_PRIV_INSTRUCTION;
		break;
	case EXCEPTION_IN_PAGE_ERROR:				e = e_EXCEPTION_IN_PAGE_ERROR;
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:			e = e_EXCEPTION_ILLEGAL_INSTRUCTION;
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:	e = e_EXCEPTION_NONCONTINUABLE_EXCEPTION;
		break;
	case EXCEPTION_STACK_OVERFLOW:				e = e_EXCEPTION_STACK_OVERFLOW;
		break;
	case EXCEPTION_INVALID_DISPOSITION:			e = e_EXCEPTION_INVALID_DISPOSITION;
		break;
	case EXCEPTION_GUARD_PAGE:					e = e_EXCEPTION_GUARD_PAGE;
		break;
	case EXCEPTION_INVALID_HANDLE:				e = e_EXCEPTION_INVALID_HANDLE;
		break;
	case CONTROL_C_EXIT:						e = e_CONTROL_C_EXIT;
		break;
	case 0xE06D7363:							e = e_0xE06D7363;
		break;
	default:									e = e_default;
		break;
	}

	CException ex( "Windows exception with code " + hn2s<std::string>( u ) + ". " + msgs[ e ] );
	throw ex;
}





void SignalHandler( int signal )
{
    if (signal == SIGABRT) 
	{
		CException ex( "Unrecoverable error: abort called." );
		throw ex;
    }
	else 
	{
		assert__( false );
    }
}


SignalHandlerPointer SetAbortSignal()
{ 
	_set_abort_behavior( 0, _WRITE_ABORT_MSG );
	_set_abort_behavior( 0, _CALL_REPORTFAULT );

	return signal( SIGABRT, SignalHandler );
}


_se_translator_function SetSETranslator( _se_translator_function NewPtFunc )
{
    UNUSED( NewPtFunc );

	return _set_se_translator( trans_func );
}


_se_translator_function original_translator = SetSETranslator( trans_func );




bool CheckSETranslator()
{
	return _set_se_translator( trans_func ) == trans_func;
}

_se_translator_function RestoreSystemSETranslator()
{
	_set_se_translator( original_translator );
	return original_translator;
}


#endif	//defined (_MSC_VER)
