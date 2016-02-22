
// For compilers that support precompilation
#include "wx/wxprec.h"

#include <vtkLookupTable.h>

#include "wxInterface.h"

#include "wx/fileconf.h" // (wxFileConfig class)

#include "BratDisplayApp.h"



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CBratLookupTable	
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define brathlFmtEntryColorMacro(entry, index)	wxString::Format(wxString(CBratLookupTable::GROUP_COLOR()) + "%d/" + entry, index)


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CBratLookupTable - Load
//////////////////////////////////////////////////////////////////////////////////////////////////////////


//bool LoadFromFile( CBratLookupTable &lut, const std::string& fileName )
//{
//	auto HandleLoadError = [&fileName]( bool bOk, const wxString& entry )
//	{
//		if ( bOk == false )
//		{
//			CException e( std::string( "ERROR in CBratLookupTable::LoadFromFile - Can't read " + entry + " from file " + fileName ),
//				BRATHL_LOGIC_ERROR );
//			throw( e );
//		}
//	};


//	auto HandleLoadColorError = [&HandleLoadError]( bool bOk, const wxString& entry, double& color )
//	{
//		HandleLoadError( bOk, entry );

//		if ( color < 0.0 )
//		{
//			::wxMessageBox( wxString::Format( "Invalid value %f for '%s'\n Value will bet set to 0", color, entry.c_str() ),
//				"Warning",
//				wxOK | wxCENTRE | wxICON_EXCLAMATION );
//			color = 0.0;
//		}

//		if ( color > 1.0 )
//		{
//			::wxMessageBox( wxString::Format( "Invalid value %f for '%s'\n Value will bet set to 1", color, entry.c_str() ),
//				"Warning",
//				wxOK | wxCENTRE | wxICON_EXCLAMATION );
//			color = 1.0;
//		}
//	};


//	auto LoadGradFromFile = [&lut, &HandleLoadColorError, &HandleLoadError]( wxFileConfig& file )
//	{
//		bool bOk = true;
//		lut.m_grad.RemoveAll();
//		lut.m_cust.RemoveAll();

//		wxString group;
//		long dummy;
//		file.SetPath( "/" );

//		bOk = file.GetFirstGroup( group, dummy );

//		double r, g, b, a;
//		r = g = b = a = 0.0;
//		int32_t xValue = 0;

//		bool isColorDef = false;
//		bool bError = false;

//		while ( bOk )
//		{
//			isColorDef =  ( wxString( CBratLookupTable::GROUP_COLOR() ).CmpNoCase( group.Left( CBratLookupTable::GROUP_COLOR().length() ) ) == 0 );
//			if ( isColorDef )
//			{
//				bool bOkEntry = true;
//				bOkEntry = file.Read( group + "/R", &r );
//				HandleLoadColorError( bOkEntry, group, r );
//				bOkEntry = file.Read( group + "/G", &g );
//				HandleLoadColorError( bOkEntry, group, g );
//				bOkEntry = file.Read( group + "/B", &b );
//				HandleLoadColorError( bOkEntry, group, b );
//				bOkEntry = file.Read( group + "/A", &a );
//				HandleLoadColorError( bOkEntry, group, a );
//				bOkEntry = file.Read( group + "/Value", &xValue );
//				HandleLoadError( bOkEntry, group );
//				if ( ( xValue != 0 ) && ( xValue != lut.m_vtkLookupTable->GetNumberOfTableValues() ) )
//				{
//					std::string msg = wxString::Format( "Invalid value %d for '%s/Value' \n Color table will not be loaded",
//						xValue,
//						group.c_str() ).ToStdString();
//					::wxMessageBox( msg,
//						"Warning",
//						wxOK | wxCENTRE | wxICON_EXCLAMATION );
//					bError = true;
//					//----------
//					break;
//					//----------
//				}


//				CPlotColor* vtkColor = new CPlotColor( r, g, b, a );
//				if ( xValue == 0 )
//				{
//					lut.m_grad.InsertAt( lut.m_grad.begin(), vtkColor );
//				}
//				else
//				{
//					lut.m_grad.InsertAt( lut.m_grad.end(), vtkColor );
//				}

//			}

//			bOk = file.GetNextGroup( group, dummy );
//		}

//		if ( bError )
//		{
//			lut.m_grad.RemoveAll();
//		}


//		if ( lut.m_grad.size() <= 0 )
//		{
//			return false;
//		}

