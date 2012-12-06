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

/*! \file   LogicalFactory.cpp
 * \brief   
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "compiler/LogicalFactory.hpp"

namespace Biips
{

  Bool lt(const Types<NodeId>::Array & par1, const Types<NodeId>::Array & par2)
  {
    if (par1.size() == par2.size())
    {
      //Equal sized vectors: Sort by ordering of elements
      for (Size i = 0; i < par1.size(); ++i)
      {
        if (par1[i] < par2[i])
          return true;
        else if (par2[i] < par1[i])
          return false;
      }
      return false;
    }
    else
      return par1.size() < par2.size();
  }

  Bool lt(LogicalPair const & arg1, LogicalPair const & arg2)
  {
    if (arg1.first == arg2.first)
    {
      //Same function: check parameters
      return lt(arg1.second, arg2.second);
    }
    else
    {
      //Diferent functions: sort by address (arbitrary)
      return arg1.first.get() < arg2.first.get();
    }
  }

  NodeId LogicalFactory::NewNode(const Function::Ptr & pFunc,
                                 const Types<NodeId>::Array & parents)
  {
//    else if (LINK(func))
//    {
//      bool ok = true;
//      for (unsigned int i = 0; i < parents.size(); ++i)
//      {
//        if (parents[i]->length() != 1)
//        {
//          ok = false;
//          break;
//        }
//      }
//      if (ok)
//      {
//        node = new LinkNode(LINK(func), parents);
//      }
//      else
//      {
//        throw runtime_error("Invalid vector argument to " + func.name());
//      }
//    }
    return graph_.AddLogicalNode(pFunc, parents);
  }

  NodeId LogicalFactory::GetNode(const Function::Ptr & pFunc,
                                 Types<NodeId>::Array const & parents)
  {
    if (!pFunc)
      throw LogicError("NULL function pointer passed to LogicalFactory::GetNode");

    LogicalPair lpair(pFunc, parents);
    std::map<LogicalPair, NodeId, ltlogical>::iterator it =
        logicalmap_.find(lpair);

    if (it != logicalmap_.end())
      return it->second;

    NodeId id = NewNode(pFunc, parents);
    logicalmap_[lpair] = id;
    return id;

  }

} /* namespace Biips */
