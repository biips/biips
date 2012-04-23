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

/*! \file Utility.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_UTILITY_HPP_
#define BIIPS_UTILITY_HPP_

#include <utility>
#include <sstream>

#include "Types.hpp"

namespace Biips
{

  template <typename Container>
  typename Types<typename Container::iterator>::Pair iterRange(Container & cont)
  {
    return std::make_pair(cont.begin(), cont.end());
  }


  template <typename Container>
  typename Types<typename Container::const_iterator>::Pair iterRange(const Container & cont)
  {
    return std::make_pair(cont.begin(), cont.end());
  }


  template<typename T>
  String print(const T & val)
  {
    std::ostringstream ostr;
    ostr << val;
    return ostr.str();
  }


  inline Bool allTrue(const Flags & mask)
  {
    return std::find(mask.begin(), mask.end(), false) == mask.end();
  }


  inline Bool anyTrue(const Flags & mask)
  {
    return std::find(mask.begin(), mask.end(), true) != mask.end();
  }


  String expandFileName(const char * s);

}

#endif /* BIIPS_UTILITY_HPP_ */
