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
    #pragma implementation "ResolutionDlg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "new-gui/Common/tools/Exception.h"
#include "Expression.h"
using namespace brathl;

#include "Validators.h"

#include "ResolutionDlg.h"

// WDR: class implementations
//----------------------------------------------------------------------------
// CResolutionDlg
//----------------------------------------------------------------------------
IMPLEMENT_CLASS(CResolutionDlg,wxDialog)

// WDR: event table for CResolutionDlg

BEGIN_EVENT_TABLE(CResolutionDlg,wxDialog)
    EVT_BUTTON( wxID_OK, CResolutionDlg::OnOk )
    EVT_BUTTON( wxID_CANCEL, CResolutionDlg::OnCancel )
    EVT_BUTTON( ID_FORMULAOPT_HELP_X_STEP, CResolutionDlg::OnHelpXStep )
    EVT_BUTTON( ID_FORMULAOPT_HELP_X_MIN_MAX, CResolutionDlg::OnHelpXMinMax )
    EVT_BUTTON( ID_FORMULAOPT_HELP_Y_STEP, CResolutionDlg::OnHelpYStep )
    EVT_BUTTON( ID_FORMULAOPT_HELP_Y_MIN_MAX, CResolutionDlg::OnHelpYMinMax )
    EVT_BUTTON( ID_FORMULAOPT_XGETMINMAX, CResolutionDlg::OnGetXMinMax )
    EVT_BUTTON( ID_FORMULAOPT_YGETMINMAX, CResolutionDlg::OnGetYMinMax )
END_EVENT_TABLE()

CResolutionDlg::CResolutionDlg( wxWindow *parent, wxWindowID id, const wxString &title,
                                COperation* operation,
                                CFormula* formula,
                                const CStringMap& mapFormulaString,
                                const wxPoint &position, const wxSize& size, long style )
              : m_mapFormulaString(mapFormulaString),
                wxDialog( parent, id, title, position, size, style )
{

  m_formula = formula;
  m_operation = operation;
  
  //----------------------------
  ResolutionDlg( this, TRUE );
  //----------------------------

  Init();

}


