/***************************************************************************
                                qgsmeasure.h
                               ------------------
        begin                : March 2005
        copyright            : (C) 2005 by Radim Blazek
        email                : blazek@itc.it
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VIEWS_QGISAPP_QGSMEASUREDIALOG_H
#define VIEWS_QGISAPP_QGSMEASUREDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

#include <qgscollapsiblegroupbox.h>


QT_BEGIN_NAMESPACE

class Ui_QgsMeasureBase
{
public:
	QGridLayout *gridLayout;
	QLabel *textLabel2;
	QLineEdit *editTotal;
	QSpacerItem *spacerItem;
	QComboBox *mUnitsCombo;
	QTreeWidget *mTable;
	QDialogButtonBox *buttonBox;
	QgsCollapsibleGroupBox *groupBox;
	QVBoxLayout *verticalLayout;
	QLabel *mNotesLabel;

	void setupUi( QDialog *QgsMeasureBase )
	{
		if ( QgsMeasureBase->objectName().isEmpty() )
			QgsMeasureBase->setObjectName( QString::fromUtf8( "QgsMeasureBase" ) );
		QgsMeasureBase->resize( 359, 301 );
		QgsMeasureBase->setBaseSize( QSize( 150, 200 ) );
		gridLayout = new QGridLayout( QgsMeasureBase );
		gridLayout->setSpacing( 6 );
		gridLayout->setContentsMargins( 11, 11, 11, 11 );
		gridLayout->setObjectName( QString::fromUtf8( "gridLayout" ) );
		gridLayout->setContentsMargins( 9, 9, 9, 9 );
		textLabel2 = new QLabel( QgsMeasureBase );
		textLabel2->setObjectName( QString::fromUtf8( "textLabel2" ) );

		gridLayout->addWidget( textLabel2, 4, 0, 1, 1 );

		editTotal = new QLineEdit( QgsMeasureBase );
		editTotal->setObjectName( QString::fromUtf8( "editTotal" ) );
		QFont font;
		font.setBold( true );
		font.setWeight( 75 );
		editTotal->setFont( font );
		editTotal->setAlignment( Qt::AlignRight );
		editTotal->setReadOnly( true );

		gridLayout->addWidget( editTotal, 4, 2, 1, 1 );

		spacerItem = new QSpacerItem( 41, 25, QSizePolicy::Fixed, QSizePolicy::Minimum );

		gridLayout->addItem( spacerItem, 4, 1, 1, 1 );

		mUnitsCombo = new QComboBox( QgsMeasureBase );
		mUnitsCombo->setObjectName( QString::fromUtf8( "mUnitsCombo" ) );

		gridLayout->addWidget( mUnitsCombo, 4, 3, 1, 1 );

		mTable = new QTreeWidget( QgsMeasureBase );
		QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
		__qtreewidgetitem->setTextAlignment( 0, Qt::AlignRight | Qt::AlignVCenter );
		mTable->setHeaderItem( __qtreewidgetitem );
		mTable->setObjectName( QString::fromUtf8( "mTable" ) );
		mTable->setEditTriggers( QAbstractItemView::NoEditTriggers );
		mTable->setRootIsDecorated( false );
		mTable->setColumnCount( 1 );

		gridLayout->addWidget( mTable, 2, 0, 1, 4 );

		buttonBox = new QDialogButtonBox( QgsMeasureBase );
		buttonBox->setObjectName( QString::fromUtf8( "buttonBox" ) );
		buttonBox->setStandardButtons( QDialogButtonBox::Close );

		gridLayout->addWidget( buttonBox, 6, 0, 1, 4 );

		groupBox = new QgsCollapsibleGroupBox( QgsMeasureBase );
		groupBox->setObjectName( QString::fromUtf8( "groupBox" ) );
		verticalLayout = new QVBoxLayout( groupBox );
		verticalLayout->setSpacing( 6 );
		verticalLayout->setContentsMargins( 11, 11, 11, 11 );
		verticalLayout->setObjectName( QString::fromUtf8( "verticalLayout" ) );
		verticalLayout->setContentsMargins( 3, 3, 3, 3 );
		mNotesLabel = new QLabel( groupBox );
		mNotesLabel->setObjectName( QString::fromUtf8( "mNotesLabel" ) );
		mNotesLabel->setWordWrap( true );

		verticalLayout->addWidget( mNotesLabel );


		gridLayout->addWidget( groupBox, 5, 0, 1, 4 );

#ifndef QT_NO_SHORTCUT
		textLabel2->setBuddy( editTotal );
#endif // QT_NO_SHORTCUT
		QWidget::setTabOrder( mTable, editTotal );

		retranslateUi( QgsMeasureBase );

		QMetaObject::connectSlotsByName( QgsMeasureBase );
	} // setupUi

	void retranslateUi( QDialog *QgsMeasureBase )
	{
		QgsMeasureBase->setWindowTitle( QApplication::translate( "QgsMeasureBase", "Measure", 0, QApplication::UnicodeUTF8 ) );
		textLabel2->setText( QApplication::translate( "QgsMeasureBase", "Total", 0, QApplication::UnicodeUTF8 ) );
		QTreeWidgetItem *___qtreewidgetitem = mTable->headerItem();
		___qtreewidgetitem->setText( 0, QApplication::translate( "QgsMeasureBase", "Segments", 0, QApplication::UnicodeUTF8 ) );
		groupBox->setTitle( QApplication::translate( "QgsMeasureBase", "Info", 0, QApplication::UnicodeUTF8 ) );
		mNotesLabel->setText( QString() );
	} // retranslateUi

};

namespace Ui {
	class QgsMeasureBase : public Ui_QgsMeasureBase {};
} // namespace Ui

QT_END_NAMESPACE




#include "qgspoint.h"
#include "qgsdistancearea.h"

class QCloseEvent;
class CMeasureTool;

class CMeasureDialog : public QDialog, private Ui::QgsMeasureBase
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

public:

	//! Constructor
	CMeasureDialog( CMeasureTool* tool, Qt::WindowFlags f = 0 );

	//! Save position
	void saveWindowLocation( void );

	//! Restore last window position/size
	void restorePosition( void );

	//! Add new point
	void addPoint( QgsPoint &point );

	//! Mose move
	void mouseMove( QgsPoint &point );

	//! Remove last point
	void removeLastPoint();

	public slots:
	//! Reject
	void on_buttonBox_rejected( void );

	//! Reset and start new
	void restart();

	//! Close event
	void closeEvent( QCloseEvent *e ) override;

	//! When any external settings change
	void updateSettings();

	private slots:
	void unitsChanged( const QString &units );

private:

	//! formats distance to most appropriate units
	QString formatDistance( double distance );

	//! formats area to most appropriate units
	QString formatArea( double area );

	//! shows/hides table, shows correct units
	void updateUi();

	//! Converts the measurement, depending on settings in options and current transformation
	void convertMeasurement( double &measure, QGis::UnitType &u, bool isArea );

	double mTotal;

	//! indicates whether we're measuring distances or areas
	bool mMeasureArea;

	//! Number of decimal places we want.
	int mDecimalPlaces;

	//! Current unit for input values
	QGis::UnitType mCanvasUnits;

	//! Current unit for output values
	QGis::UnitType mDisplayUnits;

	//! Our measurement object
	QgsDistanceArea mDa;

	//! pointer to measure tool which owns this dialog
	CMeasureTool* mTool;

	QgsPoint mLastMousePoint;
};

#endif		//VIEWS_QGISAPP_QGSMEASUREDIALOG_H
