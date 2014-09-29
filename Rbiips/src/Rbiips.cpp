//                                               -*- C++ -*-
/*
 * Rbiips package for GNU R is an interface to Biips C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * Rbiips is derived software based on:
 * Biips, Copyright (C) Inria, 2012
 * rjags, Copyright (C) Martyn Plummer, 2002-2010
 * Rcpp, Copyright (C) Dirk Eddelbuettel and Romain Francois, 2009-2011
 *
 * This file is part of Rbiips.
 *
 * Rbiips is free software: you can redistribute it and/or modify
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

/*! \file Rbiips.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "Rbiips_utils.h"
#include "RFunction.h"
#include "RDistribution.h"
#include <compiler/Compiler.hpp>
#include <fstream>
#include <iostream/ProgressBar.hpp>
#include <BiipsVersion.hpp>

RcppExport SEXP get_version()
{
  BEGIN_RBIIPS
  return Rcpp::wrap(BIIPS_VERSION());
  END_RBIIPS
}


RcppExport SEXP verbosity(SEXP verb)
{
  BEGIN_RBIIPS
  Size old_verb = VERBOSITY;
  if (!Rf_isNull(verb))
    VERBOSITY = Rcpp::as<Size>(verb);
  return Rcpp::wrap(old_verb);
  END_RBIIPS
}


RcppExport SEXP prompt_string(SEXP prompt)
{
  BEGIN_RBIIPS
  String old = PROMPT_STRING;
  if (!Rf_isNull(prompt))
    PROMPT_STRING = Rcpp::as<String>(prompt);
  return Rcpp::wrap(old);
  END_RBIIPS
}


RcppExport SEXP indent_size(SEXP size)
{
  BEGIN_RBIIPS
  Size old = INDENT_SIZE;
  if (!Rf_isNull(size))
  {
    INDENT_SIZE = Rcpp::as<Size>(size);
    INDENT_STRING = String(INDENT_SIZE, INDENT_CHAR);
  }
  return Rcpp::wrap(old);
  END_RBIIPS
}


RcppExport void message(SEXP mess)
{
  BEGIN_RBIIPS
  rbiips_cout << PROMPT_STRING << Rcpp::as<String>(mess) << endl;
  VOID_END_RBIIPS
}


RcppExport void indent_message(SEXP mess)
{
  BEGIN_RBIIPS
  rbiips_cout << INDENT_STRING << Rcpp::as<String>(mess) << endl;
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
  if (! p_console->CheckModel(Rcpp::as<String>(modelFileName), VERBOSITY))
    throw RuntimeError("Model syntax is incorrect.");

  VOID_END_RBIIPS
}

RcppExport void add_function(SEXP f_name, SEXP nargs, SEXP fun_eval, SEXP fun_dim, SEXP fun_check_param, SEXP fun_is_discrete) {
  BEGIN_RBIIPS
       String name = Rcpp::as<String>(f_name);
       Size npar = Rcpp::as<Size>(nargs);
       Rcpp::Function feval = Rcpp::as<Rcpp::Function>(fun_eval);
       Rcpp::Function fdim = Rcpp::as<Rcpp::Function>(fun_dim);
       Rcpp::Function fcheckparam = Rcpp::as<Rcpp::Function>(fun_check_param);
       Rcpp::Function fisdiscrete = Rcpp::as<Rcpp::Function>(fun_is_discrete);
       if (Compiler::FuncTab().Contains(name))
       {
         if (Compiler::FuncTab().IsLocked(name))
           throw RuntimeError(String("Can't add function: ")
                              + name + " is an existing locked function.");
         else
           rbiips_cerr << "Warning: replacing existing function " << name << endl;
       }
       if (!Compiler::FuncTab().Insert(Function::Ptr(new RFunction(name, npar, feval, fdim, fcheckparam, fisdiscrete))))
       {
         throw RuntimeError(String("Could not add function") + name);
       }
  VOID_END_RBIIPS
}

RcppExport void add_distribution(SEXP f_name, SEXP nargs, SEXP fun_sample, SEXP fun_dim, SEXP fun_check_param, SEXP fun_is_discrete) {
  BEGIN_RBIIPS
       String name = Rcpp::as<String>(f_name);
       Size npar = Rcpp::as<Size>(nargs);
       Rcpp::Function fsample = Rcpp::as<Rcpp::Function>(fun_sample);
       Rcpp::Function fdim = Rcpp::as<Rcpp::Function>(fun_dim);
       Rcpp::Function fcheckparam = Rcpp::as<Rcpp::Function>(fun_check_param);
       Rcpp::Function fisdiscrete = Rcpp::as<Rcpp::Function>(fun_is_discrete);
       if (Compiler::DistTab().Contains(name))
       {
         if (Compiler::DistTab().IsLocked(name))
           throw RuntimeError(String("Can't add distribution: ")
                              + name + " is an existing locked distribution.");
         else
           rbiips_cerr << "Warning: replacing existing distribution " << name << endl;
       }
       if (!Compiler::DistTab().Insert(Distribution::Ptr(new RDistribution(name, npar, fsample, fdim, fcheckparam, fisdiscrete))))
       {
         throw RuntimeError(String("Could not add distribution") + name);
       }
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
  Size data_rng_seed = Rcpp::as<Size>(dataRngSeed);

  // Compile model
  if (! p_console->Compile(data_map, sample_data, data_rng_seed, VERBOSITY))
    throw RuntimeError("Failed to compile model.");
  if (sample_data && VERBOSITY>1)
    rbiips_cout << INDENT_STRING << "data.rng.seed = " << data_rng_seed << endl;

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


RcppExport SEXP change_data(SEXP pConsole, SEXP varName, SEXP lower, SEXP upper,
                            SEXP data, SEXP mcmc)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Rcpp::StringVector var(varName);
  String name(var[0]);
  IndexRange range = makeRange(lower, upper);

  // write data
  Rcpp::NumericVector r_vec(data);
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

  Bool ok = p_console->ChangeData(name, range,
                                  marray, Rcpp::as<Bool>(mcmc), VERBOSITY);

  return Rcpp::wrap(ok);

  END_RBIIPS
}


RcppExport SEXP sample_data(SEXP pConsole, SEXP varName, SEXP lower, SEXP upper,
                            SEXP rngSeed)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Rcpp::StringVector var(varName);
  String name(var[0]);
  IndexRange range = makeRange(lower, upper);

  MultiArray data;

  Bool ok = p_console->SampleData(name, range, data, Rcpp::as<Size>(rngSeed),
                                  VERBOSITY);
  if (!ok)
    throw RuntimeError("Failed to sample data.");

  // read data
  Rcpp::IntegerVector dim(data.Dim().begin(), data.Dim().end());

  Size len = data.Dim().Length();
  Rcpp::NumericVector values(len);

  std::replace_copy(data.Values().begin(), data.Values().end(), values.begin(),
                    BIIPS_REALNA, NA_REAL);

  values.attr("dim") = dim;

  return values;

  END_RBIIPS
}

RcppExport void remove_data(SEXP pConsole, SEXP varName, SEXP lower, SEXP upper)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Rcpp::StringVector var(varName);
  String name(var[0]);
  IndexRange range = makeRange(lower, upper);

  if (!p_console->RemoveData(name, range ,VERBOSITY))
    throw RuntimeError("Failed to remove data.");

  VOID_END_RBIIPS
}


RcppExport void print_graphviz(SEXP pConsole, SEXP dotFileName)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  String dot_file_name = Rcpp::as<String>(dotFileName);

  if (VERBOSITY)
    rbiips_cout << PROMPT_STRING << "Writing dot file in: "
                << dot_file_name << endl;

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

  if (VERBOSITY>1)
    rbiips_cout << PROMPT_STRING << "Setting default filter monitors for backward smoothing step" << endl;

  if (!p_console->SetDefaultFilterMonitors())
    throw RuntimeError("Failed to set default filter monitors");

  VOID_END_RBIIPS
}


RcppExport void set_filter_monitors(SEXP pConsole, SEXP varNames, SEXP lower, SEXP upper)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Rcpp::StringVector monitored_var(varNames);
  Rcpp::List monitored_lower(lower);
  Rcpp::List monitored_upper(upper);

  if (VERBOSITY>1)
    rbiips_cout << PROMPT_STRING << "Filter monitoring variables:";

  for (int i=0; i<monitored_var.size(); ++i)
  {
    String name(monitored_var[i]);
    IndexRange range = makeRange(monitored_lower[i], monitored_upper[i]);

    Bool ok = p_console->SetFilterMonitor(name, range);
    if (ok && VERBOSITY>1)
    {
      rbiips_cout << " " << name;
      if (!range.IsNull())
        rbiips_cout << range;
    }
  }
  if (VERBOSITY>1)
    rbiips_cout << endl;

  VOID_END_RBIIPS
}


RcppExport void set_gen_tree_smooth_monitors(SEXP pConsole, SEXP varNames, SEXP lower, SEXP upper)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Rcpp::StringVector monitored_var(varNames);
  Rcpp::List monitored_lower(lower);
  Rcpp::List monitored_upper(upper);

  if (VERBOSITY>1)
    rbiips_cout << PROMPT_STRING << "Smooth tree monitoring variables:";

  for (int i=0; i<monitored_var.size(); ++i)
  {
    String name(monitored_var[i]);
    IndexRange range = makeRange(monitored_lower[i], monitored_upper[i]);

    Bool ok = p_console->SetGenTreeSmoothMonitor(name, range);
    if (ok && VERBOSITY>1)
    {
      rbiips_cout << " " << name;
      if (!range.IsNull())
        rbiips_cout << range;
    }
  }
  if (VERBOSITY>1)
    rbiips_cout << endl;

  VOID_END_RBIIPS
}


RcppExport void set_backward_smooth_monitors(SEXP pConsole, SEXP varNames, SEXP lower, SEXP upper)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);
  Rcpp::List monitored_lower(lower);
  Rcpp::List monitored_upper(upper);

  if (VERBOSITY>1)
    rbiips_cout << PROMPT_STRING << "Backward smoother monitoring variables:";

  Rcpp::StringVector monitored_var(varNames);

  for (int i=0; i<monitored_var.size(); ++i)
  {
    String name(monitored_var[i]);
    IndexRange range = makeRange(monitored_lower[i], monitored_upper[i]);

    Bool ok = p_console->SetBackwardSmoothMonitor(name, range);
    if (ok && VERBOSITY>1)
    {
      rbiips_cout << " " << name;
      if (!range.IsNull())
        rbiips_cout << range;
    }
  }
  if (VERBOSITY>1)
    rbiips_cout << endl;

  VOID_END_RBIIPS
}


RcppExport SEXP is_filter_monitored(SEXP pConsole, SEXP varNames, SEXP lower, SEXP upper, SEXP check_released)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Rcpp::StringVector monitored_var(varNames);
  Rcpp::List monitored_lower(lower);
  Rcpp::List monitored_upper(upper);

  for (int i=0; i<monitored_var.size(); ++i)
  {
    String name(monitored_var[i]);
    IndexRange range = makeRange(monitored_lower[i], monitored_upper[i]);

    if (!p_console->IsFilterMonitored(name, range, Rcpp::as<Bool>(check_released)))
      return Rcpp::wrap(false);
  }

  return Rcpp::wrap(true);

  END_RBIIPS
}


RcppExport SEXP is_gen_tree_smooth_monitored(SEXP pConsole, SEXP varNames, SEXP lower, SEXP upper, SEXP check_released)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Rcpp::StringVector monitored_var(varNames);
  Rcpp::List monitored_lower(lower);
  Rcpp::List monitored_upper(upper);

  for (int i=0; i<monitored_var.size(); ++i)
  {
    String name(monitored_var[i]);
    IndexRange range = makeRange(monitored_lower[i], monitored_upper[i]);

    if (!p_console->IsGenTreeSmoothMonitored(name, range, Rcpp::as<Bool>(check_released)))
      return Rcpp::wrap(false);
  }

  return Rcpp::wrap(true);

  END_RBIIPS
}


RcppExport SEXP is_backward_smooth_monitored(SEXP pConsole, SEXP varNames, SEXP lower, SEXP upper, SEXP check_released)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Rcpp::StringVector monitored_var(varNames);
  Rcpp::List monitored_lower(lower);
  Rcpp::List monitored_upper(upper);

  for (int i=0; i<monitored_var.size(); ++i)
  {
    String name(monitored_var[i]);
    IndexRange range = makeRange(monitored_lower[i], monitored_upper[i]);

    if (!p_console->IsBackwardSmoothMonitored(name, range, Rcpp::as<Bool>(check_released)))
      return Rcpp::wrap(false);
  }

  return Rcpp::wrap(true);

  END_RBIIPS
}


RcppExport void build_smc_sampler(SEXP pConsole, SEXP prior)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Bool prior_flag = Rcpp::as<Bool>(prior);

  if (!p_console->BuildSampler(prior_flag, VERBOSITY))
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

  Bool ok = p_console->RunForwardSampler(n_part, smc_rng_seed, resample_type, ess_threshold, VERBOSITY, VERBOSITY);

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


RcppExport SEXP sample_gen_tree_smooth_particle(SEXP pConsole, SEXP smcRngSeed)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  std::map<String, MultiArray> sampled_value_map;

  if (!p_console->SampleGenTreeSmoothParticle(Rcpp::as<Size>(smcRngSeed), sampled_value_map))
    throw RuntimeError("Failed to sample smooth particle.");

  return readDataTable<MultiArray::StorageOrderType>(sampled_value_map);

  END_RBIIPS
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


RcppExport SEXP get_gen_tree_smooth_monitors(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  std::map<String, NodeArrayMonitor> monitors_map;

  if (!p_console->DumpGenTreeSmoothMonitors(monitors_map))
    throw RuntimeError("Failed to dump smooth monitors.");

  return getMonitors<MultiArray::StorageOrderType>(monitors_map, "smoothing");
  END_RBIIPS
}


RcppExport SEXP get_backward_smooth_monitors(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  std::map<String, NodeArrayMonitor> monitors_map;

  if (!p_console->DumpBackwardSmoothMonitors(monitors_map))
    throw RuntimeError("Failed to dump smooth monitors.");

  return getMonitors<MultiArray::StorageOrderType>(monitors_map, "backward.smoothing");
  END_RBIIPS
}


RcppExport void clear_filter_monitors(SEXP pConsole, SEXP release_only)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  if (!p_console->ClearFilterMonitors(Rcpp::as<Bool>(release_only)))
    throw RuntimeError("Failed to clear filter monitors.");

  VOID_END_RBIIPS
}


RcppExport void clear_gen_tree_smooth_monitors(SEXP pConsole, SEXP release_only)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  if (!p_console->ClearGenTreeSmoothMonitors(Rcpp::as<Bool>(release_only)))
    throw RuntimeError("Failed to clear smooth monitors.");

  VOID_END_RBIIPS
}


RcppExport void clear_backward_smooth_monitors(SEXP pConsole, SEXP release_only)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  if (!p_console->ClearBackwardSmoothMonitors(Rcpp::as<Bool>(release_only)))
    throw RuntimeError("Failed to clear smooth monitors.");

  VOID_END_RBIIPS
}



RcppExport void run_backward_smoother(SEXP pConsole)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  if (!p_console->RunBackwardSmoother(VERBOSITY, VERBOSITY))
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
          node_types[i] = "const";
          break;
        case Biips::LOGICAL:
          node_types[i] = "logic";
          break;
        case STOCHASTIC:
          node_types[i] = "stoch";
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
    throw RuntimeError("Failed to get log prior density.");

  if (isNA(prior))
    prior = NA_REAL;

  return Rcpp::wrap(prior);
  END_RBIIPS
}


RcppExport SEXP get_fixed_support(SEXP pConsole, SEXP varName, SEXP lower, SEXP upper)
{
  BEGIN_RBIIPS
  checkConsole(pConsole);
  Rcpp::XPtr<Console> p_console(pConsole);

  Rcpp::StringVector var(varName);

  String name(var[0]);
  IndexRange range = makeRange(lower, upper);
  ValArray lower_bound;
  ValArray upper_bound;

  if(!p_console->GetFixedSupport(lower_bound, upper_bound, name, range))
    throw RuntimeError("Failed to get fixed support.");

  Size n = lower_bound.size();

  Rcpp::NumericVector support(n*2);

  std::copy(lower_bound.begin(), lower_bound.end(), support.begin());
  std::copy(upper_bound.begin(), upper_bound.end(), support.begin()+n);

  Rcpp::Dimension supp_dim(n, 2);
  support.attr("dim") = supp_dim;

  return support;
  END_RBIIPS
}
