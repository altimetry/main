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
#include "stdafx.h"

#include "new-gui/Common/QtUtils.h"
#include "new-gui/Common/GUI/TextWidget.h"

#include "ViewMapsLayerDialog.h"



void CViewMapsLayerDialog::CreateWidgets( bool enable_uri )
{
	mViewsUseVectorLayer = new QRadioButton( "Use vector layer" );
	mViewsUseRasterLayer = new QRadioButton( "Use application local raster file" );
	mViewsUseRasterLayerURI = new QRadioButton( "Use application raster layer URI" );
	mViewsUseRasterLayerURI->setToolTip( "This option is valid only if a layer URI is defined in the application settings" );

	mViewsUseRasterLayerURI->setEnabled( enable_uri );		assert__( enable_uri || mLayerBaseType != CMapWidget::ELayerBaseType::eRasterURL );

    auto *views_layers_group = CreateGroupBox( ELayoutType::Vertical,
                                        {
                                            mViewsUseVectorLayer,
                                            mViewsUseRasterLayer,
											mViewsUseRasterLayerURI
                                        },
                                        "", this );

    // Help
    //
    auto help = new CTextWidget;
    help->SetHelpProperties(
                "Select view maps base layer type"
         ,0 , 6 );
    auto help_group = CreateGroupBox( ELayoutType::Grid, { help }, "", nullptr, 6, 6, 6, 6, 6 );
    help_group->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );


	// Dialog buttons

    mButtonBox = new QDialogButtonBox( this );
	mButtonBox->setObjectName( QString::fromUtf8( "mButtonBox" ) );
	mButtonBox->setOrientation( Qt::Horizontal );
    mButtonBox->setStandardButtons( QDialogButtonBox::Cancel| QDialogButtonBox::Ok );
    //mButtonBox->button( QDialogButtonBox::Ok )->setDefault( true );

	// Main layout

	LayoutWidgets( Qt::Vertical, { views_layers_group, help_group, mButtonBox }, this, 6, 6, 6, 6, 6 );

	Wire();
}


void CViewMapsLayerDialog::Wire()
{
	//StartupOptions Page

	mViewsUseVectorLayer->setChecked( mLayerBaseType == CMapWidget::ELayerBaseType::eVectorLayer );
	mViewsUseRasterLayer->setChecked( mLayerBaseType == CMapWidget::ELayerBaseType::eRasterLayer );
	mViewsUseRasterLayerURI->setChecked( mLayerBaseType == CMapWidget::ELayerBaseType::eRasterURL );


    //button box

    connect( mButtonBox, SIGNAL(accepted()), this, SLOT(accept()) );
    connect( mButtonBox, SIGNAL(rejected()), this, SLOT(reject()) );


    adjustSize();
    setMinimumWidth( width() );
    setMaximumHeight( height() );
}


CViewMapsLayerDialog::CViewMapsLayerDialog( ELayerBaseType type, bool enable_uri, QWidget *parent )
	: QDialog( parent )
	, mLayerBaseType( type )
{
	CreateWidgets( enable_uri );
	setWindowTitle( "Layer Type" );
}

//virtual 
CViewMapsLayerDialog::~CViewMapsLayerDialog()
{}



///////////////////////////////
//	Validation and Conclusion  
///////////////////////////////


bool CViewMapsLayerDialog::ValidateAndAssign()
{

    if ( mViewsUseVectorLayer->isChecked() )
		mLayerBaseType = ELayerBaseType::eVectorLayer;
    else
    if ( mViewsUseRasterLayer->isChecked() )
		mLayerBaseType = ELayerBaseType::eRasterLayer;
	else
    if ( mViewsUseRasterLayerURI->isChecked() )
		mLayerBaseType = ELayerBaseType::eRasterURL;
	else
	{
		assert__( false );
		return false;
	}

    return true;
}


void CViewMapsLayerDialog::accept()
{
    if ( ValidateAndAssign() )
        QDialog::accept();
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ViewMapsLayerDialog.cpp"
