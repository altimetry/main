#if !defined GUI_DISPLAY_EDITORS_DIALOGS_EXPORT_IMAGE_DIALOG_H
#define GUI_DISPLAY_EDITORS_DIALOGS_EXPORT_IMAGE_DIALOG_H


#include <QDialog>

#include "new-gui/brat/DataModels/Workspaces/Display.h"



class CExportImageDialog : public QDialog
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

	using EImageExportType = CDisplayData::EImageExportType;

protected:

    /////////////////////////////
    // static data
    /////////////////////////////

	static const std::string* ImageTypeStrings()
	{
		static const std::string names[ CDisplayData::EImageExportType_size ] =
		{
			"tif",
			"bmp",
			"jpg",
			"png",
			"pnm"
		};

		return names;
	}

	static std::string ImageType2String( EImageExportType type )
	{
		static const std::string *names = ImageTypeStrings();

		assert__( type >= 0 && type < CDisplayData::EImageExportType_size );

		return names[ type ];
	}


    /////////////////////////////
    // instance data
    /////////////////////////////

    QLineEdit *mOutputPathLineEdit = nullptr;
	QPushButton *mBrowseButton = nullptr;
    QComboBox *mExtensionCombo = nullptr;
	QPushButton *mSaveButton = nullptr;

    QDialogButtonBox *mButtonBox = nullptr;

	std::string mOutputFileName;
	EImageExportType mOutputFileType = CDisplayData::eTif;


    /////////////////////////////
    //construction / destruction
    /////////////////////////////
private:

    void CreateWidgets();
    void Wire();

public:
    CExportImageDialog( std::string &ouput_path, QWidget *parent );

    virtual ~CExportImageDialog();


    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

	const std::string& OutputFileName() const { return mOutputFileName; }

	EImageExportType OutputFileType() const { return mOutputFileType; }


    /////////////////////////////
    // Operations
    /////////////////////////////

protected:
    virtual void accept() override;

	bool Check();


protected slots:

    void HandleExportExtension(int);
    void HandleChangeExportPath();
};



#endif      // GUI_DISPLAY_EDITORS_DIALOGS_EXPORT_IMAGE_DIALOG_H
