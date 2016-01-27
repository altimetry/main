#if !defined COMMON_FILE_SETTINGS_H
#define COMMON_FILE_SETTINGS_H

#include <QSettings>

#include <new-gui/Common/QtStringUtils.h>
#include "new-gui/Common/ConfigurationKeywords.h"




///////////////////////////////////////////////////
//	Extract and insert values in QVariant
///////////////////////////////////////////////////

// Extract: QVariant => Type

template< typename STRING >
inline STRING qv2string( const QVariant &v )
{
	// ","s in values are treated by QSettings as list separators, so
	//	it returns a string list when ","s are found. In this case, the 
	//	following converts the string list value back to string, reading
	//	it as string otherwise.
	//
	QString qstr = v.type() == QVariant::StringList ? v.toStringList().join( "," ) : v.toString();
	return q2t< STRING >( qstr );
}


template< typename TYPE >
inline TYPE qv2type( const QVariant &v );

template<>
inline QString qv2type( const QVariant &v )
{
	return qv2string< QString >( v );
}
template<>
inline std::string qv2type( const QVariant &v )
{
	return qv2string< std::string >( v );
}
template<>
inline bool qv2type( const QVariant &v )
{
	return v.toBool();
}
template<>
inline QByteArray qv2type( const QVariant &v )
{
	return v.toByteArray();
}
template<>
inline int qv2type( const QVariant &v )
{
	return v.toInt();
}
template<>
inline double qv2type( const QVariant &v )
{
	return v.toDouble();
}



// Insert: Type => QVariant


template< typename TYPE >
inline QVariant type2qv( const TYPE &v )
{
	return v;
}

template<>
inline QVariant type2qv( const std::string &v )
{
	return t2q( v );
}




///////////////////////////////////////////////////
//	Help client code create pair/triplet arguments
///////////////////////////////////////////////////

// Key-Value type: for writing

template< typename T >
using kv_t = std::pair< std::string, T >;

// Key-Value-Default type: for reading

template< typename T >
using kvd_t = std::pair< kv_t< T* >, T >;


// Makers of Key-Value(-Default) pairs

// ... for writing

template< typename T >
kv_t<T> k_v( const std::string &key, const T& value )
{
	return std::make_pair( key, value );
}

// ... for reading

template< typename T >
kvd_t<T> k_v( const std::string &key, T *pvar )
{
	assert__( pvar );

	return std::make_pair( std::make_pair( key, pvar ), *pvar );
}

template< typename T >
kvd_t<T> k_v( const std::string &key, T *pvar, const T& defvalue )
{
	assert__( pvar );

	return std::make_pair( std::make_pair( key, pvar ), defvalue );
}



//	Helper base class to allow the derived classes to declare
//	QSettings instance members that use the default constructor 
//	and benefit from previous QCoreApplication assignments. This
//	way, settings are available all over the application. 
//	Since QSettings is non-copyable, without this base class the
//	QSettings members could only be (created with) pointers.
//
class CGlobalApplicationSettings
{
protected:
	CGlobalApplicationSettings()
	{}

	CGlobalApplicationSettings( const std::string &org_name, const std::string &exec_name, 
		const std::string &domain_name = "", QSettings::Format f = QSettings::IniFormat )
	{
		QCoreApplication::setOrganizationName( t2q( org_name ) );
		QCoreApplication::setOrganizationDomain( t2q( domain_name ) );
		QCoreApplication::setApplicationName( t2q( exec_name ) );
		QSettings::setDefaultFormat( f );
	}

	virtual ~CGlobalApplicationSettings()
	{}
};



