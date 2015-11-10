from PyQt4.QtGui import *
from PyQt4.QtCore import *

import resources

#############################################################################

class Ui_MainWindow(object):
    def setupUi(self, window):
        window.setWindowTitle("Forest Trails")

        self.centralWidget = QWidget(window)
        self.centralWidget.setMinimumSize(800, 400)
        window.setCentralWidget(self.centralWidget)

        self.menubar = window.menuBar()
        self.fileMenu = self.menubar.addMenu("File")
        self.mapMenu = self.menubar.addMenu("Map")
        self.editMenu = self.menubar.addMenu("Edit")
        self.toolsMenu = self.menubar.addMenu("Tools")

        self.toolBar = QToolBar(window)
        window.addToolBar(Qt.TopToolBarArea, self.toolBar)

        self.actionQuit = QAction("Quit", window)
        self.actionQuit.setShortcut(QKeySequence.Quit)

        icon = QIcon(":/resources/mActionZoomIn.png")
        self.actionZoomIn = QAction(icon, "Zoom In", window)
        self.actionZoomIn.setShortcut(QKeySequence.ZoomIn)

        icon = QIcon(":/resources/mActionZoomOut.png")
        self.actionZoomOut = QAction(icon, "Zoom Out", window)
        self.actionZoomOut.setShortcut(QKeySequence.ZoomOut)

        icon = QIcon(":/resources/mActionPan.png")
        self.actionPan = QAction(icon, "Pan", window)
        self.actionPan.setShortcut("Ctrl+1")
        self.actionPan.setCheckable(True)

        icon = QIcon(":/resources/mActionEdit.svg")
        self.actionEdit = QAction(icon, "Edit", window)
        self.actionEdit.setShortcut("Ctrl+2")
        self.actionEdit.setCheckable(True)

        icon = QIcon(":/resources/mActionAddTrack.svg")
        self.actionAddTrack = QAction(icon, "Add Track", window)
        self.actionAddTrack.setShortcut("Ctrl+A")
        self.actionAddTrack.setCheckable(True)

        icon = QIcon(":/resources/mActionEditTrack.png")
        self.actionEditTrack = QAction(icon, "Edit", window)
        self.actionEditTrack.setShortcut("Ctrl+E")
        self.actionEditTrack.setCheckable(True)

        icon = QIcon(":/resources/mActionDeleteTrack.svg")
        self.actionDeleteTrack = QAction(icon, "Delete", window)
        self.actionDeleteTrack.setShortcut("Ctrl+D")
        self.actionDeleteTrack.setCheckable(True)

        icon = QIcon(":/resources/mActionGetInfo.svg")
        self.actionGetInfo = QAction(icon, "Get Info", window)
        self.actionGetInfo.setShortcut("Ctrl+I")
        self.actionGetInfo.setCheckable(True)

        icon = QIcon(":/resources/mActionSetStartPoint.svg")
        self.actionSetStartPoint = QAction(icon, "Set Start Point",
                                           window)
        self.actionSetStartPoint.setCheckable(True)

        icon = QIcon(":/resources/mActionSetEndPoint.svg")
        self.actionSetEndPoint = QAction(icon, "Set End Point", window)
        self.actionSetEndPoint.setCheckable(True)

        icon = QIcon(":/resources/mActionFindShortestPath.svg")
        self.actionFindShortestPath = QAction(icon, "Find Shortest Path",
                                              window)
        self.actionFindShortestPath.setCheckable(True)

        self.fileMenu.addAction(self.actionQuit)

        self.mapMenu.addAction(self.actionZoomIn)
        self.mapMenu.addAction(self.actionZoomOut)
        self.mapMenu.addAction(self.actionPan)
        self.mapMenu.addAction(self.actionEdit)

        self.editMenu.addAction(self.actionAddTrack)
        self.editMenu.addAction(self.actionEditTrack)
        self.editMenu.addAction(self.actionDeleteTrack)
        self.editMenu.addAction(self.actionGetInfo)

        self.toolsMenu.addAction(self.actionSetStartPoint)
        self.toolsMenu.addAction(self.actionSetEndPoint)
        self.toolsMenu.addAction(self.actionFindShortestPath)

        self.toolBar.addAction(self.actionZoomIn)
        self.toolBar.addAction(self.actionZoomOut)
        self.toolBar.addAction(self.actionPan)
        self.toolBar.addAction(self.actionEdit)
        self.toolBar.addSeparator()
        self.toolBar.addAction(self.actionAddTrack)
        self.toolBar.addAction(self.actionEditTrack)
        self.toolBar.addAction(self.actionDeleteTrack)
        self.toolBar.addAction(self.actionGetInfo)
        self.toolBar.addSeparator()
        self.toolBar.addAction(self.actionSetStartPoint)
        self.toolBar.addAction(self.actionSetEndPoint)
        self.toolBar.addAction(self.actionFindShortestPath)

        window.resize(window.sizeHint())

