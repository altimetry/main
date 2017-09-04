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

#include "BratEmbeddedPythonProcess.h"

using namespace brathl;
using namespace processes;

namespace processes
{

//-------------------------------------------------------------
//------------------- CBratProcessZFXY class --------------------
//-------------------------------------------------------------

class CBratProcessZFXY : public CBratEmbeddedPythonProcess
{
    DECLARE_BASE_TYPE( CBratEmbeddedPythonProcess )

public:
  CBratProcessZFXY( const std::string &python_dir );
    
  virtual ~CBratProcessZFXY();

  bool CheckCommandLineOptions(int argc, char	**argv);

  virtual int32_t Execute(std::string& msg);

  static bool LoessFilterGrid(CMatrix& matrix, CMatrix *result,
		                                   bool circularX, int_t waveLengthX,
		                                   bool circularY, int_t waveLengthY,
                                       bool smoothData, bool extrapolData);


  static bool LoessFilterGrid(CMatrixDoublePtr& matrix, CMatrixDoublePtr *result,
		                                   bool circularX, int_t waveLengthX,
		                                   bool circularY, int_t waveLengthY,
                                       bool smoothData, bool extrapolData);

  static bool LoessFilterGrid(CMatrixDouble& matrix, CMatrixDouble *result,
		                                   bool circularX, int_t waveLengthX,
		                                   bool circularY, int_t waveLengthY,
                                       bool smoothData, bool extrapolData);

  static bool LoessFilterGrid(double	*data, double	*result,
		                                   int_t nbX, bool circularX, int_t waveLengthX,
		                                   int_t nbY, bool circularY, int_t waveLengthY,
		                                   bool smoothData, bool extrapolData);

public:
  static const KeywordHelp ZFXYKeywordList[];

protected:

  void Init();
  void DeleteData();

  void ResizeArrayDependOnFields(uint32_t size);

  virtual void AddFieldIndexes(CFieldIndex* fieldIndex, CNetCDFVarDef* varDef);
  

  virtual void GetParameters(const std::string &commandFileName) override;
  virtual void GetParameters() override;

  //virtual void CheckNetCdfFields(CStringArray& fields);

  virtual void BeforeBuildListFieldsToRead() override;
  virtual void AfterBuildListFieldsToRead() override;

  void AddComplementDimensionsFromNetCdf();

  virtual bool IsOutputAxis(const std::string& name) { return (name == m_xName) || (name == m_yName) ; };


  void InitGrids();

  CMatrix* CreateGrid(uint32_t index, const CExpressionValue& exprValue);

  CMatrixDoublePtr* CreateGridDoublePtr(uint32_t index,const CExpressionValue& exprValue);
  CMatrixDouble* CreateGridDouble(uint32_t index);

  CMatrixDoublePtr* NewMatrixDoublePtr();
  CMatrixDouble* NewMatrixDouble();


  void RegisterData();


  void ResizeDataValues(CDoubleArrayOb* dataValues, uint32_t nbValues);
  
  int32_t WriteData();

  void GetDataValuesFromMatrix(size_t indexExpr, uint32_t xPos, uint32_t yPos, double *&dataValues, size_t& nbValues);

  void GetDataValuesFromMatrix(size_t indexExpr, uint32_t xPos, uint32_t yPos, double *&dataValues, 
	  double *&countValues, double *&meanValues, size_t& nbValues);

  virtual void SubstituteAxisDim(const CStringArray& fieldDims, CStringArray& fieldDimsOut);
  virtual void OnAddDimensionsFromNetCdf();


  bool CheckValuesSimilar(uint32_t indexExpr, double* dataValues, size_t nbValues, uint32_t xPos, uint32_t yPos, std::string& msg);
  //bool CheckValuesSimilar(uint32_t indexExpr);

  /*
  void LoessFilterGrid(double	*data, double	*result,
		                   int32_t nbX, bool circularX, int32_t waveLengthX,
		                   int32_t nbY, bool circularY, int32_t waveLengthY,
		                   bool smoothData, bool extrapolData);
  */
  double* ComputeMergeDataValueParameters(CRecordSet* recordSet, uint32_t index, double xPos, double yPos);
  

  CMatrix* GetMatrix( size_t index );
  CMatrixDouble* GetMatrixDouble( size_t index );
  CMatrixDoublePtr* GetMatrixDoublePtr( size_t index );

protected:

  double m_xMin;
  double m_xMax;
  size_t m_xCount;
  double m_xStep;
  bool m_xCircular;
  int32_t m_xLoessCutoff;


  std::string m_yComment;
  double m_yMin;
  double m_yMax;
  size_t m_yCount;
  double m_yStep;
  bool m_yCircular;
  int32_t m_yLoessCutoff;

  CBratEmbeddedPythonProcess::PositionMode m_positionMode;
  CBratEmbeddedPythonProcess::OutsideMode m_outsideMode;

  std::vector<bool> m_smooth;
  std::vector<bool> m_extrapolate;

  size_t m_nbDataByGrid;
  
  //CDoubleArray m_xData;
  //CDoubleArray m_yData;
  double *	m_xData;
  double * m_yData;

private:

	// Grids data array, contains as many CMatrixDoublePtr or CMatrixDouble as field expression (m_nbFields)
  // 
  CObjectPointersArray<CMatrix> m_grids;
};




}




#endif
