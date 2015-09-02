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
#if !defined(_BratAlgorithmBase_h_)
#define _BratAlgorithmBase_h_

#include "brathl_error.h" 
#include "brathl.h" 

#include "Stl.h"

#include "List.h" 
#include "Tools.h" 

using namespace brathl;


#if defined (DEBUG) || defined(_DEBUG) 

#if !defined(BRAT_ALGO_EXAMPLE)
#define BRAT_ALGO_EXAMPLE
#endif

#ifndef _DEBUG_BRAT_ALGO
#define _DEBUG_BRAT_ALGO 0
#endif

#endif



namespace brathl
{

class CExpressionValue;
class CExpressionValues;
class CExpression;
class CProduct;
class CProductNetCdf;
class CFieldNetCdf;

//-------------------------------------------------------------
//------------------- CBratAlgorithmParam class --------------------
//-------------------------------------------------------------
class CBratAlgorithmParam
{
public:
    typedef enum {
    T_UNDEF = 0, 
    T_INT,
    T_LONG,
    T_FLOAT,
    T_DOUBLE,
    T_CHAR,
    T_STRING,
    T_VECTOR_DOUBLE,
  } bratAlgoParamTypeVal;

public:
  CBratAlgorithmParam();
  
  CBratAlgorithmParam(const CBratAlgorithmParam &o);
  
  virtual ~CBratAlgorithmParam();

  bratAlgoParamTypeVal GetTypeVal() {return m_typeVal; };
  string GetTypeValAsString() {return TypeValAsString(m_typeVal); };

  string GetValue();

  double GetValueAsDouble() {return m_valDouble; };
  int32_t GetValueAsInt() {return m_valInt;};
  int64_t GetValueAsLong() {return m_valLong;};
  float GetValueAsFloat() {return m_valFloat;};
  string GetValueAsString() {return m_valString;};
  unsigned char GetValueAsChar() {return m_valChar;};
  CDoubleArray* GetValueAsVectDouble() {return &m_vectValDouble;};

  void SetValue(double value);
  void SetValue(float value);
  void SetValue(int32_t value);
  void SetValue(int64_t value);
  void SetValue(const string& value);
  void SetValue(unsigned char value);
  void SetValue(const CDoubleArray& value);
  void SetValue(double* value, int32_t nbValues);

  void SetValue(double value, CBratAlgorithmParam::bratAlgoParamTypeVal type);
  void SetValue(const string& value, CBratAlgorithmParam::bratAlgoParamTypeVal type);

  void SetValueAsDoubleArray(const string& value, const string& delim = ","); 

  bratAlgoParamTypeVal TypeVal(bratAlgoParamTypeVal type) { return type; };

  static string TypeValAsString(bratAlgoParamTypeVal type);

  virtual void Dump(ostream& fOut = cerr);

  const CBratAlgorithmParam& operator=(const CBratAlgorithmParam &o);
  //virtual bool operator==(const CBratAlgorithmParam& o) { return true; };


protected:
  void Set(const CBratAlgorithmParam &o);
  void Init();
  void Copy(const CDoubleArray& value);

protected:
  
  bratAlgoParamTypeVal m_typeVal;

  double m_valDouble;
  float m_valFloat;
  int32_t m_valInt;
  int64_t m_valLong;
  string m_valString;
  unsigned char m_valChar;
  CDoubleArray m_vectValDouble;
    

};

typedef vector<CBratAlgorithmParam> vectorbratalgorithmparam; 

//-------------------------------------------------------------
//------------------- CVectorBratAlgorithmParam class --------------------
//-------------------------------------------------------------


class CVectorBratAlgorithmParam : public vectorbratalgorithmparam
{
public:
  CVectorBratAlgorithmParam();  
  virtual ~CVectorBratAlgorithmParam();

  virtual void Insert(const CBratAlgorithmParam& o);
  virtual void RemoveAll();

