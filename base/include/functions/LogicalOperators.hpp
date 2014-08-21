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

/*! \file LogicalOperators.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_LOGICALOPERATORS_HPP_
#define BIIPS_LOGICALOPERATORS_HPP_

#include "functions/ScalarFunction.hpp"

namespace Biips
{

  class And: public BinaryScalarFunction<std::logical_and<Scalar> >
  {
  public:
    typedef And SelfType;
    typedef BinaryScalarFunction<std::logical_and<Scalar> > BaseType;

  protected:
    And() :
      BaseType("&&")
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

  class Or: public BinaryScalarFunction<std::logical_or<Scalar> >
  {
  public:
    typedef Or SelfType;
    typedef BinaryScalarFunction<std::logical_or<Scalar> > BaseType;

  protected:
    Or() :
      BaseType("||")
    {
    }

    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const
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

  class Not: public UnaryScalarFunction<std::logical_not<Scalar> >
  {
  public:
    typedef Not SelfType;
    typedef UnaryScalarFunction<std::logical_not<Scalar> > BaseType;

  protected:
    Not() :
      BaseType("!")
    {
    }

    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }

  public:
    virtual Bool IsPrefix() const
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

#endif /* BIIPS_LOGICALOPERATORS_HPP_ */
