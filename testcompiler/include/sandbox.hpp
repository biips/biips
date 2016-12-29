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