  void Insert(double value);
  void Insert(float value);
  void Insert(int32_t value);
  void Insert(const string& value);
  void Insert(unsigned char value);
  void Insert(const CDoubleArray& value);
  void Insert(double* value, int32_t nbValues);

  void Insert(double value, CBratAlgorithmParam::bratAlgoParamTypeVal type);
  void Insert(const string& value, CBratAlgorithmParam::bratAlgoParamTypeVal type);
  
  string ToString(const string& delim = ",", bool useBracket = true);
  
  virtual void Dump(ostream& fOut = cerr); 


protected:

  void Init();

protected:


};

//-------------------------------------------------------------
//------------------- CBratAlgorithmBase class --------------------
//-------------------------------------------------------------

/** \addtogroup Algorithm Algorithms classes
  @{ */

/** 
  Algorithm base class. 
*/

class CBratAlgorithmBase
//class CBratAlgorithmBase : public boost::noncopyable
{
public:
  /** Default contructor */
  CBratAlgorithmBase();
  /** Copy contructor */
  CBratAlgorithmBase(const CBratAlgorithmBase	&o);

  /** Destructor */
  virtual ~CBratAlgorithmBase();

  /** Runs the algorithm
      \param fmt [in] : a string that indicates the format of each value of input parameters (number, string) :
        %d for integer
        %l for long integer
        %f for double
        %s for string
      \param args [in] : the values of input parameters i(a C/C++ va_list).

     \return the result of the execution */
  virtual double Run(CVectorBratAlgorithmParam& args) = 0;
  
  /** Gets the name of the algorithm */
  virtual string GetName() = 0;
  /** Gets the description of the algorithm */
  virtual string GetDescription() = 0;
  /** Gets the number of input parameters to pass to the 'Run' function */
  virtual uint32_t GetNumInputParam() = 0;
  /** Gets the description of an input parameter.
   \param indexParam [in] : parameter index. First parameter index is 0, last one is 'number of parameters - 1'.
   */
  virtual string GetInputParamDesc(uint32_t indexParam) = 0;
  /** Gets the format of an input parameter :
      CBratAlgorithmParam::T_DOUBLE for double
      CBratAlgorithmParam::T_FLOAT for float
      CBratAlgorithmParam::T_INT for integer
      CBratAlgorithmParam::T_LONG for long integer
      CBratAlgorithmParam::T_STRING for string
      CBratAlgorithmParam::T_CHAR for a character
   \param indexParam [in] : parameter index. First parameter index is 0, last one is 'number of parameters - 1'.
   */
  virtual CBratAlgorithmParam::bratAlgoParamTypeVal GetInputParamFormat(uint32_t indexParam) = 0;
  /** Gets the unit of an input parameter :
   \param indexParam [in] : parameter index. First parameter index is 0, last one is 'number of parameters - 1'.
   */
  virtual string GetInputParamUnit(uint32_t indexParam) = 0;
  /** Gets the unit of an output value returned by the 'Run' function.
   */
  virtual string GetOutputUnit() = 0;
  
  //virtual string GetParamFormats();
  virtual string GetParamName(uint32_t /*indexParam*/) {return "";};
  virtual double GetParamDefaultValue(uint32_t /*indexParam*/) {return CTools::m_defaultValueDOUBLE;};
  
  void GetParamDefValue(uint32_t indexParam, double& value);
  void GetParamDefValue(uint32_t indexParam, float& value);
  void GetParamDefValue(uint32_t indexParam, uint32_t& value);
  void GetParamDefValue(uint32_t indexParam, uint64_t& value);
  void GetParamDefValue(uint32_t indexParam, int32_t& value);
  void GetParamDefValue(uint32_t indexParam, int64_t& value);

  string GetParamDefValueAsString(uint32_t indexParam);
  string GetParamDefValueAsLabel(uint32_t indexParam);

  string GetInputParamDescWithDefValueLabel(uint32_t indexParam);

  virtual string GetInputParamFormatAsString(uint32_t indexParam) {
    CBratAlgorithmParam::bratAlgoParamTypeVal type = GetInputParamFormat(indexParam);
    return CBratAlgorithmParam::TypeValAsString(type);
  };

  string GetAlgoExpression() { return m_algoExpression; };
  void SetAlgoExpression(const string& value);

  CObArray* GetAlgoParamExpressions() { return &m_algoParamExpressions; };
  void SetAlgoParamExpressions(const CStringArray& values);

  virtual void CheckInputParams(CVectorBratAlgorithmParam& args);
  virtual void CheckInputTypeParams(uint32_t index, CBratAlgorithmParam::bratAlgoParamTypeVal expectedType, CVectorBratAlgorithmParam& args);
  virtual void CheckInputTypeParams(uint32_t index, const CIntArray& expectedTypes, CVectorBratAlgorithmParam& args);

  virtual void SetProduct(CProduct* product, bool forceReplace = false);

  void SetAlgoParamExpressions(const CObArray& obArray);

  CProductNetCdf* GetProductNetCdf(CProduct* product);

  string GetSyntax();

  void CheckConstantParam(uint32_t indexParam);

  /** Overloads operator '=' */
  CBratAlgorithmBase& operator=(const CBratAlgorithmBase &o);

  /** Dump function */
  virtual void Dump(ostream& fOut = cerr);
 
  //static double Exec(const char* function, const char *fmt, const type_union *arg);
  static CBratAlgorithmBase* GetNew(const char* algorithName);

  static double ExecInternal(CBratAlgorithmBase* algo, CVectorBratAlgorithmParam& arg);

  static void RegisterAlgorithms();

protected:

  void Init();
  void Set(const CBratAlgorithmBase &o);

  virtual void DeleteFieldNetCdf();

  virtual void DeleteProduct();
  virtual void DeleteExpressionValuesArray();
  virtual void NewExpressionValuesArray();

  virtual void GetData1D(int32_t iRecord);
  virtual void GetNextData();
  virtual void OpenProductFile();
  virtual void OpenProductFile(CProduct* product);
  virtual void ProcessOpeningProductNetCdf();
  virtual void ProcessOpeningProductNetCdf(CProduct* product);


  virtual uint32_t ReadProductData(int32_t iRecord);
  virtual uint32_t ReadProductData(int32_t iRecord, CExpression* expression);
  virtual uint32_t ReadProductData(int32_t iRecord, const CObArrayOb& algoParamExpressions);
  virtual uint32_t ReadProductData(CProduct* product, int32_t iRecord, const CObArrayOb& arrayExpressions);

  virtual void GetAllData(CExpression* expression, CDoubleArray& data);

  virtual void SetBeginOfFile();
  virtual void SetEndOfFile();

  virtual void SetPreviousValues(bool fromProduct);
  virtual void SetNextValues();

  double GetDataValue(uint32_t indexExpr);
  double GetDataValue(uint32_t indexExpr, uint32_t x);
  double GetDataValue(uint32_t indexExpr, uint32_t x, uint32_t y);

  CObArray* GetDataExpressionValues(uint32_t indexExpr);
  
  void GetExpressionDataValuesAsArrayOfSingleValue(uint32_t indexExpr, double*& values, uint32_t& nbValues);
  
  virtual void CheckComplexExpression(uint32_t index);

  virtual void CheckVarExpression2D(uint32_t index);
  
  void InitComplexExpressionArray();
  
  void AddXOrYFieldDependency(CFieldNetCdf* field, CFieldNetCdf* field2DAsRef);
  void AddXOrYFieldDependency(CFieldNetCdf* field, const string& xDimName, const string& yDimName);

  void SetField2DAsRef();
  CFieldNetCdf* GetField2DAsRef();

  virtual void PrepareDataValues2DOneField(CExpressionValue& exprValue, uint32_t algoExprIndex);
  virtual void PrepareDataValues2DComplexExpression(CExpressionValue& exprValue, uint32_t algoExprIndex);
  virtual void PrepareDataValues2DComplexExpressionWithAlgo(CExpressionValue& exprValue, uint32_t algoExprIndex);


protected:

  CProduct* m_callerProduct;
  CProduct* m_product;

  //The expression called for this algorithm instance
  string m_algoExpression;
  //The algorithm parameters as expressions
  CObArrayOb m_algoParamExpressions;
  CStringList m_listFieldsToRead;
  CObArray* m_expressionValuesArray;

  int32_t m_indexRecordToRead;
  int32_t m_nProductRecords;

  int32_t m_callerProductRecordPrev;

  string m_currentFileName;

  CObMap m_fieldDependOnXDim;
  CObMap m_fieldDependOnYDim;
  CObMap m_fieldDependOnXYDim;

  CObMap m_fieldVarsCaller;
  CObMap m_fieldVars;
  //CObMap m_fieldVarsCache;

  CFieldNetCdf* m_field2DAsRef;

  vector<bool> m_isComplexExpression;
  vector<bool> m_isComplexExpressionWithAlgo;

  CIntArray m_expectedTypes;

  CDoubleArray* m_varValueArray;

  static bool m_algorithmsRegistered;



};

typedef map<string, CBratAlgorithmBase*> mapbratalgorithmbase; 
typedef vector<CBratAlgorithmBase*> vectorbratalgorithmbase; 

//-------------------------------------------------------------
//------------------- CMapBratAlgorithm class --------------------
//-------------------------------------------------------------


class CMapBratAlgorithm : public mapbratalgorithmbase
{
public:
  CMapBratAlgorithm(bool bDelete = true);  
  virtual ~CMapBratAlgorithm();

