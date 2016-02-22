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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "BratDisplay.h"
#endif

// For compilers that support precompilation
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <locale.h>

#include <wx/evtloop.h>

#include "brathl_c/argtable2.h"

#include "new-gui/Common/tools/Trace.h"
#include "Tools.h"
#include "new-gui/Common/tools/Exception.h"
using namespace brathl;

#include "BratProcess.h"
using namespace processes;


#include "vtkOutputWindow.h"
#include "ColorPalette.h"

// Include private headers
#include "WindowHandler.h"
#include "wxInterface.h"
#ifdef CursorShape  //collsion Qt X
#undef CursorShape
#endif
#include "WorldPlotFrame.h"
#include "XYPlotFrame.h"
#include "ZFXYPlotFrame.h"
//#include "BitSet32.h"
#include "new-gui/brat/DataModels/PlotData/MapColor.h"
#include "new-gui/brat/DataModels/MapTypeDisp.h"
#include "new-gui/brat/DataModels/PlotData/ColorPalleteNames.h"
#include "new-gui/brat/DataModels/PlotData/WorldPlotData.h"

#include "BratDisplayApp.h"

#include <map>

#ifdef __WXMAC__
void MakeApplicationActive(void);
#endif

#ifdef WIN32
const std::string BRATHL_ICON_FILENAME = "BratIcon.ico";
#else
const std::string BRATHL_ICON_FILENAME = "BratIcon.bmp";
#endif


static const KeywordHelp keywordList[]	= {
	KeywordHelp(kwFILE,			1, 0),
	KeywordHelp(kwFIELD,			1, 23, NULL, 24),
	KeywordHelp(kwFIELD_GROUP,		-24),

	KeywordHelp(kwDISPLAY_PROPERTIES,	0, 1, "No"),
	KeywordHelp(kwDISPLAY_TITLE,		0, 1, "\"\""),
	KeywordHelp(kwDISPLAY_ANIMATIONBAR,	0, 1, "No"),
	KeywordHelp(kwDISPLAY_COLORBAR,		0, 1, "Yes"),
	KeywordHelp(kwDISPLAY_CENTERLAT,	0, 1, "0"),
	KeywordHelp(kwDISPLAY_CENTERLON,	0, 1, "0"),
	KeywordHelp(kwDISPLAY_PROJECTION,	0, 1, "3D"),
	KeywordHelp(kwDISPLAY_COASTRESOLUTION,	0, 1, "low"),
	KeywordHelp(kwDISPLAY_ZOOM_LON1,	0, 1, "-180"),
	KeywordHelp(kwDISPLAY_ZOOM_LON2,	0, 1, "180"),
	KeywordHelp(kwDISPLAY_ZOOM_LAT1,	0, 1, "-90"),
	KeywordHelp(kwDISPLAY_ZOOM_LAT2,	0, 1, "90"),
	KeywordHelp(kwDISPLAY_ANIMATION,	0, 1, "No"),
	KeywordHelp(kwDISPLAY_XMINVALUE,	0, 1, "min of data values for X axis"),
	KeywordHelp(kwDISPLAY_XMAXVALUE,	0, 1, "max of data values for X axis"),
	KeywordHelp(kwDISPLAY_YMINVALUE,	0, 1, "min of data values for Y axis"),
	KeywordHelp(kwDISPLAY_YMAXVALUE,	0, 1, "max of data values for Y axis"),
	KeywordHelp(kwDISPLAY_PLOT_TYPE,	0, 1, "Type of plot: 0 = YFX, 1 = ZFXY, 2 = ZFLatLon"),
	KeywordHelp(kwDISPLAY_XAXIS,		0, 24, "X axis name"),
	KeywordHelp(kwDISPLAY_XLABEL,		0, 24, "field title or field name"),
	KeywordHelp(kwDISPLAY_YAXIS,		0, 24, "Y axis name"),
	KeywordHelp(kwDISPLAY_YLABEL,		0, 1, "field title or field name"),
	KeywordHelp(kwDISPLAY_XTICKS,		0, 1, "6"),
	KeywordHelp(kwDISPLAY_YTICKS,		0, 1, "6"),
	KeywordHelp(kwDISPLAY_NAME,		0, 0, NULL, -23),
	KeywordHelp(kwDISPLAY_OPACITY,		0, -23, "0.7"),
	KeywordHelp(kwDISPLAY_MINVALUE,		0, -23, "min of data values"),
	KeywordHelp(kwDISPLAY_MAXVALUE,		0, -23, "max of data values"),
	KeywordHelp(kwDISPLAY_NUMCOLORLABELS,	0, -23, "2"),
	KeywordHelp(kwDISPLAY_COLORTABLE,	0, -23, "Aerosol"),
	KeywordHelp(kwDISPLAY_COLORCURVE,	0, -23, "Linear"),
	KeywordHelp(kwDISPLAY_CONTOUR,		0, -23, "No"),
	KeywordHelp(kwDISPLAY_CONTOUR_NUMBER,	0, -23, "5"),
	KeywordHelp(kwDISPLAY_CONTOUR_LABEL,	0, -23, "No"),
	KeywordHelp(kwDISPLAY_CONTOUR_LABEL_NUMBER, 0, -23, "1"),
	KeywordHelp(kwDISPLAY_CONTOUR_MINVALUE,	0, -23, "min of data values"),
	KeywordHelp(kwDISPLAY_CONTOUR_MAXVALUE,	0, -23, "max of data values"),
	KeywordHelp(kwDISPLAY_SOLID_COLOR,	0, -23, "Yes"),
	KeywordHelp(kwDISPLAY_EAST_COMPONENT,	0, -23, "No"),
	KeywordHelp(kwDISPLAY_NORTH_COMPONENT,	0, -23, "No"),
	KeywordHelp(kwDISPLAY_COLOR,		0, -23, "randow color"),
	KeywordHelp(kwDISPLAY_POINTS,		0, -23, "No"),
	KeywordHelp(kwDISPLAY_LINES,		0, -23, "Yes"),
	KeywordHelp(kwDISPLAY_POINTSIZE,	0, -23, "1.0"),
	KeywordHelp(kwDISPLAY_LINEWIDTH,	0, -23, "0.8"),
	KeywordHelp(kwDISPLAY_STIPPLEPATTERN,	0, -23, "Full"),
	KeywordHelp(kwDISPLAY_POINTGLYPH,	0, -23, "Circle"),
	KeywordHelp(kwDISPLAY_POINTFILLED,	0, -23, "Yes"),

	KeywordHelp(kwALIAS_NAME,		0, 0, "None", 14),
	KeywordHelp(kwALIAS_VALUE,		-14),
	KeywordHelp(kwVERBOSE,			0, 1, "0"),
	KeywordHelp(""),
};





// WDR: class implementations

//------------------------------------------------------------------------------
// BratDisplayApp
//------------------------------------------------------------------------------

IMPLEMENT_APP(CBratDisplayApp)

CBratDisplayApp::CBratDisplayApp()
{
	//m_centerLongitude = 0;
	//m_centerLatitude = 0;
	m_config = NULL;
	//m_indexProp = 0;

	m_isYFX = false;
	m_isZFLatLon = false;
	m_isZFXY = false;

	wxColourDialog dlg;
}
//----------------------------------------

CBratDisplayApp::~CBratDisplayApp()
{

}
//----------------------------------------

bool CBratDisplayApp::OnInit()
{
	// To be sure that number have always a decimal point (and not a comma
	// or something else)
	setlocale( LC_NUMERIC, "C" );

#if wxUSE_UNICODE
	mBratPaths = new CApplicationPaths( wxGetApp().argv[ 0 ].ToStdString().c_str() );
#else
	mBratPaths = new CApplicationPaths( wxGetApp().argv[0] );
#endif

	CTools::SetInternalDataDir( mBratPaths->mInternalDataDir );

	if ( !CTools::DirectoryExists( CTools::GetInternalDataDir() ) )
	{
		std::cerr << "WARNING: " << CTools::GetInternalDataDir() << " is not a valid directory" << std::endl;
		::wxMessageBox( wxString( "WARNING: " ) + CTools::GetInternalDataDir().c_str() +
			" is not a valid directory\n\n", "BRAT WARNING" );
		return false;

	}

	std::string errorMsg;
	if ( !CTools::LoadAndCheckUdUnitsSystem( errorMsg ) )
	{
		std::cerr << errorMsg << std::endl;
		::wxMessageBox( errorMsg.c_str(), "BRAT ERROR" );
		return false;
	}

	m_config = new wxFileConfig( wxGetApp().GetAppName(), wxEmptyString, wxEmptyString, wxEmptyString, wxCONFIG_USE_LOCAL_FILE );

	try
	{
		if ( GetCommandLineOptions( CBratDisplayApp::argc, CBratDisplayApp::argv ) )
		{
			GetParameters();
		}
		else
			return false;


#if !defined(WIN32) && !defined(_WIN32)

		//Progess dialogs displayed while plotting need this
		wxEventLoop *tmpEventLoop = new wxEventLoop();
		wxEventLoopBase::SetActive( tmpEventLoop );
#endif

		if ( m_isZFLatLon )
		{
			WorldPlot();
		}
		else if ( m_isYFX )
		{
			XYPlot();
		}
		else if ( m_isZFXY )
		{
			ZFXYPlot();
		}
		else
		{
			CException e( "CBratDisplayApp::OnInit - Only World Plot Data, XY Plot Data and ZFXY Plot Data are implemented", BRATHL_UNIMPLEMENT_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );
		}

#if !defined(WIN32) && !defined(_WIN32)

		wxEventLoopBase::SetActive( NULL );
		delete tmpEventLoop;
#endif

	}
	catch ( CException &e )
	{
		::wxMessageBox( e.what(), "BRAT ERROR" );
		return FALSE;
	}
	catch ( std::exception &e )
	{
		::wxMessageBox( e.what(), "BRAT RUNTIME ERROR" );
		return FALSE;
	}
	catch ( ... )
	{
		::wxMessageBox( " Unexpected error encountered", "BRAT FATAL ERROR" );
		return FALSE;
	}

#ifdef __WXMAC__
	// Make sure we become the foreground application
	// for when we have been spawned from another application (such as BratGui)
	MakeApplicationActive();
#endif

	return TRUE;
}
//----------------------------------------

int CBratDisplayApp::OnExit()
{

  if (m_config != NULL)
  {
    delete m_config;
    m_config = NULL;
  }

  return BRATHL_SUCCESS;
}
//----------------------------------------
void CBratDisplayApp::WorldPlot()
{
  CObArray::iterator itGroup;

  wxSize size = wxDefaultSize;
  wxPoint pos = wxDefaultPosition;

  for (itGroup = m_plots.begin(); itGroup != m_plots.end(); itGroup++)
  {
    CWPlot* wplot = dynamic_cast<CWPlot*>(*itGroup);
    if (wplot == NULL)
    {
      continue;
    }

    CWindowHandler::GetSizeAndPosition(size, pos);

    CreateWPlot(wplot, size, pos);
  }
}


//----------------------------------------
void CBratDisplayApp::ZFXYPlot()
{
  CObArray::iterator itGroup;

  wxSize size = wxDefaultSize;
  wxPoint pos = wxDefaultPosition;

  for (itGroup = m_plots.begin(); itGroup != m_plots.end(); itGroup++)
  {
    CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>(*itGroup);
    if (zfxyplot == NULL)
    {
      continue;
    }

    CWindowHandler::GetSizeAndPosition(size, pos);

    CreateZFXYPlot(zfxyplot, size, pos);
  }
}


//----------------------------------------
void CBratDisplayApp::XYPlot()
{
  CObArray::iterator itGroup;

  wxSize size = wxDefaultSize;
  wxPoint pos = wxDefaultPosition;

  for (itGroup = m_plots.begin(); itGroup != m_plots.end(); itGroup++)
  {
    CPlot* plot = dynamic_cast<CPlot*>(*itGroup);
    if (plot == NULL)
    {
      continue;
    }

    CWindowHandler::GetSizeAndPosition(size, pos);

    CreateXYPlot(plot, size, pos);
  }
}
//----------------------------------------
void CBratDisplayApp::DetermineCharSize(wxWindow* wnd, int32_t& width, int32_t& height)
{
  wxStaticText w(wnd, -1, "W", wxDefaultPosition, wxSize(-1,-1));
  w.GetSize(&width, &height);
//  w.Destroy();
}
/*
//----------------------------------------
int32_t CBratDisplayApp::GetNumberOfPlots()
{

//  CObMap::iterator it;
//  int32_t n = 0;
//  for(it = m_fieldBitSets.begin() ; it != m_fieldBitSets.end() ; it++)
//  {
//    CBitSet32* fieldBitSet = GetBitSetFields(it);
//    n += fieldBitSet->m_bitSet.count();
//  }
//  return n;

  return m_fields.size();
}
*/
/*
//----------------------------------------
int32_t CBratDisplayApp::GetNumberOfWPlots()
{
  return m_fields.size();
}
*/

