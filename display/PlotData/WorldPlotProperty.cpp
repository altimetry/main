
#include "WorldPlotProperty.h"
#include "BratLookupTable.h"


//-------------------------------------------------------------
//------------------- CWorldPlotProperty class --------------------
//-------------------------------------------------------------

CWorldPlotProperty::CWorldPlotProperty()
{

  //m_projection = "";
  m_opacity = 0.7;
  m_deltaRadius = 0.007;

  m_showPropertyPanel = true;
  m_showColorBar = true;
  m_showAnimationToolbar = false;
  //m_lineWidth = 1;
  //m_pointSize = 2;
  //m_color.Insert(0);
  //m_color.Insert(0);
  //m_color.Insert(1);

  m_heightFactor = 0;
  m_numColorLabels = 5;

  //m_colorTable = NULL;

  setDefaultValue(m_minHeightValue);
  setDefaultValue(m_maxHeightValue);

  //setDefaultValue(m_stipplepattern);
  m_valueConversion = 0.0;
  m_heightConversion = 0.0;

  m_centerLongitude = 0.0;
  m_centerLatitude = 0.0;

  m_LUT = new CBratLookupTable();

  m_LUT->ExecMethodDefaultColorTable();

  m_zoom = false;
  m_zoomLon1 = -180.0;
  m_zoomLon2 = 180.0;
  m_zoomLat1 = -90.0;
  m_zoomLat2 = 90.0;

  m_withContour = false;
  m_solidColor = true;;
  m_gridLabel = true;

  m_withContourLabel = false;
  setDefaultValue(m_minContourValue);
  setDefaultValue(m_maxContourValue);
  m_numContour = 5;
  m_numContourLabel = 1;

  m_contourLabelSize = 10;
  m_contourLineWidth = 1.0;

  m_contourLineColor.Set(0.0, 0.0, 0.0);
  m_contourLineColor.Set(0.0, 0.0, 0.0);

  m_contourLabelFormat = "%-#.3g";

  m_withAnimation = false;

  m_eastComponent = false;
  m_northComponent = false;


}

//----------------------------------------

CWorldPlotProperty::CWorldPlotProperty(const CWorldPlotProperty& p)
{
  m_LUT = NULL;
  this->Copy(p);

}

//----------------------------------------
const CWorldPlotProperty& CWorldPlotProperty::operator=(const CWorldPlotProperty& p)
{
  this->Copy(p);
  return *this;
}
//----------------------------------------
CWorldPlotProperty::~CWorldPlotProperty()
{
  DeleteLUT();
}

//----------------------------------------
void CWorldPlotProperty::DeleteLUT()
{
  if (m_LUT != NULL)
  {
    delete m_LUT;
    m_LUT = NULL;
  }
}
//----------------------------------------
void CWorldPlotProperty::Copy(const CWorldPlotProperty& p)
{

  m_coastResolution = p.m_coastResolution;
  m_projection = p.m_projection;

  DeleteLUT();
  m_LUT = new CBratLookupTable(*(p.m_LUT));

  m_title = p.m_title;

  m_name = p.m_name;

  m_opacity = p.m_opacity;
  m_deltaRadius = p.m_deltaRadius;
  m_showPropertyPanel = p.m_showPropertyPanel;
  m_showColorBar = p.m_showColorBar;
  m_showAnimationToolbar = p.m_showAnimationToolbar;

  m_heightFactor = p.m_heightFactor;
  m_numColorLabels = p.m_numColorLabels;

  m_minHeightValue = p.m_minHeightValue;
  m_maxHeightValue = p.m_maxHeightValue;
  m_valueConversion = p.m_valueConversion;
  m_heightConversion = p.m_heightConversion;

  m_centerLongitude = p.m_centerLongitude;
  m_centerLatitude = p.m_centerLatitude;

  m_zoom = p.m_zoom;
  m_zoomLon1 = p.m_zoomLon1;
  m_zoomLon2 = p.m_zoomLon2;
  m_zoomLat1 = p.m_zoomLat1;
  m_zoomLat2 = p.m_zoomLat2;

  m_withContour = p.m_withContour;
  m_withContourLabel = p.m_withContourLabel;
  m_minContourValue = p.m_minContourValue;
  m_maxContourValue = p.m_maxContourValue;
  m_numContour = p.m_numContour;
  m_numContourLabel = p.m_numContourLabel;

  m_contourLabelSize = p.m_contourLabelSize;
  m_contourLineWidth = p.m_contourLineWidth;

  m_contourLineColor = p.m_contourLineColor;
  m_contourLabelColor = p.m_contourLabelColor;

  m_contourLabelFormat = p.m_contourLabelFormat;

  m_solidColor = p.m_solidColor;

  m_withAnimation = p.m_withAnimation;

  m_eastComponent = p.m_eastComponent;
  m_northComponent = p.m_northComponent;


}
