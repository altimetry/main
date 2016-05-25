#include "new-gui/brat/stdafx.h"

#include <QtGui>
#if QT_VERSION >= 0x050000
	#include <QtPrintSupport/QPrintDialog>
	#include <QtPrintSupport/QPrintPreviewDialog>
#endif

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

	//modifications / title management

	connect( document(), SIGNAL(modificationChanged(bool)), this, SLOT(slotModificationChanged(bool)));

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
	//if ( !highLightEnabled() )		//assume console type editor
	//	return;
	//setFont( QFont( "Logic Unicode" ) );
	//setFontFamily( "Logic Unicode" );
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
    static int max_uri_length = 2048;							//http://stackoverflow.com/questions/417142/what-is-the-maximum-length-of-a-url-in-different-browsers

	//base_t::contextMenuEvent( event );		does not have "Clear" action

	QMenu *menu = createStandardContextMenu();
	menu->addSeparator();

	QAction *action_ContextClear = new QAction( menu );									//make the menu the action's parent, to auto delete it
	action_ContextClear->setObjectName( QString::fromUtf8( "action_ContextClear" ) );
	action_ContextClear->setText( "Clear" );
	action_ContextClear->setEnabled( !isEmpty() );
	menu->addAction( action_ContextClear );

//	QPoint cursorPosition( mapFromGlobal( QCursor::pos() ) );
//	QTextDocument* textDocument = document();
//	int textCursorPosition = textDocument->documentLayout()->hitTest( cursorPosition, Qt::FuzzyHit );
//	QChar ch( textDocument->characterAt( textCursorPosition ) );

	QStringList paths;
	size_t paths_size = 0;
	bool enable_open_urls = false;
	if ( SelectionLength() < max_acceptable_selection_size )
	{
	    QString text = GetSelectedText().trimmed();
		paths = text.split( "\n", QString::SkipEmptyParts, Qt::CaseSensitivity::CaseSensitive );
		paths_size = paths.size();
		enable_open_urls = paths_size > 0;
		for ( size_t i = 0; i < paths_size && enable_open_urls; ++i )
		{
			QString path = paths[ (int)i ];
			if ( path.length() > max_uri_length )
				enable_open_urls = false;
			else
			{
				std::string std_path = q2t< std::string >( path );
				//if ( isPortableDataPath( std_path ) )
				//	paths.replace( i, t2q( PortableData2AbsolutePath( std_path ) ) );
			    enable_open_urls = IsFile( paths[ (int)i ] );
			}
		}
	}

	menu->addSeparator();

    QAction *action_openUrl_external = new QAction( menu );								//make the menu the action's parent, to auto delete it
    action_openUrl_external->setObjectName( QString::fromUtf8( "action_openUrl_external" ) );
    action_openUrl_external->setText( "Open in external editor..." );
    menu->addAction( action_openUrl_external );
    action_openUrl_external->setEnabled( enable_open_urls );

	QAction *action_openUrl = nullptr;
	if ( receivers( SIGNAL( openURLsRequest( const QStringList& ) ) ) > 0 )
	{
		action_openUrl = new QAction( menu );											//make the menu the action's parent, to auto delete it
		action_openUrl->setObjectName( QString::fromUtf8( "action_openUrl" ) );
		action_openUrl->setText( "Open in editor..." );
		menu->addAction( action_openUrl );
		action_openUrl->setEnabled( enable_open_urls );
	}

    auto result = menu->exec( event->globalPos() );
	if ( result )
		if ( result == action_ContextClear )
			deleteAll();
		else
		if ( result == action_openUrl_external )
		{
			//examples 
			//		//QDesktopServices::openUrl( QUrl( "file:///home/femm/dev/share/dev/common/data/RuntimeSpirit/data/ATP/TPTP/Problems/GEO/GEO001-2.p.dfg" ) );
			//		//QDesktopServices::openUrl( QUrl( "file:///mnt/vdisks/vwin-booklibrary/Books/SCIENCE/Logic/Computability&Logic/George S. Boolos, John P. Burgess, Richard C. Jeffrey - Computability and Logic (5Ed,Cambridge,2007) (1).pdf" ) );
			//		//QDesktopServices::openUrl( QUrl( "file:///D:/Books/SCIENCE/Logic/Tarski/Tarski_Whatiselementarygeometry.pdf" ) );
			//		//QDesktopServices::openUrl( QUrl( "file:///D:\Books\SCIENCE\Logic\Formalizations\Quaife_Automated-Development-of-Tarski's-Geometry.pdf" ) );

			for ( size_t i = 0; i < paths_size; ++i )
			{
				QString path = paths[ (int)i ];
				if ( path[ 0 ] != '/' )		//Windows case
					path = '/' + path;
				QDesktopServices::openUrl( QUrl( "file://" + path ) );
			}
		}
		else
		if ( result == action_openUrl )
		{
			//QStringList pathList;
			//pathList.append( path );
			emit openURLsRequest( paths );
		}

	delete menu;
}


