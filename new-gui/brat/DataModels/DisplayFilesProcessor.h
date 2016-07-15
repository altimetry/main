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

#ifndef DATA_MODELS_DISPLAY_CMD_LINE_PROCESSOR_H
#define DATA_MODELS_DISPLAY_CMD_LINE_PROCESSOR_H


#include "DisplayDataProcessor.h"
		


class CDisplayFilesProcessor : public CDisplayDataProcessorBase
{
	//types

	using base_t = CDisplayDataProcessorBase;

	//data

	std::string m_paramFile;
	std::string m_paramXAxis;
	std::string m_paramYAxis;

	CStringList m_paramVars;
	CFileParams m_params;

public:

	//Public ctor

	CDisplayFilesProcessor( bool maps_as_plots )
		: base_t( maps_as_plots )
	{}

	CDisplayFilesProcessor( bool maps_as_plots, const std::string &display_cmd_file )
		: base_t( maps_as_plots )
	{
		Process( display_cmd_file );
	}

	CDisplayFilesProcessor( bool maps_as_plots, const std::vector< std::string > &args )
		: base_t( maps_as_plots )
	{
		Process( args );
	}

	//Destruction

	virtual ~CDisplayFilesProcessor()
	{}


	//Access

    virtual const std::string& ParamFile() const override { return m_paramFile;  }


	//Operations

	bool Process( const std::vector< std::string > &args );

	bool Process( int argc, const char **argv )
	{
		std::vector< std::string > v;
		for ( int i = 0; i < argc; ++i )
			v.push_back( argv[ i ] );

		return Process( v );
	}

	bool Process( const std::string &file_path )
	{
		const char *argv[] = { "", file_path.c_str() };
		return Process( 2, argv );
	}

	bool Process( const QStringList &args );

protected:
	bool GetCommandLineOptions( const std::vector< std::string > &args );
	virtual void BuildPlotsPrivate() override;

	template< class PROPS_CONTAINER >
	void GetPlotPropertyParams4all( size_t nFields, PROPS_CONTAINER &data );

	void GetPlotPropertyCommonParams( size_t nFields, CFieldData &data );
	void GetXYPlotPropertyParams( size_t nFields );
	void GetWPlotPropertyParams( size_t nFields );
	void GetZFXYPlotPropertyParams( size_t nFields );

	bool BuildPlotsFromNetcdf();
	bool GetParametersNetcdfZFLatLon( CExternalFilesNetCDF* externalFile );
	bool GetParametersNetcdfZFXY( CExternalFilesNetCDF* externalFile );
	bool GetParametersNetcdfYFX( CExternalFilesNetCDF* externalFile );
};



#endif // DATA_MODELS_DISPLAY_CMD_LINE_PROCESSOR_H
