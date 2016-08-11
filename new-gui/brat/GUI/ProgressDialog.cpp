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


#include "ProgressDialog.h"

void f()
{
	const int numFiles = 10000;
	QWidget *parent = nullptr;
	CProgressDialog progress( "Computing contours...", "Cancel", 0, numFiles, parent );
	//progress.setr
	progress.setWindowModality( Qt::WindowModal );
	progress.show();
	int i = 0;
	for ( ; i < numFiles; i++ )
	{
		progress.setValue( i );

		if ( progress.wasCanceled() )
			break;
		qDebug() << "... copy one file";
	}
	progress.setValue( numFiles );
	qDebug() << "copied files==" << i;
}




void CProgressDialog::Wire()
{
}


//explicit 
CProgressDialog::CProgressDialog( QWidget *parent, Qt::WindowFlags flags )	//parent = nullptr, Qt::WindowFlags flags = 0 
	: base_t( parent, flags )
	, progress_base_t()
{}

CProgressDialog::CProgressDialog( const QString &labelText, const QString &cancelButtonText, int minimum, int maximum, QWidget *parent, Qt::WindowFlags flags )		//parent = 0, Qt::WindowFlags flags = 0 
	: base_t( labelText, cancelButtonText, minimum, maximum, parent, flags )
	, progress_base_t( minimum, maximum )
{}


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
