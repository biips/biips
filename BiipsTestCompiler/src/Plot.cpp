//                                               -*- C++ -*-
/*! \file Plot.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "Plot.hpp"
#include "sampler/Accumulator.hpp"

namespace Biips
{

  void Plot::addCurveCore(const QwtArray<Scalar> & x_data, const QwtArray<Scalar> & y_data, String name, const QColor & color, uint lineWidth, Qt::PenStyle style, uint symbolSize, QwtSymbol::Style symbol)
  {
    // add curve
    curves_.push_back(Types<QwtPlotCurve>::Ptr(new QwtPlotCurve(name.c_str())));
    // copy the data into the curve
    curves_.back()->setData(x_data, y_data);
    curves_.back()->attach(&qwtPlot_);
    // Set the style of the curves
    curves_.back()->setPen(QPen(color, lineWidth, style));
    curves_.back()->setSymbol(QwtSymbol(symbol, QBrush(color), QPen(color, lineWidth, Qt::SolidLine), QSize(symbolSize, symbolSize)));
    // finally, refresh the plot
    qwtPlot_.replot();
  }


  void Plot::addHistogramCore(const QwtArray<Scalar> & x_data, const QwtArray<Scalar> & y_data, String name, const QColor & color)
  {
    // add curve
    curves_.push_back(Types<QwtPlotCurve>::Ptr(new QwtPlotCurve(name.c_str())));
    // copy the data into the curve
    curves_.back()->setData(x_data, y_data);
    curves_.back()->attach(&qwtPlot_);
    // Set the style of the curves
    curves_.back()->setPen(QPen(color, 1, Qt::SolidLine));
    curves_.back()->setStyle(QwtPlotCurve::Steps);
    curves_.back()->setBrush(color);
    // finally, refresh the plot
    qwtPlot_.replot();
  }


  void Plot::AddCurve(const Types<Scalar>::Array & x, const Types<Scalar>::Array & y, String name, const QColor & color, uint width, Qt::PenStyle style, uint symbolSize, QwtSymbol::Style symbol)
  {
    QwtArray<Scalar> x_data(x.size());
    QwtArray<Scalar> y_data(y.size());
    Types<Scalar>::ConstIterator it_x = x.begin();
    Types<Scalar>::ConstIterator it_y = y.begin();
    Size i = 0;
    while( (it_x != x.end()) && (it_y != y.end()) )
    {
      x_data[i] = *it_x;
      y_data[i] = *it_y;
      ++it_x;
      ++it_y;
      ++i;
    }
    // add curve
    addCurveCore(x_data, y_data, name, color, width, style, symbolSize, symbol);
  }

  void Plot::AddCurve(const MultiArray::Array & x, const MultiArray::Array & y, String name, const QColor & color, uint width, Qt::PenStyle style, uint symbolSize, QwtSymbol::Style symbol)
  {
    QwtArray<Scalar> x_data(x.size());
    QwtArray<Scalar> y_data(y.size());
    Types<MultiArray>::ConstIterator it_x = x.begin();
    Types<MultiArray>::ConstIterator it_y = y.begin();
    Size i = 0;
    while( (it_x != x.end()) && (it_y != y.end()) )
    {
      x_data[i] = it_x->Values().ScalarView();
      y_data[i] = it_y->Values().ScalarView();
      ++it_x;
      ++it_y;
      ++i;
    }
    // add curve
    addCurveCore(x_data, y_data, name, color, width, style, symbolSize, symbol);
  }


  void Plot::AddHistogram(const ScalarHistogram & hist, String name, const QColor & color)
  {
    Size hist_size = hist.size();
    QwtArray<Scalar> x_data(hist.size()+1);
    QwtArray<Scalar> y_data(hist.size()+1);

    x_data[0] = hist[0].first;
    y_data[0] = 0.0;
    for (Size i=1; i<hist_size; ++i)
    {
      x_data[i] = hist[i].first;
      y_data[i] = hist[i-1].second;
    }
    Scalar bin_size;
    if ( hist_size > 1 )
      bin_size = hist.back().first - hist[hist_size-2].first;
    else
      bin_size = 1.0;
    x_data[hist_size] = hist.back().first + bin_size;
    y_data[hist_size] = hist.back().second;

    // add histogram
    addHistogramCore(x_data, y_data, name, color);
  }

  void Plot::AddHistogram(const Types<Scalar>::Array & x, const Types<Scalar>::Array & y, String name, const QColor & color)
  {
    QwtArray<Scalar> x_data(x.size());
    QwtArray<Scalar> y_data(y.size());
    Types<Scalar>::ConstIterator it_x = x.begin();
    Types<Scalar>::ConstIterator it_y = y.begin();
    Size i = 0;
    while( (it_x != x.end()) && (it_y != y.end()) )
    {
      x_data[i] = *it_x;
      y_data[i] = *it_y;
      ++it_x;
      ++it_y;
      ++i;
    }
    // add histogram
    addHistogramCore(x_data, y_data, name, color);
  }

  void Plot::AddHistogram(const MultiArray::Array & x, const MultiArray::Array & y, String name, const QColor & color)
  {
    QwtArray<Scalar> x_data(x.size());
    QwtArray<Scalar> y_data(y.size());
    Types<MultiArray>::ConstIterator it_x = x.begin();
    Types<MultiArray>::ConstIterator it_y = y.begin();
    Size i = 0;
    while( (it_x != x.end()) && (it_y != y.end()) )
    {
      x_data[i] = it_x->Values().ScalarView();
      y_data[i] = it_y->Values().ScalarView();
      ++it_x;
      ++it_y;
      ++i;
    }
    // add histogram
    addHistogramCore(x_data, y_data, name, color);
  }

  void Plot::SetTitle(const String & title)
  {
    qwtPlot_.setTitle(title.c_str());
  }

  void Plot::SetAxesLabels(const String & xlabel, const String & ylabel)
  {
    qwtPlot_.setAxisTitle(QwtPlot::xBottom, xlabel.c_str());
    qwtPlot_.setAxisTitle(QwtPlot::yLeft, ylabel.c_str());
  }

  void Plot::SetBackgroundColor(const QColor & c)
  {
    qwtPlot_.setCanvasBackground(c);
  }

  void Plot::SetLegend(QwtPlot::LegendPosition legendPos)
  {
    qwtPlot_.insertLegend(&legend_, legendPos);
  }

  void Plot::Show()
  {
    qApp_.setActiveWindow(&qwtPlot_);
    qwtPlot_.show();
    qApp_.exec();
  };

  void Plot::PrintPdf(const String & filename)
  {
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOrientation(QPrinter::Landscape);
    printer.setOutputFileName(filename.c_str());
    qwtPlot_.print(printer);
  }

  Plot::Plot(int argc, char* argv[])
  : qApp_(argc, argv)
  {
    qwtPlot_.setAutoReplot();
  };

}
