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

#if !defined(_Trace_h_)
#define _Trace_h_

#include "Stl.h"
#include "brathl.h"

namespace brathl
{


class CSmartCleaner;


class CTrace
{
protected:
   CTrace();

public:

   virtual ~CTrace();

// Methods
public:

   static string ParseArg(int argc, char *argv[]);
   static CTrace* CreateObject(const string& szFileLog);
   static CTrace* CreateObject(int argc, char *argv[]);

   static CTrace* GetInstance(const string& szFileLog);
   static CTrace* GetInstance(int argc, char *argv[]);
   static CTrace* GetInstance();
   
   //static void Release();
   static ostream* SetDumpContext(ostream* value);
   static ostream* GetDumpContext();

   static bool IsTrace
		(int32_t	Level	= 5);
   static void Tracer
		(const char	*message,
		 ...)
	__attribute__ ((format(printf, 1, 2)));
   static void Tracer
		(const string	&message);
   static void Tracer
		(int32_t	Level,
		 const string	&message);
   static void Tracer
		(int32_t	Level,
		 const char	*message,
		 ...)
	__attribute__ ((format(printf, 2, 3)));
   static void Print
		(const char	*message,
		 ...)
	__attribute__ ((format(printf, 1, 2)));
   static void Print
		(const string	&message);
   static void Print
		(int32_t	Level,
		 const string	&message);
   static void Print
		(int32_t	Level,
		 const char	*message,
		 ...)
	__attribute__ ((format(printf, 2, 3)));
  /**
  * Indicates the wanted level of trace:
  *	0: No trace (default)
  *	1: Program steps
  *	2: Running
  *	3: Inputs
  *     4: Inputs+outputs
  *     5: Debug (very verbose for development perpose). Default for
  *	   trace in a file
  */
   static void SetTraceLevel
		(int32_t	Level);
   static int32_t GetTraceLevel
		();


protected:
   static void PrintText
		(bool		NewLine,
		 const char	*message,
		 va_list	args);
   static void PrintText
		(bool		NewLine,
		 const string	&message);

protected:

   virtual ostream* SetDumpContextReal(ostream* value);
   virtual ostream* GetDumpContextReel();
private:
   static void prepareSmartCleaner(void);

//Attributes
public:

protected:

   /**
   * unique instance of the class
   */
   static CTrace* m_instance;

   /**
   * out stream
   */
   ostream *m_fOut;

  /**
  * Indicates is there is a file opened (whether log file is opened or closed)
  */
  bool  	m_bFileLog ;

private :
  static int32_t	m_TraceLevel;

};


class CSmartCleaner
  {
    // the cleaner stores the address of the instance pointer
    // for a singelton object. The CSmartCleaner MUST be
    // instanciated as a static object in a function, which
    // ensures it is destroyed after exit, but is safe if
    // the singleton object IS explicitly deleted.

    private :
      CTrace **m_pAddressOfStaticInstancePtr;

    public :
      CSmartCleaner( CTrace **addressOfStaticInstancePtr) : m_pAddressOfStaticInstancePtr( addressOfStaticInstancePtr ) { }
      virtual ~CSmartCleaner()
      {
	if( *m_pAddressOfStaticInstancePtr != NULL )
	  {
	    delete *m_pAddressOfStaticInstancePtr;
	  }
      }
      CTrace * GetObject()
        {
	  return *m_pAddressOfStaticInstancePtr;
        }
  };

}
#endif // !defined(_Trace_h_)
