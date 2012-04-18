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

  Bool yesNoQuestion(const String & message, istream & is, char yes, char no)
  {
    char ans = 'u';
    while (ans != yes && ans != no)
    {
      cout << message;
      is >> ans;
      is.ignore();
    }
    return (ans == yes);
  }

//  void progressBar(Scalar progress, Size & currentPos, time_t & timer, std::ostream & os, String finalValue, Size length)
//  {
//    using namespace std;
//    if (progress == 0.0)
//    {
//      os << String(length, '-') << "|" << finalValue << endl;
//      currentPos = 0;
//      time(&timer);
//    }
//    else
//    {
//      Size new_pos = floor(progress * length);
//      if (new_pos > currentPos)
//        os << String(new_pos - currentPos, '*');
//      currentPos = new_pos;
//      if (progress == 1.0)
//      {
//        time_t start = timer;
//        os << "100% in ";
//        os << time(&timer)-start << " sec." << endl;
//      }
//    }
//    os.flush();
//  }

}
