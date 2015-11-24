#ifndef WX_GUI_INTERFACE_H
#define WX_GUI_INTERFACE_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											CMapProjection
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../display/PlotData/MapProjection.h"


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



#endif		//WX_GUI_INTERFACE_H
