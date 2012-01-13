//                                               -*- C++ -*-
/*! \file DDexp.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_DDEXP_HPP_
#define BIIPS_DDEXP_HPP_

#include "distributions/BoostScalarDistribution.hpp"
#include <boost/random/exponential_distribution.hpp>
#include <boost/math/distributions/laplace.hpp>
#include <boost/random/uniform_real.hpp>

namespace Biips
{

  class DexpDistType
  {
  public:
    typedef Scalar input_type;
    typedef Scalar result_type;

    typedef boost::math::laplace_distribution<result_type> MathDistType;
    typedef boost::exponential_distribution<result_type> ExpRandomDistType;

  protected:
    Scalar location_;
    Scalar scale_;
    MathDistType mathDist_;
    ExpRandomDistType expRandomDist_;

  public:
    DexpDistType(Scalar mu, Scalar rate);

    Scalar pdf(Scalar t) const
    {
      return boost::math::pdf(mathDist_, t);
    }
    Scalar log_pdf(Scalar t) const
    {
      return std::log(scale_) - scale_ * std::fabs(t - location_)
          - std::log(2.0);
    }
    Scalar cdf(Scalar t) const
    {
      return boost::math::cdf(mathDist_, t);
    }
    Scalar quantile(Scalar t) const
    {
      return boost::math::quantile(mathDist_, t);
    }

    Scalar cdf_cmplt(Scalar t) const
    {
      return boost::math::cdf(boost::math::complement(mathDist_, t));
    }

    Scalar quantile_cmplt(Scalar t) const
    {
      return boost::math::quantile(boost::math::complement(mathDist_, t));
    }

    Scalar location() const
    {
      return location_;
    }
    Scalar scale() const
    {
      return scale_;
    }

    template<class Engine>
    Scalar operator()(Engine& eng)
    {
      typedef boost::uniform_real<Scalar> UniformDist;
      static UniformDist unif(0.0, 1.0);
      if (unif(eng) < 0.5)
        return location_ - expRandomDist_(eng);
      else
        return location_ + expRandomDist_(eng);
    }
  };

}

namespace boost
{
  namespace math
  {

    template<class RealType>
    inline RealType pdf(const Biips::DexpDistType& dist, const RealType& t)
    {
      return dist.pdf(t);
    }

    template<class RealType>
    inline RealType log_pdf(const Biips::DexpDistType& dist, const RealType& t)
    {
      return dist.log_pdf(t);
    }

    template<class RealType>
    inline RealType cdf(const Biips::DexpDistType& dist, const RealType& t)
    {
      return dist.cdf(t);
    }

    template<class RealType>
    inline RealType quantile(const Biips::DexpDistType& dist, const RealType& p)
    {
      return dist.quantile(p);
    }

    template<class RealType>
    inline RealType cdf(const complemented2_type<Biips::DexpDistType, RealType>& c)
    {
      return c.dist.cdf_cmplt(c.param);
    }

    template<class RealType>
    inline RealType quantile(const complemented2_type<Biips::DexpDistType,
        RealType>& c)
    {
      return c.dist.quantile_cmplt(c.param);
    }
  }
}

namespace Biips
{

  class DDexp: public BoostScalarDistribution<DexpDistType, DexpDistType>
  {
  public:
    typedef DDexp SelfType;
    typedef BoostScalarDistribution<DexpDistType, DexpDistType> BaseType;

  protected:
    DDexp() :
      BaseType("ddexp", 2, DIST_UNBOUNDED, false)
    {
    }
    virtual Bool checkParamValues(const NumArray::Array & paramValues) const;

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

}

#endif /* BIIPS_DDEXP_HPP_ */
