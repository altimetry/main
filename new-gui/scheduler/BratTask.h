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

#ifndef __BRAT_TASK_H__
#define __BRAT_TASK_H__

#include <string>

#include <QDateTime>

#include "../Common/QtStringUtils.h"

#include "libbrathl/brathl.h" 
#include "libbrathl/BratAlgorithmBase.h" 


using namespace brathl;


class CBratTask;



//-------------------------------------------------------------
//------------------- CVectorBratTask class --------------------
//-------------------------------------------------------------

typedef std::vector<CBratTask*> vectorbrattask; 

class CVectorBratTask : public vectorbrattask
{
public:
  CVectorBratTask(bool bDelete = true);  
  virtual ~CVectorBratTask();

  virtual void Insert(CBratTask* ob, bool bEnd = true);
  virtual void Insert(const CVectorBratTask* vec, bool bRemoveAll = true,  bool bEnd = true);
  virtual CVectorBratTask::iterator InsertAt(CVectorBratTask::iterator where, CBratTask* ob);

  virtual void RemoveAll();

  virtual void Dump(std::ostream& fOut = std::cerr); 

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

protected:

  void Init();

protected:

  bool m_bDelete;



};

//-------------------------------------------------------------
//------------------- CBratTaskFunction class --------------------
//-------------------------------------------------------------

typedef void (*BratTaskFunctionCallableN)(CVectorBratAlgorithmParam& arg);

class CBratTaskFunction
{
public:
	static const std::string m_TASK_FUNC_COPYFILE;

protected:
	std::string m_name;
    BratTaskFunctionCallableN m_call = nullptr;
	CVectorBratAlgorithmParam m_params;

public:
	CBratTaskFunction()
	{}
    CBratTaskFunction( const std::string& name, BratTaskFunctionCallableN call ) :
		m_name( name ), m_call( call )
	{}
	CBratTaskFunction( const CBratTaskFunction& o )
	{
		*this = o;
	}
	const CBratTaskFunction& operator=( const CBratTaskFunction& o )
	{
		if ( this == &o )
		{
			m_name = o.m_name;
			m_call = o.m_call;
		}
		return *this;
	}

	virtual ~CBratTaskFunction()
	{}

    const std::string& GetName() const { return m_name; }
    void SetName( const std::string& value ) { m_name = value; }

    BratTaskFunctionCallableN GetCall() { return m_call; }
    void SetCall( BratTaskFunctionCallableN value ) { m_call = value; }

    const CVectorBratAlgorithmParam* GetParams() const { return &m_params; }
    CVectorBratAlgorithmParam* GetParams() { return &m_params; }
	void GetParamsAsString( std::string& value ) const
	{
	  value = m_params.ToString().c_str();
	}

	void Execute();

	virtual void Dump( std::ostream& fOut = std::cerr );

	static void CopyFile( CVectorBratAlgorithmParam& arg );
};


//-------------------------------------------------------------
//------------------- CMapBratTaskFunction class --------------------
//-------------------------------------------------------------
typedef std::map<std::string, CBratTaskFunction*> mapbrattaskfunction; 

class CMapBratTaskFunction: public mapbrattaskfunction
{
protected:
  CMapBratTaskFunction(bool bDelete = true);

public:

  virtual ~CMapBratTaskFunction();

  static CMapBratTaskFunction&  GetInstance();

  virtual CBratTaskFunction* Insert(const std::string& key, CBratTaskFunction* ob, bool withExcept = true);

  virtual bool Remove(const std::string& id);
  virtual bool Remove(CMapBratTaskFunction::iterator it);

  virtual void RemoveAll();

  virtual void Dump(std::ostream& fOut = std::cerr); 

  virtual CBratTaskFunction* Find(const std::string& id) const;

  bool GetDelete() {return m_bDelete;}
  void SetDelete(bool value) {m_bDelete = value;}

protected:

  void Init();

protected:

  bool m_bDelete;



};
//-------------------------------------------------------------
//------------------- CBratTask class --------------------
//-------------------------------------------------------------


class task;


class CBratTask
{
	//types

public:

	typedef long long uid_t;

