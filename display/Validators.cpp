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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "Validators.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Validators.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CDigitValidator
//----------------------------------------------------------------------------

// WDR: event table for CDigitValidator

BEGIN_EVENT_TABLE(CDigitValidator,wxValidator)
    EVT_CHAR( CDigitValidator::OnChar )
END_EVENT_TABLE()

CDigitValidator::CDigitValidator()
{
  m_digits = "0123456789";
}

CDigitValidator::~CDigitValidator()
{
}

//----------------------------------------
wxObject* CDigitValidator::Clone() const
{
    return new CDigitValidator();
}

// WDR: handler implementations for CDigitValidator

void CDigitValidator::OnChar(wxKeyEvent &event)
{
  int32_t key = event.GetKeyCode();
  
  if ((key < WXK_SPACE) || (key == WXK_DELETE) || (key > 255))
  {
    event.Skip();
    return;
  }

  for (uint32_t i = 0 ; i < m_digits.size() ; i++)
  {
    if (key == static_cast<int32_t>(m_digits[i]))
    {
      event.Skip();
      return;
    }
  }

  if (wxValidator::IsSilent() == false)
  {
    wxBell();
  }

}


//----------------------------------------------------------------------------
// CFloatValidator
//----------------------------------------------------------------------------

// WDR: event table for CFloatValidator

BEGIN_EVENT_TABLE(CFloatValidator,wxValidator)
    EVT_CHAR( CFloatValidator::OnChar )
END_EVENT_TABLE()


CFloatValidator::CFloatValidator()
{
  m_floatChars = "0123456789-+.eE";

}

//----------------------------------------
CFloatValidator::~CFloatValidator()
{
}

//----------------------------------------
wxObject* CFloatValidator::Clone() const
{
    return new CFloatValidator();
}

//----------------------------------------

// WDR: handler implementations for CFloatValidator


void CFloatValidator::OnChar(wxKeyEvent &event)
{

  int32_t key = event.GetKeyCode();
  
  if ((key < WXK_SPACE) || (key == WXK_DELETE) || (key > 255))
  {
    event.Skip();
    return;
  }

  for (uint32_t i = 0 ; i < m_floatChars.size() ; i++)
  {
    if (key == static_cast<int32_t>(m_floatChars[i]))
    {
      event.Skip();
      return;
    }
  }

  if (wxValidator::IsSilent() == false)
  {
    wxBell();
  }

}
//----------------------------------------------------------------------------
// CFloatExtendedValidator 
//----------------------------------------------------------------------------

// WDR: event table for CFloatExtendedValidator 

BEGIN_EVENT_TABLE(CFloatExtendedValidator ,wxValidator)
    EVT_CHAR( CFloatExtendedValidator ::OnChar )
END_EVENT_TABLE()


CFloatExtendedValidator ::CFloatExtendedValidator ()
{
  m_floatChars = "0123456789-+.eE/";

}

//----------------------------------------
CFloatExtendedValidator ::~CFloatExtendedValidator ()
{
}

//----------------------------------------
wxObject* CFloatExtendedValidator ::Clone() const
{
    return new CFloatExtendedValidator ();
}

//----------------------------------------

// WDR: handler implementations for CFloatExtendedValidator 


void CFloatExtendedValidator ::OnChar(wxKeyEvent &event)
{

  int32_t key = event.GetKeyCode();
  
  if ((key < WXK_SPACE) || (key == WXK_DELETE) || (key > 255))
  {
    event.Skip();
    return;
  }

  for (uint32_t i = 0 ; i < m_floatChars.size() ; i++)
  {
    if (key == static_cast<int32_t>(m_floatChars[i]))
    {
      event.Skip();
      return;
    }
  }

  if (wxValidator::IsSilent() == false)
  {
    wxBell();
  }

}

//----------------------------------------------------------------------------
// CTextFileValidator
//----------------------------------------------------------------------------

// WDR: event table for CTextFileValidator

BEGIN_EVENT_TABLE(CTextFileValidator,wxValidator)
    EVT_CHAR( CTextFileValidator::OnChar )
END_EVENT_TABLE()

CTextFileValidator::CTextFileValidator()
{
  m_digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_-";
}

CTextFileValidator::~CTextFileValidator()
{
}
//----------------------------------------
wxObject* CTextFileValidator::Clone() const
{
    return new CTextFileValidator();
}


//----------------------------------------

// WDR: handler implementations for CTextLightValidator

