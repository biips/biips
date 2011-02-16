//                                               -*- C++ -*-
/*! \file t_BetaBinomial.cpp
* \brief
*
* \author  $LastChangedBy$
* \date    $LastChangedDate$
* \version $LastChangedRevision$
* Id:      $Id$
*/

#include <fstream>

#include "t_BetaBinomial.hpp"
#include "BiipsBase.hpp"
#include "BiipsCore.hpp"
#include "TestIO.hpp"
#include "Plot.hpp"

using namespace Biips;
using namespace std;

const String data_folder = "data/";
const String results_folder = "results/";


void t_BetaBinomial(int argc, char* argv[])
{

#ifdef CATCH_BIIPS_EXCEPTIONS
  try
  {
#endif

    cout << endl;
    cout << "BiiPS test: Beta prior Binomial likelihood" << endl;
    cout << "==========================================" << endl;
    cout << "         p            " << endl;
    cout << " ________|____________" << endl;
    cout << " |    |        |      " << endl;
    cout << " v    v        v      " << endl;
    cout << "y[0] y[1] ... y[t] ..." << endl;
    cout << endl;
    cout << "    p ~ Beta(alpha, beta)" << endl;
    cout << "y | p ~ Bin(n, p)  for all t>=0" << endl;
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

    cout << "Input:" << endl;
    cout << "------" << endl;
    cout << "data file: " << data_file << endl;
    ifstream in_file_stream(data_file.c_str());
    cout << endl;

    Size t_max;
    Input(t_max, "Final time:", 1, in_file_stream);
    cout << "t_max = " << t_max << endl;
    cout << endl;

    Scalar alpha_val;
    Input(alpha_val, "Prior parameter alpha:", 1, in_file_stream);
    cout << "alpha = " << alpha_val << endl;
    cout << endl;

    Scalar beta_val;
    Input(beta_val, "Prior parameter beta:", 1, in_file_stream);
    cout << "beta = " << beta_val << endl;
    cout << endl;

    Size n_trials_val;
    Input(n_trials_val, "Number of trials:", 1, in_file_stream);
    cout << "n = " << n_trials_val << endl;
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
      cout << "Test :" << endl;
      cout << "------" << endl;

      // load Base module
      //-----------------
      loadBaseModule();

      // graph
      //------
      Graph graph;

      // create constant nodes
      //----------------------
      NodeId alpha = graph.AddConstantNode(DataType(alpha_val));
      NodeId beta = graph.AddConstantNode(DataType(beta_val));
      NodeId n_trials = graph.AddConstantNode(DataType(n_trials_val));

      // create Stochastic nodeId collections
      //-----------------------------------
      Types<NodeId>::Array y(t_max+1);

      // create Nodes
      //-------------
      DimArray::Ptr scalar_dim(new DimArray(2,1));
      Types<NodeId>::Array params(2);
      params[0] = alpha;
      params[1] = beta;
      NodeId p = graph.AddStochasticNode(scalar_dim, "dbeta", params);

      params[0] = n_trials;
      params[1] = p;
      for (Size t=0; t<t_max+1; ++t)
        y[t] = graph.AddStochasticNode(scalar_dim, "dbin", params, true);

      // build graph
      //------------
      cout << "The graph has a cycle ? " << graph.HasCycle() << endl;

      graph.Build();

      cout << endl;
      if ( promptFlag && YesNoQuestion("Print graph (y/n) ? ") )
      {
        graph.PrintGraph();

        if ( promptFlag )
          PressEnterToContinue();
      }

      // generate data
      //--------------
      Rng my_rng(rng_seed);

      NodeValues gen_values = graph.SampleValues(&my_rng);
      graph.SetObsValues(gen_values);

      Types<Scalar>::Array time(t_max+1);
      Types<Scalar>::Array y_obs(t_max+1);

      Scalar p_gen = gen_values[p]->ScalarView();

      for (Size t=0; t<t_max+1; ++t)
      {
        time[t] = t;
        y_obs[t] = gen_values[y[t]]->ScalarView();
      }

      cout << endl << "Generated success probability:" << endl;
      cout << "p = " << p_gen << endl;

      if ( promptFlag )
        PressEnterToContinue();

      cout << endl << "Generated number of successes observation:" << endl;
      matlabPrint(cout, y_obs, "y_obs");

      if ( promptFlag )
        PressEnterToContinue();


      // sampler
      //--------
      SMCSampler sampler(nb_particles, &graph, &my_rng);
      sampler.SetResampleParams(SMC_RESAMPLE_STRATIFIED, rs_threshold);

      if (! prior_mutation_flag)
        sampler.PushNodeSamplerFactory(ConjugateBetaFactory::Instance());

      sampler.Initialize();
      cout << endl;
      sampler.PrintSamplersSequence();

      if ( promptFlag )
        PressEnterToContinue();

      ScalarAccumulator stats_acc;
      stats_acc.AddFeature(MEAN);
      stats_acc.AddFeature(VARIANCE);
      stats_acc.AddFeature(QUANTILES);
      stats_acc.AddFeature(PDF);
      stats_acc.AddFeature(CDF);
      stats_acc.SetPdfParam(nb_particles/4, 20);
      stats_acc.SetCdfParam(50);

      //Scalar probs[] = {0.01,0.05, 0.1, 0.25, 0.33, 0.5, 0.67, 0.75, 0.9, 0.95, 0.99};
      Scalar probs[] = {0.05, 0.95};
      stats_acc.SetQuantileProbs(probs, probs + sizeof(probs) / sizeof(probs[0]));

      cout << endl << "SMC sampler's progress:" << endl;

      sampler.Iterate();
      sampler.PrintSamplerState();

      sampler.Accumulate(p, stats_acc);
      Scalar p_est = stats_acc.Mean();
      Scalar p_var = stats_acc.Variance();
      //    Scalar p_quant_01 = stats_acc.Quantile(0);
      Scalar p_quant_05 = stats_acc.Quantile(0);
      //    Scalar p_quant_10 = stats_acc.Quantile(2);
      //    Scalar p_quant_25 = stats_acc.Quantile(3);
      //    Scalar p_quant_33 = stats_acc.Quantile(4);
      //    Scalar p_quant_50 = stats_acc.Quantile(5);
      //    Scalar p_quant_67 = stats_acc.Quantile(6);
      //    Scalar p_quant_75 = stats_acc.Quantile(7);
      //    Scalar p_quant_90 = stats_acc.Quantile(8);
      Scalar p_quant_95 = stats_acc.Quantile(1);
      //    Scalar p_quant_99 = stats_acc.Quantile(10);

      if ( promptFlag )
        PressEnterToContinue();

      cout << endl << "SMC estimate E[p|y[0:t_max]]:" << endl;
      cout << "p_est = " << p_est << endl;

      cout << endl << "SMC variance Var[p|y[0:t_max]]:" << endl;
      cout << "p_var = " << p_var << endl;

      cout << endl << "SMC 5% quantile:" << endl;
      cout << "p_quant_05 = " << p_quant_05 << endl;
      cout << endl << "SMC 95% quantile:" << endl;
      cout << "p_quant_95 = " << p_quant_95 << endl;

      if ( promptFlag )
        PressEnterToContinue();

      //    // Qwt Plot : final time cdf estimate
      //    //-----------------------------------
      //    {
      //      Histogram cdf_hist = stats_acc.Cdf();
      //      Plot cdf_plot_PF(argc, argv);
      //      cdf_plot_PF.AddHistogram(cdf_hist.GetPositions(), cdf_hist.GetValues(), "", Qt::blue);
      //      cdf_plot_PF.SetTitle("Cdf estimates");
      //      cdf_plot_PF.SetBackgroundColor(Qt::white);
      //      if( show_graph_flag )
      //        cdf_plot_PF.Show();
      //    }

      // Qwt Plot : final time pdf estimate
      //-----------------------------------
      {
        ScalarHistogram pdf_hist = stats_acc.Pdf();
        Plot pdf_plot_PF(argc, argv);
        pdf_plot_PF.AddHistogram(pdf_hist, "", Qt::blue);
        pdf_plot_PF.SetTitle("Pdf estimates");
        pdf_plot_PF.SetBackgroundColor(Qt::white);
        if( show_graph_flag )
          pdf_plot_PF.Show();
      }

    }

    cout << "===================================================" << endl << endl;

#ifdef CATCH_BIIPS_EXCEPTIONS
  }
  BIIPS_CATCH
#endif

}
