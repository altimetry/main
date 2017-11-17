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

#include "DataModels/Model.h"
#include "DataModels/Filters/BratFilters.h"
#include "DataModels/Workspaces/ProductInfo.h"
#include "DataModels/Workspaces/Dataset.h"
#include "DataModels/Workspaces/RadsDataset.h"
#include "DataModels/PlotData/MapColor.h"

#include "GUI/ProgressDialog.h"
#include "GUI/DisplayWidgets/MapWidget.h"

#include "BratApplication.h"

#include "DesktopControlPanel.h"



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//					Control Panel Specialized for Desktop
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////


//static 
CDesktopControlsPanel::CPendingTracks *CDesktopControlsPanel::smPendingTrack = nullptr;

//static 
bool CDesktopControlsPanel::smAutoSatelliteTrack = true;

//static 
const CDataset *CDesktopControlsPanel::smDataset = nullptr;

//static 
bool CDesktopControlsPanel::smSomeTrackDisplayed = false;

//static 
int CDesktopControlsPanel::smTotalRecords = -1;



//explicit 
CDesktopControlsPanel::CDesktopControlsPanel( CBratApplication &app, CDesktopManagerBase *manager, QWidget *parent, Qt::WindowFlags f )	//parent = nullptr, Qt::WindowFlags f = 0 
	: base_t( parent, f )
	, mApp( app )
	, mModel( app.DataModel() )
	, mDesktopManager( manager )
	, mMap( mDesktopManager->Map() )
{}


//virtual 
CDesktopControlsPanel::~CDesktopControlsPanel()
{
	delete smPendingTrack;
	smPendingTrack = nullptr;
}

//virtual 
void CDesktopControlsPanel::ChangePanelSelection( bool selected )
{
	mSelectedPanel = selected;
	UpdatePanelSelectionChange();
}


//virtual 
void CDesktopControlsPanel::WorkspaceChanged()	//= 0;
{
	smDataset = nullptr;
}




//////////////////////////////////////////////////////////////////////////////////////////////
//							Satellite Tracks Processing
//////////////////////////////////////////////////////////////////////////////////////////////


void CDesktopControlsPanel::SelectAreaInMap( double lonm, double lonM, double latm, double latM )
{
	auto not_empty_area = [&lonm, &lonM, &latm, &latM]() -> bool
	{
		return lonm || lonM || latm || latM;
	};

	// Avoid QGIS warnings
	//
	// When mMap is	still hidden because the application is starting,
	//	consider it as being shown, otherwise when/if mMap is displayed 
	//	the area will be missing; conveniently, in this case any warnings 
	//	are not displayed, with one exception: when the splash terminates
	//	and the main window is not yet visible. In this case, empty areas 
	//	cause geos exceptions and in turn QGIS warnings.
	//
	if ( not_empty_area() )
	{
		if ( mMap->isVisible() || mApp.SplashAvailable() )
		{
			mMap->SelectArea( lonm, lonM, latm, latM );
		}
	}
	else
	{
		RemoveAreaSelectionFromMap();
	}
}


