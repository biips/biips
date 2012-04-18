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

/*! \file IsLinearVisitor.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_ISLINEARVISITOR_HPP_
#define BIIPS_ISLINEARVISITOR_HPP_

#include "common/Types.hpp"
#include <map>

namespace Biips
{
  class Graph;

  enum LinearFuncType
  {
    ADD, MULTIPLY, MAT_MULT, SUBSTRACT, DIVIDE, NEG
  };

  const std::map<String, LinearFuncType> & linearFuncMap();

  // nodeB is expected to be a StochasticNode from the nodeSequence
  Bool isLinear(NodeId nodeA, NodeId nodeB, const Graph & graph);

  // nodeB is expected to be a StochasticNode from the nodeSequence
  Bool isScale(NodeId nodeA, NodeId nodeB, const Graph & graph);

}

#endif /* BIIPS_CHECKLINEARITYVISITOR_HPP_ */
