#include "stdafx.h"

#include "new-gui/Common/tools/Trace.h"

#include "new-gui/brat/DataModels/DisplayFilesProcessor.h"
#include "new-gui/brat/DataModels/PlotData/WorldPlot.h"

#include "QbrtMainWindow.h"
#include "GUI/QbrtMapEditor.h"
#include "GUI/QbrtPlotEditor.h"
#include "GUI/Qbrt2DPlotEditor.h"



/////////////////////////////////////////////////////////////////////////
//                      OPEN/ INPUT ACTIONS
/////////////////////////////////////////////////////////////////////////

void QbrtMainWindow::setCurrentFile( const Editor *peditor )
{
    QString curFile = peditor->GetFilename();
    QString shownName;

    if ( !peditor->IsUntitled() )
    {
        shownName = QFileInfo( curFile ).fileName();
        //
        // Most recent files logic
        //
        m_recentFiles.removeAll( curFile );
        m_recentFiles.prepend( curFile );
                updateRecentFileActions();
    }

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Rich Text")));
    setWindowModified(false);
}


void QbrtMainWindow::addEditor( Editor *editor )
{
    connectAutoUpdateEditActions( editor );

    // MDI sub-windows logic
    //
    QMdiSubWindow *subWindow = mdiArea->addSubWindow( editor );
    menu_Window->addAction( editor->windowMenuAction() );
    windowActionGroup->addAction( editor->windowMenuAction() );
//#if !defined(__APPLE__)
//	subWindow->setWindowFlags( subWindow->windowFlags() | Qt::FramelessWindowHint );
//#endif
    subWindow->show();
}
void QbrtMainWindow::addEditor( QbrtMapEditor *editor )
{
    // MDI sub-windows logic
    //
    QMdiSubWindow *subWindow = mdiArea->addSubWindow( editor );
    menu_Window->addAction( editor->windowMenuAction() );
    windowActionGroup->addAction( editor->windowMenuAction() );
//#if !defined(__APPLE__)
//    subWindow->setWindowFlags( subWindow->windowFlags() | Qt::FramelessWindowHint );
//#endif
    subWindow->show();
}
void QbrtMainWindow::addEditor( QbrtPlotEditor *editor )
{
    // MDI sub-windows logic
    //
    QMdiSubWindow *subWindow = mdiArea->addSubWindow( editor );
    menu_Window->addAction( editor->windowMenuAction() );
    windowActionGroup->addAction( editor->windowMenuAction() );
//#if !defined(__APPLE__)
//    subWindow->setWindowFlags( subWindow->windowFlags() | Qt::FramelessWindowHint );
//#endif
    subWindow->show();
}
void QbrtMainWindow::addEditor( Qbrt2DPlotEditor *editor )
{
    // MDI sub-windows logic
    //
    QMdiSubWindow *subWindow = mdiArea->addSubWindow( editor );
    menu_Window->addAction( editor->windowMenuAction() );
    windowActionGroup->addAction( editor->windowMenuAction() );
//#if !defined(__APPLE__)
//    subWindow->setWindowFlags( subWindow->windowFlags() | Qt::FramelessWindowHint );
//#endif
    subWindow->show();
}


void QbrtMainWindow::on_action_New_triggered()
{
    Editor *editor = new Editor( this );
    editor->newFile();
    addEditor(editor);
}

#define WITH_GLOBEn
//#include "new-gui/brat/Graphics/Globe/Globe.h"
//#include "Globe/osgEarthQt/ViewerWidget"

