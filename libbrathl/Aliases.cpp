/*
* Copyright (C) 2005-2011 Collecte Localisation Satellites (CLS), France (31)
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


#include "Trace.h"
#include "Field.h"
#include "Expression.h"
#include "Product.h"
#include "Aliases.h"
using namespace brathl;

// When debugging changes all calls to “new” to be calls to “DEBUG_NEW” allowing for memory leaks to
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
      string msg = CTools::Format("Error in CDefaultRecord::GetRecordByProductType - A duplicate 'productType' definition '%s' has been found. "
                                  "Native error: '%s'\n"
                                  "Check the Aliases Dictionary as a possibility. ",
                                  CAliases::m_ALL.c_str(), e.GetMessage().c_str());      
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
        string msg = CTools::Format("Error in CDefaultRecord::GetRecordByProductType - A duplicate 'productType' definition '%s' has been found. "
                                    "Native error: '%s'\n"
                                    "Check the Aliases Dictionary as a possibility. ",
                                    (*it).c_str(), e.GetMessage().c_str());      
        throw CException(msg, e.error());
      }

    }
  }

    
}
//----------------------------------------
void CDefaultRecord::Dump(ostream& fOut /* = cerr */)
{

 if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CDefaultRecord Object at "<< this << endl;
  fOut << "m_name: " << m_name << endl;
  fOut << "m_productType: " << endl;
  m_productType.Dump(fOut);
  fOut << "==> END Dump a CDefaultRecord Object at "<< this << " elements" << endl;


  fOut << endl;

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
void CAlias::Dump(ostream& fOut /* = cerr */)
{

 if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CAlias Object at "<< this << endl;
  fOut << "m_name: " << m_name << endl;
  fOut << "m_description: " << m_description << endl;
  fOut << "m_ref: " << m_ref << endl;
  fOut << "m_value: " << m_value << endl;
  fOut << "==> END Dump a CAlias Object at "<< this << " elements" << endl;

  fOut << endl;

}

//-------------------------------------------------------------
//------------------- CAliases class --------------------
//-------------------------------------------------------------

const string CAliases::m_ALL = "*";

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

  string name = value->GetName();

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
    string msg = CTools::Format("Error in CAliases::AddAlias - Unable to add alias because the alias '%s' already exists - Native error: '%s'",
                                name.c_str(), e.GetMessage().c_str());      
    throw CException(msg, e.error());
  }

}

//----------------------------------------
CAlias* CAliases::GetAlias(const string& key)
{
  CAlias* alias = dynamic_cast<CAlias*>(m_aliases.Exists(key));
  if (alias == NULL)
  {
    string msg = CTools::Format("Error in CAliases::GetAlias - Alias '%s' does not exist",
                                key.c_str());      
    throw CException(msg, BRATHL_LOGIC_ERROR);
  }

  return alias;
}

