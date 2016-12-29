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
    Types<Monitor*>::Array filterMonitors_;
    //    ValArray logWeights_;
    ValArray weights_;
    Scalar sumOfWeights_;
    Scalar ess_;
    Size iter_;
    Bool initialized_;
    Types<Size>::Array nodeIterations_;
    Types<NodeId>::Array condNodes_;

    void sumOfWeightsAndEss();
    Monitor * getParentFilterMonitor(NodeId id);

  public:
    BackwardSmoother(const Graph & graph,
                     const Types<Monitor *>::Array & filterMonitors,
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
    // last updated nodes at the current iteration (incremental)
    const Types<NodeId>::Array & LastUpdatedNodes() const
    {
      return filterMonitors_.back()->GetLastSampledNodes();
    }
    ;
    // all conditional nodes (same at each iteration) = all observed stochastic nodes
    const Types<NodeId>::Array & ConditionalNodes() const
    {
      return condNodes_;
    }
    ;
    Size GetNodeSamplingIteration(NodeId nodeId) const;

    Scalar GetNodeESS(NodeId nodeId) const;

    void
    Accumulate(NodeId nodeId, Accumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId,
                    DiscreteAccumulator & featuresAcc,
                    Size n = 0) const;
    void Accumulate(NodeId nodeId, ArrayAccumulator & featuresAcc) const;

    void InitMonitor(SmoothMonitor & monitor) const;
    void MonitorNode(NodeId nodeId, SmoothMonitor & monitor) const;
  };

}

#endif /* BIIPS_BACKWARDSMOOTHER_HPP_ */