//----------------------------------------
CResolutionDlg::~CResolutionDlg()
{

}
//----------------------------------------
void CResolutionDlg::Init()
{
	if ( m_operation != NULL )
	{
		m_x = m_operation->GetFormula( CMapTypeField::eTypeOpAsX );
		m_y = m_operation->GetFormula( CMapTypeField::eTypeOpAsY );
	}

	if ( m_x == NULL )
	{
		return;
	}
	if ( m_y == NULL )
	{
		return;
	}

	m_xFormulaTmp = *m_x;
	m_yFormulaTmp = *m_y;

	if ( m_formula != NULL )
	{
		glb_FormulaOptFilterSizer->Show( ( m_formula != m_x ) && ( m_formula != m_y ) );
	}
	else
	{
		glb_FormulaOptFilterSizer->Show( false );
	}

	setDefaultValue( m_xMinValueDefault );
	setDefaultValue( m_xMaxValueDefault );

	setDefaultValue( m_yMinValueDefault );
	setDefaultValue( m_yMaxValueDefault );

	m_xIntervalDefault = 0;
	m_yIntervalDefault = 0;



	GetFormulaoptXmin()->SetToolTip( wxString::Format( "Minimun value for '%s' field.", m_x->GetName().c_str() ) );
	GetFormulaoptXmax()->SetToolTip( wxString::Format( "Maximum value for '%s' field.",	m_x->GetName().c_str() ) );

	GetFormulaoptYmin()->SetToolTip( wxString::Format( "Minimun value for '%s' field.",	m_y->GetName().c_str() ) );
	GetFormulaoptYmax()->SetToolTip( wxString::Format( "Maximum value for '%s' field.",	m_y->GetName().c_str() ) );

	GetFormulaoptXstep()->SetToolTip( wxString::Format( "Step between min. an max value for '%s' field.\n"
		"'Step' is correlate with 'Number of intervals',",
		m_x->GetName().c_str() ) );

	GetFormulaoptYstep()->SetToolTip( wxString::Format( "Step between min. an max value for '%s' field.\n"
		"'Step' is correlate with 'Number of intervals',",
		m_y->GetName().c_str() ) );

	GetFormulaoptXinterval()->SetToolTip( wxString::Format( "Number of intervals between min. an max value for '%s' field.\n"
		"'Number of intervals' is correlate with 'Step',",
		m_x->GetName().c_str() ) );

	GetFormulaoptYinterval()->SetToolTip( wxString::Format( "Number of intervals between min. an max value for '%s' field.\n"
		"'Number of intervals' is correlate with 'Step',",
		m_y->GetName().c_str() ) );

	GetFormulaoptXloesscut()->SetToolTip( wxString::Format( "Distance (in dots) for '%s' field where LOESS filter reaches 0 along Y axis.\n"
		"Must be an odd integer. If 1 or 0, Distance computation is disabled.",
		m_x->GetName().c_str() ) );
	GetFormulaoptYloesscut()->SetToolTip( wxString::Format( "Distance (in dots) for '%s' field where LOESS filter reaches 0 along Y axis.\n"
		"Must be an odd integer. If 1 or 0, Distance computation is disabled.",
		m_y->GetName().c_str() ) );
	if ( m_x->IsTimeDataType() )
	{
		GetFormulaoptXgetminmax()->SetLabel( "Get min/max date coverage" );
		GetFormulaoptXgetminmax()->SetToolTip( "Gets the minimun and maximum datetime coverage of the dataset" );
	}
	else if ( m_x->IsLonDataType() )
	{
		GetFormulaoptXgetminmax()->SetLabel( "Get min/max longitude coverage" );
		GetFormulaoptXgetminmax()->SetToolTip( "Gets the minimun and maximum longitude coverage of the the dataset" );
	}
	else if ( m_x->IsLatDataType() )
	{
		GetFormulaoptXgetminmax()->SetLabel( "Get min/max latitude coverage" );
		GetFormulaoptXgetminmax()->SetToolTip( "Gets the minimun and maximum latitude coverage of the dataset" );
	}
	else
	{
		GetFormulaoptXgetminmax()->SetLabel( "Get min/max expression values" );
		GetFormulaoptXgetminmax()->SetToolTip( wxString::Format( "Gets the minimun and maximum values of the '%s' expression from the dataset",
			m_x->GetName().c_str() ) );
	}

	if ( m_y->IsTimeDataType() )
	{
		GetFormulaoptYgetminmax()->SetLabel( "Get min/max date coverage" );
		GetFormulaoptYgetminmax()->SetToolTip( "Gets the minimun and maximum datetime coverage of the dataset" );
	}
	else if ( m_y->IsLonDataType() )
	{
		GetFormulaoptYgetminmax()->SetLabel( "Get min/max longitude coverage" );
		GetFormulaoptYgetminmax()->SetToolTip( "Gets the minimun and maximum longitude coverage of the dataset" );
	}
	else if ( m_y->IsLatDataType() )
	{
		GetFormulaoptYgetminmax()->SetLabel( "Get min/max latitude coverage" );
		GetFormulaoptYgetminmax()->SetToolTip( "Gets the minimun and maximum latitude coverage of the dataset" );
	}
	else
	{
		GetFormulaoptYgetminmax()->SetLabel( "Get min/max expression values" );
		GetFormulaoptYgetminmax()->SetToolTip( wxString::Format( "Gets the minimun and maximum values of the '%s' expression from the dataset",
			m_y->GetName().c_str() ) );
	}


	GetFormulaoptXinterval()->SetBackgroundColour( *wxLIGHT_GREY );
	GetFormulaoptYinterval()->SetBackgroundColour( *wxLIGHT_GREY );


	if ( m_formula != NULL )
	{
		( static_cast<wxStaticBoxSizer*>( glb_FormulaOptFilterSizer ) )->GetStaticBox()->SetLabel( m_formula->GetName() );
	}
	if ( m_x != NULL )
	{
		wxString label = wxString::Format( XRESOLUTION_LABELFORMAT, m_x->GetName().c_str(), m_x->GetUnitAsText().c_str() );
		( static_cast<wxStaticBoxSizer*>( glb_FormulaOptXIntervalSizer ) )->GetStaticBox()->SetLabel( label );
	}
	if ( m_y != NULL )
	{
		wxString label = wxString::Format( YRESOLUTION_LABELFORMAT, m_y->GetName().c_str(), m_y->GetUnitAsText().c_str() );
		( static_cast<wxStaticBoxSizer*>( glb_FormulaOptYIntervalSizer ) )->GetStaticBox()->SetLabel( label );
	}



	CDateValidator dateValidator;
	CFloatValidator floatValidator;

	if ( m_x->IsTimeDataType() )
	{
		GetFormulaoptXmin()->SetTextValidator( dateValidator );
		GetFormulaoptXmax()->SetTextValidator( dateValidator );

		GetFormulaoptXmin()->SetFormat( "" );
		GetFormulaoptXmax()->SetFormat( "" );
	}
	else
	{
		GetFormulaoptXmin()->SetTextValidator( floatValidator );
		GetFormulaoptXmax()->SetTextValidator( floatValidator );

		GetFormulaoptXmin()->SetFormat( m_x->GetFormatString() );
		GetFormulaoptXmax()->SetFormat( m_x->GetFormatString() );
	}
	if ( m_y->IsTimeDataType() )
	{
		GetFormulaoptYmin()->SetTextValidator( dateValidator );
		GetFormulaoptYmax()->SetTextValidator( dateValidator );

		GetFormulaoptYmin()->SetFormat( "" );
		GetFormulaoptYmax()->SetFormat( "" );
	}
	else
	{
		GetFormulaoptYmin()->SetTextValidator( floatValidator );
		GetFormulaoptYmax()->SetTextValidator( floatValidator );

		GetFormulaoptYmin()->SetFormat( m_y->GetFormatString() );
		GetFormulaoptYmax()->SetFormat( m_y->GetFormatString() );
	}


	if ( m_formula != NULL )
	{
		GetFormulaoptFilter()->SetSelection( m_formula->GetFilter() );
	}

	if ( !isDefaultValue( m_x->GetLoessCutOff() ) )
	{
		GetFormulaoptXloesscut()->SetValue( m_x->GetLoessCutOff() );
	}

	if ( !isDefaultValue( m_y->GetLoessCutOff() ) )
	{
		GetFormulaoptYloesscut()->SetValue( m_y->GetLoessCutOff() );
	}

	if ( m_x->IsLatLonDataType() )
	{
		if ( m_x->IsLonDataType() )
		{
			m_x->ConvertToFormulaUnit( -180, m_xMinValueDefault );
			m_x->ConvertToFormulaUnit( 180, m_xMaxValueDefault );
		}
		else
		{
			m_x->ConvertToFormulaUnit( -90, m_xMinValueDefault );
			m_x->ConvertToFormulaUnit( 90, m_xMaxValueDefault );
		}

		InitCtrlMinMax( *m_x, GetFormulaoptXmin(), GetFormulaoptXmax(), m_xMinValueDefault, m_xMaxValueDefault );
	}
	else if ( m_x->IsTimeDataType() )
	{
		InitCtrlMinMaxAsDate( *m_x, GetFormulaoptXmin(), GetFormulaoptXmax() );
	}
	else
	{
		InitCtrlMinMax( *m_x, GetFormulaoptXmin(), GetFormulaoptXmax(), m_xMinValueDefault, m_xMaxValueDefault );
	}

	GetFormulaoptXstep()->SetValue( m_x->GetStep(), m_x->GetDefaultStep() );

	if ( m_y->IsLatLonDataType() )
	{
		if ( m_y->IsLonDataType() )
		{
			m_y->ConvertToFormulaUnit( -180, m_yMinValueDefault );
			m_y->ConvertToFormulaUnit( 180, m_yMaxValueDefault );
		}
		else
		{
			m_y->ConvertToFormulaUnit( -90, m_yMinValueDefault );
			m_y->ConvertToFormulaUnit( 90, m_yMaxValueDefault );
		}

		InitCtrlMinMax( *m_y, GetFormulaoptYmin(), GetFormulaoptYmax(), m_yMinValueDefault, m_yMaxValueDefault );
	}
	else if ( m_y->IsTimeDataType() )
	{
		InitCtrlMinMaxAsDate( *m_y, GetFormulaoptYmin(), GetFormulaoptYmax() );
	}
	else
	{
		InitCtrlMinMax( *m_y, GetFormulaoptYmin(), GetFormulaoptYmax(), m_yMinValueDefault, m_yMaxValueDefault );
	}

	GetFormulaoptYstep()->SetValue( m_y->GetStep(), m_y->GetDefaultStep() );


	std::string errorMsg;
	if ( !VerifyXMinMax() || isZero( m_x->GetStepAsDouble( errorMsg ) ) )
	{
		if ( m_x->IsTimeDataType() )
		{
			InitCtrlMinMaxAsDate( m_xFormulaTmp, GetFormulaoptXmin(), GetFormulaoptXmax() );
		}
		else if ( m_x->IsLatLonDataType() )
		{
			InitCtrlMinMax( m_xFormulaTmp, GetFormulaoptXmin(), GetFormulaoptXmax(), m_xMinValueDefault, m_xMaxValueDefault );
		}
		else
		{
			InitCtrlMinMax( m_xFormulaTmp, GetFormulaoptXmin(), GetFormulaoptXmax(), m_xMinValueDefault, m_xMaxValueDefault );
		}

		GetFormulaoptXstep()->SetValue( m_xFormulaTmp.GetDefaultStep() );
	}
	if ( !errorMsg.empty() ){
		wxMessageBox( errorMsg, "Warning", wxOK | wxCENTRE | wxICON_INFORMATION );
		errorMsg = "";
	}

	if ( !VerifyYMinMax() || isZero( m_y->GetStepAsDouble( errorMsg ) ) )
	{
		if ( m_y->IsTimeDataType() )
		{
			InitCtrlMinMaxAsDate( m_yFormulaTmp, GetFormulaoptYmin(), GetFormulaoptYmax() );
		}
		else if ( m_y->IsLatLonDataType() )
		{
			InitCtrlMinMax( m_yFormulaTmp, GetFormulaoptYmin(), GetFormulaoptYmax(), m_yMinValueDefault, m_yMaxValueDefault );
		}
		else
		{
			InitCtrlMinMax( m_yFormulaTmp, GetFormulaoptYmin(), GetFormulaoptYmax(), m_yMinValueDefault, m_yMaxValueDefault );
		}

		GetFormulaoptYstep()->SetValue( m_xFormulaTmp.GetDefaultStep() );
	}
	if ( !errorMsg.empty() ){
		wxMessageBox( errorMsg, "Warning", wxOK | wxCENTRE | wxICON_INFORMATION );
		errorMsg = "";
	}

	ComputeInterval();

	CLabeledTextCtrl::EvtValueChanged( *this,
		ID_FORMULAOPT_XMIN,
		(CValueChangedEventFunction)&CResolutionDlg::OnXMin );
	CLabeledTextCtrl::EvtValueChanged( *this,
		ID_FORMULAOPT_XMAX,
		(CValueChangedEventFunction)&CResolutionDlg::OnXMax );
	CLabeledTextCtrl::EvtValueChanged( *this,
		ID_FORMULAOPT_XSTEP,
		(CValueChangedEventFunction)&CResolutionDlg::OnXStep );
	CLabeledTextCtrl::EvtValueChanged( *this,
		ID_FORMULAOPT_XINTERVAL,
		(CValueChangedEventFunction)&CResolutionDlg::OnXInterval );
	CLabeledTextCtrl::EvtValueChanged( *this,
		ID_FORMULAOPT_XLOESSCUT,
		(CValueChangedEventFunction)&CResolutionDlg::OnXLoessCut );


	CLabeledTextCtrl::EvtValueChanged( *this,
		ID_FORMULAOPT_YMIN,
		(CValueChangedEventFunction)&CResolutionDlg::OnYMin );
	CLabeledTextCtrl::EvtValueChanged( *this,
		ID_FORMULAOPT_YMAX,
		(CValueChangedEventFunction)&CResolutionDlg::OnYMax );
	CLabeledTextCtrl::EvtValueChanged( *this,
		ID_FORMULAOPT_YSTEP,
		(CValueChangedEventFunction)&CResolutionDlg::OnYStep );
	CLabeledTextCtrl::EvtValueChanged( *this,
		ID_FORMULAOPT_YINTERVAL,
		(CValueChangedEventFunction)&CResolutionDlg::OnYInterval );
	CLabeledTextCtrl::EvtValueChanged( *this,
		ID_FORMULAOPT_YLOESSCUT,
		(CValueChangedEventFunction)&CResolutionDlg::OnYLoessCut );

	glb_FormulaResolutionSizer->Fit( this );

	EnableCtrl();
}
//----------------------------------------
void CResolutionDlg::EnableCtrl()
{


}

