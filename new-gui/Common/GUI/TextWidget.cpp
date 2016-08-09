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
#include "stdafx.h"

#include <QtGui>

#include "new-gui/Common/QtUtilsIO.h"
#include "TextWidget.h"


#if defined (__unix__)
#pragma GCC diagnostic ignored "-Wparentheses"
#endif


//(*) black: if it is decided to change the color, match the MoveTo default color argument
//
CTextWidget::CTextWidget( QWidget *parent ) :		//QWidget *parent = nullptr
	base_t( parent ), 
	mSizeHint( 60 * fontMetrics().width('x'), 10 * fontMetrics().lineSpacing() )
{
	auto sheet = styleSheet();
	setStyleSheet( "selection-color: white; selection-background-color: black; " + sheet );								//(*) //this is the only way of seeing the selection without focus (important in find dialog, for instance)
	//setStyleSheet( "selection-color: white; selection-background-color: black; border: 1px solid black; " + sheet );	//"border: 1px solid black;" was added to test border; can be deleted without affecting selection property

	//set tab width

	const int tabStop = 4;  // 4 characters
	QFontMetrics metrics( font() );
    setTabStopWidth( tabStop * metrics.width( ' ' ) );
}


//////////////////////////////////////////////////////////////////////////
//                          PROPERTIES
//////////////////////////////////////////////////////////////////////////


//virtual 
void CTextWidget::setToolEditor( bool tool )
{
	if ( tool )
		setStyleSheet( "QTextEdit {background: #242424; color: #ffffff;}" );
	else
		setStyleSheet("");
}

//virtual 
void CTextWidget::SetReadOnlyEditor( bool ro )
{
	::SetReadOnlyEditor( this, ro );
}

//virtual 
void CTextWidget::SetHelpProperties( const QString &text, int empty_lines, int spacing, Qt::Alignment alignment, bool wrap )	//alignment = Qt::AlignCenter , bool wrap = false
{
	SetReadOnlyEditor( true );
    setWrapText( wrap );
    setText( text );
    MoveToTop();
    MoveToEnd( true );
    setAlignment( alignment );
	int line, col;
    getPosition( line, col );
    line += empty_lines;
    const int average_line_length = text.length() / line;
    const int height = line * fontMetrics().height() + 2 * spacing;
    SetSizeHint( average_line_length * fontMetrics().width('X') + 2 * spacing , height );
    setMaximumHeight( height );
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ClearSelection();
    MoveToTop();
}




//////////////////////////////////////////////////////////////////////////
//                          CONTEXT MENU
//////////////////////////////////////////////////////////////////////////


//virtual 
void CTextWidget::contextMenuEvent( QContextMenuEvent *event )
{
    static int max_acceptable_selection_size = 2048 * 500;		//avoid huge selections
    static int max_uri_length = 2048;

	//base_t::contextMenuEvent( event );		does not have "Clear" action

	QMenu *menu = createStandardContextMenu();
	menu->addSeparator();

	QAction *action_ContextClear = new QAction( menu );									//make the menu the action's parent, to auto delete it
	action_ContextClear->setObjectName( QString::fromUtf8( "action_ContextClear" ) );
	action_ContextClear->setText( "Clear" );
	action_ContextClear->setEnabled( !isEmpty() );
	menu->addAction( action_ContextClear );

    auto result = menu->exec( event->globalPos() );
	if ( result )
		if ( result == action_ContextClear )
			deleteAll();

	delete menu;
}


//////////////////////////////////////////////////////////////////////////
//                          ACCESS ACTIONS
//////////////////////////////////////////////////////////////////////////


bool CTextWidget::isEmpty() const
{
    return ::isEmpty( this );
}

//"Clears the current selection by setting the anchor to the cursor position.
//Note that it does not delete the text of the selection."
//
void CTextWidget::ClearSelection()
{
    QTextCursor cursor = textCursor();
    cursor.clearSelection();
    setTextCursor( cursor );
}

void CTextWidget::SelectPosition( long long pos, QColor c )    //GlobalColor c = Qt::gray
{
    QList<QTextEdit::ExtraSelection> selections = extraSelections();

    QTextEdit::ExtraSelection selection;
    QTextCharFormat format = selection.format;
    format.setBackground( c );
    selection.format = format;

    QTextCursor cursor = textCursor();
    cursor.setPosition(pos);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    selection.cursor = cursor;

    selections.append(selection);

    setExtraSelections(selections);
}

void CTextWidget::MoveTo( long long position, bool select, QColor c )	//QColor c = Qt::black: match the style sheet in ctor
{
    QTextCursor cursor = textCursor();
    cursor.setPosition( position );
    setTextCursor( cursor );
    if ( select )
        SelectPosition( position, c );	//SelectPosition does NOT move the cursor
}
void CTextWidget::MoveToEnd( bool select )
{
    QTextCursor cursor = textCursor();
	cursor.movePosition( QTextCursor::End, select ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor );
    setTextCursor( cursor );
}


