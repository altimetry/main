#ifndef WX_GUI_INTERFACE_H
#define WX_GUI_INTERFACE_H

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declarations / Definitions involving (tying) wxWidgets and migrated code, stripped out of wxWidgtes dependencies
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											CMapProjection
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../display/PlotData/MapProjection.h"

// old CMapProjection member functions

inline void NamesToArrayString( const CMapProjection &mapp, wxArrayString& array )
{
	for (CMapProjection::const_iterator it = mapp.begin(); it != mapp.end(); it++ )
	{
		uint32_t value = it->second;
		if ( !isDefaultValue( value ) )
		{
			array.Add( ( it->first ).c_str() );
		}
	}
}


inline void NamesToComboBox( const CMapProjection &mapp, wxComboBox& combo )
{
	for (CMapProjection::const_iterator it = mapp.begin(); it != mapp.end(); it++ )
	{
		uint32_t value = it->second;
		if ( !isDefaultValue( value ) )
		{
			combo.Append( ( it->first ).c_str() );
		}
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											CDataSet 
//////////////////////////////////////////////////////////////////////////////////////////////////////////

class CDataset;
class CWorkspaceDataset;

// old CDataset member functions

void GetFiles( const CDataset &d, wxArrayString& array );
void GetFiles( const CDataset &d, wxListBox& array );

void GetDatasetNames( const CWorkspaceDataset *d, wxComboBox& combo );



#endif		//WX_GUI_INTERFACE_H
