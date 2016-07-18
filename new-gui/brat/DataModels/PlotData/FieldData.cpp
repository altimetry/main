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

#include "new-gui/Common/tools//Trace.h"
#include "libbrathl/InternalFilesYFX.h"

using namespace brathl;

#include "FieldData.h"

//static 
const CFieldData::unsigned_t CFieldData::smDefaultOpacity = 

#if defined(BRAT_V3)
		0.6;
#else
		0xFF;			//v4: so far used only in YFX
#endif


//static 
const unsigned CFieldData::smDefaultNumberOfDigits = 5;

//static 
const unsigned CFieldData::smDefaultNumberOfTicks = 5;


//static 
const bool CFieldData::smDefaultShowLines = true;

//static 
const bool CFieldData::smDefaultShowPoints = false;

//static 
const double CFieldData::smDefaultPointSize = 1.0;

//static 
const EPointGlyph CFieldData::smDefaultGlyphType = displayCIRCLE_GLYPH;

//static 
const bool CFieldData::smDefaultFilledPoint = true;

//static 
const CFieldData::unsigned_t CFieldData::smDefaultLineWidth = 1;		//2 -> 1

//static 
const EStipplePattern CFieldData::smDefaultStipplePattern =
#if defined(BRAT_V3)
		EStipplePattern::displayFULL;
#else
		EStipplePattern::eSolidLine;
#endif

//static 
const CPlotColor CFieldData::smDefaultPointColor = CPlotColor( 0., 0., 0. );

//static 
const int CFieldData::smDefaultNumColorLabels = 5;

//static 
const bool CFieldData::smDefaultWithContour = false;

//static 
const unsigned CFieldData::smDefaultNumContour = 5;

//static 
const double CFieldData::smDefaultContourLineWidth = 0.1;

//static 
const CPlotColor CFieldData::smDefaultContourLineColor = CPlotColor( 0., 0., 0. );

//static 
const unsigned CFieldData::smDefaultNumberOfBins = 20;






CFieldData::CFieldData()
	: base_t()
{
	mLUT.ExecMethodDefaultColorTable();
}


//ex- CPlotField function
//
CInternalFilesYFX* CFieldData::InternalFileYFX( size_t index ) const
{
	assert__( index < mInternalFiles.size() );

	CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>( InternalFile( index ) );
	if ( yfx == NULL )
	{
		CException e( CTools::Format( "CFieldData::GetInternalFilesYFX - dynamic_cast<CInternalFilesYFX*>(mInternalFiles[%d]) returns NULL"
			"- Valid range is [0, %d]"
			"- object seems not to be an instance of CInternalFilesYFX",
            (int)index, (int)mInternalFiles.size() - 1 ),
			BRATHL_LOGIC_ERROR );
		CTrace::Tracer( "%s", e.what() );
		throw ( e );

	}

	return yfx;
}


void CFieldData::SetXLog( bool value )
{
#if defined (BRAT_V3)
	if ( Xmin() <= 0 && value )
	{
		std::string msg = CTools::Format( "ERROR in CXYPlotProperties::SetXLog : X range [%g, %g] contains value <= 0 - no logarithmic axis possible", Xmin(), Xmax() );
		CException e( msg, BRATHL_LOGIC_ERROR );
		throw ( e );
	}
#endif

	mXlogarithmic = value;	//v3 assigned before throw
}


void CFieldData::SetYLog( bool value )
{
#if defined (BRAT_V3)
	if ( Ymin() <= 0 && value )
	{
		std::string msg = CTools::Format( "ERROR in CXYPlotProperties::SetYLog : Y range [%g, %g] contains value <= 0 - no logarithmic axis possible", Ymin(), Ymax() );
		CException e( msg, BRATHL_LOGIC_ERROR );
		throw ( e );
	}
#endif

	mYlogarithmic = value;	//v3 assigned before throw
}
