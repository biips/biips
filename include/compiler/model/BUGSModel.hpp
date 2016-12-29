#ifndef BIIPS_BUGSMODEL_HPP_
#define BIIPS_BUGSMODEL_HPP_

#include "model/Model.hpp"
#include "model/NodeArray.hpp"
#include "model/SymbolTable.hpp"
#include "common/Accumulator.hpp"
#include "model/NodeArrayMonitor.hpp"

#include <map>

namespace Biips
{

  class BUGSModel: public Model
  {
  public:
    typedef Model BaseType;
    typedef BUGSModel SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    SymbolTable symbolTable_;
    Types<String>::Array filterMonitorsNames_;
    Types<IndexRange>::Array filterMonitorsRanges_;
    Types<String>::Array genTreeSmoothMonitorsNames_;
    Types<IndexRange>::Array genTreeSmoothMonitorsRanges_;
    Types<String>::Array backwardSmoothMonitorsNames_;
    Types<IndexRange>::Array backwardSmoothMonitorsRanges_;

  public:
    BUGSModel(Bool dataModel = false)
        : BaseType(dataModel), symbolTable_(*this)
    {
    }

    std::map<String, MultiArray> Sample(Rng * pRng) const;

    SymbolTable & GetSymbolTable()
    {
      return symbolTable_;
    }

    Bool SetFilterMonitor(const String & name, const IndexRange & range =
        NULL_RANGE);
    Bool SetGenTreeSmoothMonitor(const String & name, const IndexRange & range =
        NULL_RANGE);
    Bool SetBackwardSmoothMonitor(const String & name, const IndexRange & range =
        NULL_RANGE);

    Bool
    IsFilterMonitored(const String & name, IndexRange range = NULL_RANGE,
                      Bool check_released = true) const;
    Bool IsGenTreeSmoothMonitored(const String & name, IndexRange range =
        NULL_RANGE,
                               Bool check_released = true) const;
    Bool
    IsBackwardSmoothMonitored(const String & name, IndexRange range = NULL_RANGE,
                      Bool check_released = true) const;

//    void PrintSamplersSequence(std::ostream & out) const;

    void PrintGraphviz(std::ostream & out) const;

    // TODO manage multi statFeature
    Bool ExtractFilterStat(String name, StatTag statFeature,
                           std::map<IndexRange, MultiArray> & statMap) const;
    Bool ExtractGenTreeSmoothStat(
        String name, StatTag statFeature,
        std::map<IndexRange, MultiArray> & statMap) const;
    Bool ExtractBackwardSmoothStat(String name, StatTag statFeature,
                           std::map<IndexRange, MultiArray> & statMap) const;

    Bool ExtractFilterPdf(String name, std::map<IndexRange, Histogram> & pdfMap,
                          Size numBins = 40, Scalar cacheFraction = 0.25) const;
    Bool ExtractGenTreeSmoothPdf(String name,
                              std::map<IndexRange, Histogram> & pdfMap,
                              Size numBins = 40,
                              Scalar cacheFraction = 0.25) const;
    Bool ExtractBackwardSmoothPdf(String name, std::map<IndexRange, Histogram> & pdfMap,
                          Size numBins = 40, Scalar cacheFraction = 0.25) const;

    Bool DumpData(std::map<String, MultiArray> & dataMap) const;
    Bool ChangeData(const String & variable, const IndexRange & range,
                    const MultiArray & data, Bool & rebuildSampler, Bool mcmc);
    Bool SampleData(const String & variable, const IndexRange & range,
                    MultiArray & data, Rng * pRng);
    Bool RemoveData(const String & variable, const IndexRange & range);
    Bool GetLogPriorDensity(Scalar & prior, const String & variable,
                            IndexRange range = NULL_RANGE) const;
    Bool GetFixedSupport(ValArray & lower, ValArray & upper,
                         const String & variable,
                         IndexRange range = NULL_RANGE) const;
    Bool
    DumpFilterMonitors(std::map<String, NodeArrayMonitor> & monitorsMap) const;
    Bool
    DumpGenTreeSmoothMonitors(
        std::map<String, NodeArrayMonitor> & monitorsMap) const;
    Bool
    DumpBackwardSmoothMonitors(std::map<String, NodeArrayMonitor> & monitorsMap) const;

    Bool SampleGenTreeSmoothParticle(
        Rng * pRng,
        std::map<String, MultiArray> & sampledValues) const;

    void virtual ClearFilterMonitors(Bool release_only = false);
    void virtual ClearGenTreeSmoothMonitors(Bool release_only = false);
    void virtual ClearBackwardSmoothMonitors(Bool release_only = false);
  };

}

#endif /* BIIPS_BUGSMODEL_HPP_ */
