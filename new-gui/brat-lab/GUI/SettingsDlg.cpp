#include "stdafx.h"

#include "SettingsDlg.h"

SettingsDlg::SettingsDlg( QWidget *parent ) : QDialog(parent), m_bs( GetBackServices() )
{
    setupUi(this);

    //this will issue no  actions besides updating the GUI: see comment inside on_OutputNow_pushButton_toggled
    //
    CurrentServerExists_textEdit->setLayoutDirection( Qt::RightToLeft);
    CurrentServerExists_textEdit->setTextColor( Qt::GlobalColor::darkRed );
    CurrentServerExists_textEdit->setReadOnly( true );
}

bool SettingsDlg::ValidateAndAssign()
{
//    if ( !m_bs.SetStartOutputServer( UseOutputServer_checkBox->isChecked(), OutputServer_lineEdit->text().toStdString() ) )
//    {
//        QMessageBox::StandardButton b =
//                QMessageBox::critical(this, tr("Output Server Settings Error"),
//                                      "Error changing the output server settings: please, check the server executable location.",
//                                      QMessageBox::Ok | QMessageBox::Cancel );
//        if ( b == QMessageBox::Ok )
//            return false;
//    }
    return true;
}

void SettingsDlg::accept()
{
    if ( ValidateAndAssign() )
        QDialog::accept();
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_SettingsDlg.cpp"
