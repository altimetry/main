/*
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

#ifndef __ExportDlg_H__
#define __ExportDlg_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/filename.h" 

#include "BratGui_wdr.h"

#include "LabeledTextCtrl.h"
#include "new-gui/brat/DataModels/Workspaces/Operation.h"
#include "DelayDlg.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CExportDlg
//----------------------------------------------------------------------------

class CExportDlg: public wxDialog
{
public:
  // constructors and destructors
  CExportDlg( wxWindow *parent, wxWindowID id, const wxString &title,
      COperation* operation, const CStringMap* aliases = NULL, 
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_DIALOG_STYLE );
  virtual ~CExportDlg();
  
  // WDR: method declarations for CExportDlg
  CLabeledTextCtrl* GetExportAsciiNumberPrecision()  { return (CLabeledTextCtrl*) FindWindow( ID_EXPORT_ASCII_NUMBER_PRECISION ); }
  wxCheckBox* GetExportNoDataComputation()  { return (wxCheckBox*) FindWindow( ID_EXPORT_NO_DATA_COMPUTATION ); }
  wxCheckBox* GetExpandArray()  { return (wxCheckBox*) FindWindow( ID_EXPAND_ARRAY ); }
  wxCheckBox* GetDateAsPeriod()  { return (wxCheckBox*) FindWindow( ID_DATE_AS_PERIOD ); }
  wxCheckBox* GetExecagain()  { return (wxCheckBox*) FindWindow( ID_EXECAGAIN ); }
  wxRadioBox* GetExportformat()  { return (wxRadioBox*) FindWindow( ID_EXPORTFORMAT ); }
  wxButton* GetExportBrowse()  { return (wxButton*) FindWindow( ID_EXPORT_BROWSE ); }
  wxButton* GetCalcColorRange()  { return (wxButton*) FindWindow( ID_EXPORT_CALC_COLOR_RANGE ); }
  wxTextCtrl* GetExportoutputfile()  { return (wxTextCtrl*) FindWindow( ID_EXPORTOUTPUTFILE ); }
  wxCheckBox* GetCreateKMLFile()  { return (wxCheckBox*) FindWindow( ID_EXPORT_CREATE_KML_FILE ); }
  wxComboBox* GetColorTable()  { return (wxComboBox*) FindWindow( ID_EXPORT_COLOR_TABLE ); }
  wxStaticText* GetColorTableLabel()  { return (wxStaticText*) FindWindow( ID_EXPORT_COLOR_TABLE_LABEL ); }
  CLabeledTextCtrl* GetColorRangeMin()  { return (CLabeledTextCtrl*) FindWindow( ID_EXPORT_COLORRANGE_MIN ); }
  CLabeledTextCtrl* GetColorRangeMax()  { return (CLabeledTextCtrl*) FindWindow( ID_EXPORT_COLORRANGE_MAX ); }

  void Format( int32_t format );
  bool AsNetCdf() { return (m_format == CExportDlg::AS_NETCDF); };
  bool AsAscii() { return (m_format == CExportDlg::AS_ASCII); };
  bool AsGeoTiff() { return (m_format == CExportDlg::AS_GEOTIFF); };

  static int32_t m_defaultDecimalPrecision;

public:
  COperation* m_operation;
    
  wxFileName m_currentName;
  wxFileName m_initialName;

  enum exportFormat {
                    AS_ASCII,
                    AS_NETCDF,
                    AS_GEOTIFF
                    };

  int32_t m_format;

  CDelayDlg m_delayDlg;
  bool m_delayExecution;

  bool m_executeAgain;
  bool m_dateAsPeriod;
  bool m_expandArray;
  bool m_noDataComputation;

  int32_t m_asciiNumberPrecision;
  
  bool m_isGeoImage;
  bool m_createKML;
  wxString m_colorTable;
  double m_colorRangeMin;
  double m_colorRangeMax;
  

  const CStringMap* m_aliases;

  static wxString m_defaultExtensionAscii;
  static wxString m_defaultExtensionNetCdf;
  static wxString m_defaultExtensionGeoTiff;


private:
  // WDR: member variable declarations for CExportDlg
    
private:
  void CalculateColorValueRange( void );

  // WDR: handler declarations for CExportDlg
  void OnDelayExecution( wxCommandEvent &event );
  void OnNoDataComputation( wxCommandEvent &event );
  void OnExpandArray( wxCommandEvent &event );
  void OnDateAsPeriod( wxCommandEvent &event );
  void OnExecAgain( wxCommandEvent &event );
  void OnFormat( wxCommandEvent &event );
  void OnBrowse( wxCommandEvent &event );
  void OnCalcColorRange( wxCommandEvent &event );
  void OnOk( wxCommandEvent &event );
  void OnCancel( wxCommandEvent &event );

private:
  DECLARE_CLASS(CExportDlg)
  DECLARE_EVENT_TABLE()
};




#endif
