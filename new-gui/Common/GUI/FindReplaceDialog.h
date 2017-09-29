#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QtWidgets/QDialog>


class QTextEdit;
class QSettings;
class QRadioButton;

///-------------------------------------------------------------------------------------------------
/// \class	FindReplaceForm
///
/// \brief	The form for the find/replace dialog.  The form presents the typical widgets you find
/// 		in standard find/replace dialogs, and it acts on a QTextEdit.
/// 		
/// 		\image html Screenshot-FindReplace.png
/// 		
/// 		You need to set the QTextEdit explicitly, using the method setTextEdit(QTextEdit
/// 		*mEditor).
/// 		
/// 		For instance \code m_findReplaceDialog = new FindReplaceDialog(this);
/// 		m_findReplaceDialog->setModal(false);
/// 		m_findReplaceDialog->setTextEdit(ui->mEditor);
/// 		\endcode
/// 		
/// 		The find functionalities is available even if the find dialog is not shown: if
/// 		something to search for was already specified, the application can call the methods
/// 		findNext() and findPrev() (e.g., by connecting them to menu items).
/// 		
/// 		In case a regular expression is used as the search term, the form also checks whether
/// 		the expression is a valid regular expression (You may want to take a look at the
/// 		syntax of regular expressions: http://doc.trolltech.com/qregexp.html).
/// 		
/// 		The form provides also functionalities to save and restore its state using a
/// 		QSettings object (i.e., the last word searched for, the options of the form, etc.)
/// 		via the methods writeSettings()
/// 		and readSettings().
/// 		
/// 		You can take a look at the \ref examples page.
///
/// \date	05-09-2017
///-------------------------------------------------------------------------------------------------

class CFindReplaceInterface;
//class CTextWidget;
//using FindReplaceInterface = CTextWidget;
using FindReplaceInterface = CFindReplaceInterface;


class FindReplaceForm : public QWidget 
{
	Q_OBJECT

	friend class FindReplaceDialog;

protected:

	/// the text editor (possibly) associated with this form
	FindReplaceInterface *mEditor = nullptr;

	QGridLayout *gridLayout = nullptr;
	QVBoxLayout *verticalLayout_5 = nullptr;
	QVBoxLayout *verticalLayout_2 = nullptr;
	QGridLayout *gridLayout_3 = nullptr;
	QLabel *label = nullptr;
	QLineEdit *mTextToFindLineEdit = nullptr;
	QLabel *replaceLabel = nullptr;
	QLineEdit *textToReplace = nullptr;
	QLabel *errorLabel = nullptr;
	QHBoxLayout *horizontalLayout = nullptr;
	QGroupBox *groupBox = nullptr;
	QVBoxLayout *verticalLayout_3 = nullptr;
	QRadioButton *downRadioButton = nullptr;
	QRadioButton *upRadioButton = nullptr;
	QGroupBox *groupBox_2 = nullptr;
	QVBoxLayout *verticalLayout_4 = nullptr;
	QCheckBox *caseCheckBox = nullptr;
	QCheckBox *wholeCheckBox = nullptr;
	QCheckBox *regexCheckBox = nullptr;
	QVBoxLayout *verticalLayout = nullptr;
	QPushButton *findButton = nullptr;
	QPushButton *closeButton = nullptr;
	QPushButton *replaceButton = nullptr;
	QPushButton *replaceAllButton = nullptr;
	QSpacerItem *verticalSpacer = nullptr;

	void CreateWidgets();
public:
	FindReplaceForm(QWidget *parent);
	virtual ~FindReplaceForm();

	void retranslateUi();

	///-------------------------------------------------------------------------------------------------
	/// \fn	void FindReplaceForm::setTextEdit( FindReplaceInterface *edit );
	///
	/// \brief	Associates the text editor where to perform the search
	/// 		@param textEdit_.
	///
	/// \date	06-09-2017
	///
	/// \param [in,out]	edit	If non-null, the edit.
	///-------------------------------------------------------------------------------------------------

	void setTextEdit( FindReplaceInterface *edit );

	void SetFindOnly( bool find_only );

	void PrepareToFind();

	///-------------------------------------------------------------------------------------------------
	/// \fn	virtual void FindReplaceForm::writeSettings(QSettings &settings, const QString &prefix = "FindReplaceDialog");
	///
	/// \brief	Writes the state of the form to the passed settings.
	/// 		@param settings
	/// 		@param prefix the prefix to insert in the settings.
	///
	/// \date	06-09-2017
	///
	/// \param [in,out]	settings	Options for controlling the operation.
	/// \param 		   	prefix  	(Optional) The prefix.
	///-------------------------------------------------------------------------------------------------

	virtual void writeSettings(QSettings &settings, const QString &prefix = "FindReplaceDialog");

