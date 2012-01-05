//                                               -*- C++ -*-
/*! \file HmmMNormalLinear.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "HmmMNormalLinear.hpp"

#include "BiipsCore.hpp"

#include "samplers/ConjugateMNormalLinear.hpp"
#include "samplers/ConjugateMNormalVarLinear.hpp"

#include "KalmanFilter.hpp"
#include "Plot.hpp"
#include "TestIO.hpp"
#include "common/cholesky.hpp"

namespace Biips
{

  const String HmmMNormalLinear::NAME_ = "HMM multivariate Normal Linear";

  HmmMNormalLinear::HmmMNormalLinear(int argc,
                                     char** argv,
                                     Size verbose,
                                     Size showMode,
                                     Bool precFlag,
                                     std::ostream & os) :
    BaseType(argc, argv, NAME_, verbose, showMode, os), precFlag_(precFlag)
  {
    // Default model parameters
    sizeParamMap_["t.max"] = 20;

    inDataVarNames_.push_back("x");
    inDataVarNames_.push_back("u");
    inDataVarNames_.push_back("y");

    DimArray::Ptr scalar_dim(new DimArray(1, 1));

    dimArrayMap_["x"] = scalar_dim;
    dimArrayMap_["u"] = scalar_dim;
    dimArrayMap_["y"] = scalar_dim;

    Size dim_x = (*dimArrayMap_["x"])[0];
    Size dim_u = (*dimArrayMap_["u"])[0];
    Size dim_y = (*dimArrayMap_["y"])[0];

    dimArrayMap_["P.0"] = DimArray::Ptr(new DimArray(2, dim_x));
    dimArrayMap_["F"] = DimArray::Ptr(new DimArray(2, dim_x));
    dimArrayMap_["B"] = DimArray::Ptr(new DimArray(2));
    (*dimArrayMap_["B"])[0] = dim_x;
    (*dimArrayMap_["B"])[1] = dim_u;
    dimArrayMap_["Q"] = DimArray::Ptr(new DimArray(2, dim_x));
    dimArrayMap_["H"] = DimArray::Ptr(new DimArray(2));
    (*dimArrayMap_["H"])[0] = dim_y;
    (*dimArrayMap_["H"])[1] = dim_x;
    dimArrayMap_["R"] = DimArray::Ptr(new DimArray(2, dim_y));

    dataTypeParamMap_["mean.x.0"].SetPtr(dimArrayMap_["x"],
                                         ValArray::Ptr(new ValArray(dimArrayMap_["x"]->Length(),
                                                                    0.0)));
    dataTypeParamMap_["P.0"].SetPtr(dimArrayMap_["P.0"],
                                    ValArray::Ptr(new ValArray(dimArrayMap_["P.0"]->Length(),
                                                               1.0)));
    dataTypeParamMap_["F"].SetPtr(dimArrayMap_["F"],
                                  ValArray::Ptr(new ValArray(dimArrayMap_["F"]->Length(),
                                                             1.0)));
    dataTypeParamMap_["B"].SetPtr(dimArrayMap_["B"],
                                  ValArray::Ptr(new ValArray(dimArrayMap_["B"]->Length(),
                                                             0.0)));
    dataTypeParamMap_["Q"].SetPtr(dimArrayMap_["Q"],
                                  ValArray::Ptr(new ValArray(dimArrayMap_["Q"]->Length(),
                                                             1.0)));
    dataTypeParamMap_["H"].SetPtr(dimArrayMap_["H"],
                                  ValArray::Ptr(new ValArray(dimArrayMap_["H"]->Length(),
                                                             1.0)));
    dataTypeParamMap_["R"].SetPtr(dimArrayMap_["R"],
                                  ValArray::Ptr(new ValArray(dimArrayMap_["R"]->Length(),
                                                             0.5)));

    if (precFlag_)
    {
      typedef ublas::matrix<Scalar, StorageOrder, ValArray> MatrixType;
      MatrixType P0_prec_mat(dataTypeParamMap_["P.0"].Dim()[0],
                             dataTypeParamMap_["P.0"].Dim()[1],
                             ValArray());
      P0_prec_mat.data().swap(dataTypeParamMap_["P.0"].Values());
      if (!ublas::cholesky_factorize(P0_prec_mat))
        throw RuntimeError("HmmMNormalLinear: matrix P0 is not positive-semidefinite.");
      ublas::cholesky_invert(P0_prec_mat);
      P0_prec_mat.data().swap(dataTypeParamMap_["P.0"].Values());

      MatrixType Q_prec_mat(dataTypeParamMap_["Q"].Dim()[0],
                            dataTypeParamMap_["Q"].Dim()[1],
                            ValArray());
      Q_prec_mat.data().swap(dataTypeParamMap_["Q"].Values());
      if (!ublas::cholesky_factorize(Q_prec_mat))
        throw RuntimeError("HmmMNormalLinear: matrix Q is not positive-semidefinite.");
      ublas::cholesky_invert(Q_prec_mat);
      Q_prec_mat.data().swap(dataTypeParamMap_["Q"].Values());

      MatrixType R_prec_mat(dataTypeParamMap_["R"].Dim()[0],
                            dataTypeParamMap_["R"].Dim()[1],
                            ValArray());
      R_prec_mat.data().swap(dataTypeParamMap_["R"].Values());
      if (!ublas::cholesky_factorize(R_prec_mat))
        throw RuntimeError("HmmMNormalLinear: matrix R is not positive-semidefinite.");
      ublas::cholesky_invert(R_prec_mat);
      R_prec_mat.data().swap(dataTypeParamMap_["R"].Values());
    }

    Size t_max = sizeParamMap_.at("t.max");
    dataValuesMap_["u"].resize(t_max);
    for (Size i = 0; i < t_max; ++i)
    {
      ValArray::Ptr p_val(new ValArray(dimArrayMap_["u"]->Length()));
      dataValuesMap_["u"][i] = MultiArray(dimArrayMap_["u"], p_val);
    }

    obsVarNames_.push_back("y");

    inBenchFilterVarNames_.push_back("x");

    printResultsVarNames_.push_back("x");

    if (precFlag_)
      nodeSamplerFactoryInvOrder_.push_back(ConjugateMNormalLinearFactory::Instance());
    else
      nodeSamplerFactoryInvOrder_.push_back(ConjugateMNormalVarLinearFactory::Instance());
  }

  void HmmMNormalLinear::PrintIntro() const
  {
    using namespace std;
    os_ << "BiiPS test: Hidden Markov Model linear gaussian multi-D" << endl;
    os_ << "comparison between Kalman Filter and Particle Filter" << endl;
    os_ << "=======================================================" << endl;
    os_ << "x[0] --> x[1] --> ... --> x[t-1] --> x[t] --> ..." << endl;
    os_ << "          |                 |         |" << endl;
    os_ << "          v                 v         v" << endl;
    os_ << "         y[1]             y[t-1]     y[t]" << endl;
    os_ << endl;
    os_ << "         x[0] ~ Normal(mean.x.0, P.0)" << endl;
    os_ << "x[t] | x[t-1] ~ Normal(F x[t-1] + B u, Q) for all t>0" << endl;
    os_ << "  y[t] | x[t] ~ Normal(H x[t], R)  for all t>0" << endl;
    os_ << endl;
  }

  //  void HmmMNormalLinear::InputModelParam(std::istream & is)
  //  {
  //    using namespace std;
  //
  //    if (verbose_)
  //      os_ << "State x dimension: ";
  //    is >> sizeParamMap_["dim.x"];
  //    if (verbose_)
  //      os_ << endl << "dim_x = " << sizeParamMap_["dim.x"] << endl << endl;
  //
  //    if (verbose_)
  //      os_ << "Control u dimension: ";
  //    is >> sizeParamMap_["dim.u"];
  //    if (verbose_)
  //      os_ << endl << "dim_u = " << sizeParamMap_["dim.u"] << endl << endl;
  //
  //    if (verbose_)
  //      os_ << "Observation y dimension: ";
  //    is >> sizeParamMap_["dim.y"];
  //    if (verbose_)
  //      os_ << endl << "dim_y = " << sizeParamMap_["dim.y"] << endl << endl;
  //
  //    if (verbose_)
  //      os_ << "Final time: ";
  //    is >> sizeParamMap_["t.max"];
  //    if (verbose_)
  //      os_ << endl << "t_max = " << sizeParamMap_["t.max"] << endl << endl;
  //
  //    Size dim_x = sizeParamMap_["dim.x"];
  //    Size dim_u = sizeParamMap_["dim.u"];
  //    Size dim_y = sizeParamMap_["dim.y"];
  //    Size t_max = sizeParamMap_["t.max"];
  //
  //    dimArrayMap_["x"] = DimArray::Ptr(new DimArray(1,dim_x));
  //    dimArrayMap_["P.0"] = DimArray::Ptr(new DimArray(2,dim_x));
  //    dimArrayMap_["F"] = DimArray::Ptr(new DimArray(2,dim_x));
  //    dimArrayMap_["B"] = DimArray::Ptr(new DimArray(2));
  //    (*dimArrayMap_["B"])[0] = dim_x;
  //    (*dimArrayMap_["B"])[1] = dim_u;
  //    dimArrayMap_["u"] = DimArray::Ptr(new DimArray(1,dim_u));
  //    dimArrayMap_["Q"] = DimArray::Ptr(new DimArray(2,dim_x));
  //    dimArrayMap_["y"] = DimArray::Ptr(new DimArray(1,dim_y));
  //    dimArrayMap_["H"] = DimArray::Ptr(new DimArray(2));
  //    (*dimArrayMap_["H"])[0] = dim_y;
  //    (*dimArrayMap_["H"])[1] = dim_x;
  //    dimArrayMap_["R"] = DimArray::Ptr(new DimArray(2,dim_y));
  //
  //    dataTypeParamMap_["mean.x.0"].SetPtr(dimArrayMap_["x"]).AllocValue();
  //    dataTypeParamMap_["P.0"].SetPtr(dimArrayMap_["P.0"]).AllocValue();
  //    dataTypeParamMap_["F"].SetPtr(dimArrayMap_["F"]).AllocValue();
  //    dataTypeParamMap_["B"].SetPtr(dimArrayMap_["B"]).AllocValue();
  //    dataTypeParamMap_["Q"].SetPtr(dimArrayMap_["Q"]).AllocValue();
  //    dataTypeParamMap_["H"].SetPtr(dimArrayMap_["H"]).AllocValue();
  //    dataTypeParamMap_["R"].SetPtr(dimArrayMap_["R"]).AllocValue();
  //
  //    if (verbose_)
  //      os_ << "Initial state mean vector: ";
  //    InputMultiArray(dataTypeParamMap_["mean.x.0"], "", 0, is);
  //    if (verbose_)
  //      os_ << endl << "mean_x0 = " << dataTypeParamMap_["mean.x.0"] << endl << endl;
  //
  //    if (verbose_)
  //      os_ << "Initial state covariance matrix: ";
  //    InputMultiArray(dataTypeParamMap_["P.0"], "", 0, is);
  //    if (verbose_)
  //      os_ << endl << "P0 = " << dataTypeParamMap_["P.0"] << endl << endl;
  //
  //    if (verbose_)
  //      os_ << "State transition matrix: ";
  //    InputMultiArray(dataTypeParamMap_["F"], "", 0, is);
  //    if (verbose_)
  //      os_ << endl << "F = " << dataTypeParamMap_["F"] << endl << endl;
  //
  //    if (verbose_)
  //      os_ << "Control transition matrix: ";
  //    InputMultiArray(dataTypeParamMap_["B"], "", 0, is);
  //    if (verbose_)
  //      os_ << endl << "B = " << dataTypeParamMap_["B"] << endl << endl;
  //
  //    if (verbose_)
  //      os_ << "State covariance matrix: ";
  //    InputMultiArray(dataTypeParamMap_["Q"], "", 0, is);
  //    if (verbose_)
  //      os_ << endl << "Q = " << dataTypeParamMap_["Q"] << endl << endl;
  //
  //    if (verbose_)
  //      os_ << "Observation transition matrix: ";
  //    InputMultiArray(dataTypeParamMap_["H"], "", 0, is);
  //    if (verbose_)
  //      os_ << endl << "H = " << dataTypeParamMap_["H"] << endl << endl;
  //
  //    if (verbose_)
  //      os_ << "Observation covariance matrix: ";
  //    InputMultiArray(dataTypeParamMap_["R"], "", 0, is);
  //    if (verbose_)
  //      os_ << endl << "R = " << dataTypeParamMap_["R"] << endl << endl;
  //
  //
  //    if (precFlag_)
  //    {
  //      MatrixRef P0_prec_mat = dataTypeParamMap_["P.0"].MatrixView();
  //      if (!ublas::cholesky_factorize(P0_prec_mat))
  //        throw RuntimeError("HmmMNormalLinear: matrix P0 is not positive-semidefinite.");
  //      ublas::cholesky_invert(P0_prec_mat);
  //
  //      MatrixRef Q_prec_mat = dataTypeParamMap_["Q"].MatrixView();
  //      if (!ublas::cholesky_factorize(Q_prec_mat))
  //        throw RuntimeError("HmmMNormalLinear: matrix Q is not positive-semidefinite.");
  //      ublas::cholesky_invert(Q_prec_mat);
  //
  //      MatrixRef R_prec_mat = dataTypeParamMap_["R"].MatrixView();
  //      if (!ublas::cholesky_factorize(R_prec_mat))
  //        throw RuntimeError("HmmMNormalLinear: matrix R is not positive-semidefinite.");
  //      ublas::cholesky_invert(R_prec_mat);
  //    }
  //  }


  void HmmMNormalLinear::BuildModelGraph()
  {
    // load Base module
    //-----------------
    loadBase();

    // graph
    //------
    pModelGraph_ = Graph::Ptr(new Graph());

    // create constant nodes
    //----------------------

    NodeId mean_x0 =
        pModelGraph_->AddConstantNode(dataTypeParamMap_["mean.x.0"]);
    NodeId F = pModelGraph_->AddConstantNode(dataTypeParamMap_["F"]);
    NodeId B = pModelGraph_->AddConstantNode(dataTypeParamMap_["B"]);
    NodeId H = pModelGraph_->AddConstantNode(dataTypeParamMap_["H"]);

    NodeId P0 = pModelGraph_->AddConstantNode(dataTypeParamMap_["P.0"]);
    NodeId Q = pModelGraph_->AddConstantNode(dataTypeParamMap_["Q"]);
    NodeId R = pModelGraph_->AddConstantNode(dataTypeParamMap_["R"]);

    Size t_max = sizeParamMap_["t.max"];

    Types<NodeId>::Array u(t_max);
    const MultiArray::Array & u_val = dataValuesMap_["u"];
    for (Size t = 1; t <= t_max; ++t)
      u[t - 1] = pModelGraph_->AddConstantNode(u_val[t - 1]);

    // create Stochastic nodeId collections
    //-----------------------------------
    modelNodeIdMap_["x"] = Types<NodeId>::Array(t_max + 1);
    modelNodeIdMap_["y"] = Types<NodeId>::Array(t_max);

    Types<NodeId>::Array & x = modelNodeIdMap_["x"];
    Types<NodeId>::Array & y = modelNodeIdMap_["y"];

    // create Logical nodeId collections
    //-----------------------------------
    Types<NodeId>::Array prod_x(t_max);
    Types<NodeId>::Array prod_u(t_max);
    Types<NodeId>::Array sum_x(t_max);
    Types<NodeId>::Array prod_y(t_max);

    // create Nodes
    //-------------
    Types<NodeId>::Array params(2);

    params[0] = mean_x0;
    params[1] = P0;
    if (precFlag_)
      x[0] = pModelGraph_->AddStochasticNode(distTab_["dmnorm"], params, false);
    else
      x[0] = pModelGraph_->AddStochasticNode(distTab_["dmnormvar"],
                                             params,
                                             false);

    for (Size t = 1; t <= t_max; ++t)
    {
      params[0] = F;
      params[1] = x[t - 1];
      prod_x[t - 1] = pModelGraph_->AddLogicalNode(funcTab_["%*%"], params);

      params[0] = B;
      params[1] = u[t - 1];
      prod_u[t - 1] = pModelGraph_->AddLogicalNode(funcTab_["%*%"], params);

      params[0] = prod_x[t - 1];
      params[1] = prod_u[t - 1];
      sum_x[t - 1] = pModelGraph_->AddLogicalNode(funcTab_["+"], params);

      params[0] = sum_x[t - 1];
      params[1] = Q;
      if (precFlag_)
        x[t] = pModelGraph_->AddStochasticNode(distTab_["dmnorm"],
                                               params,
                                               false);
      else
        x[t] = pModelGraph_->AddStochasticNode(distTab_["dmnormvar"],
                                               params,
                                               false);

      params[0] = H;
      params[1] = x[t];
      prod_y[t - 1] = pModelGraph_->AddLogicalNode(funcTab_["%*%"], params);

      params[0] = prod_y[t - 1];
      params[1] = R;
      if (precFlag_)
        y[t - 1] = pModelGraph_->AddStochasticNode(distTab_["dmnorm"],
                                                   params,
                                                   true);
      else
        y[t - 1] = pModelGraph_->AddStochasticNode(distTab_["dmnormvar"],
                                                   params,
                                                   true);
    }

    // build graph
    //------------
    if (verbose_ >= 2)
      os_ << "The graph has a cycle ? " << pModelGraph_->HasCycle()
          << std::endl;

    pModelGraph_->Build();

    setObsValues();
  }

  void HmmMNormalLinear::RunBench()
  {
    Size t_max = sizeParamMap_["t.max"];
    const MultiArray & mean_x0_val = dataTypeParamMap_["mean.x.0"];
    const MultiArray & P0_val = dataTypeParamMap_["P.0"];
    const MultiArray & Q_val = dataTypeParamMap_["Q"];
    const MultiArray & F_val = dataTypeParamMap_["F"];
    const MultiArray & B_val = dataTypeParamMap_["B"];
    const MultiArray & H_val = dataTypeParamMap_["H"];
    const MultiArray & R_val = dataTypeParamMap_["R"];

    const MultiArray::Array & y_obs = dataValuesMap_.at("y");

    // kalman filter
    //--------------
    KalmanFilter kalman_filter(mean_x0_val, P0_val, R_val);
    kalman_filter.SetEvolutionModel(F_val, B_val, Q_val);
    kalman_filter.SetObservationModel(H_val, R_val);

    benchFilterValuesMap_["x"] = MultiArray::Array(t_max + 1);
    benchFilterValuesMap_["var.x"] = MultiArray::Array(t_max + 1);
    MultiArray::Array & x_est_KF = benchFilterValuesMap_["x"];
    MultiArray::Array & x_var_KF = benchFilterValuesMap_["var.x"];

    x_est_KF[0] = mean_x0_val;
    x_var_KF[0] = P0_val;

    Bool u_flag = dataValuesMap_.count("u");
    MultiArray u_val(dimArrayMap_["u"],
                     ValArray::Ptr(new ValArray(dimArrayMap_["u"]->Length())));
    for (Size t = 1; t < t_max + 1; ++t)
    {
      if (u_flag)
        kalman_filter.Update(y_obs[t - 1], dataValuesMap_.at("u")[t - 1]);
      else
        kalman_filter.Update(y_obs[t - 1], u_val);
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

  void HmmMNormalLinear::initAccumulators(Size nParticles,
                                          Size numBins,
                                          std::map<String, MultiArray::Array> & statsValuesMap)
  {
    elementAcc_.AddFeature(MEAN);

    Size t_max = sizeParamMap_["t.max"];

    statsValuesMap["x"] = MultiArray::Array(t_max + 1);
  }

  void HmmMNormalLinear::initFilterAccumulators(Size nParticles, Size numBins)
  {
    initAccumulators(nParticles, numBins, smcFilterValuesMap_);
  }

  void HmmMNormalLinear::accumulate(Size t,
                                    std::map<String, MultiArray::Array> & statsValuesMap,
                                    const String & title)
  {
    Types<NodeId>::Array & x = modelNodeIdMap_["x"];

    MultiArray::Array & x_est = statsValuesMap["x"];

    pSampler_->Accumulate(x[t], elementAcc_);
    x_est[t].SetPtr(DimArray::Ptr(new DimArray(elementAcc_.Mean().Dim())),
                    ValArray::Ptr(new ValArray(elementAcc_.Mean().Values())));
  }

  void HmmMNormalLinear::filterAccumulate(Size t)
  {
    accumulate(t, smcFilterValuesMap_, "Filtering");
  }

  void HmmMNormalLinear::initSmoothAccumulators(Size nParticles, Size numBins)
  {
    initAccumulators(nParticles, numBins, smcSmoothValuesMap_);
  }

  void HmmMNormalLinear::smoothAccumulate(Size t)
  {
    accumulate(t, smcSmoothValuesMap_, "Smoothing");
  }

  void HmmMNormalLinear::PlotResults(const String & plotFileName) const
  {
    const MultiArray::Array & x_gen = dataValuesMap_.at("x");
    const MultiArray::Array & y_obs = dataValuesMap_.at("y");
    const MultiArray::Array & x_est_KF = benchFilterValuesMap_.at("x");
    const MultiArray::Array & x_est_PF = smcFilterValuesMap_.at("x");
    const MultiArray::Array & x_est_PS = smcSmoothValuesMap_.at("x");

    Size t_max = sizeParamMap_.at("t.max");
    Size dim_x = (*dimArrayMap_.at("x"))[0];
    Size dim_y = (*dimArrayMap_.at("y"))[0];

    Plot results_plot(argc_, argv_);

    if (dim_x == 1)
    {
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

      results_plot.AddCurve(time_x,
                            x_gen,
                            "hidden state",
                            Qt::black,
                            2,
                            Qt::NoPen,
                            9,
                            QwtSymbol::Cross);
      if (dim_y == 1)
        results_plot.AddCurve(time_y,
                              y_obs,
                              "observation",
                              Qt::gray,
                              2,
                              Qt::NoPen,
                              8,
                              QwtSymbol::XCross);
      results_plot.AddCurve(time_x, x_est_KF, "KF estimate", Qt::green, 2);
      results_plot.AddCurve(time_x, x_est_PF, "PF estimate", Qt::blue, 2);
      results_plot.AddCurve(time_x, x_est_PS, "PS estimate", Qt::cyan, 2);
      results_plot.SetTitle("");
      results_plot.SetAxesLabels("time", "state");
      results_plot.SetBackgroundColor(Qt::white);
      results_plot.SetLegend(QwtPlot::RightLegend);
    }
    else if (dim_x > 1)
    {
      Types<Scalar>::Array x_gen_0(t_max + 1);
      Types<Scalar>::Array x_gen_1(t_max + 1);
      Types<Scalar>::Array x_est_KF_0(t_max + 1);
      Types<Scalar>::Array x_est_KF_1(t_max + 1);
      Types<Scalar>::Array x_est_PF_0(t_max + 1);
      Types<Scalar>::Array x_est_PF_1(t_max + 1);
      Types<Scalar>::Array x_est_PS_0(t_max + 1);
      Types<Scalar>::Array x_est_PS_1(t_max + 1);
      for (Size t = 0; t < t_max + 1; ++t)
      {
        x_gen_0[t] = x_gen[t].Values()[0];
        x_gen_1[t] = x_gen[t].Values()[1];
        x_est_KF_0[t] = x_est_KF[t].Values()[0];
        x_est_KF_1[t] = x_est_KF[t].Values()[1];
        x_est_PF_0[t] = x_est_PF[t].Values()[0];
        x_est_PF_1[t] = x_est_PF[t].Values()[1];
        x_est_PS_0[t] = x_est_PS[t].Values()[0];
        x_est_PS_1[t] = x_est_PS[t].Values()[1];
      }

      results_plot.AddCurve(x_gen_0,
                            x_gen_1,
                            "hidden state",
                            Qt::black,
                            2,
                            Qt::NoPen,
                            9,
                            QwtSymbol::Cross);

      Types<Scalar>::Array y_obs_0(t_max);
      Types<Scalar>::Array y_obs_1(t_max);
      if (dim_y > 1)
      {
        for (Size t = 0; t < t_max; ++t)
        {
          y_obs_0[t] = y_obs[t].Values()[0];
          y_obs_1[t] = y_obs[t].Values()[1];
        }
        results_plot.AddCurve(y_obs_0,
                              y_obs_1,
                              "observation",
                              Qt::gray,
                              2,
                              Qt::NoPen,
                              8,
                              QwtSymbol::XCross);
      }

      results_plot.AddCurve(x_est_KF_0, x_est_KF_1, "KF estimate", Qt::green, 2);
      results_plot.AddCurve(x_est_PF_0, x_est_PF_1, "PF estimate", Qt::blue, 2);
      results_plot.AddCurve(x_est_PS_0, x_est_PS_1, "PS estimate", Qt::cyan, 2);

      results_plot.SetTitle("");
      results_plot.SetAxesLabels("x[0]", "x[1]");
      results_plot.SetBackgroundColor(Qt::white);
      results_plot.SetLegend(QwtPlot::RightLegend);
    }

    if (!plotFileName.empty())
      results_plot.PrintPdf(plotFileName);

    if (showMode_ >= 1)
      results_plot.Show();
  }

}
