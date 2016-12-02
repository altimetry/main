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
#if !defined(_ProductGeosatGDR_h_)
#define _ProductGeosatGDR_h_


#include "ProductNetCdfCF.h"


namespace brathl
{
/** \addtogroup product Products
  @{ */

/**
  Geosat GDR product management class.

 \version 1.0
*/

    class CProductGeosatGDR : public CProductNetCdfCF
    {
        DECLARE_BASE_TYPE( CProductNetCdfCF )

    public:

        /// Empty CProductGeosatGDR ctor
        CProductGeosatGDR()
            : base_t()
        {
            InitDateRef();
        }


        /** Creates new CProductGeosatGDR object
          \param fileName [in] : file name to be connected */
        CProductGeosatGDR( const std::string& path )
            : base_t( path )
        {
            InitDateRef();
        }


        /** Creates new CProductGeosatGDR object
          \param fileNameList [in] : list of file to be connected */
        CProductGeosatGDR( const CStringList& paths, bool check_only_first_file )
            : base_t( paths, check_only_first_file )
        {
            InitDateRef();
        }

        /// Destructor
        virtual ~CProductGeosatGDR()
        {}


        virtual void InitDateRef(){ m_refDate = REF19850101; }
    };

    /** @} */

} //end namespace

#endif // !defined(_ProductGeosatGDR_h_)
