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

#include <cstdlib>
#include <cstdio>
#include <cstring> 
#include <string> 


#include "common/tools/brathl_error.h"
#include "brathl.h" 

#include "common/tools/TraceLog.h"
#include "Tools.h" 
#include "common/tools/Exception.h"
#include "ProductRads.h" 




namespace brathl
{


	void CProductRads::Init()
	{
		mLabel = "RADS product";

		InitDateRef();
		InitCriteriaInfo();
	}


	CProductRads::CProductRads()
	{
		Init();
	}



	CProductRads::CProductRads( const std::string& fileName )
		: base_t( fileName )
	{
		Init();
	}


	CProductRads::CProductRads( const CStringList& fileNameList, bool check_only_first_file )
		: base_t( fileNameList, check_only_first_file )
	{
		Init();
	}



	//virtual 
	void CProductRads::InitDateRef()
	{
		m_refDate = REF19500101;
	}



	//----------------------------------------
	void CProductRads::Dump( std::ostream& fOut /* = std::cerr */ )
	{
		if ( CTrace::IsTrace() == false )
		{
			return;
		}


		fOut << "==> Dump a CProductRads Object at " << this << std::endl;

		//------------------
		base_t::Dump( fOut );
		//------------------

		fOut << "==> END Dump a CProductRads Object at " << this << std::endl;

		fOut << std::endl;

	}


} // end namespace
