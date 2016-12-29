#ifndef BIIPS_MATMULT_HPP_
#define BIIPS_MATMULT_HPP_

#include "function/Function.hpp"

namespace Biips
{

  class MatMult: public Function
  {
  protected:
    typedef MatMult SelfType;
    typedef Function BaseType;

    MatMult() :
      BaseType("%*%", 2)
    {
    }

    virtual Bool
        checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual void
    eval(ValArray & values, const NumArray::Array & paramValues) const;

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }
    virtual Bool IsInfix() const
    {
      return true;
    }
    virtual Bool IsScale(const Flags & scaleMask, const Flags & knownMask) const;

    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

}

#endif /* BIIPS_MATMULT_HPP_ */
