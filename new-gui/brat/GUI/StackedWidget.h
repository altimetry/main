#if !defined GUI_STACKED_WIDGET_H
#define GUI_STACKED_WIDGET_H


struct CStackedWidget : public QStackedWidget
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// types

	using base_t = QStackedWidget;
public:

	using stack_button_type = QToolButton;

	struct PageInfo
	{
		QWidget *mWidget;
		std::string mName;
		std::string mTip;
		std::string mIconPath;
		bool mUseToolButtons;
	};

	// data

protected:
	QButtonGroup *mGroup = nullptr;

public:
	CStackedWidget( QWidget *parent, const std::vector< PageInfo > &widgets, int iselected = 0 );

	virtual ~CStackedWidget()
	{}


	// access

    int CurrentIndex() const
    {
        return currentIndex();
    }


    void SetCurrentIndex( int index );



    QAbstractButton* Button( int index ) const
	{
		return mGroup->buttons().at( index );
	}


	// operations

signals:

	void PageChanged( int i );


protected slots:

    void buttonToggled( bool checked );
};





#endif	//GUI_STACKED_WIDGET_H
