//                                               -*- C++ -*-
/*! \file HmmNormalNonLinear.cpp
* \brief
*
* $LastChangedBy$
* $LastChangedDate$
* $LastChangedRevision$
* $Id$
*/

#include "HmmNormalNonLinear.hpp"
#include "BiipsCore.hpp"
#include "BiipsBase.hpp"
#include "Plot.hpp"
#include "TestIO.hpp"

namespace Biips
{

  const String HmmNormalNonLinear::NAME_ = "HMM Normal non linear 1D";

  HmmNormalNonLinear::HmmNormalNonLinear(int argc, char** argv, Size verbose, Size showMode, Bool precFlag, std::ostream & os)
  : BaseType(argc, argv, NAME_, verbose, showMode, os), precFlag_(precFlag)
  {
    // Default model parameters
    sizeParamMap_["t.max"] = 20;

    scalarParamMap_["mean.x.0"] = 0.0;
    scalarParamMap_["var.x.0"] = 5.0;
    scalarParamMap_["var.x"] = 10.0;
    scalarParamMap_["var.y"] = 1.0;

    DimArray::Ptr scalar_dim(new DimArray(1,1));

    inDataVarNames_.push_back("x");
    dimArrayMap_["x"] = scalar_dim;

    inDataVarNames_.push_back("y");
    dimArrayMap_["y"] = scalar_dim;

    obsVarNames_.push_back("y");

    inBenchFilterVarNames_.push_back("x");

    printResultsVarNames_.push_back("x");

//    if (precFlag)
//      nodeSamplerFactoryInvOrder_.push_back(ConjugateNormalFactory::Instance());
//    else
//      nodeSamplerFactoryInvOrder_.push_back(ConjugateNormalVarFactory::Instance());
  }


  void HmmNormalNonLinear::PrintIntro() const
  {
    using namespace std;
    cout << "BiiPS test : Hidden Markov Model non-linear gaussian 1D" << endl;
    cout << "=======================================================" << endl;
    cout << "x[0] --> x[1] --> ... --> x[t-1] --> x[t] --> ..." << endl;
    cout << "          |                 |         |" << endl;
    cout << "          v                 v         v" << endl;
    cout << "         y[1]             y[t-1]     y[t]" << endl;
    cout << endl;
    cout << "         x[0] ~ Normal(mean.x.0, var.x.0)" << endl;
    cout << "x[t] | x[t-1] ~ Normal(f(x[t-1], t), var.x) for all t>0" << endl;
    cout << "  y[t] | x[t] ~ Normal(g(x[t]), var.y)  for all t>0" << endl;
    cout << "where" << endl;
    cout << "  f(x, t) = 1/2*x + 25*x/(1+x^2) + 8*cos(1.2*t)" << endl;
    cout << "     g(x) = x^2/20" << endl;
    cout << endl;
  }


  void HmmNormalNonLinear::BuildModelGraph()
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
    NodeId a_f = pModelGraph_->AddConstantNode(DataType(0.5));
    NodeId b_f = pModelGraph_->AddConstantNode(DataType(25.0));
    NodeId c_f = pModelGraph_->AddConstantNode(DataType(1.0));
    NodeId d_f = pModelGraph_->AddConstantNode(DataType(8.0));
    NodeId e_f = pModelGraph_->AddConstantNode(DataType(1.2));
    NodeId a_g = pModelGraph_->AddConstantNode(DataType(20.0));
    NodeId expo = pModelGraph_->AddConstantNode(DataType(2.0));

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


    // create Logical nodeId collections
    //-----------------------------------
    Types<NodeId>::Array prod_a_f(t_max);
    Types<NodeId>::Array pow_x(t_max+1);
    Types<NodeId>::Array sum_c_f(t_max);
    Types<NodeId>::Array div_f(t_max);
    Types<NodeId>::Array prod_b_f(t_max);
    Types<NodeId>::Array prod_e_f(t_max);
    Types<NodeId>::Array cos_f(t_max);
    Types<NodeId>::Array prod_d_f(t_max);
    Types<NodeId>::Array sum1_f(t_max);
    Types<NodeId>::Array f(t_max);
    Types<NodeId>::Array g(t_max);

