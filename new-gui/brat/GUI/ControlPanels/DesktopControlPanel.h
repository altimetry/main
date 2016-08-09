#if !defined GUI_CONTROL_PANELS_DESKTOP_CONTROL_PANEL_H
#define GUI_CONTROL_PANELS_DESKTOP_CONTROL_PANEL_H

#include "libbrathl/Product.h"

#include "new-gui/Common/GUI/ControlPanel.h"
#include "GUI/DesktopManager.h"
#include "GUI/StackedWidget.h"


class CWorkspaceDataset;
class CWorkspaceOperation;
class CWorkspaceDisplay;
class CModel;



/////////////////////////////////////////////////////////////////////////////////////
//					Control Panel Specialized for Desktop
/////////////////////////////////////////////////////////////////////////////////////


class CDesktopControlsPanel : public CControlPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	//types

	using base_t = CControlPanel;

protected:
	using stack_button_type = CStackedWidget::stack_button_type;


	//instance data

protected:
	CModel &mModel;
	CDesktopManagerBase *mDesktopManager = nullptr;

	//construction / destruction

public:
	explicit CDesktopControlsPanel( CModel &model, CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

	virtual ~CDesktopControlsPanel()
	{}	



	////////////////////////////
	// GUI
	////////////////////////////


	static 
	QgsCollapsibleGroupBox* CreateCollapsibleGroupBox( ELayoutType o, const std::vector< QObject* > &v, const QString &title = "", QWidget *parent = nullptr,
		int spacing = smSpacing, int left = smLeft, int top = smTop, int right = smRight, int bottom = smBottom )
	{
		return ::CreateGroupBox< QgsCollapsibleGroupBox >( o, v, title, parent, spacing, left, top, right, bottom);
	}
};



#endif	//GUI_CONTROL_PANELS_DESKTOP_CONTROL_PANEL_H
