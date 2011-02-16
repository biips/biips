//                                               -*- C++ -*-
/*! \file Exception.hpp
 * \brief
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_EXCEPTION_HPP_
#define BIIPS_EXCEPTION_HPP_

#include <exception>
#include "smc-exception.hh"

//! A macro defining the code snippet to catch biips errors
#define BIIPS_CATCH                     \
  catch(smc::exception & e)             \
  {                                     \
    std::cerr << e;                     \
  }                                     \
  catch(const Biips::Exception & e)     \
  {                                     \
    std::cerr << e.what();              \
  }                                     \
  catch(const std::exception & e)       \
  {                                     \
    std::cerr << e.what();              \
  }

namespace Biips
{

  class Exception : public std::exception
  {
  };

}

#endif /* BIIPS_EXCEPTION_HPP_ */
