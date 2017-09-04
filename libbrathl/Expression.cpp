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

#include <cmath>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <string>

#include "brathl.h"

#include "common/tools/Exception.h"
#include "Expression.h"
#include "Tools.h" 
#include "common/tools/TraceLog.h"

// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#include "Win32MemLeaksAccurate.h"

using namespace brathl;

namespace brathl
{


#define MIN(X,Y) ((X) <= (Y) ? (X) : (Y))
#define NUMBER(X) (sizeof(X)/sizeof((X)[0]))

#define UNKNOWN_INTEGER		0x7FFFFFFF


/* Types de tokens lisibles */

#define TOKEN_END	0	/* Fin de chaine		*/
#define TOKEN_VALUE	1	/* valeur (nombre ou chaine)	*/
#define TOKEN_FIELD	2	/* Cxxx: Numero de champ	*/
#define TOKEN_FUNCTION1 3	/* xxxx: Nom de fonction	*/
#define TOKEN_FUNCTION2 4	/* xxxx: Nom de fonction 2 param*/
#define TOKEN_FUNCTION3 5	/* xxxx: Nom de fonction 3 param*/
#define TOKEN_CHARFUNC1 6	/* xxxx: Nom de fonction	*/
#define TOKEN_LEFTPAR	7	/* '('				*/
#define TOKEN_RIGHTPAR	8	/* ')'				*/
#define TOKEN_IIF	9	/* 'iif'			*/
#define TOKEN_IIF3	10	/* 'iif3'			*/
#define TOKEN_COMMA	11	/* ','				*/
#define TOKEN_OPERATOR	12	/* Un operateur			*/
#define TOKEN_FUNCTION_ALGO_N 13	/* xxxx: Nom de fonction N param*/

/* Les bits de poids faible representent la priorite de l'operateur */
/* Priorite doit etre > 0 (Sinon Pb avec CLIP_Encode)		    */
#define OP_LEFTPAR	uint32_t(0x010000FF)
#define OP_UNARYMINUS	uint32_t(0x02000001)
#define OP_BITNOT	uint32_t(0x03000002)
#define OP_POWER	uint32_t(0x04000003)
#define OP_MULTIPLY	uint32_t(0x05000004)
#define OP_DIVIDE	uint32_t(0x06000004)
#define OP_MODULO	uint32_t(0x07000004)
#define OP_PLUS		uint32_t(0x08000005)
#define OP_MINUS	uint32_t(0x09000005)
#define OP_BITAND	uint32_t(0x0A000006)
#define OP_BITOR	uint32_t(0x0B000007)
#define OP_EQ		uint32_t(0x0C000008)
#define OP_NE		uint32_t(0x0D000008)
#define OP_LT		uint32_t(0x0E000008)
#define OP_LE		uint32_t(0x0F000008)
#define OP_GT		uint32_t(0x10000008)
#define OP_GE		uint32_t(0x11000008)
#define OP_NOT		uint32_t(0x12000009)
#define OP_AND		uint32_t(0x1300000A)
#define OP_OR		uint32_t(0x1400000B)
#define OP_RIGHTPAR	uint32_t(0x150000FF)

/* Les codes Op ci-dessous ne sont jamais mis sur la pile des		*/
/* operateurs mais inseres directement dans le resultat			*/

/* Pour les fonctions, les bits de poids faible contiennent le numero de la fonction */
#define OP_FUNCTION1	uint32_t(0x30000000)
#define OP_FUNCTION2	uint32_t(0x31000000)
#define OP_FUNCTION3	uint32_t(0x32000000)
#define OP_CHARFUNC1	uint32_t(0x33000000)
#define OP_FUNCTION_ALGO_N	uint32_t(0x3B000000)


/* Pour les valeurs, les bits de poids faible contiennent le numero de la valeur a placer */
#define OP_PUSHVALUE	uint32_t(0x34000000)

/* Pour les champs, les bits de poids faible contiennent le numero du champ */
#define OP_PUSHFIELD	uint32_t(0x35000000)

/* Indique le debut d'un inlined if. Les bits de poids faible donnent le numero du iif */
#define OP_BEGIN_IIF	uint32_t(0x36000000)
/* Indique le debut d'un inlined if ternaire. Les bits de poids faible donnent le numero du iif */
#define OP_BEGIN_IIF3	uint32_t(0x37000000)
/* Indique la partie ELSE d'un inlined if. Les bits de poids faible donnent le numero du iif */
#define OP_ELSE_IIF	uint32_t(0x38000000)
/* Indique la partie DEFAULT d'un inlined if ternaire. Les bits de poids faible donnent le numero du iif */
#define OP_DEF_IIF	uint32_t(0x39000000)
/* Indique la fin d'un inlined if. Les bits de poids faible donnent le numero du iif */
#define OP_END_IIF	uint32_t(0x3A000000)

/* Permet de recuperer l'operateur */
#define OP_OPMASK	uint32_t(0xFF000000)
/* Permet de recuperer le parametre de l'operateur */
#define OP_VALUEMASK	uint32_t(0x00FFFFFF)


// To test char range
typedef std::numeric_limits<char> numeric_limits_char;


//-------------------------------------------------------------
//------------------- CCompiler class --------------------
//-------------------------------------------------------------


class CCompiler
{
public:

  CCompiler();

  void Compile
		(const std::string& text,
		 CUIntArray* code,
		 CObArray	*constants,
		 CStringArray* fieldNames,
     CVectorBratAlgorithm* algoArray);

  std::string GetText() { return m_text; };
  const std::stack<uint32_t>& GetOperators() { return m_operators; };
  const CUIntArray* GetCode() { return m_code; };
  //const std::vector<CExpressionValue>* GetConstants() { return m_constants; };
  const CObArray* GetConstants() { return m_constants; };
  const CStringArray* GetFieldNames() { return m_fieldNames; };

private:
  void Expression();
  void GetToken();
  void PushOp(uint32_t theOperator);
  void PopOp();
  void PopAllOp();
  void SubExpression();
  void Element();
  void ScanFunctionParameters(uint32_t NbParameters);
  void ScanFunctionParameters(uint32_t	NbParameters, CStringArray* params);
  std::string GetAlgoFunctionName();
  std::string GetAlgoTokenName();

  void IifInst();
  void Iif3Inst();
  static const char m_charEnd;

  std::string m_text;		// Text to compile
  std::stack<uint32_t> m_operators;	// Stack for storing operators
  uint32_t m_token;	// current token
  uint32_t m_tokenId;	// current token id
  bool m_followOperator; // Indicates if we are behind an operator
  uint32_t m_iifCount;	// Iif count for iif numbering
  uint32_t m_current;	// current char analysis pos in m_text
  CUIntArray* m_code;	// OPCodes of resulting code
  //std::vector<CExpressionValue>* m_constants;	// Constants used in the expression
  // Constants used in the expression. A "CExpressionValue*" array.
  CObArray* m_constants;	// Constants used in the expression
  CStringArray*	m_fieldNames;	// Fields mentionned in the expression
  
