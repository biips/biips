//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Contributors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

  class NodesRelationVisitor: public ConstNodeVisitor
  {
  protected:
    typedef NodesRelationVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    typedef GraphTypes::StochasticParentIterator StochasticParentNodeIdIterator;

    const Graph & graph_;
    NodeId myId_;
    NodesRelationType ans_;

    virtual void visit(const ConstantNode & node)
    {
      ans_ = KNOWN;
    } // Every ContantNode is KNOWN.
    virtual void visit(const StochasticNode & node);
    virtual void visit(const LogicalNode & node);

  public:
    NodesRelationType GetRelation() const
    {
      return ans_;
    }

    NodesRelationVisitor(const Graph & graph, NodeId myId) :
      graph_(graph), myId_(myId), ans_(UNKNOWN)
    {
    }
  };

  void NodesRelationVisitor::visit(const StochasticNode & node)
  {
    if (nodeId_ == myId_)
    {
      ans_ = DEPENDING; // A stochastic node depends on itself
      return;
    }
    if (graph_.GetObserved()[nodeId_])
    {
      ans_ = KNOWN; // Its value is KNOWN if it is observed
      return;
    }
    Types<NodeId>::ConstIterator it_node_id_1, it_node_id_2, it_node_id_end;
    boost::tie(it_node_id_1, it_node_id_end) = graph_.GetSortedNodes();
    it_node_id_2 = it_node_id_1;
    if (graph_.GetRanks()[nodeId_] < graph_.GetRanks()[myId_])
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
    boost::tie(it_stoch_parent_id, it_stoch_parent_id_end)
        = graph_.GetStochasticParents(nodeId_);
    ans_ = KNOWN; // First consider nodeId_ is KNOWN
    // Then visit its stochastic parents.
    for (; it_stoch_parent_id != it_stoch_parent_id_end; ++it_stoch_parent_id)
    {
      SelfType vis(graph_, myId_);
      graph_.VisitNode(*it_stoch_parent_id, vis);
      switch (vis.GetRelation())
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

  NodesRelationType nodesRelation(NodeId nodeA,
                                  NodeId nodeB,
                                  const Graph & graph)
  {
    // nodeB is expected to be a StochasticNode from the nodeSequence
    if (graph.GetNode(nodeB).GetType() != STOCHASTIC)
      throw LogicError("nodesRelation: nodeB must be stochastic.");
    NodesRelationVisitor vis(graph, nodeB);
    graph.VisitNode(nodeA, vis);
    return vis.GetRelation();
  }

  Bool anyUnknownParent(NodeId id, NodeId sampledId, const Graph & graph)
  {
    GraphTypes::StochasticParentIterator it_parent, it_parent_end;
    boost::tie(it_parent, it_parent_end) = graph.GetStochasticParents(id);
    for (; it_parent != it_parent_end; ++it_parent)
    {
      if (nodesRelation(*it_parent, sampledId, graph) == UNKNOWN)
        return true;
    }
    return false;
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
