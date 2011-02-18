//                                               -*- C++ -*-
/*! \file BiipsTest.cpp
* \brief
*
* \author  $LastChangedBy$
* \date    $LastChangedDate$
* \version $LastChangedRevision$
* Id:      $Id$
*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BiipsTest
#include <boost/test/unit_test.hpp>

#include "include_macro.hpp"
#include "common/Types.hpp"
#include "TestIO.hpp"
#include "ModelTest.hpp"
#include "HmmNormalLinear.hpp"
#include "HmmNormalNonLinear.hpp"
#include "HmmMNormalLinear.hpp"
#include "HmmMNormalLinear4D.hpp"

#include <iostream>
#include <fstream>
#include <ctime>
#include <map>

#include <boost/program_options.hpp>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/p_square_quantile.hpp>

#include "kolmogorov.hpp"

namespace po = boost::program_options;

namespace Biips
{
  const String BIIPSTEST_CONFIG_FILE_NAME = "biipstest.cfg";
  const String BIIPSTEST_VERSION = "BiipsTest, version 0.1";
}


void store_unregistered(const std::vector<po::parsed_options> & parsed_sources,
    const std::vector<std::string> & sources_names,
    std::map<std::string, Biips::DataType> & model_map,
    std::map<std::string, Biips::DimArray::Ptr> & dim_map,
    std::map<std::string, std::vector<Biips::DataType> > & data_map,
    std::map<std::string, std::vector<Biips::DataType> > & bench_filter_map,
    std::map<std::string, std::vector<Biips::DataType> > & bench_smooth_map,
    std::map<std::string, std::map<Biips::Size, std::vector<Biips::Scalar> > > & errors_filter_map,
    std::map<std::string, std::map<Biips::Size, std::vector<Biips::Scalar> > > & errors_smooth_map);


BOOST_AUTO_TEST_CASE( my_test )
{
  int argc = boost::unit_test::framework::master_test_suite().argc;
  char ** argv = boost::unit_test::framework::master_test_suite().argv;
  using namespace std;
  using namespace Biips;

#ifdef CATCH_BIIPS_EXCEPTIONS
  try
  {
#endif
    Size model_id;
    String model_file_name;
    Size exec_step;
    String check_mode;
    Size show_mode;
    Size data_rng_seed;
    Size smc_rng_seed;
    vector<Size> n_particles;
    Scalar ess_threshold;
    Size resample_type;
    Size n_smc;
    Scalar reject_level;
    String plot_file_name;
    String config_file_name;
    vector<String> mutations;
    Size verbosity;

    std::map<Size, ResampleType> resample_type_map;
    resample_type_map[0] = SMC_RESAMPLE_MULTINOMIAL;
    resample_type_map[1] = SMC_RESAMPLE_RESIDUAL;
    resample_type_map[2] = SMC_RESAMPLE_STRATIFIED;
    resample_type_map[3] = SMC_RESAMPLE_SYSTEMATIC;

    std::map<Size, String> resample_type_name_map;
    resample_type_name_map[0] = "MULTINOMIAL";
    resample_type_name_map[1] = "RESIDUAL";
    resample_type_name_map[2] = "STRATIFIED";
    resample_type_name_map[3] = "SYSTEMATIC";

    // Declare a group of options that will be
    // allowed only on command line
    po::options_description generic("Generic options");
    generic.add_options()
        ("help-test,h", "produces help message.")
        ("version", "prints version string.")
        ("verbose,v", po::value<Size>(&verbosity)->default_value(1), "verbosity level.\n"
            "values:\n"
            " 0: \tnone.\n"
            " 1: \tminimal.\n"
            " 2: \thigh.")
        ("interactive,i", "asks questions to the user.\n"
            "applies when verbose>0.")
        ("show-plots,s", po::value<Size>(&show_mode)->default_value(0),"shows plots, interrupting execution.\n"
            "applies when n-smc=1.\n"
            "values:\n"
            " 0: \tno plots are shown.\n"
            " 1: \tfinal results plots are shown.\n"
            " 2: \tall plots are shown.")
        ("step", po::value<Size>(&exec_step)->default_value(3), "execution step to be reached (if possible).\n"
            "values:\n"
            " 0: \tsamples or reads values of the graph.\n"
            " 1: \tcomputes or reads benchmark values.\n"
            " 2: \truns SMC sampler, computes estimates and errors vs benchmarks.\n"
            " 3: \tchecks that errors vs benchmarks are distributed according to reference SMC errors, when n-smc>1. checks that error is lesser than a 1-(reject-level) quantile of the reference SMC errors, when n-smc=1.")
        ("check-mode", po::value<String>(&check_mode)->default_value("filter"), "errors to be checked.\n"
            "values:\n"
            " filter: \tchecks filtering errors only.\n"
            " smooth: \tchecks smoothing errors only.\n"
            " all: \tchecks both.")
        ;

    // Declare a group of options that will be
    // allowed both on command line and in
    // config file
    po::options_description config("Configuration");
    config.add_options()
        ("model-id", po::value<Size>(&model_id), "model identifier.\n"
            "values:\n"
            " 1: \tHMM Normal linear 1D.\n"
            " 2: \tHMM Normal non linear 1D.\n"
            " 3: \tHMM multivariate Normal linear.\n"
            " 4: \tHMM multivariate Normal linear 4D.")
        ("data-rng-seed", po::value<Size>(&data_rng_seed), "data sampler rng seed. default=time().")
        ("resampling", po::value<Size>(&resample_type)->default_value(2), "resampling method.\n"
            "values:\n"
            " 0: \tMULTINOMIAL.\n"
            " 1: \tRESIDUAL.\n"
            " 2: \tSTRATIFIED.\n"
            " 3: \tSYSTEMATIC.")
        ("ess-threshold", po::value<Scalar>(&ess_threshold)->default_value(0.5), "ESS resampling threshold.")
        ("particles", po::value<vector<Size> >(&n_particles)->default_value(vector<Size>(1, 1000)), "numbers of particles.")
        ("mutations", po::value<vector<String> >(&mutations)->default_value(vector<String>(1, "optimal")), "type of exploration used in the mutation step of the SMC algorithm.\n"
            "values:\n"
            " optimal: \tuse optimal mutation, when possible.\n"
            " prior: \tuse prior mutation.\n"
            " <other>: \tpossible model-specific mutation.")
        ("n-smc", po::value<Size>(&n_smc)->default_value(1), "number of independent SMC executions for each mutation and number of particles.")
        ("smc-rng-seed", po::value<Size>(&smc_rng_seed), "SMC sampler rng seed. default=time().\n"
            "applies when n-smc=1.")
        ("prec-param", "uses precision parameter instead of variance for normal distributions.")
        ("reject-level", po::value<Scalar>(&reject_level)->default_value(0.01), "accepted level of rejection in checks.")
        ("plot-file", po::value<String>(&plot_file_name), "plots pdf file name.\n"
            "applies when n-smc=1.")
        ;

    // Hidden options, will be allowed both on command line and
    // in config file, but will not be show to the user.
    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("cfg", po::value<String>(&config_file_name)->default_value(BIIPSTEST_CONFIG_FILE_NAME), "configuration file name.")
        ;

    po::options_description cmdline_options;
    cmdline_options.add(generic).add(config).add(hidden);

    po::options_description config_file_options;
    config_file_options.add(config).add(hidden);

    po::options_description visible(
        "\nUsage: BiipsTest [OPTION]... --model-id=<id> [OPTION]...\n"
        "runs test <id> with default model parameters, with multiple OPTION parameters.\n"
        "       BiipsTest [OPTION]... <CONFIG_FILE> [OPTION]...\n"
        "runs test configuration from file <CONFIG_FILE>, with multiple OPTION parameters.\n"
        "\n"
        "Allowed options");
    visible.add(generic).add(config);

    po::positional_options_description pos_desc;
    pos_desc.add("cfg", 1);

    // map of parsed options from different sources
    vector<po::parsed_options> parsed_sources;
    vector<string> sources_names;

    // Command line options parsing
    po::variables_map vm;

    {
      parsed_sources.push_back(po::command_line_parser(argc, argv).
          options(cmdline_options).positional(pos_desc).run());
      sources_names.push_back("command-line");

      const po::parsed_options & parsed = parsed_sources.back();

      store(parsed, vm);
      notify(vm);

      //      if (parsed.options.size()>0)
      //        cout << "[Command line options]" << endl;
      //      for (Size op=0; op<parsed.options.size(); ++op)
      //      {
      //        cout << parsed.options[op].string_key;
      //        if (!parsed.options[op].value.empty())
      //          cout << " = ";
      //        for (Size v=0; v<parsed.options[op].value.size(); ++v)
      //          cout << parsed.options[op].value[v] << " ";
      //        cout << endl;
      //      }
    }

    // Program options pre-processing
    if (vm.count("help-test"))
    {
      cout << visible << "\n";
    }

    else if (vm.count("version"))
    {
      cout << BIIPSTEST_VERSION << endl;
    }

    else
    {
      // Configuration file options parsing
      ifstream ifs(config_file_name.c_str());
      parsed_sources.push_back(po::parse_config_file(ifs, config_file_options, true));
      sources_names.push_back(config_file_name);

      const po::parsed_options & parsed = parsed_sources.back();

      store(parsed, vm);
      notify(vm);

      //      if (parsed.options.size()>0)
      //        cout << "[Configuration options from file " << config_file_name << "]" << endl;
      //      for (Size op=0; op<parsed.options.size(); ++op)
      //      {
      //        cout << parsed.options[op].string_key << " = ";
      //        for (Size v=0; v<parsed.options[op].value.size(); ++v)
      //          cout<< parsed.options[op].value[v] << endl;
      //      }


      // Store unregistered
      map<string, DataType> model_map;
      map<string, DimArray::Ptr> dim_map;
      map<string, vector<DataType> > data_map;
      map<string, vector<DataType> > bench_filter_map;
      map<string, vector<DataType> > bench_smooth_map;
      map<string, map<Size, vector<Scalar> > > errors_filter_ref_map;
      map<string, map<Size, vector<Scalar> > > errors_smooth_ref_map;

      store_unregistered(parsed_sources, sources_names, model_map, dim_map, data_map, bench_filter_map, bench_smooth_map, errors_filter_ref_map, errors_smooth_ref_map);

      Bool interactive = vm.count("interactive");
      Bool prec_param = vm.count("prec-param");

      // Select test
      Types<ModelTest>::Ptr p_model_test;
      if (vm.count("model-id"))
      {
        switch (model_id)
        {
          case 0:
            break;
          case 1:
            p_model_test = Types<ModelTest>::Ptr(new HmmNormalLinear(argc, argv, verbosity, show_mode, prec_param, cout));
            break;
          case 2:
            p_model_test = Types<ModelTest>::Ptr(new HmmNormalNonLinear(argc, argv, verbosity, show_mode, prec_param, cout));
            break;
          case 3:
            p_model_test = Types<ModelTest>::Ptr(new HmmMNormalLinear(argc, argv, verbosity, show_mode, prec_param, cout));
            break;
          case 4:
            p_model_test = Types<ModelTest>::Ptr(new HmmMNormalLinear4D(argc, argv, verbosity, show_mode, prec_param, cout));
            break;
          default:
            boost::throw_exception(po::error("ERROR: unknown model-id value."));
            break;
        }
      }
      else
      {
        boost::throw_exception(po::error("ERROR: missing model-id option."));
      }

      // Intro
      if (verbosity == 2)
        p_model_test->PrintIntro();
      else if (verbosity == 1)
        cout << "[" << p_model_test->Name() << "]" << endl;

      if (verbosity>0 && interactive)
        pressEnterToContinue();

      if (!model_map.empty())
      {
        // Input model parameters
        if (verbosity>0)
          cout << "Setting model parameters from configuration file..." << endl;
        p_model_test->SetModelParam(model_map);
      }

      if (verbosity>0 && interactive)
        pressEnterToContinue();

      if (!dim_map.empty())
      {
        if (verbosity>0)
          cout << "Setting data dimensions from configuration file..." << endl;
        p_model_test->SetDimensions(dim_map);
      }

      if (verbosity>0 && interactive)
        pressEnterToContinue();

      // Build data graph
      if (verbosity>0)
        cout << "Building data graph..." << endl;
      p_model_test->BuildDataGraph();

      if (verbosity>0)
      {
        cout << "\t" << "Graph nodes: " << p_model_test->DataGraphSize();
        cout << " (Constant: "<< p_model_test->DataNodesSummary().at("Constant");
        cout << ", Logical: "<< p_model_test->DataNodesSummary().at("Logical");
        cout << ", Stochastic: " << p_model_test->DataNodesSummary().at("Stochastic") << ")" << endl;
      }
      Size n_data_unobs_nodes = p_model_test->DataUnobsNodesSummary().at("Logical") + p_model_test->DataUnobsNodesSummary().at("Stochastic");

      if (verbosity>0)
      {
        cout << "\t" << "Unobserved nodes: " << n_data_unobs_nodes;
        cout << " (Logical: "<< p_model_test->DataUnobsNodesSummary().at("Logical");
        cout << ", Stochastic: " << p_model_test->DataUnobsNodesSummary().at("Stochastic") << ")" << endl;
      }

      if ( verbosity>0 && interactive && yesNoQuestion("Print data graph (y/n)? ", 'y', 'n', 1) )
      {
        p_model_test->PrintDataGraph();
        pressEnterToContinue();
      }

      // Sample data graph values
      if (data_map.empty())
      {
        if (!vm.count("data-rng-seed"))
          data_rng_seed = time(0);
        if (verbosity>0)
        {
          cout << "Sampling data graph values..." << endl;
          cout << "\t" << "rng seed: " << data_rng_seed << endl;
        }
        p_model_test->SampleData(data_rng_seed);
      }
      //        // Output data graph values
      //        if (vm.count("data-file-name"))
      //        {
      //          ofstream data_ofs(data_file_name.c_str());
      //          if (verbosity>0)
      //            cout << "Writing sampled data graph values in file: " << data_file_name << endl;
      //          p_model_test->OutputData(data_ofs);
      //        }
      else
      {
        if (verbosity>0)
          cout << "Setting data values from configuration file..." << endl;
        p_model_test->SetData(data_map);
      }

      if (verbosity>0 && interactive)
        pressEnterToContinue();

      // Run Benchmark
      if (exec_step >=1)
      {
        if (bench_filter_map.empty() && bench_smooth_map.empty())
        {
          if (verbosity>0)
            cout << "Generating benchmarks values..." << endl;
          p_model_test->RunBench();
        }
        //        // Output benchmark values
        //        if (vm.count("bench-file-name"))
        //        {
        //          ofstream bench_ofs(bench_file_name.c_str());
        //          if (verbosity>0)
        //            cout << "Writing benchmarks values in file: " << bench_file_name << endl;
        //          p_model_test->OutputBench(bench_ofs);
        //        }
        else
        {
          if (verbosity>0)
            cout << "Setting benchmarks values from configuration file..." << endl;
          p_model_test->SetBenchFilter(bench_filter_map);
          p_model_test->SetBenchSmooth(bench_smooth_map);
        }
      }

      if (verbosity>0 && interactive)
        pressEnterToContinue();

      if (exec_step >= 2)
      {
        // Build model graph
        if (verbosity>0)
          cout << "Building model graph..." << endl;
        p_model_test->BuildModelGraph();

        if (verbosity>0)
        {
          cout << "\t" << "Graph nodes: " << p_model_test->ModelGraphSize();
          cout << " (Constant: "<< p_model_test->ModelNodesSummary().at("Constant");
          cout << ", Logical: "<< p_model_test->ModelNodesSummary().at("Logical");
          cout << ", Stochastic: " << p_model_test->ModelNodesSummary().at("Stochastic") << ")" << endl;
        }
        Size n_model_unobs_nodes = p_model_test->ModelUnobsNodesSummary().at("Logical") + p_model_test->ModelUnobsNodesSummary().at("Stochastic");

        if (verbosity>0)
        {
          cout << "\t" << "Unobserved nodes: " << n_model_unobs_nodes;
          cout << " (Logical: "<< p_model_test->ModelUnobsNodesSummary().at("Logical");
          cout << ", Stochastic: " << p_model_test->ModelUnobsNodesSummary().at("Stochastic") << ")" << endl;
        }

        if (verbosity>0 && interactive && yesNoQuestion("Print model graph (y/n)? ", 'y', 'n', 1) )
        {
          p_model_test->PrintModelGraph();
          pressEnterToContinue();
        }

        if (verbosity>0)
        {
          cout << "Running SMC algorithms with" << endl;
          cout << "\t" << "resampling type: " << resample_type_name_map[resample_type] << ", threshold: " << ess_threshold << " ESS/N" << endl;
        }

        map<String, map<Size, vector<Scalar> > > errors_filter_new_map;
        map<String, map<Size, vector<Scalar> > > errors_smooth_new_map;
        // Run SMC sampler
        for (Size i_n_part=0; i_n_part<n_particles.size(); ++i_n_part)
        {
          Size n_part = n_particles[i_n_part];
          for (Size i_mut=0; i_mut<mutations.size(); ++i_mut)
          {
            string mut = mutations[i_mut];
            if (verbosity>0)
            {
              cout << "Running " << n_smc << " SMC algorithms of " << p_model_test->ModelUnobsNodesSummary().at("Stochastic") << " iterations with" << endl;
              cout << "\t" << "mutation: " << mut << ", particles: " << n_part << endl;
            }

            // compute 1-alpha quantile of reference errors distribution
            Bool errors_filter_ref_valid = errors_filter_ref_map.count(mut) && errors_filter_ref_map[mut].count(n_part);
            Scalar error_filter_threshold;

            Bool errors_smooth_ref_valid = errors_smooth_ref_map.count(mut) && errors_smooth_ref_map[mut].count(n_part);
            Scalar error_smooth_threshold;

            if (exec_step>=3 && n_smc==1)
            {

              using namespace acc;
              typedef accumulator_set<Scalar, features<tag::p_square_quantile> > acc_ref_type;

              if (check_mode=="filter" || check_mode=="all")
              {
                if (errors_filter_ref_valid)
                {
                  acc_ref_type errors_filter_ref_acc;

                  errors_filter_ref_acc = acc_ref_type(quantile_probability = 1-reject_level);
                  for (Size i =0; i<errors_filter_ref_map[mut][n_part].size(); ++i)
                    errors_filter_ref_acc(errors_filter_ref_map[mut][n_part][i]);
                  error_filter_threshold = p_square_quantile(errors_filter_ref_acc);
                }
              }

              if (check_mode=="smooth" || check_mode=="all")
              {
                if (errors_smooth_ref_valid)
                {
                  acc_ref_type errors_smooth_ref_acc;

                  errors_smooth_ref_acc = acc_ref_type(quantile_probability = 1-reject_level);
                  for (Size i =0; i<errors_smooth_ref_map[mut][n_part].size(); ++i)
                    errors_smooth_ref_acc(errors_smooth_ref_map[mut][n_part][i]);
                  error_smooth_threshold = p_square_quantile(errors_smooth_ref_acc);
                }
              }
            }

            Size pogress_pos;
            if (verbosity==1 && n_smc>1)
              pogress_pos = progressBar(0.0, 0, cout, toString(n_smc));

            for (Size i_smc=0; i_smc<n_smc; ++i_smc)
            {
              p_model_test->ClearSMC();

              if (n_smc>1 || !vm.count("smc-rng-seed"))
                smc_rng_seed = time(0)+i_smc+1;
              if (n_smc==1 && verbosity>0)
                cout << "\t" << "rng seed: " << smc_rng_seed << endl;

              p_model_test->RunSMC(n_part, smc_rng_seed, mut=="prior", ess_threshold, resample_type_map[resample_type], n_smc==1);

              if (verbosity==1 && n_smc>1)
                pogress_pos = progressBar(Scalar(i_smc+1)/n_smc, pogress_pos, cout);

              Scalar error_filter;
              if (p_model_test->ErrorFilter(error_filter))
              {
                error_filter *= n_part;
                if (n_smc==1)
                {
                  if (verbosity>0)
                    cout << "\t" << "filtering error = " << error_filter << endl;
                  if (exec_step>=3 && (check_mode=="all" || check_mode=="filter") && errors_filter_ref_valid)
                    BOOST_CHECK_LT(error_filter, error_filter_threshold);
                }
                else
                  errors_filter_new_map[mut][n_part].push_back(error_filter);
              }

              Scalar error_smooth;
              if (p_model_test->ErrorSmooth(error_smooth))
              {
                error_smooth *= n_part;
                if (n_smc==1)
                {
                  if (verbosity>0)
                    cout << "\t" << "smoothing error = " << error_smooth << endl;
                  if (exec_step>=3 && (check_mode=="all" || check_mode=="smooth") && errors_smooth_ref_valid)
                    BOOST_CHECK_LT(error_smooth, error_smooth_threshold);
                }
                else
                  errors_smooth_new_map[mut][n_part].push_back(error_smooth);
              }

              if (verbosity>0 && interactive)
                pressEnterToContinue();

              // Plot results
              if (n_smc==1 && (vm.count("plot-file-name") || show_mode >= 1) )
              {
                p_model_test->PlotResults(plot_file_name);
                if (verbosity>0 && vm.count("plot-file-name"))
                  cout << "Results plot written in file: " << plot_file_name << endl;
              }

              if (verbosity>0 && interactive)
                pressEnterToContinue();
            }

            if (exec_step >= 3 && n_smc>1)
            {
              if ((check_mode=="all" || check_mode=="filter") && errors_filter_ref_valid && !errors_filter_new_map[mut][n_part].empty())
              {
                if (verbosity>0)
                  cout << "Checking filtering errors fitness with reject level " << reject_level << endl;
                Scalar ks_filter_stat = ksTwoSamplesStat(errors_filter_new_map[mut][n_part].begin(),
                    errors_filter_new_map[mut][n_part].end(),
                    errors_filter_ref_map[mut][n_part].begin(),
                    errors_filter_ref_map[mut][n_part].end());

                Size n_err = errors_filter_new_map[mut][n_part].size();
                Size n_err_ref = errors_filter_ref_map[mut][n_part].size();
                Scalar ks_filter_prob = ksProb(sqrt(n_err*n_err_ref/(n_err+n_err_ref)) * ks_filter_stat);

                if (verbosity>0)
                  cout << "\t" << "Kolmogorov-Smirnov test: D = " << ks_filter_stat << ", p-value = " << ks_filter_prob << endl;

                BOOST_CHECK_GT(ks_filter_prob, reject_level);
              }

              if ((check_mode=="all" || check_mode=="smooth") && errors_smooth_ref_valid && !errors_smooth_new_map[mut][n_part].empty())
              {
                if (verbosity>0)
                  cout << "Checking smoothing errors fitness with reject level " << reject_level << endl;
                Scalar ks_smooth_stat = ksTwoSamplesStat(errors_smooth_new_map[mut][n_part].begin(),
                    errors_smooth_new_map[mut][n_part].end(),
                    errors_smooth_ref_map[mut][n_part].begin(),
                    errors_smooth_ref_map[mut][n_part].end());

                Size n_err = errors_smooth_new_map[mut][n_part].size();
                Size n_err_ref = errors_smooth_ref_map[mut][n_part].size();
                Scalar ks_smooth_prob = ksProb(sqrt(n_err*n_err_ref/(n_err+n_err_ref)) * ks_smooth_stat);

                if (verbosity>0)
                  cout << "\t" << "Kolmogorov-Smirnov test: D = " << ks_smooth_stat << ", p-value = " << ks_smooth_prob << endl;

                BOOST_CHECK_GT(ks_smooth_prob, reject_level);
              }
            }
          }
        }
      }
    }

#ifdef CATCH_BIIPS_EXCEPTIONS
  }
  BIIPS_CATCH
#endif
}
