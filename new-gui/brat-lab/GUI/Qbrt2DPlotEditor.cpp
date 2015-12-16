#include "new-gui/brat-lab/stdafx.h"


#include <qprinter.h>					//spectogram
#if QT_VERSION >= 0x040000
#include <qprintdialog.h>				//spectogram
#endif
#include <qwt_color_map.h>				//spectogram
#include <qwt_plot_spectrogram.h>		//spectogram
#include <qwt_scale_widget.h>			//spectogram
#include <qwt_scale_draw.h>				//spectogram
#include <qwt_plot_zoomer.h>			//spectogram
#include <qwt_plot_panner.h>			//spectogram
#include <qwt_plot_layout.h>			//spectogram



#include "Qbrt2DPlotEditor.h"

#include "../System/BackServices.h"



class MyZoomer: public QwtPlotZoomer
{
public:
    MyZoomer(QwtPlotCanvas *canvas):
        QwtPlotZoomer(canvas)
    {
        setTrackerMode(AlwaysOn);
    }

    virtual QwtText trackerText(const QwtDoublePoint &pos) const
    {
        QColor bg(Qt::white);
#if QT_VERSION >= 0x040300
        bg.setAlpha(200);
#endif

        QwtText text = QwtPlotZoomer::trackerText(pos);
        text.setBackgroundBrush( QBrush( bg ));
        return text;
    }
};

class SpectrogramData: public QwtRasterData
{
public:
    SpectrogramData():
        QwtRasterData(QwtDoubleRect(-1.5, -1.5, 3.0, 3.0))
    {
    }

    virtual QwtRasterData *copy() const
    {
        return new SpectrogramData();
    }

    virtual QwtDoubleInterval range() const
    {
        return QwtDoubleInterval(0.0, 10.0);
    }

    virtual double value(double x, double y) const
    {
        const double c = 0.842;

        const double v1 = x * x + (y-c) * (y+c);
        const double v2 = x * (y+c) + x * (y+c);

        return 1.0 / (v1 * v1 + v2 * v2);
    }
};

Plot2D::Plot2D(QWidget *parent):
    QwtPlot(parent)
{
    d_spectrogram = new QwtPlotSpectrogram();

    QwtLinearColorMap colorMap(Qt::darkCyan, Qt::red);
    colorMap.addColorStop(0.1, Qt::cyan);
    colorMap.addColorStop(0.6, Qt::green);
    colorMap.addColorStop(0.95, Qt::yellow);

    d_spectrogram->setColorMap(colorMap);

    d_spectrogram->setData(SpectrogramData());
    d_spectrogram->attach(this);

    QwtValueList contourLevels;
    for ( double level = 0.5; level < 10.0; level += 1.0 )
        contourLevels += level;
    d_spectrogram->setContourLevels(contourLevels);

    // A color bar on the right axis
    QwtScaleWidget *rightAxis = axisWidget(QwtPlot::yRight);
    rightAxis->setTitle("Intensity");
    rightAxis->setColorBarEnabled(true);
    rightAxis->setColorMap(d_spectrogram->data().range(),
        d_spectrogram->colorMap());

    setAxisScale(QwtPlot::yRight,
        d_spectrogram->data().range().minValue(),
        d_spectrogram->data().range().maxValue() );
    enableAxis(QwtPlot::yRight);

    plotLayout()->setAlignCanvasToScales(true);
    replot();

    // LeftButton for the zooming
    // MidButton for the panning
    // RightButton: zoom out by 1
    // Ctrl+RighButton: zoom out to full size

    QwtPlotZoomer* zoomer = new MyZoomer(canvas());
#if QT_VERSION < 0x040000
    zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlButton);
#else
    zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier);
#endif
    zoomer->setMousePattern(QwtEventPattern::MouseSelect3,
        Qt::RightButton);

    QwtPlotPanner *panner = new QwtPlotPanner(canvas());
    panner->setAxisEnabled(QwtPlot::yRight, false);
    panner->setMouseButton(Qt::MidButton);

    // Avoid jumping when labels with more/less digits
    // appear/disappear when scrolling vertically

    const QFontMetrics fm(axisWidget(QwtPlot::yLeft)->font());
    QwtScaleDraw *sd = axisScaleDraw(QwtPlot::yLeft);
    sd->setMinimumExtent( fm.width("100.00") );

    const QColor c(Qt::darkBlue);
    zoomer->setRubberBandPen(c);
    zoomer->setTrackerPen(c);
}

void Plot2D::showContour(bool on)
{
    d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode, on);
    replot();
}

