//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
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

/*! \file   LogicalFactory.hpp
 * \brief   
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_LOGICALFACTORY_HPP_
#define BIIPS_LOGICALFACTORY_HPP_

#include "function/Function.hpp"
#include "graph/Graph.hpp"

namespace Biips
{

  /**
   * A "logical pair", consisting of a function and a vector of arguments,
   * uniquely defines a logical node.
   */
  typedef std::pair<Function::Ptr, Types<NodeId>::Array > LogicalPair;

  /**
   * @short "Less than" operator for LogicalPair objects.
   *
   * Two LogicalPairs A and B are considered to be equivalent
   * (i.e. lt(A,B) and lt(B,A) are both false) if they have the same
   * function, and the same vector of parameters. Observed parameters
   * are considered equal if they have the same value (within a certain
   * numerical tolerance).
   *
   * For non-equivalent LogicalPairs, the ordering is unique, but
   * arbitrary. The only use of this function is for the STL map class
   * with LogicalPair as key.
   */
  Bool lt(const LogicalPair & arg1, const LogicalPair & arg2);

  /**
   * @short STL function object for the map class using LogicalPair as a key
   */
  struct ltlogical
  {
    Bool operator()(const LogicalPair & arg1, const LogicalPair & arg2) const
    {
      return lt(arg1, arg2);
    }
  };

  /**
   * @short Factory object for logical nodes
   *
   * The value of a logical node is determined uniquely by its function
   * and its parameters.  The purpose of a LogicalFactory is to avoid
   * unnecessary duplication of logical nodes by having a factory object
   * for them that will create and/or lookup logical nodes based on
   * these arguments.
   */
  class LogicalFactory
  {
  protected:
    Graph & graph_;
    std::map<LogicalPair, NodeId, ltlogical> logicalmap_;

  public:
    LogicalFactory(Graph & graph) : graph_(graph) {}

    /**
     * Get a logical node with a given function and given parameters.
     * The results are cached, so if a request is repeated, the same
     * node will be returned. If a newly allocated node is returned,
     * it is also added to the given Model.
     */
    NodeId GetNode(const Function::Ptr & pFunc,
                   const Types<NodeId>::Array & param);
    /**
     * Returns a newly allocated LogicalNode.
     */
    NodeId NewNode(const Function::Ptr & pFunc,
                          const Types<NodeId>::Array & parents);
  };

} /* namespace Biips */
#endif /* BIIPS_LOGICALFACTORY_HPP_ */
