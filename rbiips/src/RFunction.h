#ifndef BIIPS_RFUNCTION_HPP_
#define BIIPS_RFUNCTION_HPP_

#include "function/Function.hpp"
#include <Rcpp.h>

namespace Biips
{

  class RFunction: public Function
  {
  protected:
    Rcpp::Function fun_dim_;
    Rcpp::Function fun_eval_;
    Rcpp::Function fun_check_param_;
    Rcpp::Function fun_is_discrete_;

    virtual Bool checkParamDims(
        const Types<DimArray::Ptr>::Array & paramDims) const
    {
      return true;
    }
    virtual DimArray

    dim(const Types<DimArray::Ptr>::Array & paramDims) const;

    virtual void eval(ValArray & values, const NumArray::Array & paramValues) const;

  public:
    typedef RFunction SelfType;
    typedef Function BaseType;

    RFunction(const String & name, Size nParam, 
              const Rcpp::Function & fun_dim,
              const Rcpp::Function & fun_eval, 
              const Rcpp::Function & fun_check_param = Rcpp::Function(),
              const Rcpp::Function & fun_is_discrete = Rcpp::Function())
        : BaseType(name, nParam), 
          fun_dim_(fun_dim), 
          fun_eval_(fun_eval),
          fun_check_param_(fun_check_param), 
          fun_is_discrete_(fun_is_discrete)
    {
    }

    virtual Bool IsDiscreteValued(const Flags & mask) const;

    virtual Bool CheckParamValues(const NumArray::Array & paramValues) const;

    virtual ~RFunction()
    {
    }

  };

}

#endif /* BIIPS_RFUNCTION_HPP_ */
