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

/*! \file kolmogorov.hpp
* \brief
*
* \author  $LastChangedBy$
* \date    $LastChangedDate$
* \version $LastChangedRevision$
* Id:      $Id$
*
*/

#ifndef BIIPS_KOLMOGOROV_HPP_
#define BIIPS_KOLMOGOROV_HPP_

#include "common/Types.hpp"

namespace Biips
{

  template<typename InputIterator>
  Scalar ksTwoSamplesStat(InputIterator firstX, InputIterator lastX, InputIterator firstY, InputIterator lastY)
  {
    Size n_x = std::distance(firstX, lastX);
    Size n_y = std::distance(firstY, lastY);
    Size n = n_x+n_y;

    std::sort(firstX, lastX);
    std::sort(firstY, lastY);

    Scalar cumul_x = 0;
    Scalar cumul = 0;
    Scalar max = 0.0;
    Scalar val;

    while(firstX != lastX)
    {
      while(firstY != lastY)
      {
        if(*firstX < *firstY)
        {
          cumul = cumul+1;
          cumul_x = cumul_x+1;

          val = std::fabs(cumul_x/n_x - cumul/n);
          if (val>max)
            max = val;

          break;
        }

        cumul = cumul+1;

        val = std::fabs(cumul_x/n_x - cumul/n);
        if (val>max)
          max = val;

        ++firstY;
      }

      if(firstY == lastY)
      {
        cumul = cumul+1;
        cumul_x = cumul_x+1;

        val = std::fabs(cumul_x/n_x - cumul/n);
        if (val>max)
          max = val;
      }

      ++firstX;
    }
    while(firstY != lastY)
    {
      cumul = cumul+1;

      val = std::fabs(1 - cumul/n);
      if (val>max)
        max = val;

      ++firstY;
    }

    return Scalar(n)/n_y * max;
  }

  Scalar ksProb(Scalar z);
}
#endif /* BIIPS_KOLMOGOROV_HPP_ */