	enum Status
	{
		e_BRAT_STATUS_PENDING,
		e_BRAT_STATUS_PROCESSING,
		e_BRAT_STATUS_ENDED,
		e_BRAT_STATUS_ERROR,
		e_BRAT_STATUS_WARNING,

		e_Status_size
	};

	//static members (functions and data)

protected:
	static const std::string* statusNames()
	{
		static const std::string labels[ e_Status_size ] =
		{
			"pending",
			"in progress",
			"ended",
			"error",
			"warning"
		};

		static_assert( ARRAY_SIZE( labels ) == e_Status_size, "CBratTask: Number of status labels and values does not match." );

		return labels;
	}


public:
	static const std::string& TaskStatusToString( Status s )
	{
		assert__( s < e_Status_size );

		return statusNames()[ s ];
	}

	static Status StringToTaskStatus( const std::string& status )
	{
		static const std::string *labels = statusNames();

		auto it = std::find( labels, &labels[ e_Status_size ], status );
		if ( it != &labels[ e_Status_size ] )
			return static_cast<Status>( it - labels );

		throw CException( "CBratTask::StringToTaskStatus: unknown status label " + status, BRATHL_INCONSISTENCY_ERROR );
	}


public:
	//static const std::string m_BRAT_STATUS_ENDED_LABEL;
	//static const std::string m_BRAT_STATUS_ERROR_LABEL;
	//static const std::string m_BRAT_STATUS_PENDING_LABEL;
	//static const std::string m_BRAT_STATUS_PROCESSING_LABEL;
	//static const std::string m_BRAT_STATUS_WARNING_LABEL;

	// datetime in ISO 8601 format
	static const std::string& formatISODateTime()
	{
		static const std::string FormatISODateTime = "%Y-%m-%d %H:%M:%S";
		return FormatISODateTime;
	}

protected:
	static const QString& qformatISODateTime()
	{
		static const QString FormatISODateTime = "yyyy-MM-dd hh:mm:ss";
		return FormatISODateTime;
	}

	//instance data members
	
protected:
	uid_t m_uid = -1;
	std::string m_name;
	std::string m_cmd;
	CBratTaskFunction m_function;
	QDateTime m_at;											//typedef wxDateTime QDateTime;
	Status m_status = CBratTask::e_BRAT_STATUS_PENDING;
	std::string m_logFile;

	CVectorBratTask m_subordinateTasks;

public:
	// construction / destruction

	CBratTask()
	{}

	CBratTask( const CBratTask &o )
	{
		*this = o;
	}

	const CBratTask& operator=( const CBratTask &o )
	{
	  if (this != &o)
	  {
		  m_uid = o.m_uid;
		  m_name = o.m_name;
		  m_cmd = o.m_cmd;
		  m_at = o.m_at;
		  m_status = o.m_status;
		  m_function = o.m_function;
		  m_logFile = o.m_logFile;

		  m_subordinateTasks.Insert(&o.m_subordinateTasks);		//CHECK THIS !!! Apparently only called here
	  }
	  return *this;
	}

	virtual CBratTask* Clone() { return new CBratTask( *this ); }

	virtual ~CBratTask()
	{}

	// serialization

	CBratTask( const task &oxml );				//USED ON LOAD

	task& IOcopy( task &oxml ) const;			//USED ON STORE

	// equality

	bool operator == ( const CBratTask &o ) const 
	{
		return 
			m_uid == o.m_uid &&
			m_name == o.m_name &&
			m_cmd == o.m_cmd &&
			//m_function;
			m_at == o.m_at &&
			m_status == o.m_status &&
			m_logFile == o.m_logFile
			;
	}

	bool operator != ( const CBratTask &o ) const
	{
		return !( *this == o );
	}

	// getters / setters

	const uid_t GetUid() const { return m_uid; }									//uid_t GetUidValue() const { return m_uid; }
	std::string GetUidAsString() const { return n2s<std::string>( m_uid ); }
	void SetUid( const std::string& value )
	{
		m_uid = s2n<uid_t>( value );	//femm m_uid = wxBratTools::wxStringTowxLongLong_t(value);
	}
	void SetUid( uid_t value ) { m_uid = value; }

