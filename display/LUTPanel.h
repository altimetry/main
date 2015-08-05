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


#ifndef __LUTPanel_H__
#define __LUTPanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "LUTPanel.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "vtkScalarBarActor.h"
#include "vtkRenderer.h"

#include "wxVTKRenderWindowInteractor.h"

#include "BratDisplay_wdr.h"


#include "LUTFrame.h"
#include "ColorPicker.h"

#include "BratLookupTable.h"


extern long ID_CLUTPANEL_APPLY_BUTTON;

extern long ID_STDMODIFYPANEL_DEFBOX;

extern long ID_GRDMODIFYPANEL_LEFT_COLOR_PICKER;
extern long ID_GRDMODIFYPANEL_RIGHT_COLOR_PICKER;

class CLUTPanel;
class CLUTFrame;


BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_CLUT_CHANGED, 7777) // 7777 is ignored, just for compatibility with v2.2
    DECLARE_EVENT_TYPE(wxEVT_CLUT_CLOSE, 7777) // 7777 is ignored, just for compatibility with v2.2
END_DECLARE_EVENT_TYPES()
//-------------------------------------------------------------
//------------------- CLutChangedEvent class --------------------
//-------------------------------------------------------------


class CLutChangedEvent : public wxCommandEvent
{
public:
  CLutChangedEvent(wxWindowID id, CBratLookupTable* lut)
    : wxCommandEvent(wxEVT_CLUT_CHANGED, id)
  {
    m_LUT = lut; //not a copy
    m_id = id;
  };

  CLutChangedEvent(const CLutChangedEvent& event)
    : wxCommandEvent(wxEVT_CLUT_CHANGED, event.m_id)
  {
    m_LUT = event.m_LUT; //not a copy
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CLutChangedEvent(*this); 
    };

  wxWindowID m_id;
  CBratLookupTable* m_LUT;
};
typedef void (wxEvtHandler::*CLutChangedEventFunction)(CLutChangedEvent&);

//-------------------------------------------------------------
//------------------- CLutFrameCloseEvent class --------------------
//-------------------------------------------------------------


class CLutFrameCloseEvent : public wxCommandEvent
{
public:
  CLutFrameCloseEvent(wxWindowID id)
    : wxCommandEvent(wxEVT_CLUT_CLOSE, id)
  {
    m_id = id;
  };

  CLutFrameCloseEvent(const CLutFrameCloseEvent& event)
    : wxCommandEvent(wxEVT_CLUT_CLOSE, event.m_id)
  {
    m_id = event.m_id;
  };
  
  virtual wxEvent *Clone() const 
    {
      return new CLutFrameCloseEvent(*this); 
    };

  wxWindowID m_id;
};
typedef void (wxEvtHandler::*CLutFrameCloseEventFunction)(CLutFrameCloseEvent&);


// WDR: class declarations

//-------------------------------------------------------------
//------------------- CCustomModifyPanel class --------------------
//-------------------------------------------------------------

class CCustomModifyPanel: public wxPanel
{
public:
    // constructors and destructors
  CCustomModifyPanel();
  CCustomModifyPanel( CLUTPanel *parent, CBratLookupTable* lut, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );

  virtual ~CCustomModifyPanel();
    
  bool Create( CLUTPanel *parent, CBratLookupTable* lut, wxWindowID id = -1,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxTAB_TRAVERSAL | wxNO_BORDER );

  CBratLookupTable* GetLUT() {return m_LUT;};

  void SetColor();
  void SetLUT(CBratLookupTable* lut, bool updateParent = false);

  vtkLookupTable* GetLookupTable() 
    { 
      if (m_LUT == NULL)
      {
        return NULL;
      }
      return m_LUT->GetLookupTable();
    };
  
  void ResetLUT(CBratLookupTable* lut);
  void Reset();

  void SetFacets();


  // WDR: method declarations for CCustomModifyPanel
    
private:
  void Init();
  void DeleteLUT();
  void ResetCust();
  void ResetSlider();

