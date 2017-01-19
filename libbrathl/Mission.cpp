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

#include "common/tools/brathl_error.h"
#include "brathl.h" 

#include "common/tools/TraceLog.h"
#include "Tools.h" 
#include "common/tools/Exception.h"
#include "File.h" 
#include "Mission.h" 
using namespace brathl;

namespace brathl
{

//const char* CMission::m_nameTP = "Topex/Poseidon";
//const char* CMission::m_nameJ1 = "Jason-1";
//const char* CMission::m_nameJ2 = CExternalFilesJason2::smMissionName;
//const char* CMission::m_nameE2 = "ERS2";
//const char* CMission::m_nameEN = "ENVISAT";
//const char* CMission::m_nameE_C = "ERS1-A";
//const char* CMission::m_nameE_G = "ERS1-B";
//const char* CMission::m_nameG2 = "GFO";
const char* CMission::m_nameUnknown = "Unknown mission";



const char* CMission::m_refFileName = "brathl_refmission.txt";

const int CMission::m_maxLenName = 30;



//ctor		
CMission::CMission(brathl_mission mission, bool printWarnings /* = true */)  
{
    m_errno = BRATHL_SUCCESS;
    m_mission = mission;
    m_printWarnings = printWarnings;
  
    // Initializing parameters
    m_missionName = CMission::m_nameUnknown;
    m_repeat = 0.0;
    m_dateRef.SetDateJulian( 0.0 );
    m_cycleRef = 0;
    m_passRef = 0;
    m_nbPass = 0;

    // Loading mission conversion parameters from file m_refFileName
    m_errno = LoadRefMission();  // (search by brathl_mission ID -> "m_mission")

    if ( m_errno != BRATHL_SUCCESS )
        printf("\nERROR - Invalid mission value : %d \n",  mission);
}


CMission::CMission( const std::string &product_type, bool printWarnings /* = true */ )
{
	m_errno = BRATHL_SUCCESS;
    m_missionName = product_type; // assigning product label as mission name (temporary)
    m_printWarnings = printWarnings;

    // Initializing parameters
    m_mission = 0;
    m_repeat = 0.0;
    m_dateRef.SetDateJulian( 0.0 );
    m_cycleRef = 0;
    m_passRef = 0;
    m_nbPass = 0;

    // Loading mission conversion parameters from file m_refFileName
    m_errno = LoadRefMission( false );  // (search by product label or mission name -> "m_missionName")

    if ( m_errno != BRATHL_SUCCESS )
        printf("\nERROR - Invalid mission name or product label : %s \n",  product_type.c_str());
}


//----------------------------------------
//dtor
CMission::~CMission()  
{
  
}

const CMission& CMission::operator =(const CMission& m)
{
    m_mission     = m.m_mission;
    m_missionName = m.m_missionName;
    m_repeat      = m.m_repeat;
    m_dateRef     = m.m_dateRef;
    m_cycleRef    = m.m_cycleRef;
    m_passRef     = m.m_passRef;
    m_nbPass      = m.m_nbPass;
    m_printWarnings = m.m_printWarnings;
    m_errno = m.m_errno;
    
    return *this;
}

//----------------------------------------
int32_t CMission::LoadRefMission(bool searchByID /* = true */ )
{
    int32_t result = BRATHL_SUCCESS;
  
    const uint32_t MAX_LINE_LEN = 255;
    char line[MAX_LINE_LEN+1];
  
    brathl_mission id = 0;
    char name[CMission::m_maxLenName+1];
    int32_t cycle  = 0;
    int32_t pass   = 0;
    double julian  = 0.0;
    double repeat  = 0.0;
    int32_t nbPass = 0;
    bool bFoundInFile = false;
  
    CMission missionSave = *this;
  
    std::string refFilePathName = CTools::FindDataFile( CMission::m_refFileName );
    if (refFilePathName == "")
    {
        if (m_printWarnings)
        {
            printf( "\nWARNING - CMission::LoadRefMission - Unable to open file '%s' "
                    "\nCheck directory '%s'\n",
                    CMission::m_refFileName,
                    CTools::GetInternalDataDir().c_str());
        }
        return BRATHL_WARNING_OPEN_FILE_REF_FILE;
    }

    // reads file contains conversion parameters
    CFile fileRef(refFilePathName, CFile::modeRead);

    if (fileRef.IsOpen() == false)
    {
        if (m_printWarnings)
        {
            printf( "\nWARNING - CMission::LoadRefMission - Unable to open file '%s'\n", refFilePathName.c_str() );
        }
        return BRATHL_WARNING_OPEN_FILE_REF_FILE;
    }
  
    int32_t nbFields = EOF;
    int32_t lineNb = 0;
    int32_t size = fileRef.ReadLineData(line, MAX_LINE_LEN);
  
    while (size > 0)
    {
        ++lineNb;
        nbFields = sscanf ( line, "%d %s %d %d %lf %lf %d",
                            &id, name, &cycle, &pass, &julian, &repeat, &nbPass );
    
        if ( nbFields < 7 )
        {
            if (m_printWarnings)
            {
                printf( "\nWARNING - CMission::LoadRefMission - Invalid reference mission file format - line: %d of file: %s \n",
                        lineNb,
                        refFilePathName.c_str() );
            }
            return BRATHL_WARNING_INVALID_REF_FILE_FIELD;
        }
        /*printf("id %d, name %s, cycle %d, pass %d, julian %lf, repead %lf, nbpass %d \n", id, name, cycle, pass, julian, repeat, nbPass);*/
	
        if ( searchByID )
            bFoundInFile = ( id == GetMission() );
        else
        {
            std::string product_type ( GetName() ); // product type/label
            std::string name_read( name );          // mission name at reference file

            product_type = CTools::StringToLower(product_type).substr(0, name_read.size());
            name_read    = CTools::StringToLower(name_read);

            bFoundInFile = ( product_type == name_read );
        }

        if ( bFoundInFile )
        {
        	missionSave = *this;

            // Assign mission conversion parameters
            m_mission     = id;
            m_missionName = name;
            m_cycleRef    = cycle;
            m_passRef     = pass;
            m_repeat      = repeat;
            m_nbPass      = nbPass;
            result        = m_dateRef.SetDateJulian(julian);

            if (result != BRATHL_SUCCESS)
            {
                if (m_printWarnings)
                {
                    printf( "\nWARNING - CMission::LoadRefMission - Invalid reference date - file name %s \n"
                            "mission %s, cycle %d, pass %d, julian %lf, repeat %lf, nb passes %d\n",
                            refFilePathName.c_str(),
                            name, cycle, pass, julian, repeat, nbPass  );
                }
                *this = missionSave;
                result = BRATHL_WARNING_INVALID_REF_FILE_FIELDDATE;
            }
        }

        // Breaks while loop if mission was found
        if ( bFoundInFile ) {  break; }
        else                { size = fileRef.ReadLineData(line, MAX_LINE_LEN); } // reads next line
    }
  
    if ( bFoundInFile == false )
    {
        if ( m_printWarnings )
        {
            if (searchByID)
                printf("\nWARNING - CMission::LoadRefMission - no mission ID=%d found in file: %s \n", GetMission(), refFilePathName.c_str());
            else
                printf("\nWARNING - CMission::LoadRefMission - no mission name='%s' found in file: %s \n", GetName(), refFilePathName.c_str());
        }
        m_missionName = CMission::m_nameUnknown;
        result = BRATHL_ERROR_INVALID_MISSION;
    }
   
    fileRef.Close();
    return result;
}


//----------------------------------------
int32_t CMission::Convert( const CDate &date, uint_t &cycle, uint_t &pass ) const
{
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


int32_t CMission::Convert( const uint_t cycle, const uint_t pass, CDate& date) const
{
  int32_t result = BRATHL_SUCCESS;

  double dateJulian = 0.0;
  double dateJulianRef = 0.0;

  m_dateRef.Convert2DecimalJulian(dateJulianRef);
  double absRef = m_passRef + (m_cycleRef - 1) * m_nbPass;
  double abs = pass + (cycle -1) *  m_nbPass;
  double passLength = m_repeat / m_nbPass;
  
  dateJulian = dateJulianRef + (abs - absRef) * passLength;
  date.SetDateJulian(dateJulian);
  
  return result;

}


void CMission::Dump(std::ostream& fOut /* = std::cerr */)
{
   
   if (CTrace::IsTrace() == false)
   {
      return;
   }
   fOut << "==> Dump a CMission Object at "<< this << std::endl;

   fOut << "m_repeat = " << m_repeat << std::endl;
   m_dateRef.Dump(fOut);
   fOut << "m_cycleRef = " << m_cycleRef << std::endl;
   fOut << "m_passRef = " << m_passRef << std::endl;
   fOut << "m_nbPass = " << m_nbPass << std::endl;

   fOut << "==> END Dump a CMission Object at "<< this << std::endl;

   fOut << std::endl;

}

}