//----------------------------------------
void CBratDisplayApp::GetXYPlotPropertyParams(int32_t nFields)
{
  int32_t i = 0;
  bool boolValue = false;
  double doubleValue = 0.0;
  std::string stringValue;
  int32_t uintValue = 0;

  CXYPlotProperties xyPlotProperty;
  CMapColor::GetInstance().ResetPrimaryColors();

  if (nFields <= 0)
  {
    throw  CParameterException("CBratDisplayApp::GetXYPlotPropertyParams - There is no field to plot",
			       BRATHL_INCONSISTENCY_ERROR);
  }

  int32_t showProp = m_params.CheckCount(kwDISPLAY_PROPERTIES, 0, 1);
  int32_t xMin = m_params.CheckCount(kwDISPLAY_XMINVALUE, 0, 1);
  int32_t xMax = m_params.CheckCount(kwDISPLAY_XMAXVALUE, 0, 1);
  int32_t yMin = m_params.CheckCount(kwDISPLAY_YMINVALUE, 0, 1);
  int32_t yMax = m_params.CheckCount(kwDISPLAY_YMAXVALUE, 0, 1);
  int32_t xTicks = m_params.CheckCount(kwDISPLAY_XTICKS, 0, 1);
  int32_t yTicks = m_params.CheckCount(kwDISPLAY_YTICKS, 0, 1);
  //int32_t xLabel = m_params.CheckCount(kwDISPLAY_XLABEL, 0, 1);
  int32_t yLabel = m_params.CheckCount(kwDISPLAY_YLABEL, 0, 1);
  int32_t title = m_params.CheckCount(kwDISPLAY_TITLE, 0, 1);

  int32_t name = m_params.CheckCount(kwDISPLAY_NAME, 0, nFields);
  int32_t color = m_params.CheckCount(kwDISPLAY_COLOR, 0, nFields);
  int32_t opacity = m_params.CheckCount(kwDISPLAY_OPACITY, 0, nFields);
  int32_t points = m_params.CheckCount(kwDISPLAY_POINTS, 0, nFields);
  int32_t lines = m_params.CheckCount(kwDISPLAY_LINES, 0, nFields);

  int32_t pointSize = m_params.CheckCount(kwDISPLAY_POINTSIZE, 0, nFields);

  int32_t lineWidth = m_params.CheckCount(kwDISPLAY_LINEWIDTH, 0, nFields);
  int32_t stipplePattern = m_params.CheckCount(kwDISPLAY_STIPPLEPATTERN, 0, nFields);

  int32_t pointGlyph = m_params.CheckCount(kwDISPLAY_POINTGLYPH, 0, nFields);
  int32_t pointFilled = m_params.CheckCount(kwDISPLAY_POINTFILLED, 0, nFields);

  //------------------------------------------
  // Get common property (same for each plot)
  //------------------------------------------

  if (showProp != 0)
  {
     m_params.m_mapParam[kwDISPLAY_PROPERTIES]->GetValue(boolValue);
     xyPlotProperty.SetShowPropertyPanel(boolValue);
  }
  if (xMin != 0)
  {
     m_params.m_mapParam[kwDISPLAY_XMINVALUE]->GetValue(doubleValue);
     xyPlotProperty.SetXMin(doubleValue);
  }
  if (xMax != 0)
  {
     m_params.m_mapParam[kwDISPLAY_XMAXVALUE]->GetValue(doubleValue);
     xyPlotProperty.SetXMax(doubleValue);
  }
  if (yMin != 0)
  {
     m_params.m_mapParam[kwDISPLAY_YMINVALUE]->GetValue(doubleValue);
     xyPlotProperty.SetYMin(doubleValue);
  }
  if (yMax != 0)
  {
     m_params.m_mapParam[kwDISPLAY_YMAXVALUE]->GetValue(doubleValue);
     xyPlotProperty.SetYMax(doubleValue);
  }
  if (xTicks != 0)
  {
     m_params.m_mapParam[kwDISPLAY_XTICKS]->GetValue(uintValue);
     xyPlotProperty.SetXNumTicks(uintValue);
  }
  if (yTicks != 0)
  {
     m_params.m_mapParam[kwDISPLAY_YTICKS]->GetValue(uintValue);
     xyPlotProperty.SetYNumTicks(uintValue);
  }
  /*
  if (xLabel != 0)
  {
     m_params.m_mapParam[kwDISPLAY_XLABEL]->GetValue(stringValue);
     xyPlotProperty.SetXLabel(stringValue.c_str());
  }
  */
  if (yLabel != 0)
  {
     m_params.m_mapParam[kwDISPLAY_YLABEL]->GetValue(stringValue);
     xyPlotProperty.SetYLabel(stringValue.c_str());
  }
  if (title != 0)
  {
     m_params.m_mapParam[kwDISPLAY_TITLE]->GetValue(stringValue);
     xyPlotProperty.SetTitle(stringValue.c_str());
  }

  //------------------------------------------
  // Fill plot porperties array and set a default color for each property
  //------------------------------------------
  CMapColor::GetInstance().ResetPrimaryColors();
  for (i = 0 ; i < nFields ; i++)
  {
    CXYPlotProperties* props = new CXYPlotProperties(xyPlotProperty);

    m_xyPlotProperties.Insert(props);
    CPlotColor vtkColor = CMapColor::GetInstance().NextPrimaryColors();
    if (vtkColor.Ok())
    {
      props->SetColor(vtkColor);
    }

  }


  //------------------------------------------
  // Get Name Property for each plot
  //------------------------------------------
  for (i = 0 ; i < name ; i++)
  {
    CXYPlotProperties* props = GetXYPlotProperty(i);
    if (props == NULL)
    {
      continue;
    }

    m_params.m_mapParam[kwDISPLAY_NAME]->GetValue(stringValue, i);
    if (stringValue.empty() == false)
    {
      props->SetName(stringValue.c_str());
    }
  }

  //------------------------------------------
  // Get Color Property for each plot
  //------------------------------------------
  for (i = 0 ; i < color ; i++)
  {
    CXYPlotProperties* props = GetXYPlotProperty(i);
    if (props == NULL)
    {
      continue;
    }

    m_params.m_mapParam[kwDISPLAY_COLOR]->GetValue(stringValue, i);
    CPlotColor vtkColor = CMapColor::GetInstance().NameToPlotColor(stringValue);

    if (vtkColor.Ok() == false)
    {
      vtkColor = CMapColor::GetInstance().NextPrimaryColors();
    }
    props->SetColor(vtkColor);
  }

  //------------------------------------------
  // Get Opacity Property for each plot
  //------------------------------------------
  for (i = 0 ; i < opacity ; i++)
  {
    CXYPlotProperties* props = GetXYPlotProperty(i);
    if (props == NULL)
    {
      continue;
    }

    m_params.m_mapParam[kwDISPLAY_OPACITY]->GetValue(doubleValue, i, 0.6);
    if (isDefaultValue(doubleValue) == false)
    {
      props->SetOpacity(doubleValue);
    }
  }

  //------------------------------------------
  // Get Points Property for each plot
  //------------------------------------------
  int32_t indexPointSize = 0;
  int32_t indexPointGlyph = 0;
  int32_t indexPointFilled = 0;
  for (i = 0 ; i < points ; i++)
  {
    CXYPlotProperties* props = GetXYPlotProperty(i);
    if (props == NULL)
    {
      continue;
    }

    m_params.m_mapParam[kwDISPLAY_POINTS]->GetValue(boolValue, i);
    props->SetPoints(boolValue);
    //------------------------------------------
    // Get Pointsize Property for each plot
    //------------------------------------------
    if (boolValue)  // points representation
    {
      if (indexPointSize < pointSize)
      {
        m_params.m_mapParam[kwDISPLAY_POINTSIZE]->GetValue(doubleValue, indexPointSize, 1.0);
        props->SetPointSize(doubleValue);
        indexPointSize++;
      }
      if (indexPointGlyph < pointGlyph)
      {
        PointGlyph pointGlyph = CMapPointGlyph::GetInstance().GetPointGlyph(m_params, indexPointGlyph);
        props->SetPointGlyph(pointGlyph);
        indexPointGlyph++;
      }
      if (indexPointFilled < pointFilled)
      {
        m_params.m_mapParam[kwDISPLAY_POINTFILLED]->GetValue(boolValue, indexPointFilled, true);
        props->SetFilledPoint(boolValue);
        indexPointFilled++;
      }
    }
  }
  //------------------------------------------
  // Get Lines Property for each plot
  //------------------------------------------
  for (i = 0 ; i < lines ; i++)
  {
    CXYPlotProperties* props = GetXYPlotProperty(i);
    if (props == NULL)
    {
      continue;
    }

    m_params.m_mapParam[kwDISPLAY_LINES]->GetValue(boolValue, i);
    props->SetLines(boolValue);
  }

  //------------------------------------------
  // Get Line Width Property for each plot
  //------------------------------------------
  for (i = 0 ; i < lineWidth ; i++)
  {
    CXYPlotProperties* props = GetXYPlotProperty(i);
    if (props == NULL)
    {
      continue;
    }

    m_params.m_mapParam[kwDISPLAY_LINEWIDTH]->GetValue(doubleValue, i, 0.8);
    props->SetLineWidth(doubleValue);
  }
  //------------------------------------------
  // Get Line Stipple Pattern Properties for each plot
  //------------------------------------------
  for (i = 0 ; i < stipplePattern ; i++)
  {
    CXYPlotProperties* props = GetXYPlotProperty(i);
    if (props == NULL)
    {
      continue;
    }

    StipplePattern stipplePatternValue = CMapStipplePattern::GetInstance().GetStipplePattern(m_params, i);
    props->SetLineStipple(stipplePatternValue);
  }
}
//----------------------------------------
CZFXYPlotProperties* CBratDisplayApp::GetZFXYPlotProperty(int32_t index)
{
  if ( (index < 0) || (static_cast<uint32_t>(index) >= m_zfxyPlotProperties.size()) )
  {
    std::string msg = CTools::Format("ERROR in  CBratDisplayApp::GetZFXYPlotProperty : index %d out-of-range "
                                "Valid range is [0, %ld]",
                                index,
                                (long)m_zfxyPlotProperties.size());
    throw CException(msg, BRATHL_LOGIC_ERROR);

  }

  CZFXYPlotProperties* props = dynamic_cast<CZFXYPlotProperties*>(m_zfxyPlotProperties.at(index));
  if (props == NULL)
  {
    throw CException("ERROR in  CBratDisplayApp::GetZFXYPlotProperty : dynamic_cast<CZFXYPlotProperties*>(m_zfxyPlotProperties->at(index)); returns NULL pointer - "
                 "zFxy Plot Property array seems to contain objects other than those of the class CZFXYPlotProperties or derived class", BRATHL_LOGIC_ERROR);
  }
  return props;

}
//----------------------------------------
CWorldPlotProperties* CBratDisplayApp::GetWorldPlotProperty(int32_t index)
{
  if ( (index < 0) || (static_cast<uint32_t>(index) >= m_wPlotProperties.size()) )
  {
    std::string msg = CTools::Format("ERROR in  CBratDisplayApp::CWorldPlotProperties : index %d out-of-range "
                                "Valid range is [0, %ld]",
                                index,
                                (long)m_wPlotProperties.size());
    throw CException(msg, BRATHL_LOGIC_ERROR);

  }

  CWorldPlotProperties* props = dynamic_cast<CWorldPlotProperties*>(m_wPlotProperties.at(index));
  if (props == NULL)
  {
    throw CException("ERROR in  CBratDisplayApp::GetWorldPlotProperty : dynamic_cast<CWorldPlotProperties*>(m_wPlotProperties->at(index)); returns NULL pointer - "
                 "world Plot Property array seems to contain objects other than those of the class CWorldPlotProperties or derived class", BRATHL_LOGIC_ERROR);
  }
  return props;

}

//----------------------------------------
CXYPlotProperties* CBratDisplayApp::GetXYPlotProperty(int32_t index)
{
  if ( (index < 0) || (static_cast<uint32_t>(index) >= m_xyPlotProperties.size()) )
  {
    std::string msg = CTools::Format("ERROR in  CBratDisplayApp::GetXYPlotProperty : index %d out-of-range "
                                "Valid range is [0, %ld]",
                                index,
                                (long)m_xyPlotProperties.size());
    throw CException(msg, BRATHL_LOGIC_ERROR);

  }

  CXYPlotProperties* props = dynamic_cast<CXYPlotProperties*>(m_xyPlotProperties.at(index));
  if (props == NULL)
  {
   // throw CException("ERROR in  CBratDisplayApp::GetXYPlotProperty : dynamic_cast<CXYPlotProperties*>(m_xyPlotProperties->at(index)); returns NULL pointer - "
   //              "XY Plot Property array seems to contain objects other than those of the class CXYPlotProperties or derived class", BRATHL_LOGIC_ERROR);
  }
  return props;

}

