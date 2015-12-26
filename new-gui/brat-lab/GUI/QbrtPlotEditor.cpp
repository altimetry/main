#include "new-gui/brat-lab/stdafx.h"

#include <QtGui>
#include <QColor>

#include <qwt3d_plot.h>
#include <qwt3d_function.h>

#include "new-gui/brat-lab/System/BackServices.h"

#include "QbrtPlotEditor.h"


QbrtPlotEditor::~QbrtPlotEditor()
{
}

//////////////////////////////////////////
//
//				The Editor
//
//////////////////////////////////////////


QbrtPlotEditor::QbrtPlotEditor( QWidget *parent ) : base_t( parent ), m_ToolEditor( false )
{
	setWindowIcon( QPixmap( ":/3.png" ) );
	setWindowTitle( "[*]" );
	setAttribute( Qt::WA_DeleteOnClose );
	setCentralWidget( new C3DPlotWidget( this ) );

	//mAction = new QAction(this);
	//mAction->setCheckable(true);
	//connect(mAction, SIGNAL(triggered()), this, SLOT(show()));
	//connect(mAction, SIGNAL(triggered()), this, SLOT(setFocus()));

	isUntitled = true;

	//connect( document(), SIGNAL(modificationChanged(bool)), this, SLOT(setWindowModified(bool)));
	//if ( qobject_cast< QMainWindow* >( parent ) )
	//{
	//    //assert( isMainChild() );
	//    connect( this, SIGNAL( setCurrentFile( const QbrtPlotEditor* )), parent, SLOT(setCurrentFile( const QbrtPlotEditor* )) );
	//}
}


void QbrtPlotEditor::ToolEditor( bool tool )
{
    m_ToolEditor = tool;
    setWindowModified( false );
    disconnect( this, SLOT(setWindowModified(bool)) );
}

//////////////////////////////////////////////////////////////////////////
//                          ACCESS ACTIONS
//////////////////////////////////////////////////////////////////////////


bool QbrtPlotEditor::isEmpty() const
{
	return false;	// ::isEmpty( this );
}

bool QbrtPlotEditor::isMDIChild() const
{
    SimpleMsgBox( "bool QbrtPlotEditor::isMDIChild() const \n Not implemented yet..." );
    return false;
}

QString QbrtPlotEditor::getSelectedText()
{
	return "";		// ::getSelectedText( this );
}


//////////////////////////////////////////////////////////////////////////
//                          UPDATE ACTIONS
//////////////////////////////////////////////////////////////////////////


//this is very nice but does not free us from manually updating the ations in the Edit update menu function
//
//void QbrtPlotEditor::connectSaveAvailableAction( QAction *pa )      {    connect( document(), SIGNAL(modificationChanged(bool)), pa, SLOT(setEnabled(bool)));}
//
//void QbrtPlotEditor::connectUndoAvailableAction( QAction *pa )      {    connect( document(), SIGNAL( undoAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
//void QbrtPlotEditor::connectRedoAvailableAction( QAction *pa )      {    connect( document(), SIGNAL( redoAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void QbrtPlotEditor::connectCutAvailableAction( QAction *pa )       {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void QbrtPlotEditor::connectCopyAvailableAction( QAction *pa )      {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void QbrtPlotEditor::connectDeleteSelAvailableAction( QAction *pa ) {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}

void  QbrtPlotEditor::focusInEvent ( QFocusEvent * event )
{
    base_t::focusInEvent ( event );
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

void QbrtPlotEditor::documentWasModified()
{
    setWindowModified(true);
}

QString QbrtPlotEditor::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


/////////////////////////////////////////////////////////////////////////
// FILE ACTIONS - OPEN/ INPUT
/////////////////////////////////////////////////////////////////////////



void QbrtPlotEditor::setCurrentFile( const QString &fileName )
{
    curFile = fileName;

    isUntitled = false;
    windowMenuAction()->setText(strippedName(curFile));
//    document()->setModified(false);
    setWindowTitle(strippedName(curFile) + "[*]");
    setWindowModified(false);
    emit setCurrentFile( this );
}


void QbrtPlotEditor::newFile()
{
    static int documentNumber = 1;

    curFile = tr("document%1.txt").arg(documentNumber);
    setWindowTitle(curFile + "[*]");
    windowMenuAction()->setText(curFile);
    isUntitled = true;
    ++documentNumber;
}


bool QbrtPlotEditor::readFile( const QString &fileName )
{
    if (!QFile::exists( fileName ))
        return false;


    return true;
}


QbrtPlotEditor *QbrtPlotEditor::openFile(const QString &fileName, QWidget *parent)
{
    QbrtPlotEditor *editor = new QbrtPlotEditor( parent );
    if (editor->readFile( fileName )) {
        editor->setCurrentFile( fileName );
        return editor;
    } else {
        delete editor;
        return 0;
    }
}

bool QbrtPlotEditor::reOpen()
{
    if ( !IsUntitled() && readFile( curFile ) )
        setCurrentFile( curFile );
    else
        return false;
    return true;
}

QbrtPlotEditor *QbrtPlotEditor::open(QWidget *parent)
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

bool QbrtPlotEditor::writeFile( const QString &fileName )
{
    //writing, the old way
    //
//    QFile file(fileName);
//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//        QMessageBox::warning(this, tr("MDI QbrtPlotEditor"),
//                             tr("Cannot write file %1:\n%2.")
//                             .arg(file.fileName())
//                             .arg(file.errorString()));
//        return false;
//    }

    bool success = false;
    return success;
}


bool QbrtPlotEditor::saveFile(const QString &fileName)
{
    if (writeFile(fileName))
    {
        setCurrentFile(fileName);
        return true;
    }
    return false;
}


bool QbrtPlotEditor::saveAs()
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


bool QbrtPlotEditor::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

void QbrtPlotEditor::closeEvent(QCloseEvent *event)
{
    if (okToContinue()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool QbrtPlotEditor::okToContinue()
{
    if (curFile.startsWith(QLatin1String(":/")))
        return true;

    //if ( document()->isModified() ) {
    //    int r = QMessageBox::warning(this, tr("MDI QbrtPlotEditor"),
    //                    tr("File %1 has been modified.\n"
    //                       "Do you want to save your changes?")
    //                    .arg(strippedName(curFile)),
    //                    QMessageBox::Yes | QMessageBox::No
    //                    | QMessageBox::Cancel);
    //    if (r == QMessageBox::Yes) {
    //        return save();
    //    } else if (r == QMessageBox::Cancel) {
    //        return false;
    //    }
    //}
    return true;
}



//////////////////////////////////////////////////////////////////////////
//                              ??? ACTIONS
//////////////////////////////////////////////////////////////////////////


QSize QbrtPlotEditor::sizeHint() const
{
	return m_SizeHint;
    //return QSize(72 * fontMetrics().width('x'),
    //             25 * fontMetrics().lineSpacing());
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_QbrtPlotEditor.cpp"
//#include "moc_Plot.cpp"
