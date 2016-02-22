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


#ifndef __AxisPropertyPanel_H__
#define __AxisPropertyPanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "AxisPropertyPanel.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "brathl.h"

#include "CallBack.h" 
using namespace brathl;

#include "BratDisplay_wdr.h"

#include "wxVTKRenderWindowInteractor.h"
#include "vtkXYPlotActor.h"
#include "vtkCommand.h"


#include "new-gui/brat/DataModels/PlotData/XYPlotData.h"

#include "LabeledTextCtrl.h"


class CVtkComputeRangeCallback;


/*
typedef CallBack0< vtkXYPlotActor, double*> CallbackGetRange;
typedef CallBackv1< vtkXYPlotActor, void, double*> CallbackGetRange2;
typedef CallBackv1< vtkXYPlotActor, void, double*> CallbackSetRange;
typedef CallBackv2< vtkXYPlotActor, void, double&, double&> CallbackSetRange2;

typedef CallBack0< vtkXYPlotActor, int32_t> CallbackGetInt;
typedef CallBackv1< vtkXYPlotActor, void, int32_t> CallbackSetInt;

typedef CallBack0< vtkXYPlotActor, double> CallbackGetDouble;
typedef CallBackv1< vtkXYPlotActor, void, double> CallbackSetDouble;

typedef CallBack0< vtkXYPlotActor, char*> CallbackGetString;
typedef CallBackv1< vtkXYPlotActor, void, char*> CallbackSetString;
*/


// WDR: class declarations

//-------------------------------------------------------------
//------------------- CAxisPropertyPanel class --------------------
//-------------------------------------------------------------

class CAxisPropertyPanel: public wxPanel
{
public:
  // constructors and destructors
  CAxisPropertyPanel();


  CAxisPropertyPanel( wxWindow *parent, vtkXYPlotActor* plotter, CXYPlotDataCollection* plotDataCollection, wxVTKRenderWindowInteractor* vtkWidget,
                      wxWindowID id = -1,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxTAB_TRAVERSAL|wxSUNKEN_BORDER );


  virtual ~CAxisPropertyPanel();

  virtual bool Create( wxWindow *parent, vtkXYPlotActor* plotter, CXYPlotDataCollection* plotDataCollection, wxVTKRenderWindowInteractor* vtkWidget,
                       wxWindowID id = -1,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxTAB_TRAVERSAL|wxSUNKEN_BORDER );

  virtual wxString GetTitle() = 0;
  virtual void SetTitle(const wxString& value) = 0;

  virtual int32_t GetNumberOfLabels() = 0;
  virtual void SetNumberOfLabels(int32_t value) = 0;

  virtual bool GetLog() = 0;
  virtual void SetLog(bool value) = 0;

  virtual double GetBase() = 0;
  virtual void SetBase(double value) = 0;

  virtual void GetRange(double& min, double& max) = 0;
  virtual void SetRange(double min, double max) = 0;

  virtual void GetBaseRange(double& min, double& max) = 0;

  virtual void GetComputedRange(double& min, double& max) = 0;

  virtual void GetDataRange(double& min, double& max) = 0;
  virtual void GetDataRange(double& min, double& max, uint32_t frame) = 0;

  virtual void ZoomToRange(double min, double max) = 0;

  virtual void UpdateControls();
  virtual void UpdateFallBackRangeFromPlot();
  virtual void UpdateCurrentRangeFromPlot();
  virtual void UpdateLogAxisFromControls();
  virtual void UpdateBaseFromControls();

  virtual void AxisResetRange() = 0;
  virtual void DigitsChanged() = 0;
  


    // WDR: method declarations for CAxisPropertyPanel
public:
  bool m_finished;

public:
  
  static const uint32_t m_DIGITS_DEF;
  static const uint32_t m_DIGITS_MIN;
  static const uint32_t m_DIGITS_MAX;
    
protected:
  wxVTKRenderWindowInteractor* m_vtkWidget;
  vtkXYPlotActor* m_plotter;
  CVtkComputeRangeCallback* m_vtkComputeRangeCallback;

  CXYPlotDataCollection* m_plotDataCollection;
  CLabeledTextCtrl m_titleCtrl;
  CLabeledTextCtrl m_nTicksCtrl;
  CLabeledTextCtrl m_baseCtrl;
  CLabeledTextCtrl m_digitsCtrl;
  CLabeledTextCtrl m_minCtrl;
  CLabeledTextCtrl m_maxCtrl;
  CLabeledTextCtrl m_dynMinCtrl;
  CLabeledTextCtrl m_dynMaxCtrl;

  wxButton m_resetAxisRange;

  wxRadioButton m_deriveRadioButton;
  wxRadioButton m_manualRadioButton;

  wxCheckBox m_logAxisCtrl;

  wxStaticBox* m_box1;
  wxStaticBox* m_box2;

  wxBoxSizer* m_rSizer1;
  wxBoxSizer* m_rSizer2;
  wxBoxSizer* m_rSizer3;

  wxStaticBoxSizer* m_sizer1;
  wxStaticBoxSizer* m_sizer2;

  wxBoxSizer* m_sizer;
  wxBoxSizer* m_mainSizer;


  long m_ID_TITLE;
  long m_ID_NTICKS;
  long m_ID_DERIVE;
  long m_ID_MANUAL;
  long m_ID_LOG;
  long m_ID_BASE;
  long m_ID_MIN;
  long m_ID_MAX;
  long m_ID_DYN_MIN;
  long m_ID_DYN_MAX;
  long m_ID_RESET_AXIS_RANGE;
  long m_ID_DIGITSAXIS;

