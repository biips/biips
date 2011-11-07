//                                               -*- C++ -*-
/*! \file DWeib.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_DWEIB_HPP_
#define BIIPS_DWEIB_HPP_

#include "distributions/BoostScalarDistribution.hpp"
#include <boost/random/weibull_distribution.hpp>
#include <boost/math/distributions/weibull.hpp>

namespace Biips
{

  typedef boost::math::weibull_distribution<Scalar> WeibMathDistType;
  typedef boost::weibull_distribution<Scalar> WeibRandomDistType;

  class DWeib : public BoostScalarDistribution<WeibMathDistType, WeibRandomDistType>
  {
  public:
    typedef DWeib SelfType;
    typedef BoostScalarDistribution<WeibMathDistType, WeibRandomDistType> BaseType;

  protected:
    DWeib() : BaseType("dweib", 2, DIST_POSITIVE, false) {}
    virtual Bool checkParamValues(const MultiArray::Array & paramValues) const;

    virtual MathDistType mathDist(const MultiArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const MultiArray::Array & paramValues) const;

  public:
    virtual String Alias() const { return "dweibull"; }
    virtual Scalar d(Scalar x, const MultiArray::Array & paramValues,
        Bool give_log) const;
    static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; }
  };

}

#endif /* DWEIB_HPP_ */
