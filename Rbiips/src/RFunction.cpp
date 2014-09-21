#include "RFunction.h"
#include <algorithm>
#include "Rbiips_utils.h"

namespace Biips 
{

  void RFunction::eval(ValArray & values,
                       const NumArray::Array & paramValues) const {

    Size nrhs  = paramValues.size();
    std::vector<Rcpp::NumericVector> vec_params(nrhs);
    for(Size i = 0; i < nrhs ; ++i ){
      vec_params[i] = arrayToVector(paramValues[i]);
    }

    Rcpp::NumericVector outvec = evalRfun(fun_eval_, vec_params);
    values.assign(outvec.begin(), outvec.end());
  }

  DimArray RFunction::dim(const Types<DimArray::Ptr>::Array & paramDims) const {
    Size nrhs = paramDims.size();
    std::vector<Rcpp::IntegerVector> vec_dims(nrhs);
    for(Size i = 0; i < nrhs; ++i) {
      vec_dims[i].assign(paramDims[i]->begin(), paramDims[i]->end());
    }
    Rcpp::IntegerVector outvec = evalRfun(fun_dim_, vec_dims);

    return DimArray(outvec.begin(), outvec.end());
  }

  Bool RFunction::CheckParamValues(const NumArray::Array & paramValues) const {

    Size nrhs = paramValues.size();
    std::vector<Rcpp::NumericVector> vec_params(nrhs);
    for(Size i = 0; i < nrhs; ++i) {
      vec_params[i] = arrayToVector(paramValues[i]);
    }
    Rcpp::NumericVector res = evalRfun(fun_check_param_, vec_params);
    return static_cast<Bool>(res[0]);
  }

  Bool RFunction::IsDiscreteValued(const Flags & mask) const {
    Size nrhs = mask.size();
    std::vector<Rcpp::LogicalVector> vec_mask(nrhs);
    for (Size i = 0; i < nrhs; ++i) {
      vec_mask[i] = Rcpp::LogicalVector(1, mask[i]);
    }
    Rcpp::LogicalVector res = evalRfun(fun_is_discrete_, vec_mask);
    return static_cast<Bool>(res[0]);
  }

}
