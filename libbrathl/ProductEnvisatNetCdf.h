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
#if !defined(CPRODUCTENVISATNETCDF_H)
#define CPRODUCTENVISATNETCDF_H


#include "ProductNetCdfCF.h"


namespace brathl
{
/** \addtogroup product Products
  @{ */

/**
  Reaper product management class.

 \version 1.0
*/

    class CProductEnvisatNetCdf : public CProductNetCdfCF
    {
        DECLARE_BASE_TYPE( CProductNetCdfCF )

    public:

        /// Empty CProductEnvisatNetCdf ctor
        CProductEnvisatNetCdf()
            : base_t()
        {
            InitDateRef();
        }


        /** Creates new CProductEnvisatNetCdf object
          \param fileName [in] : file name to be connected */
        CProductEnvisatNetCdf( const std::string& path )
            : base_t( path )
        {
            InitDateRef();
        }


        /** Creates new CProductEnvisatNetCdf object
          \param fileNameList [in] : list of file to be connected */
        CProductEnvisatNetCdf( const CStringList& paths, bool check_only_first_files )
            : base_t( paths, check_only_first_files )
        {
            InitDateRef();
        }

        /// Destructor
        virtual ~CProductEnvisatNetCdf()
        {}


        virtual void InitDateRef(){ m_refDate = REF20000101; }
    };

    /** @} */

} //end namespace

#endif // !defined(CPRODUCTENVISATNETCDF_H)
