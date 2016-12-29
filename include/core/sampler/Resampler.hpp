#ifndef BIIPS_RESAMPLER_HPP_
#define BIIPS_RESAMPLER_HPP_

#include "Particle.hpp"
#include "common/Table.hpp"

namespace Biips
{

  class Resampler
  {
  public:
    typedef Resampler SelfType;
    typedef Types<Resampler>::Ptr Ptr;

  protected:
    const String name_;
    Size nParticles_;
    ValArray weights_;
    Types<Size>::Array count_;
    Types<Size>::Array indices_;

    virtual void resample(Scalar sumOfWeights, Rng & rng) = 0;

  public:
    Resampler(const String & name) :
      name_(name), nParticles_(0)
    {
    }

    String const & Name() const
    {
      return name_;
    }
    virtual String Alias() const
    {
      return "";
    }

    void Resample(Types<Particle>::Array & particles,
                  Scalar & sumOfWeights,
                  Rng & rng);

    virtual ~Resampler()
    {
    }
  };

  class ResamplerTable: public Table<Resampler>
  {
  public:
    ResamplerTable();
  };
}

#endif /* BIIPS_RESAMPLER_HPP_ */