void CTextWidget::MoveTo( const LineCol &lc, bool select, QColor c )			//QColor c = Qt::black: match the style sheet in ctor
{
	int line = std::max( 1, lc.line ), col = std::max( 1, lc.col );
    QTextDocument *doc = document();
    QTextBlock block = doc->begin();
    int off = 0;
    int y=1;

    for( ; y < line; y++ )
    {
        off += block.length();

        if ( block != doc->end() )
            block = block.next();
        else
            return;     //???
    }
    MoveTo( off + col - 1 , select, c );		//column (like line) is 1-based
	LineCol lc_control;
	getPosition( lc_control );
	assert__( lc == lc_control );
}

void CTextWidget::getPosition( int &line, int &col ) const
{
    QTextDocument *doc = document();
    QTextBlock block = doc->begin();
    const int cursorOffset = getPosition();
    int off = 0;
    int len = block.length();
    int otherGuyLine = 1;

    while( cursorOffset >= ( off + len ) )
    {
        off += len;
        block = block.next();
		len = block.length();
		assert( len > 0 );
		assert( block.lineCount() == 1 );
        otherGuyLine += block.lineCount();
		if ( this->document()->characterAt(off) == '\xa' )
			otherGuyLine++;
    }

	QTextBlock b = doc->findBlock( cursorOffset );
	col = cursorOffset - b.position() + 1;
	line = block.blockNumber() + 1;

	assert( ( cursorOffset - off + 1 ) == col );
	assert( block.blockNumber() == block.firstLineNumber() );
	assert( otherGuyLine == line );
}


//////////////////////////////////////////////////////////////////////////
//						COMPLEMENTARY BASIC COMMANDS
//////////////////////////////////////////////////////////////////////////


void CTextWidget::deleteAll()
{
	//pe->clear();			//deletes undo/redo history
	selectAll();
	textCursor().removeSelectedText();
}


//////////////////////////////////////////////////////////////////////////
//                              FILE ACTIONS
//////////////////////////////////////////////////////////////////////////



inline EFileType TypeFromExtension( const QString &fileName )
{
    if ( fileName.endsWith(".txt", Qt::CaseInsensitive)	) 
        return e_txt;

    return e_unknown;
}



//static
bool CTextWidget::loadFileToDocument( const QString &fileName, QTextDocument &doc, EFileType ft, QWidget *pw )    //pw = NULL
{
    static QTextCodec *codec8 = QTextCodec::codecForName( "UTF-8" );

    if ( !IsFile( fileName ) )
        return false;

    QFile file( fileName );
	if ( !file.open( QIODevice::ReadOnly ) ) {				// | QIODevice::Text
		QMessageBox::warning( pw, qApp->applicationName(),
			tr( "Cannot read file %1:\n%2." )
			.arg( file.fileName() )
			.arg( file.errorString() ) );
		return false;
	}
    WaitCursor wait;

    QByteArray data = file.readAll();
	QString content;

    if ( ft == e_txt )	
	{
        content = QString::fromUtf8( data );
		doc.setPlainText( content );
    }
    else
	{
        QTextCodec *codec = QTextCodec::codecForUtfText( data, codec8 );	//default to utf8 if not detected; otherwise "If the codec cannot be detected, this overload returns a Latin-1 QTextCodec."
        content = codec->toUnicode( data );
		doc.setPlainText( content );
    }

    return true;
}



/////////////////////////////////////////////////////////////////////////
// FILE ACTIONS - OPEN/ INPUT
/////////////////////////////////////////////////////////////////////////


//virtual 
bool CTextWidget::readFromFile( const QString &fileName )
{
    QString str;
    EFileType ft = TypeFromExtension( fileName );
    return loadFileToDocument( fileName, *document(), ft, this );
}




//////////////////////////////////////////////////////////////////////////
//						"LAYOUT" ACTIONS
//////////////////////////////////////////////////////////////////////////


//virtual 
QSize CTextWidget::sizeHint() const
{
    return mSizeHint;
}


//////////////////////////////////////////////////////////////////////////
//                            WRAP TEXT
//////////////////////////////////////////////////////////////////////////


void CTextWidget::setWrapText( bool wrap )
{
    if ( wrap ) 
	{
        setLineWrapMode( WidgetWidth );
        setWordWrapMode( QTextOption::WrapAtWordBoundaryOrAnywhere );
    }
    else 
	{
        setLineWrapMode( NoWrap );
        setWordWrapMode( QTextOption::NoWrap );
    }
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_TextWidget.cpp"
