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

   void RFunction::eval(ValArray & output,
                        const NumArray::Array & params) {
   
      int nhrs  = params.size();
      std::vector<NumericVector> vecParams;
      for(int i = 0; i < nhrs ; ++i ){
          convArrayVector(params[i], vec[i]);
      }
      
      NumericVector outvec;

      switch(nrhs) {

       1 : outvec = fun_eval_(vecParams[0]); 
           break;
       2 : outvec = fun_eval_(vecParams[0],
                              vecParams[1]);
           break;
       3 : outvec = fun_eval_(vecParams[0],
                              vecParams[1],
                              vecParams[2]);
           break;                              
       4 : outvec = fun_eval_(vecParams[0],
                              vecParams[1],
                              vecParams[2],
                              vecParams[3]);
           break;                              
       5 : outvec = fun_eval_(vecParams[0],
                              vecParams[1],
                              vecParams[2],
                              vecParams[3],
                              vecParams[4]);
           break;                              
       default: throw LogicError("Too much arguments in RFunction must be <= 5");
                break;
      }
      

   }





}
