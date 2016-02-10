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

#include "new-gui/QtInterface.h"
#include "Constants.h"

#include "libbrathl/brathl.h"
#include "libbrathl/Product.h"


class CWorkspaceSettings;



class CDataset : public CBratObject
{
protected:
	CProductList m_files;
	CStringMap m_fieldSpecificUnit;

private:
	std::string m_name;

public:

	/// ctors

	CDataset( const std::string name ) : m_name ( name )
	{}
	CDataset( CDataset &o )
	{
		*this = o;
	}

	CDataset& operator=( CDataset &o )
	{
		if ( this != &o )
		{
			m_name = o.m_name;
			m_files.Insert( o.m_files );
		}
		return *this;
	}

	/// Destructor

	virtual ~CDataset()
	{
		m_fieldSpecificUnit.RemoveAll();
	}

	/// 

	const std::string& GetName() const { return m_name; }
	void SetName( const std::string& value ) { m_name = value; }

	bool CtrlFiles( std::vector< std::string > &v );

	bool SaveConfig( CWorkspaceSettings* config ) const;

public:
	bool LoadConfig( CWorkspaceSettings *config );

	CProduct* SetProduct( const std::string& fileName );
	CProduct* SetProduct();

	const CProductList* GetProductList() const { return &m_files; }
	CProductList* GetProductList() { return &m_files; }

	CStringMap* GetFieldSpecificUnits() { return &m_fieldSpecificUnit; }
	const CStringMap* GetFieldSpecificUnits() const { return &m_fieldSpecificUnit; }

	std::string GetFieldSpecificUnit( const std::string& key );
	void SetFieldSpecificUnit( const std::string& key, const std::string& value );

	void GetFiles( std::vector<std::string> &v ) const
	{
		for ( CProductList::const_iterator it = m_files.begin(); it != m_files.end(); ++it )
		{
			v.push_back( *it );
		}
	}


	///Dump fonction
	virtual void Dump( std::ostream& fOut = std::cerr );
};

/** @} */


#endif // !defined(WORKSPACES_DATASET_H)
