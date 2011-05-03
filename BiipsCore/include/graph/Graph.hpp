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

    typedef GraphTypes::FullGraph FullGraph;

    typedef boost::graph_traits<FullGraph>::edge_descriptor EdgeId;
    typedef boost::graph_traits<FullGraph>::vertex_iterator NodeIdIterator;
    typedef boost::graph_traits<FullGraph>::in_edge_iterator InEdgeIdIterator;
    typedef boost::graph_traits<FullGraph>::out_edge_iterator OutEdgeIdIterator;

    typedef GraphTypes::DirectEdgePredicate DirectEdgePredicate;
    typedef GraphTypes::DirectParentGraph DirectParentGraph;
    typedef GraphTypes::DirectParentNodeIdIterator DirectParentNodeIdIterator;

    typedef GraphTypes::DirectChildrenGraph DirectChildrenGraph;
    typedef GraphTypes::DirectChildrenNodeIdIterator DirectChildrenNodeIdIterator;

    typedef GraphTypes::StochasticParentEdgePredicate StochasticParentEdgePredicate;
    typedef GraphTypes::StochasticParentGraph StochasticParentGraph;
    typedef GraphTypes::StochasticParentNodeIdIterator StochasticParentNodeIdIterator;

    typedef GraphTypes::StochasticChildEdgePredicate StochasticChildEdgePredicate;
    typedef GraphTypes::StochasticChildrenGraph StochasticChildrenGraph;
    typedef GraphTypes::StochasticChildrenNodeIdIterator StochasticChildrenNodeIdIterator;

    typedef GraphTypes::EdgeTypePropertyMap EdgeTypePropertyMap;
    typedef GraphTypes::ValuesPropertyMap ValuesPropertyMap;
    typedef GraphTypes::ObservedPropertyMap ObservedPropertyMap;
    typedef GraphTypes::ConstValuesPropertyMap ConstValuesPropertyMap;
    typedef GraphTypes::ConstObservedPropertyMap ConstObservedPropertyMap;

    friend class SetObsValuesVisitor;

    FullGraph fullGraph_;
    DirectParentGraph directParentGraph_;
    DirectChildrenGraph directChildrenGraph_;
    StochasticParentGraph stochasticParentGraph_;
    StochasticChildrenGraph stochasticChildrenGraph_;

    Types<NodeId>::Array topoOrder_;

    Bool builtFlag_;

    std::map<String, Size> nodesSummaryMap_;
    std::map<String, Size> unobsNodesSummaryMap_;

    void topologicalSort();
    void buildStochasticParentEdges();
    void buildStochasticChildrenEdges();
    void setValue(NodeId nodeId, const ValArray::Ptr & pVal) { boost::put(boost::vertex_value, fullGraph_, nodeId, pVal); }
    Types<DimArray::Ptr>::Array getParamDims(const Types<NodeId>::Array parameters) const;

  public:
    Graph();

    NodeId AddConstantNode(const DimArray::Ptr & pDim, const Types<StorageType>::Ptr & pValue);
    NodeId AddConstantNode(const DimArray & dim, const Types<StorageType>::Ptr & pValue);

    NodeId AddConstantNode(const MultiArray & data) { return AddConstantNode(data.DimPtr(), data.ValuesPtr()); }

    NodeId AddAggNode(const DimArray::Ptr & pDim, const Types<NodeId>::Array & parameters, const Types<Size>::Array & offsets);
    NodeId AddAggNode(const DimArray & dim, const Types<NodeId>::Array & parameters, const Types<Size>::Array & offsets);

    NodeId AddLogicalNode(const DimArray::Ptr & pDim, const Function::Ptr & pFunc, const Types<NodeId>::Array & parameters);
    NodeId AddLogicalNode(const DimArray & dim, const Function::Ptr & pFunc, const Types<NodeId>::Array & parameters);
    NodeId AddLogicalNode(const Function::Ptr & pFunc, const Types<NodeId>::Array & parameters);

    NodeId AddStochasticNode(const DimArray::Ptr & pDim, const Distribution::Ptr & pDist, const Types<NodeId>::Array & parameters, Bool observed = false);
    NodeId AddStochasticNode(const DimArray & dim, const Distribution::Ptr & pDist, const Types<NodeId>::Array & parameters, Bool observed = false);
    NodeId AddStochasticNode(const Distribution::Ptr & pDist, const Types<NodeId>::Array & parameters, Bool observed = false);

    NodeId AddStochasticNode(const DimArray::Ptr & pDim, const Distribution::Ptr & pDist, const Types<NodeId>::Array & parameters, const Types<StorageType>::Ptr & pObsValue);
    NodeId AddStochasticNode(const DimArray & dim, const Distribution::Ptr & pDist, const Types<NodeId>::Array & parameters, const Types<StorageType>::Ptr & pObsValue);
    NodeId AddStochasticNode(const Distribution::Ptr & pDist, const Types<NodeId>::Array & parameters, const Types<StorageType>::Ptr & pObsValue);

    void PopNode();

    Types<DirectParentNodeIdIterator>::Pair GetParents(NodeId nodeId) const;
    Types<DirectChildrenNodeIdIterator>::Pair GetChildren(NodeId nodeId) const;
    Types<StochasticParentNodeIdIterator>::Pair GetStochasticParents(NodeId nodeId) const;
    Types<StochasticChildrenNodeIdIterator>::Pair GetStochasticChildren(NodeId nodeId) const;

    Types<NodeId>::ConstIteratorPair GetNodes() const;

    Size GetSize() const { return boost::num_vertices(fullGraph_); };
    Bool Empty() const { return boost::num_vertices(fullGraph_) == 0; };
    Bool IsBuilt() const { return builtFlag_; };

    const std::map<String, Size> & NodesSummary() const { return nodesSummaryMap_; };
    const std::map<String, Size> & UnobsNodesSummary() const { return unobsNodesSummaryMap_; };
    Bool HasCycle() const;
    void Build();
    void VisitNode(NodeId nodeId, NodeVisitor & vis);
    void VisitNode(NodeId nodeId, ConstNodeVisitor & vis) const;
    void VisitGraph(NodeVisitor & vis);
    void VisitGraph(ConstNodeVisitor & vis) const;
    NodeValues SampleValues(Rng * pRng) const;
    ConstObservedPropertyMap GetObserved() const { return boost::get(boost::vertex_observed, fullGraph_); }
    ConstValuesPropertyMap GetValues() const { return boost::get(boost::vertex_value, fullGraph_); }
    void SetObsValue(NodeId nodeId, const ValArray::Ptr & pObsValue);
    void SetObsValues(const NodeValues & nodeValues);

    //Node::Ptr operator[] (NodeId nodeId) { return GetNodePtr(nodeId); };
    Node const & GetNode(NodeId nodeId) const { return *boost::get(boost::vertex_node_ptr, fullGraph_, nodeId); };
    Node const & operator[] (NodeId nodeId) const { return GetNode(nodeId); };

    // TODO remove from the class
    void PrintGraph(std::ostream & os = std::cout) const;
    void PrintTopoOrder(std::ostream & os = std::cout) const;

    template<typename VertexWriter>
    void PrintGraphviz(std::ostream & os, VertexWriter vw) const;
    void PrintGraphviz(std::ostream & os) const;
  };


  template<typename VertexWriter>
  void Graph::PrintGraphviz(std::ostream & os, VertexWriter vw) const
  {
    boost::write_graphviz(os, directChildrenGraph_, vw);
  }


  class VertexPropertyWriter
  {
  protected:
    const Graph & graph_;

  public:
    typedef VertexPropertyWriter SelfType;

    VertexPropertyWriter(const Graph & graph): graph_(graph) {}
    virtual ~VertexPropertyWriter() {}

    virtual void operator()(std::ostream & out, NodeId id) const;

  protected:
    virtual String label(NodeId id) const;

    virtual String shape(NodeId id) const;

    virtual String color(NodeId id) const;

    virtual String style(NodeId id) const;
  };


}




#endif /* BIIPS_GRAPH_HPP_ */