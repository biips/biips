//                                               -*- C++ -*-
/*! \file Particle.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 *
 * COPY: Nearly copied and pasted from SMCTC smc::particle class
 */

#ifndef BIIPS_PARTICLE_HPP_
#define BIIPS_PARTICLE_HPP_

#include "graph/GraphTypes.hpp"

namespace Biips
{

  class Particle
  {
  private:
    typedef NodeValues Space;
    /// Value of this particle
    Space value_;
    /// Natural logarithm of this particle's weight.
    Scalar logWeight_;
    /// this particle's weight.
    Scalar weight_;

  public:
    Particle() : logWeight_(std::numeric_limits<Scalar>::quiet_NaN()), weight_(std::numeric_limits<Scalar>::quiet_NaN()) {};
    /// Constructor which initializes the particles value and weight.
    Particle(Space init, Scalar logWeight) : value_(init), logWeight_(logWeight), weight_(exp(logWeight)) {};

    /// Returns the particle's value
    const Space & GetValue() const { return value_; }
    /// Returns a particle's value reference
    Space & Value() { return value_; }

    /// Returns the particle's log weight.
    Scalar GetLogWeight() const { return logWeight_; }
    /// Returns the particle's unnormalized weight.
    Scalar GetWeight() const { return weight_; }

    /// \param dLogWeight The natural logarithm of the new particle weight
    void SetLogWeight(Scalar logWeight) { logWeight_ = logWeight; weight_ = exp(logWeight_); }
//    /// \brief Sets the particles weight explicitly
//    ///
//    /// \param dWeight The new particle weight
//    void SetWeight(Scalar weight) { logWeight_ = log(weight); weight_ = weight; }

    /// \brief Increase the log weight by a specified amount
    ///
    /// \param dIncrement The amount to add to the log weight.
    void AddToLogWeight(Scalar increment) { logWeight_ += increment; weight_ = exp(logWeight_); }
//    /// \brief Multiply the weight by a specified factor
//    ///
//    /// \param dMultiplier The factor to multiply the weight by.
//    void MultiplyWeightBy(Scalar multiplier) { logWeight_ += log(multiplier); weight_ = exp(logWeight_); }
  };

}

#endif /* BIIPS_PARTICLE_HPP_ */
