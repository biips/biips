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

  class ConjugateMNormalLinear : public NodeSampler
  {
  public:
    typedef ConjugateMNormalLinear SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef NodeSampler BaseType;

  protected:
    typedef GraphTypes::StochasticChildrenNodeIdIterator StochasticChildrenNodeIdIterator;

    static const String NAME_;

    friend class ConjugateMNormalLinearFactory;

    explicit ConjugateMNormalLinear(const Graph & graph) : BaseType(graph) {};

    virtual void sample(const StochasticNode & node);

  public:
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
    virtual Bool Create(const Graph & graph, NodeId nodeId, BaseType::CreatedPtr & pNodeSamplerInstance) const;
  };

}


#endif /* BIIPS_CONJUGATEMNORMALLINEAR_HPP_ */
