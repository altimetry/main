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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "Config.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Config.h"

CConfig* CConfig::instance = NULL;

CConfig::CConfig()
{
    m_config = new wxConfig("BratDisplay", wxEmptyString, "BratDisplay.ini", wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
    //new wxFileConfig(wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
}


CConfig * CConfig::GetInstance()
{
    if ( instance == NULL )
        instance = new CConfig();

    return instance;
}

CConfig::~CConfig()
{
    delete m_config;
}

bool CConfig::GetEntry(const wxString &key, wxString *data)
{
    if (m_config == NULL)
    {
        return false;
    }

    return m_config->Read(key, data);
}

bool CConfig::SetEntry(const wxString &key, wxString data)
{
    if (m_config == NULL)
    {
        return false;
    }

    return m_config->Write(key, data);
}

bool CConfig::Flush()
{
    return m_config->Flush();
}
