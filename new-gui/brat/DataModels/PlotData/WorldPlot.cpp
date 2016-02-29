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

#include "new-gui/Common/tools/Exception.h"
#include "ApplicationLogger.h"
#include "libbrathl/Tools.h"
#include "libbrathl/InternalFiles.h"
#include "libbrathl/InternalFilesZFXY.h"
using namespace brathl;

#include "PlotField.h"
#include "WorldPlot.h"
#include "WorldPlotData.h"


//-------------------------------------------------------------
//------------------- CWPlot class --------------------
//-------------------------------------------------------------

//----------------------------------------
void CWPlot::GetInfo()
{
	CPlotField* field = CPlotField::GetPlotField( *( m_fields.begin() ) );

	if ( field == NULL )
	{
		return;
	}

	CWorldPlotProperties* worldProps = field->m_worldProps;

	if ( worldProps != NULL )
	{
		m_title = worldProps->m_title;
	}

	if ( m_title.empty() )
	{
		if ( field->m_internalFiles.size() > 0 )
		{
			CInternalFilesZFXY* zfxy = CWPlot::GetInternalFilesZFXY( *( field->m_internalFiles.begin() ) );
			m_title = zfxy->GetTitle( "" ).c_str();
		}
	}
}
//----------------------------------------
CInternalFiles* CWPlot::GetInternalFiles( CBratObject* ob, bool withExcept /* = true */ )
{
	UNUSED( withExcept );

	return GetInternalFilesZFXY( ob );
}
//----------------------------------------
CInternalFilesZFXY* CWPlot::GetInternalFilesZFXY( CBratObject* ob, bool withExcept /* = true */ )
{
	CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>( ob );
	if ( zfxy == NULL )
	{
		if ( withExcept )
		{
			CException e( "CWPlot::GetInternalFilesZFXY - Non-ZFXY data found - dynamic_cast<CInternalFilesZFXY*>(ob) returns NULL",
				BRATHL_LOGIC_ERROR );
			LOG_TRACE( e.what() );
			throw ( e );
		}

	}
	if ( zfxy->IsGeographic() == false )
	{
		if ( withExcept )
		{
			CException e( "CWPlot::GetInternalFilesZFXY  - a ZFXY data seems to be NOT geographical  - zfxy->IsGeographic() is true",
				BRATHL_LOGIC_ERROR );
			LOG_TRACE( e.what() );
			throw e;
		}
	}

	return zfxy;
}




