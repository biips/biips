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
