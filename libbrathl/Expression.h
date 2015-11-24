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
#ifndef _Expression_h_
#define _Expression_h_

#include <cstdarg>

#include "new-gui/Common/tools/brathl_error.h"
#include "brathl.h"
#include "Tools.h"
#include "Date.h"
#include "Unit.h"
#include "List.h"
#include "BratAlgorithmBase.h"

namespace brathl
{

typedef double ExpressionCallableFunction1(double);

// under Microsoft VC6 this instruction bugs in C1001 Internal Fatal Error
//typedef std::string ExpressionCallableFunctionStrToStr1(const char *);
// replace by instruction below
typedef const char* ExpressionCallableFunctionStrToStr1(const char *);

typedef double ExpressionCallableFunctionStrToFlt1(const char *);
typedef double ExpressionCallableFunction2(double, double);
typedef double ExpressionCallableFunction3(double, double, double);
//typedef double ExpressionCallableFunctionAlgoN(const char*, const char*, ...);
typedef double ExpressionCallableFunctionAlgoN(const char*, CVectorBratAlgorithmParam& arg);
typedef double ExpressionCallableFunctionBratAlgoBaseN(CBratAlgorithmBase *algo, CVectorBratAlgorithmParam& arg);

typedef CUIntArray	ExpressionValueDimensions;
typedef CDoubleArray	ExpressionValueValues;


enum FunctionCategory {
                  MathTrigo, // Must also be first of enum
                  Statistical,
                  Logical,
                  Relational,
                  Constant,
                  BitwiseOp,
                  DateTime,
                  Algorithm,
                  Geographical // Must also be last of enum
 };


/* Definition des fonctions */
struct DefFunction1
{
  std::string			Name;
  std::string			Desc;
  ExpressionCallableFunction1	*ToCall;
  FunctionCategory              Category;
  bool				Numeric;	// If true, the result is numeric even if parameter is std::string
  DefFunction1
	(const char		*Name,
	 const char		*Desc,
	 ExpressionCallableFunction1	ToCall,
         FunctionCategory               Category = MathTrigo,
	 bool				Numeric = true)
    : Name(Name), Desc(Desc), ToCall(ToCall), Category(Category), Numeric(Numeric)
  { };
};


/* Definition des fonctions */
struct DefCharFunction1
{
  std::string				Name;
  std::string			        Desc;
  ExpressionCallableFunctionStrToStr1	*ToCallStr;	// Only one callback function must be defined at a time
  ExpressionCallableFunctionStrToFlt1	*ToCallFlt;
  FunctionCategory                      Category;
  DefCharFunction1
	(const char			*Name,
	 const char		        *Desc,
	 ExpressionCallableFunctionStrToStr1	ToCallStr,
	 ExpressionCallableFunctionStrToFlt1	ToCallFlt,
         FunctionCategory                       Category = MathTrigo)
    : Name(Name), Desc(Desc), ToCallStr(ToCallStr), ToCallFlt(ToCallFlt), Category(Category)
  { };
};

struct DefFunction2
{
  std::string			Name;
  std::string			Desc;
  ExpressionCallableFunction2	*ToCall;
  FunctionCategory              Category;
  bool				Numeric;	// If true, the result is numeric even if parameter is std::string
  DefFunction2
	(const char		*Name,
	 const char	        *Desc,
	 ExpressionCallableFunction2	ToCall,
         FunctionCategory               Category = MathTrigo,
	 bool				Numeric = true)
    : Name(Name), Desc(Desc), ToCall(ToCall), Category(Category), Numeric(Numeric)
  { };
};

struct DefFunction3
{
  std::string			Name;
  std::string			Desc;
  ExpressionCallableFunction3	*ToCall;
  FunctionCategory              Category;
  bool				Numeric;	// If true, the result is numeric even if parameter is std::string
  DefFunction3
	(const char		*Name,
	 const char	        *Desc,
	 ExpressionCallableFunction3	ToCall,
         FunctionCategory               Category = MathTrigo,
	 bool				Numeric = true)
    : Name(Name), Desc(Desc), ToCall(ToCall), Category(Category),  Numeric(Numeric)
  { };
};

struct DefFunctionAlgoN
{
  std::string			Name;
  std::string			Desc;
  ExpressionCallableFunctionAlgoN	*ToCall;
  FunctionCategory              Category;
  bool				Numeric;	// If true, the result is numeric even if parameter is std::string
  