void QbrtMainWindow::on_action_New_Map_triggered()
{
//    QbrtMapCanvas *mMapCanvas = new QbrtMapCanvas( this );

//    QMdiSubWindow *subWindow = mdiArea->addSubWindow( mMapCanvas );
//#if !defined(__APPLE__)
//    subWindow->setWindowFlags( subWindow->windowFlags() | Qt::FramelessWindowHint );
//#endif
//    subWindow->show();
//    return;

    QbrtMapEditor *editor = new QbrtMapEditor( this );
    editor->newFile();
    addEditor(editor);

#if defined( WITH_GLOBE )
	GlobePlugin* p = new GlobePlugin(nullptr, editor);
	p->run();
	QWidget *globe = dynamic_cast<QWidget*>( p->earthViewerWidget() );
	QWidget *w = new QWidget;
	QVBoxLayout *l = new QVBoxLayout( w );
	l->addWidget( globe );
	w->setLayout( l );
	QMdiSubWindow *subWindow = mdiArea->addSubWindow( w );
	//globe->setVisible( true );
	//globe->setEnabled( true );
	subWindow->show();
	globe->show();
#endif
}
void QbrtMainWindow::on_action_New_Globe_triggered()
{
}
void QbrtMainWindow::on_action_Plot_2D_triggered()
{
    Qbrt2DPlotEditor *editor = new Qbrt2DPlotEditor( this );
    editor->newFile();
    addEditor(editor);
}
void QbrtMainWindow::on_action_Plot_3D_triggered()
{
    QbrtPlotEditor *editor = new QbrtPlotEditor( this );
    editor->newFile();
    addEditor(editor);
}



void QbrtMainWindow::XYPlot()
{
	/*
	CObArray::iterator itGroup;

	wxSize size = wxDefaultSize;
	wxPoint pos = wxDefaultPosition;

	for ( itGroup = m_plots.begin(); itGroup != m_plots.end(); itGroup++ )
	{
		CPlot* plot = dynamic_cast<CPlot*>( *itGroup );
		if ( plot == NULL )
		{
			continue;
		}

		CWindowHandler::GetSizeAndPosition( size, pos );

		CreateXYPlot( plot, size, pos );
	}
	*/
}
void QbrtMainWindow::ZFXYPlot()
{
	/*
	CObArray::iterator itGroup;

	wxSize size = wxDefaultSize;
	wxPoint pos = wxDefaultPosition;

	for ( itGroup = m_plots.begin(); itGroup != m_plots.end(); itGroup++ )
	{
		CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>( *itGroup );
		if ( zfxyplot == NULL )
		{
			continue;
		}

		CWindowHandler::GetSizeAndPosition( size, pos );

		CreateZFXYPlot( zfxyplot, size, pos );
	}
	*/
}

namespace {
const int OneSecond = 1000;
const QString ZeroTime(" 0:00");
}

//L:\project\workspaces\newWP\Displays\DisplayDisplays_New.par

void QbrtMainWindow::openFile( const QString &fileName )
{
    Editor *editor = Editor::openFile( fileName, this );
    if ( editor )
        addEditor( editor );

	delete mCmdLineProcessor;
    mCmdLineProcessor = new CDisplayFilesProcessor( false );

	const std::string s = q2a( fileName );
	const char *argv[] = { "", s.c_str() };
	try
	{
		if ( mCmdLineProcessor->Process( 2, argv ) )
		{
            auto &plots = mCmdLineProcessor->plots();
            if ( mCmdLineProcessor->isZFLatLon() )		//WorldPlot
			{
                for ( auto &plot : plots )
                {
                    CWPlot* wplot = dynamic_cast<CWPlot*>( plot );
					if ( wplot == NULL )
						continue;

					/*TODO: manage title/filename in a standard way
						std::string titleTmp = wplot->m_title;
	titleTmp = CTools::SlashesDecode( titleTmp ).c_str();
	FindAndReplace( titleTmp, "\n", "-" );
	FindAndReplace( titleTmp, "\t", " " );
	title = std::string( "BRAT World Plot - " + titleTmp + n2s( wplot->m_groupNumber ) );*/

					QbrtMapEditor *map_editor = new QbrtMapEditor( this );
					map_editor->newFile();

#if defined( WITH_GLOBE )
					GlobePlugin* p = new GlobePlugin(nullptr, map_editor);
					p->run();
					QWidget *globe = dynamic_cast<QWidget*>( p->earthViewerWidget() );
					QWidget *w = new QWidget;
					QVBoxLayout *l = new QVBoxLayout( w );
					l->addWidget( globe );
					w->setLayout( l );
					QMdiSubWindow *subWindow = mdiArea->addSubWindow( w );
					//globe->setVisible( true );
					//globe->setEnabled( true );
					subWindow->show();
					globe->show();
#endif
					addEditor(map_editor);
				    //mdiArea->activateNextSubWindow();

    QEventLoop eventLoop;
    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(OneSecond);
    connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    //connect(mediaObject,
    //        SIGNAL(stateChanged(Phonon::State, Phonon::State)),
    //        &eventLoop, SLOT(quit()));
    timer.start();
    eventLoop.exec();
    //return mediaObject->state() == Phonon::StoppedState;

					map_editor->CreateWPlot( mCmdLineProcessor, wplot );
				}
			}
			else if ( mCmdLineProcessor->isYFX() )
			{
				XYPlot();
			}
			else if ( mCmdLineProcessor->isZFXY() )
			{
				ZFXYPlot();
			}
			else
			{
				CException e( "CBratDisplayApp::OnInit - Only World Plot Data, XY Plot Data and ZFXY Plot Data are implemented", BRATHL_UNIMPLEMENT_ERROR );
				CTrace::Tracer( "%s", e.what() );
				throw ( e );
			}
		}
	}
	catch ( CException &e )
	{
		SimpleErrorBox( e.what() );
		throw;
	}
	catch ( std::exception &e )
	{
		SimpleErrorBox( e.what() );
		throw;
	}
	catch ( ... )
	{
		SimpleErrorBox( "Unexpected error encountered" );
		throw;
	}
}


