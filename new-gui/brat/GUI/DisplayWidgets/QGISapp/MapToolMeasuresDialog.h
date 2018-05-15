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
#ifndef VIEWS_QGISAPP_QGSMEASURESDIALOG_H
#define VIEWS_QGISAPP_QGSMEASURESDIALOG_H

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
			QgsMeasureBase->setObjectName( QStringLiteral( "QgsMeasureBase" ) );
		QgsMeasureBase->resize( 359, 301 );
		QgsMeasureBase->setBaseSize( QSize( 150, 200 ) );
		QIcon icon;
		icon.addFile( QStringLiteral( "" ), QSize(), QIcon::Normal, QIcon::Off );
		QgsMeasureBase->setWindowIcon( icon );
		gridLayout = new QGridLayout( QgsMeasureBase );
		gridLayout->setSpacing( 6 );
		gridLayout->setContentsMargins( 11, 11, 11, 11 );
		gridLayout->setObjectName( QStringLiteral( "gridLayout" ) );
		gridLayout->setContentsMargins( 9, 9, 9, 9 );
		textLabel2 = new QLabel( QgsMeasureBase );
		textLabel2->setObjectName( QStringLiteral( "textLabel2" ) );

		gridLayout->addWidget( textLabel2, 4, 0, 1, 1 );

		editTotal = new QLineEdit( QgsMeasureBase );
		editTotal->setObjectName( QStringLiteral( "editTotal" ) );
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
		mUnitsCombo->setObjectName( QStringLiteral( "mUnitsCombo" ) );

		gridLayout->addWidget( mUnitsCombo, 4, 3, 1, 1 );

		mTable = new QTreeWidget( QgsMeasureBase );
		QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
		__qtreewidgetitem->setTextAlignment( 0, Qt::AlignRight | Qt::AlignVCenter );
		mTable->setHeaderItem( __qtreewidgetitem );
		mTable->setObjectName( QStringLiteral( "mTable" ) );
		mTable->setEditTriggers( QAbstractItemView::NoEditTriggers );
		mTable->setRootIsDecorated( false );
		mTable->setColumnCount( 1 );

		gridLayout->addWidget( mTable, 2, 0, 1, 4 );

		buttonBox = new QDialogButtonBox( QgsMeasureBase );
		buttonBox->setObjectName( QStringLiteral( "buttonBox" ) );
		buttonBox->setStandardButtons( QDialogButtonBox::Close | QDialogButtonBox::Help );

		gridLayout->addWidget( buttonBox, 6, 0, 1, 4 );

		groupBox = new QgsCollapsibleGroupBox( QgsMeasureBase );
		groupBox->setObjectName( QStringLiteral( "groupBox" ) );
		verticalLayout = new QVBoxLayout( groupBox );
		verticalLayout->setSpacing( 6 );
		verticalLayout->setContentsMargins( 11, 11, 11, 11 );
		verticalLayout->setObjectName( QStringLiteral( "verticalLayout" ) );
		verticalLayout->setContentsMargins( 3, 3, 3, 3 );
		mNotesLabel = new QLabel( groupBox );
		mNotesLabel->setObjectName( QStringLiteral( "mNotesLabel" ) );
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
		QgsMeasureBase->setWindowTitle( QApplication::translate( "QgsMeasureBase", "Measure", 0 ) );
		textLabel2->setText( QApplication::translate( "QgsMeasureBase", "Total", 0 ) );
		QTreeWidgetItem *___qtreewidgetitem = mTable->headerItem();
		___qtreewidgetitem->setText( 0, QApplication::translate( "QgsMeasureBase", "Segments", 0 ) );
		groupBox->setTitle( QApplication::translate( "QgsMeasureBase", "Info", 0 ) );
		mNotesLabel->setText( QString() );
	} // retranslateUi

};

namespace Ui {
	class QgsMeasureBase : public Ui_QgsMeasureBase {};
} // namespace Ui

QT_END_NAMESPACE



#include "qgspoint.h"
#include "qgsdistancearea.h"
#include "qgscontexthelp.h"

class QCloseEvent;
class CMeasuresTool;

class CMeasuresToolDialog : public QDialog, private Ui::QgsMeasureBase
{
	Q_OBJECT

public:

	//! Constructor
	CMeasuresToolDialog( CMeasuresTool* tool, const QgsCoordinateReferenceSystem &crs, Qt::WindowFlags f = nullptr );

	//! Save position
	void saveWindowLocation();

	//! Restore last window position/size
	void restorePosition();

	//! Add new point
	void addPoint( const QgsPoint &point );

	//! Mose move
	void mouseMove( const QgsPoint &point );

	//! Remove last point
	void removeLastPoint();

	public slots:
	virtual void reject() override;

	//! Reset and start new
	void restart();

	//! Close event
	void closeEvent( QCloseEvent *e ) override;

	//! Show the help for the dialog
	void on_buttonBox_helpRequested() { QgsContextHelp::run( metaObject()->className() ); }

	//! When any external settings change
	void updateSettings( const QgsCoordinateReferenceSystem &crs );

private slots:
	void unitsChanged( int index );

private:

	//! formats distance to most appropriate units
	QString formatDistance( double distance, bool convertUnits = true ) const;

	//! formats area to most appropriate units
	QString formatArea( double area, bool convertUnits = true ) const;

	//! shows/hides table, shows correct units
	void updateUi();

	/** Resets the units combo box to display either distance or area units
	 * @param isArea set to true to populate with areal units, or false to show distance units
	 */
	void repopulateComboBoxUnits( bool isArea );

	double convertLength( double length, QGis::UnitType toUnit ) const;

	double convertArea( double area, QgsUnitTypes::AreaUnit toUnit ) const;

	double mTotal;

	//! indicates whether we're measuring distances or areas
	bool mMeasureArea;

	//! Number of decimal places we want.
	int mDecimalPlaces;

	//! Current unit for input values
	QGis::UnitType mCanvasUnits;

	//! Current unit for distance values
	QGis::UnitType mDistanceUnits;

	//! Current unit for area values
	QgsUnitTypes::AreaUnit mAreaUnits;

	//! Our measurement object
	QgsDistanceArea mDa;

	//! pointer to measure tool which owns this dialog
	CMeasuresTool* mTool;

	QgsPoint mLastMousePoint;

	friend class TestQgsMeasureTool;
};

#endif	//VIEWS_QGISAPP_QGSMEASURESDIALOG_H

