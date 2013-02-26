#include <deque>
#include "mex.h" 
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <common/Types.hpp>
#include <boost/math/distributions/uniform.hpp>
#include <cstring>
#include <boost/random.hpp>

using namespace boost;
using namespace Biips;
std::deque<mt19937> generateurs;


inline  
void CheckRhs(int nrhs, int nb, String name_func) { 
  if (nrhs != (nb + 1)) { 
     char id_error[1024];
     sprintf(id_error, "inter_boost:%s", name_func.c_str());
     mexErrMsgIdAndTxt(id_error, " %s must have %d argument(s)", name_func.c_str(), nb);
  }
}

inline 
Size  GetRngId(const std::deque<mt19937> consoles, 
                   const mxArray * pm,
		   String name_func) {
   Size id = static_cast<Size>(*mxGetPr(pm));
   if ((id >= generateurs.size())) {
       char id_error[1024];
       sprintf(id_error, "inter_boost:%s", name_func.c_str());
       mexErrMsgIdAndTxt(id_error, " %s : the console with id %d does not exist", name_func.c_str(), id);
   }
   return id;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    
    if (nrhs < 1) {
     mexErrMsgIdAndTxt("inter_biips:Args", "must have at least one argument ");
    }
  
    if ( mxIsChar(prhs[0]) != 1)
      mexErrMsgIdAndTxt( "inter_boost:inputNotString",
              "Input must be a string.");
    
    if (mxGetM(prhs[0])!=1)
      mexErrMsgIdAndTxt( "inter_boost:inputNotVector",
              "Input must be a row vector.");
    
    char * input_buf = mxArrayToString(prhs[0]);
   
    String name_func = String(input_buf);

    /////////////////////////////////////////
    // CREATE_RNG FUNCTION
    /////////////////////////////////////////
    if (name_func == "create_rng") {
        CheckRhs(nrhs, 1, name_func);
        //CheckArgIsDouble(1);
	Size seed = static_cast<Size>(*mxGetPr(prhs[1]));
	mt19937 un_gen;
	un_gen.seed(seed);
	generateurs.push_back(un_gen);
        plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
        *mxGetPr(plhs[0]) = static_cast<double>(generateurs.size()-1);
    }

   
    if (name_func == "rnorm") {

       CheckRhs(nrhs, 3, name_func);
       //CheckArgsIsDouble(1);
       Size id = GetRngId(generateurs, prhs[1], name_func);
       mt19937 rng = generateurs[id];

       //CheckArgIsDouble(2);
       //CheckArgIsDouble(3);

       Size m = static_cast<Size> (*mxGetPr(prhs[2])); 
       Size n = static_cast<Size> (*mxGetPr(prhs[3]));

       normal_distribution<> norm_dist;
       variate_generator<mt19937&, normal_distribution<> > norm_gen(rng, norm_dist)  ;
      
       plhs[0] = mxCreateDoubleMatrix(m, n, mxREAL);
       double * mytab = mxGetPr(plhs[0]);
       std::generate(mytab , mytab + n*m, norm_gen);
    }
    
    if (name_func == "runif") {

       CheckRhs(nrhs, 3, name_func);
       Size id = GetRngId(generateurs, prhs[1], name_func);
       mt19937 rng = generateurs[id];

       Size m = static_cast<Size> (*mxGetPr(prhs[2])); 
       Size n = static_cast<Size> (*mxGetPr(prhs[3]));

       uniform_real<>  unif_dist;
       variate_generator<mt19937&, uniform_real<> > norm_gen(rng, unif_dist)  ;
      
       plhs[0] = mxCreateDoubleMatrix(m, n, mxREAL);
       double * mytab = mxGetPr(plhs[0]);
       std::generate(mytab , mytab + n*m, norm_gen);
    }
}

