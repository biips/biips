//                                               -*- C++ -*-
/*! \file Min.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_MIN_HPP_
#define BIIPS_MIN_HPP_

#include "function/Function.hpp"

namespace Biips
{

  class Min: public Function
  {
  protected:
    typedef Min SelfType;

    Min() :
      Function("min", 0)
    {
    }

    virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
    {
      return true;
    }
    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const
    {
      return *P_SCALAR_DIM;
    }
    virtual void
    eval(ValArray & values, const NumArray::Array & paramValues) const;

  public:
    virtual Bool IsDiscreteValued(const Flags & mask) const;
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

}

#endif /* BIIPS_MIN_HPP_ */
