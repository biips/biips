//                                               -*- C++ -*-
/*! \file Console.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 *
 * COPY: Adapted from JAGS Console class
 */

extern "C" {
  void BiipsCompiler_is_present(void) {}
}

//#define BIIPS_COMPILER_DEBUG_ON

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

#ifdef BIIPS_COMPILER_DEBUG_ON
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
    }/*                                                                 \
  catch(const std::exception & e)                                     \
  {                                                                   \
    std::cerr << "STD ERROR: " << e.what() << endl;                   \
    return false;                                                     \
  }*/


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
    }/*                                                                 \
  catch(const std::exception & e)                                     \
  {                                                                   \
    std::cerr << "STD ERROR: " << e.what() << endl;                   \
    ClearModel();                                                     \
    return false;                                                     \
  }*/



namespace Biips
{

  Console::Console(std::ostream & out, std::ostream & err) :
    out_(out), err_(err), pData_(NULL), pRelations_(NULL), pVariables_(NULL),
        lockBackward_(false)
  {
  }

  void Console::clearParseTrees()
  {
    delete pData_;
    pData_ = NULL;
    delete pRelations_;
    pRelations_ = NULL;
    if (pVariables_)
    {
      for (Size i = 0; i < pVariables_->size(); ++i)
        delete (*pVariables_)[i];
      delete pVariables_;
      pVariables_ = NULL;
    }
  }

  Console::~Console()
  {
    clearParseTrees();
    ClearModel(false);
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
    for (Types<ParseTree*>::Array::const_iterator p = param.begin(); p
        != param.end(); ++p)
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

  Bool Console::CheckModel(const String & modelFileName, Bool verbose)
  {
    if (pRelations_ || pData_ || pVariables_)
    {
      clearParseTrees();

      if (pModel_)
      {
        if (verbose)
          out_ << PROMPT_STRING << "Replacing existing model" << endl;
        ClearModel();
      }
    }

    FILE * model_file = fopen(modelFileName.c_str(), "r");

    if (!model_file)
      throw RuntimeError(String("Failed to open file: ") + modelFileName);

    String message;
    int status = parse_bugs(model_file,
                            pVariables_,
                            pData_,
                            pRelations_,
                            message);

    fclose(model_file);

#ifdef BIIPS_COMPILER_DEBUG_ON
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
      for (Types<ParseTree*>::Array::const_iterator it = pVariables_->begin(); it
          != pVariables_->end(); ++it)
        getVariableNames(*it, names_set, counter_stack);
    }
    if (pData_)
      getVariableNames(pData_, names_set, counter_stack);
    if (pRelations_)
      getVariableNames(pRelations_, names_set, counter_stack);

    nodeArrayNames_.assign(names_set.begin(), names_set.end());

    return true;
  }

  void Console::ClearModel(Bool verbose)
  {
    if (!pModel_)
    {
      if (verbose)
        out_ << PROMPT_STRING << "Deleting model" << endl;

      delete pModel_;
    }
  }