  void SetList();
  void FillList();

  int32_t InsertG(int32_t gValue);
  int32_t InsertGReverse(int32_t gValue); 


  void OnGSelect2();
  void OnGSelect(wxCommandEvent& event);
  void OnGChange(wxCommandEvent& event);
  void OnSlider(wxScrollEvent& event);
  void OnInsert(wxCommandEvent& event) ;
  void OnRemove(wxCommandEvent& event) ;
  void OnDeselect(wxCommandEvent& event) ;
  void OnColorChange( CColorChangedEvent& event );

  // WDR: member variable declarations for CCustomModifyPanel
    
private:
  CBratLookupTable* m_LUT;

  CObArray m_cust;

  CLUTPanel* m_parent;

  CColorPicker* m_colorPicker;
  
  wxButton* m_insertButton;
  wxButton* m_removeButton;
  wxButton* m_deselectButton;

  wxStaticText* m_label;

  wxListBox* m_gList;

  wxSlider* m_slider;
  wxBoxSizer* m_sizer;
  wxBoxSizer* m_sizer1;
  wxBoxSizer* m_sizer2;
  wxBoxSizer* m_butSizer;
  wxBoxSizer* m_hSizer;

  // WDR: handler declarations for CCustomModifyPanel

private:
    DECLARE_EVENT_TABLE()
};

//-------------------------------------------------------------
//------------------- CGradientModifyPanel class --------------------
//-------------------------------------------------------------

class CGradientModifyPanel: public wxPanel
{
public:
    // constructors and destructors
  CGradientModifyPanel();
  CGradientModifyPanel( CLUTPanel *parent, CBratLookupTable* lut, wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );

  virtual ~CGradientModifyPanel();
  
  bool Create( CLUTPanel *parent, CBratLookupTable* lut, wxWindowID id = -1,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxTAB_TRAVERSAL | wxNO_BORDER );

  CBratLookupTable* GetLUT() {return m_LUT;};

  void SetColor();
  void SetLUT(CBratLookupTable* lut, bool updateParent = false);

  vtkLookupTable* GetLookupTable() 
    { 
      if (m_LUT == NULL)
      {
        return NULL;
      }
      return m_LUT->GetLookupTable();
    };
  void ResetLUT(CBratLookupTable* lut);
  void Reset();
    
    // WDR: method declarations for CGradientModifyPanel
    
private:
  CBratLookupTable* m_LUT;

  CLUTPanel* m_parent;

  CColorPicker* m_leftColorPicker;
  CColorPicker* m_rightColorPicker;
  
  wxBoxSizer* m_sizer;


    // WDR: member variable declarations for CGradientModifyPanel
    
private:
  void Init();
  void DeleteLUT();
  void OnColorChange( CColorChangedEvent& event );

    // WDR: handler declarations for CGradientModifyPanel

private:
    DECLARE_EVENT_TABLE()
};

//-------------------------------------------------------------
//------------------- CStandardModifyPanel class --------------------
//-------------------------------------------------------------

