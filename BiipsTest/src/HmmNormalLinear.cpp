//                                               -*- C++ -*-
/*! \file HmmNormalLinear.cpp
* \brief
*
* $LastChangedBy$
* $LastChangedDate$
* $LastChangedRevision$
* $Id$
*/

#include "HmmNormalLinear.hpp"
#include "BiipsCore.hpp"
#include "BiipsBase.hpp"
#include "KalmanFilter.hpp"
#include "Plot.hpp"
#include "TestIO.hpp"

namespace Biips
{

  const String HmmNormalLinear::NAME_ = "HMM Normal Linear 1D";

  HmmNormalLinear::HmmNormalLinear(int argc, char** argv, Size verbose, Bool showFinal, Bool showAll, Bool precFlag, std::ostream & os)
  : BaseType(argc, argv, NAME_, verbose, showFinal, showAll, os), precFlag_(precFlag)
  {
    // Default model parameters
    sizeParamMap_["t.max"] = 20;

    scalarParamMap_["mean.x.0"] = 0.0;
    scalarParamMap_["var.x.0"] = 1.0;
    scalarParamMap_["var.x"] = 1.0;
    scalarParamMap_["var.y"] = 0.5;

    DimArray::Ptr scalar_dim(new DimArray(1,1));

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


  void HmmNormalLinear::PrintIntro() const
  {
    using namespace std;
    os_ << "BiiPS test: Hidden Markov Model linear gaussian 1D" << endl;
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

    const DataType::Array & y_obs = dataValuesMap_.at("y");

    // kalman filter
    //--------------
    KalmanFilter kalman_filter(mean_x0_val, var_x0_val, var_y_val);
    kalman_filter.SetEvolutionModel(1.0, 0.0, var_x_val);
    kalman_filter.SetObservationModel(1.0, var_y_val);

    benchFilterValuesMap_["x"].SetPtr(DataType::Array(t_max+1));
    benchFilterValuesMap_["var.x"].SetPtr(DataType::Array(t_max+1));
    DataType::Array & x_est_KF = benchFilterValuesMap_["x"];
    DataType::Array & x_var_KF = benchFilterValuesMap_["var.x"];

    x_est_KF[0] = DataType(mean_x0_val);
    x_var_KF[0] = DataType(var_x0_val);

    for (Size t = 1; t < t_max+1; ++t)
    {
      kalman_filter.Update(y_obs[t-1]);
      x_est_KF[t] = kalman_filter.GetPosteriorEstimate();
      x_var_KF[t] = kalman_filter.GetPosteriorCovariance();
    }

    if (verbose_>=2)
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
    loadBaseModule();

    // graph
    //------
    pModelGraph_ = Graph::Ptr(new Graph());

    // create constant nodes
    //----------------------
    Scalar mean_x0_val = scalarParamMap_["mean.x.0"];
    Scalar var_x0_val = scalarParamMap_["var.x.0"];
    Scalar var_x_val = scalarParamMap_["var.x"];
    Scalar var_y_val = scalarParamMap_["var.y"];

    NodeId mean_x0 = pModelGraph_->AddConstantNode(DataType(mean_x0_val));

    NodeId prec_or_var_x0;
    NodeId prec_or_var_x;
    NodeId prec_or_var_y;

    if (precFlag_)
    {
      prec_or_var_x0 = pModelGraph_->AddConstantNode(DataType(1.0 / var_x0_val));
      prec_or_var_x = pModelGraph_->AddConstantNode(DataType(1.0 / var_x_val));
      prec_or_var_y = pModelGraph_->AddConstantNode(DataType(1.0 / var_y_val));
    }
    else
    {
      prec_or_var_x0 = pModelGraph_->AddConstantNode(DataType(var_x0_val));
      prec_or_var_x = pModelGraph_->AddConstantNode(DataType(var_x_val));
      prec_or_var_y = pModelGraph_->AddConstantNode(DataType(var_y_val));
    }

    Size t_max = sizeParamMap_["t.max"];

    // create Stochastic nodeId collections
    //-----------------------------------
    modelNodeIdMap_["x"] = Types<NodeId>::Array(t_max+1);
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
      x[0] = pModelGraph_->AddStochasticNode(dimArrayMap_["x"], "dnorm", params);

      for (Size t=1; t<t_max+1; ++t)
      {
        params[0] = x[t-1];
        params[1] = prec_or_var_x;
        x[t] = pModelGraph_->AddStochasticNode(dimArrayMap_["x"], "dnorm", params);

        params[0] = x[t];
        params[1] = prec_or_var_y;
        y[t-1] = pModelGraph_->AddStochasticNode(dimArrayMap_["y"], "dnorm", params, true);
      }
    }
    else
    {
      params[0] = mean_x0;
      params[1] = prec_or_var_x0;
      x[0] = pModelGraph_->AddStochasticNode(dimArrayMap_["x"], "dnorm.var", params);

      for (Size t=1; t<t_max+1; ++t)
      {
        params[0] = x[t-1];
        params[1] = prec_or_var_x;
        x[t] = pModelGraph_->AddStochasticNode(dimArrayMap_["x"], "dnorm.var", params);

        params[0] = x[t];
        params[1] = prec_or_var_y;
        y[t-1] = pModelGraph_->AddStochasticNode(dimArrayMap_["y"], "dnorm.var", params, true);
      }
    }

    // build graph
    //------------
    if (verbose_>=2)
      os_ << "The graph has a cycle? " << pModelGraph_->HasCycle() << std::endl;

    pModelGraph_->Build();

    setObsValues();
  }


