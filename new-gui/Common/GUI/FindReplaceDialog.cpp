#include "stdafx.h"

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include "new-gui/Common/QtUtils.h"

#include "FindReplaceDialog.h"


#define TEXT_TO_FIND "mTextToFindLineEdit"
#define TEXT_TO_REPLACE "textToReplace"
#define DOWN_RADIO "downRadio"
#define UP_RADIO "upRadio"
#define CASE_CHECK "caseCheck"
#define WHOLE_CHECK "wholeCheck"
#define REGEXP_CHECK "regexpCheck"




void FindReplaceForm::CreateWidgets()
{
	if (objectName().isEmpty())
		setObjectName(QStringLiteral("FindReplaceForm"));
	resize(483, 288);
	gridLayout = new QGridLayout( this );
	gridLayout->setObjectName(QStringLiteral("gridLayout"));
	verticalLayout_5 = new QVBoxLayout();
	verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
	verticalLayout_2 = new QVBoxLayout();
	verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
	gridLayout_3 = new QGridLayout();
	gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
	label = new QLabel( this );
	label->setObjectName(QStringLiteral("label"));

	gridLayout_3->addWidget(label, 0, 0, 1, 1);

	mTextToFindLineEdit = new QLineEdit( this );
	mTextToFindLineEdit->setObjectName(QStringLiteral("mTextToFindLineEdit"));

	gridLayout_3->addWidget(mTextToFindLineEdit, 0, 1, 1, 1);

	replaceLabel = new QLabel( this );
	replaceLabel->setObjectName(QStringLiteral("replaceLabel"));

	gridLayout_3->addWidget(replaceLabel, 1, 0, 1, 1);

	textToReplace = new QLineEdit( this );
	textToReplace->setObjectName(QStringLiteral("textToReplace"));

	gridLayout_3->addWidget(textToReplace, 1, 1, 1, 1);


	verticalLayout_2->addLayout(gridLayout_3);


	verticalLayout_5->addLayout(verticalLayout_2);

	errorLabel = new QLabel( this );
	errorLabel->setObjectName(QStringLiteral("errorLabel"));

	verticalLayout_5->addWidget(errorLabel);

	horizontalLayout = new QHBoxLayout();
	horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
	groupBox = new QGroupBox( this );
	groupBox->setObjectName(QStringLiteral("groupBox"));
	verticalLayout_3 = new QVBoxLayout(groupBox);
	verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
	downRadioButton = new QRadioButton(groupBox);
	downRadioButton->setObjectName(QStringLiteral("downRadioButton"));
	downRadioButton->setChecked(true);

	verticalLayout_3->addWidget(downRadioButton);

	upRadioButton = new QRadioButton(groupBox);
	upRadioButton->setObjectName(QStringLiteral("upRadioButton"));

	verticalLayout_3->addWidget(upRadioButton);


	horizontalLayout->addWidget(groupBox);

	groupBox_2 = new QGroupBox( this );
	groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
	verticalLayout_4 = new QVBoxLayout(groupBox_2);
	verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
	caseCheckBox = new QCheckBox(groupBox_2);
	caseCheckBox->setObjectName(QStringLiteral("caseCheckBox"));

	verticalLayout_4->addWidget(caseCheckBox);

	wholeCheckBox = new QCheckBox(groupBox_2);
	wholeCheckBox->setObjectName(QStringLiteral("wholeCheckBox"));

	verticalLayout_4->addWidget(wholeCheckBox);

	regexCheckBox = new QCheckBox(groupBox_2);
	regexCheckBox->setObjectName(QStringLiteral("regexCheckBox"));

	verticalLayout_4->addWidget(regexCheckBox);


	horizontalLayout->addWidget(groupBox_2);


	verticalLayout_5->addLayout(horizontalLayout);


	gridLayout->addLayout(verticalLayout_5, 0, 0, 1, 1);

	verticalLayout = new QVBoxLayout();
	verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
	verticalLayout->setContentsMargins(10, -1, -1, -1);
	findButton = new QPushButton( this );
	findButton->setObjectName(QStringLiteral("findButton"));
	findButton->setEnabled(false);

	verticalLayout->addWidget(findButton);

	closeButton = new QPushButton( this );
	closeButton->setObjectName(QStringLiteral("closeButton"));

	verticalLayout->addWidget(closeButton);

	replaceButton = new QPushButton( this );
	replaceButton->setObjectName(QStringLiteral("replaceButton"));

	verticalLayout->addWidget(replaceButton);

	replaceAllButton = new QPushButton( this );
	replaceAllButton->setObjectName(QStringLiteral("replaceAllButton"));

	verticalLayout->addWidget(replaceAllButton);

	verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	verticalLayout->addItem(verticalSpacer);


	gridLayout->addLayout(verticalLayout, 0, 1, 1, 1);

#ifndef QT_NO_SHORTCUT
	label->setBuddy(mTextToFindLineEdit);
	replaceLabel->setBuddy(textToReplace);
#endif // QT_NO_SHORTCUT
	QWidget::setTabOrder(mTextToFindLineEdit, textToReplace);
	QWidget::setTabOrder(textToReplace, downRadioButton);
	QWidget::setTabOrder(downRadioButton, upRadioButton);
	QWidget::setTabOrder(upRadioButton, caseCheckBox);
	QWidget::setTabOrder(caseCheckBox, wholeCheckBox);
	QWidget::setTabOrder(wholeCheckBox, regexCheckBox);
	QWidget::setTabOrder(regexCheckBox, findButton);
	QWidget::setTabOrder(findButton, closeButton);

	retranslateUi();

	QMetaObject::connectSlotsByName( this );
} // setupUi


