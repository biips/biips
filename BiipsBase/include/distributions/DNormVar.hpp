//                                               -*- C++ -*-
/*! \file DNormVar.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_DNORMVAR_HPP_
#define BIIPS_DNORMVAR_HPP_

#include "distributions/BoostScalarDistribution.hpp"
#include <boost/random/normal_distribution.hpp>
#include <boost/math/distributions/normal.hpp>

namespace Biips
{
  typedef boost::math::normal_distribution<Scalar> NormalMathDistType;
  typedef boost::normal_distribution<Scalar> NormalRandomDistType;


  class DNormVar : public BoostScalarDistribution<NormalMathDistType, NormalRandomDistType>
  {
  public:
    typedef DNormVar SelfType;
    typedef BoostScalarDistribution<NormalMathDistType, NormalRandomDistType> BaseType;

  protected:
    DNormVar() : BaseType("dnormvar", 2, DIST_UNBOUNDED, false) {}
    virtual Bool checkParamValues(const MultiArray::Array & paramValues) const;
    virtual MultiArray sample(const MultiArray::Array & paramValues, const MultiArray::Pair & boundValues, Rng & rng) const;

    virtual MathDistType mathDist(const MultiArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const MultiArray::Array & paramValues) const;

  public:
    virtual Scalar d(Scalar x, const MultiArray::Array & paramValues,
        Bool give_log) const;
    static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; }
  };

}

#endif /* BIIPS_DNORMVAR_HPP_ */
