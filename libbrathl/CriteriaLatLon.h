/*
* Copyright (C) 2008 Collecte Localisation Satellites (CLS), France (31)
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
#if !defined(_CriteriaLatLon_h_)
#define _CriteriaLatLon_h_

#include "brathl_error.h" 
#include "brathl.h" 

#include "Stl.h"

#include "BratObject.h"
#include "LatLonRect.h"
#include "List.h"
#include "Field.h"
#include "Criteria.h"
using namespace brathl;

namespace brathl
{




//-------------------------------------------------------------
//------------------- CCriteriaLatLon class --------------------
//-------------------------------------------------------------


/** \addtogroup product Latitude/Longitude Criteria
  @{ */

/** 
  Latitude/Longitude Criteria management class.


 \version 1.0
*/





class CCriteriaLatLon : public CCriteria
{

public:
    
  /// Empty CCriteriaLatLon ctor
  CCriteriaLatLon();

  CCriteriaLatLon(CCriteriaLatLon& c);
  
  CCriteriaLatLon(CCriteriaLatLon* c);

   /**
     * Constructor.
     * 
     * @param latLonRect lat/lon bounding box
    */
  CCriteriaLatLon(CLatLonRect& latLonRect);
  /**
   * Construct a lat/lon bounding box from a point, and a delta lat, lon.
   * This disambiguates which way the box wraps around the globe.
   *
   * @param p1       one corner of the box
   * @param deltaLat delta lat from p1. (may be positive or negetive)
   * @param deltaLon delta lon from p1. (may be positive or negetive)
   */
  CCriteriaLatLon(CLatLonPoint& p1, double deltaLat, double deltaLon);

  /**
     * Constructor.
     * 
     * @param latLonLow lat/lon low point
     * @param latLonHigh lat/lon high point
     */
  CCriteriaLatLon(CLatLonPoint& latLonLow, CLatLonPoint& latLonHigh);
  /**
     * Constructor.
     * 
     * @param latLow latitude low
     * @param lonLow longitude low
     * @param latHigh latitude high
     * @param lonHigh longitude high
     */
  CCriteriaLatLon(double latLow, double lonLow, double latHigh, double lonHigh);
  /**
     * Constructor.
     * 
     * @param latLow latitude low
     * @param lonLow longitude low
     * @param latHigh latitude high
     * @param lonHigh longitude high
     */
  CCriteriaLatLon(const string& latLow, const string& lonLow, const string& latHigh, const string& lonHigh);
  /**
     * Constructor from a list that contains low latitude value, low longitude value, high latitude value,
     * high longitude value.
     * 
     * @param array to be converted
     */
  CCriteriaLatLon(const CStringArray& array);
  /**
     * Getter of the property t&amp;lt;tl&amp;gt;atLonRect/&amp;lt;tt.&amp;gt;
     * 
     * @return Returns the latLonRect.
     */

  /// Destructor
  virtual ~CCriteriaLatLon();

  CLatLonRect* GetLatLonRect() { return  &m_latLonRect; };

  /**
     * Setter of the property t&amp;lt;tl&amp;gt;atLonRect/&amp;lt;tt.&amp;gt;
     * 
     * @param latLonRect The latLonRect to set.
     */
  void Set(CLatLonRect& latLonRect);
  /**
   * Set a lat/lon bounding box from a point, and a delta lat, lon.
   * This disambiguates which way the box wraps around the globe.
   *
   * @param p1       one corner of the box
   * @param deltaLat delta lat from p1. (may be positive or negetive)
   * @param deltaLon delta lon from p1. (may be positive or negetive)
   */
  void Set(CLatLonPoint& p1, double deltaLat, double deltaLon);
  /**
     * Setter of the property t&amp;lt;tl&amp;gt;atLonRect/&amp;lt;tt.&amp;gt;
     * 
     * @param latLonLow lat/lon low point
     * @param latLonHigh lat/lon high point
     * @uml.property name="latLonRect"
     */
  void Set(CLatLonPoint& latLonLow, CLatLonPoint& latLonHigh);
  /**
     * Setter of the property t&amp;lt;tl&amp;gt;atLonRect/&amp;lt;tt.&amp;gt;
     * 
     * @param latLow latitude low
     * @param lonLow longitude low
     * @param latHigh latitude high
     * @param lonHigh longitude high
     */
  void Set(double latLow, double lonLow, double latHigh, double lonHigh);
  /**
     * Setter of the property t&amp;lt;tl&amp;gt;atLonRect/&amp;lt;tt.&amp;gt;
     * 
     * @param latLow latitude low
     * @param lonLow longitude low
     * @param latHigh latitude high
     * @param lonHigh longitude high
     */
  void Set(const string& latLow, const string& lonLow, const string& latHigh, const string& lonHigh);
  /**
     * Setter of the property t&amp;lt;tl&amp;gt;atLonRect/&amp;lt;tt.&amp;gt;
     * 
     * @param latLonRect latitude low, longitude low, latitude high, longitude high 
     */
  void Set(const string& latLonRect, const string& delimiter = CLatLonRect::m_delimiter);

