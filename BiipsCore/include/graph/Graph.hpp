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


namespace Biips
{

  class NodeVisitor;

  class Graph
  {
  public:
    typedef Graph SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    typedef std::map<String, Function::Ptr> FunctionTab;
    typedef std::map<String, Distribution::Ptr> DistributionTab;

  protected:
    typedef DataType::StorageType StorageType;

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

    void buildDirectEdges();
    void topologicalSort();
    void buildStochasticParentEdges();
    void buildStochasticChildrenEdges();
    void setValue(NodeId nodeId, const ValArray::Ptr & pVal) { boost::put(boost::vertex_value, fullGraph_, nodeId, pVal); }

  public:
    static FunctionTab & FuncTab() { static FunctionTab ans; return ans; };
    static DistributionTab & DistTab() { static DistributionTab ans; return ans; };

    NodeId AddConstantNode(const DimArray::Ptr & pDim, const Types<StorageType>::Ptr & pValue);
    NodeId AddConstantNode(const DimArray & dim, const Types<StorageType>::Ptr & pValue);

    NodeId AddConstantNode(const DataType & data) { return AddConstantNode(data.DimPtr(), data.ValuePtr()); }

    NodeId AddAggNode(const DimArray::Ptr & pDim, const Types<NodeId>::Array & parameters, const Types<Size>::Array & offsets);
    NodeId AddAggNode(const DimArray & dim, const Types<NodeId>::Array & parameters, const Types<Size>::Array & offsets);

    NodeId AddLogicalNode(const DimArray::Ptr & pDim, const String & funcName, const Types<NodeId>::Array & parameters);
    NodeId AddLogicalNode(const DimArray & dim, const String & funcName, const Types<NodeId>::Array & parameters);

    NodeId AddStochasticNode(const DimArray::Ptr & pDim, const String & distName, const Types<NodeId>::Array & parameters, Bool observed = false);
    NodeId AddStochasticNode(const DimArray & dim, const String & distName, const Types<NodeId>::Array & parameters, Bool observed = false);

    NodeId AddStochasticNode(const DimArray::Ptr & pDim, const String & distName, const Types<NodeId>::Array & parameters, const Types<StorageType>::Ptr & pObsValue);
    NodeId AddStochasticNode(const DimArray & dim, const String & distName, const Types<NodeId>::Array & parameters, const Types<StorageType>::Ptr & pObsValue);

    Types<DirectParentNodeIdIterator>::Pair GetParents(NodeId nodeId) const { return boost::adjacent_vertices(nodeId, directParentGraph_); }; // TODO check builtFlag_
    Types<DirectChildrenNodeIdIterator>::Pair GetChildren(NodeId nodeId) const { return boost::adjacent_vertices(nodeId, directChildrenGraph_); }; // TODO check builtFlag_
    Types<StochasticParentNodeIdIterator>::Pair GetStochasticParents(NodeId nodeId) const { return boost::adjacent_vertices(nodeId, stochasticParentGraph_); }; // TODO check builtFlag_
    Types<StochasticChildrenNodeIdIterator>::Pair GetStochasticChildren(NodeId nodeId) const { return boost::adjacent_vertices(nodeId, stochasticChildrenGraph_); }; // TODO check builtFlag_

    Size GetSize() const { return boost::num_vertices(fullGraph_); };
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
    void SetObsValues(const NodeValues & nodeValues);
    Types<NodeId>::ConstIteratorPair GetNodes() const { return iterRange(topoOrder_); }; // TODO check builtFlag_
    void PrintGraph(std::ostream & os = std::cout) const; // TODO check builtFlag_
    //Node::Ptr operator[] (NodeId nodeId) { return GetNodePtr(nodeId); };
    Node const & GetNode(NodeId nodeId) const { return *boost::get(boost::vertex_node_ptr, fullGraph_, nodeId); };
    Node const & operator[] (NodeId nodeId) const { return GetNode(nodeId); };

    Graph();
  };

}




#endif /* BIIPS_GRAPH_HPP_ */
