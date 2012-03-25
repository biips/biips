//                                               -*- C++ -*-
/*! \file t_DirichletMultinomialSimplified.cpp
* \brief
*
* $LastChangedBy$
* $LastChangedDate$
* $LastChangedRevision$
* $Id$
*/


#include <fstream>

#include "t_DirichletMultinomialSimplified.hpp"
#include "BiipsCore.hpp"
#include "BiipsBase.hpp"
#include "TestIO.hpp"
#include "Plot.hpp"

using namespace Biips;
using namespace std;

const String data_folder = "data/";
const String results_folder = "results/";

void t_DirichletMultinomialSimplified(int argc, char* argv[])
{

#ifdef CATCH_BIIPS_EXCEPTIONS
  try
  {
#endif

    cout << endl;
    cout << "BiiPS test : Simplified Dirichlet Multinomial mixture model" << endl;
    cout << "===========================================================" << endl;
    cout << "Generation model:" << endl;
    cout << "         pi            " << endl;
    cout << " ________|__________   " << endl;
    cout << " |      |      |       " << endl;
    cout << " v      v      v       " << endl;
    cout << "C[1]   C[2]   C[3]  ..." << endl;
    cout << " |      |      |       " << endl;
    cout << " v      v      v       " << endl;
    cout << "Y[1]   Y[2]   Y[3]  ..." << endl;
    cout << endl;
    cout << "         C[t] ~ Categorical(pi)  for all t>0" << endl;
    cout << "Y[t] | C[t]=k ~ Normal(mu.Y[k], var.Y[k])  for all t>0" << endl;
    cout << endl;
    cout << "Estimation model:" << endl;
    cout << "We want to estimate pi knowing only Y[1:t_max] and mu.Y" << endl;
    cout << "      /------------------>    " << endl;
    cout << "     /     /------------->    " << endl;
    cout << "    /-----/----->             " << endl;
    cout << "C[1] --> C[2] --> C[3] --> ..." << endl;
    cout << " |\\       |\\       |\\__       " << endl;
    cout << " | \\      | \\______|___       " << endl;
    cout << " |  \\_____|_____\\__|___       " << endl;
    cout << " |      \\ |     \\\\ |          " << endl;
    cout << " v       vv      vvv          " << endl;
    cout << "Y[1] --> Y[2] --> Y[3] --> ..." << endl;
    cout << endl;
    cout << "                  C[t] | C[1:t-1] ~ Categorical(pi[:,t])  for all t>0" << endl;
    cout << "Y[t] | C[t]=k, Y[1:t-1], C[1:t-1] ~ Normal(mu.Y[k], var.Y[k,t])  for all t>0" << endl;
    cout << endl;
    cout << "where, for all k=1,...,K" << endl;
    cout << "    n[k,0] = 0" << endl;
    cout << "    n[k,t] = n[k,t-1] + (C[t]==k)  for all t>0" << endl;
    cout << endl;
    cout << "   pi[k,t] = (n[k,t-1] + alpha/K) / (t-1 + alpha)  for all t>1" << endl;
    cout << "var.Y[k,t] = sigma^2 / (sigma^2 + n[k,t-1])  for all t>1" << endl;
    cout << endl;

    // Input
    //======
    String data_file;
    if ( argc>2 )
      data_file = data_folder + argv[2];

    Bool promptFlag = true;
    if ( argc>3 )
      promptFlag =  !(strcmp(argv[3], "--noprompt") == 0);

    Bool show_graph_flag = true;
    if ( argc>4 )
      show_graph_flag =  !(strcmp(argv[4], "--noplot") == 0);

    Bool prior_mutation_flag = false;
    if ( argc>5 )
      prior_mutation_flag =  (strcmp(argv[5], "--prior") == 0);

    Bool precision_parameter_flag = false;
    if ( argc>6 )
      precision_parameter_flag =  (strcmp(argv[6], "--prec") == 0);

    cout << "Input:" << endl;
    cout << "------" << endl;
    cout << "data file: " << data_file << endl;
    ifstream in_file_stream(data_file.c_str());
    cout << endl;

    Size t_max;
    Input(t_max, "Final time:", 1, in_file_stream);
    cout << "t_max = " << t_max << endl;
    cout << endl;

    Size K_val;
    Input(K_val, "Number of categories:", 1, in_file_stream);
    cout << "K = " << K_val << endl;
    cout << endl;

    MultiArray pi_val(DimArray(1, K_val));
    InputMultiArray(pi_val, "Categories weights to be estimated:", 1, in_file_stream);
    cout << "pi = " << endl << pi_val << endl;
    cout << endl;

    MultiArray mu_k_val(DimArray(1, K_val));
    InputMultiArray(mu_k_val, "Cluster mean values:", 1, in_file_stream);
    cout << "mu_k = " << endl << mu_k_val << endl;
    cout << endl;

    MultiArray var_k_val(DimArray(1, K_val));
    InputMultiArray(var_k_val, "Cluster variance values:", 1, in_file_stream);
    cout << "var_k = " << endl << var_k_val << endl;
    cout << endl;

    MultiArray prec_k_val(DimArray(1, K_val));
    if (precision_parameter_flag)
      prec_k_val.Value() = 1.0 / var_k_val.Value();

    Scalar alpha_val;
    Input(alpha_val, "Alpha parameter value:", 1, in_file_stream);
    cout << "alpha = " << alpha_val << endl;
    cout << endl;

    Scalar sigma_val;
    Input(sigma_val, "Sigma parameter value:", 1, in_file_stream);
    cout << "sigma = " << sigma_val << endl;
    cout << endl;

    Size rng_seed;
    Input(rng_seed, "Random number generator seed:", 1, in_file_stream);
    cout << "rng_seed = " << rng_seed << endl;
    cout << endl;

    Size nb_particles;
    Input(nb_particles, "Number of particles:", 1, in_file_stream);
    cout << "N = " << nb_particles << endl;
    cout << endl;

    Scalar rs_threshold;
    Input(rs_threshold, "Resampling ESS/N threshold:", 1, in_file_stream);
    cout << "rs_threshold = " << rs_threshold << endl;
    cout << endl;

    String results_file;
    Input(results_file, "", 0, in_file_stream);
    results_file = results_folder + results_file;

    String plot_file;
    Input(plot_file, "", 0, in_file_stream);
    plot_file = results_folder + plot_file;

    in_file_stream.close();

    if ( !promptFlag || YesNoQuestion("Run this test (y/n) ? ") )
    {

      // Test
      //======
      cout << "Test:" << endl;
      cout << "-----" << endl;

      // load Base module
      //-----------------
      loadBaseModule();


      // Data generation graph
      //----------------------
      Graph data_graph;

      DimArray::Ptr scalar_dim(new DimArray(1,1));
      DimArray::Ptr k_vec_dim(new DimArray(1,K_val));

      // create constant nodes
      //----------------------
      NodeId mu_k = data_graph.AddConstantNode(mu_k_val);
      NodeId prec_or_var_k_gen;
      if (precision_parameter_flag)
        prec_or_var_k_gen = data_graph.AddConstantNode(prec_k_val);
      else
        prec_or_var_k_gen = data_graph.AddConstantNode(var_k_val);

      MultiArray k_data(k_vec_dim);
      for (Size i = 0; i < k_data.Length(); ++i)
        k_data.Value()[i] = Scalar(i+1);
      NodeId k_vec = data_graph.AddConstantNode(k_data);

      NodeId pi_gen = data_graph.AddConstantNode(pi_val);

      Types<NodeId>::Array binary_params(2);
      Types<NodeId>::Array unary_param(1);

      // create NodeId collections
      //--------------------------
      Types<NodeId>::Array C_equal_k(t_max);
      Types<NodeId>::Array prec_or_var_k_chosen(t_max);
      Types<NodeId>::Array prec_or_var_Y(t_max);
      Types<NodeId>::Array mu_k_chosen(t_max);
      Types<NodeId>::Array mu_Y(t_max);

      Types<NodeId>::Array C(t_max);
      Types<NodeId>::Array Y_gen(t_max);

      // create Nodes
      //-------------

      for (Size t=0; t<t_max; ++t)
      {
        // C computation
        //--------------
        unary_param[0] = pi_gen;
        C[t] = data_graph.AddStochasticNode(scalar_dim, "dcat", unary_param, false);

        binary_params[0] = C[t];
        binary_params[1] = k_vec;
        C_equal_k[t] = data_graph.AddLogicalNode(k_vec_dim, "==", binary_params);

        // var.Y computation
        //------------------
        binary_params[0] = C_equal_k[t];
        binary_params[1] = prec_or_var_k_gen;
        prec_or_var_k_chosen[t] = data_graph.AddLogicalNode(k_vec_dim, "*", binary_params);

        unary_param[0] = prec_or_var_k_chosen[t];
        prec_or_var_Y[t] = data_graph.AddLogicalNode(scalar_dim, "sum", unary_param);

        // mu.Y computation
        //-----------------
        binary_params[0] = C_equal_k[t];
        binary_params[1] = mu_k;
        mu_k_chosen[t] = data_graph.AddLogicalNode(k_vec_dim, "*", binary_params);

        unary_param[0] = mu_k_chosen[t];
        mu_Y[t] = data_graph.AddLogicalNode(scalar_dim, "sum", unary_param);

        // Y computation
        //--------------
        binary_params[0] = mu_Y[t];
        binary_params[1] = prec_or_var_Y[t];
        if (precision_parameter_flag)
          Y_gen[t] = data_graph.AddStochasticNode(scalar_dim, "dnorm", binary_params, true);
        else
          Y_gen[t] = data_graph.AddStochasticNode(scalar_dim, "dnormvar", binary_params, true);
      }

      // build graph
      //------------
      cout << "The graph has a cycle ? " << data_graph.HasCycle() << endl;

      data_graph.Build();

      cout << endl;
      if ( promptFlag && YesNoQuestion("Print data graph (y/n) ? ", 'y', 'n', 1) )
      {
        data_graph.PrintGraph();

        if ( promptFlag )
          PressEnterToContinue();
      }

      // generate data
      //--------------
      Rng my_rng(rng_seed);

      NodeValues gen_values = data_graph.SampleValues(&my_rng);

      Types<Scalar>::Array time_vec(t_max);
      Types<Scalar>::Array c_gen(t_max);
      Types<Scalar>::Array y_obs(t_max);

      for (Size t=0; t<t_max; ++t)
      {
        time_vec[t] = Scalar(t+1);
        c_gen[t] = gen_values[C[t]]->ScalarView();
        y_obs[t] = gen_values[Y_gen[t]]->ScalarView();
      }

      if ( promptFlag )
        PressEnterToContinue();

      cout << endl << "Generated categories:" << endl;
      matlabPrint(cout, c_gen, "c_gen");

      if ( promptFlag )
        PressEnterToContinue();

      cout << endl << "Generated observations:" << endl;
      matlabPrint(cout, y_obs, "y_obs");

      if ( promptFlag )
        PressEnterToContinue();


      // Model graph
      //------------
      Graph graph;

      // create constant nodes
      //----------------------
      NodeId K_inv = graph.AddConstantNode(MultiArray(Scalar(1.0 / K_val)));
      mu_k = graph.AddConstantNode(mu_k_val);
      NodeId alpha = graph.AddConstantNode(MultiArray(alpha_val));
      NodeId sigma = graph.AddConstantNode(MultiArray(sigma_val));
      NodeId expo = graph.AddConstantNode(MultiArray(2.0));

      k_vec = graph.AddConstantNode(k_data);

      NodeId one = 0;
      if (precision_parameter_flag)
        one = graph.AddConstantNode(MultiArray(Scalar(1.0)));

      // create NodeId collections
      //--------------------------
      Types<NodeId>::Array time(t_max+1);
      Types<NodeId>::Array sum_time_alpha(t_max+1);
      Types<NodeId>::Array n_k(t_max+1);
      Types<NodeId>::Array sum_n_alpha_k(t_max+1);
      Types<NodeId>::Array pi_k(t_max+1);
      Types<NodeId>::Array sum_n_sigma_k(t_max);
      Types<NodeId>::Array div_sigma_sum_k(t_max);
      Types<NodeId>::Array var_k(t_max);
      Types<NodeId>::Array var_k_chosen(t_max);
      Types<NodeId>::Array var_Y(t_max);
      Types<NodeId>::Array prec_Y(t_max);

      Types<NodeId>::Array Y(t_max);

      // create Nodes
      //-------------
      binary_params[0] = alpha;
      binary_params[1] = K_inv;
      NodeId alpha_over_K = graph.AddLogicalNode(scalar_dim, "*", binary_params);

      binary_params[0] = sigma;
      binary_params[1] = expo;
      NodeId sigma_sq = graph.AddLogicalNode(scalar_dim, "^", binary_params);

      // n initialization
      //-----------------
      n_k[0] = graph.AddConstantNode(MultiArray(k_vec_dim));

      for (Size t=0; t<t_max; ++t)
      {
        time[t] = graph.AddConstantNode(MultiArray(Scalar(t)));

        binary_params[0] = time[t];
        binary_params[1] = alpha;
        sum_time_alpha[t] = graph.AddLogicalNode(scalar_dim, "+", binary_params);

        if (t>0)
        {
          // n computation
          //--------------
          binary_params[0] = n_k[t-1];
          binary_params[1] = C_equal_k[t-1];
          n_k[t] = graph.AddLogicalNode(k_vec_dim, "+", binary_params);
        }
        // pi computation
        //---------------
        binary_params[0] = n_k[t];
        binary_params[1] = alpha_over_K;
        sum_n_alpha_k[t] = graph.AddLogicalNode(k_vec_dim, "+", binary_params);

        binary_params[0] = sum_n_alpha_k[t];
        binary_params[1] = sum_time_alpha[t];
        pi_k[t] = graph.AddLogicalNode(k_vec_dim, "/", binary_params);

        // C computation
        //--------------
        unary_param[0] = pi_k[t];
        C[t] = graph.AddStochasticNode(scalar_dim, "dcat", unary_param, false);

        binary_params[0] = C[t];
        binary_params[1] = k_vec;
        C_equal_k[t] = graph.AddLogicalNode(k_vec_dim, "==", binary_params);

        // var.Y computation
        //------------------
        binary_params[0] = sigma_sq;
        binary_params[1] = n_k[t];
        sum_n_sigma_k[t] = graph.AddLogicalNode(k_vec_dim, "+", binary_params);

        binary_params[0] = sigma_sq;
        binary_params[1] = sum_n_sigma_k[t];
        div_sigma_sum_k[t] = graph.AddLogicalNode(k_vec_dim, "/", binary_params);

        binary_params[0] = sigma_sq;
        binary_params[1] = div_sigma_sum_k[t];
        var_k[t] = graph.AddLogicalNode(k_vec_dim, "+", binary_params);

        binary_params[0] = C_equal_k[t];
        binary_params[1] = var_k[t];
        var_k_chosen[t] = graph.AddLogicalNode(k_vec_dim, "*", binary_params);

        unary_param[0] = var_k_chosen[t];
        var_Y[t] = graph.AddLogicalNode(scalar_dim, "sum", unary_param);

        if (precision_parameter_flag)
        {
          binary_params[0] = one;
          binary_params[1] = var_Y[t];
          prec_Y[t] = graph.AddLogicalNode(scalar_dim, "/", binary_params);
        }

        // mu.Y computation
        //-----------------
        binary_params[0] = C_equal_k[t];
        binary_params[1] = mu_k;
        mu_k_chosen[t] = graph.AddLogicalNode(k_vec_dim, "*", binary_params);

        unary_param[0] = mu_k_chosen[t];
        mu_Y[t] = graph.AddLogicalNode(scalar_dim, "sum", unary_param);

        // Y computation
        //--------------
        if (precision_parameter_flag)
        {
          binary_params[0] = mu_Y[t];
          binary_params[1] = prec_Y[t];
          Y[t] = graph.AddStochasticNode(scalar_dim, "dnorm", binary_params, true);
        }
        else
        {
          binary_params[0] = mu_Y[t];
          binary_params[1] = var_Y[t];
          Y[t] = graph.AddStochasticNode(scalar_dim, "dnormvar", binary_params, true);
        }
      }

      // t = t_max
      //----------
      time[t_max] = graph.AddConstantNode(MultiArray(Scalar(t_max)));

      binary_params[0] = time[t_max];
      binary_params[1] = alpha;
      sum_time_alpha[t_max] = graph.AddLogicalNode(scalar_dim, "+", binary_params);

      // n computation
      //--------------
      binary_params[0] = n_k[t_max-1];
      binary_params[1] = C_equal_k[t_max-1];
      n_k[t_max] = graph.AddLogicalNode(k_vec_dim, "+", binary_params);

      // pi computation
      //---------------
      binary_params[0] = n_k[t_max];
      binary_params[1] = alpha_over_K;
      sum_n_alpha_k[t_max] = graph.AddLogicalNode(k_vec_dim, "+", binary_params);

      binary_params[0] = sum_n_alpha_k[t_max];
      binary_params[1] = sum_time_alpha[t_max];
      pi_k[t_max] = graph.AddLogicalNode(k_vec_dim, "/", binary_params);


      // build graph
      //------------
      cout << "The graph has a cycle ? " << graph.HasCycle() << endl;

      graph.Build();

      cout << endl;
      if ( promptFlag && YesNoQuestion("Print model graph (y/n) ? ", 'y', 'n', 1) )
      {
        graph.PrintGraph();

        if ( promptFlag )
          PressEnterToContinue();
      }

      NodeValues obs_values(graph.GetSize());
      for (Size t = 0; t<t_max; ++t)
      {
        obs_values[Y[t]] = gen_values[Y_gen[t]];
      }

      graph.SetObsValues(obs_values);

      // sampler
      //--------
      ForwardSampler sampler(nb_particles, &graph, &my_rng);
      sampler.SetResampleParams(SMC_RESAMPLE_STRATIFIED, rs_threshold);

      if (! prior_mutation_flag)
      {
        if (precision_parameter_flag)
        {
          sampler.PushNodeSamplerFactory(ConjugateNormalLinearFactory::Instance());
          sampler.PushNodeSamplerFactory(DiscreteBruteOptimalFactory::Instance());
        }
        else
        {
          sampler.PushNodeSamplerFactory(ConjugateNormalVarLinearFactory::Instance());
          sampler.PushNodeSamplerFactory(DiscreteBruteOptimalFactory::Instance());
        }
      }

      sampler.Initialize();
      cout << endl;
      sampler.PrintSamplersSequence();

      if ( promptFlag )
        PressEnterToContinue();

#ifndef BIIPS_VALGRIND_ON
      DiscreteAccumulator stats_acc;
      stats_acc.AddFeature(DENSITY);
      stats_acc.AddFeature(MAX_PDF);

      Types<Scalar>::Array c_est_PF(t_max);
#endif

      cout << endl << "SMC sampler's progress:" << endl;

      for (Size t=0; t<t_max; ++t)
      {
        sampler.Iterate();
        sampler.PrintSamplerState();

#ifndef BIIPS_VALGRIND_ON
        // Filtering estimates
        //-------------------
        sampler.Accumulate(C[t], stats_acc);
        c_est_PF[t] = stats_acc.MaxPdf();

        // Qwt Plot : filtering pdf estimate
        //----------------------------------
        {
          Plot pdf_plot_PF(argc, argv);
          pdf_plot_PF.AddHistogram(stats_acc.Pdf(), "", Qt::blue);
          pdf_plot_PF.SetTitle("PF C Histogram, t = "+ToString(t));
          pdf_plot_PF.SetBackgroundColor(Qt::white);
          if( show_graph_flag )
            pdf_plot_PF.Show();
        }
#endif
      }

#ifndef BIIPS_VALGRIND_ON
      if ( promptFlag )
        PressEnterToContinue();

      cout << endl << "Particle Filter C(t|1:t) MAP estimate:" << endl;
      matlabPrint(cout, c_est_PF, "c_est_PF");

      if ( promptFlag )
        PressEnterToContinue();

      ArrayAccumulator stats_pi_acc;
      stats_pi_acc.AddFeature(MEAN);
      sampler.Accumulate(pi_k[t_max], stats_pi_acc);
      Types<Scalar>::Array pi_est_PF(K_val);
      for (Size k=0; k<K_val; ++k)
      {
        pi_est_PF[k] = stats_pi_acc.Mean().Value()[k];
      }
      cout << endl << "Particle Filter MMSE estimate E[pi(t_max|1:t_max)]:" << endl;
      matlabPrint(cout, pi_est_PF, "pi_est_PF");
#endif

    }

    cout << "===================================================" << endl << endl;

#ifdef CATCH_BIIPS_EXCEPTIONS
  }
  BIIPS_CATCH
#endif

}
