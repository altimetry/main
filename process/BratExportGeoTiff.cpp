/*
* Copyright (C) 2008 Science & Technology (S&T), Netherlands
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
#include <cstdio>
#include "netcdf.h"
#include <cmath>
#include <cfloat>
#include <assert.h>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iomanip>

#include "libxtiff/xtiffio.h"
#include "geotiffio.h"

#include <string>
#include "List.h"
#include "new-gui/Common/tools/Exception.h"
#include "new-gui/Common/ConsoleApplicationPaths.h"
#include "FileParams.h"

#include "ProcessCommonTools.h"

using namespace brathl;
using namespace processes;

template <typename T>
class FlagAndValue
{
private:
  T m_value;
  bool m_valid;
  
public:
  FlagAndValue() : m_valid(false) {}
  FlagAndValue(T v) : m_value(v), m_valid(true) {}

  bool isValid(void) const { return m_valid; }
  T value(void) const { return m_value; }
  void setValue(const T &v) { m_value = v; m_valid = true; }
};

struct CoordBoundaries
{
  double latNorth;
  double latSouth;
  double lonWest;
  double lonEast;
};

struct KmlTrackPoint
{
	double lat;
	double lon;
	double alt;
	double data;
	unsigned char rgba[4];
	double timestamp;
};

//--------------------------------------------------
// Colour Table construction
//--------------------------------------------------

struct CTVertex
{
  // all values are in the range [0,1]
  double x;
  double r, g, b, a;

  CTVertex(double X, double R, double G, double B, double A) : x(X), r(R), g(G), b(B), a(A) {}
  bool operator<(const CTVertex &rhs) const { return (x < rhs.x); }
};

class ColourTable
{
public:
  ColourTable(const std::string &tableName);
  ~ColourTable();

  void getRGBA(double x, unsigned char rgba[4]) const;

private:
  void prepareAerosol(void);
  void prepareOzone(void);
  void prepareBlackbody(void);
  void prepareRainbow(void);
  void prepareBlackToWhite(void);
  void prepareWhiteToBlack(void);
  void prepareGreenToRed(void);
  void prepareRedToGreen(void);
  void prepareCloud(void);
  void prepareColdToHot(void);

private:
  std::list<CTVertex> m_vertexes;
};


//--------------------------------------------------
// Help
//--------------------------------------------------

static const KeywordHelp KeywordList[]	= {
	KeywordHelp(kwFILE, 1, 1),
	KeywordHelp(kwOUTPUT, 1, 1),
	KeywordHelp(kwOUTPUT_KML, 0, 1),
	KeywordHelp(kwDISPLAY_MINVALUE, 0, 1),
	KeywordHelp(kwDISPLAY_MAXVALUE, 0, 1),
	KeywordHelp(kwDISPLAY_COLORTABLE, 0, 1),
	KeywordHelp(kwDISPLAY_LOGO_URL, 0, 1),
	KeywordHelp(""),
};

inline bool isNaN(double x)
{
  union {
    double d;
    unsigned int ui[2];
  } tmp;
    
  tmp.d = x;
    
#ifdef WORDS_BIGENDIAN
  return ((tmp.ui[0] & 0x7ff00000U) == 0x7ff00000U) &&
    ((tmp.ui[0] & 0xfffffU) != 0 || tmp.ui[1] != 0);
#else
  return ((tmp.ui[1] & 0x7ff00000U) == 0x7ff00000U) &&
    ((tmp.ui[1] & 0xfffffU) != 0 || tmp.ui[0] != 0);
#endif
}

static bool GetParameters(const std::string &paramFile,
			  std::string &inputFileName, std::string &outputFileName, std::string &kmlFileName,
			  std::string &colourTableName, std::string &screeLogoUrl,
			  FlagAndValue<double> &minValue, FlagAndValue<double> &maxValue,
			  std::string &filetypeName, std::string &productList);
static bool ExportNetCdfAsGeoTiff(const std::string &inputFileName, const std::string &outputFileName,
				  const std::string &colourTableName,
				  const FlagAndValue<double> &minValue,
				  const FlagAndValue<double> &maxValue,
				  bool closeInLongitudeIfGlobal,
				  struct CoordBoundaries *coordBoundaries);
static int ExtractLatLonCoordinateVariableIds(int ncId, int nVariables, int nDimensions,
					      int &latVarId, int &latDimId, int &lonVarId, int &lonDimId) ;
static int ExtractGridCoordinateVariableIds(int ncId, int nVariables, int nDimensions,
					    int &latVarId, int &latBndVarId, int &latDimId, size_t &nLat,
					    int &lonVarId, int &lonBndVarId, int &lonDimId, size_t &nLon);
static int ExtractCrsVariable(int ncId, int *crsVarId); // helper
static int ExtractTimestampVariable(int ncId, int *timestampVarId);
static void ExtractMinAndMax(int ncId, int dataVarId, int nRow, int nCol, double &minValue, double &maxValue);
static bool WriteGeoTiff(const std::string &filename, int ncId, int dataVarId, bool swap_axis,
					 int nLat, int nLon, double minValue, double maxValue,
					 const ColourTable *colourTable, bool areaGrid,
					 double latNorth, double lonWest, double dLat, double dLon,
					 bool repeatFirstColumn, std::vector<double>& latAxis,
					 std::vector<double>& lonAxis, int timestampVarId);
static bool ExportKml(const std::string &kmlFileName,
		const std::string &tiffFileName, const std::string &screeLogoUrl,
		struct CoordBoundaries *coordBoundaries,
		const std::string &filetypeName, const std::string &productList);

static std::vector<KmlTrackPoint> kmlTrackVector;

static char dataUnits[128];

static char dataName[128];

static bool isAltData;

static std::vector<double> barLabelValues;

static unsigned int numberOfBarLabelValues = 9;

static std::string colourBarFilename;

static bool includeTimestamp;

static char timestampUnits[128];

int main( int argc, char *argv[] )
{
	std::string commandFile;
	FlagAndValue<double> minValue;
	FlagAndValue<double> maxValue;

	const CConsoleApplicationPaths brat_paths( argv[ 0 ] );

	CTools::SetInternalDataDir( brat_paths.mInternalDataDir );

	if ( CheckCommandLineOptions( argc, argv,
		"This program exports F(X,Y) data to a GeoTIFF file.",
		KeywordList,
		commandFile ) )
		return 2;

	try
	{
		std::string inputFileName, outputFileName, kmlFileName, colourTableName,
			screenLogoUrl, filetypeName, productList;

		if ( !GetParameters( commandFile, inputFileName, outputFileName, kmlFileName,
			colourTableName, screenLogoUrl, minValue, maxValue,
			filetypeName, productList ) ) {
			std::cerr << "Parameter File does not contain the required fields." << std::endl;
			return 2;
		}

		if ( kmlFileName.empty() ) {
			// straight TIFF export
			if ( !ExportNetCdfAsGeoTiff( inputFileName, outputFileName, colourTableName, minValue, maxValue, false, NULL ) ) {
				std::cerr << "Failed to export GeoTIFF file." << std::endl;
				return 2;
			}
		}
		else {
			// tiff and KML export
			struct CoordBoundaries cBounds;

			kmlTrackVector.clear();

			if ( !ExportNetCdfAsGeoTiff( inputFileName, outputFileName, colourTableName, minValue, maxValue, true, &cBounds ) ) {
				std::cerr << "Failed to export GeoTIFF file." << std::endl;
				return 2;
			}

			if ( !ExportKml( kmlFileName, outputFileName, screenLogoUrl, &cBounds,
				filetypeName, productList ) ) {
				std::cerr << "Failed to export KML file." << std::endl;
				return 2;
			}
		}

		return 0;
	}
	catch ( CException &e )
	{
		std::cerr << "BRAT ERROR: " << e.what() << std::endl;
		return 1;
	}
	catch ( std::exception &e )
	{
		std::cerr << "BRAT RUNTIME ERROR: " << e.what() << std::endl;
		return 254;
	}
	catch ( ... )
	{
		std::cerr << "BRAT FATAL ERROR: Unexpected error" << std::endl;
		return 255;
	}
}


//------------------------------------------------------------
// Read the parameters file and extract the required information.
// Returns true if the parmeteres were read successfully.

bool GetParameters(const std::string &paramFile,
		   std::string &inputFileName, std::string &outputFileName, std::string &kmlFileName,
		   std::string &colourTableName, std::string &screenLogoUrl,
		   FlagAndValue<double> &minValue, FlagAndValue<double> &maxValue,
		   std::string &filetypeName, std::string &productList)
{
  bool status = true;
  CFileParams params(paramFile);
  double tmpValue;

  params.SetVerboseLevel();
  params.Dump();

  CParameter *par;

  if ((par = params.m_mapParam.Exists(kwOUTPUT)) != NULL)
    par->GetValue(outputFileName);
  else
    status = false;

  if ((par = params.m_mapParam.Exists(kwFILE)) != NULL)
    par->GetValue(inputFileName);
  else
    status = false;

  // Leo: Visual C++ 6.0 does not have std::string::clear()
  //      using std::string::erase() instead.

  if ((par = params.m_mapParam.Exists(kwOUTPUT_KML)) != NULL)
    par->GetValue(kmlFileName);
  else
    kmlFileName.erase(kmlFileName.begin(), kmlFileName.end());
  
  if ((par = params.m_mapParam.Exists(kwDISPLAY_COLORTABLE)) != NULL)
    par->GetValue(colourTableName);
  else
    colourTableName.erase(colourTableName.begin(), colourTableName.end());

  if ((par = params.m_mapParam.Exists(kwDISPLAY_LOGO_URL)) != NULL)
    par->GetValue(screenLogoUrl);
  else
    screenLogoUrl.erase(screenLogoUrl.begin(), screenLogoUrl.end());

  // min and max values ???
  if ((par = params.m_mapParam.Exists(kwDISPLAY_MINVALUE)) != NULL) {
    par->GetValue(tmpValue);
    minValue.setValue(tmpValue);
  }

  if ((par = params.m_mapParam.Exists(kwDISPLAY_MAXVALUE)) != NULL) {
    par->GetValue(tmpValue);
    maxValue.setValue(tmpValue);
  }

  if ((par = params.m_mapParam.Exists(kwFILETYPE)) != NULL)
    par->GetValue(filetypeName);
  else
    filetypeName = "No Filetype";

  if ((par = params.m_mapParam.Exists(kwPRODUCT_LIST)) != NULL)
    par->GetValue(productList);
  else
	  productList = "";

  return status;
}

//------------------------------------------------------------
// Read the input NetCDF file and export as GeoTIFF. This expects
// a lat x lon grid of data (with uniform spacing), and expects the
// CF-1.0 convention to be applicable to the netCDF file.
 
bool ExportNetCdfAsGeoTiff(const std::string &inputFileName, const std::string &outputFileName, const std::string &colourTableName,
			   const FlagAndValue<double> &minValue, const FlagAndValue<double> &maxValue,
			   bool closeInLongitudeIfGlobal,
			   struct CoordBoundaries *coordBoundaries)
{
  const int cMaxDimReasonable = 16; // we DO NOT expect to have more than this number of dimensions

  // this is long flat and procedural code. It is broken into small blocks that are entered
  // only if the status is OK (NC_NOERR). The process checks the structure of the netCDF file
  // and collects the coordinate data, then creates the TIFF file, sets the TIFF TAGS and
  // fills the raster data as COLORMAPPED data with an 8-bit colour map.

  int varId;
  int nDimensions, nVariables, nDims, nAtts;
  int latDimId = -1, lonDimId = -1;
  int latVarId = -1, lonVarId = -1;
  int latBndVarId = -1;
  int lonBndVarId = -1;
  int crsVarId = -1;
  int timestampVarId = -1;
  int dataVarId = -1;
  int dimArray[cMaxDimReasonable];
  size_t nLat, nLon;
  nc_type xType;
  char name[NC_MAX_NAME+1];
  bool areaGrid = false;
  bool globalLongitude = false;
  bool swap_axis = false;

  // for GeoTIFF referencing
  double latNorth = 90.0;
  double lonWest = -180.0;
  double dLat = 1.0;
  double dLon = 1.0;

  std::vector<double> latAxis;
  std::vector<double> lonAxis;

  int ncId = 0;
  int status = nc_open(inputFileName.c_str(), NC_NOWRITE, &ncId);
  
  if (status != NC_NOERR) {
    std::cerr << "ERROR: could not open input netcdf file" << std::endl;
  }

  // expect at least two dimensions (ideally with names lat and lon)
  if (status == NC_NOERR) {
    status = nc_inq_ndims(ncId, &nDimensions);
    if (status == NC_NOERR) {
      status = (nDimensions >= 2 && nDimensions <= cMaxDimReasonable) ? NC_NOERR : 1;
      if (status != NC_NOERR)
        std::cerr << "ERROR: input netcdf file should have at least 2 named dimensions" << std::endl;
    }
  }
  // expect at least 3 variables
  if (status == NC_NOERR) {
    status = nc_inq_nvars(ncId, &nVariables);
    if (status == NC_NOERR) {
      status = (nVariables >= 3) ? NC_NOERR : 1;
      if (status != NC_NOERR)
          std::cerr << "ERROR: input netcdf file should have at least 3 variables" << std::endl;
    }
  }

  // expect a lat and lon (coordinate) variable.
  if (status == NC_NOERR) {
    status = ExtractGridCoordinateVariableIds(ncId, nVariables, nDimensions,
					      latVarId, latBndVarId, latDimId, nLat,
					      lonVarId, lonBndVarId, lonDimId, nLon);
    if (status != NC_NOERR)
      std::cerr << "ERROR: could not find lat/lon axis variables in input netcdf file" << std::endl;
  }

  // expect a crs variable, but not essential
  if (status == NC_NOERR)
    status = ExtractCrsVariable(ncId, &crsVarId);

  // expect a Timestamp variable, but not essential
  if (status == NC_NOERR)
    status = ExtractTimestampVariable(ncId, &timestampVarId);

  // the first other variable SHOULD be the grid function data ... but check
  if (status == NC_NOERR) {
    // find the first NON lat/lon/crs variable that has 2 dimensions that are
    // lat and lon (in that order) => row major
    
    varId = 0;
    while (status == NC_NOERR && varId < nVariables && dataVarId == -1) {
      
      if (varId != latVarId && varId != lonVarId
    		  && varId != crsVarId && varId != timestampVarId
    		  && varId != latBndVarId && varId != lonBndVarId) {

        status = nc_inq_var(ncId, varId, name, &xType, &nDims, dimArray, &nAtts);
        if ((status == NC_NOERR) && (nDims == 2)) {
          if ((dimArray[0] == latDimId) && (dimArray[1] == lonDimId)) {
            dataVarId = varId;
          }
          else if ((dimArray[0] == lonDimId) && (dimArray[1] == latDimId)) {
            dataVarId = varId;
            swap_axis = true;
          }
        }
      }
      
      ++varId;
    }

    // was a  data variable located
    if (status == NC_NOERR && dataVarId == -1) {
      status = 1;
      std::cerr << "ERROR: could not find plottable variable in netcdf file" << std::endl;
    }
  }

  // OK to extract the grid (axes) data ...
  if (status == NC_NOERR) {

    std::vector<double>::iterator it;

    latAxis.resize(nLat);
    lonAxis.resize(nLon);

    // ignore the bounds even if this is an area grid, since it is required that
    // the grid is uniform.
    // get grid axis data directly from the lat and lon variables - ther are in degrees.
    if (status == NC_NOERR)
      status = nc_get_var_double(ncId, latVarId, &latAxis.front());
    if (status == NC_NOERR)
      status = nc_get_var_double(ncId, lonVarId, &lonAxis.front());

    // check that both axes are 'uniform'.
    double tmpValue, tolerance;
    bool toleranceError = false;

    dLat = (latAxis.back() - latAxis.front()) / (double)(nLat - 1);
    dLon = (lonAxis.back() - lonAxis.front()) / (double)(nLon - 1);
    
    // allow a tolerance of 1% of the delta.
    tolerance = 0.01 * dLat;
    tmpValue = latAxis.front();
    it = latAxis.begin();
    while (it != latAxis.end()) {
      if (fabs(*it - tmpValue) > tolerance) {
        toleranceError = true;
        std::cerr << "ERROR: latitude grid is not equaly spaced" << std::endl;
        break;
      }
      tmpValue += dLat;
      ++it;
    }
    tolerance = 0.01 * dLon;
    tmpValue = lonAxis.front();
    it = lonAxis.begin();
    while (it != lonAxis.end()) {
      if (fabs(*it - tmpValue) > tolerance) {
          toleranceError = true;
          std::cerr << "ERROR: longitude grid is not equaly spaced" << std::endl;
          break;
      }
      tmpValue += dLon;
      ++it;
    }
    
    if (toleranceError) {
      status = 1;
    }
    else {
      // populated and checked the latitude/longitude data - only need a point and scale
      latNorth = latAxis.back();
      lonWest = lonAxis.front();
      // is the grid global??
      globalLongitude = (fabs(lonAxis.back() - lonAxis.front() + dLon - 360.0) < tolerance);

      // set the coordBoundaries if required. These are the geographic boundaries of the image.
      if (coordBoundaries != NULL) {
        coordBoundaries->latNorth = latNorth;
        coordBoundaries->latSouth = latAxis.front();
        coordBoundaries->lonWest = lonWest;
        if (globalLongitude && closeInLongitudeIfGlobal)
          coordBoundaries->lonEast = lonWest + 360.0; // precisely 360
        else
          coordBoundaries->lonEast = lonAxis.back();
      }
    }
  }

  // create the TIFF file
  if (status == NC_NOERR) {
    
    double rangeMin, rangeMax;
      
    if (minValue.isValid() && maxValue.isValid()) {
      rangeMin = minValue.value();
      rangeMax = maxValue.value();
    }
    else {
      // determine the min and max from the file itself...
      if (swap_axis)
        ExtractMinAndMax(ncId, dataVarId, nLon, nLat, rangeMin, rangeMax);
      else
        ExtractMinAndMax(ncId, dataVarId, nLat, nLon, rangeMin, rangeMax);
      
      if (rangeMin > rangeMax) {
        status = 1;
      }
      else {
        // restore the input value if it was valid.
        if (minValue.isValid())
          rangeMin = minValue.value();
        if (maxValue.isValid())
          rangeMax = maxValue.value();
      }
    }

    // check that the min and max are well ordered
    if (status == NC_NOERR) {
      status = (rangeMin <= rangeMax) ? NC_NOERR : 1;
    }
      
    if (status == NC_NOERR) {
      ColourTable colourTable(colourTableName);

      if (! WriteGeoTiff(outputFileName, ncId, dataVarId, swap_axis, nLat, nLon,
				       rangeMin, rangeMax, &colourTable,
				       areaGrid, latNorth, lonWest, dLat, dLon,
				       (globalLongitude && closeInLongitudeIfGlobal),
				       latAxis, lonAxis, timestampVarId)) {
        status = 1;
      }
    }
  }
  nc_close(ncId);
  
  return (status == NC_NOERR);
}


int ExtractLatLonCoordinateVariableIds(int ncId, int nVariables, int nDimensions,
				       int &latVarId, int &latDimId, int &lonVarId, int &lonDimId) 
{
  int status;
  int varId, nDims, nAtts;
  size_t length;
  char attrStr[32]; // big enough
  
  int *dimArray = new int[nDimensions];

  // initialize all output ints to -1 and the sizes to 0
  latVarId = latDimId = lonVarId = lonDimId = -1;

  // walk the variables and search for variables with 1 dimension and standard_name/axis/unit attributes
  varId = 0;
  while (varId < nVariables && (latVarId == -1 || lonVarId == -1)) {
    if (nc_inq_varndims(ncId, varId, &nDims) == NC_NOERR && nDims == 1 &&
        nc_inq_varnatts (ncId, varId, &nAtts) == NC_NOERR && nAtts > 0) {
      // could be a coordinate variable:
      // - test whether standard_name attribute is 'latitude' or 'longitude'
      // - test wheter axis attribute is 'X' or 'Y'
      // - test whether unit attribute is allowed variant of 'degrees_north' or 'degrees_east'.
      if (nc_inq_attlen(ncId, varId, "standard_name", &length) == NC_NOERR && length < sizeof(attrStr) &&
          nc_get_att_text(ncId, varId, "standard_name", attrStr) == NC_NOERR) {
        attrStr[length] = '\0';
        if (!strcmp(attrStr, "latitude")) {
          latVarId = varId;
          ++varId;
          continue;
        }
        if (!strcmp(attrStr, "longitude")) {
          lonVarId = varId;
          ++varId;
          continue;
        }
      }
      if (nc_inq_attlen(ncId, varId, "axis", &length) == NC_NOERR && length < sizeof(attrStr) &&
          nc_get_att_text(ncId, varId, "axis", attrStr) == NC_NOERR) {
        attrStr[length] = '\0';
        if (!strcmp(attrStr, "X")) {
          latVarId = varId;
          ++varId;
          continue;
        }
        if (!strcmp(attrStr, "Y")) {
          lonVarId = varId;
          ++varId;
          continue;
        }
      }
      if (nc_inq_attlen(ncId, varId, "units", &length) == NC_NOERR && length < sizeof(attrStr) &&
          nc_get_att_text(ncId, varId, "units", attrStr) == NC_NOERR) {
        attrStr[length] = '\0';
        if (!strcmp(attrStr, "degrees_north") || !strcmp(attrStr, "degree_north") || !strcmp(attrStr, "degree_N") ||
            !strcmp(attrStr, "degrees_N") || !strcmp(attrStr, "degreeN") || !strcmp(attrStr, "degreesN")) {
          latVarId = varId;
          ++varId;
          continue;
        }
        if (!strcmp(attrStr, "degrees_east") || !strcmp(attrStr, "degree_east") || !strcmp(attrStr, "degree_E") ||
            !strcmp(attrStr, "degrees_E") || !strcmp(attrStr, "degreeE") || !strcmp(attrStr, "degreesE")) {
          lonVarId = varId;
          ++varId;
          continue;
        }
      }
    }

    ++varId;
  }

  // MUST have found the latitude and longitude variables.
  status = (latVarId != -1 && lonVarId != -1) ? NC_NOERR : 1;

  // get the latitude dimension id - have already checked that it has only one
  if (status == NC_NOERR)
    status = nc_inq_vardimid(ncId, latVarId, &latDimId);

  // get the longitude dimension id - have already checked that it has only one
  if (status == NC_NOERR)
    status = nc_inq_vardimid(ncId, lonVarId, &lonDimId);

  delete [] dimArray;

  return status;
}


int ExtractGridCoordinateVariableIds(int ncId, int nVariables, int nDimensions,
				     int &latVarId, int &latBndVarId, int &latDimId, size_t &nLat,
				     int &lonVarId, int &lonBndVarId, int &lonDimId, size_t &nLon)
{
  const char cBoundsStr[] = "bounds";

  int status;
  int nDims, nAtts, tmpDimId;
  size_t length;
  nc_type xType;
  char name[NC_MAX_NAME+1];
  
  int *dimArray = new int[nDimensions];

  // initialize all output ints to -1 and the sizes to 0
  latBndVarId = lonBndVarId = -1;
  nLat = nLon = 0;
  
  status = ExtractLatLonCoordinateVariableIds(ncId, nVariables, nDimensions, latVarId, latDimId, lonVarId, lonDimId);

  // the latitude and longitude dimensions must be different and the dimension
  // names must be the same as the variables.

  if (status == NC_NOERR)
    status = (latDimId != lonDimId) ? NC_NOERR : 1;

  // check the lat variable
  if (status == NC_NOERR) {
    // the name of the variable MUST be the corresponding dimension    
    status = nc_inq_varname(ncId, latVarId, name);
    if (status == NC_NOERR) {
      status = nc_inq_dimid(ncId, name, &tmpDimId);
      if (status == NC_NOERR && tmpDimId == latDimId) {
        status = nc_inq_dimlen(ncId, latDimId, &nLat);
        if (status == NC_NOERR)
          status = (nLat > 0) ? NC_NOERR : 1;
      }
      else if (status == NC_NOERR) {
        status = 1; // in case the dimension ids dont match
      }
    }
    
    if (status == NC_NOERR) {
      if (nc_inq_attlen(ncId, latVarId, cBoundsStr, &length) == NC_NOERR && length <= NC_MAX_NAME) {
        status = nc_get_att_text(ncId, latVarId, cBoundsStr, name);
        if (status == NC_NOERR) {
          name[length] = '\0';
          status = nc_inq_varid(ncId, name, &latBndVarId);
        }
        // check the bounds variables dimensions ...
        if (status == NC_NOERR)
          status = nc_inq_var(ncId, latBndVarId, name, &xType, &nDims, dimArray, &nAtts);
        if (status == NC_NOERR)
          status = ((nDims == 2) && (dimArray[0] == latDimId)) ? NC_NOERR : 1;
        if (status == NC_NOERR)
          status = nc_inq_dimlen(ncId, dimArray[1], &length);
        if (status == NC_NOERR)
          status = (length == 2) ? NC_NOERR : 1;
      }
    }
  }
  
  // check the lon variable
  if (status == NC_NOERR) {
    // the name of the variable MUST be the corresponding dimension    
    status = nc_inq_varname(ncId, lonVarId, name);
    if (status == NC_NOERR) {
      status = nc_inq_dimid(ncId, name, &tmpDimId);
      if (status == NC_NOERR && tmpDimId == lonDimId) {
        status = nc_inq_dimlen(ncId, lonDimId, &nLon);
        if (status == NC_NOERR)
          status = (nLon > 0) ? NC_NOERR : 1;
      }
      else if (status == NC_NOERR) {
        status = 1; // in case the dimension ids dont match
      }
    }
    
    if (status == NC_NOERR) {
      if (nc_inq_attlen(ncId, lonVarId, cBoundsStr, &length) == NC_NOERR && length <= NC_MAX_NAME) {
        status = nc_get_att_text(ncId, lonVarId, cBoundsStr, name);
        if (status == NC_NOERR) {
          name[length] = '\0';
          status = nc_inq_varid(ncId, name, &lonBndVarId);
        }
        // check the bounds variables dimensions ...
        if (status == NC_NOERR)
          status = nc_inq_var(ncId, lonBndVarId, name, &xType, &nDims, dimArray, &nAtts);
        if (status == NC_NOERR)
          status = ((nDims == 2) && (dimArray[0] == lonDimId)) ? NC_NOERR : 1;
        if (status == NC_NOERR)
          status = nc_inq_dimlen(ncId, dimArray[1], &length);
        if (status == NC_NOERR)
          status = (length == 2) ? NC_NOERR : 1;
      }
    }
  }
  
  // should have non zero lengths for both dimensions
  if (status == NC_NOERR)
    status = (nLon > 0 && nLat > 0) ? NC_NOERR : 1;
  
  delete [] dimArray;

  return status;
}

int ExtractCrsVariable(int ncId, int *crsVarId)
{
  int status = nc_inq_varid(ncId, "crs", crsVarId);

  if (status == NC_NOERR) {

    /*
    if (status == NC_NOERR)
      status = nc_get_att_double(ncId, *crsVarId, "semi_major_axis", &a);
    if (status == NC_NOERR)
      status = nc_get_att_double(ncId, *crsVarId, "inverse_flattening", &f);
    if (status == NC_NOERR)
      status = nc_get_att_double(ncId, *crsVarId, "earth_gravity_constant", &gm);
    if (status == NC_NOERR)
      status = nc_get_att_double(ncId, *crsVarId, "earth_rotation_rate", &w);
    */

    return status;
  }

  // non-existence is not an error.
  *crsVarId = -1;

  return NC_NOERR;
}

