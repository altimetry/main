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
#if !defined(_LatLonRect_h_)
#define _LatLonRect_h_

#include <string>

#include "common/tools/brathl_error.h"
#include "brathl.h" 

#include "BratObject.h"
#include "List.h"
#include "LatLonPoint.h"

using namespace brathl;

namespace brathl
{


const std::string FORMAT_FLOAT_LATLON = "%-#.5g";

//-------------------------------------------------------------
//------------------- CLatLonRect class --------------------
//-------------------------------------------------------------


class CLatLonRect : public CBratObject 
{
public: 

  /**
   * Create a LatLonRect that covers the whole world.
   */
  CLatLonRect();


  /**
   * Construct a lat/lon bounding box from a point, and a delta lat, lon.
   * This disambiguates which way the box wraps around the globe.
   *
   * @param p1       one corner of the box
   * @param deltaLat delta lat from p1. (may be positive or negetive)
   * @param deltaLon delta lon from p1. (may be positive or negetive)
   */
  CLatLonRect(CLatLonPoint& p1, double deltaLat, double deltaLon);

  /**
   * Construct a lat/lon bounding box from two points.
   * The order of longitude coord of the two points matters:
   * pt1.lon is always the "left" point, then points contained within the box
   * increase (unless crossing the Dateline, in which case they jump to -180, but
   * then start increasing again) until pt2.lon
   * The order of lat doesnt matter: smaller will go to "lower" point (further south)
   *
   * @param left  left corner
   * @param right right corner
   */

  CLatLonRect(CLatLonPoint& left, CLatLonPoint& right);

  /**
   * Copy Constructor
   *
   * @param r rectangle to copy
   */
  CLatLonRect(CLatLonRect& r);

  /**
   * Copy Constructor
   *
   * @param r rectangle to copy
   */
  CLatLonRect(CLatLonRect* r);

   /**
     * Create a Lat/Lon rectangle
     * 
     * @param latLonRect latitude low
     * @param latLow latitude low
     * @param lonLow longitude low
     * @param latHigh latitude high
     * @param lonHigh longitude high
     */
  CLatLonRect(double latLow, double lonLow, double latHigh, double lonHigh);

   /**
     * Create a Lat/Lon rectangle
     * 
     * @param latLonRect latitude low
     * @param latLow latitude low
     * @param lonLow longitude low
     * @param latHigh latitude high
     * @param lonHigh longitude high
     */
  CLatLonRect(const std::string& latLow, const std::string& lonLow, const std::string& latHigh, const std::string& lonHigh);


  virtual ~CLatLonRect();


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
   * Set a lat/lon bounding box from two points.
   * The order of longitude coord of the two points matters:
   * pt1.lon is always the "left" point, then points contained within the box
   * increase (unless crossing the Dateline, in which case they jump to -180, but
   * then start increasing again) until pt2.lon
   * The order of lat doesnt matter: smaller will go to "lower" point (further south)
   *
   * @param left  left corner
   * @param right right corner
   */

  void Set(CLatLonPoint& left, CLatLonPoint& right);
  /**
   * Set a lat/lon bounding box from another lat/lon bounding 
   *
   * @param r rectangle to copy
   */
  void Set(CLatLonRect& r);

   /**
     * Set the Lat/Lon rectangle
     * 
     * @param latLonRect latitude low
     * @param latLow latitude low
     * @param lonLow longitude low
     * @param latHigh latitude high
     * @param lonHigh longitude high
     */
  void Set(double latLow = CLatLonRect::LATITUDE_MIN, double lonLow = CLatLonRect::LONGITUDE_MIN,
                double latHigh = CLatLonRect::LATITUDE_MAX, double lonHigh = CLatLonRect::LONGITUDE_MAX);


   /**
     * Set the Lat/Lon rectangle
     * 
     * @param latLonRect latitude low
     * @param latLow latitude low
     * @param lonLow longitude low
     * @param latHigh latitude high
     * @param lonHigh longitude high
     */
  void Set(const std::string& latLow, const std::string& lonLow, const std::string& latHigh, const std::string& lonHigh);


  void Set(const std::string& latLonRect, const std::string& delimiter = CLatLonRect::m_delimiter);

  void Set(const CStringArray& array);



  /**
   * Get the upper right corner of the bounding box.
   *
   * @return upper right corner of the bounding box
   */
  CLatLonPoint& GetUpperRightPoint() { return  m_upperRight; };

  /**
   * Get the lower left corner of the bounding box.
   *
   * @return lower left corner of the bounding box
   */
  CLatLonPoint& GetLowerLeftPoint() { return  m_lowerLeft; };
  
  /**
   * Get the upper left corner of the bounding box.
   *
   * @return upper left corner of the bounding box
   */
  CLatLonPoint GetUpperLeftPoint();

