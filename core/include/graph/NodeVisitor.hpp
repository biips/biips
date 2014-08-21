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

/*! \file NodeVisitor.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_NODEVISITOR_HPP_
#define BIIPS_NODEVISITOR_HPP_

#include "GraphTypes.hpp"

namespace Biips
{

  class ConstantNode;
  class StochasticNode;
  class LogicalNode;

  class NodeVisitor
  {
  protected:
    NodeId nodeId_;

    NodeVisitor() :
      nodeId_(NULL_NODEID)
    {
    }

    virtual void visit(ConstantNode & node)
    {
      throw LogicError("NodeVisitor: can not visit ConstantNode.");
    }
    virtual void visit(StochasticNode & node)
    {
      throw LogicError("NodeVisitor: can not visit StochasticNode.");
    }
    virtual void visit(LogicalNode & node)
    {
      throw LogicError("NodeVisitor: can not visit LogicalNode.");
    }

  public:
    void Visit(ConstantNode & node);
    void Visit(StochasticNode & node);
    void Visit(LogicalNode & node);

    void SetNodeId(NodeId nodeId)
    {
      nodeId_ = nodeId;
    }

    virtual ~NodeVisitor()
    {
    }
  };

  class ConstNodeVisitor
  {
  protected:
    NodeId nodeId_;

    ConstNodeVisitor() :
      nodeId_(NULL_NODEID)
    {
    }

    virtual void visit(const ConstantNode & node)
    {
      throw LogicError("ConstNodeVisitor: can not visit ConstantNode.");
    }
    virtual void visit(const StochasticNode & node)
    {
      throw LogicError("ConstNodeVisitor: can not visit StochasticNode.");
    }
    virtual void visit(const LogicalNode & node)
    {
      throw LogicError("ConstNodeVisitor: can not visit LogicalNode.");
    }

  public:
    void Visit(const ConstantNode & node);
    void Visit(const StochasticNode & node);
    void Visit(const LogicalNode & node);

    void SetNodeId(NodeId nodeId)
    {
      nodeId_ = nodeId;
    }

    virtual ~ConstNodeVisitor()
    {
    }
  };

}

#endif /* BIIPS_NODEVISITOR_HPP_ */
