//                                               -*- C++ -*-
/*! \file DUnif.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_DUNIF_HPP_
#define BIIPS_DUNIF_HPP_

#include "distribution/Distribution.hpp"

namespace Biips
{

  class DUnif : public Distribution
  {
  protected:
    typedef DUnif SelfType;

    DUnif() : Distribution("dunif", 2) {};

    virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual Bool checkParamValues(const MultiArray::Array & paramValues) const;
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual MultiArray sample(const MultiArray::Array & paramValues, const MultiArray::Pair & boundValues, Rng & rng) const;
    virtual Scalar logDensity(const MultiArray & x, const MultiArray::Array & paramValues, const MultiArray::Pair & boundValues) const;
    virtual MultiArray::Pair unboundedSupport(const MultiArray::Array & paramValues) const;

  public:
    virtual Bool IsSupportFixed(const Flags & fixmask) const;
    static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; }
  };

}

#endif /* BIIPS_DUNIF_HPP_ */
