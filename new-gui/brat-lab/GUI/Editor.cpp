#include "new-gui/brat-lab/stdafx.h"

#include <QtGui>
#if QT_VERSION >= 0x050000
    #include <QPrintDialog>
    #include <QPrintPreviewDialog>
#endif


#include "Editor.h"
#include "new-gui/brat-lab/System/BackServices.h"


Editor::Editor(QWidget *parent)
    : base_t(parent), m_ToolEditor( false )
{
    //mAction = new QAction(this);
    //mAction->setCheckable(true);
    //connect(mAction, SIGNAL(triggered()), this, SLOT(show()));
    //connect(mAction, SIGNAL(triggered()), this, SLOT(setFocus()));

    isUntitled = true;

    connect( document(), SIGNAL(modificationChanged(bool)), this, SLOT(setWindowModified(bool)));
    if ( qobject_cast< QMainWindow* >( parent ) )
    {
        //assert( isMainChild() );
        connect( this, SIGNAL( setCurrentFile( const Editor* )), parent, SLOT(setCurrentFile( const Editor* )) );
    }

    setWindowIcon(QPixmap(":/images/document.png"));
    setWindowTitle("[*]");
    setAttribute(Qt::WA_DeleteOnClose);
}


void Editor::ToolEditor( bool tool )
{
    m_ToolEditor = tool;
    setWindowModified( false );
    disconnect( this, SLOT(setWindowModified(bool)) );
}

//////////////////////////////////////////////////////////////////////////
//                          ACCESS ACTIONS
//////////////////////////////////////////////////////////////////////////


bool Editor::isEmpty() const
{
    return ::isEmpty( this );
}

bool Editor::isMDIChild() const
{
    SimpleMsgBox( "bool Editor::isMDIChild() const \n Not implemented yet..." );
    return false;
}

QString Editor::getSelectedText()
{
    return ::getSelectedText( this );
}


//////////////////////////////////////////////////////////////////////////
//                          UPDATE ACTIONS
//////////////////////////////////////////////////////////////////////////


//this is very nice but does not free us from manually updating the ations in the Edit update menu function
//
void Editor::connectSaveAvailableAction( QAction *pa )      {    connect( document(), SIGNAL(modificationChanged(bool)), pa, SLOT(setEnabled(bool)));}

