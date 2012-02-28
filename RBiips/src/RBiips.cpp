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
#include "iostream/outStream.hpp"
#include "iostream/ProgressBar.hpp"
#include "BiipsVersion.hpp"

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
//      std::copy(r_vec.begin(), r_vec.end(), p_val->begin());
//      std::replace(p_val->begin(), p_val->end(), Scalar(NA_REAL), BIIPS_REALNA);
      std::replace_copy(r_vec.begin(), r_vec.end(), p_val->begin(), NA_REAL, BIIPS_REALNA);
      marray.SetPtr(p_dim, p_val);
    }
    else
    {
      Rcpp::IntegerVector r_dim = r_vec.attr("dim");
      DimArray::Ptr p_dim(new DimArray(r_dim.begin(), r_dim.end()));
      ValArray::Ptr p_val(new ValArray(r_vec.size()));
//      std::copy(r_vec.begin(), r_vec.end(), p_val->begin());
//      std::replace(p_val->begin(), p_val->end(), Scalar(NA_REAL), BIIPS_REALNA);
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

//    std::copy(values_array.Values().begin(), values_array.Values().end(), values.begin());
//    std::replace(values.begin(), values.end(), BIIPS_REALNA, Scalar(NA_REAL));
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


RcppExport SEXP get_version()
{
  BEGIN_RBIIPS
  return Rcpp::wrap(BIIPS_VERSION());
  END_RBIIPS
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


RcppExport SEXP change_data(SEXP pConsole, SEXP data)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  // Read data
  std::map<String, MultiArray> data_map = writeDataTable<MultiArray::StorageOrderType>(data);

  Bool ok = p_console->ChangeData(data_map, verbosity);

  return Rcpp::wrap(ok);

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


static IndexRange makeRange(const Rcpp::RObject & lower, const Rcpp::RObject & upper)
{
  if (lower.isNULL() || upper.isNULL())
    return IndexRange();

  Rcpp::IntegerVector il(lower);
  Rcpp::IntegerVector iu(upper);
  if (il.size() != iu.size())
    throw LogicError("length mismatch between lower and upper limits");

  IndexRange::Indices lind(il.begin(), il.end());
  IndexRange::Indices uind(iu.begin(), iu.end());

  IndexRange r = IndexRange(lind, uind);
  return r;
}


RcppExport void set_filter_monitors(SEXP pConsole, SEXP varNames, SEXP lower, SEXP upper)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Rcpp::StringVector monitored_var(varNames);
  Rcpp::List monitored_lower(lower);
  Rcpp::List monitored_upper(upper);

  if (verbosity>0)
    rbiips_cout << PROMPT_STRING << "Filter monitoring variables:";

  for (Size i=0; i<monitored_var.size(); ++i)
  {
    String name(monitored_var[i]);
    IndexRange range = makeRange(monitored_lower[i], monitored_upper[i]);

    if (p_console->SetFilterMonitor(name, range))
    {
      if (verbosity == 0)
        continue;
      rbiips_cout << " " << name;
      if (!range.IsNull())
        rbiips_cout << range;
    }
  }
  if (verbosity>0)
    rbiips_cout << endl;

  VOID_END_RBIIPS
}


RcppExport void set_smooth_tree_monitors(SEXP pConsole, SEXP varNames, SEXP lower, SEXP upper)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Rcpp::StringVector monitored_var(varNames);
  Rcpp::List monitored_lower(lower);
  Rcpp::List monitored_upper(upper);

  if (verbosity>0)
    rbiips_cout << PROMPT_STRING << "Smooth tree monitoring variables:";

  for (Size i=0; i<monitored_var.size(); ++i)
  {
    String name(monitored_var[i]);
    IndexRange range = makeRange(monitored_lower[i], monitored_upper[i]);

    if (p_console->SetSmoothTreeMonitor(name, range))
    {
      if (verbosity == 0)
        continue;
      rbiips_cout << " " << name;
      if (!range.IsNull())
        rbiips_cout << range;
    }
  }
  if (verbosity>0)
    rbiips_cout << endl;

  VOID_END_RBIIPS
}


RcppExport void set_smooth_monitors(SEXP pConsole, SEXP varNames, SEXP lower, SEXP upper)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);
  Rcpp::List monitored_lower(lower);
  Rcpp::List monitored_upper(upper);

  if (verbosity>0)
    rbiips_cout << PROMPT_STRING << "Smoother monitoring variables:";

  Rcpp::StringVector monitored_var(varNames);

  for (Size i=0; i<monitored_var.size(); ++i)
  {
    String name(monitored_var[i]);
    IndexRange range = makeRange(monitored_lower[i], monitored_upper[i]);

    if (p_console->SetSmoothMonitor(name, range))
    {
      if (verbosity == 0)
        continue;
      rbiips_cout << " " << name;
      if (!range.IsNull())
        rbiips_cout << range;
    }
  }
  if (verbosity>0)
    rbiips_cout << endl;

  VOID_END_RBIIPS
}


