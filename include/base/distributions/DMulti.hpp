#ifndef BIIPS_DMULTI_HPP_
#define BIIPS_DMULTI_HPP_

#include "distribution/Distribution.hpp"

namespace Biips
{

  class DMulti: public Distribution
  {
  protected:
    typedef DMulti SelfType;

    DMulti() :
      Distribution("dmulti", 2)
    {
    }

    virtual Bool
    checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual void sample(ValArray & values,
                        const NumArray::Array & paramValues,
                        const NumArray::Pair & boundValues,
                        Rng & rng) const;
    virtual Scalar logDensity(const NumArray & x,
                              const NumArray::Array & paramValues,
                              const NumArray::Pair & boundValues) const;
    virtual void
    fixedUnboundedSupport(ValArray & lower,
                     ValArray & upper,
                     const NumArray::Array & paramValues) const;

  public:
    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const;
    virtual Bool
        CheckDensityParamValues(const NumArray & x,
                                const NumArray::Array & paramValues) const;
    virtual Bool IsDiscreteValued(const Flags & mask) const
    {
      return true;
    }
    virtual Bool CheckParamDiscrete(const Flags & mask) const{
      return mask[1];
  }
    virtual Bool IsSupportFixed(const Flags & fixmask) const
    {
      return fixmask[1];
    }
    virtual Size DegreeOfFreedom(const Types<DimArray::Ptr>::Array & paramDims) const
    {
      return paramDims[0]->Length()-1;
    }
    static Distribution::Ptr Instance()
    {
      static Distribution::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

}

#endif /* BIIPS_DMULTI_HPP_ */