  void HmmNormalLinear::initAccumulators(std::map<String, DataType::Array> & statsValuesMap)
  {
    scalarAcc_.AddFeature(MEAN);
    scalarAcc_.AddFeature(VARIANCE);
    scalarAcc_.AddFeature(QUANTILES);
    Scalar probs[] = {0.05, 0.95};
    scalarAcc_.SetQuantileProbs(probs, probs + sizeof(probs) / sizeof(probs[0]));
    scalarAcc_.AddFeature(PDF);
    scalarAcc_.SetPdfParam(200, 40);

    Size t_max = sizeParamMap_["t.max"];

    statsValuesMap["x"].SetPtr(DataType::Array(t_max+1));
    statsValuesMap["x.var"].SetPtr(DataType::Array(t_max+1));
    statsValuesMap["x.q05"].SetPtr(DataType::Array(t_max+1));
    statsValuesMap["x.q95"].SetPtr(DataType::Array(t_max+1));
  }

  void HmmNormalLinear::initFilterAccumulators()
  {
    initAccumulators(smcFilterValuesMap_);
  }

  void HmmNormalLinear::accumulate(Size t, std::map<String, DataType::Array> & statsValuesMap, const String & title)
  {
    Types<NodeId>::Array & x = modelNodeIdMap_["x"];

    DataType::Array & x_est = statsValuesMap["x"];
    DataType::Array & x_var = statsValuesMap["x.var"];
    DataType::Array & x_quant_05 = statsValuesMap["x.q05"];
    DataType::Array & x_quant_95 = statsValuesMap["x.q95"];

    pSampler_->Accumulate(x[t], scalarAcc_);
    x_est[t].Alloc(scalarAcc_.Mean());
    x_var[t].Alloc(scalarAcc_.Variance());
    x_quant_05[t].Alloc(scalarAcc_.Quantile(0));
    x_quant_95[t].Alloc(scalarAcc_.Quantile(1));

    if (showAllPlot_)
    {
      ScalarHistogram pdf_hist = scalarAcc_.Pdf();
      Plot pdf_plot_PF(argc_, argv_);
      pdf_plot_PF.AddHistogram(pdf_hist, "", Qt::blue);
      pdf_plot_PF.SetTitle(title+" x pdf estimates, t = "+toString(t));
      pdf_plot_PF.SetBackgroundColor(Qt::white);
      pdf_plot_PF.Show();
    }
  }

  void HmmNormalLinear::filterAccumulate(Size t)
  {
    accumulate(t, smcFilterValuesMap_, "Filtering");
  }

  void HmmNormalLinear::initSmoothAccumulators()
  {
    initAccumulators(smcSmoothValuesMap_);
  }

  void HmmNormalLinear::smoothAccumulate(Size t)
  {
    accumulate(t, smcSmoothValuesMap_, "Smoothing");
  }


  void HmmNormalLinear::PlotResults(const String & plotFileName) const
  {
    const DataType::Array & x_gen = dataValuesMap_.at("x");
    const DataType::Array & y_obs = dataValuesMap_.at("y");
    const DataType::Array & x_est_PF = smcFilterValuesMap_.at("x");
    const DataType::Array & x_quant_05_PF = smcFilterValuesMap_.at("x.q05");
    const DataType::Array & x_quant_95_PF = smcFilterValuesMap_.at("x.q95");
    const DataType::Array & x_est_PS = smcSmoothValuesMap_.at("x");

    Size t_max = sizeParamMap_.at("t.max");
    DataType::Array time_x(t_max+1);
    DataType::Array time_y(t_max);
    time_x[0] = DataType(0.0);
    for (Size t=1; t<t_max+1; ++t)
    {
      time_x[t] = DataType(Scalar(t));
      time_y[t-1] = DataType(Scalar(t));
    }

    Plot results_plot(argc_, argv_);
    results_plot.AddCurve(time_x, x_gen, "hidden state", Qt::black, 2, Qt::NoPen, 9, QwtSymbol::Cross);
    results_plot.AddCurve(time_y, y_obs, "observation", Qt::darkGray, 2, Qt::NoPen, 8, QwtSymbol::XCross);
    if(benchFilterValuesMap_.count("x"))
      results_plot.AddCurve(time_x, benchFilterValuesMap_.at("x"), "KF estimate", Qt::green, 2);
    results_plot.AddCurve(time_x, x_est_PF, "PF estimate", Qt::blue, 2);
    results_plot.AddCurve(time_x, x_quant_05_PF, "PF 5% quantile", Qt::blue, 0.5, Qt::DashLine);
    results_plot.AddCurve(time_x, x_quant_95_PF, "PF 95% quantile", Qt::blue, 0.5, Qt::DashLine);
    if(benchSmoothValuesMap_.count("x"))
      results_plot.AddCurve(time_x, benchSmoothValuesMap_.at("x"), "KS estimate", Qt::green, 2);
    results_plot.AddCurve(time_x, x_est_PS, "PS estimate", Qt::cyan, 2);
    results_plot.SetTitle("");
    results_plot.SetAxesLabels("time", "state");
    results_plot.SetBackgroundColor(Qt::white);
    results_plot.SetLegend(QwtPlot::RightLegend);

    if (!plotFileName.empty())
      results_plot.PrintPdf(plotFileName);

    if( showFinalPlot_ )
      results_plot.Show();
  }


}