	//void SetId(const std::string& value) {m_id = value;};
	//const std::string& GetId() {return m_id;};

	const std::string& GetName() const { return m_name; }
	void SetName( const std::string& value ) { m_name = value; }

	const QDateTime& GetAt() const { return m_at; }
	std::string GetAtAsString( /*const std::string& format = formatISODateTime() femm*/) const 
	{ 
		QString s = m_at.toString( qformatISODateTime() );
		
		// "If the format is Qt::ISODate, the string format corresponds to the ISO 8601 extended 
		//	specification for representations of dates and times, taking the form YYYY-MM-DDTHH:mm:ss[Z|[+|-]HH:mm], 
		//	depending on the timeSpec() of the QDateTime. If the timeSpec() is Qt::UTC, Z will 
		//	be appended to the string; if the timeSpec() is Qt::OffsetFromUTC the offset in hours 
		//	and minutes from UTC will be appended to the string."

		return q2t<std::string>( s );
	}
	//void SetAt( const QDateTime& value ) { m_at = value; }
	void SetAt( const std::string &value )
	{ 
		QDateTime dt = QDateTime::fromString( t2q( value ), qformatISODateTime() );
		assert__( dt.isValid() );															 	//!!! TODO !!!: error handling
		m_at = dt; 
	}
	bool laterThanNow() const { return GetAt() > QDateTime::currentDateTime(); }

	const std::string& GetCmd() const { return m_cmd; }
	void SetCmd( const std::string &value ) { m_cmd = value; }

	Status GetStatus() const { return m_status; }
	std::string GetStatusAsString() const { return TaskStatusToString( m_status ); }
	void SetStatus( const std::string& value ) { m_status = CBratTask::StringToTaskStatus( value ); }
	void SetStatus( Status value ) { m_status = value; }

	const std::string& GetLogFile() const { return m_logFile; }				//std::string GetLogFileAsString() { return m_logFile.GetFullPath(); }	
	void SetLogFile( const std::string& value ) { m_logFile = value; }		//void SetLogFile( const std::string& value ) { m_logFile.Assign( value ); }

	CVectorBratTask* GetSubordinateTasks() { return &m_subordinateTasks; }
	bool HasSubordinateTasks() { return ( m_subordinateTasks.size() > 0 ); }

	const CBratTaskFunction* GetBratTaskFunction() const { return &m_function; }
	CBratTaskFunction* GetBratTaskFunction() { return &m_function; }
	void SetBratTaskFunction( const CBratTaskFunction& value ) { m_function = value; }
	void SetBratTaskFunction( CBratTaskFunction* value )
	{
		if ( value != NULL )
		{
			m_function = *value;
		}
	};

	// testers

	bool HasCmd() { return !( m_cmd.empty() ); }
	bool HasFunction() { return !( m_function.GetName().empty() ); }

	// remaining data members

	void ExecuteFunction()
	{
		m_function.Execute();
	}

	//femm
	//static std::string TaskStatusToString( Status status );
	//static Status StringToTaskStatus( const std::string& status );

	// Dump function
	virtual void Dump( std::ostream& fOut = std::cerr );
};



//-------------------------------------------------------------
//------------------- CMapBratTask class --------------------
//-------------------------------------------------------------
typedef std::map< CBratTask::uid_t, CBratTask* > mapbrattask; 


class CMapBratTask : public mapbrattask
{
	typedef CBratTask::uid_t uid_t;

	bool m_bDelete;

public:
	CMapBratTask( bool bDelete = true );
	virtual ~CMapBratTask();

	virtual CBratTask* Insert( uid_t key, CBratTask* ob, bool withExcept = true );
	virtual void Insert( const CMapBratTask* map, bool bRemoveAll = true );

	virtual bool Remove( uid_t id );
	virtual bool Remove( CMapBratTask::iterator it );


	virtual void RemoveAll();

	virtual void Dump( std::ostream& fOut = std::cerr );

	virtual CBratTask* Find( uid_t id ) const;

    bool GetDelete() const { return m_bDelete; }
    void SetDelete( bool value ) { m_bDelete = value; }


protected:

	void Init();
};


#endif		// __BRAT_TASK_H__
