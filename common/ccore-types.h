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
#if !defined(C_CORE_TYPES_H)
#define C_CORE_TYPES_H


#if defined( __cplusplus )
extern "C" {
#endif  /* __cplusplus */


#if defined(_WIN64) || defined(__LP64__) || defined(__x86_64__) 
#define ARCHITECTURE_64
#else
#define ARCHITECTURE_32
#endif


#include <stdint.h>

//
//	I. Integer Types
//	============================================================================================
//	Type specifier			Equivalent type			Width in bits by data model				
//													C++ standard	LP32	ILP32	LLP64	LP64
//	============================================================================================
//	short					short int				at least 16		16		16		16		16
//	short int						
//	signed short						
//	signed short int						
//	------------------------------------------------
//	unsigned short			unsigned short int
//	unsigned short int						
//	--------------------------------------------------------------------------------------------
//	int						int						at least 16		16		32		32		32
//	signed						
//	signed int						
//	unsigned				unsigned int					
//	unsigned int						
//	--------------------------------------------------------------------------------------------
//	long					long int				at least 32		32		32		32		64
//	long int						
//	signed long						
//	signed long int						
//	------------------------------------------------
//	unsigned long			unsigned long int					
//	unsigned long int						
//	--------------------------------------------------------------------------------------------
//	long long				long long int			at least 64		64		64		64		64
//	long long int			 (C++11)			
//	signed long long						
//	signed long long int						
//	------------------------------------------------
//	unsigned long long		unsigned long long int 
//	unsigned long long int	(C++11)					
//	============================================================================================
//
//
//	II. Data Models per Platform/Architecture
//	============================================================================================
//	Platforms		Architectures
//	============================================================================================
//					16 bit	32 bit	64 bit
//	Windows 		LP32	ILP32	LLP64
//	Linux			----	ILP32	LP64
//	Mac OS X		----	ILP32	LP64
//	============================================================================================
//
//
//	I. Floating Point
//	============================================================================================
//	Type specifier			Width in bits
//							Win-32		Unix-32		Win-64		Unix-64
//	============================================================================================
//	float					32			32			32			32
//	double					64			64			64			64
//	long double				64			96			64			128




////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//					Generic Built-in Integer Types
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


#if defined(ARCHITECTURE_64)

	typedef int64_t			int_t;
	typedef uint64_t		uint_t;

#elif defined(ARCHITECTURE_32)

	typedef int32_t			int_t;
	typedef uint32_t		uint_t;
#else
#error one of { ARCHITECTURE_32, ARCHITECTURE_64 } must be defined
#endif



#if defined( __cplusplus )
}
#endif  /* __cplusplus */

#endif // !defined(C_CORE_TYPES_H)
