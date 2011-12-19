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
#include "graph/NodeVisitor.hpp"
#include "graph/Graph.hpp"
#include "sampler/NodeSampler.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "graph/StochasticNode.hpp"
#include "sampler/NodesRelationVisitor.hpp"

namespace Biips
{

  class LogLikeVisitor : public ConstStochasticNodeVisitor
  {
  protected:
    typedef LogLikeVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    const Graph & graph_;
    NodeId myId_;
    NodeSampler & nodeSampler_;
    Scalar logLikelihood_;

    virtual void visit(const StochasticNode & node);

  public:
    Scalar Value() const { return logLikelihood_; };

    LogLikeVisitor(const Graph & graph, NodeId myId, NodeSampler & nodeSampler);
  };


  void LogLikeVisitor::visit(const StochasticNode & node)
  {
    if ( !graph_.GetObserved()[nodeId_] )
      return;

    //TODO: remove this check ?
    if (anyUnknownParent(nodeId_, myId_, graph_))
      return;

    MultiArray x_value(node.DimPtr(), graph_.GetValues()[nodeId_]);
    MultiArray::Array param_values = getParamValues(nodeId_, graph_, nodeSampler_);
    MultiArray::Pair bound_values = getBoundValues(nodeId_, graph_, nodeSampler_);
    Scalar log_like = node.LogPriorDensity(x_value, param_values, bound_values);
    if(isNan(log_like))
      throw NodeError(nodeId_, "Failure to calculate log density.");

    logLikelihood_ += log_like;
    if(isNan(logLikelihood_))
      throw RuntimeError("Failure to calculate log likelihood.");
  }


  LogLikeVisitor::LogLikeVisitor(const Graph & graph, NodeId myId, NodeSampler & nodeSampler)
   : graph_(graph), myId_(myId), nodeSampler_(nodeSampler), logLikelihood_(0.0) {};


  Scalar getLogLikelihood(const Graph & graph, NodeId myId, NodeSampler & nodeSampler)
  {
    LogLikeVisitor log_like_vis(graph, myId, nodeSampler);
    GraphTypes::LikelihoodChildIterator it_offspring, it_offspring_end;
    boost::tie(it_offspring, it_offspring_end) = graph.GetLikelihoodChildren(myId);
    while ( it_offspring != it_offspring_end )
    {
      graph.VisitNode(*it_offspring, log_like_vis);
      ++it_offspring;
    }
    return log_like_vis.Value();
  }

}
