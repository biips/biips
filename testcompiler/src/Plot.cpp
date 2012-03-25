//                                               -*- C++ -*-
/*! \file Plot.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "config.hpp"
#ifdef USE_Qwt5_Qt4

#include "Plot.hpp"
#include "common/Accumulator.hpp"

namespace Biips
{

  void Plot::addCurveCore(const Scalar * x_data, const Scalar * y_data, Size lenght,
      const String & name, const QColor & color, uint lineWidth, Qt::PenStyle style,
      uint symbolSize, QwtSymbol::Style symbol, Bool antialiased, Bool smoothLine)
  {
    // add curve
    curves_.push_back(Types<QwtPlotCurve>::Ptr(new QwtPlotCurve(name.c_str())));
    // copy the data into the curve
    curves_.back()->setData(x_data, y_data, lenght);
    curves_.back()->attach(&qwtPlot_);
    // Set the style of the curves
    curves_.back()->setPen(QPen(color, lineWidth, style));
    curves_.back()->setRenderHint(QwtPlotItem::RenderAntialiased, antialiased);
    if (smoothLine)
      curves_.back()->setCurveAttribute(QwtPlotCurve::Fitted);
    curves_.back()->setSymbol(QwtSymbol(symbol, QBrush(color), QPen(color, lineWidth, Qt::SolidLine), QSize(symbolSize, symbolSize)));
    // finally, refresh the plot
    qwtPlot_.replot();
  }


  void Plot::AddCurve(const Types<Scalar>::Array & x, const Types<Scalar>::Array & y,
      const String & name, const QColor & color, uint lineWidth, Qt::PenStyle style,
      uint symbolSize, QwtSymbol::Style symbol, Bool antialiased, Bool smoothLine)
  {
    addCurveCore(x.data(), y.data(), x.size(), name, color, lineWidth, style, symbolSize, symbol, antialiased, smoothLine);
  }


  void Plot::AddCurve(const MultiArray::Array & x, const MultiArray::Array & y,
      const String & name, const QColor & color, uint width, Qt::PenStyle style,
      uint symbolSize, QwtSymbol::Style symbol, Bool antialiased, Bool smoothLine)
  {
    Types<Scalar>::Array x_data(x.size());
    Types<Scalar>::Array y_data(y.size());
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
    AddCurve(x_data, y_data, name, color, width, style, symbolSize, symbol, antialiased, smoothLine);
  }


  void Plot::addHistogramCore(const Scalar * x_data, const Scalar * y_data, Size length, const String & name, const QColor & color, Bool antialiased)
  {
    // add curve
    curves_.push_back(Types<QwtPlotCurve>::Ptr(new QwtPlotCurve(name.c_str())));
    // copy the data into the curve
    curves_.back()->setData(x_data, y_data, length);
    curves_.back()->attach(&qwtPlot_);
    // Set the style of the curves
    curves_.back()->setPen(QPen(color, 1, Qt::SolidLine));
    curves_.back()->setStyle(QwtPlotCurve::Steps);
    curves_.back()->setCurveAttribute(QwtPlotCurve::Inverted);
    curves_.back()->setBrush(color);
    curves_.back()->setRenderHint(QwtPlotItem::RenderAntialiased, antialiased);

    // draw a baseline
    ValArray x_base(2);
    ValArray y_base(2, 0.0);
    x_base[0] = x_data[0];
    x_base[1] = x_data[length-1];
    curves_.push_back(Types<QwtPlotCurve>::Ptr(new QwtPlotCurve()));
    // copy the data into the curve
    curves_.back()->setData(x_base.data(), y_base.data(), x_base.size());
    curves_.back()->attach(&qwtPlot_);
    // Set the style of the curves
    curves_.back()->setPen(QPen(color, 1, Qt::SolidLine));
    // finally, refresh the plot
    qwtPlot_.replot();
  }


  void Plot::AddHistogram(const Types<Scalar>::Array & x, const Types<Scalar>::Array & y, const String & name, const QColor & color, Bool antialiased)
  {
    // add curve
    addHistogramCore(x.data(), y.data(), x.size(), name, color, antialiased);
  }


  void Plot::AddHistogram(const Histogram & hist, const String & name, const QColor & color, Bool antialiased)
  {
    Types<Scalar>::Array x = hist.GetPositions();
    Types<Scalar>::Array y = hist.GetFrequencies();
    Scalar bin_size = x.back() - x[x.size()-2];
    Scalar last_pos = x.back() + bin_size;
    x.push_back(last_pos);
    y.push_back(0.0);
    AddHistogram(x, y, name, color, antialiased);
  }


  void Plot::AddHistogram(const MultiArray::Array & x, const MultiArray::Array & y, const String & name, const QColor & color, Bool antialiased)
  {
    Types<Scalar>::Array x_data(x.size());
    Types<Scalar>::Array y_data(y.size());
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
    AddHistogram(x_data, y_data, name, color, antialiased);
  }


  void Plot::SetTitle(const String & title, Size size, Size weight)
  {
    qwtPlot_.setTitle(title.c_str());
    QwtText qwt_title = qwtPlot_.title();
    QFont qfont;
    qfont.setPointSize(size);
    qfont.setWeight(weight);
    qwt_title.setFont(qfont);
    qwtPlot_.setTitle(qwt_title);
  }

  void Plot::SetAxesLabels(const String & xlabel, const String & ylabel, Size size, Size weight)
  {
    QFont qfont;
    qfont.setPointSize(size);
    qfont.setWeight(weight);

    qwtPlot_.setAxisTitle(QwtPlot::xBottom, xlabel.c_str());
    QwtText qwt_title = qwtPlot_.axisTitle(QwtPlot::xBottom);
    qwt_title.setFont(qfont);
    qwtPlot_.setAxisTitle(QwtPlot::xBottom, qwt_title);

    qwtPlot_.setAxisTitle(QwtPlot::yLeft, ylabel.c_str());
    qwt_title = qwtPlot_.axisTitle(QwtPlot::yLeft);
    qwt_title.setFont(qfont);
    qwtPlot_.setAxisTitle(QwtPlot::yLeft, qwt_title);
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

#endif //USE_Qwt5_Qt4
