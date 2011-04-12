//                                               -*- C++ -*-
/*! \file Monitor.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_MONITOR_HPP_
#define BIIPS_MONITOR_HPP_

#include "common/Types.hpp"
#include "common/ValArray.hpp"
#include "common/DimArray.hpp"
#include <map>

namespace Biips
{

  class ScalarAccumulator;
  class ElementAccumulator;

  class Monitor
  {
  public:
    typedef Monitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    typedef Types<std::pair<ValArray::Ptr, Scalar> >::Array WeightedSample;

    std::map<NodeId, WeightedSample> nodeParticles_;
    std::map<NodeId, Scalar> nodeEss_;

  public:
    Monitor() {};

    Bool Contains(NodeId nodeId) const;

    void AddNode(NodeId nodeId, Size nParticles, Scalar ess);

    void PushParticle(NodeId nodeId, const ValArray::Ptr & pValue, Scalar weight);

    Scalar GetESS(NodeId nodeId) const;

    void Accumulate(NodeId nodeId, ScalarAccumulator & featuresAcc, Size n = 0) const;

    void Accumulate(NodeId nodeId, ElementAccumulator & featuresAcc, const DimArray::Ptr & pDim) const;

    void Clear() { nodeParticles_.clear(); nodeEss_.clear(); };

    virtual ~Monitor() {};
  };

}

#endif /* BIIPS_MONITOR_HPP_ */
