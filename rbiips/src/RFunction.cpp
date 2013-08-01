#include "RFunction.h"
#include <algorithm>
#include "Rcpp.h"
#include "RBiipsCommon.h"

// FIXME a tester!
static Rcpp::NumericVector convArrayVector(const Biips::NumArray & array ) { 
   const Biips::ValArray & values = array.Values();
   const Biips::DimArray & dims = array.Dim();
   
   Rcpp::IntegerVector dim(dims.begin(), dims.end());
   Rcpp::NumericVector vec;
   vec.attr("dim") = dim;
   vec.assign(values.begin(), values.end());
   return vec;
}


namespace Biips 
{

    void RFunction::eval(ValArray & output,
                        const NumArray::Array & params) const {
   
        int nhrs  = params.size();
        std::vector<Rcpp::NumericVector> vecParams;
        for(int i = 0; i < nhrs ; ++i ){
            vecParams[i] = convArrayVector(params[i]);
        }
        
        Rcpp::NumericVector outvec = apply(vecParams, fun_eval_, nhrs); 
        output.assign(outvec.begin(), outvec.end()); 
    }

    DimArray RFunction::dim(const std::vector<DimArray::Ptr> & paramDims) const {
        int nrhs = paramDims.size();
        std::vector<Rcpp::IntegerVector> paramvec(nrhs);
        for(int i = 0; i < nrhs; ++i) {
            paramvec[i].assign(paramDims[i]->begin(), paramDims[i]->end());

        }
        Rcpp::IntegerVector outvec = apply(paramvec, fun_dim_, nrhs);
       
        return DimArray(outvec.begin(), outvec.end());
    }

    Bool RFunction::CheckParamValues(const NumArray::Array & paramValues) const {

       int nrhs = paramValues.size();
       std::vector<Rcpp::NumericVector> vecParamValues;
       for(int i = 0; i < nrhs; ++i) {
        vecParamValues[i] = convArrayVector(paramValues[i]);
       }
       int res = Rcpp::as<int>(apply(vecParamValues, fun_check_param_, nrhs));
       return res;
    }

    Bool RFunction::IsDiscreteValued(const std::vector<bool> & mask) const {
       
         int res = apply(mask, fun_is_discrete_, mask.size());
         return res; 
    }
      
}
