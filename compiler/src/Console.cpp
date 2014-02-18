//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
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

/*! \file Console.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from JAGS Console class
 */

extern "C"
{
  void BiipsCompiler_is_present(void)
  {
  }
}

#include "BiipsConfig.hpp"
#include "Console.hpp"
#include "compiler/ParseTree.h"
#include "compiler/Compiler.hpp"
#include "iostream/outStream.hpp"
#include "iostream/ProgressBar.hpp"
#include "model/BUGSModel.hpp"

// FIXME to be removed. Manage dynamically loaded modules
#include "BiipsBase.hpp"

#include "compiler/parser_extra.h"

#include <cstdio>

#ifdef BIIPS_DEBUG_PARSER
#include "printParseTree.hpp"
#endif

using std::endl;

// FIXME
#define BIIPS_CONSOLE_CATCH_ERRORS                                    \
    catch (NodeError & except)                                        \
    {                                                                 \
      err_ << "Error in node " <<                                     \
      pModel_->GetSymbolTable().GetName(except.GetNodeId()) << endl;  \
      err_ << except.what() << endl;                                  \
      return false;                                                   \
    }                                                                 \
    catch (RuntimeError & except)                                     \
    {                                                                 \
      err_ << "RUNTIME ERROR: " << except.what() << endl;             \
      return false;                                                   \
    }                                                                 \
    catch (LogicError & except)                                       \
    {                                                                 \
      err_ << "LOGIC ERROR: " << except.what() << endl;               \
      /*err_ << "Please send a bug report to "                          \
         << PACKAGE_BUGREPORT << endl;*/                              \
    return false;                                                     \
    }                                                                 \
    catch(const std::exception & e)                                   \
    {                                                                 \
      err_ << "STD ERROR: " << e.what() << endl;                      \
      return false;                                                   \
    }                                                                 \
    catch(...)                                                        \
    {                                                                 \
      err_ << "UNKNOWN ERROR: " << endl;                              \
      return false;                                                   \
    }

#define BIIPS_CONSOLE_CATCH_ERRORS_DELETE_MODEL                       \
    catch (NodeError & except)                                        \
    {                                                                 \
      err_ << "Error in node " <<                                     \
      pModel_->GetSymbolTable().GetName(except.GetNodeId()) << endl;  \
      err_ << except.what() << endl;                                  \
      ClearModel();                                                   \
      return false;                                                   \
    }                                                                 \
    catch (RuntimeError & except)                                     \
    {                                                                 \
      err_ << "RUNTIME ERROR: " << except.what() << endl;             \
      ClearModel();                                                   \
      return false;                                                   \
    }                                                                 \
    catch (LogicError & except)                                       \
    {                                                                 \
      err_ << "LOGIC ERROR: " << except.what() << endl;               \
      /*err_ << "Please send a bug report to "                          \
         << PACKAGE_BUGREPORT << endl;*/                              \
    ClearModel();                                                     \
    return false;                                                     \
    }                                                                 \
    catch(const std::exception & e)                                   \
    {                                                                 \
      err_ << "STD ERROR: " << e.what() << endl;                      \
      ClearModel();                                                   \
      return false;                                                   \
    }                                                                 \
    catch(...)                                                        \
    {                                                                 \
      err_ << "UNKNOWN ERROR: " << endl;                              \
      ClearModel();                                                   \
      return false;                                                   \
    }

namespace Biips
{
  String PROMPT_STRING = "* ";
  Size INDENT_SIZE = 2;
  const char INDENT_CHAR = ' ';
  String INDENT_STRING(INDENT_SIZE, INDENT_CHAR);

  Console::Console(std::ostream & out, std::ostream & err)
      : out_(out), err_(err), pModel_(NULL), pData_(NULL), pRelations_(NULL),
          pVariables_(NULL), lockBackward_(false), logNormConst_(BIIPS_NEGINF)
  {
  }

  void Console::clearParseTrees()
  {
    delete pData_;
    delete pRelations_;
    if (pVariables_)
    {
      for (Size i = 0; i < pVariables_->size(); ++i)
        delete (*pVariables_)[i];
      delete pVariables_;
    }
  }

  Console::~Console()
  {
    clearParseTrees();
    ClearModel(0);
  }

  static void getVariableNames(ParseTree const * pTree,
                               std::set<String> & names,
                               Types<String>::Array & counterstack)
  {
    /*
     * Get variables from model, ensuring that we ignore counters.
     */

    if (pTree->treeClass() == P_VAR)
    {
      Bool is_counter = false;
      for (Size i = 0; i < counterstack.size(); ++i)
      {
        if (pTree->name() == counterstack[i])
        {
          is_counter = true;
          break;
        }
      }
      if (!is_counter)
        names.insert(pTree->name());
    }

    const Types<ParseTree*>::Array & param = pTree->parameters();
    for (Types<ParseTree*>::Array::const_iterator p = param.begin();
        p != param.end(); ++p)
    {
      ParseTree * counter;

      if (*p)
      {
        //ParseTree objects of type P_BOUND can have null parameters
        switch ((*p)->treeClass())
        {
          case P_FOR:
            counter = (*p)->parameters()[0];
            for (Size i = 0; i < counter->parameters().size(); ++i)
              getVariableNames(counter->parameters()[i], names, counterstack);

            counterstack.push_back(counter->name());
            getVariableNames((*p)->parameters()[1], names, counterstack);
            counterstack.pop_back();
            break;
          default:
            getVariableNames(*p, names, counterstack);
            break;
        }
      }
    }
  }

