#ifndef BIIPS_PARTICLE_HPP_
#define BIIPS_PARTICLE_HPP_

#include "graph/GraphTypes.hpp"

namespace Biips
{

  class Particle
  {
  private:
    typedef NodeValues Space;

    Space value_;
    Scalar logWeight_;
    Scalar weight_;

  public:
    Particle() : logWeight_(std::numeric_limits<Scalar>::quiet_NaN()), weight_(std::numeric_limits<Scalar>::quiet_NaN()) {};
    Particle(Space init, Scalar logWeight) : value_(init), logWeight_(logWeight), weight_(std::exp(logWeight)) {};

    const Space & GetValue() const { return value_; }
    Space & Value() { return value_; }

    Scalar LogWeight() const { return logWeight_; }
    Scalar Weight() const { return weight_; }

    void ResetWeight() { logWeight_ = 0.0; weight_ = 1.0; }
    void AddToLogWeight(Scalar increment);
  };

}

#endif /* BIIPS_PARTICLE_HPP_ */