void Editor::connectUndoAvailableAction( QAction *pa )      {    connect( document(), SIGNAL( undoAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void Editor::connectRedoAvailableAction( QAction *pa )      {    connect( document(), SIGNAL( redoAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void Editor::connectCutAvailableAction( QAction *pa )       {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void Editor::connectCopyAvailableAction( QAction *pa )      {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void Editor::connectDeleteSelAvailableAction( QAction *pa ) {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}

void  Editor::focusInEvent ( QFocusEvent * event )
{
    QTextEdit::focusInEvent ( event );
    //if ( m_ToolEditor )
        emit toolWindowActivated( this );
}

//////////////////////////////////////////////////////////////////////////
//                              FILE ACTIONS
//////////////////////////////////////////////////////////////////////////


enum EFileType {
    e_txt,
    e_html,
    e_odt,
    e_unknown
};

inline EFileType TypeFromExtension( const QString &fileName )
{
    if ( fileName.endsWith(".odt", Qt::CaseInsensitive) )       //Qt does not read, write only option
        return e_odt;
    if (    fileName.endsWith(".htm", Qt::CaseInsensitive)
         || fileName.endsWith(".html", Qt::CaseInsensitive)
         || fileName.endsWith(".rtf", Qt::CaseInsensitive)      //mightBeRichText returns false for this (...) and Qt really reads it as text
         )
        return e_html;
    if ( fileName.endsWith(".txt", Qt::CaseInsensitive) )       //default on write
        return e_txt;
    return e_unknown;
}

void Editor::documentWasModified()
{
    setWindowModified(true);
}

QString Editor::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


/////////////////////////////////////////////////////////////////////////
// FILE ACTIONS - OPEN/ INPUT
/////////////////////////////////////////////////////////////////////////



void Editor::setCurrentFile( const QString &fileName )
{
    curFile = fileName;

    isUntitled = false;
    windowMenuAction()->setText(strippedName(curFile));
    document()->setModified(false);
    setWindowTitle(strippedName(curFile) + "[*]");
    setWindowModified(false);
    emit setCurrentFile( this );
}


void Editor::newFile()
{
    static int documentNumber = 1;

    curFile = tr("document%1.txt").arg(documentNumber);
    setWindowTitle(curFile + "[*]");
    windowMenuAction()->setText(curFile);
    isUntitled = true;
    ++documentNumber;
}


bool Editor::readFile( const QString &fileName )
{
    if (!QFile::exists( fileName ))
        return false;

    QFile file( fileName );
    if (!file.open( QIODevice::ReadOnly/* | QIODevice::Text */)) {
        QMessageBox::warning(this, tr("MDI Editor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }
    WaitCursor wait;

    //the next 2 lines: the old wat of reading plain text the file
    //QTextStream in(&file);
    //setPlainText(in.readAll());

    QByteArray data = file.readAll();
    QTextCodec *codec = Qt::codecForHtml( data );
    QString str = codec->toUnicode( data );
    if ( Qt::mightBeRichText( str ) || TypeFromExtension( fileName ) == e_html  //|| TypeFromExtension( fileName ) == e_odt
         )
    {
        setHtml( str );
    } else {
        str = QString::fromLocal8Bit( data );
        setPlainText( str );
    }

    return true;
}


Editor *Editor::openFile(const QString &fileName, QWidget *parent)
{
    Editor *editor = new Editor( parent );
    if (editor->readFile( fileName )) {
        editor->setCurrentFile( fileName );
        return editor;
    } else {
        delete editor;
        return 0;
    }
}

bool Editor::reOpen()
{
    if ( !IsUntitled() && readFile( curFile ) )
        setCurrentFile( curFile );
    else
        return false;
    return true;
}

Editor *Editor::open(QWidget *parent)
{
    static const TBackServices &m_bs = GetBackServices();

    QString fileName = QFileDialog::getOpenFileName( parent, tr("Open"), m_bs.GetDataPath(),
                                                     tr("Expressions (*.mm *.txt);;HTML-Files (*.htm *.html);;All Files (*)") );
    if (fileName.isEmpty())
        return 0;

    return openFile(fileName, parent);
}


/////////////////////////////////////////////////////////////////////////
// FILE ACTIONS - CLOSE / OUTPUT ACTIONS
/////////////////////////////////////////////////////////////////////////

bool Editor::writeFile( const QString &fileName )
{
    //writing, the old way
    //
//    QFile file(fileName);
//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//        QMessageBox::warning(this, tr("MDI Editor"),
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

        if ( TypeFromExtension( fileName ) == e_html )
            writer.setFormat( "HTML" );
        else
        if ( TypeFromExtension( fileName ) == e_odt )
            writer.setFormat( "ODF" );
        else
        if ( TypeFromExtension( fileName ) == e_txt )
            writer.setFormat( "plaintext" );
        else
            writer.setFormat( "plaintext" );

        success = writer.write( document() );
    }
    if ( !success )
        QMessageBox::warning(this, tr("MDI Editor"),
                             tr("Cannot write file %1:\n%2.")
                             .arg( fileName )
                             .arg( writer.device()->errorString() ) );
    return success;
}


bool Editor::saveFile(const QString &fileName)
{
    if (writeFile(fileName))
    {
        setCurrentFile(fileName);
        return true;
    }
    return false;
}


bool Editor::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As..."), curFile,
                                                    tr("Expressions (*.mm *.txt);;HTML-Files (*.htm *.html);;All Files (*);;ODF files (*.odt)"));
    if ( fileName.isEmpty() )
        return false;

    if ( TypeFromExtension( fileName ) == e_unknown )
        fileName += ".txt"; // default
        //fileName += ".odt"; // default

    return saveFile( fileName );
}


bool Editor::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

void Editor::closeEvent(QCloseEvent *event)
{
    if (okToContinue()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool Editor::okToContinue()
{
    if (curFile.startsWith(QLatin1String(":/")))
        return true;

    if ( document()->isModified() ) {
        int r = QMessageBox::warning(this, tr("MDI Editor"),
                        tr("File %1 has been modified.\n"
                           "Do you want to save your changes?")
                        .arg(strippedName(curFile)),
                        QMessageBox::Yes | QMessageBox::No
                        | QMessageBox::Cancel);
        if (r == QMessageBox::Yes) {
            return save();
        } else if (r == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}



//////////////////////////////////////////////////////////////////////////
//                              ??? ACTIONS
//////////////////////////////////////////////////////////////////////////


QSize Editor::sizeHint() const
{
    return QSize(72 * fontMetrics().width('x'),
                 25 * fontMetrics().lineSpacing());
}



//////////////////////////////////////////////////////////////////////////
//                              FORMAT ACTIONS
//////////////////////////////////////////////////////////////////////////


void Editor::mergeFormatOnWordOrSelection( const QTextCharFormat &format )
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection())
        cursor.select( QTextCursor::WordUnderCursor );
    cursor.mergeCharFormat( format );
    mergeCurrentCharFormat( format );
}

void Editor::toBold( bool bold )
{
    QTextCharFormat fmt;
    fmt.setFontWeight( bold ? QFont::Bold : QFont::Normal );
    mergeFormatOnWordOrSelection( fmt );
}

void Editor::toItalic( bool italic )
{
    QTextCharFormat fmt;
    fmt.setFontItalic( italic );
    mergeFormatOnWordOrSelection( fmt );
}

void Editor::toUnderline( bool underline )
{
    QTextCharFormat fmt;
    fmt.setFontUnderline( underline );
    mergeFormatOnWordOrSelection( fmt );
}

bool Editor::selectFont()
{
    bool ok;
    QFont f = QFontDialog::getFont( &ok, currentFont(), this ); // or font(), for all text
    if (ok) {
        setCurrentFont( f );                                    // or setFont(), for all text
    }
    return ok;
}

bool Editor::selectColor()
{
    QColor col = QColorDialog::getColor( textColor(), this );
    if ( !col.isValid() )
        return false;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection( fmt );
    return true;
}

void Editor::toFontFamily( const QString &f )
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection( fmt );
}

void Editor::toFontSize( const QString &p )
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection( fmt );
    }
}

void Editor::toListStyle( QTextListFormat::Style style/* = QTextListFormat::ListDisc */)
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
//                              PRINT ACTIONS
//////////////////////////////////////////////////////////////////////////


void Editor::PrintToPdf()
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

void Editor::PrintToPrinter()
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

void Editor::PrintPreview()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreviewSlot(QPrinter*)));
    preview.exec();
#endif
}

void Editor::printPreviewSlot(QPrinter *printer)
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

#include "moc_Editor.cpp"
