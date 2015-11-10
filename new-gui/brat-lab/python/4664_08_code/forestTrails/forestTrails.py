import os, os.path, sys
import random

from qgis.core import *
from qgis.gui import *
from qgis.networkanalysis import *
from PyQt4.QtGui import *
from PyQt4.QtCore import *

from ui_mainWindow import Ui_MainWindow

import resources
from constants import *
from mapTools import *

#############################################################################

class ForestTrailsWindow(QMainWindow, Ui_MainWindow):
    def __init__(self):
        QMainWindow.__init__(self)

        self.editing    = False
        self.curStartPt = None
        self.curEndPt   = None

        self.setupUi(self)

        self.connect(self.actionQuit, SIGNAL("triggered()"),
                     self.quit)
        self.connect(self.actionZoomIn, SIGNAL("triggered()"),
                     self.zoomIn)
        self.connect(self.actionZoomOut, SIGNAL("triggered()"),
                     self.zoomOut)
        self.connect(self.actionPan, SIGNAL("triggered()"),
                     self.setPanMode)
        self.connect(self.actionEdit, SIGNAL("triggered()"),
                     self.setEditMode)
        self.connect(self.actionAddTrack, SIGNAL("triggered()"),
                     self.addTrack)
        self.connect(self.actionEditTrack, SIGNAL("triggered()"),
                     self.editTrack)
        self.connect(self.actionDeleteTrack, SIGNAL("triggered()"),
                     self.deleteTrack)
        self.connect(self.actionGetInfo, SIGNAL("triggered()"),
                     self.getInfo)
        self.connect(self.actionSetStartPoint, SIGNAL("triggered()"),
                     self.setStartPoint)
        self.connect(self.actionSetEndPoint, SIGNAL("triggered()"),
                     self.setEndPoint)
        self.connect(self.actionFindShortestPath, SIGNAL("triggered()"),
                     self.findShortestPath)

        self.mapCanvas = QgsMapCanvas()
        self.mapCanvas.useImageToRender(False)
        self.mapCanvas.setCanvasColor(Qt.white)
        self.mapCanvas.show()

        layout = QVBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.addWidget(self.mapCanvas)
        self.centralWidget.setLayout(layout)


    def setupDatabase(self):
        cur_dir = os.path.dirname(os.path.realpath(__file__))
        dbName = os.path.join(cur_dir, "data", "tracks.sqlite")
        if not os.path.exists(dbName):
            fields = QgsFields()
            fields.append(QgsField("id", QVariant.Int))
            fields.append(QgsField("type", QVariant.String))
            fields.append(QgsField("name", QVariant.String))
            fields.append(QgsField("direction", QVariant.String))
            fields.append(QgsField("status", QVariant.String))

            crs = QgsCoordinateReferenceSystem(4326,
                        QgsCoordinateReferenceSystem.EpsgCrsId)

            writer = QgsVectorFileWriter(dbName, 'utf-8', fields,
                                         QGis.WKBLineString,
                                         crs, 'SQLite',
                                         ["SPATIALITE=YES"])

            if writer.hasError() != QgsVectorFileWriter.NoError:
                print "Error creating tracks database!"

            del writer


    def setupMapLayers(self):
        cur_dir = os.path.dirname(os.path.realpath(__file__))
        layers = []

        # Raster base layer.

        filename = os.path.join(cur_dir, "data", "basemap.tif")
        self.baseLayer = QgsRasterLayer(filename, "basemap")
        QgsMapLayerRegistry.instance().addMapLayer(self.baseLayer)
        layers.append(QgsMapCanvasLayer(self.baseLayer))

        # Track layer, stored in an sqlite database.

        uri = QgsDataSourceURI()
        uri.setDatabase(os.path.join(cur_dir, "data", "tracks.sqlite"))
        uri.setDataSource('', 'tracks', 'GEOMETRY')

        self.trackLayer = QgsVectorLayer(uri.uri(), "Tracks", "spatialite")

        QgsMapLayerRegistry.instance().addMapLayer(self.trackLayer)
        layers.append(QgsMapCanvasLayer(self.trackLayer))

        # Shortest path layer, held in memory.

        self.shortestPathLayer = QgsVectorLayer("LineString?crs=EPSG:4326",
                                                "shortestPathLayer", "memory")
        QgsMapLayerRegistry.instance().addMapLayer(self.shortestPathLayer)
        layers.append(QgsMapCanvasLayer(self.shortestPathLayer))

        # Start point layer, held in memory.

        self.startPointLayer = QgsVectorLayer("Point?crs=EPSG:4326",
                                              "startPointLayer", "memory")
        QgsMapLayerRegistry.instance().addMapLayer(self.startPointLayer)
        layers.append(QgsMapCanvasLayer(self.startPointLayer))

        # End point layer, held in memory.

        self.endPointLayer = QgsVectorLayer("Point?crs=EPSG:4326",
                                            "endPointLayer", "memory")
        QgsMapLayerRegistry.instance().addMapLayer(self.endPointLayer)
        layers.append(QgsMapCanvasLayer(self.endPointLayer))

        # Show the map layers.

        layers.reverse() # Vector layers sit in front of basemap.
        self.mapCanvas.setLayerSet(layers)
        self.mapCanvas.setExtent(self.baseLayer.extent())


    def setupRenderers(self):
        """ Create our map renderers.
        """
        # Create a renderer to show the tracks.

        root_rule = QgsRuleBasedRendererV2.Rule(None)

        for track_type in (TRACK_TYPE_ROAD, TRACK_TYPE_WALKING,
                           TRACK_TYPE_BIKE, TRACK_TYPE_HORSE):
            if track_type == TRACK_TYPE_ROAD:
                width = ROAD_WIDTH
            else:
                width = TRAIL_WIDTH

            lineColor = "light gray"
            arrowColor = "dark gray"

            for track_status in (TRACK_STATUS_OPEN, TRACK_STATUS_CLOSED):
                for track_direction in (TRACK_DIRECTION_BOTH,
                                        TRACK_DIRECTION_FORWARD,
                                        TRACK_DIRECTION_BACKWARD):
                    symbol = self.createTrackSymbol(width, lineColor,
                                                    arrowColor,
                                                    track_status,
                                                    track_direction)
                    expression = ("(type='%s') and (status='%s') and " +
                                  "(direction='%s')") % (track_type,
                                                         track_status,
                                                         track_direction)

                    rule = QgsRuleBasedRendererV2.Rule(symbol,
                                                       filterExp=expression)
                    root_rule.appendChild(rule)

        symbol = QgsLineSymbolV2.createSimple({'color' : "black"})
        rule = QgsRuleBasedRendererV2.Rule(symbol, elseRule=True)
        root_rule.appendChild(rule)

        renderer = QgsRuleBasedRendererV2(root_rule)

        self.trackLayer.setRendererV2(renderer)

        # Create a renderer to show the shortest path.

        symbol = QgsLineSymbolV2.createSimple({'color' : "blue"})
        symbol.setWidth(ROAD_WIDTH)
        symbol.setOutputUnit(QgsSymbolV2.MapUnit)
        renderer = QgsSingleSymbolRendererV2(symbol)
        self.shortestPathLayer.setRendererV2(renderer)

        # Create a renderer to show the start point.

        symbol = QgsMarkerSymbolV2.createSimple({'color' : "green"})
        symbol.setSize(POINT_SIZE)
        symbol.setOutputUnit(QgsSymbolV2.MapUnit)
        renderer = QgsSingleSymbolRendererV2(symbol)
        self.startPointLayer.setRendererV2(renderer)

        # Create a renderer to show the end point.

        symbol = QgsMarkerSymbolV2.createSimple({'color' : "red"})
        symbol.setSize(POINT_SIZE)
        symbol.setOutputUnit(QgsSymbolV2.MapUnit)
        renderer = QgsSingleSymbolRendererV2(symbol)
        self.endPointLayer.setRendererV2(renderer)


    def setupMapTools(self):
        self.panTool = PanTool(self.mapCanvas)
        self.panTool.setAction(self.actionPan)

        self.addTrackTool = AddTrackTool(self.mapCanvas,
                                         self.trackLayer,
                                         self.onTrackAdded)
        self.addTrackTool.setAction(self.actionAddTrack)

        self.editTrackTool = EditTrackTool(self.mapCanvas,
                                           self.trackLayer,
                                           self.onTrackEdited)
        self.editTrackTool.setAction(self.actionEditTrack)

        self.deleteTrackTool = DeleteTrackTool(self.mapCanvas,
                                               self.trackLayer,
                                               self.onTrackDeleted)
        self.deleteTrackTool.setAction(self.actionDeleteTrack)

        self.getInfoTool = GetInfoTool(self.mapCanvas,
                                       self.trackLayer,
                                       self.onGetInfo)
        self.getInfoTool.setAction(self.actionGetInfo)

        self.selectStartPointTool = SelectVertexTool(self.mapCanvas,
                                                     self.trackLayer,
                                                     self.onStartPointSelected)

        self.selectEndPointTool = SelectVertexTool(self.mapCanvas,
                                                   self.trackLayer,
                                                   self.onEndPointSelected)


    def closeEvent(self, event):
        self.quit()


    def quit(self):
        if self.editing and self.modified:
            reply = QMessageBox.question(self, "Confirm", "Save Changes?",
                                         QMessageBox.Yes |
                                         QMessageBox.No |
                                         QMessageBox.Cancel,
                                         QMessageBox.Yes)
            if reply == QMessageBox.Yes:
                self.trackLayer.commitChanges()
            elif reply == QMessageBox.No:
                self.trackLayer.rollBack()

            if reply != QMessageBox.Cancel:
                qApp.quit()
        else:
            qApp.quit()


    def zoomIn(self):
        self.mapCanvas.zoomIn()


    def zoomOut(self):
        self.mapCanvas.zoomOut()


    def setPanMode(self):
        self.mapCanvas.setMapTool(self.panTool)

    def setEditMode(self):
        if self.editing:
            if self.modified:
                reply = QMessageBox.question(self, "Confirm",
                                             "Save Changes?",
                                             QMessageBox.Yes |
                                             QMessageBox.No,
                                             QMessageBox.Yes)
                if reply == QMessageBox.Yes:
                    self.trackLayer.commitChanges()
                else:
                    self.trackLayer.rollBack()
            else:
                self.trackLayer.commitChanges()
            self.trackLayer.triggerRepaint()
            self.editing = False
            self.setPanMode()
        else:
            self.trackLayer.startEditing()
            self.trackLayer.triggerRepaint()
            self.editing  = True
            self.modified = False
            self.setPanMode()
        self.adjustActions()


    def addTrack(self):
        if self.actionAddTrack.isChecked():
            self.mapCanvas.setMapTool(self.addTrackTool)
        else:
            self.setPanMode()


    def editTrack(self):
        if self.actionEditTrack.isChecked():
            self.mapCanvas.setMapTool(self.editTrackTool)
        else:
            self.setPanMode()


    def deleteTrack(self):
        if self.actionDeleteTrack.isChecked():
            self.mapCanvas.setMapTool(self.deleteTrackTool)
        else:
            self.setPanMode()


    def getInfo(self):
        self.mapCanvas.setMapTool(self.getInfoTool)


    def setStartPoint(self):
        if self.actionSetStartPoint.isChecked():
            self.mapCanvas.setMapTool(self.selectStartPointTool)
        else:
            self.setPanMode()


    def setEndPoint(self):
        if self.actionSetEndPoint.isChecked():
            self.mapCanvas.setMapTool(self.selectEndPointTool)
        else:
            self.setPanMode()


    def findShortestPath(self):
        if not self.actionFindShortestPath.isChecked():
            self.clearMemoryLayer(self.shortestPathLayer)
            self.setPanMode()
            self.mapCanvas.refresh()
            return

        # Calculate the shorest path between the two points.

        directionField = self.trackLayer.fieldNameIndex("direction")
        director = QgsLineVectorLayerDirector(self.trackLayer,
                                              directionField,
                                              TRACK_DIRECTION_FORWARD,
                                              TRACK_DIRECTION_BACKWARD,
                                              TRACK_DIRECTION_BOTH, 3)

        properter = QgsDistanceArcProperter()
        director.addProperter(properter)

        crs = self.mapCanvas.mapRenderer().destinationCrs()
        builder = QgsGraphBuilder(crs)

        tiedPoints = director.makeGraph(builder, [self.curStartPt,
                                                  self.curEndPt])
        graph = builder.graph()

        startPt = tiedPoints[0]
        endPt   = tiedPoints[1]

        startVertex = graph.findVertex(startPt)
        tree = QgsGraphAnalyzer.shortestTree(graph, startVertex, 0)

        startVertex = tree.findVertex(startPt)
        endVertex   = tree.findVertex(endPt)

        if endVertex == -1:
            QMessageBox.information(self.window,
                                    "Not Found",
                                    "No path found.")
            return

        points = []
        while startVertex != endVertex:
            incomingEdges = tree.vertex(endVertex).inArc()
            if len(incomingEdges) == 0:
                break
            edge = tree.arc(incomingEdges[0])
            points.insert(0, tree.vertex(edge.inVertex()).point())
            endVertex = edge.outVertex()

        points.insert(0, startPt)

        # Finally, add the shortest path as a LineString to the path layer.

        self.clearMemoryLayer(self.shortestPathLayer)

        feature = QgsFeature()
        feature.setGeometry(QgsGeometry.fromPolyline(points))
        self.shortestPathLayer.dataProvider().addFeatures([feature])
        self.shortestPathLayer.updateExtents()
        self.mapCanvas.refresh()


    def createTrackSymbol(self, width, lineColor, arrowColor,
                          status, direction):
        """ Create the symbol to use to draw a track with the given attributes.
        """
        symbol = QgsLineSymbolV2.createSimple({})
        symbol.deleteSymbolLayer(0) # Remove default symbol layer.

        symbolLayer = QgsSimpleLineSymbolLayerV2()
        symbolLayer.setWidth(width)
        symbolLayer.setWidthUnit(QgsSymbolV2.MapUnit)
        symbolLayer.setColor(QColor(lineColor))
        if status == TRACK_STATUS_CLOSED:
            symbolLayer.setPenStyle(Qt.DotLine)
        symbol.appendSymbolLayer(symbolLayer)

        if direction == TRACK_DIRECTION_FORWARD:
            registry = QgsSymbolLayerV2Registry.instance()
            markerLineMetadata = registry.symbolLayerMetadata("MarkerLine")
            markerMetadata     = registry.symbolLayerMetadata("SimpleMarker")

            symbolLayer = markerLineMetadata.createSymbolLayer(
                                {'width': '0.26',
                                 'color': arrowColor,
                                 'rotate': '1',
                                 'placement': 'interval',
                                 'interval' : '20',
                                 'offset': '0'})
            subSymbol = symbolLayer.subSymbol()
            subSymbol.deleteSymbolLayer(0)
            triangle = markerMetadata.createSymbolLayer(
                                {'name': 'filled_arrowhead',
                                 'color': arrowColor,
                                 'color_border': arrowColor,
                                 'offset': '0,0',
                                 'size': '3',
                                 'outline_width': '0.5',
                                 'output_unit': 'mapunit',
                                 'angle': '0'})
            subSymbol.appendSymbolLayer(triangle)

            symbol.appendSymbolLayer(symbolLayer)
        elif direction == TRACK_DIRECTION_BACKWARD:
            registry = QgsSymbolLayerV2Registry.instance()
            markerLineMetadata = registry.symbolLayerMetadata("MarkerLine")
            markerMetadata     = registry.symbolLayerMetadata("SimpleMarker")

            symbolLayer = markerLineMetadata.createSymbolLayer(
                                {'width': '0.26',
                                 'color': arrowColor,
                                 'rotate': '1',
                                 'placement': 'interval',
                                 'interval' : '20',
                                 'offset': '0'})
            subSymbol = symbolLayer.subSymbol()
            subSymbol.deleteSymbolLayer(0)
            triangle = markerMetadata.createSymbolLayer(
                                {'name': 'filled_arrowhead',
                                 'color': arrowColor,
                                 'color_border': arrowColor,
                                 'offset': '0,0',
                                 'size': '3',
                                 'outline_width': '0.5',
                                 'output_unit': 'mapunit',
                                 'angle': '180'})
            subSymbol.appendSymbolLayer(triangle)

            symbol.appendSymbolLayer(symbolLayer)

        return symbol


    def adjustActions(self):
        """ Enable or disable our actions, as appropriate.
        """
        if self.editing:
            self.actionAddTrack.setEnabled(True)
            self.actionEditTrack.setEnabled(True)
            self.actionDeleteTrack.setEnabled(True)
            self.actionGetInfo.setEnabled(True)
            self.actionSetStartPoint.setEnabled(False)
            self.actionSetEndPoint.setEnabled(False)
            self.actionFindShortestPath.setEnabled(False)
        else:
            self.actionAddTrack.setEnabled(False)
            self.actionEditTrack.setEnabled(False)
            self.actionDeleteTrack.setEnabled(False)
            self.actionGetInfo.setEnabled(False)
            self.actionSetStartPoint.setEnabled(True)
            self.actionSetEndPoint.setEnabled(True)
            self.actionFindShortestPath.setEnabled(self.curStartPt != None and
                                                   self.curEndPt != None)

        curTool = self.mapCanvas.mapTool()

        self.actionPan.setChecked(curTool == self.panTool)
        self.actionEdit.setChecked(self.editing)
        self.actionAddTrack.setChecked(curTool == self.addTrackTool)
        self.actionEditTrack.setChecked(curTool == self.editTrackTool)
        self.actionDeleteTrack.setChecked(curTool == self.deleteTrackTool)
        self.actionGetInfo.setChecked(curTool == self.getInfoTool)
        self.actionSetStartPoint.setChecked(
                        curTool == self.selectStartPointTool)
        self.actionSetEndPoint.setChecked(
                        curTool == self.selectEndPointTool)
        self.actionFindShortestPath.setChecked(False)


    def onGetInfo(self, feature):
        """ Display information about the given track.

            This is called by the "GetInfo" tool when the user clicks on a
            track.
        """
        dialog = TrackInfoDialog(self)
        dialog.loadAttributes(feature)
        if dialog.exec_():
            dialog.saveAttributes(feature)
            self.trackLayer.updateFeature(feature)
            self.modified = True
            self.mapCanvas.refresh()


    def onTrackAdded(self):
        """ Respond to the user adding a new track.
        """
        self.modified = True
        self.mapCanvas.refresh()
        self.actionAddTrack.setChecked(False)
        self.setPanMode()


    def onTrackEdited(self):
        """ Respond to the user editing a track.
        """
        self.modified = True
        self.mapCanvas.refresh()


    def onTrackDeleted(self):
        """ Respond to the user deleting a track.
        """
        self.modified = True
        self.mapCanvas.refresh()
        self.actionDeleteTrack.setChecked(False)
        self.setPanMode()


    def onStartPointSelected(self, feature, vertex):
        """ Respond to the user selecting the start point for a path-find.
        """
        self.curStartPt = feature.geometry().vertexAt(vertex)

        self.clearMemoryLayer(self.startPointLayer)

        feature = QgsFeature()
        feature.setGeometry(QgsGeometry.fromPoint(self.curStartPt))
        self.startPointLayer.dataProvider().addFeatures([feature])
        self.startPointLayer.updateExtents()
        self.mapCanvas.refresh()
        self.setPanMode()
        self.adjustActions()


    def onEndPointSelected(self, feature, vertex):
        """ Respond to the user selecting the end point for a path-find.
        """
        self.curEndPt = feature.geometry().vertexAt(vertex)

        self.clearMemoryLayer(self.endPointLayer)

        feature = QgsFeature()
        feature.setGeometry(QgsGeometry.fromPoint(self.curEndPt))
        self.endPointLayer.dataProvider().addFeatures([feature])
        self.endPointLayer.updateExtents()
        self.mapCanvas.refresh()
        self.setPanMode()
        self.adjustActions()


    def clearMemoryLayer(self, layer):
        """ Delete all the features in the given memory-based layer.
        """
        featureIDs = []
        provider = layer.dataProvider()
        for feature in provider.getFeatures(QgsFeatureRequest()):
            featureIDs.append(feature.id())
        provider.deleteFeatures(featureIDs)

