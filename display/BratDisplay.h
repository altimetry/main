/*
* Copyright (C) 2005 Collecte Localisation Satellites (CLS), France (31)
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

#ifndef __BratDisplay_H__
#define __BratDisplay_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "BratDisplay.h"
#endif

// Include wxWindows' headers

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/filename.h" // (wxFileConfig class)

#include "brathl.h"

#include "Tools.h"
#include "FileParams.h"
#include "Parameter.h"
#include "InternalFiles.h"
#include "InternalFilesFactory.h"
#include "ExternalFiles.h"
#include "ExternalFilesFactory.h"
#include "ExternalFilesNetCDF.h"
#include "InternalFilesYFX.h"
#include "InternalFilesZFXY.h"
using namespace brathl;


//#include "BratDisplay_wdr.h"
#include "Plot.h"
#include "WPlot.h"
#include "ZFXYPlot.h"

#include "WorldPlotData.h"
#include "XYPlotData.h"
#include "ZFXYPlotData.h"


// WDR: class declarations

//----------------------------------------------------------------------------
// BratDisplayFrame
//----------------------------------------------------------------------------
/*
class CBratDisplayFrame: public wxFrame
{
public:
    // constructors and destructors
    CBratDisplayFrame( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE );
    

private:
    // WDR: method declarations for BratDisplayFrame
    void CreateMyMenuBar();
    void CreateMyToolBar();
    
private:
    // WDR: member variable declarations for BratDisplayFrame
    
private:
    // WDR: handler declarations for BratDisplayFrame
    void OnAbout( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );
    void OnCloseWindow( wxCloseEvent &event );
    void OnSize( wxSizeEvent &event );
    void OnUpdateUI( wxUpdateUIEvent &event );
    
private:
    DECLARE_EVENT_TABLE()
};
*/

//-----------------------------------------------------------------
// BratDisplayApp
//----------------------------------------------------------------------------

class CBratDisplayApp: public wxApp
{
public:
  CBratDisplayApp();
  virtual ~CBratDisplayApp();

  virtual bool OnInit();
  virtual int OnExit();

  wxBitmapType GetIconType();
  wxString GetIconFile();
  wxString GetIconFileName();

  //CBitSet32* GetBitSetFields(CInternalFilesZFXY* f);
  //CBitSet32* GetBitSetFields(CInternalFilesYFX* f);
  //CBitSet32* GetBitSetFields(CObMap::iterator it);
  
  //int32_t HowManyFieldsXYInBitSetBefore(int32_t index);
  //int32_t HowManyFieldsZXYInBitSetBefore(int32_t index);

  static void DetermineCharSize(wxWindow* wnd, int32_t& width, int32_t& height);
  wxFileConfig* GetConfig() {return m_config;};
  //int32_t FindFieldPosInGlobalList(const string& fieldName);

  wxFileName* GetExecName() {return &m_execName;};

  wxString GetExecPathName(int32_t flags = wxPATH_GET_VOLUME, wxPathFormat format = wxPATH_NATIVE) 
#ifdef __WXMAC__
                        {return (m_execName.IsRelative() ? 
                                 m_execName.GetPath(flags, format) + "/BratDisplay.app/Contents/MacOS" :
                                 m_execName.GetPath(flags, format));};
#else
                        {return m_execName.GetPath(flags, format);};
#endif


private:
//-------------------------------------------------------------
//------------------- CIndexArray class --------------------
//-------------------------------------------------------------
  /** 
    A list of index object management class

   \version 1.0
  */
  class CIndexArray : public CBratObject
  {
  public:
    CIndexArray() {};
    CIndexArray(int32_t index) { m_array.Insert(index); };
    virtual ~CIndexArray() {};
    CIntArray m_array;    
  };

  void LoadParameters();

  bool GetParametersNetcdf();
  bool GetParametersNetcdfZFLatLon(CExternalFilesNetCDF* externalFile);
  bool GetParametersNetcdfZFXY(CExternalFilesNetCDF* externalFile);
  bool GetParametersNetcdfYFX(CExternalFilesNetCDF* externalFile);
  void GetParameters();
  void CheckFiles();

  bool IsYFXType();
  bool IsZFXYType();

  //void Prepare();
  CInternalFiles* Prepare(int32_t indexFile, const wxString& fieldName);
  CInternalFiles* Prepare(const wxString& fileName, const wxString& fieldName);

  void CheckFieldsData(CInternalFiles* f, const wxString& fieldName);
  void CheckFieldsData(CInternalFilesZFXY* f, const wxString& fieldName);
  void CheckFieldsData(CInternalFilesYFX* f, const wxString& fieldName);
  
  bool GetCommandLineOptions(int argc, wxChar* argv[]);

  //void CheckFieldsData(CInternalFilesZFXY* zfxy, int32_t indexField);
  //void CheckFieldsData(CInternalFilesYFX* yfx, int32_t indexField);
  
  //int32_t FindFieldIndexExpression(const string& str);

  //CInternalFiles* GetFirstFile();
  string GetFirstFileName();

  bool IsWPlot();
  void WorldPlot();
  void CreateWPlot(CWPlot *wplot, wxSize& size, wxPoint& pos);
  
  
  bool IsZXYPlot();
  void ZFXYPlot();
  void CreateZFXYPlot(CZFXYPlot *zfxyplot, wxSize& size, wxPoint& pos);

  

  void XYPlot();
  bool IsXYPlot();
  void CreateXYPlot(CPlot* plot, wxSize& size, wxPoint& pos);

  //void GetWPlotPropertyParams(CWorldPlotProperty& wPlotProperty);
  void GetWPlotPropertyParams(int32_t nFields);
  void GetXYPlotPropertyParams(int32_t nFields);
  void GetZFXYPlotPropertyParams(int32_t nFields);

  CZFXYPlotProperty* GetZFXYPlotProperty(int32_t index);
  CXYPlotProperty* GetXYPlotProperty(int32_t index);
  CWorldPlotProperty* GetWorldPlotProperty(int32_t index);

  //int32_t GetNumberOfPlots();
  //int32_t GetNumberOfWPlots();

private:

  string m_paramFile;

  string m_paramXAxis;
  string m_paramYAxis;

  CStringList m_paramVars;


  CFileParams m_params;
  CStringArray m_inputFiles;
  CObMap m_fieldBitSets;
 
  vector<CExpression> m_fields;
  CObArray m_plots;

  string m_inputFileType;

  CObArray m_internalData;

  CObArray m_geoMaps;
  CObArray m_zfxyMaps;


  CWorldPlotProperty m_wPlotProperty;
  CXYPlotProperty m_xyPlotProperty;
  CZFXYPlotProperty m_zfxyPlotProperty;

  CObArray m_zfxyPlotProperties;
  CObArray m_xyPlotProperties;
  CObArray m_wPlotProperties;

  wxFileConfig* m_config;

  int32_t m_indexProp;

  bool m_isYFX;
  bool m_isZFLatLon;
  bool m_isZFXY;

  wxFileName m_execName;

};

DECLARE_APP(CBratDisplayApp)

#endif
