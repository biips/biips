//                                               -*- C++ -*-
/*! \file ConjugateNormalVarLinear.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_CONJUGATENORMALVARLINEAR_HPP_
#define BIIPS_CONJUGATENORMALVARLINEAR_HPP_

#include "sampler/NodeSampler.hpp"

namespace Biips
{

  class ConjugateNormalVarLinear : public Biips::NodeSampler
  {
  public:
    typedef ConjugateNormalVarLinear SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    typedef GraphTypes::StochasticChildrenNodeIdIterator StochasticChildrenNodeIdIterator;

    static const String NAME_;

    friend class ConjugateNormalVarLinearFactory;

    explicit ConjugateNormalVarLinear(const Graph * pGraph) : NodeSampler(pGraph) {};

  public:
    virtual void Visit(const StochasticNode & node);
    virtual Scalar LogWeight() { return logWeight_; };

    virtual const String & Name() const { return NAME_; };

    virtual ~ConjugateNormalVarLinear() {};
  };


  class ConjugateNormalVarLinearFactory : public NodeSamplerFactory
  {
  public:
    typedef ConjugateNormalVarLinearFactory SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef ConjugateNormalVarLinear CreatedType;
    typedef NodeSamplerFactory BaseType;

  protected:
    static Ptr pConjugateNormalVarLinearFactoryInstance_;
    ConjugateNormalVarLinearFactory() {};

  public:
    static BaseType::Ptr Instance() { return pConjugateNormalVarLinearFactoryInstance_; };
    virtual Bool Create(const Graph * pGraph, NodeId nodeId, BaseType::CreatedPtr & pNodeSamplerInstance) const;
  };

}

#endif /* BIIPS_CONJUGATENORMALVARLINEAR_HPP_ */
