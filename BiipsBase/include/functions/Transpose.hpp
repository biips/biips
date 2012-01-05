//                                               -*- C++ -*-
/*! \file Transpose.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef TRANSPOSE_HPP_
#define TRANSPOSE_HPP_

#include "function/Function.hpp"

namespace Biips
{

  class Transpose: public Biips::Function
  {
  protected:
    typedef Transpose SelfType;

    Transpose() :
      Function("t", 1)
    {
    }

    virtual Bool
        checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual Bool checkParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual void
        eval(ValArray & values, const NumArray::Array & paramValues) const;

  public:
    virtual Bool IsScale(const Flags & scaleMask, const Flags & knownMask) const
    {
      return true;
    }
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

}

#endif /* TRANSPOSE_HPP_ */
