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


#include "new-gui/brat/stdafx.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "brathl.h"

#include "new-gui/Common/tools/Trace.h"
#include "Tools.h"
#include "new-gui/Common/tools/Exception.h"
#include "Expression.h"
using namespace brathl;

#include "BratGui.h"

#include "Function.h"

const std::string LOGICAL_AND = "&&";
const std::string LOGICAL_AND_DESC = "The logical AND operator (&&) returns the boolean value true if both operands are true and returns false otherwise."
                                  "\nLogical AND has left-to-right associativity.";
const std::string LOGICAL_OR = "||";
const std::string LOGICAL_OR_DESC = "The logical OR operator (||) returns the boolean value true if either or both operands is true and returns false otherwise."
                                "\nLogical OR has left-to-right associativity";
const std::string LOGICAL_NOT = "!";
const std::string LOGICAL_NOT_DESC = "The logical negation operator (!) reverses the meaning of its operand"
                                  "\nThe result is true if the converted operand is false; the result is false if the converted operand is true.";

const std::string RELATIONAL_EQUAL = "==";
const std::string RELATIONAL_EQUAL_DESC = "The equal-to operator (==) returns true (1) if both operands have the same value; otherwise, it returns false (0)."
                                       "\nThe not-equal-to operator (!=) returns true if the operands do not have the same value; otherwise, it returns false";
const std::string RELATIONAL_NOTEQUAL = "!=";

const std::string RELATIONAL_LESS = "<";
const std::string RELATIONAL_LESS_THAN = "<=";
const std::string RELATIONAL_GREATER = ">";
const std::string RELATIONAL_GREATER_THAN = ">=";
const std::string RELATIONAL_LESS_GREATER_DESC = "The binary relational operators determine the following relationships:\n"
                                              "Less than (<)\n"
                                              "Greater than (>)\n"
                                              "Less than or equal to (<=)\n"
                                              "Greater than or equal to (>=)\n"
                                              "The relational operators have left-to-right associativity. Both operands of relational operators must be of arithmetic.\n"
                                              "They yield values of type bool. The value returned is false (0) if the relationship in the expression is false; otherwise, the value returned is true (1).";

const std::string IIF_TEST = "iif";
const std::string IIF_TEST_DESC = "Inline if\n"
		"If the first parameter is true (not 0 and not default value),\n"
		"the second parameter is returned, otherwise it is the third one.\n"
		"Logically equivalent to:\n"
		"  if (param1 is true)\n"
		"    return param2\n"
		"  else\n"
		"    return param3\n"
		"  end if\n";

const std::string IIF3_TEST = "iif3";
const std::string IIF3_TEST_DESC = "Inline if with default value case\n"
		"If the first parameter is true (not 0 and not default value),\n"
		"the second parameter is returned. If is is 0, the third one is\n"
		"returned, otherwise (it is a default value) the fourth one is\n"
		"returned\n"
		"Logically equivalent to:\n"
		"  if (param1 is default value)\n"
		"    return param4\n"
		"  else\n"
		"    if (param1 is true)\n"
		"      return param2\n"
		"    else\n"
		"      return param3\n"
		"    end if\n"
		"  end if\n";


const std::string BITWISE_AND = "&";
const std::string BITWISE_AND_DESC = "Then the & operator looks at the binary representation\n"
                "of the values of two expressions and does a bitwise AND operation on them.\n"
                "If at least one of the expression is default value or do not fall\n"
                "in integer range, a default value is returned.\n"
                "\n"
                "Example:\n"
                "expression1 & expression2\n"
                "0101   (expression1)\n"
                "1100   (expression2)\n"
                "----\n"
                "0100   (result)\n"
                "Any time both of the expressions have a 1 in a digit,\n"
                "the result has a 1 in that digit. Otherwise, the result has a 0 in that digit.";

const std::string BITWISE_OR = "|";
const std::string BITWISE_OR_DESC = "Then the | operator looks at the binary representation\n"
                "of the values of two expressions and does a bitwise OR operation on them.\n"
                "If at least one of the expression is default value or do not fall\n"
                "in integer range, a default value is returned.\n"
                "\n"
                "Example:\n"
                "expression1 | expression2\n"
                "0101   (expression1)\n"
                "1100   (expression2)\n"
                "----\n"
                "1101   (result)\n"
                "Any time either of the expressions have a 1 in a digit,\n"
                "the result has a 1 in that digit. Otherwise, the result has a 0 in that digit.";

