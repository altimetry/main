#if !defined WORLD_PLOT_PROPERTY_H
#define WORLD_PLOT_PROPERTY_H


#include "libbrathl/BratObject.h"
#include "vtkBratColor.h"


class CBratLookupTable;

//-------------------------------------------------------------
//------------------- CWorldPlotProperty class --------------------
//-------------------------------------------------------------

class CWorldPlotProperty : public CBratObject
{
public:

	CWorldPlotProperty();

	CWorldPlotProperty( const CWorldPlotProperty& p );

	virtual ~CWorldPlotProperty();

	const CWorldPlotProperty& operator=( const CWorldPlotProperty& p );

	std::string m_title;
	std::string m_name;

	std::string m_coastResolution;
	std::string m_projection;
	CBratLookupTable* m_LUT;
	//CDoubleArray m_colorRange;
	double m_opacity;
	double m_deltaRadius;
	bool m_showAnimationToolbar;
	bool m_showPropertyPanel;
	bool m_showColorBar;
	//int32_t m_lineWidth;
	//int32_t m_pointSize;
	//CUIntArray m_color;
	double m_heightFactor;
	double m_minHeightValue;
	double m_maxHeightValue;
	int32_t m_numColorLabels;
	//uint32_t m_stipplepattern;
	double m_valueConversion;
	double m_heightConversion;

	double m_centerLongitude;
	double m_centerLatitude;

	bool m_zoom;
	double m_zoomLon1;
	double m_zoomLon2;
	double m_zoomLat1;
	double m_zoomLat2;

	bool m_withContour;
	bool m_withContourLabel;
	double m_minContourValue;
	double m_maxContourValue;
	int32_t m_numContour;
	int32_t m_numContourLabel;

	int32_t m_contourLabelSize;
	double m_contourLineWidth;

	CVtkColor m_contourLineColor;
	CVtkColor m_contourLabelColor;

	std::string m_contourLabelFormat;

	bool m_solidColor;
	bool m_gridLabel;

	bool m_eastComponent;
	bool m_northComponent;

	bool m_withAnimation;

private:
	void DeleteLUT();

	void Copy( const CWorldPlotProperty& p );
};



#endif		//WORLD_PLOT_PROPERTY_H
