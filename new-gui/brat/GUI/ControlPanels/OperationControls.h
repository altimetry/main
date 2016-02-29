#if !defined GUI_CONTROL_PANELS_OPERATIONS_CONTROL_PANEL_H
#define GUI_CONTROL_PANELS_OPERATIONS_CONTROL_PANEL_H


#include "libbrathl/Product.h"

#include "ControlPanel.h"


class CProcessesTable;



//HAMMER SECTION
class CDisplayFilesProcessor;
//HAMMER SECTION


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


protected:

	// instance variables

    CStackedWidget *mStackWidget = nullptr;

	QPushButton *mQuickMapButton = nullptr;
	QPushButton *mQuickPlotButton = nullptr;
	QPushButton *mAvancedDisplayButton = nullptr;

	QPushButton *mShowAliases = nullptr;
	QWidget *mCommonGroup = nullptr;

	QComboBox *mOperationsCombo = nullptr;
	QComboBox *mDatasetsCombo = nullptr;
	CTextWidget *mExpressionTextWidget = nullptr;

	QToolButton *mExecuteButton = nullptr;
	CProcessesTable *mProcessesTable = nullptr;
	QTimer mTimer;


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

signals:
	void SyncProcessExecution( bool executing );


public slots:
	void HandleSelectedWorkspaceChanged( CModel *model );

protected slots:

	void HandleExecute();
	void HandleSelectedOperationChanged( int operation_index );
	void HandleSelectedDatasetChanged( int dataset_index );

	void HandleShowAliases();

	void HandleQuickMap();
	void HandleQuickPlot();

	void HandleDisplay();
};











#endif	//GUI_CONTROL_PANELS_OPERATIONS_CONTROL_PANEL_H
