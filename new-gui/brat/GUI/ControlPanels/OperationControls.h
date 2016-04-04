#if !defined GUI_CONTROL_PANELS_OPERATIONS_CONTROL_PANEL_H
#define GUI_CONTROL_PANELS_OPERATIONS_CONTROL_PANEL_H


#include "libbrathl/Product.h"

#include "ControlPanel.h"


class CProcessesTable;
class CDataExpressionsTreeWidget;
class CFieldsTreeWidget;
class CMapDisplayData;
class CBratFilters;



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


	//...fill helpers

	static QList<QAction*> CreateDataComputationActions( QObject *parent );


protected:

	// instance variables

    CStackedWidget *mStackWidget = nullptr;
	QWidget *mQuickOperationsPage = nullptr;
	QWidget *mAdvancedOperationsPage = nullptr;


	QWidget *mCommonGroup = nullptr;

	QToolButton *mOperationFilterButton = nullptr;
	//QActionGroup *mOperationFilterGroup = nullptr;
	QToolButton *mOperationExportButton = nullptr;
	QAction *mExportOperationAction = nullptr;
	QAction *mEditExportAsciiAction = nullptr;
	QToolButton *mOperationStatisticsButton = nullptr;

	QToolButton *mSplitPlotsButton = nullptr;

	//...advanced

	QComboBox *mOperationsCombo = nullptr;
	QComboBox *mAdvancedDatasetsCombo = nullptr;
	QToolButton *mNewOperationButton = nullptr;
	QToolButton *mDeleteOperationButton = nullptr;
    QToolButton *mRenameOperationButton = nullptr;
    QToolButton *mDuplicateOperationButton = nullptr;

	QToolButton *mAdvancedDisplayButton = nullptr;
	QAction *mDelayExecutionAction = nullptr;
	QAction *mLaunchSchedulerAction = nullptr;
	CProcessesTable *mProcessesTable = nullptr;

	QgsCollapsibleGroupBox *mOperationExpressionsGroup = nullptr;
	QgsCollapsibleGroupBox *mSamplingGroup = nullptr;


	QTimer mTimer;

	QToolButton *mShowAliasesButton = nullptr;

	QToolButton *mInsertFunction = nullptr;
	QToolButton *mInsertAlgorithm = nullptr;
	QToolButton *mInsertFormula = nullptr;
	QToolButton *mSaveAsFormula = nullptr;
	QToolButton *mCheckSyntaxButton = nullptr;
	QToolButton *mShowInfoButton = nullptr;
	QGroupBox *mExpressionGroup = nullptr;

	QToolButton *mDataComputation = nullptr;
	QActionGroup *mDataComputationGroup = nullptr;

	CFieldsTreeWidget *mAdvancedFieldsTree = nullptr;
    CTextWidget *mAdvancedFieldDesc = nullptr;

#if defined(USE_2_EXPRESSION_TREES)
	CStackedWidget *mDataExpressionsStack = nullptr;
	CDataExpressionsTreeWidget *mDataExpressionsTrees[ EExpressionTrees_size ];
#endif
	QToolButton *mSwitchToMapButton = nullptr;
	QToolButton *mSwitchToPlotButton = nullptr;
	CDataExpressionsTreeWidget *mDataExpressionsTree = nullptr;

	//...quick

	QComboBox *mQuickDatasetsCombo = nullptr;
	QToolButton *mAddVariable = nullptr;
	QToolButton *mClearVariables = nullptr;
	QToolButton *mDisplayMapButton = nullptr;
	QToolButton *mDisplayPlotButton = nullptr;
	QListWidget *mQuickVariablesList = nullptr;
    CTextWidget *mQuickFieldDesc = nullptr;


	//...domain variables

    CWorkspace *mWRoot = nullptr;
    CWorkspaceDataset *mWDataset = nullptr;
	CWorkspaceOperation *mWOperation = nullptr;
	CWorkspaceFormula *mWFormula = nullptr;
	CWorkspaceDisplay *mWDisplay = nullptr;

	const CBratFilters &mBratFilters;

	COperation *mCurrentOperation = nullptr;
	CDataset *mDataset = nullptr;
	CProduct *mProduct = nullptr;
	CFormula *mUserFormula = nullptr;

	CStringMap mMapFormulaString;


	// construction / destruction

    void Wire();
	QWidget* CreateCommonWidgets( QAbstractButton *b1, QAbstractButton *b2 );
	void CreateQuickOperationsPage();
	void CreateAdancedOperationsPage();

public:
	COperationControls( CProcessesTable *processes_table, CModel &model, CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

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

    bool MapRequested() const;

	std::string GetOpunit();		//TODO how to deal/assign with field units

    void ResetFilterActions();
    void ResetSelectedFilter();

	void LaunchDisplay( const std::string &display_name );

	COperation* CreateQuickOperation( CMapTypeOp::ETypeOp type );
	void SelectOperation( const std::string &name, bool select_map );


signals:
	void SyncProcessExecution( bool executing );
    void OperationModified( const COperation *operation );



public slots:
	void HandleLaunchScheduler();

protected slots:

	//quick

	void HandleWorkspaceChanged_Quick();		//not really a slot; called by advanced

	void HandleSelectedDatasetChanged_Quick( int dataset_index );
	void HandleSelectedFieldChanged_Quick( int variable_index );
	void HandleVariableStateChanged_Quick( QListWidgetItem *item );
    void HandleDatasetsChanged_Quick(CDataset*);

	void HandleQuickMap();
	void HandleQuickPlot();

	//remaining

	void HandleWorkspaceChanged();

	void HandleSelectedOperationChanged( int operation_index );
	void HandleSelectedFieldChanged_Advanced();
	void HandleSelectedDatasetChanged_Advanced( int dataset_index );
    void HandleDatasetsChanged_Advanced( CDataset *dataset );

#if defined(USE_2_EXPRESSION_TREES)

	void HandleExpressionsTreePageChanged( int index );
#else

	void HandleSwitchExpressionType();
#endif

	void HandleFormulaInserted( CFormula *formula );
	void HandleSelectedFormulaChanged( CFormula *formula );

	void HandleNewOperation();
	void HandleDeleteOperation();
    void HandleRenameOperation();
    void HandleDuplicateOperation();
    void HandleOperationFilter();
	void HandleOperationFilterButton( QAction *a );
	void HandleOperationFilterButtonToggled( bool );
	void HandleExportOperation();
	void HandleEditExportAscii();
	void HandleOperationStatistics();

	bool HandleExecute();
	void HandleProcessFinished( int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation );
	void HandleDelayExecution();
	void SchedulerProcessError( QProcess::ProcessError );

	void HandleInsertFunction();
	void HandleInsertAlgorithm();
	void HandleInsertFormula();
	void HandleSaveAsFormula();
	void HandleDataComputation();

	void HandleShowAliases();
	void HandleCheckSyntax();
	void HandleShowInfo();
};





#endif	//GUI_CONTROL_PANELS_OPERATIONS_CONTROL_PANEL_H
