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
#if !defined DATA_MODELS_PROGRESS_INTERFACE_H
#define DATA_MODELS_PROGRESS_INTERFACE_H


class CProgressInterface
{
	int mm = 0;
	int mM = 100;
	int mCurrent = 0;

public:
	CProgressInterface( int m = 0, int M = 100 )
		: mm( m )
		, mM( M )
		, mCurrent( m )
	{
		if ( m > M )
			mCurrent = m = M;
	}

	virtual void SetLabel( const std::string &label )
	{
		UNUSED( label );
	}

    virtual void SetRange( int m, int M ) = 0;

	virtual bool Cancelled() const = 0;

    virtual int CurrentValue() const = 0;

    virtual bool SetCurrentValue( int current ) = 0;
};



//virtual
inline void CProgressInterface::SetRange( int m, int M )
{
    if ( m > M )
        return;

    mm = m;
    mM = M;
}


//virtual
inline int CProgressInterface::CurrentValue() const
{
    return mCurrent;
}


//virtual
inline bool CProgressInterface::SetCurrentValue( int current )
{
    mCurrent = current;
    return !Cancelled();
}



#endif	//DATA_MODELS_PROGRESS_INTERFACE_H
