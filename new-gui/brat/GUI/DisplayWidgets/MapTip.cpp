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
#include "new-gui/brat/stdafx.h"

#include "libbrathl/Date.h"

#include "MapWidget.h"
#include "MapTip.h"


void CMapTip::ShowMapTip( QgsMapLayer *layer, QgsPoint &map_position, const QPoint &pixel_position, CMapWidget *map_canvas, QWidget *dest )
{
	// Show the map tip on the canvas
	QString tip_text = FetchFeature( layer, map_position, map_canvas );
	mMapTipVisible = !tip_text.isEmpty();

	if ( mMapTipVisible )
	{
		QToolTip::showText( map_canvas->mapToGlobal( pixel_position ), tip_text, dest );

		// store the point so we can use it to clear the map tip later
		mLastPosition = pixel_position;
	}
}


void CMapTip::Clear( CMapWidget *map_canvas )
{
	if ( !mMapTipVisible )
		return;

	// set the map tip to blank
	QToolTip::showText( map_canvas->mapToGlobal( mLastPosition ), "", map_canvas );
	// reset the visible flag
	mMapTipVisible = false;
}


QString CMapTip::FetchFeature( QgsMapLayer *layer, QgsPoint &map_position, CMapWidget *map_canvas )
{
	QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer*>( layer );
	if ( !vlayer )
		return "";

	//decreases with zoom
	//
	double searchRadius = QgsMapTool::searchRadiusMU( map_canvas ) / 6.;		//6. empirically found

	QgsRectangle r;
	r.setXMinimum( map_position.x() - searchRadius );
	r.setYMinimum( map_position.y() - searchRadius );
	r.setXMaximum( map_position.x() + searchRadius );
	r.setYMaximum( map_position.y() + searchRadius );

	r = map_canvas->mapSettings().mapToLayerCoordinates( layer, r );

	QgsFeature feature;

	if ( !vlayer->getFeatures( QgsFeatureRequest().setFilterRect( r ).setFlags( QgsFeatureRequest::ExactIntersect ) ).nextFeature( feature ) )
		return "";

	int idx = vlayer->fieldNameIndex( vlayer->displayField() );
	if ( idx < 0 )
		return "<empty>";	// QgsExpression::replaceExpressionText( vlayer->displayField(), &feature, vlayer );

	QVariant ref_date_v = vlayer->property( CMapWidget::ReferenceDateFieldKey() );
	QVariant value_factor_v = vlayer->property( CMapWidget::MagnitudeFactorKey() );
	bool isdate = ref_date_v.isValid();
	auto attr = feature.attribute( idx );
	bool ok;
	double value = attr.toDouble( &ok );
    QString tip;
	if ( ok )
	{
		if ( isdate )
		{
			brathl_refDate ref_date = (brathl_refDate)ref_date_v.toInt( &ok );
			if ( ok )
			{
				CDate d( value, ref_date );
				if ( ok && !d.IsDefaultValue() )
				{
					tip = QString( d.AsString().c_str() ) + " (" + attr.toString() + ")";
				}
			}
		}
		else
		{
			if ( value_factor_v.isValid() )
			{
				double value_factor = value_factor_v.toDouble( &ok );
				if ( ok )
					value /= value_factor;
			}
			attr.setValue( value );
			tip = attr.toString();
		}
	}

	return 

#if defined(DEBUG) || defined(_DEBUG)
	"#" + feature.fields()->at(idx).name() + "#" + 
#endif

	tip + " [" + CMapWidget::ExtractLayerBaseName( layer->name() ) + "]";
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_MapTip.cpp"
