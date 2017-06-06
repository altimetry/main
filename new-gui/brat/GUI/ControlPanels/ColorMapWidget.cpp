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

#include "new-gui/Common/QtUtils.h"

#include "DataModels/PlotData/BratLookupTable.h"

#include "GUI/ControlPanels/ColorButton.h"

#include "ColorMapWidget.h"


QColor wavelengthToColor( qreal lambda )
{
	// Based on: http://www.efg2.com/Lab/ScienceAndEngineering/Spectra.htm
	// The foregoing is based on: http://www.midnightkite.com/color.html
	struct Color {
		qreal c[ 3 ];
		QColor toColor( qreal factor ) const {
			qreal const gamma = 0.8;
			int ci[ 3 ];
			for ( int i = 0; i < 3; ++i ) {
				ci[ i ] = c[ i ] == 0.0 ? 0.0 : qRound( 255 * pow( c[ i ] * factor, gamma ) );
			}
			return QColor( ci[ 0 ], ci[ 1 ], ci[ 2 ] );
		}
	} color;
	qreal factor = 0.0;

	static qreal thresholds[] ={ 380, 440, 490, 510, 580, 645, 780 };
	for ( unsigned int i = 0; i < sizeof( thresholds ) / sizeof( thresholds[ 0 ] ); ++ i ) {
		qreal t1 = thresholds[ i ], t2 = thresholds[ i + 1 ];
		if ( lambda < t1 || lambda >= t2 ) continue;
		if ( i % 2 ) std::swap( t1, t2 );
		color.c[ i % 3 ] = ( i < 5 ) ? ( lambda - t2 ) / ( t1 - t2 ) : 0.0;;
		color.c[ 2 - i / 2 ] = 1.0;
		factor = 1.0;
		break;
	}

	// Let the intensity fall off near the vision limits
	if ( lambda >= 380 && lambda < 420 ) {
		factor = 0.3 + 0.7*( lambda - 380 ) / ( 420 - 380 );
	}
	else if ( lambda >= 700 && lambda < 780 ) {
		factor = 0.3 + 0.7*( 780 - lambda ) / ( 780 - 700 );
	}
	return color.toColor( factor );
}

QPixmap rainbow( int w, int h )
{
	QPixmap pm( w, h );
	QPainter p( &pm );
	qreal f1 = 1.0 / 400;
	qreal f2 = 1.0 / 780;
	for ( int x = 0; x < w; ++ x ) {
		// Iterate across frequencies, not wavelengths
		qreal lambda = 1.0 / ( f1 - ( x / qreal( w )*( f1 - f2 ) ) );
		p.setPen( wavelengthToColor( lambda ) );
		p.drawLine( x, 0, x, h );
	}
	return pm;
}



CColorMapLabel::CColorMapLabel( QWidget *parent )	//parent = nullptr 
	: base_t( parent )
{
	setMaximumHeight( 25 );
	setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
}


//virtual 
void CColorMapLabel::resizeEvent( QResizeEvent *event )
{
	base_t::resizeEvent( event );

	SetLUT( mLut );
}

void CColorMapLabel::SetLUT( const CBratLookupTable *lut )
{
	mLut = lut;
	int w = width();
	int h = height();
	QPixmap pm( w, h );
	pm.fill();			//critical for cloud
	if ( mLut && h > 0 && w > 0 )
	{
		QPainter p( &pm );
		auto *const lut = mLut->GetLookupTable();
		const size_t size = lut->GetNumberOfTableValues();
		for ( int x = 0; x < w; ++ x ) 
		{
			size_t index = x * size / w;
			p.setPen( lut->GetTableValue( index ) );
			p.drawLine( x, 0, x, h );
		}
	}
	setPixmap( pm );
	//else
	//	setPixmap( rainbow( w, h ) );
}




