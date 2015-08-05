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

#ifndef __Dnd_H__
#define __Dnd_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "Dnd.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif



#include <wx/dnd.h>
//#include <wx/listctrl.h>

#include "Field.h" 
#include "List.h"
using namespace brathl;


static const wxChar* fieldFormatId = _T("CDndFieldDataObject");

// WDR: class declarations
//----------------------------------------------------------------------------
// CDndField
//----------------------------------------------------------------------------

class CDndField
{

public:

//  CDndField() : m_fields(false) {  };
  CDndField() { m_field = NULL; };
  CDndField(CField* field) { m_field = field; };

  CDndField(CDndField& dndField);
  
  ~CDndField() { };

  //void AddField(CField* field);
  //void AddField(CObArray& fields);

  //void GetField(CObArray& fields);

  //CField* Item(int32_t nIndex) const {return dynamic_cast<CField*>(m_fields.at(nIndex));}
  //size_t GetCount() const {return m_fields.size();}

  CField* m_field;

};

//----------------------------------------------------------------------------
// CDndFieldDataObject
//----------------------------------------------------------------------------

class CDndFieldDataObject : public wxDataObjectSimple
{
   public:

      CDndFieldDataObject(CDndField* dndField = NULL);
      virtual ~CDndFieldDataObject() {delete m_dndField;}
      
      virtual size_t GetDataSize() const;
      virtual bool GetDataHere(void* buf) const;
      virtual bool SetData(size_t len, const void* buf);

      CDndField* GetField() {return m_dndField;}
      
   private:

      CDndField* m_dndField;
};

/*
class wxDndRemoteTarget : public wxDropTarget
{
   public:

      wxDndRemoteTarget(wxListCtrl* owner, wxMyFTPFrame* frame) :
         wxDropTarget(new wxDndUploadFileDataObject)
         {
            m_Owner = owner;
            m_Frame = frame;
         }


      virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
      virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);

   private:

      wxListCtrl* m_Owner;
      wxMyFTPFrame* m_Frame;
};


class wxDndDownloadFile
{
   public:

      wxDndDownloadFile() : m_Service(0) {}
      wxDndDownloadFile(const wxString& path, const wxString& hostname,
                        unsigned short service, const wxString& user,
                        const wxString& password, int mode) :
         m_Path(path), m_Hostname(hostname), m_Service(service), m_User(user),
         m_Password(password), m_Mode(mode) {}

      wxDndDownloadFile(const wxDndDownloadFile& dndtransfile);
      ~wxDndDownloadFile() {m_Files.Clear();}

      void AddFile(const wxString& file)
            {if (!file.IsEmpty()) m_Files.Add(file);}

      wxString& Item(size_t nIndex) const {return m_Files.Item(nIndex);}
      size_t GetCount() const {return m_Files.GetCount();}

      wxString m_Path;
      wxString m_Hostname;
      unsigned short m_Service;
      wxString m_User;
      wxString m_Password;
      int m_Mode;
      wxArrayString m_Files;
};

class wxDndDownloadFileDataObject : public wxDataObjectSimple
{
   public:

      wxDndDownloadFileDataObject(wxDndDownloadFile* dndtransfile = 0);
      virtual ~wxDndDownloadFileDataObject() {delete m_DndTransFile;}

      virtual size_t GetDataSize() const;
      virtual bool GetDataHere(void* buf) const;
      virtual bool SetData(size_t len, const void* buf);

      wxDndDownloadFile* GetTransFile() {return m_DndTransFile;}

   private:

      wxDndDownloadFile* m_DndTransFile;
};

class wxDndLocaleTarget : public wxDropTarget
{
   public:

      wxDndLocaleTarget(wxListCtrl* owner, wxMyFTPFrame* frame) :
         wxDropTarget(new wxDndDownloadFileDataObject)
         {
            m_Owner = owner;
            m_Frame = frame;
         }


      virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
      virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);

   private:

      wxListCtrl* m_Owner;
      wxMyFTPFrame* m_Frame;
};

*/


#endif