//////////////////////////////////////////////////////////////////////////
//                          ACCESS ACTIONS
//////////////////////////////////////////////////////////////////////////


bool CTextWidget::isEmpty() const
{
    return ::isEmpty( this );
}

QString CTextWidget::GetSelectedText()
{
    return ::getSelectedText( this );
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

void CTextWidget::Select( QTextCursor::SelectionType selection )
{
    QTextCursor cursor = textCursor();
    cursor.select( selection );
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

//uint TextDisplay::getLineCount()
//{
//	QTextDocument *doc = editor->document();
//	QTextBlock block = doc->begin();
//	int y=1;

//	for( block=doc->begin(); block!= doc->end(); block = block.next())
//		y++;

// 	return y;
//}



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
//                          UPDATE ACTIONS
//////////////////////////////////////////////////////////////////////////


//this is very nice but does not free us from manually updating the actions in the Edit update menu function
//
void CTextWidget::connectSaveAvailableAction( QAction *pa )      {    connect( document(), SIGNAL(modificationChanged(bool)), pa, SLOT(setEnabled(bool)));}

void CTextWidget::connectUndoAvailableAction( QAction *pa )      {    connect( document(), SIGNAL( undoAvailable(bool) ), pa, SLOT( setEnabled(bool) ) );}
void CTextWidget::connectRedoAvailableAction( QAction *pa )      {    connect( document(), SIGNAL( redoAvailable(bool) ), pa, SLOT( setEnabled(bool) ) );}
void CTextWidget::connectCutAvailableAction( QAction *pa )       {    connect( this, SIGNAL( copyAvailable(bool) ), pa, SLOT( setEnabled(bool) ) );}
void CTextWidget::connectCopyAvailableAction( QAction *pa )      {    connect( this, SIGNAL( copyAvailable(bool) ), pa, SLOT( setEnabled(bool) ) );}
void CTextWidget::connectDeleteSelAvailableAction( QAction *pa ) {    connect( this, SIGNAL( copyAvailable(bool) ), pa, SLOT( setEnabled(bool) ) );}


//////////////////////////////////////////////////////////////////////////
//                              FILE ACTIONS
//////////////////////////////////////////////////////////////////////////



inline EFileType TypeFromExtension( const QString &fileName )
{
    if ( fileName.endsWith(".utxt", Qt::CaseInsensitive) )   //Qt does not read, write only option
        return e_utxt;
    if ( fileName.endsWith(".odt", Qt::CaseInsensitive) )   //Qt does not read, write only option
        return e_odt;
    if (    fileName.endsWith(".htm", Qt::CaseInsensitive)
         || fileName.endsWith(".html", Qt::CaseInsensitive)	)
        return e_html;

	if ( fileName.endsWith(".rtf", Qt::CaseInsensitive) )   //mightBeRichText returns false for this (...) and Qt really reads it as text
        return e_rtf;

    if ( fileName.endsWith(".mm", Qt::CaseInsensitive) ||
		fileName.endsWith(".tptp", Qt::CaseInsensitive) ||
		fileName.endsWith(".txt", Qt::CaseInsensitive)
		) 
        return e_txt;

    return e_unknown;
}



//QTextCodecs; use, for instance for UTF-8: QTextCodec::codecForName( "UTF-8" ) 
//Apple Roman
//Big5
//Big5-HKSCS
//CP949
//EUC-JP
//EUC-KR
//GB18030-0
//IBM 850
//IBM 866
//IBM 874
//ISO 2022-JP
//ISO 8859-1 to 10
//ISO 8859-13 to 16
//Iscii-Bng, Dev, Gjr, Knd, Mlm, Ori, Pnj, Tlg, and Tml
//JIS X 0201
//JIS X 0208
//KOI8-R
//KOI8-U
//MuleLao-1
//ROMAN8
//Shift-JIS
//TIS-620
//TSCII
//UTF-8
//UTF-16
//UTF-16BE
//UTF-16LE
//UTF-32
//UTF-32BE
//UTF-32LE
//Windows-1250 to 1258
//WINSAMI2


//static
bool CTextWidget::loadFileToDocument( const QString &fileName, QTextDocument &doc, EFileType ft, QWidget *pw )    //pw = NULL
{
    static QTextCodec *codec8 = QTextCodec::codecForName( "UTF-8" );		//why cannot be const down there???
    static QTextCodec *codec16 = QTextCodec::codecForName( "UTF-16" );		//why cannot be const down there???

    if (!QFile::exists( fileName ))
        return false;

    QFile file( fileName );
    if (!file.open( QIODevice::ReadOnly/* | QIODevice::Text */)) {
        QMessageBox::warning( pw, qApp->applicationName(),
                             tr( "Cannot read file %1:\n%2." )
                             .arg( file.fileName() )
                             .arg( file.errorString() ));
        return false;
    }
    WaitCursor wait;

    //the next 2 lines: the old way of reading plain text the file
    //QTextStream in(&file);
    //setPlainText(in.readAll());

    QByteArray data = file.readAll();
	QString content;

    if ( //Qt::mightBeRichText( str )
        //||
        ft == e_html
        //||
        //ft == e_rtf				//useless; opens as plain text. setAcceptRichText( true ); changes nothing about this
         ) {
        QTextCodec *codec = QTextCodec::codecForHtml( data, codec16 );
		//if ( codec16->name() != codec->name() )	
		//{
			content = codec->toUnicode( data );
			doc.setHtml( content );
		//}
		//else
		//	doc.setHtml( data );
    }
  //  else if ( ft == e_utxt )	{
  //      content = QString::fromUtf16( data );
		//doc.setPlainText( content );
  //  }
    else if ( ft == e_utxt )	{
        QTextCodec *codec = QTextCodec::codecForUtfText( data, codec16 );
		content = codec->toUnicode( data );
		doc.setPlainText( content );
    }
    else if ( ft == e_txt )	{
        //str = QString::fromLocal8Bit( data );
        content = QString::fromUtf8( data );
		doc.setPlainText( content );
    }
    else if ( ft == e_rtf )	{	//also useless; opens as plain text
        QTextCodec *codec = Qt::codecForHtml( data );
        content = codec->toUnicode( data );
		doc.setPlainText( content );
    }
    else{
        QTextCodec *codec = QTextCodec::codecForUtfText( data, codec8 );	//default to utf8 if not detected; otherwise "If the codec cannot be detected, this overload returns a Latin-1 QTextCodec."
        content = codec->toUnicode( data );
		doc.setPlainText( content );
    }

    return true;
}

//static
bool CTextWidget::storeDocumentToFile(const QString &fileName, const QTextDocument &doc, QWidget *pw )	//QWidget *pw = NULL 
{
    //writing, the old way
    //
//    QFile file(fileName);
//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//        QMessageBox::warning(this, tr("MDI CTextWidget"),
//                             tr("Cannot write file %1:\n%2.")
//                             .arg(file.fileName())
//                             .arg(file.errorString()));
//        return false;
//    }

    QTextDocumentWriter writer( fileName );
    bool success;
    {
        WaitCursor wait;

        //writing, the old way
        //
        //QTextStream out(&file);
        //out << toPlainText();
        auto type = TypeFromExtension( fileName );

        switch ( type )
        {
        case e_html:
			//QTextCodec *codec = Qt::codecForHtml( data );
			writer.setCodec( QTextCodec::codecForName( "UTF-16" ) );
            writer.setFormat( "HTML" );
            break;
        case e_odt:
            writer.setFormat( "ODF" );
            break;
        case e_utxt:
			writer.setCodec( QTextCodec::codecForName( "UTF-16" ) );
        case e_txt:
            writer.setFormat( "plaintext" );
            break;
        default:
            writer.setFormat( "plaintext" );
        }
        success = writer.write( &doc );
    }
    if ( !success )
		QMessageBox::warning( pw, qApp->applicationName(),
                             tr("Cannot write file %1:\n%2.")
                             .arg( fileName )
                             .arg( writer.device()->errorString() ) );
    return success;
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




/////////////////////////////////////////////////////////////////////////
// FILE ACTIONS - CLOSE / OUTPUT ACTIONS
/////////////////////////////////////////////////////////////////////////


//virtual
bool CTextWidget::writeToFile( const QString &fileName )
{
	return storeDocumentToFile( fileName, *document(), this );
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
//                              FORMAT ACTIONS
//////////////////////////////////////////////////////////////////////////


void CTextWidget::mergeFormatOnWordOrSelection( const QTextCharFormat &format )
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection())
        cursor.select( QTextCursor::WordUnderCursor );
    cursor.mergeCharFormat( format );
    mergeCurrentCharFormat( format );
}

void CTextWidget::toBold( bool bold )
{
    QTextCharFormat fmt;
    fmt.setFontWeight( bold ? QFont::Bold : QFont::Normal );
    mergeFormatOnWordOrSelection( fmt );
}

void CTextWidget::toItalic( bool italic )
{
    QTextCharFormat fmt;
    fmt.setFontItalic( italic );
    mergeFormatOnWordOrSelection( fmt );
}

void CTextWidget::toUnderline( bool underline )
{
    QTextCharFormat fmt;
    fmt.setFontUnderline( underline );
    mergeFormatOnWordOrSelection( fmt );
}

bool CTextWidget::selectFont()
{
    bool ok;
    QFont f = QFontDialog::getFont( &ok, currentFont(), this ); // or font(), for all text
    if (ok) {
        setCurrentFont( f );                                    // or setFont(), for all text
    }
    return ok;
}

bool CTextWidget::selectColor()
{
    QColor col = QColorDialog::getColor( textColor(), this );
    if ( !col.isValid() )
        return false;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection( fmt );
    return true;
}

void CTextWidget::toFontFamily( const QString &f )
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection( fmt );
}

void CTextWidget::toFontSize( const QString &p )
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection( fmt );
    }
}

