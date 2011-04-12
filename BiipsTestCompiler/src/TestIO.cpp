//                                               -*- C++ -*-
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
    while ( ans != yes && ans != no )
    {
      cout << message;
      is >> ans;
      is.ignore();
    }
    return (ans == yes);
  }


  void progressBar(Scalar progress, Size & currentPos, time_t & timer, std::ostream & os, String finalValue, Size length)
  {
    using namespace std;
    if (progress == 0.0)
    {
      os << String(length, '-') << "|" << finalValue << endl;
      currentPos = 0;
      time(&timer);
    }
    else
    {
      Size new_pos = floor(progress * length);
      if (new_pos > currentPos)
        os << String(new_pos - currentPos, '*');
      currentPos = new_pos;
      if (progress == 1.0)
      {
        time_t start = timer;
        os << "100% in ";
        os << time(&timer)-start << " sec." << endl;
      }
    }
    os.flush();
  }

}