void QbrtMainWindow::on_action_Globe_triggered()
{
	QbrtMapEditor *map_editor = activeMapEditor();
	if ( map_editor )
	{
	//	GlobePlugin* p = new GlobePlugin( nullptr, map_editor->canvas() );
	//	p->run();
	//	QWidget *globe = dynamic_cast<QWidget*>( p->earthViewerWidget() );
	//	//QWidget *w = new QWidget;
	//	//QVBoxLayout *l = new QVBoxLayout( w );
	//	//l->addWidget( globe );
	//	//w->setLayout( l );
	//	QMdiSubWindow *subWindow = mdiArea->addSubWindow( globe );
	//	subWindow->show();
	//	//globe->show();
	}
}

void QbrtMainWindow::LoadCmdLineFiles()
{
    QStringList args = QCoreApplication::arguments();
    args.removeFirst();
    if ( !args.empty() && args[0] == "m" )
        args.removeFirst();
	//femmTODO: uncomment or review appropriate actions with cmd line passed files
	if ( args.isEmpty() )
	{
		on_action_New_Map_triggered();
		mdiArea->activateNextSubWindow();
	}
    else
        foreach (QString arg, args)
            openFile(arg);

    //mdiArea->activateNextSubWindow();
    InitialUpdateActions();
}


void QbrtMainWindow::openRecentFile()
{
    Editor *pe = activeTextEditor();
    if ( !pe || pe->okToContinue() ) {
        QAction *action = qobject_cast<QAction *>(sender());
        if (action)
            openFile( action->data().toString() );
    }
}


void QbrtMainWindow::on_action_Open_triggered()
{
    Editor *editor = Editor::open(this);
    if (editor)
        addEditor(editor);
}


void QbrtMainWindow::on_action_Reload_triggered()
{
    Editor *pe = activeTextEditor();
    if ( pe )
        pe->reOpen();
}


/////////////////////////////////////////////////////////////////////////
//                      CLOSE / OUTPUT ACTIONS
/////////////////////////////////////////////////////////////////////////


void QbrtMainWindow::closeEvent( QCloseEvent *event )
{
	auto list = mdiArea->subWindowList();
	auto const size = list.size();
	for ( int j = 0; j < size; j++ )
	{
		EditorBase *pe = dynamic_cast<EditorBase*>( list[ j ]->widget() );
		if ( !pe->okToContinue() )
		{
			event->ignore();
			return;
		}
	}

	mStopped = true;
	if ( QThreadPool::globalInstance()->activeThreadCount() )
		QThreadPool::globalInstance()->waitForDone();

	mdiArea->closeAllSubWindows();
	if ( !mdiArea->subWindowList().isEmpty() ) {
		event->ignore();
	}
	else {
		writeSettings();
		event->accept();
	}
}