class CStandardModifyPanel: public wxPanel
{
public:
  // constructors and destructors
  CStandardModifyPanel();
  CStandardModifyPanel( CLUTPanel *parent, CBratLookupTable* lut, wxWindowID id = -1,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
  virtual ~CStandardModifyPanel();

  bool Create( CLUTPanel *parent, CBratLookupTable* lut, wxWindowID id = -1,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxTAB_TRAVERSAL | wxNO_BORDER );

  CBratLookupTable* GetLUT() {return m_LUT;};

  void SetLUT(CBratLookupTable* lut, bool updateParent = false);

  vtkLookupTable* GetLookupTable() 
    { 
      if (m_LUT == NULL)
      {
        return NULL;
      }
      return m_LUT->GetLookupTable();
    };
  void ResetLUT(CBratLookupTable* lut);
  void Reset();

  void DeInstallEventListeners();

    // WDR: method declarations for CStandardModifyPanel
    
private:
  CBratLookupTable* m_LUT;

  CLUTPanel* m_parent;

  wxStaticText* m_label;
  wxListBox* m_defBox;
  wxBoxSizer* m_sizer;

    // WDR: member variable declarations for CStandardModifyPanel
    
private:
  
  void Init();
  void DeleteLUT();
  void SetDefaultLUT();
  void UpdateRender();
  void OnDefSelect(wxCommandEvent &event);

    // WDR: handler declarations for CStandardModifyPanel

private:
    DECLARE_EVENT_TABLE()
};

//-------------------------------------------------------------
//------------------- CLUTPanel class --------------------
//-------------------------------------------------------------

class CLUTPanel: public wxPanel
{
public:
  // constructors and destructors
  CLUTPanel();
  CLUTPanel( CLUTFrame *parent, CBratLookupTable* lut, wxWindowID id = -1, bool applyButton = false,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );
  virtual ~CLUTPanel();

  bool Create( CLUTFrame *parent, CBratLookupTable* lut, wxWindowID id = -1, bool applyButton = false,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );


  CBratLookupTable* GetLUT() {return m_LUT;};

  void SetLUT(CBratLookupTable* lut);

  vtkLookupTable* GetLookupTable() 
    { 
      if (m_LUT == NULL)
      {
        return NULL;
      }
      return m_LUT->GetLookupTable();
    };

  void ResetLUT();

  void UpdateForFacets(int32_t n);
  void UpdateForCurveLinear();
  void UpdateForCurveCosinus();
  void UpdateForCurveSQRT();


  void UpdateRender();
  bool Destroy();

  void SetModeToStandard(bool updateLUTPanel = true);
  void SetModeToGradient(bool updateLUTPanel = true);
  void SetModeToCustom(bool updateLUTPanel = true);

  CStandardModifyPanel* GetStandardModifyPanel() {return m_standardModifyPanel;};
  CGradientModifyPanel* GetGradientModifyPanel() {return m_gradientModifyPanel;};
  CCustomModifyPanel* GetCustomModifyPanel() {return m_customModifyPanel;};

  bool IsStandardModifyPanelVisible() {return m_standardModifyPanel->IsShown();};
  bool IsGradientModifyPanelVisible() {return m_gradientModifyPanel->IsShown();};
  bool IsCustomModifyPanelVisible() {return m_customModifyPanel->IsShown();};

  void DeInstallEventListeners();

  
  static void EvtLutChanged(wxWindow& window, int32_t winId, const CLutChangedEventFunction& method,
                                          wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);
  static void EvtLutFrameClose(wxWindow& window, int32_t winId, const CLutFrameCloseEventFunction& method,
                                          wxObject* userData = NULL, wxEvtHandler* eventSink = NULL);


    // WDR: method declarations for CLUTPanel
    
private:
  bool m_apply;
  bool m_finished;

  CBratLookupTable* m_LUT;

  vtkScalarBarActor* m_scalarBarActor;
  vtkRenderer* m_vtkRend;
  wxVTKRenderWindowInteractor* m_vtkWidget;


  CStandardModifyPanel* m_standardModifyPanel;
  CGradientModifyPanel* m_gradientModifyPanel;
  CCustomModifyPanel* m_customModifyPanel;

  wxButton* m_applyButton;

  wxBoxSizer* m_sizer;
  wxBoxSizer* m_vSizer;

  CLUTFrame* m_parent;

  // WDR: member variable declarations for CLUTPanel
    
private:
  void Init();

  void DeleteLUT();

  bool CreateControls();
  void CreateLayout();
  void InstallToolTips();
  void InstallEventListeners();

  void OnApply(wxCommandEvent& event);

    // WDR: handler declarations for CLUTPanel

private:
    DECLARE_EVENT_TABLE()
};




#endif