//	CFileSettings is at the core of all file persistence
//	of whole application parameters and of the trees of files
//	that compose a workspace. It provides a uniform interface for
//	client code to read and write parameters (key-value pairs).
//	The underlying syntax is that of the *.ini file type; the 
//	underlying parser/writer framework is QSettings.
//
//	The core of the read/write interface is a set of static member
//	functions, for single or multiple parameters, which all other 
//	member functions use; all the logic is in this set of functions.
//
//	There are 2 types of Application Settings (see constructors): 
//
//		I. for whole application
//		II. for parameter files
//
//	Settings of type II. use instance variables of the class to
//	to read-write the file they are associated with. Settings 
//	of type II. can be accessed globally, without the need of 
//	an instance variable, but they also benefit of the same 
//	interface of type II, if they have access to the instance 
//	variable stored in the application class. 
//
//	A set of global functions, prefixed with "AppSettings", 
//	encapsulates and provides access to the global settings; 
//	the interface formed by these functions exactly matches the 
//	static interface of the class, where the logic whole logic 
//	is centralized. These functions are defined after the class 
//	declaration and should be used everywhere, with these 
//	exceptions:
//	- in the class (or derived classes) implementation
//	- by the core application classes (application and main 
//		window classes) when using functions other than 
//		readers/writers that have application scope, like
//		LoadConfig or SaveConfig, which are in charge of 
//		committing to/from file/memory the global settings.
//		
//
class CFileSettings : public CGlobalApplicationSettings
{
	//////////////////////////////////////
	//	types & friends
	//////////////////////////////////////

	using base_t = CGlobalApplicationSettings;

public:

	class CSection
	{
		QSettings &mRef;

	public:
		CSection( QSettings &s, const std::string &group ) : mRef( s )
		{
			mRef.beginGroup( group.c_str() );
		}
		virtual ~CSection()
		{
			mRef.endGroup();
		}


		QStringList Keys()
		{
			return Settings().childKeys();
		}

		QSettings& Settings()
		{
			return mRef;
		}

		const QSettings& Settings() const
		{
			return mRef;
		}
	};

public:

	//////////////////////////////////////
	//	static members
	//////////////////////////////////////

	// Version Control

	static const std::string& UnknownVersionValue()
	{
		static std::string version = "Unknown";
		return version;
	}

	static const std::string& VersionValue()
	{
		static std::string version = "1.0.0";
		return version;
	}

	// Generic Read / Write functions

	//... for single values

	template< class VALUE = std::string >
	static VALUE ReadValue( CSection &section, const std::string &key, const VALUE& def = VALUE() )
	{
		return qv2type< VALUE >( section.Settings().value( key.c_str(), type2qv( def ) ) );
	}


	template< class VALUE >
	static void WriteValue( CSection &section, const std::string &key, const VALUE &value )
	{
		section.Settings().setValue( key.c_str(), type2qv( value ) );
	}


	//... for sections with the same value types
	
	//		- open the respective section (group) with beginGroup before writing and close it afterwards

	template< typename T >
	static bool ReadValues( QSettings &settings, const std::string &group, std::initializer_list< kv_t< T& > > kv_pairs )
	{
		{
			CSection section( settings, group );
			for ( auto const &s : kv_pairs )
				s.second = ReadValue< T >( section, s.first );
		}
		return settings.status() == QSettings::NoError;
	}

	template< typename T >
	static bool WriteValues( QSettings &settings, const std::string &group, std::initializer_list< kv_t< T > > kv_pairs )
	{
		{
			CSection section( settings, group );
			for ( auto const &s : kv_pairs )
				WriteValue( section, s.first, s.second );
		}
		return settings.status() == QSettings::NoError;
	}

	template< typename T, typename F >
    static bool ApplyToWholeSection( QSettings &settings, const std::string &group, const F &f )
	{
		CSection section( settings, group );
		auto const keys = section.Keys();
		for ( auto const &key : keys )
		{
			std::string stdkey = q2a( key );
			if ( !f( stdkey, ReadValue< T >( section, stdkey ) ) )
				break;
		}

		return settings.status() == QSettings::NoError;
	}

	//... for sections, independently of values types
	
	//		- open the respective section (group) with beginGroup before writing and close it afterwards


protected:
	static bool ReadSection( CSection &section )
	{
		return section.Settings().status() == QSettings::NoError;
	}
	template< typename T, typename... Types >
    static bool ReadSection( CSection &section, kvd_t<T> &kv_pair,  Types... kv_pairs )
	{
		*kv_pair.first.second = ReadValue< T >( section, kv_pair.first.first, kv_pair.second );
		return ReadSection( section, kv_pairs... );
	}
public:
	template< typename... Types >
    static bool ReadSection( QSettings &settings, const std::string &group, Types... kv_pairs )
	{
		CSection section( settings, group );
		return ReadSection( section, kv_pairs... );
	}

protected:
	static bool WriteSection( CSection &section )
	{
		return section.Settings().status() == QSettings::NoError;
	}
	template< typename T, typename... Types >
	static bool WriteSection( CSection &section, const kv_t< T > &kv_pair,  const Types&... kv_pairs )
	{
		WriteValue( section, kv_pair.first, kv_pair.second );
		return WriteSection( section, kv_pairs... );
	}
public:
	template< typename... Types >
	static bool WriteSection( QSettings &settings, const std::string &group, const Types&... kv_pairs )
	{
		CSection section( settings, group.c_str() );
		return WriteSection( section, kv_pairs... );	//if the compiler complains here, make sure you are not trying to write pointers
	}

protected:

	//////////////////////////////////////
	//	data
	//////////////////////////////////////

	std::string mFilePath;

	QSettings mSettings;
	std::string mVersion;


	//////////////////////////////////////
	//	construction / destruction
	//////////////////////////////////////

protected:

	virtual bool CheckVersion();


	// Settings of type I - For exclusive use of application-wide settings
	//
	// For Application type initialization; static and non-static read/write functions can access (THE) instance type

    CFileSettings( const std::string &org_name, const std::string &exec_name, const std::string &domain_name = "" ) :
        base_t( org_name, exec_name, domain_name )
	{
		qDebug() << mSettings.format();
		qDebug() << mSettings.fileName();

		CheckVersion();
	}

public:
	// Settings of type II - used by parameter files
	//
	// Module type instance; static read/write functions should not be used with this instance type

    CFileSettings( const std::string &path ) :
		  mFilePath( path )
		, mSettings( mFilePath.c_str(), QSettings::IniFormat )
	{
		CheckVersion();
	}

    virtual ~CFileSettings()
	{}

	//////////////////////////////////////
	//	access
	//////////////////////////////////////

	QSettings::Status Status() const { return mSettings.status(); }

	QStringList GroupKeys( const std::string &group )
	{
		CSection s( mSettings, group );
		return mSettings.childKeys();
	}

	std::string Group() const
	{
		return q2a( mSettings.group() );
	}

    std::string FilePath() const
    {
        return q2a( mSettings.fileName() );
    }

protected:

	//////////////////////////////////////
	//	access - internal class use only
	//////////////////////////////////////

	// Read / Write Section (group) Values

	//... for sections with the same value types

	template< typename T >
	bool ReadValues( const std::string &group, std::initializer_list< kv_t< T& > > kv_pairs )
	{
		return ReadValues( mSettings, group, kv_pairs );
	}

	template< typename T >
	bool WriteValues( const std::string &group, std::initializer_list< kv_t< T > > kv_pairs )
	{
		return WriteValues( mSettings, group, kv_pairs );
	}

	// Can understand why and how they work, can't understand why they are needed

	bool ReadValues( const std::string &group, std::initializer_list< kv_t< std::string& > > kv_pairs )
	{
		return ReadValues< std::string >( group, kv_pairs );
	}

	bool WriteValues( const std::string &group, std::initializer_list< kv_t< std::string > > kv_pairs )
	{
		return WriteValues< std::string >( group, kv_pairs );
	}

	//template< typename T >
 //   bool ApplyToWholeSection( const std::string &group, std::function< bool (const std::string &key, const T &value ) > &f )
	template< typename T, typename F >
    bool ApplyToWholeSection( const std::string &group, const F &f )
	{
		return ApplyToWholeSection< T >( mSettings, group, f );
	}

	//... for sections, independently of values types
	
	template< typename... Types >
    bool ReadSection( const std::string &group, Types... kv_pairs )
	{
		return ReadSection( mSettings, group, kv_pairs... );
	}

	template< typename... Types >
	inline bool WriteSection( const std::string &group, const Types&... kv_pairs )
	{
		return WriteSection( mSettings, group, kv_pairs... );
	}

public:

	//////////////////////////////////////
	//	operations
	//////////////////////////////////////

    bool WriteVersionSignature()
    {
        return WriteValues( GROUP_SETTINGS_VERSION,
        {
            { KEY_SETTINGS_VERSION, VersionValue() }
        }
        );
    };

	void Sync()
	{
		mSettings.sync();
	}

	void Clear()
	{
		mSettings.clear();
	}

	void ClearGroup( const std::string &group )
	{
		CSection section( mSettings, group );		Q_UNUSED( section );
		mSettings.remove( QString() );
	}
};






#endif	//COMMON_FILE_SETTINGS_H