  DefFunctionAlgoN(const char* Name,
               const char *Desc,
               ExpressionCallableFunctionAlgoN	ToCall, 
               FunctionCategory Category = MathTrigo,
	             bool Numeric = true)
    : Name(Name), Desc(Desc), ToCall(ToCall), Category(Category),  Numeric(Numeric)
  { };
};

struct DefFunctionBratAlgoBaseN
{
  std::string			Name;
  std::string			Desc;
  ExpressionCallableFunctionBratAlgoBaseN	*ToCall;
  FunctionCategory              Category;
  bool				Numeric;	// If true, the result is numeric even if parameter is std::string
  
  DefFunctionBratAlgoBaseN(
               ExpressionCallableFunctionBratAlgoBaseN	ToCall
	             )
    : ToCall(ToCall)
  { };
};
/* Definition des constantes */
struct DefConstant
{
  std::string			Name;
  std::string			Desc;
  double			Value;
  FunctionCategory              Category;
  DefConstant
	(const char	*Name,
	 const char	*Desc,
	 double	Value,
         FunctionCategory   Category = Constant)
    : Name(Name), Desc(Desc), Value(Value), Category(Category)
  { };
};


static const DefFunction1 Fonctions1[]	=
{
	DefFunction1("abs",		"Calculates the absolute value.",
                     CTools::Abs,   MathTrigo,		false),
	DefFunction1("ceil",		"Calculates the ceiling of a value.",
                     CTools::Ceil),
	DefFunction1("cos",		"Calculates the cosine (radian) of a value.",
                     CTools::Cos),
	DefFunction1("cosd",		"Calculates the cosine (degree) of a value.",
                     CTools::CosD),
	DefFunction1("deg2rad",		"Translates Degree to Radian.",
                     CTools::Deg2Rad),
	DefFunction1("exp",		"Calculates the exponential.",
                     CTools::Exp),
	DefFunction1("floor",		"Calculates the floor of a value",
                     CTools::Floor, MathTrigo,	false),
	DefFunction1("frac",		"Calculates the fractional parts of a value.",
                     CTools::Frac),
	DefFunction1("int",		"Calculates the integer parts of a value.",
                     CTools::Int,   MathTrigo,  false),
	DefFunction1("log",		"Calculates the logarithm of a value",
                     CTools::Log),
	DefFunction1("log10",		"Calculates the base-10 logarithm of a value",
                     CTools::Log10),
	DefFunction1("rad2deg",		"Translates Radian to Degree.",
                     CTools::Rad2Deg),
	DefFunction1("round",		"Calculates the rounded value(e.g round(20.23645) ==> 20)",
                     CTools::Round,   MathTrigo, false),
	DefFunction1("sin",		"Calculates the sine (radian) of a value.",
                     CTools::Sin),
	DefFunction1("sind",		"Calculates the sine (degreee) of a value.",
                     CTools::SinD),
	DefFunction1("sqr",		"Calculates the square of a value.",
                     CTools::Sqr),
	DefFunction1("sqrt",		"Calculates the square root of a value.",
                     CTools::Sqrt),
	DefFunction1("tan",		"Calculates the tangent (radian) of a value.",
                     CTools::Tan),
	DefFunction1("tand",		"Calculates the tangent (degree) of a value.",
                     CTools::TanD),
	DefFunction1("is_default", 	"Checks if a value is a default value (1: yes, 0: no)",
                     CTools::IsDefaultFloat,    Logical),
	DefFunction1("sign",		"Checks the sign of a value (-1: negative, 1: positive or zero)",
                     CTools::Sign),
};

static const DefCharFunction1 CharFonctions1[]	=
{
	DefCharFunction1("to_date",      "Translates a std::string value into a date value"
                                         "\nAllowed format are:"
                                         "\n\n YYYY-MM-DD HH:MN:SS.MS std::string."
                                         "\n For instance:"
                                         "\n '1995-12-05 12:02:10.1230'"
                                         "\n '1995-12-05 12:02:10'"
                                         "\n '1995-12-05'"
                                         "\n\n a julian std::string: format:positive 'Days Seconds Microseconds'"
                                         "\n Seconds must be stricty less 86400 and Microseconds must be stricty less than 1000000"
                                         "\n For instance:"
                                         "\n '2530 230 4569'"
                                         "\n\n a julian std::string: format:positive decimal julian day"
                                         "\n For instance:"
                                         "\n '850.2536985'"
                                         "\n\nFor julian std::string,  it can contain its reference date at the end by specifying @YYYY where YYYY is the reference year"
                                         " that's must be one of 1950, 1958, 1985, 1990, 2000"
                                         "\nThe reference year YYYY stands for YYYY-01-01 00:00:00.0" 
                                         "\nIf no reference date is specified the default reference date (1950) is used."
                                         "\n For instance:"
                                         "\n '2530 230 4569@2000'"
                                         "\n '850.2536985@1990'"
                                         "\n '850.2536985@1950' is equal to '850.2536985'"
                                         "\n\nDates prior to 1950-01-01 00:00:00.0 are invalid", 
                         NULL, CDate::CvDate, DateTime),
};

static const DefFunction2 Fonctions2[]	  =
{
	DefFunction2("max",		"Calculates the larger of two values",
                     CTools::Max,   Statistical),
	DefFunction2("min",		"Calculates the smaller of two values",
                     CTools::Min,   Statistical),
	DefFunction2("mod",		"Calculates the floating-point remainder",
                     CTools::Mod),
	DefFunction2("deg_normalize",	"Normalizes longitude (degree)",
                     CTools::NormalizeLongitude, Geographical),
	DefFunction2("rnd",		"Calculates the rounded value with a decimal precision (e.g round(20.23645, 3) ==> 20.236)",
                     CTools::Rnd,   MathTrigo, false),

//	DefFunction2("dn",		"Normalizes longitude (degree)",
//                     CTools::NormalizeLongitude),
};

static const DefFunction3 Fonctions3[]	  =
{
	DefFunction3("is_bounded",		"Checks if a value x is included between two value (min/max). \nis_bounded(min, x, max)",
                     CTools::IsBounded,   Relational),
//	DefFunction3("bnd",			"Checks if a value x is included between two value (min/max). \nbnd(min, x, max)",
//                     CTools::IsBounded),
	DefFunction3("is_bounded_strict",	"Checks if a value x is stricly included between two value (min/max). \nis_bounded_strict(min, x, max)",
                     CTools::IsBoundedStrict,   Relational),
//	DefFunction3("bnd_strict",		"Checks if a value x is stricly included between two value (min/max). \nbnd_strict(min, x, max)",
//                     CTools::IsBoundedStrict),
};

// External function to call algorithm from a user expression
// This is a 'virtual' function
static const DefFunctionAlgoN FonctionsAlgoN[]	  =
{
	DefFunctionAlgoN("exec",		"Execute an algorithm with variable arguments",
                     NULL,   Algorithm),
};

// Internal function to process algorithms (function to call to process algorithm)
static const DefFunctionBratAlgoBaseN FonctionsBratAlgoBaseN[]	  =
{
	DefFunctionBratAlgoBaseN(
                     CBratAlgorithmBase::ExecInternal),
};

static const DefConstant Constants[]	 =
{
	DefConstant("PI",	"PI value",       M_PI),
	DefConstant("PI2",	"PI/2 value",     M_PI_2),
	DefConstant("PI4",	"PI/4 value",     M_PI_4),
	DefConstant("DV",	"Default value",  CTools::m_defaultValueDOUBLE),
	DefConstant("dv",	"Default value",  CTools::m_defaultValueDOUBLE)
};



enum ExpressionValueType { CharacterType, FloatType };









//-------------------------------------------------------------
//------------------- CExpressionValue class --------------------
//-------------------------------------------------------------


/** \addtogroup tools Tools
  @{ */

/**
  Expression management classes.



 \version 1.0
*/

class CExpressionValue : public CBratObject
{
public:
  CExpressionValue(double			FloatValue  = CTools::m_defaultValueDOUBLE);
  
