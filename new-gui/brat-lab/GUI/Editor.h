#ifndef EDITOR_H
#define EDITOR_H

#include <cassert>

#include <QTextEdit>
#include "QbrtAbstractEditor.h"
#include <QMessageBox>

#if QT_VERSION >= 0x050000
#include <QPrinter>
#endif



///////////////////////////////////////////////////////////////////
//              Generic QTextEdit Utilities
///////////////////////////////////////////////////////////////////

template< typename TEXT_EDIT >
inline void updateText( TEXT_EDIT *pe, const QString &newData )
{
    assert( pe );

    pe->append( newData );
    pe->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
    pe->ensureCursorVisible();
}

template< typename TEXT_EDIT >
inline void WriteWithColor( TEXT_EDIT *pe, const QString &text, QColor color = Qt::red )
{
    assert( pe );

    QColor c = pe->textColor();
    pe->setTextColor( color );
    updateText( pe, text );       //setPlainText replaces all text
    pe->setTextColor( c );
}

template< typename TEXT_EDIT >
inline QString getSelectedText( TEXT_EDIT *pe )
{
    assert( pe );

    return pe->textCursor().selectedText ();
}

template< typename TEXT_EDIT >
inline bool isEmpty( const TEXT_EDIT *pe )
{
    assert( pe );

    return pe->textCursor().document()->isEmpty();
}




///////////////////////////////////////////////////////////////////
//              QTextEdit SDI/MDI child
///////////////////////////////////////////////////////////////////


class Editor : public QbrtAbstractEditor< QTextEdit >
{
	Q_OBJECT

	typedef QbrtAbstractEditor< QTextEdit > base_t;

    bool m_ToolEditor;

    QString curFile;
    bool isUntitled;

public:
    Editor(QWidget *parent = 0);

    const QString& GetFilename() const { return  curFile; }
    bool IsUntitled() const { return  isUntitled; }
    void newFile();
    bool save();
    bool saveAs();
    bool reOpen();
    QSize sizeHint() const;

    static Editor *open(QWidget *parent = 0);
    static Editor *openFile(const QString &fileName,
                            QWidget *parent = 0);
    bool okToContinue();
    bool isEmpty() const;
    bool isMDIChild() const;        //not implemented
    void ToolEditor( bool tool );
    bool isToolEditor() const { return m_ToolEditor; }
    QString getSelectedText();

    bool selectFont();
    bool selectColor();
    void toBold( bool bold );
    void toItalic( bool italic );
    void toUnderline( bool underline );
    void toFontFamily( const QString &f );
    void toFontSize( const QString &p );
    void toListStyle( QTextListFormat::Style style = QTextListFormat::ListDisc );

    void PrintToPdf();
    void PrintToPrinter();
    void PrintPreview();


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
    void printPreviewSlot(QPrinter *printer);

signals:
	void setCurrentFile( const Editor *peditor );
    void toolWindowActivated( Editor *peditor );

private:
    bool saveFile(const QString &fileName);
    void setCurrentFile( const QString &fileName );
    bool readFile(const QString &fileName);
    bool writeFile(const QString &fileName);

    void mergeFormatOnWordOrSelection( const QTextCharFormat &format );
};

#endif
