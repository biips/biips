//                                               -*- C++ -*-
/*! \file DMNormVar.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_DMNORMVAR_HPP_
#define BIIPS_DMNORMVAR_HPP_

#include "distribution/Distribution.hpp"

namespace Biips
{
  class DMNormVar : public Distribution
  {
  protected:
    typedef DMNormVar SelfType;

    DMNormVar() : Distribution("dmnormvar", 2) {};

    virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;

  public:
    virtual MultiArray Sample(const MultiArray::Array & paramValues, Rng * pRng) const;
    virtual Scalar LogPdf(const MultiArray & x, const MultiArray::Array & paramValues) const;

    static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; };
  };

} /* namespace Biips */


#endif /* BIIPS_DMNORMVAR_HPP_ */
