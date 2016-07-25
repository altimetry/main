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
#ifndef GUI_DISPLAY_EDITORS_ABSTRACT_DISPLAY_EDITOR_H
#define GUI_DISPLAY_EDITORS_ABSTRACT_DISPLAY_EDITOR_H


#include <QMainWindow>

#include "new-gui/Common/QtUtils.h"

#include "DataModels/DisplayFilesProcessor.h"
#include "DataModels/DisplayDataProcessor.h"
#include "DataModels/Workspaces/Display.h"
#include "DataModels/PlotData/PlotData.h"


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
class CBratLookupTable;

class CZFXYPlotData;
class CXYPlotData;
class CXYPlotDataCollection;
class CWorldPlotData;
class CGeoPlot;
class CYFXPlot;
class CZFXYPlot;


//#define USE_DISPLAY_FILES_PROCESSOR
#if defined( USE_DISPLAY_FILES_PROCESSOR )
using display_data_processor_t = CDisplayFilesProcessor;
#else
using display_data_processor_t = CDisplayDataProcessor;
#endif


enum EActionTag : int;


#if defined (DEBUG) || defined(_DEBUG)
#	define SHOW_TEST
#endif


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

protected:

	using EImageExportType = CDisplayData::EImageExportType;


private:
	///////////////////////////////////////////////////////////
	// static members
	///////////////////////////////////////////////////////////

protected:
	//int32_t CDate::GetDateRef(CDate& date, brathl_refDate& refDate)

	static brathl_refDate RefDateFromUnit( const CUnit &u );

	static QListWidgetItem* MakeFieldItem( const CPlotData *pdata );

	
private:
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
	QAction *mRecenterAction = nullptr;
	QAction *mExport2ImageAction = nullptr;

	QAction *mActionTest = nullptr;

	//... status-bar
    QStatusBar *mStatusBar = nullptr;

	//... views related
	QSplitter *mMainSplitter = nullptr;


private:

	// domain data

	const CWorkspaceDataset *mWDataset = nullptr;
	const CWorkspaceOperation *mWOperation = nullptr;
	const CWorkspaceFormula *mWFormula = nullptr;

	display_data_processor_t *mCurrentDisplayDataProcessor = nullptr;

protected:
	const CModel *mModel = nullptr;
	CWorkspaceDisplay *mWDisplay = nullptr;
	std::vector< CDisplay* > mFilteredDisplays;
	std::vector< const COperation* > mFilteredOperations;

	CDisplay *mDisplay = nullptr;
	const COperation *mOperation = nullptr;
	int mRequestedDisplayIndex = 0;				//trick for first requested display


	CGeoPlot *mCurrentPlotGeo = nullptr;
	CYFXPlot *mCurrentPlotYFX = nullptr;
	CZFXYPlot *mCurrentPlotZFXY = nullptr;

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
	CAbstractDisplayEditor( bool map_editor, const CDisplayDataProcessor *proc );
	CAbstractDisplayEditor( bool map_editor, const CDisplayFilesProcessor *proc );

public:
	virtual ~CAbstractDisplayEditor();

protected:

	///////////////////////////////////////////////////////////
	//access
	///////////////////////////////////////////////////////////

	const display_data_processor_t *CurrentDisplayDataProcessor() const { return mCurrentDisplayDataProcessor; }

	bool IsMapEditor() const { return mIsMapEditor; }

	void SelectTab( QWidget *tab );

	const std::string& UserDataDirectory() const;


	///////////////////////////////////////////////////////////
	// GUI creation/management helpers
	///////////////////////////////////////////////////////////

	QWidget* AddTab( QWidget *tab, const QString title = "" );

	bool AddView( QWidget *view, bool view3D );
	bool RemoveView( QWidget *view, bool view3D, bool disable_action );

	QAction* AddToolBarAction( QObject *parent, EActionTag tag );
	QAction* AddToolBarSeparator( QAction *after = nullptr );
	QToolButton* AddMenuButton( EActionTag button_tag, const QList<QAction*> &actions, QAction *after = nullptr );

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
	// This is a (quick) design enforcement, not a polymorphic construct
	//
	virtual bool ResetViews( bool reset_2d, bool reset_3d, bool enable_2d, bool enable_3d ) = 0;


	// Virtual interface for slot handling, require implementation by specialized classes

	virtual void Show2D( bool display ) = 0;
	virtual void Show3D( bool display ) = 0;
	virtual void Recenter() = 0;

