//                                               -*- C++ -*-
/*! \file DiscreteBruteOptimal.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef DISCRETECDFINVERSION_HPP_
#define DISCRETECDFINVERSION_HPP_

#include "sampler/NodeSampler.hpp"

namespace Biips
{

  class DiscreteBruteOptimal : public NodeSampler
  {
  public:
    typedef DiscreteBruteOptimal SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    typedef GraphTypes::StochasticChildrenNodeIdIterator StochasticChildrenNodeIdIterator;

    static const String NAME_;

    friend class DiscreteBruteOptimalFactory;

    explicit DiscreteBruteOptimal(const Graph * pGraph) : NodeSampler(pGraph) {};

  public:
    virtual void Visit(const StochasticNode & node);
    virtual Scalar LogWeight() { return logWeight_; };

    virtual const String & Name() const { return NAME_; };

    virtual ~DiscreteBruteOptimal() {};
  };


  class DiscreteBruteOptimalFactory : public NodeSamplerFactory
  {
  public:
    typedef DiscreteBruteOptimalFactory SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef DiscreteBruteOptimal CreatedType;
    typedef NodeSamplerFactory BaseType;

  protected:
    static Ptr pDiscreteBruteOptimalFactoryInstance_;
    DiscreteBruteOptimalFactory() {};

  public:
    static BaseType::Ptr Instance() { return pDiscreteBruteOptimalFactoryInstance_; };
    virtual Bool Create(const Graph * pGraph, NodeId nodeId, BaseType::CreatedPtr & pNodeSamplerInstance) const;
  };

}

#endif /* DISCRETECDFINVERSION_HPP_ */
