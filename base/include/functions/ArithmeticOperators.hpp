//                                               -*- C++ -*-
/*
 * Biips software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * Biips is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of Biips.
 *
 * Biips is free software: you can redistribute it and/or modify
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

/*! \file ArithmeticOperators.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_ARITHMETICOPERATORS_HPP_
#define BIIPS_ARITHMETICOPERATORS_HPP_

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  class Add: public VariableScalarFunction<std::plus<Scalar> >
  {
  public:
    typedef Add SelfType;
    typedef VariableScalarFunction<std::plus<Scalar> > BaseType;

  protected:
    Add() :
      BaseType("+")
    {
    }

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

    virtual Bool IsInfix() const
    {
      return true;
    }
    virtual Bool IsDiscreteValued(const Flags & mask) const;
    virtual Bool
    IsLinear(const Flags & linearMask, const Flags & knownMask) const;

    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

  class Subtract: public BinaryScalarFunction<std::minus<Scalar> >
  {
  public:
    typedef Subtract SelfType;
    typedef BinaryScalarFunction<std::minus<Scalar> > BaseType;

  protected:
    Subtract() :
      BaseType("-")
    {
    }

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

    virtual Bool IsInfix() const
    {
      return true;
    }
    virtual Bool IsDiscreteValued(const Flags & mask) const;
    virtual Bool
    IsLinear(const Flags & linearMask, const Flags & knownMask) const;

    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

  // TODO : check that any zero argument produces zero evaluation, cf. JAGS Multiply class
  class Multiply: public VariableScalarFunction<std::multiplies<Scalar> >
  {
  public:
    typedef Multiply SelfType;
    typedef VariableScalarFunction<std::multiplies<Scalar> > BaseType;

  protected:
    Multiply() :
      BaseType("*")
    {
    }

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }
    virtual Bool IsDiscreteValued(const Flags & mask) const;

    virtual Bool IsInfix() const
    {
      return true;
    }
    virtual Bool IsScale(const Flags & scaleMask, const Flags & knownMask) const;

    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

  class Divide: public BinaryScalarFunction<std::divides<Scalar> >
  {
  public:
    typedef Divide SelfType;
    typedef BinaryScalarFunction<std::divides<Scalar> > BaseType;

  protected:
    Divide() :
      BaseType("/")
    {
    }


  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const;
    virtual Bool IsInfix() const
    {
      return true;
    }
    Bool IsScale(const Flags & scaleMask, const Flags & knownMask) const;

    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

  class Neg: public UnaryScalarFunction<std::negate<Scalar> >
  {
  public:
    typedef Neg SelfType;
    typedef UnaryScalarFunction<std::negate<Scalar> > BaseType;

  protected:
    Neg() :
      BaseType("NEG")
    {
    }


  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }
    virtual Bool IsPrefix() const
    {
      return true;
    }
    virtual Bool IsDiscreteValued(const Flags & mask) const;
    virtual Bool IsScale(const Flags & scaleMask, const Flags & knownMask) const;

    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };
}

#endif /* BIIPS_ARITHMETICOPERATORS_HPP_ */
