//                                               -*- C++ -*-
/*
 * Biips software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * Biips is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of Biips.
 *
 * Biips is free software: you can redistribute it and/or modify
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

/*! \file DeparseVisitor.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY : Adapted from JAGS Node::deparse implementations
 */

#include "model/DeparseVisitor.hpp"
#include "graph/NodeVisitor.hpp"
#include "graph/Graph.hpp"
#include "compiler/Compiler.hpp"
#include "graph/ConstantNode.hpp"
#include "graph/LogicalNode.hpp"
#include "graph/StochasticNode.hpp"
#include "sampler/GetNodeValueVisitor.hpp"
#include <sstream>

namespace Biips
{

  class DeparseVisitor: public ConstNodeVisitor
  {
  public:
    typedef DeparseVisitor SelfType;
    typedef ConstNodeVisitor BaseType;

  protected:
    const Graph & graph_;
    Types<String>::Array parentNames_;
    String name_;

    virtual void visit(const ConstantNode & node);
    virtual void visit(const StochasticNode & node);
    virtual void visit(const LogicalNode & node);

  public:

    const String & GetName() const
    {
      return name_;
    }

    DeparseVisitor(const Graph & graph,
                   const Types<String>::Array & parentNames) :
      graph_(graph), parentNames_(parentNames)
    {
    }
    virtual ~DeparseVisitor()
    {
    }
  };

  void DeparseVisitor::visit(const ConstantNode & node)
  {
    std::ostringstream oss;
    if (node.Dim().IsScalar())
      oss << (*(graph_.GetValues()[nodeId_]))[0];
    else
    {
      const ValArray & val = *(graph_.GetValues()[nodeId_]);
      oss << "c(" << val.front() << "..." << val.back() << ")";
    }
    name_ = oss.str();
  }

  void DeparseVisitor::visit(const StochasticNode & node)
  {
    Size npar = parentNames_.size();
    if (node.IsUpperBounded())
      --npar;
    if (node.IsLowerBounded())
      --npar;
    Distribution::Ptr p_dist = Compiler::DistTab().GetPtr(node.PriorName());
    if (!p_dist->CheckNParam(npar))
    {
      //We might be deparsing after throwing a NodeError, so we
      //don't want to throw another exception.
      name_ = node.PriorName() + "(deparse error)";
      return;
    }

    std::ostringstream oss;

    oss << node.PriorName() << "(";
    Size i = 0;
    for (; i < npar; ++i)
    {
      if (i != 0)
        oss << ",";
      oss << parentNames_[i];
    }
    oss << ")";
    if (node.IsBounded())
    {
      oss << " T(";
      if (node.IsLowerBounded())
        oss << parentNames_[i++];
      oss << ",";
      if (node.IsUpperBounded())
        oss << parentNames_[i++];
      oss << ")";
    }

    name_ = oss.str();
  }

  void DeparseVisitor::visit(const LogicalNode & node)
  {
    if (!node.IsFunction())
    {
      name_ = String("aggregate(") + parentNames_.front() + "..."
          + parentNames_.back() + ")";
      return;
    }

    std::ostringstream oss;
    oss << "(";

    if (Compiler::FuncTab()[node.FuncName()]->IsPrefix())
    {
      if (node.FuncName() == "NEG")
        oss << "-" << parentNames_[0];
      else
        oss << node.FuncName() << parentNames_[0];
    }

    else if (Compiler::FuncTab()[node.FuncName()]->IsInfix())
    {
      for (Size i = 0; i < parentNames_.size(); ++i)
      {
        if (i > 0)
          oss << node.FuncName();

        oss << parentNames_[i];
      }
    }
    else
    {
      oss << node.FuncName() << "(";
      for (Size i = 0; i < parentNames_.size(); ++i)
      {
        if (i > 0)
          oss << ",";

        oss << parentNames_[i];
      }
      oss << ")";
    }

    oss << ")";

    name_ = oss.str();
  }

  String deparse(NodeId nodeId,
                 const Graph & graph,
                 const Types<String>::Array & parentNames)
  {
    DeparseVisitor deparse_vis(graph, parentNames);
    graph.VisitNode(nodeId, deparse_vis);
    return deparse_vis.GetName();
  }
}
