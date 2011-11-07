//                                               -*- C++ -*-
/*! \file DT.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_DT_HPP_
#define BIIPS_DT_HPP_

#include "distributions/BoostScalarDistribution.hpp"
#include <boost/random/students_t_distribution.hpp>
#include <boost/math/distributions/students_t.hpp>

namespace Biips
{

  class TDistType
  {
  public:
    typedef Scalar input_type;
    typedef Scalar result_type;

    typedef boost::math::students_t_distribution<result_type> MathDistType;
    typedef boost::students_t_distribution<result_type> RandomDistType;

  protected:
    Scalar mu_;
    Scalar sqrtTau_;
    Scalar k_;
    MathDistType mathDist_;
    RandomDistType randomDist_;

  public:
    TDistType(Scalar mu, Scalar tau, Scalar k);

    Scalar pdf(Scalar t) const;
    Scalar logpdf(Scalar t) const;
    Scalar cdf(Scalar t) const;
    Scalar quantile(Scalar p) const;

    template<class Engine>
    Scalar operator() (Engine& eng)
    {
      return mu_ + randomDist_(eng) / sqrtTau_;
    }
  };

}


namespace boost{ namespace math {

  template <class RealType>
  inline RealType pdf(const Biips::TDistType& dist, const RealType& t)
  {
    return dist.pdf(t);
  }

  template <class RealType>
  inline RealType cdf(const Biips::TDistType& dist, const RealType& t)
  {
    return dist.cdf(t);
  }

  template <class RealType>
  inline RealType quantile(const Biips::TDistType& dist, const RealType& p)
  {
    return dist.quantile(p);
  }

  template <class RealType>
  inline RealType cdf(const complemented2_type<Biips::TDistType, RealType>& c)
  {
     return cdf(c.dist, -c.param);
  }

  template <class RealType>
  inline RealType quantile(const complemented2_type<Biips::TDistType, RealType>& c)
  {
     return -quantile(c.dist, c.param);
  }
}}


namespace Biips
{

  class DT : public BoostScalarDistribution<TDistType, TDistType>
  {
  public:
    typedef DT SelfType;
    typedef BoostScalarDistribution<TDistType, TDistType> BaseType;

  protected:
    DT() : BaseType("dt", 3, DIST_UNBOUNDED, false) {}
    virtual Bool checkParamValues(const MultiArray::Array & paramValues) const;

    virtual MathDistType mathDist(const MultiArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const MultiArray::Array & paramValues) const;

  public:
    virtual Scalar d(Scalar x, const MultiArray::Array & paramValues,
        Bool give_log) const;
    static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; }
  };

}


#endif /* BIIPS_DT_HPP_ */