  CExpressionValue(const std::vector<double>		&FloatValues);
  
  CExpressionValue(const std::string			&StrValue);
  
  CExpressionValue(ExpressionValueType Type, ExpressionValueDimensions &Dimensions, double *Value, bool MakeCopy = true);
  
  CExpressionValue(ExpressionValueType type, ExpressionValueDimensions &dimensions, const CDoubleArray& value);

  CExpressionValue(const CExpressionValue	&Copy);
  
  CExpressionValue(ExpressionCallableFunction1 &Function, bool IsNumeric, CExpressionValue &Parameter1);

  CExpressionValue(ExpressionCallableFunctionStrToStr1 &Function, CExpressionValue &Parameter1);
  
  CExpressionValue(ExpressionCallableFunctionStrToFlt1 &Function, CExpressionValue &Parameter1);
  
  CExpressionValue(ExpressionCallableFunction2 &Function, bool	IsNumeric, CExpressionValue	&Parameter1, CExpressionValue	&Parameter2);
  
  CExpressionValue(ExpressionCallableFunction3 &Function, bool	IsNumeric, CExpressionValue	&Parameter1, CExpressionValue	&Parameter2, CExpressionValue	&Parameter3);

//  CExpressionValue(ExpressionCallableFunctionAlgoN& function, const std::string& functionName, const std::string& parametersFormat, CObList* listParams);
  CExpressionValue(ExpressionCallableFunctionAlgoN& function, const char* functionName, CVectorBratAlgorithmParam& arg);
  CExpressionValue(ExpressionCallableFunctionBratAlgoBaseN& function, CBratAlgorithmBase* algo, CVectorBratAlgorithmParam& arg);

