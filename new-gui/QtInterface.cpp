#include "new-gui/brat/stdafx.h"

#include "QtInterface.h"

#include "Common/QtUtils.h"



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CBratLookupTable	
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CBratLookupTable - Load
//////////////////////////////////////////////////////////////////////////////////////////////////////////


bool LoadFromFile( CBratLookupTable &lut, const std::string& fileName )
{
	Q_UNUSED(lut);
	Q_UNUSED(fileName);

	SimpleWarnBox("Development message: LoadFromFile( CBratLookupTable &lut, const std::string& fileName ) called");
	return true;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CBratLookupTable - Store
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void SaveToFile( CBratLookupTable &lut, const std::string& fileName )
{
	Q_UNUSED(lut);
	Q_UNUSED(fileName);

	SimpleWarnBox("Development message: SaveToFile( CBratLookupTable &lut, const std::string& fileName ) called");
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CConfiguration
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "new-gui/brat/Workspaces/Dataset.h"

CConfiguration::CConfiguration( const std::string& fileName )
	//const QString & fileName, Format format, QObject * parent = 0)	
	: base_t( t2q( fileName), QSettings::IniFormat )
{}

void CConfiguration::SetPath( const std::string& strPath )
{
}
bool CConfiguration::Write( const std::string& key, const std::string& value )
{
	return true;
}
bool CConfiguration::Write( const std::string& key, const int& value )
{
	return true;
}
size_t CConfiguration::GetNumberOfEntries()
{
	return 0;
}
bool CConfiguration::GetNextEntry( std::string& str, long& lIndex )
{
	return true;
}
std::string CConfiguration::Read( const std::string& str ) const
{
	return "";
}
bool CConfiguration::Read( const std::string& str, std::string *value ) const
{
	return true;
}
bool CConfiguration::Read( const std::string& str, int *value ) const
{
	return true;
}
bool CConfiguration::Read( const std::string& str, double *value, double defValue ) const
{
	return true;
}
bool CConfiguration::Read( const std::string& str, int32_t *value, double defValue ) const
{
	return true;
}
bool CConfiguration::Flush( bool bCurrentOnly )		//bCurrentOnly = false
{
	return true;
}



bool CConfiguration::SaveConfig( const CDataset *d )
{
	////return
	////	SaveConfig( config, ENTRY_FILE ) &&
	////	SaveConfigSpecificUnit( config, ENTRY_UNIT );

	//// SaveConfig( CConfiguration *config, const std::string& entry )
	//{
	//	const std::string entry( ENTRY_FILE );

	//	SetPath( "/" + d->GetName() );

	//	int index = 0;
	//	bool bOk = true;
	//	for ( CProductList::const_iterator it = d->GetProductList()->begin(); it != d->GetProductList()->end(); it++ )
	//	{
	//		index++;
	//		bOk &= Write( entry + n2s<std::string>( index ), it->c_str() );
	//	}
	//	if ( !bOk )
	//		return false;
	//}

	////SaveConfigSpecificUnit( CConfiguration *config, const std::string& entry )
	//{
	//	const std::string entry( ENTRY_UNIT );

	//	SetPath( "/" + d->GetName() );

	//	bool bOk = true;
	//	for ( CStringMap::const_iterator itMap = d->GetFieldSpecificUnits()->begin(); itMap != d->GetFieldSpecificUnits()->end(); itMap++ )
	//	{
	//		bOk &= Write( entry + "_" + itMap->first, ( itMap->second ).c_str() );
	//	}

	//	return bOk;
	//}

	return true;
}

bool CConfiguration::LoadConfig( CDataset *d  )
{
	//bool bOk = true;

	//in workspace

	beginGroup( t2q( GROUP_DATASETS ) );
	QStringList dkeys = allKeys();
	QStringList dvalues;
	for ( auto dkey : dkeys )
	{
		SimpleMsgBox( dkey );
		auto dvalue = value( dkey, QString() ).toString();
		SimpleMsgBox( dvalue );
		dvalues << dvalue;
	}
	endGroup();

	//here

	for ( auto dvalue : dvalues )
	{
		beginGroup( dvalue /*t2q( d->GetName() )*/ );

		QStringList fkeys = allKeys();
		for ( auto fkey : fkeys )
			SimpleMsgBox( fkey );

		endGroup();
	}


	//SetPath( "/" + d->GetName() );

	//GetNumberOfEntries();
	//std::string entry;
	//std::string valueString;
	//long i = 0;

	//CStringArray findStrings;
	//while ( GetNextEntry( entry, i ) )
	//{
	//	// Finds ENTRY_FILE entries (dataset files entries)
	//	findStrings.RemoveAll();
	//	CTools::Find( entry, ENTRY_FILE_REGEX, findStrings );

	//	if ( findStrings.size() > 0 )
	//	{
	//		valueString = Read( entry );
	//		d->GetProductList()->Insert( valueString );
	//		continue;
	//	}

	//	// Finds specific unit entries
	//	findStrings.RemoveAll();
	//	CTools::Find( entry, ENTRY_UNIT_REGEX, findStrings );

	//	if ( findStrings.size() > 0 )
	//	{
	//		valueString = Read( entry );
	//		d->GetFieldSpecificUnits()->Insert( findStrings.at( 0 ), valueString );			//m_fieldSpecificUnit.Dump(*CTrace::GetDumpContext());
	//		continue;
	//	}
	//}

	return true;
}
