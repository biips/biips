#include "config.hpp"
#include "HmmNormalNonLinear.hpp"
#include "BiipsCore.hpp"
#include "TestIO.hpp"

namespace Biips
{

  const String HmmNormalNonLinear::NAME_ = "HMM Normal non linear 1D";

  HmmNormalNonLinear::HmmNormalNonLinear(int argc,
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
    scalarParamMap_["var_x0"] = 5.0;
    scalarParamMap_["var_x"] = 10.0;
    scalarParamMap_["var_y"] = 1.0;

    DimArray::Ptr scalar_dim(new DimArray(1, 1));

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

  void HmmNormalNonLinear::PrintIntro()
  {
    using namespace std;
    cout << "Biips test : Hidden Markov Model non-linear gaussian 1D" << endl;
    cout << "=======================================================" << endl;
    cout << "x0 --> x[1] --> ... -->  x[t] --> ..." << endl;
    cout << "        |                 |" << endl;
    cout << "        v                 v" << endl;
    cout << "       y[1]     ...      y[t]     ..." << endl;
    cout << endl;
    cout << "           x0 ~ Normal(mean_x0, var_x0)" << endl;
    cout << "x[t] | x[t-1] ~ Normal(f(x[t-1], t), var_x) for all t>0" << endl;
    cout << "  y[t] | x[t] ~ Normal(g(x[t]), var_y)  for all t>0" << endl;
    cout << "where" << endl;
    cout << "  f(x, t) = 1/2*x + 25*x/(1+x^2) + 8*cos(1.2*t)" << endl;
    cout << "     g(x) = x^2/20" << endl;
    cout << endl;
  }

  void HmmNormalNonLinear::BuildModelGraph()
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
    NodeId
        a_f =
            pModelGraph_->AddConstantNode(MultiArray(P_SCALAR_DIM,
                                                     ValArray::Ptr(new ValArray(1,
                                                                                0.5))));
    NodeId
        b_f =
            pModelGraph_->AddConstantNode(MultiArray(P_SCALAR_DIM,
                                                     ValArray::Ptr(new ValArray(1,
                                                                                25.0))));
    NodeId
        c_f =
            pModelGraph_->AddConstantNode(MultiArray(P_SCALAR_DIM,
                                                     ValArray::Ptr(new ValArray(1,
                                                                                1.0))));
    NodeId
        d_f =
            pModelGraph_->AddConstantNode(MultiArray(P_SCALAR_DIM,
                                                     ValArray::Ptr(new ValArray(1,
                                                                                8.0))));
    NodeId
        e_f =
            pModelGraph_->AddConstantNode(MultiArray(P_SCALAR_DIM,
                                                     ValArray::Ptr(new ValArray(1,
                                                                                1.2))));
    NodeId
        a_g =
            pModelGraph_->AddConstantNode(MultiArray(P_SCALAR_DIM,
                                                     ValArray::Ptr(new ValArray(1,
                                                                                20.0))));
    NodeId
        expo =
            pModelGraph_->AddConstantNode(MultiArray(P_SCALAR_DIM,
                                                     ValArray::Ptr(new ValArray(1,
                                                                                2.0))));

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

    // create Logical nodeId collections
    //-----------------------------------
    NodeId pow_x0;
    Types<NodeId>::Array prod_a_f(t_max);
    Types<NodeId>::Array pow_x(t_max);
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

    Types<NodeId>::Array binary_params(2);
    Types<NodeId>::Array unary_param(1);
    Types<NodeId>::Array time(t_max);
    
    char distname[12];
    if (precFlag_) {
        strcpy(distname, "dnorm");
    } else {
        strcpy(distname, "dnormvar");
    }

    binary_params[0] = mean_x0;
    binary_params[1] = prec_or_var_x0;
    x0 = pModelGraph_->AddStochasticNode(distTab_[distname],
            binary_params,
            false);
    
    binary_params[0] = x0;
    binary_params[1] = expo;
    pow_x0 = pModelGraph_->AddLogicalNode(funcTab_["^"], binary_params);
    
    binary_params[0] = a_f;
    binary_params[1] = x0;
    prod_a_f[0] = pModelGraph_->AddLogicalNode(funcTab_["*"],
            binary_params);
    
    binary_params[0] = c_f;
    binary_params[1] = pow_x0;
    sum_c_f[0] = pModelGraph_->AddLogicalNode(funcTab_["+"],
            binary_params);
    
    binary_params[0] = x0;
    binary_params[1] = sum_c_f[0];
    div_f[0] = pModelGraph_->AddLogicalNode(funcTab_["/"],
            binary_params);
    
    binary_params[0] = b_f;
    binary_params[1] = div_f[0];
    prod_b_f[0] = pModelGraph_->AddLogicalNode(funcTab_["*"],
            binary_params);
    
    time[0]
            = pModelGraph_->AddConstantNode(MultiArray(P_SCALAR_DIM,
            ValArray::Ptr(new ValArray(1,
            Scalar(1)))));
    
    binary_params[0] = e_f;
    binary_params[1] = time[0];
    prod_e_f[0] = pModelGraph_->AddLogicalNode(funcTab_["*"],
            binary_params);
    
    unary_param[0] = prod_e_f[0];
    cos_f[0] = pModelGraph_->AddLogicalNode(funcTab_["cos"],
            unary_param);
    
    binary_params[0] = d_f;
    binary_params[1] = cos_f[0];
    prod_d_f[0] = pModelGraph_->AddLogicalNode(funcTab_["*"],
            binary_params);
    
    binary_params[0] = prod_a_f[0];
    binary_params[1] = prod_b_f[0];
    sum1_f[0] = pModelGraph_->AddLogicalNode(funcTab_["+"],
            binary_params);
    
    binary_params[0] = sum1_f[0];
    binary_params[1] = prod_d_f[0];
    f[0] = pModelGraph_->AddLogicalNode(funcTab_["+"], binary_params);
    
    binary_params[0] = f[0];
    binary_params[1] = prec_or_var_x;
    x[0] = pModelGraph_->AddStochasticNode(distTab_[distname],
            binary_params,
            false);
    
    binary_params[0] = x[0];
    binary_params[1] = expo;
    pow_x[0] = pModelGraph_->AddLogicalNode(funcTab_["^"], binary_params);
    
    binary_params[0] = pow_x[0];
    binary_params[1] = a_g;
    g[0] = pModelGraph_->AddLogicalNode(funcTab_["/"], binary_params);
    
    binary_params[0] = g[0];
    binary_params[1] = prec_or_var_y;
    y[0] = pModelGraph_->AddStochasticNode(distTab_[distname],
            binary_params,
            true);
    
    for (Size t = 1; t < t_max; ++t)
    {
        binary_params[0] = a_f;
        binary_params[1] = x[t - 1];
        prod_a_f[t] = pModelGraph_->AddLogicalNode(funcTab_["*"],
                binary_params);
        
        binary_params[0] = c_f;
        binary_params[1] = pow_x[t];
        sum_c_f[t] = pModelGraph_->AddLogicalNode(funcTab_["+"],
                binary_params);
        
        binary_params[0] = x[t];
        binary_params[1] = sum_c_f[t];
        div_f[t] = pModelGraph_->AddLogicalNode(funcTab_["/"],
                binary_params);
        
        binary_params[0] = b_f;
        binary_params[1] = div_f[t];
        prod_b_f[t] = pModelGraph_->AddLogicalNode(funcTab_["*"],
                binary_params);
        
        time[t]
                = pModelGraph_->AddConstantNode(MultiArray(P_SCALAR_DIM,
                ValArray::Ptr(new ValArray(1,
                Scalar(t+1)))));
        
        binary_params[0] = e_f;
        binary_params[1] = time[t];
        prod_e_f[t] = pModelGraph_->AddLogicalNode(funcTab_["*"],
                binary_params);
        
        unary_param[0] = prod_e_f[t];
        cos_f[t] = pModelGraph_->AddLogicalNode(funcTab_["cos"],
                unary_param);
        
        binary_params[0] = d_f;
        binary_params[1] = cos_f[t];
        prod_d_f[t] = pModelGraph_->AddLogicalNode(funcTab_["*"],
                binary_params);
        
        binary_params[0] = prod_a_f[t];
        binary_params[1] = prod_b_f[t];
        sum1_f[t] = pModelGraph_->AddLogicalNode(funcTab_["+"],
                binary_params);
        
        binary_params[0] = sum1_f[t];
        binary_params[1] = prod_d_f[t];
        f[t] = pModelGraph_->AddLogicalNode(funcTab_["+"], binary_params);
        
        binary_params[0] = f[t];
        binary_params[1] = prec_or_var_x;
        x[t] = pModelGraph_->AddStochasticNode(distTab_[distname],
                binary_params,
                false);
        
        binary_params[0] = x[t];
        binary_params[1] = expo;
        pow_x[t] = pModelGraph_->AddLogicalNode(funcTab_["^"], binary_params);
        
        binary_params[0] = pow_x[t];
        binary_params[1] = a_g;
        g[t] = pModelGraph_->AddLogicalNode(funcTab_["/"], binary_params);
        
        binary_params[0] = g[t];
        binary_params[1] = prec_or_var_y;
        y[t] = pModelGraph_->AddStochasticNode(distTab_[distname],
                binary_params,
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

  void HmmNormalNonLinear::initAccumulators(Size nParticles,
                                            Size numBins,
                                            std::map<String, MultiArray::Array> & statsValuesMap)
  {
    scalarAcc_.AddFeature(MEAN);
    scalarAcc_.AddFeature(VARIANCE);

    Scalar probs[] =
      { 0.05, 0.95 };
    quantAcc_ = QuantileAccumulator(probs, probs + sizeof(probs) / sizeof(probs[0]));
    densAcc_ = DensityAccumulator(floor(nParticles * 0.25), numBins);

    Size t_max = sizeParamMap_["t_max"];

    statsValuesMap["x"] = MultiArray::Array(t_max);
    statsValuesMap["x.var"] = MultiArray::Array(t_max);
    statsValuesMap["x.q05"] = MultiArray::Array(t_max);
    statsValuesMap["x.q95"] = MultiArray::Array(t_max);
  }

  void HmmNormalNonLinear::initFilterAccumulators(Size nParticles, Size numBins)
  {
    initAccumulators(nParticles, numBins, smcFilterValuesMap_);
  }

  void HmmNormalNonLinear::accumulate(Size t, std::map<String,
      MultiArray::Array> & statsValuesMap, const String & title)
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

  void HmmNormalNonLinear::filterAccumulate(Size iter)
  {
	  accumulate(iter, smcFilterValuesMap_, "Filtering");
  }

  void HmmNormalNonLinear::initSmoothAccumulators(Size nParticles, Size numBins)
  {
    initAccumulators(nParticles, numBins, smcSmoothValuesMap_);
  }

  void HmmNormalNonLinear::smoothAccumulate(Size iter)
  {
	  accumulate(iter, smcSmoothValuesMap_, "Smoothing");
  }

}
