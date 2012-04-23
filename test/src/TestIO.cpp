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

/*! \file TestIO.cpp
* \brief
*
* \author  $LastChangedBy$
* \date    $LastChangedDate$
* \version $LastChangedRevision$
* Id:      $Id$
*/


#include "TestIO.hpp"

#include <ctime>

namespace Biips
{
  using namespace std;

  void pressEnterToContinue(istream & is)
  {
    cout << "Press 'Enter' to continue... ";
    is.ignore();
  }


  Bool yesNoQuestion(const String & message, char yes, char no, Size nb_newlines, istream & is)
  {
    char ans = 'u';
    while ( ans != yes && ans != no )
    {
      cout << message;
      is >> ans;
      is.ignore();
    }
    for (Size i=0; i<nb_newlines; ++i)
      cout << endl;
    return (ans == yes);
  }

}
