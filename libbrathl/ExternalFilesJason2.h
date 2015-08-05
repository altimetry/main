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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA	 02110-1301, USA.
*/
#ifndef _ExternalFilesJason2_h_
#define _ExternalFilesJason2_h_

#include <netcdf.h>
#include "BratObject.h"
#include "Stl.h"
#include "Mission.h"
#include "ExternalFilesNetCDF.h"

namespace brathl
{

/** \addtogroup tools Tools
  @{ */

/**
  Jason-2 files access.



 \version 1.0
*/
//----------------------------------------





//-------------------------------------------------------------
//------------------- CExternalFilesJason2 class --------------------
//-------------------------------------------------------------

class CExternalFilesJason2 : public CExternalFilesNetCDFCF
{
public:
  CExternalFilesJason2
		(const string		&name = "");

  virtual ~CExternalFilesJason2() { };

  static string TypeOf() { return "unknown"; };

 
protected:


public:
    static const string m_missionName;

protected:

};


//-------------------------------------------------------------
//------------------- CExternalFilesJason2SSHA class --------------------
//-------------------------------------------------------------

class CExternalFilesJason2SSHA : public CExternalFilesJason2
{
public:
  CExternalFilesJason2SSHA
		(const string		&Name = "");

  virtual ~CExternalFilesJason2SSHA() { };

  static string TypeOf() { return CTools::Format("%s/%s", m_missionName.c_str(), SSHA.c_str()); };
  virtual string GetType() { return TypeOf(); };

protected:


};




//-------------------------------------------------------------
//------------------- CExternalFilesJason2SSHAOGDR class --------------------
//-------------------------------------------------------------

class CExternalFilesJason2SSHAOGDR : public CExternalFilesJason2SSHA
{
public:
  CExternalFilesJason2SSHAOGDR
		(const string		&name = "")
	: CExternalFilesJason2SSHA(name)
  {
  }

  virtual ~CExternalFilesJason2SSHAOGDR() { };


  static string TypeOf() { return CTools::Format("%s/%s/OGDR", m_missionName.c_str(), SSHA.c_str()); };
  virtual string GetType() { return TypeOf(); };
protected:

};
//-------------------------------------------------------------
//------------------- CExternalFilesJason2SSHAIGDR class --------------------
//-------------------------------------------------------------

class CExternalFilesJason2SSHAIGDR : public CExternalFilesJason2SSHA
{
public:
  CExternalFilesJason2SSHAIGDR
		(const string		&name = "")
	: CExternalFilesJason2SSHA(name)
  {
  }

  virtual ~CExternalFilesJason2SSHAIGDR() { };

  static string TypeOf() { return CTools::Format("%s/%s/IGDR", m_missionName.c_str(), SSHA.c_str());};
  virtual string GetType() { return TypeOf(); };
};
//-------------------------------------------------------------
//------------------- CExternalFilesJason2SSHAGDR class --------------------
//-------------------------------------------------------------

class CExternalFilesJason2SSHAGDR : public CExternalFilesJason2SSHA
{
public:
  CExternalFilesJason2SSHAGDR
		(const string		&name = "")
	: CExternalFilesJason2SSHA(name)
  {
  }

  virtual ~CExternalFilesJason2SSHAGDR() { };

  static string TypeOf() { return CTools::Format("%s/%s/GDR", m_missionName.c_str(), SSHA.c_str()); };
  virtual string GetType() { return TypeOf(); };
};




//-------------------------------------------------------------
//------------------- CExternalFilesJason2GDR class --------------------
//-------------------------------------------------------------

class CExternalFilesJason2GDR : public CExternalFilesJason2
{
public:
  CExternalFilesJason2GDR (const string &name = "");

  virtual ~CExternalFilesJason2GDR() { };

  static string TypeOf() { return CTools::Format("%s/%s", m_missionName.c_str(), GDR.c_str());};
  virtual string GetType() { return TypeOf(); };


protected:

};


//-------------------------------------------------------------
//------------------- CExternalFilesJason2GDROGDR class --------------------
//-------------------------------------------------------------

class CExternalFilesJason2GDROGDR : public CExternalFilesJason2GDR
{
public:
  CExternalFilesJason2GDROGDR
		(const string		&name = "")
	: CExternalFilesJason2GDR(name)
  {
  }

  virtual ~CExternalFilesJason2GDROGDR() { };

  static string TypeOf() { return CTools::Format("%s/%s/OGDR", m_missionName.c_str(), GDR.c_str()); };
  virtual string GetType() { return TypeOf(); };
};

//-------------------------------------------------------------
//------------------- CExternalFilesJason2GDRIGDR class --------------------
//-------------------------------------------------------------

class CExternalFilesJason2GDRIGDR : public CExternalFilesJason2GDR
{
public:
  CExternalFilesJason2GDRIGDR
		(const string		&name = "")
	: CExternalFilesJason2GDR(name)
  {
  }

  virtual ~CExternalFilesJason2GDRIGDR() { };

  static string TypeOf() { return CTools::Format("%s/%s/IGDR", m_missionName.c_str(), GDR.c_str()); };
  virtual string GetType() { return TypeOf(); };
};

//-------------------------------------------------------------
//------------------- CExternalFilesJason2GDRGDR class --------------------
//-------------------------------------------------------------

class CExternalFilesJason2GDRGDR : public CExternalFilesJason2GDR
{
public:
  CExternalFilesJason2GDRGDR
		(const string		&name = "")
	: CExternalFilesJason2GDR(name)
  {
  }

  virtual ~CExternalFilesJason2GDRGDR() { };

  static string TypeOf() { return CTools::Format("%s/%s/GDR", m_missionName.c_str(), GDR.c_str()); };
  virtual string GetType() { return TypeOf(); };
};








//-------------------------------------------------------------
//------------------- CExternalFilesJason2SGDR class --------------------
//-------------------------------------------------------------

class CExternalFilesJason2SGDR : public CExternalFilesJason2
{
public:
  CExternalFilesJason2SGDR (const string &name = "");

  virtual ~CExternalFilesJason2SGDR() { };

  static string TypeOf() { return CTools::Format("%s/%s", m_missionName.c_str(), SGDR.c_str()); };
  virtual string GetType() { return TypeOf(); };
  

protected:



};


//-------------------------------------------------------------
//------------------- CExternalFilesJason2SGDRIGDR class --------------------
//-------------------------------------------------------------

class CExternalFilesJason2SGDRIGDR : public CExternalFilesJason2SGDR
{
public:
  CExternalFilesJason2SGDRIGDR
		(const string		&name = "")
	: CExternalFilesJason2SGDR(name)
  {
  }
  virtual ~CExternalFilesJason2SGDRIGDR() { };

  static string TypeOf() { return CTools::Format("%s/%s/IGDR", m_missionName.c_str(), SGDR.c_str()); };
  virtual string GetType() { return TypeOf(); };
};

//-------------------------------------------------------------
//------------------- CExternalFilesJason2SGDRGDR class --------------------
//-------------------------------------------------------------

class CExternalFilesJason2SGDRGDR : public CExternalFilesJason2SGDR
{
public:
  CExternalFilesJason2SGDRGDR
		(const string		&name = "")
	: CExternalFilesJason2SGDR(name)
  {
  }
  virtual ~CExternalFilesJason2SGDRGDR() { };

  static string TypeOf() { return CTools::Format("%s/%s/GDR", m_missionName.c_str(), SGDR.c_str()); };
  virtual string GetType() { return TypeOf(); };
};





/** @} */

}

#endif // Already included .h


