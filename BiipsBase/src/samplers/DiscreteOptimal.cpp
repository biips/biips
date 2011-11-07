//                                               -*- C++ -*-
/*! \file DiscreteOptimal.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "samplers/DiscreteOptimal.hpp"
#include "graph/Graph.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "sampler/NodesRelationVisitor.hpp"
#include "sampler/LogLikeVisitor.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"
#include "distributions/DCat.hpp"

namespace Biips
{

  const String DiscreteOptimal::NAME_ = "Discrete Optimal";


  void DiscreteOptimal::sample(const StochasticNode & node)
  {
    NodeValues node_values(graph_.GetSize());
    Flags sampled_flags(graph_.GetSize());

    NodeSampler node_sampler(graph_);

    MultiArray::Array prior_param_values = getParamValues(nodeId_, graph_, *this);

    MultiArray post_param(prior_param_values[0].DimPtr());

    ValArray::Ptr k_chosen(new ValArray(1));

    for (Size k=1; k <= post_param.Length(); ++k)
    {
      for (NodeId id=0; id<graph_.GetSize(); ++id)
      {
        node_values[id] = nodeValuesMap_[id];
        sampled_flags[id] = sampledFlagsMap_[id];
      }
      k_chosen->ScalarView() = Scalar(k);
      node_values[nodeId_] = k_chosen;
      sampled_flags[nodeId_] = true;

      node_sampler.SetMembers(node_values, sampled_flags, pRng_);

      Scalar log_like = getLogLikelihood(graph_, nodeId_, node_sampler);

      Scalar post_param_k = std::exp(std::log(prior_param_values[0].Values()[k-1]) + log_like);
      if (isNan(post_param_k))
        throw RuntimeError("Failure to calculate log posterior parameter.");

      post_param.Values()[k-1] = post_param_k;
    }

    MultiArray::Array post_param_values(1);
    post_param_values[0].SetPtr(post_param);

    nodeValuesMap_[nodeId_] = DCat::Instance()->Sample(post_param_values, NULL_MULTIARRAYPAIR, *pRng_).ValuesPtr(); // FIXME Boundaries

    sampledFlagsMap_[nodeId_] = true;

    logIncrementalWeight_ = std::log(post_param_values[0].Values().Sum());
    if (isNan(logIncrementalWeight_))
      throw RuntimeError("Failure to calculate log incremental weight.");
  }


  class CanSampleDiscreteOptimalVisitor : public ConstStochasticNodeVisitor
  {
  protected:
    typedef GraphTypes::StochasticChildrenNodeIdIterator StochasticChildrenNodeIdIterator;
    typedef GraphTypes::StochasticParentNodeIdIterator StochasticParentNodeIdIterator;

    const Graph & graph_;
    Bool canSample_;

    virtual void visit(const StochasticNode & node)
    {
      canSample_ = false;

      if ( graph_.GetObserved()[nodeId_] )
        throw LogicError("CanSampleDiscreteOptimalVisitor can not visit observed node: node id sequence of the forward sampler may be bad.");

      if ( node.PriorName() != DCat::Instance()->Name() )
        return;

      // FIXME
      if (node.IsBounded())
        return;

      StochasticChildrenNodeIdIterator it_offspring, it_offspring_end;
      boost::tie(it_offspring, it_offspring_end) = graph_.GetStochasticChildren(nodeId_);

      while ( ( it_offspring != it_offspring_end ) &&  (!canSample_) )
      {
        if ( graph_.GetObserved()[*it_offspring] )
        {
          StochasticParentNodeIdIterator it_sto_parents, it_sto_parents_end;
          boost::tie(it_sto_parents, it_sto_parents_end) = graph_.GetStochasticParents(*it_offspring);

          NodesRelationType parent_relation;
          Bool have_like = true;
          while ( (it_sto_parents != it_sto_parents_end) && have_like )
          {
            if ( *it_sto_parents != nodeId_ )
            {
              parent_relation = nodesRelation(*it_sto_parents, nodeId_, graph_);
              have_like = (parent_relation == KNOWN);
            }
            ++it_sto_parents;
          }

          canSample_ = have_like;
        }
        ++it_offspring;
      }
    }

  public:
    Bool CanSample() const { return canSample_; }

    CanSampleDiscreteOptimalVisitor(const Graph & graph) : graph_(graph), canSample_(false) {}
  };



  Bool DiscreteOptimalFactory::Create(const Graph & graph, NodeId nodeId, BaseType::CreatedPtr & pNodeSamplerInstance) const
  {
    CanSampleDiscreteOptimalVisitor can_sample_vis(graph);

    graph.VisitNode(nodeId, can_sample_vis);

    Bool flag_created = can_sample_vis.CanSample();

    if ( flag_created )
    {
      pNodeSamplerInstance = NodeSamplerFactory::CreatedPtr(new CreatedType(graph));
    }

    return flag_created;
  }


  DiscreteOptimalFactory::Ptr DiscreteOptimalFactory::pDiscreteOptimalFactoryInstance_(new DiscreteOptimalFactory());
}
