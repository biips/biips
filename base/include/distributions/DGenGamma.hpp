//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file DGenGamma.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DGENGAMMA_HPP_
#define BIIPS_DGENGAMMA_HPP_

#include "distributions/BoostScalarDistribution.hpp"
#include "boost/random/gamma_distribution_2par.hpp"
#include <boost/math/distributions/gamma.hpp>

namespace Biips
{

  class GenGammaDistType
  {
  public:
    typedef Scalar input_type;
    typedef Scalar result_type;

    typedef boost::math::gamma_distribution<result_type> MathDistType;
    typedef boost::gamma_distribution<result_type> RandomDistType;

  protected:
    Scalar shape_;
    Scalar scale_;
    Scalar pow_;
    MathDistType mathDist_;
    RandomDistType randomDist_;

    Scalar transform(Scalar x) const
    {
      return std::exp(pow_ * (std::log(x) - std::log(scale_)));
    }

    Scalar untransform(Scalar x) const
    {
      return std::exp(std::log(x) / pow_ + std::log(scale_));
    }
  public:
    GenGammaDistType(Scalar shape, Scalar rate, Scalar pow);

    Scalar shape() const
    {
      return shape_;
    }
    Scalar scale() const
    {
      return scale_;
    }

    Scalar pdf(Scalar t) const;

    Scalar log_pdf(Scalar t) const;

    Scalar cdf(Scalar t) const
    {
      return boost::math::cdf(mathDist_, transform(t));
    }
    Scalar quantile(Scalar t) const
    {
      return untransform(boost::math::quantile(mathDist_, t));
    }

    Scalar cdf_cmplt(Scalar t) const
    {
      return boost::math::cdf(boost::math::complement(mathDist_, t));
    }

    Scalar quantile_cmplt(Scalar t) const
    {
      return boost::math::quantile(boost::math::complement(mathDist_, t));
    }

    template<class Engine>
    Scalar operator()(Engine& eng)
    {
      return untransform(randomDist_(eng));
    }
  };

}

namespace boost
{
  namespace math
  {

    template<class RealType>
    inline RealType pdf(const Biips::GenGammaDistType& dist, const RealType& t)
    {
      return dist.pdf(t);
    }

    template<class RealType>
    inline RealType log_pdf(const Biips::GenGammaDistType& dist,
                            const RealType& t)
    {
      return dist.log_pdf(t);
    }

    template<class RealType>
    inline RealType cdf(const Biips::GenGammaDistType& dist, const RealType& t)
    {
      return dist.cdf(t);
    }

    template<class RealType>
    inline RealType quantile(const Biips::GenGammaDistType& dist,
                             const RealType& p)
    {
      return dist.quantile(p);
    }

    template<class RealType>
    inline RealType cdf(const complemented2_type<Biips::GenGammaDistType,
        RealType>& c)
    {
      return c.dist.cdf_cmplt(c.param);
    }

    template<class RealType>
    inline RealType quantile(const complemented2_type<Biips::GenGammaDistType,
        RealType>& c)
    {
      return c.dist.quantile_cmplt(c.param);
    }
  }
}

namespace Biips
{

  class DGenGamma: public BoostScalarDistribution<GenGammaDistType,
      GenGammaDistType>
  {
  public:
    typedef DGenGamma SelfType;
    typedef BoostScalarDistribution<GenGammaDistType, GenGammaDistType>
        BaseType;

  protected:
    DGenGamma() :
      BaseType("dgen.gamma", 3, DIST_POSITIVE, false)
    {
    }
    virtual Bool checkParamValues(const NumArray::Array & paramValues) const;

    virtual MathDistType mathDist(const NumArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const NumArray::Array & paramValues) const;

  public:
    virtual String Alias() const
    {
      return "gengamma";
    }
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

#endif /* BIIPS_DGENGAMMA_HPP_ */