  void Set(CCriteriaLatLon& c);

  /**
     * @return lower left latitude of the lat/lon box, Double.MAX_VALUE if not set.
     */
  double GetLowerLeftLat() {return m_latLonRect.GetLowerLeftPoint().GetLatitude(); };
  /**
     * @return upper left latitude of the lat/lon box, Double.MAX_VALUE if not set.
     */
  double GetUpperLeftLat() {return m_latLonRect.GetUpperLeftPoint().GetLatitude(); };
  /**
     * @return lower right latitude of the lat/lon box, Double.MAX_VALUE if not set.
     */
  double GetLowerRightLat() {return m_latLonRect.GetLowerRightPoint().GetLatitude(); };
  /**
     * @return upper right latitude of the lat/lon box, Double.MAX_VALUE if not set.
     */
  double GetUpperRightLat() {return m_latLonRect.GetUpperRightPoint().GetLatitude(); };
  /**
     * @return lower left longitude of the lat/lon box, Double.MAX_VALUE if not set.
     */
  double GetLowerLeftLon() {return m_latLonRect.GetLowerLeftPoint().GetLongitude(); };
  /**
     * @return upper left longitude of the lat/lon box, Double.MAX_VALUE if not set.
     */
  double GetUpperLeftLon() {return m_latLonRect.GetUpperLeftPoint().GetLongitude(); };
  /**
     * @return lower right longitude of the lat/lon box, Double.MAX_VALUE if not set.
     */
  double GetLowerRightLon() {return m_latLonRect.GetLowerRightPoint().GetLongitude(); };
  /**
     * @return upper right longitude of the lat/lon box, Double.MAX_VALUE if not set.
     */
  double GetUpperRightLon() {return m_latLonRect.GetUpperRightPoint().GetLongitude(); };
  /**
     * Gets the min. or max. of two longitudes.
     * 
     * @param lon1 first longitude
     * @param lon2 second longitude
     * @param wantMin true: returns min., false: returns max.
     * @return min. lon or max. lon, depends on wantMin.
     */
  static double GetMinOrMaxLon(double lon1, double lon2, bool wantMin);

  /**
   * Create the intersection of this LatLon Criteria with the given one
   * @param clip intersect with this
   * @param intersection
   * @return true, or false if there is no intersection
   */
  bool Intersect(CLatLonRect& clip, CLatLonRect& intersect);

    /** Sets internal value to the default value (uninitialized)*/
  void SetDefaultValue();

    /**
     * Tests whether date period have been initialized or not
     *
     * @return true if not initialized
     */
  bool IsDefaultValue();

  virtual string GetAsText(const string& delimiter = CLatLonRect::m_delimiter);

  const CCriteriaLatLon& operator=(CCriteriaLatLon& c);

  static CCriteriaLatLon* GetCriteria(CBratObject* ob, bool withExcept = true);


  ///Dump fonction
  virtual void Dump(ostream& fOut = cerr);

protected:

  void Init();

public:
    

protected:


  /**
     * Bounding box for latitude/longitude points. This is a rectangle in lat/lon coordinates. Note that
     * LatLonPoint always has lon in the range +/-180. *
     * 
     */
  CLatLonRect m_latLonRect;

private:
  

};

/** @} */

}

#endif // !defined(_CriteriaLatLon_h_)
