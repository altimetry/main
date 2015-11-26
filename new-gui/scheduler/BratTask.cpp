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

#include "../Common/QtFileUtils.h"

#include "BratTask.h"
//#include "SchedulerTaskConfig.h"

#include "new-gui/Common/ScheduledTasksList.hxx"
#include "../Common/XmlSerializer.h"            // use after generated ScheduledTasksList.hxx


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#if defined (__unix__)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif


#include "new-gui/Common/ScheduledTasksList.cxx"


#if defined (__unix__)
#pragma GCC diagnostic warning "-Wdeprecated-declarations"
#endif

///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////


// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "libbrathl/Win32MemLeaksAccurate.h"


using namespace brathl;


const std::string CBratTaskFunction::m_TASK_FUNC_COPYFILE = "CopyFile";

//const QString CBratTask::FormatISODateTime = "%Y-%m-%d %H:%M:%S";

//const std::string CBratTask::m_BRAT_STATUS_ENDED_LABEL = "ended";
//const std::string CBratTask::m_BRAT_STATUS_ERROR_LABEL = "error";
//const std::string CBratTask::m_BRAT_STATUS_PENDING_LABEL = "pending";
//const std::string CBratTask::m_BRAT_STATUS_PROCESSING_LABEL = "in progress";
//const std::string CBratTask::m_BRAT_STATUS_WARNING_LABEL = "warning";

//-------------------------------------------------------------
//------------------- CBratTaskFunction class --------------------
//-------------------------------------------------------------
void CBratTaskFunction::Execute()
{
	try
	{
		m_call( m_params );
	}
	catch ( CException& e )
	{
		std::string msg = "Unable to execute function " + m_name + " - Native error: " + e.GetMessage();
		throw CException( msg, BRATHL_ERROR );
	}
	catch ( std::exception& e )
	{
		std::string msg = "Unable to execute function " + m_name + " - Native error: " + e.what();
		throw CException( msg, BRATHL_ERROR );

	}
	catch ( ... )
	{
		std::string msg = "Unable to execute function " + m_name + " - Native error is unknown";
		throw CException( msg, BRATHL_ERROR );
	}
}
//----------------------------------------
void CBratTaskFunction::CopyFile( CVectorBratAlgorithmParam& arg )
{
	if ( arg.size() != 2 )
	{
		std::string msg = "CBratTaskFunction::CopyFile - Unable to execute  Brat task - Expected number of parameters is 2, but found "
			+ n2s< std::string >( static_cast<int32_t>( arg.size() ) );

		throw CException( msg, BRATHL_ERROR );
	}

	std::string p1 = arg.at( 0 ).GetValueAsString().c_str();
	std::string p2 = arg.at( 1 ).GetValueAsString().c_str();

	bool bOk = copyFile( p1, p2 );		//femm bool bOk = wxCopyFile( p1, p2 );

	if ( !bOk )
	{
		std::string msg = "Unable to copy file " + p1 + " to " + p2;
		throw CException( msg, BRATHL_ERROR );
	}
}
//----------------------------------------
void CBratTaskFunction::Dump( std::ostream& fOut /*= std::cerr*/ )
{
	// femm: comment in, when trace and log modules decoupled from all externals stuff
	//
	//if (! CTrace::IsTrace())
	//{
	//  return;
	//}

	fOut << "==> Dump a CBratTaskFunction Object at " << this << std::endl;
	fOut << "m_name: " << m_name << std::endl;
	fOut << "m_call: " << m_call << std::endl;
	fOut << "m_params: " << std::endl;
	m_params.Dump( fOut );
	fOut << "==> END Dump a CBratTaskFunction Object at " << this << std::endl;

}//----------------------------------------


//-------------------------------------------------------------
//------------------- CMapBratTaskFunction class --------------------
//-------------------------------------------------------------

CMapBratTaskFunction::CMapBratTaskFunction(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;
  Init();
}


