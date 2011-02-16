//                                               -*- C++ -*-
/*! \file Graph.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include <boost/graph/topological_sort.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_utility.hpp>

#include "graph/Graph.hpp"
#include "sampler/NodeSampler.hpp"
#include "graph/ConstantNode.hpp"
#include "graph/AggNode.hpp"
#include "graph/FuncNode.hpp"
#include "graph/StochasticNode.hpp"

namespace Biips
{

  NodeId Graph::AddConstantNode(const DimArray::Ptr & pDim, const Types<StorageType>::Ptr & pValue)
  {
    NodeId node_id = boost::add_vertex(fullGraph_);
    Node::Ptr new_node(new ConstantNode(pDim));
    boost::put(boost::vertex_node_ptr, fullGraph_, node_id, new_node);
    boost::put(boost::vertex_observed, fullGraph_, node_id, true);
    boost::put(boost::vertex_value, fullGraph_, node_id, pValue);

    nodesSummaryMap_["Constant"] += 1;
    builtFlag_ = false; // TODO also set to false when SetParameter is called
    return node_id;
  }

  NodeId Graph::AddConstantNode(const DimArray & dim, const Types<StorageType>::Ptr & pValue)
  {
    DimArray::Ptr pDim(new DimArray(dim));
    return AddConstantNode(pDim, pValue);
  }


  NodeId Graph::AddAggNode(const DimArray::Ptr & pDim, const Types<NodeId>::Array & parameters, const Types<Size>::Array & offsets)
  {
    NodeId node_id = boost::add_vertex(fullGraph_);

    Node::Ptr new_node(new AggNode(pDim, parameters, offsets));

    boost::put(boost::vertex_node_ptr, fullGraph_, node_id, new_node);

    for (Size i=0; i < parameters.size(); ++i)
    {
      boost::add_edge(node_id, parameters[i], DIRECT_EDGE, fullGraph_); // TODO check parameters
    }

    Bool observed = false;
    for (Size i=0; i < parameters.size(); ++i)
    {
      if ( GetObserved()[parameters[i]] )
        observed = true;
      else
      {
        observed = false;
        break;
      }
    }

    if (observed)
    {
      boost::put(boost::vertex_observed, fullGraph_, node_id, false);
      const ValuesPropertyMap & values_map = boost::get(boost::vertex_value, fullGraph_);
      const ObservedPropertyMap & observed_map = boost::get(boost::vertex_observed, fullGraph_);

      NodeValues node_values(GetSize());
      Flags sampled_flags(GetSize());
      for (NodeId id= 0; id<GetSize(); ++id)
      {
        node_values[id] = values_map[id];
        sampled_flags[id] = observed_map[id];
      }
      NodeSampler sample_node_vis(this);
      Rng * pRng = 0;
      sample_node_vis.SetAttributes(node_values, sampled_flags, pRng);
      VisitNode(node_id, sample_node_vis);

      boost::put(boost::vertex_value, fullGraph_, node_id, node_values[node_id]);
    }

    boost::put(boost::vertex_observed, fullGraph_, node_id, observed);

    nodesSummaryMap_["Logical"] += 1;
    if(!observed)
      unobsNodesSummaryMap_["Logical"] += 1;

    builtFlag_ = false;
    return node_id;
  }


  NodeId Graph::AddAggNode(const DimArray & dim, const Types<NodeId>::Array & parameters, const Types<Size>::Array & offsets)
  {
    DimArray::Ptr pDim(new DimArray(dim));
    return AddAggNode(pDim, parameters, offsets);
  }


  NodeId Graph::AddLogicalNode(const DimArray::Ptr & pDim, const String & funcName, const Types<NodeId>::Array & parameters)
  {
    static FunctionTab & funcTab_ = FuncTab();

    NodeId node_id = boost::add_vertex(fullGraph_);
    // TODO check if funcName is in funcTab_, else throw exception
    Node::Ptr new_node(new FuncNode(pDim, funcTab_[funcName], parameters));
    boost::put(boost::vertex_node_ptr, fullGraph_, node_id, new_node);

    for (Size i=0; i < parameters.size(); ++i)
    {
      boost::add_edge(node_id, parameters[i], DIRECT_EDGE, fullGraph_); // TODO check parameters
    }

    Bool observed = false;
    for (Size i=0; i < parameters.size(); ++i)
    {
      if ( GetObserved()[parameters[i]] )
        observed = true;
      else
      {
        observed = false;
        break;
      }
    }

    if (observed)
    {
      boost::put(boost::vertex_observed, fullGraph_, node_id, false);
      const ValuesPropertyMap & values_map = boost::get(boost::vertex_value, fullGraph_);
      const ObservedPropertyMap & observed_map = boost::get(boost::vertex_observed, fullGraph_);

      NodeValues node_values(GetSize());
      Flags sampled_flags(GetSize());
      for (NodeId id= 0; id<GetSize(); ++id)
      {
        node_values[id] = values_map[id];
        sampled_flags[id] = observed_map[id];
      }
      NodeSampler sample_node_vis(this);
      Rng * pRng = 0;
      sample_node_vis.SetAttributes(node_values, sampled_flags, pRng);
      VisitNode(node_id, sample_node_vis);

      boost::put(boost::vertex_value, fullGraph_, node_id, node_values[node_id]);
    }

    boost::put(boost::vertex_observed, fullGraph_, node_id, observed);

    nodesSummaryMap_["Logical"] += 1;
    if(!observed)
      unobsNodesSummaryMap_["Logical"] += 1;

    builtFlag_ = false;
    return node_id;
  }

  NodeId Graph::AddLogicalNode(const DimArray & dim, const String & funcName, const Types<NodeId>::Array & parameters)
  {
    DimArray::Ptr pDim(new DimArray(dim));
    return AddLogicalNode(pDim, funcName, parameters);
  }


  NodeId Graph::AddStochasticNode(const DimArray::Ptr & pDim, const String & distName, const Types<NodeId>::Array & parameters, Bool observed)
  {
    static DistributionTab & distTab_ = DistTab();

    NodeId node_id = boost::add_vertex(fullGraph_);
    // TODO check if distName is in distTab_, else throw exception
    Node::Ptr new_node(new StochasticNode(pDim, distTab_[distName], parameters));
    boost::put(boost::vertex_node_ptr, fullGraph_, node_id, new_node);
    boost::put(boost::vertex_observed, fullGraph_, node_id, observed);

    for (Size i=0; i < parameters.size(); ++i)
    {
      boost::add_edge(node_id, parameters[i], DIRECT_EDGE, fullGraph_); // TODO check parameters
    }

    nodesSummaryMap_["Stochastic"] += 1;
    if(!observed)
      unobsNodesSummaryMap_["Stochastic"] += 1;

    builtFlag_ = false;
    return node_id;
  }

  NodeId Graph::AddStochasticNode(const DimArray & dim, const String & distName, const Types<NodeId>::Array & parameters, Bool observed)
  {
    DimArray::Ptr pDim(new DimArray(dim));
    return AddStochasticNode(pDim, distName, parameters, observed);
  }


  NodeId Graph::AddStochasticNode(const DimArray::Ptr & pDim, const String & distName, const Types<NodeId>::Array & parameters, const Types<StorageType>::Ptr & pObsValue)
  {
    NodeId node_id = AddStochasticNode(pDim, distName, parameters, true);

    boost::put(boost::vertex_value, fullGraph_, node_id, pObsValue);

    return node_id;
  }

  NodeId Graph::AddStochasticNode(const DimArray & dim, const String & distName, const Types<NodeId>::Array & parameters, const Types<StorageType>::Ptr & pObsValue)
  {
    DimArray::Ptr pDim(new DimArray(dim));
    return AddStochasticNode(pDim, distName, parameters, pObsValue);
  }

  void Graph::topologicalSort()
  {
    boost::topological_sort(directParentGraph_, std::back_inserter(topoOrder_));
  }


  void Graph::buildStochasticParentEdges()
  {
    DirectParentNodeIdIterator it_direct_parents, it_direct_parents_end;
    Types<NodeId>::Array parents;
    StochasticParentNodeIdIterator it_sto_parents, it_sto_parents_end;
    OutEdgeIdIterator it_parent_out_edge_id,  it_parent_out_edge_id_end;
    Types<EdgeId>::Array parent_out_edges;

    // the following algorithm relies on the fact that GetNodes() gives a topological order
    for (Types<NodeId>::ConstIterator it_nodes = topoOrder_.begin(); it_nodes != topoOrder_.end(); ++it_nodes)
    {
      boost::tie(it_direct_parents, it_direct_parents_end) = boost::adjacent_vertices(*it_nodes, directParentGraph_);
      // copy parents ids into another vector, because iterators can be invalidated by modifications of the graph
      parents.assign(it_direct_parents, it_direct_parents_end);
      for (Types<NodeId>::Iterator it_parents = parents.begin(); it_parents != parents.end(); ++it_parents)
      {
        if (GetNode(*it_parents).GetType() == STOCHASTIC)
        {
          boost::tie(it_sto_parents, it_sto_parents_end) = boost::adjacent_vertices(*it_nodes, stochasticParentGraph_);
          if (std::find(it_sto_parents, it_sto_parents_end, *it_parents) == it_sto_parents_end)
            boost::add_edge(*it_nodes, *it_parents, STOCHASTIC_PARENT_EDGE, fullGraph_);
        }
        else
        {
          boost::tie(it_parent_out_edge_id,  it_parent_out_edge_id_end) = boost::out_edges(*it_parents, fullGraph_);
          // copy parent out edge ids into another vector, because iterators can be invalidated by modifications of the graph
          parent_out_edges.assign(it_parent_out_edge_id, it_parent_out_edge_id_end);
          for (Types<EdgeId>::Iterator it_parent_out_edge = parent_out_edges.begin(); it_parent_out_edge != parent_out_edges.end(); ++it_parent_out_edge)
          {
            if (boost::get(boost::edge_type, fullGraph_, *it_parent_out_edge) == STOCHASTIC_PARENT_EDGE)
            {
              NodeId target = boost::target(*it_parent_out_edge, fullGraph_);
              boost::tie(it_sto_parents, it_sto_parents_end) = boost::adjacent_vertices(*it_nodes, stochasticParentGraph_);
              if (std::find(it_sto_parents, it_sto_parents_end, target) == it_sto_parents_end)
                boost::add_edge(*it_nodes, target, STOCHASTIC_PARENT_EDGE, fullGraph_);
            }
          }
        }
      }
    }
  }


  void Graph::buildStochasticChildrenEdges()
  {
    DirectChildrenNodeIdIterator it_direct_children, it_direct_children_end;
    Types<NodeId>::Array children;
    StochasticChildrenNodeIdIterator it_sto_children, it_sto_children_end;
    OutEdgeIdIterator it_child_out_edge_id, it_child_out_edge_id_end;
    Types<EdgeId>::Array child_out_edges;

    for (Types<NodeId>::ConstReverseIterator rit_nodes = topoOrder_.rbegin(); rit_nodes != topoOrder_.rend(); ++rit_nodes)
    {
      boost::tie(it_direct_children, it_direct_children_end) = boost::adjacent_vertices(*rit_nodes, directChildrenGraph_);
      // copy children ids into another vector, because iterators can be invalidated by modifications of the graph
      children.assign(it_direct_children, it_direct_children_end);
      for (Types<NodeId>::Iterator it_children = children.begin(); it_children != children.end(); ++it_children)
      {
        if (GetNode(*it_children).GetType() == STOCHASTIC)
        {
          boost::tie(it_sto_children, it_sto_children_end) = boost::adjacent_vertices(*rit_nodes, stochasticChildrenGraph_);
          if (std::find(it_sto_children, it_sto_children_end, *it_children) == it_sto_children_end)
            boost::add_edge(*rit_nodes, *it_children, STOCHASTIC_CHILD_EDGE, fullGraph_);
        }
        else
        {
          boost::tie(it_child_out_edge_id, it_child_out_edge_id_end) = boost::out_edges(*it_children, fullGraph_);
          // copy child out edge ids into another vector, because iterators can be invalidated by modifications of the graph
          child_out_edges.assign(it_child_out_edge_id, it_child_out_edge_id_end);
          for (Types<EdgeId>::Iterator it_child_out_edge = child_out_edges.begin(); it_child_out_edge != child_out_edges.end(); ++it_child_out_edge)
          {
            if (boost::get(boost::edge_type, fullGraph_, *it_child_out_edge) == STOCHASTIC_CHILD_EDGE)
            {
              NodeId target = boost::target(*it_child_out_edge, fullGraph_);
              boost::tie(it_sto_children, it_sto_children_end) = boost::adjacent_vertices(*rit_nodes, stochasticChildrenGraph_);
              if (std::find(it_sto_children, it_sto_children_end, target) == it_sto_children_end)
                boost::add_edge(*rit_nodes, target, STOCHASTIC_CHILD_EDGE, fullGraph_);
            }
          }
        }
      }
    }
  }

  struct cycle_detector : public boost::dfs_visitor<>
  {
    cycle_detector(bool& has_cycle)
            : m_has_cycle(has_cycle) { }

    template <class Edge, class Graph>
    void back_edge(Edge, Graph&) { m_has_cycle = true; }
  protected:
    bool& m_has_cycle;
  };


  Bool Graph::HasCycle() const
  {
    // are there any cycles in the graph?
    Bool has_cycle = false;
    cycle_detector vis(has_cycle);
    boost::depth_first_search(fullGraph_, boost::visitor(vis));
    return has_cycle;
    // TODO manage bad graphs : throw exception ?
  }


  void Graph::Build()
  {
    topologicalSort();

    buildStochasticParentEdges();

    buildStochasticChildrenEdges();

    builtFlag_ = true;
  }


  void Graph::VisitNode(NodeId nodeId, NodeVisitor & vis)
  {
    vis.SetNodeId(nodeId);
    Node::Ptr pNode = boost::get(boost::vertex_node_ptr, fullGraph_, nodeId);
    pNode->AcceptVisitor(vis);
  }


  void Graph::VisitNode(NodeId nodeId, ConstNodeVisitor & vis) const
  {
    vis.SetNodeId(nodeId);
    GetNode(nodeId).AcceptVisitor(vis);
  }


  void Graph::VisitGraph(NodeVisitor & vis)
  {
    for (Types<NodeId>::ConstIterator it_nodes = topoOrder_.begin(); it_nodes != topoOrder_.end(); ++it_nodes)
    {
      VisitNode(*it_nodes, vis);
    }
  }


  void Graph::VisitGraph(ConstNodeVisitor & vis) const
  {
    for (Types<NodeId>::ConstIterator it_nodes = topoOrder_.begin(); it_nodes != topoOrder_.end(); ++it_nodes)
    {
      VisitNode(*it_nodes, vis);
    }
  }


  NodeValues Graph::SampleValues(Rng * pRng) const
  {
    const ConstValuesPropertyMap & values_map = boost::get(boost::vertex_value, fullGraph_);
    const ConstObservedPropertyMap & observed_map = boost::get(boost::vertex_observed, fullGraph_);

    NodeValues node_values(GetSize());
    Flags sampled_flags(GetSize());
    for (NodeId id= 0; id<GetSize(); ++id)
    {
      node_values[id] = values_map[id];
      sampled_flags[id] = observed_map[id];
    }
    NodeSampler sample_node_vis(this);
    sample_node_vis.SetAttributes(node_values, sampled_flags, pRng);
    VisitGraph(sample_node_vis);
    return node_values;
  }


  class SetObsValuesVisitor : public NodeVisitor
  {
  protected:
    typedef GraphTypes::ConstNodeValuesMap NodeValuesMap;

    Graph * pGraph_;
    NodeValuesMap nodeValuesMap_;

  public:
    virtual void Visit(ConstantNode & node) {};

    virtual void Visit(StochasticNode & node)
    {
      if ( nodeIdDefined_ && pGraph_->GetObserved()[nodeId_] ) // TODO manage else case : throw exception
      {
        pGraph_->setValue(nodeId_, nodeValuesMap_[nodeId_]);
      }
    };
    virtual void Visit(LogicalNode & node) {};

    explicit SetObsValuesVisitor(Graph * pGraph, const NodeValues & nodeValues)
    : pGraph_(pGraph), nodeValuesMap_(boost::make_iterator_property_map(nodeValues.begin(), boost::identity_property_map())) {}
  };


  void Graph::SetObsValues(const NodeValues & nodeValues)
  {
    SetObsValuesVisitor set_obs_vis(this, nodeValues);
    VisitGraph(set_obs_vis);
  }


  void Graph::PrintGraph(std::ostream & os) const // TODO check builtFlag_
  {
    os << "Graph adjacency list (node's direct parents): " << std::endl;
    boost::print_graph(directParentGraph_);

    os << std::endl << "Topological order (parents before children): " << std::endl;
    Types<NodeId>::ConstIterator it_node, it_node_end;
    boost::tie(it_node, it_node_end) = GetNodes();
    os << *it_node;
    for (++it_node; it_node != it_node_end; ++it_node)
      os << ", "<< *it_node;
    os << std::endl;
  };


  Graph::Graph() : directParentGraph_(fullGraph_, DirectEdgePredicate(boost::get(boost::edge_type, fullGraph_))),
      directChildrenGraph_(directParentGraph_),
      stochasticParentGraph_(fullGraph_, StochasticParentEdgePredicate(boost::get(boost::edge_type, fullGraph_))),
      stochasticChildrenGraph_(fullGraph_, StochasticChildEdgePredicate(boost::get(boost::edge_type, fullGraph_))),
      builtFlag_(false)
  {
    nodesSummaryMap_["Constant"] = 0;
    nodesSummaryMap_["Stochastic"] = 0;
    nodesSummaryMap_["Logical"] = 0;
    unobsNodesSummaryMap_["Stochastic"] = 0;
    unobsNodesSummaryMap_["Logical"] = 0;
  };

}
