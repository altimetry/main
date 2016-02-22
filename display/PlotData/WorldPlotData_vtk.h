/*
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
#ifndef PLOTDATA_WORLDPLOT_DATA_VTK_H
#define PLOTDATA_WORLDPLOT_DATA_VTK_H


#include "libbrathl/brathl.h"

#include "WorldPlotCommonData_vtk.h"



class VTK_CWorldPlotData : public VTK_CWorldPlotCommonData
{
	using base_t = VTK_CWorldPlotCommonData;

protected:


	vtkObArray m_geoMapFilterList;


	vtkObArray m_labelPts;
	vtkObArray m_labelIds;
	vtkObArray m_labelData;

	VTK_CWorldPlotData( CPlotField* northField, CPlotField* eastField )
		: base_t( northField, eastField )
	{}
public:
	VTK_CWorldPlotData( CPlotField* field )
		: base_t( field )
	{
		Create( &( field->m_internalFiles ), field->m_name );
	}

	virtual ~VTK_CWorldPlotData()
	{}

protected:
	virtual void Create( CObArray* data, const std::string& fieldName );

public:

	virtual void OnKeyframeChanged( uint32_t i ) override;

	virtual void Update() override;

	vtkObArray* GetGeoMapFilterList() { return &m_geoMapFilterList; }

	virtual void SetProjection( int32_t proj ) override;



	virtual void SetContour3D() override;
	virtual void SetContour2D() override;

	void SetContourLabelProperties();
	void ContourGenerateValues();
	void SetContour3DProperties();
	void SetContour2DProperties();

	virtual void CreateContourLabels() override;
	static void FindVisibleSpherePoints( void* arg );
	virtual void UpdateContourLabels() override;

	virtual void CreateContourLabels2D() override;
	virtual void UpdateContourLabels2D() override;

	static void FindVisiblePlanePoints( void* arg );

	void SetContourLabels2DPosition();

};




//-------------------------------------------------------------
//------------------- VTK_CWorldPlotVelocityData class --------------------
//-------------------------------------------------------------

class VTK_CWorldPlotVelocityData : public VTK_CWorldPlotData
{
	using base_t = VTK_CWorldPlotData;

protected:

   double m_minhvN;
   double m_maxhvN;

   double m_minhvE;
   double m_maxhvE;

   bool IsGlyph;
   double zoom;


   vtkProj2DFilter *m_simple2DProjFilter;  // this could be of any implementation
   vtkVelocityGlyphFilter *m_geoMapFilter;

   // for glyphs
   vtkMaskPoints*   m_pointMask;
   vtkArrowSource* m_arrowSource;
   vtkBratArrowSource* m_barrowSource;
   vtkGlyph3D*  m_glyph;

public:

  VTK_CWorldPlotVelocityData( CPlotField* northField, CPlotField* eastField );

  virtual ~VTK_CWorldPlotVelocityData();

protected:
	void Create(CObArray* northData, CObArray* eastData, const std::string& fieldNameNorth, const std::string& fieldNameEast);

public:
  vtkVelocityGlyphFilter * GetGlyphFilter() { return m_geoMapFilter; }

  void SetIsGlyph(bool val);

  void SetZoom(double z) { zoom =z; }

protected:
   vtkProj2DFilter* GetVtkFilter();

   virtual void Set2D() override;
   virtual void Set3D() override;

   virtual void Set2DGlyphs() override;
   virtual void Set3DGlyphs() override;

};


#endif		//PLOTDATA_WORLDPLOT_DATA_VTK_H
