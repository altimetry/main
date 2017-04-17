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
#if !defined(BRAT_EXCEPTION_H)
#define BRAT_EXCEPTION_H

#include <iostream>
#include <string>
#include <cstring>
#include <limits>


#include "brathl_error.h"


//-------------------------------------------------------------
//------------------- CException class --------------------
//-------------------------------------------------------------

/** \file
  This file contains the various std::exception classes of brathl
*/



/** \addtogroup errors Errors management
  @{ */

/**
  Exception management class.

 \version 1.0
*/

#if defined(WIN32) && defined(GetMessage)
#undef GetMessage
#endif

class CException : public std::exception
{
	//types
#if defined(PRE_CPP11)
    typedef std::exception base_t;
#else
    using base_t = std::exception;
#endif


	//data
protected:
	int m_errcode;
	std::string m_message;

public:
	/// Empty CException ctor
	CException()
	{}

	//@{
	/** Creates a new CException object.
	  \param message [in] : error message
	  \param errcode [in] : error code
	  */
	CException( const std::string& message, int errcode = BRAT_V4_ERROR );	// (*)

	CException( const CException &o )
	{
		if ( this != &o )
		{
			static_cast< std::exception& >( *this ) = o;
			m_errcode = o.m_errcode;
			m_message = o.m_message;
		}
	}
	//
	// (*) Not being a default parameter in v3, and BRAT_V4_ERROR being a new value
	//	errcode is assumed to always be != of BRAT_V4_ERROR for v3 raised exceptions

	//@}


	/// Destructor
	virtual ~CException() throw( );

	///Dump function
	virtual void Dump( std::ostream& fOut = std::cerr );

	virtual const char* TypeOf() const { return "std::exception"; }

    virtual const char *what() const throw() override;
	int error() const { return m_errcode; }
	std::string Message() const { return m_message; }
};

//-------------------------------------------------------------
//------------------- CFileException class --------------------
//-------------------------------------------------------------


/**
  File Exception management class.

 \version 1.0
*/

class CFileException : public CException
{

public:

  /// Empty CFileException ctor
  CFileException() {};

  /** Creates a new CFileException object.
    \param message [in] : error message
    \param errcode [in] : error code
    */
  CFileException(const std::string& message, int errcode = BRATHL_ERROR)
      	: CException(message, errcode){};

  /** Creates a new CFileException object.
    \param message [in] : error message
    \param fileName [in] : file name in error
    \param errcode [in] : error code
    */
  CFileException(const std::string& message, const std::string& fileName, int errcode);

  /// Destructor
  virtual ~CFileException() throw() {};

  /// Identification of std::exception (human readable)
  virtual const char * TypeOf() const { return "FileException"; };


};


//-------------------------------------------------------------
//------------------- CParameterException class --------------------
//-------------------------------------------------------------


/**
  Parameter Exception management class.


 \version 1.0
*/

class CParameterException : public CException
{

public:

  /// Empty CParameterException ctor
  CParameterException() {};

  /** Creates a new CParameterException object.
    \param message [in] : error message
    \param errcode [in] : error code
    */
  CParameterException(const std::string& message, int errcode)
      	: CException(message, errcode){};
  /// Destructor
  virtual ~CParameterException()  throw() {};

  /// Identification of std::exception (human readable)
  virtual const char * TypeOf() const { return "ParameterException"; };


};



//-------------------------------------------------------------
//------------------- CProductException class --------------------
//-------------------------------------------------------------


/**
  Product Exception management class.


 \version 1.0
*/

class CProductException : public CException
{

public:

  /// Empty CProductException ctor
  CProductException() {};

  /** Creates a new CProductException object.
    \param message [in] : error message
    \param errcode [in] : error code
    */
  CProductException(const std::string& message, int errcode)
      	: CException(message, errcode){};


  /** Creates a new CFileException object.
    \param message [in] : error message
    \param fileName [in] : file name in error
    \param errcode [in] : error code
    */
  CProductException(const std::string& message, const std::string& fileName, int errcode);

    /** Creates a new CProductException object.
    \param message [in] : error message
    \param fileName [in] : product file name
    \param productClass [in] : product class
    \param productType [in] : product type
    \param errcode [in] : error code
    */
  CProductException(const std::string& message, const std::string& fileName,
                                       const std::string& productClass,
                                       const std::string& productType,
                                       int errcode);

  /// Destructor
  virtual ~CProductException()  throw() {};

  /// Identification of std::exception (human readable)
  virtual const char * TypeOf() const { return "ProductExeption"; };

};




//-------------------------------------------------------------
//---------------- CExpressionException class -----------------
//-------------------------------------------------------------


/**
  Expression Exception management class.


 \version 1.0
*/

class CExpressionException : public CException
{

public:

  /// Empty CExpressionException ctor
  CExpressionException() {};

