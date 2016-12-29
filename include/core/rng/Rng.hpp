#ifndef BIIPS_RNG_HPP_
#define BIIPS_RNG_HPP_

#include <boost/random/mersenne_twister.hpp>

namespace Biips
{
  
  class Rng
  {
  public:
    typedef Rng SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    typedef boost::mt19937 GenType;
    typedef GenType::result_type ResultType;

  protected:
    GenType gen_;

  public:
    void Seed() { gen_.seed(); }
    void Seed(ResultType value) { gen_.seed(value); }
    GenType & GetGen() { return gen_; }

    Rng() {}
    explicit Rng(ResultType seed) : gen_(seed) {}
    virtual ~Rng() {}
  };

}

#endif /* BIIPS_RNG_HPP_ */
