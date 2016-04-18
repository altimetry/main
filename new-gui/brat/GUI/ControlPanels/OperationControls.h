#if !defined GUI_CONTROL_PANELS_OPERATIONS_CONTROL_PANEL_H
#define GUI_CONTROL_PANELS_OPERATIONS_CONTROL_PANEL_H


#include "libbrathl/Product.h"

#include "ControlPanel.h"


class CProcessesTable;
class CDataExpressionsTreeWidget;
class CFieldsTreeWidget;
class CMapDisplayData;
class CBratFilters;


class COperationControls : public CDesktopControlsPanel
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


	//static members

    static const std::vector<std::string> smPredefinedVariables;


	static void SelectOperationDataset( COperation *operation, QComboBox *combo, bool block_signals );

	static bool AssignFilter( const CBratFilters &brat_filters, COperation *operation, const std::string &name );

	static bool RemoveFilter( COperation *operation, const std::string &name );


	//...fill helpers

	static QList<QAction*> CreateDataComputationActions();


protected:

	// instance variables

    CStackedWidget *mStackWidget = nullptr;
	QWidget *mQuickOperationsPage = nullptr;
	QWidget *mAdvancedOperationsPage = nullptr;


	QWidget *mCommonGroup = nullptr;

	QToolButton *mOperationExportButton = nullptr;
	QAction *mExportOperationAction = nullptr;
	QAction *mEditExportAsciiAction = nullptr;
	QToolButton *mOperationStatisticsButton = nullptr;

	QToolButton *mSplitPlotsButton = nullptr;

	//...advanced

	QToolButton *mOperationFilterButton_Advanced = nullptr;
	QComboBox *mOperationsCombo = nullptr;
	QComboBox *mAdvancedDatasetsCombo = nullptr;
	QToolButton *mNewOperationButton = nullptr;
	QToolButton *mDeleteOperationButton = nullptr;
    QToolButton *mRenameOperationButton = nullptr;
    QToolButton *mDuplicateOperationButton = nullptr;

	QToolButton *mAdvancedExecuteButton = nullptr;
	QAction *mDelayExecutionAction = nullptr;
	QAction *mLaunchSchedulerAction = nullptr;
	CProcessesTable *mProcessesTable = nullptr;

	QGroupBox *mOperationExpressionsGroup = nullptr;
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

	QToolButton *mSwitchToMapButton = nullptr;
	QToolButton *mSwitchToPlotButton = nullptr;
	CDataExpressionsTreeWidget *mDataExpressionsTree = nullptr;

	//...quick

	QToolButton *mOperationFilterButton_Quick = nullptr;
	QComboBox *mQuickDatasetsCombo = nullptr;
	QToolButton *mAddVariable = nullptr;
	QToolButton *mClearVariables = nullptr;
	QToolButton *mDisplayMapButton = nullptr;
	QToolButton *mDisplayPlotButton = nullptr;
	QListWidget *mQuickVariablesList = nullptr;
    CTextWidget *mQuickFieldDesc = nullptr;
	bool mCanExecuteQuickOperation = false;


	//...domain variables

    CWorkspace *mWRoot = nullptr;
    CWorkspaceDataset *mWDataset = nullptr;
	CWorkspaceOperation *mWOperation = nullptr;
	CWorkspaceFormula *mWFormula = nullptr;
	CWorkspaceDisplay *mWDisplay = nullptr;

	const CBratFilters &mBratFilters;

	COperation *mCurrentOperation = nullptr;
	COperation *mQuickOperation = nullptr;
	const CDataset *mCurrentOriginalDataset = nullptr;
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

	virtual ~COperationControls();


	// access 

    bool AdvancedMode() const;

    void SetAdvancedMode( bool advanced ) const;


	// remaining operations

protected:

	//filtering

    void ResetFilterActions();
    void ResetFilterSelection();

	bool AssignAdvancedFilter( const std::string &name );
	bool RemoveAdvancedFilter( const std::string &name );
	bool AssignQuickFilter( const std::string &name );
	bool RemoveQuickFilter( const std::string &name );

	void SelectOperation( const std::string &name, bool select_map );	//meant for quick, designed (not tested) for all

	//quick

	CDataset* QuickDatasetSelected() const;
	COperation* QuickOperation() const;
	COperation* CreateEmptyQuickOperation();
	COperation* CreateQuickOperation( CMapTypeOp::ETypeOp type );
    bool IsQuickOperationSelected() const;
    void UpdateFieldsCheckState();

	//advanced

	void SelectDataComputationMode();
	std::string GetOpunit();

    bool MapRequested() const;

	//both

	void LaunchDisplay( const std::string &display_name );
	bool Execute( bool sync );


	//remaining

	bool AssignDataset( const CDataset *dataset, bool changing_used_dataset );
	bool SelectDataset( const std::string &dataset_name );
	//bool SelectDataset( int dataset_index );
	bool SelectDataset( const CDataset *dataset );

    void FillDatasets_Advanced( int index );

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

	void HandleOperationFilterButton_Quick( QAction *a );
	void HandleOperationFilterButtonToggled_Quick( bool );

	void HandleQuickMap();
	void HandleQuickPlot();

	//remaining

	void HandleWorkspaceChanged();

	void HandleSelectedOperationChanged( int operation_index );
	void HandleSelectedFieldChanged_Advanced();
	void HandleSelectedDatasetChanged_Advanced( int dataset_index );
    void HandleDatasetsChanged_Advanced( CDataset *dataset );

    void HandleFiltersChanged();
	void HandleFilterCompositionChanged( std::string filter_name );

	void HandleSwitchExpressionType();

	void HandleFormulaInserted( CFormula *formula );
	void HandleSelectedFormulaChanged( CFormula *formula );

	void HandleNewOperation();
	void HandleDeleteOperation();
    void HandleRenameOperation();
    void HandleDuplicateOperation();

	void HandleOperationFilterButton_Advanced( QAction *a );
	void HandleOperationFilterButtonToggled_Advanced( bool );

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
