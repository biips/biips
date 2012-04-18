//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
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

/*! \file outStream.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_OUTSTREAM_HPP_
#define BIIPS_OUTSTREAM_HPP_

#include "common/Types.hpp"
#include <sstream>

namespace Biips
{

  class ValArray;

  /*!
   * Prints the elements of the ValArray in line and comma separated
   */
  std::ostream & operator << (std::ostream & os, const ValArray & val);


  class MultiArray;

  /*!
   * Prints the elements of the MultiArray according to its dimensions.
   */
  std::ostream & operator << (std::ostream & os, const MultiArray & dat);


  class IndexRange;

  /*!
   * Prints a BUGS language representation of the
   * given range: e.g. a range with lower limit (1,2,3) and upper limit
   * (3,3,3) will be printed as "[1:3,2:3,3]"
   */
  std::ostream & operator << (std::ostream & os, const IndexRange & range);

}

#endif /* BIIPS_OUTSTREAM_HPP_ */