//----------------------------------------
string CAliases::GetAliasValue(const string& key)
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
      string msg = CTools::Format("Error in CAliases::GetAliasesByProductType - A duplicate 'productType' definition '%s' has been found. "
                                  "Native error: '%s'\n"
                                  "Check the Aliases Dictionary as a possibility. ",
                                  CAliases::m_ALL.c_str(), e.GetMessage().c_str());      
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
      string msg = CTools::Format("Error in CAliases::GetAliasesByProductType - A duplicate 'productType' definition '%s' has been found. "
                                  "Native error: '%s'\n"
                                  "Check the Aliases Dictionary as a possibility. ",
                                  m_productType.c_str(), e.GetMessage().c_str());      
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
void CAliases::Dump(ostream& fOut /* = cerr */)
{

 if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CAliases Object at "<< this << endl;
  fOut << "m_description: " << m_description << endl;
  fOut << "m_record: " << m_record << endl;
  fOut << "m_productType: " << m_productType << endl;
  fOut << "m_aliases: " << endl;
  m_aliases.Dump(fOut);
  fOut << "==> END Dump a CAliases Object at "<< this << " elements" << endl;


  fOut << endl;

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
void CProductAlias::Dump(ostream& fOut /* = cerr */)
{

 if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CProductAlias Object at "<< this << endl;
  fOut << "m_name: " << m_name << endl;
  fOut << "m_description: " << m_description << endl;
  fOut << "m_record: " << m_record << endl;
  fOut << "m_value: " << m_value << endl;
  fOut << "==> END Dump a CProductAlias Object at "<< this << " elements" << endl;


  fOut << endl;

}
//----------------------------------------

//-------------------------------------------------------------
//------------------- CProductAliases class --------------------
//-------------------------------------------------------------

CProductAliases::CProductAliases(const string& defaultRecord)
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
void CProductAliases::AddAlias(CObMap* value, const string& record)
{
  if (value == NULL)
  {
    return;
  }

  string recordTmp = m_record;
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
void CProductAliases::AddAlias(CAlias* value, const string& record)
{
  if (value == NULL)
  {
    return;
  }

  string name = value->GetName();

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
    string msg = CTools::Format("Error in CProductAliases::AddAlias - Unable to add alias because the alias '%s' already exists - Native error: '%s'",
                                name.c_str(), e.GetMessage().c_str());      
    throw CException(msg, e.error());
  }

}

//----------------------------------------
CProductAlias* CProductAliases::GetAlias(const string& key)
{
  CProductAlias* alias = dynamic_cast<CProductAlias*>(Exists(key));
  if (alias == NULL)
  {
    string msg = CTools::Format("Error in CAliases::GetAlias - Alias '%s' does not exist",
                                key.c_str());      
    throw CException(msg, BRATHL_LOGIC_ERROR);
  }

  return alias;
}

//----------------------------------------
string CProductAliases::GetAliasValue(const string& key)
{
  CProductAlias* alias = GetAlias(key);
  return alias->GetValue();
}
//----------------------------------------
bool CProductAliases::AddRecordNameToField(CStringMap& mapAliasesString, CProduct* product, string& errorMsg)
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

    string errorString;

    string valueIn = productAlias->GetValue();
    string record = productAlias->GetRecord();
    string valueOut;

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
bool CProductAliases::AddRecordNameToField(const string& in, const string& dataSetName, string& out, CProduct* product, string& errorMsg)
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
bool CProductAliases::AddRecordNameToField(const string& in, const string& dataSetName, const CStringArray& fieldsIn, bool complexExpression, string& out, CProduct* product, string& errorMsg)
{
  bool bOk = true;

  CStringArray::const_iterator it;

  out = in;

  if (dataSetName.empty())
  {
    return true;
  }
  const string emptyString;
  for (it = fieldsIn.begin() ; it != fieldsIn.end() ; it++)
  {
    bool add = true;
    // Search if field name contains a record name (contains a dot)
    if ((*it).find(".") != string::npos)
    {
      if (product != NULL)
      {
        // If the field name already contains a record name
        // don't add any record to the field.
        CField *field = product->FindFieldByName(*it, emptyString, false, NULL);
        if (field != NULL)
        {
          add = false;
        }
      }
    }

    if (add)
    {
      string fullName = dataSetName;
      fullName.append(".");
      fullName.append(*it);
      out = CTools::ReplaceWord(out, *it, fullName);
    }
  }

  if (complexExpression)
  {
    out.insert(0, "(");
    out.append(")");
  }


  return bOk;

}

//----------------------------------------
void CProductAliases::Dump(ostream& fOut /* = cerr */)
{

 if (CTrace::IsTrace() == false)
  {
    return;
  }

  fOut << "==> Dump a CProductAliases Object at "<< this << endl;
  fOut << "m_record: " << m_record << endl;
  //fOut << "m_aliases: " << endl;
  CObMap::Dump(fOut);
  fOut << "==> END Dump a CProductAliases Object at "<< this << " elements" << endl;


  fOut << endl;

}
//----------------------------------------


} // end namespace
