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

#include "distribution/Distribution.hpp"

namespace Biips
{
  class DNormVar : public Distribution
  {
  protected:
    typedef DNormVar SelfType;

    DNormVar() : Distribution("dnorm.var", 2) {};

  public:
    virtual DataType Sample(const DataType::Array & paramValues, Rng * pRng) const;
    virtual Scalar LogUnnormPdf(const DataType & x, const DataType::Array & paramValues) const;

    static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; };
  };
}

#endif /* BIIPS_DNORMVAR_HPP_ */