void CTextFileValidator::OnChar(wxKeyEvent &event)
{
  int32_t key = event.GetKeyCode();
  
  if ((key < WXK_SPACE) || (key == WXK_DELETE) || (key > 255))
  {
    event.Skip();
    return;
  }

  for (uint32_t i = 0 ; i < m_digits.size() ; i++)
  {
    if (key == static_cast<int32_t>(m_digits[i]))
    {
      event.Skip();
      return;
    }
  }

  if (wxValidator::IsSilent() == false)
  {
    wxBell();
  }

}
//----------------------------------------------------------------------------
// CTextDirValidator
//----------------------------------------------------------------------------

// WDR: event table for CTextDirValidator

BEGIN_EVENT_TABLE(CTextDirValidator,wxValidator)
    EVT_CHAR( CTextDirValidator::OnChar )
END_EVENT_TABLE()

CTextDirValidator::CTextDirValidator()
{
  m_digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_-\\/";
}

CTextDirValidator::~CTextDirValidator()
{
}
//----------------------------------------
wxObject* CTextDirValidator::Clone() const
{
    return new CTextDirValidator();
}


//----------------------------------------

// WDR: handler implementations for CTextLightValidator

void CTextDirValidator::OnChar(wxKeyEvent &event)
{
  int32_t key = event.GetKeyCode();
  
  if ((key < WXK_SPACE) || (key == WXK_DELETE) || (key > 255))
  {
    event.Skip();
    return;
  }

  for (uint32_t i = 0 ; i < m_digits.size() ; i++)
  {
    if (key == static_cast<int32_t>(m_digits[i]))
    {
      event.Skip();
      return;
    }
  }

  if (wxValidator::IsSilent() == false)
  {
    wxBell();
  }

}


//----------------------------------------------------------------------------
// CTextLightValidator
//----------------------------------------------------------------------------

// WDR: event table for CTextLightValidator

BEGIN_EVENT_TABLE(CTextLightValidator,wxValidator)
    EVT_CHAR( CTextLightValidator::OnChar )
END_EVENT_TABLE()

CTextLightValidator::CTextLightValidator()
{
  m_digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";
}

CTextLightValidator::~CTextLightValidator()
{
}

//----------------------------------------
wxObject* CTextLightValidator::Clone() const
{
    return new CTextLightValidator();
}

//----------------------------------------
bool CTextLightValidator::IsValid(const wxString& str)
{

  bool bOk = false;

  for (uint32_t j = 0 ; j < str.Length() ; j++)
  {
    bOk = false;
    for (uint32_t i = 0 ; i < m_digits.size() ; i++)
    {
      if (static_cast<int32_t>(str[j]) == static_cast<int32_t>(m_digits[i]))
      {
        bOk = true;
        break;
      }
    }
    if (!bOk)
    {
      break;
    }
  }

  return bOk;
}
//----------------------------------------

// WDR: handler implementations for CTextLightValidator

void CTextLightValidator::OnChar(wxKeyEvent &event)
{
  int32_t key = event.GetKeyCode();
  
  if ((key < WXK_SPACE) || (key == WXK_DELETE) || (key > 255))
  {
    event.Skip();
    return;
  }

  for (uint32_t i = 0 ; i < m_digits.size() ; i++)
  {
    if (key == static_cast<int32_t>(m_digits[i]))
    {
      event.Skip();
      return;
    }
  }

  if (wxValidator::IsSilent() == false)
  {
    wxBell();
  }

}


//----------------------------------------------------------------------------
// CDateValidator
//----------------------------------------------------------------------------

// WDR: event table for CTextDirValidator

BEGIN_EVENT_TABLE(CDateValidator,wxValidator)
    EVT_CHAR( CDateValidator::OnChar )
END_EVENT_TABLE()

CDateValidator::CDateValidator()
{
  m_digits = "0123456789-:/. ";
}

CDateValidator::~CDateValidator()
{
}
//----------------------------------------
wxObject* CDateValidator::Clone() const
{
    return new CDateValidator();
}


//----------------------------------------

// WDR: handler implementations for CTextLightValidator

void CDateValidator::OnChar(wxKeyEvent &event)
{
  int32_t key = event.GetKeyCode();
  
  if ((key < WXK_SPACE) || (key == WXK_DELETE) || (key > 255))
  {
    event.Skip();
    return;
  }

  for (uint32_t i = 0 ; i < m_digits.size() ; i++)
  {
    if (key == static_cast<int32_t>(m_digits[i]))
    {
      event.Skip();
      return;
    }
  }

  if (wxValidator::IsSilent() == false)
  {
    wxBell();
  }

}
