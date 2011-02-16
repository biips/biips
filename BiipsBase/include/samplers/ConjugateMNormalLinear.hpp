//                                               -*- C++ -*-
/*! \file ConjugateMNormalLinear.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_CONJUGATEMNORMALLINEAR_HPP_
#define BIIPS_CONJUGATEMNORMALLINEAR_HPP_

#include "sampler/NodeSampler.hpp"

namespace Biips
{

  class ConjugateMNormalLinear : public Biips::NodeSampler
  {
  public:
    typedef ConjugateMNormalLinear SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    typedef GraphTypes::StochasticChildrenNodeIdIterator StochasticChildrenNodeIdIterator;

    static const String NAME_;

    friend class ConjugateMNormalLinearFactory;

    explicit ConjugateMNormalLinear(const Graph * pGraph) : NodeSampler(pGraph) {};

  public:
    virtual void Visit(const StochasticNode & node);
    virtual Scalar LogWeight() { return logWeight_; };

    virtual const String & Name() const { return NAME_; };

    virtual ~ConjugateMNormalLinear() {};
  };


  class ConjugateMNormalLinearFactory : public NodeSamplerFactory
  {
  public:
    typedef ConjugateMNormalLinearFactory SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef ConjugateMNormalLinear CreatedType;
    typedef NodeSamplerFactory BaseType;

  protected:
    static Ptr pConjugateMNormalLinearFactoryInstance_;
    ConjugateMNormalLinearFactory() {};

  public:
    static BaseType::Ptr Instance() { return pConjugateMNormalLinearFactoryInstance_; };
    virtual Bool Create(const Graph * pGraph, NodeId nodeId, BaseType::CreatedPtr & pNodeSamplerInstance) const;
  };

}


#endif /* BIIPS_CONJUGATEMNORMALLINEAR_HPP_ */
