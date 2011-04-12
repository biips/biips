//                                               -*- C++ -*-
/*! \file DCat.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_DCAT_HPP_
#define BIIPS_DCAT_HPP_

#include "distribution/Distribution.hpp"

namespace Biips
{

  class DCat : public Distribution
  {
  protected:
    typedef DCat SelfType;

    DCat() : Distribution("dcat", 1) {};

    virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;

  public:
    virtual MultiArray Sample(const MultiArray::Array & paramValues, Rng * pRng) const;
    virtual Scalar LogUnnormPdf(const MultiArray & x, const MultiArray::Array & paramValues) const;

    static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_BIIPS_DCAT_H_ */
