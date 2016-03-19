#if !defined GUI_CONTROLPANELS_DIALOGS_SELECT_RECORD_DIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_SELECT_RECORD_DIALOG_H


#include <QDialog>

namespace brathl 
{
	class CProduct;
}



class CSelectRecordDialog : public QDialog
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    /////////////////////////////
    //	types
    /////////////////////////////

    using base_t = QDialog;

    /////////////////////////////
    // static data
    /////////////////////////////

    /////////////////////////////
    // instance data
    /////////////////////////////

	QListWidget *mRecordsList = nullptr;

    QDialogButtonBox *mButtonBox = nullptr;

	std::string mSelectedRecord;


	//...domain data

	CProduct *mProduct = nullptr;


    /////////////////////////////
    //construction / destruction
    /////////////////////////////
private:

    void CreateWidgets();
    void Wire();

public:
    CSelectRecordDialog( QWidget *parent, CProduct *product  );

    virtual ~CSelectRecordDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////


	const std::string& SelectedRecord() const { return mSelectedRecord; }

    /////////////////////////////
    // Operations
    /////////////////////////////

protected:
    virtual void accept() override;

protected slots:

	void HandleRecordsListSelection( int index );

};


#endif      // GUI_CONTROLPANELS_DIALOGS_SELECT_RECORD_DIALOG_H