void CColorMapWidget::CreateWidgets( bool show_range )
{
	// generic data and tools

	QFont view_control_font = font();
	view_control_font.setPointSize( view_control_font.pointSize() - 1 );
	setFont( view_control_font );
	const int height = fontMetrics().lineSpacing() + 6;
	const int lwidth = fontMetrics().width('0') * 13;		//see line (*) in UpdateLabels(): 10 + '.' + 2 decimals

	auto shrinkh = [&height, &view_control_font]( QWidget *w )
	{
		w->setMaximumHeight( height );
		w->setFont( view_control_font );
		return w;
	};

	auto wshrinkh = [&shrinkh, &lwidth]() -> QLabel*
	{
		QLabel *w = new QLabel;
		shrinkh( w )->setMaximumWidth( lwidth );
		w->setStyleSheet("border: 1px solid  #000000 ;");
		return w;
	};


	// contours

	QLayout *solid_and_contour_l = nullptr;
	if ( !mOnlyLUT )
	{
        mShowSolidColor = new QCheckBox( "Show" );
		mNumberOfContoursEdit = new QLineEdit;		mNumberOfContoursEdit->setValidator( new QRegExpValidator( QRegExp( "[0-9]+" ) ) );
		mContourWidthEdit = new QLineEdit;	  		mContourWidthEdit->setValidator( new QRegExpValidator( QRegExp( "[0-9.]+" ) ) );

		mContourPrecisionLabel = new QLabel( "Precision" );
		mContourPrecisionGrid1Edit = new QLineEdit;	  	mContourPrecisionGrid1Edit->setValidator( new QRegExpValidator( QRegExp( "[0-9]+" ) ) );
		mContourPrecisionGrid2Edit = new QLineEdit;	  	mContourPrecisionGrid2Edit->setValidator( new QRegExpValidator( QRegExp( "[0-9]+" ) ) );

		mContourColorButton = new CColorButton;
		mContourWidthLabel = new QLabel( "Width" );
		mShowContourCheck = new QCheckBox( "Show" );
		mShowContourGroupBox = CreateGroupBox( ELayoutType::Horizontal, 
		{ 
			LayoutWidgets( Qt::Vertical, 
			{ 
				mShowContourCheck,
				LayoutWidgets( Qt::Horizontal, { new QLabel( "Number" ), shrinkh( mNumberOfContoursEdit ), mContourWidthLabel, shrinkh( mContourWidthEdit ) }, nullptr, 2,2,0,2,2 ),
				//LayoutWidgets( Qt::Horizontal, { mContourWidthLabel, shrinkh( mContourWidthEdit ) }, nullptr, 2,2,0,2,2 ),
				LayoutWidgets( Qt::Horizontal, { mContourPrecisionLabel, shrinkh( mContourPrecisionGrid1Edit ), shrinkh( mContourPrecisionGrid2Edit ) }, nullptr, 2,2,0,2,2 ),
			}, nullptr, 0,0,0,0,0 ),

			mContourColorButton, 

		}, "Contours", nullptr );
		ShowPrecisionParameters( false );
        solid_and_contour_l = LayoutWidgets( Qt::Vertical, { /*mShowSolidColor, */mShowContourGroupBox }, nullptr, 6, 2, 2, 2, 2 );
		mShowContourGroupBox->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
	}

	// color table

	mColorTables = new QComboBox;
	mColorMapLabel = new CColorMapLabel;

	// color range

	mColorRangeMinEdit = new QLineEdit;				mColorRangeMinEdit->setValidator( new QRegExpValidator( QRegExp( "[-0-9.]+" ) ) );
	mColorRangeMinEdit->setMaximumWidth( 80 );
	mColorRangeMaxEdit = new QLineEdit;		  		mColorRangeMaxEdit->setValidator( new QRegExpValidator( QRegExp( "[-0-9.]+" ) ) );
	mColorRangeMaxEdit->setMaximumWidth( 80 );
	mCalculateMinMax = new QPushButton( "Reset" );
	mCalculateMinMax->setAutoDefault( false );
	mCalculateMinMax->setDefault( false );

	QLayout *labels_l = nullptr;
	if ( show_range )
	{
		mColorRangeGroup = CreateGroupBox( ELayoutType::Vertical, 
		{
			LayoutWidgets( Qt::Horizontal, { new QLabel("min"), mColorRangeMinEdit, nullptr, new QLabel("max"), mColorRangeMaxEdit }, nullptr ,4,4,4,4,4 ),
			mCalculateMinMax,
		}
		,"Color Range", nullptr, 4,4,4,4,4 );


		for ( int i = 0; i < smNumberOfColorLabels; ++i )
		{
			mColorLabels[ i ] = wshrinkh();
		}

		labels_l = LayoutWidgets( Qt::Horizontal,
		{
			mColorLabels[ 0 ], nullptr, mColorLabels[ 1 ], nullptr, mColorLabels[ 2 ], nullptr, mColorLabels[ 3 ], nullptr, mColorLabels[ 4 ]
		}, nullptr, 0, 0, 0, 0, 0 );

		mColorMapLabel->setMinimumWidth( ( lwidth + 3 ) * 5 );
		//mColorMapLabel->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Maximum );
		//color_range_group->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Maximum );
	}

    std::vector<QObject*> v{ mColorTables, mColorMapLabel };
    if ( mShowSolidColor )
        v.insert( v.begin(), mShowSolidColor );
	if ( labels_l )
		v.push_back( labels_l );
	v.push_back( nullptr );
	mColorMapGroup = CreateGroupBox( ELayoutType::Vertical, v, "Color Table", nullptr, 1,2,0,2,2 );
	//color_map_l->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum );


	// assemble all

	if ( solid_and_contour_l )
		if ( mColorRangeGroup )
			LayoutWidgets( Qt::Horizontal, { solid_and_contour_l, mColorMapGroup, mColorRangeGroup }, this, 2,2,2,2,2 );
		else
			LayoutWidgets( Qt::Horizontal, { solid_and_contour_l, mColorMapGroup }, this, 2,2,2,2,2 );
	else
		if ( mColorRangeGroup )
			LayoutWidgets( Qt::Horizontal, { mColorMapGroup, mColorRangeGroup }, this, 2,2,2,2,2 );
		else
			LayoutWidgets( Qt::Horizontal, { mColorMapGroup }, this, 2,2,2,2,2 );
}



