#if !defined GUI_WORKSPACE_ELEMENTS_DIALOG_H
#define GUI_WORKSPACE_ELEMENTS_DIALOG_H


#include <QDialog>

#include "new-gui/brat/DataModels/Workspaces/TreeWorkspace.h"

class CTreeWorkspace;



class CWorkspaceElementsDialog : public QDialog
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif


	/////////////////////////////
	//	Types
	/////////////////////////////

	using base_t = QDialog;


	/////////////////////////////
	//	Instance Data
	/////////////////////////////

	QTreeWidget *mTreeWidget = nullptr;
	QDialogButtonBox *mButtonBox = nullptr;

	const CTreeWorkspace &mTree;

public:

	/////////////////////////////
	//	Construction /Destruction
	/////////////////////////////

private:

	void CreateGUI();

public:
	explicit CWorkspaceElementsDialog( QWidget *parent, const CTreeWorkspace &wks );

    ~CWorkspaceElementsDialog();
};


#endif	//GUI_WORKSPACE_ELEMENTS_DIALOG_H
