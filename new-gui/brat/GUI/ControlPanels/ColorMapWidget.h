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
#if !defined GUI_CONTROL_PANELS_COLOR_MAP_WIDGET_H
#define GUI_CONTROL_PANELS_COLOR_MAP_WIDGET_H


class CBratLookupTable;
class CColorButton;


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
	void DrawLUT( const CBratLookupTable *lut );

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

	//static data

	static const int smNumberOfColorLabels = 5;

	//data

	QGroupBox *mShowContourGroupBox = nullptr;
	QCheckBox *mShowContourCheck = nullptr;
	QLineEdit *mNumberOfContoursEdit = nullptr;
    QLabel *mContourPrecisionLabel = nullptr;
	QLineEdit *mContourPrecisionGrid1Edit = nullptr;
	QLineEdit *mContourPrecisionGrid2Edit = nullptr;
	CColorButton *mContourColorButton = nullptr;
	QColor mContourColor;
    QLineEdit *mContourWidthEdit = nullptr;
    QLabel *mContourWidthLabel = nullptr;
	QCheckBox *mShowSolidColor = nullptr;
	CColorMapLabel *mColorMapLabel = nullptr;
	QComboBox *mColorTables = nullptr;

	CBratLookupTable *mLut = nullptr;

	QLineEdit *mColorRangeMinEdit = nullptr;
	QLineEdit *mColorRangeMaxEdit = nullptr;
	QPushButton *mCalculateMinMax = nullptr;
	QGroupBox *mColorRangeGroup = nullptr;
	QGroupBox *mColorMapGroup = nullptr;
	std::vector<QLabel*> mColorLabels;
	double mDataMin = std::numeric_limits<double>::max();
	double mDataMax = std::numeric_limits<double>::lowest();
	double mColorRangeMin = std::numeric_limits<double>::max();
	double mColorRangeMax = std::numeric_limits<double>::lowest();
	

	bool mOnlyLUT = false;

	//construction / destruction

	void CreateWidgets( bool show_range );
public:
    explicit CColorMapWidget( bool only_lut, bool show_range, QWidget *parent );

    virtual ~CColorMapWidget()
    {}

    // access

	//...contours

	void SetContoursTitle( const QString &title )
	{
		if ( mShowContourGroupBox )
			mShowContourGroupBox->setTitle( title );
	}


	void SetShowContour( bool checked );

	unsigned NumberOfContours() const;

	void SetNumberOfContours( unsigned contours );


	QColor ContourColor() const { return mContourColor; }

	void SetContourColor( QColor color );

	void HideContourColorWidget();



	double ContoursWidth() const;

	void SetContoursWidth( double contours );

	void HideContourWidthWidget() 
	{ 
		if ( mContourWidthEdit ) 
			mContourWidthEdit->setVisible( false ); 
		if ( mContourWidthLabel ) 
			mContourWidthLabel->setVisible( false ); 
	}


	void ShowPrecisionParameters( bool show );
	void PrecisionParameters( unsigned int &grid1, unsigned int &grid2 );
	void SetPrecisionParameters( unsigned int grid1, unsigned int grid2 );


	//...show solid color

	void SetShowSolidColor( bool checked );

	void EnableOnlySolidColor( bool enable_only );


	//...color table

	void SetLUT( CBratLookupTable *lut, double data_min, double data_max );

	QString itemText( int index )
	{
		return mColorTables->itemText( index );
	}

	double ColorRangeMin() const { return mColorRangeMin; }
	double ColorRangeMax() const { return mColorRangeMax; }



	QString currentText()
	{
		return mColorTables->currentText();
	}

	void setCurrentText( const QString &text )
	{
		mColorTables->setCurrentIndex( mColorTables->findText( text ) );
	}

protected:
	void SetRange( double m, double M );
	void UpdateLabels();

signals:
	void CurrentIndexChanged( int );
	void ShowContourToggled( bool );
	void ContourColorSelected();
	void ShowSolidColorToggled( bool );
	void ContoursEditReturnPressed();
	void ContourWidthReturnPressed();
	void ContourPrecisionGrid1EditPressed();
	void ContourPrecisionGrid2EditPressed();

protected slots:

	void HandleContourColorSelected();
	void HandleColorRangeMinChanged();
	void HandleColorRangeMaxChanged();
	void HandleColorRangeMinFinished();
	void HandleColorRangeMaxFinished();
	void HandleCalculateMinMaxFromData();
	void HandleColorTablesIndexChanged( int index );
};





#endif	//GUI_CONTROL_PANELS_COLOR_MAP_WIDGET_H
