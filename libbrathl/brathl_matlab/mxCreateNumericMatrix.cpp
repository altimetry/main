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


#include "mex.h"

#if defined( __cplusplus )
extern "C" {
#endif  /* __cplusplus */

mxArray *mxCreateNumericMatrix(int m, int n, mxClassID classid, int cmplx_flag)
{
    int dims[2];

    dims[0] = m;
    dims[1] = n;

    return mxCreateNumericArray(2, dims, classid, static_cast<mxComplexity>(cmplx_flag));
}

#if defined( __cplusplus )
}
#endif  /* __cplusplus */
