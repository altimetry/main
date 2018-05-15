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
#if !defined DATA_MODELS_FILTERS_BRAT_FILTERS_H
#define DATA_MODELS_FILTERS_BRAT_FILTERS_H

#include <QDate>

#include "libbrathl/Product.h"
#include "libbrathl/Date.h"

#include "common/tools/CoreTypes.h"
#include "new-gui/Common/ApplicationSettings.h"

#include "BratAreas.h"


namespace brathl
{
    class CStringList;
	class CMapProduct;
}

using namespace brathl;



inline std::string MakeAlias( const std::string &aname )
{
    return "%{" + aname + "}";
}



inline const std::string& lon_name()
{
    static const std::string s = "lon";
    return s;
}
inline const std::string& lat_name()
{
    static const std::string s = "lat";
    return s;
}
inline const std::string& time_name()
{
    static const std::string s = "time";
    return s;
}


inline const std::string& lon_alias()
{
    static const std::string s = MakeAlias( lon_name() );
    return s;
}
inline const std::string& lat_alias()
{
    static const std::string s = MakeAlias( lat_name() );
    return s;
}
inline const std::string& time_alias()
{
    static const std::string s = MakeAlias( time_name() );
    return s;
}





enum EFilterVariablesNames
{
	eFilterVariableLon,
	eFilterVariableLat,
	eFilterVariableTime,

	EEFilterVariablesNames_size
};




//////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

class CBratFilter
{
	/////////////////////////////
    //			types
	/////////////////////////////

    friend class CBratFilters;


	/////////////////////////////
    //		static members
	/////////////////////////////
	
	static QDateTime smDateTimeMin;
	static QDateTime smDateTimeMax;

public:

	static inline CDate q2brat( const QDateTime &q )
	{
		if ( !q.isValid() )
			return CDate();

		return CDate( 
			q.date().year(), q.date().month(), q.date().day(), 
			q.time().hour(), q.time().minute(), q.time().second(), q.time().msec() );
	}


	static inline QDateTime brat2q( const CDate &b )
	{
		if ( b.IsDefaultValue() )
			return QDateTime();

		return QDateTime( 
			QDate( b.GetYear(), b.GetMonth(), b.GetDay() ), 
			QTime( b.GetHour(), b.GetMinute(), b.GetSecond(), b.GetMuSecond() ) );
	}



	static void SetRelativeTimesValidationRange( const QDateTime &m, const QDateTime &M );


	static QDateTime CorrectDateTime( const QDateTime &dt );


	static int DaysOverflow( const QDateTime &dt );


protected:

	static const bool smUsingRelativeTimes;
	static const bool smUsingCyclePass;

	//	NOTE mRelativeReferenceTime and mStopTime default
	//	to current time, and so have no matching static variable

    static const QDateTime smStartTime;

    static const int smStartCycle;
    static const int smStopCycle;
    static const int smStartPass;
    static const int smStopPass;

	static const bool smUseCurrentTime;
    static const int smRelativeStartDays;
    static const int smRelativeStopDays;


	/////////////////////////////
    //		instance data
	/////////////////////////////

    std::string mName;

    std::vector< std::string > mAreaNames;

	bool mUsingRelativeTimes = smUsingRelativeTimes;

	bool mUsingCyclePass = smUsingCyclePass;
    QDateTime mStartTime = smStartTime;
    QDateTime mStopTime = QDateTime::currentDateTime();

    int mStartCycle = smStartCycle;
    int mStopCycle = smStopCycle;
    int mStartPass = smStartPass;
    int mStopPass = smStopPass;

    int mRelativeStartDays = smRelativeStartDays;
    int mRelativeStopDays = smRelativeStopDays;
	bool mUseCurrentTime = smUseCurrentTime;
	QDateTime mRelativeReferenceTime = QDateTime::currentDateTime();


	/////////////////////////////
    // construction / destruction
	/////////////////////////////

public:
    CBratFilter( const std::string &name ) :
        mName( name )
    {
        SetDefaultValues();		//not necessary, but OK
    }

    CBratFilter( const CBratFilter &o )
    {
        *this = o;
    }

    CBratFilter& operator = ( const CBratFilter &o );

    virtual ~CBratFilter()
    {}


	/////////////////////////////
    //		name / areas
	/////////////////////////////

    const std::string& Name() const { return mName; }
    std::string& Name() { return mName; }

	const CBratAreas& Areas() const;


	/////////////////////////////
    //			space
	/////////////////////////////

    const std::vector< std::string >& AreaNames() const
    {
        return mAreaNames;
    }


    bool FindArea( const std::string &name ) const;

    bool AddArea( const std::string &name );

    bool RemoveArea( const std::string &name );

    void DeleteAllAreas()
    {
        mAreaNames.clear();
    }


	/////////////////////////////
    //			time
	/////////////////////////////


	//getters

	bool UsingRelativeTimes() const { return mUsingRelativeTimes; }

	//...absolute times

	bool UsingCyclePass() const { return mUsingCyclePass; }

    QDateTime StartTime() const { return mStartTime; }
    QDateTime StopTime() const { return mStopTime; }
    CDate BratStartTime() const { return q2brat( mStartTime ); }
    CDate BratStopTime() const { return q2brat( mStopTime ); }

	bool InvalidCyclePassValues() const;
	int StartCycle() const { return mStartCycle; }
    int StopCycle() const { return mStopCycle; }
    int StartPass() const { return mStartPass; }
    int StopPass() const { return mStopPass; }