  ~CExpressionValue();

  CExpressionValue& operator=(const CExpressionValue	&Copy);
  
  CExpressionValue& operator=(const std::string		&String);
  
  CExpressionValue& operator=(double value);
  
  CExpressionValue& operator=(const std::vector<double>	&Vector);

  void SetNewValue(ExpressionValueType	type, uint32_t* dims, uint32_t nbDims, double	*value, bool makeCopy = true);
  void SetNewValue(ExpressionValueType Type, ExpressionValueDimensions	&Dimensions, double	*Value, bool MakeCopy = true);
  void SetNewValue(CDoubleArray& vect, bool	makeCopy = true);
  void SetNewValue(CObDoubleMap& mp, bool	makeCopy = true);
  void SetNewValue(CDoublePtrDoubleMap& mp, bool	makeCopy = true);
  void SetNewValue(double* dataValue, uint32_t nbValues, bool	makeCopy = true);
  //void SetNewValue(ExpressionCallableFunctionAlgoN& function, const char* functionName, const char* parametersFormat, ...);

  ExpressionValueType	GetType() const { return m_Type; }
  
  size_t GetNbDimensions() const	{ return m_Dimensions.size(); }
  
  const ExpressionValueDimensions	&GetDimensions() const { return m_Dimensions; }
  
  size_t GetNbValues() const;

  double *GetValues() const;

  double GetValue(uint32_t index) const;
  double GetValue(uint32_t i, uint32_t j) const;

  std::string GetString() const;

  double Compare(CExpressionValue	&WithWhat);

  int32_t IsTrue();

  std::string GetName() { return m_name; }
  void SetName(const std::string& value) { m_name = value; }

  void DeleteValue();

  bool HasValue();



// Format and unit are ignored if the value is a std::string
// If Unit is a date, Format is a sting suitable for the AsString method
// of the CDate object
// If unit is not a date, it must be a valid unit std::string indicating
// the unit the data must be converted into. The value itself
// is considered to be in the base unit of the specified unit.
// If no unit is furnished, the numbers are returned without any
// convertion.
  std::string AsString(const CUnit  &Unit	= CUnit(""), const std::string Format	= "", bool dateAsPeriod = false) const;

  std::string GetDimensionsAsString();

  void Set(const CExpressionValue &Copy);

  static CExpressionValue* GetExpressionValue(CBratObject* ob, bool withExcept = true);

  void Dump(std::ostream	&fOut = std::cerr);

protected:

  //-------------------------------------------------------------
  //------------------- CValueData class --------------------
  //-------------------------------------------------------------


  class CValueData 
  {

  public:

    CValueData(uint32_t nbVal = 0, double  *data	= NULL, bool makeCopy	= true);
    
    CValueData(const CDoubleArray& data);

    CValueData(const CValueData&);
    

    virtual ~CValueData();

    void Set(const CValueData &copy);

    CValueData & operator=(const CValueData&);

    void Dump(std::ostream	&fOut = std::cerr);

  public:

    size_t	m_refCount;
    size_t	m_nbValues;
    bool	m_allocated;
    double	*m_dataValue;

  protected:

    void Init();

    void DeleteDataValue();

    double* NewValue(size_t nbValues);


  };





private:

  ExpressionValueType		m_Type;
  ExpressionValueDimensions	m_Dimensions;
  //struct  ValueData;
  //ValueData			*m_Value;

  CValueData *m_Value;

  std::string m_name;

  void Init();

  void BuildValue(ExpressionValueType	type, uint32_t* dims, uint32_t nbDims, double	*value, bool makeCopy);
  void BuildValue(ExpressionValueType	Type, ExpressionValueDimensions	&Dimensions, double	*Value = NULL, bool	MakeCopy = true);
  void BuildValue(ExpressionValueType	type, ExpressionValueDimensions	&dimensions, const CDoubleArray& value);

  void BuildFromString
	  (const std::string			&StrValue);

  void BuildFromVector
	  (const std::vector<double>		&VectValue);

  static void CheckDimensionsCombination
	  (const CExpressionValue		&Parameter1,
	   const CExpressionValue		&Parameter2,
	   const CExpressionValue		&Parameter3,
	   ExpressionValueDimensions	&Dimensions);

  static void CheckDimensionsCombination(const CUIntArray	&dim1, const CUIntArray	&dim2, const CUIntArray	&dim3, CUIntArray& dimensions);

