//                                               -*- C++ -*-
/*! \file DiscreteBruteOptimal.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "samplers/DiscreteBruteOptimal.hpp"
#include "graph/Graph.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "sampler/NodesRelationVisitor.hpp"
#include "sampler/LogLikeVisitor.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"
#include "distributions/DCat.hpp"

namespace Biips
{

  const String DiscreteBruteOptimal::NAME_ = "Discrete Brute Optimal";


  void DiscreteBruteOptimal::Visit(const StochasticNode & node)
  {
    if ( nodeIdDefined_ && attributesDefined_ ) // TODO manage else case : throw exception
    {
      StochasticChildrenNodeIdIterator it_offspring, it_offspring_end;

      NodeValues node_values(pGraph_->GetSize());
      Flags sampled_flags(pGraph_->GetSize());

      NodeSampler sample_node_vis(pGraph_);

      DataType::Array prior_param_values = getParamValues(nodeId_, pGraph_, this);

      DataType::Array post_param_values;
      post_param_values.Alloc(prior_param_values);

      ValArray::Ptr k_chosen(new ValArray(1));

      for (Size k=1; k <= post_param_values[0].Length(); ++k)
      {
        for (NodeId id=0; id<pGraph_->GetSize(); ++id)
        {
          node_values[id] = nodeValuesMap_[id];
          sampled_flags[id] = sampledFlagsMap_[id];
        }
        k_chosen->ScalarView() = Scalar(k);
        node_values[nodeId_] = k_chosen;
        sampled_flags[nodeId_] = true;

        sample_node_vis.SetAttributes(node_values, sampled_flags, pRng_);

        LogLikeVisitor like_form_vis(pGraph_, nodeId_, &sample_node_vis);

        boost::tie(it_offspring, it_offspring_end) = pGraph_->GetStochasticChildren(nodeId_);
        while ( it_offspring != it_offspring_end )
        {
          pGraph_->VisitNode(*it_offspring, like_form_vis);
          ++it_offspring;
        }
        post_param_values[0].Values()[k-1] *= exp(like_form_vis.Value());
      }

      nodeValuesMap_[nodeId_] = DCat::Instance()->Sample(post_param_values, pRng_).ValuesPtr(); // TODO GenerateValue( Numerical::Array, Rng ) to avoid use of pointer function

      sampledFlagsMap_[nodeId_] = true;

      DataType sampled_data(node.DimPtr(), nodeValuesMap_[nodeId_]);

      logWeight_ = log(post_param_values[0].Values().Sum());
    }
  }


  class CanSampleDiscreteCdfInversionVisitor : public ConstNodeVisitor
  {
  protected:
    typedef GraphTypes::StochasticChildrenNodeIdIterator StochasticChildrenNodeIdIterator;
    typedef GraphTypes::StochasticParentNodeIdIterator StochasticParentNodeIdIterator;

    const Graph * pGraph_;
    Bool canSample_;

  public:
    void Visit(const ConstantNode & node) {}; // TODO throw exception
    void Visit(const LogicalNode & node) {}; // TODO throw exception

    void Visit(const StochasticNode & node)
    {
      if ( nodeIdDefined_ )
      {
        canSample_ = false;
        if ( !pGraph_->GetObserved()[nodeId_] ) // TODO throw exception
        {
          if ( node.PriorName() == DCat::Instance()->Name() )
          {
            StochasticChildrenNodeIdIterator it_offspring, it_offspring_end;
            boost::tie(it_offspring, it_offspring_end) = pGraph_->GetStochasticChildren(nodeId_);

            canSample_ = false;
            while ( ( it_offspring != it_offspring_end ) &&  (!canSample_) )
            {
              if ( pGraph_->GetObserved()[*it_offspring] )
              {
                StochasticParentNodeIdIterator it_sto_parents, it_sto_parents_end;
                boost::tie(it_sto_parents, it_sto_parents_end) = pGraph_->GetStochasticParents(*it_offspring);

                NodesRelationType parent_relation;
                Bool have_like = true;
                while ( (it_sto_parents != it_sto_parents_end) && have_like )
                {
                  if ( *it_sto_parents != nodeId_ )
                  {
                    parent_relation = nodesRelation(*it_sto_parents, nodeId_, pGraph_);
                    have_like = (parent_relation == KNOWN);
                  }
                  ++it_sto_parents;
                }

                canSample_ = have_like;
              }
              ++it_offspring;
            }
          }
        }
      }
    };

    Bool CanSample() const { return canSample_; }

    CanSampleDiscreteCdfInversionVisitor(const Graph * pGraph) : pGraph_(pGraph), canSample_(false) {}
  };



  Bool DiscreteBruteOptimalFactory::Create(const Graph * pGraph, NodeId nodeId, BaseType::CreatedPtr & pNodeSamplerInstance) const
  {
    CanSampleDiscreteCdfInversionVisitor can_sample_vis(pGraph);

    pGraph->VisitNode(nodeId, can_sample_vis);

    Bool flag_created = can_sample_vis.CanSample();

    if ( flag_created )
    {
      pNodeSamplerInstance = NodeSamplerFactory::CreatedPtr(new CreatedType(pGraph));
    }

    return flag_created;
  }


  DiscreteBruteOptimalFactory::Ptr DiscreteBruteOptimalFactory::pDiscreteBruteOptimalFactoryInstance_(new DiscreteBruteOptimalFactory());
}