void FindReplaceForm::retranslateUi()
{
	setWindowTitle(QApplication::translate("FindReplaceForm", "Form", 0));
	label->setText(QApplication::translate("FindReplaceForm", "&Find:", 0));
	replaceLabel->setText(QApplication::translate("FindReplaceForm", "R&eplace with:", 0));
	errorLabel->setText(QApplication::translate("FindReplaceForm", "errorLabel", 0));
	groupBox->setTitle(QApplication::translate("FindReplaceForm", "D&irection", 0));
	downRadioButton->setText(QApplication::translate("FindReplaceForm", "&Down", 0));
	upRadioButton->setText(QApplication::translate("FindReplaceForm", "&Up", 0));
	groupBox_2->setTitle(QApplication::translate("FindReplaceForm", "&Options", 0));
	caseCheckBox->setText(QApplication::translate("FindReplaceForm", "&Case sensitive", 0));
	wholeCheckBox->setText(QApplication::translate("FindReplaceForm", "&Whole words only", 0));
#ifndef QT_NO_TOOLTIP
	regexCheckBox->setToolTip(QApplication::translate("FindReplaceForm", "whether the text to search should be interpreted as a regular expression", 0));
#endif // QT_NO_TOOLTIP
	regexCheckBox->setText(QApplication::translate("FindReplaceForm", "R&egular Expression", 0));
	findButton->setText(QApplication::translate("FindReplaceForm", "&Find", 0));
	closeButton->setText(QApplication::translate("FindReplaceForm", "&Close", 0));
	replaceButton->setText(QApplication::translate("FindReplaceForm", "&Replace", 0));
	replaceAllButton->setText(QApplication::translate("FindReplaceForm", "Replace &All", 0));
} // retranslateUi




FindReplaceForm::FindReplaceForm(QWidget *parent) 	//parent = nullptr
	: QWidget(parent)
{
	CreateWidgets();

	errorLabel->setText("");

	connect(mTextToFindLineEdit, SIGNAL(textChanged(QString)), this, SLOT(textToFindChanged()));
	connect(mTextToFindLineEdit, SIGNAL(textChanged(QString)), this, SLOT(validateRegExp(QString)));

	connect(regexCheckBox, SIGNAL(toggled(bool)), this, SLOT(regexpSelected(bool)));

	connect(findButton, SIGNAL(clicked()), this, SLOT(find()));
	connect(closeButton, SIGNAL(clicked()), parent, SLOT(close()));

	connect(replaceButton, SIGNAL(clicked()), this, SLOT(replace()));
	connect(replaceAllButton, SIGNAL(clicked()), this, SLOT(replaceAll()));
}

FindReplaceForm::~FindReplaceForm()
{}

void FindReplaceForm::SetFindOnly( bool find_only )
{
	replaceLabel->setVisible( !find_only );
	textToReplace->setVisible( !find_only );
	replaceButton->setVisible( !find_only );
	replaceAllButton->setVisible( !find_only );
}

