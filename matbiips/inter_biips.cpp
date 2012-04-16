#include "mex.h"
#include <string>
#include <Console.hpp>
#include <map>
#include "Mostream.h"

using namespace std;
using namespace Biips;
typedef Console * Console_ptr;
std::map<int, Console_ptr> consoles;
int nb_consoles = 0;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs,const mxArray *prhs[])
{
    
    if (nrhs < 1) {
     mexErrMsgIdAndTxt("inter_biips:Args", "must have at least one argument ");
    }
  
    if ( mxIsChar(prhs[0]) != 1)
      mexErrMsgIdAndTxt( "inter_biips:inputNotString",
              "Input must be a string.");
    
    if (mxGetM(prhs[0])!=1)
      mexErrMsgIdAndTxt( "MATLAB:revord:inputNotVector",
              "Input must be a row vector.");
    
    char * input_buf = mxArrayToString(prhs[0]);
   
    string name_func = string(input_buf);

    /////////////////////////////////////////
    // MAKE_CONSOLE FUNCTION
    /////////////////////////////////////////
    if (name_func == "make_console") {
   
	  consoles[nb_consoles] = new Console(mbiips_cout, mbiips_cerr);
	  nb_consoles++;
          plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
	  *mxGetPr(plhs[0]) = nb_consoles;
    }
   
    /////////////////////////////////////////
    // CLEAR_CONSOLE FUNCTION
    /////////////////////////////////////////
    else if (name_func == "clear_console") {

       if (nrhs < 2) {
         mexErrMsgTxt("inter_biips : clear_console must have one integer argument");
       }
       
       int console_id = static_cast<int>(*mxGetPr(prhs[1]));
       
       // the console_id does not exist in the console map
       if (consoles.find(console_id) == consoles.end()) {
         mexErrMsgTxt("inter_biips : the console id does not exist");
       }

       if (nb_consoles >0) {
	  
	  delete consoles[console_id];
	  consoles.erase(console_id);
	  nb_consoles--;
       }
       else {
         mexErrMsgTxt("not anymore consoles opened!");
      }
           
    }
    else {

       mexPrintf("bad name of function\n");

    }
    /* set C-style string output_buf to MATLAB mexFunction output*/
    mxFree(input_buf);
    return;

}

