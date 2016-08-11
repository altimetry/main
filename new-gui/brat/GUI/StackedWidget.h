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
