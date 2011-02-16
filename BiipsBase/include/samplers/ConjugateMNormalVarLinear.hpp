//                                               -*- C++ -*-
/*! \file ConjugateMNormalVarLinear.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_CONJUGATEMNORMALVARLINEAR_HPP_
#define BIIPS_CONJUGATEMNORMALVARLINEAR_HPP_

#include "sampler/NodeSampler.hpp"

namespace Biips
{

  class ConjugateMNormalVarLinear : public Biips::NodeSampler
  {
  public:
    typedef ConjugateMNormalVarLinear SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    typedef GraphTypes::StochasticChildrenNodeIdIterator StochasticChildrenNodeIdIterator;

    static const String NAME_;

    friend class ConjugateMNormalVarLinearFactory;

    explicit ConjugateMNormalVarLinear(const Graph * pGraph) : NodeSampler(pGraph) {};

  public:
    virtual void Visit(const StochasticNode & node);
    virtual Scalar LogWeight() { return logWeight_; };

    virtual const String & Name() const { return NAME_; };

    virtual ~ConjugateMNormalVarLinear() {};
  };


  class ConjugateMNormalVarLinearFactory : public NodeSamplerFactory
  {
  public:
    typedef ConjugateMNormalVarLinearFactory SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef ConjugateMNormalVarLinear CreatedType;
    typedef NodeSamplerFactory BaseType;

  protected:
    static Ptr pConjugateMNormalVarLinearFactoryInstance_;
    ConjugateMNormalVarLinearFactory() {};

  public:
    static BaseType::Ptr Instance() { return pConjugateMNormalVarLinearFactoryInstance_; };
    virtual Bool Create(const Graph * pGraph, NodeId nodeId, BaseType::CreatedPtr & pNodeSamplerInstance) const;
  };

}


#endif /* BIIPS_CONJUGATEMNORMALVARLINEAR_HPP_ */
