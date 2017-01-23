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

#include "libbrathl/ProductNetCdf.h"

#include "../Filters/BratFilters.h"

#include "ProductInfo.h"



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
		mRecord = CProductNetCdf::m_virtualRecordName;	//TODO check that a defaultRecord in aliases.xml is equivalent and m_virtualRecordName can be deleted
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

CField* CProductInfo::FindField( const std::string &name, bool try_unsupported, bool &alias_used, std::string &field_error_msg ) const
{
	return CBratFilters::FindField( mProduct, name, try_unsupported, alias_used, field_error_msg );
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
const std::string& CProductInfo::Label() const
{
	assert__( mProduct );

	return mProduct->GetLabel();
}
std::string CProductInfo::LabelForCyclePass() const
{
	assert__( mProduct );

	return mProduct->GetLabelForCyclePass();
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