//----------------------------------------
CMapBratTaskFunction::~CMapBratTaskFunction()
{
  RemoveAll();
}
//----------------------------------------
void CMapBratTaskFunction::Init()
{
  this->Insert(CBratTaskFunction::m_TASK_FUNC_COPYFILE, new CBratTaskFunction(CBratTaskFunction::m_TASK_FUNC_COPYFILE, CBratTaskFunction::CopyFile)); 
}

//----------------------------------------
CMapBratTaskFunction& CMapBratTaskFunction::GetInstance()
{
 static CMapBratTaskFunction instance;

 return instance;
}
//----------------------------------------
CBratTaskFunction* CMapBratTaskFunction::Insert(const std::string& key, CBratTaskFunction* ob, bool withExcept /* = true */)
{
  

  std::pair <CMapBratTaskFunction::iterator,bool> pairInsert;


  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object
  pairInsert = (mapbrattaskfunction::insert(CMapBratTaskFunction::value_type(key, ob)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in CMapBratTaskFunction::Insert - try to insert an task that already exists. Check that no task have the same id", BRATHL_INCONSISTENCY_ERROR);
	// femm: comment in, when trace and log modules decoupled from all externals stuff
	//
    //Dump(*CTrace::GetDumpContext());

    throw(e);
  }
  
  CMapBratTaskFunction::iterator it = (pairInsert.first);
  return (*it).second;;

}
//----------------------------------------
CBratTaskFunction* CMapBratTaskFunction::Find(const std::string& id) const
{
  CMapBratTaskFunction::const_iterator it = mapbrattaskfunction::find(id);
  if (it == end())
  {
    return NULL;
  }
  else
  {
    return (*it).second;
  }

}

//----------------------------------------
bool CMapBratTaskFunction::Remove(const std::string& id)
{
  CMapBratTaskFunction::iterator it = mapbrattaskfunction::find(id);
  
  return Remove(it);
}
//----------------------------------------
bool CMapBratTaskFunction::Remove(CMapBratTaskFunction::iterator it)
{
  if (it == end())
  {
    return false;
  }

  if (m_bDelete)
  {
    CBratTaskFunction* ob = it->second;  
    if (ob != NULL)
    {
      delete  ob;
    }
  }

  mapbrattaskfunction::erase(it);

  return true;
}
//----------------------------------------
void CMapBratTaskFunction::RemoveAll()
{
  
  CMapBratTaskFunction::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      CBratTaskFunction* ob = it->second;
      if (ob != NULL)
      {
        delete  ob;
      }
    }
  }

  mapbrattaskfunction::clear();

}

//----------------------------------------
void CMapBratTaskFunction::Dump(std::ostream& fOut /* = std::cerr */)
{

	// femm: comment in, when trace and log modules decoupled from all externals stuff
	//
   //if (CTrace::IsTrace() == false)
   //{ 
   //   return;
   //}

   fOut << "==> Dump a CMapBratTaskFunction Object at "<< this << " with " <<  size() << " elements" << std::endl;

   CMapBratTaskFunction::iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      CBratTaskFunction *ob = it->second;
      fOut << "CMapBratTaskFunction Key is = " << (*it).first << std::endl;
      fOut << "CMapBratTaskFunction Value is = " << std::endl;
      ob->Dump(fOut);
   }

   fOut << "==> END Dump a CMapBratTaskFunction Object at "<< this << " with " <<  size() << " elements" << std::endl;

}


//-------------------------------------------------------------
//------------------- CBratTask class --------------------
//-------------------------------------------------------------

	// serialization