  CVectorBratAlgorithm* m_algoArray; // Algo mentionned in the expression


};

const char CCompiler::m_charEnd	= '\0';


/*
//=========================================================================
struct CExpressionValue::ValueData {
  uint32_t	RefCount;
  uint32_t	NbValues;
  bool		Allocated;
  double	*Value;

public:

  ValueData(uint32_t NbVal = 0, double  *Data	= NULL, bool MakeCopy	= true)
  {
    RefCount	= 1;
    NbValues	= NbVal;
    Value	= Data;
    Allocated	= (NbVal > 0) && (MakeCopy || (Data == NULL));
    if (Allocated)
    {
      Value	= NewValue(NbVal);
      if (Data != NULL)
      {
	      memcpy(Value, Data, sizeof(*Value)*NbVal);
      }
    }
  }

  ValueData(const CDoubleArray& data)
  {
    RefCount	= 1;
    NbValues	= data.size();
    Value	= NULL;
    Allocated	= (NbValues > 0);
    if (Allocated)
    {
      Value	= NewValue(NbValues);
      std::copy(data.begin(), data.end(), Value);
    }
  }

  ~ValueData()
  {
    RefCount	= 0;
    if (Allocated)
    {
      delete []Value;
      Value = NULL;
    }
  }


protected:

  //----------------------------------------
double* NewValue(uint32_t nbValues)
{
  double* values = NULL;
  try
  {
    values = new double[nbValues];   
  }
  catch (CException& e)
  {
    throw CException(CTools::Format("ERROR in CExpressionValue::ValueData::NewValue. Reason: %s\n",
					  e.what()),
            BRATHL_ERROR);
  }
  catch (std::bad_alloc& e) // If memory allocation (new) failed...
  {
    throw CMemoryException(CTools::Format("ERROR in CExpressionValue::ValueData::NewValue - Unable to allocate new memory to store the result.\nNative error: '%s'\n"
                                           " or try to allocate more than your system architecture can ever handle",
                                  e.what()));
  }
  catch (...)
  {
    throw CMemoryException(CTools::Format("ERROR in CExpressionValue::ValueData::NewValue. It may be that it does not remain enough memory to allocate %ld values"
                                           " or try to allocate more than your system architecture can ever handle",
					  (long)nbValues));
  }


  if (values == NULL)
  {
    throw CMemoryException(CTools::Format("ERROR in CExpressionValue::ValueData::NewValue. It may be that it does not remain enough memory to allocate %ld values"
                                           " or try to allocate more than your system architecture can ever handle",
					  (long)nbValues));

  }
  return values;

}


private: // Prevent copy
  ValueData(const ValueData&);
  ValueData & operator=(const ValueData&);
};

*/



//-------------------------------------------------------------
//------------------- CValueData class --------------------
//-------------------------------------------------------------



CExpressionValue::CValueData::CValueData(uint32_t nbVal /*= 0*/, double  *data /*= NULL*/, bool makeCopy /*= true*/)
{
  Init();

  m_refCount	= 1;
  m_nbValues	= nbVal;
  m_dataValue	= data;
  m_allocated	= (nbVal > 0) && (makeCopy || (data == NULL));
  if (m_allocated)
  {
    m_dataValue	= NewValue(nbVal);
    if (data != NULL)
    {
	    memcpy(m_dataValue, data, sizeof(*m_dataValue) * nbVal);
    }
  }
}
//----------------------------------------

CExpressionValue::CValueData::CValueData(const CDoubleArray& data)
{
  Init();

  m_refCount	= 1;
  m_nbValues	= data.size();
  m_allocated	= (m_nbValues > 0);
  if (m_allocated)
  {
    m_dataValue	= NewValue(m_nbValues);
    std::copy(data.begin(), data.end(), m_dataValue);
  }
}

//----------------------------------------

CExpressionValue::CValueData::CValueData(const CExpressionValue::CValueData& copy)
{
  Init();

  Set(copy);

//  m_refCount	= 1;
//  m_nbValues	= copy.m_nbValues;
//  m_value	= NULL;
//  m_allocated	= (m_nbValues > 0);
//  if (m_allocated)
//  {
//    m_value	= NewValue(m_nbValues);
//    memcpy(m_value, copy.m_value, sizeof(*m_value) * m_nbValues);
//  }
}

//----------------------------------------

CExpressionValue::CValueData::~CValueData()
{
  DeleteDataValue();
}

//----------------------------------------
void CExpressionValue::CValueData::DeleteDataValue()
{
  if (m_dataValue == NULL)
  {
    return;
  }

  if (m_allocated)
  {
    delete []m_dataValue;
    m_dataValue = NULL;
  }
}

//----------------------------------------

void CExpressionValue::CValueData::Init()
{
  m_dataValue = NULL;
}

//----------------------------------------

void CExpressionValue::CValueData::Set(const CExpressionValue::CValueData &copy)
{
  DeleteDataValue();

  m_refCount	= 1;
  m_nbValues	= copy.m_nbValues;
  m_dataValue	= NULL;
  m_allocated	= (m_nbValues > 0);
  if (m_allocated)
  {
    m_dataValue	= NewValue(m_nbValues);
    memcpy(m_dataValue, copy.m_dataValue, sizeof(*m_dataValue) * m_nbValues);
  }
}
//----------------------------------------

CExpressionValue::CValueData& CExpressionValue::CValueData::operator=(const CExpressionValue::CValueData &copy)
{
  if (this == &copy)
  {
    return *this;
  }

  Set(copy);

  return *this;
}

//----------------------------------------
double* CExpressionValue::CValueData::NewValue(size_t nbValues)
{
  double* values = NULL;
  try
  {
    values = new double[nbValues];   
  }
  catch (CException& e)
  {
    throw CException(CTools::Format("ERROR in CExpressionValue::ValueData::NewValue. Reason: %s\n",
					  e.what()),
            BRATHL_ERROR);
  }
  catch (std::bad_alloc& e) // If memory allocation (new) failed...
  {
    throw CMemoryException(CTools::Format("ERROR in CValueData::NewValue - Unable to allocate new memory to store the result.\nNative error: '%s'\n"
                                           " or try to allocate more than your system architecture can ever handle",
                                  e.what()));
  }
  catch (...)
  {
    throw CMemoryException(CTools::Format("ERROR in CValueData::NewValue. It may be that it does not remain enough memory to allocate %ld values"
                                           " or try to allocate more than your system architecture can ever handle",
					  (long)nbValues));
  }


  if (values == NULL)
  {
    throw CMemoryException(CTools::Format("ERROR in CValueData::NewValue. It may be that it does not remain enough memory to allocate %ld values"
                                           " or try to allocate more than your system architecture can ever handle",
					  (long)nbValues));

  }
  return values;

}

//----------------------------------------
void CExpressionValue::CValueData::Dump(std::ostream	&fOut /* = std::cerr */)
{
  if (! CTrace::IsTrace())
  {
    return;
  }

  fOut << "==> Dump a CExpressionValue::CValueData Object at "<< this << std::endl;

  fOut << "m_refCount: "<< m_refCount << std::endl; 
  fOut << "m_nbValues: "<< m_nbValues << std::endl; 
  fOut << "m_allocated: "<< m_allocated << std::endl; 
  fOut << "m_dataValue: ";
  if (m_dataValue != NULL)
  {
    fOut << std::endl;
    for (uint32_t i = 0 ; i < m_nbValues ; i++)
    {
      fOut << "m_dataValue[" << i << "] = " << m_dataValue[i] << std::endl;
    }
  }
  else
  {
    fOut << "NULL" << std::endl;
  }
  fOut << "==> END Dump a CExpressionValue::CValueData Object at "<< this << std::endl;
}
//-------------------------------------------------------------
//------------------- CExpressionValue class --------------------
//-------------------------------------------------------------

//----------------------------------------

CExpressionValue::CExpressionValue
		(double		FloatValue /*= CTools::m_defaultValueDOUBLE*/)
{
  Init();
  ExpressionValueDimensions	Dimensions;
  BuildValue(FloatType, Dimensions, &FloatValue);
}

//----------------------------------------

CExpressionValue::CExpressionValue(const std::vector<double>	&FloatValues)
{
  Init();
  BuildFromVector(FloatValues);
}

//----------------------------------------

CExpressionValue::CExpressionValue
		(ExpressionValueType		Type,
		 ExpressionValueDimensions	&Dimensions,
		 double				*Value,
		 bool				MakeCopy /*= true*/)
{
  Init();
  BuildValue(Type, Dimensions, Value, MakeCopy);
}
//----------------------------------------

CExpressionValue::CExpressionValue(ExpressionValueType type, ExpressionValueDimensions &dimensions, const CDoubleArray& value)
{
  Init();
  BuildValue(type, dimensions, value);
}

//----------------------------------------

CExpressionValue::CExpressionValue(const std::string& StrValue)
{
  Init();
  BuildFromString(StrValue);
}


//----------------------------------------

CExpressionValue::CExpressionValue(const CExpressionValue &copy)
{
  Init();

  Set(copy);

//  m_Type	= Copy.m_Type;
//  m_Dimensions	= Copy.m_Dimensions;
//  Copy.m_Value->m_refCount++;
//  m_Value	= Copy.m_Value;
}
//----------------------------------------

CExpressionValue::CExpressionValue
		(ExpressionCallableFunction1	&Function,
		 bool				IsNumeric,
		 CExpressionValue		&Parameter1)
{
  Init();

  BuildValue(Parameter1.m_Type, Parameter1.m_Dimensions);
  
  for (uint32_t index=0; index<m_Value->m_nbValues; index++)
  {
    m_Value->m_dataValue[index]	= Function(Parameter1.m_Value->m_dataValue[index]);
  }
  if (IsNumeric)
    m_Type	= FloatType;
}

//----------------------------------------

CExpressionValue::CExpressionValue
		(ExpressionCallableFunctionStrToStr1	&Function,
		 CExpressionValue			&Parameter1)
{
  Init();
  
  m_Type	= CharacterType;
  
  BuildFromString(Function(Parameter1.GetString().c_str()));
}

////----------------------------------------
//CExpressionValue::CExpressionValue(ExpressionCallableFunctionAlgoN& function, const std::string& functionName, const std::string& parametersFormat, CObList* listParams)
//{
//  Init();
//
//
//  //ExpressionValueDimensions	Dimensions;
//  //double Result	= function(functionName.c_str(), parametersFormat.c_str(), listParams);
//  //
//
//  //BuildValue(CharacterType, Dimensions, &Result);
//}
//----------------------------------------

CExpressionValue::CExpressionValue(ExpressionCallableFunctionAlgoN& function, const char* functionName, CVectorBratAlgorithmParam& arg)
{
  Init();

  //va_list args;
  //va_start( args, parametersFormat );

	//CBratObject* o = va_arg(args, CBratObject *);
 // o->Dump(std::cout);
	//o = va_arg(args, CBratObject *);
 // o->Dump(std::cout);
	//o = va_arg(args, CBratObject *);
 // o->Dump(std::cout);

  double Result	= function(functionName, arg);
  //double Result	= function(functionName.c_str(), parametersFormat.c_str(), new CExpressionValue(10), new CExpressionValue(11), new CExpressionValue(12));
  
  ExpressionValueDimensions	Dimensions;
  BuildValue(CharacterType, Dimensions, &Result);
}
//----------------------------------------

CExpressionValue::CExpressionValue(ExpressionCallableFunctionBratAlgoBaseN& function, CBratAlgorithmBase* algo, CVectorBratAlgorithmParam& arg)
{
  Init();

  //va_list args;
  //va_start( args, parametersFormat );

	//CBratObject* o = va_arg(args, CBratObject *);
 // o->Dump(std::cout);
	//o = va_arg(args, CBratObject *);
 // o->Dump(std::cout);
	//o = va_arg(args, CBratObject *);
 // o->Dump(std::cout);

  double Result	= function(algo, arg);
  //double Result	= function(functionName.c_str(), parametersFormat.c_str(), new CExpressionValue(10), new CExpressionValue(11), new CExpressionValue(12));
  
  ExpressionValueDimensions	Dimensions;
  BuildValue(FloatType, Dimensions, &Result);
}
//----------------------------------------

CExpressionValue::CExpressionValue
		(ExpressionCallableFunctionStrToFlt1	&Function,
		 CExpressionValue			&Parameter1)
{
  Init();
  
  ExpressionValueDimensions	Dimensions;

  double Result	= Function(Parameter1.GetString().c_str());
  
  BuildValue(CharacterType, Dimensions, &Result);
}

//----------------------------------------

CExpressionValue::CExpressionValue
		(ExpressionCallableFunction2	&Function,
		 bool				IsNumeric,
		 CExpressionValue		&Parameter1,
		 CExpressionValue		&Parameter2)
{
  Init();

  SetDimensionsCombination(Parameter1, Parameter2, Parameter2);
  
  double* Current1 = Parameter1.m_Value->m_dataValue;
  double* Current2 = Parameter2.m_Value->m_dataValue;

  for (uint32_t index=0; index<m_Value->m_nbValues; index++)
  {
    m_Value->m_dataValue[index]	= Function(*Current1, *Current2);
    if (Parameter1.m_Dimensions.size() != 0) Current1++;
    if (Parameter2.m_Dimensions.size() != 0) Current2++;
  }
  if (IsNumeric ||
      (Parameter1.m_Type == FloatType) ||
      (Parameter2.m_Type == FloatType))
    m_Type	= FloatType;
}

//----------------------------------------

CExpressionValue::CExpressionValue
		(ExpressionCallableFunction3	&Function,
		 bool				IsNumeric,
		 CExpressionValue		&Parameter1,
		 CExpressionValue		&Parameter2,
		 CExpressionValue		&Parameter3)
{
  Init();
  
  SetDimensionsCombination(Parameter1, Parameter2, Parameter3);
  
  double* Current1	= Parameter1.m_Value->m_dataValue;
  double* Current2	= Parameter2.m_Value->m_dataValue;
  double* Current3	= Parameter3.m_Value->m_dataValue;

  for (uint32_t index=0; index<m_Value->m_nbValues; index++)
  {
    m_Value->m_dataValue[index]	= Function(*Current1, *Current2, *Current3);
    if (Parameter1.m_Dimensions.size() != 0) Current1++;
    if (Parameter2.m_Dimensions.size() != 0) Current2++;
    if (Parameter3.m_Dimensions.size() != 0) Current3++;
  }
  if (IsNumeric ||
      (Parameter1.m_Type == FloatType) ||
      (Parameter2.m_Type == FloatType) ||
      (Parameter3.m_Type == FloatType))
    m_Type	= FloatType;
}
//----------------------------------------

CExpressionValue::~CExpressionValue()
{
    DeleteValue();
}
//----------------------------------------
CExpressionValue* CExpressionValue::GetExpressionValue(CBratObject* ob, bool withExcept /*= true*/)
{
  CExpressionValue* exprValue = dynamic_cast<CExpressionValue*>(ob);
  if (withExcept)
  {
    if (exprValue == NULL)
    {
      CException e("CExpressionValue::GetExpressionValue - dynamic_cast<CExpressionValue*>(ob) returns NULL"
                   "object seems not to be an instance of CExpressionValue",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return exprValue;

}
//----------------------------------------
bool CExpressionValue::HasValue()
{
  return (m_Value != NULL);
}
//----------------------------------------
void CExpressionValue::DeleteValue()
{
  if (m_Value == NULL)
  {
    return;
  }

  m_Value->m_refCount--;

  if (m_Value->m_refCount == 0)
  {
    delete m_Value;
    m_Value = NULL;
    
    m_Dimensions.RemoveAll();
  }
}

//----------------------------------------

void CExpressionValue::Init()
{
  m_Value = NULL;
}

//----------------------------------------

void CExpressionValue::Set(const CExpressionValue &copy)
{
  m_Type = copy.m_Type;
  m_name = copy.m_name;
  m_Dimensions = copy.m_Dimensions;
  
  DeleteValue();
  m_Value	= new CExpressionValue::CValueData(*copy.m_Value);
}

//----------------------------------------
CExpressionValue& CExpressionValue::operator=(const CExpressionValue &Copy)
{
  if (this == &Copy)
  {
    return *this;
  }

  Set(Copy);

//  m_Type	= Copy.m_Type;
//  m_Dimensions	= Copy.m_Dimensions;
//  Copy.m_Value->m_refCount++;		// Protect against "val=val"
//  DeleteValue();
//  m_Value	= Copy.m_Value;
  return *this;
}

//----------------------------------------

CExpressionValue& CExpressionValue::operator=(const std::string &String)
{
  DeleteValue();
  m_Value = NULL;
  BuildFromString(String);
  return *this;
}

//----------------------------------------

CExpressionValue& CExpressionValue::operator=(double value)
{
  DeleteValue();
  m_Value = NULL;
  ExpressionValueDimensions	Dimensions;
  BuildValue(FloatType, Dimensions, &value);
  return *this;
}


//----------------------------------------

CExpressionValue& CExpressionValue::operator=(const std::vector<double> &Vector)
{
  DeleteValue();
  m_Value = NULL;
  BuildFromVector(Vector);
  return *this;
}

//----------------------------------------
void CExpressionValue::SetNewValue(ExpressionValueType	type, uint32_t* dims, uint32_t nbDims, double	*value, bool makeCopy)
{
  DeleteValue();
  m_Value = NULL;
  BuildValue(type, dims, nbDims, value, makeCopy);
}

//----------------------------------------

void CExpressionValue::SetNewValue(ExpressionValueType type, ExpressionValueDimensions& dimensions, double *value, bool makeCopy /*= true*/)
{
  DeleteValue();
  m_Value = NULL;
  BuildValue(type, dimensions, value, makeCopy);
}
//----------------------------------------
void CExpressionValue::SetNewValue(CDoubleArray& vect, bool	makeCopy /* = true */)
{
    UNUSED(makeCopy);

  CUIntArray dimension;
  dimension.Insert(vect.size());

  DeleteValue();
  m_Value = NULL;
  BuildValue(FloatType, dimension, vect);
  
}
//----------------------------------------
void CExpressionValue::SetNewValue(double* dataValue, uint32_t nbValues, bool	makeCopy /* = true */)
{
    UNUSED(makeCopy);

  CUIntArray dimension;
  dimension.Insert(nbValues);

  DeleteValue();
  m_Value = NULL;
  BuildValue(FloatType, dimension, dataValue);
  
}

//----------------------------------------
void CExpressionValue::SetNewValue(CObDoubleMap& mp, bool	makeCopy /* = true */)
{
  CUIntArray dimension;
  dimension.Insert(mp.size());

  SetNewValue(FloatType, dimension, NULL, makeCopy);
  
  double* data = GetValues();

  if (data == NULL)
  {
    return;
  }
  
  uint32_t nbValues = GetNbValues();
  size_t nbDataToGet = mp.size();

  if (nbValues != nbDataToGet)
  {
    throw CException(CTools::Format("Error in CExpressionValue::SetNewValue(CObDoubleMap& map) - Defined dimension of array: %ld does not correspond to number of values to get : %ld",
					                              (long)nbValues,
					                              (long)nbDataToGet),
			                    BRATHL_LOGIC_ERROR);  
  }

  uint32_t iKey = 0;
  CObDoubleMap::iterator it;
  for (it = mp.begin() ; it != mp.end() ; it++)
  {
    data[iKey] = it->first;
    iKey++;
  }

}
//----------------------------------------

void CExpressionValue::SetNewValue(CDoublePtrDoubleMap& mp, bool	makeCopy /* = true */)
{
  CUIntArray dimension;
  dimension.Insert(mp.size());

  SetNewValue(FloatType, dimension, NULL, makeCopy);
  
  double* data = GetValues();

  if (data == NULL)
  {
    return;
  }
  
  uint32_t nbValues = GetNbValues();
  size_t nbDataToGet = mp.size();

  if (nbValues != nbDataToGet)
  {
    throw CException(CTools::Format("Error in CExpressionValue::SetNewValue(CDoublePtrDoubleMap& map) - Defined dimension of array: %ld does not correspond to number of values to get : %ld",
					                              (long)nbValues,
					                              (long)nbDataToGet),
			                    BRATHL_LOGIC_ERROR);  
  }

  uint32_t iKey = 0;
  CDoublePtrDoubleMap::iterator it;
  for (it = mp.begin() ; it != mp.end() ; it++)
  {
    data[iKey] = it->first;
    iKey++;
  }

}

//----------------------------------------

//void CExpressionValue::SetNewValue(ExpressionCallableFunctionAlgoN& function, const char* functionName, const char* parametersFormat, ...)
//{
//  DeleteValue();
//  m_Value = NULL;
//
//  va_list args;
//  va_start( args, parametersFormat );
//
// 	CBratObject* o = va_arg(args, CBratObject *);
//  CExpressionValue* v = GetExpressionValue(o);
//
//  ExpressionValueDimensions	Dimensions;
//  double Result	= function(functionName, parametersFormat, args);
//  //double Result	= function(functionName.c_str(), parametersFormat.c_str(), new CExpressionValue(10), new CExpressionValue(11), new CExpressionValue(12));
//  
//  BuildValue(CharacterType, Dimensions, &Result);
//}

//----------------------------------------

void CExpressionValue::BuildValue(ExpressionValueType	type, uint32_t* dims, uint32_t nbDims, double	*value /* = NULL */,
                            		  bool makeCopy /* = true */)
{
  ExpressionValueDimensions dimensions;
  dimensions.Insert(dims, nbDims);

  BuildValue(type, dimensions, value, makeCopy);
  
}
//----------------------------------------
void CExpressionValue::BuildValue(ExpressionValueType	Type, ExpressionValueDimensions	&Dimensions, double	*Value /* = NULL */,
                            		  bool	MakeCopy /* = true */)
{
  uint32_t	NbValues;
  DeleteValue();
  m_Value = NULL;

  m_Type	= Type;
  m_Dimensions	= Dimensions;
  
  if (Dimensions.size() <= 0)
  {
    NbValues	= 1;
  }
  else
  {
    NbValues = Dimensions.GetValuesProduct();
  }

/*
  NbValues	= 1;
  for (uint32_t index=0; index<Dimensions.size(); index++)
  {
    NbValues	*= Dimensions[index];
  }
  */
  m_Value	= new CValueData(NbValues, Value, MakeCopy);
}
//----------------------------------------
void CExpressionValue::BuildValue(ExpressionValueType type, ExpressionValueDimensions &dimensions, const CDoubleArray& value)
{
  DeleteValue();
  m_Value = NULL;

  m_Type	= type;
  m_Dimensions	= dimensions;
  m_Value	= new CValueData(value);
}

//----------------------------------------

void CExpressionValue::BuildFromString(const std::string &StrValue)
{
  //double			*Value;
  ExpressionValueDimensions	Dimensions;

  DeleteValue();
  m_Value = NULL;
  Dimensions.push_back(StrValue.size());
  BuildValue(CharacterType, Dimensions);

  for (uint32_t index=0; index<Dimensions[0]; index++)
  {
    m_Value->m_dataValue[index]	= StrValue[index];
  }
}

//----------------------------------------

void CExpressionValue::BuildFromVector
		(const std::vector<double>		&VectValue)
{
  //double			*Value;
  ExpressionValueDimensions	Dimensions;

  DeleteValue();
  m_Value = NULL;
  Dimensions.push_back(VectValue.size());
  BuildValue(FloatType, Dimensions);

  for (uint32_t index=0; index<Dimensions[0]; index++)
  {
    m_Value->m_dataValue[index]	= VectValue[index];
  }
}


//----------------------------------------

void CExpressionValue::CheckDimensionsCombination
		(const CExpressionValue		&parameter1,
		 const CExpressionValue		&parameter2,
		 const CExpressionValue		&parameter3,
		 ExpressionValueDimensions	&Dimensions)
{

  CExpressionValue::CheckDimensionsCombination(parameter1.m_Dimensions, parameter2.m_Dimensions, parameter3.m_Dimensions, Dimensions);
  
/*
  const CExpressionValue	*Params[3]	= { &Parameter1,
						    &Parameter2,
						    &Parameter3 };
  int32_t	index;

  Dimensions.clear();
  for (index=0; index<3; index++)
  {
    if (Dimensions.size() < Params[index]->m_Dimensions.size())
      Dimensions	= Params[index]->m_Dimensions;
  }

  if (Dimensions.size() == 0)
    return;

  if (Dimensions.size() == 1)
  {
    for (index=0; index<3; index++)
    {
      if ((Params[index]->m_Dimensions.size() == 1) &&
	  (Params[index]->m_Dimensions[0] < Dimensions[0]))
      {
	// Minimal size of std::vector
	Dimensions[0] = Params[index]->m_Dimensions[0];
      }
    }
    return;
  }

  for (index = 0; index<3; index++)
  {
    if (Params[index]->m_Dimensions.size() != 0)
    {
      if (Params[index]->m_Dimensions.size() != Dimensions.size())
	throw CExpressionException("Combinated matrixes must have the same number of dimensions",
				   BRATHL_INCONSISTENCY_ERROR);
      
      for (uint32_t IndexDims=0; IndexDims < Dimensions.size(); IndexDims++)
      {
	if (Dimensions[IndexDims] != Params[index]->m_Dimensions[IndexDims])
	  throw CExpressionException("Combinated matrixes must have the same dimensions",
				     BRATHL_INCONSISTENCY_ERROR);
      }
    }
  }
  */
}
//----------------------------------------
void CExpressionValue::CheckDimensionsCombination(const CUIntArray	&dim1, const CUIntArray	&dim2, const CUIntArray	&dim3, CUIntArray& dimensions)
{
  const CUIntArray* params[3]	= { &dim1, &dim2, &dim3 };
  
  CExpressionValue::CheckDimensionsCombination(params, 3, dimensions);
}
//----------------------------------------
void CExpressionValue::CheckDimensionsCombination(const CUIntArray** params, uint32_t nbParams, CUIntArray& dimensions)
{
    UNUSED(nbParams);

  int32_t	index;

  dimensions.clear();
  for (index = 0; index < 3; index++)
  {
    if (dimensions.size() < params[index]->size())
    {
      dimensions	= *(params[index]);
    }
  }

  if (dimensions.size() == 0)
  {
    return;
  }

  if (dimensions.size() == 1)
  {
    for (index = 0; index < 3; index++)
    {
      if ((params[index]->size() == 1) &&(params[index]->at(0) < dimensions.at(0)))
      {
	      // Minimal size of std::vector
	      dimensions[0] = params[index]->at(0);
      }
    }
    return;
  }

  for (index = 0; index < 3; index++)
  {
    if (params[index]->size() != 0)
    {
      if (params[index]->size() != dimensions.size())
      {
        throw CExpressionException("Combinated matrixes must have the same number of dimensions",
				                           BRATHL_INCONSISTENCY_ERROR);
      }
      
      for (uint32_t indexDims = 0; indexDims < dimensions.size(); indexDims++)
      {
	      if (dimensions[indexDims] != params[index]->at(indexDims))
        {
	        throw CExpressionException("Combinated matrixes must have the same dimensions",
				     BRATHL_INCONSISTENCY_ERROR);
        }
      }
    }
  }
}

//----------------------------------------

void CExpressionValue::SetDimensionsCombination
		(const CExpressionValue &Parameter1,
		 const CExpressionValue &Parameter2,
		 const CExpressionValue &Parameter3)
{
  ExpressionValueDimensions	Dimensions;

  CExpressionValue::CheckDimensionsCombination(Parameter1,
			     Parameter2,
			     Parameter3,
			     Dimensions);
  BuildValue(Parameter1.m_Type, Dimensions);
}

//----------------------------------------


//----------------------------------------

size_t CExpressionValue::GetNbValues() const
{
	return m_Value->m_nbValues;
}

//----------------------------------------

double *CExpressionValue::GetValues() const
{
  return m_Value->m_dataValue;
}

//----------------------------------------

double CExpressionValue::GetValue(uint32_t index) const
{
  //double* data = GetValues();
  uint32_t nbValues = GetNbValues();
  if (index >= nbValues)
  {
    throw CExpressionException(CTools::Format("Inconsistent expression value (CExpressionValue): index is %d but number of values is %d", index, nbValues),
			       BRATHL_INCONSISTENCY_ERROR);

  }
  return m_Value->m_dataValue[index];
}

//----------------------------------------
double CExpressionValue::GetValue(uint32_t i, uint32_t j) const
{
  size_t nbDims = GetNbDimensions();

  if (nbDims != 2)
  {
    throw CExpressionException(CTools::Format("Inconsistent expression value call (CExpressionValue::GetValue(uint32_t i, uint32_t j)): expression value is not a 2-dimensions array: number of dimensions is %d", nbDims),
			       BRATHL_INCONSISTENCY_ERROR);

  }

  uint32_t index = (m_Dimensions.at(1) * i) + j;
  
  return GetValue(index);
}



//----------------------------------------

std::string CExpressionValue::GetString() const
{
  std::string Result;

  if ((GetType() == CharacterType) && (GetNbDimensions() <= 1))
  {
    double	*Values	= GetValues();

    for (size_t index=0; index < GetNbValues(); index++)
    {
      double Value	= Values[index];
      char ToPrint	= '?';
      
      if ((Value >= (numeric_limits_char::min)()) && (Value <= (numeric_limits_char::max)()))
      {
	      ToPrint	= static_cast<char>(Value);
      }

      Result	+= ToPrint;
    }
  }

  return Result;
}

//----------------------------------------

double CExpressionValue::Compare
		(CExpressionValue	&withWhat)
{
  ExpressionValueDimensions	dimensions;
  double diff;

  CExpressionValue::CheckDimensionsCombination(*this,
			     withWhat,
			     withWhat,
			     dimensions);

  uint32_t nbVal = 1;

  for (uint32_t index=0; index < dimensions.size(); index++)
  {
    nbVal	*= dimensions[index];
  }

  double* current1	= m_Value->m_dataValue;
  double* current2	= withWhat.m_Value->m_dataValue;
  double result	= 0.0;

  while ((result == 0.0) && (nbVal-- > 0))
  {
    //diff	= CTools::Minus(*current1, *current2);
    diff	= (*current1 - *current2);

    if (isDefaultValue(diff))
    {
      result	= diff;
    }
    else if (diff < -CTools::m_CompareEpsilon)
    {
      result	= -1.0;
    }
    else if (diff > CTools::m_CompareEpsilon)
    {
      result	= 1.0;
    }
    
    if (m_Dimensions.size() != 0)
    {
      current1++;
    }
    if (withWhat.m_Dimensions.size() != 0)
    {
      current2++;
    }
  }

  if ((result != 0.0) ||
      (m_Dimensions.size() == 0) ||
      (withWhat.m_Dimensions.size() == 0))
  {
    return result;
  }

  // Begining is identical, check the lengths
  if (m_Value->m_nbValues > withWhat.m_Value->m_nbValues)
  {
    return 1.0;
  }
  if (m_Value->m_nbValues < withWhat.m_Value->m_nbValues)
  {
    return -1.0;
  }

  return 0.0;
}

//----------------------------------------

// Return: 1: true; 0: False; -1: unknown (default values)
int32_t CExpressionValue::IsTrue
		()
{
  uint32_t	index;
  if (m_Value->m_nbValues == 0)
    return 0; // No value is false

  if (m_Type == CharacterType)
    return 1; // A non empty std::string is true

  if (m_Value->m_nbValues > 1)
  {
    throw CExpressionException("More than a single value in a test is an error",
			       BRATHL_INCONSISTENCY_ERROR);
  }

  for (index = 0; index < m_Value->m_nbValues; index++)
  {
    if (isDefaultValue(m_Value->m_dataValue[index]))
      return -1;
    if (isZero(m_Value->m_dataValue[index]))
      return 0;
  }
  return 1;
}

//----------------------------------------

std::string CExpressionValue::AsString(const CUnit &Unit	/*= ""*/, const std::string Format	/*= ""*/, bool dateAsPeriod /*= false*/) const
{
  std::string	Result;

  double	*Values	= GetValues();
  
  uint32_t	index;
  
  //-------------------
  // Value is character
  //-------------------
  if (GetType() == CharacterType)
  {
    //std::numeric_limits<char> CLimits;
    Result	= '"';
    for (index=0; index < GetNbValues() ; index++)
    {
      double Value	= Values[index];
      char ToPrint	= '?';

      if ((Value >= (numeric_limits_char::min)()) && (Value <= (numeric_limits_char::max)()))
      {
	      ToPrint	= static_cast<char>(Value);
      }

      if (ToPrint == '"')
      {
	      Result	+= "\"\"";
      }
      else
      {
	      Result	+= ToPrint;
      }
    }

    Result	+= '"';

    //--------------
    return Result;
    //--------------
  }
 
  //-------------------
  // Value is a date
  //-------------------
  if (Unit.IsDate() && !dateAsPeriod)
  {
    if (GetNbValues() != 1)
    {
      Result.append("[ ");
    }

    for (index=0; index < GetNbValues(); index++)
    {
      if (index != 0)
      {
	      Result.append(", ");
      }

      if (isDefaultValue(Values[index]))
      {
        Result.append("DV");
      }
      else
      {

        CDate date(Values[index]);          

        std::string fmtDate = Format;
        
        int32_t precision = CTools::StrToInt32(fmtDate);

        if ( ! isDefaultValue(precision) )
        {
          fmtDate = "";
        }

	      Result.append(date.AsString(fmtDate, true));
      }
    }

    if (GetNbValues() != 1)
    {
      Result.append(" ]");
    }

    //--------------
    return Result;
    //--------------
  }
  //-------------------
  // Value is a double
  //-------------------
  CUnit	WantedUnit = Unit;
  WantedUnit.SetConversionFromBaseUnit();
  if (GetNbValues() != 1)
  {
    Result.append("[ ");
  }
  for (index=0; index<GetNbValues(); index++)
  {
    if (index != 0)
    {
	    Result.append(", ");
    }
    if (isDefaultValue(Values[index]))
    {
      Result.append("DV");
    }
    else
    {
      //Result.append(CTools::TrailingZeroesTrim(CTools::Format(128, "%1.17g",
			//				   WantedUnit.Convert(Values[index]))));

      if (Format.empty())
      {
        Result.append(CTools::TrailingZeroesTrim(CTools::DoubleToStr(WantedUnit.Convert(Values[index]))));
      }
      else
      {
        int32_t precision = CTools::StrToInt32(Format);
        if ( ! isDefaultValue(precision) )
        {
          Result.append(CTools::TrailingZeroesTrim(CTools::DoubleToStr(WantedUnit.Convert(Values[index]), precision)));
        }
        else
        {
          Result.append(CTools::TrailingZeroesTrim(CTools::Format(128, Format.c_str(),
					  		   WantedUnit.Convert(Values[index]))));
        }
      }
    }
  }
  if (GetNbValues() != 1)
  {
    Result.append(" ]");
  }

  return Result;
}

//----------------------------------------
std::string CExpressionValue::GetDimensionsAsString()
{
  if (m_Dimensions.size() <= 0)
  {
    return "";
  }

  std::string str = "[";
  for (uint32_t i = 0 ; i < m_Dimensions.size() ; i++)
  {
    str += CTools::Format(50, "%d,", m_Dimensions[i]);
  }
  
  str.erase(str.size()-1);

  str.append("]");

  return str;
}
//----------------------------------------
void CExpressionValue::Dump(std::ostream	&fOut /* = std::cerr */)
{
  if (! CTrace::IsTrace())
  {
    return;
  }

  fOut << "==> Dump a CExpressionValue Object at "<< this << std::endl;
  fOut << "m_name: "<< m_name << std::endl; 
  fOut << "m_Type: "<< m_Type << std::endl; 
  fOut << "m_Dimensions: " << std::endl;
  m_Dimensions.Dump(fOut);

  fOut << "m_Value: " << std::endl;
  if (m_Value != NULL)
  {
    m_Value->Dump(fOut);
  }
  else
  {
    fOut << "NULL" << std::endl;
  }

  fOut << "==> END Dump a CExpressionValue Object at "<< this << std::endl;

  
}

//=========================================================================

//-------------------------------------------------------------
//------------------- CExpressionValues class --------------------
//-------------------------------------------------------------
 
CExpressionValues::CExpressionValues(CObArray* expressionDataValues /* = NULL */)
{
  Init();
  m_expressionDataValues = expressionDataValues;
}

//----------------------------------------

CExpressionValues::~CExpressionValues()
{
  DeleteExpressionDataValues(); 
}

//----------------------------------------
void CExpressionValues::Init()
{
  m_expressionDataValues = NULL;
}

//----------------------------------------
void CExpressionValues::SetDimensions(uint32_t i)
{
  m_dimensions.RemoveAll();
  m_dimensions.Insert(i);

}
//----------------------------------------
void CExpressionValues::SetDimensions(uint32_t i, uint32_t j)
{
  m_dimensions.RemoveAll();
  m_dimensions.Insert(i);
  m_dimensions.Insert(j);

}
//----------------------------------------
void CExpressionValues::SetDimensions(const CUIntArray& dimensions)
{
  m_dimensions = dimensions;
}

//----------------------------------------
size_t CExpressionValues::GetNbDimensions() const
{
  if (m_dimensions.size() <= 0)
  {
    if (m_expressionDataValues != NULL)
    {
      return m_expressionDataValues->size();
    }
    else
    {
      return 0;
    }
  }
  else
  {
    return m_dimensions.size(); 
  }
}
//----------------------------------------
uint32_t CExpressionValues::GetDimensions(uint32_t indexDim) const
{
  if (m_dimensions.size() <= 0)
  {
    if (indexDim > 0)
    {
      throw CException(CTools::Format("ERROR in CExpressionValues::GetDimensions - CExpressionValues has no dimension at index %ld", 
                                  (long)indexDim),
                   BRATHL_LOGIC_ERROR);
    }
    else
    {
      if (m_expressionDataValues != NULL)
      {
        return m_expressionDataValues->size();
      }
      else
      {
        return 0;
      }

    }
  }
  if (indexDim >= m_dimensions.size())
  {
    throw CException(CTools::Format("ERROR in CExpressionValues::GetDimensions - CExpressionValues has no dimension at index %ld (dimension size is %ld)", 
                                (long)indexDim, (long)m_dimensions.size()),
                  BRATHL_LOGIC_ERROR);
  }

  return m_dimensions.at(indexDim);
}
//----------------------------------------
void CExpressionValues::DeleteExpressionDataValues()
{

  if (m_expressionDataValues != NULL)
  {
    delete m_expressionDataValues;
    m_expressionDataValues = NULL;

    m_dimensions.RemoveAll();
  }
 
  
}
//----------------------------------------
double CExpressionValues::GetExpressionValueAsDouble(uint32_t index /* = 0 */)
{
  double value;
  setDefaultValue(value);

  if (m_expressionDataValues == NULL)
  {
    return value;
  }

  uint32_t nExprValues = static_cast<uint32_t>(m_expressionDataValues->size());
  if (index >= nExprValues)
  {
    return value;
  }
  
  CExpressionValue* exprValue = CExpressionValue::GetExpressionValue(m_expressionDataValues->at(index));
  if (exprValue == NULL)
  {
    return value;
  }

  uint32_t nbValues = exprValue->GetNbValues();
  if (nbValues <= 0)
  {
    return value;
  }
  return exprValue->GetValue(0);
}

//----------------------------------------
double CExpressionValues::GetExpressionValueAsDouble(uint32_t i, uint32_t j)
{
  uint32_t index = (m_dimensions.at(1) * i) + j;
  
  return GetExpressionValueAsDouble(index);
}

//----------------------------------------
void CExpressionValues::GetExpressionDataValuesAsArrayOfSingleValue(uint32_t index, double*& values, uint32_t& nbValues)
{
    UNUSED(index);

  if (m_expressionDataValues == NULL)
  {
    return;
  }

  nbValues = m_expressionDataValues->size();

  if (nbValues <= 0)
  {
    return;
  }

  values = new double[nbValues];

  for (uint32_t i = 0 ; i < nbValues ; i++)
  {
    values[i] = this->GetExpressionValueAsDouble(i);
  }

}
//----------------------------------------
CExpressionValues* CExpressionValues::GetExpressionValues(CBratObject* ob, bool withExcept /*= true*/)
{
  CExpressionValues* exprValue = dynamic_cast<CExpressionValues*>(ob);
  if (withExcept)
  {
    if (exprValue == NULL)
    {
      CException e("CExpressionValues::GetExpression - dynamic_cast<CExpressionValues*>(ob) returns NULL"
                   "object seems not to be an instance of CExpressionValues",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return exprValue;

}
//----------------------------------------
void CExpressionValues::Dump(std::ostream	&fOut /* = std::cerr */)
{
  if (! CTrace::IsTrace())
  {
    return;
  }

  fOut << "==> Dump a CExpressionValues Object at "<< this << std::endl;
  fOut << "m_expressionDataValues: " << std::endl;
  if (m_expressionDataValues != NULL)
  {
    m_expressionDataValues->Dump(fOut);
  }
  else
  {
    fOut << "NULL" << std::endl;
  }

  fOut << "==> END Dump a CExpressionValues Object at "<< this << std::endl;

  
}
//=========================================================================

//-------------------------------------------------------------
//------------------- CCompiler class --------------------
//-------------------------------------------------------------


CCompiler::CCompiler()
{
}


//----------------------------------------

void CCompiler::Compile
		(const std::string& text,
		 CUIntArray* code,
		 CObArray* constants,
		 CStringArray* fieldNames,
     CVectorBratAlgorithm* algoArray)
{

  m_followOperator = true;
  m_iifCount = 0;
  m_current = 0;
  m_text = text;
  m_code = code;
  m_constants = constants;
  m_fieldNames = fieldNames;
  m_algoArray = algoArray;

  while (! m_operators.empty())
  {
    m_operators.pop();
  }
  
  m_code->RemoveAll();
  m_constants->RemoveAll();
  m_fieldNames->RemoveAll();

  GetToken();

  if (m_token == TOKEN_END)
  {
    throw CExpressionException("Empty expression is invalid",
			       BRATHL_SYNTAX_ERROR,
			       m_text);
  }

  Expression();

  PopAllOp();

  if (m_token != TOKEN_END)
  {
    throw CExpressionException("Expression not correctly terminated",
			       BRATHL_SYNTAX_ERROR,
			       m_text);
  }

}

//----------------------------------------

void CCompiler::GetToken()
{
  uint32_t beginning;
  uint32_t current;
  bool atEnd;
  std::string buffer;
  uint32_t index;
  std::string localText; // Text beginning at m_current and a sentinnel at the end.
				   // Not suitable for printing/messages

  atEnd		= false;
  m_token	= 0;
  m_tokenId	= 0;
  current	= 0;
  localText	= m_text.substr(m_current) + m_charEnd;

  while (! atEnd)
  {
    atEnd	= true;
    beginning	= current;
    switch (CTools::ToUpper(localText[current]))
    {
    //-----------------------------
	  case ' ':
	  case '\t':
	  case '\n':
	  case '\r':
    //-----------------------------
      current++;
		  atEnd	= false;
		  break;

    //-----------------------------
	  case m_charEnd:
    //-----------------------------
		  m_token = TOKEN_END;
		  break;

    //-----------------------------
	  case '(':
    //-----------------------------
		  m_token = TOKEN_LEFTPAR;
		  current++;
		  break;

    //-----------------------------
	  case ')':
    //-----------------------------
		  m_token = TOKEN_RIGHTPAR;
		  current++;
		  break;

    //-----------------------------
	  case ',':
    //-----------------------------
		  m_token = TOKEN_COMMA;
		  current++;
		  break;

    //WARNING: The order 0..9 MUST follow +/- or there is a risk of error

    //-----------------------------
    case '+':
	  case '-':
    //-----------------------------
		  m_token		= TOKEN_OPERATOR;
		  m_tokenId	= (localText[current] == '+' ? OP_PLUS : OP_MINUS);
		  current++;

		  if (! (m_followOperator && isdigit(localText[current])))
      {
		    break;
      }
      //-----------------------------
		  // WARNING, no break here is normal
      //-----------------------------

    //-----------------------------
	  case '0':
	  case '1':
	  case '2':
	  case '3':
	  case '4':
	  case '5':
	  case '6':
	  case '7':
	  case '8':
	  case '9':
    //-----------------------------
		  while (isdigit(localText[current]))
      {
		    current++;
      }

		  if (localText[current] == '.')
		  {
		    current++; // Skip dot
		    if (! isdigit(localText[current]))
        {
		      throw CExpressionException("Invalid number format: 1.0 if OK 1. is not",
					         BRATHL_SYNTAX_ERROR,
					         m_text);
        }
		  }

		  while (isdigit(localText[current]))
      {
		    current++;
      }

		  if ((localText[current]=='E') || (localText[current]=='e'))
		  {
		    current++;

		    if ((localText[current]=='+') || (localText[current]=='-'))
        {
		      current++;
        }
		    if (! isdigit(localText[current]))
        {
		      throw CExpressionException("Exponent part is missing",
					         BRATHL_SYNTAX_ERROR,
					         m_text);
        }

		    while (isdigit(localText[current]))
        {
		      current++;
        }
		  }

		  buffer	= localText.substr(beginning, current - beginning);
		  m_constants->Insert(new CExpressionValue(strtod(buffer.c_str(), NULL)));
		  m_token		= TOKEN_VALUE;
		  m_tokenId	= m_constants->size()-1;
		  break;

    //-----------------------------
    case '"':
    //-----------------------------
		  // String analysis. String double " like "ABC""DFE" means one inside the std::string: ABC"DEF
		  // No \" interpreted
		  buffer	= "";
		  do
		  {
		    current++;	// Skip '"'
		    while (localText[current] != m_charEnd)
		    {
		      buffer	+= localText[current];
		      if (localText[current] == '"')
          {
		        break;
          }
		      current++;
		    }

		    if (localText[current] == m_charEnd)
        {
		      throw CExpressionException("Unterminated std::string constant",
					         BRATHL_SYNTAX_ERROR,
					         m_text);
        }

		    current++;	// Skip '"'
		  
      } while (localText[current] == '"');

		  buffer.resize(buffer.size()-1); // Delete the last '"'
		  m_constants->Insert(new CExpressionValue(buffer));
		  m_token	= TOKEN_VALUE;
		  m_tokenId	= m_constants->size()-1;
		  break;

    //-----------------------------
	  case '*':
    //-----------------------------
		  m_token		= TOKEN_OPERATOR;
		  m_tokenId	= OP_MULTIPLY;
		  current++;
		  break;

    //-----------------------------
	  case '/':
    //-----------------------------
		  m_token		= TOKEN_OPERATOR;
		  m_tokenId	= OP_DIVIDE;
		  current++;
		  break;

    //-----------------------------
	  case '%':
    //-----------------------------
		  m_token		= TOKEN_OPERATOR;
		  m_tokenId	= OP_MODULO;
		  current++;
		  break;

    //-----------------------------
	  case '^':
    //-----------------------------
		  m_token		= TOKEN_OPERATOR;
		  m_tokenId	= OP_POWER;
		  current++;
		  break;

    //-----------------------------
	  case '!':
    //-----------------------------
		  m_token = TOKEN_OPERATOR;
		  if (localText[current+1] == '=')
		  {
		    m_tokenId	= OP_NE;
		    current	+= 2;
		  }
		  else
		  {
		    m_tokenId	= OP_NOT;
		    current++;
		  }
		  break;

    //-----------------------------
	  case '~':
    //-----------------------------
		  m_token 	= TOKEN_OPERATOR;
		  m_tokenId	= OP_BITNOT;
		  current++;
		  break;

    //-----------------------------
	  case '=':
    //-----------------------------
		  if (localText[current+1] != '=')
      {
		    throw CExpressionException(CTools::Format(
						       "Invalid character sequence '=%c', '==' is probably wanted",
						       localText[current+1]),
					       BRATHL_SYNTAX_ERROR,
					       m_text);
      }

		  m_token		= TOKEN_OPERATOR;
		  m_tokenId	= OP_EQ;
		  current		+= 2;
		  break;

    //-----------------------------
	  case '<':
    //-----------------------------
		  m_token		= TOKEN_OPERATOR;
		  if (localText[current+1] == '=')
		  {
		    m_tokenId	= OP_LE;
		    current	+= 2;
		  }
		  else
		  {
		    m_tokenId	= OP_LT;
		    current++;
		  }
		  break;

    //-----------------------------
	  case '>':
    //-----------------------------
		  m_token		= TOKEN_OPERATOR;
		  if (localText[current+1] == '=')
		  {
		    m_tokenId	= OP_GE;
		    current	+= 2;
		  }
		  else
		  {
		    m_tokenId	= OP_GT;
		    current++;
		  }
		  break;

    //-----------------------------
	  case '&':
    //-----------------------------
		  m_token		= TOKEN_OPERATOR;
		  current++;
		  if (localText[current] != '&')
		    m_tokenId	= OP_BITAND;
		  else
		  {
		    m_tokenId	= OP_AND;
		    current++;
		  }
		  break;

    //-----------------------------
	  case '|':
    //-----------------------------
		  m_token		= TOKEN_OPERATOR;
		  current++;
		  if (localText[current] != '|')
		    m_tokenId	= OP_BITOR;
		  else
		  {
		    m_tokenId	= OP_OR;
		    current++;
		  }
		  break;

    //-----------------------------
	  case 'A':
	  case 'B':
	  case 'C':
	  case 'D':
	  case 'E':
	  case 'F':
	  case 'G':
	  case 'H':
	  case 'I':
	  case 'J':
	  case 'K':
	  case 'L':
	  case 'M':
	  case 'N':
	  case 'O':
	  case 'P':
	  case 'Q':
	  case 'R':
	  case 'S':
	  case 'T':
	  case 'U':
	  case 'V':
	  case 'W':
	  case 'X':
	  case 'Y':
	  case 'Z':
    //-----------------------------
		  while ( isascii(localText[current]) &&
		          ( isalnum(localText[current]) ||
			        ( localText[current] == '_')  ||
			        ( localText[current] == '.') ) 
            )
      {
		    current++;
      }

		  buffer	= localText.substr(beginning, current-beginning);
		  if (buffer == "iif")
      {
		    m_token	= TOKEN_IIF;
      }
		  else if (buffer == "iif3")
      {
		    m_token	= TOKEN_IIF3;
      }
		  else
		  {
		    // Search for a name of a function with 1 parameter
		    m_token	= TOKEN_END;

		    for (index=0; index<NUMBER(Fonctions1); index++)
		    {
		      if (buffer == Fonctions1[index].Name)
		      {
		        m_token	= TOKEN_FUNCTION1;
		        m_tokenId = index;
		        break;
		      }
		    }

		    for (index=0; index<NUMBER(CharFonctions1); index++)
		    {
		      if (buffer == CharFonctions1[index].Name)
		      {
		        m_token	= TOKEN_CHARFUNC1;
		        m_tokenId = index;
		        break;
		      }
		    }

		    if (m_token == TOKEN_END)
		    {
		      // Search for a name of a function with 2 parameters
		      for (index=0; index<NUMBER(Fonctions2); index++)
		      {
		        if (buffer == Fonctions2[index].Name)
		        {
			        m_token		= TOKEN_FUNCTION2;
			        m_tokenId	= index;
			        break;
		        }
		      }
		    }

		    if (m_token == TOKEN_END)
		    {
		      // Search for a name of a function with 3 parameters
		      for (index=0; index<NUMBER(Fonctions3); index++)
		      {
		        if (buffer == Fonctions3[index].Name)
		        {
			        m_token		= TOKEN_FUNCTION3;
			        m_tokenId	= index;
			        break;
		        }
		      }
		    }

		    if (m_token == TOKEN_END)
		    {
		      // Search for a name of a function with N parameters
		      for (index=0; index<NUMBER(FonctionsAlgoN); index++)
		      {
		        if (buffer == FonctionsAlgoN[index].Name)
		        {
			        m_token		= TOKEN_FUNCTION_ALGO_N;
			        m_tokenId	= index;
			        break;
		        }
		      }
		    }


		    if (m_token == TOKEN_END)
		    {
		      // Search for a constant
		      for (index=0; index<NUMBER(Constants); index++)
		      {
		        if (buffer == Constants[index].Name)
		        {
			        m_constants->Insert(new CExpressionValue(Constants[index].Value));
			        m_token = TOKEN_VALUE;
			        m_tokenId	= m_constants->size()-1;
			        break;
		        }
		      }
		    }

		    if (m_token == TOKEN_END)
		    {
		      // It is a field name. Look for an already defined field
		      for (index=0; index<m_fieldNames->size(); index++)
		      {
		        if (buffer == (*m_fieldNames)[index])
		        {
			        m_token		= TOKEN_FIELD;
			        m_tokenId	= index;
			        break;
		        }
		      }
		      if (m_token == TOKEN_END)
		      {
		        // New field
		        m_fieldNames->push_back(buffer);
		        m_token	= TOKEN_FIELD;
		        m_tokenId = m_fieldNames->size()-1;
		      }
		    }
		  }
		  break;

    //-----------------------------
	  default:
    //-----------------------------
	          throw CExpressionException("Invalid character in expression '"+m_text.substr(beginning+m_current,10)+"...'",
					     BRATHL_SYNTAX_ERROR,
					     m_text);
    } // Switch
  } // While

  m_followOperator	= (m_token == TOKEN_LEFTPAR) || (m_token == TOKEN_OPERATOR);
  m_current	+= current;
}

//----------------------------------------

std::string CCompiler::GetAlgoTokenName()
{
  uint32_t beginning;
  uint32_t current;
  bool atEnd;
  std::string buffer;
  std::string localText; // Text beginning at m_current and a sentinnel at the end.
				   // Not suitable for printing/messages

  atEnd		= false;
  //m_token	= 0;
  //m_tokenId	= 0;
  current	= 0;
  localText	= m_text.substr(m_current) + m_charEnd;

  while (! atEnd)
  {
    atEnd	= true;
    beginning	= current;
    switch (CTools::ToUpper(localText[current]))
    {
    //-----------------------------
	  case ' ':
	  case '\t':
	  case '\n':
	  case '\r':
    //-----------------------------
      current++;
		  atEnd	= false;
		  break;

    //-----------------------------
	  case m_charEnd:
    //-----------------------------
		  //m_token = TOKEN_END;
		  break;

    //-----------------------------
    case '"':
    //-----------------------------
		  // String analysis. String double " like "ABC""DFE" means one inside the std::string: ABC"DEF
		  // No \" interpreted
		  buffer	= "";
		  do
		  {
		    current++;	// Skip '"'
		    while (localText[current] != m_charEnd)
		    {
		      buffer	+= localText[current];
		      if (localText[current] == '"')
          {
		        break;
          }
		      current++;
		    }

		    if (localText[current] == m_charEnd)
        {
		      throw CExpressionException("Unterminated std::string constant",
					         BRATHL_SYNTAX_ERROR,
					         m_text);
        }

		    current++;	// Skip '"'
		  
      } while (localText[current] == '"');

		  buffer.resize(buffer.size()-1); // Delete the last '"'
		  //m_constants->Insert(new CExpressionValue(buffer));
		  //m_token	= TOKEN_VALUE;
		  //m_tokenId	= m_constants->size()-1;
		  break;

    //-----------------------------
	  default:
    //-----------------------------
	          throw CExpressionException("Invalid algorithm name in expression '"+m_text.substr(beginning+m_current,10)+"...'",
					     BRATHL_SYNTAX_ERROR,
					     m_text);
    } // Switch
  } // While

  return buffer;
}


//----------------------------------------

// Remove an operator from the operator std::stack and add it to the code
// Compile time only

void CCompiler::PopOp()
{
  uint32_t	theOperator;
  theOperator	= m_operators.top();
  m_operators.pop();

  if (theOperator != OP_LEFTPAR)
  { // Don't store parenthesis. Since they are not useful transform
    // an infix form to a reverse polish form
    m_code->push_back(theOperator);
  }
}

//----------------------------------------

void CCompiler::PopAllOp()
{
  while (! m_operators.empty())
  {
    PopOp();
  }
}

//----------------------------------------

// Put an operator on the operator std::stack and remove the ones with less priority
void CCompiler::PushOp(uint32_t	theOperator)
{
  uint32_t     priority;
  uint32_t     topPriority;

  priority	= theOperator & OP_VALUEMASK;

  if (theOperator != OP_LEFTPAR)
  { // Nothing to remove from std::stack if left parenthesis

    // ASSERT: The following mecanism pops evrything until a left parenthesis
    //	       is found in case of a right parenthesis
    while (! m_operators.empty())
    {
      topPriority	= m_operators.top() & OP_VALUEMASK;

      if (topPriority > priority)
      {
	      break; // end of loop
      }

      PopOp();

      if (topPriority == priority)
      {
	      // We pop only the first with the same priority
	      break;
      }
    }
  }

  if (theOperator != OP_RIGHTPAR) // Right parenthesis is never pushed
  {
    m_operators.push(theOperator);
  }
}

//----------------------------------------

// Grammar: Expression ::= [ '!' ] Subexpression
void CCompiler::Expression()
{
  if ((m_token == TOKEN_OPERATOR) && (m_tokenId == OP_NOT))
  {
    PushOp(OP_NOT);
    GetToken();
  }

  SubExpression();
}

//----------------------------------------

void CCompiler::SubExpression()
{

// Grammar: SubExpression ::= [ '-' | '+' | '~' ] Element [ theOperator SubExpression ]


  if ((m_token == TOKEN_OPERATOR) &&
      (
	      (m_tokenId == OP_MINUS)	||
	      (m_tokenId == OP_PLUS)	||
	      (m_tokenId == OP_BITNOT)
      )
     )
  {
    if (m_tokenId == OP_MINUS)
    {
      PushOp(OP_UNARYMINUS);
    }
    else if (m_tokenId != OP_PLUS)
    {
      PushOp(m_tokenId);
    }

    GetToken();
  }

  Element();

  if (m_token == TOKEN_OPERATOR)
  {
    if ((m_tokenId == OP_NOT) || (m_tokenId == OP_BITNOT))
    {
      throw CExpressionException("Invalid unary operator '!' or '~' between two sub-expressions",
				 BRATHL_SYNTAX_ERROR,
				 m_text);
    }

    PushOp(m_tokenId);

    GetToken(); // Consummes operator

    SubExpression();
  }
}

//----------------------------------------

void CCompiler::Element()
{
  uint32_t	localOpCode;


// Grammar: Element	::= Field | Number | Function | {  '(' Expression ')' } | iifinst | iif3inst
// Grammar: Function	::= Function1 | Function2 | Function3
// Grammar: Function1	::= FunctionName1 '(' Expression ')'
// Grammar: Function2	::= FunctionName2 '(' Expression ',' Expression ')'
// Grammar: Function3	::= FunctionName3 '(' Expression ',' Expression ',' Expression ')'
// Grammar: FunctionAlgoN	::= FunctionNameN '(' Expression ',' Expression ',' Expression ', ...)'
// Grammar: FunctionName1 ::= name of function with 1 parameter
// Grammar: FunctionName2 ::= name of function with 2 parameters
// Grammar: FunctionName3 ::= name of function with 3 parameters
// Grammar: FunctionNameN ::= name of function with N parameters
// Grammar: Number	::= ['+' | '-' ] { digit }* [ '.' [ digit ]* ] [ 'E' [ '+' | '-' ] { digit }* ]
// Grammer: NOTE: A number is a basic token. Leading '+' or '-' is valid only
// Grammar:       after an opening parenthesis or an operator, otherwise it is
// Grammar:       seen as an operator followed by a positive number.
// Grammar: Digit	::= '0' | ... | '9'

  switch (m_token)
  {
  //-----------------------------
  case TOKEN_FIELD:
  //-----------------------------
		if ((m_tokenId & OP_VALUEMASK) != m_tokenId)
    {
      throw CExpressionException(CTools::Format("Oops, field number %d (0x%x) if far too big",
                                                m_tokenId, m_tokenId),
                                 BRATHL_LOGIC_ERROR);
    }

		m_code->push_back(OP_PUSHFIELD | m_tokenId);
		
    GetToken(); // Consummes field
		
    break;

  //-----------------------------
  case TOKEN_VALUE:
  //-----------------------------
		if ((m_tokenId & OP_VALUEMASK) != m_tokenId)
    {
      throw CExpressionException(CTools::Format("Oops, value number %d (0x%x) if far too big",
                                                m_tokenId, m_tokenId),
                                 BRATHL_LOGIC_ERROR);
    }

		m_code->push_back(OP_PUSHVALUE | m_tokenId);
		
    GetToken(); // Consummes field

    break;

  //-----------------------------
  case TOKEN_FUNCTION1:
  //-----------------------------
		if ((m_tokenId & OP_VALUEMASK) != m_tokenId)
    {
                  throw CExpressionException(CTools::Format("Oops, function1 number %d (0x%x) if far too big",
							    m_tokenId, m_tokenId),
					     BRATHL_LOGIC_ERROR);
    }

		localOpCode	= OP_FUNCTION1 | m_tokenId;
		
    GetToken(); // Consummes function name
		
    ScanFunctionParameters(1);
		
    m_code->push_back(localOpCode);
		
    break;

  //-----------------------------
  case TOKEN_CHARFUNC1:
  //-----------------------------
		if ((m_tokenId & OP_VALUEMASK) != m_tokenId)
    {
                  throw CExpressionException(CTools::Format("Oops, char function1 number %d (0x%x) if far too big",
							    m_tokenId, m_tokenId),
					     BRATHL_LOGIC_ERROR);
    }
		
    localOpCode	= OP_CHARFUNC1 | m_tokenId;
		
    GetToken(); // Consummes function name
		
    ScanFunctionParameters(1);
		
    m_code->push_back(localOpCode);

		break;

  //-----------------------------
  case TOKEN_FUNCTION2:
  //-----------------------------
		if ((m_tokenId & OP_VALUEMASK) != m_tokenId)
    {
                  throw CExpressionException(CTools::Format("Oops, function2 number %d (0x%x) if far too big",
							    m_tokenId, m_tokenId),
					     BRATHL_LOGIC_ERROR);
    }

		localOpCode	= OP_FUNCTION2 | m_tokenId;
		
    GetToken(); // Consummes function name
		
    ScanFunctionParameters(2);

		m_code->push_back(localOpCode);

    break;

  //-----------------------------
  case TOKEN_FUNCTION3:
  //-----------------------------
		if ((m_tokenId & OP_VALUEMASK) != m_tokenId)
    {
                  throw CExpressionException(CTools::Format("Oops, function3 number %d (0x%x) if far too big",
							    m_tokenId, m_tokenId),
					     BRATHL_LOGIC_ERROR);
    }

		localOpCode	= OP_FUNCTION3 | m_tokenId;

		GetToken(); // Consummes function name

		ScanFunctionParameters(3);

		m_code->push_back(localOpCode);
		break;

  //-----------------------------
  case TOKEN_FUNCTION_ALGO_N:
  //-----------------------------
    {
		if ((m_tokenId & OP_VALUEMASK) != m_tokenId)
    {
                  throw CExpressionException(CTools::Format("Oops, functionAlogN number %d (0x%x) if far too big",
							    m_tokenId, m_tokenId),
					     BRATHL_LOGIC_ERROR);
    }

    uint32_t tokenIdSaved = m_tokenId;

		localOpCode	= OP_FUNCTION_ALGO_N | m_tokenId;

    uint32_t algoBeginningParam = m_current;

		GetToken(); // Consummes function name

    // Get the algorithm name (first parameter)
    std::string algoName = GetAlgoFunctionName();

    // Find the algorithm in the registry and create a new instance of the algorithm
    CBratAlgorithmBase* algo = CBratAlgorithmBase::GetNew(algoName.c_str());
    if (algo == NULL) 
    {
      throw CExpressionException(CTools::Format("The algorithm '%s' doesn't exists or is not registered - Please check the algorithm name in your expression, perhaps it's wrong. Otherwise, please contact Brat Helpdesk.", algoName.c_str()),
					     BRATHL_LOGIC_ERROR,
					     m_text);

    }
    if (m_algoArray == NULL) 
    {
		  throw CExpressionException("ERROR - The algorithm array is NULL - Please contact Brat Helpdesk.",
					     BRATHL_LOGIC_ERROR,
					     m_text);

    }
    // Get the number of arguments of the 'exec' function : number of algo. param. + 1 for the name of the algorithm.
    uint32_t nbParameters = algo->GetNumInputParam() + 1; 
    
    CStringArray params;
    
    ScanFunctionParameters(nbParameters, &params);
    
    //CTrace::Tracer("=====> Params for " + algoName);
    //params.Dump();

    std::string algoExpression;
    algoExpression.append(FonctionsAlgoN[tokenIdSaved].Name);
    algoExpression.append(m_text.substr(algoBeginningParam, m_current - algoBeginningParam));

    // Save and set the algorithm expression 
    algo->SetAlgoExpression(algoExpression);
    algo->SetAlgoParamExpressions(params);
    //Insert algorithm instance.
    m_algoArray->Insert(algo, true);
    
    // Save the number of algorithm parameters     
	  m_constants->Insert(new CExpressionValue(nbParameters));
		m_code->push_back(OP_PUSHVALUE | (m_constants->size()-1));

		m_code->push_back(localOpCode);
    //m_code->Dump();
		break;
    }
  //-----------------------------
  case TOKEN_LEFTPAR:
  //-----------------------------
    PushOp(OP_LEFTPAR);
		GetToken(); // Consummes parenthesis
		Expression();

		if (m_token != TOKEN_RIGHTPAR)
    {
		  throw CExpressionException("A right parenthesis is missing",
					     BRATHL_SYNTAX_ERROR,
					     m_text);
    }

		PushOp(OP_RIGHTPAR);
		GetToken(); // Consummes parenthesis
		break;

  //-----------------------------
  case TOKEN_IIF:
  //-----------------------------
		GetToken(); // Consummes iif
		IifInst();
		break;

  //-----------------------------
  case TOKEN_IIF3:
  //-----------------------------
		GetToken(); // Consummes iif3
		Iif3Inst();
		break;

  //-----------------------------
  default:
  //-----------------------------
		throw CExpressionException("Syntax error in expression",
					   BRATHL_SYNTAX_ERROR,
					   m_text);
  }
}
//----------------------------------------

std::string CCompiler::GetAlgoFunctionName()
{
  uint32_t currentSaved = m_current;

  if (m_token != TOKEN_LEFTPAR)
  {
    throw CExpressionException("Left parenthesis is mandatory after a function name",
			       BRATHL_SYNTAX_ERROR,
			       m_text);
  }


  std::string functionName = GetAlgoTokenName(); // Consummes parenthesis

  m_current = currentSaved;

  return functionName;

}
//----------------------------------------
void CCompiler::ScanFunctionParameters(uint32_t	NbParameters)
{
  ScanFunctionParameters(NbParameters, NULL);
}
//----------------------------------------

void CCompiler::ScanFunctionParameters(uint32_t	NbParameters, CStringArray* params)
{
  uint32_t     indexParam;

  if (m_token != TOKEN_LEFTPAR)
  {
    throw CExpressionException("Left parenthesis is mandatory after a function name",
			       BRATHL_SYNTAX_ERROR,
			       m_text);
  }

  GetToken(); // Consummes parenthesis

  if (NbParameters > 0)
  {
    // Compiles each expression as inside parenthesis to force the std::stack
    // to be emptied at the end of the expression

    PushOp(OP_LEFTPAR);
    Expression();
    PushOp(OP_RIGHTPAR);
    
    for (indexParam = 1; indexParam < NbParameters; indexParam++)
    {

      if (m_token == TOKEN_RIGHTPAR)
      {
	      throw CExpressionException("Not enough parameters for function",
				         BRATHL_SYNTAX_ERROR,
				         m_text);
      }

      if (m_token != TOKEN_COMMA)
      {
	      throw CExpressionException("Comma is mandatory between parameters of a function",
				         BRATHL_SYNTAX_ERROR,
				         m_text);
      }

      int32_t currentSaved = m_current;

      GetToken(); // Consummes comma
      
      // Compiles each expression as inside parenthesis to force the std::stack
      // to be emptied at the end of the expression
      PushOp(OP_LEFTPAR);
      Expression();
      PushOp(OP_RIGHTPAR);

      if (params != NULL)
      {
        params->Insert(m_text.substr(currentSaved, m_current - currentSaved - 1));
      }


    }
  }

  if (m_token != TOKEN_RIGHTPAR)
  {
    throw CExpressionException("Right parenthesis is mandatory after the parameters of a function",
			       BRATHL_SYNTAX_ERROR,
			       m_text);
  }

  GetToken(); // Consummes parenthesis
}

//----------------------------------------

void CCompiler::IifInst()
{
  uint32_t	iifNumber;


// Grammar: iifinst	::= 'iif' '(' Expression ',' Expression ',' Expression ')'

// The whole iif is inside parenthesis in order to save the std::stack state
  PushOp(OP_LEFTPAR);

  if (m_token != TOKEN_LEFTPAR)
  {
    throw CExpressionException("Left parenthesis is mandatory after an iif keyword",
			       BRATHL_SYNTAX_ERROR,
			       m_text);
  }

  GetToken(); // Consummes parenthesis

  /*
  ** Compile test (between parenthesis to empty the std::stack at the end)
  */
  PushOp(OP_LEFTPAR);
  Expression();
  PushOp(OP_RIGHTPAR);

  iifNumber	= m_iifCount++;

  m_code->push_back(OP_BEGIN_IIF | iifNumber);

  if (m_token != TOKEN_COMMA)
  {
    throw CExpressionException("Comma is mandatory after an iff test",
			       BRATHL_SYNTAX_ERROR,
			       m_text);
  }

  GetToken(); // Consummes the comma

  /*
  ** Compile the true part (between parenthesis to empty the std::stack at the end)
  */
  PushOp(OP_LEFTPAR);
  Expression();
  PushOp(OP_RIGHTPAR);

  m_code->push_back(OP_ELSE_IIF | iifNumber);

  if (m_token != TOKEN_COMMA)
  {
    throw CExpressionException("Comma is mandatory after the 'true' part of an iff",
			       BRATHL_SYNTAX_ERROR,
			       m_text);
  }

  GetToken(); // Consummes the comma

  /*
  ** Compile the false part (between parenthesis to empty the std::stack at the end)
  */
  PushOp(OP_LEFTPAR);
  Expression();
  PushOp(OP_RIGHTPAR);

  m_code->push_back(OP_END_IIF | iifNumber);

  if (m_token != TOKEN_RIGHTPAR)
  {
    throw CExpressionException("Right parenthesis is mandatory after the 'else' part of an iff",
			       BRATHL_SYNTAX_ERROR,
			       m_text);
  }

  GetToken(); // Consummes the parenthesis

  PushOp(OP_RIGHTPAR);
}

//----------------------------------------

void CCompiler::Iif3Inst
		()
{
  uint32_t	iifNumber;

// Grammar: iif3inst	::= 'iif3' '(' Expression ',' Expression ',' Expression ',' Expression ')'

// The whole iif3 is inside parenthesis in order to save the std::stack state
  PushOp(OP_LEFTPAR);

  if (m_token != TOKEN_LEFTPAR)
  {
    throw CExpressionException("Left parenthesis is mandatory after an iif3 keyword",
			       BRATHL_SYNTAX_ERROR,
			       m_text);
  }

  GetToken(); // Consummes parenthesis

  /*
  ** Compile test (between parenthesis to empty the std::stack at the end)
  */
  PushOp(OP_LEFTPAR);
  Expression();
  PushOp(OP_RIGHTPAR);

  iifNumber	= m_iifCount++;
  m_code->push_back(OP_BEGIN_IIF3 | iifNumber);

  if (m_token != TOKEN_COMMA)
  {
    throw CExpressionException("Comma is mandatory after an iff3 test",
			       BRATHL_SYNTAX_ERROR,
			       m_text);
  }

  GetToken(); // Consummes the comma

  /*
  ** Compile the true part (between parenthesis to empty the std::stack at the end)
  */
  PushOp(OP_LEFTPAR);
  Expression();
  PushOp(OP_RIGHTPAR);

  m_code->push_back(OP_ELSE_IIF | iifNumber);

  if (m_token != TOKEN_COMMA)
  {
    throw CExpressionException("Comma is mandatory after the 'true' part of an iff3",
			       BRATHL_SYNTAX_ERROR,
			       m_text);
  }

  GetToken(); // Consummes the comma

  /*
  ** Compile the false part (between parenthesis to empty the std::stack at the end)
  */
  PushOp(OP_LEFTPAR);
  Expression();
  PushOp(OP_RIGHTPAR);

  m_code->push_back(OP_DEF_IIF | iifNumber);

  if (m_token != TOKEN_COMMA)
  {
    throw CExpressionException("Comma is mandatory after the 'false' part of an iff3",
			       BRATHL_SYNTAX_ERROR,
			       m_text);
  }

  GetToken(); // Consummes the comma

  /*
  ** Compile the default part (between parenthesis to empty the std::stack at the end)
  */
  PushOp(OP_LEFTPAR);
  Expression();
  PushOp(OP_RIGHTPAR);


  if (m_token != TOKEN_RIGHTPAR)
  {
    throw CExpressionException("Right parenthesis is mandatory after the 'default' part of an iff3",
			       BRATHL_SYNTAX_ERROR,
			       m_text);
  }

  GetToken(); // Consummes the parenthesis

  PushOp(OP_RIGHTPAR);
}

//
//TYPE_FUN(i2u, T_INT, int32_t, i)
//TYPE_FUN(c2u, T_CHAR, int8_t, c)
//TYPE_FUN(f2u, T_FLOAT, float, f)
//TYPE_FUN(d2u, T_DOUBLE, double, d)
//TYPE_FUN(s2u, T_STRING, const char *, s)
//TYPE_FUN(d_ptr2u, T_DOUBLE_PTR, double *, d_ptr)
//

//=========================================================================
//-------------------------------------------------------------
//------------------- CExpression class --------------------
//-------------------------------------------------------------

CExpression::CExpression(const std::string &StrExpression /*= "" */)
{
  SetExpression(StrExpression);
}

//----------------------------------------
CExpression::CExpression(const CExpression& e)
{
  SetExpression(e);
}

//----------------------------------------

CExpression::~CExpression()
{
  DeleteValues();
  DeleteConstants();

}
//----------------------------------------
CBratObject* CExpression::Clone()
{
  return new CExpression(*this);
}

//----------------------------------------
const CExpression& CExpression::operator =(const CExpression& e)
{
  SetExpression(e); 
  return *this;
}
//----------------------------------------
const CExpression& CExpression::operator =(const std::string& str)
{
  SetExpression(str); 
  return *this;
}

//----------------------------------------
bool CExpression::operator==(CExpression& e)
{
  return CTools::Compare(this->AsString().c_str(), e.AsString().c_str());
}
//----------------------------------------
bool CExpression::operator==(const std::string& text)
{
  CExpression e = text;
  return (*this == e);
}


//----------------------------------------
void CExpression::SetExpression(const std::string& strExpression)
{
  CCompiler compiler;

  m_code.RemoveAll();
  m_constants.RemoveAll();
  m_fieldNames.RemoveAll();
  m_fieldValues.RemoveAll();
  m_algoArray.RemoveAll();
  
  DeleteDataStack();

  std::string strExpressionTrimmed = CTools::StringTrim(strExpression);

  if (! strExpressionTrimmed.empty())
  {
    compiler.Compile(strExpressionTrimmed, &m_code, &m_constants, &m_fieldNames, &m_algoArray);
  }
}
//----------------------------------------
void CExpression::SetExpression(const CExpression& e)
{
  SetExpression(e.AsString());
}
//----------------------------------------
bool CExpression::SetExpression(const char* value, CExpression& expr, std::string& errorMsg)
{
  std::string str;
  if (value != NULL)
  {
    str = value;
  }

  return SetExpression(str, expr, errorMsg);
  
}
//----------------------------------------
bool CExpression::SetExpression(const std::string& value, CExpression& expr, std::string& errorMsg)
{
  bool bOk = true;

  try
  {
    expr.SetExpression(value.c_str());
  }
  catch (CException& e)
  {
    errorMsg.append(CTools::Format("Syntax is not valid\nReason:\n%s", 
                                  e.what()));
    bOk = false;
    
  }

  return bOk;
}
//----------------------------------------
bool CExpression::IsConstant(const std::string& value) const
{
  bool bOk = false;

  CObArray::const_iterator it;
  
  for (it = m_constants.begin() ; it != m_constants.end() ; it++)
  {
    CExpressionValue* exprValue = CExpressionValue::GetExpressionValue(*it);

    bOk = areEqual( CTools::StrToDouble(value), exprValue->GetValues()[0] );
    if (bOk)
    {
      break;
    }
  }

  return bOk;
}

//----------------------------------------
CExpressionValue CExpression::Execute(CProduct* product /* = NULL */)
{
  uint32_t		PC;
  int32_t		OPCode;
  int32_t		SubOPCode;
  int32_t		NumIif;		/* Numero de Iif a rechercher (=pour goto) */
  CExpressionValue* oper1 = NULL;
  CExpressionValue* oper2 = NULL;
  CExpressionValue* oper3 = NULL;
  double		Comparizon;

  m_rangeAlgo = -1;
//-------------------------------

#define ExecuteDO_OP_1(OPERATOR, NUM)						\
  oper1	= CExpressionValue::GetExpressionValue(m_dataStack.Top()); \
	m_dataStack.Pop(); \
	m_dataStack.Push(new CExpressionValue(OPERATOR, NUM, *oper1)); \
  delete oper1

//-------------------------------

#define ExecuteDO_CHAROP_1(OPERATOR)						\
	oper1	= CExpressionValue::GetExpressionValue(m_dataStack.Top());						\
	m_dataStack.Pop();							\
  m_dataStack.Push(new CExpressionValue(OPERATOR, *oper1)); \
  delete oper1

//-------------------------------

#define ExecuteDO_OP_2(OPERATOR, NUM)						\
	oper2	= CExpressionValue::GetExpressionValue(m_dataStack.Top());						\
	m_dataStack.Pop();							\
	oper1	= CExpressionValue::GetExpressionValue(m_dataStack.Top());						\
	m_dataStack.Pop();							\
	m_dataStack.Push(new CExpressionValue(OPERATOR, NUM, *oper1, *oper2)); \
  delete oper2; \
  delete oper1

//-------------------------------

#define ExecuteDO_OP_3(OPERATOR, NUM)						\
	oper3	= CExpressionValue::GetExpressionValue(m_dataStack.Top());						\
	m_dataStack.Pop();							\
	oper2	= CExpressionValue::GetExpressionValue(m_dataStack.Top());						\
	m_dataStack.Pop();							\
	oper1	= CExpressionValue::GetExpressionValue(m_dataStack.Top());						\
	m_dataStack.Pop();							\
	m_dataStack.Push(new CExpressionValue(OPERATOR, NUM, *oper1, *oper2, *oper3)); \
  delete oper3; \
  delete oper2; \
  delete oper1

//-------------------------------

#define ExecuteDO_OP_COMPARE(OPERATOR)						\
	oper2	= CExpressionValue::GetExpressionValue(m_dataStack.Top());						\
	m_dataStack.Pop();							\
	oper1	= CExpressionValue::GetExpressionValue(m_dataStack.Top());						\
	m_dataStack.Pop();							\
	Comparizon	= oper1->Compare(*oper2);					\
	m_dataStack.Push(new CExpressionValue					\
				(isDefaultValue(Comparizon) ?		\
					Comparizon :				\
					Comparizon OPERATOR 0.0)); \
  delete oper2; \
  delete oper1

  //-------------------------------

  DeleteDataStack();

  if (m_fieldValues.size() != m_fieldNames.size())
  {
    throw CExpressionException("All field values have not been set",
			       BRATHL_LOGIC_ERROR);
  }

  PC	= 0;
  while (PC < m_code.size())
  {
    OPCode	= m_code[PC];
    SubOPCode	= OPCode & OP_VALUEMASK;
    switch (OPCode & OP_OPMASK)
    {
    //-----------------------------
  	case (OP_UNARYMINUS & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_1(CTools::UnaryMinus, true);
		  break;

    //-----------------------------
	  case (OP_POWER & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_2(CTools::Pow, true);
		  break;

    //-----------------------------
	  case (OP_MULTIPLY & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_2(CTools::Multiply, true);
		  break;

    //-----------------------------
	  case (OP_DIVIDE & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_2(CTools::Divide, true);
		  break;

    //-----------------------------
	  case (OP_MODULO & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_2(CTools::Mod, true);
		  break;

    //-----------------------------
	  case (OP_PLUS & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_2(CTools::Plus, true);
		  break;

    //-----------------------------
	  case (OP_MINUS & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_2(CTools::Minus, true);
		  break;

    //-----------------------------
	  case (OP_EQ & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_COMPARE(==);
		  break;

    //-----------------------------
	  case (OP_NE & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_COMPARE(!=);
		  break;

    //-----------------------------
	  case (OP_LT & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_COMPARE(<);
		  break;

    //-----------------------------
	  case (OP_LE & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_COMPARE(<=);
		  break;

    //-----------------------------
	  case (OP_GT & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_COMPARE(>);
		  break;

    //-----------------------------
	  case (OP_GE & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_COMPARE(>=);
		  break;

    //-----------------------------
	  case (OP_BITNOT & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_1(CTools::BitwiseNot, true);
		  break;

    //-----------------------------
	  case (OP_NOT & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_1(CTools::UnaryNot, true);
		  break;

    //-----------------------------
	  case (OP_BITAND & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_2(CTools::BitwiseAnd, false);
		  break;

    //-----------------------------
	  case (OP_AND & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_2(CTools::And, false);
		  break;

    //-----------------------------
	  case (OP_BITOR & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_2(CTools::BitwiseOr, false);
		  break;

    //-----------------------------
	  case (OP_OR & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_2(CTools::Or, false);
		  break;

    //-----------------------------
	  case (OP_FUNCTION1 & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_1(*Fonctions1[SubOPCode].ToCall,
			         Fonctions1[SubOPCode].Numeric);
		  break;

    //-----------------------------
	  case (OP_FUNCTION2 & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_2(*Fonctions2[SubOPCode].ToCall,
			         Fonctions2[SubOPCode].Numeric);
		  break;

    //-----------------------------
	  case (OP_FUNCTION3 & OP_OPMASK):
    //-----------------------------
		  ExecuteDO_OP_3(*Fonctions3[SubOPCode].ToCall,
			         Fonctions3[SubOPCode].Numeric);
		  break;

    //-----------------------------
	  case (OP_FUNCTION_ALGO_N & OP_OPMASK):
    //-----------------------------
      {
        CExpressionValue* exprNbParameters	= CExpressionValue::GetExpressionValue(m_dataStack.Top());					
 	      m_dataStack.Pop();							
        uint32_t nbParameters = static_cast<uint32_t>(exprNbParameters->GetValue(0));
        
        // nbParametersValues = nbParameters - param function name ===> nbParameters - 1
        uint32_t nbParametersValues = nbParameters - 1;
        
        CExpressionValue** arrayExprValue = new CExpressionValue*[nbParametersValues];
        CExpressionValue* algoNameExpr = NULL;
        
        uint32_t i = 0;
        for (i = 0 ; i < nbParameters ; i++)
        {
          // algo functionName is the last param
          if (i == (nbParameters - 1))
          {
            algoNameExpr = CExpressionValue::GetExpressionValue(m_dataStack.Top());	
    	      m_dataStack.Pop();							
          }
          else
          {
            // store parameters in right call order (at top)
            arrayExprValue[nbParametersValues - i - 1] = CExpressionValue::GetExpressionValue(m_dataStack.Top());
    	      m_dataStack.Pop();							
          }

        }
  
        std::string algoName = algoNameExpr->GetString();
        if (algoName.empty())
        {
          throw CExpressionException(CTools::Format("ERROR while processing algorithm. In your expression below an algorithm name is empty (near position %d)", PC),
		                                 BRATHL_LOGIC_ERROR,
                                     this->AsString());
        }

        // Get the instance of the algorithm.
        m_rangeAlgo++;
        CBratAlgorithmBase* algo = m_algoArray.at(m_rangeAlgo);

        algo->SetProduct(product);

        //type_union* parameterValues = new type_union[nbParametersValues + 1]; // nbParametersValues + END
        CVectorBratAlgorithmParam parameterValues;

        std::string valueStr;
        double valueDouble;
        double *valuesDoubleArray = NULL;

        for (i = 0 ; i < nbParametersValues ; i++)
        { 
          ExpressionValueType expressionValueType = arrayExprValue[i]->GetType();
          
          if (expressionValueType == CharacterType)
          {
            //-------------------
            // Value is character type
            //-------------------
            valueStr = arrayExprValue[i]->GetString();
            
            // warning on valueStr visibility : must be visible when calling function
            //parameterValues[i] = CTools::s2u(valueStr.c_str());
            
            parameterValues.Insert(valueStr);
          }
          else
          {
            //-------------------
            // Value is foat type
            //-------------------
            uint32_t nbValues = arrayExprValue[i]->GetNbValues();
            if (nbValues <= 0)
            {
              throw CExpressionException(CTools::Format("ERROR while processing algorithm '%s'. Parameter %d has no value - Please check the algorithm parameter value in your expression, perhaps it's wrong. Otherwise, please contact Brat Helpdesk.", algoName.c_str(), i),
					                               BRATHL_LOGIC_ERROR,
					                               this->AsString());
            } 
            if (nbValues == 1)
            {
              valueDouble = arrayExprValue[i]->GetValue(0);
              
              //parameterValues[i] = CTools::d2u(valueDouble);
              
              parameterValues.Insert(valueDouble, algo->GetInputParamFormat(i));

            } 
            else
            {
            // warning on valuesDoubleArray visibility : must be visible when calling function
            // valuesDoubleArray is available until arrayExprValue is deleted
              valuesDoubleArray = arrayExprValue[i]->GetValues();
              
              //parameterValues[i] = CTools::d_ptr2u(valuesDoubleArray);
              
              parameterValues.Insert(valuesDoubleArray, arrayExprValue[i]->GetNbValues());

            } 

          }
        }
        
        //parameterValues[nbParametersValues] = END;


        
        //m_algoArray.Dump();
        //algo->Dump();

        //Get parameters format.
        //std::string paramFormats = algo->GetParamFormats();

        //m_dataStack.Push(new CExpressionValue(*FonctionsAlgoN[SubOPCode].ToCall, algoName.c_str(), paramFormats.c_str(), parameterValues)); 
        //--------------------------------------------------------
        // Run algorithm and stored the result in an expression value.
        //--------------------------------------------------------
        m_dataStack.Push(new CExpressionValue(*FonctionsBratAlgoBaseN[SubOPCode].ToCall, algo, parameterValues)); 

        //--------------------------------------------------------
        //the algorithm has ben processed, now allocation can be deleted
        //--------------------------------------------------------

        //delete arrayExprValue and its content
        for (i = 0 ; i < nbParametersValues ; i++)
        {
          if (arrayExprValue[i] != NULL) 
          {
            delete arrayExprValue[i];
            arrayExprValue[i] = NULL;
          }
        }

        delete []arrayExprValue;
        arrayExprValue = NULL;

        //delete parameterValues;
        //parameterValues = NULL;

        delete algoNameExpr;
        algoNameExpr = NULL;

        delete exprNbParameters;
        exprNbParameters = NULL;


      }
		  break;

    //-----------------------------
	  case (OP_CHARFUNC1 & OP_OPMASK):
    //-----------------------------
		  if (CharFonctions1[SubOPCode].ToCallStr != NULL)
		  {
		    ExecuteDO_CHAROP_1(*CharFonctions1[SubOPCode].ToCallStr);
		  }
		  else
		  {
		    ExecuteDO_CHAROP_1(*CharFonctions1[SubOPCode].ToCallFlt);
		  }
		  break;

    //-----------------------------
	  case (OP_PUSHVALUE & OP_OPMASK):
    //-----------------------------
      oper1 = CExpressionValue::GetExpressionValue(m_constants[SubOPCode]);
      m_dataStack.Push(new CExpressionValue(*oper1));
		  break;

    //-----------------------------
	  case (OP_PUSHFIELD & OP_OPMASK):
    //-----------------------------
      oper1 = CExpressionValue::GetExpressionValue(m_fieldValues[SubOPCode]);
		  m_dataStack.Push(new CExpressionValue(*oper1));
		  break;


    //-----------------------------
	  case (OP_BEGIN_IIF & OP_OPMASK):
	  case (OP_BEGIN_IIF3 & OP_OPMASK):
    //-----------------------------
		  {
		    uint32_t	WhatToSearch	= OP_ELSE_IIF;
		    oper1 = CExpressionValue::GetExpressionValue(m_dataStack.Top());

		    switch (oper1->IsTrue())
		    {
        //-----------------------------
		    case -1:		// Default Value
        //-----------------------------
		      if ((OPCode & OP_OPMASK) == (OP_BEGIN_IIF3 & OP_OPMASK))
          {
            WhatToSearch	= OP_DEF_IIF;
          }
        //-----------------------------
		    // No break and it is wanted
        //-----------------------------
        
        //-----------------------------
		    case 0:		// False
        //-----------------------------
		      WhatToSearch	&= OP_OPMASK;

		      NumIif	= SubOPCode;
		      do
		      {
			      PC++;
			      OPCode		= m_code[PC];
			      SubOPCode	= OPCode & OP_VALUEMASK;
		      } while (((OPCode & OP_OPMASK) != WhatToSearch) || (SubOPCode != NumIif));

		      break;
        } // end switch (oper1.IsTrue())

		    m_dataStack.Pop();
        delete oper1;
		  }
		  break;

    //-----------------------------
	  case (OP_ELSE_IIF & OP_OPMASK):
	  case (OP_DEF_IIF & OP_OPMASK):
    //-----------------------------
		  {
		    // End of 'then' or 'else' part: goto end if
		    NumIif	= SubOPCode;
		    do
		    {
		        PC++;
		        OPCode	= m_code[PC];
		        SubOPCode = OPCode & OP_VALUEMASK;
		    } while (((OPCode & OP_OPMASK) != (OP_END_IIF & OP_OPMASK)) || (SubOPCode != NumIif));

		  }
		  break;

    //-----------------------------
	  case (OP_END_IIF & OP_OPMASK):
    //-----------------------------
		  // End of the whole iif (or iif3) NOTHING TO DO
		  break;

    //-----------------------------
	  default:
    //-----------------------------
		  throw CExpressionException(CTools::Format("Unknown OPCode %d (0x%x). Probably an inconsistent modification have been done",
							    OPCode, OPCode),
					     BRATHL_LOGIC_ERROR);
    }
    PC++;
  }


  DeleteValues();	   // Field values have to be reloaded for another execution

  //  std::assert(m_dataStack.size() == 1); // Since only the result should be left

  if (m_dataStack.size() != 1)
  {
    throw CExpressionException("CExpression::Execute - Try to get a result of an expression which have never been executed",
			       BRATHL_LOGIC_ERROR);
  }

  return *CExpressionValue::GetExpressionValue(m_dataStack.Top());
}

//----------------------------------------
void CExpression::Dump(std::ostream	&Output /* = std::cerr */)
{
  uint32_t		PC;
  int32_t		OPCode;
  int32_t		SubOPCode;

  if (! CTrace::IsTrace())
  {
    return;
  }

  Output << "==> Dump a CExpression Object at "<< this << std::endl;

  Output << std::endl << "Decompiled expression: " << AsString() << std::endl;

  Output << "Expression code:" << std::endl;
  
  if (m_code.size() == 0)
  {
    Output << "\tEmpty expression" << std::endl;
  }

  for (PC=0; PC < m_code.size(); PC++)
  {
    OPCode	= m_code[PC];
    //printf("Code %d, %x\n", OPCode, OPCode);
    SubOPCode	= OPCode & OP_VALUEMASK;
    Output << "\t";
    Output.width(3);
    Output << PC << "\t";
    switch (OPCode & OP_OPMASK)
    {
    //-----------------------------
	  case (OP_UNARYMINUS	& OP_OPMASK):
    //-----------------------------
		  Output << "exec\tunary -";
		  break;
    //-----------------------------
	  case (OP_BITNOT		& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t~";
		  break;
    //-----------------------------
	  case (OP_NOT		& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t!";
		  break;
    //-----------------------------
	  case (OP_POWER		& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t^";
		  break;
    //-----------------------------
	  case (OP_MODULO		& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t%";
		  break;
    //-----------------------------
	  case (OP_MULTIPLY	& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t*";
		  break;
    //-----------------------------
	  case (OP_DIVIDE		& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t/";
		  break;
    //-----------------------------
	  case (OP_PLUS		& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t+";
		  break;
    //-----------------------------
	  case (OP_MINUS		& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t-";
		  break;
    //-----------------------------
	  case (OP_EQ		& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t==";
		  break;
    //-----------------------------
	  case (OP_NE		& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t!=";
		  break;
    //-----------------------------
	  case (OP_LE		& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t<=";
		  break;
    //-----------------------------
	  case (OP_LT		& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t<";
		  break;
    //-----------------------------
	  case (OP_GE		& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t>=";
		  break;
    //-----------------------------
	  case (OP_GT		& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t>";
		  break;
    //-----------------------------
	  case (OP_BITAND		& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t&";
		  break;
    //-----------------------------
	  case (OP_AND		& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t&&";
		  break;
    //-----------------------------
	  case (OP_BITOR		& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t|";
		  break;
    //-----------------------------
	  case (OP_OR		& OP_OPMASK):
    //-----------------------------
		  Output << "exec\t||";
		  break;
    //-----------------------------
	  case (OP_FUNCTION1 & OP_OPMASK):
    //-----------------------------
		  Output << "call1\t" << Fonctions1[SubOPCode].Name;
		  break;
    //-----------------------------
	  case (OP_FUNCTION2 & OP_OPMASK):
    //-----------------------------
		  Output << "call2\t" << Fonctions2[SubOPCode].Name;
		  break;
    //-----------------------------
	  case (OP_FUNCTION3 & OP_OPMASK):
    //-----------------------------
		  Output << "call3\t" << Fonctions3[SubOPCode].Name;
		  break;
    //-----------------------------
	  case (OP_CHARFUNC1 & OP_OPMASK):
    //-----------------------------
		  Output << "call1\t" << CharFonctions1[SubOPCode].Name;
		  break;
    //-----------------------------
	  case (OP_FUNCTION_ALGO_N & OP_OPMASK):
    //-----------------------------
		  Output << "calln\t" << FonctionsAlgoN[SubOPCode].Name;
		  break;
    //-----------------------------
	  case (OP_PUSHVALUE & OP_OPMASK):
    //-----------------------------
      {
      CExpressionValue* exprValue = CExpressionValue::GetExpressionValue(m_constants[SubOPCode]);
		  Output << "push\t" << exprValue->AsString();
		  break;
      }
    //-----------------------------
	  case (OP_PUSHFIELD & OP_OPMASK):
    //-----------------------------
		  Output << "push\tfield " << m_fieldNames[SubOPCode];
		  break;
    //-----------------------------
	  case (OP_BEGIN_IIF	& OP_OPMASK):
    //-----------------------------
		  Output << "iif\t#" << SubOPCode;
		  break;
    //-----------------------------
	  case (OP_BEGIN_IIF3	& OP_OPMASK):
    //-----------------------------
		  Output << "iif3\t#" << SubOPCode;
		  break;
    //-----------------------------
	  case (OP_ELSE_IIF	& OP_OPMASK):
    //-----------------------------
		  Output << "else\t#" << SubOPCode;
		  break;
    //-----------------------------
	  case (OP_DEF_IIF	& OP_OPMASK):
    //-----------------------------
		  Output << "default\t#" << SubOPCode;
		  break;
    //-----------------------------
	  case (OP_END_IIF	& OP_OPMASK):
    //-----------------------------
		  Output << "end\t#" << SubOPCode;
		  break;
    //-----------------------------
	  default:
    //-----------------------------
		  throw CExpressionException(CTools::Format("Unknown OPCode %d (0x%x). Probably an inconsistent modification have been done",
							    OPCode, OPCode),
					     BRATHL_LOGIC_ERROR);
    }

    Output << std::endl;
  }

  CExpressionValue* exprValue = NULL;


  if (m_fieldNames.size() != 0)
  {
    Output << std::endl << "Fields used:" << std::endl;
  }

  for (PC=0; PC<m_fieldNames.size(); ++PC)
  {
    Output << "\t" << m_fieldNames[PC];

    if (PC < m_fieldValues.size())
    {
      exprValue = CExpressionValue::GetExpressionValue(m_fieldValues[PC]);
      Output << " = " << exprValue->AsString();
    }

    Output << std::endl;
  }

  if (m_constants.size() != 0)
  {
    Output << std::endl << "Constants used:" << std::endl;
  }

  for (PC=0; PC<m_constants.size(); ++PC)
  {
    exprValue = CExpressionValue::GetExpressionValue(m_constants[PC]);
    Output << "\t" << exprValue->AsString() << std::endl;
  }

  Output << "==> END Dump a CExpression Object at "<< this << std::endl;


}
//----------------------------------------

std::string CExpression::AsString() const
		
{
  uint32_t		PC;
  int32_t		OPCode;
  uint32_t		SubOPCode;
  //int32_t		NbOPCode;
  CStringArray	Stack;
  CUIntArray	PriorityStack;
  std::string		Text;

#define AsString_POP						\
	Stack.pop_back();					\
	PriorityStack.pop_back();

#define AsString_PUSH(OperationCode)					\
	Stack.push_back(Text);					\
	PriorityStack.push_back(OperationCode);

  for (PC=0; PC < m_code.size(); ++PC)
  {
    OPCode	= m_code[PC];
    //printf("Code %d, %x\n", OPCode, OPCode);
    SubOPCode	= OPCode & OP_VALUEMASK;
    Text	= "";

    switch (OPCode & OP_OPMASK)
    {
    //-----------------------------
	  case (OP_UNARYMINUS	& OP_OPMASK):
	  case (OP_NOT		& OP_OPMASK):
	  case (OP_BITNOT		& OP_OPMASK):
    //-----------------------------
		  if (Stack.size() != 1)
      {
		    Text	+= "(";
      }

		  Text += (OPCode == OP_NOT ?	"!" :
			         (OPCode == OP_BITNOT ?	"~" :
						  "-"));
		  if (PriorityStack.back() != 0)
		  {
		    Text += "(" + Stack.back() + ")";
		  }
		  else
      {
		    Text += Stack.back();
      }

		  if (Stack.size() != 1)
		  {
		    Text	+= ")";
		  }
		  
		  AsString_POP;
		  AsString_PUSH(SubOPCode);
		  break;

    //-----------------------------
	  case (OP_POWER		& OP_OPMASK):
	  case (OP_MODULO		& OP_OPMASK):
	  case (OP_MULTIPLY	& OP_OPMASK):
	  case (OP_DIVIDE		& OP_OPMASK):
	  case (OP_PLUS		& OP_OPMASK):
	  case (OP_MINUS		& OP_OPMASK):
	  case (OP_EQ		& OP_OPMASK):
	  case (OP_NE		& OP_OPMASK):
	  case (OP_LE		& OP_OPMASK):
	  case (OP_LT		& OP_OPMASK):
	  case (OP_GE		& OP_OPMASK):
	  case (OP_GT		& OP_OPMASK):
	  case (OP_AND		& OP_OPMASK):
	  case (OP_BITAND		& OP_OPMASK):
	  case (OP_OR		& OP_OPMASK):
	  case (OP_BITOR		& OP_OPMASK):
    //-----------------------------

      //Stack.Dump();
      //PriorityStack.Dump();
      
      if (m_fieldNames.Exists(Stack.at(Stack.size()-2)))
      {
        Text	+= Stack.at(Stack.size()-2);
      }
      else if (IsConstant(Stack.at(Stack.size()-2)))
      {
        Text	+= Stack.at(Stack.size()-2);
      }
      else
      {
		    Text	+= "(" + Stack.at(Stack.size()-2) + ")";
      }

      /*
		  if (PriorityStack[Stack.size()-2] > SubOPCode)
		  {
		    Text	+= "(" + Stack[Stack.size()-2] + ")";
		  }
		  else
		  {
		    Text	+= Stack[Stack.size()-2];
		  }
      */
      

		  switch (OPCode)
		  {
		  case OP_POWER:	Text	+= " ^ ";	break;
		  case OP_MULTIPLY:	Text	+= " * ";	break;
		  case OP_MODULO:	Text	+= " % ";	break;
		  case OP_DIVIDE:	Text	+= " / ";	break;
		  case OP_PLUS:		Text	+= " + ";	break;
		  case OP_MINUS:	Text	+= " - ";	break;
		  case OP_EQ:		Text	+= " == ";	break;
		  case OP_NE:		Text	+= " != ";	break;
		  case OP_LT:		Text	+= " < ";	break;
		  case OP_LE:		Text	+= " <= ";	break;
		  case OP_GT:		Text	+= " > ";	break;
		  case OP_GE:		Text	+= " >= ";	break;
		  case OP_AND:		Text	+= " && ";	break;
		  case OP_BITAND:	Text	+= " & ";	break;
		  case OP_OR:		Text	+= " || ";	break;
		  case OP_BITOR:	Text	+= " | ";	break;
		  default:
		        throw CExpressionException(CTools::Format("OPCode(2) 0x%x", OPCode),
						   BRATHL_LOGIC_ERROR);
		  }

      //Stack.Dump();
      //PriorityStack.Dump();

      if (m_fieldNames.Exists(Stack.back()))
      {
		    Text	+= Stack.back();
      }
      else if (IsConstant(Stack.back()))
      {
		    Text	+= Stack.back();
      }
      else
      {
		    Text += "(" + Stack.back() + ")";
      }

      /*
		  if (PriorityStack.back() > SubOPCode)
		  {
		    Text	+= "(" + Stack.back() + ")";
		  }
		  else
		  {
		    Text	+= Stack.back();
		  }
      */

		  AsString_POP;
		  AsString_POP;
		  AsString_PUSH(SubOPCode);
		  break;

    //-----------------------------
	  case (OP_FUNCTION1 & OP_OPMASK):
    //-----------------------------
		  Text	+= Fonctions1[SubOPCode].Name + "(" + Stack.back() + ")";
		  AsString_POP;
		  AsString_PUSH(0);
		  break;

    //-----------------------------
	  case (OP_FUNCTION2 & OP_OPMASK):
    //-----------------------------
		  Text	+= Fonctions2[SubOPCode].Name + "(" +
			     Stack[Stack.size()-2] + ", " +
			     Stack.back() + ")";
		  AsString_POP;
		  AsString_POP;
		  AsString_PUSH(0);
		  break;

    //-----------------------------
	  case (OP_FUNCTION3 & OP_OPMASK):
    //-----------------------------
		  Text	+= Fonctions3[SubOPCode].Name + "(" +
			                   Stack[Stack.size()-3] + ", " +
			                   Stack[Stack.size()-2] + ", " +
			                   Stack.back() + ")";
		  AsString_POP;
		  AsString_POP;
		  AsString_POP;
		  AsString_PUSH(0);
		  break;


    //-----------------------------
	  case (OP_FUNCTION_ALGO_N & OP_OPMASK):
    //-----------------------------
      {
        //Stack.Dump();

        uint32_t nbParameters = static_cast<uint32_t>(CTools::StrToInt32(Stack.back()));

        AsString_POP;							

        //Stack.Dump();

        Text	+= FonctionsAlgoN[SubOPCode].Name + "(";

        for (uint32_t i = nbParameters  ; i > 0 ; i--)
        {
          Text += Stack.at(Stack.size() - i);	
          // algo functionName is the last param
          if (i == 1)
          {
            Text	+= ")";	
          }
          else
          {
            Text	+= ", ";	
          }
        }

        for (uint32_t i = 0  ; i < nbParameters ; i++)
        {
   	      AsString_POP;							
          //Stack.Dump();
        }

        AsString_PUSH(0);
      }
		  break;

    //-----------------------------
	  case (OP_CHARFUNC1 & OP_OPMASK):
    //-----------------------------
		  Text	+= CharFonctions1[SubOPCode].Name + "(" + Stack.back() + ")";
		  AsString_POP;
		  AsString_PUSH(0);
		  break;

    //-----------------------------
	  case (OP_PUSHVALUE & OP_OPMASK):
    //-----------------------------
      {
      CExpressionValue* exprValue = CExpressionValue::GetExpressionValue(m_constants[SubOPCode]);
      Text	= exprValue->AsString();
		  AsString_PUSH(0);
		  break;
      }
    //-----------------------------
	  case (OP_PUSHFIELD & OP_OPMASK):
    //-----------------------------
		  Text	= m_fieldNames[SubOPCode];
		  AsString_PUSH(0);
		  break;

    //-----------------------------
	  case (OP_BEGIN_IIF	& OP_OPMASK):
    //-----------------------------
		  Text	+= "iif(" + Stack.back() + ", ";
		  AsString_POP;
		  AsString_PUSH(0);
		  break;

    //-----------------------------
	  case (OP_BEGIN_IIF3	& OP_OPMASK):
    //-----------------------------
		  Text	+= "iif3(" + Stack.back() + ", ";
		  AsString_POP;
		  AsString_PUSH(0);
		  break;

    //-----------------------------
	  case (OP_ELSE_IIF	& OP_OPMASK):
	  case (OP_DEF_IIF	& OP_OPMASK):
    //-----------------------------
		  Text	+= Stack[Stack.size()-2] + Stack.back() + ", ";
		  AsString_POP;
		  AsString_POP;
		  AsString_PUSH(0);
		  break;

    //-----------------------------
    case (OP_END_IIF	& OP_OPMASK):
    //-----------------------------
		  Text	+= Stack[Stack.size()-2] + Stack.back() + ")";
		  AsString_POP;
		  AsString_POP;
		  AsString_PUSH(0);
		  break;

    //-----------------------------
	  default:
    //-----------------------------
		  throw CExpressionException(CTools::Format("Unknown OPCode %d (0x%x). Probably an inconsistent modification have been done",
							    OPCode, OPCode),
					     BRATHL_LOGIC_ERROR);
    }
  }
  
  if (Stack.size() <= 0)
  {
    return "";
  }

  assert(Stack.size() == 1);

  return Stack[0];
}

//----------------------------------------

CExpressionValue CExpression::GetResult()
{
  if (m_dataStack.empty())
  {
    throw CExpressionException("CExpression::GetResult - Try to get a result of an expression which have never been executed",
			       BRATHL_LOGIC_ERROR);
  }

  return *CExpressionValue::GetExpressionValue(m_dataStack.Top());
}

//----------------------------------------

void CExpression::SetValue(std::string fieldName, CExpressionValue* value)
{
  if (m_fieldValues.size() != m_fieldNames.size())
  {
    m_fieldValues.resize(m_fieldNames.size());
  }

  for (uint32_t index=0; index < m_fieldNames.size(); index++)
  {
    if (fieldName == m_fieldNames[index])
    {
      if ((m_fieldValues[index] != NULL) && (m_fieldValues.GetDelete()))
      {
        delete m_fieldValues[index];
      }

      m_fieldValues[index] = value;
      return;
    }
  }

  throw CExpressionException("Try to set value of '" + fieldName + "' field which is unknown",
			     BRATHL_LOGIC_ERROR);
}

//----------------------------------------
void CExpression::SetValue(std::string fieldName, CExpressionValue& value)
{
  if (m_fieldValues.size() != m_fieldNames.size())
  {
    m_fieldValues.resize(m_fieldNames.size());
  }

  for (uint32_t index=0; index < m_fieldNames.size(); index++)
  {
    if (fieldName == m_fieldNames[index])
    {
      if (m_fieldValues[index] != NULL) 
      {
        delete m_fieldValues[index];
      }

      m_fieldValues[index] = new CExpressionValue(value);
      return;
    }
  }

  throw CExpressionException("Try to set value of '" + fieldName + "' field which is unknown",
			     BRATHL_LOGIC_ERROR);
}

//----------------------------------------
void CExpression::SetValue(std::string fieldName, double value)
{
  if (m_fieldValues.size() != m_fieldNames.size())
  {
    m_fieldValues.resize(m_fieldNames.size());
  }

  for (uint32_t index=0; index < m_fieldNames.size(); index++)
  {
    if (fieldName == m_fieldNames[index])
    {
      if (m_fieldValues[index] != NULL) 
      {
        delete m_fieldValues[index];
      }

      m_fieldValues[index] = new CExpressionValue(value);
      return;
    }
  }

  throw CExpressionException("Try to set value of '" + fieldName + "' field which is unknown",
			     BRATHL_LOGIC_ERROR);
}

//----------------------------------------

void CExpression::DeleteValues()
{
  m_fieldValues.RemoveAll();
}
//----------------------------------------
void CExpression::DeleteConstants()
{
  m_constants.RemoveAll();
}
//----------------------------------------
void CExpression::DeleteDataStack()
{
  m_dataStack.RemoveAll();
}
//----------------------------------------
CExpression* CExpression::GetExpression(CBratObject* ob, bool withExcept /*= true*/)
{
  CExpression* exprValue = dynamic_cast<CExpression*>(ob);
  if (withExcept)
  {
    if (exprValue == NULL)
    {
      CException e("CExpression::GetExpression - dynamic_cast<CExpression*>(ob) returns NULL"
                   "object seems not to be an instance of CExpression",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return exprValue;

}

//----------------------------------------
bool CExpression::GetFieldNames(const std::string& stringExpr, CStringArray& fieldNames, std::string& errorMsg)
{
    CExpression expr;
    
    std::string out;
    CTools::ReplaceAliases(stringExpr, out);

    try
    {
      expr.SetExpression(out);
    }
    catch (CException& e)
    {
      errorMsg = e.what();
      return false;
    }

    fieldNames.Insert(*(expr.GetFieldNames()));
    return true;

}


}