	///-------------------------------------------------------------------------------------------------
	/// \fn	virtual void FindReplaceForm::readSettings(QSettings &settings, const QString &prefix = "FindReplaceDialog");
	///
	/// \brief	Reads the state of the form from the passed settings.
	/// 		@param settings
	/// 		@param prefix the prefix to look for in the settings.
	///
	/// \date	06-09-2017
	///
	/// \param [in,out]	settings	Options for controlling the operation.
	/// \param 		   	prefix  	(Optional) The prefix.
	///-------------------------------------------------------------------------------------------------

	virtual void readSettings(QSettings &settings, const QString &prefix = "FindReplaceDialog");


public slots:

	///-------------------------------------------------------------------------------------------------
	/// \fn	void FindReplaceForm::find(bool down);
	///
	/// \brief	performs the find task
	/// 		@param down whether to find the next or the previous
	/// 		occurrence.
	///
	/// \date	06-09-2017
	///
	/// \param	down	True to down.
	///-------------------------------------------------------------------------------------------------

	void find(bool down);

	///-------------------------------------------------------------------------------------------------
	/// \fn	void FindReplaceForm::find();
	///
	/// \brief	Finds the next occurrence.
	///
	/// \date	06-09-2017
	///-------------------------------------------------------------------------------------------------

	void find();

	///-------------------------------------------------------------------------------------------------
	/// \fn	void FindReplaceForm::findNext()
	///
	/// \brief	Finds the next occurrence.
	///
	/// \date	06-09-2017
	///-------------------------------------------------------------------------------------------------

	void findNext() { find(true); }

	///-------------------------------------------------------------------------------------------------
	/// \fn	void FindReplaceForm::findPrev()
	///
	/// \brief	Finds the previous occurrence.
	///
	/// \date	06-09-2017
	///-------------------------------------------------------------------------------------------------

	void findPrev() { find(false); }

	///-------------------------------------------------------------------------------------------------
	/// \fn	void FindReplaceForm::replace();
	///
	/// \brief	Replaces the found occurrences and goes to the next occurrence.
	///
	/// \date	06-09-2017
	///-------------------------------------------------------------------------------------------------

	void replace();

	///-------------------------------------------------------------------------------------------------
	/// \fn	void FindReplaceForm::replaceAll();
	///
	/// \brief	Replaces all the found occurrences.
	///
	/// \date	06-09-2017
	///-------------------------------------------------------------------------------------------------

	void replaceAll();

protected:
	void changeEvent(QEvent *e);

	///-------------------------------------------------------------------------------------------------
	/// \fn	void FindReplaceForm::showError(const QString &error);
	///
	/// \brief	shows an error in the dialog.
	///
	/// \date	06-09-2017
	///
	/// \param	error	The error.
	///-------------------------------------------------------------------------------------------------

	void showError(const QString &error);

	///-------------------------------------------------------------------------------------------------
	/// \fn	void FindReplaceForm::showMessage(const QString &message);
	///
	/// \brief	shows a message in the dialog.
	///
	/// \date	06-09-2017
	///
	/// \param	message	The message.
	///-------------------------------------------------------------------------------------------------

	void showMessage(const QString &message);

protected slots:

	///-------------------------------------------------------------------------------------------------
	/// \fn	void FindReplaceForm::textToFindChanged();
	///
	/// \brief	when the text edit contents changed.
	///
	/// \date	06-09-2017
	///-------------------------------------------------------------------------------------------------

	void textToFindChanged();

	///-------------------------------------------------------------------------------------------------
	/// \fn	void FindReplaceForm::validateRegExp(const QString &text);
	///
	/// \brief	checks whether the passed text is a valid regexp.
	///
	/// \date	06-09-2017
	///
	/// \param	text	The text.
	///-------------------------------------------------------------------------------------------------

	void validateRegExp(const QString &text);

	///-------------------------------------------------------------------------------------------------
	/// \fn	void FindReplaceForm::regexpSelected(bool sel);
	///
	/// \brief	the regexp checkbox was selected.
	///
	/// \date	06-09-2017
	///
	/// \param	sel	True to selected.
	///-------------------------------------------------------------------------------------------------

	void regexpSelected(bool sel);
};





///-------------------------------------------------------------------------------------------------
/// \class	FindReplaceDialog
///
/// \brief	A find/replace dialog.
/// 		
/// 		It relies on a FindReplaceForm object (see that class for the functionalities
/// 		provided).
///
/// \date	06-09-2017
///-------------------------------------------------------------------------------------------------

class FindReplaceDialog : public QDialog
{
	Q_OBJECT

	//types

	using base_t = QDialog;


	// other

	QGridLayout *gridLayout = nullptr;

	QWidget *mEditorWidget = nullptr;
	bool mFindOnly = true;				//this can change during dialog lifetime

protected:

	FindReplaceForm *findReplaceForm = nullptr;

public:

    FindReplaceDialog( bool find_only, bool modal, QWidget *parent = nullptr );

