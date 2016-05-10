#if !defined(BRAT_EXCEPTION_WINDOWS_H)
#define BRAT_EXCEPTION_WINDOWS_H

#ifndef __cplusplus
#error Must use C++ for ExceptionWin.h
#endif

#if defined (_MSC_VER)


#include <eh.h>			//to capture structured exception handling in VC



typedef void (*SignalHandlerPointer)(int);

SignalHandlerPointer SetAbortSignal();



bool CheckSETranslator();

_se_translator_function RestoreSystemSETranslator();



#endif  // _MSC_VER
#endif  // BRAT_EXCEPTION_WINDOWS_H
