//                                               -*- C++ -*-
/*! \file DChisqr.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_DCHISQR_HPP_
#define BIIPS_DCHISQR_HPP_

#include "distributions/BoostScalarDistribution.hpp"
#include <boost/random/chi_squared_distribution.hpp>
#include <boost/math/distributions/chi_squared.hpp>

namespace Biips
{

  typedef boost::math::chi_squared_distribution<Scalar> ChisqrMathDistType;
  typedef boost::chi_squared_distribution<Scalar> ChisqrRandomDistType;

  class DChisqr: public BoostScalarDistribution<ChisqrMathDistType,
      ChisqrRandomDistType>
  {
  public:
    typedef DChisqr SelfType;
    typedef BoostScalarDistribution<ChisqrMathDistType, ChisqrRandomDistType>
        BaseType;

  protected:
    DChisqr() :
      BaseType("dchisqr", 1, DIST_POSITIVE, false)
    {
    }
    virtual Bool checkParamValues(const NumArray::Array & paramValues) const;

    virtual MathDistType mathDist(const NumArray::Array & paramValues) const;
    virtual RandomDistType randomDist(const NumArray::Array & paramValues) const;

  public:
    virtual String Alias() const
    {
      return "dchisq";
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

#endif /* BIIPS_DCHISQR_HPP_ */
