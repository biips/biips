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

/*! \file   Logit.hpp
 * \brief   
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */
#ifndef BIIPS_LOGIT_HPP_
#define BIIPS_LOGIT_HPP_

#include <cmath>

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  struct LogitScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return std::log(val)-std::log(1-val);
    }
  };

  struct ILogitScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return 1/(1 + std::exp(-val));
    }
  };

  class Logit: public UnaryScalarFunction<LogitScalar>
  {
    public:
    typedef Logit SelfType;
    typedef UnaryScalarFunction<LogitScalar> BaseType;

  protected:
    Logit() :
      BaseType("logit")
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

  class ILogit: public UnaryScalarFunction<ILogitScalar>
  {
    public:
    typedef ILogit SelfType;
    typedef UnaryScalarFunction<ILogitScalar> BaseType;

  protected:
    ILogit() :
      BaseType("ilogit")
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

#endif /* BIIPS_LOGIT_HPP_ */
