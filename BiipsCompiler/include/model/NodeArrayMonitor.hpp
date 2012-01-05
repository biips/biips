//                                               -*- C++ -*-
/*! \file NodeArrayMonitor.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_NODEARRAYMONITOR_HPP_
#define BIIPS_NODEARRAYMONITOR_HPP_

#include "model/Monitor.hpp"
#include "common/MultiArray.hpp"
#include "common/IndexRange.hpp"

namespace Biips
{
  class NodeArray;
  class Graph;

  class NodeArrayMonitor
  {
  protected:
    String name_;
    IndexRange range_;
    Size nParticles_;
    MultiArray values_;
    MultiArray weights_;
    MultiArray ess_;
    MultiArray iterations_;
    MultiArray nodeIds_;
    MultiArray discrete_;

    template<typename StorageOrderType>
    void addObservedNode(NodeId id,
                         const IndexRange & subRange,
                         const Graph & graph);
    template<typename StorageOrderType>
    void addMonitoredNode(NodeId id,
                          const IndexRange & subRange,
                          const Monitor::Ptr & pMonitor);

  public:
    NodeArrayMonitor(const NodeArray & nodeArray,
                     const IndexRange & range,
                     const std::map<NodeId, Monitor::Ptr> & monitorsMap,
                     Size nParticles,
                     const Graph & graph);
    NodeArrayMonitor(const NodeArray & nodeArray,
                     const IndexRange & range,
                     const Monitor::Ptr & pMonitor,
                     Size nParticles,
                     const Graph & graph);

    const String & GetName() const
    {
      return name_;
    }
    const IndexRange & GetRange() const
    {
      return range_;
    }
    const MultiArray & GetNodeIds() const
    {
      return nodeIds_;
    }
    const MultiArray & GetIterations() const
    {
      return iterations_;
    }
    const MultiArray & GetESS() const
    {
      return ess_;
    }
    const MultiArray & GetWeights() const
    {
      return weights_;
    }
    const MultiArray & GetValues() const
    {
      return values_;
    }
    const MultiArray & GetDiscrete() const
    {
      return discrete_;
    }
  };

}

#endif /* BIIPS_NODEARRAYMONITOR_HPP_ */
