#if !defined GUI_CONTROL_PANELS_OPERATIONS_CONTROL_PANEL_H
#define GUI_CONTROL_PANELS_OPERATIONS_CONTROL_PANEL_H


#include "libbrathl/Product.h"

#include "ControlPanel.h"


class CProcessesTable;
class CDataExpressionsTree;
class CFieldsTree;



//HAMMER SECTION
class CDisplayFilesProcessor;
//HAMMER SECTION



//#define USE_2_EXPRESSION_TREES



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

	enum EPredefinedVariables
	{
		eSSH,
		eSWH,
		eWinds,
		eSigma0,

		EPredefinedVariables_size
	};


#if defined(USE_2_EXPRESSION_TREES)
	enum EExpressionTrees
	{
		eExpressionTreeMap,
		eExpressionTreePlot,

		EExpressionTrees_size
	};
#endif


	//static members

    static const std::vector<std::string> smPredefinedVariables;


	//domain

	static bool CreateDisplayData( COperation *operation, CMapDisplayData &m_dataList );


	//...fill helpers

	static QList<QAction*> CreateDataComputationActions( QObject *parent );


protected:

	// instance variables

    CStackedWidget *mStackWidget = nullptr;

	QWidget *mCommonGroup = nullptr;

	QToolButton *mOperationFilterButton = nullptr;
	QActionGroup *mOperationFilterGroup = nullptr;
	QToolButton *mOperationExportButton = nullptr;
	QAction *mExportOperationAction = nullptr;
	QAction *mEditExportAsciiAction = nullptr;
	QToolButton *mOperationStatisticsButton = nullptr;

	QToolButton *mSplitPlotsButton = nullptr;

	//...advanced

	QComboBox *mOperationsCombo = nullptr;
	QComboBox *mAdvancedDatasetsCombo = nullptr;
	CTextWidget *mExpressionTextWidget = nullptr;
	QToolButton *mNewOperationButton = nullptr;
	QToolButton *mDeleteOperationButton = nullptr;
    QToolButton *mRenameOperationButton = nullptr;

	QToolButton *mAdvancedDisplayButton = nullptr;
	QAction *mDelayExecutionAction = nullptr;
	QAction *mLaunchSchedulerAction = nullptr;
	CProcessesTable *mProcessesTable = nullptr;

	QgsCollapsibleGroupBox *mExpressionGroup = nullptr;

	QTimer mTimer;


	QToolButton *mInsertFunction = nullptr;
	QToolButton *mInsertAlgorithm = nullptr;
	QToolButton *mInsertFormula = nullptr;
	QToolButton *mSaveAsFormula = nullptr;

	QToolButton *mShowAliasesButton = nullptr;
	QToolButton *mCheckSyntaxButton = nullptr;
	QToolButton *mShowInfoButton = nullptr;

	QToolButton *mDataComputation = nullptr;
	QActionGroup *mDataComputationGroup = nullptr;

	CFieldsTree *mAdvancedFieldsTree = nullptr;

#if defined(USE_2_EXPRESSION_TREES)
	CStackedWidget *mDataExpressionsStack = nullptr;
	CDataExpressionsTree *mDataExpressionsTrees[ EExpressionTrees_size ];
#endif
	QToolButton *mSwitchToMapButton = nullptr;
	QToolButton *mSwitchToPlotButton = nullptr;
	CDataExpressionsTree *mDataExpressionsTree = nullptr;

	//...quick

	QComboBox *mQuickDatasetsCombo = nullptr;
	QToolButton *mAddVariable = nullptr;
	QToolButton *mClearVariables = nullptr;
	QListWidget *mQuickVariablesList = nullptr;
	QToolButton *mDisplayMapButton = nullptr;
	QToolButton *mDisplayPlotButton = nullptr;


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
	QWidget* CreateCommonWidgets( QAbstractButton *b1, QAbstractButton *b2 );
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

	CDataset* QuickDataset();

	bool Execute( bool sync );

	void SelectDataComputationMode();


signals:
	void SyncProcessExecution( bool executing );


public slots:
	void HandleSelectedWorkspaceChanged( CModel *model );

protected slots:

	//quick

	void HandleSelectedDatasetChanged_Quick( int dataset_index );
	void HandleSelectedVariableChanged_Quick( int variable_index );
	void HandleVariableStateChanged_Quick( QListWidgetItem *item );
	void HandleDatasetsChanged_Quick();

	void HandleQuickMap();
	void HandleQuickPlot();

	//remaining

	void HandleSelectedOperationChanged( int operation_index );
	void HandleSelectedDatasetChanged_Advanced( int dataset_index );
	void HandleDatasetsChanged_Advanced();

#if defined(USE_2_EXPRESSION_TREES)

	void HandleExpressionsTreePageChanged( int index );
#else

	void HandleSwitchExpressionType();
#endif

	void HandleFormulaInserted( CFormula *formula );

	void HandleNewOperation();
	void HandleDeleteOperation();
    void HandleRenameOperation();
	void HandleOperationFilter();
	void HandleExportOperation();
	void HandleEditExportAscii();
	void HandleOperationStatistics();

	bool HandleExecute();
	void HandleProcessFinished( int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation );
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

	void LaunchDisplay();
};





#endif	//GUI_CONTROL_PANELS_OPERATIONS_CONTROL_PANEL_H
