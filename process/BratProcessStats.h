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
#ifndef _BratProcessStats_h_
#define _BratProcessStats_h_

#include "BratProcess.h"
#include "brathlc.h"

using namespace brathl;
using namespace processes;

namespace processes
{

//-------------------------------------------------------------
//------------------- CBratProcessStats class --------------------
//-------------------------------------------------------------

class CBratProcessStats : public CBratProcess
{
public:
  CBratProcessStats();
    
  virtual ~CBratProcessStats();

  virtual bool Initialize(string& msg);

  bool CheckCommandLineOptions(int argc, char	**argv);

  virtual int32_t Execute(string& msg);

public:
  static const KeywordHelp StatsKeywordList[];

protected:

  void Init();
  void ResizeArrayDependOnFields(uint32_t size);

  virtual void GetParameters(const string &commandFileName);
  virtual void GetParameters();

protected:


};




}





#endif