RcppExport void build_smc_sampler(SEXP pConsole, SEXP prior)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Bool prior_flag = Rcpp::as<Bool>(prior);

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


RcppExport SEXP is_sampler_built(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);
  return Rcpp::wrap(p_console->SamplerBuilt());
  END_RBIIPS
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

  if (verbosity>0)
  {
    rbiips_cout << PROMPT_STRING << "Running SMC sampler" << endl;
    rbiips_cout << INDENT_STRING << "n.part = " << n_part << endl;
    rbiips_cout << INDENT_STRING << "smc.rng.seed = " << smc_rng_seed << endl;
    rbiips_cout << INDENT_STRING << "rs.thres = " << ess_threshold << endl;
    rbiips_cout << INDENT_STRING << "rs.type = " << resample_type << endl;
  }

  Bool ok = p_console->RunForwardSampler(n_part, smc_rng_seed, resample_type, ess_threshold, false, verbosity>0);

  return Rcpp::wrap(ok);

  END_RBIIPS
}


RcppExport SEXP is_smc_sampler_at_end(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Bool ans = p_console->ForwardSamplerAtEnd();
  return Rcpp::wrap(ans);

  END_RBIIPS
}


RcppExport SEXP get_log_norm_const(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Scalar log_norm_const;
  if(!p_console->GetLogNormConst(log_norm_const))
    throw RuntimeError("Failed to get log normalizing constant.");

  return Rcpp::wrap(log_norm_const);
  END_RBIIPS
}


RcppExport void set_log_norm_const(SEXP pConsole, SEXP logNormConst)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  p_console->SetLogNormConst(Rcpp::as<Scalar>(logNormConst));

  VOID_END_RBIIPS
}


RcppExport void sample_smooth_tree_particle(SEXP pConsole, SEXP smcRngSeed)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  if (!p_console->SampleSmoothTreeParticle(Rcpp::as<Size>(smcRngSeed)))
    throw RuntimeError("Failed to sample smooth tree particle.");

  VOID_END_RBIIPS
}


RcppExport SEXP get_sampled_smooth_tree_particle(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  std::map<String, MultiArray> sampled_value_map;

  if(!p_console->DumpSampledSmoothTreeParticle(sampled_value_map))
    throw RuntimeError("Failed to get sampled smooth tree particle.");

  return readDataTable<MultiArray::StorageOrderType>(sampled_value_map);

  END_RBIIPS
}


RcppExport void set_sampled_smooth_tree_particle(SEXP pConsole, SEXP sampledValue)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  std::map<String, MultiArray> sampled_value_map = writeDataTable<MultiArray::StorageOrderType>(sampledValue);

  if(!p_console->SetSampledSmoothTreeParticle(sampled_value_map))
    throw RuntimeError("Failed to set sampled smooth tree particle.");

  VOID_END_RBIIPS
}


template<typename StorageOrderType>
static SEXP getMonitors(const std::map<String, NodeArrayMonitor> & monitorsMap, const String & type);

