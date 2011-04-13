//                                               -*- C++ -*-
/*! \file BUGSModel.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_BUGSMODEL_HPP_
#define BIIPS_BUGSMODEL_HPP_

#include "model/Model.hpp"
#include "model/NodeArray.hpp"
#include "model/SymbolTable.hpp"
#include "sampler/Accumulator.hpp"

#include <map>

namespace Biips
{

  class BUGSModel : public Model
  {
  public:
    typedef Model BaseType;
    typedef BUGSModel SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    std::map<String, Monitor::Ptr> monitorsMap_;

  protected:
    SymbolTable symbolTable_;

  public:
    BUGSModel() : symbolTable_(*this) {}

    std::map<String, MultiArray> Sample(Rng * pRng) const;

    SymbolTable & GetSymbolTable() { return symbolTable_; }

    Bool SetFilterMonitor(const String & name);

    Bool IsFilterMonitored(const String & name) const { return monitorsMap_.find(name) != monitorsMap_.end(); }

    void PrintSamplersSequence(std::ostream & out) const;

    void PrintGraphviz(std::ostream & out) const;

    // TODO manage multi statFeature
    Bool ExtractFilterStat(String name, StatsTag statFeature, std::map<IndexRange, MultiArray> & statMap) const;

    Bool ExtractFilterPdf(String name, std::map<IndexRange, ScalarHistogram> & pdfMap, Size numBins = 40, Scalar cacheFraction = 0.25) const;
  };

}

#endif /* BIIPS_BUGSMODEL_HPP_ */
