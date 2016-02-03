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


#include "new-gui/Common/tools/Trace.h"
#include "Field.h"
#include "Expression.h"
#include "Product.h"
#include "Aliases.h"
using namespace brathl;

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

namespace brathl
{

//-------------------------------------------------------------
//------------------- CDefaultRecord class --------------------
//-------------------------------------------------------------

CDefaultRecord::CDefaultRecord()
{
  Init();
}


//----------------------------------------
CDefaultRecord::~CDefaultRecord()
{
}

//----------------------------------------
void CDefaultRecord::Init()
{
}
//----------------------------------------------------
void CDefaultRecord::GetRecordByProductType(CObMap& recordByProductType)
{
  recordByProductType.SetDelete(false);

  if (this->IsValidForAllProductType())
  {
    try
    {
      recordByProductType.Insert(CAliases::m_ALL, this);
    }
    catch(CException& e)
    {
      std::string msg = CTools::Format("Error in CDefaultRecord::GetRecordByProductType - A duplicate 'productType' definition '%s' has been found. "
                                  "Native error: '%s'\n"
                                  "Check the Aliases Dictionary as a possibility. ",
                                  CAliases::m_ALL.c_str(), e.Message().c_str());      
      throw CException(msg, e.error());
    }

  }
  else
  {
    CStringArray::const_iterator it;

    for (it = m_productType.begin() ; it != m_productType.end() ; it++)
    {
      try
      {
        recordByProductType.Insert(*it, this);
      }
      catch(CException& e)
      {
        std::string msg = CTools::Format("Error in CDefaultRecord::GetRecordByProductType - A duplicate 'productType' definition '%s' has been found. "
                                    "Native error: '%s'\n"
                                    "Check the Aliases Dictionary as a possibility. ",
                                    (*it).c_str(), e.Message().c_str());      
        throw CException(msg, e.error());
      }

    }
  }

    
}
//----------------------------------------
void CDefaultRecord::Dump(std::ostream& fOut /* = std::cerr */)
{

 if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CDefaultRecord Object at "<< this << std::endl;
  fOut << "m_name: " << m_name << std::endl;
  fOut << "m_productType: " << std::endl;
  m_productType.Dump(fOut);
  fOut << "==> END Dump a CDefaultRecord Object at "<< this << " elements" << std::endl;


  fOut << std::endl;

}
//----------------------------------------

//-------------------------------------------------------------
//------------------- CAlias class --------------------
//-------------------------------------------------------------

CAlias::CAlias()
{
  Init();
}


//----------------------------------------
CAlias::~CAlias()
{
}

//----------------------------------------
void CAlias::Init()
{
}

//----------------------------------------
void CAlias::Dump(std::ostream& fOut /* = std::cerr */)
{

 if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CAlias Object at "<< this << std::endl;
  fOut << "m_name: " << m_name << std::endl;
  fOut << "m_description: " << m_description << std::endl;
  fOut << "m_ref: " << m_ref << std::endl;
  fOut << "m_value: " << m_value << std::endl;
  fOut << "==> END Dump a CAlias Object at "<< this << " elements" << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CAliases class --------------------
//-------------------------------------------------------------

const std::string CAliases::m_ALL = "*";

CAliases::CAliases()
{
  Init();
}


//----------------------------------------
CAliases::~CAliases()
{
}

//----------------------------------------
void CAliases::Init()
{
}
//----------------------------------------
void CAliases::AddAlias(CAlias* value)
{
  if (value == NULL)
  {
    return;
  }

  std::string name = value->GetName();

  if (name.empty())
  {
    throw CException("Error in CAliases::AddAlias - Unable to add alias because the alias name is empty", BRATHL_ERROR);
  }

  if (value == NULL)
  {
    return;
  }

  try
  {
    m_aliases.Insert(name, value);
  }
  catch(CException& e)
  {
    std::string msg = CTools::Format("Error in CAliases::AddAlias - Unable to add alias because the alias '%s' already exists - Native error: '%s'",
                                name.c_str(), e.Message().c_str());      
    throw CException(msg, e.error());
  }

}

//----------------------------------------
CAlias* CAliases::GetAlias(const std::string& key)
{
  CAlias* alias = dynamic_cast<CAlias*>(m_aliases.Exists(key));
  if (alias == NULL)
  {
    std::string msg = CTools::Format("Error in CAliases::GetAlias - Alias '%s' does not exist",
                                key.c_str());      
    throw CException(msg, BRATHL_LOGIC_ERROR);
  }

  return alias;
}

//----------------------------------------
std::string CAliases::GetAliasValue(const std::string& key)
{

  CAlias* alias = GetAlias(key);
  if (alias->IsSynonym())
  {
    alias = GetAlias(alias->GetRef());
  }

  return alias->GetValue();
}
//----------------------------------------------------
void CAliases::GetAliasesByProductType(CObMap& aliasesByProductType)
{
  aliasesByProductType.SetDelete(false);

  if (this->IsValidForAllProductType())
  {
    try
    {
      aliasesByProductType.Insert(CAliases::m_ALL, this);
    }
    catch(CException& e)
    {
      std::string msg = CTools::Format("Error in CAliases::GetAliasesByProductType - A duplicate 'productType' definition '%s' has been found. "
                                  "Native error: '%s'\n"
                                  "Check the Aliases Dictionary as a possibility. ",
                                  CAliases::m_ALL.c_str(), e.Message().c_str());      
      throw CException(msg, e.error());
    }
  }
  else
  {
    try
    {
      aliasesByProductType.Insert(m_productType, this);
    }
    catch(CException& e)
    {
      std::string msg = CTools::Format("Error in CAliases::GetAliasesByProductType - A duplicate 'productType' definition '%s' has been found. "
                                  "Native error: '%s'\n"
                                  "Check the Aliases Dictionary as a possibility. ",
                                  m_productType.c_str(), e.Message().c_str());      
      throw CException(msg, e.error());
    }

  }

    
}

//----------------------------------------
void CAliases::ResolveSynonyms()
{
  CObMap::iterator it;
  
  for(it = m_aliases.begin() ; it != m_aliases.end() ; it++)
  {
    CAlias* alias = dynamic_cast<CAlias*>(it->second);
    if (alias == NULL)
    {
      continue;
    }
    if (alias->IsSynonym())
    {
      alias->SetValue(this->GetAliasValue(it->first));
    }
  }

}
//----------------------------------------
void CAliases::Dump(std::ostream& fOut /* = std::cerr */)
{

 if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CAliases Object at "<< this << std::endl;
  fOut << "m_description: " << m_description << std::endl;
  fOut << "m_record: " << m_record << std::endl;
  fOut << "m_productType: " << m_productType << std::endl;
  fOut << "m_aliases: " << std::endl;
  m_aliases.Dump(fOut);
  fOut << "==> END Dump a CAliases Object at "<< this << " elements" << std::endl;


  fOut << std::endl;

}
//----------------------------------------

//-------------------------------------------------------------
//------------------- CProductAlias class --------------------
//-------------------------------------------------------------

CProductAlias::CProductAlias()
{
  Init();
}
//----------------------------------------
CProductAlias::CProductAlias(const CAlias& alias)
{
  Init();
  Set(alias);
}
//----------------------------------------
CProductAlias::~CProductAlias()
{
}

//----------------------------------------
void CProductAlias::Init()
{
}
//----------------------------------------
void CProductAlias::Set(const CAlias& alias)
{
  m_name = alias.GetName();
  m_value = alias.GetValue();
  m_description = alias.GetDescription();

}
//----------------------------------------

//----------------------------------------
void CProductAlias::Dump(std::ostream& fOut /* = std::cerr */)
{

 if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CProductAlias Object at "<< this << std::endl;
  fOut << "m_name: " << m_name << std::endl;
  fOut << "m_description: " << m_description << std::endl;
  fOut << "m_record: " << m_record << std::endl;
  fOut << "m_value: " << m_value << std::endl;
  fOut << "==> END Dump a CProductAlias Object at "<< this << " elements" << std::endl;


  fOut << std::endl;

}
//----------------------------------------

//-------------------------------------------------------------
//------------------- CProductAliases class --------------------
//-------------------------------------------------------------

CProductAliases::CProductAliases(const std::string& defaultRecord)
{
  Init();

  m_record = defaultRecord;
}


//----------------------------------------
CProductAliases::~CProductAliases()
{
  RemoveAll();
}

//----------------------------------------
void CProductAliases::Init()
{
}

//----------------------------------------
void CProductAliases::AddAlias(CAliases* value)
{
  if (value == NULL)
  {
    return;
  }

  AddAlias(const_cast<CObMap*>(value->GetAliases()), value->GetRecord());
}
//----------------------------------------
void CProductAliases::AddAlias(CObMap* value, const std::string& record)
{
  if (value == NULL)
  {
    return;
  }

  std::string recordTmp = m_record;
  if (!record.empty())
  {
    recordTmp = record;
  }

  CObMap::const_iterator it;
  for(it = value->begin() ; it != value->end() ; it++)
  {
    CAlias* alias = dynamic_cast<CAlias*>(it->second);
    if (alias == NULL)
    {
      continue;
    }
    
    AddAlias(alias, recordTmp);   
  }
}
//----------------------------------------
void CProductAliases::AddAlias(CAlias* value, const std::string& record)
{
  if (value == NULL)
  {
    return;
  }

  std::string name = value->GetName();

  if (name.empty())
  {
    throw CException("Error in CProductAliases::AddAlias - Unable to add alias because the alias name is empty", BRATHL_ERROR);
  }

  if (value == NULL)
  {
    return;
  }

  Erase(name);

  CProductAlias* productAlias = new CProductAlias(*value);
  productAlias->SetRecord(record);

  try
  {
    Insert(name, productAlias);
  }
  catch(CException& e)
  {
    std::string msg = CTools::Format("Error in CProductAliases::AddAlias - Unable to add alias because the alias '%s' already exists - Native error: '%s'",
                                name.c_str(), e.Message().c_str());      
    throw CException(msg, e.error());
  }

}

//----------------------------------------
CProductAlias* CProductAliases::GetAlias(const std::string& key)
{
  CProductAlias* alias = dynamic_cast<CProductAlias*>(Exists(key));
  if (alias == NULL)
  {
    std::string msg = CTools::Format("Error in CAliases::GetAlias - Alias '%s' does not exist",
                                key.c_str());      
    throw CException(msg, BRATHL_LOGIC_ERROR);
  }

  return alias;
}

//----------------------------------------
std::string CProductAliases::GetAliasValue(const std::string& key)
{
  CProductAlias* alias = GetAlias(key);
  return alias->GetValue();
}
//----------------------------------------
bool CProductAliases::AddRecordNameToField(CStringMap& mapAliasesString, CProduct* product, std::string& errorMsg)
{

  bool bOk = true;

  CObMap::const_iterator it;
  for(it = this->begin() ; it != this->end() ; it++)
  {
    CProductAlias* productAlias = dynamic_cast<CProductAlias*>(it->second);
    if (productAlias == NULL)
    {
      continue;
    }

    std::string errorString;

    std::string valueIn = productAlias->GetValue();
    std::string record = productAlias->GetRecord();
    std::string valueOut;

    bool recordNameAdded = AddRecordNameToField(valueIn, record, valueOut, product, errorString);
    bOk &= recordNameAdded;
    
    if (!recordNameAdded) 
    {
      errorMsg.append("\n");
      errorMsg.append(errorString.c_str());
      errorMsg.append("\n");
    }
    else
    {
      mapAliasesString.Insert(it->first, valueOut);
    }

  }

  return bOk;
}
//----------------------------------------
bool CProductAliases::AddRecordNameToField(const std::string& in, const std::string& dataSetName, std::string& out, CProduct* product, std::string& errorMsg)
{
  if (dataSetName.empty())
  {
    out = in;
    return true;
  }

  CTools::ReplaceAliases(in, out);

  CExpression expr;

  bool complexExpression = false;

  try
  {
    expr.SetExpression(out);
    complexExpression = (expr.GetCode()->size() > 1);
  }
  catch (CException& e)
  {
    errorMsg = e.what();
    return false;
  }

  const CStringArray* fields = expr.GetFieldNames();

  return AddRecordNameToField(in, dataSetName, *fields, complexExpression, out, product, errorMsg);



}
//----------------------------------------
bool CProductAliases::AddRecordNameToField( const std::string& in, const std::string& dataSetName, const CStringArray& fieldsIn, bool complexExpression, std::string& out, CProduct* product, std::string& errorMsg )
{
	UNUSED(errorMsg);
	bool bOk = true;

	CStringArray::const_iterator it;

	out = in;

	if ( dataSetName.empty() )
	{
		return true;
	}
	const std::string emptyString;
	for ( it = fieldsIn.begin(); it != fieldsIn.end(); it++ )
	{
		bool add = true;
		// Search if field name contains a record name (contains a dot)
		if ( ( *it ).find( "." ) != std::string::npos )
		{
			if ( product != NULL )
			{
				// If the field name already contains a record name
				// don't add any record to the field.
				CField *field = product->FindFieldByName( *it, emptyString, false, NULL );
				if ( field != NULL )
				{
					add = false;
				}
			}
		}

		if ( add )
		{
			std::string fullName = dataSetName;
			fullName.append( "." );
			fullName.append( *it );
			out = CTools::ReplaceWord( out, *it, fullName );
		}
	}

	if ( complexExpression )
	{
		out.insert( 0, "(" );
		out.append( ")" );
	}


	return bOk;

}

//----------------------------------------
void CProductAliases::Dump(std::ostream& fOut /* = std::cerr */)
{

 if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CProductAliases Object at "<< this << std::endl;
  fOut << "m_record: " << m_record << std::endl;
  //fOut << "m_aliases: " << std::endl;
  CObMap::Dump(fOut);
  fOut << "==> END Dump a CProductAliases Object at "<< this << " elements" << std::endl;


  fOut << std::endl;

}
//----------------------------------------


} // end namespace
