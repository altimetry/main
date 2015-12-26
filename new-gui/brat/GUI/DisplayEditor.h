#ifndef GUI_DISPLAY_EDITOR_H
#define GUI_DISPLAY_EDITOR_H


#include <QMainWindow>

class CAbstractDisplayEditor : public QMainWindow
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	using base_t = QMainWindow;

	QDockWidget *mWorkingDock = nullptr;
	QWidget *mWorkingDockContent = nullptr;
	QVBoxLayout *mWorkingDockLayout = nullptr;

	QToolBar *mToolBar = nullptr;
	QWidget *mGraphic = nullptr;

	void CreateWorkingDock();
	void CreateGraphicsBar();
	void CreateStatusBar();
public:
    CAbstractDisplayEditor( QWidget *parent = nullptr, QWidget *view = nullptr );

	virtual ~CAbstractDisplayEditor();

public:
	void AddView( QWidget *view );

protected:
	QLayout* CreateWorkingLayout( Qt::Orientation orientation );
	QLayout* AddWorkingWidget( QWidget *w, QLayout *l = nullptr );

	QSize sizeHint() const override
    {
        return QSize(72 * fontMetrics().width('x'),
                     25 * fontMetrics().lineSpacing());
    }

	virtual void Handle2D( bool checked ) = 0;
	virtual void Handle3D( bool checked ) = 0;
	virtual void HandleText( bool checked ) = 0;
	virtual void HandleLog( bool checked ) = 0;

protected slots:
	void Handle2D_slot( bool checked );
	void Handle3D_slot( bool checked );
	void HandleText_slot( bool checked );
	void HandleLog_slot( bool checked );
};



class CMapEditor : public CAbstractDisplayEditor
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif


	using base_t = CAbstractDisplayEditor;

public:
    CMapEditor( QWidget *parent = nullptr, QWidget *view = nullptr );

	virtual ~CMapEditor();

protected:
	virtual void Handle2D( bool checked ) override;
	virtual void Handle3D( bool checked ) override;
	virtual void HandleText( bool checked ) override;
	virtual void HandleLog( bool checked ) override;
};




class CPlotEditor : public CAbstractDisplayEditor
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	using base_t = CAbstractDisplayEditor;

public:
    CPlotEditor( QWidget *parent = nullptr, QWidget *view = nullptr );

	virtual ~CPlotEditor();

protected:
	virtual void Handle2D( bool checked ) override;
	virtual void Handle3D( bool checked ) override;
	virtual void HandleText( bool checked ) override;
	virtual void HandleLog( bool checked ) override;
};





#endif			// GUI_DISPLAY_EDITOR_H