//----------------------------------------
bool CResolutionDlg::ValidateData()
{
  if (VerifyMinMax() == false)
  {
    return false;
  }


  ComputeInterval();

  return true;

}
//----------------------------------------
bool CResolutionDlg::IsZero (double X)
{
  const double compareEpsilon = 1.0E-5;
  return fabs(X) < compareEpsilon;
}

//----------------------------------------
bool CResolutionDlg::ComputeInterval()
{
  bool bOk = ComputeXInterval();
  bOk &= ComputeYInterval();

  return bOk;
}

//----------------------------------------
bool CResolutionDlg::ComputeInterval( CFormula& formula, CLabeledTextCtrl* ctrlInterval, CLabeledTextCtrl* ctrlStep, wxStaticText* ctrlWarning )
{
	std::string errorMsg;
	bool bOk = formula.ComputeInterval( errorMsg );
	if ( !errorMsg.empty() )
		wxMessageBox( errorMsg, "Warning", wxOK | wxCENTRE | wxICON_INFORMATION );

	wxString label;
	if ( bOk == false )
	{
		label = "Interval was round up or down\nto the nearest integer value.";
	}
	ctrlWarning->SetLabel( label );

	ctrlWarning->Layout();
	this->Layout();

	int32_t interval = formula.GetInterval();
	wxString step = formula.GetStep();

	if ( !isDefaultValue( interval ) )
	{
		ctrlInterval->SetValue( interval );
	}
	ctrlStep->SetValue( step );

	return bOk;
}
//----------------------------------------
bool CResolutionDlg::ComputeXInterval()
{
  return ComputeInterval(m_xFormulaTmp, GetFormulaoptXinterval(), GetFormulaoptXstep(), GetFormulaoptIntervalXWarning());
}

