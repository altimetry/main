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

#include "TraceLog.h"
using namespace brathl;

namespace brathl
{

CTraceLog::CTraceLog(const string& szFilelog)
{
   m_pFileLog		= NULL;
   m_szFileLogName	= szFilelog;

   OpenLogFile();
   SetTraceLevel(5);
}

CTraceLog::~CTraceLog()
{

   if (m_bFileLog)
   {
      m_pFileLog->close();
      delete m_pFileLog;
      m_pFileLog = NULL;
   }



}
void CTraceLog::OpenLogFile()
{

   m_pFileLog = new ofstream;
   m_pFileLog->open(m_szFileLogName.c_str(), ios::out | ios::trunc);
   //m_pFileLog.open(m_szFileLogName, ios::out)
   if (m_pFileLog->good() != true)
   {
      cerr << "CTrace::OpenLogFile() - Open trace file failed - Filename " << m_szFileLogName << ", error " << m_pFileLog->rdstate() << endl;
      return;
   }

   m_bFileLog = true;
}

ostream* CTraceLog::GetDumpContextReel()
{

   if (m_bFileLog)
   {
      return m_pFileLog;
   }
   else
   {
      return m_fOut;
   }
};

}
