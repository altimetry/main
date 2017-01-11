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
#include "new-gui/brat/stdafx.h"

#include "common/tools/Trace.h"
#include "common/tools/Exception.h"
#include "common/QtUtilsIO.h"

#include "libbrathl/Tools.h"

#include "../Filters/BratFilters.h"
#include "WorkspaceSettings.h"
#include "Dataset.h"



std::string CDataset::GetFieldSpecificUnit( const std::string& key )
{
	return m_fieldSpecificUnits.Exists( key );
}


void CDataset::SetFieldSpecificUnit( const std::string& key, const std::string& value )
{
	m_fieldSpecificUnits.Erase( key );
	m_fieldSpecificUnits.Insert( key, value, false );
}

// If filtering fails, all files of the original dataset are inserted
//
std::pair< bool, bool > CDataset::ApplyFilter( const CBratFilter *filter, const CDataset *original_dataset, std::string &error_msg, CProgressInterface *progress )
{
	assert__( GetProductList()->mCodaProductClass == original_dataset->GetProductList()->mCodaProductClass );
	assert__( GetProductList()->mCodaProductType == original_dataset->GetProductList()->mCodaProductType );
	assert__( GetProductList()->m_productClass == original_dataset->GetProductList()->m_productClass );
	assert__( GetProductList()->m_productType == original_dataset->GetProductList()->m_productType );
	assert__( GetProductList()->m_productFormat == original_dataset->GetProductList()->m_productFormat );


	std::pair< bool, bool > result { true, false };

	if ( filter )
	{
		result = filter->Apply( *original_dataset->GetProductList(), *GetProductList(), error_msg, progress );
	}

	if ( !filter || !result.first )
		GetProductList()->Insert( *original_dataset->GetProductList() );

	return result;
}


//v4 Returns list of files in error (not found) for client code to display
//
bool CDataset::CheckFilesExist( std::vector< std::string > &v )
{
	for ( CProductList::iterator it = m_files.begin(); it != m_files.end(); it++ )
	{
		if ( !IsFile( it->c_str() ) )
			v.push_back(*it);
	}
	return v.empty();
}

CProduct* CDataset::OpenProduct() const
{
	if ( m_files.size() == 0 )
		return nullptr;

	return OpenProduct( *m_files.begin() );
}

CProduct* CDataset::OpenProduct( const std::string& fileName ) const
{
	return OpenProduct( fileName, m_fieldSpecificUnits );
}

//----------------------------------------
//became static in v4
//
CProduct* CDataset::OpenProduct( const std::string& fileName, const CStringMap& fieldSpecificUnit )
{
	CProduct* product = nullptr;
	try
	{
		product = CProduct::Construct( fileName );
        product->SetFieldSpecificUnits( fieldSpecificUnit );
        product->Open( fileName );
	}
	catch ( const CException& e )
	{
		UNUSED( e );

		if ( product != nullptr )
		{
			delete product;
			product = nullptr;
		}
		throw;
	}

	return product;
}


//----------------------------------------
bool CDataset::SaveConfig( CWorkspaceSettings *config ) const
{
	assert__( config );

	return config->SaveConfig( this );
}
bool CDataset::LoadConfig( CWorkspaceSettings *config )
{
	assert__( config );

	return config->LoadConfig( this );
}

#if defined(BRAT_V3)

//----------------------------------------
void CDataset::Dump( std::ostream& fOut /* = std::cerr */ )
{
	if ( !CTrace::IsTrace() )
		return;

	fOut << "==> Dump a CDataset Object at " << this << std::endl;

	m_files.Dump( fOut );
	fOut << "==> END Dump a CDataset Object at " << this << std::endl;

	fOut << std::endl;
}

#endif
