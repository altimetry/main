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
#if !defined(_ProductReaper_h_)
#define _ProductReaper_h_


#include "ProductNetCdfCF.h"


namespace brathl
{
/** \addtogroup product Products
  @{ */

/** 
  Reaper product management class.

 \version 1.0
*/

	class CProductReaper : public CProductNetCdfCF
	{
		using base_t = CProductNetCdfCF;

	public:

		/// Empty CProductReaper ctor
		CProductReaper()
			: base_t()
		{
			InitDateRef();
		}


		/** Creates new CProductReaper object
		  \param fileName [in] : file name to be connected */
		CProductReaper( const std::string& path )
			: base_t( path )
		{
			InitDateRef();
		}


		/** Creates new CProductReaper object
		  \param fileNameList [in] : list of file to be connected */
		CProductReaper( const CStringList& paths )
			: base_t( paths )
		{
			InitDateRef();
		}

		/// Destructor
		virtual ~CProductReaper()
		{}


		virtual void InitDateRef(){ m_refDate = REF19900101; }
	};

	/** @} */

} //end namespace

#endif // !defined(_ProductReaper_h_)
