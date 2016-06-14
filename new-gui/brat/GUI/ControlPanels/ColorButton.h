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
#if !defined GUI_CONTROL_PANELS_COLOR_BUTTON_H
#define GUI_CONTROL_PANELS_COLOR_BUTTON_H

#include <QToolButton>


////////////////////////////////////////////////////////////////
//	 Color Button (Builds a button for selecting a color)
////////////////////////////////////////////////////////////////

class CColorButton : public QToolButton
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
    using base_t = QToolButton;

    // static members
    static const QString smStyleSheet_colorButton;
    static const QString smInitColor;

    // instance data members
    QColor mCurrentColor;
    QWidget *mColorDisplay = nullptr;


public:
    CColorButton( QWidget *parent = nullptr );

    virtual ~CColorButton()
    {}

    // access


    // operations

    QColor GetColor()
    {
        return mCurrentColor;
    }


signals:

	void ColorChanged();


public slots:

    void InputColor()
	{
		SetColor( QColorDialog::getColor( mCurrentColor, parentWidget() ) );
	}

    void SetColor( QColor color )
    {
		if ( !color.isValid() )
			return;

        mCurrentColor = color;
        mColorDisplay->setStyleSheet( smStyleSheet_colorButton + mCurrentColor.name() );
		emit ColorChanged();
    }
};



#endif	//GUI_CONTROL_PANELS_COLOR_BUTTON_H
