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
#if !defined(_Function_h_)
#define _Function_h_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "Function.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#include "brathl.h"

//-------------------------------------------------------------
//------------------- CFunction class --------------------
//-------------------------------------------------------------

class CFunction : public CBratObject
{

public:


  CFunction(const wxString& name, const wxString& description, int32_t category = MathTrigo, int32_t nbParams = 1);
  CFunction(const string& name, const string& description, int32_t category = MathTrigo, int32_t nbParams = 1);


  /// Destructor
  virtual ~CFunction();

  wxString GetSyntax();

  wxString GetName() {return m_name;};
  void SetName(const wxString& value) {m_name = value;};

  wxString GetDescription() {return m_description;};
  void SetDescription(const wxString& value) {m_description = value;};

  int32_t GetNbparams() {return m_nbParams;};
  void SetNbParams(int32_t value) {m_nbParams = value;};

  int32_t GetCategory() {return m_category;};
  wxString GetCategoryAsString();
  void SetCategory(int32_t value) {m_category = value;};
  
  static wxString GetCategoryAsString(int32_t category);

   ///Dump fonction
   virtual void Dump(ostream& fOut = cerr);
   virtual void DumpFmt(ostream& fOut = cerr);


protected:

  wxString m_name;
  wxString m_description;
  int32_t m_nbParams;
  int32_t m_category;



};


//-------------------------------------------------------------
//------------------- CMapFunction class --------------------
//-------------------------------------------------------------

class CMapFunction: public CObMap
{
public:
  /// CIntMap ctor
  CMapFunction();

  /// CIntMap dtor
  virtual ~CMapFunction();

  static CMapFunction&  GetInstance();

  wxString GetDescFunc(const wxString& name);
  wxString GetSyntaxFunc(const wxString& name);


  bool ValidName(const char* name);
  bool ValidName(const string& name);
  
  void NamesToArrayString(wxArrayString& array);
  void NamesToComboBox(wxComboBox& combo);
  void NamesToListBox(wxListBox& listBox, int32_t category = -1);

  void GetCategory(wxChoice& combo);

  wxString GetFunctionExtraDescr(const wxString& pathSuff);

  void SetExtraDescr();
  
  static wxString GetFunctionExtraDescr(wxFileConfig* config, const wxString& pathSuff = "");

  static void SaveFunctionDescrTemplate(wxFileConfig* config, bool flush = true);

  virtual void Dump(ostream& fOut  = cerr); 
  virtual void DumpFmt(ostream& fOut  = cerr); 



protected:

  wxFileConfig* m_config;
  static const wxString m_configFilename;


protected:

  void DeleteConfig();
  void Init();


};



/** @} */


#endif 
