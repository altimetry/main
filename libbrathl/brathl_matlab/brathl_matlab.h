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

#ifndef _brathl_matlab_h_
#define _brathl_matlab_h_

#include "brathl_error.h" 
#include "brathlc.h" 
#include "brathl.h" 

/* workaround to prevent double definition of char16_t */
#ifdef _CHAR16T
#define CHAR16_T
#endif
#include "mex.h"

#if defined( __cplusplus )
extern "C" {
#endif  /* __cplusplus */

#ifndef HAVE_MXCREATEDOUBLESCALAR
mxArray *mxCreateDoubleScalar(double value);
#endif

#ifndef HAVE_MXCREATENUMERICMATRIX
mxArray *mxCreateNumericMatrix(int m, int n, mxClassID classid, int cmplx_flag);
#endif

/* brat-matlab functions */
void brathlm_Error();

#if defined( __cplusplus )
}
#endif  /* __cplusplus */

#endif  // ifndef _brathl_matlab_h_
