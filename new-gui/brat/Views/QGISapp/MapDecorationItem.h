/***************************************************************************
                         qgsdecorationitem.h
                         ----------------------
    begin                : May 10, 2012
    copyright            : (C) 2012 by Etienne Tourigny
    email                : etourigny.dev at gmail dot com

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef VIEWS_QGISAPP_QGSDECORATIONITEM_H
#define VIEWS_QGISAPP_QGSDECORATIONITEM_H

#include <QObject>
#include <qgslogger.h>

class QPainter;

class CDecorationItem : public QObject
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
	CDecorationItem( QgsMapCanvas *parent );
	//! Destructor
	virtual ~ CDecorationItem();

	void setEnabled( bool enabled ) { mEnabled = enabled; }
	bool enabled() const { return mEnabled; }

	void update();

signals:
	void toggled( bool t );

public slots:

	//! this does the meaty bit of the work
	virtual void render( QPainter * ) {}
	//! Show the dialog box
	virtual void run() {}

	virtual void setName( const char *name );
	virtual QString name() const { return mName; }

protected:

	/**True if decoration item has to be displayed*/
	bool mEnabled;

	QString mName;
	QString mNameConfig;
	QString mNameTranslated;

	QgsMapCanvas *mMap = nullptr;
};

#endif		//VIEWS_QGISAPP_QGSDECORATIONITEM_H
