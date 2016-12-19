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
bool CDataset::ApplyFilter( const CBratFilter *filter, const CDataset *original_dataset, std::string &error_msg, CProgressInterface *pi )
{
	assert__( GetProductList()->mCodaProductClass == original_dataset->GetProductList()->mCodaProductClass );
	assert__( GetProductList()->mCodaProductType == original_dataset->GetProductList()->mCodaProductType );
	assert__( GetProductList()->m_productClass == original_dataset->GetProductList()->m_productClass );
	assert__( GetProductList()->m_productType == original_dataset->GetProductList()->m_productType );
	assert__( GetProductList()->m_productFormat == original_dataset->GetProductList()->m_productFormat );


	bool result = true;

	if ( filter )
	{
		result = filter->Apply( *original_dataset->GetProductList(), *GetProductList(), error_msg, pi );
	}

	if ( !filter || !result )
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




//static 
const CProductInfo CProductInfo::smInvalidProduct;


// (*) The purpose of this ctor is to have a !IsValid() instance (that in consequence 
//	cannot be used), where product parameters are required with default null
//
CProductInfo::CProductInfo()
	: base_t()
	, mErrorMessages( "Null product" )	//never displayed (*)
{
	assert__( !IsValid() );				//(*) 
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
	: base_t()
	, mProduct( product )
	, mExternalProduct( true )
{
	assert__( !IsValid() );

	if ( !mProduct )
		mErrorMessages = "There is no product.";
	else
		ExtractInfo();
}

CProductInfo::CProductInfo( const CDataset *dataset, const std::string &file_path )
	: base_t()
	, mExternalProduct( false )
{
	assert__( !IsValid() );

	const std::string& path = 
		file_path.empty() ? 
		( dataset->IsEmpty() ? file_path : dataset->GetFirstFile() ) :
		file_path;

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


bool CProductInfo::IsLongitudeFieldName( const std::string &name ) const 
{ 
	assert__( mProduct );

	return mProduct->IsLongitudeFieldName( name );
}

bool CProductInfo::IsLatitudeFieldName( const std::string &name ) const 
{ 
	assert__( mProduct );

	return mProduct->IsLatitudeFieldName( name );
}


const std::string& CProductInfo::FindAliasValue( const std::string &alias_name ) const
{
	assert__( mProduct );

	return CBratFilters::FindAliasValue( mProduct, alias_name );
}

std::pair<CField*, CField*> CProductInfo::FindLonLatFields( bool try_unsupported, bool &lon_alias_used, bool &lat_alias_used, std::string &field_error_msg ) const
{
    assert__( mProduct );

	return CBratFilters::FindLonLatFields( mProduct, try_unsupported, lon_alias_used, lat_alias_used, field_error_msg );
}

CField* CProductInfo::FindTimeField( bool try_unsupported, bool &alias_used, std::string &field_error_msg ) const
{
	assert__( mProduct );

	return CBratFilters::FindTimeField( mProduct, try_unsupported, alias_used, field_error_msg );
}

CField* CProductInfo::FindFieldByName( const std::string& field_name, const std::string& dataset_name, std::string &error_msg ) const
{
	assert__( mProduct );

	return mProduct->FindFieldByName( field_name, dataset_name, false, &error_msg, false );
}

CField* CProductInfo::FindFieldByName( const std::string& field_name, std::string &error_msg ) const
{
	assert__( mProduct );

	const bool with_except = false;

	return mProduct->FindFieldByName( field_name, with_except, &error_msg, false );
}

bool CProductInfo::CheckFieldNames( const CExpression &expr, const std::string &dataset_name, CStringArray &field_names_not_found ) const
{
	assert__( mProduct );

	return mProduct->CheckFieldNames( expr, dataset_name, field_names_not_found );
}

bool CProductInfo::AddRecordNameToField( const CExpression &expr, const std::string &dataset_name, CExpression &expr_out, std::string &error_msg ) const
{
	assert__( mProduct );

	return mProduct->AddRecordNameToField( expr, dataset_name, expr_out, error_msg );
}

bool CProductInfo::AddRecordNameToField( const std::string &in, const std::string &dataset_name, std::string &out, std::string &error_msg ) const
{
	assert__( mProduct );

	return mProduct->AddRecordNameToField( in, dataset_name, out, error_msg );
}



void CProductInfo::AliasKeys( CStringArray& keys ) const
{
	assert__( mProduct );

	return mProduct->GetAliasKeys( keys );
}

std::string CProductInfo::AliasExpandedValue( const std::string &key ) const
{
	assert__( mProduct );

	return mProduct->GetAliasExpandedValue( key );
}

const CProductAlias* CProductInfo::Alias( const std::string &key ) const
{
	assert__( mProduct );

	return mProduct->GetAlias( key );
}

bool CProductInfo::HasAliases() const
{
	assert__( mProduct );

	return mProduct->HasAliases();
}
const CStringMap* CProductInfo::AliasesAsString() const 
{ 
	assert__( mProduct );

	return mProduct->GetAliasesAsString(); 
}

void CProductInfo::ReplaceNamesCaseSensitive( const std::string &in, std::string &out, bool force_reload ) const		//force_reload = false 
{
	assert__( mProduct );

	return mProduct->ReplaceNamesCaseSensitive( in, out, force_reload );
}

void CProductInfo::ReplaceNamesCaseSensitive( const CExpression &expr_in, const CStringArray &fields_in, CExpression &expr_out, bool force_reload ) const		//force_reload = false 
{
	assert__( mProduct );

	return mProduct->ReplaceNamesCaseSensitive( expr_in, fields_in, expr_out, force_reload );
}


bool CProductInfo::HasCompatibleDims( const std::string &value, const std::string &dataset_name, std::string &msg, bool use_virtual_dims, CUIntArray *common_dimensions ) const	//common_dimensions = nullptr 
{
	assert__( mProduct );

	return mProduct->HasCompatibleDims( value, dataset_name, msg, use_virtual_dims, common_dimensions );
}




brathl_refDate CProductInfo::RefDate() const 
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
const std::string CProductInfo::Label() const
{
	assert__( mProduct );

	return mProduct->GetLabel();
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
std::string CProductInfo::ProductClassAndType() const
{ 
	assert__( mProduct );

	return mProduct->GetProductClassAndType();
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
