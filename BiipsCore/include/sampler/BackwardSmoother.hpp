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
    const Graph::Ptr pGraph_;
    Types<Monitor::Ptr>::Array filterMonitors_;
    ValArray weights_;
    Scalar sumOfweights_;
    Size t_;
    Bool initialized_;

  public:
    BackwardSmoother(const Graph::Ptr & pGraph, const Types<Monitor::Ptr>::Array & filterMonitors);

    void Initialize();
    void IterateBack();

    void Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId, DiscreteScalarAccumulator & featuresAcc, Size n = 0) const;
    void Accumulate(NodeId nodeId, ElementAccumulator & featuresAcc) const;
  };

}

#endif /* BIIPS_BACKWARDSMOOTHER_HPP_ */
