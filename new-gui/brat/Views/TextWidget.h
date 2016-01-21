#ifndef EDITOR_H
#define EDITOR_H

#include <cassert>

#include <QTextEdit>
#include <qfilesystemwatcher.h>
#include <qapplication.h>
#if QT_VERSION >= 0x050000
#include <QtPrintSupport/QPrinter>
#endif

#include "new-gui/Common/QtUtils.h"

 #ifdef _MSC_VER
#define toStdWString THIS_IS_AN_ERROR_QT_INCOMPETENTS_IN_WINDOWS___USE_Q2T_INSTEAD
#define fromStdWString THIS_IS_AN_ERROR_QT_INCOMPETENTS_IN_WINDOWS___USE_T2Q_INSTEAD
#endif


///////////////////////////////////////////////////////////////////
//              Generic QTextEdit Utilities
///////////////////////////////////////////////////////////////////

template< typename TEXT_EDIT >
inline void replaceSelection( TEXT_EDIT *pe, const QString &newData )
{
    assert( pe );

    pe->textCursor().insertText( newData );
    pe->ensureCursorVisible();
}

template< typename TEXT_EDIT >
inline void updateText( TEXT_EDIT *pe, const QString &newData )
{
    assert( pe );

    pe->append( newData );
    pe->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
    pe->ensureCursorVisible();
}

template< typename TEXT_EDIT >
inline void WriteWithColor( TEXT_EDIT *pe, const QString &text, QColor color = Qt::red, bool replace = false )
{
    assert( pe );

    QColor c = pe->textColor();
    pe->setTextColor( color );
    if ( replace )
		pe->setText( text );
	else
		updateText( pe, text );       //setPlainText replaces all text
    pe->setTextColor( c );
}

template< typename TEXT_EDIT >
inline void ReplaceWithColor( TEXT_EDIT *pe, const QString &text, QColor color = Qt::red )
{
    WriteWithColor( pe, text, color, true );
}

template< typename TEXT_EDIT >
inline QString getSelectedText( TEXT_EDIT *pe )
{
    assert( pe );

    return pe->textCursor().selectedText().replace( QChar( L'\x2029' ), "\n" );
}

template< typename TEXT_EDIT >
inline bool isEmpty( const TEXT_EDIT *pe )
{
    assert( pe );

    return pe->textCursor().document()->isEmpty();
}



///////////////////////////////////////////////
/// q2X conversions - in QtUtils.h
///////////////////////////////////////////////

///////////////////////////////////////////////
/// X2q conversions - in QtUtils.h
///////////////////////////////////////////////

///////////////////////////////////////////////
/// get a/w/q text from editors
///////////////////////////////////////////////

inline std::string& getUTF8EditorText( QTextEdit *pe, std::string &s )
{
    if ( pe->textCursor().hasSelection() )
		s += q2a( getSelectedText( pe ).toUtf8() );
    else
        s += q2a( pe->toPlainText().toUtf8() );
    return s;
}

inline std::wstring& getUTF8EditorText( QTextEdit *pe, std::wstring &ws )
{
    if ( pe->textCursor().hasSelection() )
        ws += q2w( getSelectedText( pe ).toUtf8() );
    else
        ws += q2w( pe->toPlainText().toUtf8() );
    return ws;
}

inline std::string getEditorText( QTextEdit *pe, std::string &s )
{
    if ( pe->textCursor().hasSelection() )
        s = q2a( getSelectedText( pe ) );       //.toStdString();
    else
        s = q2a( pe->toPlainText() );           //.toStdString();
    return s;
}

inline std::wstring getEditorText( QTextEdit *pe, std::wstring &ws )
{
    if ( pe->textCursor().hasSelection() )
        ws = q2w( getSelectedText( pe ) );    //.toStdWString();
    else
        ws = q2w( pe->toPlainText() );        //.toStdWString();
    return ws;
}

inline QString getEditorText( QTextEdit *pe, QString &qs )
{
    if ( pe->textCursor().hasSelection() )
        qs = getSelectedText( pe );
    else
        qs = pe->toPlainText();
    return qs;
}



///////////////////////////////////////////////////////////////////
//					 QTextEdit child
///////////////////////////////////////////////////////////////////

enum EFileType {
    e_txt,
    e_utxt,			 //experimental... (2014/10)
    e_html,
    e_odt,
    e_rtf,
    e_unknown
};



class CTextWidget : public QTextEdit
{
	Q_OBJECT

public:
	typedef QTextEdit base_t;

	friend class TextEditor;

protected:
	QSize m_SizeHint;

public:
    CTextWidget( QWidget *parent = nullptr );

