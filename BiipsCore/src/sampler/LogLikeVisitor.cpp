//                                               -*- C++ -*-
/*! \file LogLikeVisitor.cpp
* \brief
*
* $LastChangedBy$
* $LastChangedDate$
* $LastChangedRevision$
* $Id$
*/

#include "sampler/LogLikeVisitor.hpp"
#include "graph/Graph.hpp"
#include "sampler/NodeSampler.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "graph/StochasticNode.hpp"
#include "sampler/NodesRelationVisitor.hpp"

namespace Biips
{

  void LogLikeVisitor::Visit(const StochasticNode & node)
  {
    if ( nodeIdDefined_ && pGraph_->GetObserved()[nodeId_] ) // TODO manage else case : throw exception
    {
      GraphTypes::StochasticParentNodeIdIterator it_sto_parents, it_sto_parents_end;
      boost::tie(it_sto_parents, it_sto_parents_end) = pGraph_->GetStochasticParents(nodeId_);
      NodesRelationType parent_relation;
      Bool compute_log_like = true;
      while ( it_sto_parents != it_sto_parents_end && compute_log_like)
      {
//        if ( *it_sto_parents != myId_ )
//        {
          parent_relation = nodesRelation(*it_sto_parents, myId_, pGraph_);
          compute_log_like = (parent_relation != UNKNOWN);
//        }
        ++it_sto_parents;
      }

      if (compute_log_like)
      {
        logLikelihood_ += node.LogUnnormLike(MultiArray(node.DimPtr(), pGraph_->GetValues()[nodeId_]),
          getParamValues(nodeId_, pGraph_, pSampleNodeVis_)); // TODO check dims ?
      }
    }
  }


  LogLikeVisitor::LogLikeVisitor(const Graph * pGraph, NodeId myId, NodeSampler * pSampleNodeVis)
   : pGraph_(pGraph), myId_(myId), pSampleNodeVis_(pSampleNodeVis), logLikelihood_(0.0) {};

}
