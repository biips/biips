//                                               -*- C++ -*-
/*! \file Graph.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_GRAPH_HPP_
#define BIIPS_GRAPH_HPP_

#include "GraphTypes.hpp"
#include "function/Function.hpp"
#include "distribution/Distribution.hpp"

#include <boost/graph/graphviz.hpp>

namespace Biips
{

  class NodeVisitor;

  class Graph
  {
  public:
    typedef Graph SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    typedef MultiArray::StorageType StorageType;

    typedef GraphTypes::ParentsGraph ParentsGraph;
    typedef GraphTypes::ChildrenGraph ChildrenGraph;

    typedef GraphTypes::ParentIterator ParentIterator;
    typedef GraphTypes::ChildIterator ChildIterator;
    typedef GraphTypes::StochasticParentIterator StochasticParentIterator;
    typedef GraphTypes::StochasticChildIterator StochasticChildIterator;
    typedef GraphTypes::LikelihoodChildIterator LikelihoodChildIterator;

    typedef GraphTypes::ValuesPropertyMap ValuesPropertyMap;
    typedef GraphTypes::ObservedPropertyMap ObservedPropertyMap;
    typedef GraphTypes::DiscretePropertyMap DiscretePropertyMap;
    typedef GraphTypes::ConstValuesPropertyMap ConstValuesPropertyMap;
    typedef GraphTypes::ConstObservedPropertyMap ConstObservedPropertyMap;
    typedef GraphTypes::ConstDiscretePropertyMap ConstDiscretePropertyMap;

    friend class SetObsValuesVisitor;

    ParentsGraph parentsGraph_;
    ChildrenGraph childrenGraph_;

    Types<std::set<NodeId> >::Array stochasticParents_;
    Types<std::set<NodeId> >::Array stochasticChildren_;
    Types<std::set<NodeId> >::Array likelihoodChildren_;

    Types<NodeId>::Array topoSort_;
    Types<Size>::Array ranks_;

    Bool builtFlag_;

    std::map<String, Size> nodesSummaryMap_;
    std::map<String, Size> unobsNodesSummaryMap_;

    void topologicalSort();
    void buildStochasticParents();
    void buildStochasticChildren();
    void buildLikelihoodChildren();
    void setValue(NodeId nodeId, const ValArray::Ptr & pVal)
    {
      boost::put(boost::vertex_value, parentsGraph_, nodeId, pVal);
    }
    Types<DimArray::Ptr>::Array
        getParamDims(const Types<NodeId>::Array parameters) const;

  public:
    Graph();

    NodeId AddConstantNode(const DimArray::Ptr & pDim,
                           const Types<StorageType>::Ptr & pValue);
    NodeId AddConstantNode(const MultiArray & data)
    {
      return AddConstantNode(data.DimPtr(), data.ValuesPtr());
    }

    NodeId AddAggNode(const DimArray::Ptr & pDim,
                      const Types<NodeId>::Array & parameters,
                      const Types<Size>::Array & offsets);

    NodeId AddLogicalNode(const Function::Ptr & pFunc,
                          const Types<NodeId>::Array & parameters);

    NodeId AddStochasticNode(const Distribution::Ptr & pDist,
                             const Types<NodeId>::Array & parameters,
                             Bool observed,
                             NodeId lower = NULL_NODEID,
                             NodeId upper = NULL_NODEID);
    NodeId AddStochasticNode(const Distribution::Ptr & pDist,
                             const Types<NodeId>::Array & parameters,
                             const Types<StorageType>::Ptr & pObsValue,
                             NodeId lower = NULL_NODEID,
                             NodeId upper = NULL_NODEID);

    void PopNode();

    Types<ParentIterator>::Pair GetParents(NodeId nodeId) const;
    Types<ChildIterator>::Pair GetChildren(NodeId nodeId) const;
    Types<StochasticParentIterator>::Pair
        GetStochasticParents(NodeId nodeId) const;
    Types<StochasticChildIterator>::Pair
        GetStochasticChildren(NodeId nodeId) const;
    Types<LikelihoodChildIterator>::Pair
        GetLikelihoodChildren(NodeId nodeId) const;

    Types<NodeId>::ConstIteratorPair GetSortedNodes() const;

    Size GetSize() const
    {
      return boost::num_vertices(parentsGraph_);
    }
    Bool Empty() const
    {
      return boost::num_vertices(parentsGraph_) == 0;
    }
    ;
    Bool IsBuilt() const
    {
      return builtFlag_;
    }

    const std::map<String, Size> & NodesSummary() const
    {
      return nodesSummaryMap_;
    }
    const std::map<String, Size> & UnobsNodesSummary() const
    {
      return unobsNodesSummaryMap_;
    }
    Bool HasCycle() const;
    void Build();
    void VisitNode(NodeId nodeId, NodeVisitor & vis);
    void VisitNode(NodeId nodeId, ConstNodeVisitor & vis) const;
    void VisitGraph(NodeVisitor & vis);
    void VisitGraph(ConstNodeVisitor & vis) const;
    NodeValues SampleValues(Rng * pRng) const;
    ConstValuesPropertyMap GetValues() const
    {
      return boost::get(boost::vertex_value, parentsGraph_);
    }
    ConstObservedPropertyMap GetObserved() const
    {
      return boost::get(boost::vertex_observed, parentsGraph_);
    }
    ConstDiscretePropertyMap GetDiscrete() const
    {
      return boost::get(boost::vertex_discrete, parentsGraph_);
    }
    void SetObsValue(NodeId nodeId, const ValArray::Ptr & pObsValue);
    void SetObsValues(const NodeValues & nodeValues);

    //Node::Ptr operator[] (NodeId nodeId) { return GetNodePtr(nodeId); };
    Node const & GetNode(NodeId nodeId) const
    {
      return *boost::get(boost::vertex_node_ptr, parentsGraph_, nodeId);
    }
    Node const & operator[](NodeId nodeId) const
    {
      return GetNode(nodeId);
    }

    const Types<Size>::Array & GetRanks() const;

    // TODO remove from the class
    void PrintGraph(std::ostream & os = std::cout) const;

    template<typename VertexWriter>
    void PrintGraphviz(std::ostream & os, VertexWriter vw) const;
    void PrintGraphviz(std::ostream & os) const;
  };


  template<typename VertexWriter>
  void Graph::PrintGraphviz(std::ostream & os, VertexWriter vw) const
  {
    boost::write_graphviz(os, childrenGraph_, vw);
  }


  class VertexPropertyWriter
  {
  protected:
    const Graph & graph_;

  public:
    typedef VertexPropertyWriter SelfType;

    VertexPropertyWriter(const Graph & graph) :
      graph_(graph)
    {
    }
    virtual ~VertexPropertyWriter()
    {
    }

    virtual void operator()(std::ostream & out, NodeId id) const;

  protected:
    virtual String label(NodeId id) const;

    virtual String shape(NodeId id) const;

    virtual String color(NodeId id) const;

    virtual String style(NodeId id) const;
  };

}

#endif /* BIIPS_GRAPH_HPP_ */
