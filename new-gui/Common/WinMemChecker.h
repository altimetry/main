#ifndef WIN_MEM_CHECKER
#define WIN_MEM_CHECKER

#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAPALLOC
#include <stdlib.h>
#include <crtdbg.h>


#if !defined(_T)
#if defined(UNICODE) || defined(_UNICODE)
#define _T(x) L ## x
#else
#define _T(x) x
#endif
#endif


class MemChecker
{
public:
	static MemChecker instance;

protected:
	//_CrtMemState FirstState;			//before all run-time allocations
	//_CrtMemState BeforeRunState;		//after app creation, before running engine
	//_CrtMemState AfterRunState;		//after running engine, before de-allocating

	//#define _CRTDBG_ALLOC_MEM_DF        0x01  /* Turn on debug allocation */
	//#define _CRTDBG_DELAY_FREE_MEM_DF   0x02  /* Don't actually free memory */
	//#define _CRTDBG_CHECK_ALWAYS_DF     0x04  /* Check heap every alloc/dealloc */
	//#define _CRTDBG_RESERVED_DF         0x08  /* Reserved - do not use */
	//#define _CRTDBG_CHECK_CRT_DF        0x10  /* Leak check/diff CRT blocks */
	//#define _CRTDBG_LEAK_CHECK_DF       0x20  /* Leak check at program exit */

public:
	MemChecker( int other_flags = _CRTDBG_LEAK_CHECK_DF )
	{
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | other_flags | _CRTDBG_CHECK_CRT_DF );

		_CrtMemState ControlState;					//just to see _CrtMemCheckpoint and CrtMemDifference behaviour when no memory is allocated: expected results down here (*) are all zeros
		_CrtMemCheckpoint( &ControlState );
		dumpStatistics( ControlState );
	}

	static int dumpStatistics( const _CrtMemState &someState, bool sep = true )
	{
		if ( sep )
			OutputDebugString( _T("\n\n") );

		_CrtMemState CurrentState, ComparisonState;	//support states

		_CrtMemCheckpoint( &CurrentState );
		//
		//int _CrtMemDifference(
		//	_CrtMemState *stateDiff,
		//	const _CrtMemState *oldState,
		//	const _CrtMemState *newState
		//	);
		//
		int result = _CrtMemDifference( &ComparisonState, &someState, &CurrentState );
		_CrtMemDumpStatistics( &ComparisonState );	//(*) expected results: all zeros

		if ( sep )
			OutputDebugString( _T("\n\n") );

		return result;
	}
};


//for local simple use: just declare a variable in a block

struct CheckLeaks
{
	_CrtMemState FirstState;

	CheckLeaks()
	{
		_CrtMemCheckpoint( &FirstState );	//set first flag before all allocations
	}
	~CheckLeaks()
	{
		if ( MemChecker::dumpStatistics( FirstState ) )		//all memory wasted
			OutputDebugString( _T("Possible Memory Leaks\n") );
	}
};


#endif		// WIN_MEM_CHECKER
