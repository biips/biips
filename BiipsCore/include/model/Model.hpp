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
#include "sampler/ForwardSampler.hpp"
#include "sampler/BackwardSmoother.hpp"
#include "model/Monitor.hpp"
#include "sampler/Accumulator.hpp"

namespace Biips
{

  class Model
  {
  protected:
    Graph::Ptr pGraph_;
    ForwardSampler::Ptr pSampler_;
    BackwardSmoother::Ptr pSmoother_;
    Types<Monitor::Ptr>::Array filterMonitors_;
    std::map<NodeId, Monitor::Ptr> filterMonitorsMap_;
    Types<Monitor::Ptr>::Array smoothMonitors_;
    std::map<NodeId, Monitor::Ptr> smoothMonitorsMap_;
    Monitor::Ptr pSmoothTreeMonitor_;
    std::set<NodeId> smoothTreeMonitoredNodeIds_;
    Bool defaultMonitorsSet_;

    MultiArray extractMonitorStat(NodeId nodeId, StatsTag statFeature, const std::map<NodeId, Monitor::Ptr> & monitorsMap) const;
    ScalarHistogram extractMonitorPdf(NodeId nodeId, Size numBins, Scalar cacheFraction, const std::map<NodeId, Monitor::Ptr> & monitorsMap) const;

  public:

    Model() : pGraph_(new Graph()), defaultMonitorsSet_(false) {}
    virtual ~Model() {};

    const Graph::Ptr & GraphPtr() { return pGraph_; }

    void SetDefaultFilterMonitors();

    Bool SetFilterMonitor(NodeId nodeId);
    Bool SetSmoothTreeMonitor(NodeId nodeId);
    Bool SetSmoothMonitor(NodeId nodeId);

    Bool SamplerBuilt() const { return pSampler_ && pSampler_->Built(); }
    const ForwardSampler & Sampler() const;

    void BuildSampler();

    void InitSampler(Size nParticles, const Rng::Ptr & pRng, const String & rsType, Scalar threshold);
    void IterateSampler();

    Bool SmootherInitialized() const { return pSmoother_ && pSmoother_->Initialized(); }
    const BackwardSmoother & Smoother() const;

    void InitBackwardSmoother();

    void IterateBackwardSmoother();

    // TODO manage multi statFeature
    MultiArray ExtractFilterStat(NodeId nodeId, StatsTag statFeature) const;
    MultiArray ExtractSmoothStat(NodeId nodeId, StatsTag statFeature) const;

    MultiArray ExtractSmoothTreeStat(NodeId nodeId, StatsTag statFeature) const;
    ScalarHistogram ExtractSmoothTreePdf(NodeId nodeId, Size numBins = 40, Scalar cacheFraction = 0.25) const;
    
    ScalarHistogram ExtractFilterPdf(NodeId nodeId, Size numBins = 40, Scalar cacheFraction = 0.25) const;
    ScalarHistogram ExtractSmoothPdf(NodeId nodeId, Size numBins = 40, Scalar cacheFraction = 0.25) const;

    // only release monitor objects
    // but keep nodeIds
    void virtual ReleaseFilterMonitors();
    void virtual ReleaseSmoothTreeMonitors();
    void virtual ReleaseSmoothMonitors();

    void virtual ClearFilterMonitors();
    void virtual ClearSmoothTreeMonitors();
    void virtual ClearSmoothMonitors();

    Scalar GetLogPriorDensity(NodeId nodeId) const;
  };

}

#endif /* BIIPS_MODEL_HPP_ */
