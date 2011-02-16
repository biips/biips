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
    }
    for (Size i=0; i<nb_newlines; ++i)
      cout << endl;
    return (ans == yes);
  }


  Size progressBar(Scalar progress, Size currentPos, std::ostream & os, String finalValue, Size length)
  {
    using namespace std;
    if (progress == 0.0)
    {
      os << String(length, '-') << "|" << finalValue << endl;
      currentPos = 0;
    }
    else
    {
      Size new_pos = floor(progress * length);
      if (new_pos > currentPos)
        os << String(new_pos - currentPos, '*');
      currentPos = new_pos;
      if (progress == 1.0)
        os << "100%" << endl;
    }
    os.flush();

    return currentPos;
  }

}
