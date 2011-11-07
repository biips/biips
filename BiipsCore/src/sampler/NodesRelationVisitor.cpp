//                                               -*- C++ -*-
/*! \file NodesRelationVisitor.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "sampler/NodesRelationVisitor.hpp"
#include "graph/NodeVisitor.hpp"
#include "graph/Graph.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"

namespace Biips
{

  class NodesRelationVisitor : public ConstNodeVisitor
  {
  protected:
    typedef NodesRelationVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    typedef GraphTypes::StochasticParentNodeIdIterator StochasticParentNodeIdIterator;

    const Graph & graph_;
    NodeId myId_;
    NodesRelationType ans_;

    virtual void visit(const ConstantNode & node)  { ans_ = KNOWN; } // Every ContantNode is KNOWN.
    virtual void visit(const StochasticNode & node);
    virtual void visit(const LogicalNode & node);

  public:
    NodesRelationType GetRelation() const { return ans_; }

    NodesRelationVisitor(const Graph & graph, NodeId myId)
    : graph_(graph), myId_(myId), ans_(UNKNOWN) {}
  };


  void NodesRelationVisitor::visit(const StochasticNode & node)
  {
    if ( nodeId_ == myId_ )
    {
      ans_ = DEPENDING; // A stochastic node depends on itself
      return;
    }
    if ( graph_.GetObserved()[nodeId_] )
    {
      ans_ = KNOWN; // Its value is KNOWN if it is observed
      return;
    }
    Types<NodeId>::ConstIterator it_node_id_1, it_node_id_2, it_node_id_end;
    boost::tie(it_node_id_1, it_node_id_end) = graph_.GetNodes();
    it_node_id_2 = it_node_id_1;
    if ( std::find(it_node_id_1, it_node_id_end, nodeId_) < std::find(it_node_id_2, it_node_id_end, myId_) )
    {
      ans_ = KNOWN;
      // Its value is KNOWN if it has been computed before, i.e. if nodeID_ is before myId_
      // in the nodes sequence of the ForwardSampler object.
      return;
    }
    // else its value is UNKNOWN
    ans_ = UNKNOWN;
  }


  void NodesRelationVisitor::visit(const LogicalNode & node)
  {
    StochasticParentNodeIdIterator it_stoch_parent_id, it_stoch_parent_id_end;
    boost::tie(it_stoch_parent_id, it_stoch_parent_id_end) = graph_.GetStochasticParents(nodeId_);
    ans_ = KNOWN; // First consider nodeId_ is KNOWN
    // Then visit its stochastic parents.
    for (; it_stoch_parent_id != it_stoch_parent_id_end; ++it_stoch_parent_id)
    {
      SelfType vis(graph_, myId_);
      graph_.VisitNode(*it_stoch_parent_id, vis);
      switch ( vis.GetRelation() )
      {
        case DEPENDING:
          ans_ = DEPENDING;
          break;
          // if myId_ depends on one stochastic parent's value, relation is changed to DEPENDING,
          // until other stochastic parent is UNKNOWN
        case UNKNOWN:
          ans_ = UNKNOWN;
          return;
          // if one is UNKNOWN, then nodeId_ is UNKNOWN and the loop is stopped
          break;
        default:
          break;
          // else, i.e. nodeId_ is KNOWN, continue
      }
    }
  }


  // nodeB is expected to be a StochasticNode from the nodeSequence
  NodesRelationType nodesRelation(NodeId nodeA, NodeId nodeB, const Graph & graph)
  {
    NodesRelationVisitor vis(graph, nodeB);
    graph.VisitNode(nodeA, vis);
    return vis.GetRelation();
  }


//  Types<NodesRelationType>::Array getParentnodesRelations(NodeId nodeId, const Graph & graph)
//  {
//    GraphTypes::DirectParentNodeIdIterator it_parents, it_parents_end;
//    boost::tie(it_parents, it_parents_end) = graph.GetParents(nodeId);
//
//    Types<NodesRelationType>::Array parent_relations(std::distance(it_parents, it_parents_end));
//    for (Size i=0; it_parents != it_parents_end; ++it_parents, ++i)
//      parent_relations[i] = nodesRelation(*it_parents, nodeId, graph);
//
//    return parent_relations;
//  }
}