#############################################################################

class TrackInfoDialog(QDialog):
    """ Custom dialog to let the user view/edit information about a track.
    """
    def __init__(self, parent=None):
        QDialog.__init__(self, parent)
        self.setWindowTitle("Track Info")

        self.trackTypes = ["Road",
                           "Walking Trail",
                           "Bike Trail",
                           "Horse Trail"]
        self.directions = ["Both",
                           "Forward",
                           "Backward"]
        self.statuses = ["Open",
                         "Closed"]

        self.form = QFormLayout()

        self.trackType = QComboBox(self)
        self.trackType.addItems(self.trackTypes)

        self.trackName = QLineEdit(self)

        self.trackDirection = QComboBox(self)
        self.trackDirection.addItems(self.directions)

        self.trackStatus = QComboBox(self)
        self.trackStatus.addItems(self.statuses)

        self.form.addRow("Type",      self.trackType)
        self.form.addRow("Name",      self.trackName)
        self.form.addRow("Direction", self.trackDirection)
        self.form.addRow("Status",    self.trackStatus)

        self.buttons = QHBoxLayout()

        self.okButton = QPushButton("OK", self)
        self.connect(self.okButton, SIGNAL("clicked()"),
                     self.accept)

        self.cancelButton = QPushButton("Cancel", self)
        self.connect(self.cancelButton, SIGNAL("clicked()"),
                     self.reject)

        self.buttons.addStretch(1)
        self.buttons.addWidget(self.okButton)
        self.buttons.addWidget(self.cancelButton)

        self.layout = QVBoxLayout(self)
        self.layout.addLayout(self.form)
        self.layout.addSpacing(10)

        self.layout.addLayout(self.buttons)
        self.setLayout(self.layout)
        self.resize(self.sizeHint())


    def loadAttributes(self, feature):
        type_attr      = feature.attribute("type")
        name_attr      = feature.attribute("name")
        direction_attr = feature.attribute("direction")
        status_attr    = feature.attribute("status")

        if   type_attr == TRACK_TYPE_ROAD:    index = 0
        elif type_attr == TRACK_TYPE_WALKING: index = 1
        elif type_attr == TRACK_TYPE_BIKE:    index = 2
        elif type_attr == TRACK_TYPE_HORSE:   index = 3
        else:                                 index = 0
        self.trackType.setCurrentIndex(index)

        if name_attr != None:
            self.trackName.setText(name_attr)
        else:
            self.trackName.setText("")

        if   direction_attr == TRACK_DIRECTION_BOTH:     index = 0
        elif direction_attr == TRACK_DIRECTION_FORWARD:  index = 1
        elif direction_attr == TRACK_DIRECTION_BACKWARD: index = 2
        else:                                            index = 0
        self.trackDirection.setCurrentIndex(index)

        if   status_attr == TRACK_STATUS_OPEN:   index = 0
        elif status_attr == TRACK_STATUS_CLOSED: index = 1
        else:                                    index = 0
        self.trackStatus.setCurrentIndex(index)


    def saveAttributes(self, feature):
        index = self.trackType.currentIndex()
        if   index == 0: type_attr = TRACK_TYPE_ROAD
        elif index == 1: type_attr = TRACK_TYPE_WALKING
        elif index == 2: type_attr = TRACK_TYPE_BIKE
        elif index == 3: type_attr = TRACK_TYPE_HORSE
        else:            type_attr = TRACK_TYPE_ROAD

        name_attr = self.trackName.text()

        index = self.trackDirection.currentIndex()
        if   index == 0: direction_attr = TRACK_DIRECTION_BOTH
        elif index == 1: direction_attr = TRACK_DIRECTION_FORWARD
        elif index == 2: direction_attr = TRACK_DIRECTION_BACKWARD
        else:            direction_attr = TRACK_DIRECTION_BOTH

        index = self.trackStatus.currentIndex()
        if   index == 0: status_attr = TRACK_STATUS_OPEN
        elif index == 1: status_attr = TRACK_STATUS_CLOSED
        else:            status_attr = TRACK_STATUS_OPEN

        feature.setAttribute("type",      type_attr)
        feature.setAttribute("name",      name_attr)
        feature.setAttribute("direction", direction_attr)
        feature.setAttribute("status",    status_attr)

#############################################################################

def main():
    QgsApplication.setPrefixPath(os.environ['QGIS_PREFIX'], True)
    QgsApplication.initQgis()

    app = QApplication(sys.argv)

    window = ForestTrailsWindow()
    window.show()
    window.raise_()
    window.setupDatabase()
    window.setupMapLayers()
    window.setupRenderers()
    window.setupMapTools()
    window.setPanMode()
    window.adjustActions()

    app.exec_()
    app.deleteLater()
    QgsApplication.exitQgis()

#############################################################################

if __name__ == "__main__":
    main()

