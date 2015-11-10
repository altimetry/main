/*
* 
*
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
#if !defined(_Mission_h_)
#define _Mission_h_

#include "brathl_error.h" 
#include "brathl.h" 


#include "List.h" 
#include "Date.h" 
using namespace brathl;

namespace brathl
{

/** \addtogroup date_conversionCPP
  @{ */


/** 
  Satellite cycle/date conversion class.

 A class to convert a date in a satellite cycle and pass number, or vice versa
 
 \version 1.0
*/


class CMission 
{

public: 
  /** Constructs a CMission object 
    \param mission [in] : mission type (see #brathl_mission)
    \param printWarnings [in] : set to true for printing warnings on standard output, false otherwise. Default value is true.

  */
  CMission(brathl_mission mission, bool printWarnings = true); 
 
  /** Constructs a CMission object 
    \param mission [in] : mission type (see #brathl_mission)
    \param repeat [in] : duration that takes the satellite to return at the same point
    \param dateRef [in] : date reference in decimal julian day
    \param cycleRef [in] : cycle reference
    \param passRef [in] : pass reference
    \param nbPass [in] : numbers of half passes in a cycle
    \param printWarnings [in] : set to true for printing warnings on standard output, false otherwise. Default value is true.
  */
  CMission(brathl_mission mission, const double repeat, const CDate& dateRef, const uint32_t cycleRef, 
      	   const uint32_t passRef,  const uint32_t nbPass, bool printWarnings = true); 
  
  virtual ~CMission();

public:
  
  /** Gets the mission (see #brathl_mission)*/
  brathl_mission GetMission() {return m_mission;};
  /** Gets the repeat attributes (see #m_repeat)*/
  double GetRepeat() {return m_repeat;};
  /** Gets the date reference attributes (see #m_dateRef)*/
  const CDate& GetDateRef() {return m_dateRef;};
  /** Gets the cycle reference attributes (see #m_cycleRef)*/
  uint32_t GetCycleRef() {return m_cycleRef;};
  /** Gets the pass reference attributes (see #m_passRef)*/
  uint32_t GetPassRef() {return m_passRef;};
  /** Gets the number of passes attributes (see #m_nbPass)*/
  uint32_t GetNbPass() {return m_nbPass;};

  /** Gets the name of the mission */
  const char* const GetName();

  /** Tests if the mission is valid 
    \return #BRATHL_SUCCESS or error code (see \ref Cycle_date_error_codes) */
  int32_t CtrlMission(); 

  /** Converts a CDate object into acycle/pass
    \param date [in] : date to convert 
    \param cycle [out] : number of cycle
    \param pass [out] : number of pass in the cycle 
    \return #BRATHL_SUCCESS or error code (see \ref Cycle_date_error_codes) */
  int32_t Convert(CDate& date, uint32_t& cycle, uint32_t& pass);
  /** Converts a cyle/pass into a CDate object 
    \param cycle [in] : number of cycle to convert
    \param pass [in] : number of pass in the cycle  to cinvert
    \param date [out] : date corresponding to the cycle/pass 
    \return #BRATHL_SUCCESS or error code (see \ref Cycle_date_error_codes) */
  int32_t Convert(uint32_t cycle, uint32_t pass, CDate& date);


  /** Gets aliases names for the mission
    \param aliases [out] : aliases for the mission
    \return #BRATHL_SUCCESS or error code (see \ref Cycle_date_error_codes) */
  int32_t LoadAliasName(CStringList& aliases);

  static double GetGlobalConstant(brathl_global_constants constantValue);

  /** Assigns a new value to the CMission object, with a CMission object */
  const CMission& operator= (const CMission& m);

   ///Dump fonction
   virtual void Dump(std::ostream& fOut = std::cerr);


private:
	
  /** Initializes default class attribute values according to the mission */
  void InitDefault();
  /** initializes default class attribute values according to the mission and its largest date reference
      	  with data contained in the #m_refFileName */
  int32_t SearchNearestDateRef();


public:
    
  /** Name of the Topex/Poseidon mission */
  static const char* m_nameTP;
  /** Name of the Jason-1 mission */
  static const char* m_nameJ1;
  /** Name of the Jason-2 mission */
  static const char* m_nameJ2;
  /** Name of the ERS2 mission */
  static const char* m_nameE2;
  /** Name of the ENVISAT mission */
  static const char* m_nameEN;
  /** Name of the ERS1-A mission */
  static const char* m_nameE_C;
  /** Name of the ERS1-B mission */
  static const char* m_nameE_G;
  /** Name of the GFO mission */
  static const char* m_nameG2;
  /** Name of an unknown mission */
  static const char* m_nameUnknown;
  /** Name of the mission reference file
 
    An ascii file with records :
     field 1 : Name of the mission
     field 2 : cycle reference
     field 3 : pass reference
     field 4 : date reference in decimal julian day

    Each field has to be separated by at least  a non-numeric character

    The file can contained several record for a same mission. 
    Only the field with the greatest date is taken into account 
    */
  static const char* m_refFileName;

  /** Name of the mission aliases file
 
    An ascii file with records :
     field 1 : Name of the mission
     field 2 : Alias of the mission

    Each field has to be separated by at least  a non-numeric character

    The file can contained several record for a same mission. 
    */
  static const char* m_refAliasName;

  /** Max length of the name of the mission */
  static const int m_maxLenName;

  
  
  
  
 
private:
 
  /** Satellite (mission) enumeration - see #brathl_mission */
  brathl_mission m_mission;

  /** duration that takes the satellite to return at the same point */
  double m_repeat;
  /** date reference */
  CDate m_dateRef;
  /** cycle reference */
  uint32_t m_cycleRef;
  /** pass reference */
  uint32_t m_passRef;
  /** numbers of half passes in a cycle */
  uint32_t m_nbPass;

  /** set to true for printing warnings on standard output */
  bool m_printWarnings;

};                      

/** @} */
}
#endif // !defined(_Mission_h_)
