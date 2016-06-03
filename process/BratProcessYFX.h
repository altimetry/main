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
#ifndef _BratProcessYFX_h_
#define _BratProcessYFX_h_

#include "BratEmbeddedPythonProcess.h"

using namespace brathl;
using namespace processes;

namespace processes
{
//-------------------------------------------------------------
//------------------- CBratProcessYFX class --------------------
//-------------------------------------------------------------

class CBratProcessYFX : public CBratEmbeddedPythonProcess
{
	using base_t = CBratEmbeddedPythonProcess;

public:
  CBratProcessYFX( const std::string &executable_dir );
    
  virtual ~CBratProcessYFX();

  bool CheckCommandLineOptions(int argc, char	**argv);

  virtual int32_t Execute(std::string& msg);

public:
  static const KeywordHelp YFXKeywordList[];

protected:

  void Init();
  void ResizeArrayDependOnFields(uint32_t size);

  virtual void AddFieldIndexes(CFieldIndex* fieldIndex, CNetCDFVarDef* varDef);
  

  virtual void GetParameters(const std::string &commandFileName);
  virtual void GetParameters();

  //virtual void CheckNetCdfFields(CStringArray& fields);

  virtual void BuildListFieldsToRead();
  virtual void BeforeBuildListFieldsToRead();
  virtual void AfterBuildListFieldsToRead();

  void AddComplementDimensionsFromNetCdf();

  virtual bool IsOutputAxis(const std::string& name) { return (name == m_xName); };


  void RegisterData();

  DoublePtr* GetOneMeasure(double key);

  void ResizeDataValues(CDoubleArrayOb* dataValues, uint32_t nbValues);
  
  int32_t WriteData();

  virtual void SubstituteAxisDim(const CStringArray& fieldDims, CStringArray& fieldDimsOut);
  virtual void OnAddDimensionsFromNetCdf();

  bool CheckValuesSimilar(uint32_t indexExpr, double* dataValues, uint32_t nbValues, std::string& msg);
  bool CheckValuesSimilar(uint32_t indexExpr);


protected:
  
  //int32_t m_nbMaxDataSlices;

  /*
   * m_measures : map of data and keys are X values
   *    Each key is associated with an non rectangular matrix  
   *            
  */

  //CObDoubleMap m_measures;
  CDoublePtrDoubleMap m_measures;
  CUIntArray m_matrixDims;

};




}





#endif
