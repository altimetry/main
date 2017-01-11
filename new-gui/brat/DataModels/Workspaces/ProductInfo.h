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
#if !defined(WORKSPACES_PRODUCT_INFO_H)
#define WORKSPACES_PRODUCT_INFO_H

#include "Dataset.h"


// Constructs and opens a product based on 
//	- passed file or 
//	- the first file of the dataset or
//	- existing product, when one is already at hand
//
// Centralizes most (if not all) uses made by brat v3 of CProduct, and those uses were 
//		always made based on a passed file (typically a dataset candidate) or on the first 
//		file of the dataset.
//
// When an unfiltered product is enough, use this class and not CProduct
//
class CProductInfo : public non_copyable
{
	//types

	using base_t = non_copyable;

	
	//statics

public:
	static const CProductInfo smInvalidProduct;


private:

	//instance data 

	bool mValid = false;

	std::string mErrorMessages;

	CProduct *mProduct = nullptr;

	const bool mExternalProduct = true;

	std::vector< const CField *> mFields;

	std::string mRecord;

	
	//construction / destruction

	void ExtractInfo();

	//	An always INVALID product (to be used where null product pointers were used)
	//
	CProductInfo();
public:

	//	Always call Valid after construction and do not use if false
	//	Construction errors retrieved in mErrorMessages
	//
	CProductInfo( const CDataset *dataset, const std::string &file_path = empty_string() );

	explicit CProductInfo( CProduct *product );	//brathl turns difficult to read const CProduct information

	virtual ~CProductInfo();


	// Properties

	const bool IsValid() const { return mValid; }

	const std::string& ErrorMessages() const { return mErrorMessages; }


	//...ref date

	brathl_refDate RefDate() const;


	//...record

	const std::string& Record() const { return mRecord; }


	//...classification

	const std::string& Type() const;
	const std::string& Class() const;
	const std::string& Description() const;
	std::string ProductClassAndType() const;
	bool IsNetCdf() const;
	bool IsNetCdfOrNetCdfCF() const;
	const std::string Label() const;


	//...aliases

	bool HasAliases() const;

	const std::string& FindAliasValue( const std::string &alias_name ) const;

	void AliasKeys( CStringArray& keys ) const;

	std::string AliasExpandedValue( const std::string &key ) const;

	const CProductAlias* Alias( const std::string &key ) const;

	const CStringMap* AliasesAsString() const;

	void ReplaceNamesCaseSensitive( const std::string &in, std::string &out, bool force_reload = false ) const;
	void ReplaceNamesCaseSensitive( const CExpression& expr_in, const CStringArray& fields_in, CExpression& expr_out, bool force_reload = false ) const;



	//...fields

	// Caution: this is a flattening of the tree structure (when you only need an unstructured list)
	//
	const std::vector< const CField* >& Fields() const { return mFields; }

	bool IsLongitudeFieldName( const std::string& name ) const;
	bool IsLatitudeFieldName( const std::string& name ) const;


	CField* FindField( const std::string &name, bool try_unsupported, bool &alias_used, std::string &field_error_msg ) const;

	std::pair<CField*, CField*> FindLonLatFields( bool try_unsupported, bool &lon_alias_used, bool &lat_alias_used, std::string &field_error_msg ) const;

	CField* FindTimeField( bool try_unsupported, bool &alias_used, std::string &field_error_msg ) const;

	CField* FindFieldByName( const std::string& field_name, const std::string& dataset_name, std::string &error_msg ) const;
	CField* FindFieldByName( const std::string& field_name, std::string &error_msg ) const;

	bool CheckFieldNames( const CExpression &expr, const std::string &dataset_name, CStringArray &field_names_not_found ) const;

	bool AddRecordNameToField( const CExpression &expr, const std::string &dataset_name, CExpression &expr_out, std::string &error_msg ) const;
	bool AddRecordNameToField( const std::string &in, const std::string &dataset_name, std::string &out, std::string &error_msg ) const;


	//...dimensions

	bool HasCompatibleDims( const std::string &value, const std::string &dataset_name, std::string &msg, bool use_virtual_dims, CUIntArray *common_dimensions = nullptr ) const;
};



#endif // !defined(WORKSPACES_PRODUCT_INFO_H)
