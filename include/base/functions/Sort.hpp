#ifndef BIIPS_SORT_HPP_
#define BIIPS_SORT_HPP_

#include "function/Function.hpp"

namespace Biips
{

  class Sort: public Function
  {
  protected:
    typedef Sort SelfType;

    Sort() :
      Function("sort", 1)
    {
    }

    virtual Bool
    checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const
    {
      return paramDims[0]->Drop().IsVector();
    }
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const
    {
      return *(paramDims[0]);
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
      return mask[0];
    }
    static Function::Ptr Instance()
    {
      static Function::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

} /* namespace Biips */

#endif /* BIIPS_SORT_HPP_ */