void CTextWidget::toListStyle( QTextListFormat::Style style/* = QTextListFormat::ListDisc */)
{
    QTextCursor cursor = textCursor();

    if ( style != QTextListFormat::ListStyleUndefined )
    {
        cursor.beginEditBlock();
        QTextBlockFormat blockFmt = cursor.blockFormat();
        QTextListFormat listFmt;
        if (cursor.currentList())
        {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }
        listFmt.setStyle(style);
        cursor.createList(listFmt);
        cursor.endEditBlock();
    } else {
        // ####
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(-1);
        cursor.mergeBlockFormat(bfmt);
    }
}



//////////////////////////////////////////////////////////////////////////
//                            WRAP TEXT
//////////////////////////////////////////////////////////////////////////


void CTextWidget::setWrapText( bool wrap )
{
    if ( wrap ) {
        setLineWrapMode( WidgetWidth );
        setWordWrapMode( QTextOption::WrapAtWordBoundaryOrAnywhere );
    }
    else {
        setLineWrapMode( NoWrap );
        setWordWrapMode( QTextOption::NoWrap );
    }
}

//////////////////////////////////////////////////////////////////////////
//                              FIND ACTIONS
//////////////////////////////////////////////////////////////////////////



bool CTextWidget::replace( const QString &to_find, const QString &to_replace, QTextDocument::FindFlags ff )
{
    if ( !find (to_find, ff ))
        return false;

    textCursor().insertText( to_replace );
    //find (to_find, ff );                     //position for possible next replacement
    return true;
}

