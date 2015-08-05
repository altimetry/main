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
#if !defined(_CriteriaPass_h_)
#define _CriteriaPass_h_

#include "brathl_error.h" 
#include "brathl.h" 

#include "Stl.h"

#include "BratObject.h"
#include "List.h"
#include "Criteria.h"

using namespace brathl;

namespace brathl
{




//-------------------------------------------------------------
//------------------- CCriteriaPass class --------------------
//-------------------------------------------------------------


/** \addtogroup product Pass number Criteria
  @{ */

/** 
  Pass number Criteria management class.


 \version 1.0
*/





class CCriteriaPass : public CCriteria
{

public:
    

  ///Destructor
  virtual ~CCriteriaPass();

  /** Sets internal value to the default value (uninitialized)*/
  virtual void SetDefaultValue() = 0;
  /**
     * Tests whether date period have been initialized or not
     *
     * @return true if not initialized
     */
  virtual bool IsDefaultValue() = 0;

  static CCriteriaPass* GetCriteria(CBratObject* ob, bool withExcept = true);

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr);

protected:

  /// Empty CCriteriaPass ctor
  CCriteriaPass();

  void Init();

public:
    

protected:



private:
  

};

//-------------------------------------------------------------
//------------------- CCriteriaPassString class --------------------
//-------------------------------------------------------------


/** \addtogroup product Pass number Criteria
  @{ */

/** 
  Pass number (as string) Criteria management class.


 \version 1.0
*/





class CCriteriaPassString : public CCriteriaPass
{

public:
    
  /// Empty CCriteriaPassString ctor
  CCriteriaPassString();

  CCriteriaPassString(CCriteriaPassString& c);

  CCriteriaPassString(CCriteriaPassString* c);


   /**
     * Constructor from a string that contans passes delimited by a comma)
     * 
     * @param passes passes to set
    */
  CCriteriaPassString(const string& passes, const string& delimiter = CCriteriaPassString::m_delimiter); 
  
  /**
     * Constructor from a array that contains passes
     * 
     * @param array start and end dates
     */
  CCriteriaPassString(const CStringArray& array);


  ///Destructor
  virtual ~CCriteriaPassString();

  CStringArray* GetPasses() { return  &m_passes; };


  /**
     * Sets one or more passes from a string (delimited by a comma)
     * 
     * @param passes passes to set
    */
  void Set(const string& passes, const string& delimiter = CCriteriaPassString::m_delimiter);
  /**
     * Sets passes from a array
     * 
     * @param array array of passes
    */
  void Set(const CStringArray& array);

  void Set(CCriteriaPassString& c);

  /**
   * Creates the intersection of these passes with the given onee
   * @param passes intersect with this
   * @param intersect intersection passes
   * @return true, or false if there is no intersection
   */
  bool Intersect(const string& passes, CStringArray& intersect);
  /**
   * Creates the intersection of these passes with the given onee
   * @param passes intersect with this
   * @param intersect intersection passes
   * @return true, or false if there is no intersection
   */
  bool Intersect(CStringArray& passes, CStringArray& intersect);

  /** Sets internal value to the default value (uninitialized)*/
  void SetDefaultValue();
  /**
     * Tests whether passes have been initialized or not
     *
     * @return true if not initialized
     */
  bool IsDefaultValue();

  string GetAsText(const string& delimiter = CCriteriaPassString::m_delimiter);

  const CCriteriaPassString& operator=(CCriteriaPassString& c);

  static CCriteriaPassString* GetCriteria(CBratObject* ob, bool withExcept = true);

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr);

protected:

  void Init();

  static void ExtractPass(const string& passes, CStringArray& arrayPass, const string& delimiter = CCriteriaPassString::m_delimiter);
  static void ExtractPass(const CStringArray& array, CStringArray& arrayPass);

public:
   
  static const string m_delimiter;


protected:


  /**
     * Date period
     */
  CStringArray m_passes;

private:
  

};


//-------------------------------------------------------------
//------------------- CCriteriaPassInt class --------------------
//-------------------------------------------------------------


/** \addtogroup product Pass number Criteria
  @{ */

/** 
  Pass number (from/to) Criteria management class.


 \version 1.0
*/