//----------------------------------------
void CBratDisplayApp::GetWPlotPropertyParams(int32_t nFields)
{
  int32_t i = 0;
  bool boolValue;
  double doubleValue;
  std::string stringValue;
  int32_t intValue;

  CWorldPlotProperties wPlotProperty;
  //CMapColor::GetInstance().ResetPrimaryColors();

  if (nFields <= 0)
  {
    throw  CParameterException("CBratDisplayApp::GetWPlotPropertyParams - There is no field to plot",
			       BRATHL_INCONSISTENCY_ERROR);
  }

  int32_t title = m_params.CheckCount(kwDISPLAY_TITLE, 0, 1);
  int32_t showProp = m_params.CheckCount(kwDISPLAY_PROPERTIES, 0, 1);
  int32_t showColorBar = m_params.CheckCount(kwDISPLAY_COLORBAR, 0, 1);
  int32_t showAnimationBar = m_params.CheckCount(kwDISPLAY_ANIMATIONBAR, 0, 1);
  int32_t centerLat = m_params.CheckCount(kwDISPLAY_CENTERLAT, 0, 1);
  int32_t centerLon = m_params.CheckCount(kwDISPLAY_CENTERLON, 0, 1);
  int32_t projection = m_params.CheckCount(kwDISPLAY_PROJECTION, 0, 1);
  int32_t coastResolution = m_params.CheckCount(kwDISPLAY_COASTRESOLUTION, 0, 1);
  int32_t zoomLon1 = m_params.CheckCount(kwDISPLAY_ZOOM_LON1, 0, 1);
  int32_t zoomLon2 = m_params.CheckCount(kwDISPLAY_ZOOM_LON2, 0, 1);
  int32_t zoomLat1 = m_params.CheckCount(kwDISPLAY_ZOOM_LAT1, 0, 1);
  int32_t zoomLat2 = m_params.CheckCount(kwDISPLAY_ZOOM_LAT2, 0, 1);
  int32_t withAnimation = m_params.CheckCount(kwDISPLAY_ANIMATION, 0, 1);

  int32_t minHeigth = m_params.CheckCount(kwDISPLAY_MINVALUE, 0, nFields);
  int32_t maxHeigth = m_params.CheckCount(kwDISPLAY_MAXVALUE, 0, nFields);
  int32_t opacity = m_params.CheckCount(kwDISPLAY_OPACITY, 0, nFields);
  int32_t numColorLabels = m_params.CheckCount(kwDISPLAY_NUMCOLORLABELS, 0, nFields);
  int32_t colorTable = m_params.CheckCount(kwDISPLAY_COLORTABLE, 0, nFields);
  int32_t colorCurve = m_params.CheckCount(kwDISPLAY_COLORCURVE, 0, nFields);
  int32_t contour = m_params.CheckCount(kwDISPLAY_CONTOUR, 0, nFields);
  int32_t contourNum = m_params.CheckCount(kwDISPLAY_CONTOUR_NUMBER, 0, nFields);
  int32_t contourLabel = m_params.CheckCount(kwDISPLAY_CONTOUR_LABEL, 0, nFields);
  int32_t contourLabelNumber = m_params.CheckCount(kwDISPLAY_CONTOUR_LABEL_NUMBER, 0, nFields);
  int32_t contourMinValue = m_params.CheckCount(kwDISPLAY_CONTOUR_MINVALUE, 0, nFields);
  int32_t contourMaxValue = m_params.CheckCount(kwDISPLAY_CONTOUR_MAXVALUE, 0, nFields);
  int32_t solidColor = m_params.CheckCount(kwDISPLAY_SOLID_COLOR, 0, nFields);

  int32_t eastComponent = m_params.CheckCount(kwDISPLAY_EAST_COMPONENT, 0, nFields);
  int32_t northComponent = m_params.CheckCount(kwDISPLAY_NORTH_COMPONENT, 0, nFields);
  //int32_t gridLabels = m_params.CheckCount(kwDISPLAY_GRID_LABELS, 0, nFields);
  int32_t name = m_params.CheckCount(kwDISPLAY_NAME, 0, nFields);

  if (title != 0)
  {
    m_params.m_mapParam[kwDISPLAY_TITLE]->GetValue(stringValue);
    wPlotProperty.m_title = stringValue.c_str();
  }
  if (showProp != 0)
  {
    m_params.m_mapParam[kwDISPLAY_PROPERTIES]->GetValue(boolValue);
    wPlotProperty.m_showPropertyPanel = boolValue;
  }
  if (showColorBar != 0)
  {
    m_params.m_mapParam[kwDISPLAY_COLORBAR]->GetValue(boolValue);
    wPlotProperty.m_showColorBar = boolValue;
  }
  if (showAnimationBar != 0)
  {
    m_params.m_mapParam[kwDISPLAY_ANIMATIONBAR]->GetValue(boolValue);
    wPlotProperty.m_showAnimationToolbar = boolValue;
  }
  if (centerLat != 0)
  {
    m_params.m_mapParam[kwDISPLAY_CENTERLAT]->GetValue(doubleValue);
    wPlotProperty.m_centerLatitude = doubleValue;
  }
  if (centerLon != 0)
  {
    m_params.m_mapParam[kwDISPLAY_CENTERLON]->GetValue(doubleValue);
    wPlotProperty.m_centerLongitude = doubleValue;
  }
  if (projection != 0)
  {
    m_params.m_mapParam[kwDISPLAY_PROJECTION]->GetValue(stringValue);
    wPlotProperty.m_projection = CMapProjection::GetInstance()->NameToLabeledName(stringValue);
    if (wPlotProperty.m_projection.empty())
    {
      throw  CParameterException(CTools::Format("Unknown projection name value '%s' for  parameter '%s'",
					        stringValue.c_str(),
					        kwDISPLAY_PROJECTION.c_str()),
				 BRATHL_SYNTAX_ERROR);

    }
  }
  if (coastResolution != 0)
  {
    m_params.m_mapParam[kwDISPLAY_COASTRESOLUTION]->GetValue(stringValue);
    wPlotProperty.m_coastResolution = CTools::StringToLower(stringValue);
  }
  if (zoomLon1 != 0)
  {
    m_params.m_mapParam[kwDISPLAY_ZOOM_LON1]->GetValue(doubleValue);
    wPlotProperty.m_zoomLon1 = doubleValue;
  }
  if (zoomLon2 != 0)
  {
    m_params.m_mapParam[kwDISPLAY_ZOOM_LON2]->GetValue(doubleValue);
    wPlotProperty.m_zoomLon2 = doubleValue;
  }
  if (zoomLat1 != 0)
  {
    m_params.m_mapParam[kwDISPLAY_ZOOM_LAT1]->GetValue(doubleValue);
    wPlotProperty.m_zoomLat1 = doubleValue;
  }
  if (zoomLat2 != 0)
  {
    m_params.m_mapParam[kwDISPLAY_ZOOM_LAT2]->GetValue(doubleValue);
    wPlotProperty.m_zoomLat2 = doubleValue;
  }
  if ( (zoomLon1 != 0) || (zoomLon2 != 0) || (zoomLat1 != 0) || (zoomLat2 != 0) )
  {
    wPlotProperty.m_zoom = true;
  }

  if (withAnimation != 0)
  {
    m_params.m_mapParam[kwDISPLAY_ANIMATION]->GetValue(boolValue);
    wPlotProperty.m_withAnimation = boolValue;

    // if 'withAnimation' is true, force animation bar to be shown.
    if (wPlotProperty.m_withAnimation)
    {
      wPlotProperty.m_showAnimationToolbar = wPlotProperty.m_withAnimation;
    }
  }

  for (i = 0 ; i < nFields ; i++)
  {
    m_wPlotProperties.Insert(new CWorldPlotProperties(wPlotProperty));
  }

  //------------------------------------------
  // Get max value Property for each plot
  //------------------------------------------
  for (i = 0 ; i < maxHeigth ; i++)
  {
    CWorldPlotProperties* props = GetWorldPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_MAXVALUE]->GetValue(doubleValue, i);
    props->m_maxHeightValue = doubleValue;
  }
  //------------------------------------------
  // Get min value Property for each plot
  //------------------------------------------
  for (i = 0 ; i < minHeigth ; i++)
  {
    CWorldPlotProperties* props = GetWorldPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_MINVALUE]->GetValue(doubleValue, i);
    props->m_minHeightValue = doubleValue;
  }
  //------------------------------------------
  // Get opacity Property for each plot
  //------------------------------------------
  for (i = 0 ; i < opacity ; i++)
  {
    CWorldPlotProperties* props = GetWorldPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_OPACITY]->GetValue(doubleValue, i);
    props->m_opacity = doubleValue;
  }
  //------------------------------------------
  // Get number of color labels Property for each plot
  //------------------------------------------
  for (i = 0 ; i < numColorLabels ; i++)
  {
    CWorldPlotProperties* props = GetWorldPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_NUMCOLORLABELS]->GetValue(intValue, i);
    props->m_numColorLabels = intValue;
  }
  //------------------------------------------
  // Get color Curve Property for each plot
  //------------------------------------------
  for (i = 0 ; i < colorCurve ; i++)
  {
    CWorldPlotProperties* props = GetWorldPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_COLORCURVE]->GetValue(stringValue, i);
    std::string stringValueOk = wPlotProperty.m_LUT->CurveToLabeledCurve(stringValue);
    if (stringValueOk.empty())
    {
      throw  CParameterException(CTools::Format("Unknown color curve name value '%s' for  parameter '%s'",
					        stringValue.c_str(),
					        kwDISPLAY_COLORCURVE.c_str()),
			         BRATHL_SYNTAX_ERROR);

    }
    props->m_LUT->ExecCurveMethod(stringValueOk);
  }
  //------------------------------------------
  // Get color table Property for each plot
  //------------------------------------------
  for (i = 0 ; i < colorTable ; i++)
  {
    CWorldPlotProperties* props = GetWorldPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_COLORTABLE]->GetValue(stringValue, i, PALETTE_AEROSOL);
    std::string stringValueOk = wPlotProperty.m_LUT->MethodToLabeledMethod(stringValue);

    if (stringValueOk.empty())
    {
      wxFileName fileName;
      fileName.Assign(stringValue.c_str());
      fileName.Normalize();

      if (wxFileName::FileExists(fileName.GetFullPath()) == false)
      {
        throw  CParameterException(CTools::Format("Unknown color table name value or color file name '%s' for  parameter '%s'",
	  				          stringValue.c_str(),
					          kwDISPLAY_COLORTABLE.c_str()),
			            BRATHL_SYNTAX_ERROR);
      }
      else
      {
        // load the color table file definition
        try
        {
          props->m_LUT->LoadFromFile(fileName.GetFullPath().ToStdString());
        }
        catch(CException& e)
        {
          ::wxMessageBox(e.what(), "Load error");
        }
        catch(...)
        {
          ::wxMessageBox(wxString::Format("Can't load %s",
                                          fileName.GetFullPath().c_str()),
                         "Load error");
        }
      }
    }
    else  //stringValueOk is not empty, it's a predefined color
    {
      props->m_LUT->ExecMethod(stringValueOk);
    }
  }
  //------------------------------------------
  // Get contour Property for each plot
  //------------------------------------------
  for (i = 0 ; i < contour ; i++)
  {
    CWorldPlotProperties* props = GetWorldPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_CONTOUR]->GetValue(boolValue, i);
    props->m_withContour = boolValue;
  }
  //------------------------------------------
  // Get number of contours Property for each plot
  //------------------------------------------
  for (i = 0 ; i < contourNum ; i++)
  {
    CWorldPlotProperties* props = GetWorldPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_CONTOUR_NUMBER]->GetValue(intValue, i);
    props->m_numContour = intValue;
  }
  //------------------------------------------
  // Get contour Label Property for each plot
  //------------------------------------------
  for (i = 0 ; i < contourLabel ; i++)
  {
    CWorldPlotProperties* props = GetWorldPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_CONTOUR_LABEL]->GetValue(boolValue, i);
    props->m_withContourLabel = boolValue;
  }
  //------------------------------------------
  // Get number of contour label Property for each plot
  //------------------------------------------
  for (i = 0 ; i < contourLabelNumber ; i++)
  {
    CWorldPlotProperties* props = GetWorldPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_CONTOUR_LABEL_NUMBER]->GetValue(intValue, i);
    props->m_numContourLabel = intValue;
  }
  //------------------------------------------
  // Get contour min value Property for each plot
  //------------------------------------------
  for (i = 0 ; i < contourMinValue ; i++)
  {
    CWorldPlotProperties* props = GetWorldPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_CONTOUR_MINVALUE]->GetValue(doubleValue, i);
    props->m_minContourValue = doubleValue;
  }
  //------------------------------------------
  // Get contour max value Property for each plot
  //------------------------------------------
  for (i = 0 ; i < contourMaxValue ; i++)
  {
    CWorldPlotProperties* props = GetWorldPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_CONTOUR_MAXVALUE]->GetValue(doubleValue, i);
    props->m_maxContourValue = doubleValue;
  }
  //------------------------------------------
  // Get solid color Property for each plot
  //------------------------------------------
  for (i = 0 ; i < solidColor ; i++)
  {
    CWorldPlotProperties* props = GetWorldPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_SOLID_COLOR]->GetValue(boolValue, i);
    props->m_solidColor = boolValue;
  }


  //-------------------------------------------------------------
  // Are the plots std::vector components? Just one east and one north
  //-------------------------------------------------------------
  for (i = 0 ; i < eastComponent ; i++)
  {
    CWorldPlotProperties* props = GetWorldPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_EAST_COMPONENT]->GetValue(boolValue, i);
    props->m_eastComponent = boolValue;
    /*if ( boolValue )
        break;*/
  }

  for (i = 0 ; i < northComponent ; i++)
  {
    CWorldPlotProperties* props = GetWorldPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_NORTH_COMPONENT]->GetValue(boolValue, i);
    props->m_northComponent = boolValue;
    /*if ( boolValue )
        break;*/
  }


  //------------------------------------------
  // Get Name Property for each plot
  //------------------------------------------
  for (i = 0 ; i < name ; i++)
  {
    CWorldPlotProperties* props = GetWorldPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_NAME]->GetValue(stringValue, i);
    props->m_name = stringValue.c_str();
  }


}

