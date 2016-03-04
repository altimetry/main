#if !defined GUI_CONTROL_PANELS_OPERATIONS_CONTROL_PANEL_H
#define GUI_CONTROL_PANELS_OPERATIONS_CONTROL_PANEL_H


#include "libbrathl/Product.h"

#include "ControlPanel.h"


class CProcessesTable;



//HAMMER SECTION
class CDisplayFilesProcessor;
//HAMMER SECTION


class CDataExpressionsTree : public QTreeWidget
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

	using base_t = QTreeWidget;


	// instance variables

    QIcon mGroupIcon;
    QIcon mKeyIcon;

	QWidget *mDragSource = nullptr;

	//construction / destruction

public:

	CDataExpressionsTree( QWidget *parent, QWidget *drag_source );

	virtual ~CDataExpressionsTree()
	{}

protected:

	virtual void dragEnterEvent( QDragEnterEvent *event ) override;
	virtual void dropEvent( QDropEvent *event ) override;

};









class COperationControls : public CDesktopControlsPanel
{
	//HAMMER SECTION
	CDisplayFilesProcessor *mCmdLineProcessor = nullptr;
	void openTestFile( const QString &fileName );
	//HAMMER SECTION

#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// types

	using base_t = CDesktopControlsPanel;

public:

    enum EMode
    {
        eQuick,
        eAdvanced
    };


	//static members

	//...fill helpers

	static QList<QAction*> CreateDataComputationActions( QObject *parent );


protected:

	// instance variables

    CStackedWidget *mStackWidget = nullptr;

	QWidget *mCommonGroup = nullptr;

	QListWidget *mOperationsList = nullptr;
	QListWidget *mDatasetsList = nullptr;
	CTextWidget *mExpressionTextWidget = nullptr;
	QToolButton *mNewOperationButton = nullptr;
	QToolButton *mDeleteOperationButton = nullptr;
    QToolButton *mRenameOperationButton = nullptr;
	QToolButton *mOperationFilterButton = nullptr;
	QActionGroup *mOperationFilterGroup = nullptr;
	QToolButton *mOperationExportButton = nullptr;
	QAction *mExportOperationAction = nullptr;
	QAction *mEditExportAsciiAction = nullptr;
	QToolButton *mOperationStatisticsButton = nullptr;

	QToolButton *mDisplayButton = nullptr;
	QAction *mSplitPlotsAction = nullptr;

	QToolButton *mExecuteButton = nullptr;
	QAction *mDelayExecutionAction = nullptr;
	QAction *mLaunchSchedulerAction = nullptr;
	CProcessesTable *mProcessesTable = nullptr;
	QTimer mTimer;

	//...advanced

	QToolButton *mInsertFunction = nullptr;
	QToolButton *mInsertAlgorithm = nullptr;
	QToolButton *mInsertFormula = nullptr;
	QToolButton *mSaveAsFormula = nullptr;

	QToolButton *mShowAliasesButton = nullptr;
	QToolButton *mCheckSyntaxButton = nullptr;
	QToolButton *mShowInfoButton = nullptr;

	QToolButton *mDataComputation = nullptr;
	QActionGroup *mDataComputationGroup = nullptr;

	QListWidget *mAdvancedFieldList = nullptr;
	CDataExpressionsTree *mDataExpressionsTree = nullptr;

	//...quick

	QListWidget *mQuickVariablesList = nullptr;

	//...domain variables

	CModel *mModel = nullptr;
    CWorkspace *mWRoot = nullptr;
    CWorkspaceDataset *mWDataset = nullptr;
	CWorkspaceOperation *mWOperation = nullptr;
	CWorkspaceFormula *mWFormula = nullptr;
	CWorkspaceDisplay *mWDisplay = nullptr;

	COperation *mOperation = nullptr;
	CProduct *mProduct = nullptr;
	CFormula *mUserFormula = nullptr;

	CStringMap mMapFormulaString;


	// construction / destruction

    void Wire();
	CDataExpressionsTree* CreateDataExpressionsTree( QWidget *parent, QWidget *drag_source );
	QWidget* CreateCommonWidgets();
	QWidget* CreateQuickOperationsPage();
	QWidget* CreateAdancedOperationsPage();

public:
	COperationControls( CProcessesTable *processes_table, CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~COperationControls()
	{}	

	// access 

    bool AdvancedMode() const;

    void SetAdvancedMode( bool advanced ) const;


	// remaining operations

protected:
	void FillFieldList();

signals:
	void SyncProcessExecution( bool executing );


public slots:
	void HandleSelectedWorkspaceChanged( CModel *model );

protected slots:

	void HandleSelectedOperationChanged( int operation_index );
	void HandleSelectedDatasetChanged( int dataset_index );

	void HandleNewOperation();
	void HandleDeleteOperation();
    void HandleRenameOperation();
	void HandleOperationFilter();
	void HandleExportOperation();
	void HandleEditExportAscii();
	void HandleOperationStatistics();

	void HandleExecute();
	void HandleDelayExecution();
	void HandleLaunchScheduler();


	void HandleInsertFunction();
	void HandleInsertAlgorithm();
	void HandleInsertFormula();
	void HandleSaveAsFormula();
	void HandleDataComputation();

	void HandleShowAliases();
	void HandleCheckSyntax();
	void HandleShowInfo();

	void HandleQuickMap();
	void HandleQuickPlot();

	void HandleLaunchDisplay();
};





#endif	//GUI_CONTROL_PANELS_OPERATIONS_CONTROL_PANEL_H