    //Match AbstractEditor overrides (see also in signals and slots)
    //
	virtual void setModified( bool modified ) { document()->setModified( modified ); }
protected:
	virtual void MoveToTop(){ MoveTo( LineCol(), false ); }
    virtual bool writeToFile( const QString &fileName );
public:
    virtual bool readFromFile( const QString &fileName );							//turned public in widget, protected in editor
	virtual bool isModified() const { return document()->isModified(); }
    virtual bool isUndoAvailable() const { return document()->isUndoAvailable(); }
    virtual bool isRedoAvailable() const { return document()->isRedoAvailable(); }

public:
    //file
    //
    static bool loadFileToDocument( const QString &fileName, QTextDocument &doc, EFileType ft, QWidget *pw = NULL );
    static bool storeDocumentToFile( const QString &fileName, const QTextDocument &doc, QWidget *pw = NULL );
protected:

public:
	//properties
    //
    bool isEmpty() const;
	virtual void setToolEditor( bool tool );
	virtual void setHelpEditor( bool help );
	virtual void setHelpProperties( const QString &text, int spacing, Qt::Alignment alignment = Qt::AlignCenter );
    virtual QSize sizeHint() const;
    void setSizeHint( int w, int h ){ m_SizeHint = QSize( w, h ); }

    //selection / position
    //

	//this structure was created to disambiguate calls to MoveTo: it is not enough that 
	//the 1st (long long versus int) and the third (QColor versus bool) parameters differ  
	//
	struct LineCol{ 
		int line, col; 
		LineCol( int l = 1, int c = 1 ):line(l), col(c){}
		bool operator == ( const LineCol &o ) const { return line == o.line && col == o.col; }
	};

	bool hasSelection() const { return textCursor().hasSelection(); }
	QString GetSelectedText();
    int SelectionLength() const { return textCursor().selectionEnd() - textCursor().selectionStart(); }
    void Select( QTextCursor::SelectionType selection );
    void SelectPosition( long long pos, QColor c = Qt::gray );
    void ClearSelection();
	void MoveTo( const LineCol &lc, bool select, QColor c = Qt::black );	//black: match the style sheet in ctor
	void MoveTo( long long position, bool select, QColor c = Qt::black );	//black: match the style sheet in ctor
	void MoveToEnd( bool select );
    int  getPosition() const { return textCursor().position(); }
    void getPosition( int &line, int &col ) const;
    void getPosition( LineCol &lc ) const {	getPosition( lc.line, lc.col ); }
	int position() const { return textCursor().position(); }

    //complementary basic commands
    //
	void deleteAll();
	void removeSelectedText() { textCursor().removeSelectedText(); }        //from Qt source code

    //format
    //
    bool selectFont();
    bool selectColor();
    void toBold( bool bold );
    void toItalic( bool italic );
    void toUnderline( bool underline );
    void toFontFamily( const QString &f );
    void toFontSize( const QString &p );
    void toListStyle( QTextListFormat::Style style = QTextListFormat::ListDisc );

private:
    void mergeFormatOnWordOrSelection( const QTextCharFormat &format );

public:

    //text wrap
    //
    void setWrapText( bool wrap );

    //find/replace
    //
    bool replace( const QString &to_find, const QString &to_replace, QTextDocument::FindFlags ff );
    uint replaceAll( const QString &to_find, const QString &to_replace, QTextDocument::FindFlags ff, bool rec = false );

	void PrintToPdf();
    void PrintToPrinter();
    void PrintPreview();

    //update actions
    //
    void connectSaveAvailableAction( QAction *pa );
    void connectCutAvailableAction( QAction *pa );
    void connectCopyAvailableAction( QAction *pa );
    void connectUndoAvailableAction( QAction *pa );
    void connectRedoAvailableAction( QAction *pa );
    void connectDeleteSelAvailableAction( QAction *pa );

protected:
	virtual void contextMenuEvent( QContextMenuEvent *event );

protected slots:
	void slotModificationChanged ( bool changed ){ emit modificationChanged ( changed ); }			  //to relay document() signal as common editors signal

private slots:
    void printPreviewSlot(QPrinter *printer);

signals:
	//	these signals are virtual, but the moc tool says: "warning : Signals cannot be declared virtual", so... 
	//
	void modificationChanged ( bool changed );
    //void toolWindowActivated( EditorBase *peditor );	
    //void currentFileWasSet( EditorBase *peditor );

    void openURLsRequest( const QStringList &pathList );
};





template< class STRING >
inline STRING getAllEditorText( CTextWidget *pe, STRING &s )
{
    pe->ClearSelection();
    return getEditorText( pe, s );
}

#endif