//----------------------------------------
bool CResolutionDlg::ComputeYInterval()
{
  return ComputeInterval(m_yFormulaTmp, GetFormulaoptYinterval(), GetFormulaoptYstep(), GetFormulaoptIntervalYWarning());
}
//----------------------------------------
bool CResolutionDlg::VerifyMinMax()
{
  return (VerifyXMinMax() && VerifyYMinMax());
}
//----------------------------------------
bool CResolutionDlg::VerifyXMinMax()
{
  bool bOk = true;

  if (m_x->IsTimeDataType())
  {
    bOk = VerifyMinMaxAsDate(m_xFormulaTmp, GetFormulaoptXmin(), GetFormulaoptXmax());
  }
  else if (m_x->IsLatLonDataType())
  {
    bOk = VerifyMinMax(m_xFormulaTmp, GetFormulaoptXmin(), GetFormulaoptXmax(), m_xMinValueDefault, m_xMaxValueDefault);
  }
  else
  {
    bOk = VerifyMinMax(m_xFormulaTmp, GetFormulaoptXmin(), GetFormulaoptXmax(), m_xMinValueDefault, m_xMaxValueDefault);
  }

  return bOk;
}
//----------------------------------------
bool CResolutionDlg::VerifyYMinMax()
{
  bool bOk = true;

  if (m_y->IsTimeDataType())
  {
    bOk = VerifyMinMaxAsDate(m_yFormulaTmp, GetFormulaoptYmin(), GetFormulaoptYmax());
  }
  else if (m_y->IsLatLonDataType())
  {
    bOk = VerifyMinMax(m_yFormulaTmp, GetFormulaoptYmin(), GetFormulaoptYmax(), m_yMinValueDefault, m_yMaxValueDefault);
  }
  else
  {
    bOk = VerifyMinMax(m_yFormulaTmp, GetFormulaoptYmin(), GetFormulaoptYmax(), m_yMinValueDefault, m_yMaxValueDefault);
  }

  return bOk;
}

