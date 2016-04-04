#ifndef GUI_DISPLAY_EDITORS_ABSTRACT_DISPLAY_EDITOR_H
#define GUI_DISPLAY_EDITORS_ABSTRACT_DISPLAY_EDITOR_H


#include <QMainWindow>

#include "new-gui/Common/QtUtils.h"

#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/Workspaces/Display.h"


class CTabbedDock;
class CTextWidget;

struct CViewControlsPanelGeneral;

class CModel;
class CWorkspaceDataset;
class CWorkspaceOperation;
class CWorkspaceFormula;
class CWorkspaceDisplay;
class COperation;
class CDisplay;
class CDisplayFilesProcessor;

enum EActionTag : int;



/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////

// IMPORTANT note: the derived classes must call the base
//	CreateWidgets, specializing for the type of the general tab
//
// IMPORTANT note 2: it is assumed that an editor is child of 
//	a desktop sub-window (MDI or not). This is critical because
//	the editor can auto-close itself and this can only be done
//	by invoking the parent close event. If design changes, and 
//	parent window is null or the main window, the auto-closing
//	spots in this class must be inspected.
//
class CAbstractDisplayEditor : public QMainWindow
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	///////////////////////////////////////////////////////////
	// types
	///////////////////////////////////////////////////////////

	using base_t = QMainWindow;


	///////////////////////////////////////////////////////////
	// static members
	///////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////
	// instance data
	///////////////////////////////////////////////////////////

	//... working dock
	CTabbedDock *mWorkingDock = nullptr;

	//... main tool-bar
	QComboBox *mOperationsCombo = nullptr;
	QLineEdit *mFilterLineEdit = nullptr;
	QLineEdit *mDatasetName = nullptr;
	QPushButton *mSaveOneClickButton = nullptr;


	//... graphics tool-bar
protected:
	QToolBar *mGraphicsToolBar = nullptr;

	//tab general
	CViewControlsPanelGeneral *mTabGeneral = nullptr;

//private:
	QAction *m2DAction = nullptr;
	QAction *m3DAction = nullptr;

	//... status-bar
    QStatusBar *mStatusBar = nullptr;

	//... views related
	QSplitter *mMainSplitter = nullptr;


private:

	// domain data

	const CModel *mModel = nullptr;
	const CWorkspaceDataset *mWDataset = nullptr;
	const CWorkspaceOperation *mWOperation = nullptr;
	const CWorkspaceFormula *mWFormula = nullptr;

protected:
	CWorkspaceDisplay *mWDisplay = nullptr;
	std::vector< CDisplay* > mFilteredDisplays;
	std::vector< const COperation* > mFilteredOperations;

	CDisplay *mDisplay = nullptr;
	const COperation *mOperation = nullptr;
	int mRequestedDisplayIndex = 0;				//trick for first requested display

	const CDisplayFilesProcessor *mCurrentDisplayFilesProcessor = nullptr;
	const bool mDisplayOnlyMode;
	const bool mIsMapEditor;

	///////////////////////////////////////////////////////////
	//construction / destruction
	///////////////////////////////////////////////////////////

private:
	void CreateMainToolbar();
	void CreateWorkingDock();
	void CreateGraphicsBar();
	void CreateStatusBar();
	void Wire();

protected:
	template< class GENERAL_TAB >
	void CreateWidgets();

protected:
	CAbstractDisplayEditor( bool map_editor, CModel *model, const COperation *op, const std::string &display_name );
	CAbstractDisplayEditor( bool map_editor, const CDisplayFilesProcessor *proc );

public:
	virtual ~CAbstractDisplayEditor();

protected:

	///////////////////////////////////////////////////////////
	//access
	///////////////////////////////////////////////////////////

	bool IsMapEditor() const { return mIsMapEditor; }

	void SelectTab( QWidget *tab );


	///////////////////////////////////////////////////////////
	// GUI creation/management helpers
	///////////////////////////////////////////////////////////

	QWidget* AddTab( QWidget *tab, const QString title = "" );

	bool AddView( QWidget *view, bool view3D );
	bool RemoveView( QWidget *view, bool view3D, bool disable_action );

	QAction* AddToolBarAction( QObject *parent, EActionTag tag );
	QAction* AddToolBarSeparator();
	QToolButton* AddMenuButton( EActionTag button_tag, const QList<QAction*> &actions );

	QSize sizeHint() const override;

	int DisplayIndex( const CDisplay *display ) const;
	//bool SelectDisplay( const CDisplay *display );

	int OperationIndex( const COperation *op ) const;
	bool Start( const std::string &display_name );

	void SetWindowTitle();


	///////////////////////////////////////////////////////////
	// Virtual interface (enforce some uniformity of behavior)
	///////////////////////////////////////////////////////////

	// Delete existing view widgets and replace by newly created ones
	//	typically preparing for new display
	//
	virtual bool ResetViews( bool enable_2d, bool enable_3d ) = 0;


	// Virtual interface for slot handling, require implementation by specialized classes

	virtual void Show2D( bool display ) = 0;
	virtual void Show3D( bool display ) = 0;

	virtual void NewButtonClicked() = 0;
	virtual void RenameButtonClicked() = 0;
	virtual void DeleteButtonClicked() = 0;

	virtual void OperationChanged( int index ) = 0;

	virtual void OneClick() = 0;

	virtual bool ViewChanged() = 0;


	///////////////////////////////////////////////////////////
	// domain management helpers
	///////////////////////////////////////////////////////////

	bool RefreshDisplayData();

	std::vector< CDisplay* > FilterDisplays( const COperation *operation );
	void FilterDisplays();
	void FilterOperations();

	template< typename PLOT >
	std::vector< PLOT* > GetDisplayPlots( CDisplay *display );

	///. inherited from v3, must be reviewed
	bool ControlSolidColor();
	bool ControlVectorComponents( std::string& msg );


	///////////////////////////////////////////////////////////
	//					Handlers
	///////////////////////////////////////////////////////////

private:
	void UpdateDisplaysCombo( int index );
	void UpdateOperationsCombo();

private slots:

	void Handle2D( bool checked );
	void Handle3D( bool checked );

	void HandleOperationsIndexChanged( int index );
	void HandleOneClickClicked()
	{
		OneClick();
	}

	void RunButtonClicked();
	void HandleViewChanged( int index );

	void HandleNewButtonClicked();
	void HandleRenameButtonClicked();
	void HandleDeleteButtonClicked();

	void HandlePlotTitleEntered();
};




//////////////////////////////////////////////////////////////////////////////////////



template< typename PLOT >
std::vector< PLOT* > CAbstractDisplayEditor::GetDisplayPlots( CDisplay *display )
{
	std::vector< PLOT* > v;

	std::string errors;
	delete mCurrentDisplayFilesProcessor;
	try
	{
		mCurrentDisplayFilesProcessor = new CDisplayFilesProcessor( !IsMapEditor(), display->GetCmdFilePath() );

		auto &plots = mCurrentDisplayFilesProcessor->plots();
		for ( auto *plot : plots )
		{
			PLOT *p = dynamic_cast< PLOT* >( plot );
			if ( p )
			{
				p->GetInfo();
				v.push_back( p );
			}
		}
	}
	catch ( const CException &e )
	{
		errors = e.Message();
		delete mCurrentDisplayFilesProcessor;
		mCurrentDisplayFilesProcessor = nullptr;
	}

	if ( !errors.empty() )
		SimpleWarnBox( errors );

	return v;
}





#endif			// GUI_DISPLAY_EDITORS_ABSTRACT_DISPLAY_EDITOR_H
