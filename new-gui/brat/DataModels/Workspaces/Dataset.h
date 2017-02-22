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
#if !defined(WORKSPACES_DATASET_H)
#define WORKSPACES_DATASET_H

#include "libbrathl/brathl.h"
#include "libbrathl/Product.h"


class CWorkspaceSettings;
class CBratFilter;


class CDataset : public CBratObject
{
	// types

	using base_t = CBratObject;

	friend class CWorkspaceSettings;


	// instance data

protected:
	CProductList m_files;
	CStringMap m_fieldSpecificUnits;

private:
	std::string m_name;

public:

	// construction / destruction

	explicit CDataset( const std::string &name ) 
		: m_name ( name )
	{}

	CDataset( const CDataset &o )
	{
		*this = o;
	}


	CDataset& operator= ( const CDataset &o )
	{
		if ( this != &o )
		{
			m_files.clear();
			m_name = o.m_name;
			m_files.Insert( o.m_files );
		}
		return *this;
	}


	virtual ~CDataset()
	{
		m_fieldSpecificUnits.RemoveAll();
	}


	// persistence

	virtual bool SaveConfig( CWorkspaceSettings *config ) const;
	virtual bool LoadConfig( CWorkspaceSettings *config );


	// ...  

	// identification

	const std::string& GetName() const { return m_name; }
	void SetName( const std::string& value ) { m_name = value; }


	// field specific units

	const CStringMap* GetFieldSpecificUnits() const { return &m_fieldSpecificUnits; }
	std::string GetFieldSpecificUnit( const std::string& key );

protected:

	// TODO
	// SetFieldSpecificUnit is used in v3, in the fields list menu at the operations level
	//	- the menu item is enabled if "CField::GetFieldNetCdfCFAttr( field, false ) != NULL";
	//	- it is odd that a change in the operation affects a dataset that can be used by multiple operations
	//	- it is odd that all values in m_fieldSpecificUnits are written to the configuration and only the first one loaded
	//
	CStringMap* GetFieldSpecificUnits() { return &m_fieldSpecificUnits; }
	void SetFieldSpecificUnit( const std::string& key, const std::string& value );


	// dataset files

protected:

	virtual bool CheckFiles( bool onlyFirstFile = false, bool onlyFirstNetcdf = false )
	{
		return m_files.CheckFiles( onlyFirstFile, onlyFirstNetcdf );
	}


public:

	CProductList* GetProductList() { return &m_files; }

	const CProductList* GetProductList() const { return &m_files; }

	const std::string& ProductClass() const
	{
		return m_files.m_productClass;
	}

	std::string ProductType() const
	{
		return m_files.m_productType;
	}


	size_t Size() const { return m_files.size(); }

	bool IsEmpty() const { return m_files.empty(); }

protected:

	bool CheckFilesExist( std::vector< std::string > &v );

public:
	virtual const std::string& GetFirstFile() const { return *m_files.begin(); }

	std::string ToString( const std::string& delim ) const
	{
		return m_files.ToString( delim );
	}

	std::pair< bool, bool > ApplyFilter( const CBratFilter *filter, const CDataset *original_dataset, std::string &error_msg, CProgressInterface *progress );


public:

	void InsertProduct( const std::string &file ) 
	{ 
		return m_files.Insert( file ); 
	}

	void AddProduct( const std::string &file )
	{
		m_files.InsertUnique( file, true );
		m_files.CheckFile( std::prev( m_files.end() ), true );
	}

	void InsertUniqueProducts( const CStringList &files )
	{
		m_files.InsertUnique( files );
	}

	void EraseProduct( const std::string &file )
	{
		m_files.Erase( file );
	}

	void ClearProductList()
	{
		m_files.clear();
	}


	template< typename STRING_CONTAINER = std::vector< std::string > >
	STRING_CONTAINER GetFiles( bool only_one_file = false ) const
	{
		STRING_CONTAINER v;

		if ( only_one_file && !m_files.empty() )
		{
			v.push_back( m_files.begin()->c_str() );
		}
		else
			for ( auto const &file : m_files )
			{
				v.push_back( file.c_str() );
			}

		return v;
	}


	// dataset product

private:
	//v4 NOTE: all OpenProduct renamed from SetProduct

	static CProduct* OpenProduct( const std::string& fileName, const CStringMap& fieldSpecificUnit );

	CProduct* OpenProduct( const std::string& fileName ) const;

public:
	CProduct* OpenProduct() const;

private:

#if defined(BRAT_V3)

	//Dump function

	virtual void Dump( std::ostream& fOut = std::cerr );
#endif
};



#endif // !defined(WORKSPACES_DATASET_H)