//		if ( lut.m_grad.size() != 2 )
//		{
//			::wxMessageBox( wxString::Format( "There are %ld color definitions in file '%s'\n File must contains only 2 (min/max)\n"
//				"Color table will not be loaded", (long)lut.m_grad.size(), lut.m_fileName.c_str() ),
//				"Warning",
//				wxOK | wxCENTRE | wxICON_EXCLAMATION );
//			bError = true;
//		}

//		return ( bError == false );

//	};

//	auto LoadCustFromFile = [&]( wxFileConfig& file )
//	{

//		bool bOk = true;
//		lut.m_grad.RemoveAll();
//		lut.m_cust.RemoveAll();

//		wxString group;
//		long dummy;
//		file.SetPath( "/" );

//		bOk = file.GetFirstGroup( group, dummy );

//		double r, g, b, a;
//		r = g = b = a = 0.0;
//		int32_t xValue = 0;

//		bool isColorDef = false;
//		bool bError = false;

//		while ( bOk )
//		{
//			isColorDef =  ( wxString( CBratLookupTable::GROUP_COLOR() ).CmpNoCase( group.Left( CBratLookupTable::GROUP_COLOR().length() ) ) == 0 );
//			if ( isColorDef )
//			{
//				bool bOkEntry = true;
//				bOkEntry = file.Read( group + "/R", &r );
//				HandleLoadColorError( bOkEntry, group, r );
//				bOkEntry = file.Read( group + "/G", &g );
//				HandleLoadColorError( bOkEntry, group, g );
//				bOkEntry = file.Read( group + "/B", &b );
//				HandleLoadColorError( bOkEntry, group, b );
//				bOkEntry = file.Read( group + "/A", &a );
//				HandleLoadColorError( bOkEntry, group, a );
//				bOkEntry = file.Read( group + "/Value", &xValue );
//				HandleLoadError( bOkEntry, group );
//				if ( ( xValue < 0 ) || ( xValue > lut.m_vtkLookupTable->GetNumberOfTableValues() ) )
//				{
//					::wxMessageBox( wxString::Format( "Invalid value %d for '%s'\n"
//						"Correct range is [%d,%d]\n"
//						"Color table will not be loaded",
//						xValue, group.c_str(), 0, static_cast<int>( lut.m_vtkLookupTable->GetNumberOfTableValues() ) ),
//						"Warning",
//						wxOK | wxCENTRE | wxICON_EXCLAMATION );
//					bError = true;
//					//----------
//					break;
//					//----------
//				}

//				CCustomColor* c = new CCustomColor( CPlotColor( r, g, b, a ), xValue );
//				std::string warning;
//				if ( lut.InsertCustomColor( c, warning ) == -1 && !warning.empty() )
//					::wxMessageBox(warning, "Warning", wxOK | wxCENTRE | wxICON_EXCLAMATION);
//			}

//			bOk = file.GetNextGroup( group, dummy );
//		}

//		if ( bError )
//		{
//			lut.m_cust.RemoveAll();
//		}


//		if ( lut.m_cust.size() <= 0 )
//		{
//			return false;
//		}

//		if ( lut.m_cust.size() < 2 )
//		{
//			::wxMessageBox( wxString::Format( "There are %ld color definitions in file '%s'\n File must contains at least 2\n"
//				"Color table will not be loaded", (long)lut.m_cust.size(), lut.m_fileName.c_str() ),
//				"Warning",
//				wxOK | wxCENTRE | wxICON_EXCLAMATION );
//			bError = true;
//		}

//		return ( bError == false );
//	};


//	////////////
//	// Main Body
//	////////////

//	bool bOk = true;

//	CBratLookupTable* lutBackup = new CBratLookupTable( lut );

//	lut.m_fileName = fileName;

//	wxFileConfig file( wxGetApp().GetAppName(), wxEmptyString, lut.m_fileName, wxEmptyString, wxCONFIG_USE_LOCAL_FILE );

//	int32_t valueLong;
//	wxString valueString;

//	bOk = file.Read( CBratLookupTable::ENTRY_NUMCOLORS(), &valueLong );
//	HandleLoadError( bOk, CBratLookupTable::ENTRY_NUMCOLORS() );
//	valueLong = ( valueLong > 65536 ) ? 65536 : valueLong;
//	lut.m_vtkLookupTable->SetNumberOfTableValues( valueLong );