public:
	virtual void Export2Image( bool save_2d, bool save_3d, const std::string path2d, const std::string path3d, EImageExportType type ) = 0;

protected:

	virtual void SetPlotTitle() = 0;

	virtual void NewButtonClicked() = 0;
	virtual void RenameButtonClicked() = 0;
	virtual void DeleteButtonClicked() = 0;

	virtual void OperationChanged( int index ) = 0;

	virtual void OneClick() = 0;

	virtual bool ChangeView() = 0;									//when display changes
	virtual bool UpdateCurrentPointers( int field_index ) = 0;		//when field changes: update mCurrentBratLookupTable and any class "current" pointers

	//debug helpers
	template< typename T >
	inline std::string TF( const std::string &s, T n, const std::string &separator = "\n" )
	{
		std::string dv;
		if ( isDefaultValue( n ) )
			dv = "!!! DV !!!";

		return s + " == " + n2s<std::string>( n ) + dv + separator;
	};

	virtual bool Test() = 0;


	///////////////////////////////////////////////////////////
	// domain management helpers
	///////////////////////////////////////////////////////////

	bool RefreshDisplayData();

	std::vector< CDisplay* > FilterDisplays( const COperation *operation );
	void FilterDisplays();
	void FilterOperations();

protected:
	template< typename PLOT >
	std::vector< PLOT* > GetPlotsFromDisplayData( bool maps_as_plots, const std::vector< std::string > &args );

	template< typename PLOT >
	std::vector< PLOT* > GetPlotsFromDisplayData( CDisplay *display )
	{
		std::vector< std::string > v;
		v.push_back( "" );
		v.push_back( display->GetCmdFilePath() );

		return GetPlotsFromDisplayData< PLOT >( !IsMapEditor(), v );
	}

#if defined( USE_DISPLAY_FILES_PROCESSOR )
	template< typename PLOT >
	std::vector< PLOT* > GetPlotsFromDisplayData( const COperation *operation )
	{
		std::vector< std::string > v;
		v.push_back( "" );
		v.push_back( operation->GetOutputPath() );

		return GetPlotsFromDisplayData< PLOT >( !IsMapEditor(), v );
	}
#endif

public:

	///. inherited from v3, must be reviewed
	//bool ControlSolidColor();	- bug mask
	bool ControlVectorComponents( std::string& msg );


	///////////////////////////////////////////////////////////
	//					Handlers
	///////////////////////////////////////////////////////////

private:
	void UpdateDisplaysCombo( int index );
	void UpdateOperationsCombo();

protected slots:
	void RunButtonClicked();

public slots:

	void Handle2D( bool checked );    
	void Handle3D( bool checked );

private slots:

    void HandleRecenter();
	void HandleExport2Image();
	void HandleTest()
	{
		Test();
	}

	void HandleOperationsIndexChanged( int index );
	void HandleOneClickClicked()
	{
		OneClick();
	}

	//void RunButtonClicked();
	void HandleViewChanged( int index );

	void HandleNewButtonClicked();
	void HandleRenameButtonClicked();
	void HandleDeleteButtonClicked();

	void HandlePlotTitleEntered();
};




//////////////////////////////////////////////////////////////////////////////////////



template< typename PLOT >
std::vector< PLOT* > CAbstractDisplayEditor::GetPlotsFromDisplayData( bool maps_as_plots, const std::vector< std::string > &args )
{
	std::vector< PLOT* > v;														 Q_UNUSED( args );

	//std::string errors;
	delete mCurrentDisplayDataProcessor;
	try
	{
#if defined( USE_DISPLAY_FILES_PROCESSOR )
		mCurrentDisplayDataProcessor = new display_data_processor_t( maps_as_plots, args );
#else
		mCurrentDisplayDataProcessor = new display_data_processor_t( maps_as_plots, mDisplay );
#endif
		auto &plots = mCurrentDisplayDataProcessor->BuildPlots();
		for ( auto *plot : plots )
		{
			PLOT *p = dynamic_cast< PLOT* >( plot );
			if ( p )
			{
				v.push_back( p );
			}
		}
	}
	catch ( const CException & )
	{
		//errors = e.Message();
		delete mCurrentDisplayDataProcessor;
		mCurrentDisplayDataProcessor = nullptr;
		throw;		//propagate outside
	}

	return v;
}




#endif			// GUI_DISPLAY_EDITORS_ABSTRACT_DISPLAY_EDITOR_H
