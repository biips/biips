//                                               -*- C++ -*-
/*! \file Particle.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "sampler/Particle.hpp"

namespace Biips
{

  void Particle::AddToLogWeight(Scalar increment)
  {
    if (isNan(logWeight_+increment))
    {
      if (!isFinite(logWeight_) && !isFinite(increment))
      {
        throw RuntimeError(String("Log weight and incremental log weight are incompatible: ")+print(logWeight_)+" ,  "+print(increment));
      }
      throw RuntimeError("Failure to calculate particle log weight.");
    }
    logWeight_ += increment;
    weight_ = std::exp(logWeight_);
    if (isNan(weight_))
      throw RuntimeError("Failure to calculate particle weight.");

  }
}