  Bool Console::CheckModel(const String & modelFileName, Size verbosity)
  {
    if (verbosity)
      out_ << PROMPT_STRING << "Parsing model in: " << modelFileName << endl;
    if (pRelations_ || pData_ || pVariables_)
    {
      clearParseTrees();

      if (pModel_)
      {
        if (verbosity)
          out_ << PROMPT_STRING << "Replacing existing model" << endl;
        ClearModel();
      }
    }

    FILE * model_file = fopen(modelFileName.c_str(), "r");

    if (!model_file)
      throw RuntimeError(String("Failed to open file: ") + modelFileName);

    String message;
    int status = parse_bugs(model_file, pVariables_, pData_, pRelations_,
                            message);

    fclose(model_file);

#ifdef BIIPS_DEBUG_PARSER
    out_ << "============================" << endl;
    out_ << "Parsed variables:" << endl;
    out_ << "-----------------" << endl;
    if (pVariables_)
    {
      for (std::vector<ParseTree*>::iterator it_pvar = pVariables_->begin(); it_pvar != pVariables_->end(); ++it_pvar)
      {
        printParseTree(out_, *it_pvar);
        out_ << endl;
      }
    }

    out_ << "Parsed data:" << endl;
    out_ << "------------" << endl;
    if (pData_)
    {
      printParseTree(out_, pData_);
    }

    out_ << endl;
    out_ << "Parsed relations:" << endl;
    out_ << "-----------------" << endl;
    if (pRelations_)
    {
      printParseTree(out_, pRelations_);
    }
    out_ << "============================" << endl;
#endif

    if (status != 0)
    {
      err_ << endl << "Error parsing model file:" << endl << message << endl;
      //Tidy up
      clearParseTrees();
      return false;
    }

    //Get names of all variables in the model
    std::set<String> names_set;
    Types<String>::Array counter_stack;
    if (pVariables_)
    {
      for (Types<ParseTree*>::Array::const_iterator it = pVariables_->begin();
          it != pVariables_->end(); ++it)
        getVariableNames(*it, names_set, counter_stack);
    }
    if (pData_)
      getVariableNames(pData_, names_set, counter_stack);
    if (pRelations_)
      getVariableNames(pRelations_, names_set, counter_stack);

    nodeArrayNames_.assign(names_set.begin(), names_set.end());

    return true;
  }

  void Console::ClearModel(Size verbosity)
  {
    if (!pModel_)
    {
      if (verbosity)
        out_ << PROMPT_STRING << "Deleting model" << endl;

      delete pModel_;
    }
  }

