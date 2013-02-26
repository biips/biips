#include <boost/random/normal_distribution.hpp>

using namespace boost;
std::deque<Rng::Ptr> generateurs;

inline 
Size  GetRngId(const std::deque<Rng::Ptr> consoles, 
                   const mxArray * pm,
		   String name_func) {
   Size id = static_cast<Size>(*mxGetPr(pm));
   if ((id >= generateurs.size()) || (generateurs[id] == NULL)) {
       char id_error[1024];
       sprintf(id_error, "inter_biips:%s", name_func.c_str());
       mexErrMsgIdAndTxt(id_error, " %s : the console with id %d does not exist", name_func.c_str(), id);
   }
   return id;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
    
    if (nrhs < 1) {
     mexErrMsgIdAndTxt("inter_biips:Args", "must have at least one argument ");
    }
  
    if ( mxIsChar(prhs[0]) != 1)
      mexErrMsgIdAndTxt( "inter_biips:inputNotString",
              "Input must be a string.");
    
    if (mxGetM(prhs[0])!=1)
      mexErrMsgIdAndTxt( "inter_biips:inputNotVector",
              "Input must be a row vector.");
    
    char * input_buf = mxArrayToString(prhs[0]);
   
    String name_func = String(input_buf);

    /////////////////////////////////////////
    // CREATE_RNG FUNCTION
    /////////////////////////////////////////
    if (name_func == "create_rng") {
        CheckRhs(nrhs, 1, name_func);
        CheckArgIsDouble(1);
	Size seed = static_cast<Size>(*mxgetPr(prhs[1]));
	generateurs.push_back(new Rng(seed));
        plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
        *mxGetPr(plhs[0]) = static_cast<double>(generateurs.size()-1);
    }

   
    if (name_func == "dnorm") {

       CheckRhs(nrhs, 3, name_func);
       CheckArgsIsDouble(1);
       Size id =   GetRngId(generateurs, prhs[1], name_func);
       Rng::Ptr p_rng = generateurs[id];

       CheckArgIsDouble(2);
       CheckArgIsDouble(3);

       Size m = static_cast<Size> (*mxGetPr(prhs[2])); 
       Size n = static_cast<Size> (*mxGetPr(prhs[3]));

       normal_distribution<double> norm_dist;
       variate_generator<Rng::GenType&, normal_distribution<double> > norm_gen(*r_rng, norm_dist)  ;
      
       plhs[0] = mxCreateDoubleMatrix(m, n, mxReal);
       double * mytab = *mxGetPr(plhs[0])
       std::generate(mytab , mytab + n*m, norm_gen);
    }
}