CColorMapWidget::CColorMapWidget( bool only_lut, bool show_range, QWidget *parent )
	: base_t( parent )
	, mOnlyLUT( only_lut )
	, mColorLabels( smNumberOfColorLabels )
{
	CreateWidgets( show_range );


	if ( !mOnlyLUT )
	{
		connect( mShowContourCheck, SIGNAL( toggled( bool ) ), this, SIGNAL( ShowContourToggled( bool ) ), Qt::QueuedConnection );

		connect( mShowSolidColor, SIGNAL( toggled( bool ) ), this, SIGNAL( ShowSolidColorToggled( bool ) ), Qt::QueuedConnection );
		connect( mNumberOfContoursEdit, SIGNAL( returnPressed() ), this, SIGNAL( ContoursEditReturnPressed() ), Qt::QueuedConnection );
		connect( mContourWidthEdit, SIGNAL( returnPressed() ), this, SIGNAL( ContourWidthReturnPressed() ), Qt::QueuedConnection );

		connect( mContourPrecisionGrid1Edit, SIGNAL( returnPressed() ), this, SIGNAL( ContourPrecisionGrid1EditPressed() ), Qt::QueuedConnection );
		connect( mContourPrecisionGrid2Edit, SIGNAL( returnPressed() ), this, SIGNAL( ContourPrecisionGrid2EditPressed() ), Qt::QueuedConnection );

		connect( mContourColorButton, SIGNAL( ColorChanged() ), this, SLOT( HandleContourColorSelected() ) );
	}

	connect( mCalculateMinMax, SIGNAL( clicked() ), this, SLOT( HandleCalculateMinMaxFromData() ) );
	connect( mColorRangeMinEdit, SIGNAL( returnPressed() ), this, SLOT( HandleColorRangeMinChanged( ) ) );
	connect( mColorRangeMaxEdit, SIGNAL( returnPressed() ), this, SLOT( HandleColorRangeMaxChanged( ) ) );
	connect( mColorRangeMinEdit, SIGNAL( editingFinished() ), this, SLOT( HandleColorRangeMinFinished( ) ) );
	connect( mColorRangeMaxEdit, SIGNAL( editingFinished() ), this, SLOT( HandleColorRangeMaxFinished( ) ) );
	connect( mColorTables, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleColorTablesIndexChanged( int ) ) );
}


