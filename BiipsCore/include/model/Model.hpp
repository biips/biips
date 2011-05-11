//                                               -*- C++ -*-
/*! \file Model.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_MODEL_HPP_
#define BIIPS_MODEL_HPP_

#include "graph/Graph.hpp"
#include "sampler/SMCSampler.hpp"
#include "sampler/BackwardSmoother.hpp"
#include "model/Monitor.hpp"
#include "sampler/Accumulator.hpp"

namespace Biips
{

  class Model
  {
  protected:
    Graph::Ptr pGraph_;
    SMCSampler::Ptr pSampler_;
    BackwardSmoother::Ptr pSmoother_;
    Rng::Ptr pRng_;
    Types<Monitor::Ptr>::Array filterMonitors_;
    std::map<NodeId, Monitor::Ptr> filterMonitorsMap_;
    Types<Monitor::Ptr>::Array smoothMonitors_;
    std::map<NodeId, Monitor::Ptr> smoothMonitorsMap_;
    Bool defaultMonitorsSet_;

    MultiArray extractMonitorStat(NodeId nodeId, StatsTag statFeature, const std::map<NodeId, Monitor::Ptr> & monitorsMap) const;
    ScalarHistogram extractMonitorPdf(NodeId nodeId, Size numBins, Scalar cacheFraction, const std::map<NodeId, Monitor::Ptr> & monitorsMap) const;

  public:

    Model() : pGraph_(new Graph()), defaultMonitorsSet_(false) {}

    Graph * GraphPtr() { return pGraph_.get(); }

    void SetDefaultFilterMonitors();

    void SetFilterMonitor(const Types<NodeId>::Array & nodeIds);
    void SetSmoothMonitor(const Types<NodeId>::Array & nodeIds);

    void SetResampleParam(ResampleType rtMode, Scalar threshold);

    Bool SamplerInitialized() const { return pSampler_; }
    const SMCSampler & Sampler() const;

    void InitSampler(Size nParticles, Rng::Ptr pRng);

    void IterateSampler();

    Bool SmootherInitialized() const { return pSmoother_; }
    const BackwardSmoother & Smoother() const;

    void InitBackwardSmoother();

    void IterateBackwardSmoother();

    // TODO manage multi statFeature
    MultiArray ExtractFilterStat(NodeId nodeId, StatsTag statFeature) const;
    MultiArray ExtractSmoothStat(NodeId nodeId, StatsTag statFeature) const;

    ScalarHistogram ExtractFilterPdf(NodeId nodeId, Size numBins = 40, Scalar cacheFraction = 0.25) const;
    ScalarHistogram ExtractSmoothPdf(NodeId nodeId, Size numBins = 40, Scalar cacheFraction = 0.25) const;

    // TODO manage multi statFeature
    MultiArray ExtractSmoothTreeStat(NodeId nodeId, StatsTag statFeature) const;

    ScalarHistogram ExtractSmoothTreePdf(NodeId nodeId, Size numBins = 40, Scalar cacheFraction = 0.25) const;

    virtual ~Model() {};
  };

}

#endif /* BIIPS_MODEL_HPP_ */
