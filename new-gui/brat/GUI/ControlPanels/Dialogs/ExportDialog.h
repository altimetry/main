#if !defined GUI_CONTROLPANELS_DIALOGS_EXPORTDIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_EXPORTDIALOG_H


#include <QDialog>

#include "GUI/StackedWidget.h"


//------------------------------------------------------------------------------------
// CExportDialog
//------------------------------------------------------------------------------------
class CExportDialog : public QDialog
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


public:

    enum EExportFormat
    {
        eASCII,
        eNETCDF,
        eGEOTIFF
    };


    /////////////////////////////
    // static data
    /////////////////////////////
//OLD BRAT CODE //////////////////////////////////
//  static int32_t m_defaultDecimalPrecision;


    /////////////////////////////
    // instance data
    /////////////////////////////

    CStackedWidget *mStackedWidget = nullptr;

	QFrame *mFormatASCIIPage = nullptr;
	QFrame *mFormatNetCdfPage = nullptr;
	QFrame *mFormatGeoTiffPage = nullptr;

	QCheckBox *mDeliverDataAsPeriod = nullptr;
	QCheckBox *mExpandArrays = nullptr;
	QCheckBox *mExecuteOperationAscii = nullptr;
	QCheckBox *mExecuteOperationNetCdf = nullptr;
	QCheckBox *mOnlyDumpExpressions = nullptr;

	QLineEdit *mASCIINumberPrecision = nullptr;

	QCheckBox *mCreateGoogleKMLFile = nullptr;
	QComboBox *mColorTableCombo = nullptr;
	QLineEdit *mColorRangeMin = nullptr;
	QLineEdit *mColorRangeMax = nullptr;
	QPushButton *mCalculateMinMax = nullptr;

	QPushButton *mBrowseButton = nullptr;

    QDialogButtonBox *mButtonBox = nullptr;

//OLD BRAT CODE /////////////////////////////////////////
//    CLabeledTextCtrl* GetExportAsciiNumberPrecision()  { return (CLabeledTextCtrl*) FindWindow( ID_EXPORT_ASCII_NUMBER_PRECISION ); }
//    wxCheckBox* GetExportNoDataComputation()  { return (wxCheckBox*) FindWindow( ID_EXPORT_NO_DATA_COMPUTATION ); }
//    wxCheckBox* GetExpandArray()  { return (wxCheckBox*) FindWindow( ID_EXPAND_ARRAY ); }
//    wxCheckBox* GetDateAsPeriod()  { return (wxCheckBox*) FindWindow( ID_DATE_AS_PERIOD ); }
//    wxCheckBox* GetExecagain()  { return (wxCheckBox*) FindWindow( ID_EXECAGAIN ); }
//    wxRadioBox* GetExportformat()  { return (wxRadioBox*) FindWindow( ID_EXPORTFORMAT ); }
//    wxButton* GetExportBrowse()  { return (wxButton*) FindWindow( ID_EXPORT_BROWSE ); }
//    wxButton* GetCalcColorRange()  { return (wxButton*) FindWindow( ID_EXPORT_CALC_COLOR_RANGE ); }
//    wxTextCtrl* GetExportoutputfile()  { return (wxTextCtrl*) FindWindow( ID_EXPORTOUTPUTFILE ); }
//    wxCheckBox* GetCreateKMLFile()  { return (wxCheckBox*) FindWindow( ID_EXPORT_CREATE_KML_FILE ); }
//    wxComboBox* GetColorTable()  { return (wxComboBox*) FindWindow( ID_EXPORT_COLOR_TABLE ); }
//    wxStaticText* GetColorTableLabel()  { return (wxStaticText*) FindWindow( ID_EXPORT_COLOR_TABLE_LABEL ); }
//    CLabeledTextCtrl* GetColorRangeMin()  { return (CLabeledTextCtrl*) FindWindow( ID_EXPORT_COLORRANGE_MIN ); }
//    CLabeledTextCtrl* GetColorRangeMax()  { return (CLabeledTextCtrl*) FindWindow( ID_EXPORT_COLORRANGE_MAX ); }

//    COperation* m_operation;

//    wxFileName m_currentName;
//    wxFileName m_initialName;

//    int32_t m_format;

//    CDelayDlg m_delayDlg;
//    bool m_delayExecution;

//    bool m_executeAgain;
//    bool m_dateAsPeriod;
//    bool m_expandArray;
//    bool m_noDataComputation;

//    int32_t m_asciiNumberPrecision;

//    bool m_isGeoImage;
//    bool m_createKML;
//    wxString m_colorTable;
//    double m_colorRangeMin;
//    double m_colorRangeMax;

//    const CStringMap* m_aliases;

//    static wxString m_defaultExtensionAscii;
//    static wxString m_defaultExtensionNetCdf;
//    static wxString m_defaultExtensionGeoTiff;

    /////////////////////////////
    //construction / destruction
    /////////////////////////////
private:

    void CreateWidgets();
    void Wire();
    //OLD BRAT CODE //////////////////////////////
    //void CalculateColorValueRange( void );

public:
    CExportDialog( QWidget *parent );
    //OLD BRAT CODE //////////////////////////////////////
    //    ( wxWindow *parent, wxWindowID id, const wxString &title,
    //          COperation* operation, const CStringMap* aliases = NULL,
    //          const wxPoint& pos = wxDefaultPosition,
    //          const wxSize& size = wxDefaultSize,
    //          long style = wxDEFAULT_DIALOG_STYLE );

    virtual ~CExportDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

    /////////////////////////////
    // Operations
    /////////////////////////////
    //OLD BRAT CODE ///////////////////////////////
//    void Format( int32_t format );
//    bool AsNetCdf() { return (m_format == CExportDlg::AS_NETCDF); };
//    bool AsAscii() { return (m_format == CExportDlg::AS_ASCII); };
//    bool AsGeoTiff() { return (m_format == CExportDlg::AS_GEOTIFF); };

protected:
    virtual void accept() override;

	void DelayExecution();
	void Execute();

protected slots:
    // NEW CODE - TO BE IMPLEMENTED /////////////
//    void HandleDelayExecution();
//    void HandleNoDataComputation();
//    void HandleExpandArray();
//    void HandleDateAsPeriod();
//    void HandleExecuteAgain();
//    void HandleFormat();
//    void HandleBrowse();
//    void HandleCalcColorRange();
//    void HandleOk();
//    void HandleCancel();


	void HandleButtonClicked( QAbstractButton *button );
};



#endif      // GUI_CONTROLPANELS_DIALOGS_EXPORTDIALOG_H