void CColorMapWidget::ShowPrecisionParameters( bool show )
{
	mContourPrecisionLabel->setVisible( show );
	mContourPrecisionGrid1Edit->setVisible( show );
	mContourPrecisionGrid2Edit->setVisible( show );
}


void CColorMapWidget::EnableOnlySolidColor( bool enable_only )
{
	assert__( mShowSolidColor );

	if ( mColorRangeGroup )
		mColorRangeGroup->setEnabled( !enable_only );

	if ( mShowContourGroupBox )
		mShowContourGroupBox->setEnabled( !enable_only );

	if ( mColorMapGroup )
	{
		//enable / disable selectively

		for ( auto *label : mColorLabels )
			label->setEnabled( !enable_only );
		mColorMapLabel->setEnabled( !enable_only );
		mColorTables->setEnabled( !enable_only );
	}

	mShowSolidColor->setEnabled( true );
}



void CColorMapWidget::SetShowContour( bool checked )
{
	assert__( !mOnlyLUT );

	mShowContourCheck->setChecked( checked );		
}


unsigned CColorMapWidget::NumberOfContours() const
{
    bool ok = false;
    unsigned n = mNumberOfContoursEdit->text().toUInt( &ok );
	if ( !ok )
		n = 0;

	return n;
}


void CColorMapWidget::SetNumberOfContours( unsigned contours )
{
	mNumberOfContoursEdit->setText( n2s( contours ).c_str() );
}


double CColorMapWidget::ContoursWidth() const
{
    bool ok = false;
    double n = mContourWidthEdit->text().toDouble( &ok );
	if ( !ok )
		n = 0.;

	return n;
}

void CColorMapWidget::SetContoursWidth( double width )
{
	mContourWidthEdit->setText( n2s( width ).c_str() );
}


void CColorMapWidget::PrecisionParameters( unsigned int &grid1, unsigned int &grid2 )
{
    bool ok = false;
    grid1 = mContourPrecisionGrid1Edit->text().toUInt( &ok );
	if ( !ok )
		grid1 = 0.;

    grid2 = mContourPrecisionGrid2Edit->text().toUInt( &ok );
	if ( !ok )
		grid2 = 0.;
}


void CColorMapWidget::SetPrecisionParameters( unsigned int grid1, unsigned int grid2 )
{
	mContourPrecisionGrid1Edit->setText( n2q( grid1 ) );
	mContourPrecisionGrid2Edit->setText( n2q( grid2 ) );
}


void CColorMapWidget::SetContourColor( QColor color ) 
{ 
    mContourColor = color;

	mContourColorButton->SetColor( mContourColor );
}


void CColorMapWidget::HandleContourColorSelected()
{
    mContourColor = mContourColorButton->GetColor();

	emit ContourColorSelected();
}


void CColorMapWidget::HideContourColorWidget() 
{ 
	if ( mContourColorButton ) 
		mContourColorButton->setVisible( false ); 
}



void CColorMapWidget::SetShowSolidColor( bool checked )
{
	assert__( !mOnlyLUT );

	mShowSolidColor->setChecked( checked );	
}



void CColorMapWidget::SetLUT( CBratLookupTable *lut, double data_min, double data_max )
{
	assert__( data_min <= data_max );

	mLut = lut;
	mColorTables->blockSignals( true );		//we don't want client code to think that the user changed the color map
	mColorTables->clear();
	for ( auto ii = mLut->GetColorTableList()->cbegin(); ii != mLut->GetColorTableList()->cend(); ++ii )
		mColorTables->addItem( ii->c_str() );	
	mColorTables->blockSignals( false );

	mDataMin = data_min;
	mDataMax = data_max;
	if ( mLut->CurrentMinValue() == CBratLookupTable::smDefaultRangeValues || mLut->CurrentMaxValue() == CBratLookupTable::smDefaultRangeValues )
	{
		SetRange( mDataMin, mDataMax );		//assume internal range not initialized and initialize with given defaults (absolute values);
	}
	else
	{
		SetRange( mLut->CurrentMinValue(), mLut->CurrentMaxValue() );	//assume internal range initialized and use it
	}
}