//	/*
//	bOk = file.Read(ENTRY_STD, valueString);
//	HandleLoadError(bOk, ENTRY_STD);
//	m_std = valueString.c_str();
//	*/
//	bOk = file.Read( CBratLookupTable::ENTRY_CURVE(), &valueString );
//	HandleLoadError( bOk, CBratLookupTable::ENTRY_CURVE() );
//	lut.m_curve = valueString.c_str();

//	//if ( (m_curve != CURVE_LINEAR) || (m_curve != CURVE_SQRT) || (m_curve != CURVE_COSINUS) )
//	if ( lut.IsValidCurve( lut.m_curve ) == false )
//	{
//		::wxMessageBox( wxString::Format( "Unknown entry value '%s' for '%s' in file '%s'\n Linear value will be set",
//			lut.m_curve.c_str(), CBratLookupTable::ENTRY_CURVE().c_str(), lut.m_fileName.c_str() ),
//			"Warning",
//			wxOK | wxCENTRE | wxICON_EXCLAMATION );

//		lut.m_curve = CBratLookupTable::CURVE_LINEAR();

//	}

//	bOk = file.Read( CBratLookupTable::ENTRY_CURRENTFCT(), &valueString );
//	HandleLoadError( bOk, CBratLookupTable::ENTRY_CURRENTFCT() );
//	lut.m_currentFunction = valueString.c_str();


//	if ( lut.m_currentFunction == lut.m_customFunction )
//	{
//		bOk = LoadCustFromFile( file );
//	}
//	else if ( lut.m_currentFunction == lut.m_gradientFunction )
//	{
//		bOk = LoadGradFromFile( file );
//	}
//	else
//	{
//		::wxMessageBox( wxString::Format( "Unknown entry value '%s' for '%s' in file '%s'\n Color table will not be loaded",
//			lut.m_currentFunction.c_str(), CBratLookupTable::ENTRY_CURRENTFCT().c_str(), lut.m_fileName.c_str() ),
//			"Warning",
//			wxOK | wxCENTRE | wxICON_EXCLAMATION );
//		bOk = false;

//	}

//	if ( bOk == false )
//	{
//		lut = *lutBackup;
//	}

//	delete lutBackup;
//	lutBackup = NULL;

//	lut.ExecMethod( lut.m_currentFunction );

//	lut.ExecCurveMethod( lut.m_curve );

//	return bOk;
//}




////////////////////////////////////////////////////////////////////////////////////////////////////////////
////										CBratLookupTable - Store
////////////////////////////////////////////////////////////////////////////////////////////////////////////


//void SaveToFile( CBratLookupTable &lut, const std::string& fileName )
//{

//	auto SaveCustToFile = [&lut]( wxFileConfig& file )
//	{
//		bool bOk = true;

//		if ( lut.m_cust.size() <= 0 )
//		{
//			return;
//		}

//		if ( lut.m_cust.size() < 2 )
//		{
//			std::string msg = CTools::Format( "ERROR in CBratLookupTable::SaveCustToFile() - size of custom color array is less 2 : %ld",
//				(long)lut.m_cust.size() );
//			CException e( msg, BRATHL_LOGIC_ERROR );
//			throw( e );
//		}


//		CObArray::const_iterator it;

//		for ( it = lut.m_cust.begin(); it != lut.m_cust.end(); it++ )
//		{
//			CCustomColor* customColor = dynamic_cast<CCustomColor*>( *it );
//			if ( customColor == NULL )
//			{
//				CException e( "ERROR in CBratLookupTable::SaveCustToFile - at least one of the array custom color object is not a CCustomColor object", BRATHL_LOGIC_ERROR );
//				throw( e );
//			}

//			CPlotColor* vtkColor = customColor->GetVtkColor();
//			if ( vtkColor == NULL )
//			{
//				CException e( "ERROR in CBratLookupTable::SaveCustToFile - In Custom Color object there isn't a CPlotColor object", BRATHL_LOGIC_ERROR );
//				throw( e );
//			}


//			bOk &= file.Write( brathlFmtEntryColorMacro( "R", it - lut.m_cust.begin() ), vtkColor->Red() );
//			bOk &= file.Write( brathlFmtEntryColorMacro( "G", it - lut.m_cust.begin() ), vtkColor->Green() );
//			bOk &= file.Write( brathlFmtEntryColorMacro( "B", it - lut.m_cust.begin() ), vtkColor->Blue() );
//			bOk &= file.Write( brathlFmtEntryColorMacro( "A", it - lut.m_cust.begin() ), vtkColor->Alpha() );
//			bOk &= file.Write( brathlFmtEntryColorMacro( "Value", it - lut.m_cust.begin() ), customColor->GetXValue() );
//		}