int ExtractTimestampVariable(int ncId, int *timestampVarId)
{
  size_t length;

  int status = nc_inq_varid(ncId, "zztimestamp", timestampVarId);

  if (status == NC_NOERR) {

	includeTimestamp = true;

	memset(timestampUnits, 0, sizeof(timestampUnits));

	if (nc_inq_attlen(ncId, *timestampVarId, "units", &length) == NC_NOERR
		  && length < sizeof(timestampUnits)
		  && nc_get_att_text(ncId, *timestampVarId, "units", timestampUnits)
		  	  	  == NC_NOERR) {

		timestampUnits[length] = '\0';
	}

    return status;
  }

  // non-existence is not an error.
  *timestampVarId = -1;

  includeTimestamp = false;

  return NC_NOERR;
}

bool WriteGeoTiff(const std::string &filename,
				  int ncId, int dataVarId, bool swap_axis,
				  int nLat, int nLon, 
				  double minValue, double maxValue,
				  const ColourTable *colourTable,
                  bool areaGrid,
				  double latNorth, double lonWest,
				  double dLat, double dLon,
				  bool repeatFirstColumn,
				  std::vector<double>& latAxis,
				  std::vector<double>& lonAxis,
				  int timestampVarId)
{
  const int samplePerPixel = 4;
  size_t startArray[2], countArray[2];

  size_t length;

  // open the tiff file 
  TIFF *tiff = XTIFFOpen(filename.c_str(), "w");
  
  if (tiff == NULL) {
    std::cerr << "ERROR: could not open TIFF file for output" << std::endl;
    return false;
  }

  int imageWidth = repeatFirstColumn ? (nLon + 1) : nLon;

  // TIFF tags for the raster image
  TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, imageWidth);            // set the width of the image
  TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, nLat);                 // set the height of the image + extra for colour bar
  TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, samplePerPixel);   // set number of channels per pixel
  TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8);                  // set the size of the channels
  TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);  // set the origin of the image.
  TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, 1);                   // place each row in a separate strip
  TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_LZW);      // enable compression (on a per strip basis)

  // geotiff keys.
  GTIF *geoTiff = GTIFNew(tiff);
  
  if (geoTiff != NULL) {
    // setup the tiepoint and scale data
    double tiePoint[6] = {0.0};
    double pixelScale[3] = {0.0};

    // map upper left (pixel) to North-West corner of grid
    if (areaGrid) {
      tiePoint[0] = tiePoint[1] = 0.5;
    }
    tiePoint[3] = lonWest;
    tiePoint[4] = latNorth;

    pixelScale[0] = dLon;
    pixelScale[1] = dLat;

    TIFFSetField(tiff, TIFFTAG_GEOTIEPOINTS, 6, tiePoint);
    TIFFSetField(tiff, TIFFTAG_GEOPIXELSCALE, 3, pixelScale);

    GTIFKeySet(geoTiff, GTModelTypeGeoKey, TYPE_SHORT, 1, ModelTypeGeographic);
    GTIFKeySet(geoTiff, GTRasterTypeGeoKey, TYPE_SHORT, 1,
	       (areaGrid ? RasterPixelIsArea : RasterPixelIsPoint));
    
    // the basic geometry of the ellipsoid
    GTIFKeySet(geoTiff, GeogLinearUnitsGeoKey, TYPE_SHORT, 1, Linear_Meter);
    /*
    // TODO - Reference Ellipsoid
    tmpDouble = re.ellipsoid().semimajor();
    GTIFKeySet(geoTiff, GeogSemiMajorAxisGeoKey, TYPE_DOUBLE, 1, tmpDouble);
    tmpDouble = re.ellipsoid().semiminor();
    GTIFKeySet(geoTiff, GeogSemiMinorAxisGeoKey, TYPE_DOUBLE, 1, tmpDouble);
    tmpDouble = 1.0 / re.ellipsoid().flattening();
    GTIFKeySet(geoTiff, GeogInvFlatteningGeoKey, TYPE_DOUBLE, 1, tmpDouble);
    */

    GTIFWriteKeys(geoTiff);
    GTIFFree(geoTiff);
    geoTiff = NULL;
  }

  // Allocate memory to store one row of data and one row of pixels
  unsigned char *pixelBuffer = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(tiff));
  
  if (pixelBuffer == NULL) {
    TIFFClose(tiff);
    return false;
  }

  std::vector<double> dataBuffer(nLon);
  std::vector<double> timestampBuffer(nLon);
  unsigned char *pCursor;
  uint32 row = 0;
  uint32 column = 0;
  uint32 height = nLat;
  double fillValue;
  double scale = 1.0 / (maxValue - minValue);
  
  if (nc_get_att_double(ncId, dataVarId, "_FillValue", &fillValue) != NC_NOERR)
    fillValue = NC_FILL_DOUBLE;

  // setup the values to fetch a single row, starting at the bottom;
  if (swap_axis) {
    startArray[0] = 0;
    startArray[1] = nLat;
    countArray[0] = nLon;
    countArray[1] = 1;
  }
  else {
    startArray[0] = nLat;
    startArray[1] = 0;
    countArray[0] = 1;
    countArray[1] = nLon;
  }

  /*
   * Stores the units and name of the data variable
   */
  memset(dataUnits, 0, sizeof(dataUnits));

  if (nc_inq_attlen(ncId, dataVarId, "units", &length) == NC_NOERR
		  && length < sizeof(dataUnits)
		  && nc_get_att_text(ncId, dataVarId, "units", dataUnits) == NC_NOERR) {

    dataUnits[length] = '\0';
  }

  if ((strcmp(dataUnits, "mm") == 0)
		  || (strcmp(dataUnits, "cm") == 0)
		  || (strcmp(dataUnits, "m") == 0)
		  || (strcmp(dataUnits, "meters") == 0)
		  || (strcmp(dataUnits, "meter") == 0)
		  || (strcmp(dataUnits, "km") == 0)) {

	  isAltData = true;

  } else {

	  isAltData = false;
  }

  memset(dataName, 0, sizeof(dataName));

  nc_inq_varname(ncId, dataVarId, dataName);

  // north to south construction of the image raster
  for (row = 0; row < height; row++) {
    
    // work north to south in source data
    if (swap_axis)
      --startArray[1];
    else
      --startArray[0];

    if (nc_get_vara_double(ncId, dataVarId, startArray, countArray, &(dataBuffer.front())) != NC_NOERR) {
      // set to transparent on failure ...
      std::fill(pixelBuffer, pixelBuffer + 4 * imageWidth, 0);
    }
    else {
      // map the data value to a colour value
      std::vector<double>::const_iterator it = dataBuffer.begin();
      std::vector<double>::const_iterator endIt = dataBuffer.end();
      pCursor = pixelBuffer;

      // reads the timestamp variable if it exists in the NC file
      if (includeTimestamp) {

    	  if (nc_get_vara_double(ncId, timestampVarId, startArray, countArray,
    			  &(timestampBuffer.front())) != NC_NOERR) {

    		  // set timestamp to -1 if any error reading this row

    		  std::fill(timestampBuffer.begin(), timestampBuffer.end(), -1);
    	  }
      }

      // populate the scan-line from the row data
      column = 0;
      while (it != endIt) {
      
        if (*it == fillValue || isNaN(*it)) {
          pCursor[0] = 0;
          pCursor[1] = 0;
          pCursor[2] = 0;
          pCursor[3] = 0;
        }
        else {

        	colourTable->getRGBA((*it - minValue) * scale, pCursor);

			/*
			 * Adds the point information to the kmlTrackVector
			 */
			KmlTrackPoint kmlTrackPoint;

			kmlTrackPoint.lat = latAxis.at(nLat - 1 - row);
			kmlTrackPoint.lon = lonAxis.at(column);
			kmlTrackPoint.alt = 0;
			kmlTrackPoint.data = *it;
			std::copy(pCursor, pCursor + 4, kmlTrackPoint.rgba);

			if (includeTimestamp) {

				kmlTrackPoint.timestamp = timestampBuffer.at(column);
			}

			kmlTrackVector.push_back(kmlTrackPoint);
        }

        pCursor += 4;
        ++it;
        column++;
      }
      if (repeatFirstColumn) {
        pCursor[0] = pixelBuffer[0];
        pCursor[1] = pixelBuffer[1];
        pCursor[2] = pixelBuffer[2];
        pCursor[3] = pixelBuffer[3];
        pCursor += 4;
      }
    }

    if (TIFFWriteScanline(tiff, pixelBuffer, row, 0) < 0)
      break;
  }

  TIFFClose(tiff);

  if (pixelBuffer)
    _TIFFfree(pixelBuffer);

  if (row != height) {

	  return false;
  }

  /*
   * Writes the colour bar in another tiff file
   */
  
  colourBarFilename =
		  filename.substr(0, filename.find_last_of(".")) + "_colourbar.tif";

  // open the tiff file
  TIFF *tiffBar = XTIFFOpen(colourBarFilename.c_str(), "w");

  if (tiffBar == NULL) {
    std::cerr << "ERROR: could not open TIFF file for colour bar output" << std::endl;
    return false;
  }

  const unsigned int colourBarWidth = 50;
  unsigned int colourBarHeight = 500;

  // TIFF tags for the raster image
  TIFFSetField(tiffBar, TIFFTAG_IMAGEWIDTH, colourBarWidth);        // set the width of the image
  TIFFSetField(tiffBar, TIFFTAG_IMAGELENGTH, colourBarHeight);      // set the height of the image + extra for colour bar
  TIFFSetField(tiffBar, TIFFTAG_SAMPLESPERPIXEL, samplePerPixel);   // set number of channels per pixel
  TIFFSetField(tiffBar, TIFFTAG_BITSPERSAMPLE, 8);                  // set the size of the channels
  TIFFSetField(tiffBar, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);  // set the origin of the image.
  TIFFSetField(tiffBar, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(tiffBar, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  TIFFSetField(tiffBar, TIFFTAG_ROWSPERSTRIP, 1);                   // place each row in a separate strip
  TIFFSetField(tiffBar, TIFFTAG_COMPRESSION, COMPRESSION_LZW);      // enable compression (on a per strip basis)

  unsigned char *colourBarpixelBuffer =
		  (unsigned char *)_TIFFmalloc(TIFFScanlineSize(tiffBar));

  if (colourBarpixelBuffer == NULL) {

    TIFFClose(tiffBar);
    return false;
  }

  double step = (maxValue - minValue) / (colourBarHeight - 1);

  double colour_map_in_array[colourBarWidth];

  for (row = 0; row < colourBarHeight; row++) {

	  pCursor = colourBarpixelBuffer;

	  for (column = 0; column < colourBarWidth; ++column) {

		  if ((row == 0) || (row == colourBarHeight - 1)
				  || (column == 0) || (column == colourBarWidth - 1)) {

	          pCursor[0] = 255;
	          pCursor[1] = 255;
	          pCursor[2] = 255;
	          pCursor[3] = 255;

		  } else {

			  colour_map_in_array[column] =
					  ((minValue + row * step) - minValue) * scale;

			  colourTable->getRGBA(colour_map_in_array[column], pCursor);
		  }

		  pCursor += 4;
	  }

	  if (TIFFWriteScanline(tiffBar, colourBarpixelBuffer, row, 0) < 0) {

		  break;
	  }
  }

  // colour bar label values
  barLabelValues.resize(numberOfBarLabelValues);

  double barLabelValuesScale =
		  (maxValue - minValue) / (numberOfBarLabelValues - 1);

  for (unsigned int valueIndex = 0; valueIndex < numberOfBarLabelValues;
		  ++valueIndex) {

	  barLabelValues[valueIndex] = minValue + valueIndex * barLabelValuesScale;
  }

  TIFFClose(tiffBar);

  if (colourBarpixelBuffer)
    _TIFFfree(colourBarpixelBuffer);

  return (row == colourBarHeight);
}

void ExtractMinAndMax(int ncId, int dataVarId, int nRow, int nCol, double &minValue, double &maxValue)
{
  std::vector<double> dataBuffer(nCol);
  size_t startArray[2], countArray[2];

  // setup the values to fetch a single row, starting at the bottom;
  startArray[0] = 0;
  startArray[1] = 0;
  countArray[0] = 1;
  countArray[1] = nCol;
  
  // initialize
  minValue = DBL_MAX;
  maxValue = -DBL_MAX;

  // blast through the data, row by row
  for (int row = 0; row < nRow; ++row) {

    if (nc_get_vara_double(ncId, dataVarId, startArray, countArray, &(dataBuffer.front())) == NC_NOERR) {

      // scan the data
      std::vector<double>::const_iterator it = dataBuffer.begin();
      std::vector<double>::const_iterator endIt = dataBuffer.end();
    
      while (it != endIt) {
        if (*it < minValue)
          minValue = *it;
        if (*it > maxValue)
          maxValue = *it;
        ++it;
      }
    }
    
    ++startArray[0];
  }
}

bool ExportKml(const std::string &kmlFileName, const std::string &tiffFileName,
		const std::string &screenLogoUrl,
		struct CoordBoundaries *coordBoundaries,
		const std::string &filetypeName, const std::string &productList)
{

  std::ofstream stream(kmlFileName.c_str());

  if (!stream.is_open()) {
    std::cerr << "ERROR: could not open KML file for output" << std::endl;
    return false;
  }

  stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
  stream << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">" << std::endl;
  stream << "	<Document>" << std::endl;

  stream << " 		<name>" << filetypeName << "</name>" << std::endl;
  stream << " 		<description>" << productList << "</description>" << std::endl;
  stream << "		<open>0</open>" << std::endl;
  stream << " 		<visibility>1</visibility>" << std::endl;

  /*
   * Ground Overlays folder: Geotiff image
   */
  stream << "		<Folder id=\"Geotiff\">" << std::endl;
  stream << "			<name>Geotiff</name>" << std::endl;
  stream << "			<GroundOverlay>" << std::endl;
  stream << "				<name>"<< dataName << "</name>" << std::endl;
  stream << "				<Icon>" << std::endl;
  stream << "					<href>" << tiffFileName << "</href>" << std::endl;
  stream << "				</Icon>" << std::endl;
  stream << "				<LatLonBox>" << std::endl;
  stream << "					<north>" << std::setw(18) << coordBoundaries->latNorth << "</north>" << std::endl;
  stream << "					<south>" << std::setw(18) << coordBoundaries->latSouth << "</south>" << std::endl;
  stream << "					<east>" << std::setw(18) << coordBoundaries->lonEast << "</east>" << std::endl;
  stream << "					<west>" << std::setw(18) << coordBoundaries->lonWest << "</west>" << std::endl;
  stream << "					<rotation>0.0</rotation>" << std::endl;
  stream << "				</LatLonBox>" << std::endl;
  stream << "			</GroundOverlay>" << std::endl;
  stream << "		</Folder>" << std::endl;

  /*
   * Screen Overlays folder: logo and colour bar (image and texts)
   */
  stream << "		<Folder id=\"Legend\">" << std::endl;
  stream << "			<name>Legend</name>" << std::endl;

  if (!screenLogoUrl.empty()) {
    stream << "			<ScreenOverlay>" << std::endl;
    stream << "				<name>Logo</name>" << std::endl;
    stream << "				<Icon>" << std::endl;
    stream << "					<href>" << screenLogoUrl << "</href>" << std::endl;
    stream << "				</Icon>" << std::endl;
    stream << "				<overlayXY x=\"0\" y=\"54\" xunits=\"pixels\" yunits=\"pixels\"/>" << std::endl;
    stream << "				<screenXY x=\"0.01\" y=\"0.99\" xunits=\"fraction\" yunits=\"fraction\"/>" << std::endl;
    stream << "				<rotation>0</rotation>" << std::endl;
    stream << "				<size x=\"0\" y=\"0\" xunits=\"pixels\" yunits=\"pixels\"/>" << std::endl;
    stream << "			</ScreenOverlay>" << std::endl;
  }

  stream << "			<Folder id=\"Colour_Bar\">" << std::endl;
  stream << "				<name>Colour Bar</name>" << std::endl;

  stream << "				<ScreenOverlay>" << std::endl;
  stream << "					<name>Label</name>" << std::endl;
  stream << "					<Icon>" << std::endl;
  stream << "						<href><![CDATA[http://chart.apis.google.com/chart?chst=d_text_outline&chld=FFFFFF|16|h|000000|b|"
		  << dataName << " [" << dataUnits << "]]]></href>" << std::endl;
  stream << "					</Icon>" << std::endl;
  stream << "					<overlayXY x=\"0\" y=\"90\" xunits=\"pixels\" yunits=\"pixels\"/>" << std::endl;
  stream << "					<screenXY x=\"0.01\" y=\"0.99\" xunits=\"fraction\" yunits=\"fraction\"/>" << std::endl;
  stream << "					<rotation>0</rotation>" << std::endl;
  stream << "					<size x=\"0\" y=\"0\" xunits=\"pixels\" yunits=\"pixels\"/>" << std::endl;
  stream << "				</ScreenOverlay>" << std::endl;

  stream << "				<ScreenOverlay>" << std::endl;
  stream << "					<name>Colour Bar</name>" << std::endl;
  stream << "					<Icon>" << std::endl;
  stream << "						<href>" << colourBarFilename << "</href>" << std::endl;
  stream << "					</Icon>" << std::endl;
  stream << "					<overlayXY x=\"0\" y=\"600\" xunits=\"pixels\" yunits=\"pixels\"/>" << std::endl;
  stream << "					<screenXY x=\"0.01\" y=\"0.99\" xunits=\"fraction\" yunits=\"fraction\"/>" << std::endl;
  stream << "					<rotation>0</rotation>" << std::endl;
  stream << "					<size x=\"0\" y=\"0\" xunits=\"pixels\" yunits=\"pixels\"/>" << std::endl;
  stream << "				</ScreenOverlay>" << std::endl;

  for (unsigned int valueIndex = 0; valueIndex < numberOfBarLabelValues;
		  ++valueIndex) {

	  double yValue = 115.0 + (double)valueIndex * 60.625;

	  stream << "				<ScreenOverlay>" << std::endl;
	  stream << "					<name>Tick Value #" << valueIndex
			  << "</name>" << std::endl;
	  stream << "					<Icon>" << std::endl;
	  stream << "						<href><![CDATA[http://chart.apis.google.com/chart?chst=d_text_outline&chld=FFFFFF|16|h|000000|b|"
			  << barLabelValues[valueIndex] << "]]></href>" << std::endl;
	  stream << "					</Icon>" << std::endl;
	  stream << "					<overlayXY x=\"-60\" y=\"" << yValue << "\" xunits=\"pixels\" yunits=\"pixels\"/>" << std::endl;
	  stream << "					<screenXY x=\"0.01\" y=\"0.99\" xunits=\"fraction\" yunits=\"fraction\"/>" << std::endl;
	  stream << "					<rotation>0</rotation>" << std::endl;
	  stream << "					<size x=\"0\" y=\"0\" xunits=\"pixels\" yunits=\"pixels\"/>" << std::endl;
	  stream << "				</ScreenOverlay>" << std::endl;
  }

  stream << "			</Folder>" << std::endl;
  stream << "		</Folder>" << std::endl;

  /*
   * Placemarks folder: Track map points
   */
  stream << "		<Folder id=\"Track\">" << std::endl;
  stream << "			<name>Along Track Data </name>" << std::endl;

  for (unsigned int pointIndex = 0; pointIndex < kmlTrackVector.size();
		  ++pointIndex) {

	  double altCoordinate;
	  std::stringstream altDescription;
	  std::string altitudeMode;
	  std::stringstream timestampDescription;

	  if (isAltData) {

		  if (strcmp(dataUnits, "mm") == 0) {

			  altCoordinate = kmlTrackVector.at(pointIndex).data / 1000.0;

	  	  } else if (strcmp(dataUnits, "cm") == 0) {

		  		altCoordinate = kmlTrackVector.at(pointIndex).data / 100.0;

	  	  } else if (strcmp(dataUnits, "km") == 0) {

		  		altCoordinate = kmlTrackVector.at(pointIndex).data * 1000.0;

	  	  } else {

	  		altCoordinate = kmlTrackVector.at(pointIndex).data;
	  	  }

		  altDescription << dataName << " = "
				  << kmlTrackVector.at(pointIndex).data << " " << dataUnits;

		  altitudeMode = "relativeToGround";

	  } else {

		  altCoordinate = kmlTrackVector.at(pointIndex).alt;

		  altDescription << "Alt = "
				  << kmlTrackVector.at(pointIndex).alt << " m, "
				  << dataName << " = " << kmlTrackVector.at(pointIndex).data
				  << " " << dataUnits;

		  altitudeMode = "clampedToGround";
	  }

	  if (includeTimestamp) {

		  timestampDescription << ", Timestamp = "
				  << kmlTrackVector.at(pointIndex).timestamp << " "
				  << timestampUnits;

	  } else {

		  timestampDescription << "";
	  }

	  std::stringstream ssColour;
	  ssColour << std::setw(2) << std::hex << std::setfill('0')
			  << (int)kmlTrackVector.at(pointIndex).rgba[2]
			  << std::setw(2) << std::hex << std::setfill('0')
	          << (int)kmlTrackVector.at(pointIndex).rgba[1]
			  << std::setw(2) << std::hex << std::setfill('0')
	          << (int)kmlTrackVector.at(pointIndex).rgba[0];

	  stream << "			<Placemark>" << std::endl;
	  stream << "				<Point>" << std::endl;
	  stream << "   				<altitudeMode>" << altitudeMode << "</altitudeMode>" << std::endl;
	  stream << "					<coordinates>"
			  << kmlTrackVector.at(pointIndex).lon << ","
			  << kmlTrackVector.at(pointIndex).lat << ","
			  << altCoordinate
			  << "</coordinates>" << std::endl;
	  stream << "				</Point>" << std::endl;
	  stream << "				<Style>" << std::endl;
	  stream << "					<IconStyle>" << std::endl;
	  stream << "						<color>FF" << ssColour.str() << "</color>" << std::endl;
	  stream << "					</IconStyle>" << std::endl;
	  stream << "					<LabelStyle>" << std::endl;
	  stream << "						<color>FF" << ssColour.str() << "</color>" << std::endl;
	  stream << "					</LabelStyle>" << std::endl;
	  stream << "				</Style>" << std::endl;
	  stream << "				<description>"
			  << "Lon = " << kmlTrackVector.at(pointIndex).lon << " deg, "
			  << "Lat = " << kmlTrackVector.at(pointIndex).lat << " deg, "
			  << altDescription.str()
			  << timestampDescription.str()
			  << "</description>" << std::endl;
	  stream << "				<name>Track point #" << pointIndex << "</name>" << std::endl;
	  stream << "				<styleUrl>#gv_waypoint</styleUrl>" << std::endl;
	  stream << "			</Placemark>" << std::endl;
  }

  stream << "		</Folder>" << std::endl;

  /*
   * Points style
   */
  stream << "		<Style id=\"gv_waypoint_normal\">" << std::endl;
  stream << "			<BalloonStyle>" << std::endl;
  stream << "				<text><![CDATA[<p align=\"left\" style=\"white-space:nowrap;\"><font size=\"+1\"><b>$[name]</b></font></p> <p align=\"left\">$[description]</p>]]></text>" << std::endl;
  stream << "			</BalloonStyle>" << std::endl;
  stream << "			<IconStyle>" << std::endl;
  stream << "        		<Icon>" << std::endl;
  stream << "					<href>http://maps.google.ca/mapfiles/kml/pal4/icon57.png</href>" << std::endl;
  stream << "				</Icon>" << std::endl;
  stream << "				<color>FFFFFFFF</color>" << std::endl;
  stream << "				<hotSpot x=\"0.5\" xunits=\"fraction\" y=\"0.5\" yunits=\"fraction\" />" << std::endl;
  stream << "			</IconStyle>" << std::endl;
  stream << "			<LabelStyle>" << std::endl;
  stream << "        		<color>FFFFFFFF</color>" << std::endl;
  stream << "        		<scale>0</scale>" << std::endl;
  stream << "     		</LabelStyle>" << std::endl;
  stream << " 		</Style>" << std::endl;
  stream << "		<Style id=\"gv_waypoint_highlight\">" << std::endl;
  stream << "     		<BalloonStyle>" << std::endl;
  stream << "        		<text><![CDATA[<p align=\"left\" style=\"white-space:nowrap;\"><font size=\"+1\"><b>$[name]</b></font></p> <p align=\"left\">$[description]</p>]]></text>" << std::endl;
  stream << "    		</BalloonStyle>" << std::endl;
  stream << "    		<IconStyle>" << std::endl;
  stream << "        		<Icon>" << std::endl;
  stream << "           		<href>http://maps.google.ca/mapfiles/kml/pal4/icon57.png</href>" << std::endl;
  stream << "         		</Icon>" << std::endl;
  stream << "       		<color>FFFFFFFF</color>" << std::endl;
  stream << "       		<hotSpot x=\"0.5\" xunits=\"fraction\" y=\"0.5\" yunits=\"fraction\" />" << std::endl;
  stream << "        		<scale>1.2</scale>" << std::endl;
  stream << "    		</IconStyle>" << std::endl;
  stream << "    		<LabelStyle>" << std::endl;
  stream << "    			<color>FFFFFFFF</color>" << std::endl;
  stream << "     			<scale>1</scale>" << std::endl;
  stream << "   		</LabelStyle>" << std::endl;
  stream << "		</Style>" << std::endl;
  stream << " 		<StyleMap id=\"gv_waypoint\">" << std::endl;
  stream << "   		<Pair>" << std::endl;
  stream << "      			<key>normal</key>" << std::endl;
  stream << "      			<styleUrl>#gv_waypoint_normal</styleUrl>" << std::endl;
  stream << "  			</Pair>" << std::endl;
  stream << "   		<Pair>" << std::endl;
  stream << "       		<key>highlight</key>" << std::endl;
  stream << "       		<styleUrl>#gv_waypoint_highlight</styleUrl>" << std::endl;
  stream << "   		</Pair>" << std::endl;
  stream << "		</StyleMap>" << std::endl;

  stream << "	</Document>" << std::endl;
  stream << "</kml>" << std::endl;

  return true;
}


//--------------------------------------------------
// Colour Table
//--------------------------------------------------

ColourTable::ColourTable(const std::string &tableName)
{
  if (tableName == "Aerosol")
    prepareAerosol();
  else if (tableName == "Blackbody")
    prepareBlackbody();
  else if (tableName == "BlackToWhite")
    prepareBlackToWhite();
  else if (tableName == "Cloud")
    prepareCloud();
  else if (tableName == "Ozone")
    prepareOzone();
  else if (tableName == "GreenToRed")
    prepareGreenToRed();
  else if (tableName == "Rainbow")
    prepareRainbow();
  else if (tableName == "RedToGreen")
    prepareRedToGreen();
  else if (tableName == "WhiteToBlack")
    prepareWhiteToBlack();
  else
    prepareColdToHot(); // default
}

ColourTable::~ColourTable()
{
}

void ColourTable::getRGBA(double x, unsigned char rgba[4]) const
{
  // Guaranteed that there are at least 2 vertexes in the list and that
  // they are sorted in increasing x order and that the first vertex
  // has x=0.0 and the last has x=1.0;

  // iterators for the start and end of an edge (consequtive vertexes)
  std::list<CTVertex>::const_iterator itA, itB;

  itA = m_vertexes.begin();

  if (x <= itA->x) {
    rgba[0] = (unsigned char)(itA->r * 255);
    rgba[1] = (unsigned char)(itA->g * 255);
    rgba[2] = (unsigned char)(itA->b * 255);
    rgba[3] = (unsigned char)(itA->a * 255);
    return;
  }

  itB = itA;
  ++itB;
  while (itB != m_vertexes.end() && x > itB->x)
  {
      ++itA;
      ++itB;
  }

  if (itB == m_vertexes.end())
  {
    rgba[0] = (unsigned char)(itA->r * 255);
    rgba[1] = (unsigned char)(itA->g * 255);
    rgba[2] = (unsigned char)(itA->b * 255);
    rgba[3] = (unsigned char)(itA->a * 255);
    return;
  }
  
  double fracB = (x - itA->x) / (itB->x - itA->x);
  double fracA = 1.0 - fracB;

  rgba[0] = (unsigned char)((fracA * itA->r + fracB * itB->r) * 255);
  rgba[1] = (unsigned char)((fracA * itA->g + fracB * itB->g) * 255);
  rgba[2] = (unsigned char)((fracA * itA->b + fracB * itB->b) * 255);
  rgba[3] = (unsigned char)((fracA * itA->a + fracB * itB->a) * 255);

  return;
}

void ColourTable::prepareAerosol(void)
{
  // added in sorted order ...   x     r    g    b    a
  m_vertexes.push_back(CTVertex(0.0,  0.0, 0.0, 0.0, 1.0));
  m_vertexes.push_back(CTVertex(0.25, 0.0, 0.5, 1.0, 1.0));
  m_vertexes.push_back(CTVertex(0.5,  0.0, 1.0, 1.0, 1.0));
  m_vertexes.push_back(CTVertex(0.75, 1.0, 1.0, 0.0, 1.0));
  m_vertexes.push_back(CTVertex(1.0,  1.0, 0.0, 0.0, 1.0));
}

void ColourTable::prepareBlackToWhite(void)
{
  // added in sorted order ...   x     r    g    b    a
  m_vertexes.push_back(CTVertex(0.0,  0.0, 0.0, 0.0, 1.0));
  m_vertexes.push_back(CTVertex(1.0,  1.0, 1.0, 1.0, 1.0));
}

void ColourTable::prepareWhiteToBlack(void)
{
  // added in sorted order ...   x     r    g    b    a
  m_vertexes.push_back(CTVertex(0.0,  1.0, 1.0, 1.0, 1.0));
  m_vertexes.push_back(CTVertex(1.0,  0.0, 0.0, 0.0, 1.0));
}

void ColourTable::prepareGreenToRed(void)
{
  // added in sorted order ...   x     r    g    b    a
  m_vertexes.push_back(CTVertex(0.0,  0.0, 1.0, 0.0, 1.0));
  m_vertexes.push_back(CTVertex(1.0,  1.0, 0.0, 0.0, 1.0));
}

void ColourTable::prepareRedToGreen(void)
{
  // added in sorted order ...   x     r    g    b    a
  m_vertexes.push_back(CTVertex(0.0,  1.0, 0.0, 0.0, 1.0));
  m_vertexes.push_back(CTVertex(1.0,  0.0, 1.0, 0.0, 1.0));
}

void ColourTable::prepareOzone(void)
{
  // added in sorted order ...   x         r    g    b    a
  m_vertexes.push_back(CTVertex(0.0,      0.0, 0.0, 0.0, 1.0));
  m_vertexes.push_back(CTVertex(1.0/7.0,  0.0, 0.0, 1.0, 1.0));
  m_vertexes.push_back(CTVertex(2.0/7.0,  0.0, 1.0, 1.0, 1.0));
  m_vertexes.push_back(CTVertex(3.0/7.0,  0.0, 1.0, 0.0, 1.0));
  m_vertexes.push_back(CTVertex(4.0/7.0,  1.0, 1.0, 0.0, 1.0));
  m_vertexes.push_back(CTVertex(5.0/7.0,  1.0, 0.0, 0.0, 1.0));
  m_vertexes.push_back(CTVertex(6.0/7.0,  1.0, 0.0, 1.0, 1.0));
  m_vertexes.push_back(CTVertex(1.0,      1.0, 1.0, 1.0, 1.0));
}

void ColourTable::prepareBlackbody(void)
{
  // added in sorted order ...   x         r    g    b    a
  m_vertexes.push_back(CTVertex(0.0,      0.0, 0.0, 0.0, 1.0));
  m_vertexes.push_back(CTVertex(1.0/3.0,  1.0, 0.0, 0.0, 1.0));
  m_vertexes.push_back(CTVertex(2.0/3.0,  1.0, 1.0, 0.0, 1.0));
  m_vertexes.push_back(CTVertex(1.0,      1.0, 1.0, 1.0, 1.0));
}

void ColourTable::prepareRainbow(void)
{
  // added in sorted order ...   x         r    g    b    a
  m_vertexes.push_back(CTVertex(0.0,      1.0, 0.0, 0.0, 1.0));
  m_vertexes.push_back(CTVertex(1.0/3.0,  1.0, 1.0, 0.0, 1.0));
  m_vertexes.push_back(CTVertex(0.5,      0.0, 1.0, 0.0, 1.0));
  m_vertexes.push_back(CTVertex(2.0/3.0,  0.0, 0.0, 1.0, 1.0));
  m_vertexes.push_back(CTVertex(1.0,      1.0, 0.0, 1.0, 1.0));
}

void ColourTable::prepareCloud(void)
{
  // added in sorted order ...   x     r    g    b    a
  m_vertexes.push_back(CTVertex(0.0,  1.0, 1.0, 1.0, 0.0));
  m_vertexes.push_back(CTVertex(1.0,  0.5, 0.5, 0.5, 0.5));
}

void ColourTable::prepareColdToHot(void)
{
  // added in sorted order ...   x         r    g    b    a
  m_vertexes.push_back(CTVertex(0.0,      0.0, 0.0, 1.0, 1.0));
  m_vertexes.push_back(CTVertex(0.25,     0.0, 1.0, 1.0, 1.0));
  m_vertexes.push_back(CTVertex(0.5,      0.0, 1.0, 0.0, 1.0));
  m_vertexes.push_back(CTVertex(0.75,     1.0, 1.0, 0.0, 1.0));
  m_vertexes.push_back(CTVertex(1.0,      1.0, 0.0, 0.0, 1.0));
}

