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
#ifndef GUI_DISPLAY_WIDGETS_HISTOGRAM_H
#define GUI_DISPLAY_WIDGETS_HISTOGRAM_H

#include <qglobal.h>
#include <qcolor.h>

#include "qwt_plot_item.h" 

class QwtIntervalData;
class QString;



class CHistogram: public QwtPlotItem
{
	//This is only a trick, taken from curves, to preserve original plot zoom
	double mMinXValue = 0.;
	double mMaxXValue = 0.;
	double mMinYValue = 0.;
	double mMaxYValue = 0.;

	double mXScaleFactor = 1.;
	double mYScaleFactor = 1.;
	double mZScaleFactor = 1.;

	std::string mXtitle;
	std::string mYtitle;
	std::string mZtitle;

public:
    explicit CHistogram(const QString &title = QString::null);
    explicit CHistogram(const QwtText &title);
    virtual ~CHistogram();

    void setData(const QwtIntervalData &data);
    const QwtIntervalData &data() const;

	//This is only a trick, taken from curves, to preserve original plot zoom
	void Ranges( double &xMin, double &xMax, double &yMin, double &yMax ) const;
	void SetRanges( double xMin, double xMax, double yMin, double yMax );

	//idem for titles
	void AxisTitles( std::string &xtitle, std::string &ytitle, std::string &ztitle ) const;
	void SetAxisTitles( const std::string &xtitle, const std::string &ytitle, const std::string &ztitle );

	//idem for legends
    virtual void updateLegend( QwtLegend *) const override;

	//Added to preserve scale
	void ScaleFactors( double &x, double &y, double &y2 ) const;
	void SetScaleFactors( double x, double y, double y2 );

    void setColor(const QColor &);
    QColor color() const;

    virtual QwtDoubleRect boundingRect() const override;

    virtual int rtti() const override;

    virtual void draw(QPainter *, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const override;

    void setBaseline(double reference);
    double baseline() const;

    enum HistogramAttribute
    {
        Auto = 0,
        Xfy = 1
    };

    void setHistogramAttribute(HistogramAttribute, bool on = true);
    bool testHistogramAttribute(HistogramAttribute) const;

protected:
    virtual void drawBar(QPainter *, Qt::Orientation o, const QRect &) const;

private:
    void init();

    class PrivateData;
    PrivateData *d_data;
};



#endif		//GUI_DISPLAY_WIDGETS_HISTOGRAM_H

