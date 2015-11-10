#ifndef QGSAPPLAYERTREEVIEWMENUPROVIDER_H
#define QGSAPPLAYERTREEVIEWMENUPROVIDER_H

#include <QObject>

#include "qgslayertreeview.h"
#include "qgsmaplayer.h"

class QAction;
class QbrtMapEditor;

struct LegendLayerAction
{
	LegendLayerAction( QAction* a, QString m, QString i, bool all )
		: action( a ), menu( m ), id( i ), allLayers( all ) {}
	QAction* action;
	QString menu;
	QString id;
	bool allLayers;
	QList<QgsMapLayer*> layers;
};

class QgsMapCanvas;

class QgsAppLayerTreeViewMenuProvider : public QObject, public QgsLayerTreeViewMenuProvider
{
	Q_OBJECT

	QgsLayerTreeView* mView = nullptr;
	QbrtMapEditor *mEditor = nullptr;

	QMap< QgsMapLayer::LayerType, QList< LegendLayerAction > > mLegendLayerActionMap;

public:
	QgsAppLayerTreeViewMenuProvider( QgsLayerTreeView* view, QbrtMapEditor *editor );

	QMenu* createContextMenu() override;

	void addLegendLayerAction( QAction* action, QString menu, QString id,
		QgsMapLayer::LayerType type, bool allLayers );
	bool removeLegendLayerAction( QAction* action );
	void addLegendLayerActionForLayer( QAction* action, QgsMapLayer* layer );
	void removeLegendLayerActionsForLayer( QgsMapLayer* layer );
	QList< LegendLayerAction > legendLayerActions( QgsMapLayer::LayerType type ) const;

protected:

	void addCustomLayerActions( QMenu* menu, QgsMapLayer* layer );
};

#endif // QGSAPPLAYERTREEVIEWMENUPROVIDER_H
