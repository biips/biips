//                                               -*- C++ -*-
/*! \file NodeSampler.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "sampler/NodeSampler.hpp"
#include "graph/Graph.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "sampler/LogLikeVisitor.hpp"
#include "graph/ConstantNode.hpp"
#include "graph/LogicalNode.hpp"
#include "graph/StochasticNode.hpp"

namespace Biips
{

  const String NodeSampler::NAME_ = "Prior";


  void NodeSampler::Visit(const LogicalNode & node)
  {
    if ( nodeIdDefined_ && attributesDefined_ && !sampledFlagsMap_[nodeId_]) // TODO manage else case : throw exception
    {
      MultiArray::Array params = getParamValues(nodeId_, pGraph_, this);
      nodeValuesMap_[nodeId_] = node.Eval(params).ValuesPtr();

      sampledFlagsMap_[nodeId_] = true;
    }
  }


  void NodeSampler::Visit(const StochasticNode & node)
  {
    if ( nodeIdDefined_ && attributesDefined_ ) // TODO manage else case : throw exception
    {
      MultiArray::Array params = getParamValues(nodeId_, pGraph_, this);
      nodeValuesMap_[nodeId_] = node.Sample(params, pRng_).ValuesPtr(); // TODO check dims ?
      sampledFlagsMap_[nodeId_] = true;
    }
  }


  Scalar NodeSampler::LogWeight()
  {
    StochasticChildrenNodeIdIterator it_offspring, it_offspring_end;
    boost::tie(it_offspring, it_offspring_end) = pGraph_->GetStochasticChildren(nodeId_);

    LogLikeVisitor log_like_vis(pGraph_, nodeId_, this);

    while ( it_offspring != it_offspring_end )
    {
      pGraph_->VisitNode(*it_offspring, log_like_vis);
      ++it_offspring;
    }

    logWeight_ = log_like_vis.Value();

    return logWeight_;
  }


  void NodeSampler::Sample(NodeId nodeId)
  {
    pGraph_->VisitNode(nodeId, *this);
  };


  NodeSamplerFactory::Ptr NodeSamplerFactory::pFactoryInstance_(new NodeSamplerFactory());

}
