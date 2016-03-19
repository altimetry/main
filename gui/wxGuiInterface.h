#ifndef WX_GUI_INTERFACE_H
#define WX_GUI_INTERFACE_H

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declarations / Definitions involving (tying) wxWidgets and migrated code, stripped out of wxWidgtes dependencies
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											CMapFunction 
//////////////////////////////////////////////////////////////////////////////////////////////////////////


class CMapFunction;


void NamesToArrayString( const CMapFunction &mfunction, wxArrayString& array );
void NamesToComboBox( const CMapFunction &mfunction, wxComboBox& combo );
void NamesToListBox( const CMapFunction &mfunction, wxListBox& listBox, int category = -1 );
void GetCategory( const CMapFunction &mfunction, wxChoice& choice );



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											CMapProjection
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "new-gui/brat/DataModels/PlotData/MapProjection.h"

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



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											CFormula and related
//////////////////////////////////////////////////////////////////////////////////////////////////////////


class CMapTypeFilter;

void NamesToComboBox( const CMapTypeFilter &map, wxComboBox& combo );


class CMapTypeData;

void NamesToComboBox( const CMapTypeData &map, wxComboBox& combo, bool noData = false );


class CMapTypeOp;

void NamesToComboBox( const CMapTypeOp &map, wxComboBox& combo );


class CMapDataMode;

void NamesToComboBox( const CMapDataMode &map, wxComboBox& combo );


class CMapTypeField;

void NamesToComboBox( const CMapTypeField &map, wxComboBox& combo );


class CMapFormula;

void NamesToComboBox( const CMapFormula &map, wxComboBox& combo );




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											CWorkspace and related
//////////////////////////////////////////////////////////////////////////////////////////////////////////


class CWorkspaceFormula;

void GetFormulaNames( const CWorkspaceFormula &wks, wxComboBox& combo, bool getPredefined = true, bool getUser = true );
void GetFormulaNames( const CWorkspaceFormula &wks, wxListBox& lb, bool getPredefined = true, bool getUser = true );



class CWorkspaceOperation;

void GetOperationNames( const CWorkspaceOperation &wks, wxComboBox& combo );



class CWorkspaceDisplay;

void GetDisplayNames( const CWorkspaceDisplay &wks, wxComboBox& combo );



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											COperation and related
//////////////////////////////////////////////////////////////////////////////////////////////////////////


class COperation;

void GetFormulaNames( const COperation &op, wxComboBox& combo );




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//											CDisplay related
//////////////////////////////////////////////////////////////////////////////////////////////////////////

class CMapTypeDisp;

void NamesToComboBox( const CMapTypeDisp &disp, wxComboBox& combo );


class CMapDisplayData;

void NamesToComboBox( const CMapDisplayData &data, wxComboBox& combo);




#endif		//WX_GUI_INTERFACE_H
