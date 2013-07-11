#include "RFunction.h"
#include <algorithm>
#include "Rcpp.h"


// FIXME a tester!
void convArrayVector(const NumArray & array,  NumericVector & vec) {
   ValArray & values = array.Values();
   DimArray & dims = array.Dim();
   
   Rcpp::IntegerVector dim(dims.begin(), dims.end());
   vec.attr("dim") = dim;
   vec.assign(values.begin(), values.end());
}





namespace Biips 
{

   void RFunction::eval(ValArray & outputs,
                        const NumArray::Array & params) {
   
      int nhrs  = params.size();
      for(int i = 0; i < nhrs ; ++i ){
          NumericVector v;
          convArrayVector(params[i], v);
      }
   
   }





}
