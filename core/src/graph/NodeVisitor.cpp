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

/*! \file NodeVisitor.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "graph/NodeVisitor.hpp"

namespace Biips
{

  void NodeVisitor::Visit(ConstantNode & node)
  {
    if (nodeId_ == NULL_NODEID)
      throw LogicError("Can not visit ConstantNode: node id not set.");

    visit(node);
  }

  void NodeVisitor::Visit(StochasticNode & node)
  {
    if (nodeId_ == NULL_NODEID)
      throw LogicError("Can not visit StochasticNode: node id not set.");

    visit(node);
  }

  void NodeVisitor::Visit(LogicalNode & node)
  {
    if (nodeId_ == NULL_NODEID)
      throw LogicError("Can not visit LogicalNode: node id not set.");

    visit(node);
  }

  void ConstNodeVisitor::Visit(const ConstantNode & node)
  {
    if (nodeId_ == NULL_NODEID)
      throw LogicError("Can not visit ConstantNode: node id not set.");

    visit(node);
  }

  void ConstNodeVisitor::Visit(const StochasticNode & node)
  {
    if (nodeId_ == NULL_NODEID)
      throw LogicError("Can not visit StochasticNode: node id not set.");

    visit(node);
  }

  void ConstNodeVisitor::Visit(const LogicalNode & node)
  {
    if (nodeId_ == NULL_NODEID)
      throw LogicError("Can not visit LogicalNode: node id not set.");

    visit(node);
  }

}
