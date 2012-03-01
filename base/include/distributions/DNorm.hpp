//                                               -*- C++ -*-
/*! \file DNorm.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DNORM_HPP_
#define BIIPS_DNORM_HPP_

#include "distributions/BoostScalarDistribution.hpp"
#include <boost/random/normal_distribution.hpp>
#include <boost/math/distributions/normal.hpp>

namespace Biips
{
  typedef boost::math::normal_distribution<Scalar> NormalMathDistType;
  typedef boost::normal_distribution<Scalar> NormalRandomDistType;

  class DNorm: public BoostScalarDistribution<NormalMathDistType,
      NormalRandomDistType>
  {
  public:
    typedef DNorm SelfType;
    typedef BoostScalarDistribution<NormalMathDistType, NormalRandomDistType>
        BaseType;

  protected:
    DNorm() :
      BaseType("dnorm", 2, DIST_UNBOUNDED, false)
    {
    }
    virtual Bool checkParamValues(const NumArray::Array & paramValues) const;
    virtual void sample(ValArray & values,
                        const NumArray::Array & paramValues,
                        const NumArray::Pair & boundValues,
                        Rng & rng) const;

    virtual MathDistType mathDist(const NumArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const NumArray::Array & paramValues) const;

  public:
    virtual Scalar d(Scalar x,
                     const NumArray::Array & paramValues,
                     Bool give_log) const;
    static Distribution::Ptr Instance()
    {
      static Distribution::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

} /* namespace Biips */

#endif /* BIIPS_DNORM_HPP_ */