//__cdecl brathl::CException::CException(class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const &,int)" (??0CException@brathl@@QEAA@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z) referenced in function "public: static void __cdecl CBratTaskFunction::CopyFileW(class brathl::CVectorBratAlgorithmParam &)" (?CopyFileW@CBratTaskFunction@@SAXAEAVCVectorBratAlgorithmParam@brathl@@@Z)
//__cdecl brathl::CException::~CException(void)" (??1CException@brathl@@UEAA@XZ) referenced in function "public: virtual class CBratTaskFunction * __cdecl CMapBratTaskFunction::Insert(class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const &,class CBratTaskFunction *,bool)" (?Insert@CMapBratTaskFunction@@UEAAPEAVCBratTaskFunction@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PEAV2@_N@Z)
//__cdecl brathl::CException::Dump(class std::basic_ostream<char,struct std::char_traits<char> > &)" (?Dump@CException@brathl@@UEAAXAEAV?$basic_ostream@DU?$char_traits@D@std@@@std@@@Z)
//__cdecl brathl::CException::what(void)const " (?what@CException@brathl@@UEBAPEBDXZ)
//__cdecl brathl::CVectorBratAlgorithmParam::CVectorBratAlgorithmParam(void)" (??0CVectorBratAlgorithmParam@brathl@@QEAA@XZ) referenced in function "public: __cdecl CBratTaskFunction::CBratTaskFunction(class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const &,void (__cdecl*)(class brathl::CVectorBratAlgorithmParam &))" (??0CBratTaskFunction@@QEAA@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@P6AXAEAVCVectorBratAlgorithmParam@brathl@@@Z@Z)
//__cdecl brathl::CVectorBratAlgorithmParam::CVectorBratAlgorithmParam(void)" (??0CVectorBratAlgorithmParam@brathl@@QEAA@XZ)
//__cdecl brathl::CVectorBratAlgorithmParam::~CVectorBratAlgorithmParam(void)" (??1CVectorBratAlgorithmParam@brathl@@UEAA@XZ) referenced in function "int `public: __cdecl CBratTaskFunction::CBratTaskFunction(class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const &,void (__cdecl*)(class brathl::CVectorBratAlgorithmParam &))'::`1'::dtor$1" (?dtor$1@?0???0CBratTaskFunction@@QEAA@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@P6AXAEAVCVectorBratAlgorithmParam@brathl@@@Z@Z@4HA)
//__cdecl brathl::CVectorBratAlgorithmParam::~CVectorBratAlgorithmParam(void)" (??1CVectorBratAlgorithmParam@brathl@@UEAA@XZ)
//__cdecl brathl::CVectorBratAlgorithmParam::Insert(class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const &)" (?Insert@CVectorBratAlgorithmParam@brathl@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z) referenced in function "public: __cdecl CBratTask::CBratTask(class task const &)" (??

CBratTask::CBratTask( const task &oxml )				//USED ON LOAD
{
	m_uid = oxml.uid();
	m_name = oxml.name();
	m_cmd = fromOpt( oxml.cmd(), std::string() );

	CBratTaskFunction* functionRef = CMapBratTaskFunction::GetInstance().Find( fromOpt( oxml.function(), std::string() ) );
	SetBratTaskFunction( functionRef );
	if ( oxml.task1().present() )										//femm!!! NOT CLEAR so far: there can only be one child t the same level????????
		GetSubordinateTasks()->Insert( new CBratTask( *oxml.task1() ) );

	//wxXmlNode* argNode = taskNode->GetChildren();
	CVectorBratAlgorithmParam* params = GetBratTaskFunction()->GetParams();
	auto const &args = oxml.arg();
	for ( auto ii = args.begin(); ii != args.end(); ++ii )
	{
		params->Insert( *ii );
	}

	//while ( argNode != NULL )
	//{
	//	if ( argNode->GetName().CmpNoCase( CSchedulerTaskConfig::m_ARG_ELT ) == 0 )
	//	{
	//		wxXmlNode* argTextNode = argNode->GetChildren();
	//		if ( argTextNode != NULL )
	//		{
	//			params->Insert( argTextNode->GetContent().ToStdString() );
	//		}
	//	}
	//	argNode = argNode->GetNext();
	//}

	SetAt( oxml.at() );
	m_status = enum_cast<Status>( oxml.status() );
	m_logFile = oxml.logFile();
}

