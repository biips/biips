#include "RDistribution.h"
#include <algorithm>
#include "Rbiips_utils.h"


namespace Biips
{

  void RDistribution::sample(ValArray & values,
                             const NumArray::Array & paramValues,
                             const NumArray::Pair & boundValues,
                             Rng & rng) const {

    int nrhs  = paramValues.size();
    std::vector<Rcpp::NumericVector> vecParams(nrhs);
    for(int i = 0; i < nrhs ; ++i ){
      vecParams[i] = convArrayVector(paramValues[i]);
    }

    Rcpp::NumericVector outvec = apply(vecParams, fun_sample_, nrhs);
    values.assign(outvec.begin(), outvec.end());
  }

  DimArray RDistribution::dim(const std::vector<DimArray::Ptr> & paramDims) const {
    int nrhs = paramDims.size();
    std::vector<Rcpp::IntegerVector> paramvec(nrhs);
    for(int i = 0; i < nrhs; ++i) {
      paramvec[i].assign(paramDims[i]->begin(), paramDims[i]->end());

    }
    Rcpp::IntegerVector outvec = apply(paramvec, fun_dim_, nrhs);

    return DimArray(outvec.begin(), outvec.end());
  }

  Bool RDistribution::CheckParamValues(const NumArray::Array & paramValues) const {

    int nrhs = paramValues.size();
    std::vector<Rcpp::NumericVector> vecParamValues(nrhs);
    for(int i = 0; i < nrhs; ++i) {
      vecParamValues[i] = convArrayVector(paramValues[i]);
    }
    Rcpp::NumericVector res = apply(vecParamValues, fun_check_param_, nrhs);

    int mybool = static_cast<int>(res[0]);
    return mybool;
  }

  Bool RDistribution::IsDiscreteValued(const std::vector<bool> & mask) const {

    bool res = apply(mask, fun_is_discrete_, mask.size());
    return static_cast<bool>(res);
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
