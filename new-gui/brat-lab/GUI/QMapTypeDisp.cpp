
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "QMapTypeDisp.h"
#endif

#include "../stdafx.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif


#include "../../display/Libraries/+/QtUtils.h"

#include "../../libbrathl/brathl.h"


/*
#include "Trace.h"
#include "Tools.h"
#include "Exception.h"
using namespace brathl;

#include "BratProcess.h"
using namespace processes;

#include "BratGui.h"
#include "Workspace.h"
*/
#include "QMapTypeDisp.h"


//-------------------------------------------------------------
//------------------- QMapTypeDisp class --------------------
//-------------------------------------------------------------

QMapTypeDisp::QMapTypeDisp()
{
	Insert( "Y=F(X)", typeDispYFX );
	Insert( "Z=F(Lon,Lat)", typeDispZFLatLon );
	Insert("Z=F(X,Y)", typeDispZFXY);
}

//----------------------------------------
QMapTypeDisp::~QMapTypeDisp()
{}

//----------------------------------------
QMapTypeDisp& QMapTypeDisp::GetInstance()
{
	static QMapTypeDisp instance;

	return instance;
}

//----------------------------------------
bool QMapTypeDisp::ValidName( const std::string& name )
{
	return ValidName( name.c_str() );
}
//----------------------------------------
bool QMapTypeDisp::ValidName( const char* name )
{
	uint32_t value = Exists( name );
	return ( !CTools::IsDefaultValue( value ) );
}

//----------------------------------------

QString QMapTypeDisp::IdToName( uint32_t id )
{
	QMapTypeDisp::iterator it;

	for ( it = begin(); it != end(); it++ )
	{
		uint32_t value = it->second;
		if ( value == id )
		{
			return ( it->first ).c_str();
		}
	}

	return "";
}


//----------------------------------------
uint32_t QMapTypeDisp::NameToId( const QString& name )
{
	return Exists( q2a( name ) );
}

//----------------------------------------
void QMapTypeDisp::NamesToArrayString( QStringList& array )
{
	QMapTypeDisp::iterator it;

	for ( it = begin(); it != end(); it++ )
	{
		uint32_t value = it->second;
		if ( !CTools::IsDefaultValue( value ) )
		{
			array << it->first.c_str();
		}
	}

}
//----------------------------------------
void QMapTypeDisp::NamesToComboBox( QComboBox& combo )
{
	QMapTypeDisp::iterator it;

	for ( it = begin(); it != end(); it++ )
	{
		uint32_t value = it->second;
		if ( !CTools::IsDefaultValue( value ) )
		{
			//combo.Insert((it->first).c_str(), value);
			combo.addItem( ( it->first ).c_str() );
		}
	}

}
//----------------------------------------

QString QMapTypeDisp::Enum()
{
	QString result;

	for ( QMapTypeDisp::iterator it = begin(); it != end(); it++ )
	{
		result += t2q( it->first.c_str() + std::string( " ==> " ) + n2s<std::string>( it->second ) + "  " );
	}

	return result;
}


