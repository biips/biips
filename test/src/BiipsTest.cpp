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

/*! \file BiipsTest.cpp
* \brief
*
* \author  $LastChangedBy$
* \date    $LastChangedDate$
* \version $LastChangedRevision$
* Id:      $Id$
*/

#define BOOST_TEST_MODULE BiipsTest
#include <boost/test/included/unit_test.hpp>

#include "config.hpp"
#include "BiipsVersion.hpp"
#include "storeUnregistered.hpp"
#include "TestIO.hpp"
#include "ModelTest.hpp"
#include "HmmNormalLinear.hpp"
#include "HmmNormalNonLinear.hpp"
#include "HmmMNormalLinear.hpp"
#include "HmmMNormalLinear4D.hpp"
#include "kolmogorov.hpp"

#include <iostream>
#include <fstream>
#include <ctime>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/p_square_quantile.hpp>

#include <boost/math/distributions/students_t.hpp>

#include <boost/progress.hpp>

namespace Biips
{
  const String BIIPSTEST_CONFIG_FILE_NAME = "biipstest.cfg";

  const Size INDENT_SIZE = 2;
  const String INDENT_STRING(INDENT_SIZE, ' ');
}


BOOST_AUTO_TEST_CASE( my_test )
{
  int argc = boost::unit_test::framework::master_test_suite().argc;
  char ** argv = boost::unit_test::framework::master_test_suite().argv;
  using namespace std;
  using namespace Biips;

#ifdef BIIPS_CATCH_TEST
  try
  {
#endif
    Size model_id;
    Size exec_step;
    Size check_mode;
    Size data_rng_seed;
    Size smc_rng_seed;
    vector<Size> n_particles;
    Scalar ess_threshold;
    String resample_type;
    Size n_smc;
    Scalar reject_level;
    String config_file_name;
    vector<String> mutations;
    Size verbosity;
    Size num_bins;
    Size show_mode = 0;
#ifdef USE_Qwt5_Qt4
    String plot_file_name;
#endif //USE_Qwt5_Qt4

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
        ("interactive", "asks questions to the user.\n"
            "applies when verbose>0.")
        ("num-bins", po::value<Size>(&num_bins)->default_value(40), "number of bins in the histograms.")
        ("step", po::value<Size>(&exec_step)->default_value(3), "execution step to be reached (if possible).\n"
            "values:\n"
            " 0: \tsamples or reads values of the graph.\n"
            " 1: \t0 + computes or reads benchmark values.\n"
            " 2: \t1 + runs SMC sampler, computes estimates and errors vs benchmarks.\n"
            " 3: \t2 + checks that errors vs benchmarks are distributed according to reference SMC errors, when repeat-smc>1. checks that error is lesser than a 1-alpha quantile of the reference SMC errors, when repeat-smc=1.")
        ("check-mode", po::value<Size>(&check_mode)->default_value(1), "errors to be checked.\n"
            "values:\n"
            " 0: \tchecks normalizing-constant mean.\n"
            " 1: \t0 + checks filtering errors goodness of fit.\n"
            " 2: \t1 + checks smoothing errors goodness of fit.")
#ifdef USE_Qwt5_Qt4
        ("show-plots,s", po::value<Size>(&show_mode)->default_value(0),"shows plots, interrupting execution.\n"
            "applies when repeat-smc=1.\n"
            "values:\n"
            " 0: \tno plots.\n"
            " 1: \tshows final results plots.\n"
            " 2: \t1 + shows pdf histogram plots.")
#endif //USE_Qwt5_Qt4
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
        ("data-rng-seed", po::value<Size>(&data_rng_seed)->default_value(clock()), "data sampler rng seed. default=time().")
        ("resampling", po::value<String>(&resample_type)->default_value("stratified"), "resampling method.\n"
            "values:\n"
            " multinomial\n"
            " residual\n"
            " stratified\n"
            " systematic")
        ("ess-threshold", po::value<Scalar>(&ess_threshold)->default_value(0.5), "ESS resampling threshold.")
        ("particles", po::value<vector<Size> >(&n_particles)->default_value(vector<Size>(1, 1000)), "numbers of particles.")
        ("mutations", po::value<vector<String> >(&mutations)->default_value(vector<String>(1, "optimal")), "type of exploration used in the mutation step of the SMC algorithm.\n"
            "values:\n"
            " optimal: \tuse optimal mutation, when possible.\n"
            " prior: \tuse prior mutation.\n"
            " <other>: \tpossible model-specific mutation.")
        ("repeat-smc", po::value<Size>(&n_smc)->default_value(1), "number of independent SMC executions for each mutation and number of particles.")
        ("smc-rng-seed", po::value<Size>(&smc_rng_seed)->default_value(clock()), "SMC sampler rng seed. default=time().\n"
            "applies when repeat-smc=1.")
        ("prec-param", "uses precision parameter instead of variance for normal distributions.")
        ("alpha", po::value<Scalar>(&reject_level)->default_value(1e-4), "accepted level of rejection in checks.")
#ifdef USE_Qwt5_Qt4
        ("plot-file", po::value<String>(&plot_file_name), "plots pdf file name.\n"
            "applies when repeat-smc=1.")
#endif //USE_Qwt5_Qt4
        ;

    // Hidden options, will be allowed both on command line and
    // in config file, but will not be shown to the user.
    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("cfg", po::value<String>(&config_file_name)->default_value(BIIPSTEST_CONFIG_FILE_NAME), "configuration file name.")
        ;

    po::options_description cmdline_options;
    cmdline_options.add(generic).add(config).add(hidden);

    po::options_description config_file_options;
    config_file_options.add(config).add(hidden);

    po::options_description visible(
        "\nUsage: BiipsTest [<option>]... --model-id=<id> [<option>]...\n"
        "  runs model <id> with default model parameters, with multiple <option> parameters.\n"
        "       BiipsTest [<option>]... <config_file> [<option>]...\n"
        "  runs test configuration from file <config_file>, with multiple <option> parameters.\n"
        "\n"
        "Options, denoted here by <option>, are parsed from the general syntax:"
        "     --<long_key>[=<value>]\n"
        "  or -<short_key>[=<value>]\n"
        "where the bracketed text [...] is optional\n"
        "and the angle bracketed text <...> must be replaced by the corresponding value.\n"
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

#ifdef BIIPS_DEBUG
      if (parsed.options.size()>0)
        cout << "[Command line options]" << endl;
      for (Size op=0; op<parsed.options.size(); ++op)
      {
        cout << parsed.options[op].string_key;
        if (!parsed.options[op].value.empty())
          cout << " = ";
        for (Size v=0; v<parsed.options[op].value.size(); ++v)
          cout << parsed.options[op].value[v] << " ";
        cout << endl;
      }
#endif
    }

    // Program options pre-processing
    if (vm.count("help-test"))
    {
      cout << visible << "\n";
      return;
    }

    if (vm.count("version"))
    {
      cout << "BiipsTest, version " << BIIPS_VERSION() << endl;
      return;
    }

    // Configuration file options parsing
    if (vm.count("cfg"))
    {
      ifstream ifs(config_file_name.c_str());
      if (ifs.fail())
        boost::throw_exception(po::error(String("Failed not open file: ")+config_file_name));
      parsed_sources.push_back(po::parse_config_file(ifs, config_file_options, true));
      sources_names.push_back(config_file_name);

      const po::parsed_options & parsed = parsed_sources.back();

#ifdef BIIPS_DEBUG
      if (parsed.options.size()>0)
      cout << "[Configuration options from file " << config_file_name << "]" << endl;
      for (Size op=0; op<parsed.options.size(); ++op)
      {
        cout << parsed.options[op].string_key << " = ";
        for (Size v=0; v<parsed.options[op].value.size(); ++v)
        cout<< parsed.options[op].value[v] << endl;
      }
#endif

      store(parsed, vm);
      notify(vm);
    }



    // Store unregistered
    StoredModelMap model_map;
    StoredDimMap dim_map;
    StoredDataMap data_map;
    Scalar log_norm_const_bench = 0.0;
    StoredDataMap bench_filter_map;
    StoredDataMap bench_smooth_map;
    StoredErrorsMap errors_filter_ref_map;
    StoredErrorsMap errors_smooth_ref_map;

    storeUnregistered(parsed_sources, sources_names, model_map, dim_map, data_map, log_norm_const_bench, bench_filter_map, bench_smooth_map, errors_filter_ref_map, errors_smooth_ref_map);

    Bool interactive = vm.count("interactive");
    Bool prec_param = vm.count("prec-param");

    // Select test
    Types<ModelTest>::Ptr p_model_test;
    if (!vm.count("model-id"))
    {
      boost::throw_exception(po::error("Missing model-id option."));
    }
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
        boost::throw_exception(po::error("Unknown model-id value."));
        break;
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
        cout << "Reading model parameters in file: " << config_file_name << endl;
      p_model_test->SetModelParam(model_map);

      if (verbosity>0 && interactive)
        pressEnterToContinue();
    }

    if (!dim_map.empty())
    {
      if (verbosity>0)
        cout << "Reading data dimensions in file: " << config_file_name << endl;
      p_model_test->SetDimensions(dim_map);

      if (verbosity>0 && interactive)
        pressEnterToContinue();
    }

    // Build data graph
    if (verbosity>0)
      cout << "Building data graph..." << endl;
    p_model_test->BuildDataGraph();

    if (verbosity>0)
    {
      cout << INDENT_STRING << "Graph nodes: " << p_model_test->DataGraphSize();
      cout << " (Constant: "<< p_model_test->DataNodesSummary().at(CONSTANT);
      cout << ", Logical: "<< p_model_test->DataNodesSummary().at(LOGICAL);
      cout << ", Stochastic: " << p_model_test->DataNodesSummary().at(STOCHASTIC) << ")" << endl;
    }
    Size n_data_unobs_nodes = p_model_test->DataUnobsNodesSummary().at(LOGICAL) + p_model_test->DataUnobsNodesSummary().at(STOCHASTIC);

    if (verbosity>0)
    {
      cout << INDENT_STRING << "Unobserved nodes: " << n_data_unobs_nodes;
      cout << " (Logical: "<< p_model_test->DataUnobsNodesSummary().at(LOGICAL);
      cout << ", Stochastic: " << p_model_test->DataUnobsNodesSummary().at(STOCHASTIC) << ")" << endl;
    }

    if ( verbosity>0 && interactive && yesNoQuestion("Print data graph (y/n)? ", 'y', 'n', 1) )
    {
      p_model_test->PrintDataGraph();
      pressEnterToContinue();
    }

    if ( verbosity>0 && interactive && yesNoQuestion("Print data graphviz (y/n)? ", 'y', 'n', 1) )
    {
      p_model_test->PrintDataGraphviz();
      pressEnterToContinue();
    }

    // Sample data graph values
    if (data_map.empty())
    {
      if (!vm.count("data-rng-seed"))
        data_rng_seed = clock();
      if (verbosity>0)
      {
        cout << "Sampling data graph values..." << endl;
        cout << INDENT_STRING << "rng seed: " << data_rng_seed << endl;
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
        cout << "Reading data values in file: " << config_file_name << endl;
      p_model_test->SetData(data_map);
    }

    if (verbosity>0 && interactive)
      pressEnterToContinue();

    if (exec_step < 1)
      return;

    // Run Benchmark
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
        cout << "Reading benchmarks values in file: " << config_file_name << endl;
      p_model_test->SetBenchFilter(bench_filter_map);
      p_model_test->SetBenchSmooth(bench_smooth_map);
    }

    if (exec_step < 2)
      return;

    if (verbosity>0 && interactive)
      pressEnterToContinue();

    // Build model graph
    if (verbosity>0)
      cout << "Building model graph..." << endl;
    p_model_test->BuildModelGraph();

    if (verbosity>0)
    {
      cout << INDENT_STRING << "Graph nodes: " << p_model_test->ModelGraphSize();
      cout << " (Constant: "<< p_model_test->ModelNodesSummary().at(CONSTANT);
      cout << ", Logical: "<< p_model_test->ModelNodesSummary().at(LOGICAL);
      cout << ", Stochastic: " << p_model_test->ModelNodesSummary().at(STOCHASTIC) << ")" << endl;
    }
    Size n_model_unobs_nodes = p_model_test->ModelUnobsNodesSummary().at(LOGICAL) + p_model_test->ModelUnobsNodesSummary().at(STOCHASTIC);

    if (verbosity>0)
    {
      cout << INDENT_STRING << "Unobserved nodes: " << n_model_unobs_nodes;
      cout << " (Logical: "<< p_model_test->ModelUnobsNodesSummary().at(LOGICAL);
      cout << ", Stochastic: " << p_model_test->ModelUnobsNodesSummary().at(STOCHASTIC) << ")" << endl;
    }

    if (verbosity>0 && interactive && yesNoQuestion("Print model graph (y/n)? ", 'y', 'n', 1) )
    {
      p_model_test->PrintModelGraph();
      pressEnterToContinue();
    }

    if (verbosity>0 && interactive && yesNoQuestion("Print model graphviz (y/n)? ", 'y', 'n', 1) )
    {
      p_model_test->PrintModelGraphviz();
      pressEnterToContinue();
    }

    if (verbosity>0)
    {
      cout << "Running SMC algorithms with" << endl;
      cout << INDENT_STRING << "resampling type: " << resample_type << ", threshold: " << ess_threshold << " ESS/N" << endl;
    }

    // Run SMC sampler
    for (Size i_n_part=0; i_n_part<n_particles.size(); ++i_n_part)
    {
      Size n_part = n_particles[i_n_part];
      for (Size i_mut=0; i_mut<mutations.size(); ++i_mut)
      {
        vector<Scalar> errors_filter_new;
        vector<Scalar> errors_smooth_new;
        vector<Scalar> log_norm_const_smc;

        string mut = mutations[i_mut];
        p_model_test->BuildSMC(mut=="prior");

        if (verbosity>0)
        {
          cout << "Running " << n_smc << " SMC algorithms of " << p_model_test->NIterations() << " iterations with" << endl;
          cout << INDENT_STRING << "mutation: " << mut << ", particles: " << n_part << endl;
        }

        Types<boost::progress_display>::Ptr p_show_progress;
        if (verbosity==1 && n_smc>1)
          p_show_progress = Types<boost::progress_display>::Ptr(new boost::progress_display(n_smc, cout, ""));

        for (Size i_smc=0; i_smc<n_smc; ++i_smc)
        {
          p_model_test->ClearSMC();

          if (n_smc>1 || !vm.count("smc-rng-seed"))
            smc_rng_seed = clock()+i_smc+1;
          if (n_smc==1 && verbosity>0)
            cout << INDENT_STRING << "rng seed: " << smc_rng_seed << endl;

          p_model_test->RunSMC(n_part, smc_rng_seed, resample_type, ess_threshold, n_smc==1, num_bins);

          if (verbosity==1 && n_smc>1)
            ++(*p_show_progress);
          else if (verbosity>0)
            cout << INDENT_STRING << "log-normalizing constant = " << p_model_test->LogNormConst() << endl;

          log_norm_const_smc.push_back(p_model_test->LogNormConst());

          Scalar error_filter;
          if (p_model_test->ErrorFilter(error_filter))
          {
            error_filter *= n_part;
            if (n_smc==1 && verbosity>0)
            {
              cout << INDENT_STRING << "filtering error = " << error_filter << endl;
            }
            errors_filter_new.push_back(error_filter);
          }

          Scalar error_smooth;
          if (p_model_test->ErrorSmooth(error_smooth))
          {
            error_smooth *= n_part;
            if (n_smc==1 && verbosity>0)
            {
              cout << INDENT_STRING << "smoothing error = " << error_smooth << endl;
            }
            errors_smooth_new.push_back(error_smooth);
          }

          if (verbosity>0 && interactive && n_smc==1)
            pressEnterToContinue();

#ifdef USE_Qwt5_Qt4
          // Plot results
          if (n_smc==1 && (vm.count("plot-file-name") || show_mode >= 1) )
          {
            p_model_test->PlotResults(plot_file_name);
            if (verbosity>0 && vm.count("plot-file-name"))
            {
              cout << "Results plot written in file: " << plot_file_name << endl;
              if (interactive)
                pressEnterToContinue();
            }
          }
#endif //USE_Qwt5_Qt4
        }

        if (exec_step < 3)
          continue;

        // Check normalizing constant mean;
        if (!log_norm_const_bench)
        {
          cerr << "Warning: no log-normalizing constant reference." << endl;
          cerr << "         missing 'log-norm-const' option in section [bench]." << endl;
        }
        else if (n_smc==1)
        {
          cerr << "Warning: can not check normalizing constant mean with repeat-smc = 1." << endl;
        }
        else
        {
          if (verbosity>0)
          {
            cout << "Checking normalizing constant mean with reject level " << reject_level << endl;
            cout << INDENT_STRING << "expected log-norm-const mean = " << log_norm_const_bench << endl;
          }

          using namespace acc;
          typedef accumulator_set<long double, features<tag::mean, tag::variance> > acc_ref_type;

          acc_ref_type norm_const_smc_acc;

          for (Size i =0; i<log_norm_const_smc.size(); ++i)
            norm_const_smc_acc(expl(log_norm_const_smc[i]));

          cout << INDENT_STRING << "SMC sample log-norm-const mean = " << log(mean(norm_const_smc_acc)) << endl;

          Scalar student_stat = (mean(norm_const_smc_acc) - exp(log_norm_const_bench)) / sqrt(variance(norm_const_smc_acc)) * sqrt(Scalar(n_smc));
          boost::math::students_t student_dist(n_smc-1);

          Scalar t_p_value = 2*cdf(complement(student_dist, fabs(student_stat)));

          cout << INDENT_STRING << "Student t-test: t = " << student_stat << ", p-value = " << t_p_value << endl;

          BOOST_CHECK_GT(t_p_value, reject_level);
        }


        if (check_mode<1)
          continue;

        Bool check_filter = check_mode>=1;
        Bool check_smooth = check_mode>=2;

        if (check_filter)
        {
          if (!errors_filter_ref_map.count(mut) || !errors_filter_ref_map[mut].count(n_part))
          {
            cerr << "Warning: no filtering reference errors." << endl;
            cerr << "         missing '" << mut << "." << n_part << "' option in section [bench.filter]." << endl;
            check_filter = false;
          }
          else if (errors_filter_ref_map[mut][n_part].empty())
          {
            cerr << "Warning: no filtering reference errors." << endl;
            cerr << "         option '" << mut << "." << n_part << "'  in section [bench.filter] has no value." << endl;
            check_filter = false;
          }
        }

        if (check_smooth)
        {
          if (!errors_smooth_ref_map.count(mut) || !errors_smooth_ref_map[mut].count(n_part))
          {
            cerr << "Warning: no smoothing reference errors." << endl;
            cerr << "         missing '" << mut << "." << n_part << "' option in section [bench.smooth]." << endl;
            check_smooth = false;
          }
          else if (errors_smooth_ref_map[mut][n_part].empty())
          {
            cerr << "Warning: no smoothing reference errors." << endl;
            cerr << "         option '" << mut << "." << n_part << "'  in section [bench.smooth] has no value." << endl;
            check_smooth = false;
          }
        }


        if (n_smc==1)
        {
          // compute 1-alpha quantile of reference errors distribution
          Scalar error_filter_threshold;
          Scalar error_smooth_threshold;

          if (verbosity>0)
          {
            cout << "Computing 1-alpha quantile of reference errors distribution" << endl;
            cout << INDENT_STRING << "alpha = " << reject_level << endl;
          }

          using namespace acc;
          typedef accumulator_set<Scalar, features<tag::p_square_quantile> > acc_ref_type;

          if (check_filter)
          {
            acc_ref_type errors_filter_ref_acc;

            errors_filter_ref_acc = acc_ref_type(quantile_probability = 1-reject_level);
            
            cout << "sample size = " << errors_filter_ref_map[mut][n_part].size() << endl;
            for (Size i =0; i<errors_filter_ref_map[mut][n_part].size(); ++i)
              errors_filter_ref_acc(errors_filter_ref_map[mut][n_part][i]);
            
            error_filter_threshold = p_square_quantile(errors_filter_ref_acc);

            if (verbosity>0)
              cout << INDENT_STRING << "filtering errors quantile = " << error_filter_threshold << endl;

            BOOST_CHECK_LT(errors_filter_new.front(), error_filter_threshold);
          }

          if (check_smooth)
          {
            acc_ref_type errors_smooth_ref_acc;

            errors_smooth_ref_acc = acc_ref_type(quantile_probability = 1-reject_level);
            
            for (Size i =0; i<errors_smooth_ref_map[mut][n_part].size(); ++i)
              errors_smooth_ref_acc(errors_smooth_ref_map[mut][n_part][i]);
            
            error_smooth_threshold = p_square_quantile(errors_smooth_ref_acc);

            if (verbosity>0)
              cout << INDENT_STRING << "smoothing errors quantile = " << error_smooth_threshold << endl;

            BOOST_CHECK_LT(errors_smooth_new.front(), error_smooth_threshold);
          }
        }


        else if (n_smc>1)
        {
          if (check_filter && !errors_filter_new.empty())
          {
            if (verbosity>0)
              cout << "Checking filtering errors fitness with reject level " << reject_level << endl;
            Scalar ks_filter_stat = ksTwoSamplesStat(errors_filter_new.begin(),
                errors_filter_new.end(),
                errors_filter_ref_map[mut][n_part].begin(),
                errors_filter_ref_map[mut][n_part].end());

            Size n_err = errors_filter_new.size();
            Size n_err_ref = errors_filter_ref_map[mut][n_part].size();
            Scalar ks_filter_prob = ksProb(sqrt(n_err*n_err_ref/(n_err+n_err_ref)) * ks_filter_stat);

            if (verbosity>0)
              cout << INDENT_STRING << "Kolmogorov-Smirnov test: D = " << ks_filter_stat << ", p-value = " << ks_filter_prob << endl;

            BOOST_CHECK_GT(ks_filter_prob, reject_level);
          }

          if (check_smooth && !errors_smooth_new.empty())
          {
            if (verbosity>0)
              cout << "Checking smoothing errors fitness with reject level " << reject_level << endl;
            Scalar ks_smooth_stat = ksTwoSamplesStat(errors_smooth_new.begin(),
                errors_smooth_new.end(),
                errors_smooth_ref_map[mut][n_part].begin(),
                errors_smooth_ref_map[mut][n_part].end());

            Size n_err = errors_smooth_new.size();
            Size n_err_ref = errors_smooth_ref_map[mut][n_part].size();
            Scalar ks_smooth_prob = ksProb(sqrt(n_err*n_err_ref/(n_err+n_err_ref)) * ks_smooth_stat);

            if (verbosity>0)
              cout << INDENT_STRING << "Kolmogorov-Smirnov test: D = " << ks_smooth_stat << ", p-value = " << ks_smooth_prob << endl;

            BOOST_CHECK_GT(ks_smooth_prob, reject_level);
          }
        }
      }
    }

#ifdef BIIPS_CATCH_TEST
  }
  BIIPS_CATCH_ERRORS
  catch (po::error & err)
  {
    cerr << "ERROR: " << err.what() << endl;
  }
#endif
}
