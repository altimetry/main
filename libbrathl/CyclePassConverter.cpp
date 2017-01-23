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

#include <sys/stat.h>
#include <cstdio>
#include <cstring> 
#include <string>

#include "common/+UtilsIO.h"
#include "common/tools/brathl_error.h"
#include "common/tools/TraceLog.h"
#include "common/tools/Exception.h"

#include "brathl.h" 
#include "Tools.h" 
#include "File.h" 
#include "CyclePassConverter.h" 


using namespace brathl;


namespace brathl
{
	////////////////////////////////////////////////
	//	Cycle/Pass Converter Static Members
	////////////////////////////////////////////////

	//static 
	std::vector< CCyclePassConverter > CCyclePassConverter::smCyclePassConverters;
	//static 
	int CCyclePassConverter::smError = BRATHL_SUCCESS;
	//static 
	std::string CCyclePassConverter::smErrorMessages;
	//static 
	const size_t CCyclePassConverter::smFieldsCount = 6;	//number of the instance data members read from file



	//static 
	bool CCyclePassConverter::LoadCyclePassParameters()
	{
		static const std::string refFileName = "brathl_refmission.txt";
		static const std::string refFilePathName = CTools::FindDataFile( refFileName );
		static bool initialized = false;

		if ( initialized )
			return smError == BRATHL_SUCCESS;

		initialized = true;
		smError = BRATHL_SUCCESS;

		std::string file_content;
		if ( !Read2String( file_content, refFilePathName ) )
		{
			smErrorMessages = "Unable to open file '" + refFileName + "'\nCheck directory '" + CTools::GetInternalDataDir();
			smError = BRATHL_WARNING_OPEN_FILE_REF_FILE;
			return false;
		}

		std::vector< std::string > vfile_content, vdirty_vfile_content = String2Vector( file_content );
		for ( std::vector< std::string >::const_iterator it = vdirty_vfile_content.begin(); it != vdirty_vfile_content.end(); ++it )
		{
			std::string s = *it;
			if ( !trim( s ).empty() && s[ 0 ] != '#' )
				vfile_content.push_back( s );
		}

		int nline = 0;
		for ( std::vector< std::string >::const_iterator it = vfile_content.begin(); it != vfile_content.end(); ++it )
		{
			auto const &entry = *it;
			std::vector< std::string > ventry, vdirty_entry = String2Vector( entry, std::string( " " ) );
			for ( std::vector< std::string >::const_iterator it = vdirty_entry.begin(); it != vdirty_entry.end(); ++it )
			{
				std::string s = *it;
				if ( !trim( s ).empty() )
					ventry.push_back( s );
			}

			nline++;
			if ( ventry.size() != smFieldsCount )
			{
				smErrorMessages += (
					"\nInvalid reference mission cycle/pass format - line: " + n2s< std::string >( nline )
					+ " of file: " + refFilePathName
					);

				// (*) Preserve 1st error code found, but do not abort, maybe some work 
				//	can still be done. Reporting the specific error is not so important,
				//	the whole file should be correct.
				//
				if ( smError == BRATHL_SUCCESS )
					smError = BRATHL_WARNING_INVALID_REF_FILE_FIELD;
				continue;
			}

			smCyclePassConverters.resize( smCyclePassConverters.size() + 1 );
			CCyclePassConverter &cc = smCyclePassConverters.back();

			size_t index = 0;
			cc.m_missionName	= ventry[ index++ ];
			cc.m_cycleRef		= s2n<decltype( cc.m_cycleRef )>( ventry[ index++ ] );
			cc.m_passRef		= s2n<decltype( cc.m_passRef )>( ventry[ index++ ] );
			double julian		= s2n<decltype( julian )>( ventry[ index++ ] );
			cc.m_repeat			= s2n<decltype( cc.m_repeat )>( ventry[ index++ ] );
			cc.m_nbPass			= s2n<decltype( cc.m_nbPass )>( ventry[ index++ ] );
			cc.mErrorCode		= cc.m_dateRef.SetDateJulian( julian );

			if ( cc.mErrorCode != BRATHL_SUCCESS )
			{
				smErrorMessages += (
					"\nInvalid reference date - file name " + refFilePathName
					+ " \nmission " + cc.m_missionName
					+ ", cycle " + n2s< std::string >( cc.m_cycleRef )
					+ ", pass "	 + n2s< std::string >( cc.m_passRef )
					+ ", Julian date " + n2s< std::string >( julian )
					+ ", repeat " + n2s< std::string >( cc.m_repeat )
					+ ", # passes " + n2s< std::string >( cc.m_nbPass )
					);

				// see comment (*) above
				if ( smError == BRATHL_SUCCESS )
					smError = BRATHL_WARNING_INVALID_REF_FILE_FIELDDATE;

				smCyclePassConverters.erase( smCyclePassConverters.end() - 1 );
				continue;
			}

			assert__( cc.m_missionName.length() > 0 );
		}

		return smError == BRATHL_SUCCESS;
	}



#if defined(PRE_CPP11)
	struct cyclepass_converter_pred_t
	{
		std::string lower_product_reference;