class CCriteriaPassInt : public CCriteriaPass
{

public:
    
  /// Empty CCriteriaPassInt ctor
  CCriteriaPassInt();

  CCriteriaPassInt(CCriteriaPassInt& c);

  CCriteriaPassInt(CCriteriaPassInt* c);

   /**
     * Constructor.
     * 
     * @param from start pass
     * @param to end pass
    */
  CCriteriaPassInt(int32_t from, int32_t to); 
  
   /**
     * Constructor.
     * 
     * @param from start pass
     * @param to end pass
    */
  CCriteriaPassInt(const string& from, const string& to);

  /**
     * Constructor from a array that contains start pass as string, end pass as string
     * 
     * @param array start and end dates
     */
  CCriteriaPassInt(const CStringArray& array);


  ///Destructor
  virtual ~CCriteriaPassInt();

  void Set(CCriteriaPassInt& c);

  int32_t GetFrom() { return  m_from; };
  int32_t GetTo() { return  m_to; };

   /**
     * Sets end pass
     * 
     * @param to end pass
    */
  void SetTo(int32_t to);

   /**
     * Sets end pass
     * 
     * @param to end pass
    */
  void SetTo(const string& to);
  
   /**
     * Sets start pass
     * 
     * @param to start pass
    */
  void SetFrom(int32_t from);
   /**
     * Sets start pass
     * 
     * @param to start pass
    */
  void SetFrom(const string& from);

   /**
     * Sets date period from start and end pass
     * 
     * @param from start pass
     * @param to end pass
    */
  void Set(int32_t from, int32_t to);
   /**
     * Sets date period from start and end pass
     * 
     * @param from start pass
     * @param to end pass
    */
  void Set(const string& from, const string& to);

  void SetFromText(const string& values, const string& delimiter = CCriteriaPassInt::m_delimiter);


  /**
     * Sets a date period from a array that contains start pass as string, end pass as string
     * 
     * @param array start and end dates
    */
  void Set(const CStringArray& array);
  
  /**
   * Create the intersection of this date period with the given one
   * @param array that contains start pass as string, end pass as string
   * @param intersect intersection period
   * @return true, or false if there is no intersection
   */
  bool Intersect(CStringArray& array, CStringArray& intersect);
  /**
   * Create the intersection of this date period with the given one
   * @param array that contains start pass as string, end pass as string
   * @param intersect intersection period
   * @return true, or false if there is no intersection
   */
  bool Intersect(CStringArray& array, CIntArray& intersect);
  /**
   * Create the intersection of this date period with the given one
   * @param array that contains start pass as string, end pass as string
   * @param intersect intersection period
   * @return true, or false if there is no intersection
   */
  bool Intersect(CIntArray& array, CStringArray& intersect);
  /**
   * Create the intersection of this date period with the given one
   * @param array that contains start pass as string, end pass as string
   * @param intersect intersection period
   * @return true, or false if there is no intersection
   */
  bool Intersect(CIntArray& array, CIntArray& intersect);

  bool Intersect(int32_t from, int32_t to, CStringArray& intersect);
  bool Intersect(int32_t from, int32_t to, CIntArray& intersect);
  bool Intersect(double otherFrom, double otherTo, CIntArray& intersect);
  
  bool Intersect(const string& from, const string& to, CIntArray& intersect);

  bool Intersect(const string& from, const string& to, CStringArray& intersect);  /** Sets internal value to the default value (uninitialized)*/
  void SetDefaultValue();
  /**
     * Tests whether the pass have been initialized or not
     *
     * @return true if not initialized
     */
  bool IsDefaultValue();

  string GetAsText(const string& delimiter = CCriteriaPassInt::m_delimiter);

  const CCriteriaPassInt& operator=(CCriteriaPassInt& c);

  static CCriteriaPassInt* GetCriteria(CBratObject* ob, bool withExcept = true);

  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr);

protected:

  void Init();

  void Adjust();

public:
  static const string m_delimiter;    

protected:


  /**
     * start pass
     */
  int32_t m_from;
  /**
     * end pass
     */
  int32_t m_to;

private:
  

};
/** @} */

}

#endif // !defined(_CriteriaPass_h_)
