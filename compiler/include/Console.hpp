//                                               -*- C++ -*-
/*! \file Console.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 *
 * COPY: Adapted from JAGS Console class
 */

#ifndef BIIPS_CONSOLE_HPP_
#define BIIPS_CONSOLE_HPP_

#include "common/Types.hpp"
#include <map>
#include "common/IndexRange.hpp"
#include "common/MultiArray.hpp"
#include "sampler/Accumulator.hpp"
#include "model/NodeArrayMonitor.hpp"

class ParseTree;

namespace Biips
{

  const String PROMPT_STRING = ">>> ";
  const Size INDENT_SIZE = 4;
  const String INDENT_STRING(INDENT_SIZE, ' ');

  class BUGSModel;

  class Console
  {
  protected:
    std::ostream & out_;
    std::ostream & err_;
    BUGSModel * pModel_;
    ParseTree * pData_;
    ParseTree * pRelations_;
    Types<ParseTree*>::Array * pVariables_;
    Types<String>::Array nodeArrayNames_;
    Bool lockBackward_;
    Scalar logNormConst_;
    std::map<String, MultiArray> sampledValueMap_;

    void clearParseTrees();

  public:
    /*!
     * Constructor
     *
     * @param out Output stream to which information messages will be printed.
     *
     * @param err Output stream to which error messages will be printed.
     *
     */
    Console(std::ostream & out, std::ostream & err);

    ~Console();

    /*!
     * Checks syntactic correctness of model
     *
     * @param file containing BUGS-language description of the model
     *
     * @return true on success or false on error.
     */
    Bool CheckModel(const String & modelFileName, Bool verbose = true);

    Bool GraphSize(Size & s);

    // FIXME add module manager and a load module by name function
    Bool LoadBaseModule(Bool verbose = true);

    /*!
     * Compiles the model.
     *
     * @param data Map relating the names of the observed variables to
     * their values.
     *
     * @param nchain Number of chains in the model.
     *
     * @param gendata Boolean flag indicating whether the data generation
     * sub-model should be run, if there is one.
     *
     * @return true on success or false on error.
     */
    Bool Compile(std::map<String, MultiArray> & dataMap,
                 Bool genData,
                 Size dataRngSeed,
                 Bool verbose = true);

    Bool PrintGraphviz(std::ostream & os);
    /*!
     * Returns a vector of variable names used by the model. This vector
     * excludes any counters used by the model within a for loop.
     */
    Types<String>::Array const & VariableNames() const
    {
      return nodeArrayNames_;
    }

    /*! Clears the model */
    void ClearModel(Bool verbose = true);

    Bool SetDefaultFilterMonitors();

    Bool SetFilterMonitor(const String & name, const IndexRange & range =
        NULL_RANGE);
    Bool SetSmoothTreeMonitor(const String & name, const IndexRange & range =
        NULL_RANGE);
    Bool SetSmoothMonitor(const String & name, const IndexRange & range =
        NULL_RANGE);

    Bool ReleaseFilterMonitors();
    Bool ReleaseSmoothTreeMonitors();
    Bool ReleaseSmoothMonitors();

    Bool ClearFilterMonitors();
    Bool ClearSmoothTreeMonitors();
    Bool ClearSmoothMonitors();

    /*!
     * @short Builds the SMC sampler.
     *
     * The samplers are chosen for the unobserved
     * stochastic nodes based on the list of sampler factories.
     *
     * @returns true on success, false on failure
     */
    Bool BuildSampler(Bool prior, Size verbose = 1);
    Bool SamplerBuilt();

    Bool RunForwardSampler(Size nParticles,
                           Size smcRngSeed,
                           const String & rsType,
                           Scalar essThreshold,
                           Bool verbose = true,
                           Bool progressBar = true);

    Bool ForwardSamplerAtEnd();

    Bool GetLogNormConst(Scalar & logNormConst);
    Bool SetLogNormConst(Scalar logNormConst);

    Bool SampleSmoothTreeParticle(Size rngSeed);
    Bool DumpSampledSmoothTreeParticle(std::map<String, MultiArray> & sampledValueMap);
    Bool SetSampledSmoothTreeParticle(const std::map<String, MultiArray> & sampledValueMap);

    Bool RunBackwardSmoother(Bool verbose = true, Bool progressBar = true);

    Bool ExtractFilterStat(const String & name, StatsTag statFeature, std::map<
        IndexRange, MultiArray> & statMap);
    Bool ExtractSmoothTreeStat(const String & name,
                               StatsTag statFeature,
                               std::map<IndexRange, MultiArray> & statMap);
    Bool ExtractSmoothStat(const String & name, StatsTag statFeature, std::map<
        IndexRange, MultiArray> & statMap);

    Bool ExtractFilterPdf(const String & name, std::map<IndexRange,
        ScalarHistogram> & pdfMap, Size numBins = 40, Scalar cacheFraction =
        0.25);
    Bool ExtractSmoothTreePdf(const String & name, std::map<IndexRange,
        ScalarHistogram> & pdfMap, Size numBins = 40, Scalar cacheFraction =
        0.25);
    Bool ExtractSmoothPdf(const String & name, std::map<IndexRange,
        ScalarHistogram> & pdfMap, Size numBins = 40, Scalar cacheFraction =
        0.25);

    Bool DumpData(std::map<String, MultiArray> & dataMap);
    Bool
    ChangeData(std::map<String, MultiArray> & dataMap, Bool verbose = true);
    Bool GetLogPriorDensity(Scalar & prior,
                            const String & variable,
                            const IndexRange & range = NULL_RANGE);

    Bool DumpFilterMonitors(std::map<String, NodeArrayMonitor> & particlesMap);
    Bool
    DumpSmoothTreeMonitors(std::map<String, NodeArrayMonitor> & particlesMap);
    Bool DumpSmoothMonitors(std::map<String, NodeArrayMonitor> & particlesMap);

    Bool DumpNodeIds(Types<NodeId>::Array & nodeIds);
    Bool DumpNodeNames(Types<String>::Array & nodeNames);
    Bool DumpNodeTypes(Types<NodeType>::Array & nodeTypes);
    Bool DumpNodeObserved(Flags & nodeObserved);
    Bool DumpNodeIterations(Types<Size>::Array & nodeIterations);
    Bool DumpNodeSamplers(Types<String>::Array & nodeSamplers);
  };
}

#endif /* BIIPS_CONSOLE_HPP_ */