void FindReplaceForm::PrepareToFind()
{
	if ( mEditor->HasSelectedText() )
		mTextToFindLineEdit->setText( mEditor->SelectedText() );

	mTextToFindLineEdit->setFocus();
}


void FindReplaceForm::setTextEdit( FindReplaceInterface *edit ) 
{
	mEditor = edit;
	//connect( edit, &FindReplaceInterface::CopyAvailable, replaceButton, &QPushButton::setEnabled );
	//connect( edit, &FindReplaceInterface::CopyAvailable, replaceAllButton, &QPushButton::setEnabled );
}

void FindReplaceForm::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
		case QEvent::LanguageChange:
			retranslateUi();
			break;
		default:
			break;
	}
}

void FindReplaceForm::textToFindChanged() 
{
	findButton->setEnabled(mTextToFindLineEdit->text().size() > 0);
}

void FindReplaceForm::regexpSelected(bool sel) 
{
	if (sel)
		validateRegExp(mTextToFindLineEdit->text());
	else
		validateRegExp("");
}

void FindReplaceForm::validateRegExp(const QString &text) 
{
	if (!regexCheckBox->isChecked() || text.size() == 0) {
		errorLabel->setText("");
		return; // nothing to validate
	}

	QRegExp reg(text, caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive );

	if (reg.isValid()) 
	{
		showError("");
	} 
	else 
	{
		showError(reg.errorString());
	}
}

void FindReplaceForm::showError(const QString &error) 
{
	if (error == "") 
	{
		errorLabel->setText("");
	} 
	else 
	{
		errorLabel->setText("<span style=\" font-weight:600; color:#ff0000;\">" +
			error +
			"</span>");
	}
}

void FindReplaceForm::showMessage(const QString &message) 
{
	if (message == "") 
	{
		errorLabel->setText("");
	} 
	else 
	{
		errorLabel->setText("<span style=\" font-weight:600; color:green;\">" +
			message +
			"</span>");
	}
}

void FindReplaceForm::find() 
{
	find(downRadioButton->isChecked());
}

void FindReplaceForm::find(bool next) 
{
	assert__( mEditor );

	// backward search
	// 
	bool back = !next;

	const QString &toSearch = mTextToFindLineEdit->text();

	bool result = false;

	QTextDocument::FindFlags flags;

	if (back)
		flags |= QTextDocument::FindBackward;

	if (caseCheckBox->isChecked())
		flags |= QTextDocument::FindCaseSensitively;

	if (wholeCheckBox->isChecked())
		flags |= QTextDocument::FindWholeWords;

	if (regexCheckBox->isChecked()) 
	{
		QRegExp reg( toSearch, caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive );

		//qDebug() << "searching for regexp: " << reg.pattern();

		result = mEditor->FindRE( reg, flags );
	} 
	else 
	{
		//qDebug() << "searching for: " << toSearch;

		result = mEditor->Find( toSearch, flags );
	}

	if (result) 
	{
		showError("");
	} 
	else 
	{
		showError(tr("no match found"));

		// move to the beginning of next find
		// 
		if ( back )
			mEditor->MoveToTextEnd();
		else 
			mEditor->MoveToFirstLine();
	}
}

void FindReplaceForm::replace() 
{
	if (!mEditor->HasSelectedText()) 
	{
		find();
	} 
	else 
	{
		mEditor->InsertText( textToReplace->text() );
		find();
	}
}

void FindReplaceForm::replaceAll() 
{
	if (!mEditor->HasSelectedText()) 
	{
		find();
	} 
	int i=0;
	while (mEditor->HasSelectedText())
	{
		mEditor->InsertText( textToReplace->text() );
		find();
		i++;
	}
	showMessage(tr("Replaced %1 occurrence(s)").arg(i));
}

void FindReplaceForm::writeSettings(QSettings &settings, const QString &prefix) 
{
	settings.beginGroup(prefix);
	settings.setValue(TEXT_TO_FIND, mTextToFindLineEdit->text());
	settings.setValue(TEXT_TO_REPLACE, textToReplace->text());
	settings.setValue(DOWN_RADIO, downRadioButton->isChecked());
	settings.setValue(UP_RADIO, upRadioButton->isChecked());
	settings.setValue(CASE_CHECK, caseCheckBox->isChecked());
	settings.setValue(WHOLE_CHECK, wholeCheckBox->isChecked());
	settings.setValue(REGEXP_CHECK, regexCheckBox->isChecked());
	settings.endGroup();
}