    // create Nodes
    //-------------
    DimArray::Ptr scalar_dim(new DimArray(1,1));

    Types<NodeId>::Array binary_params(2);
    Types<NodeId>::Array unary_param(1);
    Types<NodeId>::Array time(t_max);

    if (precFlag_)
    {
      binary_params[0] = mean_x0;
      binary_params[1] = prec_or_var_x0;
      x[0] = pModelGraph_->AddStochasticNode(dimArrayMap_["x"], "dnorm", binary_params, false);

      binary_params[0] = x[0];
      binary_params[1] = expo;
      pow_x[0] = pModelGraph_->AddLogicalNode(scalar_dim, "^", binary_params);

      for (Size t=1; t<t_max+1; ++t)
      {
        binary_params[0] = a_f;
        binary_params[1] = x[t-1];
        prod_a_f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "*", binary_params);

        binary_params[0] = c_f;
        binary_params[1] = pow_x[t-1];
        sum_c_f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "+", binary_params);

        binary_params[0] = x[t-1];
        binary_params[1] = sum_c_f[t-1];
        div_f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "/", binary_params);

        binary_params[0] = b_f;
        binary_params[1] = div_f[t-1];
        prod_b_f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "*", binary_params);

        time[t-1] = pModelGraph_->AddConstantNode(DataType(Scalar(t)));

        binary_params[0] = e_f;
        binary_params[1] = time[t-1];
        prod_e_f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "*", binary_params);

        unary_param[0] = prod_e_f[t-1];
        cos_f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "cos", unary_param);

        binary_params[0] = d_f;
        binary_params[1] = cos_f[t-1];
        prod_d_f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "*", binary_params);

        binary_params[0] = prod_a_f[t-1];
        binary_params[1] = prod_b_f[t-1];
        sum1_f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "+", binary_params);

        binary_params[0] = sum1_f[t-1];
        binary_params[1] = prod_d_f[t-1];
        f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "+", binary_params);

        binary_params[0] = f[t-1];
        binary_params[1] = prec_or_var_x;
        x[t] = pModelGraph_->AddStochasticNode(dimArrayMap_["x"], "dnorm", binary_params, false);

        binary_params[0] = x[t];
        binary_params[1] = expo;
        pow_x[t] = pModelGraph_->AddLogicalNode(scalar_dim, "^", binary_params);

        binary_params[0] = pow_x[t];
        binary_params[1] = a_g;
        g[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "/", binary_params);

        binary_params[0] = g[t-1];
        binary_params[1] = prec_or_var_y;
        y[t-1] = pModelGraph_->AddStochasticNode(dimArrayMap_["y"], "dnorm", binary_params, true);
      }
    }
    else
    {
      binary_params[0] = mean_x0;
      binary_params[1] = prec_or_var_x0;
      x[0] = pModelGraph_->AddStochasticNode(dimArrayMap_["x"], "dnorm.var", binary_params, false);

      binary_params[0] = x[0];
      binary_params[1] = expo;
      pow_x[0] = pModelGraph_->AddLogicalNode(scalar_dim, "^", binary_params);


      for (Size t=1; t<t_max+1; ++t)
      {
        binary_params[0] = a_f;
        binary_params[1] = x[t-1];
        prod_a_f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "*", binary_params);

        binary_params[0] = c_f;
        binary_params[1] = pow_x[t-1];
        sum_c_f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "+", binary_params);

        binary_params[0] = x[t-1];
        binary_params[1] = sum_c_f[t-1];
        div_f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "/", binary_params);

        binary_params[0] = b_f;
        binary_params[1] = div_f[t-1];
        prod_b_f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "*", binary_params);

        time[t-1] = pModelGraph_->AddConstantNode(DataType(Scalar(t)));

        binary_params[0] = e_f;
        binary_params[1] = time[t-1];
        prod_e_f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "*", binary_params);

        unary_param[0] = prod_e_f[t-1];
        cos_f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "cos", unary_param);

        binary_params[0] = d_f;
        binary_params[1] = cos_f[t-1];
        prod_d_f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "*", binary_params);

        binary_params[0] = prod_a_f[t-1];
        binary_params[1] = prod_b_f[t-1];
        sum1_f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "+", binary_params);

        binary_params[0] = sum1_f[t-1];
        binary_params[1] = prod_d_f[t-1];
        f[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "+", binary_params);

        binary_params[0] = f[t-1];
        binary_params[1] = prec_or_var_x;
        x[t] = pModelGraph_->AddStochasticNode(dimArrayMap_["x"], "dnorm.var", binary_params, false);

        binary_params[0] = x[t];
        binary_params[1] = expo;
        pow_x[t] = pModelGraph_->AddLogicalNode(scalar_dim, "^", binary_params);

        binary_params[0] = pow_x[t];
        binary_params[1] = a_g;
        g[t-1] = pModelGraph_->AddLogicalNode(scalar_dim, "/", binary_params);

        binary_params[0] = g[t-1];
        binary_params[1] = prec_or_var_y;
        y[t-1] = pModelGraph_->AddStochasticNode(dimArrayMap_["y"], "dnorm.var", binary_params, true);
      }
    }


    // build graph
    //------------
    if (verbose_>=2)
      os_ << "The graph has a cycle ? " << pModelGraph_->HasCycle() << std::endl;

    pModelGraph_->Build();

    setObsValues();
  }


  void HmmNormalNonLinear::initAccumulators(std::map<String, DataType::Array> & statsValuesMap)
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

  void HmmNormalNonLinear::initFilterAccumulators()
  {
    initAccumulators(smcFilterValuesMap_);
  }

  void HmmNormalNonLinear::accumulate(Size t, std::map<String, DataType::Array> & statsValuesMap, const String & title)
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

    if (showMode_ >= 2)
    {
      ScalarHistogram pdf_hist = scalarAcc_.Pdf();
      Plot pdf_plot_PF(argc_, argv_);
      pdf_plot_PF.AddHistogram(pdf_hist, "", Qt::blue);
      pdf_plot_PF.SetTitle(title+" x pdf estimates, t = "+toString(t));
      pdf_plot_PF.SetBackgroundColor(Qt::white);
      pdf_plot_PF.Show();
    }
  }

  void HmmNormalNonLinear::filterAccumulate(Size t)
  {
    accumulate(t, smcFilterValuesMap_, "Filtering");
  }

  void HmmNormalNonLinear::initSmoothAccumulators()
  {
    initAccumulators(smcSmoothValuesMap_);
  }

  void HmmNormalNonLinear::smoothAccumulate(Size t)
  {
    accumulate(t, smcSmoothValuesMap_, "Smoothing");
  }


  void HmmNormalNonLinear::PlotResults(const String & plotFileName) const
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
      results_plot.AddCurve(time_x, benchFilterValuesMap_.at("x"), "grid filtering estimate", Qt::green, 2);
    results_plot.AddCurve(time_x, x_est_PF, "particle filtering estimate", Qt::blue, 2);
    results_plot.AddCurve(time_x, x_quant_05_PF, "particle filtering 5% quantile", Qt::blue, 0.5, Qt::DashLine);
    results_plot.AddCurve(time_x, x_quant_95_PF, "particle filtering 95% quantile", Qt::blue, 0.5, Qt::DashLine);
    if(benchSmoothValuesMap_.count("x"))
      results_plot.AddCurve(time_x, benchSmoothValuesMap_.at("x"), "grid smoothing estimate", Qt::magenta, 2);
    results_plot.AddCurve(time_x, x_est_PS, "particle smoothing estimate", Qt::cyan, 2);
    results_plot.SetTitle("");
    results_plot.SetAxesLabels("time", "state");
    results_plot.SetBackgroundColor(Qt::white);
    results_plot.SetLegend(QwtPlot::RightLegend);

    if (!plotFileName.empty())
    {
      results_plot.PrintPdf(plotFileName);
    }

    if( showMode_ >= 1 )
      results_plot.Show();
  }


}
