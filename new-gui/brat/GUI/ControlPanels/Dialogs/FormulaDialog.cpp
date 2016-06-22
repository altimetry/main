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

#include "FormulaDialog.h"




void  CFormulaDialog::CreateWidgets()
{
    //	Create

	mFormulasListWidget = new QListWidget;
	mRemoveButton = new QPushButton( "Remove" );

	auto *formulas = LayoutWidgets( Qt::Vertical, { new QLabel( "Formulas" ), mFormulasListWidget, mRemoveButton }, nullptr, 2, 2, 2, 2, 2 );

	mUniLineEdit = new QLineEdit;
	mUniLineEdit->setReadOnly( true );
	auto *unit = LayoutWidgets( Qt::Horizontal, { new QLabel( "Unit" ), mUniLineEdit }, nullptr, 2, 2, 2, 2, 2 );

	mFormulaContent = new QTextEdit;
	mFormulaContent->setReadOnly( true );
	mAsAliasCheck = new QCheckBox( "As Alias" );
	auto *unit_and_remove_and_fcontent = LayoutWidgets( Qt::Vertical, { unit, new QLabel( "Content" ), mFormulaContent, mAsAliasCheck }, nullptr, 2, 2, 2, 2, 2 );

	auto *content = LayoutWidgets( Qt::Horizontal, { formulas, unit_and_remove_and_fcontent }, nullptr, 2, 2, 2, 2, 2 );


    //	... Help

    auto help = new CTextWidget;
    help->SetHelpProperties(
        "List of pre-defined formulas (Sea Surface Height and Sea Level Anomaly formulas from\n"
        " the different satellites GDR fields, and also 'Ocean editing' formulas, to use as\n"
        " selection criteria to select only valid data over ocean)."
         ,0 , 6, Qt::AlignCenter );
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
                                help_group,
                                mButtonBox
                            }, this, 6, 6, 6, 6, 6 );                       Q_UNUSED( main_l );


    setWindowTitle( "Insert Formula");

    //	Wrap up dimensions

    adjustSize();
    setMinimumWidth( width() );

    Wire();
}


void  CFormulaDialog::Wire()
{
	connect( mFormulasListWidget, SIGNAL( currentRowChanged( int ) ), this, SLOT( HandleSelectedFormulaChanged( int ) ) );
	connect( mAsAliasCheck, SIGNAL( toggled( bool ) ), this, SLOT( HandleAsAliasCheckToggled( bool ) ) );
	connect( mRemoveButton, SIGNAL( clicked() ), this, SLOT( HandleRemoveButtonClicked() ) );

    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );

	FillFormulaList();
}


CFormulaDialog::CFormulaDialog( CWorkspaceFormula *wkspc, QWidget *parent )
    : base_t( parent )
	, mWFormula( wkspc )
{
	assert__( mWFormula );

    CreateWidgets();
}

CFormulaDialog::~CFormulaDialog()
{}


void CFormulaDialog::FillFormulaList()
{
	assert__( mWFormula != nullptr );

	mFormulasListWidget->clear();

	if ( mWFormula->GetFormulaCount() <= 0 )
	{
		return;
	}

	std::vector< std::string > v;
	mWFormula->GetFormulaNames( v );
	for ( auto &s : v )
		mFormulasListWidget->addItem( s.c_str() );

	HandleSelectedFormulaChanged( mFormulasListWidget->count() > 0 ? 0 : -1 );
}


void CFormulaDialog::HandleSelectedFormulaChanged( int index )
{
	mCurrentFormula = nullptr;
	mUniLineEdit->setText("");
	mFormulaContent->setText("");

	mRemoveButton->setEnabled( index >= 0 );
	mAsAliasCheck->setEnabled( index >= 0 );

	if ( index < 0 )
		return;

	std::string name = q2a( mFormulasListWidget->item( index )->text() );
	mCurrentFormula = mWFormula->GetFormula( name );							assert__( mCurrentFormula );
	std::string value = mWFormula->GetDescFormula( name, false );
	mFormulaContent->setText( value.c_str() );
	mUniLineEdit->setText( mCurrentFormula->GetUnitAsText().c_str() );
	mRemoveButton->setEnabled( !mCurrentFormula->IsPredefined() );
}


void CFormulaDialog::HandleAsAliasCheckToggled( bool toggled )
{
	mAsAlias = toggled;
}


void CFormulaDialog::HandleRemoveButtonClicked()
{
	assert__( mCurrentFormula );

	if ( mCurrentFormula->IsPredefined() )		//should never happen
	{
		SimpleErrorBox( "Sorry, you are not allowed to remove 'predefined' formulas" );
	}

	if ( !SimpleQuestion( 
		"Do you to really want to remove formula '"
		+ mCurrentFormula->GetName()
		+ "'\nWARNING:If you remove it, be sure that it's not use anymore in the current workspace\n"
		"Otherwise, an error will occurs while executing operations that use it." ) )
	{
		return;
	}

	mWFormula->RemoveFormula( mCurrentFormula->GetName() );

	FillFormulaList();
}



//virtual
void  CFormulaDialog::accept()
{
	assert__( mCurrentFormula );

	if ( mCurrentFormula )
		base_t::accept();
	else
		SimpleErrorBox( "A formula must be selected." );
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////


#include "moc_FormulaDialog.cpp"
