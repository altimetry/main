#if !defined GUI_TABBED_DOCK_H
#define GUI_TABBED_DOCK_H

#include "new-gui/Common/+Utils.h"



//////////////////////////////////////////////////////////////////////////
//						Expandable TabWidget
//////////////////////////////////////////////////////////////////////////


class CExpandableTabWidget : public QTabWidget
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	Q_PROPERTY( bool expanding_tabs READ Expanding WRITE SetExpanding );

public:
	CExpandableTabWidget( QWidget *parent = nullptr );

	bool Expanding() const
	{
		return tabBar()->expanding();
	}

	void SetExpanding( bool expanding = true )
	{
		tabBar()->setExpanding( expanding );
	}
};




//////////////////////////////////////////////////////////////////////////
//							Tabbed Dock
//////////////////////////////////////////////////////////////////////////


class CTabbedDock : public QDockWidget
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
		
	using base_t = QDockWidget;


	// data
		
	QWidget *mDockContents = nullptr;
	CExpandableTabWidget *mTabWidget = nullptr;


	// construction / destruction

	void CreateContents( QWidget *top_widget = nullptr );

public:
	CTabbedDock( QWidget *top_widget, const QString &title, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	CTabbedDock( const QString &title, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CTabbedDock()
	{}	


	// access 

	int TabIndex( QWidget *tab ) const
	{
		return mTabWidget->indexOf( tab );
	}

	QWidget* Tab( int tab ) const
	{
		return mTabWidget->widget( tab );
	}

    int SelectedTab() const
	{
		return mTabWidget->currentIndex();
	}

	void SelectTab( int index )
	{
		return mTabWidget->setCurrentIndex( index );
	}


	// operations

	void SetTabShape( QTabWidget::TabShape s );


	// Sets QWidget parameter ("tab_widget") parent
	//
	QWidget* AddTab( QWidget *tab_widget, const QString &title );

	
	QWidget* TabWidget( int index );


	void SetTabToolTip( int index, const QString & tip )
	{
		assert__( Tab( index ) );

		mTabWidget->setTabToolTip( index, tip );
	}


	void SetTabToolTip( QWidget *tab, const QString & tip )
	{
		SetTabToolTip( TabIndex( tab ), tip );
	}
};

#endif	//GUI_TABBED_DOCK_H