void CDesktopControlsPanel::RemoveAreaSelectionFromMap()
{
	if ( mMap->isVisible() )
	{
		mMap->RemoveAreaSelection();
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////
//							Satellite Tracks Processing
//////////////////////////////////////////////////////////////////////////////////////////////


enum AliasIndex
{
	eAliasIndexLon,
	eAliasIndexLat,
	eAliasIndexTime,
};


// Here is an expression using the  alias 'swh': %{sig_wave_height} -->  'swh' field in tha aliases dictionary (Jason-1)
//Expressions[1]	= "%{sig_wave_height}";
//Units[1]		= "count";
//
// Here is an expression calling the 'BratAlgoFilterMedianAtp' algorithm
//Expressions[1] = "exec(\"BratAlgoFilterMedianAtp\", %{sig_wave_height}, 7, 1, 0)";
//Units[1]		= "count";
//
int ReadTrack( const std::vector< unsigned char > &can_use_alias, 
	const std::string &path, const std::string &record, double *&x, size_t &sizex, double *&y, size_t &sizey, double *&z, size_t &sizez, int nfields = -1 )
{
	static const int default_nfields = 3;
	static const char *Units[ default_nfields ] = { "count", "count", "second" };

	const char *Expressions[] =
	{
		can_use_alias[eAliasIndexLon]	? lon_alias().c_str()	: lon_name().c_str(),
		can_use_alias[eAliasIndexLat]	? lat_alias().c_str()	: lat_name().c_str(),
		can_use_alias[eAliasIndexTime]	? time_alias().c_str()	: time_name().c_str()
	};
	char *Names[] = { const_cast<char*>( path.c_str() ) };

	double	*Data[ default_nfields ]	= { nullptr, nullptr, nullptr };
	int32_t	Sizes[ default_nfields ]	= { -1, -1, -1 };

	size_t	ActualSize;
	if ( nfields < 0 )
		nfields = default_nfields;


	int ReturnCode = CProduct::ReadData(
		1, Names,
		record.c_str(),		//"data",
		nullptr,			//"latitude > 20 && latitude < 30",
		nfields,
		const_cast< char** >( Expressions ),
		const_cast< char** >( Units ),
		Data,
		Sizes,
		&ActualSize,
		1,
		0,
		defaultValue< double >()
	);

	LOG_TRACEstd( "Return code          : " + n2s<std::string>( ReturnCode ) );
	LOG_TRACEstd( "Actual number of data: " + n2s<std::string>( ActualSize ) );

	x = Data[ 0 ];
	y = Data[ 1 ];
	z = Data[ 2 ];

	sizez = sizex = sizey = ActualSize;

	return ReturnCode;
}


void CDesktopControlsPanel::UpdateMapTitle()
{
	if ( smSomeTrackDisplayed && smDataset )
		mMap->setWindowTitle( t2q( smDataset->GetName() ) );
}


void CDesktopControlsPanel::ProcessPendingTracks()
{
	if ( smPendingTrack )
	{
		CPendingTracks tmp = *smPendingTrack;
		delete smPendingTrack;
		smPendingTrack = nullptr;
		DrawDatasetTracks( tmp.mDataset, tmp.mForceRedraw );
	}
}

bool CDesktopControlsPanel::DrawDatasetTracks( const CDataset *dataset, bool force_redraw )
{
	static CMapColor &mc = CMapColor::GetInstance();        Q_UNUSED(mc);
	static const std::string unknown( "???" );
	static bool processing = false;

	//if ( dataset )
	//	LOG_WARN( "================================>" + dataset->GetName() );
	//else
	//	LOG_WARN( "NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL NULL" );

	if ( processing )
	{
		//LOG_FATAL( "reentering reentering reentering reentering reentering reentering reentering reentering reentering " );

		if ( smPendingTrack )
		{
			// New dataset, not from timer
			//	...so, a timer is active

			*smPendingTrack = { dataset, force_redraw };
		}
		else
		{
			// Either new dataset or from timer
			//	...so, a timer is not active

			smPendingTrack = new CPendingTracks{ dataset, force_redraw };
			QTimer::singleShot( 1500, this,	[this]()
			{
				ProcessPendingTracks();
			}
			);
		}

		return false;
	}

	// Delete any saved dataset waiting for ProcessPendingTracks
	//
	delete smPendingTrack;
	smPendingTrack = nullptr;


	//lambdas

	auto debug_log = []( const std::string &msg )
	{
		LOG_TRACEstd( msg  );
		qApp->processEvents();
	};


	//function body

	boolean_locker_t l ( processing );


	if ( dynamic_cast< const CRadsDataset* >( dataset ) )
		dataset = nullptr;

	const bool skip_drawing = smDataset == dataset && !force_redraw;
	if ( !skip_drawing )
	{
		mMap->RemoveTracksLayerFeatures( true );
		mMap->setWindowTitle( "" );
	}

	smDataset = dataset;
	smSomeTrackDisplayed = false;

	if ( !smAutoSatelliteTrack || !smDataset || skip_drawing )
	{
		return false;
	}

	std::string error_msg;

	mMap->setRenderFlag( false );							//kills render jobs
	std::vector< std::string > paths = smDataset->GetFiles();

	CProgressInterface *pi = nullptr;
	if ( paths.size() > mModel.Settings().MinimumFilesToProgressTrack() )
	{
		CProgressDialog *progress_dlg = new CProgressDialog( "Reading Track Data...", "Cancel", 0, paths.size(), parentWidget() );
		progress_dlg->setAttribute( Qt::WA_DeleteOnClose );
		pi = progress_dlg;
		//progress_dlg->show();
	}

	for ( auto &path : paths )
	{
		if ( smPendingTrack )
			break;

		WaitCursor wait;

		if ( pi )
		{
			wait.Restore();
			if ( pi->Cancelled() )
			{
				break;
			}
			pi->SetCurrentValue( pi->CurrentValue() + 1 );
		}

		bool is_netcdf = false, skip_iteration = false;
		std::vector< unsigned char > alias_used = { false, false, false };

		std::string record, info;

		auto ref_date = REF19500101;	//Use ReadData ref date instead of product->GetRefDate();

		long expected_lon_dim = 0, expected_lat_dim = 0; auto expected_time_dim = 0;                Q_UNUSED( expected_time_dim );

		{
			CProductInfo product_info( smDataset, path );
			if ( !product_info.IsValid() )
			{
				error_msg += ( "\n\n" + path + " caused error: " + product_info.ErrorMessages() );
				continue;
			}

			//mTotalRecords += product_info->GetNumberOfRecords();
			skip_iteration = !product_info.HasAliases();
			if ( skip_iteration )
			{
				LOG_WARN( "Aliases not supported: reading file " + path );
			}
			is_netcdf = product_info.IsNetCdfOrNetCdfCF();

			record = product_info.Record();

			std::string lonlat_error_msg, time_error_msg;
			std::pair<CAliasInfo, CAliasInfo> lon_lat_alias_info = product_info.FindLonLatFields( mModel.Settings().UseUnsupportedFields(), 
				(bool&)alias_used[ eAliasIndexLon ], (bool&)alias_used[ eAliasIndexLat ], lonlat_error_msg );
			CAliasInfo time_alias_info = product_info.FindTimeField( mModel.Settings().UseUnsupportedFields(), (bool&)alias_used[ eAliasIndexTime ], time_error_msg );
			if ( lon_lat_alias_info.first.Empty() || lon_lat_alias_info.second.Empty() || time_alias_info.Empty() )
			{
				skip_iteration = true;
				LOG_WARN( "File - " + path + "\n" + replace( lonlat_error_msg, "\n", " - " ) + " - " + time_error_msg );
			}

			CField *lon = lon_lat_alias_info.first.Field();
			CField *lat = lon_lat_alias_info.second.Field();
			CField *time = time_alias_info.Field();

			expected_lon_dim = lon ? lon->GetDim()[ 0 ] : 0;
			expected_lat_dim = lat ? lat->GetDim()[ 0 ] : 0;	expected_time_dim = time ? time->GetDim()[ 0 ] : 0;
			if ( expected_lon_dim != expected_lat_dim )
			{
				skip_iteration = true;
				LOG_WARN( "Different latitude/longitude dimensions in file " + path );
			}

			const std::string &lon_value = lon_lat_alias_info.first.Empty() ? unknown : lon_lat_alias_info.first.Value();
			const std::string &lat_value = lon_lat_alias_info.second.Empty() ? unknown : lon_lat_alias_info.second.Value();
			const std::string &time_value = time_alias_info.Empty() ? unknown : time_alias_info.Value();

			info =
				( alias_used[ eAliasIndexLon ] ?	lon_alias() : lon_name() ) +	"==" + lon_value + " " +
				( alias_used[ eAliasIndexLat ] ?	lat_alias() : lat_name() ) +	"==" + lat_value + " " +
				( alias_used[ eAliasIndexTime ] ?	time_alias() : time_name() ) +	"==" + time_value;

		}	//delete product;


		if ( skip_iteration )
		{
			LOG_WARN( info );
			continue;
		}

		LOG_INFO( info );

		size_t lon_dim = 0;
		size_t lat_dim = 0;
		size_t time_dim = 0;
		double *lonv = nullptr;
		double *latv = nullptr;
		double *timev = nullptr;

		debug_log( "About to read variables from file " + path );

		int ReturnCode = ReadTrack( alias_used, path, record, lonv, lon_dim, latv, lat_dim, timev, time_dim );
		if ( ReturnCode == BRATHL_SUCCESS )
		{
			assert__( lon_dim == lat_dim );
			assert__( !is_netcdf || ( lon_dim == expected_lon_dim && lat_dim == expected_lat_dim ) );

			smSomeTrackDisplayed = true;

			debug_log( "Normalizing longitudes..." );

			for ( size_t i = 0; i < lon_dim; ++i )
				lonv[ i ] = CTools::NormalizeLongitude( -180.0, lonv[ i ] );

			debug_log( "About to plot..." );

			mMap->PlotTrack( lonv, latv, timev, lon_dim, ref_date, Qt::red );	// <QColor>( mc.NextPrimaryColors() ) );

			debug_log( "Finished plotting..." );
		}
		else
			error_msg += ( "\n\nError reading " + path + ".\nReturn code: " + n2s<std::string>( ReturnCode ) );

		free( lonv );
		free( latv );
	}

	UpdateMapTitle();
	mMap->setRenderFlag( true );

	if ( !error_msg.empty() )
	{
		LOG_WARN( smDataset ?
			( "Problems reading satellite tracks from " + smDataset->GetName() + ":\n" + error_msg )
			: error_msg );
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_DesktopControlPanel.cpp"