void Plot2D::showSpectrogram(bool on)
{
    d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ImageMode, on);
    d_spectrogram->setDefaultContourPen(on ? QPen() : QPen(Qt::NoPen));
    replot();
}

void Plot2D::printPlot()
{
    QPrinter printer;
    printer.setOrientation(QPrinter::Landscape);
#if QT_VERSION < 0x040000
    printer.setColorMode(QPrinter::Color);
#if 0
    printer.setOutputFileName("/tmp/spectrogram.ps");
#endif
    if (printer.setup())
#else
#if 0
    printer.setOutputFileName("/tmp/spectrogram.pdf");
#endif
    QPrintDialog dialog(&printer);
    if ( dialog.exec() )
#endif
    {
        print(printer);
    }
}



/////////////////////////////////////////////////////////////////////////////////
//									EDITOR
/////////////////////////////////////////////////////////////////////////////////

Qbrt2DPlotEditor::~Qbrt2DPlotEditor()
{}


enum Plot2dExample
{
	e_Spectogram, e_Plot2dExample_first = e_Spectogram,

	e_Plot2dExample_size
};

Plot2dExample& operator ++( Plot2dExample &ex )
{
	int exi = static_cast<int>( ex ) + 1;

	if ( exi >= e_Plot2dExample_size )
		ex = e_Plot2dExample_first;
	else
		ex = static_cast<Plot2dExample>( exi );

	return ex;// static_cast<Plot2dExample>( static_cast<int>(ex)+ 1 );
}

void Qbrt2DPlotEditor::Spectogram()
{
    d_plot = new Plot2D(this);

    setCentralWidget(d_plot);

    QToolBar *toolBar = new QToolBar(this);

    QToolButton *btnSpectrogram = new QToolButton(toolBar);
    QToolButton *btnContour = new QToolButton(toolBar);
    QToolButton *btnPrint = new QToolButton(toolBar);

#if QT_VERSION >= 0x040000
    btnSpectrogram->setText("Spectrogram");
    //btnSpectrogram->setIcon(QIcon());
    btnSpectrogram->setCheckable(true);
    btnSpectrogram->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(btnSpectrogram);

    btnContour->setText("Contour");
    //btnContour->setIcon(QIcon());
    btnContour->setCheckable(true);
    btnContour->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(btnContour);

    btnPrint->setText("Print");
    btnPrint->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(btnPrint);
#else
    btnSpectrogram->setTextLabel("Spectrogram");
    //btnSpectrogram->setPixmap(zoom_xpm);
    btnSpectrogram->setToggleButton(true);
    btnSpectrogram->setUsesTextLabel(true);

    btnContour->setTextLabel("Contour");
    //btnContour->setPixmap(zoom_xpm);
    btnContour->setToggleButton(true);
    btnContour->setUsesTextLabel(true);

    btnPrint->setTextLabel("Print");
    btnPrint->setUsesTextLabel(true);
#endif

    addToolBar(toolBar);

    connect(btnSpectrogram, SIGNAL(toggled(bool)), 
        d_plot, SLOT(showSpectrogram(bool)));
    connect(btnContour, SIGNAL(toggled(bool)), 
        d_plot, SLOT(showContour(bool)));
    connect(btnPrint, SIGNAL(clicked()), 
        d_plot, SLOT(printPlot()) );

#if QT_VERSION >= 0x040000
    btnSpectrogram->setChecked(true);
    btnContour->setChecked(false);
#else
    btnSpectrogram->setOn(true);
    btnContour->setOn(false);
#endif
}



Qbrt2DPlotEditor::Qbrt2DPlotEditor( QWidget *parent ) : base_t( parent ), m_ToolEditor( false )
{
	static Plot2dExample ex = e_Plot2dExample_size;

	setWindowIcon( QPixmap( ":/2.png" ) );
	setWindowTitle( "[*]" );
	setAttribute( Qt::WA_DeleteOnClose );

	switch ( ++ex )
	{
		case e_Spectogram:
				Spectogram();
			break;

		default:
			throw "2D error";
	}

	//mAction = new QAction(this);
	//mAction->setCheckable(true);
	//connect(mAction, SIGNAL(triggered()), this, SLOT(show()));
	//connect(mAction, SIGNAL(triggered()), this, SLOT(setFocus()));

	isUntitled = true;

	//connect( document(), SIGNAL(modificationChanged(bool)), this, SLOT(setWindowModified(bool)));
	//if ( qobject_cast< QMainWindow* >( parent ) )
	//{
	//    //assert( isMainChild() );
	//    connect( this, SIGNAL( setCurrentFile( const Qbrt2DPlotEditor* )), parent, SLOT(setCurrentFile( const Qbrt2DPlotEditor* )) );
	//}
}


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
