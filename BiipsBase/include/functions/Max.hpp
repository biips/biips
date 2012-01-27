//                                               -*- C++ -*-
/*! \file Max.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_MAX_HPP_
#define BIIPS_MAX_HPP_

#include "function/Function.hpp"

namespace Biips
{

  class Max: public Function
  {
  protected:
    typedef Max SelfType;

    Max() :
      Function("max", 0)
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

#endif /* BIIPS_MAX_HPP_ */
