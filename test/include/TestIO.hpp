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

/*! \file TestIO.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_TESTIO_HPP_
#define BIIPS_TESTIO_HPP_

#include <iostream>
#include <sstream>

#include "common/Common.hpp"


namespace std
{
  template<class T>
  ostream& operator<<(ostream& os, const vector<T>& v)
  {
    copy(v.begin(), v.end(), ostream_iterator<T>(os, " "));
    return os;
  }
}


namespace Biips
{

  void pressEnterToContinue(std::istream & is = std::cin);


  Bool yesNoQuestion(const String & message, char yes = 'y', char no = 'n', Size nb_newlines = 1, std::istream & is = std::cin);


  template<typename T>
  String toString( const T & value )
  {
    using namespace std;

    ostringstream oss;
    oss << value;
    return oss.str();
  }

}

#endif /* BIIPS_TESTIO_HPP_ */
