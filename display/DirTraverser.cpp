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
    #pragma implementation "DirTraverser.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "new-gui/Common/tools/brathl_error.h"
#include "DirTraverser.h"

// WDR: class implementations

//-------------------------------------------------------------
//------------------- CDirTraverserDeleteFile class --------------------
//-------------------------------------------------------------

//----------------------------------------
CDirTraverserDeleteFile::CDirTraverserDeleteFile()
{

}
//----------------------------------------
CDirTraverserDeleteFile::~CDirTraverserDeleteFile()
{

}
//----------------------------------------
wxDirTraverseResult CDirTraverserDeleteFile::OnFile(const wxString& filename)
{

  if (wxRemoveFile(filename) == false)
  {
    wxMessageBox(wxString::Format("Unable to remove file '%s",
                                  filename.c_str()),
                 "Warning",
                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
    //return wxDIR_CONTINUE;
  
  }
  return wxDIR_CONTINUE;
}
//----------------------------------------
wxDirTraverseResult CDirTraverserDeleteFile::OnDir(const wxString& dirname)
{
    UNUSED(dirname);

  return wxDIR_CONTINUE;
}


//-------------------------------------------------------------
//------------------- CDirTraverserDeleteDir class --------------------
//-------------------------------------------------------------

//----------------------------------------
CDirTraverserDeleteDir::CDirTraverserDeleteDir()
{

}
//----------------------------------------
CDirTraverserDeleteDir::~CDirTraverserDeleteDir()
{

}
//----------------------------------------
wxDirTraverseResult CDirTraverserDeleteDir::OnFile(const wxString& filename)
{
    UNUSED(filename);

  return wxDIR_CONTINUE;
}
//----------------------------------------
wxDirTraverseResult CDirTraverserDeleteDir::OnDir(const wxString& dirname)
{

  if (wxRmdir(dirname) == false)
  {
    wxMessageBox(wxString::Format("Unable to remove directory '%s",
                                  dirname.c_str()),
                 "Warning",
                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
    //return wxDIR_CONTINUE;
  
  }
  return wxDIR_IGNORE;
}

//-------------------------------------------------------------
//------------------- CDirTraverserCopyFile class --------------------
//-------------------------------------------------------------

//----------------------------------------
CDirTraverserCopyFile::CDirTraverserCopyFile(const wxString& destDir, const wxString& extension)
{
  m_destDir.AssignDir(destDir);
  m_destDir.Normalize();
  m_extension = extension;
}
//----------------------------------------
CDirTraverserCopyFile::CDirTraverserCopyFile(const wxFileName& destDir, const wxString& extension)
{
  m_destDir = destDir;
  m_destDir.Normalize();
  m_extension = extension;
}
//----------------------------------------
CDirTraverserCopyFile::~CDirTraverserCopyFile()
{
}
//----------------------------------------
wxDirTraverseResult CDirTraverserCopyFile::OnFile(const wxString& fileName)
{
  wxFileName srcFile;
  srcFile.Assign(fileName);
  srcFile.Normalize();

  if ( (m_extension.IsEmpty() == false) || (m_extension != "*") )
  {
    if (srcFile.GetExt() != m_extension)
    {
        return wxDIR_CONTINUE;
    }
  }

  wxFileName destFile = m_destDir;
  destFile.SetFullName(srcFile.GetFullName());

  if (wxCopyFile(fileName, destFile.GetFullPath()) == false)
  {
    wxMessageBox(wxString::Format("Unable to copy file '%s' to '%s'" ,
                                  fileName.c_str(),
                                  destFile.GetFullPath().c_str()),
                 "Warning",
                  wxOK | wxCENTRE | wxICON_EXCLAMATION);
    //return wxDIR_CONTINUE;
  
  }

  return wxDIR_CONTINUE;
}
//----------------------------------------
wxDirTraverseResult CDirTraverserCopyFile::OnDir(const wxString& dirName)
{
    UNUSED(dirName);

  return wxDIR_CONTINUE;
}


//-------------------------------------------------------------
//------------------- CDirTraverserListFile class --------------------
//-------------------------------------------------------------

//----------------------------------------
CDirTraverserListFile::CDirTraverserListFile()
{

}
//----------------------------------------
CDirTraverserListFile::~CDirTraverserListFile()
{

}
//----------------------------------------
wxDirTraverseResult CDirTraverserListFile::OnFile(const wxString& fileName)
{  
  m_array.Add(fileName);
  return wxDIR_CONTINUE;
}
//----------------------------------------
wxDirTraverseResult CDirTraverserListFile::OnDir(const wxString& dirName)
{
    UNUSED(dirName);

  return wxDIR_CONTINUE;
}
//-------------------------------------------------------------
//------------------- CDirTraverserExistFile class --------------------
//-------------------------------------------------------------

//----------------------------------------
CDirTraverserExistFile::CDirTraverserExistFile(const wxString& fileName)
{
  m_fileToFind = fileName;
  m_found = false;
}
//----------------------------------------
CDirTraverserExistFile::~CDirTraverserExistFile()
{

}
//----------------------------------------
wxDirTraverseResult CDirTraverserExistFile::OnFile(const wxString& fileName)
{ 
  wxFileName fileNameTmp;
  fileNameTmp.Assign(fileName);

  if (fileNameTmp.GetFullName().CmpNoCase(m_fileToFind) == 0)
  {
    m_found = true;
    m_fileName.Assign(fileName);
    // stop traversing
    return wxDIR_STOP;
  }

  return wxDIR_CONTINUE;
}
//----------------------------------------
wxDirTraverseResult CDirTraverserExistFile::OnDir(const wxString& dirName)
{ 
    UNUSED(dirName);

  return wxDIR_CONTINUE;
}