  int32_t m_maxTicks;

  uint32_t m_numberOfDigits;

    // WDR: member variable declarations for CAxisPropertyPanel

protected:
  virtual void Init();

  virtual void CreateBoxSizers();

  virtual bool CreateControls();
  virtual void CreateLayout();
  virtual void InstallToolTips();
  virtual void InstallEventListeners();
  // WDR: handler declarations for CAxisPropertyPanel
  void OnLog(wxCommandEvent& event);
  void OnBase(CValueChangedEvent& event);
  void OnDerive(wxCommandEvent& event);
  void OnManual(wxCommandEvent& event);
  void OnTitle(CValueChangedEvent& event);
  void OnNTicks(CValueChangedEvent& event);
  void OnDigits(CValueChangedEvent& event);
  void OnMin(CValueChangedEvent& event);
  void OnMax(CValueChangedEvent& event);
  void OnDynMin(CValueChangedEvent& event);
  void OnDynMax(CValueChangedEvent& event);
  void OnAxisResetRange( wxCommandEvent &event );




private:
    DECLARE_EVENT_TABLE()
};

//-------------------------------------------------------------
//------------------- CAxisXPropertyPanel class --------------------
//-------------------------------------------------------------

class CAxisXPropertyPanel: public CAxisPropertyPanel
{
public:
  // constructors and destructors
  CAxisXPropertyPanel();


  CAxisXPropertyPanel( wxWindow *parent, vtkXYPlotActor* plotter, CXYPlotDataCollection* plotDataCollection, wxVTKRenderWindowInteractor* vtkWidget,
                      wxWindowID id = -1,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxTAB_TRAVERSAL|wxSUNKEN_BORDER );


  virtual ~CAxisXPropertyPanel();


  virtual wxString GetTitle();
  virtual void SetTitle(const wxString& value);

  virtual int32_t GetNumberOfLabels();
  virtual void SetNumberOfLabels(int32_t value);

  virtual bool GetLog();
  virtual void SetLog(bool value);

  virtual double GetBase();
  virtual void SetBase(double value);

  virtual void GetRange(double& min, double& max);
  virtual void SetRange(double min, double max);

  virtual void GetBaseRange(double& min, double& max);

  virtual void GetComputedRange(double& min, double& max);

  virtual void GetDataRange(double& min, double& max);
  virtual void GetDataRange(double& min, double& max, uint32_t frame);

  virtual void ZoomToRange(double min, double max);
  
  virtual void AxisResetRange();
  virtual void DigitsChanged();

  // WDR: method declarations for CAxisXPropertyPanel
    
private:

  // WDR: member variable declarations for CAxisXPropertyPanel
    
private:

  // WDR: handler declarations for CAxisXPropertyPanel

private:
    DECLARE_EVENT_TABLE()
};

//-------------------------------------------------------------
//------------------- CAxisYPropertyPanel class --------------------
//-------------------------------------------------------------

class CAxisYPropertyPanel: public CAxisPropertyPanel
{
public:
  // constructors and destructors
  CAxisYPropertyPanel();


  CAxisYPropertyPanel( wxWindow *parent, vtkXYPlotActor* plotter, CXYPlotDataCollection* plotDataCollection, wxVTKRenderWindowInteractor* vtkWidget,
                      wxWindowID id = -1,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxTAB_TRAVERSAL|wxSUNKEN_BORDER );


  virtual ~CAxisYPropertyPanel();


  virtual wxString GetTitle();
  virtual void SetTitle(const wxString& value);

  virtual int32_t GetNumberOfLabels();
  virtual void SetNumberOfLabels(int32_t value);

  virtual bool GetLog();
  virtual void SetLog(bool value);

  virtual double GetBase();
  virtual void SetBase(double value);

  virtual void GetRange(double& min, double& max);
  virtual void SetRange(double min, double max);

  virtual void GetBaseRange(double& min, double& max);

  virtual void GetComputedRange(double& min, double& max);

  virtual void GetDataRange(double& min, double& max);
  virtual void GetDataRange(double& min, double& max, uint32_t frame);

  virtual void ZoomToRange(double min, double max);

  virtual void AxisResetRange();
  virtual void DigitsChanged();

  // WDR: method declarations for CAxisYPropertyPanel
    
private:

  // WDR: member variable declarations for CAxisYPropertyPanel
    
private:

  // WDR: handler declarations for CAxisYPropertyPanel

private:
    DECLARE_EVENT_TABLE()
};


//-------------------------------------------------------------
//------------------- CVtkComputeRangeCallback class --------------------
//-------------------------------------------------------------

class CVtkComputeRangeCallback : public vtkCommand
{
protected:
  CVtkComputeRangeCallback(CAxisPropertyPanel *axisPropPanel)
    {
      m_axisPropPanel = axisPropPanel;
    }

public:  
  static CVtkComputeRangeCallback *New(CAxisPropertyPanel *axisPropPanel) 
    { return new CVtkComputeRangeCallback(axisPropPanel); }
  
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      if (m_axisPropPanel == NULL)
      {
        return;
      }
      m_axisPropPanel->UpdateCurrentRangeFromPlot();

    }
protected:

  CAxisPropertyPanel* m_axisPropPanel;
};




#endif
