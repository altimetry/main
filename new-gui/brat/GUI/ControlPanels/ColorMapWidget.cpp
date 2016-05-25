#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtils.h"

#include "DataModels/PlotData/BratLookupTable.h"

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
	setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
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
	if ( mLut )
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




CColorMapWidget::CColorMapWidget( bool only_lut, QWidget *parent )
	: base_t( parent )
	, mOnlyLUT( only_lut )
{
	QLayout *contour_l = nullptr;
	if ( !mOnlyLUT )
	{
		mShowSolidColor = new QCheckBox( "Solid Color" );
		mShowContour = new QCheckBox( "Contour" );
		contour_l = LayoutWidgets( Qt::Vertical, { mShowSolidColor, mShowContour }, nullptr, 2, 2, 2, 2, 2 );
	}
	mColorTables = new QComboBox;
	mColorMapLabel = new CColorMapLabel;
	auto *color_map_l = LayoutWidgets( Qt::Vertical, { mColorTables, mColorMapLabel }, nullptr, 2,2,2,2,2 );

	if ( contour_l )
		LayoutWidgets( Qt::Horizontal, { contour_l, color_map_l }, this, 2,2,2,2,2 );
	else
		LayoutWidgets( Qt::Horizontal, { color_map_l }, this, 2,2,2,2,2 );


	if ( !mOnlyLUT )
	{
		connect( mShowContour, SIGNAL( toggled( bool ) ), this, SIGNAL( ShowContourToggled( bool ) ), Qt::QueuedConnection );
		connect( mShowSolidColor, SIGNAL( toggled( bool ) ), this, SIGNAL( ShowSolidColorToggled( bool ) ), Qt::QueuedConnection );
	}

	connect( mColorTables, SIGNAL( currentIndexChanged( int ) ), this, SLOT( HandleColorTablesIndexChanged( int ) ) );
}


void CColorMapWidget::SetShowContour( bool checked )
{
	assert__( !mOnlyLUT );

	mShowContour->setChecked( checked );		
}

void CColorMapWidget::SetShowSolidColor( bool checked )
{
	assert__( !mOnlyLUT );

	mShowSolidColor->setChecked( checked );	
}



void CColorMapWidget::SetLUT( CBratLookupTable *lut )
{
	mLut = lut;
	mColorTables->blockSignals( true );		//we don't want client code to think that the user changed the color map
	mColorTables->clear();
	for ( auto ii = mLut->GetColorTableList()->cbegin(); ii != mLut->GetColorTableList()->cend(); ++ii )
		mColorTables->addItem( ii->c_str() );	
	mColorTables->blockSignals( false );

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
