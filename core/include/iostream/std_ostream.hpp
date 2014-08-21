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

/*! \file std_ostream.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_STD_OSTREAM_HPP_
#define BIIPS_STD_OSTREAM_HPP_

#include <vector>
#include <sstream>

namespace std
{

  /*!
   * Prints the elements of a std::vector.
   */
  template<typename T>
  std::ostream & operator << (std::ostream & os, const std::vector<T> & vec)
  {
    if (vec.size()>0) {
      os << vec[0];
      for (unsigned int i=1; i<vec.size(); ++i) {
        os << ", " << vec[i];
      }
    }
    return os;
  }

}

#endif /* BIIPS_STD_OSTREAM_HPP_ */