		bool operator()( const CCyclePassConverter &cc ) const
		{
			const std::string lower_name = ToLowerCopy( cc.m_missionName );
			const std::string lower_product_reference_prefix = lower_product_reference.substr( 0, lower_name.size() );

			return lower_name == lower_product_reference_prefix;
		}
	};
#endif


	//static
	const CCyclePassConverter& CCyclePassConverter::GetConverter( const std::string &product_reference )
	{
		// An ASCII file with records :
		// field 1 : Name of the mission
		// field 2 : cycle reference
		// field 3 : pass reference
		// field 4 : date reference in decimal Julian day
		// field 5 : repeat period in decimal number of days
		// field 6 : number of passes in one repeat cycle
		//
		//Each field has to be separated by at least  a non-numeric character

		static const bool all_loaded = LoadCyclePassParameters();
		static const CCyclePassConverter invalid_converter;

		if ( all_loaded )
		{
			const std::string lower_product_reference = ToLowerCopy( product_reference );

#if defined(PRE_CPP11)
			cyclepass_converter_pred_t pred;
			pred.lower_product_reference = lower_product_reference;
			std::vector< CCyclePassConverter >::const_iterator icc =
			        std::find_if( smCyclePassConverters.cbegin(), smCyclePassConverters.cend(), pred );
#else
			auto const &icc = std::find_if( smCyclePassConverters.cbegin(), smCyclePassConverters.cend(),

			    [&lower_product_reference]( const CCyclePassConverter &cc )
				{ 
					const std::string lower_name = ToLowerCopy( cc.m_missionName );
					const std::string lower_product_reference_prefix = lower_product_reference.substr( 0, lower_name.size() );

					return lower_name == lower_product_reference_prefix; 
				}
			);
#endif
			if ( icc != smCyclePassConverters.end() )
				return *icc;
		}

		return invalid_converter;
	}


	// Legacy: for brathl API
	//
	//static 
	const CCyclePassConverter& CCyclePassConverter::GetConverter( ECyclePassMission m )
	{
		if ( (size_t)m < smCyclePassConverters.size() )
			return smCyclePassConverters[ m ];

		return GetConverter( "" );
	}




	////////////////////////////////////////////////
	//	Cycle/Pass Converter Instance Functions
	////////////////////////////////////////////////


	// Vector erase needs this
	//
	CCyclePassConverter& CCyclePassConverter::operator=( const CCyclePassConverter &o )
	{
		if ( this != &o )
		{
			m_missionName = o.m_missionName;
			m_repeat      = o.m_repeat;
			m_dateRef     = o.m_dateRef;
			m_cycleRef    = o.m_cycleRef;
			m_passRef     = o.m_passRef;
			m_nbPass      = o.m_nbPass;
			mErrorCode = o.mErrorCode;
		}
		return *this;
	}


	CCyclePassConverter::CCyclePassConverter()
	{
		mErrorCode = smError == BRATHL_SUCCESS ? BRATHL_ERROR_INVALID_MISSION : smError;
		//m_missionName;
		m_repeat = 0.0;
		m_dateRef.SetDateJulian( 0.0 );
		m_cycleRef = 0;
		m_passRef = 0;
		m_nbPass = 0;
	}



	int32_t CCyclePassConverter::Convert( const CDate &date, int32_t &cycle, int32_t &pass ) const
	{
		if ( smError != BRATHL_SUCCESS )
			return smError;

		double dateJulian = 0.0;
		double dateJulianRef = 0.0;

		int32_t result = BRATHL_SUCCESS;

		if ( m_nbPass == 0 )
		{
			return BRATHL_ERROR_INVALID_NB_PASS;
		}

		if ( m_repeat == 0 )
		{
			return BRATHL_ERROR_INVALID_REPETITION;
		}
		/*
		  if ((cycle == NULL) || (size <= 0))
		  {
			return BRATHL_ERROR_INVALID_CTODATE_PARAMETER;
		  }

		*/
		date.Convert2DecimalJulian( dateJulian );
		m_dateRef.Convert2DecimalJulian( dateJulianRef );

		double absRef = m_passRef + ( m_cycleRef - 1 ) * m_nbPass + 0.5;
		double passLength = m_repeat / m_nbPass;

		double deltaPass = ( dateJulian - dateJulianRef ) / passLength;

		double abs = absRef + deltaPass - 1;

		cycle = 1 + static_cast<int32_t>( ( abs ) / m_nbPass );
		pass = 1 + static_cast<int32_t>( ( abs ) ) % m_nbPass;


		return result;
	}



	int32_t CCyclePassConverter::Convert( const int32_t cycle, const int32_t pass, CDate& date ) const
	{
		if ( smError != BRATHL_SUCCESS )
			return smError;

		int32_t result = BRATHL_SUCCESS;

		double dateJulian = 0.0;
		double dateJulianRef = 0.0;

		m_dateRef.Convert2DecimalJulian( dateJulianRef );
		double absRef = m_passRef + ( m_cycleRef - 1 ) * m_nbPass;
		double abs = pass + ( cycle - 1 ) *  m_nbPass;
		double passLength = m_repeat / m_nbPass;

		dateJulian = dateJulianRef + ( abs - absRef ) * passLength;
		date.SetDateJulian( dateJulian );

		return result;

	}
}