  Bool Console::Compile(std::map<String, MultiArray> & dataMap, Bool genData,
                        Size dataRngSeed, Size verbosity)
  {
    if (pModel_)
    {
      if (verbosity)
        out_ << PROMPT_STRING << "Replacing existing model" << endl;
      ClearModel();
    }

    if (pData_ && genData)
    {
      // FIXME
      boost::scoped_ptr<Rng> p_datagen_rng(new Rng(dataRngSeed));

      pModel_ = new BUGSModel(true);

      Compiler compiler(*pModel_, dataMap);

      if (verbosity)
        out_ << PROMPT_STRING << "Compiling data graph" << endl;
      try
      {
        if (pVariables_)
        {
          if (verbosity)
            out_ << INDENT_STRING << "Declaring variables" << endl;
          compiler.DeclareVariables(*pVariables_);
        }
        if (verbosity)
          out_ << INDENT_STRING << "Resolving undeclared variables" << endl;
        compiler.UndeclaredVariables(pData_);

        if (verbosity)
          out_ << INDENT_STRING << "Allocating nodes" << endl;
        compiler.WriteRelations(pData_);

        Graph & data_graph = pModel_->graph();
        data_graph.Build();

        /* Check validity of data generating model */
        Types<NodeId>::ConstIterator it_node_id, it_node_id_end;
        boost::tie(it_node_id, it_node_id_end) = data_graph.GetSortedNodes();
        for (; it_node_id != it_node_id_end; ++it_node_id)
        {
          if (data_graph.GetObserved()[*it_node_id])
          {
            GraphTypes::ParentIterator it_parents, it_parents_end;
            boost::tie(it_parents, it_parents_end) = data_graph.GetParents(
                *it_node_id);
            for (; it_parents != it_parents_end; ++it_parents)
            {
              if (!data_graph.GetObserved()[*it_parents])
              {
                err_ << "Invalid data graph: observed node "
                << pModel_->GetSymbolTable().GetName(*it_node_id)
                << " has unobserved parent "
                << pModel_->GetSymbolTable().GetName(*it_parents) << std::endl;
                ClearModel();
                return false;
              }
            }
          }
        }

        if (verbosity)
        {
          out_ << INDENT_STRING << "Graph size: " << data_graph.GetSize();
          if (verbosity > 1)
          {
            out_ << " (Constant: " << data_graph.NodesSummary().at(CONSTANT);
            out_ << ", Logical: " << data_graph.NodesSummary().at(LOGICAL);
            out_ << ", Stochastic: " << data_graph.NodesSummary().at(STOCHASTIC)
                 << ")";
          }
          out_ << endl;
          if (verbosity > 1)
          {
            Size n_data_unobs_nodes = data_graph.UnobsNodesSummary().at(LOGICAL)
                                      + data_graph.UnobsNodesSummary().at(
                                          STOCHASTIC);
            out_ << INDENT_STRING << "Unobserved nodes: " << n_data_unobs_nodes;
            out_ << " (Logical: " << data_graph.UnobsNodesSummary().at(LOGICAL);
            out_ << ", Stochastic: "
                 << data_graph.UnobsNodesSummary().at(STOCHASTIC) << ")"
                 << endl;
          }
          out_ << INDENT_STRING << "Sampling data" << endl;
        }

        std::map<String, MultiArray> sampled_data_map(
            pModel_->Sample(p_datagen_rng.get()));

        // FIXME
        //        //Save data generating RNG for later use. It is owned by the
        //        //RNGFactory, not the model.
        //        datagen_rng = pModel_->rng(0);

        if (verbosity)
          out_ << INDENT_STRING << "Reading data back into data table" << endl;

        for (std::map<String, MultiArray>::const_iterator it_sampled =
            sampled_data_map.begin(); it_sampled != sampled_data_map.end();
            ++it_sampled)
        {
          const MultiArray & sampled_values = it_sampled->second;

          if (allMissing(it_sampled->second))
            continue;

          const String & name = it_sampled->first;
          if (dataMap.find(name) != dataMap.end())
          {
            //Replace any existing entry
            // FIXME: observed variables are replaced by themselves. Unnecessary !
            dataMap.erase(name);
            //            err_ << "Warning: replacing values of variable " << name << " by sampled ones." << endl;
          }
          dataMap.insert(std::make_pair(name, sampled_values));
        }

        ClearModel(0);
      }
      BIIPS_CONSOLE_CATCH_ERRORS_DELETE_MODEL
    }

    pModel_ = new BUGSModel();
    Compiler compiler(*pModel_, dataMap);

    if (verbosity)
      out_ << PROMPT_STRING << "Compiling model graph" << endl;
    try
    {
      if (pVariables_)
      {
        if (verbosity)
          out_ << INDENT_STRING << "Declaring variables" << endl;
        compiler.DeclareVariables(*pVariables_);
      }
      if (pRelations_)
      {
        if (verbosity)
          out_ << INDENT_STRING << "Resolving undeclared variables" << endl;
        compiler.UndeclaredVariables(pRelations_);

        if (verbosity)
          out_ << INDENT_STRING << "Allocating nodes" << endl;
        compiler.WriteRelations(pRelations_);
      }
      else
      {
        err_ << "Nothing to compile" << endl;
        return true;
      }
      // FIXME
      if (pModel_)
      {
        Graph & model_graph = pModel_->graph();
        if (verbosity > 1)
        {
          out_ << INDENT_STRING << "Building and checking graph" << endl;
        }
        model_graph.Build();

        if (verbosity)
        {
          out_ << INDENT_STRING << "Graph size: " << model_graph.GetSize();
          if (verbosity > 1)
          {
            out_ << " (Constant: " << model_graph.NodesSummary().at(CONSTANT);
            out_ << ", Logical: " << model_graph.NodesSummary().at(LOGICAL);
            out_ << ", Stochastic: "
                 << model_graph.NodesSummary().at(STOCHASTIC) << ")";

          }
          out_ << endl;
          if (verbosity > 1)
          {
            Size n_unobs_nodes = model_graph.UnobsNodesSummary().at(LOGICAL)
                                 + model_graph.UnobsNodesSummary().at(
                                     STOCHASTIC);
            out_ << INDENT_STRING << "Unobserved nodes: " << n_unobs_nodes;
            out_ << " (Logical: "
                 << model_graph.UnobsNodesSummary().at(LOGICAL);
            out_ << ", Stochastic: "
                 << model_graph.UnobsNodesSummary().at(STOCHASTIC) << ")"
                 << endl;
          }
        }

        // FIXME
        //        if (datagen_rng)
        //        {
        //          // Reuse the data-generation RNG, if there is one, for chain 0
        //          pModel_->setRNG(datagen_rng, 0);
        //        }
      }
      else
      {
        err_ << "No model" << endl;
        return true;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS_DELETE_MODEL;

    return true;
  }

  Bool Console::BuildSampler(Bool prior, Size verbosity)
  {
    if (!pModel_)
    {
      err_ << "Can't build SMC sampler. No model!" << endl;
      return false;
    }
    if (pModel_->graph().Empty())
    {
      err_
          << "Can't build SMC sampler. No nodes in graph (Have you compiled the model?)"
          << endl;
      return false;
    }
    try
    {
      // TODO manage this more finely
      // set all NodeSampler factories inactive if prior is true
      std::list<std::pair<NodeSamplerFactory::Ptr, Bool> >::iterator it_sampler_fact =
          ForwardSampler::NodeSamplerFactories().begin();
      for (; it_sampler_fact != ForwardSampler::NodeSamplerFactories().end();
          ++it_sampler_fact)
      {
        it_sampler_fact->second = !prior;
      }

      if (verbosity)
        out_ << PROMPT_STRING << "Assigning node samplers" << endl;

      pModel_->BuildSampler();

      if (verbosity > 1)
      {
        out_ << INDENT_STRING << "Samplers sequence :" << endl;
        pModel_->PrintSamplersSequence(out_);
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS_DELETE_MODEL

    return true;
  }

  Bool Console::SamplerBuilt()
  {
    return (pModel_ && pModel_->SamplerBuilt());
  }

  Bool Console::RunForwardSampler(Size nParticles, Size smcRngSeed,
                                  const String & rsType, Scalar essThreshold,
                                  Size verbosity, Bool progressBar)
  {
    if (!pModel_)
    {
      err_ << "Can't run SMC sampler. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't run SMC sampler. Not built!" << endl;
      return false;
    }

    try
    {
      Size n_iter = pModel_->Sampler().NIterations();

      if (verbosity)
      {
        if (n_iter > 0)
          out_ << PROMPT_STRING << "Running SMC forward sampler with "
               << nParticles << " particles" << endl;
        else
          out_ << PROMPT_STRING << "Skipping SMC forward sampler: no iterations"
               << endl;
      }

      if (n_iter == 0)
        return true;

      Types<ProgressBar>::Ptr p_show_progress;
      if (progressBar)
        p_show_progress = Types<ProgressBar>::Ptr(
            new ProgressBar(n_iter, out_, INDENT_STRING));

      // filtering

      // FIXME
      boost::scoped_ptr<Rng> p_smc_rng(new Rng(smcRngSeed));

      pModel_->InitSampler(nParticles, p_smc_rng.get(), rsType, essThreshold);

      if (p_show_progress)
        ++(*p_show_progress);
      else if (verbosity > 1)
        printSamplerState(pModel_->Sampler(), out_);

      for (Size n = 1; n < n_iter; ++n)
      {
        pModel_->IterateSampler();

        if (p_show_progress)
          ++(*p_show_progress);
        else if (verbosity > 1)
          printSamplerState(pModel_->Sampler(), out_);
      }

      // normalizing constant
      logNormConst_ = pModel_->Sampler().LogNormConst();

      lockBackward_ = false;
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ForwardSamplerAtEnd()
  {
    return (pModel_ && pModel_->SamplerBuilt() && pModel_->Sampler().AtEnd());
  }

  Bool Console::GetLogNormConst(Scalar & logNormConst)
  {
    if (!pModel_)
    {
      err_ << "Can't get log normalizing constant. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't get log normalizing constant. SMC sampler did not run!"
           << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_ << "Can't get log normalizing constant. SMC sampler did not finish!"
           << endl;
      return false;
    }
    try
    {
      logNormConst = logNormConst_;
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::SetLogNormConst(Scalar logNormConst)
  {
    if (!pModel_)
    {
      err_ << "Can't set log normalizing constant. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't set log normalizing constant. SMC sampler did not run!"
           << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_ << "Can't set log normalizing constant. SMC sampler did not finish!"
           << endl;
      return false;
    }
    try
    {
      logNormConst_ = logNormConst;
      lockBackward_ = true;
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::RunBackwardSmoother(Size verbosity, Bool progressBar)
  {
    if (!pModel_)
    {
      err_ << "Can't run backward smoother. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't run backward smoother. SMC sampler did not run!" << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_ << "Can't run backward smoother. SMC sampler did not finish!"
           << endl;
      return false;
    }
    if (lockBackward_)
    {
      err_
          << "Can't run backward smoother. Data has been changed. You must run forward SMC sampler again."
          << endl;
      return false;
    }

    try
    {
      if (pModel_->Sampler().NIterations() <= 1)
      {
        if (verbosity)
          out_ << PROMPT_STRING << "Skipping backward smoother: no iterations"
               << endl;
        return true;
      }

      pModel_->InitBackwardSmoother();

      Size n_iter = pModel_->Sampler().NIterations() - 1;

      if (verbosity)
        out_ << PROMPT_STRING << "Running backward smoother" << endl;

      Types<ProgressBar>::Ptr p_show_progress;
      if (progressBar)
        p_show_progress = Types<ProgressBar>::Ptr(
            new ProgressBar(n_iter, out_, INDENT_STRING));

      // smoothing
      for (Size n = n_iter; n > 0; --n)
      {
        pModel_->IterateBackwardSmoother();

        if (p_show_progress)
          ++(*p_show_progress);
      }

    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  // FIXME add module manager and a load module by name function
  Bool Console::LoadBaseModule(Size verbosity)
  {
    if (verbosity)
      out_ << PROMPT_STRING << "Loading Base module" << endl;

    try
    {
      loadBaseModule(Compiler::FuncTab(), Compiler::DistTab());
    }
    BIIPS_CONSOLE_CATCH_ERRORS_DELETE_MODEL

    return true;
  }

  Bool Console::SetDefaultFilterMonitors()
  {
    if (!pModel_)
    {
      err_ << "Can't set default filter monitors. No model!" << endl;
      return false;
    }

    try
    {
      pModel_->SetDefaultFilterMonitors();
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::SetFilterMonitor(const String & name, const IndexRange & range)
  {
    if (!pModel_)
    {
      err_ << "Can't set filter monitor. No model!" << endl;
      return false;
    }
    // TODO: check that sampler did not start

    try
    {
      Bool ok = pModel_->SetFilterMonitor(name, range);
      if (!ok)
      {
        err_ << "Failed to set filter monitor for variable " << name;
        if (!range.IsNull())
          err_ << range;
        err_ << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::SetGenTreeSmoothMonitor(const String & name,
                                     const IndexRange & range)
  {
    if (!pModel_)
    {
      err_ << "Can't set smooth monitor. No model!" << endl;
      return false;
    }
    // TODO: check that sampler did not start

    try
    {
      Bool ok = pModel_->SetGenTreeSmoothMonitor(name, range);
      if (!ok)
      {
        err_ << "Failed to set smooth monitor for variable " << name;
        if (!range.IsNull())
          err_ << range;
        err_ << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::SetBackwardSmoothMonitor(const String & name, const IndexRange & range)
  {
    if (!pModel_)
    {
      err_ << "Can't set backward smooth monitor. No model!" << endl;
      return false;
    }
    // TODO: check that sampler did not start

    try
    {
      Bool ok = pModel_->SetBackwardSmoothMonitor(name, range);
      if (!ok)
      {
        err_ << "Failed to set backward smooth monitor for variable " << name;
        if (!range.IsNull())
          err_ << range;
        err_ << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::IsFilterMonitored(const String & name, const IndexRange & range,
                                  Bool check_released)
  {
    if (!pModel_)
    {
      err_ << "Can't check filter monitor. No model!" << endl;
      return false;
    }
    return pModel_->IsFilterMonitored(name, range, check_released);
  }

  Bool Console::IsGenTreeSmoothMonitored(const String & name,
                                      const IndexRange & range,
                                      Bool check_released)
  {
    if (!pModel_)
    {
      err_ << "Can't check smooth monitor. No model!" << endl;
      return false;
    }
    return pModel_->IsGenTreeSmoothMonitored(name, range, check_released);
  }

  Bool Console::IsBackwardSmoothMonitored(const String & name, const IndexRange & range,
                                  Bool check_released)
  {
    if (!pModel_)
    {
      err_ << "Can't check backward smooth monitor. No model!" << endl;
      return false;
    }
    return pModel_->IsBackwardSmoothMonitored(name, range, check_released);
  }

  Bool Console::ClearFilterMonitors(Bool release_only)
  {
    if (!pModel_)
    {
      err_ << "Can't clear filter monitors. No model!" << endl;
      return false;
    }

    try
    {
      pModel_->ClearFilterMonitors(release_only);
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ClearGenTreeSmoothMonitors(Bool release_only)
  {
    if (!pModel_)
    {
      err_ << "Can't clear smooth monitors. No model!" << endl;
      return false;
    }

    try
    {
      pModel_->ClearGenTreeSmoothMonitors(release_only);
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ClearBackwardSmoothMonitors(Bool release_only)
  {
    if (!pModel_)
    {
      err_ << "Can't clear smooth monitors. No model!" << endl;
      return false;
    }

    try
    {
      pModel_->ClearBackwardSmoothMonitors(release_only);
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ExtractFilterStat(const String & name, StatTag statFeature,
                                  std::map<IndexRange, MultiArray> & statMap)
  {
    if (!pModel_)
    {
      err_ << "Can't extract filter statistic. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't extract filter statistic. SMC sampler not built!" << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_ << "Can't extract filter statistic. SMC sampler still running!"
           << endl;
      return false;
    }

    try
    {
      Bool ok = pModel_->ExtractFilterStat(name, statFeature, statMap);
      if (!ok)
      {
        err_ << "Failed to extract filter statistic for variable " << name
             << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ExtractGenTreeSmoothStat(
      const String & name, StatTag statFeature,
      std::map<IndexRange, MultiArray> & statMap)
  {
    if (!pModel_)
    {
      err_ << "Can't extract smooth statistic. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't extract smooth statistic. SMC sampler not built!"
           << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_ << "Can't extract smooth statistic. SMC sampler still running!"
           << endl;
      return false;
    }

    try
    {
      Bool ok = pModel_->ExtractGenTreeSmoothStat(name, statFeature, statMap);
      if (!ok)
      {
        err_ << "Failed to extract smooth statistic for variable " << name
             << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ExtractBackwardSmoothStat(const String & name, StatTag statFeature,
                                  std::map<IndexRange, MultiArray> & statMap)
  {
    if (!pModel_)
    {
      err_ << "Can't extract backward smoother statistic. No model!" << endl;
      return false;
    }
    if (!pModel_->SmootherInitialized())
    {
      err_
          << "Can't extract backward smoother statistic. Backward smoother not initialized!"
          << endl;
      return false;
    }
    if (!pModel_->Smoother().AtEnd())
    {
      err_
          << "Can't extract backward smoother statistic. Backward smoother still running!"
          << endl;
      return false;
    }

    try
    {
      Bool ok = pModel_->ExtractBackwardSmoothStat(name, statFeature, statMap);
      if (!ok)
      {
        err_ << "Failed to extract backward smoother statistic for variable "
             << name << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ExtractFilterPdf(const String & name,
                                 std::map<IndexRange, Histogram> & pdfMap,
                                 Size numBins, Scalar cacheFraction)
  {
    if (!pModel_)
    {
      err_ << "Can't extract filter pdf. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't extract filter pdf. SMC sampler not built!" << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_ << "Can't extract filter pdf. SMC sampler still running!" << endl;
      return false;
    }

    try
    {
      Bool ok = pModel_->ExtractFilterPdf(name, pdfMap, numBins, cacheFraction);
      if (!ok)
      {
        err_ << "Failed to extract filter pdf for variable " << name << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ExtractGenTreeSmoothPdf(const String & name,
                                     std::map<IndexRange, Histogram> & pdfMap,
                                     Size numBins, Scalar cacheFraction)
  {
    if (!pModel_)
    {
      err_ << "Can't extract smooth pdf. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't extract smooth pdf. SMC sampler not built!" << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_ << "Can't extract smooth pdf. SMC sampler still running!"
           << endl;
      return false;
    }

    try
    {
      Bool ok = pModel_->ExtractGenTreeSmoothPdf(name, pdfMap, numBins,
                                              cacheFraction);
      if (!ok)
      {
        err_ << "Failed to extract smooth pdf for variable " << name
             << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ExtractBackwardSmoothPdf(const String & name,
                                 std::map<IndexRange, Histogram> & pdfMap,
                                 Size numBins, Scalar cacheFraction)
  {
    if (!pModel_)
    {
      err_ << "Can't extract backward smoother pdf. No model!" << endl;
      return false;
    }
    if (!pModel_->SmootherInitialized())
    {
      err_
          << "Can't extract backward smoother pdf. SMC sampler not initialized!"
          << endl;
      return false;
    }
    if (!pModel_->Smoother().AtEnd())
    {
      err_ << "Can't extract backward smoother pdf. SMC sampler still running!"
           << endl;
      return false;
    }

    try
    {
      Bool ok = pModel_->ExtractBackwardSmoothPdf(name, pdfMap, numBins, cacheFraction);
      if (!ok)
      {
        err_ << "Failed to extract backward smoother pdf for variable " << name
             << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::DumpData(std::map<String, MultiArray> & dataMap)
  {
    if (!pModel_)
    {
      err_ << "Can't dump data. No model!" << endl;
      return false;
    }

    try
    {
      if (!pModel_->DumpData(dataMap))
      {
        err_ << "Failed to dump data" << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ChangeData(const String & variable, const IndexRange & range,
                           const MultiArray & data, Bool mcmc, Size verbosity)
  {
    if (!pModel_)
    {
      err_ << "Can't change data. No model!" << endl;
      return false;
    }
    if (pModel_->graph().Empty())
    {
      err_
          << "Can't change data. No nodes in graph (Have you compiled the model?)"
          << endl;
      return false;
    }
    if (pModel_->SamplerBuilt() && pModel_->Sampler().Initialized()
        && !pModel_->Sampler().AtEnd())
    {
      err_ << "Can't change data. SMC sampler is running." << endl;
      return false;
    }
    if (pModel_->SmootherInitialized() && !pModel_->Smoother().AtEnd())
    {
      err_ << "Can't change data. Backward smoother is running." << endl;
      return false;
    }
    try
    {
      if (verbosity)
      {
        out_ << PROMPT_STRING << "Changing data" << endl;
      }
      Bool rebuild_sampler;
      if (!pModel_->ChangeData(variable, range, data, mcmc, rebuild_sampler))
      {
        err_ << "Failed to change data" << endl;
        return false;
      }
      if (pModel_->SamplerBuilt() && rebuild_sampler)
        pModel_->ClearSampler();

      lockBackward_ = true;
      ClearFilterMonitors(true);
      ClearGenTreeSmoothMonitors(true);
      if (pModel_->SmootherInitialized())
        ClearBackwardSmoothMonitors(true);
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::SampleData(const String & variable, const IndexRange & range,
                           MultiArray & data, Size rngSeed, Size verbosity)
  {
    if (!pModel_)
    {
      err_ << "Can't sample data. No model!" << endl;
      return false;
    }
    if (pModel_->graph().Empty())
    {
      err_
          << "Can't sample data. No nodes in graph (Have you compiled the model?)"
          << endl;
      return false;
    }
    if (pModel_->SamplerBuilt() && pModel_->Sampler().Initialized()
        && !pModel_->Sampler().AtEnd())
    {
      err_ << "Can't sample data. SMC sampler is running." << endl;
      return false;
    }
    if (pModel_->SmootherInitialized() && !pModel_->Smoother().AtEnd())
    {
      err_ << "Can't sample data. Backward smoother is running." << endl;
      return false;
    }
    try
    {
      if (verbosity)
      {
        out_ << PROMPT_STRING << "Sampling data" << endl;
      }

      // FIXME
      boost::scoped_ptr<Rng> p_rng(new Rng(rngSeed));

      if (!pModel_->SampleData(variable, range, data, p_rng.get()))
      {
        err_ << "Failed to sample data" << endl;
        return false;
      }
      if (pModel_->SamplerBuilt())
        pModel_->ClearSampler();

      lockBackward_ = true;
      ClearFilterMonitors(true);
      ClearGenTreeSmoothMonitors(true);
      if (pModel_->SmootherInitialized())
        ClearBackwardSmoothMonitors(true);
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::RemoveData(const String & variable, const IndexRange & range,
                           Size verbosity)
  {
    if (!pModel_)
    {
      err_ << "Can't remove data. No model!" << endl;
      return false;
    }
    if (pModel_->graph().Empty())
    {
      err_
          << "Can't remove data. No nodes in graph (Have you compiled the model?)"
          << endl;
      return false;
    }
    if (pModel_->SamplerBuilt() && pModel_->Sampler().Initialized()
        && !pModel_->Sampler().AtEnd())
    {
      err_ << "Can't remove data. SMC sampler is running." << endl;
      return false;
    }
    if (pModel_->SmootherInitialized() && !pModel_->Smoother().AtEnd())
    {
      err_ << "Can't remove data. Backward smoother is running." << endl;
      return false;
    }
    try
    {
      if (verbosity)
      {
        out_ << PROMPT_STRING << "Removing data" << endl;
      }
      if (!pModel_->RemoveData(variable, range))
      {
        err_ << "Failed to remove data" << endl;
        return false;
      }
      if (pModel_->SamplerBuilt())
        pModel_->ClearSampler();

      lockBackward_ = true;
      ClearFilterMonitors(true);
      ClearGenTreeSmoothMonitors(true);
      if (pModel_->SmootherInitialized())
        ClearBackwardSmoothMonitors(true);
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::DumpFilterMonitors(
      std::map<String, NodeArrayMonitor> & particlesMap)
  {

    if (!pModel_)
    {
      err_ << "Can't dump filter monitors. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't dump filter monitors. SMC sampler not built!" << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_ << "Can't dump filter monitors. SMC sampler still running!" << endl;
      return false;
    }

    try
    {
      Bool ok = pModel_->DumpFilterMonitors(particlesMap);
      if (!ok)
      {
        err_ << "Failed to dump filter monitors" << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::DumpGenTreeSmoothMonitors(
      std::map<String, NodeArrayMonitor> & particlesMap)
  {
    if (!pModel_)
    {
      err_ << "Can't dump smooth monitors. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't dump smooth monitors. SMC sampler not built!" << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_ << "Can't dump smooth monitors. SMC sampler still running!"
           << endl;
      return false;
    }

    try
    {
      Bool ok = pModel_->DumpGenTreeSmoothMonitors(particlesMap);
      if (!ok)
      {
        err_ << "Failed to dump smooth monitors" << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::DumpBackwardSmoothMonitors(
      std::map<String, NodeArrayMonitor> & particlesMap)
  {
    if (!pModel_)
    {
      err_ << "Can't dump backward smooth monitors. No model!" << endl;
      return false;
    }
    if (!pModel_->SmootherInitialized())
    {
      err_ << "Can't dump backward smooth monitors. Smoother not initialized!" << endl;
      return false;
    }
    if (!pModel_->Smoother().AtEnd())
    {
      err_ << "Can't dump backward smooth monitors. Smoother still running!" << endl;
      return false;
    }

    try
    {
      Bool ok = pModel_->DumpBackwardSmoothMonitors(particlesMap);
      if (!ok)
      {
        err_ << "Failed to dump backward smooth monitors" << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::SampleGenTreeSmoothParticle(Size rngSeed)
  {
    if (!pModel_)
    {
      err_ << "Can't sample smooth particle. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't sample smooth particle. SMC sampler not built!"
           << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_ << "Can't sample smooth particle. SMC sampler still running!"
           << endl;
      return false;
    }

    try
    {
      // FIXME
      boost::scoped_ptr<Rng> p_rng(new Rng(rngSeed));

      Bool ok = pModel_->SampleGenTreeSmoothParticle(p_rng.get(), sampledValueMap_);
      if (!ok)
      {
        err_ << "Failed to sample smooth particle" << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::DumpSampledGenTreeSmoothParticle(
      std::map<String, MultiArray> & sampledValueMap)
  {
    if (!pModel_)
    {
      err_ << "Can't sample smooth particle. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't sample smooth particle. SMC sampler not built!"
           << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_ << "Can't sample smooth particle. SMC sampler still running!"
           << endl;
      return false;
    }

    try
    {
      sampledValueMap = sampledValueMap_;
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::SetSampledGenTreeSmoothParticle(
      const std::map<String, MultiArray> & sampledValueMap)
  {
    if (!pModel_)
    {
      err_ << "Can't set sampled smooth particle. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't set sampled smooth particle. SMC sampler not built!"
           << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_
          << "Can't set sampled smooth particle. SMC sampler still running!"
          << endl;
      return false;
    }

    try
    {
      std::map<String, MultiArray>::const_iterator it = sampledValueMap.begin();
      for (; it != sampledValueMap.end(); ++it)
      {
        if (!sampledValueMap_.count(it->first))
          throw RuntimeError("Can't set sampled value: variable not found.");

        // check dropped dimensions
        if (it->second.Dim().Drop() != sampledValueMap_.at(it->first).Dim().Drop()) {
          throw RuntimeError("Can't set sampled value: dimension mismatch.");
        }
        // change only the values
        sampledValueMap_[it->first].SetPtr(sampledValueMap_[it->first].DimPtr(),
                                           it->second.ValuesPtr());
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::PrintGraphviz(std::ostream & os)
  {
    if (!pModel_)
    {
      err_ << "Can't print graphviz. No model!" << endl;
      return false;
    }
    try
    {
      pModel_->PrintGraphviz(os);
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::DumpNodeIds(Types<NodeId>::Array & nodeIds)
  {
    if (!pModel_)
    {
      err_ << "Can't dump node ids. No model!" << endl;
      return false;
    }
    try
    {
      Types<NodeId>::ConstIterator it_nodes, it_nodes_end;
      boost::tie(it_nodes, it_nodes_end) =
          pModel_->graph().GetSortedNodes();

      nodeIds.assign(it_nodes, it_nodes_end);
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::DumpNodeNames(Types<String>::Array & nodeNames)
  {
    if (!pModel_)
    {
      err_ << "Can't dump node names. No model!" << endl;
      return false;
    }
    try
    {
      Types<NodeId>::ConstIterator it_nodes, it_nodes_end;
      boost::tie(it_nodes, it_nodes_end) =
          pModel_->graph().GetSortedNodes();

      nodeNames.resize(std::distance(it_nodes, it_nodes_end));

      for (Size i = 0; it_nodes != it_nodes_end; ++it_nodes, ++i)
        nodeNames[i] = pModel_->GetSymbolTable().GetName(*it_nodes);
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::DumpNodeTypes(Types<NodeType>::Array & nodeTypes)
  {
    if (!pModel_)
    {
      err_ << "Can't dump node types. No model!" << endl;
      return false;
    }
    try
    {
      Types<NodeId>::ConstIterator it_nodes, it_nodes_end;
      boost::tie(it_nodes, it_nodes_end) =
          pModel_->graph().GetSortedNodes();

      nodeTypes.resize(std::distance(it_nodes, it_nodes_end));

      for (Size i = 0; it_nodes != it_nodes_end; ++it_nodes, ++i)
        nodeTypes[i] = pModel_->graph().GetNode(*it_nodes).GetType();
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::DumpNodeObserved(Flags & nodeObserved)
  {
    if (!pModel_)
    {
      err_ << "Can't dump node observed boolean. No model!" << endl;
      return false;
    }
    try
    {
      Types<NodeId>::ConstIterator it_nodes, it_nodes_end;
      boost::tie(it_nodes, it_nodes_end) =
          pModel_->graph().GetSortedNodes();

      nodeObserved.resize(std::distance(it_nodes, it_nodes_end));

      for (Size i = 0; it_nodes != it_nodes_end; ++it_nodes, ++i)
        nodeObserved[i] = pModel_->graph().GetObserved()[*it_nodes];
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::DumpNodeIterations(Types<Size>::Array & nodeIterations)
  {
    if (!pModel_)
    {
      err_ << "Can't dump node sampling iterations. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't dump node sampling iterations. SMC sampler not built!"
           << endl;
      return false;
    }
    try
    {
      Types<NodeId>::ConstIterator it_nodes, it_nodes_end;
      boost::tie(it_nodes, it_nodes_end) =
          pModel_->graph().GetSortedNodes();

      nodeIterations.resize(std::distance(it_nodes, it_nodes_end));

      for (Size i = 0; it_nodes != it_nodes_end; ++it_nodes, ++i)
      {
        if (pModel_->graph().GetObserved()[*it_nodes])
          nodeIterations[i] = BIIPS_SIZENA;
        else
          nodeIterations[i] = pModel_->Sampler().GetNodeSamplingIteration(
              *it_nodes);
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::DumpNodeSamplers(Types<String>::Array & nodeSamplers)
  {
    if (!pModel_)
    {
      err_ << "Can't dump node samplers. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't dump node samplers. SMC sampler not built!" << endl;
      return false;
    }
    try
    {
      nodeSamplers.clear();
      nodeSamplers.resize(pModel_->graph().GetSize());

      Types<std::pair<NodeId, String> >::Array samplers_sequence = pModel_
          ->Sampler().GetSamplersSequence();
      for (Size i = 0; i < samplers_sequence.size(); ++i)
      {
        NodeId id = samplers_sequence[i].first;
        Size rank = pModel_->graph().GetRanks()[id];
        const String & sampler_name = samplers_sequence[i].second;
        nodeSamplers[rank] = sampler_name;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::GraphSize(Size & s)
  {
    if (!pModel_)
    {
      err_ << "Can't get graph size. No model!" << endl;
      return false;
    }
    try
    {
      s = pModel_->graph().GetSize();
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::GetLogPriorDensity(Scalar & prior, const String & variable,
                                   const IndexRange & range)
  {
    if (!pModel_)
    {
      err_ << "Can't get prior density. No model!" << endl;
      return false;
    }
    try
    {
      if (!pModel_->GetLogPriorDensity(prior, variable, range))
      {
        err_ << "Failed to get log prior density." << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }
}
