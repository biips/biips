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

/*! \file MatlabFunction.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_MATLABFUNCTION_HPP_
#define BIIPS_MATLABFUNCTION_HPP_

#include "function/Function.hpp"
namespace Biips
{

  class MatlabFunction: public Function
  {
  protected:
    String fun_dim_;
    String fun_eval_;
    String fun_check_param_;
    String fun_is_discrete_;

    virtual Bool checkParamDims(
        const Types<DimArray::Ptr>::Array & paramDims) const
    {
      return true;
    }
    virtual DimArray

    dim(const Types<DimArray::Ptr>::Array & paramDims) const;

    virtual void eval(ValArray & values, const NumArray::Array & paramValues) const;

  public:
    typedef MatlabFunction SelfType;
    typedef Function BaseType;

    MatlabFunction(const String & name, Size nParam, const String & fun_dim,
                   const String & fun_eval, const String & fun_check_param = "",
                   const String & fun_is_discrete = "")
        : BaseType(name, nParam), fun_dim_(fun_dim), fun_eval_(fun_eval),
            fun_check_param_(fun_check_param), fun_is_discrete_(fun_is_discrete)
    {
      // TODO: check name is valid
      // TODO: check functions exist in matlab
      // TODO: check functions nb of arguments
    }

    virtual Bool IsDiscreteValued(const Flags & mask) const;

    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const;

    virtual ~MatlabFunction()
    {
    }

  };

}

#endif /* BIIPS_MATLABFUNCTION_HPP_ */

