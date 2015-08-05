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


#ifndef __DirTraverser_H__
#define __DirTraverser_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "DirTraverser.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/dir.h"
#include "wx/filename.h"

// WDR: class declarations


//-------------------------------------------------------------
//------------------- CDirTraverserDeleteFile class --------------------
//-------------------------------------------------------------
class CDirTraverserDeleteFile : public wxDirTraverser
{
public:
    CDirTraverserDeleteFile();
    virtual ~CDirTraverserDeleteFile();

    virtual wxDirTraverseResult OnFile(const wxString& filename);

    virtual wxDirTraverseResult OnDir(const wxString& dirname);
};


//-------------------------------------------------------------
//------------------- CDirTraverserDeleteDir class --------------------
//-------------------------------------------------------------
class CDirTraverserDeleteDir : public wxDirTraverser
{
public:
    CDirTraverserDeleteDir();
    virtual ~CDirTraverserDeleteDir();

    virtual wxDirTraverseResult OnFile(const wxString& filename);

    virtual wxDirTraverseResult OnDir(const wxString& dirname);
};

//-------------------------------------------------------------
//------------------- CDirTraverserCopyFile class --------------------
//-------------------------------------------------------------
class CDirTraverserCopyFile : public wxDirTraverser
{
public:
    CDirTraverserCopyFile(const wxString& destDir, const wxString& extension );
    CDirTraverserCopyFile(const wxFileName& destDir, const wxString& extension);
    virtual ~CDirTraverserCopyFile();

    virtual wxDirTraverseResult OnFile(const wxString& filename);

    virtual wxDirTraverseResult OnDir(const wxString& dirname);

protected:
  wxFileName m_destDir;
  wxString m_extension;
};

//-------------------------------------------------------------
//------------------- CDirTraverserListFile class --------------------
//-------------------------------------------------------------
class CDirTraverserListFile : public wxDirTraverser
{
public:
    CDirTraverserListFile();
    virtual ~CDirTraverserListFile();

    virtual wxDirTraverseResult OnFile(const wxString& filename);

    virtual wxDirTraverseResult OnDir(const wxString& dirname);

    wxArrayString m_array;
};

//-------------------------------------------------------------
//------------------- CDirTraverserExistFile class --------------------
//-------------------------------------------------------------
class CDirTraverserExistFile : public wxDirTraverser
{
public:
    CDirTraverserExistFile(const wxString& fileName);
    virtual ~CDirTraverserExistFile();

    virtual wxDirTraverseResult OnFile(const wxString& filename);

    virtual wxDirTraverseResult OnDir(const wxString& dirname);

    wxFileName m_fileName;
    bool m_found;

protected:
  wxString m_fileToFind;
};


#endif
