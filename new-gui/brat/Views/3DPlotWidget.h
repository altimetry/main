#ifndef GRAPHICS_3D_PLOT_WIDGET_H
#define GRAPHICS_3D_PLOT_WIDGET_H

#include <qtimer.h>
#include <qwt3d_surfaceplot.h>


typedef Qwt3D::SurfacePlot SurfacePlot; // VC6/moc issue



class Plot : public Qwt3D::SurfacePlot
{
	Q_OBJECT

public:
    Plot(QWidget* pw, int updateinterval = 0 );

public slots:
	void rotate();
};



class Bar;	//enrichment example
class Hat2;	//enrichment example


class C3DPlotWidget : public QWidget
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	using base_t = QWidget;

	void setCentralWidget( QWidget *w );


	std::vector<Plot*> mPlots;

    bool m_ToolEditor;

    QString curFile;
    bool isUntitled;

	QSize m_SizeHint = QSize( 72 * fontMetrics().width( 'x' ), 25 * fontMetrics().lineSpacing() );
	double level_, width_;			//enrichment example
	Bar *bar = nullptr;				//enrichment example
	Hat2* hat = nullptr;			//enrichment example
	QFrame *frame = nullptr;		//enrichment example
	QSlider *levelSlider = nullptr;	//enrichment example
	QSlider *widthSlider = nullptr;	//enrichment example
	void setupUi();					//enrichment example

public:
    C3DPlotWidget(QWidget *parent = 0);
	virtual ~C3DPlotWidget();
	void Autoswitching_axes();
	void Simple_SurfacePlot();
	void setColor();			//enrichment example
	void Vertex_Enrichment();	//enrichment example

public slots:
  void setLevel(int);			//enrichment example
  void setWidth(int);			//enrichment example
  void barSlot();				//enrichment example

public:
    const QString& GetFilename() const { return  curFile; }
    bool IsUntitled() const { return  isUntitled; }
    void newFile();
    bool save();
    bool saveAs();
    bool reOpen();
    QSize sizeHint() const override;

    static C3DPlotWidget *open(QWidget *parent = nullptr );
    static C3DPlotWidget *openFile(const QString &fileName, QWidget *parent = nullptr );
    bool okToContinue();
    bool isEmpty() const;
    bool isMDIChild() const;        //not implemented
    void ToolEditor( bool tool );
    bool isToolEditor() const { return m_ToolEditor; }
    QString getSelectedText();

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

    void connectSaveAvailableAction( QAction *pa );
    void connectCutAvailableAction( QAction *pa );
    void connectCopyAvailableAction( QAction *pa );
    void connectUndoAvailableAction( QAction *pa );
    void connectRedoAvailableAction( QAction *pa );
    void connectDeleteSelAvailableAction( QAction *pa );

protected:
    void closeEvent(QCloseEvent *event) override;            //virtual void contextMenuEvent(QContextMenuEvent *e);
    void  focusInEvent ( QFocusEvent * event ) override;

private slots:
    void documentWasModified();

signals:
	void setCurrentFile( const C3DPlotWidget *peditor );
    void toolWindowActivated( C3DPlotWidget *peditor );

private:
    bool saveFile(const QString &fileName);
    void setCurrentFile( const QString &fileName );
    bool readFile(const QString &fileName);
    bool writeFile(const QString &fileName);
};



#endif			// GRAPHICS_3D_PLOT_WIDGET_H
