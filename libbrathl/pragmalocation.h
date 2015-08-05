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
// 
// Under Windows try to add a line number of a compile time message to your source code like this :
//  #pragma message(__FILE__ __LINE__ ": important part to be changed!!"
// 
// Won't work. Anyway, with some tricks you can do exactly this. The nice thing is that you will never ever again need to use "Find" to locate a particular message in your source code. Just double click the line with your message in the output window and VC's editor will jump to that location.
// 
// Here is what you need to add to your source code to make this work:
// 
// #define __STR2__(x) #x
// #define __STR1__(x) __STR2__(x)
// #define __LOC__ __FILE__ "("__STR1__(__LINE__)") : Warning Msg: "
// #define __LOC2__ __FILE__ "("__STR1__(__LINE__)") : "
// 
// Now you can use the #pragma message to add the location of the message:
// 
// #pragma message(__LOC__ "important part to be changed")
// #pragma message(__LOC2__ "error C9901: wish that error would exist")
// 
// The preceding pragmas will yield the following output from the compiler (preprocessor)
// 
// C:\code\pragma\test.cpp(33) : Warning Msg: important part to be changed
// c:\code\pragma\test.cpp(34) : error C9901: wish that error would exist
// 
// see http://www.codeproject.com/macro/location_pragma.asp
// 



#ifndef _pragmalocation_h_
#define _pragmalocation_h_

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)") : Warning Msg: "
#define __LOC2__ __FILE__ "("__STR1__(__LINE__)") : "

#endif
