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
#if !defined(_Dataset_h_)
#define _Dataset_h_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "Dataset.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/filename.h"
#include "wx/config.h"  // (to let wxWidgets choose a wxConfig class for your platform)
#include "wx/confbase.h"  // (base config class)
#include "wx/fileconf.h" // (wxFileConfig class)
//#include "wx/msw/regconf.h" // (wxRegConfig class)


#include "brathl.h"

#include "Product.h"
using namespace brathl;



class CDataset : public CBratObject
{

public:


  /// Empty CDataset ctor
  CDataset(const wxString name);

  CDataset(CDataset& d);


  /// Destructor
  virtual ~CDataset();

  wxString GetName() {return m_name;};
  void SetName(const wxString& value) {m_name = value;};
  
  void GetFiles( wxArrayString& array );
  void GetFiles( wxListBox& array );

  bool CtrlFiles();

  /*
  void GetRecordNames( wxArrayString& array, CProduct* product);
  void GetRecordNames( CStringArray& array, CProduct* product );
  void GetRecordNames(wxComboBox& combo, CProduct* product);
*/

  bool SaveConfig(wxFileConfig* config);
  bool SaveConfig(wxFileConfig* config, const wxString& entry);
  bool SaveConfigSpecificUnit(wxFileConfig* config, const wxString& entry);
  
  bool LoadConfig(wxFileConfig* config);

  //CProduct* GetProduct() {return m_product;};
  CProduct* SetProduct( const wxString& fileName );
  CProduct* SetProduct();
  
  //bool CloseProduct();
  //bool OpenProduct();

  //void DeleteProduct();

  CProductList* GetProductList() {return &m_files;};


  CStringMap* GetFieldSpecificUnits() { return &m_fieldSpecificUnit; };
  
  string GetFieldSpecificUnit(const string& key);
  void SetFieldSpecificUnit(const string& key, const string& value);

  CDataset& operator=(CDataset& d);

   ///Dump fonction
   virtual void Dump(ostream& fOut = cerr);

protected:
  void Init();
  void Copy(CDataset& d);

protected:

  CProductList m_files;
  //CProduct* m_product;
  CStringMap m_fieldSpecificUnit;


private:
  wxString m_name;



};

/** @} */


#endif // !defined(_Parameter_h_)
