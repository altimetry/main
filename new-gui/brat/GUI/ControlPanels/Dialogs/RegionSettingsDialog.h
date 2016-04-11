#if !defined GUI_CONTROLPANELS_DIALOGS_REGIONSETTINGSDIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_REGIONSETTINGSDIALOG_H


#include <QDialog>

#include "new-gui/brat/DataModels/Filters/BratFilters.h"



//------------------------------------------------------------------------------------
// CRegionSettingsDialog
//------------------------------------------------------------------------------------
class CRegionSettingsDialog : public QDialog
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

    QListWidget *mAreasListWidget = nullptr;
    QComboBox *mRegionsCombo = nullptr;

    QDialogButtonBox *mButtonBox = nullptr;

    QToolButton *mNewRegion = nullptr;
    QToolButton *mRenameRegion = nullptr;
    QToolButton *mDeleteRegion = nullptr;
    //QToolButton *mSaveRegion = nullptr;

    CBratRegions &mBratRegions;
    CBratAreas &mBratAreas;

    CRegion *mCurrentRegion = nullptr;

    /////////////////////////////
    //construction / destruction
    /////////////////////////////
private:

    void CreateWidgets();
    void Wire();

public:
    CRegionSettingsDialog( QWidget *parent, CBratRegions &BratRegions, CBratAreas &BratAreas );

    virtual ~CRegionSettingsDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

    /////////////////////////////
    // Operations
    /////////////////////////////

protected:
    void FillRegionsCombo();
    void FillAreasList();
    void SaveAllRegions();

public slots:
    void HandleNewRegion();
    void HandleRenameRegion();
    void HandleDeleteRegion();
    //void HandleSaveRegion();
    void HandleRegionsCurrentIndexChanged(int region_index);
    //void HandleAreaChecked(QListWidgetItem *area_item);
    void HandleAreaClicked(QListWidgetItem *area_item);

protected:
    virtual void accept() override;

};




#endif      // GUI_CONTROLPANELS_DIALOGS_REGIONSETTINGSDIALOG_H