uint CTextWidget::replaceAll( const QString &to_find, const QString &to_replace, QTextDocument::FindFlags ff, bool rec /*= false*/ )
{
    int savedPosition = textCursor().position();
    uint times = 0;
    while ( replace( to_find, to_replace, ff ) )
        ++times;
    if ( !rec )
    {               //not in the recursive call, let's move to replace from the beginning
        MoveTo( 0, false );
        uint before_times = replaceAll( to_find, to_replace, ff, true );
        times += before_times;
        MoveTo( savedPosition + ( before_times * ( to_replace.length() - to_find.length() ) ), false );
        ensureCursorVisible();
    }
    return times;
}




//////////////////////////////////////////////////////////////////////////
//                              PRINT ACTIONS
//////////////////////////////////////////////////////////////////////////


void CTextWidget::PrintToPdf()
{
#ifndef QT_NO_PRINTER
//! [0]
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF", QString(), "*.pdf");
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        document()->print(&printer);
    }
//! [0]
#endif
}

void CTextWidget::PrintToPrinter()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted) {
        print(&printer);
    }
    delete dlg;
#endif
}

void CTextWidget::PrintPreview()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreviewSlot(QPrinter*)));
    preview.exec();
#endif
}

void CTextWidget::printPreviewSlot(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    print(printer);
#endif
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_TextWidget.cpp"
