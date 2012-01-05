//                                               -*- C++ -*-
/*! \file DBin.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DBIN_HPP_
#define BIIPS_DBIN_HPP_

#include "distributions/BoostScalarDistribution.hpp"
#include <boost/random/binomial_distribution.hpp>
#include <boost/math/distributions/binomial.hpp>

namespace Biips
{

  typedef boost::math::binomial_distribution<Scalar> BinMathDistType;
  typedef boost::binomial_distribution<Scalar> BinRandomDistType;

  class DBin: public BoostScalarDistribution<BinMathDistType, BinRandomDistType>
  {
  public:
    typedef DBin SelfType;
    typedef BoostScalarDistribution<BinMathDistType, BinRandomDistType>
        BaseType;

  protected:
    DBin() :
      BaseType("dbin", 2, DIST_SPECIAL, true)
    {
    }
    virtual Bool checkParamValues(const NumArray::Array & paramValues) const;
    virtual Scalar unboundedLower(const NumArray::Array & paramValues) const;
    virtual Scalar unboundedUpper(const NumArray::Array & paramValues) const;

    virtual MathDistType mathDist(const NumArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const NumArray::Array & paramValues) const;

  public:
    virtual String Alias() const
    {
      return "dbinom";
    }
    virtual Bool IsSupportFixed(const Flags & fixmask) const;
    virtual Scalar d(Scalar x,
                     const NumArray::Array & paramValues,
                     Bool give_log) const;
    static Distribution::Ptr Instance()
    {
      static Distribution::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

}

#endif /* BIIPS_DBIN_HPP_ */
