#ifndef VIEWS_QGISAPP_MAPTOOLS_H
#define VIEWS_QGISAPP_MAPTOOLS_H


#include <qgsconfig.h>


//21601 vs 20803

#if (VERSION_INT < 21601)

	using map_tool_mouse_event_t = QMouseEvent;


	inline void SelectFeatures( QgsVectorLayer* vlayer, const QgsFeatureIds &newSelectedFeatures )
	{
		vlayer->setSelectedFeatures( newSelectedFeatures );
	}


	inline QString EncodeUnit( QGis::UnitType unit )
	{
		return QGis::toLiteral( unit );
	}


	inline QString MapUnit2String( QGis::UnitType unit )
	{
		return QGis::tr( unit );
	}


	inline QGis::UnitType DecodeDistanceUnit( const QString &qunit )
	{
		return QGis::fromLiteral( qunit );
	}


	inline QGis::UnitType StringToDistanceUnit( const QString &qunit )
	{
		return QGis::fromTr( qunit );
	}


	inline QString FormatDistance( double distance, int decimals, QGis::UnitType unit, bool keepBaseUnit = false )
	{
		return QgsDistanceArea::textUnit( distance, decimals, unit, false, keepBaseUnit );
	}


	inline QString FormatArea( double distance, int decimals, QGis::UnitType unit, bool keepBaseUnit = false )
	{
		return QgsDistanceArea::textUnit( distance, decimals, unit, true, keepBaseUnit );
	}


#else

#include <qgsdistancearea.h>
#include <qgsdatadefined.h>


	using map_tool_mouse_event_t = QgsMapMouseEvent;



	inline void SelectFeatures( QgsVectorLayer* vlayer, const QgsFeatureIds &newSelectedFeatures )
	{
		vlayer->selectByIds( newSelectedFeatures );
	}


	inline QString EncodeUnit( QGis::UnitType unit )
	{
		return QgsUnitTypes::encodeUnit( unit );
	}


	inline QString MapUnit2String( QGis::UnitType unit )
	{
		return QgsUnitTypes::toString( unit );
	}


	inline QGis::UnitType DecodeDistanceUnit( const QString &qunit )
	{
		return QgsUnitTypes::decodeDistanceUnit( qunit );
	}


	inline QGis::UnitType StringToDistanceUnit( const QString &qunit )
	{
		return QgsUnitTypes::stringToDistanceUnit( qunit );
	}


	inline QString FormatDistance( double distance, int decimals, QGis::UnitType unit, bool keepBaseUnit = false )
	{
		return QgsDistanceArea::formatDistance( distance, decimals, unit, keepBaseUnit );
	}


	inline QString FormatArea( double distance, int decimals, QgsUnitTypes::AreaUnit unit, bool keepBaseUnit = false )
	{
		return QgsDistanceArea::formatArea( distance, decimals, unit, keepBaseUnit );
	}



#endif



#endif			//VIEWS_QGISAPP_MAPTOOLS_H
