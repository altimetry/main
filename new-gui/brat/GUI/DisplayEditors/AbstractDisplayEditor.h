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
class CBratLookupTable;

class CZFXYPlotData;
class CXYPlotData;
class CXYPlotDataCollection;
class CWorldPlotData;


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

	// This class's interface simulates a single CDisplayData
	//
	//	Works as a unit for displaying and assigning properties
	//	- Several plot units can be displayed at the same time
	//	- Properties are displayed and assigned only for the selected plot unit (in GUI fields/layers list)
	//
	class CPlotUnitData : protected std::vector<CDisplayData*> 
	{
		using base_t = std::vector<CDisplayData*>;

		template< typename T >
		using getter_t = T( CDisplayData::* )() const;


		// assignment

		void Assign( CDisplayData *data )
		{
			assert__( size() == 0 );

			push_back( data );
		}

		//CWorldPlotData initializes north first; here we follow the order in which field names form the v3 CWorldPlotProperty name
		//
		void Assign( CDisplayData *east_data, CDisplayData *north_data )
		{
			assert__( size() == 0 );

			push_back( east_data );
			push_back( north_data );
		}

		// Must be called before assignment operator
		void Clear()
		{
			clear();
		}


public:
		CPlotUnitData& operator = ( const CPlotUnitData &o )
		{
			if ( this != &o )
			{
				Clear();				//really not necessary
				base_t::operator=( o );
			}

			return *this;
		}

		CPlotUnitData& operator = ( CDisplayData *data )
		{
			if ( !data )
				Clear();
			else
				Assign( data );

			return *this;
		}

		CPlotUnitData& operator = ( std::pair<CDisplayData*, CDisplayData*> data )
		{
			Assign( data.first, data.second );

			return *this;
		}

		operator bool() const { return size() > 0;  }


	protected:

		template< typename T, typename SETTER >
		void Set( const T &value, SETTER set )
		{
			assert__( size() > 0 );

			(at( 0 )->*set)( value );
			if ( size() > 1 )
				(at( 1 )->*set)( value );
		}

		template< typename T1, typename T2, typename SETTER >
		void Set( const T1 &value1, const T2 &value2, SETTER set )
		{
			assert__( size() > 0 );

			(at( 0 )->*set)( value1, value2 );
			if ( size() > 1 )
				(at( 1 )->*set)( value1, value2 );
		}

		template< typename T >
		T Get( getter_t< T > get )
		{
			assert__( size() > 0 );

			return (at( 0 )->*get)();
		}

	public:

		std::string Name()									
		{	
			std::string name = Get( &CDisplayData::FieldName );			//fieldNameEast + "/" + fieldNameNorth;
			if ( size() > 1 )
				name += ( "/" + at( 1 )->FieldName() );

			return name;
		}

		double GetCurrentMinValue()										{	return Get( &CDisplayData::GetCurrentMinValue );	}
		void SetCurrentMinValue( const double &value )					{	Set( value, &CDisplayData::SetCurrentMinValue );	}
		double GetCurrentMaxValue()										{	return Get( &CDisplayData::GetCurrentMaxValue );	}
		void SetCurrentMaxValue( const double &value )					{	Set( value, &CDisplayData::SetCurrentMaxValue );	}

		double GetAbsoluteMinValue()									{	return Get( &CDisplayData::GetAbsoluteMinValue );	}
		double GetAbsoluteMaxValue()									{	return Get( &CDisplayData::GetAbsoluteMaxValue );	}
		void SetAbsoluteRangeValues( const double &m, const double &M )	{	Set( m, M, &CDisplayData::SetAbsoluteRangeValues );	}

		const std::string& GetColorPalette()							{	return Get( &CDisplayData::GetColorPalette );		}
		void SetColorPalette( const std::string &value )				{	Set( value, &CDisplayData::SetColorPalette );		}

		void SetXAxisText( unsigned index, const std::string& value )	{	Set( index, value, &CDisplayData::SetXAxisText );	}

		unsigned GetNumberOfBins()										{	return Get( &CDisplayData::GetNumberOfBins );	}
		void SetNumberOfBins( unsigned value )							{	Set( value, &CDisplayData::SetNumberOfBins );	}
		
		void SetContour( bool value )									{	Set( value, &CDisplayData::SetContour );	}
		void SetSolidColor( bool value )								{	Set( value, &CDisplayData::SetSolidColor );	}
	};



private:
	///////////////////////////////////////////////////////////
	// static members
	///////////////////////////////////////////////////////////