template<>
SEXP getMonitors<ColumnMajorOrder>(const std::map<String, NodeArrayMonitor> & monitorsMap, const String & type)
{
  Rcpp::List particles_list;

  std::map<String, NodeArrayMonitor>::const_iterator it_map;
  for (it_map = monitorsMap.begin(); it_map != monitorsMap.end(); ++it_map)
  {
    const String & name = it_map->first;
    const NodeArrayMonitor & monitor = it_map->second;

    // dim
    Rcpp::IntegerVector dim_particles(monitor.GetValues().Dim().begin(), monitor.GetValues().Dim().end());
    Rcpp::IntegerVector dim_array(monitor.GetRange().Dim().begin(), monitor.GetRange().Dim().end());

    // names(dim)
    Rcpp::CharacterVector dim_names(dim_particles.size(), "");
    dim_names[dim_names.size()-1] = "particle";

    dim_particles.attr("names") = dim_names;

    Size len = monitor.GetValues().Dim().Length();
    Rcpp::NumericVector values(len);
    const ValArray & values_val = monitor.GetValues().Values();
//    std::copy(values_val.begin(),values_val.end(), values.begin());
//    std::replace(values.begin(), values.end(), BIIPS_REALNA, Scalar(NA_REAL));
    std::replace_copy(values_val.begin(), values_val.end(), values.begin(), BIIPS_REALNA, NA_REAL);
    values.attr("dim") = dim_particles;

    const ValArray & weight_val = monitor.GetWeights().Values();
    Rcpp::NumericVector weights(weight_val.begin(), weight_val.end());
    weights.attr("dim") = dim_particles;

    const ValArray & ess_val(monitor.GetESS().Values());
    Rcpp::NumericVector ess(ess_val.begin(), ess_val.end());
    ess.attr("dim") = dim_array;

    const ValArray & discrete_val(monitor.GetDiscrete().Values());
    Rcpp::LogicalVector discrete(discrete_val.begin(), discrete_val.end());
    discrete.attr("dim") = dim_array;

    const IndexRange::Indices & lower_ind = monitor.GetRange().Lower();
    Rcpp::IntegerVector lower(lower_ind.begin(), lower_ind.end());

    const IndexRange::Indices & upper_ind = monitor.GetRange().Upper();
    Rcpp::IntegerVector upper(upper_ind.begin(), upper_ind.end());

    Rcpp::List particles;
    particles["values"] = values;
    particles["weights"] = weights;
    particles["ess"] = ess;
    particles["discrete"] = discrete;
    particles["name"] = Rcpp::wrap(monitor.GetName());
    particles["lower"] = lower;
    particles["upper"] = upper;
    particles["type"] = Rcpp::wrap(type);

    particles.attr("class") = "particles";

    particles_list[name] = particles;
  }

  return particles_list;
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
    throw RuntimeError("Failed to dump smooth monitors.");

  return getMonitors<MultiArray::StorageOrderType>(monitors_map, "backward.smoothing");
  END_RBIIPS
}


RcppExport void clear_filter_monitors(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  if (!p_console->ClearFilterMonitors())
    throw RuntimeError("Failed to clear filter monitors.");

  VOID_END_RBIIPS
}


RcppExport void clear_smooth_tree_monitors(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  if (!p_console->ClearSmoothTreeMonitors())
    throw RuntimeError("Failed to clear smooth tree monitors.");

  VOID_END_RBIIPS
}


RcppExport void clear_smooth_monitors(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  if (!p_console->ClearSmoothMonitors())
    throw RuntimeError("Failed to clear smooth monitors.");

  VOID_END_RBIIPS
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


RcppExport SEXP progress_bar(SEXP expected_count, SEXP symbol, SEXP iter_name)
{
  BEGIN_RBIIPS
  std::string symbol_str = Rcpp::as<std::string>(symbol);
  if (symbol_str.size() != 1)
    throw RuntimeError("Error in progress_bar: symbol argument must be one character sized.");

  Rcpp::XPtr<ProgressBar> p_progress_bar(new ProgressBar(Rcpp::as<unsigned long>(expected_count),
                                                    rbiips_cout, INDENT_STRING, symbol_str[0],
                                                    Rcpp::as<std::string>(iter_name)));
  return p_progress_bar;

  END_RBIIPS
}


RcppExport void restart_progress_bar(SEXP pProgressBar, SEXP expected_count, SEXP symbol, SEXP iter_name, SEXP display_bar)
{
  BEGIN_RBIIPS
  Rcpp::XPtr<ProgressBar> p_progress_bar(pProgressBar);
  std::string symbol_str = Rcpp::as<std::string>(symbol);
  if (symbol_str.size() != 1)
    throw RuntimeError("Error in restart_progress_bar: symbol argument must be one character sized.");
  p_progress_bar->restart(Rcpp::as<unsigned long>(expected_count), symbol_str[0],
                 Rcpp::as<std::string>(iter_name), Rcpp::as<bool>(display_bar));
  VOID_END_RBIIPS
}


RcppExport void advance_progress_bar(SEXP pProgressBar, SEXP count)
{
  BEGIN_RBIIPS
  Rcpp::XPtr<ProgressBar> p_progress_bar(pProgressBar);
  (*p_progress_bar) += Rcpp::as<unsigned long>(count);
  VOID_END_RBIIPS
}


RcppExport SEXP get_log_prior_density(SEXP pConsole, SEXP varName, SEXP lower, SEXP upper)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Rcpp::StringVector var(varName);

  String name(var[0]);
  IndexRange range = makeRange(lower, upper);
  Scalar prior;

  if(!p_console->GetLogPriorDensity(prior, name, range))
    throw RuntimeError("Failed to get prior density.");

  return Rcpp::wrap(prior);
  END_RBIIPS
}