void CColorMapWidget::HandleCalculateMinMaxFromData()
{
	SetRange( mDataMin, mDataMax );
}


void CColorMapWidget::HandleColorRangeMinFinished()
{
	QString smin = mColorRangeMinEdit->text();
	bool ok_conv = false;
	double rangeMin = smin.toDouble( &ok_conv );
	if ( !ok_conv || rangeMin != mColorRangeMin )
		mColorRangeMinEdit->setText( n2s< std::string >( mColorRangeMin ).c_str() );
}
void CColorMapWidget::HandleColorRangeMaxFinished()
{
	QString smax = mColorRangeMaxEdit->text();
	bool ok_conv = false;
	double rangeMax = smax.toDouble( &ok_conv );
	if ( !ok_conv || rangeMax != mColorRangeMax )
		mColorRangeMaxEdit->setText( n2s< std::string >( mColorRangeMax ).c_str() );
}


void CColorMapWidget::HandleColorRangeMinChanged()
{
	QString smin = mColorRangeMinEdit->text();
	bool ok_conv = false;
	double rangeMin = smin.toDouble( &ok_conv );
	if ( !ok_conv || rangeMin > mColorRangeMax )
	{
		SimpleErrorBox( "Invalid minimum range value." );
		mColorRangeMinEdit->setFocus();
		return;
	}
	SetRange( rangeMin, mColorRangeMax );
}
void CColorMapWidget::HandleColorRangeMaxChanged()
{
	QString smax = mColorRangeMaxEdit->text();
	bool ok_conv = false;
	double rangeMax = smax.toDouble( &ok_conv );
	if ( !ok_conv || rangeMax < mColorRangeMin )
	{
		SimpleErrorBox( "Invalid maximum range value." );
		mColorRangeMaxEdit->setFocus();
		return;
	}
	SetRange( mColorRangeMin, rangeMax );
}


void CColorMapWidget::UpdateLabels()
{
	if ( mColorLabels[ 0 ] )
	{
		double range = mColorRangeMax - mColorRangeMin;
		double step = range / ( smNumberOfColorLabels - 1 );
		for ( int i = 0; i < smNumberOfColorLabels; ++i )
		{
			double value = mColorRangeMin + i * step;
			//for very large steps, "mColorRangeMin + i * step" can loose precision and be only very
			//near but not equal to mColorRangeMax; but we want to show the exact upper limit used
			if ( i == smNumberOfColorLabels - 1 )
				value = mColorRangeMax;
			mColorLabels[ i ]->setText( QString::number( value, 'f', 2 ) );		//(*)
			mColorLabels[ i ]->setToolTip( n2s( value ).c_str() );
		}
	}
}


void CColorMapWidget::SetRange( double m, double M )
{
	assert__( m <= M );

	mColorRangeMin = std::min( M, m );
	mColorRangeMax = M;

	mLut->SetTableRange( mColorRangeMin, mColorRangeMax );

	mColorRangeMinEdit->setText( n2s( mColorRangeMin ).c_str() );
	mColorRangeMaxEdit->setText( n2s( mColorRangeMax ).c_str() );

	UpdateLabels();

	int index = mColorTables->findText( mLut->GetCurrentFunction().c_str() );
	if ( index == mColorTables->currentIndex() )
		HandleColorTablesIndexChanged( index );
	else
		mColorTables->setCurrentIndex( index );
}


void CColorMapWidget::HandleColorTablesIndexChanged( int index )
{
	auto name = q2a( itemText( index ) );
	if ( mLut )
		mLut->ExecMethod( name );

	mColorMapLabel->SetLUT( mLut );
	emit CurrentIndexChanged( index );
}





///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ColorMapWidget.cpp"
