//                                               -*- C++ -*-
/*! \file BiipsTestCompiler.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BiipsTest
#include <boost/test/unit_test.hpp>

#include "config.hpp"
#include "storeUnregistered.hpp"
#include "TestIO.hpp"
#include "Console.hpp"
#include "kolmogorov.hpp"
#include "common/cholesky.hpp"
#include "Plot.hpp"
#include "print/print.hpp"

#include <fstream>
#include <ctime>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/p_square_quantile.hpp>
#include <boost/progress.hpp>

namespace Biips
{
  const String BIIPSTEST_CONFIG_FILE_NAME = "biipstest.cfg";
  const String BIIPSTEST_VERSION = "BiipsTestCompiler, version 0.05";

  Bool computeError(Scalar & error, const String & varName,
      const std::map<String, std::map<IndexRange, MultiArray> > & smcMeanValuesMap,
      const std::map<String, std::vector<MultiArray> > & benchValuesMap);
}

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;

BOOST_AUTO_TEST_CASE( my_test )
{
  int argc = boost::unit_test::framework::master_test_suite().argc;
  char ** argv = boost::unit_test::framework::master_test_suite().argv;
  using namespace Biips;

#ifdef BIIPS_CATCH_ON
  try
  {
#endif
    String model_file_name;
    Size exec_step;
    String check_mode;
    Size show_mode;
    Size data_rng_seed;
    Size smc_rng_seed;
    vector<Size> n_particles;
//    Size n_particles;
    Scalar ess_threshold;
    Size resample_type;
    Size n_smc;
    Scalar reject_level;
//    String plot_file_name;
    String dot_file_name;
    String config_file_name;
    vector<String> mutations;
    Size verbosity;
    Size num_bins;

    map<Size, ResampleType> resample_type_map;
    resample_type_map[0] = SMC_RESAMPLE_MULTINOMIAL;
    resample_type_map[1] = SMC_RESAMPLE_RESIDUAL;
    resample_type_map[2] = SMC_RESAMPLE_STRATIFIED;
    resample_type_map[3] = SMC_RESAMPLE_SYSTEMATIC;

    map<Size, String> resample_type_name_map;
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
        ("interactive", "asks questions to the user.\n"
            "applies when verbose>0.")
        ("show-plots,s", po::value<Size>(&show_mode)->default_value(0), "shows plots, interrupting execution.\n"
            "applies when n-smc=1.\n"
            "values:\n"
            " 0: \tno plots are shown.\n"
            " 1: \tfinal results plots are shown.\n"
            " 2: \tall plots are shown.")
        ("num-bins", po::value<Size>(&num_bins)->default_value(40), "number of bins in the histogram plots.")
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
        ("model-file", po::value<String>(&model_file_name), "BUGS model file name.")
        ("data-rng-seed", po::value<Size>(&data_rng_seed), "data sampler rng seed. default=time().")
        ("particles", po::value<vector<Size> >(&n_particles)->default_value(vector<Size>(1, 1000)), "numbers of particles.")
        ("smc-rng-seed", po::value<Size>(&smc_rng_seed), "SMC sampler rng seed. default=time().\n"
            "applies when n-smc=1.")
        ("mutations", po::value<vector<String> >(&mutations)->default_value(vector<String>(1, "optimal")), "type of exploration used in the mutation step of the SMC algorithm.\n"
            "values:\n"
            " optimal: \tuse optimal mutation, when possible.\n"
            " prior: \tuse prior mutation.\n"
            " <other>: \tpossible model-specific mutation.")
        ("resampling", po::value<Size>(&resample_type)->default_value(2), "resampling method.\n"
            "values:\n"
            " 0: \tMULTINOMIAL.\n"
            " 1: \tRESIDUAL.\n"
            " 2: \tSTRATIFIED.\n"
            " 3: \tSYSTEMATIC.")
        ("ess-threshold", po::value<Scalar>(&ess_threshold)->default_value(0.5), "ESS resampling threshold.")
        ("n-smc", po::value<Size>(&n_smc)->default_value(1), "number of independent SMC executions for each mutation and number of particles.")
//        ("prec-param", "uses precision parameter instead of variance for normal distributions.")
        ("reject-level", po::value<Scalar>(&reject_level)->default_value(0.01), "accepted level of rejection in checks.")
//        ("plot-file", po::value<String>(&plot_file_name), "plots pdf file name.\n"
//            "applies when n-smc=1.")
        ("dot-file", po::value<String>(&dot_file_name), "dot file name.\n"
            "The file will be created or overwritten.")
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
        "\nUsage: BiipsTestCompiler [<option>]... --model-file=<file_name> [<option>]...\n"
        "  runs model <file_name> with default model parameters, with multiple <option> parameters.\n"
        "       BiipsTestCompiler [<option>]... <config_file> [<option>]...\n"
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
    }

    // Program options pre-processing
    if (vm.count("help-test"))
    {
      cout << visible << "\n";
      return;
    }

    if (vm.count("version"))
    {
      cout << BIIPSTEST_VERSION << endl;
      return;
    }

    // Configuration file options parsing
    ifstream ifs(config_file_name.c_str());
    if (ifs.fail())
      throw RuntimeError(String("Failed to open file ")+dot_file_name);

    parsed_sources.push_back(po::parse_config_file(ifs, config_file_options, true));
    sources_names.push_back(config_file_name);

    const po::parsed_options & parsed = parsed_sources.back();

    store(parsed, vm);
    notify(vm);

    // Store unregistered
    vector<String> monitored_var;
    StoredDimMap dim_map_stored;
    StoredDataMap data_map_stored;
    StoredDataMap bench_filter_map_stored;
    StoredDataMap bench_smooth_map_stored;
    StoredErrorsMap errors_filter_ref_map_stored;
    StoredErrorsMap errors_smooth_ref_map_stored;

    storeUnregistered(parsed_sources, sources_names, monitored_var, dim_map_stored, data_map_stored, bench_filter_map_stored, bench_smooth_map_stored, errors_filter_ref_map_stored, errors_smooth_ref_map_stored);

#ifdef BIIPS_DEBUG_ON
    cout << "Stored data variables: ";
    for (StoredDataMap::const_iterator it = data_map_stored.begin();
        it != data_map_stored.end(); ++it)
      cout << it->first << " ";
    cout << endl;
#endif

    Bool interactive = vm.count("interactive");
//      Bool prec_param = vm.count("prec-param");
//    Bool prior = vm.count("prior");

    if (!vm.count("model-file"))
      boost::throw_exception(po::error("Missing --model-file option."));

    // Open model file
    // ------------------
    FILE * model_file = fopen(model_file_name.c_str(), "r");

    if (!model_file)
      throw RuntimeError(String("Failed to open file: ") + model_file_name);

    Console console(cout, cerr);

    // Check model syntax
    // ------------------
    if (verbosity>0)
      cout << "Parsing model in: --model-file=" << model_file_name << endl;

    if (!console.CheckModel(model_file, verbosity>0))
      throw RuntimeError("Model syntax is incorrect.");

    if (verbosity>0)
      cout << INDENT_STRING << "Model syntax is correct." << endl;

    if (verbosity>0 && interactive)
      pressEnterToContinue();

    // Read data
    // ------------------
    if (verbosity>0)
      cout << "Reading data in: --cfg=" << config_file_name << endl;

    map<String, MultiArray> data_map = transformStoredDataMap(data_map_stored);

#ifdef BIIPS_DEBUG_ON
    cout << "Parsed data variables: ";
    for (map<String, MultiArray>::const_iterator it = data_map.begin();
        it != data_map.end(); ++it)
      cout << it->first << " ";
    cout << endl;
#endif

    if (verbosity>0 && interactive)
      pressEnterToContinue();

    // Load Base module
    // ------------------
    if (!console.LoadBaseModule(verbosity>0))
      throw RuntimeError("Failed to load Base module.");

    if (verbosity>0 && interactive)
      pressEnterToContinue();

    // Compile model
    // ------------------

    if (!vm.count("data-rng-seed"))
      data_rng_seed = time(0);
    if (verbosity>0)
      cout << "--" << "data-rng-seed=" << data_rng_seed << endl;

    if (!console.Compile(data_map, true, data_rng_seed, verbosity>0))
      throw RuntimeError("Failed to compile model.");

    // Write dot file
    //----------------------

    if (vm.count("dot-file"))
    {
      if (verbosity>0)
        cout << "Writing dot file in:" << dot_file_name << endl;

      std::ofstream ofs(dot_file_name.c_str());

      if (ofs.fail())
        throw RuntimeError(String("Failed to open file ")+dot_file_name);

      if (!console.PrintGraphviz(ofs))
        throw RuntimeError("Failed to print dot file.");

      if (verbosity>0 && interactive)
        pressEnterToContinue();
    }

    if (exec_step <1)
      return;


    // Monitor variables
    for (Size i=0; i<monitored_var.size(); ++i)
    {
      const String & name = monitored_var[i];
      if (!console.SetFilterMonitor(name))
      {
        throw RuntimeError(String("Failed to monitor variable ") + name);
      }

      if (verbosity>0)
        cout << "Monitoring variable " << name << endl;
    }

    if (verbosity>0 && interactive && !monitored_var.empty())
      pressEnterToContinue();

//    // Reading benchmarks values
//    //----------------------
//    else
//    {
//      if (verbosity>0)
//        cout << "Reading benchmarks values in file: " << config_file_name << endl;
//      p_model_test->SetBenchFilter(bench_filter_map_stored);
//      p_model_test->SetBenchSmooth(bench_smooth_map_stored);
//    }
//
//    if (exec_step < 2)
//      return;
//
//    if (verbosity>0 && interactive)
//      pressEnterToContinue();

    // Run SMC samplers
    //----------------------

    if (verbosity>0)
    {
      cout << "Resampling method of SMC algorithms:" << endl;
      cout << "--" << "resampling=" << resample_type << endl;
      cout << "--" << "ess-threshold=" << ess_threshold << endl;
      //      cout << INDENT_STRING << "resampling type: " << resample_type_name_map[resample_type] << ", threshold: " << ess_threshold << " ESS/N" << endl;
    }

    if (verbosity>0 && interactive)
      pressEnterToContinue();

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
          cout << "Running " << n_smc << " SMC algorithms" << endl;
          cout << "--" << "mutation=" << mut << endl;
          cout << "--" << "particles=" << n_part << endl;
//          cout << INDENT_STRING << "mutation: " << mut << ", particles: " << n_part << endl;
        }

        if (verbosity>0 && interactive && n_smc>1)
          pressEnterToContinue();

        Types<boost::progress_display>::Ptr p_show_progress;
        if (verbosity==1 && n_smc>1)
          p_show_progress = Types<boost::progress_display>::Ptr(new boost::progress_display(n_smc, cout, ""));

        for (Size i_smc=0; i_smc<n_smc; ++i_smc)
        {
          // Initialize sampler
          //----------------------

          if (n_smc>1 || !vm.count("smc-rng-seed"))
            smc_rng_seed = time(0)+i_smc+1;
          if (n_smc==1 && verbosity>0)
          {
            cout << "--" << "smc-rng-seed=" << smc_rng_seed << endl;
//            cout << INDENT_STRING << "rng seed: " << smc_rng_seed << endl;
          }

          if (!console.Initialize(n_part, smc_rng_seed, mut=="prior", verbosity * (n_smc==1 || verbosity>1)))
            throw RuntimeError("Failed to initialize model.");

          if (verbosity>0 && interactive && n_smc==1)
            pressEnterToContinue();

          // Run sampler
          //----------------------
          console.RunSMCSampler(resample_type_map[resample_type], ess_threshold, (verbosity>1 || (verbosity>0 && n_smc==1)));

          if (verbosity==1 && n_smc>1)
            ++(*p_show_progress);

          if (verbosity>0 && interactive && n_smc==1 && i_mut < mutations.size()-1 && i_n_part < n_particles.size()-1)
            pressEnterToContinue();

          // Extract filter mean of monitored values
          //----------------------------------------
          std::map<String, std::map<IndexRange, MultiArray> > filter_mean_map;

          for (Size i=0; i<monitored_var.size(); ++i)
          {
            const String & name = monitored_var[i];

            if (verbosity>0 && n_smc==1)
              cout << "Extracting filtering mean of variable " << name << endl;

            if (!console.ExtractFilterStat(name, MEAN, filter_mean_map[name]))
              throw RuntimeError(String("Failed to extract filtering mean of variable") + name);
          }

          if (exec_step<2)
            continue;

          if (verbosity>0 && interactive && n_smc==1 && i_mut < mutations.size()-1 && i_n_part < n_particles.size()-1)
            pressEnterToContinue();

          // Compute filter mean error of monitored values
          //----------------------------------------------
          Scalar error_filter = 0.0;

          for (Size i=0; i<monitored_var.size(); ++i)
          {
            const String & name = monitored_var[i];

            if (verbosity>0 && n_smc==1)
              cout << "Computing filtering error of variable " << name << endl;

            if(!computeError(error_filter, name, filter_mean_map, bench_filter_map_stored))
              throw RuntimeError(String("Failed to compute filtering error of variable ") + name);
          }

          error_filter *= n_part;

          if (verbosity>0 && n_smc==1)
          {
            cout << INDENT_STRING << "filtering error = " << error_filter << endl;
          }
          errors_filter_new_map[mut][n_part].push_back(error_filter);

          // plot filtering Pdf
          //---------------------------
          if (show_mode >= 2 && n_smc==1)
          {
            std::map<String, std::map<IndexRange, ScalarHistogram> > filter_pdf_map;

            for (Size i=0; i<monitored_var.size(); ++i)
            {
              const String & name = monitored_var[i];

              if (verbosity>0 && n_smc==1)
                cout << "Extracting filtering Pdf of variable " << name << endl;

              if(!console.ExtractFilterPdf(name, filter_pdf_map[name], num_bins))
                throw RuntimeError(String("Failed to extract filtering Pdf of variable ") + name);

              for (std::map<IndexRange, ScalarHistogram>::iterator it_filter_pdf_map = filter_pdf_map[name].begin();
                  it_filter_pdf_map != filter_pdf_map[name].end(); ++it_filter_pdf_map)
              {
                Plot pdf_plot_PF(argc, argv);
                pdf_plot_PF.AddHistogram(it_filter_pdf_map->second, "", Qt::blue);
                pdf_plot_PF.SetTitle(String("Filtering Pdf histogram of variable: ") + name + print(it_filter_pdf_map->first));
                pdf_plot_PF.SetBackgroundColor(Qt::white);
                pdf_plot_PF.Show();
              }
            }
          }

          // plot final curve
          //---------------------------
          if (show_mode >= 1 && n_smc==1)
          {
            for (Size i=0; i<monitored_var.size(); ++i)
            {
              const String & name = monitored_var[i];

              if (filter_mean_map.at(name).begin()->first.Dim().IsScalar())
              {
                Size len = filter_mean_map.at(name).size();

                Types<Scalar>::Array x_est_PF(len);
                Types<Scalar>::Array x_bench(len);
                Types<Scalar>::Array index(len);

                std::map<IndexRange, MultiArray>::iterator it_filter_mean = filter_mean_map.at(name).begin();
                for (Size k=0; it_filter_mean != filter_mean_map.at(name).end();
                    ++k, ++it_filter_mean)
                {
                  x_bench[k] = bench_filter_map_stored.at(name)[k].ScalarView();
                  x_est_PF[k] = it_filter_mean->second.ScalarView();
                  index[k] = k;
                }

                Plot results_plot(argc, argv);
                results_plot.AddCurve(index, x_bench, "Bench estimate", Qt::green, 2);
                results_plot.AddCurve(index, x_est_PF, "PF estimate", Qt::blue, 2);

                results_plot.SetTitle(String("Filtering mean estimate of variable ") + name);
                results_plot.SetAxesLabels("index", name);
                results_plot.SetBackgroundColor(Qt::white);
                results_plot.SetLegend(QwtPlot::RightLegend);
                results_plot.Show();
              }
              else
              {
                Size len = filter_mean_map.at(name).size();

                Types<Scalar>::Array x_bench_0(len);
                Types<Scalar>::Array x_bench_1(len);
                Types<Scalar>::Array x_est_PF_0(len);
                Types<Scalar>::Array x_est_PF_1(len);
                std::map<IndexRange, MultiArray>::iterator it_filter_mean = filter_mean_map.at(name).begin();
                for (Size k=0; it_filter_mean != filter_mean_map.at(name).end();
                    ++k, ++it_filter_mean)
                {
                  x_bench_0[k] = bench_filter_map_stored.at(name)[k].Values()[0];
                  x_bench_1[k] = bench_filter_map_stored.at(name)[k].Values()[1];
                  x_est_PF_0[k] = it_filter_mean->second.Values()[0];
                  x_est_PF_1[k] = it_filter_mean->second.Values()[1];
                }

                Plot results_plot(argc, argv);
                results_plot.AddCurve(x_bench_0, x_bench_1, "Bench estimate", Qt::green, 2);
                results_plot.AddCurve(x_est_PF_0, x_est_PF_1, "PF estimate", Qt::blue, 2);

                results_plot.SetTitle(String("Filtering mean estimate of variable ") + name + " only the 2 first component");
                results_plot.SetAxesLabels(name+" (component 1)", name+" (component 2");
                results_plot.SetBackgroundColor(Qt::white);
                results_plot.SetLegend(QwtPlot::RightLegend);
                results_plot.Show();
              }
            }
          }

          if (verbosity>0 && interactive)
            pressEnterToContinue();
        }


        if (exec_step < 3)
          continue;
        // Check errors
        //-------------

        Bool check_filter = check_mode=="filter" || check_mode=="all";
        Bool check_smooth = check_mode=="smooth" || check_mode=="all";

        if (check_filter)
        {
          if (!errors_filter_ref_map_stored.count(mut) || !errors_filter_ref_map_stored[mut].count(n_part))
          {
            cerr << "Warning: no filtering reference errors." << endl;
            cerr << "         missing " << mut << "." << n_part << " option in section [bench.filter]." << endl;
            check_filter = false;
          }
          else if (errors_filter_ref_map_stored[mut][n_part].empty())
          {
            cerr << "Warning: no filtering reference errors." << endl;
            cerr << "         option " << mut << "." << n_part << "  in section [bench.filter] has no value." << endl;
            check_filter = false;
          }
        }
        if (check_smooth)
        {
          if (!errors_smooth_ref_map_stored.count(mut) || !errors_smooth_ref_map_stored[mut].count(n_part))
          {
            cerr << "Warning: no smoothing reference errors." << endl;
            cerr << "         missing " << mut << "." << n_part << " option in section [bench.smooth]." << endl;
            check_smooth = false;
          }
          else if (errors_smooth_ref_map_stored[mut][n_part].empty())
          {
            cerr << "Warning: no smoothing reference errors." << endl;
            cerr << "         option " << mut << "." << n_part << "  in section [bench.smooth] has no value." << endl;
            check_smooth = false;
          }
        }

        if (!check_filter && !check_smooth)
          continue;

        if (n_smc==1)
        {
          // compute 1-alpha quantile of reference errors distribution
          Scalar error_filter_threshold;
          Scalar error_smooth_threshold;

          if (verbosity>0)
          {
            cout << "Checking errors < 1-alpha quantile of reference errors" << endl;
            cout << INDENT_STRING << "alpha: " << reject_level << endl;
          }

          if (verbosity>0 && interactive)
            pressEnterToContinue();

          using namespace acc;
          typedef accumulator_set<Scalar, features<tag::p_square_quantile> > acc_ref_type;

          if (check_filter)
          {
            acc_ref_type errors_filter_ref_acc;

            errors_filter_ref_acc = acc_ref_type(quantile_probability = 1-reject_level);
            for (Size i =0; i<errors_filter_ref_map_stored[mut][n_part].size(); ++i)
              errors_filter_ref_acc(errors_filter_ref_map_stored[mut][n_part][i]);
            error_filter_threshold = p_square_quantile(errors_filter_ref_acc);

            if (verbosity>0)
            {
              cout << INDENT_STRING << "filtering errors quantile = " << error_filter_threshold << endl;
            }

            BOOST_CHECK_LT(errors_filter_new_map[mut][n_part].front(), error_filter_threshold);
          }

          if (check_smooth)
          {
            acc_ref_type errors_smooth_ref_acc;

            errors_smooth_ref_acc = acc_ref_type(quantile_probability = 1-reject_level);
            for (Size i =0; i<errors_smooth_ref_map_stored[mut][n_part].size(); ++i)
              errors_smooth_ref_acc(errors_smooth_ref_map_stored[mut][n_part][i]);
            error_smooth_threshold = p_square_quantile(errors_smooth_ref_acc);

            if (verbosity>0)
              cout << INDENT_STRING << "smoothing errors quantile = " << error_smooth_threshold << endl;

            BOOST_CHECK_LT(errors_smooth_new_map[mut][n_part].front(), error_smooth_threshold);
          }


          if (verbosity>0 && interactive)
            pressEnterToContinue();
        }



        else if (n_smc>1)
        {
          if (check_filter && !errors_filter_new_map[mut][n_part].empty())
          {
            if (verbosity>0)
              cout << "Checking filtering errors fitness with reject level " << reject_level << endl;
            Scalar ks_filter_stat = ksTwoSamplesStat(errors_filter_new_map[mut][n_part].begin(),
                errors_filter_new_map[mut][n_part].end(),
                errors_filter_ref_map_stored[mut][n_part].begin(),
                errors_filter_ref_map_stored[mut][n_part].end());

            Size n_err = errors_filter_new_map[mut][n_part].size();
            Size n_err_ref = errors_filter_ref_map_stored[mut][n_part].size();
            Scalar ks_filter_prob = ksProb(sqrt(n_err*n_err_ref/(n_err+n_err_ref)) * ks_filter_stat);

            if (verbosity>0)
              cout << INDENT_STRING << "Kolmogorov-Smirnov test: D = " << ks_filter_stat << ", p-value = " << ks_filter_prob << endl;

            BOOST_CHECK_GT(ks_filter_prob, reject_level);

          }

          if (check_smooth && !errors_smooth_new_map[mut][n_part].empty())
          {
            if (verbosity>0)
              cout << "Checking smoothing errors fitness with reject level " << reject_level << endl;
            Scalar ks_smooth_stat = ksTwoSamplesStat(errors_smooth_new_map[mut][n_part].begin(),
                errors_smooth_new_map[mut][n_part].end(),
                errors_smooth_ref_map_stored[mut][n_part].begin(),
                errors_smooth_ref_map_stored[mut][n_part].end());

            Size n_err = errors_smooth_new_map[mut][n_part].size();
            Size n_err_ref = errors_smooth_ref_map_stored[mut][n_part].size();
            Scalar ks_smooth_prob = ksProb(sqrt(n_err*n_err_ref/(n_err+n_err_ref)) * ks_smooth_stat);

            if (verbosity>0)
              cout << INDENT_STRING << "Kolmogorov-Smirnov test: D = " << ks_smooth_stat << ", p-value = " << ks_smooth_prob << endl;

            BOOST_CHECK_GT(ks_smooth_prob, reject_level);
          }

          if (verbosity>0 && interactive)
            pressEnterToContinue();
        }
      }
    }

#ifdef BIIPS_CATCH_ON
  }
  BIIPS_CATCH_ERRORS
  catch (po::error & err)
  {
    cerr << "ERROR: " << err.what() << endl;
  }
#endif
}


namespace Biips
{
  Bool computeError(Scalar & error, const String & varName,
      const std::map<String, std::map<IndexRange, MultiArray> > & smcMeanValuesMap,
      const std::map<String, std::vector<MultiArray> > & benchValuesMap)
  {
    if ( !(smcMeanValuesMap.count(varName) && benchValuesMap.count(varName) && benchValuesMap.count("var."+varName)))
      return false;

    const std::map<IndexRange, MultiArray> & smc_values_map = smcMeanValuesMap.find(varName)->second;
    const std::vector<MultiArray> & bench_values = benchValuesMap.find(varName)->second;
    const std::vector<MultiArray> & bench_var_values = benchValuesMap.find("var."+varName)->second;

    if (smc_values_map.size() != bench_values.size())
      return false;
    if (smc_values_map.size() != bench_var_values.size())
      return false;

    std::map<IndexRange, MultiArray>::const_iterator it_smc_values = smc_values_map.begin();
    for (Size k=0;
        it_smc_values != smc_values_map.end();
        ++k, ++it_smc_values)
    {
      if (!(it_smc_values->second.IsVector() && bench_values[k].IsVector()))
        return false;

      Size dim = it_smc_values->second.Length();

      if (dim != bench_values[k].Length())
        return false;

      if (dim == 1)
      {
        if (bench_var_values[k].Length() != 1)
          return false;
      }
      else
      {
        if (!bench_var_values[k].IsSquared())
          return false;
        else if (dim != bench_var_values[k].Dim()[0])
          return false;
      }

      Vector diff_vec(dim, it_smc_values->second.Values() - bench_values[k].Values());
      Matrix var_chol(dim, dim, bench_var_values[k].Values());
      ublas::cholesky_factorize(var_chol);
      ublas::inplace_solve(var_chol, diff_vec, ublas::lower_tag());
      error += ublas::inner_prod(diff_vec, diff_vec);
    }

    return true;
  }
}
