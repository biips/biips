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


  class NormalVarLinearLikeFormVisitor : public ConstNodeVisitor
  {
  protected:
    typedef NormalVarLinearLikeFormVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    const Graph * pGraph_;
    NodeId myId_;
    ConjugateNormalVarLinear * pNodeSampler_;
    Scalar mean_;
    Scalar varInv_;

  public:
    virtual void
    Visit(const ConstantNode & node)
    {
      mean_ = 0.0;
      varInv_ = 0.0;
    }
    ;

    virtual void
    Visit(const StochasticNode & node)
    {
      mean_ = 0.0;
      varInv_ = 0.0;
      if (nodeIdDefined_) // TODO manage else case : throw exception
      {
        if ( pGraph_->GetObserved()[nodeId_] )
        {
          NodeId mean_id = node.Parents()[0];
          NodeId var_id = node.Parents()[1];

          Scalar var =
              getNodeValue(var_id, pGraph_, pNodeSampler_).ScalarView();
          GetLinearTransformVisitor get_lin_trans_vis(pGraph_, myId_,
              pNodeSampler_);
          pGraph_->VisitNode(mean_id, get_lin_trans_vis);

          varInv_ = get_lin_trans_vis.GetA() / var;
          mean_ = (pGraph_->GetValues()[nodeId_]->ScalarView() - get_lin_trans_vis.GetB())
              * varInv_;
          varInv_ *= get_lin_trans_vis.GetA();
        }
      }
    }
    ;

    virtual void
    Visit(const LogicalNode & node)
    {
      mean_ = 0.0;
      varInv_ = 0.0;
    }
    ;

    Scalar
    GetMean() const
    {
      return mean_;
    }
    ;
    Scalar
    GetVarInv() const
    {
      return varInv_;
    }
    ;

    NormalVarLinearLikeFormVisitor(const Graph * pGraph, NodeId myId,
        ConjugateNormalVarLinear * pNodeSampler) :
      pGraph_(pGraph), myId_(myId), pNodeSampler_(pNodeSampler), mean_(0.0),
          varInv_(0.0)
    {
    }
    ;
  };

  void
  ConjugateNormalVarLinear::Visit(const StochasticNode & node)
  {
    if (nodeIdDefined_ && attributesDefined_) // TODO manage else case : throw exception
    {
      NodeId prior_mean_id = node.Parents()[0];
      NodeId prior_var_id = node.Parents()[1];

      Scalar prior_mean =
          getNodeValue(prior_mean_id, pGraph_, this).ScalarView();
      Scalar prior_var = getNodeValue(prior_var_id, pGraph_, this).ScalarView();

      Scalar like_var_inv = 0.0;
      Scalar like_mean = 0.0;

      StochasticChildrenNodeIdIterator it_offspring, it_offspring_end;
      boost::tie(it_offspring, it_offspring_end)
          = pGraph_->GetStochasticChildren(nodeId_);
      NormalVarLinearLikeFormVisitor like_form_vis(pGraph_, nodeId_, this);
      while (it_offspring != it_offspring_end)
      {
        pGraph_->VisitNode(*it_offspring, like_form_vis);
        like_var_inv += like_form_vis.GetVarInv();
        like_mean += like_form_vis.GetMean();
        ++it_offspring;
      }

      Scalar post_var = 1 / (1 / prior_var + like_var_inv);
      Scalar post_mean = post_var * (prior_mean / prior_var + like_mean);
      like_mean /= like_var_inv;

      DataType::Array post_param_values(2);
      post_param_values[0] = DataType(post_mean);
      post_param_values[1] = DataType(post_var);
      nodeValuesMap_[nodeId_] = DNormVar::Instance()->Sample(post_param_values,
          pRng_).ValuePtr();

      DataType::Array norm_const_param_values(2);
      norm_const_param_values[0] = DataType(like_mean);
      norm_const_param_values[1] = DataType(prior_var + 1 / like_var_inv);
      logWeight_ = DNormVar::Instance()->LogUnnormPdf(DataType(prior_mean),
          norm_const_param_values);
      // TODO optimize computation removing constant terms

      sampledFlagsMap_[nodeId_] = true;
    }
  }

  class IsConjugateNormalVarLinearVisitor : public ConstNodeVisitor
  {
  protected:
    const Graph * pGraph_;
    const NodeId myId_;
    Bool conjugate_;

  public:
    void
    Visit(const ConstantNode & node)
    {
    }
    ; // TODO throw exception
    void
    Visit(const LogicalNode & node)
    {
    }
    ; // TODO throw exception

    void
    Visit(const StochasticNode & node)
    {
      if (nodeIdDefined_)
      {
        conjugate_ = false;
        if (node.PriorName() == "dnorm.var")
        {
          NodeId mean_id = node.Parents()[0];
          NodeId var_id = node.Parents()[1];
          conjugate_ = ((nodesRelation(var_id, myId_, pGraph_) == KNOWN)
              && isLinear(mean_id, myId_, pGraph_)) ? true : false;
        }
      }
    }

    Bool
    IsConjugate() const
    {
      return conjugate_;
    }

    IsConjugateNormalVarLinearVisitor(const Graph * pGraph, NodeId myId) :
      pGraph_(pGraph), myId_(myId), conjugate_(false)
    {
    }
  };


  class CanSampleNormalVarLinearVisitor : public ConstNodeVisitor
  {
  protected:
    typedef GraphTypes::StochasticChildrenNodeIdIterator
        StochasticChildrenNodeIdIterator;

    const Graph * pGraph_;
    Bool canSample_;

  public:
    void
    Visit(const ConstantNode & node)
    {
    }
    ; // TODO throw exception
    void
    Visit(const LogicalNode & node)
    {
    }
    ; // TODO throw exception

    void
    Visit(const StochasticNode & node)
    {
      if (nodeIdDefined_)
      {
        canSample_ = false;
        if ( !pGraph_->GetObserved()[nodeId_] ) // TODO throw exception
        {
          if (node.PriorName() == "dnorm.var")
          {
            StochasticChildrenNodeIdIterator it_offspring, it_offspring_end;
            boost::tie(it_offspring, it_offspring_end)
                = pGraph_->GetStochasticChildren(nodeId_);

            IsConjugateNormalVarLinearVisitor child_vis(pGraph_, nodeId_);

            while (it_offspring != it_offspring_end)
            {
              if ( pGraph_->GetObserved()[*it_offspring] )
              {
                pGraph_->VisitNode(*it_offspring, child_vis);
                if (child_vis.IsConjugate())
                {
                  canSample_ = true;
                }
                else
                {
                  canSample_ = false;
                  break;
                }
              }
              ++it_offspring;
            }
          }
        }
      }
    }
    ;

    Bool
    CanSample() const
    {
      return canSample_;
    }

    CanSampleNormalVarLinearVisitor(const Graph * pGraph) :
      pGraph_(pGraph), canSample_(false)
    {
    }
  };

  Bool
  ConjugateNormalVarLinearFactory::Create(const Graph * pGraph, NodeId nodeId,
      BaseType::CreatedPtr & pNodeSamplerInstance) const
  {
    CanSampleNormalVarLinearVisitor can_sample_vis(pGraph);

    pGraph->VisitNode(nodeId, can_sample_vis);

    Bool flag_created = can_sample_vis.CanSample();

    if (flag_created)
    {
      pNodeSamplerInstance = NodeSamplerFactory::CreatedPtr(new CreatedType(
          pGraph));
    }

    return flag_created;
  }

  ConjugateNormalVarLinearFactory::Ptr
  ConjugateNormalVarLinearFactory::pConjugateNormalVarLinearFactoryInstance_(
          new ConjugateNormalVarLinearFactory());

}

