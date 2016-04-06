/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

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

public:
    explicit CHistogram(const QString &title = QString::null);
    explicit CHistogram(const QwtText &title);
    virtual ~CHistogram();

    void setData(const QwtIntervalData &data);
    const QwtIntervalData &data() const;

	//This is only a trick, taken from curves, to preserve original plot zoom
	void Ranges( double &xMin, double &xMax, double &yMin, double &yMax );
	void SetRanges( double xMin, double xMax, double yMin, double yMax );

    void setColor(const QColor &);
    QColor color() const;

    virtual QwtDoubleRect boundingRect() const;

    virtual int rtti() const;

    virtual void draw(QPainter *, const QwtScaleMap &xMap, 
        const QwtScaleMap &yMap, const QRect &) const;

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

