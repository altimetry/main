#if !defined GUI_CONTROL_PANELS_COLOR_MAP_WIDGET_H
#define GUI_CONTROL_PANELS_COLOR_MAP_WIDGET_H


class CBratLookupTable;


class CColorMapLabel : public QLabel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types

	using base_t = QLabel;

	//data

	const CBratLookupTable *mLut = nullptr;

public:
	CColorMapLabel( QWidget *parent = nullptr );

	virtual ~CColorMapLabel()
	{}

	void SetLUT( const CBratLookupTable *lut );

protected:

	virtual void resizeEvent( QResizeEvent * ) override;
};





class CColorMapWidget : public QWidget
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    // types

    using base_t = QWidget;

	friend struct CPlotControlsPanelCurveOptions;
	friend struct CMapControlsPanelDataLayers;


	//data

	QCheckBox *mShowContour = nullptr;
	QCheckBox *mShowSolidColor = nullptr;
	CColorMapLabel *mColorMapLabel = nullptr;
	QComboBox *mColorTables = nullptr;

	CBratLookupTable *mLut = nullptr;

	bool mOnlyLUT = false;

	//construction / destruction

public:
    explicit CColorMapWidget( bool only_lut, QWidget *parent );

    virtual ~CColorMapWidget()
    {}

    // access

	void SetShowContour( bool checked );

	void SetShowSolidColor( bool checked );

	void SetLUT( CBratLookupTable *lut );

	QString itemText( int index )
	{
		return mColorTables->itemText( index );
	}

	// hack
	void HideCheckBoxes();

	QString currentText()
	{
		return mColorTables->currentText();
	}

	void setCurrentText( const QString &text )
	{
		mColorTables->setCurrentIndex( mColorTables->findText( text ) );
	}

signals:
	void CurrentIndexChanged( int );
	void ShowContourToggled( bool );
	void ShowSolidColorToggled( bool );

protected slots:

void HandleColorTablesIndexChanged( int index );
};





#endif	//GUI_CONTROL_PANELS_COLOR_MAP_WIDGET_H
