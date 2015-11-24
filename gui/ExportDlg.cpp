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
    #pragma implementation "ExportDlg.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "BratGui.h"
#include "DelayDlg.h"
#include "ExportDlg.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CExportDlg
//----------------------------------------------------------------------------

wxString CExportDlg::m_defaultExtensionAscii = "txt";
wxString CExportDlg::m_defaultExtensionNetCdf = "nc";
wxString CExportDlg::m_defaultExtensionGeoTiff = "tif";
int32_t  CExportDlg::m_defaultDecimalPrecision = 10;


IMPLEMENT_CLASS(CExportDlg,wxDialog)

// WDR: event table for CExportDlg

BEGIN_EVENT_TABLE(CExportDlg,wxDialog)
    EVT_BUTTON( wxID_OK, CExportDlg::OnOk )
    EVT_BUTTON( wxID_CANCEL, CExportDlg::OnCancel )
    EVT_BUTTON( ID_EXPORT_BROWSE, CExportDlg::OnBrowse )
    EVT_BUTTON( ID_EXPORT_CALC_COLOR_RANGE, CExportDlg::OnCalcColorRange )
    EVT_RADIOBOX( ID_EXPORTFORMAT, CExportDlg::OnFormat )
    EVT_CHECKBOX( ID_EXECAGAIN, CExportDlg::OnExecAgain )
    EVT_CHECKBOX( ID_DATE_AS_PERIOD, CExportDlg::OnDateAsPeriod )
    EVT_CHECKBOX( ID_EXPAND_ARRAY, CExportDlg::OnExpandArray )
    EVT_CHECKBOX( ID_EXPORT_NO_DATA_COMPUTATION, CExportDlg::OnNoDataComputation )
    EVT_BUTTON( ID_EXPORT_DELAY_EXECUTION, CExportDlg::OnDelayExecution )
END_EVENT_TABLE()
//----------------------------------------

CExportDlg::CExportDlg( wxWindow *parent, wxWindowID id, const wxString &title,
                        COperation* operation, const CStringMap* aliases /* = NULL*/,
                        const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style ), 
    m_delayDlg(this, -1, "Delay execution...")
{
  m_delayExecution = false;
  m_operation = operation;
  m_noDataComputation = false;
  m_executeAgain = false;
  m_dateAsPeriod = false;
  m_expandArray = false;
  m_isGeoImage = m_operation->IsMap() && m_operation->GetFormulaCountDataFields() > 0;
  m_createKML = false;
  m_colorTable = "Aerosol";
  setDefaultValue(m_colorRangeMin);
  setDefaultValue(m_colorRangeMax);

  m_asciiNumberPrecision = CExportDlg::m_defaultDecimalPrecision;

  m_aliases = aliases;

  // WDR: dialog function ExportDlg for CExportDlg
  //----------------------------
  ExportDlg( this, TRUE );
  //----------------------------

  if (m_isGeoImage)
  {
    GetColorTable()->SetSelection(0);
  }
  else
  {
    glb_ExportOptionsSizer->Hide(glb_ExportGeoTiffSizer, true);
    GetExportformat()->Show(AS_GEOTIFF, false);
  }


  m_initialName = operation->GetExportAsciiOutputName();
  m_currentName = m_initialName;

  m_currentName.Normalize();
  GetExportoutputfile()->SetValue(m_currentName.GetFullPath());

  GetExportAsciiNumberPrecision()->SetValue(m_asciiNumberPrecision);
  GetExportAsciiNumberPrecision()->SetMaxLength(2);
  GetExportAsciiNumberPrecision()->SetToolTip(wxString::Format("Precision specifies the minimum number of digits to be written after the decimal point.\n"
                                                       "This precision is applied on all fields, except for date std::string representation.\n"
                                                       "When no precision is specified, the decimal precision is %d.\n"
                                                       "The maximum valid precision depends on the operating system (usually 16).\n",
                                                        m_asciiNumberPrecision));


  Format(CExportDlg::AS_ASCII);

}
//----------------------------------------

CExportDlg::~CExportDlg()
{
}
//----------------------------------------

void CExportDlg::CalculateColorValueRange( void )
{
  CFormula *formula = m_operation->GetFormula(CMapTypeField::typeOpAsField);

  CProduct* productTmp = CProduct::Construct(*(m_operation->GetDataset()->GetProductList()));

  wxString errorMsg;
  CExpression expr;
  bool bOk = CFormula::SetExpression((const char *)formula->GetDescription(true, m_aliases, productTmp->GetAliasesAsString()).c_str(), expr, errorMsg);

  if (!bOk)
  {
      wxMessageBox(wxString::Format("Unable to calculate min/max. Expressson can't be set:\n'%s'" ,
                                    errorMsg.c_str()),
                   "Warning",
                    wxOK | wxCENTRE | wxICON_EXCLAMATION);

  }

  productTmp->GetValueMinMax(expr, (const char *)m_operation->GetRecord().c_str(), m_colorRangeMin, m_colorRangeMax, *(formula->GetUnit()));

  GetColorRangeMin()->SetValue(m_colorRangeMin);
  GetColorRangeMax()->SetValue(m_colorRangeMax);

  if (productTmp != NULL)
  {
    delete productTmp;
    productTmp = NULL;
  }
}

//----------------------------------------

