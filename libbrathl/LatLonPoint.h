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
#if !defined(_LatLonPoint_h_)
#define _LatLonPoint_h_

#include "new-gui/Common/tools/brathl_error.h" 
#include "brathl.h" 

#include <string>

#include "BratObject.h"
#include "Unit.h" 

using namespace brathl;

namespace brathl
{

//-------------------------------------------------------------
//------------------- CLatLonPoint class --------------------
//-------------------------------------------------------------


class CLatLonPoint : public CBratObject 
{

public: 

  /** Default constructor with values 0,0. */
  CLatLonPoint();
  
  /**
     * Copy Constructor.
     *
     * @param pt  point to copy
     */
  CLatLonPoint(CLatLonPoint &pt);
  /**
    * Creates a LatLonPoint from component latitude and longitude values.
    * The longitude is adjusted to be in the range [-180.,180.].
    *
    * @param lat north latitude in degrees
    * @param lon east longitude in degrees
    */

    CLatLonPoint(double lat,double lon);

    virtual ~CLatLonPoint();

/**
     * Test if point lies between two longitudes, deal with wrapping.
     * @param lon point to test
     * @param lonBeg beginning longitude
     * @param lonEnd ending longitude
     * @return true if lon is between lonBeg and lonEnd.
     */
  static bool BetweenLon(double lon,double lonBeg,double lonEnd);
  /**
     * put longitude into the range [-180, 180] deg
     *
     * @param lon  lon to normalize
     * @return longitude in range [-180, 180] deg
     */
  static double Range180(double lon);
  /**
     * put longitude into the range [0, 360] deg
     *
     * @param lon  lon to normalize
     * @return longitude into the range [0, 360] deg
     */
  static double LonNormal360(double lon);
  
  static double LonNormal360(double value, CUnit* unitIn);

  /**
     * put longitude into the range [center +/- 180] deg
     *
     * @param lon  lon to normalize
     * @param center  center point
     * @return longitude into the range [center +/- 180] deg
     */
  static double LonNormal(double lon,double center);
  static double LonNormal(double lon, double center, CUnit* unitIn);

  static double LonNormalFrom(double lon, double from);
  static double LonNormalFrom(double lon, double from, CUnit* unitIn);

  /**
     * Normalize the longitude to lie between +/-180
     * @param lon east latitude in degrees
     * @return normalized lon
     */
  static double LonNormal(double lon);
  /**
     * Normalize the latitude to lie between +/-90
     * @param lat north latitude in degrees
     * @return normalized lat
     */
  static double LatNormal(double lat);
  static double LatNormal(double lat, CUnit* unitIn);

    /**
     * Returns the longitude, in degrees.
     * @return the longitude, in degrees
     */
     double GetLongitude() { return m_lon; };/**

     * Returns the latitude, in degrees.

     * @return the latitude, in degrees

     */
  double GetLatitude()  { return m_lat; };
  /**
     * set lat, lon using values of pt
     *
     * @param pt  point to use
     */
  void Set(CLatLonPoint& pt);
  /**
     * set lat, lon using double values
     *
     * @param lat  lat value
     * @param lon  lon value
     */
  void Set(double lat,double lon);
  /**
     * set lat, lon using float values
     *
     * @param lat  lat value
     * @param lon  lon value
     */
  void Set(float lat,float lon);
  /**
     *  Set the longitude, in degrees. It is normalized to +/-180.
     *
     *  @param lon east longitude in degrees
     */
  void SetLongitude(double lon);
  /**
     * Set the latitude, in degrees. Must lie beween +/-90
     *
     * @param lat north latitude in degrees
     */
  void SetLatitude(double lat);
/**
     * Check for equality with another object.
     *
     * @param obj object to check
     * @return true if this represents the same point as pt
     */
  //bool Equals(CObject& obj);
  /**
     * Check for equality with another point.
     *
     * @param pt   point to check
     * @return true if this represents the same point as pt
     */
  bool Equals(CLatLonPoint &pt);
  /**
     * Check to see if the values are close enough.
     *
     * @param d1  first value
     * @param d2  second value
     *
     * @return true if they are pretty close
     */

  /** Sets internal value to the default value (uninitialized)*/
  void SetDefaultValue();

  /**
     * Tests whether Lat/Lon point have been initialized or not
     *
     * @return true if not initialized
     */
  bool IsDefaultValue();

  const CLatLonPoint& operator= (CLatLonPoint& pt);

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

public:

  static const char* m_DEFAULT_UNIT_LONGITUDE;
  static const char* m_DEFAULT_UNIT_LATITUDE;
  
  static const double m_LONGITUDE_COMPARE_EPSILON;


private: 
  bool CloseEnough(double d1,double d2);

protected :
  /** East latitude in degrees, always +/- 90 */
  double m_lat;
/** North longitude in degrees, always +/- 180 */
  double m_lon;


};

} 
#endif // !defined(_LatLonPoint_h_)
