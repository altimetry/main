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

#include "new-gui/Common/GUI/TextWidget.h"
#include "GUI/ProgressDialog.h"

#include "EditExportAsciiDialog.h"



void  CEditExportAsciiDialog::CreateWidgets()
{
    //	Text

    mAsciiTableWidget = new QTableWidget;
	mAsciiTableWidget->verticalHeader()->setVisible( false );

	auto *content_l = LayoutWidgets( Qt::Vertical, 
	{ 
		//CreateSplitter( nullptr, Qt::Vertical, { mAsciiTableWidget, mTextEdit }, true ) 
		mAsciiTableWidget 
	}, 
	nullptr, 2, 2, 2, 2, 2 );


    //	... Help

    auto help = new CTextWidget;
    help->SetHelpProperties( "Exported ASCII file for operation " + t2q( mOperation->GetName() ) + ".", 0 , 6 );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


    //	... Buttons

    mButtonBox = new QDialogButtonBox( this );
    mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
    mButtonBox->setOrientation( Qt::Horizontal );
    mButtonBox->setStandardButtons( QDialogButtonBox::Close );
    mButtonBox->button( QDialogButtonBox::Close )->setDefault( true );

    QBoxLayout *main_l =
            LayoutWidgets( Qt::Vertical,
                            {
								content_l,
								WidgetLine( nullptr, Qt::Horizontal ),
                                help_group,
                                mButtonBox

                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );

   Wire();
}


void CEditExportAsciiDialog::FillTable( const std::string &ascii, CProgressInterface *pi )
{
	//statics

	static const std::string &nl = "\n";
	static const size_t nl_len = nl.length();

	//nested lambdas

	auto parse_line = []( const std::string &s, std::vector< std::string > &vline )
	{
		String2Vector( vline, s, std::string( "\t" ) );
		trim( vline.back() );
		if ( vline.back().empty() )
			vline.erase( vline.end() -1 );
	};


	auto parse_number = []( const std::string &s, double &n ) -> bool
	{
		try {
			n = s2n<double>( s );
			return true;
		}
		catch ( ... ) {}
		return false;
	};


	//function body

	std::vector< std::string > vline;
	std::string headers;
	size_t pos = 0;
	size_t ptr = ascii.find( nl, pos );
	if ( ptr != std::string::npos )
	{
		std::string headers = ascii.substr( pos, ptr - pos );
		vline.push_back( "" );
		parse_line( headers, vline );
		const size_t size = vline.size();
		mAsciiTableWidget->setColumnCount( size );
		for ( size_t i = 0; i < size; ++i )
		{
			QTableWidgetItem *item = new QTableWidgetItem( vline[i].c_str() );
			mAsciiTableWidget->setHorizontalHeaderItem( i, item );
		}
	}
	pos = ptr + nl_len;

	do {

		ptr = ascii.find( nl, pos );
		std::string line;
		if ( ptr != std::string::npos )
		{
			line = ascii.substr( pos, ptr - pos );
			pos = ptr + nl_len;
		}
		else
		{
			line = ascii.substr( pos );
			if ( line.empty() )
				break;              //discard empty lines at the end of file
		}

		int index = mAsciiTableWidget->rowCount();
		vline.clear();
		vline.push_back( n2s( index ) );
		parse_line( line, vline );
		const size_t size = vline.size();

		mAsciiTableWidget->setRowCount( index + 1 );
		double n = 0.;
		for ( size_t i = 0; i < size; ++i )
		{
			auto *item = new QTableWidgetItem;
			if ( parse_number( vline[ i ], n ) )
			{
				if ( i == 0 )
					item->setData( Qt::DisplayRole, (qulonglong)n );    //QVariant does not have size_t
				else
					item->setData( Qt::DisplayRole, n );
			}
			else
			{
				item->setData( Qt::DisplayRole, vline[ i ].c_str() );
			}
			mAsciiTableWidget->setItem( index, i, item );
		}

		if ( !pi->SetCurrentValue( pi->CurrentValue() + line.length() ) )
			break;

	} while ( ptr != std::string::npos );

	// Enable sort
	mAsciiTableWidget->setSortingEnabled( true );
	// Changing selection behavior to whole row selection
	mAsciiTableWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
	// Setting alternating row colors
	mAsciiTableWidget->setAlternatingRowColors( true );
	// Disable editable table items
	mAsciiTableWidget->setEditTriggers( QAbstractItemView::NoEditTriggers );
	// select only one item
	mAsciiTableWidget->setSelectionMode( QAbstractItemView::SingleSelection );

	mAsciiTableWidget->resizeColumnsToContents();
}


void  CEditExportAsciiDialog::Wire()
{
	std::string ascii;
	if ( !Read2String( ascii, mOperation->GetExportAsciiOutputPath() ) )
	{
		SimpleErrorBox( "Could not read from " + mOperation->GetExportAsciiOutputPath() );
		QTimer::singleShot( 1000, this, &CEditExportAsciiDialog::close );
		return;
	}

    CProgressDialog progress( "Building data table...", "Cancel", 0, ascii.length(), parentWidget() );
    progress.show();
	FillTable( ascii, &progress );
	if ( progress.Cancelled() && !SimpleQuestion( "Do you want to see the data read before canceling?" ) )
	{
		QTimer::singleShot( 1000, this, &CEditExportAsciiDialog::close );
		return;
	}

    //	connect

    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}


//virtual 
QSize CEditExportAsciiDialog::sizeHint() const
{
	static QSize s( ComputeTableWidth( mAsciiTableWidget ) + 30 , ComputeTableHeight( mAsciiTableWidget, 20 ) + 50 );
	return s;
}


CEditExportAsciiDialog::CEditExportAsciiDialog( bool modal, COperation *operation, QWidget *parent )
    : base_t( parent )
	, mOperation( operation )
{
	if ( !modal )
		setAttribute( Qt::WA_DeleteOnClose );

    CreateWidgets();

	setWindowTitle( "Exported ASCII file: " + t2q( mOperation->GetExportAsciiOutputPath() ) );

	if ( !modal )
		mOperation = nullptr;	//for possible evolutions of implementation: if !modal, cannot assume mOperation is valid beyond dialog creation
}


CEditExportAsciiDialog::~CEditExportAsciiDialog()
{}


//virtual
void  CEditExportAsciiDialog::accept()
{
    base_t::accept();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_EditExportAsciiDialog.cpp"