//		if ( bOk == false )
//		{
//			CException e( std::string( "ERROR in CBratLookupTable::SaveToFile - Can't write file " + lut.m_fileName ),
//				BRATHL_LOGIC_ERROR );
//			throw( e );
//		}

//	};


//	auto SaveGradToFile = [&lut]( wxFileConfig& file )
//	{
//		bool bOk = true;

//		if ( lut.m_grad.size() <= 0 )
//		{
//			return;
//		}
//		if ( lut.m_grad.size() != 2 )
//		{
//			std::string msg = CTools::Format( "ERROR in CBratLookupTable::SaveGradToFile() - size of color array (must contains min/max) not equal 2 : %ld",
//				(long)lut.m_grad.size() );
//			CException e( msg, BRATHL_LOGIC_ERROR );
//			throw( e );
//		}

//		CPlotColor* c1 = dynamic_cast<CPlotColor*>( lut.m_grad.at( 0 ) );
//		CPlotColor* c2 = dynamic_cast<CPlotColor*>( lut.m_grad.at( 1 ) );
//		if ( ( c1 == NULL ) || ( c2 == NULL ) )
//		{
//			CException e( "ERROR in CBratLookupTable::SaveGradToFile() - at least one of the color object is not a CPlotColor object", BRATHL_LOGIC_ERROR );
//			throw( e );
//		}

//		bOk &= file.Write( brathlFmtEntryColorMacro( "R", 0 ), c1->Red() );
//		bOk &= file.Write( brathlFmtEntryColorMacro( "G", 0 ), c1->Green() );
//		bOk &= file.Write( brathlFmtEntryColorMacro( "B", 0 ), c1->Blue() );
//		bOk &= file.Write( brathlFmtEntryColorMacro( "A", 0 ), c1->Alpha() );
//		bOk &= file.Write( brathlFmtEntryColorMacro( "Value", 0 ), 0 );

//		bOk &= file.Write( brathlFmtEntryColorMacro( "R", 1 ), c2->Red() );
//		bOk &= file.Write( brathlFmtEntryColorMacro( "G", 1 ), c2->Green() );
//		bOk &= file.Write( brathlFmtEntryColorMacro( "B", 1 ), c2->Blue() );
//		bOk &= file.Write( brathlFmtEntryColorMacro( "A", 1 ), c2->Alpha() );
//		bOk &= file.Write( brathlFmtEntryColorMacro( "Value", 1 ), static_cast<int>( lut.m_vtkLookupTable->GetNumberOfTableValues() ) );

//		if ( bOk == false )
//		{
//			CException e( std::string( "ERROR in CBratLookupTable::SaveToFile - Can't write file " + lut.m_fileName ),
//				BRATHL_LOGIC_ERROR );
//			throw( e );
//		}
//	};


//	////////////
//	// Main Body
//	////////////

//	bool bOk = true;
//	lut.m_fileName = fileName;
//	try
//	{
//		::wxRemoveFile( lut.m_fileName );
//	}
//	catch ( ... )
//	{
//		//do nothing
//	}

//	wxFileConfig file( wxGetApp().GetAppName(), wxEmptyString, lut.m_fileName, wxEmptyString, wxCONFIG_USE_LOCAL_FILE );

//	//  file.DeleteAll();

//	bOk &= file.Write( CBratLookupTable::ENTRY_NUMCOLORS(), static_cast<int>( lut.m_vtkLookupTable->GetNumberOfTableValues() ) );
//	bOk &= file.Write( CBratLookupTable::ENTRY_CURRENTFCT(), lut.m_currentFunction.c_str() );
//	bOk &= file.Write( CBratLookupTable::ENTRY_CURVE(), lut.m_curve.c_str() );
//	//bOk &= file.Write(ENTRY_STD, m_std.c_str());
//	if ( bOk == false )
//	{
//		CException e( std::string( "ERROR in CBratLookupTable::SaveToFile - Can't write file " + lut.m_fileName ),
//			BRATHL_LOGIC_ERROR );
//		throw( e );
//	}

//	SaveGradToFile( file );

//	SaveCustToFile( file );
//}
