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
#if !defined GUI_PROGRESS_DIALOG_H
#define GUI_PROGRESS_DIALOG_H

#include <QProgressDialog>

#include "common/ProgressInterface.h"



class CProgressDialog : public QProgressDialog, public CProgressInterface
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif


	/////////////////////////////
	//	Types
	/////////////////////////////

	using base_t = QProgressDialog;
	using progress_base_t = CProgressInterface;


	/////////////////////////////
	//	Instance Data
	/////////////////////////////

public:

	/////////////////////////////
	//	Construction /Destruction
	/////////////////////////////

private:

	QWidget* PreConstruction( QWidget *parent );
	void PostConstruction( int minimum );
	void Wire();
public:
    explicit CProgressDialog( QWidget *parent = nullptr, Qt::WindowFlags flags = 0 );

    CProgressDialog( const QString &labelText, const QString &cancelButtonText, int minimum, int maximum, QWidget *parent = 0, Qt::WindowFlags flags = 0 );

    virtual ~CProgressDialog();


	/////////////////////////////
	//		Remaining
	/////////////////////////////

	virtual void Abort() override	//internal cancel
	{
		close();
	}

	virtual void SetLabel( const std::string &label ) override
	{
		progress_base_t::SetLabel( label );		//does nothing, so far
		setLabelText( label.c_str() );
	}

	virtual void SetRange( int m, int M ) override
	{
		progress_base_t::SetRange( m, M );
		setRange( m, M );
		setValue( progress_base_t::CurrentValue() );
	}

	virtual bool Cancelled() const override
	{
		return wasCanceled();
	}

	virtual int CurrentValue() const override
	{
        assert__( Cancelled() || value() == progress_base_t::CurrentValue() );

		return value();
	}

	virtual bool SetCurrentValue( int current ) override
	{
		if ( progress_base_t::SetCurrentValue( current ) )
			setValue( current );
		else
			return false;
		return true;
	}


protected:

	virtual void accept() override;

protected slots:
};


#endif	//GUI_PROGRESS_DIALOG_H