	//...relative times

	int RelativeStartDays() const { return mRelativeStartDays; }
    int RelativeStopDays() const { return mRelativeStopDays; }
	bool UseCurrentTime() const { return mUseCurrentTime; }
	const QDateTime& RelativeReferenceTime() const { return mRelativeReferenceTime; }


	//setters

	void SetDefaultValues();

	//...absolute times

	void EnableCyclePass( bool enable ) { mUsingCyclePass = enable; }

	QDateTime& StartTime() { return mStartTime; }
    QDateTime& StopTime() { return mStopTime; }

    int& StartCycle() { return mStartCycle; }
    int& StopCycle() { return mStopCycle; }
    int& StartPass() { return mStartPass; }
    int& StopPass() { return mStopPass; }


	//...relative times	
	// - see also static members
	
	void EnableRelativeTimes( bool enable );

	bool UpdateRelative2AbsoluteTimes( int start, int stop, bool use_current, const QDateTime &relative_ref = QDateTime::currentDateTime() );



	/////////////////////////////
	//		operations
	/////////////////////////////

	bool BoundingArea( double &lon1, double &lat1, double &lon2, double &lat2 ) const;


protected:

	bool GetTimeBounds( CDate &Start, CDate &Stop, const std::string &label_for_cycle_pass, std::string &error_msg ) const;

	void SetDefaultCyclePassValues();

	void UpdateRelativeDaysFromAbsoluteTimes();

public:
	std::pair< bool, bool > Apply( const CStringList& files_in, CStringList& files_out, std::string &error_msg, CProgressInterface *progress ) const;

	// Argument label_for_cycle_pass must be the value returned by CProduct::GetLabelForCyclePass()
	// Items in op_vars should be empty or the variables names inserted by the user in the operation; but must be 3
	//
    std::string GetSelectionCriteriaExpression( const std::string &label_for_cycle_pass, const std::vector< std::string > &op_vars = std::vector< std::string >() ) const;
};




//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

// The std::string pair element is the alias value, if the name has a corresponding alias
// The CField pair element means is the corresponding variable in product, if found and the 
//	alias values is not a formula
//
struct CAliasInfo
{
	std::pair< std::string, CField* > mInfo;

	CAliasInfo()
	{}

	CAliasInfo( const std::string &s, CField *pf )
		: mInfo( s, pf )
	{}


	bool Empty() const { return Value().empty(); }


	std::string& Value() { return mInfo.first; }

	const std::string& Value() const { return mInfo.first; }


	CField*& Field() { return mInfo.second; }

	const CField* Field() const { return mInfo.second; }
};





//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

class CBratFilters : public CFileSettings
{
    //types

    using base_t = CFileSettings;


	// static data

	static CBratFilters *smInstance;

public:

	//client in charge of deletion
	//
	static CBratFilters* CreateInstance( const std::string &workspaces_directory )
	{
		assert__( !smInstance );

		smInstance = new CBratFilters( workspaces_directory );
		return smInstance;
	}

	static const CBratFilters& GetInstance()
	{
		assert__( smInstance );

		return *smInstance;
	}

    // statics ( Fields, Aliases... )

    static const std::string& FindAliasValue( CProduct *product, const std::string &alias_name );

    static CAliasInfo FindField( CProduct *product, const std::string &name, bool try_unsupported, bool &alias_used, std::string &field_error_msg );


    static std::pair<CAliasInfo, CAliasInfo> FindLonLatFields( CProduct *product, bool try_unsupported, bool &lon_alias_used, bool &lat_alias_used, std::string &field_error_msg );

    static CAliasInfo FindTimeField( CProduct *product, bool try_unsupported, bool &alias_used, std::string &field_error_msg );



protected:

    // instance data

    std::string mWorkspacesPath;

    std::map< std::string, CBratFilter > mFiltersMap;


    // construction / destruction

    CBratFilters( const std::string &dir )
        : base_t( dir + "/Filters.ini" )
        , mWorkspacesPath( dir )
    {}
public:

    virtual ~CBratFilters()
    {}

	//access related data managed by filters

    CBratAreas& Areas()
    {
        static CBratAreas a( mWorkspacesPath + "/UserAreas.ini" );
        return a;
    }

	const CBratAreas& Areas() const
	{
		return const_cast<CBratFilters*>( this )->Areas();
	}

    CBratRegions& Regions()
    {
        static CBratRegions r( mWorkspacesPath + "/UserRegions.ini" );
        return r;
    }


    // access filters

    const std::map< std::string, CBratFilter >& FiltersMap() const { return mFiltersMap; }

    CBratFilter* Find( const std::string &name );

    const CBratFilter* Find( const std::string &name ) const
    {
        return const_cast< CBratFilters* >( this )->Find( name );
    }

    std::string MakeNewName() const;


    // operations

    bool AddFilter( const std::string &name );

    bool RenameFilter( const std::string &name, const std::string &new_name );

    bool DeleteFilter( const std::string &name );

    std::pair< bool, bool > Apply( const std::string &name, const CStringList& files_in, CStringList& files_out, std::string& error_msg, CProgressInterface *progress, bool allow_unsupported = false ) const;

protected:
	
	bool Translate2SelectionCriteria( const std::string &label_for_cycle_pass, CProduct *product_ref, const std::string &name , std::string &error_msg ) const;

public:

    // persistence

    bool Load();

    bool Save();
};



#endif	//DATA_MODELS_FILTERS_BRAT_FILTERS_H