task& CBratTask::IOcopy( task &oxml ) const			//USED ON STORE
{
	oxml.uid( m_uid );
	oxml.name( m_name );
	oxml.cmd( m_cmd );
	oxml.at( GetAtAsString() );
	oxml.status( enum_reverse_cast< task::status_type >( m_status ) );
	oxml.logFile( m_logFile );

	return oxml;
}


void CBratTask::Dump( std::ostream& fOut )	//fOut = std::cerr
{
	// femm !!! : comment in, when trace and log modules decoupled from all externals stuff
	//
	//if (! CTrace::IsTrace())
	//{
	//  return;
	//}

	fOut << "==> Dump a CBratTask Object at " << this << std::endl;

	fOut << "m_uid: " << m_uid << std::endl;
	fOut << "m_at: " << GetAtAsString() << std::endl;
	fOut << "m_status: " << m_status << std::endl;
	fOut << "m_cmd: " << m_cmd << std::endl;
	fOut << "m_function: " << std::endl;
	m_function.Dump( fOut );
	fOut << "m_subordinateTasks: " << std::endl;
	m_subordinateTasks.Dump( fOut );

	fOut << "==> END Dump a CBratTask Object at " << this << std::endl;
}

//-------------------------------------------------------------
//------------------- CMapBratTask class --------------------
//-------------------------------------------------------------

CMapBratTask::CMapBratTask(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;
  Init();

}


//----------------------------------------
CMapBratTask::~CMapBratTask()
{
  RemoveAll();
}

//----------------------------------------
void CMapBratTask::Insert(const CMapBratTask* map, bool bRemoveAll /* = true */)
{
  if (map == NULL)
  {
    return;
  }

  if (bRemoveAll)
  {
    this->RemoveAll();
  }


  CMapBratTask::const_iterator it;

  for (it = map->begin() ; it != map->end() ; it++)
  {
    CBratTask* ob = (it->second)->Clone();
    Insert(it->first, ob);
  }

}

//----------------------------------------
CBratTask* CMapBratTask::Insert(uid_t key, CBratTask* ob, bool withExcept /* = true */)
{
  

  std::pair <CMapBratTask::iterator,bool> pairInsert;


  //If 'key' already exists --> pairInsert.second == false and
  // pairInsert.first then contains an iterator on the existing object
  // If 'key' does not exist --> pairInsert.second == true and
  // pairInsert.first then contains a iterator on the inserted object
  pairInsert = (mapbrattask::insert(CMapBratTask::value_type(key, ob)));

  if( (pairInsert.second == false) && (withExcept))
  {
    CException e("ERROR in CMapBratTask::Insert - try to insert an task that already exists. Check that no task have the same id", BRATHL_INCONSISTENCY_ERROR);
	// femm: comment in, when trace and log modules decoupled from all externals stuff
	//
    //Dump(*CTrace::GetDumpContext());

    throw(e);
  }
  
  CMapBratTask::iterator it = (pairInsert.first);
  return (*it).second;;

}
//----------------------------------------
void CMapBratTask::Init()
{
}

//----------------------------------------
CBratTask* CMapBratTask::Find(uid_t id) const
{
  CMapBratTask::const_iterator it = mapbrattask::find(id);
  if (it == end())
  {
    return NULL;
  }
  else
  {
    return (*it).second;
  }

}

