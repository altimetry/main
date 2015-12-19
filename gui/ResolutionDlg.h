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

#ifndef __ResolutionDlg_H__
#define __ResolutionDlg_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "ResolutionDlg.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "BratGui_wdr.h"

#include "LabeledTextCtrl.h"

#include "new-gui/brat/Workspaces/Operation.h"

// WDR: class declarations

const wxString XRESOLUTION_LABELFORMAT = " X Resolution (expression: '%s' unit: %s) ";
const wxString YRESOLUTION_LABELFORMAT = " Y Resolution (expression: '%s' unit: %s) ";

//----------------------------------------------------------------------------
// CResolutionDlg
//----------------------------------------------------------------------------

class CResolutionDlg: public wxDialog
{
public:
  // constructors and destructors
  CResolutionDlg( wxWindow *parent, wxWindowID id, const wxString &title,
                  COperation* operation,
                  CFormula* formula,
                  const CStringMap& mapFormulaString,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
  
  virtual ~CResolutionDlg();

  // WDR: method declarations for CResolutionDlg
  wxButton* GetFormulaoptYgetminmax()  { return (wxButton*) FindWindow( ID_FORMULAOPT_YGETMINMAX ); }
  wxButton* GetFormulaoptXgetminmax()  { return (wxButton*) FindWindow( ID_FORMULAOPT_XGETMINMAX ); }
  wxStaticText* GetFormulaoptIntervalXWarning()  { return (wxStaticText*) FindWindow( ID_FORMULAOPT_INTERVAL_X_WARNING ); }
  wxStaticText* GetFormulaoptIntervalYWarning()  { return (wxStaticText*) FindWindow( ID_FORMULAOPT_INTERVAL_Y_WARNING ); }
  wxButton* GetFormulaoptHelpYStep()  { return (wxButton*) FindWindow( ID_FORMULAOPT_HELP_Y_STEP ); }
  wxButton* GetFormulaoptHelpXStep()  { return (wxButton*) FindWindow( ID_FORMULAOPT_HELP_X_STEP ); }
  wxButton* GetFormulaoptHelpYMinMax()  { return (wxButton*) FindWindow( ID_FORMULAOPT_HELP_Y_MIN_MAX ); }
  wxButton* GetFormulaoptHelpXMinMax()  { return (wxButton*) FindWindow( ID_FORMULAOPT_HELP_X_MIN_MAX ); }
  CLabeledTextCtrl* GetFormulaoptYinterval()  { return (CLabeledTextCtrl*) FindWindow( ID_FORMULAOPT_YINTERVAL ); }
  CLabeledTextCtrl* GetFormulaoptYstep()  { return (CLabeledTextCtrl*) FindWindow( ID_FORMULAOPT_YSTEP ); }
  CLabeledTextCtrl* GetFormulaoptYloesscut()  { return (CLabeledTextCtrl*) FindWindow( ID_FORMULAOPT_YLOESSCUT ); }
  CLabeledTextCtrl* GetFormulaoptYmax()  { return (CLabeledTextCtrl*) FindWindow( ID_FORMULAOPT_YMAX ); }
  CLabeledTextCtrl* GetFormulaoptYmin()  { return (CLabeledTextCtrl*) FindWindow( ID_FORMULAOPT_YMIN ); }
  CLabeledTextCtrl* GetFormulaoptXinterval()  { return (CLabeledTextCtrl*) FindWindow( ID_FORMULAOPT_XINTERVAL ); }
  CLabeledTextCtrl* GetFormulaoptXstep()  { return (CLabeledTextCtrl*) FindWindow( ID_FORMULAOPT_XSTEP ); }
  CLabeledTextCtrl* GetFormulaoptXloesscut()  { return (CLabeledTextCtrl*) FindWindow( ID_FORMULAOPT_XLOESSCUT ); }
  CLabeledTextCtrl* GetFormulaoptXmax()  { return (CLabeledTextCtrl*) FindWindow( ID_FORMULAOPT_XMAX ); }
  CLabeledTextCtrl* GetFormulaoptXmin()  { return (CLabeledTextCtrl*) FindWindow( ID_FORMULAOPT_XMIN ); }
  wxRadioBox* GetFormulaoptFilter()  { return (wxRadioBox*) FindWindow( ID_FORMULAOPT_FILTER ); }
    

private:
  // WDR: member variable declarations for CResolutionDlg
  COperation* m_operation;
  CFormula* m_formula;
  CFormula* m_x;
  CFormula* m_y;
  
  //double m_xMinValue;
  //double m_xMaxValue;
//  int32_t m_xInterval;
//  wxString m_xStep;

//  double m_yMinValue;
  //double m_yMaxValue;
  //int32_t m_yInterval;
  //wxString m_yStep;

  double m_xMinValueDefault;
  double m_xMaxValueDefault;
  int32_t m_xIntervalDefault;

  double m_yMinValueDefault;
  double m_yMaxValueDefault;
  int32_t m_yIntervalDefault;

  CFormula m_xFormulaTmp;
  CFormula m_yFormulaTmp;

  const CStringMap& m_mapFormulaString;

    
private:
  void Init();
  void EnableCtrl();
  bool ValidateData();

  void GetCtrlMinMaxAsDate(CFormula& formula, CLabeledTextCtrl* ctrlMin, CLabeledTextCtrl* ctrlMax);
  void GetCtrlMinMax(CFormula& formula, CLabeledTextCtrl* ctrlMin, CLabeledTextCtrl* ctrlMax, double minDefault, double maxDefault);
  
  void InitCtrlMinMax(CFormula& formula, CLabeledTextCtrl* ctrlMin, CLabeledTextCtrl* ctrlMax, 
                      double minDefault = CTools::m_defaultValueDOUBLE, double maxDefault = CTools::m_defaultValueDOUBLE);
  void InitCtrlMinMaxAsDate(CFormula& formula, CLabeledTextCtrl* ctrlMin, CLabeledTextCtrl* ctrlMax);


  bool VerifyMinMax();
  bool VerifyXMinMax();
  bool VerifyYMinMax();
  /*bool VerifyXMinMaxAsLatLon();
  bool VerifyYMinMaxAsLatLon();
  bool VerifyXMinMaxAsDouble();
  bool VerifyYMinMaxAsDouble();
  */
  bool VerifyMinMaxAsDate(CFormula& formula, CLabeledTextCtrl* ctrlMin, CLabeledTextCtrl* ctrlMax);
  bool VerifyMinMax(CFormula& formula, CLabeledTextCtrl* ctrlMin, CLabeledTextCtrl* ctrlMax,
                      double minDefault = CTools::m_defaultValueDOUBLE, double maxDefault = CTools::m_defaultValueDOUBLE);

  bool ComputeInterval();
  bool ComputeInterval(CFormula& formula, CLabeledTextCtrl* ctrlInterval, CLabeledTextCtrl* ctrlStep, wxStaticText* ctrlWarning);
  bool ComputeXInterval();
  bool ComputeYInterval();

  void HelpStep(CFormula* formula);
  void HelpMinMax(CFormula* formula);

  void GetMinMax(CFormula* formula);

  static bool IsZero (double X);

    // WDR: handler declarations for CResolutionDlg
  void OnGetYMinMax( wxCommandEvent &event );
  void OnGetXMinMax( wxCommandEvent &event );
  void OnHelpXMinMax( wxCommandEvent &event );
  void OnHelpXStep( wxCommandEvent &event );
  void OnHelpYMinMax( wxCommandEvent &event );
  void OnHelpYStep( wxCommandEvent &event );
  void OnOk( wxCommandEvent &event );
  void OnCancel( wxCommandEvent &event );
  void OnXMin(CValueChangedEvent& event);
  void OnXMax(CValueChangedEvent& event);
  void OnXStep(CValueChangedEvent& event);
  void OnXInterval(CValueChangedEvent& event);
  void OnXLoessCut(CValueChangedEvent& event);
  void OnYMin(CValueChangedEvent& event);
  void OnYMax(CValueChangedEvent& event);
  void OnYStep(CValueChangedEvent& event);
  void OnYInterval(CValueChangedEvent& event);
  void OnYLoessCut(CValueChangedEvent& event);

private:
    DECLARE_CLASS(CResolutionDlg)
    DECLARE_EVENT_TABLE()
};


#endif
