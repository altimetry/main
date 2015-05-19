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

/*
 * Special STL include file because on WINDOWS, using warning level 4 and including STL stuff flooded with warnings. 
 * On WINDOWS, STL itself switches on warnings. This happens in the file yvals.h. 
 * Therefore a peculiar including technique is necessary. 
 * 
 * 
 * The purpose is to keep all warnings in the own source and disable the STL warning noise. 
 * This is done by switching off the warnings before you include the STL stuff 
 * and restore the original warning state afterwards. 
 *
 * Before switching off the warnings, the file yvals.h must be included to activate the files 
 * include guard and avoid a further change of the warning state. 
 *
 * MSKB article 'BUG: C4786 Warning Is Not Disabled with #pragma Warning' (Article ID: Q167355) describes 
 * a warning that can't be disabled. This occured to me using a map.
 * 
*/

// macro system to specify with STL elements to use or a STL_USING_ALL to include 
// all objects covered.
// ------------------------------------------
// The STL include file is not complete 
// (just the objects used frequently) and must be extended if necessary. 
// ------------------------------------------
// You don't need to declare using namespace std;. 
// This is done at the end of the include file. 
//
//        // defines for using the STL
//        #define STL_USING_ALL
//        #define STL_USING_MAP
//        #define STL_USING_VECTOR
//        #define STL_USING_LIST
//        #define STL_USING_STRING
//        #define STL_USING_STREAM
//        #define STL_USING_ASSERT
//        #define STL_USING_MEMORY
//        #define STL_USING_STACK
//        #define STL_USING_STDEXCEPT
//        #define STL_USING_LIMITS
//        #define STL_USING_BITSET


//
// Sample:
//        #define STL_USING_ALL
//        #include "STL.h"
//

#ifndef _Stl_h_
#define _Stl_h_


#ifdef STL_USING_ALL
  #define STL_USING_MAP
  #define STL_USING_VECTOR
  #define STL_USING_LIST
  #define STL_USING_STRING
  #define STL_USING_STREAM
  #define STL_USING_ASSERT
  #define STL_USING_MEMORY
  #define STL_USING_STACK
  #define STL_USING_STDEXCEPT
  #define STL_USING_LIMITS
  #define STL_USING_BITSET
#endif

// STL include for non-windows system
#ifndef WIN32

  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for string
  //////////////////////////////////////////////////////////////////////
  #ifdef STL_USING_STRING
    #include <string>
  #endif

  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for memory
  //////////////////////////////////////////////////////////////////////
  #ifdef STL_USING_MEMORY
    #include <memory>
  #endif

  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for stack
  //////////////////////////////////////////////////////////////////////
  #ifdef STL_USING_STACK
    #include <stack>
  #endif

  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for streams
  //////////////////////////////////////////////////////////////////////
  #ifdef STL_USING_STREAM
    #include <iostream>
    #include <sstream>
    #include <fstream>
  #endif

  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for list
  //////////////////////////////////////////////////////////////////////
  #ifdef STL_USING_LIST
    #include <list>
  #endif

  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for map
  //////////////////////////////////////////////////////////////////////
  #ifdef STL_USING_MAP
    #include <map>
  #endif

  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for vector
  //////////////////////////////////////////////////////////////////////
  #ifdef STL_USING_VECTOR
    #include <vector>
  #endif

  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for assert
  //////////////////////////////////////////////////////////////////////
  #ifdef STL_USING_ASSERT
    #include <cassert>
  #endif

  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for stdexcept
  //////////////////////////////////////////////////////////////////////
  #ifdef STL_USING_STDEXCEPT
    #include <stdexcept>
  #endif

  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for limits
  //////////////////////////////////////////////////////////////////////
  #ifdef STL_USING_LIMITS
    #include <limits>
  #endif

  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for bitset
  //////////////////////////////////////////////////////////////////////
  #ifdef STL_USING_BITSET
    #include <bitset>
  #endif

  //////////////////////////////////////////////////////////////////////
  // verify proper use of macros
  //////////////////////////////////////////////////////////////////////
  #if defined STL_USING_MAP || defined STL_USING_VECTOR || defined STL_USING_LIST || defined STL_USING_STRING || defined STL_USING_STREAM || defined STL_USING_ASSERT || defined STL_USING_MEMORY || defined STL_USING_STACK || defined STL_USING_STDEXCEPT || defined STL_USING_LIMITS  || defined STL_USING_BITSET
  using namespace std;
  #else
  #pragma message( "Warning: You included <STL.H> without using any STL features!" )
  #endif


// STL include for windows system

