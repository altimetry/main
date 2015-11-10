""" mapTools.py

    This module defines the various QgsMapTool subclasses used by the
    "ForestTrails" application.
"""
import math
import time

from qgis.core import *
from qgis.gui import *
from PyQt4.QtGui import *
from PyQt4.QtCore import *

from constants import *

#############################################################################

class MapToolMixin:
    """ Mixin class that defines various helper methods for a QgsMapTool.
    """
    def setLayer(self, layer):
        self.layer        = layer
        self.lastPlayTime = None


    def transformCoordinates(self, screenPt):
        """ Convert a screen coordinate to map and layer coordinates.

            returns a (mapPt,layerPt) tuple.
        """
        return (self.toMapCoordinates(screenPt),
                self.toLayerCoordinates(self.layer, screenPt))
    

    def calcTolerance(self, pos):
        """ Calculate the "tolerance" to use for a mouse-click.

            'pos' is a QPoint object representing the clicked-on point, in
            canvas coordinates.

            The tolerance is the number of map units away from the click
            position that a vertex or geometry can be and we still consider it
            to be a click on that vertex or geometry.
        """
        pt1 = QPoint(pos.x(), pos.y())
        pt2 = QPoint(pos.x() + 10, pos.y())

        mapPt1,layerPt1 = self.transformCoordinates(pt1)
        mapPt2,layerPt2 = self.transformCoordinates(pt2)
        tolerance = layerPt2.x() - layerPt1.x()

        return tolerance


    def findFeatureAt(self, pos, excludeFeature=None):
        """ Find the feature close to the given position.

            'pos' is the position to check, in canvas coordinates.

            if 'excludeFeature' is specified, we ignore this feature when
            finding the clicked-on feature.

            If no feature is close to the given coordinate, we return None.
        """
        mapPt,layerPt = self.transformCoordinates(pos)
        tolerance = self.calcTolerance(pos)
        searchRect = QgsRectangle(layerPt.x() - tolerance,
                                  layerPt.y() - tolerance,
                                  layerPt.x() + tolerance,
                                  layerPt.y() + tolerance)

        request = QgsFeatureRequest()
        request.setFilterRect(searchRect)
        request.setFlags(QgsFeatureRequest.ExactIntersect)

        for feature in self.layer.getFeatures(request):
            if excludeFeature != None:
                if feature.id() == excludeFeature.id():
                    continue
            return feature

        return None


    def findVertexAt(self, feature, pos):
        """ Find the vertex of the given feature close to the given position.

            'feature' is the QgsFeature to check, and 'pos' is the position to
            check, in canvas coordinates.

            We return the vertex number for the closest vertex, or None if no
            vertex is close enough to the given click position.
        """
        mapPt,layerPt = self.transformCoordinates(pos)
        tolerance     = self.calcTolerance(pos)

        vertexCoord,vertex,prevVertex,nextVertex,distSquared = \
            feature.geometry().closestVertex(layerPt)

        distance = math.sqrt(distSquared)
        if distance > tolerance:
            return None
        else:
            return vertex


    def snapToNearestVertex(self, pos, trackLayer, excludeFeature=None):
        """ Attempt to snap the given point to the nearest vertex.

            The parameters are as follows:

                'pos'

                    The click position, in canvas coordinates.

                'trackLayer'

                    The QgsVectorLayer which holds our track data.

                'excludeFeature'

                    If specified, this is a QgsFeature which will be excluded
                    from the check for nearby vertices.  This is used to
                    prevent snapping an object to itself.

            If the click position is close enough to a vertex in the track
            layer (excluding the given feature, if any), we return the
            coordinates of that vertex.  Otherwise, we return the click
            position itself in layer coordinates.  Either way, the returned
            point is in the map tool's layer's coordinates.
        """
        mapPt,layerPt = self.transformCoordinates(pos)
        feature = self.findFeatureAt(pos, excludeFeature)

        if feature == None:
            return layerPt

        vertex = self.findVertexAt(feature, pos)
        if vertex == None:
            return layerPt

        return feature.geometry().vertexAt(vertex)

#############################################################################

class PanTool(QgsMapTool):
    def __init__(self, mapCanvas):
        QgsMapTool.__init__(self, mapCanvas)
        self.setCursor(Qt.OpenHandCursor)
        self.dragging = False

    def canvasMoveEvent(self, event):
        if event.buttons() == Qt.LeftButton:
            self.dragging = True
            self.canvas().panAction(event)

    def canvasReleaseEvent(self, event):
        if event.button() == Qt.LeftButton and self.dragging:
            self.canvas().panActionEnd(event.pos())
            self.dragging = False

