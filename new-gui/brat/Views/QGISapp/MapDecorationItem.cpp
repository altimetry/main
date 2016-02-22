/***************************************************************************
                         qgsdecorationitem.cpp
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
#include "new-gui/brat/stdafx.h"

#include "MapDecorationItem.h"


CDecorationItem::CDecorationItem( QgsMapCanvas *parent )
	: QObject( parent )
	, mMap( parent )
{
	mEnabled = false;
}

CDecorationItem::~CDecorationItem()
{}

void CDecorationItem::update()
{
	mMap->refresh();
}

void CDecorationItem::setName( const char *name )
{
	mName = name;
	mNameConfig = name;
	mNameConfig.remove( " " );
	mNameTranslated = tr( name );
	QgsDebugMsg( QString( "name=%1 nameconfig=%2 nametrans=%3" ).arg( mName ).arg( mNameConfig ).arg( mNameTranslated ) );
}





///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_MapDecorationItem.cpp"
