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
#include "distribution/DistError.hpp"

namespace Biips
{

  NodeId Graph::AddConstantNode(const DimArray::Ptr & pDim, const Types<StorageType>::Ptr & pValue)
  {
    if (!pDim)
      throw LogicError("Can not add constant node: DimArray::Ptr is NULL.");

    if (!pValue)
      throw LogicError("Can not add constant node: value pointer is NULL.");

    if (pDim->Length() != pValue->size())
      throw LogicError("Can not add constant node: values size does not match dimension.");

    NodeId node_id = boost::add_vertex(fullGraph_);
    Node::Ptr new_node(new ConstantNode(pDim));
    boost::put(boost::vertex_node_ptr, fullGraph_, node_id, new_node);
    boost::put(boost::vertex_observed, fullGraph_, node_id, true);
    boost::put(boost::vertex_value, fullGraph_, node_id, pValue);

    // set dicreteness
    Bool discrete = true;
    for (Size i = 0; i < pValue->size(); ++i)
    {
      if (!checkInteger((*pValue)[i]))
      {
        discrete = false;
        break;
      }
    }
    boost::put(boost::vertex_discrete, fullGraph_, node_id, discrete);

    nodesSummaryMap_["Constant"] += 1;
    builtFlag_ = false; // TODO also set to false when SetParameter is called
    return node_id;
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
      NodeSampler sample_node_vis(*this);
      Rng::Ptr pRng;
      sample_node_vis.SetMembers(node_values, sampled_flags, pRng);
      VisitNode(node_id, sample_node_vis);

      boost::put(boost::vertex_value, fullGraph_, node_id, node_values[node_id]);
    }

    boost::put(boost::vertex_observed, fullGraph_, node_id, observed);

    //set discreteness
    Bool discrete = true;
    for (Size i = 0; i < parameters.size(); ++i)
    {
      if (!GetDiscrete()[parameters[i]])
      {
        discrete = false;
        break;
      }
    }
    boost::put(boost::vertex_discrete, fullGraph_, node_id, discrete);

    nodesSummaryMap_["Logical"] += 1;
    if(!observed)
      unobsNodesSummaryMap_["Logical"] += 1;

