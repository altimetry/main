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
#if !defined(_vtkList_h_)
#define _vtkList_h_

#include "libbrathl/brathl.h"

#include "vtkObject.h"


class vtkObArray : public std::vector<vtkObject*>
{
	using base_t = std::vector<vtkObject*>;

public:

	vtkObArray() : base_t()
	{}

	virtual ~vtkObArray()
	{
		RemoveAll();
	}

	//

//protected:
	virtual void Insert( vtkObject* ob )
	{
		push_back( ob );
	}

	virtual vtkObArray::iterator ReplaceAt( vtkObArray::iterator where, vtkObject* ob )
	{
		Erase( where );
		return InsertAt( where, ob );
	}

	// Remove all elements and clear
	//
	virtual void RemoveAll()
	{
		for ( auto *ptr : *this )
			if ( ptr != nullptr )
				ptr->Delete();

		clear();
	}

private:
#if defined (BRAT_V3)
public:
#endif

	// Delete an element referenced by it
	//	return true if no error, otherwise false
	//
	virtual bool Erase( vtkObArray::iterator it )
	{
		if ( it == end() )
			return false;

		vtkObject *ob = *it;
		if ( ob == nullptr )
			return false;

		erase( it );
		ob->Delete();

		return true;
	}

private:

	virtual vtkObArray::iterator InsertAt( vtkObArray::iterator where, vtkObject* ob )
	{
		return insert( where, ob );
	}

	///Dump function
	virtual void Dump( std::ostream& fOut = std::cerr );
};



/** @} */


#endif // !defined(_List_h_)
