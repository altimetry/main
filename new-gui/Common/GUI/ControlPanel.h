#if !defined COMMON_GUI_CONTROL_PANEL_H
#define COMMON_GUI_CONTROL_PANEL_H

#include "new-gui/Common/QtUtils.h"

class QgsCollapsibleGroupBox;



/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//							Control Panels Base Class
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

using controls_panel_base_t = QWidget;


class CControlPanel : public controls_panel_base_t
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	///////
	//types
	///////

	using base_t = controls_panel_base_t;

protected:

	//using stack_button_type = CStackedWidget::stack_button_type;


protected:
	// For use with CreateBooleanList
	//
	struct BooleanListItemInfo
	{
		std::string mName;
		bool mChecked;

		BooleanListItemInfo( const std::string &name, bool checked = false )
			: mName( name ), mChecked( checked )
		{}
	};

	////////////////
	// static data
	////////////////

	static const int smSpacing = default_spacing;
	static const int smLeft = default_left;
	static const int smTop = default_top;
	static const int smRight = default_right;
	static const int smBottom = default_bottom;


	////////////////
	// instance data
	////////////////

protected:
	const int m = smLeft;
	const int s = smSpacing;

private:
	QVBoxLayout *mMainLayout = nullptr;

	////////////////////////////
	//construction / destruction
	////////////////////////////

public:
	explicit CControlPanel( QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CControlPanel()
	{}	

protected:
	////////////////////////////
	// GUI
	////////////////////////////

	int MaxSmallListsHeight( int items = 4 ) const
	{
		return fontMetrics().lineSpacing() * items + 10;		//very rough 
	}


	QBoxLayout* LayoutWidgets( Qt::Orientation o, const std::vector< QObject* > &v, QWidget *parent = nullptr,
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom )
	{
		assert__( parent != this );

		return ::LayoutWidgets( o, v, parent, spacing, left, top, right, bottom );
	}

	QGridLayout* LayoutWidgets( const std::vector< QObject* > &v, QWidget *parent = nullptr,
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom )
	{
		assert__( parent != this );

		return ::LayoutWidgets( v, parent, spacing, left, top, right, bottom );
	}

	static 
	QGroupBox* CreateGroupBox( ELayoutType o, const std::vector< QObject* > &v, const QString &title = "", QWidget *parent = nullptr,
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom )
	{
		return ::CreateGroupBox< QGroupBox >( o, v, title, parent, spacing, left, top, right, bottom );
	}


	static QListWidget* CreateBooleanList( QWidget *parent, std::initializer_list< BooleanListItemInfo > il );



	QSpacerItem* AddTopSpace( int w = 0, int h = 0, QSizePolicy::Policy hData = QSizePolicy::Minimum, QSizePolicy::Policy vData = QSizePolicy::Minimum );

	QObject* AddTopWidget( QObject *ob );

	QLayout* AddTopLayout( ELayoutType o, const std::vector< QObject* > &v, 
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom );

	QGroupBox* AddTopGroupBox( ELayoutType o, const std::vector< QObject* > &v, const QString &title = "",
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom );

	QSplitter* AddTopSplitter( Qt::Orientation o, const std::vector< QWidget* > &v, bool collapsible = false, const QList< int > sizes = QList<int>() );
};


#endif	//COMMON_GUI_CONTROL_PANEL_H
