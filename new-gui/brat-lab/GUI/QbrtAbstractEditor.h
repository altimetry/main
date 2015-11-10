#if !defined QBRT_ABSTRACT_EDITOR_H
#define QBRT_ABSTRACT_EDITOR_H




class EditorBase
{
protected:

	//data

private:

protected:
	//ctor/dtor

	EditorBase()
	{}

	virtual ~EditorBase()
	{}


public:
	virtual QAction *windowMenuAction() const = 0;

	virtual QString title() const = 0;		//for external (editors clients) use

	virtual bool okToContinue() = 0;
};


template< class QWIDGET >
class QbrtAbstractEditor : public QWIDGET, public EditorBase
{
	typedef QWIDGET base_t;

	QAction *mAction = nullptr;

public:
	QbrtAbstractEditor(QWidget *parent) : QWIDGET(parent), EditorBase()
	{
		mAction = new QAction(this);
		mAction->setCheckable(true);
        this->connect(mAction, SIGNAL(triggered()), this, SLOT(show()));
        this->connect(mAction, SIGNAL(triggered()), this, SLOT(setFocus()));
	}

    QAction *windowMenuAction() const override { return mAction; }

	virtual QString title() const override { return mAction->text(); }
};



#endif	//QBRT_ABSTRACT_EDITOR_H
