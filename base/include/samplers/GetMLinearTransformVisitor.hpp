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

/*! \file GetMLinearTransformVisitor.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_GETMLINEARTRANSFORMVISITOR_HPP_
#define BIIPS_GETMLINEARTRANSFORMVISITOR_HPP_

#include "graph/NodeVisitor.hpp"
#include "common/Vector.hpp"
#include "common/Matrix.hpp"

namespace Biips
{

  class Graph;
  class NodeSampler;

  class GetMLinearTransformVisitor: public ConstNodeVisitor
  {
  protected:
    typedef GetMLinearTransformVisitor SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    typedef MultiArray::StorageType StorageType;

    const Graph & graph_;
    NodeId myId_;
    NodeSampler & nodeSampler_;
    Size dimNode_;
    Size dimObs_;
    Matrix A_;
    Vector b_;

    virtual void visit(const ConstantNode & node)
    {
      throw LogicError("ConstantNode can not be visited by GetLinearTransformVisitor.");
    }

    virtual void visit(const StochasticNode & node);

    virtual void visit(const LogicalNode & node);

  public:
    const Matrix & GetA()
    {
      return A_;
    }
    const Vector & GetB()
    {
      return b_;
    }

    GetMLinearTransformVisitor(const Graph & graph,
                               NodeId myId,
                               NodeSampler & nodeSampler,
                               Size dimNode,
                               Size dimObs);
  };

}

#endif /* BIIPS_GETMLINEARTRANSFORMVISITOR_HPP_ */