  virtual CBratAlgorithmBase* Insert(const string& key, CBratAlgorithmBase* ob, bool withExcept = true);

  virtual void RemoveAll();

  virtual void Dump(ostream& fOut  = cerr); 

  virtual CBratAlgorithmBase* Find(const string& algoName) const;

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

  string GetSyntaxAlgo(const string& name);

protected:

  void Init();

protected:

  bool m_bDelete;



};

//-------------------------------------------------------------
//------------------- CVectorBratAlgorithm class --------------------
//-------------------------------------------------------------


class CVectorBratAlgorithm : public vectorbratalgorithmbase
{
public:
  CVectorBratAlgorithm(bool bDelete = true);  
  virtual ~CVectorBratAlgorithm();

  virtual void Insert(CBratAlgorithmBase* ob, bool bEnd = true);
  virtual CVectorBratAlgorithm::iterator InsertAt(CVectorBratAlgorithm::iterator where, CBratAlgorithmBase* ob);

  virtual void RemoveAll();

  virtual void Dump(ostream& fOut  = cerr); 

  bool GetDelete() {return m_bDelete;};
  void SetDelete(bool value) {m_bDelete = value;};

protected:

  void Init();

protected:

  bool m_bDelete;



};
//-------------------------------------------------------------
//------------------- CBratAlgorithmBaseRegistry class --------------------
//-------------------------------------------------------------

template<class T> CBratAlgorithmBase* base_factory()
{
  return new T;
}

typedef CBratAlgorithmBase* (*base_creator)(void);

class CBratAlgorithmBaseRegistry
{
private:
  CBratAlgorithmBaseRegistry();
  
