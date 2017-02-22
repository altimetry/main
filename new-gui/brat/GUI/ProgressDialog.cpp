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
#include "new-gui/brat/stdafx.h"


#include "new-gui/Common/QtUtils.h"
#include "ProgressDialog.h"




void CProgressDialog::Wire()
{
}


QWidget* CProgressDialog::PreConstruction( QWidget *parent )
{
	if ( !parent || !parent->isVisible() )
		parent = ApplicationWindow();
	if ( parent && !parent->isVisible() )
		parent = nullptr;

	return parent;
}

void CProgressDialog::PostConstruction( int minimum = 0 )
{
	if ( parentWidget() && parentWidget()->isVisible() )
	{
		setWindowModality( Qt::ApplicationModal );
	}
	base_t::setValue( minimum );
	setMaximumHeight( height() );
}

//explicit 
CProgressDialog::CProgressDialog( QWidget *parent, Qt::WindowFlags flags )	//parent = nullptr, Qt::WindowFlags flags = 0 
	: base_t( PreConstruction( parent ), flags )
	, progress_base_t()
{
	PostConstruction();
}

CProgressDialog::CProgressDialog( const QString &labelText, const QString &cancelButtonText, int minimum, int maximum, QWidget *parent, Qt::WindowFlags flags )		//parent = 0, Qt::WindowFlags flags = 0 
	: base_t( labelText, cancelButtonText, minimum, maximum, PreConstruction( parent ), flags )
	, progress_base_t( minimum, maximum )
{
	PostConstruction( minimum );
}


//virtual 
CProgressDialog::~CProgressDialog()
{}


//virtual 
void CProgressDialog::accept()
{
	base_t::accept();
}






///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ProgressDialog.cpp"