  /**
   * Get the lower left corner of the bounding box.
   *
   * @return lower left corner of the bounding box
   */
  CLatLonPoint GetLowerRightPoint();
;
  /**
   * Get whether the bounding box crosses the +/- 180 seam
   *
   * @return true if the bounding box crosses the +/- 180 seam
   */
  bool CrossDateline() { return  m_crossDateline; };
  /**
   * get whether two bounding boxes are equal in values
   *
   * @param other other bounding box
   * @return true if this represents the same bounding box as other
   */
  bool Equals(CLatLonRect& other) { return m_lowerLeft.Equals(other.GetLowerLeftPoint()) && m_upperRight.Equals(other.GetUpperRightPoint()); };

  /**
   * return width of bounding box, always between 0 and 360 degrees.
   *
   * @return width of bounding box in degrees longitude
   */
  double GetWidth() { return  m_width; };

  /**
   * return center Longitude, always in the range +/-180
   *
   * @return center Longitude
   */
  double GetCenterLon() { return  m_lon0; };
  /**
   * Get minimum longitude, aka "west" edge
   * @return minimum longitude
   */
  double GetLonMin() { return m_lowerLeft.GetLongitude(); };
  /**
   * Get maximum longitude, aka "east" edge
   * @return maximum longitude
   */
  double GetLonMax() { return m_lowerLeft.GetLongitude() + m_width; };
  /**
   * Get minimum latitude, aka "south" edge
   * @return minimum latitude
   */
  double GetLatMin() { return m_lowerLeft.GetLatitude(); };

  /**
   * Get maximum latitude, aka "west" edge
   * @return maximum latitude
   */
  double GetLatMax() { return m_upperRight.GetLatitude(); };
  /**
   * Determine if a specified LatLonPoint is contained in this bounding box.
   *
   * @param p the specified point to be tested
   * @return true if point is contained in this bounding box
   */
  bool Contains(CLatLonPoint& p) { return Contains(p.GetLatitude(),p.GetLongitude()); };
  /**
   * Determine if the given lat/lon point is contined inside this rectangle.
   *
   * @param lat lat of point
   * @param lon lon of point
   * @return true if the given lat/lon point is contined inside this rectangle
   */
  bool Contains(double lat, double lon);

  /**
   * Determine if this bounding box is contained in another LatLonRect.
   *
   * @param b the other box to see if it contains this one
   * @return true if b contained in this bounding box
   */
  bool ContainedIn(CLatLonRect& b) { return (b.GetWidth() >= m_width) && b.Contains( m_upperRight); };
  
  /**
   * Extend the bounding box to contain this point
   *
   * @param p point to include
   */
  void Extend(CLatLonPoint& p);
  /**
   * Extend the bounding box to contain the given rectanle
   *
   * @param r rectangle to include
   */
  void Extend(CLatLonRect& r);
  /**
   * Create the instersection of this LatLon with the given one
   * @param clip intersect with this
   * @param intersect intersection rectangle
   * @return true, or false if there is no intersection
   */
  bool Intersect(CLatLonRect& clip, CLatLonRect& intersect);

  /** Sets internal value to the default value (uninitialized)*/
  void SetDefaultValue();

  /**
     * Tests whether Lat/Lon rectangle have been initialized or not
     *
     * @return true if not initialized
     */
  bool IsDefaultValue();

  void GetAsString(CStringArray& array) const;

  std::string GetAsText(const std::string& delimiter = CLatLonRect::m_delimiter) const;

  std::string AsString(const std::string& format = "");

  void Reset();

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);

  const CLatLonRect& operator= (CLatLonRect& f);

public:

  /**
   * Latitude min.
   */
  static const std::string LATITUDE_MIN_STR;
  /**
   * Latitude max.
   */
  static const std::string LATITUDE_MAX_STR;
  /**
   * Longitude min.
   */
  static const std::string LONGITUDE_MIN_STR;
  /**
   * Longitude max.
   */
  static const std::string LONGITUDE_MAX_STR;
  
  /**
   * Latitude min.
   */
  static const double LATITUDE_MIN;
  /**
   * Latitude max.
   */
  static const double LATITUDE_MAX;
  /**
   * Longitude min.
   */
  static const double LONGITUDE_MIN;
  /**
   * Longitude max.
   */
  static const double LONGITUDE_MAX;

  static const std::string m_delimiter;



private: 
  void Init();
  //void DeleteLatLonPoint();
  bool Intersect(double min1,double max1,double min2,double max2);

protected:
/**
   * upper right corner
   */
  CLatLonPoint m_upperRight;
  /**
   * lower left corner
   */
  CLatLonPoint m_lowerLeft;
/**
   * flag for dateline cross
   */
  bool m_crossDateline;
  /**
   * All longitudes are included
   */
  bool m_allLongitude;
  /**
   * width and initial longitude
   */
  double m_width;
  double m_lon0;

};


} 
#endif // !defined(_LatLonRect_h_)