  ~CBratAlgorithmBaseRegistry();

  void FillAlgorithmsSortedArray();

private:
  std::vector<base_creator> m_bases;
  
  CMapBratAlgorithm m_mapBratAlgo;
  CVectorBratAlgorithm m_sortedArrayBratAlgo;

public:
  typedef std::vector<base_creator>::iterator iterator;
  
  void Add(base_creator);
  
  iterator Begin();
  
  iterator End();

  static bool AlgorithmCompare(CBratAlgorithmBase* T1, CBratAlgorithmBase* T2);


  static CBratAlgorithmBaseRegistry& GetInstance();        
  static CBratAlgorithmBase* Find(const string& algoName);
  static CBratAlgorithmBase* GetAlgorithm(const string& algoName, bool withExcept = true);
  static CMapBratAlgorithm* GetAlgorithms();
  static CVectorBratAlgorithm* GetAlgorithmsAsSortedArray(bool forceToFill = false);

};

//-------------------------------------------------------------
//------------------- CBratAlgorithmBaseRegistration class --------------------
//-------------------------------------------------------------

class CBratAlgorithmBaseRegistration
{
public:
  CBratAlgorithmBaseRegistration(base_creator);
};

#define AUTO_REGISTER_BASE(base) \
    CBratAlgorithmBaseRegistration _base_registration_ ## base(&base_factory<base>);



//-------------------------------------------------------------
//------------------- CBratAlgoExample1 class --------------------
//-------------------------------------------------------------

#ifdef BRAT_ALGO_EXAMPLE

class CBratAlgoExample1 : public CBratAlgorithmBase
{
public:
  CBratAlgoExample1() {}
  virtual ~CBratAlgoExample1() {}

