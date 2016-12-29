#include "config.hpp"
#include "HmmNormalLinear.hpp"
#include "BiipsCore.hpp"

#include "samplers/ConjugateNormal.hpp"
#include "samplers/ConjugateNormalVar.hpp"

#include "KalmanFilter.hpp"
#include "TestIO.hpp"

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
    sizeParamMap_["t_max"] = 20;

    scalarParamMap_["mean_x0"] = 0.0;
    scalarParamMap_["var_x0"] = 1.0;
    scalarParamMap_["var_x"] = 1.0;
    scalarParamMap_["var_y"] = 0.5;

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
    os_ << "x0 --> x[1] --> ... --> x[t] --> ..." << endl;
    os_ << "        |                 |" << endl;
    os_ << "        v                 v" << endl;
    os_ << "       y[1]     ...     y[t]     ..." << endl;
    os_ << endl;
    os_ << "           x0 ~ Normal(mean_x0, var_x0)" << endl;
    os_ << "x[t] | x[t-1] ~ Normal(x[t-1], var_x) for all t>0" << endl;
    os_ << "  y[t] | x[t] ~ Normal(x[t], var_y)  for all t>0" << endl;
    os_ << endl;
  }

  //  void Hmm1D::InputModelParam(std::istream & is)
  //  {
  //    using namespace std;
  //
  //    if (verbose_)
  //      os_ << "Final time: ";
  //    is >> sizeParamMap_["t_max"];
  //    if (verbose_)
  //      os_ << endl << "t_max = " << sizeParamMap_["t_max"] << endl << endl;
  //  }


  void HmmNormalLinear::RunBench()
  {
    Size t_max = sizeParamMap_["t_max"];
    Scalar mean_x0_val = scalarParamMap_["mean_x0"];
    Scalar var_x0_val = scalarParamMap_["var_x0"];
    Scalar var_x_val = scalarParamMap_["var_x"];
    Scalar var_y_val = scalarParamMap_["var_y"];

    const MultiArray::Array & y_obs = dataValuesMap_.at("y");

    // kalman filter
    //--------------
    KalmanFilter kalman_filter(mean_x0_val, var_x0_val, var_y_val);
    kalman_filter.SetEvolutionModel(1.0, 0.0, var_x_val);
    kalman_filter.SetObservationModel(1.0, var_y_val);

    benchFilterValuesMap_["x"] = MultiArray::Array(t_max);
    benchFilterValuesMap_["var_x"] = MultiArray::Array(t_max);
    MultiArray::Array & x_est_KF = benchFilterValuesMap_["x"];
    MultiArray::Array & x_var_KF = benchFilterValuesMap_["var_x"];

    for (Size t = 0; t < t_max; ++t)
    {
      kalman_filter.Update(y_obs[t]);
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
    Scalar mean_x0_val = scalarParamMap_["mean_x0"];
    Scalar var_x0_val = scalarParamMap_["var_x0"];
    Scalar var_x_val = scalarParamMap_["var_x"];
    Scalar var_y_val = scalarParamMap_["var_y"];

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

    Size t_max = sizeParamMap_["t_max"];

    // create Stochastic nodeId collections
    //-----------------------------------
    NodeId x0;
    modelNodeIdMap_["x"] = Types<NodeId>::Array(t_max);
    modelNodeIdMap_["y"] = Types<NodeId>::Array(t_max);

    Types<NodeId>::Array & x = modelNodeIdMap_["x"];
    Types<NodeId>::Array & y = modelNodeIdMap_["y"];

    // create Nodes
    //-------------
    Types<NodeId>::Array params(2);

    char distname[12];
    if (precFlag_) {
        strcpy(distname, "dnorm");
    } else {
        strcpy(distname, "dnormvar");
    }
    
    params[0] = mean_x0;
    params[1] = prec_or_var_x0;
    x0 = pModelGraph_->AddStochasticNode(distTab_[distname], params, false);
    
    params[0] = x0;
    params[1] = prec_or_var_x;
    x[0]
            = pModelGraph_->AddStochasticNode(distTab_[distname], params, false);
    
    params[0] = x[0];
    params[1] = prec_or_var_y;
    y[0] = pModelGraph_->AddStochasticNode(distTab_[distname],
            params,
            true);
    
    for (Size t = 1; t < t_max; ++t)
    {
        params[0] = x[t - 1];
        params[1] = prec_or_var_x;
        x[t]
                = pModelGraph_->AddStochasticNode(distTab_[distname], params, false);
        
        params[0] = x[t];
        params[1] = prec_or_var_y;
        y[t] = pModelGraph_->AddStochasticNode(distTab_[distname],
                params,
                true);
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

    Size t_max = sizeParamMap_["t_max"];

    statsValuesMap["x"] = MultiArray::Array(t_max);
    statsValuesMap["x.var"] = MultiArray::Array(t_max);
    statsValuesMap["x.q05"] = MultiArray::Array(t_max);
    statsValuesMap["x.q95"] = MultiArray::Array(t_max);
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

  }

  void HmmNormalLinear::filterAccumulate(Size iter)
  {
	  accumulate(iter, smcFilterValuesMap_, "Filtering");
  }

  void HmmNormalLinear::initSmoothAccumulators(Size nParticles, Size numBins)
  {
    initAccumulators(nParticles, numBins, smcSmoothValuesMap_);
  }

  void HmmNormalLinear::smoothAccumulate(Size iter)
  {
	  accumulate(iter, smcSmoothValuesMap_, "Smoothing");
  }

}
