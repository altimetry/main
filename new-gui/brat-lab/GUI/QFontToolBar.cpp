#include "stdafx.h"

#include "QFontToolBar.h"


QFontToolBar::QFontToolBar( QWidget *parent /*= 0*/ ):
    QToolBar( parent )
{
    comboStyle = new QComboBox( this );
    addWidget( comboStyle );
    comboStyle->addItem("Standard");
    comboStyle->addItem("Bullet List (Disc)");
    comboStyle->addItem("Bullet List (Circle)");
    comboStyle->addItem("Bullet List (Square)");
    comboStyle->addItem("Ordered List (Decimal)");
    comboStyle->addItem("Ordered List (Alpha lower)");
    comboStyle->addItem("Ordered List (Alpha upper)");
    comboStyle->addItem("Ordered List (Roman lower)");
    comboStyle->addItem("Ordered List (Roman upper)");
    comboStyle->setFocusPolicy( Qt::NoFocus );

    comboFont = new QFontComboBox( this );
    comboFont->setFocusPolicy( Qt::NoFocus );
    addWidget( comboFont );

    comboSize = new QComboBox( this );
    comboSize->setObjectName("comboSize");
    addWidget( comboSize );
    comboSize->setEditable( true );
    comboSize->setFocusPolicy( Qt::NoFocus );
    QFontDatabase db;
    foreach(int size, db.standardSizes())
        comboSize->addItem(QString::number(size));
}

void QFontToolBar::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_QFontToolBar.cpp"
