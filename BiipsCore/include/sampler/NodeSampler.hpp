//                                               -*- C++ -*-
/*! \file NodeSampler.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_NODESAMPLER_HPP_
#define BIIPS_NODESAMPLER_HPP_

#include "graph/NodeVisitor.hpp"

namespace Biips
{

  class Graph;
  
  class NodeSampler : public ConstNodeVisitor
  {
  public:
    typedef NodeSampler SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    typedef GraphTypes::DirectParentNodeIdIterator DirectParentNodeIdIterator;
    typedef GraphTypes::StochasticChildrenNodeIdIterator StochasticChildrenNodeIdIterator;
    typedef GraphTypes::NodeValuesMap NodeValuesMap;
    typedef GraphTypes::FlagsMap FlagsMap;

    friend class GetNodeValueVisitor;
//    friend class NodeSamplerFactory;

    const Graph * pGraph_;
    NodeValuesMap nodeValuesMap_;
    FlagsMap sampledFlagsMap_;
    Rng * pRng_;
    Scalar logWeight_;
    Bool attributesDefined_;

    static const String NAME_;

  public:
    void Visit(const ConstantNode & node) {};
    void Visit(const LogicalNode & node);
    virtual void Visit(const StochasticNode & node);

    virtual Scalar LogWeight();

    virtual const String & Name() const { return NAME_; };

    template <typename NodeValuesMapType, typename FlagsMapType>
    void SetAttributes(NodeValuesMapType & nodeValues, FlagsMapType & sampledFlags, Rng * pRng);
    void Sample(NodeId nodeId);

    explicit NodeSampler(const Graph * pGraph) :
      pGraph_(pGraph), logWeight_(0.0), attributesDefined_(false) {}

    virtual ~NodeSampler() {};
  };


  class NodeSamplerFactory
  {
  public:
    typedef NodeSamplerFactory SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef NodeSampler CreatedType;
    typedef Types<CreatedType>::Ptr CreatedPtr;

  protected:
    static Ptr pFactoryInstance_;
    NodeSamplerFactory() {};

  public:
    static Ptr Instance() { return pFactoryInstance_; }; // TODO throw exception
    virtual Bool Create(const Graph * pGraph, NodeId nodeId, CreatedPtr & pNodeSamplerInstance) const { pNodeSamplerInstance = CreatedPtr(new CreatedType(pGraph)); return true; };
    virtual ~NodeSamplerFactory() {};
  };


  template <typename NodeValuesMapType, typename FlagsMapType>
  void NodeSampler::SetAttributes(NodeValuesMapType & nodeValues, FlagsMapType & sampledFlags, Rng * pRng)
  {
    nodeValuesMap_ = boost::make_iterator_property_map(nodeValues.begin(), boost::identity_property_map());
    sampledFlagsMap_ = boost::make_iterator_property_map(/*Flags::iterator(*/sampledFlags.begin()/*)*/, boost::identity_property_map());
    pRng_ = pRng;
    logWeight_ = 0.0; // FIXME
    attributesDefined_ = true;
  }

}

#endif /* BIIPS_NODESAMPLER_HPP_ */
