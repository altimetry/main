/*
* Copyright (C) 2010 Science & Technology BV (S&T). Delft,NL
*
* This file is part of BRATDisplay
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

#ifndef __Workspace_H__
#define __Workspace_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "Config.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/filename.h"
#include "wx/config.h"  // (to let wxWidgets choose a wxConfig class for your platform)
#include "wx/confbase.h"  // (base config class)
#include "wx/fileconf.h" // (wxFileConfig class)
#include "wx/app.h"

#include "BratObject.h"
using namespace brathl;


// keys
const wxString CONFIG_SAVE_LOCATION = "SAVE_DIR";

//-------------------------------------------------------------
//------------------- CConfig class --------------------
//-------------------------------------------------------------

class CConfig: public CBratObject
{

private:

  static CConfig *instance;
  wxConfig *m_config;


  CConfig();

public:

   static CConfig *GetInstance();
   bool GetEntry(const wxString &key, wxString *data);
   bool SetEntry(const wxString &key, wxString data);
   bool Flush();

  ~CConfig();
};

#endif
