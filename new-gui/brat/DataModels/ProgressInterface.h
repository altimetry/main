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
