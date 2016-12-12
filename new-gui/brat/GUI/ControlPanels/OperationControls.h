/*
* This file is part of BRAT
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#if !defined GUI_CONTROL_PANELS_OPERATIONS_CONTROL_PANEL_H
#define GUI_CONTROL_PANELS_OPERATIONS_CONTROL_PANEL_H


#include "libbrathl/Product.h"

#include "DataModels/Workspaces/Operation.h"

#include "DesktopControlPanel.h"


class CProcessesTable;
class CDataExpressionsTreeWidget;
class CFieldsTreeWidget;
class CMapDisplayData;
class CBratFilters;
class CBratTask;
class CProgressInterface;


#if defined (_MSC_VER) && (_MSC_VER >= 1900 ) && defined (USE_DATA_VISUALIZATION)

#include "../support/code/future/NewPlotEditor.h"

using plot_editor_t = CNewPlotEditor;

#else

using plot_editor_t = CPlotEditor;

#endif




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
        eSLA,
		eWinds,
		eSigma0,
		eRange,

		EPredefinedVariables_size
	};

	enum EPredefinedSelectionCriteria
	{
		eOceanEditing,

		EPredefinedSelectionCriteria_size
	};



	using EExecutionType = COperation::EExecutionType;

	using post_execution_handler_t = void(COperationControls::*)( int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation );

	struct post_execution_handler_wrapper_t
	{
		COperation *operation;
		post_execution_handler_t post_execution_handler;
	};


	//static members

    static const std::vector<std::string> smQuickPredefinedVariableNames;

	static bool FormulaNameMatchesQuickAlias( const std::string &description, EPredefinedVariables index );

	static const std::string& QuickFindAliasValue( const CProductInfo &pi, EPredefinedVariables index );

	static const std::string& QuickFindAliasValue( const CProductInfo &pi, EPredefinedSelectionCriteria index );


	static void SelectOperationDatasetIndex( COperation *operation, QComboBox *combo );

	static bool AssignFilter( const CBratFilters &brat_filters, COperation *operation, const std::string &name );

	static bool RemoveFilter( COperation *operation, const std::string &name );


	//...fill helpers

	static QList<QAction*> CreateDataComputationActions();
    static QList<QAction*> CreateDataSmoothingActions();


protected:

	// instance variables

    CStackedWidget *mStackWidget = nullptr;
	QWidget *mQuickOperationsPage = nullptr;
	QWidget *mAdvancedOperationsPage = nullptr;


	QWidget *mCommonGroup = nullptr;

	QToolButton *mOperationExportButton = nullptr;
	QAction *mExportOperationAction = nullptr;
	QAction *mEditExportAsciiAction = nullptr;
	QAction *mCreateExportedDisplaysAction = nullptr;
	QToolButton *mOperationStatisticsButton = nullptr;

	QToolButton *mSplitPlotsButton = nullptr;

	stack_button_type *mQuickPageButton = nullptr;
	stack_button_type *mAdvancedPageButton = nullptr;


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
	bool mSyncProcessExecuting = false;

	QGroupBox *mOperationExpressionsGroup = nullptr;

	QTimer mTimer;

	QToolButton *mShowAliasesButton = nullptr;

	QToolButton *mInsertFunction = nullptr;
	QToolButton *mInsertAlgorithm = nullptr;
	QToolButton *mInsertFormula = nullptr;
	QToolButton *mSaveAsFormula = nullptr;
    QToolButton *mDataDisplayProperties = nullptr;
    QToolButton *mShowInfoButton = nullptr;
	QGroupBox *mExpressionGroup = nullptr;

	QToolButton *mDataComputation = nullptr;
	QActionGroup *mDataComputationGroup = nullptr;

    // NOTE: 'filter' (Brat v.3) was renamed to 'Smoothing' (Brat v.4)
    // In Brat v.4 'filter' is a reserved word to spatial and temporal filters
    QToolButton *mDataSmoothing = nullptr;
    QActionGroup *mDataSmoothingGroup = nullptr;

	CFieldsTreeWidget *mAdvancedFieldsTree = nullptr;
    CTextWidget *mAdvancedFieldDesc = nullptr;

	QToolButton *mSwitchToMapButton = nullptr;
	QToolButton *mSwitchToPlotButton = nullptr;
	CDataExpressionsTreeWidget *mDataExpressionsTree = nullptr;

	QgsCollapsibleGroupBox *mSamplingGroup = nullptr;
    QLabel *mXLonLabel = nullptr;
    QLabel *mYLatLabel = nullptr;
    QLineEdit *mXLonMinValue = nullptr;
    QLineEdit *mXLonMaxValue = nullptr;
    QLineEdit *mYLatMinValue = nullptr;
    QLineEdit *mYLatMaxValue = nullptr;
    QToolButton *mGetDataMaxMinX = nullptr;
    QToolButton *mGetDataMaxMinY = nullptr;
    QLineEdit *mXLonStep = nullptr;
    QLineEdit *mYLatStep = nullptr;
    QLineEdit *mXLonCutOff = nullptr;
    QLineEdit *mYLatCutOff = nullptr;
    QLabel *mXLonIntervalsLabel = nullptr;
    QLabel *mYLatIntervalsLabel = nullptr;
    QLabel *XiconWarning = nullptr;
    QLabel *YiconWarning = nullptr;


	//...quick

	QToolButton *mOperationFilterButton_Quick = nullptr;
	QComboBox *mQuickDatasetsCombo = nullptr;
	QToolButton *mAddVariable = nullptr;
	QToolButton *mClearVariables = nullptr;
	QToolButton *mDisplayMapButton = nullptr;
	QToolButton *mDisplayPlotButton = nullptr;
	QListWidget *mQuickVariablesList = nullptr;
	QCheckBox *mQuickSelectionCriteriaCheck = nullptr;
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
	bool mQuickInitializing = false;					//ugly
	const CDataset *mCurrentOriginalDataset = nullptr;
	CProduct *mProduct = nullptr;
    CFormula *mUserFormula = nullptr;
    CFormula *mXformula = nullptr;
    CFormula *mYformula = nullptr;

    CStringMap mMapFormulaString;


	// construction / destruction

    void Wire();
	QWidget* CreateCommonWidgets( QAbstractButton *b1, QAbstractButton *b2 );
	void CreateQuickOperationsPage();
	void CreateAdvancedOperationsPage();

public:
	COperationControls( CProcessesTable *processes_table, CModel &model, CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~COperationControls();


	// access 

    bool AdvancedMode() const;

    void SetAdvancedMode( bool advanced );

	COperation* CurrentOperation() { return mCurrentOperation; }

	COperation* QuickOperation() { return mQuickOperation; }


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

	//both

	// for enabling / disabling widgets dependent of multiple change types
	//
	void UpdateGUIState();

	bool CreateOperationExecutionDisplays( std::string &to_display, int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation );
	void OperationSyncExecutionFinished( int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation );
	void OperationSyncExecutionFinishedWithDisplay( int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation );
	void StatsAsyncComputationFinished( int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation );

	bool CheckOperation( COperation *operation, std::string& msg, const CStringMap* aliases );	//CtrlOperation
	void LaunchDisplay( const std::string &display_name );
	bool ExecuteCurrentOperation( post_execution_handler_t post_execution_handler = &COperationControls::OperationSyncExecutionFinishedWithDisplay );
	std::pair< bool, CProgressInterface* > ConfirmFiltering( const COperation *operation );
	bool Execute( EExecutionType type, COperation *operation, bool sync, post_execution_handler_t post_execution_handler );
	CBratTask* Schedule( EExecutionType type, const QDateTime &at, CBratTask *parent = nullptr );


	//quick

	std::string QuickDatasetSelectedName() const;
	CDataset* QuickDatasetSelected() const;
	COperation* GetOrCreateEmptyQuickOperation();
	COperation* CreateQuickOperation( CMapTypeOp::ETypeOp type );
    bool IsQuickOperationSelected() const;
    void UpdateFieldsCheckState();

	CField* QuickFindField( CProduct *product, EPredefinedVariables index, bool &alias_used, std::string &field_error_msg );
	CField* QuickFindField( CProduct *product, EPredefinedSelectionCriteria index, bool &alias_used, std::string &field_error_msg );

	//advanced

	void SelectDataComputationMode();
    void SelectDataSmoothingMode();
    std::string GetOpunit();
    
	/// Sampling methods ////////////////
    void UpdateSamplingGroup();
    void GetDataMinMax(CFormula *formula);
    void ValidateData();
    bool VerifyMinMaxX();
    bool VerifyMinMaxY();
    bool VerifyMinMaxAsDate( CFormula *formula, QLineEdit *LineEdit_Min, QLineEdit *LineEdit_Max );
    bool VerifyMinMax( CFormula *formula, QLineEdit *LineEdit_Min, QLineEdit *LineEdit_Max );
    void GetValueAsDate( QLineEdit *LineEdit_Axisvalue, double &seconds, double defValue, double min, double max );
    void GetValue      ( QLineEdit *LineEdit_Axisvalue, double &value, double defValue,
                         double min = defaultValue<double>(), double max = defaultValue<double>() );
    void ComputeXYInterval();
    bool ComputeInterval( CFormula *formula, QLabel *IntervalsLabel, QLineEdit *StepLineEdit, QLabel *IconWarning );
    /////////////////////////////////////

    bool MapPlotSelected() const;
	void ExportASCIIAsyncComputationFinished( int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation );
	void ExportGeoTIFFAsyncComputationFinished( int exit_code, QProcess::ExitStatus exitStatus, const COperation *operation );


	//remaining

	bool AssignDataset( const std::string &new_dataset_name, bool changing_used_dataset );
	//bool SelectDataset( int dataset_index );
	bool UpdateDatasetSelection( const CDataset *dataset );
	bool UpdateSelectedDatasetAdvancedSelection();

    void FillDatasets_Advanced( int index );

signals:
	void AsyncProcessExecution( bool executing );
	void SyncProcessExecution( bool executing );
    void OperationModified( const COperation *operation );


public slots:
	void HandleLaunchScheduler();

	//entry point
	void HandleWorkspaceChanged();

protected slots:

	//quick

	void HandleWorkspaceChanged_Quick();		//not really a slot; called by advanced

	void HandleSelectedDatasetChanged_Quick( int dataset_index );
	void HandleSelectedFieldChanged_Quick( int variable_index );
	void HandleVariableStateChanged_Quick( QListWidgetItem *item );
    void HandleDatasetsChanged_Quick( const CDataset *dataset );

	void HandleOperationFilterButton_Quick( QAction *a );
	void HandleOperationFilterButtonToggled_Quick( bool );

	void HandleQuickMap();
	void HandleQuickPlot();

	//remaining

	void HandlePageChanged( int index );

	void HandleSelectedOperationChanged( int operation_index );
	void HandleSelectedFieldChanged_Advanced();
	void HandleSelectedDatasetChanged_Advanced( int dataset_index );
    void HandleDatasetsChanged_Advanced( const CDataset *dataset );

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
	void HandleCreateExportedDisplays();
	void HandleOperationStatistics();

	bool HandleExecute();

	void HandleProcessFinished( int exit_code, QProcess::ExitStatus exitStatus, bool sync, void *user_data );
	void HandleDelayExecution();
	void SchedulerProcessError( QProcess::ProcessError );

	void HandleInsertFunction();
	void HandleInsertAlgorithm();
	void HandleInsertFormula();
	void HandleSaveAsFormula();
    void HandleDataDisplayProperties();

	void HandleDataComputation();
    void HandleDataSmoothing();

	void HandleShowAliases();
	void HandleShowInfo();

    // Sampling //////////////
    void HandleXLonMin_changed();
    void HandleXLonMax_changed();
    void HandleYLatMin_changed();
    void HandleYLatMax_changed();
    void HandleXLonStep_changed();
    void HandleYLatStep_changed();
    void HandleXLoessCut_changed();
    void HandleYLoessCut_changed();
    void HandleGetDataMinMaxX();
    void HandleGetDataMinMaxY();

};





#endif	//GUI_CONTROL_PANELS_OPERATIONS_CONTROL_PANEL_H
