#ifndef BRT_MAP_CANVAS_H
#define BRT_MAP_CANVAS_H

#include <cassert>

#include <QMessageBox>

#include <qgsmapcanvas.h>
#include <qgssinglesymbolrendererv2.h>

#include "new-gui/brat/Views/MapWidget.h"

#include "QbrtAbstractEditor.h"

class CmdLineProcessor;
class CWorldPlotData;
class QgsMapCanvasLayer;
class QgsSymbolV2;
class QgsSingleSymbolRendererV2;
class QgsFeature;
typedef QList<QgsFeature> QgsFeatureList;
typedef QVector<QgsPoint> QgsPolyline;
class QgsRubberBand;
class QgsRasterLayer;
class QgsLayerTreeView;


class GlobePlugin;
namespace osgEarth { namespace QtGui { class ViewerWidget; } }
namespace osgViewer { class Viewer; } 

class CMapWidget;
class CGlobeWidget;


class CWPlot;


//
// http://foodforsamurai.com/post/483440483/git-to-jira
// https://github.com/joyjit/git-jira-hook
//
// https://sites.google.com/site/midvattenpluginforqgis/download/dependencies
//
// http://doc.qt.io/QtDataVisualization/index.html
// http://plplot.sourceforge.net/index.php
// http://www.qcustomplot.com/
//

class QbrtMapCanvas : public CMapWidget
{
	Q_OBJECT

	typedef CMapWidget base_t;

public:
    QbrtMapCanvas( QWidget *parent = 0 );

	virtual ~QbrtMapCanvas();

    void CreateWPlot( const CmdLineProcessor *proc, CWPlot* wplot );
	void AddData( CWorldPlotData* data );
};





typedef QMainWindow maps_base_t;

class QbrtMapEditor : public QbrtAbstractEditor< maps_base_t >
{
	Q_OBJECT

	typedef QbrtAbstractEditor< maps_base_t > base_t;

	static const int IconSize = 16;

    bool m_ToolEditor;

    QString curFile;
    bool isUntitled;

	QSplitter *mSplitter = nullptr;
	QbrtMapCanvas *mMapCanvas = nullptr;
	//osgEarth::QtGui::ViewerWidget* mGlobeViewerWidget = nullptr;
	//osgViewer::Viewer* mOsgViewer = nullptr;
	//GlobePlugin *mGlobe = nullptr;
	CGlobeWidget *mGlobeWidget = nullptr;

	QAction *mAction_View_Globe = nullptr;
	QProgressBar *mProgressBar = nullptr;
	QWidgetAction *mProgressBarAction = nullptr;
	QAction *mRenderSuppressionAction = nullptr;

	void createToolBar();

    QgsLayerTreeView *mLayerTreeView = nullptr;
    QDockWidget *mLayerTreeDock = nullptr;

	void setupLayerTreeViewFromSettings();
	void createLayerTreeView();
public:
    QbrtMapEditor( QWidget *parent = 0 );

	virtual ~QbrtMapEditor();

	void CreateWPlot( const CmdLineProcessor *proc, CWPlot* wplot );
	void createGlobe();
	QbrtMapCanvas *canvas(){
		return mMapCanvas;
	}
	void globeSettings();

    const QString& GetFilename() const { return  curFile; }
    bool IsUntitled() const { return  isUntitled; }
    void newFile();
    bool save();
    bool saveAs();
    bool reOpen();
    QSize sizeHint() const;

    static QbrtMapEditor *open(QWidget *parent = 0);
    static QbrtMapEditor *openFile(const QString &fileName,
                            QWidget *parent = 0);
    bool okToContinue();
//    bool isEmpty() const;
    bool isMDIChild() const;        //not implemented
    void ToolEditor( bool tool );
    bool isToolEditor() const { return m_ToolEditor; }
//    QString getSelectedText();

    //bool selectFont();
    //bool selectColor();
    //void toBold( bool bold );
    //void toItalic( bool italic );
    //void toUnderline( bool underline );
    //void toFontFamily( const QString &f );
    //void toFontSize( const QString &p );
    //void toListStyle( QTextListFormat::Style style = QTextListFormat::ListDisc );

    //void PrintToPdf();
    //void PrintToPrinter();
    //void PrintPreview();


    static QString strippedName(const QString &fullFileName);

//    void connectSaveAvailableAction( QAction *pa );
    void connectCutAvailableAction( QAction *pa );
    void connectCopyAvailableAction( QAction *pa );
//    void connectUndoAvailableAction( QAction *pa );
//    void connectRedoAvailableAction( QAction *pa );
    void connectDeleteSelAvailableAction( QAction *pa );

protected:
    void closeEvent(QCloseEvent *event);            //virtual void contextMenuEvent(QContextMenuEvent *e);
    void  focusInEvent ( QFocusEvent * event );

private slots:
    void documentWasModified();
//    void printPreviewSlot(QPrinter *printer);

	void canvasRefreshStarted();
	void canvasRefreshFinished();
	void showProgress( int theProgress, int theTotalSteps );
	void action_View_Globe( bool visible );

	void removeLayer();
	void activeLayerChanged( QgsMapLayer* layer );
signals:
	void setCurrentFile( const QbrtMapEditor *peditor );
    void toolWindowActivated( QbrtMapEditor *peditor );

private:
    bool saveFile(const QString &fileName);
    void setCurrentFile( const QString &fileName );
    bool readFile(const QString &fileName);
    bool writeFile(const QString &fileName);

//    void mergeFormatOnWordOrSelection( const QTextCharFormat &format );
};




#endif
