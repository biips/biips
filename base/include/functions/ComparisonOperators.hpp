//                                               -*- C++ -*-
/*
 * BiiPS software is a set of libraries for
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

/*! \file ComparisonOperators.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_COMPARISONOPERATORS_HPP_
#define BIIPS_COMPARISONOPERATORS_HPP_

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  class GreaterThan: public BinaryScalarFunction<std::greater<Scalar> >
  {
  public:
    typedef GreaterThan SelfType;
    typedef BinaryScalarFunction<std::greater<Scalar> > BaseType;

  protected:
    GreaterThan() :
      BaseType(">")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    virtual Bool IsInfix() const
    {
      return true;
    }
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

  class GreaterOrEqual: public BinaryScalarFunction<std::greater_equal<Scalar> >
  {
  public:
    typedef GreaterOrEqual SelfType;
    typedef BinaryScalarFunction<std::greater_equal<Scalar> > BaseType;

  protected:
    GreaterOrEqual() :
      BaseType(">=")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    virtual Bool IsInfix() const
    {
      return true;
    }
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

  class LessThan: public BinaryScalarFunction<std::less<Scalar> >
  {
  public:
    typedef LessThan SelfType;
    typedef BinaryScalarFunction<std::less<Scalar> > BaseType;

  protected:
    LessThan() :
      BaseType("<")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    virtual Bool IsInfix() const
    {
      return true;
    }
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

  class LessOrEqual: public BinaryScalarFunction<std::less_equal<Scalar> >
  {
  public:
    typedef LessOrEqual SelfType;
    typedef BinaryScalarFunction<std::less_equal<Scalar> > BaseType;

  protected:
    LessOrEqual() :
      BaseType("<=")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    virtual Bool IsInfix() const
    {
      return true;
    }
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

  class Equal: public BinaryScalarFunction<std::equal_to<Scalar> >
  {
  public:
    typedef Equal SelfType;
    typedef BinaryScalarFunction<std::equal_to<Scalar> > BaseType;

  protected:
    Equal() :
      BaseType("==")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    virtual Bool IsInfix() const
    {
      return true;
    }
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

  class NotEqual: public BinaryScalarFunction<std::not_equal_to<Scalar> >
  {
  public:
    typedef NotEqual SelfType;
    typedef BinaryScalarFunction<std::not_equal_to<Scalar> > BaseType;

  protected:
    NotEqual() :
      BaseType("!=")
    {
    }

    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    virtual Bool IsInfix() const
    {
      return true;
    }
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

#endif /* BIIPS_EQUAL_HPP_ */
