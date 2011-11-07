//                                               -*- C++ -*-
/*! \file RBiips.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "Console.hpp"
#include "RBiipsCommon.h"
#include <fstream>

using namespace Biips;
using std::endl;

static Size verbosity = 0;

static inline void checkConsole(SEXP ptr)
{
  // FIXME
}


static std::map<String, MultiArray> read_data_list(SEXP data)
{
  std::map<String, MultiArray> data_map;

  if (verbosity>0)
    rbiips_cout << PROMPT_STRING << "Reading data in the list" << endl;

  Rcpp::List data_list(data);
  if (!data_list.hasAttribute("names"))
  {
    rbiips_cerr << "Warning: Missing variable names" << endl;
    return data_map;
  }

  if (verbosity>0)
    rbiips_cout << INDENT_STRING << "Variables:";

  Rcpp::CharacterVector names = data_list.attr("names");
  for (Size i=0; i<names.size(); ++i)
  {
    String var_name(names[i]);
    if (verbosity>0)
      rbiips_cout << " " << var_name;

    Rcpp::NumericVector r_vec = data_list[var_name];
    MultiArray marray;

    // TODO: check MultiArray::StorageOrder
    if (!r_vec.hasAttribute("dim"))
    {
      DimArray::Ptr p_dim(new DimArray(1, r_vec.size()));
      ValArray::Ptr p_val(new ValArray(r_vec.size()));
      std::replace_copy(r_vec.begin(), r_vec.end(), p_val->begin(), NA_REAL, BIIPS_REALNA);
      marray.SetPtr(p_dim, p_val);
    }
    else
    {
      Rcpp::IntegerVector r_dim = r_vec.attr("dim");
      DimArray::Ptr p_dim(new DimArray(r_dim.begin(), r_dim.end()));
      ValArray::Ptr p_val(new ValArray(r_vec.size()));
      std::replace_copy(r_vec.begin(), r_vec.end(), p_val->begin(), NA_REAL, BIIPS_REALNA);
      marray.SetPtr(p_dim, p_val);
    }

    data_map[var_name] = marray;
  }
  if (verbosity>0)
    rbiips_cout << endl;

  return data_map;
}


RcppExport void set_verbosity(SEXP verb)
{
  BEGIN_RBIIPS
  verbosity = Rcpp::as<Size>(verb);
  VOID_END_RBIIPS
}


RcppExport SEXP load_module(SEXP name)
{
  BEGIN_RBIIPS
  String mod_name = Rcpp::as<String>(name);
  if (mod_name == "basemod")
  {
    load_base_module();
    return Rcpp::wrap(true);
  }
  else
    return Rcpp::wrap(false);
  END_RBIIPS
}


RcppExport void clear_console(SEXP pConsole)
{
  /* Finalizer for console pointers. Frees the external memory
       and zeroes the pointer when the R object is deleted */

  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);
  if (!p_console.isNULL())
  {
    p_console = Rcpp::XPtr<Console>((Console *)NULL);
  }
  VOID_END_RBIIPS
}


RcppExport SEXP make_console()
{
  BEGIN_RBIIPS
  Rcpp::XPtr<Console> p_console(new Console(rbiips_cout, rbiips_cerr));
  return p_console;
  END_RBIIPS
}


RcppExport void check_model(SEXP pConsole, SEXP modelFileName)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  // Check model syntax
  if (verbosity>0)
    rbiips_cout << PROMPT_STRING << "Parsing model in: " << Rcpp::as<String>(modelFileName) << endl;

  if (! p_console->CheckModel(Rcpp::as<String>(modelFileName), true))
    throw RuntimeError("Model syntax is incorrect.");

  VOID_END_RBIIPS
}


RcppExport void compile_model(SEXP pConsole, SEXP data, SEXP sampleData, SEXP dataRngSeed)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  // Read data
  std::map<String, MultiArray> data_map = read_data_list(data);

  Bool sample_data = Rcpp::as<Bool>(sampleData);
  Size data_rng_seed = 0;
  if (sample_data)
  {
    data_rng_seed = Rcpp::as<Size>(dataRngSeed);

    if (verbosity>0)
      rbiips_cout << INDENT_STRING << "data.rng.seed = " << data_rng_seed << endl;
  }

  // Compile model
  if (! p_console->Compile(data_map, sample_data, data_rng_seed, verbosity>0))
    throw RuntimeError("Failed to compile model.");

  VOID_END_RBIIPS
}


