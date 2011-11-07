//                                               -*- C++ -*-
/*! \file DiscreteOptimal.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_DISCRETEOPTIMAL_HPP_
#define BIIPS_DISCRETEOPTIMAL_HPP_

#include "sampler/NodeSampler.hpp"

namespace Biips
{

  class DiscreteOptimal : public NodeSampler
  {
  public:
    typedef DiscreteOptimal SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    static const String NAME_;

    friend class DiscreteOptimalFactory;

    explicit DiscreteOptimal(const Graph & graph) : NodeSampler(graph) {};

    virtual void sample(const StochasticNode & node);

  public:
    virtual const String & Name() const { return NAME_; };

    virtual ~DiscreteOptimal() {};
  };


  class DiscreteOptimalFactory : public NodeSamplerFactory
  {
  public:
    typedef DiscreteOptimalFactory SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef DiscreteOptimal CreatedType;
    typedef NodeSamplerFactory BaseType;

  protected:
    static Ptr pDiscreteOptimalFactoryInstance_;
    DiscreteOptimalFactory() {};

  public:
    static BaseType::Ptr Instance() { return pDiscreteOptimalFactoryInstance_; };
    virtual Bool Create(const Graph & graph, NodeId nodeId, BaseType::CreatedPtr & pNodeSamplerInstance) const;
  };

}

#endif /* BIIPS_DISCRETEOPTIMAL_HPP_ */
