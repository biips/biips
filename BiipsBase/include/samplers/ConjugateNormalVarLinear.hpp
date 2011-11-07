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

  class ConjugateNormalVarLinear : public NodeSampler
  {
  public:
    typedef ConjugateNormalVarLinear SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef NodeSampler BaseType;

  protected:
    typedef GraphTypes::StochasticChildrenNodeIdIterator StochasticChildrenNodeIdIterator;

    static const String NAME_;

    friend class ConjugateNormalVarLinearFactory;

    explicit ConjugateNormalVarLinear(const Graph & graph ) : BaseType(graph) {}

    virtual void sample(const StochasticNode & node);

  public:
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
    virtual Bool Create(const Graph & graph, NodeId nodeId, BaseType::CreatedPtr & pNodeSamplerInstance) const;
  };

}

#endif /* BIIPS_CONJUGATENORMALVARLINEAR_HPP_ */
