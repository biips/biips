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
#include "model/NodeArrayMonitor.hpp"

#include <map>

namespace Biips
{

  class BUGSModel : public Model
  {
  public:
    typedef Model BaseType;
    typedef BUGSModel SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    SymbolTable symbolTable_;
    Types<String>::Array filterMonitorsNames_;
    Types<String>::Array smoothTreeMonitorsNames_;
    Types<String>::Array smoothMonitorsNames_;

  public:
    BUGSModel() : symbolTable_(*this) {}

    std::map<String, MultiArray> Sample(const Rng::Ptr & pRng) const;

    SymbolTable & GetSymbolTable() { return symbolTable_; }

    Bool SetFilterMonitor(const String & name);
    Bool SetSmoothTreeMonitor(const String & name);
    Bool SetSmoothMonitor(const String & name);

    Bool IsFilterMonitored(const String & name) const;
    Bool IsSmoothTreeMonitored(const String & name) const;
    Bool IsSmoothMonitored(const String & name) const;

    void PrintSamplersSequence(std::ostream & out) const;

    void PrintGraphviz(std::ostream & out) const;

    // TODO manage multi statFeature
    Bool ExtractFilterStat(String name, StatsTag statFeature, std::map<IndexRange, MultiArray> & statMap) const;
    Bool ExtractSmoothTreeStat(String name, StatsTag statFeature, std::map<IndexRange, MultiArray> & statMap) const;
    Bool ExtractSmoothStat(String name, StatsTag statFeature, std::map<IndexRange, MultiArray> & statMap) const;

    Bool ExtractFilterPdf(String name, std::map<IndexRange, ScalarHistogram> & pdfMap, Size numBins = 40, Scalar cacheFraction = 0.25) const;
    Bool ExtractSmoothTreePdf(String name, std::map<IndexRange, ScalarHistogram> & pdfMap, Size numBins = 40, Scalar cacheFraction = 0.25) const;
    Bool ExtractSmoothPdf(String name, std::map<IndexRange, ScalarHistogram> & pdfMap, Size numBins = 40, Scalar cacheFraction = 0.25) const;

    Bool DumpFilterMonitors(std::map<String, NodeArrayMonitor> & monitorsMap) const;
    Bool DumpSmoothTreeMonitors(std::map<String, NodeArrayMonitor> & monitorsMap) const;
    Bool DumpSmoothMonitors(std::map<String, NodeArrayMonitor> & monitorsMap) const;
  };

}

#endif /* BIIPS_BUGSMODEL_HPP_ */
