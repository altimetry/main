#if !defined GUI_WORKSPACE_VIEWS_DIALOG_H
#define GUI_WORKSPACE_VIEWS_DIALOG_H


#include <QDialog>

#include "DataModels/Workspaces/TreeWorkspace.h"


class CModel;



class CWorkspaceViewsDialog : public QDialog
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

	QListWidget *mViewsListWidget = nullptr;
	QDialogButtonBox *mButtonBox = nullptr;

	CDisplay *mSelectedDisplay = nullptr;

	CModel &mModel;

public:

	/////////////////////////////
	//	Construction /Destruction
	/////////////////////////////

private:

	void CreateWidgets();
	void Wire();

public:
	explicit CWorkspaceViewsDialog( QWidget *parent, CModel &model );

    ~CWorkspaceViewsDialog();


	CDisplay* SelectedDisplay() { return mSelectedDisplay; }


protected:
	virtual QSize sizeHint() const override;

	virtual void accept() override;
};


#endif	//GUI_WORKSPACE_VIEWS_DIALOG_H
