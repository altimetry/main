#include "new-gui/brat/stdafx.h"

#include "DataModels/Workspaces/Workspace.h"

#include "new-gui/Common/GUI/TextWidget.h"

#include "FormulaDialog.h"




CFormulaDialog::CFormulaDialog( CWorkspaceFormula *wkspc, QWidget *parent )
    : base_t( parent )
	, mWFormula( wkspc )
{
    Q_UNUSED( parent );

    CreateWidgets();
}

CFormulaDialog::~CFormulaDialog()
{}


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
    //	Setup things

	connect( mFormulasListWidget, SIGNAL( currentRowChanged( int ) ), this, SLOT( HandleSelectedFormulaChanged( int ) ) );
	connect( mAsAliasCheck, SIGNAL( toggled( bool ) ), this, SLOT( HandleAsAliasCheckToggled( bool ) ) );
	connect( mRemoveButton, SIGNAL( clicked() ), this, SLOT( HandleRemoveButtonClicked() ) );

    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );

	FillFormulaList();
}


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

	BRAT_NOT_IMPLEMENTED;

	//CWorkspaceFormula* wks = wxGetApp().GetCurrentWorkspaceFormula();
	//if ( wks == NULL )
	//{
	//	return;
	//}

	//if ( GetFormulaList()->GetSelection() < 0 )
	//{
	//	return;
	//}

	//wxString name = GetFormulaList()->GetStringSelection();

	//CFormula* formula = wks->GetFormula( name );
	//if ( formula == NULL )
	//{
	//	return;
	//}

	//if ( formula->IsPredefined() )
	//{
	//	wxMessageBox( "Sorry, you are not allowed to remove 'predefined' formulas",
	//		"Warning",
	//		wxOK | wxICON_EXCLAMATION );
	//	return;
	//}


	//int32_t result = wxMessageBox( wxString::Format( "Do you to really want to remove formula '%s'\n"
	//	"WARNING:If you remove it, be sure that it's not use anymore in the current workspace\n"
	//	"Otherwise, an error will occurs while executing operations that use it.",
	//	name.c_str() ),
	//	"Warning",
	//	wxYES_NO | wxCENTRE | wxICON_QUESTION );

	//if ( result != wxYES )
	//{
	//	return;
	//}

	//wks->RemoveFormula( name );
	//m_formula = NULL;

	//FillFormulaList();

	//GetFormulaList()->SetSelection( -1 );
	//GetFormulaValue()->SetValue( "" );
}



//virtual
void  CFormulaDialog::accept()
{
	assert__( mCurrentFormula );

    base_t::accept();
}








///////////////////////////////////////////
//  CSaveAsFormula
///////////////////////////////////////////

CSaveAsFormula::CSaveAsFormula( QWidget *parent )
    : base_t( parent )
{

    CreateWidgets();
}

CSaveAsFormula::~CSaveAsFormula()
{}


void  CSaveAsFormula::CreateWidgets()
{
    //	Create

	QLineEdit *mNameLineEdit = new QLineEdit;
	auto *name_l = LayoutWidgets( Qt::Vertical, { new QLabel( "Name" ), mNameLineEdit }, nullptr, 2, 2, 2, 2, 2 );

	CTextWidget *mValueLineEdit = new CTextWidget;
	mValueLineEdit->SetSizeHint( 60 * mValueLineEdit->fontMetrics().width('x'), 5 * mValueLineEdit->fontMetrics().lineSpacing() );
	auto *value_l = LayoutWidgets( Qt::Vertical, { new QLabel( "Value" ), mValueLineEdit }, nullptr, 2, 2, 2, 2, 2 );

	QLineEdit *mUnitLineEdit = new QLineEdit;
	auto *unit_l = LayoutWidgets( Qt::Vertical, { new QLabel( "Unit" ), mUnitLineEdit }, nullptr, 2, 2, 2, 2, 2 );

	QLineEdit *mTitleLineEdit = new QLineEdit;
	auto *title_l = LayoutWidgets( Qt::Vertical, { new QLabel( "Title" ), mTitleLineEdit }, nullptr, 2, 2, 2, 2, 2 );

	CTextWidget *mCommentLineEdit = new CTextWidget;
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


void  CSaveAsFormula::Wire()
{
    //	Setup things

    connect( mButtonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( mButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}



//virtual
void  CSaveAsFormula::accept()
{

    base_t::accept();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////


#include "moc_FormulaDialog.cpp"
