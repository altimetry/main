/*
* This file is part of BRAT
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#ifndef COMMON_TEXTWIDGET_H
#define COMMON_TEXTWIDGET_H

#include <cassert>

#include <QTextEdit>

#include "new-gui/Common/QtUtils.h"

#include "FindReplaceDialog.h"


#ifdef _MSC_VER
#define toStdWString THIS_IS_AN_ERROR_QT_INCOMPETENTS_IN_WINDOWS___USE_Q2T_INSTEAD
#define fromStdWString THIS_IS_AN_ERROR_QT_INCOMPETENTS_IN_WINDOWS___USE_T2Q_INSTEAD
#endif


///////////////////////////////////////////////////////////////////
//              Generic QTextEdit Utilities
///////////////////////////////////////////////////////////////////
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

enum EFileType 
{
    e_txt,
    e_unknown
};



class CTextWidget : public QTextEdit, public CFindReplaceInterface
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// types

	typedef QTextEdit base_t;
	using find_base_t = CFindReplaceInterface;


	// statics

    static bool loadFileToDocument( const QString &fileName, QTextDocument &doc, EFileType ft, QWidget *pw = NULL );


	// instance data

protected:
	QSize mSizeHint;


	// construction / destruction

public:
    explicit CTextWidget( QWidget *parent = nullptr );

	virtual ~CTextWidget()
	{}

	// 

public:
	// I/O
    
    virtual bool readFromFile( const QString &fileName );

	//properties
    
    bool isEmpty() const;
	virtual void setToolEditor( bool tool );
	virtual void SetReadOnlyEditor( bool ro );
	virtual void SetHelpProperties( const QString &text, int empty_lines, int spacing, Qt::Alignment alignment = Qt::AlignCenter, bool wrap = false );
	virtual void SetFontBold( bool bold );		//should be called after SetMonoFont
	virtual void SetMonoFont( bool mono );		//should be called before SetFontBold
	virtual QSize sizeHint() const override;
    void SetSizeHint( int w, int h ){ mSizeHint = QSize( w, h ); }

	//CFindReplaceInterface	for FindReplaceDialog

	virtual void CopyAvailable( bool yes) override { return copyAvailable( yes ); }
	virtual bool HasSelectedText() const override { return textCursor().hasSelection(); }
	virtual QString SelectedText() override { QString qs;  return getEditorText( this, qs ); }
	virtual bool Find( const QString &toSearch, QTextDocument::FindFlags options ) override { return find( toSearch, options ); }
	virtual bool FindRE( const QRegExp &expr, QTextDocument::FindFlags options ) override 
	{ 
		QTextCursor c = document()->find( expr, textCursor(), options );
		if ( !c.isNull() )		//otherwise the CTextWidget becomes corrupt
			setTextCursor( c );
		return !c.isNull();
	}
	virtual void InsertText( const QString &text ) override { textCursor().insertText( text ); }
	virtual void MoveToFirstLine() override { MoveToTop(); }
	virtual void MoveToTextEnd() override { MoveToEnd( false ); }
	virtual QWidget* EditorWidget() override { return dynamic_cast< base_t* >( this ); }


	//selection / position
    
public:
	//this structure was created to disambiguate calls to MoveTo: it is not enough that 
	//the 1st (long long versus int) and the third (QColor versus bool) parameters differ  
	//
	struct LineCol{ 
		int line, col; 
		explicit LineCol( int l = 1, int c = 1 ): line(l), col(c) {}
		bool operator == ( const LineCol &o ) const { return line == o.line && col == o.col; }
	};

	
    void SelectPosition( long long pos, QColor c = Qt::gray );
	void ClearSelection();
	
	void MoveTo( const LineCol &lc, bool select, QColor c = Qt::black );	//black: match the style sheet in ctor
	void MoveTo( long long position, bool select, QColor c = Qt::black );	//black: match the style sheet in ctor
	void MoveToEnd( bool select );

	//text wrap

	void SetWrapText( bool wrap );
	bool HasWrapText() const;

protected:
	virtual void MoveToTop(){ MoveTo( LineCol(), false ); }

    int  getPosition() const { return textCursor().position(); }
    void getPosition( int &line, int &col ) const;							//line & col are 1-based
    void getPosition( LineCol &lc ) const {	getPosition( lc.line, lc.col ); }

    //complementary basic commands
    
	void deleteAll();

    //text wrap
    
    void setWrapText( bool wrap );

    // menu
    
    virtual void contextMenuEvent( QContextMenuEvent *event ) override;
};





template< class STRING >
inline STRING getAllEditorText( CTextWidget *pe, STRING &s )
{
    pe->ClearSelection();
	s = q2a( pe->toPlainText() );           //.toStdString();
    return s;
}

#endif