  virtual string GetName() override
  {
      return "example1";
  }
  virtual string GetDescription() override
  {
      return "example 1 of algorithm - return (parameter 2 + parameter 4)";
  }
  virtual uint32_t GetNumInputParam() override
  {
      return 4;
  }
  virtual string GetInputParamDesc(uint32_t indexParam) override
  {
        string value = "";
        switch (indexParam) 
        {
          case 0: value = "first parameter"; break;
          case 1: value = "second parameter"; break;
          case 2: value = "third parameter"; break;
          case 3: value = "fourth parameter"; break;
          default: value = CTools::Format("CBratAlgoExample1#GetInputParamDesc: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam()); break;


        }
        return value;
  };
  virtual CBratAlgorithmParam::bratAlgoParamTypeVal  GetInputParamFormat(uint32_t indexParam) override
  {
        CBratAlgorithmParam::bratAlgoParamTypeVal value = CBratAlgorithmParam::T_UNDEF;
        switch (indexParam) 
        {
          case 0: value = CBratAlgorithmParam::T_DOUBLE; break;
          case 1: value = CBratAlgorithmParam::T_INT; break;
          case 2: value = CBratAlgorithmParam::T_STRING; break;
          case 3: value = CBratAlgorithmParam::T_LONG; break;
          default: throw CAlgorithmException(CTools::Format("CBratAlgoExample1#GetInputParamFormat: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam())); break;

        }
        return value;
  };

  // Not an override. Used by CBratAlgoExample1::Run
  //
  virtual void SetParamValues(CVectorBratAlgorithmParam& args);



  /** Gets the unit of an input parameter :
   \param indexParam [in] : parameter index.
   */
  virtual string GetInputParamUnit(uint32_t indexParam) override
  {
        string value = "";
        switch (indexParam) 
        {
          case 0: value = "degrees_north"; break;
          case 1: value = "m"; break;
          case 2: value = ""; break;
          case 3: value = "m"; break;
          default: value = CTools::Format("CBratAlgoExample1#GetInputParamUnit: out of range parameter index %d. Valid range is [0,%d].",
                     indexParam, GetNumInputParam()); break;

        }
        return value;
  };


  /** Gets the unit of an output value returned by the 'Run' function.
   \param indexParam [in] : parameter index.
   */
  virtual string GetOutputUnit() override
  {
      return "m";
  }

  virtual double Run(CVectorBratAlgorithmParam& args) override ;

  virtual void Dump(ostream& fOut = cerr) override ;


protected:

  double m_1;
  int32_t m_2;
  string m_3;
  int64_t m_4;


};

#endif

/** @} */
}

#endif // !defined(_BratAlgorithmBase_h_)
