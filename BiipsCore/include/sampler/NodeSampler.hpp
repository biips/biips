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
    typedef GraphTypes::NodeValuesMap NodeValuesMap;
    typedef GraphTypes::FlagsMap FlagsMap;

    friend class GetNodeValueVisitor;

    const Graph & graph_;
    NodeValuesMap nodeValuesMap_;
    FlagsMap sampledFlagsMap_;
    Rng::Ptr pRng_;
    Scalar logIncrementalWeight_;
    Bool membersSet_;

    static const String NAME_;

    virtual void visit(const ConstantNode & node) {}
    virtual void visit(const LogicalNode & node);
    virtual void visit(const StochasticNode & node);
    virtual void sample(const StochasticNode & node);

  public:
    Scalar LogIncrementalWeight() { return logIncrementalWeight_; }

    virtual const String & Name() const { return NAME_; };

    template <typename NodeValuesMapType, typename FlagsMapType>
    void SetMembers(NodeValuesMapType & nodeValues, FlagsMapType & sampledFlags, const Rng::Ptr & pRng);
    void Sample(NodeId nodeId);

    explicit NodeSampler(const Graph & graph) :
      graph_(graph), logIncrementalWeight_(0.0), membersSet_(false) {}

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
    virtual Bool Create(const Graph & graph, NodeId nodeId, CreatedPtr & pNodeSamplerInstance) const { pNodeSamplerInstance = CreatedPtr(new CreatedType(graph)); return true; };
    virtual ~NodeSamplerFactory() {};
  };


  template <typename NodeValuesMapType, typename FlagsMapType>
  void NodeSampler::SetMembers(NodeValuesMapType & nodeValues, FlagsMapType & sampledFlags, const Rng::Ptr & pRng)
  {
    nodeValuesMap_ = boost::make_iterator_property_map(nodeValues.begin(), boost::identity_property_map());
    sampledFlagsMap_ = boost::make_iterator_property_map(/*Flags::iterator(*/sampledFlags.begin()/*)*/, boost::identity_property_map());
    pRng_ = pRng;
    logIncrementalWeight_ = 0.0; // FIXME
    membersSet_ = true;
  }

}

#endif /* BIIPS_NODESAMPLER_HPP_ */
