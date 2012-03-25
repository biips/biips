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
  class Histogram;
  class DiscreteHistogram;

  class Plot
  {
  protected:
    QApplication qApp_;
    QwtPlot qwtPlot_;
    Types<Types<QwtPlotCurve>::Ptr>::Array curves_;
    QwtLegend legend_;

    void addCurveCore(const QwtArray<Scalar> & x_data, const QwtArray<Scalar> & y_data, String name, const QColor & color, uint lineWidth, Qt::PenStyle style, uint symbolSize, QwtSymbol::Style symbol);

    void addHistogramCore(const QwtArray<Scalar> & x_data, const QwtArray<Scalar> & y_data, String name, const QColor & color = Qt::blue);

  public:
    Plot(int argc, char* argv[]);
    virtual ~Plot() {};

    void AddCurve(const Types<Scalar>::Array & x, const Types<Scalar>::Array & y, String name, const QColor & color = Qt::blue, uint width = 1, Qt::PenStyle style = Qt::SolidLine, uint symbolSize = 5, QwtSymbol::Style symbol = QwtSymbol::NoSymbol);

    void AddCurve(const MultiArray::Array & x, const MultiArray::Array & y, String name, const QColor & color = Qt::blue, uint width = 1, Qt::PenStyle style = Qt::SolidLine, uint symbolSize = 5, QwtSymbol::Style symbol = QwtSymbol::NoSymbol);

    void AddHistogram(const Histogram & hist, String name, const QColor & color);

    void AddHistogram(const Types<Scalar>::Array & x, const Types<Scalar>::Array & y, String name, const QColor & color = Qt::blue);

    void AddHistogram(const MultiArray::Array & x, const MultiArray::Array & y, String name, const QColor & color = Qt::blue);

    void SetTitle(const String & title);

    void SetAxesLabels(const String & xlabel, const String & ylabel);

    void SetBackgroundColor(const QColor & c);

    void SetLegend(QwtPlot::LegendPosition legendPos = QwtPlot::RightLegend);

    void Show();

    void PrintPdf(const String & filename);

  };

}

#endif /* BIIPS_PLOT_HPP_ */