//----------------------------------------
void CBratDisplayApp::GetZFXYPlotPropertyParams(int32_t nFields)
{
  int32_t i = 0;
  bool boolValue;
  double doubleValue;
  std::string stringValue;
  int32_t intValue;
  uint32_t uintValue;

  CZFXYPlotProperties zfxyPlotProperty;
  //CMapColor::GetInstance().ResetPrimaryColors();

  if (nFields <= 0)
  {
    throw  CParameterException("CBratDisplayApp::GetZFXYPlotPropertyParams - There is no field to plot",
			       BRATHL_INCONSISTENCY_ERROR);
  }

  int32_t title = m_params.CheckCount(kwDISPLAY_TITLE, 0, 1);
  int32_t showProp = m_params.CheckCount(kwDISPLAY_PROPERTIES, 0, 1);
  int32_t showColorBar = m_params.CheckCount(kwDISPLAY_COLORBAR, 0, 1);
  int32_t showAnimationBar = m_params.CheckCount(kwDISPLAY_ANIMATIONBAR, 0, 1);


  int32_t xMin = m_params.CheckCount(kwDISPLAY_XMINVALUE, 0, 1);
  int32_t xMax = m_params.CheckCount(kwDISPLAY_XMAXVALUE, 0, 1);
  int32_t yMin = m_params.CheckCount(kwDISPLAY_YMINVALUE, 0, 1);
  int32_t yMax = m_params.CheckCount(kwDISPLAY_YMAXVALUE, 0, 1);
  int32_t xTicks = m_params.CheckCount(kwDISPLAY_XTICKS, 0, 1);
  int32_t yTicks = m_params.CheckCount(kwDISPLAY_YTICKS, 0, 1);

  int32_t withAnimation = m_params.CheckCount(kwDISPLAY_ANIMATION, 0, 1);

  int32_t yLabel = m_params.CheckCount(kwDISPLAY_YLABEL, 0, 1);

  int32_t minHeigth = m_params.CheckCount(kwDISPLAY_MINVALUE, 0, nFields);
  int32_t maxHeigth = m_params.CheckCount(kwDISPLAY_MAXVALUE, 0, nFields);
  int32_t opacity = m_params.CheckCount(kwDISPLAY_OPACITY, 0, nFields);
  int32_t numColorLabels = m_params.CheckCount(kwDISPLAY_NUMCOLORLABELS, 0, nFields);
  int32_t colorTable = m_params.CheckCount(kwDISPLAY_COLORTABLE, 0, nFields);
  int32_t colorCurve = m_params.CheckCount(kwDISPLAY_COLORCURVE, 0, nFields);
  int32_t contour = m_params.CheckCount(kwDISPLAY_CONTOUR, 0, nFields);
  int32_t contourNum = m_params.CheckCount(kwDISPLAY_CONTOUR_NUMBER, 0, nFields);
  int32_t contourLabel = m_params.CheckCount(kwDISPLAY_CONTOUR_LABEL, 0, nFields);
  int32_t contourLabelNumber = m_params.CheckCount(kwDISPLAY_CONTOUR_LABEL_NUMBER, 0, nFields);
  int32_t contourMinValue = m_params.CheckCount(kwDISPLAY_CONTOUR_MINVALUE, 0, nFields);
  int32_t contourMaxValue = m_params.CheckCount(kwDISPLAY_CONTOUR_MAXVALUE, 0, nFields);
  int32_t solidColor = m_params.CheckCount(kwDISPLAY_SOLID_COLOR, 0, nFields);

  int32_t name = m_params.CheckCount(kwDISPLAY_NAME, 0, nFields);

  if (yLabel != 0)
  {
     m_params.m_mapParam[kwDISPLAY_YLABEL]->GetValue(stringValue);
     zfxyPlotProperty.m_yLabel = stringValue.c_str();
  }

  if (title != 0)
  {
    m_params.m_mapParam[kwDISPLAY_TITLE]->GetValue(stringValue);
    zfxyPlotProperty.m_title = stringValue.c_str();
  }
  if (showProp != 0)
  {
    m_params.m_mapParam[kwDISPLAY_PROPERTIES]->GetValue(boolValue);
    zfxyPlotProperty.m_showPropertyPanel = boolValue;
  }
  if (showColorBar != 0)
  {
    m_params.m_mapParam[kwDISPLAY_COLORBAR]->GetValue(boolValue);
    zfxyPlotProperty.m_showColorBar = boolValue;
  }
  if (showAnimationBar != 0)
  {
    m_params.m_mapParam[kwDISPLAY_ANIMATIONBAR]->GetValue(boolValue);
    zfxyPlotProperty.m_showAnimationToolbar = boolValue;
  }
  if (xMin != 0)
  {
     m_params.m_mapParam[kwDISPLAY_XMINVALUE]->GetValue(doubleValue);
     zfxyPlotProperty.m_xMin = doubleValue;
  }
  if (xMax != 0)
  {
     m_params.m_mapParam[kwDISPLAY_XMAXVALUE]->GetValue(doubleValue);
     zfxyPlotProperty.m_xMax = doubleValue;
  }
  if (yMin != 0)
  {
     m_params.m_mapParam[kwDISPLAY_YMINVALUE]->GetValue(doubleValue);
     zfxyPlotProperty.m_yMin = doubleValue;
  }
  if (yMax != 0)
  {
     m_params.m_mapParam[kwDISPLAY_YMAXVALUE]->GetValue(doubleValue);
     zfxyPlotProperty.m_yMax = doubleValue;
  }
  if (xTicks != 0)
  {
     m_params.m_mapParam[kwDISPLAY_XTICKS]->GetValue(uintValue);
     zfxyPlotProperty.m_xNumTicks = uintValue;
  }
  if (yTicks != 0)
  {
     m_params.m_mapParam[kwDISPLAY_YTICKS]->GetValue(uintValue);
     zfxyPlotProperty.m_yNumTicks = uintValue;
  }

  if (withAnimation != 0)
  {
    m_params.m_mapParam[kwDISPLAY_ANIMATION]->GetValue(boolValue);
    zfxyPlotProperty.m_withAnimation = boolValue;

    // if 'withAnimation' is true, force animation bar to be shown.
    if (zfxyPlotProperty.m_withAnimation)
    {
      zfxyPlotProperty.m_showAnimationToolbar = zfxyPlotProperty.m_withAnimation;
    }
  }


  for (i = 0 ; i < nFields ; i++)
  {
    m_zfxyPlotProperties.Insert(new CZFXYPlotProperties(zfxyPlotProperty));
  }

  //------------------------------------------
  // Get max value Property for each plot
  //------------------------------------------
  for (i = 0 ; i < maxHeigth ; i++)
  {
    CZFXYPlotProperties* props = GetZFXYPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_MAXVALUE]->GetValue(doubleValue, i);
    props->m_maxHeightValue = doubleValue;
  }
  //------------------------------------------
  // Get min value Property for each plot
  //------------------------------------------
  for (i = 0 ; i < minHeigth ; i++)
  {
    CZFXYPlotProperties* props = GetZFXYPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_MINVALUE]->GetValue(doubleValue, i);
    props->m_minHeightValue = doubleValue;
  }
  //------------------------------------------
  // Get opacity Property for each plot
  //------------------------------------------
  for (i = 0 ; i < opacity ; i++)
  {
    CZFXYPlotProperties* props = GetZFXYPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_OPACITY]->GetValue(doubleValue, i);
    props->m_opacity = doubleValue;
  }
  //------------------------------------------
  // Get number of color labels Property for each plot
  //------------------------------------------
  for (i = 0 ; i < numColorLabels ; i++)
  {
    CZFXYPlotProperties* props = GetZFXYPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_NUMCOLORLABELS]->GetValue(intValue, i);
    props->m_numColorLabels = intValue;
  }
  //------------------------------------------
  // Get color Curve Property for each plot
  //------------------------------------------
  for (i = 0 ; i < colorCurve ; i++)
  {
    CZFXYPlotProperties* props = GetZFXYPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_COLORCURVE]->GetValue(stringValue, i);
    std::string stringValueOk = zfxyPlotProperty.m_LUT->CurveToLabeledCurve(stringValue);
    if (stringValueOk.empty())
    {
      throw  CParameterException(CTools::Format("Unknown color curve name value '%s' for  parameter '%s'",
					        stringValue.c_str(),
					        kwDISPLAY_COLORCURVE.c_str()),
			         BRATHL_SYNTAX_ERROR);

    }
    props->m_LUT->ExecCurveMethod(stringValueOk);
  }
  //------------------------------------------
  // Get color table Property for each plot
  //------------------------------------------
  for (i = 0 ; i < colorTable ; i++)
  {
    CZFXYPlotProperties* props = GetZFXYPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_COLORTABLE]->GetValue(stringValue, i, PALETTE_AEROSOL);
    std::string stringValueOk = zfxyPlotProperty.m_LUT->MethodToLabeledMethod(stringValue);

    if (stringValueOk.empty())
    {
      wxFileName fileName;
      fileName.Assign(stringValue.c_str());
      fileName.Normalize();

      if (wxFileName::FileExists(fileName.GetFullPath()) == false)
      {
        throw  CParameterException(CTools::Format("Unknown color table name value or color file name '%s' for  parameter '%s'",
	  				          stringValue.c_str(),
					          kwDISPLAY_COLORTABLE.c_str()),
			            BRATHL_SYNTAX_ERROR);
      }
      else
      {
        // load the color table file definition
        try
        {
          props->m_LUT->LoadFromFile(fileName.GetFullPath().ToStdString());
        }
        catch(CException& e)
        {
          ::wxMessageBox(e.what(), "Load error");
        }
        catch(...)
        {
          ::wxMessageBox(wxString::Format("Can't load %s",
                                          fileName.GetFullPath().c_str()),
                         "Load error");
        }
      }
    }
    else  //stringValueOk is not empty, it's a predefined color
    {
      props->m_LUT->ExecMethod(stringValueOk);
    }
  }
  //------------------------------------------
  // Get contour Property for each plot
  //------------------------------------------
  for (i = 0 ; i < contour ; i++)
  {
    CZFXYPlotProperties* props = GetZFXYPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_CONTOUR]->GetValue(boolValue, i);
    props->m_withContour = boolValue;
  }
  //------------------------------------------
  // Get number of contours Property for each plot
  //------------------------------------------
  for (i = 0 ; i < contourNum ; i++)
  {
    CZFXYPlotProperties* props = GetZFXYPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_CONTOUR_NUMBER]->GetValue(intValue, i);
    props->m_numContour = intValue;
  }
  //------------------------------------------
  // Get contour Label Property for each plot
  //------------------------------------------
  for (i = 0 ; i < contourLabel ; i++)
  {
    CZFXYPlotProperties* props = GetZFXYPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_CONTOUR_LABEL]->GetValue(boolValue, i);
    props->m_withContourLabel = boolValue;
  }
  //------------------------------------------
  // Get number of contour label Property for each plot
  //------------------------------------------
  for (i = 0 ; i < contourLabelNumber ; i++)
  {
    CZFXYPlotProperties* props = GetZFXYPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_CONTOUR_LABEL_NUMBER]->GetValue(intValue, i);
    props->m_numContourLabel = intValue;
  }
  //------------------------------------------
  // Get contour min value Property for each plot
  //------------------------------------------
  for (i = 0 ; i < contourMinValue ; i++)
  {
    CZFXYPlotProperties* props = GetZFXYPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_CONTOUR_MINVALUE]->GetValue(doubleValue, i);
    props->m_minContourValue = doubleValue;
  }
  //------------------------------------------
  // Get contour max value Property for each plot
  //------------------------------------------
  for (i = 0 ; i < contourMaxValue ; i++)
  {
    CZFXYPlotProperties* props = GetZFXYPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_CONTOUR_MAXVALUE]->GetValue(doubleValue, i);
    props->m_maxContourValue = doubleValue;
  }
  //------------------------------------------
  // Get solid color Property for each plot
  //------------------------------------------
  for (i = 0 ; i < solidColor ; i++)
  {
    CZFXYPlotProperties* props = GetZFXYPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_SOLID_COLOR]->GetValue(boolValue, i);
    props->m_solidColor = boolValue;
  }



  //------------------------------------------
  // Get Name Property for each plot
  //------------------------------------------
  for (i = 0 ; i < name ; i++)
  {
    CZFXYPlotProperties* props = GetZFXYPlotProperty(i);
    m_params.m_mapParam[kwDISPLAY_NAME]->GetValue(stringValue, i);
    props->m_name = stringValue.c_str();
  }


}
/*
//----------------------------------------
void CBratDisplayApp::GetWPlotPropertyParams(CWorldPlotProperties& wPlotProperty)
{
  bool boolValue;
  double doubleValue;
  std::string stringValue;
  int32_t intValue;

  int32_t showProp = m_params.CheckCount(kwDISPLAY_PROPERTIES, 0, 1);
  int32_t showColorBar = m_params.CheckCount(kwDISPLAY_COLORBAR, 0, 1);
  int32_t showAnimationBar = m_params.CheckCount(kwDISPLAY_ANIMATIONBAR, 0, 1);
  int32_t centerLat = m_params.CheckCount(kwDISPLAY_CENTERLAT, 0, 1);
  int32_t centerLon = m_params.CheckCount(kwDISPLAY_CENTERLON, 0, 1);
  int32_t minHeigth = m_params.CheckCount(kwDISPLAY_MINVALUE, 0, 1);
  int32_t maxHeigth = m_params.CheckCount(kwDISPLAY_MAXVALUE, 0, 1);
  int32_t opacity = m_params.CheckCount(kwDISPLAY_OPACITY, 0, 1);
  int32_t projection = m_params.CheckCount(kwDISPLAY_PROJECTION, 0, 1);
  int32_t numColorLabels = m_params.CheckCount(kwDISPLAY_NUMCOLORLABELS, 0, 1);
  int32_t colorTable = m_params.CheckCount(kwDISPLAY_COLORTABLE, 0, 1);
  int32_t colorCurve = m_params.CheckCount(kwDISPLAY_COLORCURVE, 0, 1);
  int32_t coastResolution = m_params.CheckCount(kwDISPLAY_COASTRESOLUTION, 0, 1);
  int32_t zoomLon1 = m_params.CheckCount(kwDISPLAY_ZOOM_LON1, 0, 1);
  int32_t zoomLon2 = m_params.CheckCount(kwDISPLAY_ZOOM_LON2, 0, 1);
  int32_t zoomLat1 = m_params.CheckCount(kwDISPLAY_ZOOM_LAT1, 0, 1);
  int32_t zoomLat2 = m_params.CheckCount(kwDISPLAY_ZOOM_LAT2, 0, 1);
  int32_t contour = m_params.CheckCount(kwDISPLAY_CONTOUR, 0, 1);
  int32_t contourNum = m_params.CheckCount(kwDISPLAY_CONTOUR_NUMBER, 0, 1);
  int32_t contourLabel = m_params.CheckCount(kwDISPLAY_CONTOUR_LABEL, 0, 1);
  int32_t contourLabelNumber = m_params.CheckCount(kwDISPLAY_CONTOUR_LABEL_NUMBER, 0, 1);
  int32_t contourMinValue = m_params.CheckCount(kwDISPLAY_CONTOUR_MINVALUE, 0, 1);
  int32_t contourMaxValue = m_params.CheckCount(kwDISPLAY_CONTOUR_MAXVALUE, 0, 1);
  int32_t solidColor = m_params.CheckCount(kwDISPLAY_SOLID_COLOR, 0, 1);


  if (showProp != 0)
  {
     m_params.m_mapParam[kwDISPLAY_PROPERTIES]->GetValue(boolValue);
     wPlotProperty.m_showPropertyPanel = boolValue;
  }
  if (showColorBar != 0)
  {
     m_params.m_mapParam[kwDISPLAY_COLORBAR]->GetValue(boolValue);
     wPlotProperty.m_showColorBar = boolValue;
  }
  if (showAnimationBar != 0)
  {
   m_params.m_mapParam[kwDISPLAY_ANIMATIONBAR]->GetValue(boolValue);
   wPlotProperty.m_showAnimationToolbar = boolValue;
  }
  if (centerLat != 0)
  {
    m_params.m_mapParam[kwDISPLAY_CENTERLAT]->GetValue(doubleValue);
    wPlotProperty.m_centerLatitude = doubleValue;
  }
  if (centerLon != 0)
  {
    m_params.m_mapParam[kwDISPLAY_CENTERLON]->GetValue(doubleValue);
    wPlotProperty.m_centerLongitude = doubleValue;
  }
  if (minHeigth != 0)
  {
    m_params.m_mapParam[kwDISPLAY_MINVALUE]->GetValue(doubleValue);
    wPlotProperty.m_minHeightValue = doubleValue;
  }
  if (maxHeigth != 0)
  {
    m_params.m_mapParam[kwDISPLAY_MAXVALUE]->GetValue(doubleValue);
    wPlotProperty.m_maxHeightValue = doubleValue;
  }
  if (opacity != 0)
  {
    m_params.m_mapParam[kwDISPLAY_OPACITY]->GetValue(doubleValue);
    wPlotProperty.m_opacity = doubleValue;
  }
  if (projection != 0)
  {
    m_params.m_mapParam[kwDISPLAY_PROJECTION]->GetValue(stringValue);
    wPlotProperty.m_projection = CMapProjection::GetInstance()->NameToLabeledName(stringValue);
    if (wPlotProperty.m_projection.empty())
    {
      throw  CParameterException(CTools::Format("Unknown projection name value '%s' for  parameter '%s'",
					        stringValue.c_str(),
					        kwDISPLAY_PROJECTION.c_str()),
				 BRATHL_SYNTAX_ERROR);

    }
  }
  if (numColorLabels != 0)
  {
    m_params.m_mapParam[kwDISPLAY_NUMCOLORLABELS]->GetValue(intValue);
    wPlotProperty.m_numColorLabels = intValue;
  }
  if (colorCurve != 0)
  {
    m_params.m_mapParam[kwDISPLAY_COLORCURVE]->GetValue(stringValue);
    std::string stringValueOk = wPlotProperty.m_LUT->CurveToLabeledCurve(stringValue);
    if (stringValueOk.empty())
    {
      throw  CParameterException(CTools::Format("Unknown color curve name value '%s' for  parameter '%s'",
					    stringValue.c_str(),
					    kwDISPLAY_COLORCURVE.c_str()),
			     BRATHL_SYNTAX_ERROR);

    }
    wPlotProperty.m_LUT->ExecCurveMethod(stringValueOk);
  }
  if (colorTable != 0)
  {
    m_params.m_mapParam[kwDISPLAY_COLORTABLE]->GetValue(stringValue);
    //??? FromFile
    std::string stringValueOk = wPlotProperty.m_LUT->MethodToLabeledMethod(stringValue);

    if (stringValueOk.empty())
    {
      wxFileName fileName;
      fileName.Assign(stringValue.c_str());
      fileName.Normalize();

      if (wxFileName::FileExists(fileName.GetFullPath()) == false)
      {
        throw  CParameterException(CTools::Format("Unknown color table name value or color file name '%s' for  parameter '%s'",
	  				          stringValue.c_str(),
					          kwDISPLAY_COLORTABLE.c_str()),
			            BRATHL_SYNTAX_ERROR);
      }
      else
      {
        // load the color table file definition
        try
        {
          wPlotProperty.m_LUT->LoadFromFile(fileName.GetFullPath());
        }
        catch(CException& e)
        {
          ::wxMessageBox(e.what(), "Load error");

        }
        catch(...)
        {

          ::wxMessageBox(wxString::Format("Can't load %s",
                                          fileName.GetFullPath().c_str()),
                         "Load error");
        }
      }

    }
    else  //stringValueOk is not empty, it's a predefined color
    {
      wPlotProperty.m_LUT->ExecMethod(stringValueOk);
    }
  }
  if (coastResolution != 0)
  {
    m_params.m_mapParam[kwDISPLAY_COASTRESOLUTION]->GetValue(stringValue);
    wPlotProperty.m_coastResolution = CTools::StringToLower(stringValue);
  }

  if (zoomLon1 != 0)
  {
    m_params.m_mapParam[kwDISPLAY_ZOOM_LON1]->GetValue(doubleValue);
    wPlotProperty.m_zoomLon1 = doubleValue;
  }
  if (zoomLon2 != 0)
  {
    m_params.m_mapParam[kwDISPLAY_ZOOM_LON2]->GetValue(doubleValue);
    wPlotProperty.m_zoomLon2 = doubleValue;
  }
  if (zoomLat1 != 0)
  {
    m_params.m_mapParam[kwDISPLAY_ZOOM_LAT1]->GetValue(doubleValue);
    wPlotProperty.m_zoomLat1 = doubleValue;
  }
  if (zoomLat2 != 0)
  {
    m_params.m_mapParam[kwDISPLAY_ZOOM_LAT2]->GetValue(doubleValue);
    wPlotProperty.m_zoomLat2 = doubleValue;
  }
  if ( (zoomLon1 != 0) || (zoomLon2 != 0) || (zoomLat1 != 0) || (zoomLat2 != 0) )
  {
    wPlotProperty.m_zoom = true;
  }

  if (contour != 0)
  {
    m_params.m_mapParam[kwDISPLAY_CONTOUR]->GetValue(boolValue);
    wPlotProperty.m_withContour = boolValue;
  }
  if (contourNum != 0)
  {
    m_params.m_mapParam[kwDISPLAY_CONTOUR_NUMBER]->GetValue(intValue);
    wPlotProperty.m_numContour = intValue;
  }
  if (contourLabel != 0)
  {
    m_params.m_mapParam[kwDISPLAY_CONTOUR_LABEL]->GetValue(boolValue);
    wPlotProperty.m_withContourLabel = boolValue;
  }
  if (contourLabelNumber != 0)
  {
    m_params.m_mapParam[kwDISPLAY_CONTOUR_LABEL_NUMBER]->GetValue(intValue);
    wPlotProperty.m_numContourLabel = intValue;
  }

  if (contourMinValue != 0)
  {
    m_params.m_mapParam[kwDISPLAY_CONTOUR_MINVALUE]->GetValue(doubleValue);
    wPlotProperty.m_minContourValue = doubleValue;
  }
  if (contourMaxValue != 0)
  {
    m_params.m_mapParam[kwDISPLAY_CONTOUR_MAXVALUE]->GetValue(doubleValue);
    wPlotProperty.m_maxContourValue = doubleValue;
  }
  if (solidColor != 0)
  {
    m_params.m_mapParam[kwDISPLAY_SOLID_COLOR]->GetValue(boolValue);
    wPlotProperty.m_solidColor = boolValue;
  }

}
*/