//----------------------------------------
bool CResolutionDlg::VerifyMinMax(CFormula& formula, CLabeledTextCtrl* ctrlMin, CLabeledTextCtrl* ctrlMax, double minDefault, double maxDefault)
{
  if (m_operation == NULL)
  {
    return true;
  }

  GetCtrlMinMax(formula, ctrlMin, ctrlMax, minDefault, maxDefault);
  double min = formula.GetMinValue();
  double max = formula.GetMaxValue();

  if (isDefaultValue(min) && isDefaultValue(max))
  {
    return true;
  }

  std::string errorMsg = "resolution for '" + formula.GetName() + "': ";
  if (!formula.CtrlMinMaxValue(errorMsg))
  {
     wxMessageBox(errorMsg,
           "Warning",
            wxOK | wxCENTRE | wxICON_EXCLAMATION);
    return false;
  }

  return true;

}
/*
//----------------------------------------
bool CResolutionDlg::VerifyXMinMaxAsLatLon()
{
  if ( GetFormulaoptXmin()->GetStringValue().IsEmpty() && GetFormulaoptXmax()->GetStringValue().IsEmpty() )
  {
    return true;
  }

  GetFormulaoptXmin()->GetValue(m_xMinValue, m_xMinValueDefault, m_xMinValueDefault, m_xMaxValueDefault);
  GetFormulaoptXmax()->GetValue(m_xMaxValue, m_xMaxValueDefault, m_xMinValueDefault, m_xMaxValueDefault);

  if (m_xMinValue >= m_xMaxValue)
  {
     wxMessageBox(wxString::Format("'%s' minimun value must be strictly less than '%s' maximum value.",
                                  m_x->GetName().c_str(),
                                  m_x->GetName().c_str()),
           "Warning",
            wxOK | wxCENTRE | wxICON_EXCLAMATION);
    return false;
  }

  return true;

}


//----------------------------------------
bool CResolutionDlg::VerifyYMinMaxAsLatLon()
{
  if ( GetFormulaoptYmin()->GetStringValue().IsEmpty() && GetFormulaoptYmax()->GetStringValue().IsEmpty() )
  {
    return true;
  }

  GetFormulaoptYmin()->GetValue(m_yMinValue, m_yMinValueDefault, m_yMinValueDefault, m_yMaxValueDefault);
  GetFormulaoptYmax()->GetValue(m_yMaxValue, m_yMaxValueDefault, m_yMinValueDefault, m_yMaxValueDefault);

  if (m_yMinValue >= m_yMaxValue)
  {
     wxMessageBox(wxString::Format("'%s' minimun value must be strictly less than '%s' maximum value.",
                                  m_y->GetName().c_str(),
                                  m_y->GetName().c_str()),
           "Warning",
            wxOK | wxCENTRE | wxICON_EXCLAMATION);
    return false;
  }

  return true;

}

//----------------------------------------
bool CResolutionDlg::VerifyXMinMaxAsDouble()
{
  if ( GetFormulaoptXmin()->GetStringValue().IsEmpty() && GetFormulaoptXmax()->GetStringValue().IsEmpty() )
  {
    return true;
  }

  GetFormulaoptXmin()->GetValue(m_xMinValue, 0.0, m_xMinValueDefault, m_xMaxValueDefault);
  GetFormulaoptXmax()->GetValue(m_xMaxValue, 1.0, m_xMinValueDefault, m_xMaxValueDefault);

  if (m_xMinValue >= m_xMaxValue)
  {
     wxMessageBox(wxString::Format("'%s' minimun value must be strictly less than '%s' maximum value.",
                                  m_x->GetName().c_str(),
                                  m_x->GetName().c_str()),
           "Warning",
            wxOK | wxCENTRE | wxICON_EXCLAMATION);
    return false;
  }

  return true;

}
//----------------------------------------
bool CResolutionDlg::VerifyYMinMaxAsDouble()
{
  if ( GetFormulaoptYmin()->GetStringValue().IsEmpty() && GetFormulaoptYmax()->GetStringValue().IsEmpty() )
  {
    return true;
  }

  GetFormulaoptYmin()->GetValue(m_yMinValue, 0.0, m_yMinValueDefault, m_yMaxValueDefault);
  GetFormulaoptYmax()->GetValue(m_yMaxValue, 1.0, m_yMinValueDefault, m_yMaxValueDefault);

  if (m_yMinValue >= m_yMaxValue)
  {
     wxMessageBox(wxString::Format("'%s' minimun value must be strictly less than '%s' maximum value.",
                                  m_y->GetName().c_str(),
                                  m_y->GetName().c_str()),
           "Warning",
            wxOK | wxCENTRE | wxICON_EXCLAMATION);
    return false;
  }

  return true;

}
*/
//----------------------------------------
void CResolutionDlg::InitCtrlMinMax(CFormula& formula, CLabeledTextCtrl* ctrlMin, CLabeledTextCtrl* ctrlMax, double minDefault, double maxDefault)
{
  if (formula.IsLonDataType())
  {
    ctrlMin->SetValue(formula.GetMinValue(), minDefault);
    ctrlMax->SetValue(formula.GetMaxValue(), maxDefault);
  }
  else
  {
    ctrlMin->SetValue(formula.GetMinValue(), minDefault, minDefault, maxDefault);
    ctrlMax->SetValue(formula.GetMaxValue(), maxDefault, minDefault, maxDefault);
  }


}
//----------------------------------------
void CResolutionDlg::InitCtrlMinMaxAsDate(CFormula& formula, CLabeledTextCtrl* ctrlMin, CLabeledTextCtrl* ctrlMax)
{
  ctrlMin->SetValueAsDate(formula.GetMinValue(), "");
  ctrlMax->SetValueAsDate(formula.GetMaxValue(), "");

}

