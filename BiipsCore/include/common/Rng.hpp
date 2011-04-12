//                                               -*- C++ -*-
/*! \file Rng.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_RNG_HPP_
#define BIIPS_RNG_HPP_

#include <boost/random/mersenne_twister.hpp>
#include <rng.hh>

namespace Biips
{
  
  class Rng : public smc::rng
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