RcppExport void print_graphviz(SEXP pConsole, SEXP dotFileName)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  String dot_file_name = Rcpp::as<String>(dotFileName);

  if (verbosity>0)
    rbiips_cout << PROMPT_STRING << "Writing dot file in: " << dot_file_name << endl;

  std::ofstream ofs(dot_file_name.c_str());

  if (ofs.fail())
    throw RuntimeError(String("Failed to open file ") + dot_file_name);

  if (!p_console->PrintGraphviz(ofs))
    throw RuntimeError("Failed to print dot file.");

  VOID_END_RBIIPS
}


RcppExport void set_default_monitors(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  if (verbosity>0)
    rbiips_cout << PROMPT_STRING << "Setting default filter monitors for backward smoothing step" << endl;

  if (!p_console->SetDefaultFilterMonitors())
    throw RuntimeError("Failed to set default filter monitors");

  VOID_END_RBIIPS
}


RcppExport void set_filter_monitors(SEXP pConsole, SEXP varNames)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Rcpp::StringVector monitored_var(varNames);

  if (verbosity>0)
    rbiips_cout << PROMPT_STRING << "Filter monitoring variables:";

  for (Size i=0; i<monitored_var.size(); ++i)
  {
    String name(monitored_var[i]);

    if (p_console->SetFilterMonitor(name))
    {
      if (verbosity>0)
        rbiips_cout << " " << name;
    }
  }
  rbiips_cout << endl;

  VOID_END_RBIIPS
}


RcppExport void set_smooth_tree_monitors(SEXP pConsole, SEXP varNames)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Rcpp::StringVector monitored_var(varNames);

  if (verbosity>0)
    rbiips_cout << PROMPT_STRING << "Smooth tree monitoring variables:";

  for (Size i=0; i<monitored_var.size(); ++i)
  {
    String name(monitored_var[i]);

    if (p_console->SetSmoothTreeMonitor(name))
    {
      if (verbosity>0)
        rbiips_cout << " " << name;
    }
  }
  rbiips_cout << endl;

  VOID_END_RBIIPS
}


RcppExport void set_smooth_monitors(SEXP pConsole, SEXP varNames)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  if (verbosity>0)
    rbiips_cout << PROMPT_STRING << "Smoother monitoring variables:";

  Rcpp::StringVector monitored_var(varNames);

  for (Size i=0; i<monitored_var.size(); ++i)
  {
    String name(monitored_var[i]);

    if (p_console->SetSmoothMonitor(name))
    {
      if (verbosity>0)
        rbiips_cout << " " << name;
    }
  }
  rbiips_cout << endl;

  VOID_END_RBIIPS
}


RcppExport void build_smc_sampler(SEXP pConsole, SEXP nParticles, SEXP smcRngSeed, SEXP prior)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Size n_part = Rcpp::as<Size>(nParticles);
  Size smc_rng_seed = Rcpp::as<Size>(smcRngSeed);
  Bool prior_flag = Rcpp::as<Size>(prior);

  if (verbosity>0)
  {
    rbiips_cout << PROMPT_STRING << "Building SMC sampler";
    if (prior_flag)
      rbiips_cout << " with prior mutation";
    rbiips_cout << endl;
    rbiips_cout << INDENT_STRING << "n.part = " << n_part << endl;
    rbiips_cout << INDENT_STRING << "smc.rng.seed = " << smc_rng_seed << endl;
  }

  if (!p_console->BuildSampler(n_part, smc_rng_seed, prior_flag, false))
    throw RuntimeError("Failed to build sampler.");

  VOID_END_RBIIPS
}


