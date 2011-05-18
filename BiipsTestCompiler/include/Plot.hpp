//                                               -*- C++ -*-
/*! \file Plot.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_PLOT_HPP_
#define BIIPS_PLOT_HPP_

#include <qapplication.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_symbol.h>
#include <QPrinter>

#include "common/Common.hpp"

namespace Biips
{
  class ScalarHistogram;
  class DiscreteScalarHistogram;

  class Plot
  {
  public:
    typedef Plot SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    QApplication qApp_;
    QwtPlot qwtPlot_;
    Types<Types<QwtPlotCurve>::Ptr>::Array curves_;
    QwtLegend legend_;

    void addCurveCore(const Scalar * x_data, const Scalar * y_data, Size lenght,
        const String & name, const QColor & color, uint lineWidth, Qt::PenStyle style,
        uint symbolSize, QwtSymbol::Style symbol, Bool antialiased, Bool smoothLine);

    void addHistogramCore(const Scalar * x_data, const Scalar * y_data, Size length,
        const String & name, const QColor & color, Bool antialiased);

  public:
    Plot(int argc, char* argv[]);
    virtual ~Plot() {};

    void AddCurve(const Types<Scalar>::Array & x, const Types<Scalar>::Array & y,
        const String & name, const QColor & color = Qt::blue, uint width = 1, Qt::PenStyle style = Qt::SolidLine,
        uint symbolSize = 5, QwtSymbol::Style symbol = QwtSymbol::NoSymbol,
        Bool antialiased = true, Bool smoothLine = false);

    void AddCurve(const MultiArray::Array & x, const MultiArray::Array & y,
        const String & name, const QColor & color = Qt::blue, uint width = 1, Qt::PenStyle style = Qt::SolidLine,
        uint symbolSize = 5, QwtSymbol::Style symbol = QwtSymbol::NoSymbol,
        Bool antialiased = true, Bool smoothLine = false);

    void AddHistogram(const ScalarHistogram & hist, const String & name, const QColor & color, Bool antialiased = false);

    void AddHistogram(const Types<Scalar>::Array & x, const Types<Scalar>::Array & y,
        const String & name, const QColor & color = Qt::blue, Bool antialiased = false);

    void AddHistogram(const MultiArray::Array & x, const MultiArray::Array & y,
        const String & name, const QColor & color = Qt::blue, Bool antialiased = false);

    void SetTitle(const String & title, Size size = 10, Size weight = 50);

    void SetAxesLabels(const String & xlabel, const String & ylabel, Size size = 10, Size weight = 50);

    void SetBackgroundColor(const QColor & c);

    void SetLegend(QwtPlot::LegendPosition legendPos = QwtPlot::RightLegend);

    void Show();

    void PrintPdf(const String & filename);

  };

}

#endif /* BIIPS_PLOT_HPP_ */
