//                                               -*- C++ -*-
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


  void progressBar(Scalar progress, Size & currentPos, time_t & timer, std::ostream & os = std::cout, String finalValue = "", Size length = 50);
}

#endif /* BIIPS_TESTIO_HPP_ */