const std::string BITWISE_NOT = "~";
const std::string BITWISE_NOT_DESC = "Then the ~ operator looks at the binary representation\n"
                "of the values of an expression and does a bitwise negation operation on it.\n"
                "If the expression is default value or do not fall\n"
                "in integer range, a default value is returned.\n"
                "\n"
                "Example:\n"
                "~expression\n"
                "0101   (expression)\n"
                "----\n"
                "1010   (result)\n"
                "Any digit that is a 1 in the expression becomes a 0 in the result,\n"
                "Any digit that is a 0 in the expression becomes a 1 in the result.";

#define BRATGUI_NUMBER_ELT(X) (sizeof(X)/sizeof((X)[0]))

//-------------------------------------------------------------
//------------------- CFunction class --------------------
//-------------------------------------------------------------


//----------------------------------------

CFunction::CFunction(const wxString& name, const wxString& description, int32_t category, int32_t nbParams)
{
  m_name = name;
  m_description = description;
  m_nbParams = nbParams;
  m_category = category;
}

//----------------------------------------
CFunction::CFunction(const std::string& name, const std::string& description, int32_t category, int32_t nbParams)
{
  m_name = name.c_str();
  m_description = description.c_str();
  m_nbParams = nbParams;
  m_category = category;
}

//----------------------------------------

CFunction::~CFunction()
{
}
//----------------------------------------
wxString CFunction::GetCategoryAsString()
{
  return GetCategoryAsString(m_category);
}
//----------------------------------------
wxString CFunction::GetCategoryAsString(int32_t category)
{

  wxString label;

  switch (category)
  {
  case MathTrigo:
    {
      label = "Math & Trigo";
      break;
    }
  case Statistical:
    {
      label = "Statistical";
      break;
    }
  case Logical:
    {
      label = "Logical";
      break;
    }
  case Relational:
    {
      label = "Relational";
      break;
    }
  case Constant:
    {
      label = "Constant";
      break;
    }
  case BitwiseOp:
    {
      label = "Bitwise Operators";
      break;
    }
  case DateTime:
    {
      label = "Date & Time";
      break;
    }
  case Geographical:
    {
      label = "Geographical";
      break;
    }
  default:
    {
      label = "Math & Trigo";
      break;
    }
  }

  return label;
}
//----------------------------------------
wxString CFunction::GetSyntax()
{
  wxString params;
  if (m_nbParams < 0) // constants
  {
    return m_name;
  }

  for (int32_t i = 0 ; i < m_nbParams ; i++)
  {
    params += wxString::Format("param%d,", i + 1);
  }
  if (params.IsEmpty() == false)
  {
    params.RemoveLast();
  }

  return wxString::Format("%s(%s)", m_name.c_str(), params.c_str());
}

//----------------------------------------
void CFunction::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CFunction Object at "<< this << std::endl;
  fOut << "m_name " << m_name;
  fOut << "m_description " << m_description;
  fOut << "m_nbParams " << m_nbParams;
  fOut << "==> END Dump a CFunction Object at "<< this << std::endl;

  fOut << std::endl;

}

//----------------------------------------
void CFunction::DumpFmt(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << m_name.c_str() << "\t\t";
  fOut << m_description.c_str() << "\t\t";
  fOut << GetSyntax().c_str() << "\t\t";

  fOut << std::endl;

}


//-------------------------------------------------------------
//------------------- CMapFunction class --------------------
//-------------------------------------------------------------
const wxString CMapFunction::m_configFilename = "function_descr.txt";

