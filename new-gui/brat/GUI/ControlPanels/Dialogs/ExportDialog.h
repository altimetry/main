/*
* This file is part of BRAT 
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#if !defined GUI_CONTROLPANELS_DIALOGS_EXPORTDIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_EXPORTDIALOG_H


#include <QDialog>

#include "GUI/StackedWidget.h"

class COperation;
class CColorMapWidget;


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
        eGEOTIFF,

		EExportFormat_size
    };

	using EExecutionType = COperation::EExecutionType;


protected:

    /////////////////////////////
    // static data
    /////////////////////////////

	static const int smDefaultDecimalPrecision = 10;

    static const std::string smDefaultExtensionAscii;
    static const std::string smDefaultExtensionNetCdf;
    static const std::string smDefaultExtensionGeoTiff;
	static const std::string smDefaultExtensionKML;


    /////////////////////////////
    // instance data
    /////////////////////////////

    CStackedWidget *mStackedWidget = nullptr;

	QFrame *mFormatASCIIPage = nullptr;
	QFrame *mFormatNetCdfPage = nullptr;
	QFrame *mFormatGeoTiffPage = nullptr;
	QToolButton *mFormatGeoTiffButton = nullptr;

	QCheckBox *mDeliverDataAsPeriod = nullptr;
	QCheckBox *mExpandArrays = nullptr;

	QCheckBox *mOnlyDumpExpressions = nullptr;

	QLineEdit *mASCIINumberPrecisionLineEdit = nullptr;

	CBratLookupTable *mLUT = nullptr;
	CColorMapWidget *mColorMapWidget = nullptr;
	QGroupBox *mCreateGoogleKMLFileGroup = nullptr;
	QCheckBox *KMLAlongTrackDataCheck = nullptr;
	QCheckBox *KMLFieldsDataCheck = nullptr;
	QCheckBox *mCreateGeoTiffFilesCheck = nullptr;

	QPushButton *mBrowseButton = nullptr;

    QDialogButtonBox *mButtonBox = nullptr;

    QLineEdit *mOutputPathLineEdit = nullptr;

	QPushButton *mDelayExecutionButton = nullptr;
	QCheckBox *mDelayExecutionCheckBox = nullptr;

	QPushButton *mExecuteButton = nullptr;

	std::string mCurrentAsciiOutputFileName;
	std::string mCurrentGeoTIFFOutputFileName;
	std::string mCurrentNetcdfOutputFileName;
//    std::string m_initialName;

    bool mDelayExecution = false;
    //std::string mTaskLabel;
    QDateTime mDateTime;

//    bool m_dateAsPeriod;
//    bool m_expandArray;
    //bool mOnlyDumpExpressions == NoDataComputation = false;

	int mASCIINumberPrecision = smDefaultDecimalPrecision;

	bool mIsGeoImage = false;

    bool mCreateGeoTIFFs;
	bool mCreateKMLTrackData;
    bool mCreateKMLFieldsData;
    std::string mColorTable;
    double mColorRangeMin = defaultValue<double>();
    double mColorRangeMax = defaultValue<double>();

	CWorkspaceOperation *mWOperation = nullptr;
	COperation *mOperation = nullptr;
    const CStringMap *mAliases = nullptr;
	const std::string mLogoPath;

    EExportFormat mExportType = eASCII;

    /////////////////////////////
    //construction / destruction
    /////////////////////////////
private:

	void CalculateMinMax();
    void CreateWidgets();
    void Wire();

public:
    CExportDialog( const std::string logo_path, CWorkspaceOperation *wkso, COperation *operation, const CStringMap *aliases, QWidget *parent );
    //OLD BRAT CODE //////////////////////////////////////
    //    ( wxWindow *parent, wxWindowID id, const std::string &title,
    //          COperation* operation, const CStringMap* aliases = NULL,
    //          const wxPoint& pos = wxDefaultPosition,
    //          const wxSize& size = wxDefaultSize,
    //          long style = wxDEFAULT_DIALOG_STYLE );

    virtual ~CExportDialog();


    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

	EExportFormat ExportFormat() const { return mExportType; }


    bool DelayExecution() const { return mDelayExecution; }


	const std::string& OutputPath() const
	{
		switch ( mExportType )
		{
			case eASCII:
				return mCurrentAsciiOutputFileName;
				break;
			case eNETCDF:
				return mCurrentNetcdfOutputFileName;
				break;
			case eGEOTIFF:
				return mCurrentGeoTIFFOutputFileName;
				break;
			default:
				assert__( false );
		}
		return empty_string<std::string>();
	}


	QDateTime DateTime() const { return mDateTime; }



    /////////////////////////////
    // Operations
    /////////////////////////////

protected:
    virtual void accept() override;

	bool Validate();

protected slots:

	void HandleColorTablesIndexChanged( int );
	void HandleExportType(int);
    void HandleChangeExportPath();
	void HandleDelayExecution();

	void HandleCreateGeoTiffFilesChecked( bool );
	void HandleCreateGoogleKMLFileChecked( bool );
	void HandleKMLFieldsDataChecked( bool );
	void HandleKMLAlongTrackDataChecked( bool );
};



#endif      // GUI_CONTROLPANELS_DIALOGS_EXPORTDIALOG_H
