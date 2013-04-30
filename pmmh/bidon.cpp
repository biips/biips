#include "BiipsVersion.hpp"
#include "Console.hpp"
#include "common/Types.hpp"
#include "common/DimArray.hpp"
#include "common/MultiArray.hpp"
#include "iostream/outStream.hpp"
#include "iostream/ProgressBar.hpp"
#include "iostream/DumpReader.hpp"

#include <fstream>
#include <ctime>
#include <map>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/p_square_quantile.hpp>
#include <boost/math/distributions/students_t.hpp>

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using namespace std;
using namespace Biips;
typedef map<string, DimArray::Ptr> StoredDimMap;
typedef map<string, vector<MultiArray> > StoredDataMap;
typedef map<string, map<Size, vector<Scalar> > > StoredErrorsMap;


int main(int argc, char ** argv) 
{
  using namespace Biips;

  String model_file_name = "hmm_1d_lin.bug";
  Size exec_step;
  String do_smooth_str;
  Size check_mode;
  Size data_rng_seed;
  Size smc_rng_seed;
  vector<Size> n_particles;
  Scalar ess_threshold;
  String resample_type;
  Scalar reject_level;
  String dot_file_name;
  String config_file_name;
  vector<String> mutations;
  Size verbosity;
  Size num_bins;
  String data_file_name = "data.txt";


  cout << "BiipsTestCompiler, version " << BIIPS_VERSION() << endl;

  vector<String> monitored_var;
  StoredDimMap dim_map_stored;
  StoredDataMap data_map_stored;
  Scalar log_norm_const_bench = 0.0;
  StoredDataMap bench_filter_map_stored;
  StoredDataMap bench_smooth_map_stored;
  StoredErrorsMap errors_filter_ref_map_stored;
  StoredErrorsMap errors_smooth_ref_map_stored;


  Bool interactive = false; 

  Bool do_smooth = true;

  // Read data
  // ---------------------------------
  map<String, MultiArray> data_map;

  if (verbosity > 0)
  {
    cout << PROMPT_STRING << "Reading data in:" << endl;
    cout << INDENT_STRING << "data-file = " << data_file_name << endl;
  }

  ifstream ifs_dump(data_file_name.c_str());
  if (ifs_dump.fail())
    throw RuntimeError(String("Failed to open file ") + data_file_name);

  DumpReader dump_reader(ifs_dump);
  while (ifs_dump.good())
  {
    if (!dump_reader.next())
      throw RuntimeError(String("Error parsing file: ") + data_file_name);

    DimArray::Ptr dim_ptr(
        new DimArray(dump_reader.dims().begin(), dump_reader.dims().end()));
    ValArray::Ptr val_ptr(new ValArray());
    
    if (dump_reader.is_int())
    {
      if (dump_reader.has_na())
      {
        val_ptr->resize(dump_reader.int_values().size());
        std::replace_copy(dump_reader.int_values().begin(),
                          dump_reader.int_values().end(), val_ptr->begin(),
                          Scalar(BIIPS_INTNA), BIIPS_REALNA);
      }
      else
      {
        val_ptr->assign(dump_reader.int_values().begin(),
                        dump_reader.int_values().end());
      }
    }
    else
    {
      val_ptr->assign(dump_reader.double_values().begin(),
                      dump_reader.double_values().end());
    }
    if (val_ptr->empty())
      throw RuntimeError(
          String("Error reading data: variable ") + dump_reader.name()
          + " has no values.");
    if (dim_ptr->empty())
      dim_ptr->push_back(val_ptr->size());
    else if (dim_ptr->Length() != val_ptr->size())
      throw RuntimeError(
          String("Error reading data: dimensions of variable ")
          + dump_reader.name() + " do not match the number of values.");

    data_map.insert(std::make_pair(dump_reader.name(), MultiArray(dim_ptr, val_ptr)));
  }


#ifdef BIIPS_DEBUG
  cout << "Parsed data variables: ";
  for (map<String, MultiArray>::const_iterator it = data_map.begin();
      it != data_map.end(); ++it)
    cout << it->first << " ";
  cout << endl;
#endif


  // Make a console
  // ------------------
  Console console(cout, cerr);

  // Check model syntax
  // ------------------
  if (!console.CheckModel(model_file_name, verbosity > 0))
    throw RuntimeError("Model syntax is incorrect.");

  // Load Base module
  // ------------------
  if (!console.LoadBaseModule(verbosity > 0))
    throw RuntimeError("Failed to load Base module.");

  // Compile model
  // ------------------

  data_rng_seed = 42;
  if (verbosity > 0)
    cout << INDENT_STRING << "data-rng-seed = " << data_rng_seed << endl;

  if (!console.Compile(data_map, true, data_rng_seed, verbosity))
    throw RuntimeError("Failed to compile model.");


  if (exec_step < 1)
    return 0;

  // Monitor variables
  if (verbosity > 0)
    cout << PROMPT_STRING << "Setting user filter monitors" << endl;

  for (Size i = 0; i < monitored_var.size(); ++i)
  {
    const String & name = monitored_var[i];

    if (!console.SetFilterMonitor(name))
      throw RuntimeError(String("Failed to monitor variable ") + name);

    if (verbosity > 0)
      cout << INDENT_STRING << "monitoring variable " << name << endl;
  }


  if (do_smooth)
  {
    if (verbosity > 0)
      cout << PROMPT_STRING
           << "Setting default filter monitors for backward smoothing step"
           << endl;

    if (!console.SetDefaultFilterMonitors())
      throw RuntimeError("Failed to set default filter monitors");


    if (verbosity > 0)
      cout << PROMPT_STRING << "Setting user smoother monitors" << endl;

    for (Size i = 0; i < monitored_var.size(); ++i)
    {
      const String & name = monitored_var[i];

      if (!console.SetBackwardSmoothMonitor(name))
        throw RuntimeError(String("Failed to monitor variable ") + name);

      if (verbosity > 0)
        cout << INDENT_STRING << "monitoring variable " << name << endl;
    }

  }

  // Run SMC samplers
  //----------------------
  for (Size i_n_part = 0; i_n_part < n_particles.size(); ++i_n_part)
  {
    Size n_part = n_particles[i_n_part];
    for (Size i_mut = 0; i_mut < mutations.size(); ++i_mut)
    {
      string mut = mutations[i_mut];

      if (verbosity > 0)
      {
        cout << endl;
        cout << INDENT_STRING << "mutation = " << mut << endl;
        cout << INDENT_STRING << "particles = " << n_part << endl;
        cout << INDENT_STRING << "resampling = " << resample_type << endl;
        cout << INDENT_STRING << "ess-threshold = " << ess_threshold << endl;
      }


      Types<ProgressBar>::Ptr p_show_progress;

      vector<Scalar> errors_filter_new;
      vector<Scalar> errors_smooth_new;
      vector<Scalar> log_norm_const_smc;

      if (!console.BuildSampler(mut == "prior",
                                verbosity * (verbosity > 1)))
        throw RuntimeError("Failed to build sampler.");


      // Build sampler
      //----------------------
      if (verbosity > 0)
        cout << INDENT_STRING << "smc-rng-seed = " << smc_rng_seed << endl;
      
      // Run sampler
      //----------------------
      Bool verbose_run_smc = verbosity > 1 || (verbosity > 0);
      if (!console.RunForwardSampler(n_part, smc_rng_seed, resample_type,
                                     ess_threshold, verbose_run_smc,
                                     verbose_run_smc))
        throw RuntimeError("Failed to run SMC sampler.");

      Scalar log_norm_const;
      if (!console.GetLogNormConst(log_norm_const))
        throw RuntimeError("Failed to get log normalizing constant.");

      if (verbosity > 0)
        cout << INDENT_STRING << "log-normalizing constant = "
             << log_norm_const << endl;

      log_norm_const_smc.push_back(log_norm_const);

      if (exec_step < 2)
        continue;

      // Compute filter mean error of monitored values
      //----------------------------------------------
      Scalar error_filter = 0.0;

      for (Size i = 0; i < monitored_var.size(); ++i)
      {
        const String & name = monitored_var[i];

        if (verbosity > 0)
          cout << PROMPT_STRING << "Computing filtering error of variable "
               << name << endl;

      }

      error_filter *= n_part;

      if (verbosity > 0)
        cout << INDENT_STRING << "filtering error = " << error_filter << endl;

      errors_filter_new.push_back(error_filter);


      // Run backward smoother
      //-------------------------------------------------
      if (do_smooth)
      {
        Bool verbose_run_back = verbosity > 1
                                || (verbosity > 0);
        if (!console.RunBackwardSmoother(verbose_run_back, verbose_run_back))

        if (verbosity == 1)
          ++(*p_show_progress);

        if (exec_step < 2)
          continue;

        // Compute smooth mean error of monitored values
        //----------------------------------------------
        Scalar error_smooth = 0.0;

        for (Size i = 0; i < monitored_var.size(); ++i)
        {
          const String & name = monitored_var[i];

          if (verbosity > 0)
            cout << PROMPT_STRING << "Computing smoothing error of variable "
                 << name << endl;

        }

        error_smooth *= n_part;

        if (verbosity > 0)
          cout << INDENT_STRING << "smoothing error = " << error_smooth
               << endl;

        errors_smooth_new.push_back(error_smooth);


    }
    }
  }
} // fin main

