#ifndef BRT_PLOT_EDITOR_H
#define BRT_PLOT_EDITOR_H

#include "new-gui/brat/GUI/DisplayWidgets/3DPlotWidget.h"

#include "QbrtAbstractEditor.h"


class QbrtPlotEditor : public QbrtAbstractEditor< QMainWindow >
{
	Q_OBJECT

	typedef QbrtAbstractEditor< QMainWindow > base_t;


	std::vector<C3DPlotWidget*> mPlots;

    bool m_ToolEditor;

    QString curFile;
    bool isUntitled;

	QSize m_SizeHint = QSize( 72 * fontMetrics().width( 'x' ), 25 * fontMetrics().lineSpacing() );
public:
    QbrtPlotEditor(QWidget *parent = 0);
	virtual ~QbrtPlotEditor();

public:
    const QString& GetFilename() const { return  curFile; }
    bool IsUntitled() const { return  isUntitled; }
    void newFile();
    bool save();
    bool saveAs();
    bool reOpen();
    QSize sizeHint() const override;

    static QbrtPlotEditor *open(QWidget *parent = 0);
    static QbrtPlotEditor *openFile(const QString &fileName,
                            QWidget *parent = 0);
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
    void closeEvent(QCloseEvent *event);            //virtual void contextMenuEvent(QContextMenuEvent *e);
    void  focusInEvent ( QFocusEvent * event );

private slots:
    void documentWasModified();

signals:
	void setCurrentFile( const QbrtPlotEditor *peditor );
    void toolWindowActivated( QbrtPlotEditor *peditor );

private:
    bool saveFile(const QString &fileName);
    void setCurrentFile( const QString &fileName );
    bool readFile(const QString &fileName);
    bool writeFile(const QString &fileName);
};



#endif			// BRT_PLOT_EDITOR_H
