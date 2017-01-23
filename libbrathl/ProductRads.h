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
#if !defined(_ProductRads_h_)
#define _ProductRads_h_

#include "ProductNetCdfCF.h"
using namespace brathl;

namespace brathl
{


/** \addtogroup product Products
  @{ */

/** 
  RADS product management class.


 \version 1.0
*/

	class CProductRads : public CProductNetCdfCF
	{
		DECLARE_BASE_TYPE( CProductNetCdfCF )

		//construction / destruction

		void Init();
	public:

		/// Empty CProductRads ctor
		CProductRads();

		/** Creates new CProductRads object
		  \param fileName [in] : file name to be connected */
		CProductRads( const std::string &fileName );

		/** Creates new CProductRads object
		  \param fileNameList [in] : list of file to be connected */
		CProductRads( const CStringList &fileNameList, bool check_only_first_file );

		/// Destructor
		virtual ~CProductRads()
		{}


		//overrides

		virtual std::string GetLabelForCyclePass() const override;

	protected:

		virtual void InitDateRef() override;

	public:
		//Dump function
		virtual void Dump( std::ostream& fOut = std::cerr ) override;
	};

/** @} */

} //end namespace

#endif // !defined(_ProductRads_h_)
