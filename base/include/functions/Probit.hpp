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

/*! \file   Probit.hpp
 * \brief   
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */
#ifndef BIIPS_PROBIT_HPP_
#define BIIPS_PROBIT_HPP_

#include <cmath>

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  struct ProbitScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const;
  };

  struct PhiScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const;
  };

  class Probit: public UnaryScalarFunction<ProbitScalar>
  {
    public:
    typedef Probit SelfType;
    typedef UnaryScalarFunction<ProbitScalar> BaseType;

  protected:
    Probit() :
      BaseType("probit")
    {
    }

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const;

    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

  class Phi: public UnaryScalarFunction<PhiScalar>
  {
    public:
    typedef Phi SelfType;
    typedef UnaryScalarFunction<PhiScalar> BaseType;

  protected:
    Phi() :
      BaseType("phi")
    {
    }

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

} /* namespace Biips */

#endif /* BIIPS_PROBIT_HPP_ */
