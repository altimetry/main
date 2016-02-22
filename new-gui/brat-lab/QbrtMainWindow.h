#ifndef BRAT_LAB_MAIN_WINDOW_H
#define BRAT_LAB_MAIN_WINDOW_H

#include "ui_QbrtMainWindow.h"
#include "GUI/Editor.h"
#include "GUI/FindDialog.h"
#include "System/BackServices.h"


extern const QString rsrcPath;


class QbrtMapEditor;
class QbrtPlotEditor;
class Qbrt2DPlotEditor;
class DisplayFilesProcessor;
class CWPlot;

class CBratSettings;


class QbrtMainWindow : public QMainWindow, private Ui::QbrtMainWindow
{
	Q_OBJECT

	typedef QMainWindow base_t;

	CBratSettings &mSettings;
    TBackServices &m_bs;
    FindDialog *m_FindDialog;

    // MDI sub-windows logic
    //
    QActionGroup *windowActionGroup;
    QAction *SubWindows_separatorAction;

    // Most recent files logic
    //
    QStringList m_recentFiles;
    static const int sm_MaxRecentFiles = 5;
    QAction *m_recentFileActions[sm_MaxRecentFiles];
    QAction *m_RecentFiles_separatorAction;

    // Business logic
    //
    DisplayFilesProcessor *mCmdLineProcessor = nullptr;

    // Threading Lab
    //
	QProgressBar *mProgressBar = nullptr;
	QToolButton *mMessageButton = nullptr;
	QToolButton *mThreadsButton = nullptr;

	void createStatusBar();

    int mTotal = 0;
    size_t mDone = 0;
    volatile bool mStopped = true;
	std::vector<QAction*> mConcurrentActions;

	bool event( QEvent *event ) override;

public:
    explicit QbrtMainWindow( CBratSettings &settings );

	
	virtual ~QbrtMainWindow();


public slots:
    void openFile(const QString &fileName);
    void setCurrentFile( const Editor *peditor );
    void findNext( const QString &str, Qt::CaseSensitivity cs );
    void findPrevious( const QString &str, Qt::CaseSensitivity cs );

protected:
    void closeEvent(QCloseEvent *event);

    void readSettings();
    void writeSettings();

	template< class EDITOR >
	EDITOR* activeEditor();
    Editor *activeTextEditor();
    QbrtMapEditor *activeMapEditor();
    EditorBase *activeMDIEditor();
    void addEditor(Editor *editor);
    void addEditor(QbrtMapEditor *editor);
    void addEditor(QbrtPlotEditor *editor);
    void addEditor(Qbrt2DPlotEditor *editor);


    //not slots: manually called
    //
    void updateRecentFileActions();
    void connectAutoUpdateEditActions( Editor* pe );

    // file print
    //
    void setupPrintActions();


    // format
    //
    QAction
        *actionTextBold,
        *actionTextUnderline,
        *actionTextItalic,
        *actionTextColor,
        *actionAlignLeft,
        *actionAlignCenter,
        *actionAlignRight,
        *actionAlignJustify
    ;

    void SetFormatActions();

    // font / format
    //
    void alignmentChanged( Qt::Alignment a );
    void fontChanged( const QFont &f );
    void colorChanged( const QColor &c );


    // Business Logic
    //
	//void CreateWPlot( CWPlot* wplot, QSize& size, QPoint& pos );
	//void WorldPlot();
	void XYPlot();
	void ZFXYPlot();


private slots:
    void InitialUpdateActions();
    void UpdateFileActions();
    void UpdateEditorsActions();
    void UpdateToolsActions();
    void LoadCmdLineFiles();
    void openRecentFile();

    void on_action_New_triggered();
    void on_action_New_Map_triggered();
    void on_action_New_Globe_triggered();
    void on_action_Open_triggered();
    void on_action_Reload_triggered();
    void on_action_Save_triggered();
    void on_action_Save_As_triggered();
    void on_action_Undo_triggered();
    void on_action_Redo_triggered();
    void on_action_Cut_triggered();
    void on_action_Copy_triggered();
    void on_action_Paste_triggered();
    void on_action_Delete_triggered();
    void on_action_Select_All_triggered();
    void on_action_Font_triggered();
    void on_action_Read_Only_triggered();
    void on_action_Options_triggered();
    void on_action_About_triggered();

    void clipboardDataChanged();

    // format
    //
    void textBold();
    void textItalic();
    void textUnderline();
    void textAlign( QAction *a );
    void textColor();

    // font
    //
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textStyle(int styleIndex);

    // font / format
    //
    void currentCharFormatChanged(const QTextCharFormat &format);
    void cursorPositionChanged();

    // file print
    //
    void filePrint();
    void filePrintPreview();
    void filePrintPdf();

    void on_action_Find_triggered();
    void on_action_Find_Next_triggered();
    void on_action_Find_Previous_triggered();
    void on_action_Globe_triggered();
    void on_action_Plot_2D_triggered();
    void on_action_Plot_3D_triggered();

    // threaded tests
    //
	void checkIfStuffDone();
	void doThreadedStuff();
	void cancelStuff( bool cancel );
    void on_mAction_Test_triggered();
};

#endif // BRAT_LAB_MAIN_WINDOW_H
