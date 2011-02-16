//                                               -*- C++ -*-
/*! \file ConjugateNormalLinear.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_CONJUGATENORMALLINEAR_HPP_
#define BIIPS_CONJUGATENORMALLINEAR_HPP_

#include "sampler/NodeSampler.hpp"

namespace Biips
{

  class ConjugateNormalLinear : public Biips::NodeSampler
  {
  public:
    typedef ConjugateNormalLinear SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    typedef GraphTypes::StochasticChildrenNodeIdIterator StochasticChildrenNodeIdIterator;

    static const String NAME_;

    friend class ConjugateNormalLinearFactory;

    explicit ConjugateNormalLinear(const Graph * pGraph) : NodeSampler(pGraph) {};

  public:
    virtual void Visit(const StochasticNode & node);
    virtual Scalar LogWeight() { return logWeight_; };

    virtual const String & Name() const { return NAME_; };

    virtual ~ConjugateNormalLinear() {};
  };


  class ConjugateNormalLinearFactory : public NodeSamplerFactory
  {
  public:
    typedef ConjugateNormalLinearFactory SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef ConjugateNormalLinear CreatedType;
    typedef NodeSamplerFactory BaseType;

  protected:
    static Ptr pConjugateNormalLinearFactoryInstance_;
    ConjugateNormalLinearFactory() {};

  public:
    static BaseType::Ptr Instance() { return pConjugateNormalLinearFactoryInstance_; };
    virtual Bool Create(const Graph * pGraph, NodeId nodeId, BaseType::CreatedPtr & pNodeSamplerInstance) const;
  };

}

#endif /* BIIPS_CONJUGATENORMALLINEAR_HPP_ */
