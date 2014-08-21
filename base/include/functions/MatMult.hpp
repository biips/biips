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

/*! \file MatMult.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_MATMULT_HPP_
#define BIIPS_MATMULT_HPP_

#include "function/Function.hpp"

namespace Biips
{

  class MatMult: public Function
  {
  protected:
    typedef MatMult SelfType;
    typedef Function BaseType;

    MatMult() :
      BaseType("%*%", 2)
    {
    }

    virtual Bool
        checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual void
    eval(ValArray & values, const NumArray::Array & paramValues) const;

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }
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

}

#endif /* BIIPS_MATMULT_HPP_ */