//----------------------------------------
void CResolutionDlg::GetCtrlMinMax(CFormula& formula, CLabeledTextCtrl* ctrlMin, CLabeledTextCtrl* ctrlMax, double minDefault, double maxDefault)
{
  if (m_operation == NULL)
  {
    return;
  }

  double min = 0.0;
  double max = 0.0;

  if (formula.IsLonDataType())
  {
    ctrlMin->GetValue(min, minDefault);
    ctrlMax->GetValue(max, maxDefault);
  }
  else
  {
    ctrlMin->GetValue(min, minDefault, minDefault, maxDefault);
    ctrlMax->GetValue(max, maxDefault, minDefault, maxDefault);
  }

  ctrlMin->SetValue(min);
  ctrlMax->SetValue(max);

  formula.SetMinValue(min);
  formula.SetMaxValue(max);


}

//----------------------------------------
void CResolutionDlg::GetCtrlMinMaxAsDate(CFormula& formula, CLabeledTextCtrl* ctrlMin, CLabeledTextCtrl* ctrlMax)
{
  if (m_operation == NULL)
  {
    return;
  }

//  if ( ctrlMin()->GetStringValue().IsEmpty() && ctrlMax()->GetStringValue().IsEmpty() )
//  {
//    return false;
//  }

  CProduct* product = m_operation->GetProduct();

  CDate dateMin;
  CDate dateMax;
  double min = 0.0;
  double max = 0.0;

  dateMin.InitDateZero();

  if (product != NULL)
  {
    dateMin.SetDate(0.0, product->GetRefDate());
  }

  dateMax.SetDateNow();

  ctrlMin->GetValueAsDate(min, dateMin.Value(), dateMin.Value(), dateMax.Value());
  ctrlMax->GetValueAsDate(max, dateMax.Value(), dateMin.Value(), dateMax.Value());

  // to set right date std::string format
  ctrlMin->SetValueAsDate(min, "");
  ctrlMax->SetValueAsDate(max, "");

  formula.SetMinValue(min);
  formula.SetMaxValue(max);


}

//----------------------------------------
bool CResolutionDlg::VerifyMinMaxAsDate(CFormula& formula, CLabeledTextCtrl* ctrlMin, CLabeledTextCtrl* ctrlMax)
{

  if (m_operation == NULL)
  {
    return true;
  }

  GetCtrlMinMaxAsDate(formula, ctrlMin, ctrlMax);
  double min = formula.GetMinValue();
  double max = formula.GetMaxValue();

  if (isDefaultValue(min) && isDefaultValue(max))
  {
    return true;
  }

  if (min >= max)
  {
     wxMessageBox(wxString::Format("'%s' minimun value must be strictly less than '%s' maximum value.",
                                  formula.GetName().c_str(),
                                  formula.GetName().c_str()),
           "Warning",
            wxOK | wxCENTRE | wxICON_EXCLAMATION);
    return false;
  }

  return true;

}
// WDR: handler implementations for CResolutionDlg
//----------------------------------------
void CResolutionDlg::OnGetYMinMax( wxCommandEvent &event )
{
  GetMinMax(&m_yFormulaTmp);

}

//----------------------------------------
void CResolutionDlg::OnGetXMinMax( wxCommandEvent &event )
{
  GetMinMax(&m_xFormulaTmp);
}

//----------------------------------------
void CResolutionDlg::OnHelpXMinMax( wxCommandEvent &event )
{
  HelpMinMax(m_x);

}
//----------------------------------------
void CResolutionDlg::OnHelpYMinMax( wxCommandEvent &event )
{
  HelpMinMax(m_y);

}

