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
#include "new-gui/Common/QtUtilsIO.h"

#include "libbrathl/Tools.h"
#include "libbrathl/ProductNetCdf.h"

#include "DataModels/Filters/BratFilters.h"
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
bool CDataset::ApplyFilter( const CBratFilter *filter, const CDataset *original_dataset, std::string &error_msg )
{
	bool result = true;

	CStringList all_files_list = *original_dataset->GetProductList();

	if ( filter )
	{
		result = filter->Apply( all_files_list, *GetProductList(), error_msg );
	}

	if ( !filter || !result )
		GetProductList()->Insert( all_files_list );

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

//----------------------------------------
CProduct* CDataset::SafeOpenProduct() const
{
	try
	{
		return OpenProduct();
	}
	catch ( const CException & )
	{
	}

	return nullptr;
}
CProduct* CDataset::SafeOpenProduct( const std::string& fileName ) const
{
	try
	{
		return OpenProduct( fileName );
	}
	catch ( const CException & )
	{
	}

	return nullptr;
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


void CProductInfo::ExtractInfo()
{
	assert__( mProduct );

	CTreeField *tree = mProduct->GetTreeField();
	for ( auto const *object : *tree )
	{
		const CField *field  = static_cast<const CField*>( object );			assert__( dynamic_cast<const CField*>( object ) != nullptr );
		mFields.push_back( field );
	}

	if ( mProduct->IsNetCdfOrNetCdfCFProduct() )
	{
		mRecord = CProductNetCdf::m_virtualRecordName;		//TODO this is done for COperation; should we do it HERE????
	}
	else
	{
		auto *aliases = mProduct->GetAliases();
		if ( aliases )
			mRecord = aliases->GetRecord();
	}

	mValid = true;
}

CProductInfo::CProductInfo( CProduct *product )
	: mProduct( product )
	, mExternalProduct( true )
{
	ExtractInfo();
}

CProductInfo::CProductInfo( const CDataset *dataset, const std::string &file_path )
	: mExternalProduct( false )
{
	const std::string& path = file_path.empty() ? dataset->GetFirstFile() : file_path;

	try
	{
		mProduct = CProduct::Construct( path );
		mProduct->SetFieldSpecificUnits( *dataset->GetFieldSpecificUnits() );
		mProduct->Open( path );

		ExtractInfo();
	}
	catch ( const CException &e )
	{
		mErrorMessages = e.Message();
	}
	catch ( ... )
	{
		mErrorMessages = "Unknown error reading file " + path;
	}
}

//virtual 
CProductInfo::~CProductInfo()
{
	if ( !mExternalProduct )
		delete mProduct;
}


const std::string& CProductInfo::FindAliasValue( const std::string &alias_name )
{
	assert__( mProduct );

	return CBratFilters::FindAliasValue( mProduct, alias_name );
}

std::pair<CField*, CField*> CProductInfo::FindLonLatFields( bool try_unsupported, bool &lon_alias_used, bool &lat_alias_used, std::string &field_error_msg )
{
    assert__( mProduct );

	return CBratFilters::FindLonLatFields( mProduct, try_unsupported, lon_alias_used, lat_alias_used, field_error_msg );
}

CField* CProductInfo::FindTimeField( bool try_unsupported, bool &alias_used, std::string &field_error_msg )
{
	assert__( mProduct );

	return CBratFilters::FindTimeField( mProduct, try_unsupported, alias_used, field_error_msg );
}


brathl_refDate CProductInfo::GetRefDate() const 
{ 
	assert__( mProduct );

	return mProduct->GetRefDate(); 
}

bool CProductInfo::IsNetCdf() const
{ 
	assert__( mProduct );

	return mProduct->IsNetCdf();
}
bool CProductInfo::IsNetCdfOrNetCdfCF() const
{
	assert__( mProduct );

	return mProduct->IsNetCdfOrNetCdfCFProduct();
}
bool CProductInfo::HasAliases() const
{
	assert__( mProduct );

	return mProduct->HasAliases();
}


const std::string& CProductInfo::Type() const
{ 
	assert__( mProduct );

	return mProduct->GetProductType();
}
const std::string& CProductInfo::Class() const
{ 
	assert__( mProduct );

	return mProduct->GetProductClass();
}
const std::string& CProductInfo::Description() const
{ 
	assert__( mProduct );

	return mProduct->GetDescription();
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

