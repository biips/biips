#include "RFunction.h"
#include <algorithm>
#include "Rcpp.h"
#include "RBiipsCommon.h"

// FIXME a tester!
static void convArrayVector(const NumArray & array,  NumericVector & vec) {
   ValArray & values = array.Values();
   DimArray & dims = array.Dim();
   
   Rcpp::IntegerVector dim(dims.begin(), dims.end());
   vec.attr("dim") = dim;
   vec.assign(values.begin(), values.end());
}


namespace Biips 
{

    void RFunction::eval(ValArray & output,
                        const NumArray::Array & params) {
   
        int nhrs  = params.size();
        std::vector<NumericVector> vecParams;
        for(int i = 0; i < nhrs ; ++i ){
            convArrayVector(params[i], vec[i]);
        }
        
        NumericVector outvec;
        apply(outvec, vecParams, fun_eval_, nhrs); 
        output.assign(outvec.begin(), outvec.end()); 
    }

    DimArray RFunction::dim(const std::vector<DimArray::Ptr> & paramDims) const {
        int nrhs = paramDims.size()
        std::vector<Rcpp::IntegerVector> paramvec;
        for(int i = 0; i < nhrs; ++i) {
            paramvec[i].assign(paramDims[i]->begin(), paramDims[i]->end());

        }
        Rcpp::IntegerVector outvec;
        apply(outvec, paramvec, fun_dim_, nrhs);
       
        return DimArray(outvec.begin(), outvec.end());
    }


    Bool RFunction::IsDiscreteValued(const std::vector<bool> & mask) const {
       
         if (fun_is_discret_.empty())
            return false;

         Rcpp::IntegerVector bool_vect = as<int>(mask);
         int res;
         apply(res, bool_vect, fun_is_discrete_, mask.size());
         return res; 
    }
      
}
