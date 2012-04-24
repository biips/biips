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

/*! \file Inverse.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "functions/Inverse.hpp"
#include "common/cholesky.hpp"
#include "boost/numeric/ublas/io.hpp"

namespace Biips
{

  Bool Inverse::checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & dim = *paramDims[0];

    return dim.Drop().IsSquared() || dim.IsScalar();
  }

  Bool Inverse::checkParamValues(const NumArray::Array & paramValues) const
  {
    const NumArray & m = paramValues[0];

    static const Scalar TOL = 1e-7;

    // check symmetry
    Matrix m_mat(m);
    for (Size i = 1; i < m_mat.size1(); ++i)
    {
      for (Size j = 0; j < i; ++j)
      {
        if (std::fabs(m_mat(i, j) - m_mat(j, i)) > TOL) // TODO use epsilon
          return false;
      }
    }
    return true;
  }

  void Inverse::eval(ValArray & values, const NumArray::Array & paramValues) const
  {
    const NumArray & m = paramValues[0];

    Matrix m_inv(m);
    if (!ublas::cholesky_factorize(m_inv))
    {
      std::cerr << Matrix(m) << std::endl;
      throw RuntimeError("Inverse::eval: matrix is not positive-semidefinite.");
    }
    ublas::cholesky_invert(m_inv);

    values.swap(m_inv.data());
  }

  DimArray Inverse::dim(const Types<DimArray::Ptr>::Array & paramDims) const
  {
    const DimArray & dim = *paramDims[0];

    return dim.Drop();
  }
}