  static void CheckDimensionsCombination(const CUIntArray** params, uint32_t nbParams, CUIntArray& dimensions);

    void SetDimensionsCombination
	  (const CExpressionValue		&Parameter1,
	   const CExpressionValue		&Parameter2,
	   const CExpressionValue		&Parameter3);

};

//-------------------------------------------------------------
//------------------- CExpressionValues class --------------------
//-------------------------------------------------------------

class CExpressionValues : public CBratObject
{

public:   
  /// Ctor
  CExpressionValues(CObArray* expressionDataValues = NULL);  

  /// Dtor
  virtual ~CExpressionValues();
  
  CObArray* GetExpressionDataValues() {return m_expressionDataValues;}

  double GetExpressionValueAsDouble(uint32_t index = 0);
  double GetExpressionValueAsDouble(uint32_t x, uint32_t y);

  void GetExpressionDataValuesAsArrayOfSingleValue(uint32_t index, double*& values, uint32_t& nbValues);

  size_t GetNbDimensions() const;
  
  void SetDimensions(uint32_t i);
  void SetDimensions(uint32_t i, uint32_t j);
  void SetDimensions(const CUIntArray& dimensions);
  uint32_t GetDimensions(uint32_t indexDim) const;
  
  static CExpressionValues* GetExpressionValues(CBratObject* ob, bool withExcept = true);


  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

protected:
  void Init();
  void DeleteExpressionDataValues();

protected:

  CUIntArray m_dimensions;

  /// A CExpression values array
  CObArray* m_expressionDataValues;

};                      


//-------------------------------------------------------------
//------------------- CExpression class --------------------
//-------------------------------------------------------------

class CExpression : public CBratObject
{
public:

  CExpression(const std::string &StrExpression = "");

  CExpression(const CExpression& e);

  virtual ~CExpression();

  virtual CBratObject* Clone();

  void SetExpression(const CExpression& e);

  void SetExpression(const std::string &StrExpression);

  static bool SetExpression(const char* value, CExpression& expr, std::string& errorMsg);
  static bool SetExpression(const std::string& value, CExpression& expr, std::string& errorMsg);

  bool IsConstant(const std::string& value) const;

  CExpressionValue Execute(CProduct* product = NULL);
  
  void Dump(std::ostream &Output = std::cerr);
  
  std::string AsString() const;

  CExpressionValue GetResult();
  
  const CStringArray* GetFieldNames() const { return &m_fieldNames; }
  const CObArray* GetConstants() const { return &m_constants; }
  const CVectorBratAlgorithm* GetAlgorithms() const { return &m_algoArray; }
  const CUIntArray* GetCode() const	{ return &m_code; }

  bool HasFieldNames() const	{ return m_fieldNames.size() > 0; }
  bool HasConstants() const	{ return m_constants.size() > 0; }
  bool HasAlgorithms() const	{ return m_algoArray.size() > 0; }
  
  size_t GetNbFieldNames() const	{ return m_fieldNames.size(); }
  size_t GetNbConstants() const	{ return m_constants.size(); }
  size_t GetNbAlgorithms() const	{ return m_algoArray.size(); }
  
  
  void SetValue(std::string FieldName, CExpressionValue* value);
  void SetValue(std::string fieldName, CExpressionValue& value);
  void SetValue(std::string fieldName, double value);

  void DeleteValues();
  void DeleteConstants();
  void DeleteDataStack();

  static CExpression* GetExpression(CBratObject* ob, bool withExcept = true);
  static bool GetFieldNames(const std::string& stringExpr, CStringArray& fieldNames, std::string& errorMsg);

  const CExpression& operator= (const CExpression& e);
  const CExpression& operator= (const std::string& str);

  bool operator== (CExpression& u);
  bool operator== (const std::string& text);
  bool operator!= ( CExpression& e ) { return ! (*this == e); }
  bool operator!= ( const std::string& text ) { return ! (*this == text); }


protected:

  CUIntArray		m_code;
  //std::vector<CExpressionValue>	m_constants;
  // A "CExpressionValue*" array
  CObArray	m_constants;
  CStringArray		m_fieldNames;
  //std::vector<CExpressionValue>	m_fieldValues;
  // A "CExpressionValue*" array
  CObArray	m_fieldValues;
  //stack<CExpressionValue>	m_dataStack;
  // A "CExpressionValue*" stack
  CObStack m_dataStack;

  int32_t m_rangeAlgo;

  CVectorBratAlgorithm m_algoArray; // Algo mentionned in the expression

public:


};

/** @} */

}
#endif // !defined(_Expression_h_)
