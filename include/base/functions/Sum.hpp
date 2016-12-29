#ifndef BIIPS_SUM_HPP_
#define BIIPS_SUM_HPP_

#include "function/Function.hpp"

namespace Biips
{

  class Sum: public Function
  {
  protected:
    typedef Sum SelfType;

    Sum() :
      Function("sum", 1)
    {
    }

    virtual Bool
    checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
    {
      return true;
    }
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const
    {
      return *P_SCALAR_DIM;
    }
    virtual void
        eval(ValArray & values, const NumArray::Array & paramValues) const
    {
      values[0] = paramValues[0].Values().Sum();
    }

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }
    virtual Bool IsDiscreteValued(const Flags & mask) const
    {
      return mask[0];
    }
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

}

#endif /* BIIPS_SUM_HPP_ */
