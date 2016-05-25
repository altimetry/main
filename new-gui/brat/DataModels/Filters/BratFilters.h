#if !defined DATA_MODELS_FILTERS_BRAT_FILTERS_H
#define DATA_MODELS_FILTERS_BRAT_FILTERS_H

#include <QDate>

#include "libbrathl/Date.h"

#include "new-gui/Common/ApplicationSettings.h"
#include "new-gui/Common/tools/CoreTypes.h"

#include "BratAreas.h"


namespace brathl
{
    class CStringList;
	class CMapProduct;
}

using namespace brathl;



class CBratFilter
{
    //types

    friend class CBratFilters;

protected:

    //static members

	static inline CDate q2brat( const QDateTime &q )
	{
		if ( !q.isValid() )
			return CDate();

		return CDate( 
			q.date().year(), q.date().month(), q.date().day(), 
			q.time().hour(), q.time().minute(), q.time().second(), q.time().msec() );
	}


    // instance data

    std::string mName;

    std::vector< std::string > mAreaNames;

    QDateTime mStartTime;
    QDateTime mStopTime;

    int mStartCycle;
    int mStopCycle;
    int mStartPass;
    int mStopPass;


    // construction / destruction

public:
    CBratFilter( const std::string &name ) :
        mName( name )
    {
        setDefaultValues();
    }

    CBratFilter( const CBratFilter &o )
    {
        *this = o;
    }

    CBratFilter& operator = ( const CBratFilter &o );

    virtual ~CBratFilter()
    {}


    // access

    const std::string& Name() const { return mName; }
    std::string& Name() { return mName; }

	const CBratAreas& Areas() const;


    //space

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


    //time


    QDateTime StartTime() const { return mStartTime; }
    QDateTime StopTime() const { return mStopTime; }

    CDate BratStartTime() const { return q2brat( mStartTime ); }
    CDate BratStopTime() const { return q2brat( mStopTime ); }


    int StartCycle() const { return mStartCycle; }
    int StopCycle() const { return mStopCycle; }
    int StartPass() const { return mStartPass; }
    int StopPass() const { return mStopPass; }


    QDateTime& StartTime() { return mStartTime; }
    QDateTime& StopTime() { return mStopTime; }

    int& StartCycle() { return mStartCycle; }
    int& StopCycle() { return mStopCycle; }
    int& StartPass() { return mStartPass; }
    int& StopPass() { return mStopPass; }


	// operations

	void BoundingArea( double &lon1, double &lat1, double &lon2, double &lat2 ) const;

    bool Apply( const CStringList& files_in, CStringList& files_out ) const;

    void setDefaultValues();
};






class CBratFilters : public CFileSettings
{
    //types

    using base_t = CFileSettings;


	// static data

	static CBratFilters *smInstance;

public:

	//client in charge of deletion
	//
	static CBratFilters* CreateInstance( const std::string &internal_data_dir )
	{
		assert__( !smInstance );

		smInstance = new CBratFilters( internal_data_dir );
		return smInstance;
	}

	static const CBratFilters& GetInstance()
	{
		assert__( smInstance );

		return *smInstance;
	}



protected:
    // instance data

    std::string mInternalDataPath;

    std::map< std::string, CBratFilter > mFiltersMap;


    // construction / destruction

    CBratFilters( const std::string &internal_data_dir )
        : base_t( internal_data_dir + "/Filters.ini" )
        , mInternalDataPath( internal_data_dir )
    {}
public:

    virtual ~CBratFilters()
    {}

	//access related data managed by filters

    CBratAreas& Areas()
    {
        static CBratAreas a( mInternalDataPath + "/UserAreas.ini" );
        return a;
    }

	const CBratAreas& Areas() const
	{
		return const_cast<CBratFilters*>( this )->Areas();
	}

    CBratRegions& Regions()
    {
        static CBratRegions r( mInternalDataPath + "/UserRegions.ini" );
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

    bool Apply( const std::string &name, const CStringList& files_in, CStringList& files_out ) const;
    bool Translate2SelectionCriteria( CProduct *product_ref, const std::string &name ) const;


    // persistence

    bool Load();

    bool Save();
};


//
//void CDatasetPanel::ApplySelectionCriteria( CStringList& files_in, CStringList& files_out )
//{
//	CProduct* p = NULL;
//	bool bOk = true;
//
//	try
//	{
//		p = CProduct::Construct( *( files_in.begin() ) );
//	}
//	catch ( CException e )
//	{
//		bOk = false;
//	}
//	catch ( ... )
//	{
//		bOk = false;
//	}
//
//	if ( p == NULL )
//	{
//		bOk = false;
//	}
//
//	if ( !bOk )
//	{
//		files_out.Insert( files_in );
//		if ( p != NULL )
//		{
//			delete p;
//		}
//		return;
//	}
//
//	this->SetCursor( *wxHOURGLASS_CURSOR );
//
//	p->GetProductList().clear();
//	p->GetProductList().Insert( files_in );
//
//
//	string label = p->GetLabel();
//	CProduct* productRef = dynamic_cast<CProduct*>( CMapProduct::GetInstance().Exists( label ) );
//
//	if ( productRef == NULL )
//	{
//		files_out.Insert( files_in );
//		delete p;
//		return;
//	}
//
//
//	p->AddCriteria( productRef );
//
//	wxFileName logFileName;
//	logFileName.AssignDir( wxGetApp().GetExecPathName() );
//	logFileName.SetFullName( DATASET_SELECTION_LOG_FILENAME );
//	logFileName.Normalize();
//
//	p->ApplyCriteria( files_out, (const char *)m_logFileName.GetFullPath().c_str() );
//
//	delete p;
//	p = NULL;
//
//	this->SetCursor( wxNullCursor );
//
//
//}

#endif	//DATA_MODELS_FILTERS_BRAT_FILTERS_H
