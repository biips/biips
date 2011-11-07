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

  class NodeArrayMonitor
  {
  protected:
    String name_;
    IndexRange range_;
    MultiArray values_;
    MultiArray weights_;
    MultiArray ess_;
    MultiArray iterations_;
    MultiArray nodeIds_;

  public:
    NodeArrayMonitor(const NodeArray & nodeArray, const std::map<NodeId, Monitor::Ptr> & monitorsMap);
    NodeArrayMonitor(const NodeArray & nodeArray, const Monitor::Ptr & pMonitor);

    const MultiArray & GetNodeIds() const { return nodeIds_; }
    const MultiArray & GetIterations() const { return iterations_; }
    const MultiArray & GetESS() const { return ess_; }
    const MultiArray & GetWeights() const { return weights_; }
    const MultiArray & GetValues() const { return values_; }
  };

}

#endif /* BIIPS_NODEARRAYMONITOR_HPP_ */
