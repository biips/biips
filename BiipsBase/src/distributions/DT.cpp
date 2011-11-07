//                                               -*- C++ -*-
/*! \file DT.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "distributions/DT.hpp"
#include <boost/math/special_functions/gamma.hpp>


namespace Biips
{

  TDistType::TDistType(Scalar mu, Scalar tau, Scalar k)
    : mu_(mu), sqrtTau_(std::sqrt(tau)), k_(k), mathDist_(k), randomDist_(k)
  {
  }

  Scalar TDistType::pdf(Scalar t) const
  {
    Scalar y = (t-mu_) * sqrtTau_;
    return sqrtTau_ * boost::math::pdf(mathDist_, y);
  }

  Scalar TDistType::logpdf(Scalar t) const
  {
    Scalar y = (t-mu_) * sqrtTau_;
    using std::log;
    using std::pow;
    using boost::math::tgamma;
    return log(tgamma(0.5*(k_+1.0))) - log(tgamma(0.5*k_))
        + log(sqrtTau_) - 0.5*log(k_*M_PI)
    - 0.5*(k_+1.0) * log(1.0 + pow(y, 2)/k_);
  }

  Scalar TDistType::cdf(Scalar t) const
  {
    Scalar y = (t-mu_) * sqrtTau_;
    return boost::math::cdf(mathDist_, y);

  }

  Scalar TDistType::quantile(Scalar p) const
  {
    return mu_ + boost::math::quantile(mathDist_, p) / sqrtTau_;
  }


  Bool DT::checkParamValues(const MultiArray::Array & paramValues) const
  {
    Scalar tau = paramValues[1].ScalarView();
    Scalar k = paramValues[2].ScalarView();
    return (tau > 0.0) && (k > 0.0);
  }


  DT::MathDistType DT::mathDist(const MultiArray::Array & paramValues) const
  {
    Scalar mu = paramValues[0].ScalarView();
    Scalar tau = paramValues[1].ScalarView();
    Scalar k = paramValues[2].ScalarView();
    return MathDistType(mu, tau, k);
  }


  DT::RandomDistType DT::randomDist(const MultiArray::Array & paramValues) const
  {
    Scalar mu = paramValues[0].ScalarView();
    Scalar tau = paramValues[1].ScalarView();
    Scalar k = paramValues[2].ScalarView();
    return RandomDistType(mu, tau, k);
  }


  Scalar DT::d(Scalar x, const MultiArray::Array & paramValues,
      Bool give_log) const
  {
    MathDistType dist = mathDist(paramValues);

    if (give_log)
      return dist.logpdf(x);
    else
      return dist.pdf(x);
  }
}
