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

	using base_t = QMainWindow;


	// instance data

	//... working dock
	CTabbedDock *mWorkingDock = nullptr;
	//QFrame *mTopFrame = nullptr;
	QComboBox *mOperationsCombo = nullptr;
	QComboBox *mFiltersCombo = nullptr;
	QLineEdit *mDatasetName = nullptr;

	CViewControlsPanelGeneral *mTabGeneral = nullptr;
	//tab general
	QComboBox *mDisplaysCombo = nullptr;
	

	QPushButton *mSaveOneClickButton = nullptr;

	//... tool-bar
protected:
	QToolBar *mToolBar = nullptr;
private:
	QAction *m2DAction = nullptr;
	QAction *m3DAction = nullptr;
	QAction *mLogAction = nullptr;

	//... status-bar
    QStatusBar *mStatusBar = nullptr;

	//... views related
	QSplitter *mMainSplitter = nullptr;
	QSplitter *mGraphicsSplitter = nullptr;
	CTextWidget *mLogText = nullptr;


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


	//construction / destruction

private:
	void CreateWorkingDock();
	void CreateGraphicsBar();
	void CreateStatusBar();
	void Wire();

protected:
	template< class GENERAL_TAB >
	void CreateWidgets();

protected:
	CAbstractDisplayEditor( CModel *model, const COperation *op, const std::string &display_name = "", QWidget *parent = nullptr );
	CAbstractDisplayEditor( bool map_editor, const CDisplayFilesProcessor *proc, QWidget *parent = nullptr );

public:
	virtual ~CAbstractDisplayEditor();

protected:

	// GUI creation/management helpers

	QWidget* AddTab( QWidget *tab, const QString title = "" );

	bool AddView( QWidget *view, bool view3D );
	bool RemoveView( QWidget *view, bool view3D );

	QAction* AddToolBarAction( QObject *parent, EActionTag tag );
	QAction* AddToolBarSeparator();
	QToolButton* AddMenuButton( EActionTag button_tag, const QList<QAction*> &actions );

	QSize sizeHint() const override;

	int DisplayIndex( const CDisplay *display ) const;
	//bool SelectDisplay( const CDisplay *display );

	int OperationIndex( const COperation *op ) const;
	bool Start( const std::string &display_name );


	// Virtual interface to enforce some uniformity of behavior

	// Delete existing view widgets and replace by newly created ones
	//	typically preparing for new display
	//
	virtual void ResetViews() = 0;


	// Virtual interface for slot handling, require implementation by specialized classes

	virtual void Show2D( bool display ) = 0;
	virtual void Show3D( bool display ) = 0;

	virtual void NewButtonClicked() = 0;

	virtual void OperationChanged( int index ) = 0;
	virtual void FilterChanged( int index ) = 0;

	virtual void OneClick() = 0;

	virtual bool Refresh() = 0;


	// domain management helpers

	void FilterDisplays();
	void FilterOperations( bool with_maps );

	template< typename PLOT >
	std::vector< PLOT* > GetDisplayPlots( CDisplay *display );

	///. inherited from v3, must be reviewed
	bool ControlSolidColor();
	bool ControlVectorComponents( std::string& msg );

private:
	void UpdateDisplaysCombo( int index );

private slots:

	void HandleNewButtonClicked()
	{
		NewButtonClicked();
	}

	void Handle2D( bool checked );
	void Handle3D( bool checked );
	void HandleLog( bool checked );

	void HandleViewChanged( int index );

	void HandleOneClickClicked()
	{
		OneClick();
	}
	
	void HandleOperationsIndexChanged( int index );

	void HandleFiltersIndexChanged( int index )
	{
		FilterChanged( index );
	}
};





template< typename PLOT >
std::vector< PLOT* > CAbstractDisplayEditor::GetDisplayPlots( CDisplay *display )
{
	std::vector< PLOT* > v;

	std::string errors;
	delete mCurrentDisplayFilesProcessor;
	try
	{
		mCurrentDisplayFilesProcessor = new CDisplayFilesProcessor( display->GetCmdFilePath() );

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
