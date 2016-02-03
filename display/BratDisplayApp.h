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
#include "wx/fileconf.h" // (wxFileConfig class)

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
#include "PlotData/Plot.h"
#include "PlotData/WPlot.h"
#include "PlotData/ZFXYPlot.h"

#include "PlotData/WorldPlotData.h"
#include "XYPlotData.h"
#include "ZFXYPlotData.h"


#include "wxInterface.h"
#ifdef CursorShape  //collsion Qt X
#undef CursorShape
#endif
#include "new-gui/Common/ApplicationPaths.h"


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
    CApplicationPaths *mBratPaths = nullptr;;

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
  //int32_t FindFieldPosInGlobalList(const std::string& fieldName);

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
  
  //int32_t FindFieldIndexExpression(const std::string& str);

  //CInternalFiles* GetFirstFile();
  std::string GetFirstFileName();

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

  std::string m_paramFile;

  std::string m_paramXAxis;
  std::string m_paramYAxis;

  CStringList m_paramVars;


  CFileParams m_params;
  CStringArray m_inputFiles;
  CObMap m_fieldBitSets;
 
  std::vector<CExpression> m_fields;
  CObArray m_plots;

  std::string m_inputFileType;

  //femm CObArray m_internalData;

  //CObArray m_geoMaps;
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

  //wxFileName m_execName;

};

DECLARE_APP(CBratDisplayApp)

#endif
