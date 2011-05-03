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
#include "model/Monitor.hpp"
#include "sampler/Accumulator.hpp"

namespace Biips
{

  class Model
  {
  protected:
    Graph::Ptr pGraph_;
    SMCSampler::Ptr pSampler_;
    Rng::Ptr pRng_;
    std::map<NodeId, Monitor::Ptr> filterMonitorsMap_;

  public:

    Model() : pGraph_(new Graph()) {}

    Graph * GraphPtr() { return pGraph_.get(); }

    Monitor::Ptr SetFilterMonitor(const Types<NodeId>::Array & nodeIds);

    const SMCSampler & Sampler() const;

    void SetResampleParam(ResampleType rtMode, Scalar threshold);

    void InitSampler(Size nParticles, Rng::Ptr pRng);

    Bool IsInitialized() const { return pSampler_; }

    void IterateSampler();

    // TODO manage multi statFeature
    MultiArray ExtractFilterStat(NodeId nodeId, StatsTag statFeature) const;

    ScalarHistogram ExtractFilterPdf(NodeId nodeId, Size numBins = 40, Scalar cacheFraction = 0.25) const;

    // TODO manage multi statFeature
    MultiArray ExtractSmoothTreeStat(NodeId nodeId, StatsTag statFeature) const;

    ScalarHistogram ExtractSmoothTreePdf(NodeId nodeId, Size numBins = 40, Scalar cacheFraction = 0.25) const;

    virtual ~Model() {};
  };

}

#endif /* BIIPS_MODEL_HPP_ */