//----------------------------------------
void CResolutionDlg::GetMinMax(CFormula* formula)
{
  if (formula == NULL)
  {
    return;
  }

  CProduct* product = m_operation->GetProduct();
  if (product == NULL)
  {
    return;
  }

  this->SetCursor(*wxHOURGLASS_CURSOR);

  // Don't use product of operation object to read data
  // because reading reload fiels info and the we get bad pointer
  // So, we use a CProduct temporary object
  CProduct* productTmp = NULL;

  wxString errorMsg;
  bool bOk = true;

  try
  {
    //productTmp = CProduct::Construct(product->GetProductList());
    productTmp = CProduct::Construct(*(m_operation->GetDataset()->GetProductList()));
  }
  catch (CException &e)
  {
    errorMsg = e.what();
    bOk = false;
  }
  catch (std::exception &e)
  {
    errorMsg = e.what();
    bOk = false;
  }
  catch (...)
  {
    errorMsg = " Unexpected error while getting product (runtime error).";
    bOk = false;
  }


  if (!bOk)
  {
    this->SetCursor(wxNullCursor);
    wxMessageBox(wxString::Format("Unable to get min/max values - Reason:\n%s",
                                  errorMsg.c_str()),
                "ERROR",
                wxOK | wxICON_ERROR);
    return;
  }

  bool resultGetCoverage = false;
  try
  {

    if (formula->IsTimeDataType())
    {
      CDatePeriod datePeriod;

      resultGetCoverage = productTmp->GetDateMinMax(datePeriod);

      if (formula->IsXType())
      {
        GetFormulaoptXmin()->SetValue(datePeriod.GetFrom().AsString());
        GetFormulaoptXmax()->SetValue(datePeriod.GetTo().AsString());
      }
      else if (formula->IsYType())
      {
        GetFormulaoptYmin()->SetValue(datePeriod.GetFrom().AsString());
        GetFormulaoptYmax()->SetValue(datePeriod.GetTo().AsString());
      }

    }
    //else if (formula->IsLatLonDataType() && isDefaultValue(productTmp->GetForceLatMinCriteriaValue()))
    else if (formula->IsLatLonDataType())
    {
      double latMin;
      double latMax;
      double lonMin;
      double lonMax;

      resultGetCoverage = productTmp->GetLatLonMinMax(latMin, lonMin, latMax, lonMax);

      latMin = formula->ConvertToFormulaUnit(latMin);
      latMax = formula->ConvertToFormulaUnit(latMax);
      lonMin = formula->ConvertToFormulaUnit(lonMin);
      lonMax = formula->ConvertToFormulaUnit(lonMax);

      double valueMin = 0.0;
      double valueMax = 0.0;
      if (formula->IsLonDataType())
      {
        valueMin = lonMin;
        valueMax = lonMax;

      }
      else
      {
        if (!isDefaultValue(productTmp->GetForceLatMinCriteriaValue()))
        {
          CExpression expr = std::string(formula->GetDescription(true, &m_mapFormulaString, productTmp->GetAliasesAsString()).c_str());
          productTmp->GetValueMinMax(expr, (const char *)m_operation->GetRecord().c_str(), valueMin, valueMax, *(formula->GetUnit()));
        }
        else
        {
          valueMin = latMin;
          valueMax = latMax;
        }
      }
      if (formula->IsXType())
      {
        GetFormulaoptXmin()->SetValue(valueMin);
        GetFormulaoptXmax()->SetValue(valueMax);
      }
      else if (formula->IsYType())
      {
        GetFormulaoptYmin()->SetValue(valueMin);
        GetFormulaoptYmax()->SetValue(valueMax);
      }
    }

    if (!resultGetCoverage)
    {
      double valueMin = 0.0;
      double valueMax = 0.0;
      CExpression expr = std::string(formula->GetDescription(true, &m_mapFormulaString, productTmp->GetAliasesAsString()).c_str());
      if (formula->IsTimeDataType())
      {
        CUnit unit;
        productTmp->GetValueMinMax(expr, (const char *)m_operation->GetRecord().c_str(), valueMin, valueMax, unit);
        
        CDatePeriod datePeriod(valueMin, valueMax);

        if (formula->IsXType())
        {
          GetFormulaoptXmin()->SetValue(datePeriod.GetFrom().AsString());
          GetFormulaoptXmax()->SetValue(datePeriod.GetTo().AsString());
        }
        else if (formula->IsYType())
        {
          GetFormulaoptYmin()->SetValue(datePeriod.GetFrom().AsString());
          GetFormulaoptYmax()->SetValue(datePeriod.GetTo().AsString());
        }
      }
      else
      {
        productTmp->GetValueMinMax(expr, (const char *)m_operation->GetRecord().c_str(), valueMin, valueMax, *(formula->GetUnit()));

        //valueMin = formula->ConvertToFormulaUnit(valueMin);
        //valueMax = formula->ConvertToFormulaUnit(valueMax);

        if (formula->IsXType())
        {
          GetFormulaoptXmin()->SetValue(valueMin);
          GetFormulaoptXmax()->SetValue(valueMax);
        }
        else if (formula->IsYType())
        {
          GetFormulaoptYmin()->SetValue(valueMin);
          GetFormulaoptYmax()->SetValue(valueMax);
        }
      }

    }

  }
  catch (CException &e)
  {
    errorMsg = e.what();
    bOk = false;
  }
  catch (std::exception &e)
  {
    errorMsg = e.what();
    bOk = false;
  }
  catch (...)
  {
    errorMsg = " Unexpected error while getting product (runtime error).";
    bOk = false;
  }

  if (productTmp != NULL)
  {
    delete productTmp;
    productTmp = NULL;
  }

  if (!bOk)
  {
    this->SetCursor(wxNullCursor);
    wxMessageBox(wxString::Format("Unable to get min/max values - Reason:\n%s",
                                  errorMsg.c_str()),
                "ERROR",
                wxOK | wxICON_ERROR);
    return;
  }

  ValidateData();
  this->SetCursor(wxNullCursor);


}
//----------------------------------------
void CResolutionDlg::HelpMinMax(CFormula* formula )
{
  wxString msg;
  if (formula->IsLatLonDataType())
  {
    msg = "Min and Max have to be expressed in degrees (decimal point value).\n"
          "Minimum and maximun latitude are -90.0 and 90.0 respectively.\n"
          "Minimum and maximun longitude are -180.0 and 180.0 respectively.";
  }
  else if (formula->IsTimeDataType())
  {
    msg = "Min and Max have to correspond to:\n"
          "\n"
          "- either a date std::string: allowed formats are 'YYYY/MM/DD HH:MN:SS' or 'YYYY-MM-DD HH:MN:SS'. "
          " Hours, minutes and seconds are optional:\n"
          " For instance:  1998-03-12 or 1998-03-12 22 or 1998-03-12 22:16 are valid dates\n"
          "\n"
          "- or a positive decimal julian day from 1950-01-01 00:00:00 (ie 17602.927778 --> 1998-03-12 22:16:00)\n";
  }
  else
  {
    msg = "Min and Max have to be expressed in number of the field's unit (or formula's unit).";
  }


  wxMessageBox(msg,
              "Information",
               wxOK | wxCENTRE | wxICON_INFORMATION);

}


