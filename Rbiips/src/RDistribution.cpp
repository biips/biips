#include "RDistribution.h"
#include <algorithm>
#include "Rbiips_utils.h"


namespace Biips
{

  void RDistribution::sample(ValArray & values,
                             const NumArray::Array & paramValues,
                             const NumArray::Pair & boundValues,
                             Rng & rng) const {

    Size nrhs  = paramValues.size();
    std::vector<Rcpp::NumericVector> vec_params(nrhs);
    for(Size i = 0; i < nrhs ; ++i ){
      vec_params[i] = arrayToVector(paramValues[i]);
    }

    Rcpp::NumericVector outvec = evalRfun(fun_sample_, vec_params);
    values.assign(outvec.begin(), outvec.end());
  }

  DimArray RDistribution::dim(const Types<DimArray::Ptr>::Array & paramDims) const {
    Size nrhs = paramDims.size();
    std::vector<Rcpp::IntegerVector> vec_dims(nrhs);
    for(Size i = 0; i < nrhs; ++i) {
      vec_dims[i].assign(paramDims[i]->begin(), paramDims[i]->end());
    }
    Rcpp::IntegerVector outvec = evalRfun(fun_dim_, vec_dims);

    return DimArray(outvec.begin(), outvec.end());
  }

  Bool RDistribution::CheckParamValues(const NumArray::Array & paramValues) const {

    Size nrhs = paramValues.size();
    std::vector<Rcpp::NumericVector> vecParamValues(nrhs);
    for(Size i = 0; i < nrhs; ++i) {
      vecParamValues[i] = arrayToVector(paramValues[i]);
    }
    Rcpp::NumericVector res = evalRfun(fun_check_param_, vecParamValues);
    return static_cast<Bool>(res[0]);
  }

  Bool RDistribution::IsDiscreteValued(const Flags & mask) const {
    Size nrhs = mask.size();
    std::vector<Rcpp::LogicalVector> vec_mask(nrhs);
    for (Size i = 0; i < nrhs; ++i) {
      vec_mask[i] = Rcpp::LogicalVector(1, mask[i]);
    }
    Rcpp::LogicalVector res = evalRfun(fun_is_discrete_, vec_mask);
    return static_cast<Bool>(res[0]);
  }

  Scalar RDistribution::logDensity(const NumArray& x,
                                   const NumArray::Array& paramValues,
                                   const NumArray::Pair& boundValues) const
  {
    throw LogicError("logDensity call is not available for distribution "+name_);
    return BIIPS_REALNAN;
  }

  void RDistribution::fixedUnboundedSupport(ValArray& lower,
                                            ValArray& upper,
                                            const NumArray::Array& fixedParamValues) const
  {
    throw LogicError("fixedUnboundedSupport call is not available for distribution "+name_);
  }

  Bool RDistribution::IsSupportFixed(const Flags& fixmask) const
  {
    return false;
  }

}
