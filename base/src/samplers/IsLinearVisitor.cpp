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

/*! \file IsLinearVisitor.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "samplers/IsLinearVisitor.hpp"
#include "graph/NodeVisitor.hpp"
#include "graph/Graph.hpp"
#include "sampler/NodesRelationVisitor.hpp"
#include "graph/StochasticNode.hpp"
#include "graph/LogicalNode.hpp"
#include "functions/ArithmeticOperators.hpp"
#include "functions/MatMult.hpp"

namespace Biips
{

  const std::map<String, LinearFuncType> & linearFuncMap()
  {
    static std::map<String, LinearFuncType> linear_func_map;
    if (linear_func_map.empty())
    {
      linear_func_map[Multiply::Instance()->Name()] = MULTIPLY;
      linear_func_map[Add::Instance()->Name()] = ADD;
      linear_func_map[MatMult::Instance()->Name()] = MAT_MULT;
      linear_func_map[Subtract::Instance()->Name()] = SUBSTRACT;
      linear_func_map[Divide::Instance()->Name()] = DIVIDE;
      linear_func_map[Neg::Instance()->Name()] = NEG;
    }
    return linear_func_map;
  }

  // --------------------------------------------------------------
  // Is Linear
  // --------------------------------------------------------------

  class IsLinearVisitor: public ConstNodeVisitor
  {
  public:
    typedef IsLinearVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    const Graph & graph_;
    NodeId myId_;
    Bool ans_;

    virtual void visit(const ConstantNode & node)
    {
      ans_ = false;
    }

    virtual void visit(const StochasticNode & node);

    virtual void visit(const LogicalNode & node);

  public:
    Bool IsLinear() const
    {
      return ans_;
    }

    IsLinearVisitor(const Graph & graph, NodeId myId) :
      graph_(graph), myId_(myId), ans_(false)
    {
    }
  };

  void IsLinearVisitor::visit(const StochasticNode & node)
  {
    ans_ = (nodeId_ == myId_);
  }

  void IsLinearVisitor::visit(const LogicalNode & node)
  {
    ans_ = false;

    GraphTypes::ParentIterator it_parents, it_parents_end;
    boost::tie(it_parents, it_parents_end) = graph_.GetParents(nodeId_);

    Size n_par = std::distance(it_parents, it_parents_end);
    Flags parents_linear(n_par, false);
    Flags parents_known(n_par, false);

    for (Size i = 0; it_parents != it_parents_end; ++it_parents, ++i)
    {
      SelfType is_linear_vis(graph_, myId_);
      graph_.VisitNode(*it_parents, is_linear_vis);
      parents_linear[i] = is_linear_vis.IsLinear();
      NodesRelationType parent_rel = nodesRelation(*it_parents, myId_, graph_);
      if (parent_rel == UNKNOWN)
        return;
      parents_known[i] = (parent_rel == KNOWN);
    }

    ans_ = node.IsLinear(parents_linear, parents_known);
  }

  // nodeB is expected to be a StochasticNode from the nodeSequence
  Bool isLinear(NodeId nodeA, NodeId nodeB, const Graph & graph)
  {
    IsLinearVisitor vis(graph, nodeB);
    graph.VisitNode(nodeA, vis);
    return vis.IsLinear();
  }

  // --------------------------------------------------------------
  // Is Scale
  // --------------------------------------------------------------

  class IsScaleVisitor: public ConstNodeVisitor
  {
  public:
    typedef IsScaleVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    const Graph & graph_;
    NodeId myId_;
    Bool ans_;

    virtual void visit(const ConstantNode & node)
    {
      ans_ = false;
    }

    virtual void visit(const StochasticNode & node);

    virtual void visit(const LogicalNode & node);

  public:
    Bool IsScale() const
    {
      return ans_;
    }

    IsScaleVisitor(const Graph & graph, NodeId myId) :
      graph_(graph), myId_(myId), ans_(false)
    {
    }
  };

  void IsScaleVisitor::visit(const StochasticNode & node)
  {
    ans_ = (nodeId_ == myId_);
  }

  void IsScaleVisitor::visit(const LogicalNode & node)
  {
    ans_ = false;

    GraphTypes::ParentIterator it_parents, it_parents_end;
    boost::tie(it_parents, it_parents_end) = graph_.GetParents(nodeId_);

    Size n_par = std::distance(it_parents, it_parents_end);
    Flags parents_scale(n_par, false);
    Flags parents_known(n_par, false);

    for (Size i = 0; it_parents != it_parents_end; ++it_parents, ++i)
    {
      SelfType is_scale_vis(graph_, myId_);
      graph_.VisitNode(*it_parents, is_scale_vis);
      parents_scale[i] = is_scale_vis.IsScale();
      NodesRelationType parent_rel = nodesRelation(*it_parents, myId_, graph_);
      if (parent_rel == UNKNOWN)
        return;
      parents_known[i] = (parent_rel == KNOWN);
    }

    ans_ = node.IsScale(parents_scale, parents_known);
  }

  // nodeB is expected to be a StochasticNode from the nodeSequence
  Bool isScale(NodeId nodeA, NodeId nodeB, const Graph & graph)
  {
    IsScaleVisitor vis(graph, nodeB);
    graph.VisitNode(nodeA, vis);
    return vis.IsScale();
  }
}
