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

#include "DataModels/Workspaces/Workspace.h"

#include "new-gui/Common/GUI/TextWidget.h"

#include "SaveAsFormulaDialog.h"


///////////////////////////////////////////
//  CSaveAsFormula
///////////////////////////////////////////

void  CSaveAsFormula::CreateWidgets()
{
    //	Create

	mNameLineEdit = new QLineEdit;
	auto *name_l = LayoutWidgets( Qt::Vertical, { new QLabel( "Name" ), mNameLineEdit }, nullptr, 2, 2, 2, 2, 2 );
	mNameLineEdit->setValidator( new QRegExpValidator( QRegExp( "[_a-zA-Z0-9]+" ) ) );

	mValueLineEdit = new CTextWidget;
	mValueLineEdit->SetSizeHint( 60 * mValueLineEdit->fontMetrics().width('x'), 5 * mValueLineEdit->fontMetrics().lineSpacing() );
	auto *value_l = LayoutWidgets( Qt::Vertical, { new QLabel( "Value" ), mValueLineEdit }, nullptr, 2, 2, 2, 2, 2 );
	mValueLineEdit->setReadOnly( true );

	mUnitLineEdit = new QLineEdit;
	auto *unit_l = LayoutWidgets( Qt::Vertical, { new QLabel( "Unit" ), mUnitLineEdit }, nullptr, 2, 2, 2, 2, 2 );

	mTitleLineEdit = new QLineEdit;
	auto *title_l = LayoutWidgets( Qt::Vertical, { new QLabel( "Title" ), mTitleLineEdit }, nullptr, 2, 2, 2, 2, 2 );

	mCommentLineEdit = new CTextWidget;
	mCommentLineEdit->SetSizeHint( 60 * mValueLineEdit->fontMetrics().width('x'), 5 * mValueLineEdit->fontMetrics().lineSpacing() );
	auto *comment_l = LayoutWidgets( Qt::Vertical, { new QLabel( "Comment" ), mCommentLineEdit }, nullptr, 2, 2, 2, 2, 2 );


	auto *small_l = LayoutWidgets( Qt::Vertical,
	{
								name_l,
								title_l,
								unit_l
	}, 
	nullptr, 2, 2, 2, 2, 2 );


	auto *big_l = LayoutWidgets( Qt::Horizontal,
	{
								small_l,
								value_l,
	}, 
	nullptr, 2, 2, 2, 2, 2 );

	
	auto *content = LayoutWidgets( Qt::Vertical,
	{
								big_l,
								comment_l,
	}, 
	nullptr, 2, 2, 2, 2, 2 );

	
	//	... Help

    auto help = new CTextWidget;
    help->SetHelpProperties(
        " Any expression, i.e. valid combination of data fields and functions can be saved as\n"
        " formula. You can insert a developed formula and modify it, or use a formula as part\n"
        " of an expression."
         ,0 , 6 );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


    //	... Buttons

    mButtonBox = new QDialogButtonBox( this );
    mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
    mButtonBox->setOrientation( Qt::Horizontal );
    mButtonBox->setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    mButtonBox->button( QDialogButtonBox::Ok )->setDefault( true );


    QBoxLayout *main_l =
            LayoutWidgets( Qt::Vertical,
                            {
								content,
								//name_l,
								//value_l,
								//unit_l,
								//title_l,
								//comment_l,
                                help_group,
                                mButtonBox
                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "Save Formula As");

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

    Wire();
}


CSaveAsFormula::CSaveAsFormula( CFormula *formula, QWidget *parent )
    : base_t( parent )
	, mFormula( formula )
{
    CreateWidgets();
}

CSaveAsFormula::~CSaveAsFormula()
{}


void  CSaveAsFormula::Wire()
{
	mNameLineEdit->setText( mFormula->GetName().c_str() );
	mValueLineEdit->setText( mFormula->GetDescription( false ).c_str() );
	mUnitLineEdit->setText( mFormula->GetUnitAsText().c_str() );
	mTitleLineEdit->setText( mFormula->GetTitle().c_str() );
	mCommentLineEdit->setText( mFormula->GetComment( false ).c_str() );

    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}


//virtual
void  CSaveAsFormula::accept()
{
	QString name = mNameLineEdit->text();
	if ( name.isEmpty() )
	{
		SimpleErrorBox( "A formula cannot have an empty name." );
		mNameLineEdit->setFocus();
		return;
	}
	std::string error_msg;
	mFormula->SetUnit( q2a( mUnitLineEdit->text() ), error_msg, "" );
	if ( !error_msg.empty() )
	{
		SimpleErrorBox( error_msg );
		mUnitLineEdit->setFocus();
		return;
	}

	mFormula->SetName( q2a( name ) );

	std::string comment;
	getAllEditorText( mCommentLineEdit, comment );
	mFormula->SetComment( comment );

	mFormula->SetTitle( q2a( mTitleLineEdit->text() ) );

	base_t::accept();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////


#include "moc_SaveAsFormulaDialog.cpp"