  Bool Console::Compile(std::map<String, MultiArray> & dataMap,
                        Bool genData,
                        Size dataRngSeed,
                        Bool verbose)
  {
    if (pModel_)
    {
      if (verbose)
        out_ << PROMPT_STRING << "Replacing existing model" << endl;
      ClearModel();
    }

    // FIXME
    Rng::Ptr p_datagen_rng(new Rng(dataRngSeed));

    if (pData_ && genData)
    {
      pModel_ = new BUGSModel();

      Compiler compiler(*pModel_, dataMap);

      if (verbose)
        out_ << PROMPT_STRING << "Compiling data graph" << endl;
      try
      {
        if (pVariables_)
        {
          if (verbose)
            out_ << INDENT_STRING << "Declaring variables" << endl;
          compiler.DeclareVariables(*pVariables_);
        }
        if (verbose)
          out_ << INDENT_STRING << "Resolving undeclared variables" << endl;
        compiler.UndeclaredVariables(pData_);

        if (verbose)
          out_ << INDENT_STRING << "Allocating nodes" << endl;
        compiler.WriteRelations(pData_);

        Graph & data_graph = *(pModel_->GraphPtr());
        data_graph.Build();

        /* Check validity of data generating model */
        Types<NodeId>::ConstIterator it_node_id, it_node_id_end;
        boost::tie(it_node_id, it_node_id_end) = data_graph.GetSortedNodes();
        for (; it_node_id != it_node_id_end; ++it_node_id)
        {
          if (data_graph.GetObserved()[*it_node_id])
          {
            GraphTypes::ParentIterator it_parents, it_parents_end;
            boost::tie(it_parents, it_parents_end)
                = data_graph.GetParents(*it_node_id);
            for (; it_parents != it_parents_end; ++it_parents)
            {
              if (!data_graph.GetObserved()[*it_parents])
              {
                err_ << "Invalid data graph: observed node "
                    << pModel_->GetSymbolTable().GetName(*it_node_id)
                    << " has unobserved parent "
                    << pModel_->GetSymbolTable().GetName(*it_parents)
                    << std::endl;
                ClearModel();
                return false;
              }
            }
          }
        }

        if (verbose)
        {
          out_ << INDENT_STRING << "Graph size: " << data_graph.GetSize();
          out_ << " (Constant: " << data_graph.NodesSummary().at("Constant");
          out_ << ", Logical: " << data_graph.NodesSummary().at("Logical");
          out_ << ", Stochastic: "
              << data_graph.NodesSummary().at("Stochastic") << ")" << endl;
          Size n_data_unobs_nodes =
              data_graph.UnobsNodesSummary().at("Logical")
                  + data_graph.UnobsNodesSummary().at("Stochastic");
          out_ << INDENT_STRING << "Unobserved nodes: " << n_data_unobs_nodes;
          out_ << " (Logical: " << data_graph.UnobsNodesSummary().at("Logical");
          out_ << ", Stochastic: "
              << data_graph.UnobsNodesSummary().at("Stochastic") << ")" << endl;
        }

        if (verbose)
          out_ << INDENT_STRING << "Sampling data" << endl;

        std::map<String, MultiArray>
            sampled_data_map(pModel_->Sample(p_datagen_rng.get()));

        // FIXME
        //        //Save data generating RNG for later use. It is owned by the
        //        //RNGFactory, not the model.
        //        datagen_rng = pModel_->rng(0);

        if (verbose)
          out_ << INDENT_STRING << "Reading data back into data table" << endl;

        for (std::map<String, MultiArray>::const_iterator it_sampled =
            sampled_data_map.begin(); it_sampled != sampled_data_map.end(); ++it_sampled)
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

        ClearModel(false);
      }
      BIIPS_CONSOLE_CATCH_ERRORS_DELETE_MODEL
    }

    pModel_ = new BUGSModel();
    Compiler compiler(*pModel_, dataMap);

    if (verbose)
      out_ << PROMPT_STRING << "Compiling model graph" << endl;
    try
    {
      if (pVariables_)
      {
        if (verbose)
          out_ << INDENT_STRING << "Declaring variables" << endl;
        compiler.DeclareVariables(*pVariables_);
      }
      if (pRelations_)
      {
        if (verbose)
          out_ << INDENT_STRING << "Resolving undeclared variables" << endl;
        compiler.UndeclaredVariables(pRelations_);

        if (verbose)
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
        Graph & model_graph = *(pModel_->GraphPtr());
        if (verbose)
        {
          out_ << INDENT_STRING << "Building and checking graph" << endl;
        }
        model_graph.Build();

        if (verbose)
        {
          out_ << INDENT_STRING << "Graph size: " << model_graph.GetSize();
          out_ << " (Constant: " << model_graph.NodesSummary().at("Constant");
          out_ << ", Logical: " << model_graph.NodesSummary().at("Logical");
          out_ << ", Stochastic: "
              << model_graph.NodesSummary().at("Stochastic") << ")" << endl;
          Size n_unobs_nodes = model_graph.UnobsNodesSummary().at("Logical")
              + model_graph.UnobsNodesSummary().at("Stochastic");
          out_ << INDENT_STRING << "Unobserved nodes: " << n_unobs_nodes;
          out_ << " (Logical: "
              << model_graph.UnobsNodesSummary().at("Logical");
          out_ << ", Stochastic: "
              << model_graph.UnobsNodesSummary().at("Stochastic") << ")"
              << endl;
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

  Bool Console::BuildSampler(Bool prior, Size verbose)
  {
    if (!pModel_)
    {
      err_ << "Can't build SMC sampler. No model!" << endl;
      return false;
    }
    if (pModel_->GraphPtr()->Empty())
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
      std::list<std::pair<NodeSamplerFactory::Ptr, Bool> >::iterator
          it_sampler_fact = ForwardSampler::NodeSamplerFactories().begin();
      for (; it_sampler_fact != ForwardSampler::NodeSamplerFactories().end(); ++it_sampler_fact)
      {
        it_sampler_fact->second = !prior;
      }

      if (verbose > 0)
      {
        out_ << PROMPT_STRING << "Building SMC sampler";
        if (prior)
          out_ << " using prior mutation";

        out_ << endl;
      }

      pModel_->BuildSampler();

      if (verbose > 1)
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

  Bool Console::RunForwardSampler(Size nParticles,
                                  Size smcRngSeed,
                                  const String & rsType,
                                  Scalar essThreshold,
                                  Bool verbose,
                                  Bool progressBar)
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

      if (verbose)
        out_ << PROMPT_STRING << "Running SMC sampler of " << n_iter
            << " iterations" << endl;

      Types<ProgressBar>::Ptr p_show_progress;
      if (progressBar)
        p_show_progress
            = Types<ProgressBar>::Ptr(new ProgressBar(n_iter,
                                                      out_,
                                                      INDENT_STRING));

      // filtering

      // FIXME
      Rng::Ptr p_smc_rng(new Rng(smcRngSeed));

      pModel_->InitSampler(nParticles, p_smc_rng, rsType, essThreshold);

      if (p_show_progress)
        ++(*p_show_progress);
      else if (verbose)
        printSamplerState(pModel_->Sampler(), out_);

      for (Size n = 1; n < n_iter; ++n)
      {
        pModel_->IterateSampler();

        if (p_show_progress)
          ++(*p_show_progress);
        else if (verbose)
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

  Bool Console::RunBackwardSmoother(Bool verbose, Bool progressBar)
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
      pModel_->InitBackwardSmoother();

      Size n_iter = pModel_->Sampler().NIterations() - 1;

      if (verbose)
        out_ << PROMPT_STRING << "Running backward smoother of " << n_iter
            << " iterations" << endl;

      Types<ProgressBar>::Ptr p_show_progress;
      if (progressBar)
        p_show_progress
            = Types<ProgressBar>::Ptr(new ProgressBar(n_iter,
                                                      out_,
                                                      INDENT_STRING));

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
  Bool Console::LoadBaseModule(Bool verbose)
  {
    if (verbose)
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

  Bool Console::SetSmoothTreeMonitor(const String & name,
                                     const IndexRange & range)
  {
    if (!pModel_)
    {
      err_ << "Can't set smooth tree monitor. No model!" << endl;
      return false;
    }
    // TODO: check that sampler did not start

    try
    {
      Bool ok = pModel_->SetSmoothTreeMonitor(name, range);
      if (!ok)
      {
        err_ << "Failed to set smooth tree monitor for variable " << name;
        if (!range.IsNull())
          err_ << range;
        err_ << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::SetSmoothMonitor(const String & name, const IndexRange & range)
  {
    if (!pModel_)
    {
      err_ << "Can't set smooth monitor. No model!" << endl;
      return false;
    }
    // TODO: check that sampler did not start

    try
    {
      Bool ok = pModel_->SetSmoothMonitor(name, range);
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

  Bool Console::ClearFilterMonitors()
  {
    if (!pModel_)
    {
      err_ << "Can't clear filter monitors. No model!" << endl;
      return false;
    }

    try
    {
      pModel_->ClearFilterMonitors();
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ClearSmoothTreeMonitors()
  {
    if (!pModel_)
    {
      err_ << "Can't clear smooth tree monitors. No model!" << endl;
      return false;
    }

    try
    {
      pModel_->ClearSmoothTreeMonitors();
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ClearSmoothMonitors()
  {
    if (!pModel_)
    {
      err_ << "Can't clear smooth monitors. No model!" << endl;
      return false;
    }

    try
    {
      pModel_->ClearSmoothMonitors();
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ReleaseFilterMonitors()
  {
    if (!pModel_)
    {
      err_ << "Can't clear filter monitors. No model!" << endl;
      return false;
    }
    try
    {
      pModel_->ReleaseFilterMonitors();
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ReleaseSmoothTreeMonitors()
  {
    if (!pModel_)
    {
      err_ << "Can't clear smooth tree monitors. No model!" << endl;
      return false;
    }
    try
    {
      pModel_->ReleaseSmoothTreeMonitors();
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ReleaseSmoothMonitors()
  {
    if (!pModel_)
    {
      err_ << "Can't clear smooth monitors. No model!" << endl;
      return false;
    }
    try
    {
      pModel_->ReleaseSmoothMonitors();
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ExtractFilterStat(const String & name,
                                  StatsTag statFeature,
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

  Bool Console::ExtractSmoothTreeStat(const String & name,
                                      StatsTag statFeature,
                                      std::map<IndexRange, MultiArray> & statMap)
  {
    if (!pModel_)
    {
      err_ << "Can't extract smooth tree statistic. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't extract smooth tree statistic. SMC sampler not built!"
          << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_ << "Can't extract smooth tree statistic. SMC sampler still running!"
          << endl;
      return false;
    }

    try
    {
      Bool ok = pModel_->ExtractSmoothTreeStat(name, statFeature, statMap);
      if (!ok)
      {
        err_ << "Failed to extract smooth tree statistic for variable " << name
            << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ExtractSmoothStat(const String & name,
                                  StatsTag statFeature,
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
      Bool ok = pModel_->ExtractSmoothStat(name, statFeature, statMap);
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

  Bool Console::ExtractFilterPdf(const String & name, std::map<IndexRange,
      ScalarHistogram> & pdfMap, Size numBins, Scalar cacheFraction)
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

  Bool Console::ExtractSmoothTreePdf(const String & name, std::map<IndexRange,
      ScalarHistogram> & pdfMap, Size numBins, Scalar cacheFraction)
  {
    if (!pModel_)
    {
      err_ << "Can't extract smooth tree pdf. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't extract smooth tree pdf. SMC sampler not built!" << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_ << "Can't extract smooth tree pdf. SMC sampler still running!"
          << endl;
      return false;
    }

    try
    {
      Bool ok = pModel_->ExtractSmoothTreePdf(name,
                                              pdfMap,
                                              numBins,
                                              cacheFraction);
      if (!ok)
      {
        err_ << "Failed to extract smooth tree pdf for variable " << name
            << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::ExtractSmoothPdf(const String & name, std::map<IndexRange,
      ScalarHistogram> & pdfMap, Size numBins, Scalar cacheFraction)
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
      Bool ok = pModel_->ExtractSmoothPdf(name, pdfMap, numBins, cacheFraction);
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

  Bool Console::ChangeData(std::map<String, MultiArray> & dataMap, Bool verbose)
  {
    if (!pModel_)
    {
      err_ << "Can't change data. No model!" << endl;
      return false;
    }
    if (pModel_->GraphPtr()->Empty())
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
      if (verbose)
      {
        out_ << PROMPT_STRING << "Changing data" << endl;
      }
      if (!pModel_->ChangeData(dataMap))
      {
        err_ << "Failed to change data" << endl;
        return false;
      }
      lockBackward_ = true;
      ReleaseFilterMonitors();
      ReleaseSmoothTreeMonitors();
      if (pModel_->SmootherInitialized())
        ReleaseSmoothMonitors();
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::DumpFilterMonitors(std::map<String, NodeArrayMonitor> & particlesMap)
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

  Bool Console::DumpSmoothTreeMonitors(std::map<String, NodeArrayMonitor> & particlesMap)
  {
    if (!pModel_)
    {
      err_ << "Can't dump smooth tree monitors. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't dump smooth tree monitors. SMC sampler not built!" << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_ << "Can't dump smooth tree monitors. SMC sampler still running!"
          << endl;
      return false;
    }

    try
    {
      Bool ok = pModel_->DumpSmoothTreeMonitors(particlesMap);
      if (!ok)
      {
        err_ << "Failed to dump smooth tree monitors" << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::DumpSmoothMonitors(std::map<String, NodeArrayMonitor> & particlesMap)
  {
    if (!pModel_)
    {
      err_ << "Can't dump smooth monitors. No model!" << endl;
      return false;
    }
    if (!pModel_->SmootherInitialized())
    {
      err_ << "Can't dump smooth monitors. Smoother not initialized!" << endl;
      return false;
    }
    if (!pModel_->Smoother().AtEnd())
    {
      err_ << "Can't dump smooth monitors. Smoother still running!" << endl;
      return false;
    }

    try
    {
      Bool ok = pModel_->DumpSmoothMonitors(particlesMap);
      if (!ok)
      {
        err_ << "Failed to dump smooth monitors" << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }


  Bool Console::SampleSmoothTreeParticle(Size rngSeed)
  {
    if (!pModel_)
    {
      err_ << "Can't sample smooth tree particle. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't sample smooth tree particle. SMC sampler not built!" << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_ << "Can't sample smooth tree particle. SMC sampler still running!"
          << endl;
      return false;
    }

    try
    {
      // FIXME
      Rng::Ptr p_rng(new Rng(rngSeed));

      Bool ok = pModel_->SampleSmoothTreeParticle(p_rng, sampledValueMap_);
      if (!ok)
      {
        err_ << "Failed to sample smooth tree particle" << endl;
        return false;
      }
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }


  Bool Console::DumpSampledSmoothTreeParticle(std::map<String, MultiArray> & sampledValueMap)
  {
    if (!pModel_)
    {
      err_ << "Can't sample smooth tree particle. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't sample smooth tree particle. SMC sampler not built!" << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_ << "Can't sample smooth tree particle. SMC sampler still running!"
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


  Bool Console::SetSampledSmoothTreeParticle(const std::map<String, MultiArray> & sampledValueMap)
  {
    if (!pModel_)
    {
      err_ << "Can't set sampled smooth tree particle. No model!" << endl;
      return false;
    }
    if (!pModel_->SamplerBuilt())
    {
      err_ << "Can't set sampled smooth tree particle. SMC sampler not built!" << endl;
      return false;
    }
    if (!pModel_->Sampler().AtEnd())
    {
      err_ << "Can't set sampled smooth tree particle. SMC sampler still running!"
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

        if(it->second.Dim() != sampledValueMap_.at(it->first).Dim())
          throw RuntimeError("Can't set sampled value: dimension mismatch.");
        sampledValueMap_[it->first] = it->second;
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
      boost::tie(it_nodes, it_nodes_end)
          = pModel_->GraphPtr()->GetSortedNodes();

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
      boost::tie(it_nodes, it_nodes_end)
          = pModel_->GraphPtr()->GetSortedNodes();

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
      boost::tie(it_nodes, it_nodes_end)
          = pModel_->GraphPtr()->GetSortedNodes();

      nodeTypes.resize(std::distance(it_nodes, it_nodes_end));

      for (Size i = 0; it_nodes != it_nodes_end; ++it_nodes, ++i)
        nodeTypes[i] = pModel_->GraphPtr()->GetNode(*it_nodes).GetType();
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
      boost::tie(it_nodes, it_nodes_end)
          = pModel_->GraphPtr()->GetSortedNodes();

      nodeObserved.resize(std::distance(it_nodes, it_nodes_end));

      for (Size i = 0; it_nodes != it_nodes_end; ++it_nodes, ++i)
        nodeObserved[i] = pModel_->GraphPtr()->GetObserved()[*it_nodes];
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
      boost::tie(it_nodes, it_nodes_end)
          = pModel_->GraphPtr()->GetSortedNodes();

      nodeIterations.resize(std::distance(it_nodes, it_nodes_end));

      for (Size i = 0; it_nodes != it_nodes_end; ++it_nodes, ++i)
      {
        if (pModel_->GraphPtr()->GetObserved()[*it_nodes])
          nodeIterations[i] = BIIPS_SIZENA;
        else
          nodeIterations[i]
              = pModel_->Sampler().GetNodeSamplingIteration(*it_nodes);
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
      nodeSamplers.resize(pModel_->GraphPtr()->GetSize());

      Types<std::pair<NodeId, String> >::Array samplers_sequence =
          pModel_->Sampler().GetSamplersSequence();
      for (Size i = 0; i < samplers_sequence.size(); ++i)
      {
        NodeId id = samplers_sequence[i].first;
        Size rank = pModel_->GraphPtr()->GetRanks()[id];
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
      s = pModel_->GraphPtr()->GetSize();
    }
    BIIPS_CONSOLE_CATCH_ERRORS

    return true;
  }

  Bool Console::GetLogPriorDensity(Scalar & prior,
                                   const String & variable,
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
