//                                               -*- C++ -*-
/*! \file ConjugateSampler.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_CONJUGATESAMPLER_HPP_
#define BIIPS_CONJUGATESAMPLER_HPP_

#include "sampler/NodeSampler.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/Graph.hpp"
#include "sampler/NodesRelationVisitor.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "sampler/LogLikeVisitor.hpp"

namespace Biips
{

  template<class ConjugateSampler>
  class ConjugateSamplerFactory;

  template<typename ConjugateSampler>
  class LikeFormVisitor;


  template<typename PriorDist, typename LikeDist, Size paramIndex>
  class ConjugateSampler : public NodeSampler
  {
  public:
    typedef ConjugateSampler<PriorDist, LikeDist, paramIndex> SelfType;
    typedef typename Types<SelfType>::Ptr Ptr;
    typedef PriorDist PriorDistType;
    typedef LikeDist LikeDistType;

  protected:
    typedef GraphTypes::StochasticChildrenNodeIdIterator StochasticChildrenNodeIdIterator;

    friend class ConjugateSamplerFactory<SelfType>;
    friend class LikeFormVisitor<SelfType>;

    explicit ConjugateSampler(const Graph * pGraph )
     : NodeSampler(pGraph) {};

    virtual DataType::Array initLikeParamContrib() const;
    virtual void formLikeParamContrib(NodeId likeId, DataType::Array & likeParamContribValues) = 0;
    virtual DataType::Array postParam(const DataType::Array & priorParamContribValues,
        const DataType::Array & likeParamContribValues) const = 0;
    virtual Scalar computeLogWeight(const DataType & sampfedData,
        const DataType::Array & priorParamValues,
        const DataType::Array & postParamValues,
        const DataType::Array & LikeParamContrib = DataType::Array());

  public:
    static Bool IsConjugate(const StochasticNode & node, NodeId priorId, const Graph * pGraph);

    virtual void Visit(const StochasticNode & node);
    virtual Scalar LogWeight() { return logWeight_; };

    virtual const String & Name() const = 0;

    virtual
    ~ConjugateSampler() {};
  };


  template<typename ConjugateSampler>
  class ConjugateSamplerFactory : public NodeSamplerFactory
  {
  public:
    typedef ConjugateSamplerFactory<ConjugateSampler> SelfType;
    typedef typename Types<SelfType>::Ptr Ptr;
    typedef ConjugateSampler CreatedType;
    typedef NodeSamplerFactory BaseType;
    typedef typename ConjugateSampler::PriorDistType PriorDistType;
    typedef typename ConjugateSampler::LikeDistType LikeDistType;

  protected:
    static Ptr pConjugateSamplerFactoryInstance_;
    ConjugateSamplerFactory() {};

  public:
    static BaseType::Ptr Instance() { return pConjugateSamplerFactoryInstance_; };
    virtual Bool Create(const Graph * pGraph, NodeId nodeId, BaseType::CreatedPtr & pNodeSamplerInstance) const;
    virtual ~ConjugateSamplerFactory() {};
  };



  template<typename PriorDist, typename LikeDist, Size paramIndex>
  DataType::Array ConjugateSampler<PriorDist, LikeDist, paramIndex>::initLikeParamContrib() const
  {
    DataType::Array paramContribValues(PriorDist::Instance()->NParam());

    GraphTypes::DirectParentNodeIdIterator it_parents, it_parents_end;
    boost::tie(it_parents, it_parents_end) = pGraph_->GetParents(nodeId_);
    Size i = 0;
    while(it_parents != it_parents_end)
    {
      paramContribValues[i] = DataType(pGraph_->GetNode(*it_parents).DimPtr());
      ++it_parents;
      ++i;
    }
    return paramContribValues;
  }


  template<typename PriorDist, typename LikeDist, Size paramIndex>
  Scalar ConjugateSampler<PriorDist, LikeDist, paramIndex>::computeLogWeight(const DataType & sampledData,
      const DataType::Array & priorParamValues,
      const DataType::Array & postParamValues,
      const DataType::Array & LikeParamContrib)
  {
    Scalar logWeight = PriorDist::Instance()->LogUnnormPdf(sampledData, priorParamValues);
    logWeight -= PriorDist::Instance()->LogUnnormPdf(sampledData, postParamValues);

    StochasticChildrenNodeIdIterator it_offspring, it_offspring_end;
    boost::tie(it_offspring, it_offspring_end) = pGraph_->GetStochasticChildren(nodeId_);

    LogLikeVisitor log_like_vis(pGraph_, nodeId_, this);

    while ( it_offspring != it_offspring_end )
    {
      pGraph_->VisitNode(*it_offspring, log_like_vis);
      ++it_offspring;
    }

    // TODO optimize computation removing constant terms
    return logWeight+log_like_vis.Value();
  }



  template<typename ConjugateSampler>
  class LikeFormVisitor : public ConstNodeVisitor
  {
  protected:
    typedef LikeFormVisitor<ConjugateSampler> SelfType;
    typedef typename Types<SelfType>::Ptr Ptr;
    typedef typename ConjugateSampler::PriorDistType PriorDistType;

    const Graph * pGraph_;
    ConjugateSampler * pNodeSampler_;
    DataType::Array paramContribValues_;

  public:
    virtual void Visit(const ConstantNode & node) {} // TODO throw exception
    virtual void Visit(const LogicalNode & node) {} // TODO throw exception

    virtual void Visit(const StochasticNode & node)
    {
      if (nodeIdDefined_ && pGraph_->GetObserved()[nodeId_] ) // TODO manage else case : throw exception
        pNodeSampler_->formLikeParamContrib(nodeId_, paramContribValues_);
    }

    const DataType::Array & GetParamContribValues() const { return paramContribValues_; }

    LikeFormVisitor(const Graph * pGraph, ConjugateSampler * pNodeSampler) :
      pGraph_(pGraph), pNodeSampler_(pNodeSampler),
      paramContribValues_(pNodeSampler->initLikeParamContrib())
    {
    }
  };


  template<typename PriorDist, typename LikeDist, Size paramIndex>
  void ConjugateSampler<PriorDist, LikeDist, paramIndex>::Visit(const StochasticNode & node)
  {
    if (nodeIdDefined_ && attributesDefined_) // TODO manage else case : throw exception
    {
      DataType::Array prior_param_values = getParamValues(nodeId_, pGraph_, this);

      StochasticChildrenNodeIdIterator it_offspring, it_offspring_end;
      boost::tie(it_offspring, it_offspring_end)
          = pGraph_->GetStochasticChildren(nodeId_);
      LikeFormVisitor<SelfType> like_form_vis(pGraph_, this);
      while (it_offspring != it_offspring_end)
      {
        pGraph_->VisitNode(*it_offspring, like_form_vis);
        ++it_offspring;
      }
      DataType::Array like_param_contrib = like_form_vis.GetParamContribValues();

      DataType::Array post_param_values = postParam(prior_param_values, like_param_contrib);

      DataType sampled_data = PriorDist::Instance()->Sample(post_param_values, pRng_);
      nodeValuesMap_[nodeId_] = sampled_data.ValuesPtr();

      sampledFlagsMap_[nodeId_] = true;

      logWeight_ = computeLogWeight(sampled_data,
          prior_param_values, post_param_values, like_param_contrib);
    }
  }



  template<typename PriorDist, typename LikeDist, Size paramIndex>
  Bool ConjugateSampler<PriorDist, LikeDist, paramIndex>::IsConjugate(const StochasticNode & node,
      NodeId priorId, const Graph * pGraph)
  {
    Bool conjugate = false;
    const Types<NodeId>::Array & params = node.Parents();
    if ( ( node.PriorName() == LikeDist::Instance()->Name() ) && ( params[paramIndex] == priorId ) )
    {
      conjugate = true;
      for (Size i = 0; i < params.size(); ++i)
      {
        if (i != paramIndex)
          conjugate = ( nodesRelation(params[i], priorId, pGraph) == KNOWN );
        if (!conjugate)
          break;
      }
    }
    return conjugate;
  }


  template<typename ConjugateSampler>
  class IsConjugateVisitor : public ConstNodeVisitor
  {
  protected:
    const Graph * pGraph_;
    const NodeId priorId_;
    Bool conjugate_;

  public:
    void Visit(const ConstantNode & node) {}; // TODO throw exception
    void Visit(const LogicalNode & node) {}; // TODO throw exception

    void Visit(const StochasticNode & node)
    {
      if (nodeIdDefined_)
        conjugate_ = ConjugateSampler::IsConjugate(node, priorId_, pGraph_);
    }

    Bool IsConjugate() const { return conjugate_; }

    IsConjugateVisitor(const Graph * pGraph, NodeId myId) :
      pGraph_(pGraph), priorId_(myId), conjugate_(false) {}
  };


  template<typename ConjugateSampler>
  class CanSampleVisitor : public ConstNodeVisitor
  {
  protected:
    typedef GraphTypes::StochasticChildrenNodeIdIterator
        StochasticChildrenNodeIdIterator;

    typedef typename ConjugateSampler::PriorDistType PriorDistType;

    const Graph * pGraph_;
    Bool canSample_;

  public:
    void Visit(const ConstantNode & node) {}; // TODO throw exception
    void Visit(const LogicalNode & node) {}; // TODO throw exception

    void Visit(const StochasticNode & node)
    {
      if (nodeIdDefined_)
      {
        canSample_ = false;
          if (!pGraph_->GetObserved()[nodeId_]) // TODO throw exception
        {
          if (node.PriorName() == PriorDistType::Instance()->Name())
          {
            StochasticChildrenNodeIdIterator it_offspring, it_offspring_end;
            boost::tie(it_offspring, it_offspring_end)
                = pGraph_->GetStochasticChildren(nodeId_);

            IsConjugateVisitor<ConjugateSampler> child_vis(pGraph_, nodeId_);

            while (it_offspring != it_offspring_end)
            {
              if ( pGraph_->GetObserved()[*it_offspring] )
              {
                pGraph_->VisitNode(*it_offspring, child_vis);
                canSample_ = child_vis.IsConjugate();
                if (!canSample_)
                  break;
              }
              ++it_offspring;
            }
          }
        }
      }
    }

    Bool CanSample() const { return canSample_; }

    CanSampleVisitor(const Graph * pGraph) :
      pGraph_(pGraph), canSample_(false) {}
  };


  template<typename ConjugateSampler>
  Bool ConjugateSamplerFactory<ConjugateSampler>::Create(const Graph * pGraph, NodeId nodeId,
      BaseType::CreatedPtr & pNodeSamplerInstance) const
  {
    CanSampleVisitor<CreatedType> can_sample_vis(pGraph);

    pGraph->VisitNode(nodeId, can_sample_vis);

    Bool flag_created = can_sample_vis.CanSample();

    if (flag_created)
      pNodeSamplerInstance = BaseType::CreatedPtr(new CreatedType(pGraph));

    return flag_created;
  }


  template<typename ConjugateSampler>
  typename ConjugateSamplerFactory<ConjugateSampler>::Ptr
  ConjugateSamplerFactory<ConjugateSampler>::pConjugateSamplerFactoryInstance_(
          new SelfType());

}

#endif /* BIIPS_CONJUGATESAMPLER_HPP_ */
