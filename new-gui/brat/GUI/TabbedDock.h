#if !defined GUI_TABBED_DOCK_H
#define GUI_TABBED_DOCK_H

#include "new-gui/Common/+Utils.h"



class CTabbedDock : public QDockWidget
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

		
	using base_t = QDockWidget;


	QWidget *mDockContents = nullptr;
	QTabWidget *mTabWidget = nullptr;

public:
	explicit CTabbedDock( const QString &title, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

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


	// operations

	QWidget* AddTab( QWidget *tab_widget, const QString &title );


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
