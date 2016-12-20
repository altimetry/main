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
#if !defined(CPRODUCTJASON1NETCDF_H)
#define CPRODUCTJASON1NETCDF_H


#include "ProductNetCdfCF.h"


namespace brathl
{
/** \addtogroup product Products
  @{ */

/**
  Jason-1 GDR (Native/Expertise) product management class.

 \version 1.0
*/

    class CProductJason1NetCdf : public CProductNetCdfCF
    {
        DECLARE_BASE_TYPE( CProductNetCdfCF )

    public:

        /// Empty CProductJason1NetCdf ctor
        CProductJason1NetCdf()
            : base_t()
        {
            InitDateRef();
        }


        /** Creates new CProductJason1NetCdf object
          \param fileName [in] : file name to be connected */
        CProductJason1NetCdf( const std::string& path )
            : base_t( path )
        {
            InitDateRef();
        }


        /** Creates new CProductJason1NetCdf object
          \param fileNameList [in] : list of file to be connected */
        CProductJason1NetCdf( const CStringList& paths, bool check_only_first_file )
            : base_t( paths, check_only_first_file )
        {
            InitDateRef();
        }

        /// Destructor
        virtual ~CProductJason1NetCdf()
        {}


        virtual void InitDateRef(){ m_refDate = REF20000101; }
    };

    /** @} */

} //end namespace

#endif // !defined(CPRODUCTJASON1NETCDF_H)