//----------------------------------------
bool CBratDisplayApp::GetParametersNetcdf()
{
  bool bOk = false;

  CExternalFiles* externalFileTmp = NULL;

  try
  {
    // Test if file is a Netcdf file
    externalFileTmp = BuildExistingExternalFileKind(m_paramFile);
    if (externalFileTmp != NULL)
    {
      bOk = true;
    }
  }
  catch(CFileException &e)
  {
    // it's not a netcdf file (it's a parameters file) or not a valid netcdf file
    //Do Nothing
    e.what(); // to avoid warning compilation error
  }
  catch(CException& e)
  {
    throw (e);
  }
  catch(...)
  {
    throw;
  }

  //-----------------
  if (!bOk)
  {
    return bOk;
  }
  //-----------------

  CExternalFilesNetCDF* externalFile = dynamic_cast<CExternalFilesNetCDF*>(externalFileTmp);

  if (externalFile == NULL)
  {
    std::string msg = CTools::Format("ERROR - The input file '%s' is not a NetCdf file",
                                 externalFile->GetName().c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }

  // Open the Netcdf file
  externalFile->Open();

  m_inputFiles.Insert(m_paramFile);

  if  ( ( (!m_paramXAxis.empty()) && (!m_paramYAxis.empty()) )
    ||  ( (m_paramXAxis.empty()) && (m_paramYAxis.empty()) ) )
  {

    CFieldNetCdf* fieldLat = externalFile->FindLatField();
    CFieldNetCdf* fieldLon = externalFile->FindLonField();

    if ((fieldLat != NULL) && (fieldLon != NULL))
    {
      bOk = GetParametersNetcdfZFLatLon(externalFile);
    }
    else
    {
      bOk = GetParametersNetcdfZFXY(externalFile);
    }


  }
  else
  {
    bOk = GetParametersNetcdfYFX(externalFile);
  }

  externalFile->Close();
  delete externalFile;
  externalFile = NULL;

  return bOk;
}

//----------------------------------------
bool CBratDisplayApp::GetParametersNetcdfZFXY(CExternalFilesNetCDF* externalFile)
{

  CNetCDFFiles* netCDFFile = externalFile->GetFile();

  if (netCDFFile == NULL)
  {
    std::string msg = CTools::Format("ERROR - The input file '%s' is not a NetCdf file (internal file is NULL)",
                                 externalFile->GetName().c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }


  uint32_t groupNumber = 0;

  m_inputFileType = CInternalFilesZFXY::TypeOf();

  m_plots.resize(groupNumber + 1);

  int32_t nFields = m_paramVars.size();

  CFieldNetCdf* fieldX = NULL;
  CFieldNetCdf* fieldY = NULL;

  bool bFoundLatDim = false;
  bool bFoundLonDim = false;

  std::string coordAxisX;
  std::string coordAxisY;

  if (nFields == 0)
  {
    CStringArray varNames;
    externalFile->GetVariables(varNames);
    CStringArray::const_iterator it;

    for (it = varNames.begin() ; it != varNames.end() ; it++)
    {
      bFoundLatDim = false;
      bFoundLonDim = false;

      if (externalFile->IsAxisVar((*it)))
      {
        continue;
      }

      CStringArray dimNames;
      externalFile->GetDimensions((*it), dimNames);

      if (dimNames.size() != 2)
      {
        continue;
      }

      fieldX = externalFile->GetFieldNetCdf(dimNames.at(0));
      fieldY = externalFile->GetFieldNetCdf(dimNames.at(1));

      if ( (fieldX == NULL) || (fieldY == NULL) )
      {
        continue;
      }

      m_paramVars.Insert((*it));
      nFields = m_paramVars.size();
      break;

    }

    if (m_paramVars.size() <= 0)
    {
      std::string msg = CTools::Format("CBratDisplayApp::GetParametersNetcdfZFXY - No field with at least one dimension to display has been found in file '%s'",
                                   externalFile->GetName().c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }
  }




  if (fieldX->GetDimIds().size() != 1)
  {
    std::string msg = CTools::Format("CBratDisplayApp::GetParametersNetcdfZFXY : Array of field '%s' is not 1 dimensional.",
                               fieldX->GetName().c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }

  if (fieldY->GetDimIds().size() != 1)
  {
    std::string msg = CTools::Format("CBratDisplayApp::GetParametersNetcdfZFXY : Array of field '%s' is not 1 dimensional.",
                               fieldY->GetName().c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }


  CStringList::const_iterator itParamVar;

  for (itParamVar = m_paramVars.begin() ; itParamVar != m_paramVars.end() ; itParamVar++)
  {
    CUIntArray dimValues;
    externalFile->GetDimensions((*itParamVar), dimValues);


    if (dimValues.size() != 2 )
    {
      std::string msg = CTools::Format("CBratDisplayApp::GetParametersNetcdfZFXY : Array of field '%s' is not 2 dimensional.",
                                 (*itParamVar).c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      throw (e);
    }
  }


  CNetCDFVarDef* firstNetCDFVardef = NULL;

  for (itParamVar = m_paramVars.begin() ; itParamVar != m_paramVars.end() ; itParamVar++)
  {
    CNetCDFVarDef* netCDFVarDef = netCDFFile->GetNetCDFVarDef(*itParamVar);

    if (firstNetCDFVardef == NULL)
    {
      continue;
    }

    if (netCDFVarDef->GetNbDims() != 2 )
    {
      std::string msg = CTools::Format("CBratDisplayApp::GetParametersNetcdfZFXY : Array of field '%s' is not 2 dimensional.",
                                 (*itParamVar).c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      throw (e);
    }

    if (firstNetCDFVardef == NULL)
    {
      firstNetCDFVardef = netCDFVarDef;
      continue;
    }


    if ( ! firstNetCDFVardef->HaveEqualDims(netCDFVarDef))
    {
      std::string msg = CTools::Format("ERROR : All the fields to plot have not the same dimensions.");
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      throw (e);
    }

  }


  m_fields.resize(nFields);

  GetZFXYPlotPropertyParams(nFields);

  uint32_t index = 0;

  for (itParamVar = m_paramVars.begin() ; itParamVar != m_paramVars.end() ; itParamVar++)
  {
    // Add field to ZFXY plot group
    CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>(m_plots[groupNumber]);
    if (zfxyplot == NULL)
    {
      zfxyplot = new CZFXYPlot(groupNumber);
      m_plots[groupNumber] = zfxyplot;
    }

    CPlotField* field = new CPlotField((*itParamVar).c_str());
    zfxyplot->m_fields.Insert(field);


    if (m_paramXAxis.empty() == false)
    {
      zfxyplot->SetForcedVarXname(m_paramXAxis.c_str());
    }

    if (m_paramYAxis.empty() == false)
    {
      zfxyplot->SetForcedVarYname(m_paramYAxis.c_str());
    }


    CInternalFiles* zfxy = Prepare(externalFile->GetName().c_str(), (*itParamVar).c_str());

    field->m_internalFiles.Insert(zfxy);
    field->m_zfxyProps = GetZFXYPlotProperty(index);

    m_fields[index] = (*itParamVar);

    index++;
  }

  m_isZFXY = true;

  return true;




}

//----------------------------------------
bool CBratDisplayApp::GetParametersNetcdfZFLatLon(CExternalFilesNetCDF* externalFile)
{

  uint32_t groupNumber = 0;

  m_inputFileType = CInternalFilesZFXY::TypeOf();

  m_plots.resize(groupNumber + 1);

  int32_t nFields = m_paramVars.size();

  CFieldNetCdf* fieldLat = externalFile->FindLatField();
  CFieldNetCdf* fieldLon = externalFile->FindLonField();

  if (fieldLat == NULL)
  {
    std::string msg = CTools::Format("CBratDisplayApp::GetParametersNetcdfZFLatLon - No Latitude field has been found in file '%s'",
                                 externalFile->GetName().c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  if (fieldLon == NULL)
  {
    std::string msg = CTools::Format("CBratDisplayApp::GetParametersNetcdfZFLatLon - No Longitude field has been found in file '%s'",
                                 externalFile->GetName().c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }
  bool bFoundLatDim = false;
  bool bFoundLonDim = false;

  if (nFields == 0)
  {
    CStringArray varNames;
    externalFile->GetVariables(varNames);
    CStringArray::const_iterator it;

    for (it = varNames.begin() ; it != varNames.end() ; it++)
    {
      bFoundLatDim = false;
      bFoundLonDim = false;

      if (externalFile->IsAxisVar((*it)))
      {
        continue;
      }

      CUIntArray dimValues;
      externalFile->GetDimensions((*it), dimValues);

      if (dimValues.size() != 2)
      {
        continue;
      }

      CStringArray dimNames;
      externalFile->GetDimensions((*it), dimNames);


      CStringArray::const_iterator itDimNames;

      for (itDimNames = dimNames.begin() ; itDimNames != dimNames.end(); itDimNames++)
      {
        if ((*itDimNames).compare(fieldLat->GetName()))
        {
          bFoundLatDim = true;
        }
        if ((*itDimNames).compare(fieldLon->GetName()))
        {
          bFoundLonDim = true;
        }

      }

      if ( (!bFoundLatDim) || (!bFoundLonDim) )
      {
        continue;
      }

      m_paramVars.Insert((*it));
      nFields = m_paramVars.size();
      break;

    }

    if (m_paramVars.size() <= 0)
    {
      std::string msg = CTools::Format("CBratDisplayApp::GetParametersNetcdfZFLatLon - No field with at least one dimension to display has been found in file '%s'",
                                   externalFile->GetName().c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }
  }




  if (fieldLat->GetDimIds().size() != 1)
  {
    std::string msg = CTools::Format("CBratDisplayApp::GetParametersNetcdfZFLatLon : Array of field '%s' is not 1 dimensional",
                               fieldLat->GetName().c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }

  if (fieldLon->GetDimIds().size() != 1)
  {
    std::string msg = CTools::Format("CBratDisplayApp::GetParametersNetcdfZFLatLon : Array of field '%s' is not 1 dimensional",
                               fieldLon->GetName().c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }


  CStringList::const_iterator itParamVar;

  for (itParamVar = m_paramVars.begin() ; itParamVar != m_paramVars.end() ; itParamVar++)
  {
    CUIntArray dimValues;
    externalFile->GetDimensions((*itParamVar), dimValues);

    if (dimValues.size() != 2 )
    {
      std::string msg = CTools::Format("CBratDisplayApp::GetParametersNetcdfZFXY : Array of field '%s' is not 2 dimensional",
                                 (*itParamVar).c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      throw (e);
    }
  }


  m_fields.resize(nFields);

  GetWPlotPropertyParams(nFields);

  uint32_t index = 0;

  for (itParamVar = m_paramVars.begin() ; itParamVar != m_paramVars.end() ; itParamVar++)
  {
    // Add field to world plot group
    CWPlot* wplot = dynamic_cast<CWPlot*>(m_plots[groupNumber]);
    if (wplot == NULL)
    {
      wplot = new CWPlot(groupNumber);
      m_plots[groupNumber] = wplot;
    }

    CPlotField* field = new CPlotField((*itParamVar).c_str());
    wplot->m_fields.Insert(field);


    CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>(Prepare(externalFile->GetName().c_str(), (*itParamVar).c_str()));

    field->m_internalFiles.Insert(zfxy);
    field->m_worldProps = GetWorldPlotProperty(index);

    m_fields[index] = (*itParamVar);

    index++;
  }

  m_isZFLatLon = true;

  return true;


}
//----------------------------------------
bool CBratDisplayApp::GetParametersNetcdfYFX(CExternalFilesNetCDF* externalFile)
{
  uint32_t groupNumber = 0;

  m_inputFileType = CInternalFilesYFX::TypeOf();

  m_plots.resize(groupNumber + 1);

  int32_t nFields = m_paramVars.size();

  if (nFields == 0)
  {
    CStringArray varNames;
    externalFile->GetVariables(varNames);
    CStringArray::const_iterator it;

    for (it = varNames.begin() ; it != varNames.end() ; it++)
    {
      if ((*it).compare(m_paramXAxis.c_str()) == 0)
      {
        continue;
      }

      if ((*it).compare(m_paramYAxis.c_str()) == 0)
      {
        continue;
      }

      if (externalFile->IsAxisVar((*it)))
      {
        continue;
      }

      CUIntArray dimValues;
      externalFile->GetDimensions((*it), dimValues);

      if ( (dimValues.size() < 1) ||(dimValues.size() > 2) )
      {
        continue;
      }

      m_paramVars.Insert((*it));
      nFields = m_paramVars.size();
      break;

    }

    if (m_paramVars.size() <= 0)
    {
      std::string msg = CTools::Format("CBratDisplayApp::GetParametersNetcdfYFX - No field with one dimension to display has been found in file '%s'",
                                   externalFile->GetName().c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }
  }

  if ( (m_paramYAxis.empty() == false) && (nFields > 1) )
  {
    m_paramXAxis = m_paramYAxis;
    m_paramYAxis = "";
  }

  if ( (m_paramYAxis.empty() == false) && (nFields == 1) )
  {
    m_paramXAxis = *(m_paramVars.begin());
    m_paramVars.RemoveAll();
    m_paramVars.Insert(m_paramYAxis);
  }


  CUIntArray xDimValues;
  externalFile->GetDimensions(m_paramXAxis, xDimValues);
  if (xDimValues.size() <= 0)
  {
    std::string msg = CTools::Format("CBratDisplayApp::GetParametersNetcdfYFX : Array of field '%s' is 0 dimensional - It have to be at least 1 or 2 dimensional",
                               m_paramXAxis.c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);

  }

  if (xDimValues.size() > 2)
  {
    std::string msg = CTools::Format("CBratDisplayApp::GetParametersNetcdfYFX : Array of field '%s' is more than 2 dimensional - It have to be 1 or 2 dimensional",
                               m_paramXAxis.c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    throw (e);
  }


  CStringList::const_iterator itParamVar;

  for (itParamVar = m_paramVars.begin() ; itParamVar != m_paramVars.end() ; itParamVar++)
  {
    CUIntArray dimValues;
    externalFile->GetDimensions((*itParamVar), dimValues);

    if (dimValues.size() <= 0)
    {
      std::string msg = CTools::Format("CBratDisplayApp::GetParametersNetcdfYFX : Array of field '%s' is 0 dimensional - It have to be at least 1 or 2 dimensional",
                                 (*itParamVar).c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      throw (e);

    }

    if (dimValues.size() > 2 )
    {
      std::string msg = CTools::Format("CBratDisplayApp::GetParametersNetcdfYFX : Array of field '%s' is more than 2 dimensional - It have to be 1 or 2 dimensional",
                                 (*itParamVar).c_str());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      throw (e);
    }

  }


  m_fields.resize(nFields);

  GetXYPlotPropertyParams(nFields);

  uint32_t index = 0;

  for (itParamVar = m_paramVars.begin() ; itParamVar != m_paramVars.end() ; itParamVar++)
  {
    // Add field to xy plot group
    CPlot* plot = dynamic_cast<CPlot*>(m_plots[groupNumber]);
    if (plot == NULL)
    {
      plot = new CPlot(groupNumber);
      m_plots[groupNumber] = plot;
    }
    CPlotField* field = new CPlotField((*itParamVar).c_str());
    plot->m_fields.Insert(field);

    if (m_paramXAxis.empty() == false)
    {
      plot->SetForcedVarXname(m_paramXAxis.c_str());
    }


    CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>(Prepare(externalFile->GetName().c_str(), (*itParamVar).c_str()));

    field->m_internalFiles.Insert(yfx);
    field->m_xyProps = GetXYPlotProperty(index);

    m_fields[index] = (*itParamVar);

    index++;
  }

  m_isYFX = true;

  return true;
}
//----------------------------------------
void CBratDisplayApp::LoadParameters()
{
  if (!m_params.IsLoaded())
  {
    m_params.Load(m_paramFile);
  }

}
//----------------------------------------
void CBratDisplayApp::GetParameters()
{
  //--------------------------------------------
  // Init parameters with a netcdf input file
  //--------------------------------------------
  if (GetParametersNetcdf())
  {
    //---------
    return;
    //---------
  }

  // Init parameters with a parameter input file
  uint32_t index = 0;

  LoadParameters();

  m_params.SetVerboseLevel();
  m_params.Dump();


// Verify keyword occurences
  uint32_t nbExpr = m_params.CheckCount(kwFIELD,  1, 32); // max 32 because of class CBitSet32


  // Get keyword values
  m_inputFiles.RemoveAll();
  CBratProcess::GetFileList(m_params, kwFILE, m_inputFiles, "Input file", 1, -1);

  // Check all input files
  CheckFiles();

  uint32_t groupNumber;
  uint32_t groupNumberMax = 0;
  uint32_t groupNumberMin = 0;
  setDefaultValue(groupNumberMin);

  int32_t nbGroup = m_params.CheckCount(kwFIELD_GROUP, 0, nbExpr);
  if (nbGroup != 0)
  {
    m_params.CheckCount(kwFIELD_GROUP, nbExpr, nbExpr);

    for (index = 0; index < nbExpr; index++)
    {
      m_params.m_mapParam[kwFIELD_GROUP]->GetValue(groupNumber, index);
      groupNumberMax = (groupNumber > groupNumberMax) ? groupNumber : groupNumberMax;
      groupNumberMin = (groupNumber < groupNumberMin) ? groupNumber : groupNumberMin;
    }
  }

  // get type of plot
  m_isYFX = IsXYPlot();
  m_isZFLatLon = IsWPlot();
  m_isZFXY = IsZXYPlot();

  if (m_isYFX && m_isZFXY)
  {
    uint32_t nPlotType = m_params.CheckCount(kwDISPLAY_PLOT_TYPE, 0, 1);
    if (nPlotType > 0)
    {
      uint32_t plotType;
      m_params.m_mapParam[kwDISPLAY_PLOT_TYPE]->GetValue(plotType);
      if (plotType == CMapTypeDisp::eTypeDispYFX)
      {
        m_isZFXY = false;
      }
      else
      {
        m_isYFX = false;
      }
    }
  }

  if (m_isYFX && m_isZFLatLon)
  {
    uint32_t nPlotType = m_params.CheckCount(kwDISPLAY_PLOT_TYPE, 0, 1);
    if (nPlotType > 0)
    {
      uint32_t plotType;
      m_params.m_mapParam[kwDISPLAY_PLOT_TYPE]->GetValue(plotType);
      if (plotType == CMapTypeDisp::eTypeDispYFX)
      {
        m_isZFLatLon = false;
      }
      else
      {
        m_isYFX = false;
      }
    }
  }


  m_plots.resize(groupNumberMax + 1);

  if (m_isYFX)
  {
     GetXYPlotPropertyParams(nbExpr);
  }

  if (m_isZFLatLon)
  {
    GetWPlotPropertyParams(nbExpr);
  }

  if (m_isZFXY)
  {
    GetZFXYPlotPropertyParams(nbExpr);
  }

  m_fields.resize(nbExpr);

  for (index = 0; index < nbExpr; index++)
  {
    CExpression expr;
    //CUnit* unit = new CUnit;

    CBratProcess::GetVarDef(m_params,
	        kwFIELD,
	        expr,
	        NULL,
	        NULL,
	        NULL,
	        NULL,
	        NULL,
	        NULL,
          &groupNumber,
	        "Data Field",
	        index,
	        nbExpr);


    if (expr.GetFieldNames()->size() != 1)
    {
      std::string msg = CTools::Format("CBratDisplayApp::GetParameters - Expression '%s' has incorrect number of fields '%ld' (correct is 1)",
                                   expr.AsString().c_str(), (long)expr.GetFieldNames()->size());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }

    std::string fieldName = expr.GetFieldNames()->at(0).c_str();

    // ------------------------------
    // -------------- YFX plot
    // ------------------------------
    if (m_isYFX)
    {
      // Add field to xy plot group
      CPlot* plot = dynamic_cast<CPlot*>(m_plots[groupNumber]);
      if (plot == NULL)
      {
        plot = new CPlot(groupNumber);
        m_plots[groupNumber] = plot;
      }
      CPlotField* field = new CPlotField(fieldName.c_str());
      plot->m_fields.Insert(field);

      std::string xAxisName;
      uint32_t nXAxisName = m_params.CheckCount(kwDISPLAY_XAXIS, 0, nbGroup);

      if (nXAxisName > 0)
      {
        m_params.m_mapParam[kwDISPLAY_XAXIS]->GetValue(xAxisName, groupNumber - groupNumberMin, "");
      }


      if ( ! xAxisName.empty() )
      {
        plot->SetForcedVarXname(xAxisName.c_str());

        std::string xAxisLabel;
        uint32_t nXAxisLabel = m_params.CheckCount(kwDISPLAY_XLABEL, 0, nbGroup);

        if (nXAxisLabel > 0)
        {
          m_params.m_mapParam[kwDISPLAY_XLABEL]->GetValue(xAxisLabel, groupNumber - groupNumberMin, xAxisName);
        }

        plot->m_titleX = xAxisLabel.c_str();

      }



      CInternalFiles* yfx = dynamic_cast<CInternalFiles*>(Prepare(index, fieldName));

      field->m_internalFiles.Insert(yfx);
      field->m_xyProps = GetXYPlotProperty(index);

    }



    // ------------------------------
    // -------------- World plot
    // ------------------------------
    if (m_isZFLatLon)
    {
      // Add field to world plot group
      CWPlot* wplot = dynamic_cast<CWPlot*>(m_plots[groupNumber]);
      if (wplot == NULL)
      {
        wplot = new CWPlot(groupNumber);
        m_plots[groupNumber] = wplot;
      }

      CWorldPlotProperties* worldProps = GetWorldPlotProperty(index);

      if ((! worldProps->m_withContour)  && (! worldProps->m_solidColor))
      {
        worldProps->m_solidColor = true;
      }

      CPlotField* field = NULL;

      if (worldProps->m_withAnimation)
      {
        //group files by field
        // find an already existing field with the same name
        // and the same contour flag
        // (don't animate same named fields which with different contour flag)
        field = wplot->FindPlotField(fieldName, &worldProps->m_withContour, &worldProps->m_solidColor);
        if (field == NULL)
        {
          field = new CPlotField(fieldName.c_str());
          wplot->m_fields.Insert(field);
        }
      }
      else
      {
        field = new CPlotField(fieldName.c_str());
        wplot->m_fields.Insert(field);
      }

      CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>(Prepare(index, fieldName));

      field->m_internalFiles.Insert(zfxy);
      field->m_worldProps = worldProps;
    }

    // ------------------------------
    // -------------- ZFXY plot
    // ------------------------------
    if (m_isZFXY)
    {
      // Add field to ZFXY plot group
      CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>(m_plots[groupNumber]);
      if (zfxyplot == NULL)
      {
        zfxyplot = new CZFXYPlot(groupNumber);
        m_plots[groupNumber] = zfxyplot;
      }

      CZFXYPlotProperties* zfxyProps = GetZFXYPlotProperty(index);

      if ((! zfxyProps->m_withContour)  && (! zfxyProps->m_solidColor))
      {
        zfxyProps->m_solidColor = true;
      }

      CPlotField* field = NULL;

      if (zfxyProps->m_withAnimation)
      {
        //group files by field
        // find an already existing field with the same name
        // and the same contour flag
        // (don't animate same named fields which with different contour flag)
        field = zfxyplot->FindPlotField(fieldName, &zfxyProps->m_withContour, &zfxyProps->m_solidColor);
        if (field == NULL)
        {
          field = new CPlotField(fieldName.c_str());
          zfxyplot->m_fields.Insert(field);
        }
      }
      else
      {
        field = new CPlotField(fieldName.c_str());
        zfxyplot->m_fields.Insert(field);
      }


      std::string xAxisName;
      uint32_t nXAxisName = m_params.CheckCount(kwDISPLAY_XAXIS, 0, nbGroup);

      if (nXAxisName > 0)
      {
        m_params.m_mapParam[kwDISPLAY_XAXIS]->GetValue(xAxisName, groupNumber - groupNumberMin, "");
      }

      if ( ! xAxisName.empty() )
      {
        zfxyplot->SetForcedVarXname(xAxisName.c_str());
      }

      std::string yAxisName;
      uint32_t nYAxisName = m_params.CheckCount(kwDISPLAY_YAXIS, 0, nbGroup);

      if (nYAxisName > 0)
      {
        m_params.m_mapParam[kwDISPLAY_YAXIS]->GetValue(yAxisName, groupNumber - groupNumberMin, "");
      }

      if ( ! yAxisName.empty() )
      {
        zfxyplot->SetForcedVarYname(yAxisName.c_str());
      }

      CInternalFiles* f = dynamic_cast<CInternalFiles*>(Prepare(index, fieldName));

      field->m_internalFiles.Insert(f);
      field->m_zfxyProps = zfxyProps;

    }

    m_fields[index] = expr;
  }

}

//----------------------------------------
/*
int32_t CBratDisplayApp::FindFieldPosInGlobalList(const std::string& fieldName)
{

  for (uint32_t i = 0 ; i < m_fields.size() ; i++)
  {
    std::string fieldNameTmp = m_fields[i].GetFieldNames()->at(0);
    if (fieldName.compare(fieldNameTmp) == 0)
    {
      return i;
    }
  }

  return -1;
}
*/
//----------------------------------------
void CBratDisplayApp::CheckFiles()
{

  CStringArray::iterator it;
  std::string inputFileTypeRead;

  if (m_inputFiles.empty())
  {
    CException e("CBratDisplayApp::CheckFiles - input data file std::list is empty()", BRATHL_COUNT_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }


/*
  if (m_inputFiles.size() == 1)
  {
    m_inputFileType = CNetCDFFiles::IdentifyExistingFile(*(m_inputFiles.begin()));
    return;
  }
*/

  m_inputFileType = "";


  for ( it = m_inputFiles.begin(); it != m_inputFiles.end(); it++ )
  {
    inputFileTypeRead = CNetCDFFiles::IdentifyExistingFile(*it);

    if (it != m_inputFiles.begin())
    {
      if ( m_inputFileType.compare(inputFileTypeRead) != 0)
      {
        std::string msg = CTools::Format("CBratDisplayApp::CheckFiles - Files are not in the same way - Expected type '%s' and found '%s' for file '%s'",
                                     m_inputFileType.c_str(), inputFileTypeRead.c_str(), (*it).c_str());
        CException e(msg, BRATHL_INCONSISTENCY_ERROR);
        CTrace::Tracer("%s", e.what());
        throw (e);

      }
    }
    m_inputFileType = inputFileTypeRead;
  }

}

//----------------------------------------
bool CBratDisplayApp::IsYFXType()
{
  if (m_inputFileType.empty())
  {
    CException e("CBratDisplayApp::isYFX - input file type is empty", BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  return m_inputFileType.compare(CInternalFilesYFX::TypeOf()) == 0;

}

//----------------------------------------
bool CBratDisplayApp::IsZFXYType()
{
  if (m_inputFileType.empty())
  {
    CException e("CBratDisplayApp::IsZFXYType - input file type is empty", BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  return m_inputFileType.compare(CInternalFilesZFXY::TypeOf()) == 0;

}
//----------------------------------------
CInternalFiles* CBratDisplayApp::Prepare(int32_t indexFile, const wxString& fieldName)
{

  CInternalFiles* f = BuildExistingInternalFileKind(m_inputFiles.at(indexFile).c_str());

  f->Open(ReadOnly);

  CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>(f);
  CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>(f);
  if (zfxy != NULL)
  {
    CheckFieldsData(zfxy, fieldName);
  }
  if (yfx != NULL)
  {
    CheckFieldsData(yfx, fieldName);
  }

//femm  m_internalData.Insert(f);

  return f;
}


//----------------------------------------
CInternalFiles* CBratDisplayApp::Prepare(const wxString& fileName, const wxString& fieldName)
{

  CInternalFiles* f = NULL;

  if (IsYFXType())
  {
    f = new CInternalFilesYFX((const char *)(fileName));
  }
  else if (IsZFXYType())
  {
    f = new CInternalFilesZFXY((const char *)(fileName));
  }
  else
  {
    CException e(CTools::Format("CBratDisplayApp::Prepare -  unknown input file type: '%s'", (const char *)(fileName)),
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  f->Open(ReadOnly);

/*
    CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>(f);
  CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>(f);
  if (zfxy != NULL)
  {
    CheckFieldsData(zfxy, fieldName);
  }
  if (yfx != NULL)
  {
    CheckFieldsData(yfx, fieldName);
  }
  */
//femm  m_internalData.Insert(f);

  return f;
}



//----------------------------------------
/*
void CBratDisplayApp::Prepare()
{

  CInternalFiles *f;

  for ( uint32_t i = 0 ; i < m_inputFiles.size() ; i++ )
  {
    f = BuildExistingInternalFileKind(m_inputFiles.at(i).c_str());
    f->Open(ReadOnly);

    CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>(f);
    CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>(f);
    if (zfxy != NULL)
    {
      CheckFieldsData(zfxy, i);
    }
    if (yfx != NULL)
    {
      CheckFieldsData(yfx, i);
    }

    m_internalData.Insert(f);
  }

}
*/
/*
//----------------------------------------
void CBratDisplayApp::CheckFieldsData(CInternalFilesYFX* yfx, int32_t indexField)
{
  if (yfx == NULL)
  {
    return;
  }

  CBitSet32* fieldBitSet = dynamic_cast<CBitSet32*>(m_fieldBitSets.Exists(yfx->GetName()));

  if (fieldBitSet == NULL)
  {
    fieldBitSet = new CBitSet32;
    m_fieldBitSets.Insert(yfx->GetName(), fieldBitSet);
  }

  CStringArray names;

  yfx->GetDataVars(names);

  CExpression expr = m_fields.at(indexField);

  if (expr.GetFieldNames().size() != 1)
  {
    std::string msg = CTools::Format("CBratDisplayApp::CheckFieldsData - Expression '%s' has incorrect number of fields '%d' (correct is 1)",
                                 expr.AsString().c_str(), expr.GetFieldNames().size());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  std::string fieldName = expr.GetFieldNames().at(0);
  if (names.Exists(fieldName) == false)
  {
    std::string msg = CTools::Format("CBratDisplayApp::CheckFieldsData - Field '%s' in command file doesn't exist in intput file '%s'",
                                fieldName.c_str(), yfx->GetName().c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  fieldBitSet->m_bitSet.set(indexField);

}
*/
//----------------------------------------
void CBratDisplayApp::CheckFieldsData(CInternalFiles* f, const wxString& fieldName)
{
  CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>(f);
  CInternalFilesYFX* yfx = dynamic_cast<CInternalFilesYFX*>(f);
  if (zfxy != NULL)
  {
    CheckFieldsData(zfxy, fieldName);
  }
  if (yfx != NULL)
  {
    CheckFieldsData(yfx, fieldName);
  }
}
//----------------------------------------
void CBratDisplayApp::CheckFieldsData(CInternalFilesYFX* yfx, const wxString& fieldName)
{
  if (yfx == NULL)
  {
    return;
  }


  CStringArray names;

  yfx->GetDataVars(names);

  if (names.Exists((const char *)(fieldName)) == false)
  {
    std::string msg = CTools::Format("CBratDisplayApp::CheckFieldsData - Field '%s' in command file doesn't exist in intput file '%s'",
                                (const char *)(fieldName), yfx->GetName().c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

}//----------------------------------------
void CBratDisplayApp::CheckFieldsData(CInternalFilesZFXY* zfxy, const wxString& fieldName)
{
  if (zfxy == NULL)
  {
    return;
  }


  CStringArray names;

  zfxy->GetDataVars(names);

  if (names.Exists((const char *)(fieldName)) == false)
  {
    std::string msg = CTools::Format("CBratDisplayApp::CheckFieldsData - Field '%s' in command file doesn't exist in intput file '%s'",
                                (const char *)(fieldName), zfxy->GetName().c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

}
/*
//----------------------------------------
void CBratDisplayApp::CheckFieldsData(CInternalFilesZFXY* zfxy, int32_t indexField)
{
  if (zfxy == NULL)
  {
    return;
  }
  if (zfxy->IsGeographic() == false)
  {
    std::string msg = CTools::Format("CBratDisplayApp::CheckFieldsData - Non-geographical data type '%s' are not yet implemented",
                                  zfxy->TypeOf().c_str());
    CException e(msg, BRATHL_UNIMPLEMENT_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  CBitSet32* fieldBitSet = dynamic_cast<CBitSet32*>(m_fieldBitSets.Exists(zfxy->GetName()));

  if (fieldBitSet == NULL)
  {
    fieldBitSet = new CBitSet32;
    m_fieldBitSets.Insert(zfxy->GetName(), fieldBitSet);
  }

  CStringArray names;

  zfxy->GetDataVars(names);

  CExpression expr = m_fields.at(indexField);

  if (expr.GetFieldNames().size() != 1)
  {
    std::string msg = CTools::Format("CBratDisplayApp::CheckFieldsData - Expression '%s' has incorrect number of fields '%d' (correct is 1)",
                                 expr.AsString().c_str(), expr.GetFieldNames().size());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  std::string fieldName = expr.GetFieldNames().at(0);
  if (names.Exists(fieldName) == false)
  {
    std::string msg = CTools::Format("CBratDisplayApp::CheckFieldsData - Field '%s' in command file doesn't exist in intput file '%s'",
                                fieldName.c_str(), zfxy->GetName().c_str());
    CException e(msg, BRATHL_INCONSISTENCY_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  fieldBitSet->m_bitSet.set(indexField);

}
*/
/*
//----------------------------------------
void CBratDisplayApp::CheckFieldsData(CInternalFilesZFXY* zfxy)
{
  if (zfxy == NULL)
  {
    return;
  }
  if (zfxy->IsGeographic() == false)
  {
    std::string msg = CTools::Format("CBratDisplayApp::CheckFieldsData - Non-geographical data type '%s' are not yet implemented",
                                  zfxy->TypeOf().c_str());
    CException e(msg, BRATHL_UNIMPLEMENT_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

 // CStringArray::iterator itName;
  std::vector<CExpression>::iterator itField;


  CBitSet32* fieldBitSet = new CBitSet32;
  m_fieldBitSets.Insert(zfxy->GetName(), fieldBitSet);

  CStringArray names;

  zfxy->GetDataVars(names);

  for (itField = m_fields.begin(); itField != m_fields.end(); itField++)
  {
    if ((*itField).GetFieldNames().size() != 1)
    {
      std::string msg = CTools::Format("CBratDisplayApp::CheckFieldsData - Expression '%s' has incorrect number of fields '%d' (correct is 1)",
                                   (*itField).AsString().c_str(), (*itField).GetFieldNames().size());
      CException e(msg, BRATHL_INCONSISTENCY_ERROR);
      CTrace::Tracer("%s", e.what());
      throw (e);
    }
    std::string fieldName = (*itField).GetFieldNames().at(0);
    if (names.Exists(fieldName) == false)
    {
      // in ZFXY plot all field must exist in all input files
      if (zfxy != NULL)
      {
        std::string msg = CTools::Format("CBratDisplayApp::CheckFieldsData - Field '%s' in command file doesn't exist in intput file '%s'",
                                    fieldName.c_str(), f->GetName().c_str());
        CException e(msg, BRATHL_INCONSISTENCY_ERROR);
        CTrace::Tracer("%s", e.what());
        throw (e);
      }
    }
    else
    {
      fieldBitSet->m_bitSet.set(itField - m_fields.begin());
    }
  }
}
*/
//----------------------------------------
std::string CBratDisplayApp::GetFirstFileName()
{
  LoadParameters();

  return m_params.GetFirstFile(kwFILE);

}
/*
//----------------------------------------
CInternalFiles* CBratDisplayApp::GetFirstFile()
{
  LoadParameters();

  std::string name = m_params.GetFirstFile(kwFILE);

  CInternalFiles* f = NULL;

  try
  {
    f = BuildExistingInternalFileKind(name);

    if (f != NULL)
    {
      f->Open(ReadOnly);
    }
  }
  catch (CException& e)
  {
    e.what();

    CTools::DeleteObject(file);
    file = NULL;
    return NULL;
  }


  return f;

}
*/
//----------------------------------------
bool CBratDisplayApp::IsWPlot()
{
  std::string name = GetFirstFileName();

  return CInternalFiles::IsZFLatLonFile(name);
}
//----------------------------------------
bool CBratDisplayApp::IsZXYPlot()
{

  LoadParameters();

  CStringArray fieldNames;

  m_params.GetFieldNames(kwFIELD, fieldNames);

  std::string name = GetFirstFileName();

  return CInternalFiles::IsZFXYFile(name, &fieldNames);


}
//----------------------------------------
bool CBratDisplayApp::IsXYPlot()
{

  std::string name = GetFirstFileName();

  return CInternalFiles::IsYFXFile(name);

}
//----------------------------------------
void CBratDisplayApp::CreateWPlot( CWPlot* wplot, wxSize& size, wxPoint& pos )
{
	wplot->GetInfo();

    CWorldPlotProperties* wPlotProperty = GetWorldPlotProperty( 0 );

	wxString titleTmp = wplot->m_title;
	titleTmp = CTools::SlashesDecode( (const char *)( titleTmp ) ).c_str();
	titleTmp.Replace( "\n", "-" );
	titleTmp.Replace( "\t", " " );
	wxString title = wxString::Format( "BRAT World Plot - %s #%d", titleTmp.c_str(), wplot->m_groupNumber );

	CWorldPlotFrame* frame = new CWorldPlotFrame( NULL, -1, title, wPlotProperty, pos, size );

	// for geostrphic velocity
	CPlotField * northField =NULL;
	CPlotField * eastField =NULL;
	for ( CObArray::iterator itField = wplot->m_fields.begin(); itField != wplot->m_fields.end(); itField++ )
	{
		CPlotField* field = CPlotField::GetPlotField( *itField );
		if ( field->m_internalFiles.empty() )
			continue;

		if ( field->m_worldProps->m_northComponent && northField == NULL ) {
			northField = field;
			continue;
		}
		else
		if ( field->m_worldProps->m_eastComponent && eastField == NULL ) {
			eastField = field;
			continue;
		}

		// otherwise just add it as regular data
		VTK_CWorldPlotData *geoMap = new VTK_CWorldPlotData( field );		//v4: note VTK_CWorldPlotData ctor is only invoked here
		//femm m_geoMaps.Insert( geoMap );
		frame->AddData( geoMap );
	}

	// we have a Vector Plot!
	if ( northField != NULL && eastField != NULL ) {

		VTK_CWorldPlotVelocityData *gvelocityMap = new VTK_CWorldPlotVelocityData( northField, eastField );			//v4 note: VTK_CWorldPlotVelocityData ctor is only invoked here
		gvelocityMap->SetIsGlyph( true );
		//femm m_geoMaps.Insert( gvelocityMap );
		frame->AddData( gvelocityMap );
	}
	else if ( northField != eastField ) {
		CException e( "CBratDisplayApp::CreateWPlot - incomplete std::vector plot components", BRATHL_INCONSISTENCY_ERROR );
		CTrace::Tracer( "%s", e.what() );
		throw ( e );
	}


	frame->UpdateView();
	frame->Raise();
	frame->Show( TRUE );

	// ---- Fix for bug on Linux to resize correctly VTK widget.
	wxSize sizefFrame = frame->GetSize();
	sizefFrame += wxSize( 1, 1 );
	frame->SetSize( sizefFrame );
	// ----

	pos = frame->GetPosition();

	//frame->GetPlotPlanel()->GetVtkWidget()->GetRenderWindow()->Render();
	//frame->GetPlotPlanel()->GetVtkWidget()->Render();

	// Fix for Windows refusing to draw initial screen:
#ifdef WIN32
	///frame->GetPlotPlanel()->Layout();
#endif
}
//L:\project\workspaces\S3A\Displays\DisplayDisplays_MW_1_MWR____2004.par
//----------------------------------------
void CBratDisplayApp::CreateZFXYPlot(CZFXYPlot* zfxyplot, wxSize& size, wxPoint& pos)
{
  zfxyplot->GetInfo();

  CZFXYPlotProperties* zfxyPlotProperty = GetZFXYPlotProperty(0);

  wxString titleTmp = zfxyplot->m_title;

  titleTmp = CTools::SlashesDecode((const char *)(titleTmp)).c_str();

  titleTmp.Replace("\n", "-");
  titleTmp.Replace("\t", " ");

  wxString title = wxString::Format("BRAT Z=F(X,Y) Plot - %s #%d",
                                   titleTmp.c_str(),
                                   zfxyplot->m_groupNumber);

  CZFXYPlotFrame* frame = new CZFXYPlotFrame( NULL, -1, title, zfxyPlotProperty, pos, size);


  CObArray::iterator itField;
  for (itField = zfxyplot->m_fields.begin(); itField !=  zfxyplot->m_fields.end(); itField++)
  {
    CPlotField* field = CPlotField::GetPlotField(*itField);

    if (field->m_internalFiles.empty())
    {
      continue;
    }

    VTK_CZFXYPlotData *geoMap = new VTK_CZFXYPlotData(/*frame, */zfxyplot, field);		// v4 note: VTK_CZFXYPlotData ctor only invoked here

    frame->AddData(geoMap);
  }

  frame->UpdateView();
  frame->Raise();
  frame->Show( TRUE );

  // ---- Fix for bug on Linux to resize correctly VTK widget.
  wxSize sizefFrame = frame->GetSize();
  sizefFrame += wxSize(1,1);
  frame->SetSize(sizefFrame);
  // ----

  pos = frame->GetPosition();

  //frame->GetPlotPlanel()->GetVtkWidget()->GetRenderWindow()->Render();
  //frame->GetPlotPlanel()->GetVtkWidget()->Render();

   // Fix for Windows refusing to draw initial screen:
#ifdef WIN32
    ///frame->GetPlotPlanel()->Layout();
#endif

}
/*
//----------------------------------------
void CBratDisplayApp::CreateWPlot(CWPlot* wplot, CObArray& internalData, wxSize& size, wxPoint& pos)
{
  wplot->GetInfo(&internalData);

  CWorldPlotProperties* wPlotProperty = GetWorldPlotProperty(0);

  wxString titleTmp = wPlotProperty->m_title;
  if (titleTmp.IsEmpty())
  {
    titleTmp = wplot->m_title.c_str();
    if (wPlotProperty->m_groupByFile)
    {
      wPlotProperty->m_title = titleTmp;
    }
  }

  titleTmp = CTools::SlashesDecode(titleTmp.c_str()).c_str();

  titleTmp.Replace("\n", "-");
  titleTmp.Replace("\t", " ");

  wxString title = wxString::Format("BRAT World Plot - %s #%d",
                                   titleTmp.c_str(),
                                   wplot->m_groupNumber + 1);

  CWorldPlotFrame* frame = new CWorldPlotFrame( NULL, -1, title, wPlotProperty, pos, size);

  int32_t nrFields = wplot->m_fields.size();
  int32_t nrFiles = internalData.size();

  for (int32_t iField = 0 ; iField < nrFields ; iField++)
  {
    CObArray internalDataGeoMap(false);

    for (int32_t iFile = 0 ; iFile < nrFiles ; iFile++)
    {
      CInternalFilesZFXY* zfxy = wplot->GetInternalFilesZFXY(internalData[iFile]);
      CBitSet32* fieldBitSet = GetBitSetFields(zfxy);

      int32_t posField = FindFieldPosInGlobalList( wplot->m_fields[iField].GetFieldNames()[0]);
      /////////////bool isFieldInFile = fieldBitSet->m_bitSet.test(iField);
      bool isFieldInFile = fieldBitSet->m_bitSet.test(posField);

      if (isFieldInFile)
      {
        internalDataGeoMap.Insert(zfxy);
      }
    }

    if (internalDataGeoMap.empty())
    {
      //throw  CParameterException(CTools::Format("CBratDisplayApp::CreateWPlot - Field %s is not in any files",
      //                                          wplot->m_fieldsName[iField].c_str()),
      //			       BRATHL_INCONSISTENCY_ERROR);
      continue;
    }

    CWorldPlotProperties* props = GetWorldPlotProperty(iField);

    VTK_CWorldPlotData *geoMap = new VTK_CWorldPlotData(frame, &internalDataGeoMap, wplot->m_fieldsName[iField], props);

    m_geoMaps.Insert(geoMap);

    frame->AddData(geoMap);

  }

  frame->UpdateView();
  frame->Raise();
  frame->Show( TRUE );


  //frame->GetPlotPlanel()->GetVtkWidget()->GetRenderWindow()->Render();
  //frame->GetPlotPlanel()->GetVtkWidget()->Render();

   // Fix for Windows refusing to draw initial screen:
#ifdef WIN32
    ///frame->GetPlotPlanel()->Layout();
#endif

}
*/
//----------------------------------------
void CBratDisplayApp::CreateXYPlot( CPlot* plot, wxSize& size, wxPoint& pos )
{

	//CInternalFilesYFX* yfx = plot->GetInternalFilesYFX(m_internalData.at(0));
	plot->GetInfo();

	std::string strTempTitle = CTools::StringReplace( plot->m_title, "\\n", "-" );
	strTempTitle = CTools::StringReplace( strTempTitle, "\\t", " " );

	wxString title = CTools::Format( "BRAT Y=F(X) Plot - %s #%d",
		strTempTitle.c_str(),
		plot->m_groupNumber ).c_str();

	CXYPlotFrame* frame = new CXYPlotFrame( NULL, -1, title, pos, size );

	int32_t nrFields = plot->m_fields.size();
	//  int32_t nrFiles = m_internalData.size();

	for ( int32_t iField = 0; iField < nrFields; iField++ )
	{
		VTK_CXYPlotData* plotData = new VTK_CXYPlotData( plot, iField );	//v4: VTK_CXYPlotData ctor only invoked here
		frame->AddData( plotData );
	}


	frame->Raise();

	frame->ShowPropertyPanel();
	frame->Show( TRUE );

	pos = frame->GetPosition();
}

/*
//----------------------------------------
void CBratDisplayApp::CreateXYPlot(CPlot* plot, wxSize& size, wxPoint& pos)
{

  //CInternalFilesYFX* yfx = plot->GetInternalFilesYFX(m_internalData.at(0));
  plot->GetInfo(&m_internalData);

  std::string strTempTitle = CTools::StringReplace(plot->m_title.c_str(), "\\n", "-");
  strTempTitle = CTools::StringReplace(strTempTitle, "\\t", " ");

  wxString title = CTools::Format("BRAT XY Plot - %s #%d",
                                   strTempTitle.c_str(),
                                   plot->m_groupNumber + 1).c_str();

  CXYPlotFrame* frame = new CXYPlotFrame( NULL, -1, title, pos, size);

  int32_t nrFields = plot->m_fields.size();
  int32_t nrFiles = m_internalData.size();

  for (int32_t iField = 0 ; iField < nrFields ; iField++)
  {
    for (int32_t iFile = 0 ; iFile < nrFiles ; iFile++)
    {
      CInternalFilesYFX* yfx = plot->GetInternalFilesYFX(m_internalData[iFile]);
      CBitSet32* fieldBitSet = GetBitSetFields(yfx);

      int32_t posField = FindFieldPosInGlobalList( plot->m_fields[iField].GetFieldNames()[0]);
      ////////////bool isFieldInFile = fieldBitSet->m_bitSet.test(iField);
      bool isFieldInFile = fieldBitSet->m_bitSet.test(posField);

      if (isFieldInFile)
      {
        int32_t indexProp = HowManyFieldsXYInBitSetBefore(iFile) + posField;
        CXYPlotProperties* props = GetXYPlotProperty( indexProp );
        CXYPlotDataMulti* plotData = new CXYPlotDataMulti("", props);

        /////plotData->Create(&m_internalData, plot, iField);
        plotData->Create(yfx, plot, iField);

        frame->AddData(plotData);
        //m_indexProp++;
      }
    }
  }


  frame->Raise();

  frame->ShowPropertyPanel();
  frame->Show( TRUE );

}
*/
/*
//----------------------------------------
int32_t CBratDisplayApp::HowManyFieldsXYInBitSetBefore(int32_t index)
{
  if (index < 0)
  {
    std::string msg = CTools::Format("CBratDisplayApp::HowManyFieldsZXYInBitSetBefore - invalid index (<0):%d",
                                index);
    CException e(msg,
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  if (static_cast<uint32_t>(index) >= m_internalData.size())
  {
    return -1;
  }


  int32_t n = 0;

  for(int32_t i = 0 ; i < index ; i++)
  {
    CInternalFilesYFX* yfx = CPlot::GetInternalFilesYFX(m_internalData[i]);
    CBitSet32* fieldBitSet = GetBitSetFields(yfx);
    n += fieldBitSet->m_bitSet.count();
  }

  return n;

}
*/
/*
//----------------------------------------
int32_t CBratDisplayApp::HowManyFieldsZXYInBitSetBefore(int32_t index)
{
  if (index < 0)
  {
    std::string msg = CTools::Format("CBratDisplayApp::HowManyFieldsZXYInBitSetBefore - invalid index (<0):%d",
                                index);
    CException e(msg,
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  if (static_cast<uint32_t>(index) >= m_internalData.size())
  {
    return -1;
  }


  int32_t n = 0;

  for(int32_t i = 0 ; i < index ; i++)
  {
    CInternalFilesZFXY* zfxy = CWPlot::GetInternalFilesZFXY(m_internalData[i]);
    CBitSet32* fieldBitSet = GetBitSetFields(zfxy);
    n += fieldBitSet->m_bitSet.count();
  }

  return n;

}
*/

//----------------------------------------
/*
CBitSet32* CBratDisplayApp::GetBitSetFields(CInternalFilesZFXY* f)
{

  CBitSet32* bitSet = dynamic_cast<CBitSet32*>(m_fieldBitSets.Exists(f->GetName()));
  if (bitSet == NULL)
  {
    std::string msg = CTools::Format("CBratDisplayApp::GetBitSetFields - bitSet object found - dynamic_cast<CBitSet32*>(m_fieldBitSets.Exists(f->GetName()) returns NULL"
                                "- file '%s'",
                                f->GetName().c_str());
    CException e(msg,
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);

  }

  return bitSet;
}
//----------------------------------------
CBitSet32* CBratDisplayApp::GetBitSetFields(CInternalFilesYFX* f)
{

  CBitSet32* bitSet = dynamic_cast<CBitSet32*>(m_fieldBitSets.Exists(f->GetName()));
  if (bitSet == NULL)
  {
    std::string msg = CTools::Format("CBratDisplayApp::GetBitSetFields - bitSet object found - dynamic_cast<CBitSet32*>(m_fieldBitSets.Exists(f->GetName()) returns NULL"
                                "- file '%s'",
                                f->GetName().c_str());
    CException e(msg,
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);

  }

  return bitSet;
}
//----------------------------------------
CBitSet32* CBratDisplayApp::GetBitSetFields(CObMap::iterator it)
{

  CBitSet32* bitSet = dynamic_cast<CBitSet32*>(it->second);
  if (bitSet == NULL)
  {
    std::string msg = CTools::Format("CBratDisplayApp::GetBitSetFields - bitSet object found - dynamic_cast<CBitSet32*>(it->second) returns NULL"
                                "- key '%s'",
                                it->first.c_str());
    CException e(msg,
                 BRATHL_LOGIC_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);

  }

  return bitSet;
}
*/
//----------------------------------------
wxBitmapType CBratDisplayApp::GetIconType()
{
  //return wxBITMAP_TYPE_BMP;
  //return wxBITMAP_TYPE_JPEG;
  //return wxBITMAP_TYPE_GIF;
#ifdef WIN32
  return wxBITMAP_TYPE_ICO;
#else
  return wxBITMAP_TYPE_BMP;
#endif
}
//----------------------------------------
wxString CBratDisplayApp::GetIconFile()
{
  return CTools::FindDataFile((const char *)(GetIconFileName())).c_str();
}
//----------------------------------------
wxString CBratDisplayApp::GetIconFileName()
{
  return BRATHL_ICON_FILENAME.c_str();
}

//----------------------------------------
bool CBratDisplayApp::GetCommandLineOptions(int argc, wxChar* argv[])
{
  struct arg_file *infiles = arg_file1(NULL, NULL, NULL, "parameter file or netcdf file");
  struct arg_str  *xAxis    = arg_str0("xX", NULL, NULL, "specifies the x axis (only with netcdf file)");
  struct arg_str  *yAxis   = arg_str0("yY", NULL, NULL, "specifies the y axis (only with netcdf file)");
  struct arg_str  *vars    = arg_str0("vV", NULL, "var,var2,varN...", "specifies the std::list of variables to display. (only with netcdf file)");
  struct arg_lit  *help    = arg_lit0("h","help","print this help and exit");
  struct arg_lit  *keywords    = arg_lit0("k","keywords", "print std::list of allowed parameters in the parameter file");
  struct arg_end  *end     = arg_end(20);

  void* argtable[] = {infiles, xAxis, yAxis, vars, help, keywords, end};

  std::string progname = wxGetApp().GetAppName().ToStdString();

  int nerrors;
  int i;

  /* verify the argtable[] entries were allocated sucessfully */
  if (arg_nullcheck(argtable) != 0)
  {
    /* NULL entries were detected, some allocations must have failed */
    CException e("CBratDisplayApp::GetCommandLineOptions - insufficient memory", BRATHL_UNIMPLEMENT_ERROR);
    CTrace::Tracer("%s", e.what());
    throw (e);
  }

  /* Parse the command line as defined by argtable[] */
  std::vector<wxString> v;
  for ( int i = 0; i < argc; ++i )
	  v.push_back( argv[ i ] );
  std::vector<const char*> charv;
  for ( int i = 0; i < argc; ++i )
	  charv.push_back( v[ i ].c_str() );
  nerrors = arg_parse(argc,const_cast<char**>(&charv[0]),argtable);

  /* special case: '--help' takes precedence over error reporting */
  if ( (help->count > 0) || (argc == 1) )
  {
    printf("Usage: %s", progname.c_str());
    arg_print_syntax(stdout,argtable,"\n");
    printf("BratDisplay - An application to display BRAT netCDF data\n");
    arg_print_glossary(stdout,argtable,"  %-25s %s\n");

    /* deallocate each non-null entry in argtable[] */
    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
    return false;
  }

  if ( (keywords->count > 0) && (keywordList != NULL) )
  {
    CBratProcess::PrintParameterHelp(cout, keywordList);

    /* deallocate each non-null entry in argtable[] */
    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
    return false;
  }

  /* If the parser returned any errors then display them and exit */
  if (nerrors > 0)
  {
    /* Display the error details contained in the arg_end struct.*/
    arg_print_errors(stdout,end,progname.c_str());
    printf("Try '%s --help' for more information.\n",progname.c_str());

    /* deallocate each non-null entry in argtable[] */
    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
    return false;
  }



  for ( i = 0; i < infiles->count; i++)
  {
    m_paramFile = infiles->filename[i];
  }

  for (i = 0; i < xAxis->count; i++)
  {
    m_paramXAxis = xAxis->sval[i];
  }

  for (i = 0; i < yAxis->count; i++)
  {
    m_paramYAxis = yAxis->sval[i];
  }

  for ( i = 0; i < vars->count; i++)
  {
    m_paramVars.ExtractStrings(vars->sval[i], ',');
  }


  /* deallocate each non-null entry in argtable[] */
  arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));

  return true;
}