    builtFlag_ = false;
    return node_id;
  }


  Types<DimArray::Ptr>::Array Graph::getParamDims(const Types<NodeId>::Array parameters) const
  {
    Types<DimArray::Ptr>::Array param_dims(parameters.size());
    for (Size i = 0; i<parameters.size(); ++i)
      param_dims[i] = GetNode(parameters[i]).DimPtr();

    return param_dims;
  }


  NodeId Graph::AddLogicalNode(const Function::Ptr & pFunc, const Types<NodeId>::Array & parameters)
  {
    Types<DimArray::Ptr>::Array param_dims = getParamDims(parameters);

    if (!pFunc)
      throw LogicError("Can not add logical node: Function::Ptr is NULL.");

    DimArray::Ptr pDim(new DimArray(pFunc->Dim(param_dims)));

    NodeId node_id = boost::add_vertex(fullGraph_);
    Node::Ptr new_node(new FuncNode(pDim, pFunc, parameters));
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
      NodeSampler sample_node_vis(*this);
      Rng::Ptr pRng;
      sample_node_vis.SetMembers(node_values, sampled_flags, pRng);
      VisitNode(node_id, sample_node_vis);

      boost::put(boost::vertex_value, fullGraph_, node_id, node_values[node_id]);
    }

    boost::put(boost::vertex_observed, fullGraph_, node_id, observed);

    //set discreteness
    Flags mask(parameters.size());
    for (Size i = 0; i < parameters.size(); ++i)
      mask[i] = GetDiscrete()[parameters[i]];

    boost::put(boost::vertex_discrete, fullGraph_, node_id, pFunc->IsDiscreteValued(mask));

    nodesSummaryMap_["Logical"] += 1;
    if(!observed)
      unobsNodesSummaryMap_["Logical"] += 1;

    builtFlag_ = false;
    return node_id;
  }


  NodeId Graph::AddStochasticNode(const Distribution::Ptr & pDist, const Types<NodeId>::Array & parameters,
      Bool observed, NodeId lower, NodeId upper)
  {
    Types<DimArray::Ptr>::Array param_dims = getParamDims(parameters);

    if (!pDist)
      throw LogicError("Can not add stochastic node: Distribution::Ptr is NULL.");

    DimArray::Ptr pDim(new DimArray(pDist->Dim(param_dims)));

    NodeId node_id = boost::add_vertex(fullGraph_);

    Node::Ptr new_node(new StochasticNode(pDim, pDist, parameters, lower, upper));
    boost::put(boost::vertex_node_ptr, fullGraph_, node_id, new_node);
    boost::put(boost::vertex_observed, fullGraph_, node_id, observed);

    for (Size i=0; i < parameters.size(); ++i)
    {
      boost::add_edge(node_id, parameters[i], DIRECT_EDGE, fullGraph_); // TODO check parameters (discreteness)
    }

    //check boundaries
    if (lower != NULL_NODEID)
    {
      if (!pDist->CanBound())
        throw DistError(pDist, "Distribution cannot be bounded");
      if (*(GetNode(lower).DimPtr()) != (*pDim))
        throw DistError(pDist, "Dimension mismatch when setting lower bounds");

      boost::add_edge(node_id, lower, DIRECT_EDGE, fullGraph_);
    }
    if (upper != NULL_NODEID)
    {
      if (!pDist->CanBound())
        throw DistError(pDist, "Distribution cannot be bounded");
      if (*(GetNode(lower).DimPtr()) != (*pDim))
        throw DistError(pDist, "Dimension mismatch when setting upper bounds");

      boost::add_edge(node_id, upper, DIRECT_EDGE, fullGraph_);
    }

    //check discreteness of parents
    Flags mask(parameters.size());
    for (Size i = 0; i < parameters.size(); ++i)
      mask[i] = GetDiscrete()[parameters[i]];
    if (!pDist->CheckParamDiscrete(mask))
      throw DistError(pDist, "Failed check for discrete-valued parameters");

    //set discreteness
    boost::put(boost::vertex_discrete, fullGraph_, node_id, pDist->IsDiscreteValued(mask));

    nodesSummaryMap_["Stochastic"] += 1;
    if(!observed)
      unobsNodesSummaryMap_["Stochastic"] += 1;

    builtFlag_ = false;
    return node_id;
  }


  NodeId Graph::AddStochasticNode(const Distribution::Ptr & pDist, const Types<NodeId>::Array & parameters,
    const Types<StorageType>::Ptr & pObsValue, NodeId lower, NodeId upper)
  {
    NodeId node_id = AddStochasticNode(pDist, parameters, Bool(pObsValue), lower, upper);

    if (!pObsValue)
      throw LogicError("Can not add stochastic node: observed value pointer is NULL.");

    if (pObsValue->size() != GetNode(node_id).Dim().Length())
      throw LogicError("Can not add stochastic node: observed value size does not match dimension.");

    boost::put(boost::vertex_value, fullGraph_, node_id, pObsValue);

    return node_id;
  }


  void Graph::PopNode()
  {
    if (Empty())
      throw LogicError("Can not pop node: the graph is empty.");

    NodeId last_node_id = GetSize()-1;

    DirectChildrenNodeIdIterator it_children, it_children_end;
    boost::tie(it_children, it_children_end) = GetChildren(last_node_id);
    if (it_children != it_children_end)
      throw LogicError("Last inserted node can not have children.");

    boost::clear_vertex(last_node_id, fullGraph_);

    switch(GetNode(last_node_id).GetType())
    {
      case CONSTANT:
        nodesSummaryMap_["Constant"] -= 1;
        break;
      case LOGICAL:
        nodesSummaryMap_["Logical"] -= 1;
        if (!GetObserved()[last_node_id])
          unobsNodesSummaryMap_["Logical"] -= 1;
        break;
      case STOCHASTIC:
        nodesSummaryMap_["Stochastic"] -= 1;
        if (!GetObserved()[last_node_id])
          unobsNodesSummaryMap_["Stochastic"] -= 1;
        break;
      default:
        throw LogicError("Unknown node type.");
        break;

    }
    boost::remove_vertex(last_node_id, fullGraph_);

    builtFlag_ = false;
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

    for (Types<NodeId>::Array::const_reverse_iterator rit_nodes = topoOrder_.rbegin(); rit_nodes != topoOrder_.rend(); ++rit_nodes)
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
    boost::depth_first_search(directParentGraph_, boost::visitor(vis));
    return has_cycle;
  }


  void Graph::Build()
  {
    if (builtFlag_)
      throw LogicError("The graph is already built.");

    if (HasCycle())
      throw RuntimeError("The graph has a cycle.");

    topologicalSort();

    buildStochasticParentEdges();

    buildStochasticChildrenEdges();

    builtFlag_ = true;
  }


  Types<Graph::DirectParentNodeIdIterator>::Pair Graph::GetParents(NodeId nodeId) const
  {
    return boost::adjacent_vertices(nodeId, directParentGraph_);
  };

  Types<Graph::DirectChildrenNodeIdIterator>::Pair Graph::GetChildren(NodeId nodeId) const
  {
    return boost::adjacent_vertices(nodeId, directChildrenGraph_);
  };

  Types<Graph::StochasticParentNodeIdIterator>::Pair Graph::GetStochasticParents(NodeId nodeId) const
  {
    if (!builtFlag_)
      throw LogicError("Can not access a graph that is not built.");

    return boost::adjacent_vertices(nodeId, stochasticParentGraph_);
  };

  Types<Graph::StochasticChildrenNodeIdIterator>::Pair Graph::GetStochasticChildren(NodeId nodeId) const
  {
    if (!builtFlag_)
      throw LogicError("Can not access a graph that is not built.");

    return boost::adjacent_vertices(nodeId, stochasticChildrenGraph_);
  };


  Types<NodeId>::ConstIteratorPair Graph::GetNodes() const
  {
    if (!builtFlag_)
      throw LogicError("Can not access a graph that is not built.");

    return iterRange(topoOrder_);
  };


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


  NodeValues Graph::SampleValues(const Rng::Ptr & pRng) const
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
    NodeSampler sample_node_vis(*this);
    sample_node_vis.SetMembers(node_values, sampled_flags, pRng);
    VisitGraph(sample_node_vis);
    return node_values;
  }


  void Graph::SetObsValue(NodeId nodeId, const ValArray::Ptr & pObsValue)
  {
    if ( GetObserved()[nodeId] ) // TODO manage else case : throw exception
    {
      setValue(nodeId, pObsValue);
    }
  }


  class SetObsValuesVisitor : public NodeVisitor
  {
  protected:
    typedef GraphTypes::ConstNodeValuesMap NodeValuesMap;

    Graph & graph_;
    NodeValuesMap nodeValuesMap_;

    virtual void visit(ConstantNode & node) {}

    virtual void visit(StochasticNode & node)
    {
      if ( graph_.GetObserved()[nodeId_] )
        graph_.setValue(nodeId_, nodeValuesMap_[nodeId_]);
    }

    virtual void visit(LogicalNode & node) {}

  public:

    explicit SetObsValuesVisitor(Graph & graph, const NodeValues & nodeValues)
    : graph_(graph), nodeValuesMap_(boost::make_iterator_property_map(nodeValues.begin(), boost::identity_property_map())) {}
  };


  void Graph::SetObsValues(const NodeValues & nodeValues)
  {
    SetObsValuesVisitor set_obs_vis(*this, nodeValues);
    VisitGraph(set_obs_vis);
  }


  void Graph::PrintGraph(std::ostream & os) const
  {
    boost::print_graph(directParentGraph_);
  };


  void Graph::PrintTopoOrder(std::ostream & os) const
  {
    if (!builtFlag_)
    {
      os << "Can not print topological order: graph is not built." << std::endl;
      return;
    }

    Types<NodeId>::ConstIterator it_node, it_node_end;
    boost::tie(it_node, it_node_end) = GetNodes();
    os << *it_node;
    for (++it_node; it_node != it_node_end; ++it_node)
      os << ", "<< *it_node;
    os << std::endl;
  }


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


  void Graph::PrintGraphviz(std::ostream & os) const
  {
    VertexPropertyWriter vpw(*this);
    PrintGraphviz(os, vpw);
  }


  void VertexPropertyWriter::operator()(std::ostream & out, NodeId id) const
  {
    out << "[label=\"" << label(id) << "\"";
    out << ", shape=\"" << shape(id) << "\"";
    out << ", style=\"" << style(id) << "\"";
    out << ", color=\"" << color(id) << "\"";
    out << ", fillcolor=\"" << "gray" << "\"";

    out << "]";
  }



  class GetLabelVisitor : public ConstNodeVisitor
  {
  protected:
    const Graph & graph_;
    String label_;

    virtual void visit(const ConstantNode & node)
    {
      label_.clear();
    }

    virtual void visit(const StochasticNode & node)
    {
      label_ = node.PriorName() + "\\n";
    }

    virtual void visit(const LogicalNode & node)
    {
      label_ = node.FuncName() + "\\n";
    }

  public:

    const String & GetLabel() const { return label_; }

    explicit GetLabelVisitor(const Graph & graph)
    : graph_(graph) {}
  };


  String VertexPropertyWriter::label(NodeId id) const
  {
    GetLabelVisitor get_label_vis(graph_);
    graph_.VisitNode(id, get_label_vis);
    String ans = get_label_vis.GetLabel();

    std::ostringstream oss;
    oss << id;
    ans += oss.str();

    return ans;
  }


  String VertexPropertyWriter::shape(NodeId id) const
  {
    String ans;
    switch(graph_.GetNode(id).GetType())
    {
      case CONSTANT:
        ans = "box";
        break;
      case LOGICAL:
        ans = "box";
        break;
      case STOCHASTIC:
        ans = "ellipse";
        break;
      default:
        break;
    }
    return ans;
  }


  String VertexPropertyWriter::color(NodeId id) const
  {
    String ans;
    switch(graph_.GetNode(id).GetType())
    {
      case CONSTANT:
        ans += "gray";
        break;
      case LOGICAL:
        break;
      case STOCHASTIC:
        break;
      default:
        break;
    }
    return ans;
  }


  String VertexPropertyWriter::style(NodeId id) const
  {
    String ans;
    switch(graph_.GetNode(id).GetType())
    {
      case CONSTANT:
        break;
      case LOGICAL:
        ans = "rounded";
        break;
      case STOCHASTIC:
        break;
      default:
        break;
    }

    if (graph_.GetObserved()[id])
    {
      ans += ", filled";
    }

    return ans;
  }

}
