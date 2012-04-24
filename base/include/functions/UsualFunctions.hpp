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

/*! \file UsualFunctions.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_USUALFUNCTIONS_HPP_
#define BIIPS_USUALFUNCTIONS_HPP_

#include "functions/ScalarFunction.hpp"
#include <boost/math/special_functions/round.hpp>
#include <boost/math/special_functions/trunc.hpp>

namespace Biips
{

  struct AbsScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return std::fabs(val);
    }
  };

  struct SqrtScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return std::sqrt(val);
    }
  };

  struct LogScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return std::log(val);
    }
  };

  struct ExpScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return std::exp(val);
    }
  };

  struct RoundScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return boost::math::round(val);
    }
  };

  struct TruncScalar: public std::unary_function<Scalar, Scalar>
  {
    Scalar operator()(Scalar val) const
    {
      return boost::math::trunc(val);
    }
  };

  class Abs: public UnaryScalarFunction<AbsScalar>
  {
  public:
    typedef Abs SelfType;
    typedef UnaryScalarFunction<AbsScalar> BaseType;

  protected:
    Abs() :
      BaseType("abs")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    virtual Bool IsDiscreteValued(const Flags & mask) const
    {
      return mask[0];
    }
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };

  class Sqrt: public UnaryScalarFunction<SqrtScalar>
  {
  public:
    typedef Sqrt SelfType;
    typedef UnaryScalarFunction<SqrtScalar> BaseType;

  protected:
    Sqrt() :
      BaseType("sqrt")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const;

  public:
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };

  class Log: public UnaryScalarFunction<LogScalar>
  {
  public:
    typedef Log SelfType;
    typedef UnaryScalarFunction<LogScalar> BaseType;

  protected:
    Log() :
      BaseType("log")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const;

  public:
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };

  class Exp: public UnaryScalarFunction<ExpScalar>
  {
  public:
    typedef Exp SelfType;
    typedef UnaryScalarFunction<ExpScalar> BaseType;

  protected:
    Exp() :
      BaseType("exp")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }

  };

  class Round: public UnaryScalarFunction<RoundScalar>
  {
  public:
    typedef Round SelfType;
    typedef UnaryScalarFunction<RoundScalar> BaseType;

  protected:
    Round() :
      BaseType("round")
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

  class Trunc: public UnaryScalarFunction<TruncScalar>
  {
  public:
    typedef Trunc SelfType;
    typedef UnaryScalarFunction<TruncScalar> BaseType;

  protected:
    Trunc() :
      BaseType("trunc")
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

#endif /* BIIPS_USUALFUNCTIONS_HPP_ */