// WDR: handler implementations for CExportDlg

//----------------------------------------
void CExportDlg::OnDelayExecution( wxCommandEvent &event )
{
  m_delayExecution = false;
  
  int32_t result = m_delayDlg.ShowModal();
  
  if (result != wxID_OK)
  {
    return;
  }

  m_delayExecution = true;

  EndModal(wxID_OK);

}
//----------------------------------------
void CExportDlg::OnNoDataComputation( wxCommandEvent &event )
{
  m_noDataComputation = GetExportNoDataComputation()->GetValue();

  bool enableExecAgain = AsNetCdf() || (AsAscii() && !m_noDataComputation);

  GetExecagain()->Enable(enableExecAgain);

}

//----------------------------------------
void CExportDlg::OnExpandArray( wxCommandEvent &event )
{
  m_expandArray = GetExpandArray()->GetValue();

}
//----------------------------------------

void CExportDlg::OnDateAsPeriod( wxCommandEvent &event )
{
  m_dateAsPeriod = GetDateAsPeriod()->GetValue();

}

//----------------------------------------
void CExportDlg::OnExecAgain( wxCommandEvent &event )
{

  m_executeAgain = GetExecagain()->GetValue();

}
//----------------------------------------
void CExportDlg::OnFormat( wxCommandEvent &event )
{
  int32_t sel = event.GetInt();
  Format(sel);

}
//----------------------------------------
void CExportDlg::Format( int32_t format )
{
  m_format = format;
  wxString extension = CExportDlg::m_defaultExtensionAscii;

  if (format == AS_GEOTIFF && !m_isGeoImage)
  {
      wxMessageBox("Export to GeoTiff is only allowed for Z=F(lat,lon) operations", "Warning",
                   wxOK | wxCENTRE | wxICON_EXCLAMATION);
      GetExportformat()->SetSelection(AS_ASCII);
      return;
  }

  bool enableExecAgain = AsNetCdf() || (AsAscii() && !m_noDataComputation);

  GetExecagain()->Enable(enableExecAgain);
  GetDateAsPeriod()->Enable(AsAscii());
  GetExpandArray()->Enable(AsAscii());
  GetExportNoDataComputation()->Enable(AsAscii());
  GetExportAsciiNumberPrecision()->Enable(AsAscii());

  GetCreateKMLFile()->Enable(AsGeoTiff());
  GetColorTable()->Enable(AsGeoTiff());
  GetColorTableLabel()->Enable(AsGeoTiff());
  GetColorRangeMin()->Enable(AsGeoTiff());
  GetColorRangeMax()->Enable(AsGeoTiff());
  GetCalcColorRange()->Enable(AsGeoTiff());

  if (AsNetCdf())
  {
    extension = CExportDlg::m_defaultExtensionNetCdf;
  }
  else if (AsGeoTiff())
  {
    extension = CExportDlg::m_defaultExtensionGeoTiff;
  }

  m_currentName.Assign(GetExportoutputfile()->GetValue());
  m_currentName.SetExt(extension);
  m_currentName.Normalize();
  GetExportoutputfile()->SetValue(m_currentName.GetFullPath());

}

//----------------------------------------
void CExportDlg::OnBrowse( wxCommandEvent &event )
{
  m_currentName.Assign(GetExportoutputfile()->GetValue());
  m_currentName.Normalize();

  int32_t style = wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR;

  wxFileDialog fileDlg(this, "Choose a file...", m_currentName.GetPath(), m_currentName.GetFullName(), "*.*", style);

  if (fileDlg.ShowModal() != wxID_OK)
  {
    return;
  }

  m_currentName.Assign( fileDlg.GetPath());

  if (m_currentName.GetFullPath().IsEmpty())
  {
    m_currentName = m_initialName;
    m_currentName.Normalize();
  }

  GetExportoutputfile()->SetValue(m_currentName.GetFullPath());
}
//----------------------------------------

void CExportDlg::OnCalcColorRange( wxCommandEvent &event )
{
    CalculateColorValueRange();
}
//----------------------------------------

void CExportDlg::OnOk(wxCommandEvent &event)
{
  m_delayExecution = false;

  m_currentName.Assign(GetExportoutputfile()->GetValue());
  m_currentName.Normalize();

  bool bOk = true;

  if (m_currentName.GetFullPath().IsEmpty())
  {
    m_currentName = m_initialName;
    m_currentName.Normalize();
    ::wxBell();
    bOk = false;
  }

  if (m_currentName.GetFullName().IsEmpty())
  {
    m_currentName = m_initialName;
    m_currentName.Normalize();
    ::wxBell();
    bOk = false;
  }

  if (!bOk)
  {
    GetExportoutputfile()->SetValue(m_currentName.GetFullPath());
    return;
  }

  GetExportAsciiNumberPrecision()->GetValue(m_asciiNumberPrecision, m_defaultDecimalPrecision);

  m_createKML = GetCreateKMLFile()->GetValue();
  m_colorTable = GetColorTable()->GetValue();
  GetColorRangeMin()->GetValue(m_colorRangeMin);
  GetColorRangeMax()->GetValue(m_colorRangeMax);

  event.Skip();
}
//----------------------------------------

void CExportDlg::OnCancel(wxCommandEvent &event)
{
    event.Skip();
}
//----------------------------------------




