#include "new-gui/brat-lab/stdafx.h"

#include "Qbrt2DPlotEditor.h"

#include "../System/BackServices.h"


Qbrt2DPlotEditor::Qbrt2DPlotEditor( QWidget *parent ) : base_t( parent ), m_ToolEditor( false )
{
    static E2DPlotType ex = e2DPlotType_size;

	setWindowIcon( QPixmap( ":/2.png" ) );
	setWindowTitle( "[*]" );
	setAttribute( Qt::WA_DeleteOnClose );

	switch ( ++ex )
	{
		case eSpectogram:
            d_plot = new C2DPlotWidget( this );
			setCentralWidget( d_plot );
			break;

		default:
			throw "2D error";
	}

	isUntitled = true;
}

Qbrt2DPlotEditor::~Qbrt2DPlotEditor()
{}

void Qbrt2DPlotEditor::ToolEditor( bool tool )
{
    m_ToolEditor = tool;
    setWindowModified( false );
    disconnect( this, SLOT(setWindowModified(bool)) );
}




//////////////////////////////////////////////////////////////////////////
//                          ACCESS ACTIONS
//////////////////////////////////////////////////////////////////////////


bool Qbrt2DPlotEditor::isEmpty() const
{
	return false;	// ::isEmpty( this );
}

bool Qbrt2DPlotEditor::isMDIChild() const
{
    SimpleMsgBox( "bool Qbrt2DPlotEditor::isMDIChild() const \n Not implemented yet..." );
    return false;
}

QString Qbrt2DPlotEditor::getSelectedText()
{
	return "";		// ::getSelectedText( this );
}


//////////////////////////////////////////////////////////////////////////
//                          UPDATE ACTIONS
//////////////////////////////////////////////////////////////////////////


//this is very nice but does not free us from manually updating the ations in the Edit update menu function
//
//void Qbrt2DPlotEditor::connectSaveAvailableAction( QAction *pa )      {    connect( document(), SIGNAL(modificationChanged(bool)), pa, SLOT(setEnabled(bool)));}
//
//void Qbrt2DPlotEditor::connectUndoAvailableAction( QAction *pa )      {    connect( document(), SIGNAL( undoAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
//void Qbrt2DPlotEditor::connectRedoAvailableAction( QAction *pa )      {    connect( document(), SIGNAL( redoAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void Qbrt2DPlotEditor::connectCutAvailableAction( QAction *pa )       {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void Qbrt2DPlotEditor::connectCopyAvailableAction( QAction *pa )      {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}
void Qbrt2DPlotEditor::connectDeleteSelAvailableAction( QAction *pa ) {    connect( this, SIGNAL( copyAvailable(bool)), pa, SLOT(setEnabled(bool)) );}

void  Qbrt2DPlotEditor::focusInEvent ( QFocusEvent * event )
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

void Qbrt2DPlotEditor::documentWasModified()
{
    setWindowModified(true);
}

QString Qbrt2DPlotEditor::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


/////////////////////////////////////////////////////////////////////////
// FILE ACTIONS - OPEN/ INPUT
/////////////////////////////////////////////////////////////////////////



void Qbrt2DPlotEditor::setCurrentFile( const QString &fileName )
{
    curFile = fileName;

    isUntitled = false;
    windowMenuAction()->setText(strippedName(curFile));
//    document()->setModified(false);
    setWindowTitle(strippedName(curFile) + "[*]");
    setWindowModified(false);
    emit setCurrentFile( this );
}


void Qbrt2DPlotEditor::newFile()
{
    static int documentNumber = 1;

    curFile = tr("document%1.txt").arg(documentNumber);
    setWindowTitle(curFile + "[*]");
    windowMenuAction()->setText(curFile);
    isUntitled = true;
    ++documentNumber;
}


bool Qbrt2DPlotEditor::readFile( const QString &fileName )
{
    if (!QFile::exists( fileName ))
        return false;


    return true;
}


Qbrt2DPlotEditor *Qbrt2DPlotEditor::openFile(const QString &fileName, QWidget *parent)
{
    Qbrt2DPlotEditor *editor = new Qbrt2DPlotEditor( parent );
    if (editor->readFile( fileName )) {
        editor->setCurrentFile( fileName );
        return editor;
    } else {
        delete editor;
        return 0;
    }
}

bool Qbrt2DPlotEditor::reOpen()
{
    if ( !IsUntitled() && readFile( curFile ) )
        setCurrentFile( curFile );
    else
        return false;
    return true;
}

Qbrt2DPlotEditor *Qbrt2DPlotEditor::open(QWidget *parent)
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

bool Qbrt2DPlotEditor::writeFile( const QString &fileName )
{
    //writing, the old way
    //
//    QFile file(fileName);
//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//        QMessageBox::warning(this, tr("MDI Qbrt2DPlotEditor"),
//                             tr("Cannot write file %1:\n%2.")
//                             .arg(file.fileName())
//                             .arg(file.errorString()));
//        return false;
//    }

    bool success = false;
    return success;
}


bool Qbrt2DPlotEditor::saveFile(const QString &fileName)
{
    if (writeFile(fileName))
    {
        setCurrentFile(fileName);
        return true;
    }
    return false;
}


bool Qbrt2DPlotEditor::saveAs()
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


bool Qbrt2DPlotEditor::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

void Qbrt2DPlotEditor::closeEvent(QCloseEvent *event)
{
    if (okToContinue()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool Qbrt2DPlotEditor::okToContinue()
{
    if (curFile.startsWith(QLatin1String(":/")))
        return true;

    //if ( document()->isModified() ) {
    //    int r = QMessageBox::warning(this, tr("MDI Qbrt2DPlotEditor"),
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


QSize Qbrt2DPlotEditor::sizeHint() const
{
	return m_SizeHint;
    //return QSize(72 * fontMetrics().width('x'),
    //             25 * fontMetrics().lineSpacing());
}







///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_Qbrt2DPlotEditor.cpp"