//----------------------------------------
bool CMapBratTask::Remove(uid_t id)
{
  CMapBratTask::iterator it = mapbrattask::find(id);
  
  return Remove(it);
}
//----------------------------------------
bool CMapBratTask::Remove(CMapBratTask::iterator it)
{
  if (it == end())
  {
    return false;
  }

  if (m_bDelete)
  {
    CBratTask* ob = it->second;  
    if (ob != NULL)
    {
      delete  ob;
    }
  }

  mapbrattask::erase(it);

  return true;
}
//----------------------------------------
void CMapBratTask::RemoveAll()
{
  
  CMapBratTask::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      CBratTask* ob = it->second;
      if (ob != NULL)
      {
        delete  ob;
      }
    }
  }

  mapbrattask::clear();

}
//----------------------------------------
void CMapBratTask::Dump(std::ostream& fOut /* = std::cerr */)
{

	// femm: comment in, when trace and log modules decoupled from all externals stuff
	//
   //if (CTrace::IsTrace() == false)
   //{ 
   //   return;
   //}

   fOut << "==> Dump a CMapBratTask Object at "<< this << " with " <<  size() << " elements" << std::endl;

   CMapBratTask::iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      CBratTask *ob = it->second;
      fOut << "CMapBratTask Key is = " << (*it).first << std::endl;
      fOut << "CMapBratTask Value is = " << std::endl;
      ob->Dump(fOut);
   }

   fOut << "==> END Dump a CMapBratTask Object at "<< this << " with " <<  size() << " elements" << std::endl;

}

//-------------------------------------------------------------
//------------------- CVectorBratTask class --------------------
//-------------------------------------------------------------

CVectorBratTask::CVectorBratTask(bool bDelete /*= true*/)
{
  m_bDelete = bDelete;
  Init();

}


//----------------------------------------
CVectorBratTask::~CVectorBratTask()
{
  RemoveAll();
}

//----------------------------------------
void CVectorBratTask::Init()
{
}
//----------------------------------------
void CVectorBratTask::Insert(CBratTask* ob, bool bEnd /* = true */)
{
  if (bEnd)
  {  
    vectorbrattask::push_back(ob);   
  }
  else
  {  
    CVectorBratTask::InsertAt(this->begin(), ob);   
  }

}
//----------------------------------------
void CVectorBratTask::Insert(const CVectorBratTask* vec, bool bRemoveAll /* = true */, bool bEnd /* = true */)
{
  if (vec == NULL)
  {
    return;
  }

  if (bRemoveAll)
  {
    this->RemoveAll();
  }


  CVectorBratTask::const_iterator it;

  for (it = vec->begin() ; it != vec->end() ; it++)
  {
    CBratTask* ob = (*it)->Clone();
    Insert(ob, bEnd);
  }

}

//----------------------------------------
CVectorBratTask::iterator CVectorBratTask::InsertAt(CVectorBratTask::iterator where, CBratTask* ob)
{ 
  return vectorbrattask::insert(where, ob);     
}


//----------------------------------------
void CVectorBratTask::RemoveAll()
{
  
  CVectorBratTask::iterator it;

  if (m_bDelete)
  {
    for (it = begin() ; it != end() ; it++)
    {
      CBratTask* ob = *it;
      if (ob != NULL)
      {
        delete  ob;
      }
    }
  }

  vectorbrattask::clear();

}
//----------------------------------------
void CVectorBratTask::Dump(std::ostream& fOut /* = std::cerr */)
{


	// femm: comment in, when trace and log modules decoupled from all externals stuff
	//
 //if (CTrace::IsTrace() == false)
 // {
 //   return;
 // }

  CVectorBratTask::const_iterator it;
  int i = 0;

  fOut << "==> Dump a CVectorBratTask Object at "<< this << " with " <<  size() << " elements" << std::endl;

  for ( it = this->begin( ); it != this->end( ); it++ )
  {
    fOut << "CVectorBratTask[" << i << "]= " << std::endl;  
    if ((*it) != NULL)
    {
     (*it)->Dump(fOut);
    }
    i++;
  }

  fOut << "==> END Dump a CVectorBratTask Object at "<< this << " with " <<  size() << " elements" << std::endl;


  fOut << std::endl;

}