protected:
	//int32_t CDate::GetDateRef(CDate& date, brathl_refDate& refDate)

	static brathl_refDate RefDateFromUnit( const CUnit &u );


	static CZFXYPlotData* GetFieldData( size_t field_index, std::vector< CZFXYPlotData* > &zfxy_data_array );
	static CXYPlotData* GetFieldData( size_t field_index, CXYPlotDataCollection &yfx_data_collection );
	static CWorldPlotData* GetFieldData( size_t field_index, std::vector< CWorldPlotData* > &lon_lat_data_array );

	static CZFXYPlotProperties* GetProperties( size_t field_index, std::vector< CZFXYPlotData* > &zfxy_data_array );
	static CXYPlotProperties* GetProperties( size_t field_index, CXYPlotDataCollection &yfx_data_collection );
	static CWorldPlotProperties* GetProperties( size_t field_index, std::vector< CWorldPlotData* > &lon_lat_data_array );

	static CPlotUnitData GetDisplayData( size_t field_index, const COperation *operation, CDisplay *display, std::vector< CZFXYPlotData* > &zfxy_data_array );
	static CPlotUnitData GetDisplayData( size_t field_index, const COperation *operation, CDisplay *display, CXYPlotDataCollection &yfx_data_collection );
	static CPlotUnitData GetDisplayData( size_t field_index, const COperation *operation, CDisplay *display, std::vector< CWorldPlotData* > &lon_lat_data_array );

	
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

#if defined (SHOW_TEST)
	QAction *mActionTest = nullptr;
#endif

	//... status-bar
    QStatusBar *mStatusBar = nullptr;

	//... views related
	QSplitter *mMainSplitter = nullptr;


private:

	// domain data

	const CWorkspaceDataset *mWDataset = nullptr;
	const CWorkspaceOperation *mWOperation = nullptr;
	const CWorkspaceFormula *mWFormula = nullptr;

protected:
	const CModel *mModel = nullptr;
	CWorkspaceDisplay *mWDisplay = nullptr;
	std::vector< CDisplay* > mFilteredDisplays;
	std::vector< const COperation* > mFilteredOperations;

	CDisplay *mDisplay = nullptr;
	const COperation *mOperation = nullptr;
	int mRequestedDisplayIndex = 0;				//trick for first requested display


	CBratLookupTable *mCurrentBratLookupTable = nullptr;
	CPlotUnitData mCurrentPlotUnit;


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

	virtual void NewButtonClicked() = 0;
	virtual void RenameButtonClicked() = 0;
	virtual void DeleteButtonClicked() = 0;

	virtual void OperationChanged( int index ) = 0;

	virtual void OneClick() = 0;

	virtual bool ChangeView() = 0;

	//debug helpers
	template< typename T >
	inline std::string TF( const std::string &s, T n )
	{
		std::string dv;
		if ( isDefaultValue( n ) )
			dv = "!!! DV !!!";

		return s + " == " + n2s<std::string>( n ) + dv + "\n";
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
	std::vector< PLOT* > GetPlotsFromDisplayFile( bool maps_as_plots, const std::vector< std::string > &args );

	template< typename PLOT >
	std::vector< PLOT* > GetPlotsFromDisplayFile( CDisplay *display )
	{
		std::vector< std::string > v;
		v.push_back( "" );
		v.push_back( display->GetCmdFilePath() );

		return GetPlotsFromDisplayFile< PLOT >( !IsMapEditor(), v );
	}

	template< typename PLOT >
	std::vector< PLOT* > GetPlotsFromDisplayFile( const COperation *operation )
	{
		std::vector< std::string > v;
		v.push_back( "" );
		v.push_back( operation->GetOutputPath() );

		return GetPlotsFromDisplayFile< PLOT >( !IsMapEditor(), v );
	}

public:

	///. inherited from v3, must be reviewed
	bool ControlSolidColor();
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
std::vector< PLOT* > CAbstractDisplayEditor::GetPlotsFromDisplayFile( bool maps_as_plots, const std::vector< std::string > &args )
{
	std::vector< PLOT* > v;

	//std::string errors;
	delete mCurrentDisplayFilesProcessor;
	try
	{
		mCurrentDisplayFilesProcessor = new CDisplayFilesProcessor( maps_as_plots, args );

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
	catch ( const CException & )
	{
		//errors = e.Message();
		delete mCurrentDisplayFilesProcessor;
		mCurrentDisplayFilesProcessor = nullptr;
		throw;		//propagate outside
	}

	//if ( !errors.empty() )
	//	SimpleWarnBox( errors );

	return v;
}




//////////////////////////////////////////////////////////////////////////////////////



template< typename T >
inline bool VirtuallyEqual( T a, T b )
{
	//return std::abs( a - b ) < std::numeric_limits< T >::epsilon();
	return true;
}



#endif			// GUI_DISPLAY_EDITORS_ABSTRACT_DISPLAY_EDITOR_H