CMapFunction::CMapFunction()
{
  Init();

  int32_t i = 0;

  //Functions with 1 parameter (of type double)
  for (i = 0 ; i < BRATGUI_NUMBER_ELT(Fonctions1) ; i++)
  {
    Insert(Fonctions1[i].Name, new CFunction(Fonctions1[i].Name, Fonctions1[i].Desc, Fonctions1[i].Category));
  }

  //Functions with 2 parameters (of type double)
  for (i = 0 ; i < BRATGUI_NUMBER_ELT(Fonctions2) ; i++)
  {
    Insert(Fonctions2[i].Name, new CFunction(Fonctions2[i].Name, Fonctions2[i].Desc, Fonctions2[i].Category, 2));
  }

  //Functions with 1 parameter (of type char*)
  for (i = 0 ; i < BRATGUI_NUMBER_ELT(CharFonctions1) ; i++)
  {
    Insert(CharFonctions1[i].Name, new CFunction(CharFonctions1[i].Name, CharFonctions1[i].Desc, CharFonctions1[i].Category, 1));
  }

  //Functions with 3 parameters (of type double)
  for (i = 0 ; i < BRATGUI_NUMBER_ELT(Fonctions3) ; i++)
  {
    Insert(Fonctions3[i].Name, new CFunction(Fonctions3[i].Name, Fonctions3[i].Desc, Fonctions3[i].Category, 3));
  }

  //Constants
  for (i = 0 ; i < BRATGUI_NUMBER_ELT(Constants) ; i++)
  {
    Insert(Constants[i].Name, new CFunction(Constants[i].Name, Constants[i].Desc, Constants[i].Category, -1));
  }


  //Boolean operators
  Insert(LOGICAL_AND, new CFunction(LOGICAL_AND, LOGICAL_AND_DESC, Logical,  -1));
  Insert(LOGICAL_OR, new CFunction(LOGICAL_OR, LOGICAL_OR_DESC, Logical, -1));
  Insert(LOGICAL_NOT, new CFunction(LOGICAL_NOT, LOGICAL_NOT_DESC, Logical, -1));

  //Relational operators
  Insert(RELATIONAL_EQUAL, new CFunction(RELATIONAL_EQUAL, RELATIONAL_EQUAL_DESC, Relational, -1));
  Insert(RELATIONAL_NOTEQUAL, new CFunction(RELATIONAL_NOTEQUAL, RELATIONAL_EQUAL_DESC, Relational, -1));
  Insert(RELATIONAL_LESS, new CFunction(RELATIONAL_LESS, RELATIONAL_LESS_GREATER_DESC, Relational, -1));
  Insert(RELATIONAL_LESS_THAN, new CFunction(RELATIONAL_LESS_THAN, RELATIONAL_LESS_GREATER_DESC, Relational, -1));
  Insert(RELATIONAL_GREATER, new CFunction(RELATIONAL_GREATER, RELATIONAL_LESS_GREATER_DESC, Relational, -1));
  Insert(RELATIONAL_GREATER_THAN, new CFunction(RELATIONAL_GREATER_THAN, RELATIONAL_LESS_GREATER_DESC, Relational, -1));

  // Inline if tests
  Insert(IIF_TEST, new CFunction(IIF_TEST, IIF_TEST_DESC, Logical, 3));
  Insert(IIF3_TEST, new CFunction(IIF3_TEST, IIF3_TEST_DESC, Logical, 4));

  //Bitwise operators
  Insert(BITWISE_AND, new CFunction(BITWISE_AND, BITWISE_AND_DESC, BitwiseOp, -1));
  Insert(BITWISE_OR, new CFunction(BITWISE_OR, BITWISE_OR_DESC, BitwiseOp, -1));
  Insert(BITWISE_NOT, new CFunction(BITWISE_NOT, BITWISE_NOT_DESC, BitwiseOp, -1));

  SetExtraDescr();

  //SaveFunctionDescrTemplate(m_config);
}


//----------------------------------------
CMapFunction::~CMapFunction()
{
  DeleteConfig();
}