#############################################################################

class GetInfoTool(QgsMapTool, MapToolMixin):
    def __init__(self, canvas, layer, onGetInfo):
        QgsMapTool.__init__(self, canvas)
        self.onGetInfo = onGetInfo
        self.setLayer(layer)
        self.setCursor(Qt.WhatsThisCursor)


    def canvasReleaseEvent(self, event):
        if event.button() != Qt.LeftButton: return

        feature = self.findFeatureAt(event.pos())
        if feature != None:
            self.onGetInfo(feature)

#############################################################################

class AddTrackTool(QgsMapTool, MapToolMixin):
    def __init__(self, canvas, layer, onTrackAdded):
        QgsMapTool.__init__(self, canvas)
        self.canvas         = canvas
        self.onTrackAdded   = onTrackAdded
        self.rubberBand     = None
        self.tempRubberBand = None
        self.capturedPoints = []
        self.capturing      = False
        self.setLayer(layer)
        self.setCursor(Qt.CrossCursor)


    def canvasReleaseEvent(self, event):
        if event.button() == Qt.LeftButton:
            if not self.capturing:
                self.startCapturing()
            self.addVertex(event.pos())
        elif event.button() == Qt.RightButton:
            points = self.getCapturedPoints()
            self.stopCapturing()
            if points != None:
                self.pointsCaptured(points)


    def canvasMoveEvent(self, event):
        if self.tempRubberBand != None and self.capturing:
            mapPt,layerPt = self.transformCoordinates(event.pos())
            self.tempRubberBand.movePoint(mapPt)


    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Backspace or event.key() == Qt.Key_Delete:
            self.removeLastVertex()
            event.ignore()
        if event.key() == Qt.Key_Return or event.key() == Qt.Key_Enter:
            points = self.getCapturedPoints()
            self.stopCapturing()
            if points != None:
                self.pointsCaptured(points)


    def startCapturing(self):
        color = QColor("red")
        color.setAlphaF(0.78)

        self.rubberBand = QgsRubberBand(self.canvas, QGis.Line)
        self.rubberBand.setWidth(2)
        self.rubberBand.setColor(color)
        self.rubberBand.show()

        self.tempRubberBand = QgsRubberBand(self.canvas, QGis.Line)
        self.tempRubberBand.setWidth(2)
        self.tempRubberBand.setColor(color)
        self.tempRubberBand.setLineStyle(Qt.DotLine)
        self.tempRubberBand.show()

        self.capturing = True


    def stopCapturing(self):
        if self.rubberBand:
            self.canvas.scene().removeItem(self.rubberBand)
            self.rubberBand = None
        if self.tempRubberBand:
            self.canvas.scene().removeItem(self.tempRubberBand)
            self.tempRubberBand = None
        self.capturing = False
        self.capturedPoints = []
        self.canvas.refresh()


    def addVertex(self, canvasPoint):
        snappedPt = self.snapToNearestVertex(canvasPoint, self.layer)
        mapPt = self.toMapCoordinates(self.layer, snappedPt)

        self.rubberBand.addPoint(mapPt)
        self.capturedPoints.append(snappedPt)

        self.tempRubberBand.reset(QGis.Line)
        self.tempRubberBand.addPoint(mapPt)


    def removeLastVertex(self):
        if not self.capturing: return

        bandSize     = self.rubberBand.numberOfVertices()
        tempBandSize = self.tempRubberBand.numberOfVertices()
        numPoints    = len(self.capturedPoints)

        if bandSize < 1 or numPoints < 1:
            return

        self.rubberBand.removePoint(-1)

        if bandSize > 1:
            if tempBandSize > 1:
                point = self.rubberBand.getPoint(0, bandSize-2)
                self.tempRubberBand.movePoint(tempBandSize-2, point)
        else:
            self.tempRubberBand.reset(QGis.Line)

        del self.capturedPoints[-1]


    def getCapturedPoints(self):
        points = self.capturedPoints
        if len(points) < 2:
            return None
        else:
            return points


    def pointsCaptured(self, points):
        print "in pointsCaptured"
        fields = self.layer.dataProvider().fields()

        feature = QgsFeature()
        feature.setGeometry(QgsGeometry.fromPolyline(points))
        feature.setFields(fields)
        feature.setAttribute("type",      TRACK_TYPE_ROAD)
        feature.setAttribute("status",    TRACK_STATUS_OPEN)
        feature.setAttribute("direction", TRACK_DIRECTION_BOTH)

        self.layer.addFeature(feature)
        self.layer.updateExtents()
        self.onTrackAdded()

