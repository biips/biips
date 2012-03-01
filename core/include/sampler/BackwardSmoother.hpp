//                                               -*- C++ -*-
/*! \file BackwardSmoother.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_BACKWARDSMOOTHER_HPP_
#define BIIPS_BACKWARDSMOOTHER_HPP_

#include "graph/Graph.hpp"
#include "model/Monitor.hpp"

namespace Biips
{

  class Graph;

  class BackwardSmoother
  {
  public:
    typedef BackwardSmoother SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    const Graph & graph_;
    Types<Monitor::Ptr>::Array filterMonitors_;
    //    ValArray logWeights_;
    ValArray weights_;
    Scalar sumOfWeights_;
    Scalar ess_;
    Size iter_;
    Bool initialized_;
    Types<Size>::Array nodeIterations_;

    void sumOfWeightsAndEss();
    const Monitor::Ptr & getParentFilterMonitor(NodeId id);

  public:
    BackwardSmoother(const Graph & graph,
                     const Types<Monitor::Ptr>::Array & filterMonitors,
                     const Types<Size>::Array & nodeIterations);

    void Initialize();
    void IterateBack();

    Bool Initialized() const
    {
      return initialized_;
    }
    ;

    Size Iteration() const
    {
      return iter_;
    }
    Bool AtEnd() const
    {
      return filterMonitors_.size() == 1;
    }
    const Types<NodeId>::Array & UpdatedNodes() const
    {
      return filterMonitors_.back()->GetSampledNodes();
    }
    ;
    Size GetNodeSamplingIteration(NodeId nodeId) const;

    Scalar GetNodeESS(NodeId nodeId) const;

    void
    Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId,
                    DiscreteScalarAccumulator & featuresAcc,
                    Size n = 0) const;
    void Accumulate(NodeId nodeId, ElementAccumulator & featuresAcc) const;

    void InitMonitor(SmoothMonitor & monitor) const;
    void MonitorNode(NodeId nodeId, SmoothMonitor & monitor) const;
  };

}

#endif /* BIIPS_BACKWARDSMOOTHER_HPP_ */