//----------------------------------------
CMapFunction& CMapFunction::GetInstance()
{
 static CMapFunction instance;

 return instance;
}
//----------------------------------------
void CMapFunction::Init()
{
  m_config = NULL;

  wxString filePathName = CTools::FindDataFile((const char *)CMapFunction::m_configFilename.c_str()).c_str();
  if (!filePathName.IsEmpty())
  {
    m_config = new wxFileConfig(wxEmptyString, wxEmptyString, filePathName, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
  }

}
//----------------------------------------
void CMapFunction::DeleteConfig()
{
  if (m_config != NULL)
  {
    delete m_config;
    m_config = NULL;
  }

}
//----------------------------------------
wxString CMapFunction::GetFunctionExtraDescr(const wxString& pathSuff)
{
  return GetFunctionExtraDescr(m_config, pathSuff);
}
//----------------------------------------
wxString CMapFunction::GetFunctionExtraDescr(wxFileConfig* config, const wxString& pathSuff)
{
  bool bOk = true;

  if (config == NULL)
  {
    return "";
  }
  config->SetPath("/" + pathSuff);

  wxString valueString;

  valueString = config->Read(ENTRY_COMMENT);


  return valueString;

}
//----------------------------------------
void CMapFunction::SetExtraDescr()
{
  if (m_config == NULL)
  {
    return;
  }

  CMapFunction::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CFunction* value = dynamic_cast<CFunction*>(it->second);
    if (value == NULL)
    {
      continue;
    }

    wxString str = GetFunctionExtraDescr(value->GetName());
    if (str.IsEmpty())
    {
      continue;
    }
    value->SetDescription(wxString::Format("%s\n\n%s", value->GetDescription().c_str(), str.c_str()));

  }


}
//----------------------------------------
void CMapFunction::SaveFunctionDescrTemplate(const std::string &internal_data_path, wxFileConfig* config, bool flush)
{
  if (config == NULL)
  {
    std::string filePathName = internal_data_path;
    filePathName.append(PATH_SEPARATOR);
    filePathName.append(CMapFunction::m_configFilename.c_str());
    config = new wxFileConfig(wxEmptyString, wxEmptyString, filePathName.c_str(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
  }

  CMapFunction::iterator it;

  for (it = CMapFunction::GetInstance().begin() ; it != CMapFunction::GetInstance().end() ; it++)
  {
    CFunction* value = dynamic_cast<CFunction*>(it->second);
    if (value == NULL)
    {
      continue;
    }

    config->SetPath("/" + value->GetName());

    wxString str;
    for (int32_t i = 0 ; i < value->GetNbparams() ; i++)
    {
      str += wxString::Format("param%d: \n", i+1);
    }

    config->Write(ENTRY_COMMENT, str);

  }

  if (flush)
  {
    config->Flush();
  }


}

//----------------------------------------
void CMapFunction::GetCategory(wxChoice& choice)
{
  choice.Append("All", new int32_t(-1));

  for (int32_t category = MathTrigo; category <= Geographical; category++)
  {
    choice.Append(CFunction::GetCategoryAsString(category), new int32_t(category));
  }
}
//----------------------------------------
bool CMapFunction::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapFunction::ValidName(const char* name)
{
  CFunction* value = dynamic_cast<CFunction*>(Exists(name));
  return (value != NULL);
}
//----------------------------------------
wxString CMapFunction::GetDescFunc(const wxString& name)
{
  CFunction* value = dynamic_cast<CFunction*>(Exists((const char *)name.c_str()));
  if (value == NULL)
  {
    return "";
  }

  return value->GetDescription();
}

//----------------------------------------
wxString CMapFunction::GetSyntaxFunc(const wxString& name)
{
  CFunction* value = dynamic_cast<CFunction*>(Exists((const char *)name.c_str()));
  if (value == NULL)
  {
    return "";
  }

  return value->GetSyntax();
}

//----------------------------------------
void CMapFunction::NamesToArrayString(wxArrayString& array)
{
  CMapFunction::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CFunction* value = dynamic_cast<CFunction*>(it->second);
    if (value != NULL)
    {
      array.Add(value->GetName());
    }
  }

}
//----------------------------------------
void CMapFunction::NamesToComboBox(wxComboBox& combo)
{
  CMapFunction::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CFunction* value = dynamic_cast<CFunction*>(it->second);
    if (value != NULL)
    {
      combo.Append(value->GetName());
    }
  }

}

//----------------------------------------
void CMapFunction::NamesToListBox(wxListBox& listBox, int32_t category /*= -1*/)
{
  CMapFunction::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CFunction* value = dynamic_cast<CFunction*>(it->second);
    if (value != NULL)
    {
      if (category != -1)
      {
        if (value->GetCategory() != category)
        {
          continue;
        }
      }
      listBox.Append(value->GetName());
    }
  }

}


//----------------------------------------
void CMapFunction::Dump(std::ostream& fOut /* = std::cerr */)
{

   if (CTrace::IsTrace() == false)
   {
      return;
   }

   fOut << "==> Dump a CMapFunction Object at "<< this << " with " <<  size() << " elements" << std::endl;

   CMapFunction::iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      CBratObject *ob = it->second;
      fOut << "CMapFunction Key is = " << (*it).first << std::endl;
      fOut << "CMapFunction Value is = " << std::endl;
      ob->Dump(fOut);
   }

   fOut << "==> END Dump a CMapFunction Object at "<< this << " with " <<  size() << " elements" << std::endl;

}

//----------------------------------------
void CMapFunction::DumpFmt(std::ostream& fOut /* = std::cerr */)
{

   if (CTrace::IsTrace() == false)
   {
      return;
   }

  fOut << "Name" << "\t\t";
  fOut << "Description" << "\t\t";
  fOut << "Syntax" << "\t\t";
  fOut << std::endl;
  fOut << "=====" << "\t\t";
  fOut << "============" << "\t\t";
  fOut << "======" << "\t\t";
  fOut << std::endl;

   CMapFunction::iterator it;

   for (it = this->begin() ; it != this->end() ; it++)
   {
      CFunction *ob = dynamic_cast<CFunction*>(it->second);
      if (ob == NULL)
      {
        continue;
      }
      ob->DumpFmt(fOut);
   }

}

