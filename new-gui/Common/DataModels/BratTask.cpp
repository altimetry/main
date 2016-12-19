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

#include "common/QtUtilsIO.h"
#include "common/tools/Trace.h"

#include "BratTask.h"
//#include "SchedulerTaskConfig.h"

#include "new-gui/Common/ScheduledTasksList.hxx"
#include "new-gui/Common/XmlSerializer.h"            // use after generated ScheduledTasksList.hxx


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////


using namespace brathl;


//	IMPORTANT: this identifier is referenced in xsd, which takes it as an enumerator value
//
//static
const std::string CBratTaskFunction::sm_TASK_FUNC_COPYFILE = "BratTaskFunctionCopyFile";



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
		std::string msg = "Unable to execute function " + m_name + " - Native error: " + e.Message();
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
void CBratTaskFunction::BratTaskFunctionCopyFile( CVectorBratAlgorithmParam& arg )
{
	if ( arg.size() != 2 )
	{
		std::string msg = "CBratTaskFunction::CopyFile - Unable to execute  Brat task - Expected number of parameters is 2, but found "
			+ n2s< std::string >( static_cast<int32_t>( arg.size() ) );

		throw CException( msg, BRATHL_ERROR );
	}

	std::string p1 = arg.at( 0 ).GetValueAsString().c_str();
	std::string p2 = arg.at( 1 ).GetValueAsString().c_str();

	bool bOk = DuplicateFile( p1, p2 );		//femm bool bOk = wxCopyFile( p1, p2 );

	if ( !bOk )
	{
		std::string msg = "Unable to copy file " + p1 + " to " + p2;
		throw CException( msg, BRATHL_ERROR );
	}
}
//----------------------------------------
void CBratTaskFunction::Dump( std::ostream& fOut /*= std::cerr*/ )
{
	if ( !CTrace::IsTrace() )
	{
	  return;
	}

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
  this->Insert(CBratTaskFunction::sm_TASK_FUNC_COPYFILE, new CBratTaskFunction(CBratTaskFunction::sm_TASK_FUNC_COPYFILE, CBratTaskFunction::BratTaskFunctionCopyFile)); 
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
   if (CTrace::IsTrace() == false)
   { 
      return;
   }

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

CBratTask::CBratTask( const task &oxml )				//USED ON LOAD
{
	//1
	m_uid = oxml.uid();									
	//2
	m_name = oxml.name();
	//3
	m_cmd = fromOpt( oxml.cmd(), std::string() );
	//4
	CBratTaskFunction* functionRef = CMapBratTaskFunction::GetInstance().Find( fromOpt( oxml.function(), std::string() ) );
	SetBratTaskFunction( functionRef );
	CVectorBratAlgorithmParam *params = GetBratTaskFunction()->GetParams();
	auto const &args = oxml.arg();
	for ( auto ii = args.begin(); ii != args.end(); ++ii )
	{
		params->Insert( *ii );
	}

    //assert__( ( m_cmd.empty() && !args.empty() ) || ( !m_cmd.empty() && args.empty() ) );		//Not sure this can be asserted: TODO: delete when there is a full understanding

	//5
	SetAt( oxml.at() );
	//6
	m_status = enum_cast<EStatus>( oxml.status() );
	//7
	m_logFile = oxml.logFile();

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

	//8
	if ( oxml.task1().present() )									//TODO: NOT CLEAR so far: there can only be one child at the same level?...
		GetSubordinateTasks()->Insert( new CBratTask( *oxml.task1() ) );
}

task& CBratTask::IOcopy( task &oxml ) const			//USED ON STORE
{
	//1
	oxml.uid( m_uid );
	//2
	oxml.name( m_name );
	//3
	if ( !m_cmd.empty() )
		oxml.cmd( m_cmd );
	else 
	//4
	{
		oxml.function( GetBratTaskFunction()->GetName() );
		const CVectorBratAlgorithmParam *params = GetBratTaskFunction()->GetParams();
		task::arg_sequence args;
		for ( auto ii = params->begin(); ii != params->end(); ++ii )
		{
			task::arg_type arg( ii->GetValueAsString(), ii->GetTypeValAsString() );
			args.push_back( arg );
		}
		oxml.arg( args );
	}
	//5
	oxml.at( GetAtAsString() );
	//6
	oxml.status( enum_reverse_cast< task::status_type >( m_status ) );
	//7
	oxml.logFile( m_logFile );
	//8
	for ( auto ii = m_subordinateTasks.begin(); ii != m_subordinateTasks.end(); ++ii )
	{
		task child;
		(*ii)->IOcopy( child );
		oxml.task1().set( child );
	}
	return oxml;
}


void CBratTask::Dump( std::ostream& fOut )	//fOut = std::cerr
{
	if ( !CTrace::IsTrace() )
	  return;

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

//----------------------------------------
void CMapBratTask::Dump(std::ostream& fOut /* = std::cerr */)
{
   if ( !CTrace::IsTrace() )
      return;

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

//---------------------------------------- v4: new
bool CVectorBratTask::operator == ( const CVectorBratTask &o ) const
{
	const size_t size = base_t::size();

	if ( size != o.size() )
		return false;

	for ( size_t i = 0; i < size; ++i )
	{
		if ( *(at( i )) != *(o[ i ]) )
			return false;
	}

	return true;
}
//----------------------------------------
void CVectorBratTask::Insert( const CVectorBratTask* vec, bool bRemoveAll /* = true */ )
{
	if ( !vec )
		return;

	if ( bRemoveAll )
		RemoveAll();

	for ( CVectorBratTask::const_iterator it = vec->begin(); it != vec->end(); it++ )
	{
		CBratTask* ob = ( *it )->Clone();
		Insert( ob );
	}
}

//----------------------------------------
void CVectorBratTask::RemoveAll()
{
	if ( m_bDelete )
		for ( CVectorBratTask::iterator it = begin(); it != end(); it++ )
			delete  *it;

	clear();
}

//----------------------------------------
void CVectorBratTask::Dump( std::ostream& fOut /* = std::cerr */ )
{
	if ( !CTrace::IsTrace() )
		return;

	fOut << "==> Dump a CVectorBratTask Object at " << this << " with " << size() << " elements" << std::endl;

	int i = 0;
	for ( CVectorBratTask::const_iterator it = this->begin(); it != this->end(); it++ )
	{
		fOut << "CVectorBratTask[" << i << "]= " << std::endl;
		if ( ( *it ) != NULL )
		{
			( *it )->Dump( fOut );
		}
		i++;
	}

	fOut << "==> END Dump a CVectorBratTask Object at " << this << " with " << size() << " elements" << std::endl;

	fOut << std::endl;
}



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