void FindReplaceForm::readSettings(QSettings &settings, const QString &prefix) 
{
	settings.beginGroup(prefix);
	mTextToFindLineEdit->setText(settings.value(TEXT_TO_FIND, "").toString());
	textToReplace->setText(settings.value(TEXT_TO_REPLACE, "").toString());
	downRadioButton->setChecked(settings.value(DOWN_RADIO, true).toBool());
	upRadioButton->setChecked(settings.value(UP_RADIO, false).toBool());
	caseCheckBox->setChecked(settings.value(CASE_CHECK, false).toBool());
	wholeCheckBox->setChecked(settings.value(WHOLE_CHECK, false).toBool());
	regexCheckBox->setChecked(settings.value(REGEXP_CHECK, false).toBool());
	settings.endGroup();
}










FindReplaceDialog::FindReplaceDialog( bool find_only, bool modal, QWidget *parent ) 
	: QDialog(parent)
	, mFindOnly( find_only )
{
	setModal( modal );

	if (objectName().isEmpty())
		setObjectName(QStringLiteral("FindReplaceDialog"));
	resize(342, 140);
	gridLayout = new QGridLayout( this );
	gridLayout->setObjectName(QStringLiteral("gridLayout"));
	findReplaceForm = new FindReplaceForm( this );
	findReplaceForm->setObjectName(QStringLiteral("findReplaceForm"));

	gridLayout->addWidget(findReplaceForm, 0, 0, 1, 1);

	QMetaObject::connectSlotsByName( this );

	SetFindOnly( mFindOnly );
}

FindReplaceDialog::~FindReplaceDialog()
{
}


//virtual 
void FindReplaceDialog::changeEvent( QEvent *e )
{
	base_t::changeEvent(e);

    switch (e->type()) 
	{
    case QEvent::LanguageChange:
		findReplaceForm->retranslateUi();
        break;
	default:
        break;
    }
}


//virtual 
bool FindReplaceDialog::eventFilter( QObject *object, QEvent *event )
{
	if ( event->type() == QEvent::FocusOut )
	{
		auto *f = qApp->focusWidget();
		if ( mEditorWidget && f != this && !isAncestorOf( f ) )
		{
			if ( !mEditorWidget->hasFocus() )
			{
				findReplaceForm->showError("");
				setVisible( false );
			}
		}
	}

	return base_t::eventFilter( object, event );
}

// (*) If we don't use the timer, we risk a pure call exception if 
// setTextEdit is called by the FindReplaceInterface ctor (as it is),
// because the object is not completely constructed (typically
// FindReplaceInterface is used as a mixin, that is, part of another
// object, and EditorWidget cannot be called during construction)
// 
void FindReplaceDialog::setTextEdit( FindReplaceInterface *editor ) 
{
    findReplaceForm->setTextEdit( editor );
	QTimer::singleShot( 1000, [this, editor]()	// (*)
	{
		mEditorWidget = editor ? editor->EditorWidget() : nullptr;
		if ( mEditorWidget )
		{
			if ( !isModal() )
				mEditorWidget->installEventFilter( this );
		}
	} );
}

void FindReplaceDialog::SetFindOnly( bool find_only )
{
	mFindOnly = find_only;
	findReplaceForm->SetFindOnly( mFindOnly );
	QString sub_title, title = mFindOnly ? tr( "Find" ) : QApplication::translate( "FindReplaceDialog", "Find/Replace", 0 );
	if ( mEditorWidget )
		sub_title = " - " + mEditorWidget->windowTitle();

	setWindowTitle( title + sub_title );
}

void FindReplaceDialog::PrepareToFind( bool find_only )
{
	SetFindOnly( find_only );
	findReplaceForm->PrepareToFind();
	show();
}


void FindReplaceDialog::writeSettings(QSettings &settings, const QString &prefix) 
{
    findReplaceForm->writeSettings(settings, prefix);
}

void FindReplaceDialog::readSettings(QSettings &settings, const QString &prefix) 
{
    findReplaceForm->readSettings(settings, prefix);
}

void FindReplaceDialog::findNext() 
{
    findReplaceForm->findNext();
}

void FindReplaceDialog::findPrev() 
{
    findReplaceForm->findPrev();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_FindReplaceDialog.cpp"
