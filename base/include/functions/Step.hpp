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

/*! \file Step.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_STEP_HPP_
#define BIIPS_STEP_HPP_

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  struct StepScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return val >= 0.0 ? 1.0 : 0.0;
    }
  };

  class Step: public UnaryScalarFunction<StepScalar>
  {
  public:
    typedef Step SelfType;
    typedef UnaryScalarFunction<StepScalar> BaseType;

  protected:
    Step() :
      BaseType("step")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    virtual Bool IsDiscreteValued(const Flags & mask) const
    {
      return true;
    }
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };

}

#endif /* BIIPS_STEP_HPP_ */
