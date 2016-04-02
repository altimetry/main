#if !defined GUI_ACTIVE_VIEWS_DIALOG_H
#define GUI_ACTIVE_VIEWS_DIALOG_H

#include <QDialog>

#include "DesktopManager.h"


class CActiveViewsDialog : public QDialog
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
	QPushButton *mCloseDialogButton = nullptr;
	QPushButton *mCloseWindowButton = nullptr;
	QPushButton *mSelectWindowButton = nullptr;

	QList< QWidget* > mSubWindows;

	CDesktopManagerBase *mDesktopManager = nullptr;

public:

	/////////////////////////////
	//	Construction /Destruction
	/////////////////////////////

private:

	void CreateWidgets();
	void Wire();

public:
	explicit CActiveViewsDialog( QWidget *parent, CDesktopManagerBase *manager );

    ~CActiveViewsDialog();


	QList< QWidget* > SubWindows() { return mSubWindows; }


protected:
	virtual QSize sizeHint() const override;

	virtual void accept() override;

	void RefreshWindowsList();

protected slots:

	void HandleCloseWindow();
	void HandleSelectWindow();
};


#endif	//GUI_ACTIVE_VIEWS_DIALOG_H
