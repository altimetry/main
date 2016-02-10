#ifndef DATAMODELS_PLOTDATA_PLOT_VALUES_H
#define DATAMODELS_PLOTDATA_PLOT_VALUES_H


struct C3DPlotValues
{
	std::vector<double>	mValues;
	std::vector<bool>	mBits;
	std::vector<double>	mXaxis;
	std::vector<double>	mYaxis;
};


struct C3DPlotInfo : public std::vector<C3DPlotValues>
{
	using base_t = std::vector<C3DPlotValues>;

	//

	C3DPlotInfo()
	{}
	virtual ~C3DPlotInfo()
	{}

	//

	inline void AddMap()
	{
		push_back(C3DPlotValues());
	}

	inline void AddValue( double value )
	{
		back().mValues.push_back( value );
	}
	void AddBit( bool bit )
	{
		back().mBits.push_back( bit );
	}
	inline void AddX( double value )
	{
		back().mXaxis.push_back( value );
	}
	inline void AddY( double value )
	{
		back().mYaxis.push_back( value );
	}
};




#endif			// DATAMODELS_PLOTDATA_PLOT_VALUES_H