    virtual ~FindReplaceDialog();


    ///-------------------------------------------------------------------------------------------------
    /// \fn	void FindReplaceDialog::setTextEdit( FindReplaceInterface *editor );
    ///
    /// \brief	Associates the text editor where to perform the search
    /// 		@param mEditor.
    ///
    /// \date	06-09-2017
    ///
    /// \param [in,out]	editor	If non-null, the editor.
    ///-------------------------------------------------------------------------------------------------

    void setTextEdit( FindReplaceInterface *editor );

	///-------------------------------------------------------------------------------------------------
	/// \fn	void FindReplaceDialog::SetFindOnly( bool find_only );
	///
	/// \brief	Sets find only.
	///
	/// \date	06-09-2017
	///
	/// \param	find_only	True to find only.
	///-------------------------------------------------------------------------------------------------

	void SetFindOnly( bool find_only );

	///-------------------------------------------------------------------------------------------------
	/// \fn	void FindReplaceDialog::PrepareToFind( bool find_only );
	///
	/// \brief	Prepare to find.
	///
	/// \date	13-09-2017
	///
	/// \param	find_only	True to find only.
	///-------------------------------------------------------------------------------------------------

	void PrepareToFind( bool find_only );

	///-------------------------------------------------------------------------------------------------
    /// \fn	virtual void FindReplaceDialog::writeSettings(QSettings &settings, const QString &prefix = "FindReplaceDialog");
    ///
    /// \brief	Writes the state of the form to the passed settings.
    /// 		@param settings
    /// 		@param prefix the prefix to insert in the settings.
    ///
    /// \date	06-09-2017
    ///
    /// \param [in,out]	settings	Options for controlling the operation.
    /// \param 		   	prefix  	(Optional) The prefix.
    ///-------------------------------------------------------------------------------------------------

    virtual void writeSettings(QSettings &settings, const QString &prefix = "FindReplaceDialog");

    ///-------------------------------------------------------------------------------------------------
    /// \fn	virtual void FindReplaceDialog::readSettings(QSettings &settings, const QString &prefix = "FindReplaceDialog");
    ///
    /// \brief	Reads the state of the form from the passed settings.
    /// 		@param settings
    /// 		@param prefix the prefix to look for in the settings.
    ///
    /// \date	06-09-2017
    ///
    /// \param [in,out]	settings	Options for controlling the operation.
    /// \param 		   	prefix  	(Optional) The prefix.
    ///-------------------------------------------------------------------------------------------------

    virtual void readSettings(QSettings &settings, const QString &prefix = "FindReplaceDialog");

public slots:

    ///-------------------------------------------------------------------------------------------------
    /// \fn	void FindReplaceDialog::findNext();
    ///
    /// \brief	Finds the next occurrence.
    ///
    /// \date	06-09-2017
    ///-------------------------------------------------------------------------------------------------

    void findNext();

    ///-------------------------------------------------------------------------------------------------
    /// \fn	void FindReplaceDialog::findPrev();
    ///
    /// \brief	Finds the previous occurrence.
    ///
    /// \date	06-09-2017
    ///-------------------------------------------------------------------------------------------------

    void findPrev();

protected:

	virtual void changeEvent( QEvent *e ) override;

	virtual bool eventFilter(QObject *object, QEvent *event) override;
};





class CFindReplaceInterface
{
protected:
	FindReplaceDialog mFindReplaceDialog;

public:
	CFindReplaceInterface( bool find_only, bool modal, QWidget *dialog_parent )

        : mFindReplaceDialog( find_only, modal, dialog_parent )
	{
		mFindReplaceDialog.setTextEdit( this );
	}

	virtual ~CFindReplaceInterface()
	{}


	// For FindReplaceDialog usage

	virtual void CopyAvailable ( bool yes) = 0;			// cannot be const: QTextEdit

	virtual bool Find( const QString &toSearch, QTextDocument::FindFlags options ) = 0;

	virtual bool FindRE( const QRegExp &expr, QTextDocument::FindFlags options ) = 0;

	virtual void MoveToFirstLine() = 0;

	virtual void MoveToTextEnd() = 0;

	virtual bool HasSelectedText() const = 0;			// cannot be const: QTextEdit

	virtual QString SelectedText() = 0;					// cannot be const: QTextEdit

	virtual void InsertText( const QString &text ) = 0;

	virtual QWidget* EditorWidget() = 0;


	// For Editor Clients usage

	virtual void FindDialog()
	{
		mFindReplaceDialog.PrepareToFind( true );
	}
	virtual void ReplaceDialog()
	{
		mFindReplaceDialog.PrepareToFind( false );
	}
	virtual void FindNext()
	{
		mFindReplaceDialog.findNext();
	}

	virtual void FindPrev()
	{
		mFindReplaceDialog.findPrev();
	}
};





#endif // FINDREPLACEDIALOG_H
