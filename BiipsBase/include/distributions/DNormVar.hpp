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

    DNormVar() : Distribution("dnormvar", 2) {};

    virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;

  public:
    virtual MultiArray Sample(const MultiArray::Array & paramValues, Rng * pRng) const;
    virtual Scalar LogPdf(const MultiArray & x, const MultiArray::Array & paramValues) const;

    static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; };
  };
}

#endif /* BIIPS_DNORMVAR_HPP_ */
