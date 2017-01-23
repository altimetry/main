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
#if !defined(LIBBRATHL_CYCLE_PASS_CONVERTER_H)
#define LIBBRATHL_CYCLE_PASS_CONVERTER_H

#include "common/tools/brathl_error.h"
#include "brathl.h" 


#include "List.h" 
#include "Date.h" 



namespace brathl
{

	class CCyclePassConverter
	{
	public:
		// types 


		// brathl legacy API
		//
		enum ECyclePassMission
		{
			eTOPEX,
			eJASON2,
			eJASON1,
			eERS2,
			eENVISAT,
			eERS1_A,
			eERS1_B,
			eGFO,

			ECyclePassMission_size
		};


#if defined(PRE_CPP11)
		friend struct cyclepass_converter_pred_t;
#endif

	private:

		// static members

		static std::vector< CCyclePassConverter > smCyclePassConverters;
		static std::string smErrorMessages;
		static int smError;
		static const size_t smFieldsCount;	//number of the instance data members read from file

		static bool LoadCyclePassParameters();


	public:

		static const CCyclePassConverter& GetConverter( const std::string &product_reference );


		// Legacy: for brathl API
		//
		static const CCyclePassConverter& GetConverter( ECyclePassMission m );


		// instance data
		//
		//	- see smFieldsCount 

	private:

		// Name of the mission/product reference
		//
		std::string m_missionName;

		// duration that takes the satellite to return at the same point */
		//
		double m_repeat;

		// date reference
		//
		CDate m_dateRef;

		// cycle reference
		//
		uint32_t m_cycleRef;

		// pass reference
		//
		uint32_t m_passRef;

		// numbers of half passes in a cycle
		//
		uint32_t m_nbPass;



		int mErrorCode;


		// construction / destruction

	public:
		// Always creates an invalid instance
		// - public only because required by the compiler
		//
		explicit CCyclePassConverter();


		CCyclePassConverter& operator=( const CCyclePassConverter &o );


		virtual ~CCyclePassConverter()
		{}


		// error handling

		// Checks if the mission is valid (no errors loading file and class/type reference passed valid)
		// return #BRATHL_SUCCESS or error code (see \ref Cycle_date_error_codes) */
		//
		int ErrorCode() const 
		{ 
			return mErrorCode; 
		}


		const std::string& ErrorMessages() const
		{
			static const std::string instance_error_message = "Cycle/pass conversion references not found for the given product identification.";

			if ( smErrorMessages.empty() )
				return instance_error_message;

			return smErrorMessages;
		}


		// computation

		// Converts a CDate object into a cycle/pass
		// date [in] : date to convert
		// cycle [out] : number of cycle
		// pass [out] : number of pass in the cycle
		// return #BRATHL_SUCCESS or error code (see \ref Cycle_date_error_codes)
		//
		int Convert( const CDate& date, int32_t& cycle, int32_t& pass ) const;


		// Converts a cyle/pass into a CDate object 
		//  cycle [in] : number of cycle to convert
		//  pass [in] : number of pass in the cycle  to convert
		//  date [out] : date corresponding to the cycle/pass 
		//  return #BRATHL_SUCCESS or error code (see \ref Cycle_date_error_codes)
		//
		int Convert( const int32_t cycle, const int32_t pass, CDate& date ) const;

	};


}
#endif // !defined(LIBBRATHL_CYCLE_PASS_CONVERTER_H)