#else // WIN32 defined

  #if _MSC_VER > 1000
  #pragma once
  #endif


  //////////////////////////////////////////////////////////////////////
  // handy define to include all stuff
  //////////////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for map
  //////////////////////////////////////////////////////////////////////

  #ifdef STL_USING_MAP

  #pragma warning(push)

  #include <yvals.h>              // warning numbers get enabled in yvals.h 

  #pragma warning(disable: 4018)  // signed/unsigned mismatch
  #pragma warning(disable: 4100)  // unreferenced formal parameter
  #pragma warning(disable: 4245)  // conversion from 'type1' to 'type2', signed/unsigned mismatch
  #pragma warning(disable: 4512)  // 'class' : assignment operator could not be generated
  #pragma warning(disable: 4663)  // C++ language change: to explicitly specialize class template 'vector'
  #pragma warning(disable: 4710)  // 'function' : function not inlined
  #pragma warning(disable: 4786)  // identifier was truncated to 'number' characters in the debug information

  // BUG: C4786 Warning Is Not Disabled with #pragma Warning
  // STATUS: Microsoft has confirmed this to be a bug in the Microsoft product. This warning can be ignored.
  // This occured only in the <map> container.

  #include <map>

  #pragma warning(pop)

  #endif


  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for vector
  //////////////////////////////////////////////////////////////////////

  #ifdef STL_USING_VECTOR

  #pragma warning(push)

  #include <yvals.h>              // warning numbers get enabled in yvals.h 

  #pragma warning(disable: 4018)  // signed/unsigned mismatch
  #pragma warning(disable: 4100)  // unreferenced formal parameter
  #pragma warning(disable: 4245)  // conversion from 'type1' to 'type2', signed/unsigned mismatch
  #pragma warning(disable: 4663)  // C++ language change: to explicitly specialize class template 'vector'
  #pragma warning(disable: 4702)  // unreachable code
  #pragma warning(disable: 4710)  // 'function' : function not inlined
  #pragma warning(disable: 4786)  // identifier was truncated to 'number' characters in the debug information

  #include <vector>

  #pragma warning(pop)

  #endif


  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for list
  //////////////////////////////////////////////////////////////////////

  #ifdef STL_USING_LIST

  #pragma warning(push)

  #include <yvals.h>              // warning numbers get enabled in yvals.h 

  #pragma warning(disable: 4100)  // unreferenced formal parameter
  #pragma warning(disable: 4284)  // return type for 'identifier::operator ->' is not a UDT or reference 
                                  // to a UDT. Will produce errors if applied using infix notation
  #pragma warning(disable: 4710)  // 'function' : function not inlined
  #pragma warning(disable: 4786)  // identifier was truncated to 'number' characters in the debug information

  #include <list>

  #pragma warning(pop)

  #endif


  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for string
  //////////////////////////////////////////////////////////////////////

  #ifdef STL_USING_STRING

  #pragma warning(push)

  #include <yvals.h>              // warning numbers get enabled in yvals.h 

  #pragma warning(disable: 4018)  // signed/unsigned mismatch
  #pragma warning(disable: 4100)  // unreferenced formal parameter
  #pragma warning(disable: 4146)  // unary minus operator applied to unsigned type, result still unsigned
  #pragma warning(disable: 4244)  // 'conversion' conversion from 'type1' to 'type2', possible loss of data
  #pragma warning(disable: 4245)  // conversion from 'type1' to 'type2', signed/unsigned mismatch
  #pragma warning(disable: 4511)  // 'class' : copy constructor could not be generated
  #pragma warning(disable: 4512)  // 'class' : assignment operator could not be generated
  #pragma warning(disable: 4663)  // C++ language change: to explicitly specialize class template 'vector'
  #pragma warning(disable: 4710)  // 'function' : function not inlined
  #pragma warning(disable: 4786)  // identifier was truncated to 'number' characters in the debug information

  #include <string>

  #pragma warning(pop)

  #pragma warning(disable: 4514)  // unreferenced inline/local function has been removed
  #pragma warning(disable: 4710)  // 'function' : function not inlined
  #pragma warning(disable: 4786)  // identifier was truncated to 'number' characters in the debug information

  #endif


  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for streams
  //////////////////////////////////////////////////////////////////////

  #ifdef STL_USING_STREAM

  #pragma warning(push)

  #include <yvals.h>              // warning numbers get enabled in yvals.h 

  #pragma warning(disable: 4097)  // typedef-name 'identifier1' used as synonym for class-name 'identifier2'
  #pragma warning(disable: 4127)  // conditional expression is constant

  #include <iostream>
  #include <sstream>
  #include <fstream>

  #pragma warning(pop)

  #endif


  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for memory
  //////////////////////////////////////////////////////////////////////

  #ifdef STL_USING_MEMORY

  // The STL library provides a type called auto_ptr for managing pointers.  
  // This template class acts as a stack variable for dynamically allocated 
  // memory.  When the variable goes out of scope, its destructor gets called.  
  // In its de-structor, it calls delete on the contained pointer, making sure 
  // that the memory is returned to the heap.

  #pragma warning(push)

  #include <yvals.h>              // warning numbers get enabled in yvals.h 

  #pragma warning(disable: 4018)  // signed/unsigned mismatch
  #pragma warning(disable: 4100)  // unreferenced formal parameter
  #pragma warning(disable: 4245)  // conversion from 'type1' to 'type2', signed/unsigned mismatch
  #pragma warning(disable: 4710)  // 'function' : function not inlined
  #pragma warning(disable: 4786)  // identifier was truncated to 'number' characters in the debug information

  #include <memory>

  #pragma warning(pop)

  #endif


  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for stack
  //////////////////////////////////////////////////////////////////////

  #ifdef STL_USING_STACK

  #pragma warning(push)

  #include <yvals.h>              // warning numbers get enabled in yvals.h 

  #pragma warning(disable: 4018)  // signed/unsigned mismatch
  #pragma warning(disable: 4100)  // unreferenced formal parameter
  #pragma warning(disable: 4146)  // unary minus operator applied to unsigned type, result still unsigned
  #pragma warning(disable: 4245)  // conversion from 'type1' to 'type2', signed/unsigned mismatch
  #pragma warning(disable: 4284)  // return type for 'identifier::operator ->' is not a UDT or reference 
  #pragma warning(disable: 4710)  // 'function' : function not inlined
  #pragma warning(disable: 4786)  // identifier was truncated to 'number' characters in the debug information

  #include <stack>

  #pragma warning(pop)

  #endif


  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for assert
  //////////////////////////////////////////////////////////////////////

  #ifdef STL_USING_ASSERT

  // avoid macro redefinition when using MFC
  #ifndef ASSERT

    #include <cassert>
    /*
    // macros for tracking down errors
    #ifdef _DEBUG

    #define ASSERT( exp )           assert( exp )
    #define VERIFY( exp )           assert( exp )
    #define TRACE                   ::OutputDebugString

    #else

    #define ASSERT( exp )           ((void)0)
    #define VERIFY( exp )           ((void)(exp))
    #define TRACE                   1 ? (void)0 : ::OutputDebugString

    #endif  // _DEBUG
    */
  #endif  // ASSERT
  /*
  // additional macros 
  #define ASSERT_BREAK( exp )             { ASSERT(exp); if( !(exp) ) break; }
  #define ASSERT_CONTINUE( exp )          { ASSERT(exp); if( !(exp) ) continue; }
  #define ASSERT_RETURN( exp )            { ASSERT(exp); if( !(exp) ) return; }
  #define ASSERT_RETURN_NULL( exp )       { ASSERT(exp); if( !(exp) ) return 0; }
  #define ASSERT_RETURN_FALSE( exp )      { ASSERT(exp); if( !(exp) ) return false; }
  */
  #endif  // STL_USING_ASSERT

  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for stdexcept
  //////////////////////////////////////////////////////////////////////
  #ifdef STL_USING_STDEXCEPT
    #include <stdexcept>
  #endif

  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for limits
  //////////////////////////////////////////////////////////////////////
  #ifdef STL_USING_LIMITS
    #include <limits>
  #endif

  //////////////////////////////////////////////////////////////////////
  // STL neccessary declaration for bitset
  //////////////////////////////////////////////////////////////////////
  #ifdef STL_USING_BITSET
    #include <bitset>
  #endif

  //////////////////////////////////////////////////////////////////////
  // verify proper use of macros
  //////////////////////////////////////////////////////////////////////

  #if defined STL_USING_MAP || defined STL_USING_VECTOR || defined STL_USING_LIST || defined STL_USING_STRING || defined STL_USING_STREAM || defined STL_USING_ASSERT || defined STL_USING_MEMORY || defined STL_USING_STACK || defined STL_USING_STDEXCEPT || defined STL_USING_LIMITS  || defined STL_USING_BITSET
  using namespace std;
  #else
  #pragma message( "Warning: You included <STL.H> without using any STL features!" )
  #endif

#endif  // ifndef WIN32

#endif  // _Stl_h_