//----------------------------------------
void CResolutionDlg::OnHelpXStep( wxCommandEvent &event )
{
  HelpStep(m_x);
}
//----------------------------------------
void CResolutionDlg::OnHelpYStep( wxCommandEvent &event )
{
  HelpStep(m_y);
}
//----------------------------------------
void CResolutionDlg::HelpStep(CFormula* formula)
{
  wxString msg;
  if (formula->IsLatLonDataType())
  {
    msg = "Step is expressed in degrees or a fraction of a degree.\n"
                 "For instance, set a step to 1/5 or 0.2 to stand for a fifth of a dregree.";
  }
  else if (formula->IsTimeDataType())
  {
    msg = "Step is expressed in number of days or a fraction of a day.\n"
                 "For instance, set a step to 1/24 to stand for an hour.";
  }
  else
  {
    msg = "Step is expressed in number of the field's unit (or formula's unit) or a fraction of it.\n"
                 "For instance, set a step to 1/10 or 0.1 to stand for a tenth of the unit.";
  }


  wxMessageBox(msg,
              "Information",
               wxOK | wxCENTRE | wxICON_INFORMATION);
}
//----------------------------------------
void CResolutionDlg::OnXMin(CValueChangedEvent& event)
{
  if (VerifyXMinMax() == false)
  {
    /*
    if (wxWindow::FindFocus() != GetFormulaoptMax())
    {
      GetFormulaoptMin()->SetFocus();
    }
    */
  }

  ComputeXInterval();
}
//----------------------------------------
void CResolutionDlg::OnXMax(CValueChangedEvent& event)
{
  if (VerifyXMinMax() == false)
  {
    /*
    if (wxWindow::FindFocus() != GetFormulaoptMin())
    {
      GetFormulaoptMax()->SetFocus();
    }
    */
  }

  ComputeXInterval();
}
//----------------------------------------
void CResolutionDlg::OnYMin(CValueChangedEvent& event)
{
  if (VerifyYMinMax() == false)
  {
    /*
    if (wxWindow::FindFocus() != GetFormulaoptMax())
    {
      GetFormulaoptMin()->SetFocus();
    }
    */
  }

  ComputeYInterval();
}
//----------------------------------------
void CResolutionDlg::OnYMax(CValueChangedEvent& event)
{
  if (VerifyYMinMax() == false)
  {
    /*
    if (wxWindow::FindFocus() != GetFormulaoptMin())
    {
      GetFormulaoptMax()->SetFocus();
    }
    */
  }

  ComputeYInterval();
}
//----------------------------------------
void CResolutionDlg::OnXStep(CValueChangedEvent& event)
{
  std::string step;

  GetFormulaoptXstep()->GetValue(step, m_xFormulaTmp.GetDefaultStep());

  m_xFormulaTmp.SetStep(step);

  ComputeXInterval();
}
//----------------------------------------
void CResolutionDlg::OnYStep(CValueChangedEvent& event)
{
  std::string step;

  GetFormulaoptYstep()->GetValue(step, m_yFormulaTmp.GetDefaultStep());

  m_yFormulaTmp.SetStep(step);

  ComputeYInterval();
}
//----------------------------------------
void CResolutionDlg::OnXInterval(CValueChangedEvent& event)
{

}
//----------------------------------------
void CResolutionDlg::OnYInterval(CValueChangedEvent& event)
{

}
//----------------------------------------
void CResolutionDlg::OnXLoessCut(CValueChangedEvent& event)
{
  int32_t value;

  GetFormulaoptXloesscut()->GetValue(value, CTools::m_defaultValueINT32);

  m_xFormulaTmp.SetLoessCutOff(value);
}
//----------------------------------------
void CResolutionDlg::OnYLoessCut(CValueChangedEvent& event)
{
  int32_t value;

  GetFormulaoptYloesscut()->GetValue(value, CTools::m_defaultValueINT32);

  m_yFormulaTmp.SetLoessCutOff(value);

}

//----------------------------------------
void CResolutionDlg::OnOk(wxCommandEvent &event)
{
  bool bOk = ValidateData();

  if (!bOk)
  {
    return;
  }

  if (m_formula != NULL)
  {
    m_formula->SetFilter(GetFormulaoptFilter()->GetSelection());
  }

  if (m_x != NULL)
  {

    m_x->SetMinValue(m_xFormulaTmp.GetMinValue());
    m_x->SetMaxValue(m_xFormulaTmp.GetMaxValue());
    m_x->SetStep(m_xFormulaTmp.GetStep());
    m_x->SetInterval(m_xFormulaTmp.GetInterval());
    m_x->SetLoessCutOff(m_xFormulaTmp.GetLoessCutOff());
  }

  if (m_y != NULL)
  {

    m_y->SetMinValue(m_yFormulaTmp.GetMinValue());
    m_y->SetMaxValue(m_yFormulaTmp.GetMaxValue());
    m_y->SetStep(m_yFormulaTmp.GetStep());
    m_y->SetInterval(m_yFormulaTmp.GetInterval());
    m_y->SetLoessCutOff(m_yFormulaTmp.GetLoessCutOff());
  }


  event.Skip();
}

//----------------------------------------
void CResolutionDlg::OnCancel(wxCommandEvent &event)
{
    event.Skip();
}


// WDR: handler implementations for CResolutionDlg




