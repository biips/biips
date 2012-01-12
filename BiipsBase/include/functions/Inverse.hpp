//                                               -*- C++ -*-
/*! \file Inverse.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_INVERSE_HPP_
#define BIIPS_INVERSE_HPP_

#include "function/Function.hpp"

namespace Biips
{

  class Inverse: public Function
  {
  protected:
    typedef Inverse SelfType;
    typedef Function BaseType;

    Inverse() :
      BaseType("inverse", 1)
    {
    }

    virtual Bool
    checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual Bool checkParamValues(const NumArray::Array & paramValues) const;
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual void
    eval(ValArray & values, const NumArray::Array & paramValues) const;

  public:
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

}

#endif /* BIIPS_INVERSE_HPP_ */