#############################################################################

class EditTrackTool(QgsMapTool, MapToolMixin):
    def __init__(self, canvas, layer, onTrackEdited):
        QgsMapTool.__init__(self, canvas)
        self.onTrackEdited = onTrackEdited
        self.dragging      = False
        self.feature       = None
        self.vertex        = None
        self.setLayer(layer)
        self.setCursor(Qt.CrossCursor)


    def canvasPressEvent(self, event):
        """ Respond to the mouse button being pressed.
        """
        feature = self.findFeatureAt(event.pos())
        if feature == None:
            return

        vertex = self.findVertexAt(feature, event.pos())
        if vertex == None: return

        if event.button() == Qt.LeftButton:
            # Left click -> move vertex.
            self.dragging = True
            self.feature  = feature
            self.vertex   = vertex
            self.moveVertexTo(event.pos())
            self.canvas().refresh()
        elif event.button() == Qt.RightButton:
            # Right click -> delete vertex.
            self.deleteVertex(feature, vertex)
            self.canvas().refresh()


    def canvasMoveEvent(self, event):
        if self.dragging:
            self.moveVertexTo(event.pos())
            self.canvas().refresh()


    def canvasReleaseEvent(self, event):
        if self.dragging:
            self.moveVertexTo(event.pos())
            self.layer.updateExtents()
            self.canvas().refresh()
            self.dragging = False
            self.feature  = None
            self.vertex   = None


    def canvasDoubleClickEvent(self, event):
        feature = self.findFeatureAt(event.pos())
        if feature == None:
            return

        mapPt,layerPt = self.transformCoordinates(event.pos())
        geometry      = feature.geometry()

        distSquared,closestPt,beforeVertex = \
            geometry.closestSegmentWithContext(layerPt)

        distance = math.sqrt(distSquared)
        tolerance = self.calcTolerance(event.pos())
        if distance > tolerance: return

        geometry.insertVertex(closestPt.x(), closestPt.y(), beforeVertex)
        self.layer.changeGeometry(feature.id(), geometry)
        self.onTrackEdited()
        self.canvas().refresh()


    def moveVertexTo(self, pos):
        """ Move the edited vertex to the given position.

            'pos' is in canvas coordinates.
        """
        snappedPt = self.snapToNearestVertex(pos, self.layer, self.feature)

        geometry = self.feature.geometry()
        layerPt = self.toLayerCoordinates(self.layer, pos)
        geometry.moveVertex(snappedPt.x(), snappedPt.y(), self.vertex)
        self.layer.changeGeometry(self.feature.id(), geometry)
        self.onTrackEdited()


    def deleteVertex(self, feature, vertex):
        """ Delete the given vertex from the given feature's geometry.
        """
        geometry = feature.geometry()

        lineString = geometry.asPolyline()
        if len(lineString) <= 2:
            return

        if geometry.deleteVertex(vertex):
            self.layer.changeGeometry(feature.id(), geometry)
            self.onTrackEdited()

#############################################################################

class DeleteTrackTool(QgsMapTool, MapToolMixin):
    def __init__(self, canvas, layer, onTrackDeleted):
        QgsMapTool.__init__(self, canvas)
        self.onTrackDeleted = onTrackDeleted
        self.feature        = None
        self.setLayer(layer)
        self.setCursor(Qt.CrossCursor)


    def canvasPressEvent(self, event):
        self.feature = self.findFeatureAt(event.pos())


    def canvasReleaseEvent(self, event):
        feature = self.findFeatureAt(event.pos())
        if feature != None and feature.id() == self.feature.id():
            self.layer.deleteFeature(self.feature.id())
            self.onTrackDeleted()

#############################################################################

class SelectVertexTool(QgsMapTool, MapToolMixin):
    """ Map tool to let user select a vertex.

        We use this to let the user select a starting or ending point for the
        shortest route calculator.
    """
    def __init__(self, canvas, trackLayer, onVertexSelected):
        QgsMapTool.__init__(self, canvas)
        self.onVertexSelected = onVertexSelected
        self.setLayer(trackLayer)
        self.setCursor(Qt.CrossCursor)


    def canvasReleaseEvent(self, event):
        feature = self.findFeatureAt(event.pos())
        if feature != None:
            vertex = self.findVertexAt(feature, event.pos())
            if vertex != None:
                self.onVertexSelected(feature, vertex)

