//                                               -*- C++ -*-
/*! \file DBern.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_DBERN_HPP_
#define BIIPS_DBERN_HPP_

#include "distribution/Distribution.hpp"

namespace Biips
{

  class DBern : public Distribution
  {
  protected:
    typedef DBern SelfType;

    DBern() : Distribution("dbern", 1) {};

    virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual Bool checkParamValues(const MultiArray::Array & paramValues) const;
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual MultiArray sample(const MultiArray::Array & paramValues, const MultiArray::Pair & boundValues, Rng & rng) const;
    virtual Scalar logDensity(const MultiArray & x, const MultiArray::Array & paramValues, const MultiArray::Pair & boundValues) const;
    virtual MultiArray::Pair unboundedSupport(const MultiArray::Array & paramValues) const;

  public:
    virtual Bool IsDiscreteValued(const Flags & mask) const { return true; }
    virtual Bool IsSupportFixed(const Flags & fixmask) const { return true; }
    static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; }
  };

}

#endif /* BIIPS_DBERN_HPP_ */