void QbrtMainWindow::on_action_Save_triggered()
{
    if (activeTextEditor())
        activeTextEditor()->save();
}


void QbrtMainWindow::on_action_Save_As_triggered()
{
    if (activeTextEditor())
        activeTextEditor()->saveAs();
}



/////////////////////////////////////////////////////////////////////////
//                          PRINT ACTIONS
/////////////////////////////////////////////////////////////////////////


void QbrtMainWindow::setupPrintActions()
{
#ifndef QT_NO_PRINTER
    action_Print->setIcon( QIcon::fromTheme("document-print", QIcon(rsrcPath + "/fileprint.png") ) );
    action_Print->setPriority(QAction::LowPriority);
    action_Print->setShortcut(QKeySequence::Print);
    connect( action_Print, SIGNAL(triggered()), this, SLOT(filePrint()) );

    action_Print_Preview->setIcon( QIcon::fromTheme("fileprint", QIcon(rsrcPath + "/fileprint.png") ) );
    connect( action_Print_Preview, SIGNAL(triggered()), this, SLOT(filePrintPreview()));

    action_Export_To_PDF->setIcon( QIcon::fromTheme("exportpdf", QIcon(rsrcPath + "/exportpdf.png")) );
    action_Export_To_PDF->setPriority(QAction::LowPriority);
    action_Export_To_PDF->setShortcut(Qt::CTRL + Qt::Key_D);
    connect( action_Export_To_PDF, SIGNAL(triggered()), this, SLOT(filePrintPdf()));
#endif
}

void QbrtMainWindow::filePrint()
{
    if ( Editor *pe = activeTextEditor() )
        pe->PrintToPrinter();
}


void QbrtMainWindow::filePrintPreview()
{
    if ( Editor *pe = activeTextEditor() )
        pe->PrintPreview();
}


void QbrtMainWindow::filePrintPdf()
{
    if ( Editor *pe = activeTextEditor() )
        pe->PrintToPdf();
}




/////////////////////////////////////////////////////////////////////////
//                          UPDATE ACTIONS
/////////////////////////////////////////////////////////////////////////


#include <iostream>
void QbrtMainWindow::UpdateFileActions()
{
    Editor *pe = activeTextEditor();    //has the focus
    EditorBase *pmdi = activeMDIEditor();   //does not necessarily have the focus

    bool hasMDIEditor = ( pmdi != 0 );
    bool isMDITextEditor = ( hasMDIEditor && pmdi == pe );

    action_Save->setEnabled( isMDITextEditor && pe->document()->isModified() );
    action_Save_As->setEnabled( isMDITextEditor );
    action_Close->setEnabled( isMDITextEditor );
    action_Close_All->setEnabled( isMDITextEditor );
    action_Next->setEnabled( hasMDIEditor );
    action_Previous->setEnabled( hasMDIEditor );
    action_Tile->setEnabled( hasMDIEditor );
    action_Cascade->setEnabled( hasMDIEditor );
    action_Reload->setEnabled( isMDITextEditor && !pe->IsUntitled() );
    SubWindows_separatorAction->setVisible( hasMDIEditor );
    if ( hasMDIEditor )
        pmdi->windowMenuAction()->setChecked(true);
    std::cout << "isMDITextEditor" << isMDITextEditor << std::endl;
}

void QbrtMainWindow::updateRecentFileActions()
{
    QMutableStringListIterator i(m_recentFiles);
    while (i.hasNext()) {
        if (!QFile::exists(i.next()))
            i.remove();
    }

    for (int j = 0; j < sm_MaxRecentFiles; ++j) {
        if (j < m_recentFiles.count()) {
            QString text = tr("&%1 %2").arg(j + 1).arg(Editor::strippedName(m_recentFiles[j]));
            m_recentFileActions[j]->setText(text);
            m_recentFileActions[j]->setData( m_recentFiles[j] );
            m_recentFileActions[j]->setVisible(true);
        }
        else
            m_recentFileActions[j]->setVisible(false);
    }
    m_RecentFiles_separatorAction->setVisible(!m_recentFiles.isEmpty());
}
