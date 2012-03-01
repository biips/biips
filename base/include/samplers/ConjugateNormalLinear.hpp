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

  class ConjugateNormalLinear: public NodeSampler
  {
  public:
    typedef ConjugateNormalLinear SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef NodeSampler BaseType;

  protected:
    static const String NAME_;

    friend class ConjugateNormalLinearFactory;

    explicit ConjugateNormalLinear(const Graph & graph) :
      BaseType(graph)
    {
    }

    virtual void sample(const StochasticNode & node);

  public:
    virtual const String & Name() const
    {
      return NAME_;
    }

    virtual ~ConjugateNormalLinear()
    {
    }
  };

  class ConjugateNormalLinearFactory: public NodeSamplerFactory
  {
  public:
    typedef ConjugateNormalLinearFactory SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef ConjugateNormalLinear CreatedType;
    typedef NodeSamplerFactory BaseType;

  protected:
    static Ptr pConjugateNormalLinearFactoryInstance_;
    ConjugateNormalLinearFactory()
    {
    }

  public:
    static BaseType::Ptr Instance()
    {
      return pConjugateNormalLinearFactoryInstance_;
    }
    virtual Bool Create(const Graph & graph,
                        NodeId nodeId,
                        BaseType::CreatedPtr & pNodeSamplerInstance) const;
  };

}

#endif /* BIIPS_CONJUGATENORMALLINEAR_HPP_ */
