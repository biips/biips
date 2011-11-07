//                                               -*- C++ -*-
/*! \file ConjugateNormalVarLinear.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "samplers/ConjugateNormalVarLinear.hpp"
#include "graph/Graph.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "sampler/NodesRelationVisitor.hpp"
#include "samplers/GetLinearTransformVisitor.hpp"
#include "samplers/IsLinearVisitor.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"
#include "distributions/DNormVar.hpp"

namespace Biips
{

  const String ConjugateNormalVarLinear::NAME_ = "Conjugate Normal (with known variance and linear mean function)";


  class NormalVarLinearLikeFormVisitor : public ConstStochasticNodeVisitor
  {
  protected:
    typedef NormalVarLinearLikeFormVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    const Graph & graph_;
    NodeId myId_;
    ConjugateNormalVarLinear & nodeSampler_;
    Scalar mean_;
    Scalar varInv_;

    virtual void visit(const StochasticNode & node)
    {
      mean_ = 0.0;
      varInv_ = 0.0;

      if ( !graph_.GetObserved()[nodeId_] )
        return;

      NodeId mean_id = node.Parents()[0];
      NodeId var_id = node.Parents()[1];

      Scalar var =
          getNodeValue(var_id, graph_, nodeSampler_).ScalarView();
      GetLinearTransformVisitor get_lin_trans_vis(graph_, myId_,
          nodeSampler_);
      graph_.VisitNode(mean_id, get_lin_trans_vis);

      varInv_ = get_lin_trans_vis.GetA() / var;
      mean_ = (graph_.GetValues()[nodeId_]->ScalarView() - get_lin_trans_vis.GetB())
                  * varInv_;
      varInv_ *= get_lin_trans_vis.GetA();
    }

  public:
    Scalar GetMean() const
    {
      return mean_;
    }

    Scalar GetVarInv() const
    {
      return varInv_;
    }

    NormalVarLinearLikeFormVisitor(const Graph & graph, NodeId myId,
        ConjugateNormalVarLinear & nodeSampler) :
      graph_(graph), myId_(myId), nodeSampler_(nodeSampler), mean_(0.0),
      varInv_(0.0)
    {
    }
  };


  void ConjugateNormalVarLinear::sample(const StochasticNode & node)
  {
    NodeId prior_mean_id = node.Parents()[0];
    NodeId prior_var_id = node.Parents()[1];

    Scalar prior_mean =
        getNodeValue(prior_mean_id, graph_, *this).ScalarView();
    Scalar prior_var = getNodeValue(prior_var_id, graph_, *this).ScalarView();

    Scalar like_var_inv = 0.0;
    Scalar like_mean = 0.0;

    StochasticChildrenNodeIdIterator it_offspring, it_offspring_end;
    boost::tie(it_offspring, it_offspring_end)
    = graph_.GetStochasticChildren(nodeId_);
    NormalVarLinearLikeFormVisitor like_form_vis(graph_, nodeId_, *this);
    while (it_offspring != it_offspring_end)
    {
      graph_.VisitNode(*it_offspring, like_form_vis);
      like_var_inv += like_form_vis.GetVarInv();
      like_mean += like_form_vis.GetMean();
      ++it_offspring;
    }

    Scalar post_var = 1 / (1 / prior_var + like_var_inv);
    Scalar post_mean = post_var * (prior_mean / prior_var + like_mean);
    like_mean /= like_var_inv;

    MultiArray::Array post_param_values(2);
    post_param_values[0] = MultiArray(post_mean);
    post_param_values[1] = MultiArray(post_var);
    nodeValuesMap_[nodeId_] = DNormVar::Instance()->Sample(post_param_values, NULL_MULTIARRAYPAIR, *pRng_).ValuesPtr();

    MultiArray::Array norm_const_param_values(2);
    norm_const_param_values[0] = MultiArray(like_mean);
    norm_const_param_values[1] = MultiArray(prior_var + 1 / like_var_inv);
    logIncrementalWeight_ = DNormVar::Instance()->LogDensity(MultiArray(prior_mean), norm_const_param_values, NULL_MULTIARRAYPAIR); // FIXME Boundaries
    if (isNan(logIncrementalWeight_))
      throw RuntimeError("Failure to calculate log incremental weight.");
    // TODO optimize computation removing constant terms

    sampledFlagsMap_[nodeId_] = true;
  }

  class IsConjugateNormalVarLinearVisitor : public ConstStochasticNodeVisitor
  {
  protected:
    const Graph & graph_;
    const NodeId myId_;
    Bool conjugate_;

    void visit(const StochasticNode & node)
    {
      conjugate_ = false;
      if (node.PriorName() != "dnormvar")
        return;

      // FIXME
      if (node.IsBounded())
        return;

      NodeId mean_id = node.Parents()[0];
      NodeId var_id = node.Parents()[1];
      conjugate_ = ((nodesRelation(var_id, myId_, graph_) == KNOWN)
          && isLinear(mean_id, myId_, graph_)) ? true : false;
    }

  public:
    Bool IsConjugate() const
    {
      return conjugate_;
    }

    IsConjugateNormalVarLinearVisitor(const Graph & graph, NodeId myId) :
      graph_(graph), myId_(myId), conjugate_(false)
    {
    }
  };


  class CanSampleNormalVarLinearVisitor : public ConstStochasticNodeVisitor
  {
  protected:
    typedef GraphTypes::StochasticChildrenNodeIdIterator
        StochasticChildrenNodeIdIterator;

    const Graph & graph_;
    Bool canSample_;

    void visit(const StochasticNode & node)
    {
      canSample_ = false;

      if ( graph_.GetObserved()[nodeId_] )
        throw LogicError("CanSampleNormalVarLinearVisitor can not visit observed node: node id sequence of the forward sampler may be bad.");

      if (node.PriorName() != "dnormvar")
        return;

      // FIXME
      if (node.IsBounded())
        return;

      StochasticChildrenNodeIdIterator it_offspring, it_offspring_end;
      boost::tie(it_offspring, it_offspring_end)
      = graph_.GetStochasticChildren(nodeId_);

      IsConjugateNormalVarLinearVisitor child_vis(graph_, nodeId_);

      while (it_offspring != it_offspring_end)
      {
        if ( graph_.GetObserved()[*it_offspring] )
        {
          graph_.VisitNode(*it_offspring, child_vis);
          canSample_ = child_vis.IsConjugate();
          if (!canSample_)
            break;
        }
        ++it_offspring;
      }
    }

  public:
    Bool CanSample() const
    {
      return canSample_;
    }

    CanSampleNormalVarLinearVisitor(const Graph & graph) :
      graph_(graph), canSample_(false)
    {
    }
  };

  Bool
  ConjugateNormalVarLinearFactory::Create(const Graph & graph, NodeId nodeId,
      BaseType::CreatedPtr & pNodeSamplerInstance) const
  {
    CanSampleNormalVarLinearVisitor can_sample_vis(graph);

    graph.VisitNode(nodeId, can_sample_vis);

    Bool flag_created = can_sample_vis.CanSample();

    if (flag_created)
    {
      pNodeSamplerInstance = NodeSamplerFactory::CreatedPtr(new CreatedType(
          graph));
    }

    return flag_created;
  }

  ConjugateNormalVarLinearFactory::Ptr
  ConjugateNormalVarLinearFactory::pConjugateNormalVarLinearFactoryInstance_(
          new ConjugateNormalVarLinearFactory());

}

