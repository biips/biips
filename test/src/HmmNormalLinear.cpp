//                                               -*- C++ -*-
/*
 * Biips software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * Biips is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of Biips.
 *
 * Biips is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file HmmNormalLinear.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "config.hpp"
#include "HmmNormalLinear.hpp"
#include "BiipsCore.hpp"

#include "samplers/ConjugateNormal.hpp"
#include "samplers/ConjugateNormalVar.hpp"

#include "KalmanFilter.hpp"
#include "TestIO.hpp"

#ifdef USE_Qwt5_Qt4
#include "Plot.hpp"
#endif //USE_Qwt5_Qt4

namespace Biips
{

  const String HmmNormalLinear::NAME_ = "HMM Normal Linear 1D";

  HmmNormalLinear::HmmNormalLinear(int argc,
                                   char** argv,
                                   Size verbose,
                                   Size showMode,
                                   Bool precFlag,
                                   std::ostream & os) :
    BaseType(argc, argv, NAME_, verbose, showMode, os), precFlag_(precFlag)
  {
    // Default model parameters
    sizeParamMap_["t.max"] = 20;

    scalarParamMap_["mean.x.0"] = 0.0;
    scalarParamMap_["var.x.0"] = 1.0;
    scalarParamMap_["var.x"] = 1.0;
    scalarParamMap_["var.y"] = 0.5;

    DimArray::Ptr scalar_dim(new DimArray(1, 1));

    inDataVarNames_.push_back("x");
    dimArrayMap_["x"] = scalar_dim;

    inDataVarNames_.push_back("y");
    dimArrayMap_["y"] = scalar_dim;

    obsVarNames_.push_back("y");

    inBenchFilterVarNames_.push_back("x");

    printResultsVarNames_.push_back("x");

    if (precFlag)
      nodeSamplerFactoryInvOrder_.push_back(ConjugateNormalFactory::Instance());
    else
      nodeSamplerFactoryInvOrder_.push_back(ConjugateNormalVarFactory::Instance());
  }

  void HmmNormalLinear::PrintIntro()
  {
    using namespace std;
    os_ << "Biips test: Hidden Markov Model linear gaussian 1D" << endl;
    os_ << "comparison between Kalman Filter and Particle Filter" << endl;
    os_ << "====================================================" << endl;
    os_ << "x[0] --> x[1] --> ... --> x[t-1] --> x[t] --> ..." << endl;
    os_ << "          |                 |         |" << endl;
    os_ << "          v                 v         v" << endl;
    os_ << "         y[1]             y[t-1]     y[t]" << endl;
    os_ << endl;
    os_ << "         x[0] ~ Normal(mean.x.0, var.x.0)" << endl;
    os_ << "x[t] | x[t-1] ~ Normal(x[t-1], var.x) for all t>0" << endl;
    os_ << "  y[t] | x[t] ~ Normal(x[t], var.y)  for all t>0" << endl;
    os_ << endl;
  }

  //  void Hmm1D::InputModelParam(std::istream & is)
  //  {
  //    using namespace std;
  //
  //    if (verbose_)
  //      os_ << "Final time: ";
  //    is >> sizeParamMap_["t.max"];
  //    if (verbose_)
  //      os_ << endl << "t_max = " << sizeParamMap_["t.max"] << endl << endl;
  //  }


  void HmmNormalLinear::RunBench()
  {
    Size t_max = sizeParamMap_["t.max"];
    Scalar mean_x0_val = scalarParamMap_["mean.x.0"];
    Scalar var_x0_val = scalarParamMap_["var.x.0"];
    Scalar var_x_val = scalarParamMap_["var.x"];
    Scalar var_y_val = scalarParamMap_["var.y"];

    const MultiArray::Array & y_obs = dataValuesMap_.at("y");

    // kalman filter
    //--------------
    KalmanFilter kalman_filter(mean_x0_val, var_x0_val, var_y_val);
    kalman_filter.SetEvolutionModel(1.0, 0.0, var_x_val);
    kalman_filter.SetObservationModel(1.0, var_y_val);

    benchFilterValuesMap_["x"] = MultiArray::Array(t_max + 1);
    benchFilterValuesMap_["var.x"] = MultiArray::Array(t_max + 1);
    MultiArray::Array & x_est_KF = benchFilterValuesMap_["x"];
    MultiArray::Array & x_var_KF = benchFilterValuesMap_["var.x"];

    x_est_KF[0] = MultiArray(P_SCALAR_DIM,
                             ValArray::Ptr(new ValArray(1, mean_x0_val)));
    x_var_KF[0] = MultiArray(P_SCALAR_DIM,
                             ValArray::Ptr(new ValArray(1, var_x0_val)));

    for (Size t = 1; t < t_max + 1; ++t)
    {
      kalman_filter.Update(y_obs[t - 1]);
      x_est_KF[t] = kalman_filter.GetPosteriorEstimate();
      x_var_KF[t] = kalman_filter.GetPosteriorCovariance();
    }

    if (verbose_ >= 2)
    {
      os_ << "Kalman Filter estimate E[x(t|0:t)]:" << std::endl;
      printValues(os_, "x", x_est_KF);
      os_ << std::endl;
    }
  }

  void HmmNormalLinear::BuildModelGraph()
  {
    // load Base module
    //-----------------
    loadBase();

    // graph
    //------
    pModelGraph_ = Graph::Ptr(new Graph());

    // create constant nodes
    //----------------------
    Scalar mean_x0_val = scalarParamMap_["mean.x.0"];
    Scalar var_x0_val = scalarParamMap_["var.x.0"];
    Scalar var_x_val = scalarParamMap_["var.x"];
    Scalar var_y_val = scalarParamMap_["var.y"];

    NodeId
        mean_x0 =
            pModelGraph_->AddConstantNode(MultiArray(P_SCALAR_DIM,
                                                     ValArray::Ptr(new ValArray(1,
                                                                                mean_x0_val))));

    NodeId prec_or_var_x0;
    NodeId prec_or_var_x;
    NodeId prec_or_var_y;

    if (precFlag_)
    {
      prec_or_var_x0
          = pModelGraph_->AddConstantNode(MultiArray(P_SCALAR_DIM,
                                                     ValArray::Ptr(new ValArray(1,
                                                                                1.0
                                                                                    / var_x0_val))));
      prec_or_var_x
          = pModelGraph_->AddConstantNode(MultiArray(P_SCALAR_DIM,
                                                     ValArray::Ptr(new ValArray(1,
                                                                                1.0
                                                                                    / var_x_val))));
      prec_or_var_y
          = pModelGraph_->AddConstantNode(MultiArray(P_SCALAR_DIM,
                                                     ValArray::Ptr(new ValArray(1,
                                                                                1.0
                                                                                    / var_y_val))));
    }
    else
    {
      prec_or_var_x0
          = pModelGraph_->AddConstantNode(MultiArray(P_SCALAR_DIM,
                                                     ValArray::Ptr(new ValArray(1,
                                                                                var_x0_val))));
      prec_or_var_x
          = pModelGraph_->AddConstantNode(MultiArray(P_SCALAR_DIM,
                                                     ValArray::Ptr(new ValArray(1,
                                                                                var_x_val))));
      prec_or_var_y
          = pModelGraph_->AddConstantNode(MultiArray(P_SCALAR_DIM,
                                                     ValArray::Ptr(new ValArray(1,
                                                                                var_y_val))));
    }

    Size t_max = sizeParamMap_["t.max"];

    // create Stochastic nodeId collections
    //-----------------------------------
    modelNodeIdMap_["x"] = Types<NodeId>::Array(t_max + 1);
    modelNodeIdMap_["y"] = Types<NodeId>::Array(t_max);

    Types<NodeId>::Array & x = modelNodeIdMap_["x"];
    Types<NodeId>::Array & y = modelNodeIdMap_["y"];

    // create Nodes
    //-------------
    Types<NodeId>::Array params(2);

    if (precFlag_)
    {
      params[0] = mean_x0;
      params[1] = prec_or_var_x0;
      x[0] = pModelGraph_->AddStochasticNode(distTab_["dnorm"], params, false);

      for (Size t = 1; t < t_max + 1; ++t)
      {
        params[0] = x[t - 1];
        params[1] = prec_or_var_x;
        x[t]
            = pModelGraph_->AddStochasticNode(distTab_["dnorm"], params, false);

        params[0] = x[t];
        params[1] = prec_or_var_y;
        y[t - 1] = pModelGraph_->AddStochasticNode(distTab_["dnorm"],
                                                   params,
                                                   true);
      }
    }
    else
    {
      params[0] = mean_x0;
      params[1] = prec_or_var_x0;
      x[0] = pModelGraph_->AddStochasticNode(distTab_["dnormvar"],
                                             params,
                                             false);

      for (Size t = 1; t < t_max + 1; ++t)
      {
        params[0] = x[t - 1];
        params[1] = prec_or_var_x;
        x[t] = pModelGraph_->AddStochasticNode(distTab_["dnormvar"],
                                               params,
                                               false);

        params[0] = x[t];
        params[1] = prec_or_var_y;
        y[t - 1] = pModelGraph_->AddStochasticNode(distTab_["dnormvar"],
                                                   params,
                                                   true);
      }
    }

    // build graph
    //------------
    if (verbose_ >= 2)
      os_ << "The graph has a cycle? " << pModelGraph_->HasCycle() << std::endl;

    pModelGraph_->Build();

    setObsValues();
  }

  void HmmNormalLinear::initAccumulators(Size nParticles,
                                         Size numBins,
                                         std::map<String, MultiArray::Array> & statsValuesMap)
  {
    scalarAcc_.AddFeature(MEAN);
    scalarAcc_.AddFeature(VARIANCE);
    densAcc_ = DensityAccumulator(floor(nParticles * 0.25), numBins);
    Scalar probs[] =
      { 0.05, 0.95 };
    quantAcc_ = QuantileAccumulator(probs, probs + sizeof(probs) / sizeof(probs[0]));

    Size t_max = sizeParamMap_["t.max"];

    statsValuesMap["x"] = MultiArray::Array(t_max + 1);
    statsValuesMap["x.var"] = MultiArray::Array(t_max + 1);
    statsValuesMap["x.q05"] = MultiArray::Array(t_max + 1);
    statsValuesMap["x.q95"] = MultiArray::Array(t_max + 1);
  }

  void HmmNormalLinear::initFilterAccumulators(Size nParticles, Size numBins)
  {
    initAccumulators(nParticles, numBins, smcFilterValuesMap_);
  }

  void HmmNormalLinear::accumulate(Size t,
                                   std::map<String, MultiArray::Array> & statsValuesMap,
                                   const String & title)
  {
    Types<NodeId>::Array & x = modelNodeIdMap_["x"];

    MultiArray::Array & x_est = statsValuesMap["x"];
    MultiArray::Array & x_var = statsValuesMap["x.var"];
    MultiArray::Array & x_quant_05 = statsValuesMap["x.q05"];
    MultiArray::Array & x_quant_95 = statsValuesMap["x.q95"];

    pSampler_->Accumulate(x[t], scalarAcc_);
    pSampler_->Accumulate(x[t], quantAcc_);
    x_est[t].SetPtr(P_SCALAR_DIM,
                    ValArray::Ptr(new ValArray(1, scalarAcc_.Mean())));
    x_var[t].SetPtr(P_SCALAR_DIM,
                    ValArray::Ptr(new ValArray(1, scalarAcc_.Variance())));
    x_quant_05[t].SetPtr(P_SCALAR_DIM,
                         ValArray::Ptr(new ValArray(1, quantAcc_.Quantile(0U))));
    x_quant_95[t].SetPtr(P_SCALAR_DIM,
                         ValArray::Ptr(new ValArray(1, quantAcc_.Quantile(1U))));

#ifdef USE_Qwt5_Qt4
    pSampler_->Accumulate(x[t], densAcc_);
    if (showMode_ >= 2)
    {
      Histogram pdf_hist = densAcc_.Density();
      Plot pdf_plot_PF(argc_, argv_);
      pdf_plot_PF.AddHistogram(pdf_hist, "", Qt::blue);
      pdf_plot_PF.SetTitle(title + " x pdf estimates, t = " + toString(t));
      pdf_plot_PF.SetBackgroundColor(Qt::white);
      pdf_plot_PF.Show();
    }
#endif //USE_Qwt5_Qt4
  }

  void HmmNormalLinear::filterAccumulate(Size t)
  {
    accumulate(t, smcFilterValuesMap_, "Filtering");
  }

  void HmmNormalLinear::initSmoothAccumulators(Size nParticles, Size numBins)
  {
    initAccumulators(nParticles, numBins, smcSmoothValuesMap_);
  }

  void HmmNormalLinear::smoothAccumulate(Size t)
  {
    accumulate(t, smcSmoothValuesMap_, "Smoothing");
  }

  void HmmNormalLinear::PlotResults(const String & plotFileName) const
  {
#ifdef USE_Qwt5_Qt4
    const MultiArray::Array & x_gen = dataValuesMap_.at("x");
    const MultiArray::Array & y_obs = dataValuesMap_.at("y");
    const MultiArray::Array & x_est_PF = smcFilterValuesMap_.at("x");
    const MultiArray::Array & x_quant_05_PF = smcFilterValuesMap_.at("x.q05");
    const MultiArray::Array & x_quant_95_PF = smcFilterValuesMap_.at("x.q95");
    const MultiArray::Array & x_est_PS = smcSmoothValuesMap_.at("x");

    Size t_max = sizeParamMap_.at("t.max");
    MultiArray::Array time_x(t_max + 1);
    MultiArray::Array time_y(t_max);
    time_x[0] = MultiArray(P_SCALAR_DIM, ValArray::Ptr(new ValArray(1, 0.0)));
    for (Size t = 1; t < t_max + 1; ++t)
    {
      time_x[t] = MultiArray(P_SCALAR_DIM,
                             ValArray::Ptr(new ValArray(1, Scalar(t))));
      time_y[t - 1] = MultiArray(P_SCALAR_DIM,
                                 ValArray::Ptr(new ValArray(1, Scalar(t))));
    }

    Plot results_plot(argc_, argv_);
    results_plot.AddCurve(time_x,
                          x_gen,
                          "hidden state",
                          Qt::black,
                          2,
                          Qt::NoPen,
                          9,
                          QwtSymbol::Cross);
    results_plot.AddCurve(time_y,
                          y_obs,
                          "observation",
                          Qt::darkGray,
                          2,
                          Qt::NoPen,
                          8,
                          QwtSymbol::XCross);
    if (benchFilterValuesMap_.count("x"))
      results_plot.AddCurve(time_x,
                            benchFilterValuesMap_.at("x"),
                            "KF estimate",
                            Qt::green,
                            2);
    results_plot.AddCurve(time_x, x_est_PF, "PF estimate", Qt::blue, 2);
    results_plot.AddCurve(time_x,
                          x_quant_05_PF,
                          "PF 5% quantile",
                          Qt::blue,
                          0.5,
                          Qt::DashLine);
    results_plot.AddCurve(time_x,
                          x_quant_95_PF,
                          "PF 95% quantile",
                          Qt::blue,
                          0.5,
                          Qt::DashLine);
    if (benchSmoothValuesMap_.count("x"))
      results_plot.AddCurve(time_x,
                            benchSmoothValuesMap_.at("x"),
                            "KS estimate",
                            Qt::magenta,
                            2);
    results_plot.AddCurve(time_x, x_est_PS, "PS estimate", Qt::cyan, 2);
    results_plot.SetTitle("");
    results_plot.SetAxesLabels("time", "state");
    results_plot.SetBackgroundColor(Qt::white);
    results_plot.SetLegend(QwtPlot::RightLegend);

    if (!plotFileName.empty())
      results_plot.PrintPdf(plotFileName);

    if (showMode_ >= 1)
      results_plot.Show();
#endif //USE_Qwt5_Qt4
  }

}