RcppExport SEXP run_smc_sampler(SEXP pConsole, SEXP essThreshold, SEXP resampleType)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  String resample_type = Rcpp::as<String>(resampleType);
  Scalar ess_threshold = Rcpp::as<Scalar>(essThreshold);
  Scalar log_norm_const;

  if (verbosity>0)
  {
    rbiips_cout << PROMPT_STRING << "Running SMC sampler" << endl;
    rbiips_cout << INDENT_STRING << "rs.thres = " << ess_threshold << endl;
    rbiips_cout << INDENT_STRING << "rs.type = " << resample_type << endl;
  }

  if (!p_console->RunForwardSampler(resample_type, ess_threshold, log_norm_const, false, verbosity>0))
    throw RuntimeError("Failed to run SMC sampler.");

  if (verbosity>0)
    rbiips_cout << INDENT_STRING << "log-normalizing constant = " << log_norm_const << endl;

  return Rcpp::wrap(log_norm_const);

  END_RBIIPS
}


static SEXP get_monitors(const std::map<String, NodeArrayMonitor> & monitorsMap, const String & type)
{
  Rcpp::List monitors_list;

  std::map<String, NodeArrayMonitor>::const_iterator it_map;
  for (it_map = monitorsMap.begin(); it_map != monitorsMap.end(); ++it_map)
  {
    const String & name = it_map->first;
    const NodeArrayMonitor & monitor = it_map->second;

    Rcpp::List particles_list;

    {
      // dim
      Rcpp::IntegerVector dim(monitor.GetValues().Dim().begin(), monitor.GetValues().Dim().end());
      Rcpp::IntegerVector dim_ess(monitor.GetESS().Dim().begin(), monitor.GetESS().Dim().end());

      // names(dim)
      Rcpp::CharacterVector dim_names(dim.size(), "");
      dim_names[dim_names.size()-1] = "particle";

      dim.attr("names") = dim_names;

      Size len = monitor.GetValues().Dim().Length();
      Size len_ess = monitor.GetESS().Dim().Length();
      Rcpp::NumericVector values(len);
      Rcpp::NumericVector weights(len);
      Rcpp::NumericVector ess(len_ess);

      std::replace_copy(monitor.GetValues().Values().begin(), monitor.GetValues().Values().end(), values.begin(), BIIPS_REALNA, NA_REAL);
      std::copy(monitor.GetWeights().Values().begin(), monitor.GetWeights().Values().end(), weights.begin());
      std::copy(monitor.GetESS().Values().begin(), monitor.GetESS().Values().end(), ess.begin());

      values.attr("dim") = dim;
      weights.attr("dim") = dim;
      ess.attr("dim") = dim_ess;

      Rcpp::List particles;
      particles["values"] = values;
      particles["weights"] = weights;
      particles["ess"] = ess;
      particles["variable.name"] = Rcpp::wrap(name);
      particles["type"] = Rcpp::wrap(type);

      particles_list[type] = particles;
    }

    monitors_list[name] = particles_list;
  }

  return monitors_list;
}


RcppExport SEXP get_filter_monitors(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  std::map<String, NodeArrayMonitor> monitors_map;

  if (!p_console->DumpFilterMonitors(monitors_map))
    throw RuntimeError("Failed to dump filter monitors.");

  return get_monitors(monitors_map, "filtering");
  END_RBIIPS
}


RcppExport SEXP get_smooth_tree_monitors(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  std::map<String, NodeArrayMonitor> monitors_map;

  if (!p_console->DumpSmoothTreeMonitors(monitors_map))
    throw RuntimeError("Failed to dump smooth tree monitors.");

  return get_monitors(monitors_map, "smoothing");
  END_RBIIPS
}


RcppExport SEXP get_smooth_monitors(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  std::map<String, NodeArrayMonitor> monitors_map;

  if (!p_console->DumpSmoothMonitors(monitors_map))
    throw RuntimeError("Failed to dump filter monitors.");

  return get_monitors(monitors_map, "backward.smoothing");
  END_RBIIPS
}


RcppExport void run_backward_smoother(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  if (verbosity>0)
    rbiips_cout << PROMPT_STRING << "Running backward smoother" << endl;

  if (!p_console->RunBackwardSmoother(false, verbosity>0))
    throw RuntimeError("Failed to run backward smoother.");

  VOID_END_RBIIPS
}


RcppExport SEXP get_variable_names(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);
  const Types<String>::Array & names = p_console->VariableNames();
  Rcpp::StringVector names_vec(names.begin(), names.end());
  return names_vec;
  END_RBIIPS
}

