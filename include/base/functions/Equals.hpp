#ifndef BIIPS_EQUALS_HPP_
#define BIIPS_EQUALS_HPP_

#include "function/Function.hpp"

namespace Biips
{

  class Equals: public Function
  {
  public:
    typedef Function BaseType;
    typedef Equals SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    Equals() :
      BaseType("equals", 2)
    {
    }

    virtual Bool
    checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const
    {
      return *P_SCALAR_DIM;
    }
    virtual void
    eval(ValArray & values, const NumArray::Array & paramValues) const;

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const
    {
      return true;
    }
    virtual Bool IsDiscreteValued(const Flags & mask) const
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

#endif /* BIIPS_EQUALS_HPP_ */
