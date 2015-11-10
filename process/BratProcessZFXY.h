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
#ifndef _BratProcessZFXY_h_
#define _BratProcessZFXY_h_

#include "BratProcess.h"

using namespace brathl;
using namespace processes;

namespace processes
{

//-------------------------------------------------------------
//------------------- CBratProcessZFXY class --------------------
//-------------------------------------------------------------

class CBratProcessZFXY : public CBratProcess
{
public:
  CBratProcessZFXY();
    
  virtual ~CBratProcessZFXY();

  bool CheckCommandLineOptions(int argc, char	**argv);

  virtual int32_t Execute(std::string& msg);

  static bool LoessFilterGrid(CMatrix& matrix, CMatrix *result,
		                                   bool circularX, int32_t waveLengthX,
		                                   bool circularY, int32_t waveLengthY,
                                       bool smoothData, bool extrapolData);


  static bool LoessFilterGrid(CMatrixDoublePtr& matrix, CMatrixDoublePtr *result,
		                                   bool circularX, int32_t waveLengthX,
		                                   bool circularY, int32_t waveLengthY,
                                       bool smoothData, bool extrapolData);

  static bool LoessFilterGrid(CMatrixDouble& matrix, CMatrixDouble *result,
		                                   bool circularX, int32_t waveLengthX,
		                                   bool circularY, int32_t waveLengthY,
                                       bool smoothData, bool extrapolData);

  static bool LoessFilterGrid(double	*data, double	*result,
		                                   int32_t nbX, bool circularX, int32_t waveLengthX,
		                                   int32_t nbY, bool circularY, int32_t waveLengthY,
		                                   bool smoothData, bool extrapolData);

public:
  static const KeywordHelp ZFXYKeywordList[];

protected:

  void Init();
  void DeleteData();

  void ResizeArrayDependOnFields(uint32_t size);

  virtual void AddFieldIndexes(CFieldIndex* fieldIndex, CNetCDFVarDef* varDef);
  

  virtual void GetParameters(const std::string &commandFileName);
  virtual void GetParameters();

  //virtual void CheckNetCdfFields(CStringArray& fields);

  virtual void BuildListFieldsToRead();
  virtual void BeforeBuildListFieldsToRead();
  virtual void AfterBuildListFieldsToRead();

  void AddComplementDimensionsFromNetCdf();

  virtual bool IsOutputAxis(const std::string& name) { return (name == m_xName) || (name == m_yName) ; };


  void InitGrids();

  CMatrix* CreateGrid(uint32_t index, const CExpressionValue& exprValue);

  CMatrixDoublePtr* CreateGridDoublePtr(uint32_t index,const CExpressionValue& exprValue);
  CMatrixDouble* CreateGridDouble(uint32_t index);

  CMatrixDoublePtr* NewMatrixDoublePtr();
  CMatrixDouble* NewMatrixDouble();

  double* NewDoubleArray(uint32_t nbElts);


  void RegisterData();


  void ResizeDataValues(CDoubleArrayOb* dataValues, uint32_t nbValues);
  
  int32_t WriteData();

  void GetDataValuesFromMatrix(uint32_t indexExpr, uint32_t xPos, uint32_t yPos, DoublePtr& dataValues, uint32_t& nbValues);

  //void GetDataValuesFromMatrix(CMatrixDouble* matrix, uint32_t indexExpr, uint32_t xPos, uint32_t yPos, DoublePtr& dataValues, uint32_t& nbValues);
  //void GetDataValuesFromMatrix(CMatrixDoublePtr* matrix, uint32_t indexExpr, uint32_t xPos, uint32_t yPos, DoublePtr& dataValues, uint32_t& nbValues);

  void GetDataValuesFromMatrix(uint32_t indexExpr, uint32_t xPos, uint32_t yPos, 
                              DoublePtr& dataValues, DoublePtr& countValues, DoublePtr& meanValues, uint32_t& nbValues);

  virtual void SubstituteAxisDim(const CStringArray& fieldDims, CStringArray& fieldDimsOut);
  virtual void OnAddDimensionsFromNetCdf();


  bool CheckValuesSimilar(uint32_t indexExpr, double* dataValues, uint32_t nbValues, uint32_t xPos, uint32_t yPos, std::string& msg);
  //bool CheckValuesSimilar(uint32_t indexExpr);

  /*
  void LoessFilterGrid(double	*data, double	*result,
		                   int32_t nbX, bool circularX, int32_t waveLengthX,
		                   int32_t nbY, bool circularY, int32_t waveLengthY,
		                   bool smoothData, bool extrapolData);
  */
  

  CMatrix* GetMatrix(uint32_t index, bool withExcept = true);
  CMatrixDouble* GetMatrixDouble(uint32_t index, bool withExcept = true);
  CMatrixDoublePtr* GetMatrixDoublePtr(uint32_t index, bool withExcept = true);

  static CMatrix* GetMatrix(CBratObject* ob, bool withExcept = true);
  static CMatrixDouble* GetMatrixDouble(CBratObject* ob, bool withExcept = true);
  static CMatrixDoublePtr* GetMatrixDoublePtr(CBratObject* ob, bool withExcept = true);

  bool IsMatrixDouble(uint32_t index);
  bool IsMatrixDoublePtr(uint32_t index);

  static bool IsMatrixDouble(CMatrix* matrix);
  static bool IsMatrixDoublePtr(CMatrix* matrix);

  static CUIntArray* CheckMatrixDataPtr(CUIntArray* matrixData);



protected:

  double m_xMin;
  double m_xMax;
  uint32_t m_xCount;
  double m_xStep;
  bool m_xCircular;
  int32_t	m_xLoessCutoff;


  std::string m_yComment;
  double m_yMin;
  double m_yMax;
  uint32_t m_yCount;
  double m_yStep;
  bool m_yCircular;
  int32_t	m_yLoessCutoff;

  CBratProcess::PositionMode m_positionMode;
  CBratProcess::OutsideMode m_outsideMode;

  std::vector<bool> m_smooth;
  std::vector<bool> m_extrapolate;

  uint32_t m_nbDataByGrid;
  
  //CDoubleArray m_xData;
  //CDoubleArray m_yData;
  DoublePtr	m_xData;
  DoublePtr m_yData;

  // Grids data array, contains as many CMatrixDoublePtr object as field expression (m_nbFields)
  CObArray m_grids;



  //CDoublePtrDoubleMap m_measures;
//  CUIntArray m_matrixDims;


};




}





#endif
