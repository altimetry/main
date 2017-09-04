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
#if !defined NC_DUMP_DIALOG_H
#define NC_DUMP_DIALOG_H


#include <QDialog>
#include <QDialogButtonBox>

class CTextWidget;
class COsProcess;


class CNcDumpDialog : public QDialog
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

	QLineEdit *mNcDumpOptionsLineEdit = nullptr;
	QPushButton *mRunAgainButton = nullptr;
	CTextWidget *mDumpTextWidget = nullptr;
	CTextWidget *mHelpText = nullptr;
    QDialogButtonBox *mButtonBox = nullptr;

	const QString mNetcdfFilePath;
	const std::string mNcDumpath;
	const bool mModal;
	COsProcess *mProcess = nullptr;
	const bool mSync = true;


    /////////////////////////////
    //construction / destruction
    /////////////////////////////

	void Setup();
	void CreateWidgets();
public:
    CNcDumpDialog( bool modal, const std::string &ncdump_path, const QString &netcdf_file_path, QWidget *parent );

    virtual ~CNcDumpDialog();


    ///////////////////////////////
    // getters / setters / testers
    ///////////////////////////////


protected:

	void SetHelpText();

	COsProcess* ProcessFinished( int exit_code, QProcess::ExitStatus exitStatus );

		/////////////////////////////
    //		overrides
    /////////////////////////////


	virtual void  accept() override;


protected slots:

	void HandleRunAgainButtonClicked();

	// QProcess report handling

	void HandleUpdateOutput();
	void HandleAsyncProcessFinished( int exit_code, QProcess::ExitStatus exitStatus );
	void HandleSyncProcessFinished( int exit_code, QProcess::ExitStatus exitStatus );
	void HandleProcessError( QProcess::ProcessError error );
};


#endif      // NC_DUMP_DIALOG_H
