//                                               -*- C++ -*-
/*! \file Counter.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 *
 * COPY: Nearly copied and pasted from JAGS Counter class
 */

#include "common/Counter.hpp"
#include "common/Error.hpp"

namespace Biips
{

  Counter::Counter(const IndexRange & range)
    : BaseType(range)
  {
    if (range.NDim(false) != 1)
      throw LogicError("Attempt to construct Counter from non-scalar Range");
  }
}
