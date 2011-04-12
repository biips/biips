//                                               -*- C++ -*-
/*! \file ConjugateNormalLinear.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "samplers/ConjugateNormalLinear.hpp"
#include "graph/Graph.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "sampler/NodesRelationVisitor.hpp"
#include "samplers/GetLinearTransformVisitor.hpp"
#include "samplers/IsLinearVisitor.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"
#include "distributions/DNorm.hpp"

namespace Biips
{

  const String ConjugateNormalLinear::NAME_ = "Conjugate Normal (with known precision and linear mean function)";


  class NormalLinearLikeFormVisitor : public ConstNodeVisitor
  {
  protected:
    typedef NormalLinearLikeFormVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    const Graph * pGraph_;
    NodeId myId_;
    ConjugateNormalLinear * pNodeSampler_;
    Scalar mean_;
    Scalar prec_;

  public:
    virtual void
    Visit(const ConstantNode & node)
    {
      mean_ = 0.0;
      prec_ = 0.0;
    }
    ;

    virtual void
    Visit(const StochasticNode & node)
    {
      mean_ = 0.0;
      prec_ = 0.0;
      if (nodeIdDefined_) // TODO manage else case : throw exception
      {
        if ( pGraph_->GetObserved()[nodeId_] ) // TODO throw exception
        {
          NodeId mean_id = node.Parents()[0];
          NodeId prec_id = node.Parents()[1];

          Scalar prec =
              getNodeValue(prec_id, pGraph_, pNodeSampler_).ScalarView();
          GetLinearTransformVisitor get_lin_trans_vis(pGraph_, myId_,
              pNodeSampler_);
          pGraph_->VisitNode(mean_id, get_lin_trans_vis);

          prec_ = get_lin_trans_vis.GetA() * prec;
          mean_ = (pGraph_->GetValues()[nodeId_]->ScalarView() - get_lin_trans_vis.GetB())
              * prec_;
          prec_ *= get_lin_trans_vis.GetA();
        }
      }
    }
    ;

    virtual void
    Visit(const LogicalNode & node)
    {
      mean_ = 0.0;
      prec_ = 0.0;
    }
    ;

    Scalar
    GetMean() const
    {
      return mean_;
    }
    ;
    Scalar
    GetPrec() const
    {
      return prec_;
    }
    ;

    NormalLinearLikeFormVisitor(const Graph * pGraph, NodeId myId,
        ConjugateNormalLinear * pNodeSampler) :
      pGraph_(pGraph), myId_(myId), pNodeSampler_(pNodeSampler), mean_(0.0),
          prec_(0.0)
    {
    }
    ;
  };

  void
  ConjugateNormalLinear::Visit(const StochasticNode & node)
  {
    if (nodeIdDefined_ && attributesDefined_) // TODO manage else case : throw exception
    {
      NodeId prior_mean_id = node.Parents()[0];
      NodeId prior_prec_id = node.Parents()[1];

      Scalar prior_mean =
          getNodeValue(prior_mean_id, pGraph_, this).ScalarView();
      Scalar prior_prec = getNodeValue(prior_prec_id, pGraph_, this).ScalarView();

      Scalar like_prec = 0.0;
      Scalar like_mean = 0.0;

      StochasticChildrenNodeIdIterator it_offspring, it_offspring_end;
      boost::tie(it_offspring, it_offspring_end)
          = pGraph_->GetStochasticChildren(nodeId_);
      NormalLinearLikeFormVisitor like_form_vis(pGraph_, nodeId_, this);
      while (it_offspring != it_offspring_end)
      {
        pGraph_->VisitNode(*it_offspring, like_form_vis);
        like_prec += like_form_vis.GetPrec();
        like_mean += like_form_vis.GetMean();
        ++it_offspring;
      }

      Scalar post_prec = prior_prec + like_prec;
      Scalar post_mean = (prior_mean * prior_prec + like_mean) / post_prec;
      like_mean /= like_prec;

      MultiArray::Array post_param_values(2);
      post_param_values[0] = MultiArray(post_mean);
      post_param_values[1] = MultiArray(post_prec);
      nodeValuesMap_[nodeId_] = DNorm::Instance()->Sample(post_param_values,
          pRng_).ValuesPtr();

      MultiArray::Array norm_const_param_values(2);
      norm_const_param_values[0] = MultiArray(like_mean);
      norm_const_param_values[1] = MultiArray(1.0 / (1.0 / prior_prec + 1.0 / like_prec) );
      logWeight_ = DNorm::Instance()->LogUnnormPdf(MultiArray(prior_mean),
          norm_const_param_values);
      // TODO optimize computation removing constant terms

      sampledFlagsMap_[nodeId_] = true;
    }
  }

  class IsConjugateNormalLinearVisitor : public ConstNodeVisitor
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
        if (node.PriorName() == "dnorm")
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

    IsConjugateNormalLinearVisitor(const Graph * pGraph, NodeId myId) :
      pGraph_(pGraph), myId_(myId), conjugate_(false)
    {
    }
  };

  class CanSampleNormalLinearVisitor : public ConstNodeVisitor
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
          if (node.PriorName() == "dnorm")
          {
            StochasticChildrenNodeIdIterator it_offspring, it_offspring_end;
            boost::tie(it_offspring, it_offspring_end)
                = pGraph_->GetStochasticChildren(nodeId_);

            IsConjugateNormalLinearVisitor child_vis(pGraph_, nodeId_);

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

    CanSampleNormalLinearVisitor(const Graph * pGraph) :
      pGraph_(pGraph), canSample_(false)
    {
    }
  };

  Bool
  ConjugateNormalLinearFactory::Create(const Graph * pGraph, NodeId nodeId,
      BaseType::CreatedPtr & pNodeSamplerInstance) const
  {
    CanSampleNormalLinearVisitor can_sample_vis(pGraph);

    pGraph->VisitNode(nodeId, can_sample_vis);

    Bool flag_created = can_sample_vis.CanSample();

    if (flag_created)
    {
      pNodeSamplerInstance = NodeSamplerFactory::CreatedPtr(new CreatedType(
          pGraph));
    }

    return flag_created;
  }

  ConjugateNormalLinearFactory::Ptr
  ConjugateNormalLinearFactory::pConjugateNormalLinearFactoryInstance_(
          new ConjugateNormalLinearFactory());

}

