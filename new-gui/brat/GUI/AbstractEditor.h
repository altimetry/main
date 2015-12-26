#if !defined GUI_ABSTRACT_EDITOR_H
#define GUI_ABSTRACT_EDITOR_H




class CEditorBase
{
protected:

	//data

private:

protected:
	//ctor/dtor

	CEditorBase()
	{}

	virtual ~CEditorBase()
	{}


public:
	virtual QAction *windowMenuAction() const = 0;

	virtual QString title() const = 0;		//for external (editors clients) use

	virtual bool okToContinue() = 0;
};


template< class QWIDGET >
class CAbstractEditor : public QWIDGET, public CEditorBase
{
	typedef QWIDGET base_t;

	QAction *mAction = nullptr;

public:
	CAbstractEditor(QWidget *parent) : QWIDGET(parent), CEditorBase()
	{
		mAction = new QAction(this);
		mAction->setCheckable(true);
        this->connect(mAction, SIGNAL(triggered()), this, SLOT(show()));
        this->connect(mAction, SIGNAL(triggered()), this, SLOT(setFocus()));
	}

    QAction *windowMenuAction() const override { return mAction; }

	virtual QString title() const override { return mAction->text(); }
};



#endif	//GUI_ABSTRACT_EDITOR_H
