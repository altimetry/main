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

#ifndef __Validators_H__
#define __Validators_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "Validators.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "brathl.h"
#include <string>

//#include "BratDisplay_wdr.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CDigitValidator
//----------------------------------------------------------------------------

class CDigitValidator: public wxValidator
{
public:
  // constructors and destructors
  CDigitValidator();
  virtual ~CDigitValidator();
    
  // WDR: method declarations for CDigitValidator
  virtual wxObject* Clone() const;

  // Called to transfer data to the window
  virtual bool TransferToWindow() { return true; }

  // Called to transfer data from the window
  virtual bool TransferFromWindow() { return true; };

  // Called when the value in the window must be validated.
  // This function can pop up an error message.
  virtual bool Validate(wxWindow *WXUNUSED(parent)) { return true; };

  std::string GetDigits() {return m_digits;};   
    
private:
  // WDR: member variable declarations for CDigitValidator
  std::string m_digits;    
private:
  // WDR: handler declarations for CDigitValidator
  void OnChar( wxKeyEvent &event );
  DECLARE_NO_COPY_CLASS(CDigitValidator)

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// CFloatValidator
//----------------------------------------------------------------------------

class CFloatValidator: public wxValidator
{
public:
    // constructors and destructors
  CFloatValidator();
  virtual ~CFloatValidator();
    
  // WDR: method declarations for CFloatValidator
  virtual wxObject* Clone() const;

  // Called to transfer data to the window
  virtual bool TransferToWindow() { return true; }

  // Called to transfer data from the window
  virtual bool TransferFromWindow() { return true; };

  // Called when the value in the window must be validated.
  // This function can pop up an error message.
  virtual bool Validate(wxWindow *WXUNUSED(parent)) { return true; };

  std::string GetFloatChars() {return m_floatChars;};   

private:
  // WDR: member variable declarations for CFloatValidator
  std::string m_floatChars;
private:
  // WDR: handler declarations for CFloatValidator
  void OnChar( wxKeyEvent &event );
  DECLARE_NO_COPY_CLASS(CFloatValidator)

private:
    DECLARE_EVENT_TABLE()
};
//----------------------------------------------------------------------------
// CFloatExtendedValidator 
//----------------------------------------------------------------------------

class CFloatExtendedValidator : public wxValidator
{
public:
    // constructors and destructors
  CFloatExtendedValidator ();
  virtual ~CFloatExtendedValidator ();
    
  // WDR: method declarations for CFloatExtendedValidator 
  virtual wxObject* Clone() const;

  // Called to transfer data to the window
  virtual bool TransferToWindow() { return true; }

  // Called to transfer data from the window
  virtual bool TransferFromWindow() { return true; };

  // Called when the value in the window must be validated.
  // This function can pop up an error message.
  virtual bool Validate(wxWindow *WXUNUSED(parent)) { return true; };

  std::string GetFloatChars() {return m_floatChars;};   

private:
  // WDR: member variable declarations for CFloatExtendedValidator 
  std::string m_floatChars;
private:
  // WDR: handler declarations for CFloatExtendedValidator 
  void OnChar( wxKeyEvent &event );
  DECLARE_NO_COPY_CLASS(CFloatExtendedValidator )

private:
    DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------------------
// CTextFileValidator
//----------------------------------------------------------------------------

class CTextFileValidator: public wxValidator
{
public:
  // constructors and destructors
  CTextFileValidator();
  virtual ~CTextFileValidator();
    
  // WDR: method declarations for CDigitValidator
  virtual wxObject* Clone() const;

  // Called to transfer data to the window
  virtual bool TransferToWindow() { return true; }

  // Called to transfer data from the window
  virtual bool TransferFromWindow() { return true; };

  // Called when the value in the window must be validated.
  // This function can pop up an error message.
  virtual bool Validate(wxWindow *WXUNUSED(parent)) { return true; };

  std::string GetDigits() {return m_digits;};   
    
private:
  // WDR: member variable declarations for CDigitValidator
  std::string m_digits;    
private:
  // WDR: handler declarations for CDigitValidator
  void OnChar( wxKeyEvent &event );
  DECLARE_NO_COPY_CLASS(CTextFileValidator)

private:
    DECLARE_EVENT_TABLE()
};
//----------------------------------------------------------------------------
// CTextDirValidator
//----------------------------------------------------------------------------

class CTextDirValidator: public wxValidator
{
public:
  // constructors and destructors
  CTextDirValidator();
  virtual ~CTextDirValidator();
    
  // WDR: method declarations for CDigitValidator
  virtual wxObject* Clone() const;

  // Called to transfer data to the window
  virtual bool TransferToWindow() { return true; }

  // Called to transfer data from the window
  virtual bool TransferFromWindow() { return true; };

  // Called when the value in the window must be validated.
  // This function can pop up an error message.
  virtual bool Validate(wxWindow *WXUNUSED(parent)) { return true; };

  std::string GetDigits() {return m_digits;};   
    
private:
  // WDR: member variable declarations for CDigitValidator
  std::string m_digits;    
private:
  // WDR: handler declarations for CDigitValidator
  void OnChar( wxKeyEvent &event );
  DECLARE_NO_COPY_CLASS(CTextDirValidator)

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// CTextLightValidator
//----------------------------------------------------------------------------

class CTextLightValidator: public wxValidator
{
public:
  // constructors and destructors
  CTextLightValidator();
  virtual ~CTextLightValidator();
    
  // WDR: method declarations for CDigitValidator
  virtual wxObject* Clone() const;

  // Called to transfer data to the window
  virtual bool TransferToWindow() { return true; }

  // Called to transfer data from the window
  virtual bool TransferFromWindow() { return true; };

  // Called when the value in the window must be validated.
  // This function can pop up an error message.
  virtual bool Validate(wxWindow *WXUNUSED(parent)) { return true; };
  
  std::string GetDigits() {return m_digits;};   

  bool IsValid(const wxString& string);

private:
  // WDR: member variable declarations for CDigitValidator
  std::string m_digits;    
private:
  // WDR: handler declarations for CDigitValidator
  void OnChar( wxKeyEvent &event );
  DECLARE_NO_COPY_CLASS(CTextLightValidator)

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// CDateValidator
//----------------------------------------------------------------------------

class CDateValidator: public wxValidator
{
public:
  // constructors and destructors
  CDateValidator();
  virtual ~CDateValidator();
    
  // WDR: method declarations for CDigitValidator
  virtual wxObject* Clone() const;

  // Called to transfer data to the window
  virtual bool TransferToWindow() { return true; }

  // Called to transfer data from the window
  virtual bool TransferFromWindow() { return true; };

  // Called when the value in the window must be validated.
  // This function can pop up an error message.
  virtual bool Validate(wxWindow *WXUNUSED(parent)) { return true; };

  std::string GetDigits() {return m_digits;};   

private:
  // WDR: member variable declarations for CDigitValidator
  std::string m_digits;    
private:
  // WDR: handler declarations for CDigitValidator
  void OnChar( wxKeyEvent &event );
  DECLARE_NO_COPY_CLASS(CDateValidator)

private:
    DECLARE_EVENT_TABLE()
};


#endif
