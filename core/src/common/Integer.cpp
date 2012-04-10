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

/*! \file Integer.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "common/Integer.hpp"

#include <boost/math/special_functions/round.hpp>
#include <boost/numeric/conversion/converter.hpp>
#include <cmath>

namespace Biips
{

  Bool checkInteger(Scalar s, Scalar * pIntPart)
  {
    Scalar int_part;
    if (!pIntPart)
      pIntPart = &int_part;
    return std::modf(s, pIntPart) <= std::numeric_limits<Scalar>::round_error();
  }


  Int roundInteger(Scalar s)
  {
    return boost::math::round(s);
  }


  Bool checkSize(Scalar s, Scalar * pIntPart)
  {
    return (s > 0.0) && checkInteger(s, pIntPart);
  }

  typedef boost::numeric::converter<Size, Scalar,
    boost::numeric::conversion_traits<Size, Scalar>,
    boost::numeric::def_overflow_handler,
    boost::numeric::RoundEven<boost::numeric::conversion_traits<Size, Scalar>::source_type>
    > Scalar2SizeRounder;

  Size roundSize(Scalar s)
  {
    return Scalar2SizeRounder::convert(s);
  }
}
