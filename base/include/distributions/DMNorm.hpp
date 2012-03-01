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
  class DMNorm: public Distribution
  {
  protected:
    typedef DMNorm SelfType;

    DMNorm() :
      Distribution("dmnorm", 2)
    {
    }

    virtual Bool
    checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual Bool checkParamValues(const NumArray::Array & paramValues) const;
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual void sample(ValArray & values,
                        const NumArray::Array & paramValues,
                        const NumArray::Pair & boundValues,
                        Rng & rng) const;
    virtual Scalar logDensity(const NumArray & x,
                              const NumArray::Array & paramValues,
                              const NumArray::Pair & boundValues) const;
    virtual void
    unboundedSupport(ValArray & lower,
                     ValArray & upper,
                     const NumArray::Array & paramValues) const;

  public:
    virtual Bool IsSupportFixed(const Flags & fixmask) const
    {
      return true;
    }

    static Distribution::Ptr Instance()
    {
      static Distribution::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

} /* namespace Biips */

#endif /* BIIPS_DMNORM_HPP_ */
