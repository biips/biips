//                                               -*- C++ -*-
/*! \file DMNorm.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DMNORM_HPP_
#define BIIPS_DMNORM_HPP_

#include "distribution/Distribution.hpp"

namespace Biips
{
  class DMNorm : public Distribution
  {
  protected:
    typedef DMNorm SelfType;

    DMNorm() : Distribution("dmnorm", 2) {};

    virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;

  public:
    virtual MultiArray Sample(const MultiArray::Array & paramValues, Rng * pRng) const;
    virtual Scalar LogUnnormPdf(const MultiArray & x, const MultiArray::Array & paramValues) const;

    static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; };
  };

} /* namespace Biips */

#endif /* BIIPS_DMNORM_HPP_ */
