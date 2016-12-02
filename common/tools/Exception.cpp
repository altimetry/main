/*
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
#include "stdafx.h"

#include "brathl_error.h"
//#include "brathl.h"

#include "TraceLog.h"
//#include "Tools.h"
#include "Exception.h"


//----------------------------------------

CException::CException(const std::string& message, int errcode)
{
  m_errcode = errcode;
  m_message = message;
}

CException::~CException() throw()
{

}


//----------------------------------------
const char* CException::what() const throw( )
{
	static std::string buffer;
	buffer = std::string( TypeOf() ) + " #" + n2s<std::string>( m_errcode ) + ": " + m_message;
	return buffer.c_str();
}


//----------------------------------------
void CException::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CException Object at "<< this << std::endl;
  fOut << "m_message = " << this->what() << std::endl;
  fOut << "m_errcode = " << m_errcode << std::endl;

  fOut << "==> END Dump a CException Object at "<< this << std::endl;

  fOut << std::endl;
}



//-------------------------------------------------------------
//------------------- CFileException class --------------------
//-------------------------------------------------------------



CFileException::CFileException(const std::string& message, const std::string& fileName, int errcode)
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
 
CProductException::CProductException(const std::string& message, const std::string& fileName, int errcode)
      	: CException(message, errcode)
{
  m_message += " - Product file name: '" + fileName + "'";
}



CProductException::CProductException(const std::string& message, const std::string& fileName,
                                     const std::string& productClass,
                                     const std::string& productType,
                                     int errcode)
      	: CException(message, errcode)
{
  m_message += " - Product file name: '" + fileName + "' - Product Class/Type '" + productClass + "/" + productType;
}




//-------------------------------------------------------------
//---------------- CExpressionException class -----------------
//-------------------------------------------------------------


CExpressionException::CExpressionException(const std::string& message, int errcode, const std::string &expression /*= ""*/)
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

CAlgorithmException::CAlgorithmException(const std::string& message, const std::string& algorithmName, int errcode)
  : CException(message, errcode)
{
  m_message += " - Algorithm name: '" + algorithmName + "'";
}
