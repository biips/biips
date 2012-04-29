#include "mex.h"
#include <string>
#include <Console.hpp>
#include <deque>
#include "Mostream.h"

using namespace std;
using namespace Biips;
typedef Console * Console_ptr;
std::deque<Console_ptr> consoles;

inline bool
CheckConsoleId(std::deque<Console_ptr> consoles, int id) { 
return ((id < consoles.size()) && (consoles[id] != NULL)); 
}


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
   
          consoles.push_back(new Console(mbiips_cout, mbiips_cerr));
          plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
          *mxGetPr(plhs[0]) = consoles.size()-1;
    }
   
    /////////////////////////////////////////
    // CLEAR_CONSOLE FUNCTION
    /////////////////////////////////////////
    else if (name_func == "clear_console") {

       if (nrhs < 2) {
         mexErrMsgTxt("clear_console :  must have one integer argument");
       }
       
       int console_id = static_cast<int>(*mxGetPr(prhs[1]));
       
       // the console_id does not exist in the console map
       if (CheckConsoleId(consoles, console_id)) {
           delete consoles[console_id];
           consoles[console_id] = NULL;
       }
       else { 
         mexErrMsgTxt("clear_console : the console id does not exist");
       }
    }       
    /////////////////////////////////////////
    // CHECK_MODEL FUNCTION
    /////////////////////////////////////////
    else if (name_func == "check_model") {

       if (nrhs < 3) {
         mexErrMsgTxt("check_model: must have two arguments");
       }
       
       int console_id = static_cast<int>(*mxGetPr(prhs[1]));
       char * filename = mxArrayToString(prhs[2]);
       
       if (CheckConsoleId(consoles, console_id)) {
              Console * p_console = consoles[console_id];
	      mbiips_cout << PROMPT_STRING << "Parsing model in: " << filename << endl;
              if (! p_console->CheckModel(string(filename), true)) 
                 mexErrMsgTxt("Model syntax is incorrect.");
       }
       else { 
         mexErrMsgTxt("check_model : the console id does not exist");
       }
       mxFree(filename);
    }       
    else {
       mexPrintf("bad name of function\n");

    }
    /* set C-style string output_buf to MATLAB mexFunction output*/
    mxFree(input_buf);
    return;

}

