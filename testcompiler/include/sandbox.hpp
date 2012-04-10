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

/*! \file sandbox.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_SANDBOX_HPP_
#define BIIPS_SANDBOX_HPP_

#include <iostream>
#include "common/Types.hpp"

void sandbox()
{
  using namespace std;
  using namespace Biips;

  cout << "======== sandbox ========" << endl;
  cout << "BIIPS_SIZEMIN = " << BIIPS_SIZEMIN << endl;
  cout << "BIIPS_SIZEMAX = " << BIIPS_SIZEMAX << endl;
  cout << "BIIPS_SIZENA = " << BIIPS_SIZENA << endl;
  cout << endl;

  cout << "BIIPS_INTMIN = " << BIIPS_INTMIN << endl;
  cout << "BIIPS_INTMAX = " << BIIPS_INTMAX << endl;
  cout << endl;

  cout << "BIIPS_REALMIN = " << BIIPS_REALMIN << endl;
  cout << "BIIPS_REALMAX = " << BIIPS_REALMAX << endl;
  cout << "BIIPS_NEGINF = " << BIIPS_NEGINF << endl;
  cout << "BIIPS_POSINF = " << BIIPS_POSINF << endl;
  cout << "BIIPS_REALNAN = " << BIIPS_REALNAN << endl;
  cout << "BIIPS_NA = " << BIIPS_REALNA << endl;
  cout << "Real has infinity ? " << std::numeric_limits<Real>::has_infinity << endl;
  cout << "BIIPS_NEGINF is infinity ? " << !isFinite(BIIPS_NEGINF) << endl;
  cout << "BIIPS_POSINF is infinity ? " << !isFinite(BIIPS_POSINF) << endl;
  cout << "Real has NaN ? " << std::numeric_limits<Real>::has_quiet_NaN << endl;
  cout << "BIIPS_REALNAN is NaN ? " << isNan(BIIPS_REALNAN) << endl;
  cout << "BIIPS_REALNA != BIIPS_REALMIN ? " << (BIIPS_REALNA != BIIPS_REALMIN) << endl;
  cout << "BIIPS_REALNA != BIIPS_REALMAX ? " << (BIIPS_REALNA != BIIPS_REALMAX) << endl;
  cout << "BIIPS_REALNA != BIIPS_NEGINF ? " << (BIIPS_REALNA != BIIPS_NEGINF) << endl;
  cout << "BIIPS_REALNA != BIIPS_POSINF ? " << (BIIPS_REALNA != BIIPS_POSINF) << endl;
  cout << "BIIPS_REALNA != BIIPS_REALNAN ? " << (BIIPS_REALNA != BIIPS_REALNAN) << endl;
  cout << "=========================" << endl;
  cout << endl;

}

#endif /* BIIPS_SANDBOX_HPP_ */
