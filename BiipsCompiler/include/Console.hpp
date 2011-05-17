//                                               -*- C++ -*-
/*! \file Console.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 *
 * COPY: Nearly copied and pasted from JAGS Console class
 */

#ifndef BIIPS_CONSOLE_HPP_
#define BIIPS_CONSOLE_HPP_

#include <cstdio>

#include "model/BUGSModel.hpp"
#include "compiler/ParseTree.h"

namespace Biips
{

  const String PROMPT_STRING = ">>> ";
  const Size INDENT_SIZE = 4;
  const String INDENT_STRING(INDENT_SIZE, ' ');

  class Console
  {
  protected:
    std::ostream & out_;
    std::ostream & err_;
    BUGSModel::Ptr pModel_;
    ParseTree * pData_;
    ParseTree * pRelations_;
    Types<ParseTree*>::Array * pVariables_;
    Types<String>::Array names_;

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
    Bool CheckModel(std::FILE * file, Bool verbose = true);

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
    Bool Compile(std::map<String, MultiArray> & dataMap, Bool genData, Size dataRngSeed, Bool verbose = true);

    Bool PrintGraphviz(std::ostream & os);

    /*! Clears the model */
    void ClearModel(Bool verbose = true);

    Bool SetDefaultFilterMonitors();

    Bool SetFilterMonitor(const String & name);
    Bool SetSmoothMonitor(const String & name);

    /*!
     * @short Builds the SMC sampler.
     *
     * The samplers are chosen for the unobserved
     * stochastic nodes based on the list of sampler factories.
     *
     * @returns true on success, false on failure
     */
    Bool BuildSampler(Size nParticles, Size rng_seed, Bool prior, Size verbose = 1);

    Bool RunSMCSampler(ResampleType rsType, Scalar ess_threshold, Scalar & log_norm_const, Bool verbose = true);

    Bool RunBackwardSmoother(Bool verbose = true);

    Bool ExtractFilterStat(const String & name, StatsTag statFeature, std::map<IndexRange, MultiArray> & statMap);
    Bool ExtractSmoothStat(const String & name, StatsTag statFeature, std::map<IndexRange, MultiArray> & statMap);
    Bool ExtractSmoothTreeStat(const String & name, StatsTag statFeature, std::map<IndexRange, MultiArray> & statMap);

    Bool ExtractFilterPdf(const String & name, std::map<IndexRange, ScalarHistogram> & pdfMap, Size numBins = 40, Scalar cacheFraction = 0.25);
    Bool ExtractSmoothPdf(const String & name, std::map<IndexRange, ScalarHistogram> & pdfMap, Size numBins = 40, Scalar cacheFraction = 0.25);
    Bool ExtractSmoothTreePdf(const String & name, std::map<IndexRange, ScalarHistogram> & pdfMap, Size numBins = 40, Scalar cacheFraction = 0.25);
  };

}

#endif /* BIIPS_CONSOLE_HPP_ */
