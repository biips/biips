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


template<typename StorageOrderType>
static std::map<String, MultiArray> writeDataTable(SEXP data);

template<>
std::map<String, MultiArray> writeDataTable<ColumnMajorOrder>(SEXP data)
{
  std::map<String, MultiArray> data_map;

  if (verbosity>0)
    rbiips_cout << PROMPT_STRING << "Writing data table" << endl;

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


template<typename StorageOrderType>
static SEXP readDataTable(const std::map<String, MultiArray> & dataMap);

template<>
SEXP readDataTable<ColumnMajorOrder>(const std::map<String, MultiArray> & dataMap)
{
  if (verbosity>0)
    rbiips_cout << PROMPT_STRING << "Reading data table" << endl;

  Rcpp::List data_list;

  if (verbosity>0)
    rbiips_cout << INDENT_STRING << "Variables:";

  Rcpp::CharacterVector names;
  std::map<String, MultiArray>::const_iterator it_table = dataMap.begin();
  for (; it_table!=dataMap.end(); ++it_table)
  {
    const String & var_name = it_table->first;
    const MultiArray & values_array = it_table->second;

    // dim
    Rcpp::IntegerVector dim(values_array.Dim().begin(), values_array.Dim().end());

    Size len = values_array.Dim().Length();
    Rcpp::NumericVector values(len);

    std::replace_copy(values_array.Values().begin(), values_array.Values().end(), values.begin(), BIIPS_REALNA, NA_REAL);

    values.attr("dim") = dim;

    data_list[var_name] = values;

    if (verbosity>0)
      rbiips_cout << " " << var_name;
  }
  if (verbosity>0)
    rbiips_cout << endl;

  return data_list;
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
  std::map<String, MultiArray> data_map = writeDataTable<MultiArray::StorageOrderType>(data);

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


RcppExport SEXP get_data(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  std::map<String, MultiArray> data_table;

  if (! p_console->DumpData(data_table))
    throw RuntimeError("Failed to read data.");

  return readDataTable<MultiArray::StorageOrderType>(data_table);
  END_RBIIPS
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


RcppExport void build_smc_sampler(SEXP pConsole, SEXP prior)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Bool prior_flag = Rcpp::as<Size>(prior);

  if (verbosity>0)
  {
    rbiips_cout << PROMPT_STRING << "Building SMC sampler";
    if (prior_flag)
      rbiips_cout << " with prior mutation";
    rbiips_cout << endl;
  }

  if (!p_console->BuildSampler(prior_flag, false))
    throw RuntimeError("Failed to build sampler.");

  VOID_END_RBIIPS
}


RcppExport SEXP run_smc_sampler(SEXP pConsole, SEXP nParticles, SEXP smcRngSeed, SEXP essThreshold, SEXP resampleType)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Size n_part = Rcpp::as<Size>(nParticles);
  Size smc_rng_seed = Rcpp::as<Size>(smcRngSeed);
  String resample_type = Rcpp::as<String>(resampleType);
  Scalar ess_threshold = Rcpp::as<Scalar>(essThreshold);
  Scalar log_norm_const;

  if (verbosity>0)
  {
    rbiips_cout << PROMPT_STRING << "Running SMC sampler" << endl;
    rbiips_cout << INDENT_STRING << "n.part = " << n_part << endl;
    rbiips_cout << INDENT_STRING << "smc.rng.seed = " << smc_rng_seed << endl;
    rbiips_cout << INDENT_STRING << "rs.thres = " << ess_threshold << endl;
    rbiips_cout << INDENT_STRING << "rs.type = " << resample_type << endl;
  }

  if (!p_console->RunForwardSampler(n_part, smc_rng_seed, resample_type, ess_threshold, log_norm_const, false, verbosity>0))
    throw RuntimeError("Failed to run SMC sampler.");

  if (verbosity>0)
    rbiips_cout << INDENT_STRING << "log-normalizing constant = " << log_norm_const << endl;

  return Rcpp::wrap(log_norm_const);

  END_RBIIPS
}


template<typename StorageOrderType>
static SEXP getMonitors(const std::map<String, NodeArrayMonitor> & monitorsMap, const String & type);

template<>
SEXP getMonitors<ColumnMajorOrder>(const std::map<String, NodeArrayMonitor> & monitorsMap, const String & type)
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
      Rcpp::LogicalVector discrete(len_ess);

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
      particles["discrete"] = discrete;

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

  return getMonitors<MultiArray::StorageOrderType>(monitors_map, "filtering");

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

  return getMonitors<MultiArray::StorageOrderType>(monitors_map, "smoothing");
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

  return getMonitors<MultiArray::StorageOrderType>(monitors_map, "backward.smoothing");
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


RcppExport SEXP get_sorted_nodes(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Rcpp::DataFrame nodes_data_frame;

  Size graph_size;
  if (!p_console->GraphSize(graph_size))
    throw RuntimeError("Failed to get graph size.");

  {
    Rcpp::IntegerVector node_ids(graph_size);
    Types<Size>::Array node_ids_vec;
    if (!p_console->DumpNodeIds(node_ids_vec))
      throw RuntimeError("Failed to dump node ids.");
    std::replace_copy(node_ids_vec.begin(), node_ids_vec.end(), node_ids.begin(), int(NULL_NODEID), NA_INTEGER);
    nodes_data_frame["id"] = node_ids;
  }
  {
    Rcpp::StringVector node_names(graph_size);
    Types<String>::Array node_names_vec;
    if (!p_console->DumpNodeNames(node_names_vec))
      throw RuntimeError("Failed to dump node names.");
    std::copy(node_names_vec.begin(), node_names_vec.end(), node_names.begin());
    nodes_data_frame["name"] = node_names;
  }
  {
    Rcpp::StringVector node_types(graph_size);
    Types<NodeType>::Array node_types_vec;
    if (!p_console->DumpNodeTypes(node_types_vec))
      throw RuntimeError("Failed to dump node types.");
    for (Size i=0; i<graph_size; ++i)
    {
      switch (node_types_vec[i])
      {
        case CONSTANT:
          node_types[i] = "Constant";
          break;
        case Biips::LOGICAL:
          node_types[i] = "Logical";
          break;
        case STOCHASTIC:
          node_types[i] = "Stochastic";
          break;
        default:
          break;
      }
    }
    nodes_data_frame["type"] = node_types;
  }
  {
    Rcpp::LogicalVector node_observed(graph_size);
    Flags node_obs_vec;
    if (!p_console->DumpNodeObserved(node_obs_vec))
      throw RuntimeError("Failed to dump node observed boolean.");
    std::copy(node_obs_vec.begin(), node_obs_vec.end(), node_observed.begin());
    nodes_data_frame["observed"] = node_observed;
  }

  return nodes_data_frame;
  END_RBIIPS
}


RcppExport SEXP is_sampler_built(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  return Rcpp::wrap(p_console->SamplerBuilt());
  END_RBIIPS
}


RcppExport SEXP get_node_samplers(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Rcpp::DataFrame nodes_data_frame;

  Size graph_size;
  if (!p_console->GraphSize(graph_size))
    throw RuntimeError("Failed to get graph size.");

  {
    Rcpp::IntegerVector node_iterations(graph_size);
    Types<Size>::Array node_iterations_vec;
    if (!p_console->DumpNodeIterations(node_iterations_vec))
      throw RuntimeError("Failed to dump node iterations.");
    std::replace_copy(node_iterations_vec.begin(), node_iterations_vec.end(), node_iterations.begin(), int(BIIPS_SIZENA), NA_INTEGER);
    nodes_data_frame["iteration"] = node_iterations;
  }
  {
    Rcpp::StringVector node_samplers(graph_size);
    Types<String>::Array node_samplers_vec;
    if (!p_console->DumpNodeSamplers(node_samplers_vec))
      throw RuntimeError("Failed to dump node samplers.");
    std::copy(node_samplers_vec.begin(), node_samplers_vec.end(), node_samplers.begin());
    nodes_data_frame["sampler"] = node_samplers;
  }

  return nodes_data_frame;
  END_RBIIPS
}
