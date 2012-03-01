//                                               -*- C++ -*-
/*! \file Integer.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_INTEGER_HPP_
#define BIIPS_INTEGER_HPP_

#include "common/Types.hpp"

namespace Biips
{

  Bool checkInteger(Scalar s, Scalar * pIntPart = NULL);

  Int roundInteger(Scalar s);

  Bool checkSize(Scalar s, Scalar * pIntPart = NULL);

  Size roundSize(Scalar s);
}

#endif /* INTEGER_HPP_ */
