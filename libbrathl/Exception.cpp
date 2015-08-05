
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


#include <cerrno>
#include <cstdio>
#include <cstring>

#include "Stl.h"

#include "brathl_error.h"
#include "brathl.h"

#include "TraceLog.h"
#include "Tools.h"
#include "Exception.h"
using namespace brathl;

namespace brathl
{

CException::CException()
{

}

//----------------------------------------

CException::CException(const string& message, int32_t errcode)
{
  m_errcode = errcode;
  m_message = message;
}

CException::~CException() throw()
{

}


//----------------------------------------
const char* CException::what() const throw()
{
  static char msg[2048];
  CTools::snprintf(msg, sizeof(msg), "%s #%d: %s", TypeOf(), m_errcode, m_message.c_str());
  return msg;
}


//----------------------------------------
void CException::Dump(ostream& fOut /* = cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CException Object at "<< this << endl;
  fOut << "m_message = " << this->what() << endl;
  fOut << "m_errcode = " << m_errcode << endl;



  fOut << "==> END Dump a CException Object at "<< this << endl;

  fOut << endl;

}



//-------------------------------------------------------------
//------------------- CFileException class --------------------
//-------------------------------------------------------------



CFileException::CFileException(const string& message, const string& fileName, int32_t errcode)
  : CException(message, errcode)
{
  m_message += " - File name: '" + fileName + "'";
}






//-------------------------------------------------------------
//------------------- CParameterException class ---------------
//-------------------------------------------------------------



//-------------------------------------------------------------
//------------------- CProductException class --------------------
//-------------------------------------------------------------
 
CProductException::CProductException(const string& message, const string& fileName, int32_t errcode)
      	: CException(message, errcode)
{
  m_message += " - Product file name: '" + fileName + "'";
}



CProductException::CProductException(const string& message, const string& fileName,
                                     const string& productClass,
                                     const string& productType,
                                     int32_t errcode)
      	: CException(message, errcode)
{
  m_message += " - Product file name: '" + fileName + "' - Product Class/Type '" + productClass + "/" + productType;
}




//-------------------------------------------------------------
//---------------- CExpressionException class -----------------
//-------------------------------------------------------------


CExpressionException::CExpressionException(const string& message, int32_t errcode, const string &expression /*= ""*/)
      	: CException(message, errcode)
{
  if (expression != "")
  {
    m_message += " (Expression='" + expression + "')";
  }
}




//-------------------------------------------------------------
//---------------- CUnImplementException class ----------------
//-------------------------------------------------------------

//-------------------------------------------------------------
//------------------- CAlgorithmException class --------------------
//-------------------------------------------------------------

CAlgorithmException::CAlgorithmException(const string& message, const string& algorithmName, int32_t errcode)
  : CException(message, errcode)
{
  m_message += " - Algorithm name: '" + algorithmName + "'";
}


}
