#ifndef RDISTRIBUTION_H_
#define RDISTRIBUTION_H_

#include <distribution/Distribution.hpp>
#include <Rcpp.h>


namespace Biips
{

  class RDistribution: public Distribution
  {
  protected:
    const Rcpp::Function fun_dim_;
    const Rcpp::Function fun_sample_;
    const Rcpp::Function fun_check_param_;
    const Rcpp::Function fun_is_discrete_;

    virtual Bool checkParamDims(
        const Types<DimArray::Ptr>::Array & paramDims) const
    {
      return true;
    }
    virtual DimArray

    dim(const Types<DimArray::Ptr>::Array & paramDims) const;

    virtual void sample(ValArray & values,
                        const NumArray::Array & paramValues,
                        const NumArray::Pair & boundValues,
                        Rng & rng) const;
    virtual Scalar logDensity(const NumArray & x,
                              const NumArray::Array & paramValues,
                              const NumArray::Pair & boundValues) const;
    virtual void fixedUnboundedSupport(ValArray & lower,
                                       ValArray & upper,
                                       const NumArray::Array & fixedParamValues) const;

  public:
    typedef RDistribution SelfType;
    typedef Distribution BaseType;

    RDistribution(const String & name, Size nParam,
                  const Rcpp::Function & fun_dim,
                  const Rcpp::Function & fun_sample,
                  const Rcpp::Function & fun_check_param,
                  const Rcpp::Function & fun_is_discrete)
    : BaseType(name, nParam),
      fun_dim_(fun_dim),
      fun_sample_(fun_sample),
      fun_check_param_(fun_check_param),
      fun_is_discrete_(fun_is_discrete)
    {
      // TODO: check name is valid
      // TODO: check functions nb of arguments
    }

    virtual Bool IsSupportFixed(const Flags & fixmask) const;

    virtual Bool IsDiscreteValued(const Flags & mask) const;

    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const;

    virtual Bool Observable() const
    {
      return false;
    }

    virtual ~RDistribution() {}

  };

}

#endif /* RDISTRIBUTION_H_ */
