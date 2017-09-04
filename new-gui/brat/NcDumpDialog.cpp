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
#include "stdafx.h"

#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/GUI/TextWidget.h"
#include "new-gui/Common/System/OsProcess.h"

#include "NcDumpDialog.h"



void CNcDumpDialog::Setup()
{
	connect( mRunAgainButton, &QPushButton::clicked, this, &CNcDumpDialog::HandleRunAgainButtonClicked );

	connect( mButtonBox, &QDialogButtonBox::accepted, this, &CNcDumpDialog::accept );
    connect( mButtonBox, &QDialogButtonBox::rejected, this, &CNcDumpDialog::reject );

	HandleRunAgainButtonClicked();
}


void CNcDumpDialog::CreateWidgets()
{
	mDumpTextWidget = new CTextWidget;
	mNcDumpOptionsLineEdit = new QLineEdit;
	mRunAgainButton = new QPushButton( "Run Again" );

	auto run_l = LayoutWidgets( Qt::Horizontal, { mNcDumpOptionsLineEdit, mRunAgainButton }, nullptr, 2, 2, 2, 2, 2 );
	auto *widgets_l = CreateGroupBox( ELayoutType::Vertical,
	{
		new QLabel( mNetcdfFilePath ), 
		mDumpTextWidget, 
		LayoutWidgets( Qt::Vertical, { new QLabel("Options"), run_l }, nullptr, 2, 2, 2, 2, 2 )
	},
	"", nullptr, 2, 2, 2, 2, 2 );


    //... Help

    mHelpText = new CTextWidget;
	mHelpText->SetHelpProperties( "ncdump of " + mNetcdfFilePath, 1, 6, Qt::AlignCenter, true );
	mHelpText->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	mHelpText->adjustSize();
	auto help_group = CreateGroupBox( ELayoutType::Vertical, { mHelpText }, "", nullptr, 6, 6, 6, 6, 6 );

    //... Buttons

    mButtonBox = new QDialogButtonBox( this );
    mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
    mButtonBox->setOrientation( Qt::Horizontal );
    mButtonBox->setStandardButtons( QDialogButtonBox::Close );
	//if ( mModal )
	//{
	//	mButtonBox->setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
	//	mButtonBox->button( QDialogButtonBox::Ok )->setDefault( true );
	//}
	//else
	//{
	//	mButtonBox->setStandardButtons( QDialogButtonBox::Close | QDialogButtonBox::Apply );
	//}


    //... All

	QSplitter *splitter = CreateSplitter( nullptr, Qt::Vertical, { widgets_l, help_group } );
	splitter->setStretchFactor( 0, 1 );
	splitter->setStretchFactor( 1, 0 );

    QBoxLayout *main_l = LayoutWidgets( Qt::Vertical,
                            {
								splitter,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "NcDump");

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

	Setup();
}


CNcDumpDialog::CNcDumpDialog( bool modal, const std::string &ncdump_path, const QString &netcdf_file_path, QWidget *parent )

	: base_t( parent )
	, mModal( modal )
	, mNetcdfFilePath( netcdf_file_path )
	, mNcDumpath( ncdump_path )
{
	if ( !mModal )
		setAttribute( Qt::WA_DeleteOnClose );

	CreateWidgets();
}


CNcDumpDialog::~CNcDumpDialog()
{
	delete mProcess;
}


void CNcDumpDialog::SetHelpText()
{
	assert__( mProcess );

	QString cmd_line = t2q( mProcess->CmdLine() );
	mHelpText->SetHelpProperties( "ncdump command line:\n" + cmd_line, 1, 6, Qt::AlignCenter, true );
}


void CNcDumpDialog::HandleRunAgainButtonClicked()
{
	if ( mProcess && mProcess->state() != COsProcess::NotRunning )
	{
		SimpleWarnBox("ncdump is still running. Please try again later.");
		return;
	}

	auto options = q2a( mNcDumpOptionsLineEdit->text() );
	if ( !options.empty() )
		options += " ";

	mDumpTextWidget->clear();

	delete mProcess;
	mProcess = new COsProcess( mSync, "ncdump", this, "\"" + mNcDumpath + "\" " + options + q2a( mNetcdfFilePath ) );

	connect( mProcess, &COsProcess::readyReadStandardOutput,	this, &CNcDumpDialog::HandleUpdateOutput );
	connect( mProcess, &COsProcess::readyReadStandardError,		this, &CNcDumpDialog::HandleUpdateOutput );
	connect( mProcess, (void(COsProcess::*)( int, QProcess::ExitStatus))&COsProcess::finished,	this, 
		mSync ?
		&CNcDumpDialog::HandleSyncProcessFinished
		:
		&CNcDumpDialog::HandleAsyncProcessFinished
	);
	connect( mProcess, (void(COsProcess::*)( QProcess::ProcessError))&COsProcess::error,		this, &CNcDumpDialog::HandleProcessError );

	SetHelpText();

	mProcess->Execute();
	mProcess->waitForStarted( 5000 ) ;	//calling this from thread or GUI risks to block if called process never returns
}


//virtual
void CNcDumpDialog::accept()
{
	base_t::accept();
}


//////////////////////////////
// COsProcess Signals Handling
//////////////////////////////


//slot
void CNcDumpDialog::HandleUpdateOutput()
{
	COsProcess *process = qobject_cast<COsProcess*>( sender() );			assert__( process && mProcess == process );

	QString text = process->readAllStandardError();
	if ( !text.isEmpty() )
		LOG_WARN( q2a( text ) );

	text = process->readAllStandardOutput();
	if ( !text.isEmpty() )
		mDumpTextWidget->append( text );
}


COsProcess* CNcDumpDialog::ProcessFinished( int exit_code, QProcess::ExitStatus exitStatus )
{
	COsProcess *process = qobject_cast<COsProcess*>( sender() );			assert__( process && mProcess == process );

	if (exitStatus == QProcess::CrashExit) 
	{
		SimpleWarnBox( "ncdump crashed" );
	} 
	else if ( exit_code != 0 )	//this is ExitStatus::NormalExit, although error code != 0
	{
		SimpleWarnBox( "ncdump exit code " + n2s<std::string>( exit_code ) );
	} 
	else 
	{
		LOG_INFO( "ncdump finished with success" );
	}

	return process;
}
//slot
void CNcDumpDialog::HandleAsyncProcessFinished( int exit_code, QProcess::ExitStatus exitStatus )
{
	ProcessFinished( exit_code, exitStatus );
}
//slot
void CNcDumpDialog::HandleSyncProcessFinished( int exit_code, QProcess::ExitStatus exitStatus )
{
	ProcessFinished( exit_code, exitStatus );
}


void CNcDumpDialog::HandleProcessError( QProcess::ProcessError error )
{
	COsProcess *process = qobject_cast<COsProcess*>( sender() );			assert__( process && mProcess == process );

	SimpleErrorBox( COsProcess::ProcessErrorMessage( error ) + "\nSee the log window for eventual details." );
}






///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_NcDumpDialog.cpp"