  /** Creates a new CParameterException object.
    \param message [in] : error message
    \param errcode [in] : error code
    \param expression [in] : expression being compiled
    */
  CExpressionException(const std::string& message, int errcode, const std::string& expression = "");

  /// Destructor
  virtual ~CExpressionException()  throw() {};

  /// Identification of std::exception (human readable)
  virtual const char * TypeOf() const { return "ExpressionException"; };

};




//-------------------------------------------------------------
//---------------- CMemoryException class -----------------
//-------------------------------------------------------------


/**
  memory Exception management class.


 \version 1.0
*/

class CMemoryException : public CException
{

public:

  /// Empty CMemoryException ctor
  CMemoryException() {};

  /** Creates a new CMemoryException object.
    \param message [in] : error message
    \param errcode [in] : error code
    */
  CMemoryException(const std::string& message, int errcode = BRATHL_MEMORY_ERROR)
      	: CException(message, errcode){};

  /// Destructor
  virtual ~CMemoryException()  throw() {};

  /// Identification of std::exception (human readable)
  virtual const char * TypeOf() const { return "MemoryException"; };

};



//-------------------------------------------------------------
//------------------- CUnImplementException class --------------------
//-------------------------------------------------------------

/**
  Unimplement Exception management class.

 \version 1.0
*/

class CUnImplementException : public CException
{

public:

  /// Empty CUnImplementException ctor
  CUnImplementException() {};

  //@{
  /** Creates a new CUnImplementException object.
    \param message [in] : error message
    \param errcode [in] : error code
    */
  CUnImplementException(const std::string& message, int errcode = BRATHL_UNIMPLEMENT_ERROR)
      	: CException(message, errcode)
  {};

   //@}


  /// Destructor
  virtual ~CUnImplementException() throw() {};

  /// Identification of std::exception (human readable)
  virtual const char * TypeOf() const { return "UnimplementedException"; };

};



//-------------------------------------------------------------
//------------------- CAlgorithmException class --------------------
//-------------------------------------------------------------


/**
  Algorithm Exception management class.

 \version 1.0
*/

class CAlgorithmException : public CException
{

public:

  /// Empty CAlgorithmException ctor
  CAlgorithmException() {};

  /** Creates a new CAlgorithmException object.
    \param message [in] : error message
    \param errcode [in] : error code
    */
  CAlgorithmException(const std::string& message, int errcode = BRATHL_ERROR)
      	: CException(message, errcode){};

  /** Creates a new CAlgorithmException object.
    \param message [in] : error message
    \param fileName [in] : file name in error
    \param errcode [in] : error code
    */
  CAlgorithmException(const std::string& message, const std::string& algorithmName, int errcode);

  /// Destructor
  virtual ~CAlgorithmException() throw() {};

  /// Identification of std::exception (human readable)
  virtual const char * TypeOf() const { return "AlgorithmException"; };


};


//-------------------------------------------------------------
//------------------- CXMLException class --------------------
//-------------------------------------------------------------


/**
  XML Exception management class.


 \version 1.0
*/

class CXMLException : public CException
{

public:

  /// Empty CParameterException ctor
  CXMLException() {};

  /** Creates a new CParameterException object.
    \param message [in] : error message
    \param errcode [in] : error code
    */
  CXMLException(const std::string& message, int errcode)
      	: CException(message, errcode){};
  /// Destructor
  virtual ~CXMLException()  throw() {};

  /// Identification of std::exception (human readable)
  virtual const char * TypeOf() const { return "XMLException"; };


};

//-------------------------------------------------------------
//------------------- CXMLParseException class --------------------
//-------------------------------------------------------------


/**
  XML Parse Exception management class.


 \version 1.0
*/

class CXMLParseException : public CXMLException
{

public:

  CXMLParseException() {};

  /** Creates a new CParameterException object.
    \param message [in] : error message
    \param errcode [in] : error code
    */
  CXMLParseException(const std::string& message, int errcode)
      	: CXMLException(message, errcode){};
  /// Destructor
  virtual ~CXMLParseException()  throw() {};

  /// Identification of std::exception (human readable)
  virtual const char * TypeOf() const { return "XMLParseException"; };


};

//-------------------------------------------------------------
//------------------- CLoadAliasesException class --------------------
//-------------------------------------------------------------


/**
  Aliases loading Exception management class.


 \version 1.0
*/

class CLoadAliasesException : public CException
{

public:

  CLoadAliasesException() {};

  /** Creates a new CParameterException object.
    \param message [in] : error message
    \param errcode [in] : error code
    */
  CLoadAliasesException(const std::string& message, int errcode)
      	: CException(message, errcode){};
  /// Destructor
  virtual ~CLoadAliasesException()  throw() {};

  /// Identification of std::exception (human readable)
  virtual const char * TypeOf() const { return "LoadAliasesException"; };


};
/** @} */

#endif // !defined(BRAT_EXCEPTION_H)
